#include "AnomalyViewport.h"

#include "AnomalyTargeting.h"
#include "AnomalyInjectorLog.h"

#include "ConvexVolume.h"
#include "Camera/CameraTypes.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/PrimitiveComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/SkinnedMeshComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "Engine/GameViewportClient.h"
#include "Engine/EngineTypes.h"
#include "Engine/HitResult.h"
#include "CollisionQueryParams.h"
#include "HAL/IConsoleManager.h"
#include "Engine/EngineBaseTypes.h"
#include "DrawDebugHelpers.h"

namespace
{
	using namespace AnomalyViewport;

	float GPollRadius = 1800.0f;
	FDelegateHandle GPollRadiusDrawHandle;
	bool GOverlaysSuppressed = false;

	float GMinScreenCoveragePct = 6.0f;

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

	void DrawPollRadiusTick(UWorld* World, ELevelTick  , float  )
	{
		const float R = GPollRadius;
		if (R <= 0.0f || GOverlaysSuppressed || !World || !World->IsGameWorld())
		{
			return;
		}
		APlayerController* PC = World->GetFirstPlayerController();
		if (!PC)
		{
			return;
		}
		FVector Center;
		if (const APawn* Pawn = PC->GetPawn())
		{
			Center = Pawn->GetActorLocation();
		}
		else
		{
			FRotator Unused;
			PC->GetPlayerViewPoint(Center, Unused);
		}
		DrawDebugSphere(World, Center, R, 24, FColor::Yellow,  false,  -1.0f,  0,  2.0f);
	}

	FMatrix BuildViewProjectionMatrix(const FAnomalyViewInfo& View)
	{
		FMinimalViewInfo VI;
		VI.Location = View.Origin;
		VI.Rotation = View.Rotation;
		VI.FOV = View.HorizontalFOVDeg;
		VI.AspectRatio = View.AspectRatio;
		VI.ProjectionMode = ECameraProjectionMode::Perspective;
		VI.bConstrainAspectRatio = false;
		const FMatrix ProjectionMatrix = VI.CalculateProjectionMatrix();

		const FMatrix ViewRotationMatrix = FInverseRotationMatrix(View.Rotation) * FMatrix(
			FPlane(0, 0, 1, 0),
			FPlane(1, 0, 0, 0),
			FPlane(0, 1, 0, 0),
			FPlane(0, 0, 0, 1));

		return FTranslationMatrix(-View.Origin) * ViewRotationMatrix * ProjectionMatrix;
	}

	FConvexVolume BuildFrustum(const FAnomalyViewInfo& View)
	{
		FConvexVolume Frustum;
		GetViewFrustumBounds(Frustum, BuildViewProjectionMatrix(View),  true,  false);
		return Frustum;
	}

	bool IsInFrustum(const FConvexVolume& Frustum, const UPrimitiveComponent* Component)
	{
		const FBoxSphereBounds& B = Component->Bounds;
		return Frustum.IntersectSphere(B.Origin, B.SphereRadius) && Frustum.IntersectBox(B.Origin, B.BoxExtent);
	}

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

		FCollisionQueryParams Params(FName(TEXT("AnomalyViewportOcclusion")),  false);
		if (const AActor* Owner = Component->GetOwner())
		{
			Params.AddIgnoredActor(Owner);
		}

