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
