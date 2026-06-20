// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#include "AnomalyViewport.h"

#include "AnomalyTargeting.h"
#include "AnomalyInjectorLog.h"

#include "ConvexVolume.h"                  // FConvexVolume, GetViewFrustumBounds
#include "Camera/CameraTypes.h"           // FMinimalViewInfo::CalculateProjectionMatrix (reversed-Z)
#include "Camera/PlayerCameraManager.h"   // APlayerCameraManager::GetFOVAngle
#include "Components/PrimitiveComponent.h" // Bounds, GetOwner, IsVisible
#include "Components/StaticMeshComponent.h"   // UStaticMeshComponent (renderable allowlist)
#include "Components/InstancedStaticMeshComponent.h" // UInstancedStaticMeshComponent (empty-instance guard; HISM derives)
#include "Components/SkinnedMeshComponent.h"  // USkinnedMeshComponent (renderable allowlist; skeletal derives)
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"           // APawn (poll origin = pawn location, not camera)
#include "GameFramework/PlayerController.h"
#include "EngineUtils.h"                  // TActorIterator (GetVisibleRenderableActors)
#include "Engine/World.h"                 // UWorld::LineTraceSingleByChannel / GetFirstPlayerController / GetGameViewport
#include "Engine/GameViewportClient.h"    // GetViewportSize (aspect)
#include "Engine/EngineTypes.h"           // ECC_Visibility
#include "Engine/HitResult.h"             // FHitResult (5.1)
#include "CollisionQueryParams.h"         // FCollisionQueryParams
#include "HAL/IConsoleManager.h"          // FAutoConsoleCommand (IAI.SetPollRadius)
#include "Debug/DebugDrawService.h"       // UDebugDrawService::Register (poll-radius debug sphere, G25)
#include "DrawDebugHelpers.h"             // DrawDebugSphere (poll-radius debug sphere)

namespace
{
	using namespace AnomalyViewport;

	// --- Poll-radius distance cull: shared state (single source of truth; default OFF, R <= 0 disables) ---
	float GPollRadius = 0.0f;              // Unreal units (cm)
	FDelegateHandle GPollRadiusDrawHandle; // dev debug sphere; valid only while a positive radius is set

	/**
	 * Poll origin for the distance cull = the player PAWN location (LOCKED: pawn, not camera). Falls back to
	 * Fallback (the camera origin, passed by the live callers) when there is no pawn — e.g. a spectator or a
	 * Simulate session with no possessed pawn — so the cull never reads a garbage origin.
	 */
	FVector ResolvePollOrigin(UWorld* World, const FVector& Fallback)
	{
		if (World)
		{
			if (const APlayerController* PC = World->GetFirstPlayerController())
			{
				if (const APawn* Pawn = PC->GetPawn())
				{
					return Pawn->GetActorLocation();
				}
			}
		}
		return Fallback;
	}

	/**
	 * Dev debug-draw delegate (UDebugDrawService "Game", G25): draws the poll radius as a sphere centered on the
	 * LIVE pawn, re-resolved EVERY frame (the pawn moves — never cache a position at registration). Self-gates on
	 * R > 0, so it is a cheap no-op when the cull is off; SetPollRadius (un)registers it on the OFF<->ON boundary.
	 */
	void DrawPollRadiusHUD(UCanvas* /*Canvas*/, APlayerController* PC)
	{
		const float R = GPollRadius;
		if (R <= 0.0f || !PC)
		{
			return;
		}
		UWorld* World = PC->GetWorld();
		if (!World)
		{
			return;
		}
		FVector Center;
		if (const APawn* Pawn = PC->GetPawn())
		{
			Center = Pawn->GetActorLocation();   // fresh each frame
		}
		else
		{
			FRotator Unused;
			PC->GetPlayerViewPoint(Center, Unused);   // no pawn -> camera origin (matches ResolvePollOrigin's fallback)
		}
		// PHASE (load-bearing): this delegate fires during the POST-scene canvas/HUD draw, after the world line
		// batcher has already rendered this frame. A one-frame line (LifeTime <= 0) queued here is cleared before
		// the next frame's scene pass and NEVER renders (that is why the sphere was invisible). A small POSITIVE
		// lifetime routes the lines to the PERSISTENT batcher, which survives into the next scene pass; re-added
		// every frame, the sphere is continuously visible (~1 frame latency). ~2x the last frame delta keeps it
		// alive just long enough (minimal motion smear) while surviving low framerates via the 0.05s floor.
		// (The selector's DrawDebugBox can use LifeTime=-1 only because it is drawn from the subsystem Tick, which
		// runs BEFORE the scene render. AnomalyViewport has no Tick, hence the persistent-batcher route here.)
		const float SphereLife = FMath::Max(World->GetDeltaSeconds() * 2.0f, 0.05f);
		DrawDebugSphere(World, Center, R, 24, FColor::Yellow, /*bPersistent=*/false, SphereLife, /*DepthPriority=*/0, /*Thickness=*/2.0f);
	}

