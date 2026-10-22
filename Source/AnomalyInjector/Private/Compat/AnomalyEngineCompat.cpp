#include "Compat/AnomalyEngineCompat.h"
#include "Compat/AnomalyEngineVersion.h"
#include "Camera/CameraTypes.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SkinnedMeshComponent.h"
#include "Engine/Engine.h"
#include "EngineGlobals.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/StaticMesh.h"
#include "StaticMeshResources.h"
#include "ConvexVolume.h"
#include "PhysicsEngine/BodySetup.h"
#if ANOMALY_ENGINE_51
#include "Engine/SkinnedAsset.h"
#endif

const UObject* AnomalyCompat::SkinnedAssetIdentity(const USkinnedMeshComponent* Component)
{
#if ANOMALY_ENGINE_425
	return Component ? Component->SkeletalMesh : nullptr;
#else
	return Component ? Component->GetSkinnedAsset() : nullptr;
#endif
}

const FStaticMeshRenderData* AnomalyCompat::StaticRenderData(const UStaticMesh* Mesh)
{
#if ANOMALY_ENGINE_425
	return Mesh ? Mesh->RenderData.Get() : nullptr;
#else
	return Mesh ? Mesh->GetRenderData() : nullptr;
#endif
}

FMinimalViewInfo AnomalyCompat::CameraCachePOV(const APlayerCameraManager* Camera)
{
#if ANOMALY_ENGINE_425
	return Camera->GetCameraCachePOV();
#else
	return Camera->GetCameraCacheView();
#endif
}

float AnomalyCompat::EffectiveNearClip(const FMinimalViewInfo& View)
{
#if ANOMALY_ENGINE_425
	return GNearClippingPlane;
#else
	return View.GetFinalPerspectiveNearClipPlane();
#endif
}

bool AnomalyCompat::HasNearClipOverride(const FMinimalViewInfo& View)
{
#if ANOMALY_ENGINE_425
	return false;
#else
	return View.PerspectiveNearClipPlane > 0.0f;
#endif
}

bool AnomalyCompat::CastsContactShadow(const UPrimitiveComponent* Component)
{
#if ANOMALY_ENGINE_425
	return false;
#else
	return Component->bCastContactShadow;
#endif
}

void AnomalyCompat::SetCastsContactShadow(UPrimitiveComponent* Component, bool bEnabled)
{
#if ANOMALY_ENGINE_51
	Component->bCastContactShadow = bEnabled;
#endif
}

bool AnomalyCompat::HasComplexCollision(const UBodySetup* Setup)
{
#if ANOMALY_ENGINE_425
	return Setup && Setup->CollisionTraceFlag != CTF_UseSimpleAsComplex && Setup->TriMeshes.Num() > 0;
#else
	return Setup && Setup->CollisionTraceFlag != CTF_UseSimpleAsComplex && Setup->ChaosTriMeshes.Num() > 0;
#endif
}

float AnomalyCompat::StaticLodScreenSize(const FStaticMeshRenderData* Data, int32 LodIndex)
{
#if ANOMALY_ENGINE_425
	return Data->ScreenSize[LodIndex].Default;
#else
	return Data->ScreenSize[LodIndex].GetValue();
#endif
}

int32 AnomalyCompat::PredictedLod(const USkinnedMeshComponent* Component)
{
#if ANOMALY_ENGINE_425
	return Component->PredictedLODLevel;
#else
	return Component->GetPredictedLODLevel();
#endif
}

void AnomalyCompat::ViewFrustumWithoutFarPlane(FConvexVolume& Out, const FMatrix& ViewProjection)
{
#if ANOMALY_ENGINE_425
	Out.Planes.Empty(6);
	FPlane Plane;
	if (ViewProjection.GetFrustumNearPlane(Plane)) { Out.Planes.Add(Plane); }
	if (ViewProjection.GetFrustumLeftPlane(Plane)) { Out.Planes.Add(Plane); }
	if (ViewProjection.GetFrustumRightPlane(Plane)) { Out.Planes.Add(Plane); }
	if (ViewProjection.GetFrustumTopPlane(Plane)) { Out.Planes.Add(Plane); }
	if (ViewProjection.GetFrustumBottomPlane(Plane)) { Out.Planes.Add(Plane); }
	Out.Init();
#else
	GetViewFrustumBounds(Out, ViewProjection, true, false);
#endif
}

FVector AnomalyCompat::AxisVector(int32 Axis)
{
#if ANOMALY_ENGINE_425
	return Axis == 0 ? FVector(1.0f, 0.0f, 0.0f) : (Axis == 1 ? FVector(0.0f, 1.0f, 0.0f) : FVector(0.0f, 0.0f, 1.0f));
#else
	return Axis == 0 ? FVector::XAxisVector : (Axis == 1 ? FVector::YAxisVector : FVector::ZAxisVector);
#endif
}

bool AnomalyCompat::IsSceneCaptureOnly(const UPrimitiveComponent* Component)
{
#if ANOMALY_ENGINE_425
	return false;
#else
	return Component->bVisibleInSceneCaptureOnly != 0;
#endif
}
