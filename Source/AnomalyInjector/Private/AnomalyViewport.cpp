#include "AnomalyViewport.h"

#include "AnomalyNearClipSlab.h"
#include "AnomalyTargeting.h"
#include "AnomalyInjectorLog.h"
#include "AnomalyDefaults.h"
#include "AnomalyTargetPolicy.h"

#include "Engine/StaticMesh.h"
#include "Engine/SkinnedAsset.h"

#include "ConvexVolume.h"
#include "SceneManagement.h"
#include "Camera/CameraTypes.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/PrimitiveComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Components/SkinnedMeshComponent.h"
#include "PhysicsEngine/BodyInstance.h"
#include "PhysicsEngine/BodySetup.h"
#include "UObject/UnrealType.h"
#include "InstancedFoliageActor.h"
#include "Materials/MaterialInterface.h"
#include "MaterialShared.h"
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
#include "Engine/Level.h"
#include "Engine/LocalPlayer.h"
#include "Components/ModelComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "HAL/PlatformTime.h"

namespace
{
	using namespace AnomalyViewport;

	float GPollRadius = 1800.0f;
	FDelegateHandle GPollRadiusDrawHandle;
	bool GOverlaysSuppressed = false;

	float GMinScreenCoveragePct = 6.0f;

	thread_local int32 GReadOnlyEnumerationDepth = 0;

	TSet<FString>& ExcludedActorsSeen()
	{
		static TSet<FString> Seen;
		return Seen;
	}

	TSet<FString>& TranslucentOnlyActorsSeen()
	{
		static TSet<FString> Seen;
		return Seen;
	}

	TSet<FString>& NaniteRefusedActorsSeen()
	{
		static TSet<FString> Seen;
		return Seen;
	}

	int32 GNaniteRefusalAttempts = 0;

	int32 GNaniteProbeMissingRefusals = 0;

	AnomalyViewport::FNaniteComponentProbe GNaniteComponentProbe = nullptr;

	FString MeshAssetNameOf(const UPrimitiveComponent* Component)
	{
		if (const UStaticMeshComponent* SMC = Cast<UStaticMeshComponent>(Component))
		{
			if (const UStaticMesh* Mesh = SMC->GetStaticMesh())
			{
				return Mesh->GetName();
			}
		}
		else if (const USkinnedMeshComponent* SKC = Cast<USkinnedMeshComponent>(Component))
		{
			if (const USkinnedAsset* Asset = SKC->GetSkinnedAsset())
			{
				return Asset->GetName();
			}
		}
		return FString();
	}

	bool MatchesExcludedTargetPattern(const UPrimitiveComponent* Component)
	{
		const TArray<FString>& Patterns = AnomalyDefaults::GetExcludedTargetPatterns();
		if (Patterns.Num() == 0)
		{
			return false;
		}

		const AActor* Owner = Component->GetOwner();
		const FString ActorName = Owner ? Owner->GetName() : FString();
		const FString CompName = Component->GetName();
		const FString AssetName = MeshAssetNameOf(Component);

		for (const FString& Pattern : Patterns)
		{
			const TCHAR* Field = nullptr;
			const FString* Value = nullptr;
			if (!ActorName.IsEmpty() && ActorName.Contains(Pattern, ESearchCase::IgnoreCase))
			{
				Field = TEXT("actor");
				Value = &ActorName;
			}
			else if (!CompName.IsEmpty() && CompName.Contains(Pattern, ESearchCase::IgnoreCase))
			{
				Field = TEXT("component");
				Value = &CompName;
			}
			else if (!AssetName.IsEmpty() && AssetName.Contains(Pattern, ESearchCase::IgnoreCase))
			{
				Field = TEXT("asset");
				Value = &AssetName;
			}

			if (Field)
			{
				if (AnomalyViewport::IsReadOnlyEnumeration())
				{
					return true;
				}
				bool bAlready = false;
				ExcludedActorsSeen().Add(ActorName, &bAlready);
				if (!bAlready)
				{
					UE_LOG(LogAnomaly, Warning,
						TEXT("EXCLUDED-TARGET actor='%s' matched_field=%s matched_value='%s' pattern='%s' source=%s — ")
						TEXT("refused as an injection candidate. This is a LABEL-QUALITY exclusion: the anomaly would ")
						TEXT("still occur, but its label would point at something a viewer cannot see change."),
						*ActorName, Field, **Value, *Pattern, *AnomalyDefaults::ExcludedTargetPatternsSource());
				}
				return true;
			}
		}
		return false;
	}

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

	FMatrix BuildProjectionMatrix(const FAnomalyViewInfo& View)
	{
		FMinimalViewInfo VI;
		VI.Location = View.Origin;
		VI.Rotation = View.Rotation;
		VI.FOV = View.HorizontalFOVDeg;
		VI.AspectRatio = View.AspectRatio;
		VI.ProjectionMode = ECameraProjectionMode::Perspective;
		VI.bConstrainAspectRatio = false;
		return VI.CalculateProjectionMatrix();
	}