	/**
	 * Build the reversed-Z view-projection matrix from an explicit view, exactly as the engine's live
	 * path does (verified against 5.1: LocalPlayer.cpp view-rotation basis swap + FMinimalViewInfo's
	 * FReversedZPerspectiveMatrix). Synthetic and live results therefore match for the same pose/FOV/aspect.
	 */
	FMatrix BuildViewProjectionMatrix(const FAnomalyViewInfo& View)
	{
		FMinimalViewInfo VI;
		VI.Location = View.Origin;
		VI.Rotation = View.Rotation;
		VI.FOV = View.HorizontalFOVDeg;
		VI.AspectRatio = View.AspectRatio;
		VI.ProjectionMode = ECameraProjectionMode::Perspective;
		VI.bConstrainAspectRatio = false;
		const FMatrix ProjectionMatrix = VI.CalculateProjectionMatrix();   // reversed-Z, infinite far

		// World -> view basis swap (the canonical UE form, LocalPlayer.cpp:1139).
		const FMatrix ViewRotationMatrix = FInverseRotationMatrix(View.Rotation) * FMatrix(
			FPlane(0, 0, 1, 0),
			FPlane(1, 0, 0, 0),
			FPlane(0, 1, 0, 0),
			FPlane(0, 0, 0, 1));

		// == FSceneViewProjectionData::ComputeViewProjectionMatrix().
		return FTranslationMatrix(-View.Origin) * ViewRotationMatrix * ProjectionMatrix;
	}

	/**
	 * Frustum from an explicit view. bUseNearPlane=true, bUseFarPlane=false under reversed-Z: the near
	 * plane (clip Z=1) rejects geometry behind the camera; the far plane is infinite/degenerate so it is
	 * skipped (distant-but-in-cone objects stay "in frustum"). See gotcha G24.
	 */
	FConvexVolume BuildFrustum(const FAnomalyViewInfo& View)
	{
		FConvexVolume Frustum;
		GetViewFrustumBounds(Frustum, BuildViewProjectionMatrix(View), /*bUseNearPlane=*/true, /*bUseFarPlane=*/false);
		return Frustum;
	}

	bool IsInFrustum(const FConvexVolume& Frustum, const UPrimitiveComponent* Component)
	{
		const FBoxSphereBounds& B = Component->Bounds;
		// Cheap sphere reject, then the tighter box test.
		return Frustum.IntersectSphere(B.Origin, B.SphereRadius) && Frustum.IntersectBox(B.Origin, B.BoxExtent);
	}