		for (const FVector& Target : Samples)
		{
			FHitResult Hit;
			const bool bBlocked = World->LineTraceSingleByChannel(Hit, ViewOrigin, Target, ECC_Visibility, Params);
			if (!bBlocked)
			{
				return true;
			}
		}
		return false;
	}

	bool IsComponentVisibleInternal(const FConvexVolume& Frustum, const FVector& ViewOrigin, UWorld* World, const UPrimitiveComponent* Component)
	{
		return Component && IsInFrustum(Frustum, Component) && IsUnoccluded(ViewOrigin, World, Component);
	}

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

	bool IsComponentRenderableVisibleInternal(const FConvexVolume& Frustum, const FVector& ViewOrigin,
		const FVector& PollOrigin, float PollRadius, UWorld* World, const UPrimitiveComponent* Component)
	{
		if (!AnomalyViewport::IsRenderableComponent(Component))
		{
			return false;
		}
		if (PollRadius > 0.0f)
		{
			const FBoxSphereBounds& B = Component->Bounds;
			if ((float)FVector::Dist(PollOrigin, B.Origin) - (float)B.SphereRadius > PollRadius)
			{
				return false;
			}
		}
		return IsInFrustum(Frustum, Component) && IsUnoccluded(ViewOrigin, World, Component);
	}

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

	bool IsActorRenderableVisibleInternal(const FConvexVolume& Frustum, const FVector& ViewOrigin,
		const FVector& PollOrigin, float PollRadius, UWorld* World, const AActor* Actor)
	{
		return FirstRenderableVisibleComponent(Frustum, ViewOrigin, PollOrigin, PollRadius, World, Actor) != nullptr;
	}

	FString ClassifyRenderableComponent(const UPrimitiveComponent* Component)
	{
		if (Component->IsA<UStaticMeshComponent>())  { return TEXT("SM"); }
		if (Component->IsA<USkinnedMeshComponent>()) { return TEXT("SK"); }
		return TEXT("?");
	}

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
				continue;
			}
			const double InvW = 1.0 / Clip.W;
			const double Sx = (Clip.X * InvW) * 0.5 + 0.5;
			const double Sy = 1.0 - ((Clip.Y * InvW) * 0.5 + 0.5);
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
				continue;
			}
			const double InvW = 1.0 / Clip.W;
			const double Sx = (Clip.X * InvW) * 0.5 + 0.5;
			const double Sy = 1.0 - ((Clip.Y * InvW) * 0.5 + 0.5);
			MinX = FMath::Min(MinX, Sx); MaxX = FMath::Max(MaxX, Sx);
			MinY = FMath::Min(MinY, Sy); MaxY = FMath::Max(MaxY, Sy);
			++NumInFront;
		}

		if (NumInFront == 0)
		{
			OutMin = OutMax = FVector2D::ZeroVector;
			return false;
		}
		OutMin = FVector2D(MinX, MinY);
		OutMax = FVector2D(MaxX, MaxY);
		return true;
	}


	UPrimitiveComponent* CollectRenderableVisibleUnion(const FConvexVolume& Frustum, const FVector& ViewOrigin,
		const FVector& PollOrigin, float PollRadius, UWorld* World, const AActor* Actor, FBox& OutUnion)
	{
		UPrimitiveComponent* First = nullptr;
		TArray<UPrimitiveComponent*> Prims;
		Actor->GetComponents<UPrimitiveComponent>(Prims);
		for (UPrimitiveComponent* Prim : Prims)
		{
			if (IsComponentRenderableVisibleInternal(Frustum, ViewOrigin, PollOrigin, PollRadius, World, Prim))
			{
				if (!First)
				{
					First = Prim;
				}
				OutUnion += Prim->Bounds.GetBox();
			}
		}
		return First;
	}

	bool PassesScreenCoverage(const FMatrix& ViewProj, const FBox& UnionBox, float MinPct)
	{
		if (MinPct <= 0.0f)
		{
			return true;
		}
		FVector2D Min(FVector2D::ZeroVector), Max(FVector2D::ZeroVector);
		if (!ProjectBoundsToScreenRect(ViewProj, FBoxSphereBounds(UnionBox), Min, Max))
		{
			return false;
		}
		const float CoveragePct = (float)((Max.X - Min.X) * (Max.Y - Min.Y)) * 100.0f;
		return CoveragePct >= MinPct;
	}

	int32 CountUnoccludedSamples(const FVector& ViewOrigin, UWorld* World, const UPrimitiveComponent* Component,
		int32& OutTotal)
	{
		OutTotal = 0;
		if (!World || !Component)
		{
			return 0;
		}

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

		FCollisionQueryParams Params(FName(TEXT("AnomalyViewportProvenance")), false);
		if (const AActor* Owner = Component->GetOwner())
		{
			Params.AddIgnoredActor(Owner);
		}

		int32 Passed = 0;
		for (const FVector& Target : Samples)
		{
			FHitResult Hit;
			if (!World->LineTraceSingleByChannel(Hit, ViewOrigin, Target, ECC_Visibility, Params))
			{
				++Passed;
			}
			++OutTotal;
		}
		return Passed;
	}

	bool ClassifyRenderableVisibleLive(const FConvexVolume& Frustum, const FMatrix& ViewProj, const FVector& ViewOrigin,
		const FVector& PollOrigin, float PollRadius, float MinCoveragePct, UWorld* World, const AActor* Actor,
		UPrimitiveComponent*& OutFirstMatch)
	{
		if (MinCoveragePct <= 0.0f)
		{
			OutFirstMatch = FirstRenderableVisibleComponent(Frustum, ViewOrigin, PollOrigin, PollRadius, World, Actor);
			return OutFirstMatch != nullptr;
		}

		FBox Union(ForceInit);
		OutFirstMatch = CollectRenderableVisibleUnion(Frustum, ViewOrigin, PollOrigin, PollRadius, World, Actor, Union);
		if (!OutFirstMatch || !Union.IsValid)
		{
			return false;
		}
		return PassesScreenCoverage(ViewProj, Union, MinCoveragePct);
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
		PC->GetPlayerViewPoint(Location, Rotation);
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
			return Result;
		}

		const FConvexVolume Frustum = BuildFrustum(View);
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
			return Matched;
		}
		return FilterVisibleActors(View, World, Matched);
	}


	bool IsRenderableComponent(const UPrimitiveComponent* Component)
	{
		if (!Component || !Component->IsVisible())
		{
			return false;
		}

		if (const UInstancedStaticMeshComponent* ISM = Cast<UInstancedStaticMeshComponent>(Component))
		{
			if (ISM->GetInstanceCount() <= 0)
			{
				return false;
			}
		}

		return Component->IsA<UStaticMeshComponent>()
			|| Component->IsA<USkinnedMeshComponent>();
	}

	bool IsActorRenderableVisible(const FAnomalyViewInfo& View, UWorld* World, const AActor* Actor)
	{
		if (!View.bValid || !World || !Actor)
		{
			return false;
		}
		return IsActorRenderableVisibleInternal(BuildFrustum(View), View.Origin, View.Origin, 0.0f, World, Actor);
	}

	TArray<TWeakObjectPtr<AActor>> FilterRenderableVisibleActors(const FAnomalyViewInfo& View, UWorld* World, const TArray<TWeakObjectPtr<AActor>>& In)
	{
		TArray<TWeakObjectPtr<AActor>> Result;
		if (!View.bValid || !World)
		{
			return Result;
		}
		const FConvexVolume Frustum = BuildFrustum(View);
		for (const TWeakObjectPtr<AActor>& Weak : In)
		{
			AActor* Actor = Weak.Get();
			if (Actor && IsActorRenderableVisibleInternal(Frustum, View.Origin, View.Origin, 0.0f, World, Actor))
			{
				Result.Add(Weak);
			}
		}
		return Result;
	}

	bool EvaluateSelectionProvenance(UWorld* World, const AActor* Actor, FSelectionProvenance& Out)
	{
		Out = FSelectionProvenance();
		if (!World || !Actor)
		{
			return false;
		}

		FAnomalyViewInfo View;
		if (!GetActiveViewInfo(World, View))
		{
			return false;
		}

		const FConvexVolume Frustum = BuildFrustum(View);
		const FMatrix ViewProj = BuildViewProjectionMatrix(View);
		const FVector PollOrigin = ResolvePollOrigin(World, View.Origin);

		FBox Union(ForceInit);
		UPrimitiveComponent* First = CollectRenderableVisibleUnion(Frustum, View.Origin, PollOrigin,
			GPollRadius, World, Actor, Union);
		if (!First)
		{
			return false;
		}

		FVector2D Min(FVector2D::ZeroVector), Max(FVector2D::ZeroVector);
		if (ProjectBoundsToScreenRect(ViewProj, FBoxSphereBounds(Union), Min, Max))
		{
			Out.CoveragePct = (float)((Max.X - Min.X) * (Max.Y - Min.Y)) * 100.0f;
		}

		Out.OcclusionSamplesPassed = CountUnoccludedSamples(View.Origin, World, First, Out.OcclusionSamplesTotal);

		const FBoxSphereBounds& B = First->Bounds;
		Out.PollDistance = (float)(FVector::Dist(PollOrigin, B.Origin) - B.SphereRadius);

		Out.bValid = true;
		return true;
	}

	TArray<TWeakObjectPtr<AActor>> GetVisibleRenderableActors(UWorld* World)
	{
		TArray<TWeakObjectPtr<AActor>> Result;

		FAnomalyViewInfo View;
		if (!GetActiveViewInfo(World, View))
		{
			return Result;
		}

		const FMatrix ViewProj = BuildViewProjectionMatrix(View);
		FConvexVolume Frustum;
		GetViewFrustumBounds(Frustum, ViewProj,  true,  false);

		const FVector PollOrigin = ResolvePollOrigin(World, View.Origin);
		const float PollRadius = GPollRadius;
		const float MinCoveragePct = GMinScreenCoveragePct;
		for (TActorIterator<AActor> It(World); It; ++It)
		{
			AActor* Actor = *It;
			UPrimitiveComponent* Match = nullptr;
			if (Actor && ClassifyRenderableVisibleLive(Frustum, ViewProj, View.Origin, PollOrigin, PollRadius, MinCoveragePct, World, Actor, Match))
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
			return Result;
		}

		const FMatrix ViewProj = BuildViewProjectionMatrix(View);
		FConvexVolume Frustum;
		GetViewFrustumBounds(Frustum, ViewProj,  true,  false);

		const FVector PollOrigin = ResolvePollOrigin(World, View.Origin);
		const float PollRadius = GPollRadius;
		const float MinCoveragePct = GMinScreenCoveragePct;

		for (TActorIterator<AActor> It(World); It; ++It)
		{
			AActor* Actor = *It;
			if (!Actor)
			{
				continue;
			}
			UPrimitiveComponent* Match = nullptr;
			if (!ClassifyRenderableVisibleLive(Frustum, ViewProj, View.Origin, PollOrigin, PollRadius, MinCoveragePct, World, Actor, Match))
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

	bool ProjectActorBoundsToScreenRect(const FAnomalyViewInfo& View, const AActor* Actor, FVector2D& OutMin, FVector2D& OutMax)
	{
		OutMin = OutMax = FVector2D::ZeroVector;
		if (!View.bValid || !Actor)
		{
			return false;
		}

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
			return false;
		}

		const FMatrix ViewProj = BuildViewProjectionMatrix(View);
		if (!ProjectBoxToNormalizedRect(ViewProj, Box.GetCenter(), Box.GetExtent(), OutMin, OutMax))
		{
			return false;
		}

		const bool bIntersectsScreen =
			(OutMax.X > 0.0) && (OutMin.X < 1.0) && (OutMax.Y > 0.0) && (OutMin.Y < 1.0);
		return bIntersectsScreen;
	}


	void SetPollRadius(float Radius)
	{
		const bool bWasOn = (GPollRadius > 0.0f);
		const bool bNowOn = (Radius > 0.0f);
		GPollRadius = Radius;

		if (bNowOn && !bWasOn && !GPollRadiusDrawHandle.IsValid())
		{
			GPollRadiusDrawHandle = FWorldDelegates::OnWorldPostActorTick.AddStatic(&DrawPollRadiusTick);
			UE_LOG(LogAnomaly, Log, TEXT("IAI.SetPollRadius: debug sphere draw hook registered (real Play only)."));
		}
		else if (!bNowOn && bWasOn && GPollRadiusDrawHandle.IsValid())
		{
			FWorldDelegates::OnWorldPostActorTick.Remove(GPollRadiusDrawHandle);
			GPollRadiusDrawHandle.Reset();
		}
	}

	float GetPollRadius()
	{
		return GPollRadius;
	}

	void SetOverlaysSuppressed(bool bSuppressed)
	{
		GOverlaysSuppressed = bSuppressed;
	}

	bool AreOverlaysSuppressed()
	{
		return GOverlaysSuppressed;
	}

	void SetMinScreenCoveragePct(float Pct)
	{
		GMinScreenCoveragePct = (Pct <= 0.0f) ? 0.0f : FMath::Clamp(Pct, 0.0f, 100.0f);
	}

	float GetMinScreenCoveragePct()
	{
		return GMinScreenCoveragePct;
	}
}

