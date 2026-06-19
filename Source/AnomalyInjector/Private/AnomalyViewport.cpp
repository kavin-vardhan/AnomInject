// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#include "AnomalyViewport.h"

#include "AnomalyTargeting.h"
#include "AnomalyInjectorLog.h"

#include "ConvexVolume.h"                  // FConvexVolume, GetViewFrustumBounds
#include "Camera/CameraTypes.h"           // FMinimalViewInfo::CalculateProjectionMatrix (reversed-Z)
#include "Camera/PlayerCameraManager.h"   // APlayerCameraManager::GetFOVAngle
#include "Components/PrimitiveComponent.h" // Bounds, GetOwner
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
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
}