	/**
	 * Occlusion backend (PRIVATE — AMB-V1). Multi-sample camera-to-bounds line trace: sample the bounds
	 * center + 8 AABB corners; the component is UNOCCLUDED if the path from the view origin to ANY sample
	 * is unblocked (ECC_Visibility), ignoring the target's own actor. Synchronous and deterministic; a
	 * pure function of (origin, world, component) so a synthetic view gates it. Over-includes on
	 * no-collision / translucent occluders (accepted v1 trade-off; render-time is the future live backend).
	 */
	bool IsUnoccluded(const FVector& ViewOrigin, UWorld* World, const UPrimitiveComponent* Component)
	{
		const FBoxSphereBounds& B = Component->Bounds;
		const FVector C = B.Origin;
		const FVector E = B.BoxExtent;

		TArray<FVector, TInlineAllocator<9>> Samples;
		Samples.Add(C);
		for (int32 SignX = -1; SignX <= 1; SignX += 2)
		{
			for (int32 SignY = -1; SignY <= 1; SignY += 2)
			{
				for (int32 SignZ = -1; SignZ <= 1; SignZ += 2)
				{
					Samples.Add(C + FVector(SignX * E.X, SignY * E.Y, SignZ * E.Z));
				}
			}
		}

		FCollisionQueryParams Params(FName(TEXT("AnomalyViewportOcclusion")), /*bTraceComplex=*/false);
		if (const AActor* Owner = Component->GetOwner())
		{
			Params.AddIgnoredActor(Owner);   // the target's own surface must not count as its occluder
		}

		for (const FVector& Target : Samples)
		{
			FHitResult Hit;
			const bool bBlocked = World->LineTraceSingleByChannel(Hit, ViewOrigin, Target, ECC_Visibility, Params);
			if (!bBlocked)
			{
				return true;   // a clear path to this sample -> at least partly unoccluded
			}
		}
		return false;
	}

	/** Combined test against a prebuilt frustum (so a filter loop builds the frustum once). */
	bool IsComponentVisibleInternal(const FConvexVolume& Frustum, const FVector& ViewOrigin, UWorld* World, const UPrimitiveComponent* Component)
	{
		return Component && IsInFrustum(Frustum, Component) && IsUnoccluded(ViewOrigin, World, Component);
	}

	/** Actor visibility against a prebuilt frustum: ANY primitive component visible (AMB-V5). */
	bool IsActorVisibleInternal(const FConvexVolume& Frustum, const FVector& ViewOrigin, UWorld* World, const AActor* Actor)
	{
		TArray<UPrimitiveComponent*> Prims;
		Actor->GetComponents<UPrimitiveComponent>(Prims);
		for (const UPrimitiveComponent* Prim : Prims)
		{
			if (IsComponentVisibleInternal(Frustum, ViewOrigin, World, Prim))
			{
				return true;
			}
		}
		return false;
	}

	/**
	 * Renderable-visible test (m5 follow-on). Predicate order: renderable type-test FIRST (cheap), then the
	 * poll-radius distance cull (cheap; only when PollRadius > 0), then frustum, then the occlusion line traces
	 * LAST. Cheapest-cull-first: non-rendering primitives and out-of-range actors are rejected before any trace
	 * (correctness AND a perf win). The result is an AND of all predicates, so the ordering is purely a
	 * short-circuit choice with no behavioral effect.
	 * Poll-radius cull (sphere-approximated bounds distance): dist(PollOrigin, Bounds.Origin) - SphereRadius <= R.
	 * Component->Bounds is a cached member (no recomputation); the same one the frustum/occlusion tests read.
	 * PollRadius <= 0 disables the cull (the synthetic-view callers pass 0, keeping that surface byte-identical).
	 */
	bool IsComponentRenderableVisibleInternal(const FConvexVolume& Frustum, const FVector& ViewOrigin,
		const FVector& PollOrigin, float PollRadius, UWorld* World, const UPrimitiveComponent* Component)
	{
		if (!AnomalyViewport::IsRenderableComponent(Component))
		{
			return false;
		}
		if (PollRadius > 0.0f)
		{
			const FBoxSphereBounds& B = Component->Bounds;   // cached member; shared with frustum/occlusion below
			if ((float)FVector::Dist(PollOrigin, B.Origin) - (float)B.SphereRadius > PollRadius)
			{
				return false;   // outside the poll radius -> culled before any line trace
			}
		}
		return IsInFrustum(Frustum, Component) && IsUnoccluded(ViewOrigin, World, Component);
	}

	/**
	 * The FIRST renderable-visible primitive component of Actor (same iteration order + predicate as the
	 * actor test), or nullptr. Single source so GetVisibleRenderableActors and the A4 Infos pass classify
	 * the SAME actor set; the Infos pass additionally reads the matched component for its type + bounds.
	 */
	UPrimitiveComponent* FirstRenderableVisibleComponent(const FConvexVolume& Frustum, const FVector& ViewOrigin,
		const FVector& PollOrigin, float PollRadius, UWorld* World, const AActor* Actor)
	{
		TArray<UPrimitiveComponent*> Prims;
		Actor->GetComponents<UPrimitiveComponent>(Prims);
		for (UPrimitiveComponent* Prim : Prims)
		{
			if (IsComponentRenderableVisibleInternal(Frustum, ViewOrigin, PollOrigin, PollRadius, World, Prim))
			{
				return Prim;
			}
		}
		return nullptr;
	}