static FAutoConsoleCommand GSetPollRadiusCmd(
	TEXT("IAI.SetPollRadius"),
	TEXT("Set the renderable-visible poll-radius distance cull around the player pawn, in Unreal units (cm). ")
	TEXT("<= 0 disables it (default 1800 cm = 18 m). With no argument, prints the current radius. Usage: IAI.SetPollRadius <value>"),
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

static FAutoConsoleCommand GSetMinScreenCoverageCmd(
	TEXT("IAI.SetMinScreenCoverage"),
	TEXT("Set the renderable-visible minimum on-screen coverage cull, as a PERCENT of the viewport area [0,100]. ")
	TEXT("<= 0 disables it (default 6%). With no argument, prints the current value. Usage: IAI.SetMinScreenCoverage <pct>"),
	FConsoleCommandWithArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args)
		{
			if (Args.Num() < 1)
			{
				const float Cur = AnomalyViewport::GetMinScreenCoveragePct();
				UE_LOG(LogAnomaly, Log, TEXT("IAI.SetMinScreenCoverage: current = %.2f%% (cull %s). Usage: IAI.SetMinScreenCoverage <pct>"),
					Cur, (Cur > 0.0f) ? TEXT("ON") : TEXT("OFF"));
				return;
			}
			const float P = FCString::Atof(*Args[0]);
			AnomalyViewport::SetMinScreenCoveragePct(P);
			const float Now = AnomalyViewport::GetMinScreenCoveragePct();
			UE_LOG(LogAnomaly, Log, TEXT("IAI.SetMinScreenCoverage -> %.2f%% (cull %s)."), Now, (Now > 0.0f) ? TEXT("ON") : TEXT("OFF"));
		}));