	FMatrix BuildViewProjectionMatrix(const FAnomalyViewInfo& View)
	{
		const FMatrix ProjectionMatrix = BuildProjectionMatrix(View);

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

	bool IsActorRenderableTranslucentOnly(const FConvexVolume& Frustum, const FVector& ViewOrigin,
		const FVector& PollOrigin, float PollRadius, UWorld* World, const AActor* Actor)
	{
		TArray<UPrimitiveComponent*> Prims;
		const_cast<AActor*>(Actor)->GetComponents<UPrimitiveComponent>(Prims);
		int32 Renderable = 0;
		for (UPrimitiveComponent* Prim : Prims)
		{
			if (!IsComponentRenderableVisibleInternal(Frustum, ViewOrigin, PollOrigin, PollRadius, World, Prim))
			{
				continue;
			}
			++Renderable;
			if (!AnomalyViewport::IsTranslucentOnlyComponent(Prim, true))
			{
				return false;
			}
		}
		return Renderable > 0;
	}

	bool RefusedAsTranslucentOnly(const FConvexVolume& Frustum, const FVector& ViewOrigin,
		const FVector& PollOrigin, float PollRadius, UWorld* World, const AActor* Actor,
		const UPrimitiveComponent* FirstMatch)
	{
		if (AnomalyDefaults::GetAllowTranslucentOnlyTargets())
		{
			return false;
		}
		if (!FirstMatch || !AnomalyViewport::IsTranslucentOnlyComponent(FirstMatch, true))
		{
			return false;
		}
		if (!IsActorRenderableTranslucentOnly(Frustum, ViewOrigin, PollOrigin, PollRadius, World, Actor))
		{
			return false;
		}

		if (AnomalyViewport::IsReadOnlyEnumeration())
		{
			return true;
		}
		const FString ActorName = Actor->GetName();
		bool bAlready = false;
		TranslucentOnlyActorsSeen().Add(ActorName, &bAlready);
		if (!bAlready)
		{
			UE_LOG(LogAnomaly, Warning,
				TEXT("EXCLUDED-TRANSLUCENT actor='%s' class=%s firstComponent='%s' asset='%s' source=%s - refused as ")
				TEXT("an AUTO-POOL injection candidate. Every renderable-visible component draws only through ")
				TEXT("translucent slots that do not opt into custom depth, so the m43 target mask can measure NO ")
				TEXT("pixels for it: every frame of such an event would read target_pixels -1 / observable null and ")
				TEXT("the event would ship with observability_measured=false. This is a MEASURABILITY exclusion - the ")
				TEXT("anomaly would still occur, it just could not be verified. A TARGETED fire is never blocked, and ")
				TEXT("IAI.Select.AllowTranslucentOnlyTargets 1 restores the previous candidate set."),
				*ActorName, *Actor->GetClass()->GetName(), *FirstMatch->GetName(),
				*MeshAssetNameOf(FirstMatch), *AnomalyDefaults::DescribeAllowTranslucentOnlyTargets());
		}
		return true;
	}

	bool ClassifyRenderableVisibleLive(const FConvexVolume& Frustum, const FMatrix& ViewProj, const FVector& ViewOrigin,
		const FVector& PollOrigin, float PollRadius, float MinCoveragePct, UWorld* World, const AActor* Actor,
		UPrimitiveComponent*& OutFirstMatch)
	{
		if (MinCoveragePct <= 0.0f)
		{
			OutFirstMatch = FirstRenderableVisibleComponent(Frustum, ViewOrigin, PollOrigin, PollRadius, World, Actor);
			if (!OutFirstMatch)
			{
				return false;
			}
			return !RefusedAsTranslucentOnly(Frustum, ViewOrigin, PollOrigin, PollRadius, World, Actor, OutFirstMatch);
		}

		FBox Union(ForceInit);
		OutFirstMatch = CollectRenderableVisibleUnion(Frustum, ViewOrigin, PollOrigin, PollRadius, World, Actor, Union);
		if (!OutFirstMatch || !Union.IsValid)
		{
			return false;
		}
		if (!PassesScreenCoverage(ViewProj, Union, MinCoveragePct))
		{
			return false;
		}
		return !RefusedAsTranslucentOnly(Frustum, ViewOrigin, PollOrigin, PollRadius, World, Actor, OutFirstMatch);
	}
}

namespace
{
	AnomalyNearClipSlab::FV3 ToSlabV3(const FVector& V)
	{
		return AnomalyNearClipSlab::MakeV3(V.X, V.Y, V.Z);
	}

	FVector FromSlabV3(const AnomalyNearClipSlab::FV3& V)
	{
		return FVector(V.X, V.Y, V.Z);
	}

	AnomalyNearClipSlab::FAffine34 AffineFromMatrix(const FMatrix& M)
	{
		return AnomalyNearClipSlab::MakeAffine(
			AnomalyNearClipSlab::MakeV3(M.M[0][0], M.M[0][1], M.M[0][2]),
			AnomalyNearClipSlab::MakeV3(M.M[1][0], M.M[1][1], M.M[1][2]),
			AnomalyNearClipSlab::MakeV3(M.M[2][0], M.M[2][1], M.M[2][2]),
			AnomalyNearClipSlab::MakeV3(M.M[3][0], M.M[3][1], M.M[3][2]));
	}

	AnomalyNearClipSlab::FBox3 OrientedBoxFromLocal(const FVector& LocalCenter, const FVector& LocalHalf,
		const FTransform& Transform)
	{
		AnomalyNearClipSlab::FBox3 B;
		B.Center = ToSlabV3(Transform.TransformPosition(LocalCenter));
		const FQuat Rotation = Transform.GetRotation();
		const FVector Scale = Transform.GetScale3D().GetAbs();
		B.Axis[0] = ToSlabV3(Rotation.RotateVector(FVector::XAxisVector));
		B.Axis[1] = ToSlabV3(Rotation.RotateVector(FVector::YAxisVector));
		B.Axis[2] = ToSlabV3(Rotation.RotateVector(FVector::ZAxisVector));
		B.Half[0] = FMath::Abs(LocalHalf.X) * Scale.X;
		B.Half[1] = FMath::Abs(LocalHalf.Y) * Scale.Y;
		B.Half[2] = FMath::Abs(LocalHalf.Z) * Scale.Z;
		return B;
	}