	/** Actor renderable-visibility against a prebuilt frustum: ANY component renderable-visible. Behaviour-
	 *  preserving wrapper over FirstRenderableVisibleComponent (same loop / predicate / short-circuit), so
	 *  GetVisibleRenderableActors stays BYTE-IDENTICAL (result + order). */
	bool IsActorRenderableVisibleInternal(const FConvexVolume& Frustum, const FVector& ViewOrigin,
		const FVector& PollOrigin, float PollRadius, UWorld* World, const AActor* Actor)
	{
		return FirstRenderableVisibleComponent(Frustum, ViewOrigin, PollOrigin, PollRadius, World, Actor) != nullptr;
	}

	/** Renderable component family tag for the read-back (matches the IsRenderableComponent allowlist;
	 *  ISM/HISM derive from UStaticMeshComponent -> "SM"). VFX was removed from the set (G33). */
	FString ClassifyRenderableComponent(const UPrimitiveComponent* Component)
	{
		if (Component->IsA<UStaticMeshComponent>())  { return TEXT("SM"); }
		if (Component->IsA<USkinnedMeshComponent>()) { return TEXT("SK"); }
		return TEXT("?");
	}

	/**
	 * Project a world-space AABB (bounds centre +/- box extent) to a normalized [0,1] screen rect via the
	 * view-projection matrix, top-left origin. Corners behind the camera (clip W <= 0) are skipped; returns
	 * false if none project or the rect is degenerate. Reversed-Z affects only depth, so X/Y is the standard
	 * perspective divide (G24 concerns the frustum near/far, not this projection).
	 */
	bool ProjectBoundsToScreenRect(const FMatrix& ViewProj, const FBoxSphereBounds& Bounds, FVector2D& OutMin, FVector2D& OutMax)
	{
		const FVector C = Bounds.Origin;
		const FVector E = Bounds.BoxExtent;

		double MinX = 1.0e30, MinY = 1.0e30, MaxX = -1.0e30, MaxY = -1.0e30;
		int32 NumInFront = 0;
		for (int32 Corner = 0; Corner < 8; ++Corner)
		{
			const FVector P = C + FVector(
				(Corner & 1) ? E.X : -E.X,
				(Corner & 2) ? E.Y : -E.Y,
				(Corner & 4) ? E.Z : -E.Z);

			const FVector4 Clip = ViewProj.TransformFVector4(FVector4(P, 1.0));
			if (Clip.W <= SMALL_NUMBER)
			{
				continue;   // behind the camera
			}
			const double InvW = 1.0 / Clip.W;
			const double Sx = (Clip.X * InvW) * 0.5 + 0.5;
			const double Sy = 1.0 - ((Clip.Y * InvW) * 0.5 + 0.5);   // flip Y for top-left origin
			MinX = FMath::Min(MinX, Sx); MaxX = FMath::Max(MaxX, Sx);
			MinY = FMath::Min(MinY, Sy); MaxY = FMath::Max(MaxY, Sy);
			++NumInFront;
		}

		if (NumInFront == 0)
		{
			return false;
		}
		OutMin = FVector2D(FMath::Clamp(MinX, 0.0, 1.0), FMath::Clamp(MinY, 0.0, 1.0));
		OutMax = FVector2D(FMath::Clamp(MaxX, 0.0, 1.0), FMath::Clamp(MaxY, 0.0, 1.0));
		return (OutMax.X > OutMin.X) && (OutMax.Y > OutMin.Y);
	}