static FAutoConsoleCommandWithWorldAndArgs GDumpCoverageCmd(
	TEXT("IAI.DumpCoverage"),
	TEXT("Diagnostic: log each renderable-visible actor's on-screen coverage percent (ascending), marking which would ")
	TEXT("be culled at the current IAI.SetMinScreenCoverage threshold."),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>&  , UWorld* World)
		{
			FAnomalyViewInfo View;
			if (!AnomalyViewport::GetActiveViewInfo(World, View))
			{
				UE_LOG(LogAnomaly, Log, TEXT("IAI.DumpCoverage: no resolvable view (offer-nothing) — nothing to report."));
				return;
			}

			const FMatrix ViewProj = BuildViewProjectionMatrix(View);
			FConvexVolume Frustum;
			GetViewFrustumBounds(Frustum, ViewProj,  true,  false);
			const FVector PollOrigin = ResolvePollOrigin(World, View.Origin);
			const float PollRadius = GPollRadius;
			const float Threshold = GMinScreenCoveragePct;

			struct FCovRow { float Pct; FString Name; FString Class; };
			TArray<FCovRow> Rows;
			for (TActorIterator<AActor> It(World); It; ++It)
			{
				AActor* Actor = *It;
				if (!Actor)
				{
					continue;
				}
				FBox Union(ForceInit);
				if (!CollectRenderableVisibleUnion(Frustum, View.Origin, PollOrigin, PollRadius, World, Actor, Union) || !Union.IsValid)
				{
					continue;
				}
				float Pct = 0.0f;
				FVector2D Min(FVector2D::ZeroVector), Max(FVector2D::ZeroVector);
				if (ProjectBoundsToScreenRect(ViewProj, FBoxSphereBounds(Union), Min, Max))
				{
					Pct = (float)((Max.X - Min.X) * (Max.Y - Min.Y)) * 100.0f;
				}
				Rows.Add({ Pct, Actor->GetName(), Actor->GetClass()->GetName() });
			}

			Rows.Sort([](const FCovRow& A, const FCovRow& B) { return A.Pct < B.Pct; });

			UE_LOG(LogAnomaly, Log, TEXT("--- IAI.DumpCoverage: %d renderable-visible actor(s) | threshold = %.2f%% (cull %s) ---"),
				Rows.Num(), Threshold, (Threshold > 0.0f) ? TEXT("ON") : TEXT("OFF"));
			for (const FCovRow& Row : Rows)
			{
				const bool bCulled = (Threshold > 0.0f) && (Row.Pct < Threshold);
				UE_LOG(LogAnomaly, Log, TEXT("  %7.3f%%  %-44s [%s]%s"),
					Row.Pct, *Row.Name, *Row.Class, bCulled ? TEXT("  <-- CULLED") : TEXT(""));
			}
		}));
