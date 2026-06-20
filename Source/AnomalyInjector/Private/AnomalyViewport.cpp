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
#include "GameFramework/PlayerController.h"
#include "EngineUtils.h"                  // TActorIterator (GetVisibleRenderableActors)
#include "Engine/World.h"                 // UWorld::LineTraceSingleByChannel / GetFirstPlayerController / GetGameViewport
#include "Engine/GameViewportClient.h"    // GetViewportSize (aspect)
#include "Engine/EngineTypes.h"           // ECC_Visibility
#include "Engine/HitResult.h"             // FHitResult (5.1)
#include "CollisionQueryParams.h"         // FCollisionQueryParams

namespace
{
	using namespace AnomalyViewport;

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
	 * Renderable-visible test (m5 follow-on). The renderability check runs FIRST (cheap type+flag test,
	 * IsRenderableComponent is declared in the header / defined below) so non-rendering primitives are
	 * rejected before the occlusion line traces — correctness AND a perf win.
	 */
	bool IsComponentRenderableVisibleInternal(const FConvexVolume& Frustum, const FVector& ViewOrigin, UWorld* World, const UPrimitiveComponent* Component)
	{
		return AnomalyViewport::IsRenderableComponent(Component)
			&& IsInFrustum(Frustum, Component)
			&& IsUnoccluded(ViewOrigin, World, Component);
	}

	/**
	 * The FIRST renderable-visible primitive component of Actor (same iteration order + predicate as the
	 * actor test), or nullptr. Single source so GetVisibleRenderableActors and the A4 Infos pass classify
	 * the SAME actor set; the Infos pass additionally reads the matched component for its type + bounds.
	 */
	UPrimitiveComponent* FirstRenderableVisibleComponent(const FConvexVolume& Frustum, const FVector& ViewOrigin, UWorld* World, const AActor* Actor)
	{
		TArray<UPrimitiveComponent*> Prims;
		Actor->GetComponents<UPrimitiveComponent>(Prims);
		for (UPrimitiveComponent* Prim : Prims)
		{
			if (IsComponentRenderableVisibleInternal(Frustum, ViewOrigin, World, Prim))
			{
				return Prim;
			}
		}
		return nullptr;
	}

	/** Actor renderable-visibility against a prebuilt frustum: ANY component renderable-visible. Behaviour-
	 *  preserving wrapper over FirstRenderableVisibleComponent (same loop / predicate / short-circuit), so
	 *  GetVisibleRenderableActors stays BYTE-IDENTICAL (result + order). */
	bool IsActorRenderableVisibleInternal(const FConvexVolume& Frustum, const FVector& ViewOrigin, UWorld* World, const AActor* Actor)
	{
		return FirstRenderableVisibleComponent(Frustum, ViewOrigin, World, Actor) != nullptr;
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
		return IsActorRenderableVisibleInternal(BuildFrustum(View), View.Origin, World, Actor);
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
			if (Actor && IsActorRenderableVisibleInternal(Frustum, View.Origin, World, Actor))
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
		for (TActorIterator<AActor> It(World); It; ++It)
		{
			AActor* Actor = *It;
			if (Actor && IsActorRenderableVisibleInternal(Frustum, View.Origin, World, Actor))
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

		for (TActorIterator<AActor> It(World); It; ++It)
		{
			AActor* Actor = *It;
			if (!Actor)
			{
				continue;
			}
			// ONE occlusion pass per actor (short-circuits on the first renderable-visible component) —
			// same predicate/order as GetVisibleRenderableActors, so the actor SET is identical.
			UPrimitiveComponent* Match = FirstRenderableVisibleComponent(Frustum, View.Origin, World, Actor);
			if (!Match)
			{
				continue;
			}

			FRenderableActorInfo Info;
			Info.Actor = Actor;
			Info.ActorName = Actor->GetName();
			Info.ClassName = Actor->GetClass()->GetName();
			Info.ComponentType = ClassifyRenderableComponent(Match);
			Info.Distance = (float)(Match->Bounds.Origin - View.Origin).Size();
			Info.bRectValid = ProjectBoundsToScreenRect(ViewProj, Match->Bounds, Info.ScreenMin, Info.ScreenMax);
			Result.Add(MoveTemp(Info));
		}
		return Result;
	}
}