	/**
	 * UNCLAMPED variant of ProjectBoundsToScreenRect for the capture/labeling milestone (L2): same VP /
	 * 8-corner / top-left-origin convention, but it returns the RAW normalized rect (values may fall outside
	 * [0,1] for a partially off-screen object — the label writer clamps for pixels) and reports how many
	 * corners are in front of the camera. The clamping ProjectBoundsToScreenRect above is left byte-identical
	 * (it feeds the dashboard overlay, where clamping to screen is wanted). Returns false (no corner in front)
	 * iff the box is entirely behind the camera.
	 */
	bool ProjectBoxToNormalizedRect(const FMatrix& ViewProj, const FVector& Center, const FVector& Extent,
		FVector2D& OutMin, FVector2D& OutMax)
	{
		double MinX = 1.0e30, MinY = 1.0e30, MaxX = -1.0e30, MaxY = -1.0e30;
		int32 NumInFront = 0;
		for (int32 Corner = 0; Corner < 8; ++Corner)
		{
			const FVector P = Center + FVector(
				(Corner & 1) ? Extent.X : -Extent.X,
				(Corner & 2) ? Extent.Y : -Extent.Y,
				(Corner & 4) ? Extent.Z : -Extent.Z);

			const FVector4 Clip = ViewProj.TransformFVector4(FVector4(P, 1.0));
			if (Clip.W <= SMALL_NUMBER)
			{
				continue;   // behind the camera
			}
			const double InvW = 1.0 / Clip.W;
			const double Sx = (Clip.X * InvW) * 0.5 + 0.5;
			const double Sy = 1.0 - ((Clip.Y * InvW) * 0.5 + 0.5);   // flip Y for top-left origin
			MinX = FMath::Min(MinX, Sx); MaxX = FMath::Max(MaxX, Sx);
			MinY = FMath::Min(MinY, Sy); MaxY = FMath::Max(MaxY, Sy);
			++NumInFront;
		}

		if (NumInFront == 0)
		{
			OutMin = OutMax = FVector2D::ZeroVector;
			return false;
		}
		OutMin = FVector2D(MinX, MinY);   // UNCLAMPED — may be < 0 or > 1
		OutMax = FVector2D(MaxX, MaxY);
		return true;
	}
}

namespace AnomalyViewport
{
	bool GetActiveViewInfo(UWorld* World, FAnomalyViewInfo& OutView)
	{
		OutView = FAnomalyViewInfo{};

		if (!World)
		{
			UE_LOG(LogAnomaly, Warning, TEXT("AnomalyViewport: no world; viewport scoping treated as unscoped."));
			return false;
		}

		APlayerController* PC = World->GetFirstPlayerController();
		if (!PC)
		{
			UE_LOG(LogAnomaly, Warning,
				TEXT("AnomalyViewport: no local player controller for world '%s' (e.g. a Simulate session); viewport scoping treated as unscoped."),
				*GetNameSafe(World));
			return false;
		}

		FVector Location;
		FRotator Rotation;
		PC->GetPlayerViewPoint(Location, Rotation);   // POV via the camera manager; no FViewport needed
		OutView.Origin = Location;
		OutView.Rotation = Rotation;

		float FOV = 90.0f;
		if (PC->PlayerCameraManager)
		{
			FOV = PC->PlayerCameraManager->GetFOVAngle();
		}
		OutView.HorizontalFOVDeg = (FOV > 1.0f) ? FOV : 90.0f;

		float Aspect = 16.0f / 9.0f;
		if (UGameViewportClient* ViewportClient = World->GetGameViewport())
		{
			FVector2D Size = FVector2D::ZeroVector;
			ViewportClient->GetViewportSize(Size);
			if (Size.X > 0.0f && Size.Y > 0.0f)
			{
				Aspect = Size.X / Size.Y;
			}
		}
		OutView.AspectRatio = Aspect;

		OutView.bValid = true;
		return true;
	}

	bool IsComponentInFrustum(const FAnomalyViewInfo& View, const UPrimitiveComponent* Component)
	{
		if (!View.bValid || !Component)
		{
			return false;
		}
		return IsInFrustum(BuildFrustum(View), Component);
	}

	bool IsComponentVisible(const FAnomalyViewInfo& View, UWorld* World, const UPrimitiveComponent* Component)
	{
		if (!View.bValid || !World || !Component)
		{
			return false;
		}
		return IsComponentVisibleInternal(BuildFrustum(View), View.Origin, World, Component);
	}