	AnomalyNearClipSlab::EConfirmBody ResolveConfirmTarget(UPrimitiveComponent* P, int32 InstanceIndex, FBodyInstance*& OutBody)
	{
		OutBody = nullptr;
		AnomalyNearClipSlab::FConfirmBodyFacts F;
		F.bSkinned = !P || P->IsA<USkinnedMeshComponent>();
		if (F.bSkinned)
		{
			return AnomalyNearClipSlab::ClassifyConfirmBody(F);
		}
		FBodyInstance* Body = nullptr;
		if (const FLazyObjectProperty* Prop = FindFProperty<FLazyObjectProperty>(P->GetClass(), TEXT("CollisionComponent")))
		{
			UPrimitiveComponent* Collision = Cast<UPrimitiveComponent>(Prop->GetObjectPropertyValue_InContainer(P));
			Body = Collision ? Collision->GetBodyInstance() : nullptr;
			F.bLandscape = true;
			F.bBodyValid = Body && Body->IsValidBodyInstance();
		}
		else
		{
			if (UInstancedStaticMeshComponent* Ism = Cast<UInstancedStaticMeshComponent>(P))
			{
				Body = Ism->InstanceBodies.IsValidIndex(InstanceIndex) ? Ism->InstanceBodies[InstanceIndex] : nullptr;
				F.bInstance = true;
				F.bInstanceBodyMatchesRendered = false;
				if (Ism->PerInstanceSMData.IsValidIndex(InstanceIndex))
				{
					const FMatrix InstanceMatrix = Ism->PerInstanceSMData[InstanceIndex].Transform;
					const FMatrix Rendered = InstanceMatrix * Ism->GetComponentTransform().ToMatrixWithScale();
					const FMatrix BodyMatrix = (FTransform(InstanceMatrix) * Ism->GetComponentTransform()).ToMatrixWithScale();
					F.bInstanceBodyMatchesRendered = AnomalyNearClipSlab::AffineMatchesRendered(AffineFromMatrix(Rendered),
						AffineFromMatrix(BodyMatrix), AnomalyNearClipSlab::InstanceBodyMatchRelEps);
				}
			}
			else
			{
				Body = P->GetBodyInstance();
			}
			F.bCollisionEnabled = P->GetCollisionEnabled() != ECollisionEnabled::NoCollision;
			F.bBodyValid = Body && Body->IsValidBodyInstance();
			const auto* WeldInfo = Body ? Body->GetCurrentWeldInfo() : nullptr;
			F.bWelded = P->IsWelded() || (Body && Body->WeldParent != nullptr) || (WeldInfo && WeldInfo->Num() > 0);
			const UBodySetup* Setup = P->GetBodySetup();
			F.bHasComplex = Setup && Setup->CollisionTraceFlag != CTF_UseSimpleAsComplex && Setup->ChaosTriMeshes.Num() > 0;
		}
		const AnomalyNearClipSlab::EConfirmBody Kind = AnomalyNearClipSlab::ClassifyConfirmBody(F);
		if (AnomalyNearClipSlab::IsConfirmableBody(Kind))
		{
			OutBody = Body;
		}
		return Kind;
	}

	bool IsOrientedBoxPlausible(const AnomalyNearClipSlab::FBox3& B, const FBoxSphereBounds& WorldBounds,
		const FVector& LocalExtent)
	{
		if (LocalExtent.IsNearlyZero() && !WorldBounds.BoxExtent.IsNearlyZero())
		{
			return false;
		}
		const FBox Aabb = WorldBounds.GetBox();
		const double Slack = FMath::Max(1.0, 0.01 * WorldBounds.BoxExtent.GetMax());
		const FVector C = FromSlabV3(B.Center);
		return C.X >= Aabb.Min.X - Slack && C.Y >= Aabb.Min.Y - Slack && C.Z >= Aabb.Min.Z - Slack
			&& C.X <= Aabb.Max.X + Slack && C.Y <= Aabb.Max.Y + Slack && C.Z <= Aabb.Max.Z + Slack;
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
			if (!IsReadOnlyEnumeration())
			{
				UE_LOG(LogAnomaly, Warning,
					TEXT("AnomalyViewport: no local player controller for world '%s' (e.g. a Simulate session); viewport scoping treated as unscoped."),
					*GetNameSafe(World));
			}
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

		if (const AActor* Owner = Component->GetOwner())
		{
			if (Owner->IsA<AInstancedFoliageActor>())
			{
				return false;
			}
		}

		if (MatchesExcludedTargetPattern(Component))
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

		return IsRenderableGeometryComponent(Component);
	}

	bool IsRenderableGeometryComponent(const UPrimitiveComponent* Component)
	{
		return Component
			&& (Component->IsA<UStaticMeshComponent>() || Component->IsA<USkinnedMeshComponent>());
	}

	bool IsTranslucentOnlyComponent(const UPrimitiveComponent* Component, bool bAllowCustomDepthOptIn)
	{
		if (!Component)
		{
			return false;
		}
		const int32 NumSlots = Component->GetNumMaterials();
		if (NumSlots <= 0)
		{
			return false;
		}
		for (int32 Slot = 0; Slot < NumSlots; ++Slot)
		{
			UMaterialInterface* M = const_cast<UPrimitiveComponent*>(Component)->GetMaterial(Slot);
			if (!M)
			{
				return false;
			}
			if (!IsTranslucentBlendMode(M->GetBlendMode()))
			{
				return false;
			}
			if (bAllowCustomDepthOptIn && M->IsTranslucencyWritingCustomDepth())
			{
				return false;
			}
		}
		return true;
	}