	bool IsActorVisible(const FAnomalyViewInfo& View, UWorld* World, const AActor* Actor)
	{
		if (!View.bValid || !World || !Actor)
		{
			return false;
		}
		return IsActorVisibleInternal(BuildFrustum(View), View.Origin, World, Actor);
	}

	TArray<TWeakObjectPtr<AActor>> FilterVisibleActors(const FAnomalyViewInfo& View, UWorld* World, const TArray<TWeakObjectPtr<AActor>>& In)
	{
		TArray<TWeakObjectPtr<AActor>> Result;
		if (!View.bValid || !World)
		{
			return Result;   // no valid view -> nothing classifies visible (callers handle treat-as-unscoped)
		}

		const FConvexVolume Frustum = BuildFrustum(View);   // build once for the whole set
		for (const TWeakObjectPtr<AActor>& Weak : In)
		{
			AActor* Actor = Weak.Get();
			if (Actor && IsActorVisibleInternal(Frustum, View.Origin, World, Actor))
			{
				Result.Add(Weak);
			}
		}
		return Result;
	}

	TArray<TWeakObjectPtr<AActor>> FindVisibleActorsMatching(UWorld* World, const FString& Substring)
	{
		TArray<TWeakObjectPtr<AActor>> Matched = AnomalyTargeting::FindActorsMatching(World, Substring);

		FAnomalyViewInfo View;
		if (!GetActiveViewInfo(World, View))
		{
			return Matched;   // no live view -> treat-as-unscoped (full set; already warned)
		}
		return FilterVisibleActors(View, World, Matched);
	}

	// --- Renderable-visible set (m5 follow-on; additive — the functions above are unchanged) ---

	bool IsRenderableComponent(const UPrimitiveComponent* Component)
	{
		if (!Component || !Component->IsVisible())   // IsVisible(): not hidden-in-game, visible flag, level visible
		{
			return false;
		}

		// Empty-instance guard (R1 refinement): an instanced static mesh with ZERO instances is visible-flagged
		// but draws nothing — e.g. the 0-instance landscape-grass ISMs that would otherwise leak a
		// LandscapeStreamingProxy back into the set. "Renders nothing => not renderable." Populated ISMs
		// (count > 0, incl. real foliage) and HISM (derives from ISM, so the Cast catches it) stay. This is a
		// capability refinement, NOT a class blocklist (stays game-agnostic; generalizes to any empty ISM).
		if (const UInstancedStaticMeshComponent* ISM = Cast<UInstancedStaticMeshComponent>(Component))
		{
			if (ISM->GetInstanceCount() <= 0)
			{
				return false;
			}
		}

		// Capability/TYPE allowlist (game-agnostic; not a class blocklist): static OR skeletal/skinned mesh.
		// VFX (UFXSystemComponent / Niagara + Cascade) was DELIBERATELY REMOVED from this set (G33, reverses
		// the G29/R1 inclusion): particles are not useful injectable geometry targets for the selector / auto /
		// dashboard set. The "=name" console escape hatch still reaches VFX actors (it bypasses this predicate).
		return Component->IsA<UStaticMeshComponent>()
			|| Component->IsA<USkinnedMeshComponent>();
		// EXTENSION POINT (intentionally inactive — landscape excluded for v1): to make terrain selectable,
		// add `|| Component->IsA<ULandscapeComponent>()` here (include Landscape/LandscapeComponent.h).
	}

	bool IsActorRenderableVisible(const FAnomalyViewInfo& View, UWorld* World, const AActor* Actor)
	{
		if (!View.bValid || !World || !Actor)
		{
			return false;
		}
		// Explicit-view (synthetic) surface: NOT poll-radius-culled (PollRadius 0). PollOrigin is unused at 0.
		return IsActorRenderableVisibleInternal(BuildFrustum(View), View.Origin, View.Origin, 0.0f, World, Actor);
	}