	bool GetActorRenderableBounds(const AActor* Actor, FBox& OutBox)
	{
		OutBox = FBox(ForceInit);
		if (!Actor)
		{
			return false;
		}

		TArray<UPrimitiveComponent*> Prims;
		Actor->GetComponents<UPrimitiveComponent>(Prims);
		for (const UPrimitiveComponent* Prim : Prims)
		{
			if (IsRenderableGeometryComponent(Prim))
			{
				OutBox += Prim->Bounds.GetBox();
			}
		}
		return OutBox.IsValid != 0;
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

	float GetActorPollDistanceCm(UWorld* World, const AActor* Actor)
	{
		const float Unmeasurable = TNumericLimits<float>::Max();
		if (!World || !Actor)
		{
			return Unmeasurable;
		}

		FAnomalyViewInfo View;
		if (!GetActiveViewInfo(World, View))
		{
			return Unmeasurable;
		}

		const FVector PollOrigin = ResolvePollOrigin(World, View.Origin);

		bool bAny = false;
		float Best = 0.0f;
		TArray<UPrimitiveComponent*> Prims;
		Actor->GetComponents<UPrimitiveComponent>(Prims);
		for (const UPrimitiveComponent* Prim : Prims)
		{
			if (!Prim || !IsRenderableComponent(Prim))
			{
				continue;
			}
			const FBoxSphereBounds& B = Prim->Bounds;
			const float D = (float)FVector::Dist(PollOrigin, B.Origin) - (float)B.SphereRadius;
			if (!bAny || D < Best)
			{
				Best = D;
				bAny = true;
			}
		}
		return bAny ? Best : Unmeasurable;
	}

	float ComputeBoundsScreenSizeForActiveView(UWorld* World, const FVector& BoundsOrigin, float SphereRadius)
	{
		FAnomalyViewInfo View;
		if (!World || !GetActiveViewInfo(World, View))
		{
			return -1.0f;
		}
		return ComputeBoundsScreenSize(FVector4(BoundsOrigin, 1.0),
			SphereRadius, FVector4(View.Origin, 1.0), BuildProjectionMatrix(View));
	}

	bool IsGeometryWithinNearClipRadius(UWorld* World)
	{
		if (!World)
		{
			return false;
		}

		FAnomalyViewInfo View;
		if (!GetActiveViewInfo(World, View))
		{
			return false;
		}

		const float Radius = GNearClippingPlane;
		if (Radius <= 0.0f)
		{
			return false;
		}

		FCollisionQueryParams Params(FName(TEXT("AnomalyNearClipProbe")), false);
		if (const APlayerController* PC = World->GetFirstPlayerController())
		{
			if (const APawn* Pawn = PC->GetPawn())
			{
				Params.AddIgnoredActor(Pawn);
			}
		}

		return World->OverlapAnyTestByChannel(View.Origin, FQuat::Identity, ECC_Visibility,
			FCollisionShape::MakeSphere(Radius), Params);
	}

	bool EvaluateNearClipSlab(UWorld* World, float BaselineNear, float AnomalousNear, FAnomalyNearClipSlabResult& Out)
	{
		using namespace AnomalyNearClipSlab;

		Out = FAnomalyNearClipSlabResult();
		Out.BaselineNear = BaselineNear;
		Out.AnomalousNear = AnomalousNear;
		const uint64 StartCycles = FPlatformTime::Cycles64();

		APlayerController* PC = World ? World->GetFirstPlayerController() : nullptr;
		if (!PC)
		{
			return false;
		}

		FMinimalViewInfo Pov;
		if (PC->PlayerCameraManager && PC->PlayerCameraManager->GetCameraCacheTime() > 0.0f)
		{
			Pov = PC->PlayerCameraManager->GetCameraCacheView();
		}
		else
		{
			FAnomalyViewInfo Fallback;
			if (!GetActiveViewInfo(World, Fallback))
			{
				return false;
			}
			Pov.Location = Fallback.Origin;
			Pov.Rotation = Fallback.Rotation;
			Pov.FOV = Fallback.HorizontalFOVDeg;
			Pov.AspectRatio = Fallback.AspectRatio;
		}
		Out.bEvaluated = true;
		Out.EffectiveNear = Pov.GetFinalPerspectiveNearClipPlane();

		auto Finish = [&Out, StartCycles]()
		{
			Out.Micros = FPlatformTime::ToMilliseconds64(FPlatformTime::Cycles64() - StartCycles) * 1000.0;
		};

		if (Pov.ProjectionMode != ECameraProjectionMode::Perspective)
		{
			Out.bNotPerspective = true;
			Finish();
			return true;
		}
		if (Pov.PerspectiveNearClipPlane > 0.0f)
		{
			Out.bNearOverridden = true;
			Finish();
			return true;
		}

		int32 SizeX = 0;
		int32 SizeY = 0;
		if (UGameViewportClient* ViewportClient = World->GetGameViewport())
		{
			FVector2D Size = FVector2D::ZeroVector;
			ViewportClient->GetViewportSize(Size);
			SizeX = (int32)Size.X;
			SizeY = (int32)Size.Y;
		}
		int32 Axis = AxisMaintainX;
		if (const ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
		{
			if (LocalPlayer->AspectRatioAxisConstraint == AspectRatio_MaintainYFOV)
			{
				Axis = AxisMaintainY;
			}
			else if (LocalPlayer->AspectRatioAxisConstraint == AspectRatio_MajorAxisFOV)
			{
				Axis = AxisMajor;
			}
		}
		static IConsoleVariable* LegacyMaintainY =
			IConsoleManager::Get().FindConsoleVariable(TEXT("r.UseLegacyMaintainYFOVViewMatrix"));
		const bool bLegacyMaintainY = LegacyMaintainY && LegacyMaintainY->GetInt() != 0;

		const FTanExtents Tan = ComputeTanExtents(Pov.FOV, Pov.AspectRatio, Pov.bConstrainAspectRatio != 0,
			SizeX, SizeY, Axis, bLegacyMaintainY);
		const FRotationMatrix Basis(Pov.Rotation);
		const FV3 Eye = ToSlabV3(Pov.Location);
		const FSlab Slab = BuildSlab(Eye, ToSlabV3(Basis.GetScaledAxis(EAxis::X)), ToSlabV3(Basis.GetScaledAxis(EAxis::Y)),
			ToSlabV3(Basis.GetScaledAxis(EAxis::Z)), Tan.TanH, Tan.TanV, BaselineNear, AnomalousNear);
		if (Slab.bEmpty)
		{
			Finish();
			return true;
		}
		const FBox SlabBox(FromSlabV3(Slab.AabbMin), FromSlabV3(Slab.AabbMax));
		const AActor* ViewActor = PC->GetViewTarget();

		TArray<FConfirmCandidate> ConfirmCands;
		TArray<FBodyInstance*> ConfirmBodies;
		TArray<FString> ConfirmNames;
		auto AddConfirm = [&](UPrimitiveComponent* P, int32 InstanceIndex, const FBox3& Box)
		{
			FConfirmCandidate C;
			C.Box = Box;
			FBodyInstance* Body = nullptr;
			const AnomalyNearClipSlab::EConfirmBody Kind = ResolveConfirmTarget(P, InstanceIndex, Body);
			switch (Kind)
			{
			case AnomalyNearClipSlab::EConfirmBody::Landscape: ++Out.LandscapeCandidates; break;
			case AnomalyNearClipSlab::EConfirmBody::Skinned:   ++Out.SkinnedUnconfirmable; break;
			case AnomalyNearClipSlab::EConfirmBody::NoCollision: ++Out.NoCollisionUnconfirmable; break;
			case AnomalyNearClipSlab::EConfirmBody::NoComplex: ++Out.NoComplexUnconfirmable; break;
			case AnomalyNearClipSlab::EConfirmBody::Welded: ++Out.WeldedUnconfirmable; break;
			case AnomalyNearClipSlab::EConfirmBody::InstanceTransform: ++Out.InstanceTransformUnconfirmable; break;
			default: break;
			}
			C.bConfirmable = Body != nullptr && AnomalyNearClipSlab::IsConfirmableBody(Kind);
			ConfirmCands.Add(C);
			ConfirmBodies.Add(C.bConfirmable ? Body : nullptr);
			ConfirmNames.Add(InstanceIndex == INDEX_NONE
				? FString::Printf(TEXT("%s.%s"), *GetNameSafe(P->GetOwner()), *P->GetName())
				: FString::Printf(TEXT("%s.%s[%d]"), *GetNameSafe(P->GetOwner()), *P->GetName(), InstanceIndex));
		};

		auto Visit = [&](UPrimitiveComponent* P)
		{
			if (!P)
			{
				return;
			}
			++Out.Enumerated;
			const AActor* Owner = P->GetOwner();
			FPrimitiveFlags Flags;
			Flags.bRegistered = P->IsRegistered();
			Flags.bHasSceneProxy = P->SceneProxy != nullptr;
			Flags.bVisible = P->IsVisible();
			Flags.bOwnerHidden = Owner && Owner->IsHidden();
			Flags.bRenderInMainPass = P->bRenderInMainPass != 0;
			Flags.bSceneCaptureOnly = P->bVisibleInSceneCaptureOnly != 0;
			Flags.bIsFxSystem = P->IsA<UFXSystemComponent>();
			Flags.bOnlyOwnerSee = P->bOnlyOwnerSee != 0;
			Flags.bOwnerNoSee = P->bOwnerNoSee != 0;
			if (Flags.bOnlyOwnerSee || Flags.bOwnerNoSee)
			{
				for (const AActor* Chain = Owner; Chain; Chain = Chain->GetOwner())
				{
					if (Chain == ViewActor)
					{
						Flags.bOwnedByViewActor = true;
						break;
					}
				}
			}
			Flags.MinDrawDistance = P->MinDrawDistance;
			Flags.DistanceSqToView = FVector::DistSquared(P->Bounds.Origin, Pov.Location);
			const EPrimitiveVerdict Verdict = ClassifyPrimitive(Flags);
			if (Verdict == EPrimitiveVerdict::FxExcluded)
			{
				++Out.FxExcluded;
				return;
			}
			if (Verdict != EPrimitiveVerdict::Counted)
			{
				return;
			}
			const FBox WorldBox = P->Bounds.GetBox();
			if (!AabbOverlap(Slab.AabbMin, Slab.AabbMax, ToSlabV3(WorldBox.Min), ToSlabV3(WorldBox.Max)))
			{
				return;
			}
			++Out.Candidates;

			bool bHit = false;
			bool bEyeInside = false;
			if (const UInstancedStaticMeshComponent* Ism = Cast<UInstancedStaticMeshComponent>(P))
			{
				const UStaticMesh* Mesh = Ism->GetStaticMesh();
				if (!Mesh)
				{
					return;
				}
				const FBoxSphereBounds MeshBounds = Mesh->GetBounds();
				TArray<int32> Indices;
				const UHierarchicalInstancedStaticMeshComponent* Hism = Cast<UHierarchicalInstancedStaticMeshComponent>(Ism);
				if (Hism && Hism->ClusterTreePtr.IsValid() && Hism->ClusterTreePtr->Num() > 0 && Hism->IsTreeFullyBuilt())
				{
					++Out.HismTreeQueries;
					Indices = Hism->GetInstancesOverlappingBox(SlabBox, true);
				}
				else
				{
					++Out.IsmFullScans;
					const int32 Count = Ism->GetInstanceCount();
					Indices.Reserve(Count);
					for (int32 Index = 0; Index < Count; ++Index)
					{
						Indices.Add(Index);
					}
				}
				const FMatrix ComponentMatrix = Ism->GetComponentTransform().ToMatrixWithScale();
				for (const int32 Index : Indices)
				{
					if (!Ism->PerInstanceSMData.IsValidIndex(Index))
					{
						continue;
					}
					++Out.InstancesTested;
					const FMatrix InstanceWorld = Ism->PerInstanceSMData[Index].Transform * ComponentMatrix;
					const FBox3 Box = MakeMatrixBox(ToSlabV3(MeshBounds.Origin), ToSlabV3(MeshBounds.BoxExtent * Ism->BoundsScale),
						AffineFromMatrix(InstanceWorld));
					if (Box.Half[0] + Box.Half[1] + Box.Half[2] <= UE_KINDA_SMALL_NUMBER)
					{
						continue;
					}
					if (InstanceBoxMayTouchSlab(Slab, Box))
					{
						bHit = true;
						bEyeInside = bEyeInside || PointInsideBox(Box, Eye);
						AddConfirm(const_cast<UInstancedStaticMeshComponent*>(Ism), Index, Box);
					}
				}
			}
			else
			{
				const FBox3 WorldAabb = MakeAxisAlignedBox(ToSlabV3(P->Bounds.Origin), ToSlabV3(P->Bounds.BoxExtent));
				const FBoxSphereBounds Local = P->CalcBounds(FTransform::Identity);
				const FBox3 Oriented = OrientedBoxFromLocal(Local.Origin, Local.BoxExtent * P->BoundsScale,
					P->GetComponentTransform());
				if (IsOrientedBoxPlausible(Oriented, P->Bounds, Local.BoxExtent))
				{
					bHit = BoxIntersectsSlab(Slab, Oriented) && BoxIntersectsSlab(Slab, WorldAabb);
					bEyeInside = bHit && PointInsideBox(Oriented, Eye);
					if (bHit)
					{
						AddConfirm(P, INDEX_NONE, Oriented);
					}
				}
				else
				{
					++Out.BoxFallbacks;
					bHit = BoxIntersectsSlab(Slab, WorldAabb);
					bEyeInside = bHit && PointInsideBox(WorldAabb, Eye);
					if (bHit)
					{
						AddConfirm(P, INDEX_NONE, WorldAabb);
					}
				}
			}
			if (!bHit)
			{
				return;
			}
			++Out.SlabPrimitives;
			if (bEyeInside)
			{
				++Out.EyeInsideBox;
			}
			if (Out.FirstHit.IsEmpty())
			{
				Out.FirstHit = FString::Printf(TEXT("%s.%s"), *GetNameSafe(Owner), *P->GetName());
			}
		};

		for (TActorIterator<AActor> It(World); It; ++It)
		{
			It->ForEachComponent<UPrimitiveComponent>(false, Visit);
		}
		for (ULevel* Level : World->GetLevels())
		{
			if (!Level)
			{
				continue;
			}
			for (UModelComponent* Model : Level->ModelComponents)
			{
				Visit(Model);
			}
		}

		Out.bSatPositive = Out.SlabPrimitives > 0;
		Out.bSlab = false;
		Out.ConfirmCandidates = ConfirmCands.Num();
		if (Out.bSatPositive && ConfirmCands.Num() > 0)
		{
			const uint64 ConfirmStart = FPlatformTime::Cycles64();
			TArray<ECandidateOutcome> Outcomes;
			Outcomes.SetNum(ConfirmCands.Num());
			const FConfirmConfig Cfg;
			const FConfirmResult R = ConfirmSlab(Eye, ToSlabV3(Basis.GetScaledAxis(EAxis::X)),
				ToSlabV3(Basis.GetScaledAxis(EAxis::Y)), ToSlabV3(Basis.GetScaledAxis(EAxis::Z)), Tan.TanH, Tan.TanV,
				BaselineNear, AnomalousNear, ConfirmCands.GetData(), ConfirmCands.Num(), Outcomes.GetData(), Cfg,
				[&ConfirmBodies](int32 Index, const FV3& P0, const FV3& P1)
				{
					FBodyInstance* Body = ConfirmBodies.IsValidIndex(Index) ? ConfirmBodies[Index] : nullptr;
					if (!Body || !Body->IsValidBodyInstance())
					{
						return false;
					}
					FHitResult Hit;
					return Body->LineTrace(Hit, FromSlabV3(P0), FromSlabV3(P1), true, false);
				});
			Out.ConfirmMicros = FPlatformTime::ToMilliseconds64(FPlatformTime::Cycles64() - ConfirmStart) * 1000.0;
			Out.bSlab = R.bPositive;
			Out.bUnconfirmed = R.bUnconfirmed;
			Out.ClippedRayFraction = (float)R.ClippedRayFraction;
			Out.ConfirmTraces = R.Traces;
			Out.ConfirmHits = R.Hits;
			Out.ConfirmMisses = R.Misses;
			Out.ConfirmUnconfirmable = R.Unconfirmable;
			Out.ConfirmOverCap = R.OverCap;
			Out.ConfirmTraceCapped = R.TraceCapped;
			Out.ConfirmNoRay = R.NoRay;
			Out.ConfirmTooFewValid = R.TooFewValid;
			Out.ConfirmFullSlabFallbacks = R.FullSlabFallbacks;
			Out.ConfirmInvalidSegments = R.InvalidSegments;
			for (int32 i = 0; i < Outcomes.Num(); ++i)
			{
				if (Outcomes[i] == ECandidateOutcome::Hit)
				{
					Out.FirstConfirmedHit = ConfirmNames[i];
					break;
				}
			}
		}
		Finish();
		return true;
	}

	void ResetTargetExclusionStats()
	{
		ExcludedActorsSeen().Reset();
		TranslucentOnlyActorsSeen().Reset();
		NaniteRefusedActorsSeen().Reset();
		GNaniteRefusalAttempts = 0;
		GNaniteProbeMissingRefusals = 0;
	}

	int32 GetTargetExclusionCount()
	{
		return ExcludedActorsSeen().Num();
	}

	int32 GetTranslucentOnlyExclusionCount()
	{
		return TranslucentOnlyActorsSeen().Num();
	}

	FReadOnlyEnumerationScope::FReadOnlyEnumerationScope()
	{
		++GReadOnlyEnumerationDepth;
	}

	FReadOnlyEnumerationScope::~FReadOnlyEnumerationScope()
	{
		--GReadOnlyEnumerationDepth;
	}

	bool IsReadOnlyEnumeration()
	{
		return GReadOnlyEnumerationDepth > 0;
	}

	bool IsRenderableComponentReadOnly(const UPrimitiveComponent* Component)
	{
		FReadOnlyEnumerationScope ReadOnly;
		return IsRenderableComponent(Component);
	}

	TArray<TWeakObjectPtr<AActor>> GetVisibleRenderableActorsReadOnly(UWorld* World)
	{
		FReadOnlyEnumerationScope ReadOnly;
		return GetVisibleRenderableActors(World);
	}

	bool ActorDrawsAnyNaniteReadOnly(const AActor* Actor)
	{
		FReadOnlyEnumerationScope ReadOnly;
		return ActorDrawsAnyNanite(Actor);
	}

	void SetNaniteComponentProbe(FNaniteComponentProbe Probe)
	{
		GNaniteComponentProbe = Probe;
	}

	bool HasNaniteComponentProbe()
	{
		return GNaniteComponentProbe != nullptr;
	}

	bool ComponentDrawsNanite(const UPrimitiveComponent* Component)
	{
		return Component && GNaniteComponentProbe && GNaniteComponentProbe(Component);
	}

	bool ActorDrawsAnyNanite(const AActor* Actor, int32* OutNaniteComponents, int32* OutRenderableComponents)
	{
		int32 Nanite = 0;
		int32 Drawn = 0;
		if (Actor)
		{
			TInlineComponentArray<UPrimitiveComponent*> Prims;
			const_cast<AActor*>(Actor)->GetComponents(Prims);
			TArray<AnomalyTargetPolicy::FPrimitiveFacts> Facts;
			Facts.Reserve(Prims.Num());
			for (const UPrimitiveComponent* Prim : Prims)
			{
				if (!Prim)
				{
					continue;
				}
				AnomalyTargetPolicy::FPrimitiveFacts F;
				F.bVisible = Prim->IsVisible();
				F.bGeometryClass = IsRenderableGeometryComponent(Prim);
				const UInstancedStaticMeshComponent* ISM = Cast<UInstancedStaticMeshComponent>(Prim);
				F.bZeroInstances = ISM && ISM->GetInstanceCount() <= 0;
				F.bNanite = F.bVisible && F.bGeometryClass && ComponentDrawsNanite(Prim);
				Facts.Add(F);
			}
			Nanite = AnomalyTargetPolicy::CountDrawnNanite(Facts.GetData(), Facts.Num(), Drawn);
		}
		if (OutNaniteComponents) { *OutNaniteComponents = Nanite; }
		if (OutRenderableComponents) { *OutRenderableComponents = Drawn; }
		return AnomalyTargetPolicy::DecideNanite(false, Drawn, Nanite) == AnomalyTargetPolicy::ENaniteDecision::Refuse;
	}

	bool ActorBlocksLabelForNanite(const AActor* Actor)
	{
		if (!Actor)
		{
			return false;
		}
		int32 Nanite = 0;
		int32 Drawn = 0;
		ActorDrawsAnyNanite(Actor, &Nanite, &Drawn);
		return AnomalyTargetPolicy::NaniteBlocksLabel(AnomalyDefaults::GetAllowNaniteTargets(), HasNaniteComponentProbe(),
			true, Drawn, Nanite);
	}

	bool RefuseNaniteTarget(const AActor* Actor, const TCHAR* Site)
	{
		if (!Actor)
		{
			return false;
		}
		const bool bAllow = AnomalyDefaults::GetAllowNaniteTargets();
		const bool bProbe = HasNaniteComponentProbe();
		int32 Nanite = 0;
		int32 Renderable = 0;
		ActorDrawsAnyNanite(Actor, &Nanite, &Renderable);
		const AnomalyTargetPolicy::ENaniteDecision Decision =
			AnomalyTargetPolicy::DecideNaniteTarget(bAllow, bProbe, Renderable, Nanite);
		if (Decision == AnomalyTargetPolicy::ENaniteDecision::Admit)
		{
			return false;
		}
		if (Decision == AnomalyTargetPolicy::ENaniteDecision::RefuseProbeMissing)
		{
			++GNaniteProbeMissingRefusals;
			UE_LOG(LogAnomaly, Error,
				TEXT("REFUSED-NANITE-PROBE-MISSING actor='%s' class=%s site=%s reason=%s setting=%s - IAI.Targets.AllowNanite is 0 ")
				TEXT("but no Nanite classifier is registered (the AnomalyCapture module is not loaded or has shut down), so this ")
				TEXT("target cannot be shown to be free of Nanite parts. It is refused (fail closed). Load the AnomalyCapture ")
				TEXT("module, or set IAI.Targets.AllowNanite 1 to accept unmaskable targets."),
				*Actor->GetName(), *Actor->GetClass()->GetName(), Site,
				UTF8_TO_TCHAR(AnomalyTargetPolicy::DescribeNaniteProbeMissingReason()), *AnomalyDefaults::DescribeAllowNaniteTargets());
			return true;
		}
		++GNaniteRefusalAttempts;
		const FString ActorName = Actor->GetName();
		bool bAlready = false;
		NaniteRefusedActorsSeen().Add(ActorName, &bAlready);
		if (!bAlready || FCString::Strcmp(Site, TEXT("auto_pool")) != 0)
		{
			UE_LOG(LogAnomaly, Warning,
				TEXT("REFUSED-NANITE actor='%s' class=%s site=%s nanite=%d/%d reason=%s setting=%s - at least one drawn ")
				TEXT("primitive of this target renders as Nanite, and on UE 5.1 a Nanite primitive never writes custom ")
				TEXT("depth, so the target mask could not carry it. The target is refused BEFORE apply, so no event, box ")
				TEXT("or label is made for it. IAI.Targets.AllowNanite 1 restores the previous behaviour (label and box, ")
				TEXT("no mask, observability_measured false)."),
				*ActorName, *Actor->GetClass()->GetName(), Site, Nanite, Renderable,
				UTF8_TO_TCHAR(AnomalyTargetPolicy::DescribeNaniteReason()), *AnomalyDefaults::DescribeAllowNaniteTargets());
		}
		return true;
	}

	int32 GetNaniteRefusalCount()
	{
		return NaniteRefusedActorsSeen().Num();
	}

	int32 GetNaniteRefusalAttempts()
	{
		return GNaniteRefusalAttempts;
	}

	int32 GetNaniteProbeMissingRefusals()
	{
		return GNaniteProbeMissingRefusals;
	}

	float GetActorScreenCoveragePct(UWorld* World, const AActor* Actor)
	{
		if (!World || !Actor)
		{
			return -1.0f;
		}

		FAnomalyViewInfo View;
		if (!GetActiveViewInfo(World, View))
		{
			return -1.0f;
		}

		FBox Union(ForceInit);
		bool bAny = false;
		TArray<UPrimitiveComponent*> Prims;
		Actor->GetComponents<UPrimitiveComponent>(Prims);
		for (const UPrimitiveComponent* Prim : Prims)
		{
			if (Prim && IsRenderableComponent(Prim))
			{
				Union += Prim->Bounds.GetBox();
				bAny = true;
			}
		}
		if (!bAny || !Union.IsValid)
		{
			return -1.0f;
		}

		const FMatrix ViewProj = BuildViewProjectionMatrix(View);
		FVector2D Min(FVector2D::ZeroVector), Max(FVector2D::ZeroVector);
		if (!ProjectBoundsToScreenRect(ViewProj, FBoxSphereBounds(Union), Min, Max))
		{
			return -1.0f;
		}
		return (float)((Max.X - Min.X) * (Max.Y - Min.Y)) * 100.0f;
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

	TArray<TWeakObjectPtr<AActor>> GetCensusPrefilterActors(UWorld* World)
	{
		TArray<TWeakObjectPtr<AActor>> Result;

		FAnomalyViewInfo View;
		if (!GetActiveViewInfo(World, View))
		{
			return Result;
		}

		const FConvexVolume Frustum = BuildFrustum(View);
		const FVector PollOrigin = ResolvePollOrigin(World, View.Origin);
		const float PollRadius = GPollRadius;

		for (TActorIterator<AActor> It(World); It; ++It)
		{
			AActor* Actor = *It;
			if (!Actor)
			{
				continue;
			}
			TArray<UPrimitiveComponent*> Prims;
			Actor->GetComponents<UPrimitiveComponent>(Prims);
			for (const UPrimitiveComponent* Prim : Prims)
			{
				if (!IsRenderableComponent(Prim))
				{
					continue;
				}
				if (PollRadius > 0.0f)
				{
					const FBoxSphereBounds& B = Prim->Bounds;
					if ((float)FVector::Dist(PollOrigin, B.Origin) - (float)B.SphereRadius > PollRadius)
					{
						continue;
					}
				}
				if (!IsInFrustum(Frustum, Prim))
				{
					continue;
				}
				Result.Add(Actor);
				break;
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
			Info.AssetName = MeshAssetNameOf(Match);
			Info.ComponentClass = Match->GetClass()->GetName();
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
		if (!GetActorRenderableBounds(Actor, Box))
		{
			return false;
		}

		return ProjectBoxToScreenRect(View, Box, OutMin, OutMax);
	}

	bool ProjectBoxToScreenRect(const FAnomalyViewInfo& View, const FBox& Box, FVector2D& OutMin, FVector2D& OutMax)
	{
		OutMin = OutMax = FVector2D::ZeroVector;
		if (!View.bValid)
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