	TArray<TWeakObjectPtr<AActor>> FilterRenderableVisibleActors(const FAnomalyViewInfo& View, UWorld* World, const TArray<TWeakObjectPtr<AActor>>& In)
	{
		TArray<TWeakObjectPtr<AActor>> Result;
		if (!View.bValid || !World)
		{
			return Result;
		}
		const FConvexVolume Frustum = BuildFrustum(View);   // build once for the whole set
		for (const TWeakObjectPtr<AActor>& Weak : In)
		{
			AActor* Actor = Weak.Get();
			// Explicit-view (synthetic) surface: NOT poll-radius-culled (PollRadius 0).
			if (Actor && IsActorRenderableVisibleInternal(Frustum, View.Origin, View.Origin, 0.0f, World, Actor))
			{
				Result.Add(Weak);
			}
		}
		return Result;
	}

	TArray<TWeakObjectPtr<AActor>> GetVisibleRenderableActors(UWorld* World)
	{
		TArray<TWeakObjectPtr<AActor>> Result;

		FAnomalyViewInfo View;
		if (!GetActiveViewInfo(World, View))
		{
			// DELIBERATE: no resolvable view -> offer NOTHING (never inject/select blind). This is the
			// opposite of the FindVisible*Matching finders' treat-as-unscoped, and intentionally so —
			// see the header. GetActiveViewInfo already logged one warning.
			return Result;
		}

		const FConvexVolume Frustum = BuildFrustum(View);   // build once, then filter the whole world
		// Live poll: cull by the shared poll-radius around the player PAWN (camera origin as the no-pawn fallback).
		const FVector PollOrigin = ResolvePollOrigin(World, View.Origin);
		const float PollRadius = GPollRadius;
		for (TActorIterator<AActor> It(World); It; ++It)
		{
			AActor* Actor = *It;
			if (Actor && IsActorRenderableVisibleInternal(Frustum, View.Origin, PollOrigin, PollRadius, World, Actor))
			{
				Result.Add(Actor);
			}
		}
		return Result;
	}

	TArray<FRenderableActorInfo> GetVisibleRenderableActorInfos(UWorld* World)
	{
		TArray<FRenderableActorInfo> Result;

		FAnomalyViewInfo View;
		if (!GetActiveViewInfo(World, View))
		{
			return Result;   // offer nothing on no view (same contract as GetVisibleRenderableActors)
		}

		// Build the VP once; derive the frustum from it (the projection reuses the same VP) — no double build.
		const FMatrix ViewProj = BuildViewProjectionMatrix(View);
		FConvexVolume Frustum;
		GetViewFrustumBounds(Frustum, ViewProj, /*bUseNearPlane=*/true, /*bUseFarPlane=*/false);

		// SAME poll-radius cull as GetVisibleRenderableActors (identical PollOrigin + radius) so the actor SET
		// stays byte-identical between the two — preserving the IAI.DumpVisible set-identity gate.
		const FVector PollOrigin = ResolvePollOrigin(World, View.Origin);
		const float PollRadius = GPollRadius;

		for (TActorIterator<AActor> It(World); It; ++It)
		{
			AActor* Actor = *It;
			if (!Actor)
			{
				continue;
			}
			// ONE occlusion pass per actor (short-circuits on the first renderable-visible component) —
			// same predicate/order as GetVisibleRenderableActors, so the actor SET is identical.
			UPrimitiveComponent* Match = FirstRenderableVisibleComponent(Frustum, View.Origin, PollOrigin, PollRadius, World, Actor);
			if (!Match)
			{
				continue;
			}

			FRenderableActorInfo Info;
			Info.Actor = Actor;
			Info.ActorName = Actor->GetName();
			Info.ClassName = Actor->GetClass()->GetName();
			Info.ComponentType = ClassifyRenderableComponent(Match);
			// Display distance is CAMERA-relative (View.Origin) — deliberately distinct from the poll-radius
			// cull, which measures from the PAWN. Two metrics, two purposes; do not "reconcile" them.
			Info.Distance = (float)(Match->Bounds.Origin - View.Origin).Size();
			Info.bRectValid = ProjectBoundsToScreenRect(ViewProj, Match->Bounds, Info.ScreenMin, Info.ScreenMax);
			Result.Add(MoveTemp(Info));
		}
		return Result;
	}

	bool ProjectActorBoundsToScreenRect(const FAnomalyViewInfo& View, const AActor* Actor, FVector2D& OutMin, FVector2D& OutMax)
	{
		OutMin = OutMax = FVector2D::ZeroVector;
		if (!View.bValid || !Actor)
		{
			return false;
		}

		// Union the actor's static- + skeletal-mesh component bounds, selected by TYPE ONLY (NOT IsVisible()):
		// a hidden missing_object / flicker actor still contributes its persisted bounds — that is the point
		// (the label box must mark where the now-hidden object is). Mirrors the IsRenderableComponent allowlist
		// families minus the visibility gate.
		FBox Box(ForceInit);
		TArray<UPrimitiveComponent*> Prims;
		Actor->GetComponents<UPrimitiveComponent>(Prims);
		for (const UPrimitiveComponent* Prim : Prims)
		{
			if (Prim && (Prim->IsA<UStaticMeshComponent>() || Prim->IsA<USkinnedMeshComponent>()))
			{
				Box += Prim->Bounds.GetBox();
			}
		}
		if (!Box.IsValid)
		{
			return false;   // no mesh component to bound
		}

		const FMatrix ViewProj = BuildViewProjectionMatrix(View);
		if (!ProjectBoxToNormalizedRect(ViewProj, Box.GetCenter(), Box.GetExtent(), OutMin, OutMax))
		{
			return false;   // entirely behind the camera
		}

		// Off-screen iff the (unclamped) rect does not intersect the [0,1]x[0,1] screen.
		const bool bIntersectsScreen =
			(OutMax.X > 0.0) && (OutMin.X < 1.0) && (OutMax.Y > 0.0) && (OutMin.Y < 1.0);
		return bIntersectsScreen;
	}

	// --- Poll-radius accessors (shared state lives in the anonymous namespace above) ---

	void SetPollRadius(float Radius)
	{
		const bool bWasOn = (GPollRadius > 0.0f);
		const bool bNowOn = (Radius > 0.0f);
		GPollRadius = Radius;

		// Dev debug-draw sphere lifecycle (G25 hygiene): register on OFF->ON, unregister on ON->OFF. While ON the
		// delegate self-gates on R > 0 anyway; this keeps a dangling delegate from lingering once the cull is off.
		if (bNowOn && !bWasOn && !GPollRadiusDrawHandle.IsValid())
		{
			GPollRadiusDrawHandle = UDebugDrawService::Register(TEXT("Game"), FDebugDrawDelegate::CreateStatic(&DrawPollRadiusHUD));
		}
		else if (!bNowOn && bWasOn && GPollRadiusDrawHandle.IsValid())
		{
			UDebugDrawService::Unregister(GPollRadiusDrawHandle);
			GPollRadiusDrawHandle.Reset();
		}
	}

	float GetPollRadius()
	{
		return GPollRadius;
	}
}

// Console: IAI.SetPollRadius <value> — set the renderable-visible poll-radius cull (cm) around the player pawn.
// Parallels IAI.SetViewportScoping, but it sets a world-independent global, so it is a plain FAutoConsoleCommand
// living here in AnomalyViewport.cpp (state + accessor + command co-located). Defensive: a bare call with no arg
// logs the current radius + usage and changes nothing (FCString::Atof on a missing Args[0] would crash).
static FAutoConsoleCommand GSetPollRadiusCmd(
	TEXT("IAI.SetPollRadius"),
	TEXT("Set the renderable-visible poll-radius distance cull around the player pawn, in Unreal units (cm). ")
	TEXT("<= 0 disables it (default OFF). With no argument, prints the current radius. Usage: IAI.SetPollRadius <value>"),
	FConsoleCommandWithArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args)
		{
			if (Args.Num() < 1)
			{
				const float Cur = AnomalyViewport::GetPollRadius();
				UE_LOG(LogAnomaly, Log, TEXT("IAI.SetPollRadius: current radius = %.1f cm (cull %s). Usage: IAI.SetPollRadius <value>"),
					Cur, (Cur > 0.0f) ? TEXT("ON") : TEXT("OFF"));
				return;
			}
			const float R = FCString::Atof(*Args[0]);
			AnomalyViewport::SetPollRadius(R);
			UE_LOG(LogAnomaly, Log, TEXT("IAI.SetPollRadius -> %.1f cm (cull %s)."), R, (R > 0.0f) ? TEXT("ON") : TEXT("OFF"));
		}));
