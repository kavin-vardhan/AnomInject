#pragma once

#include "CoreMinimal.h"
#include "Compat/AnomalyEngineVersion.h"
#include "Templates/Function.h"

class USkinnedMeshComponent;
class UStaticMesh;
class UPrimitiveComponent;
class UBodySetup;
class APlayerCameraManager;
class FStaticMeshRenderData;
struct FMinimalViewInfo;
struct FConvexVolume;

namespace AnomalyCompat
{
	ANOMALYINJECTOR_API const UObject* SkinnedAssetIdentity(const USkinnedMeshComponent* Component);
	ANOMALYINJECTOR_API const FStaticMeshRenderData* StaticRenderData(const UStaticMesh* Mesh);
	ANOMALYINJECTOR_API FMinimalViewInfo CameraCachePOV(const APlayerCameraManager* Camera);
	ANOMALYINJECTOR_API float EffectiveNearClip(const FMinimalViewInfo& View);
	ANOMALYINJECTOR_API bool HasNearClipOverride(const FMinimalViewInfo& View);
	ANOMALYINJECTOR_API bool CastsContactShadow(const UPrimitiveComponent* Component);
	ANOMALYINJECTOR_API void SetCastsContactShadow(UPrimitiveComponent* Component, bool bEnabled);
	ANOMALYINJECTOR_API bool HasComplexCollision(const UBodySetup* Setup);
	ANOMALYINJECTOR_API float StaticLodScreenSize(const FStaticMeshRenderData* Data, int32 LodIndex);
	ANOMALYINJECTOR_API int32 PredictedLod(const USkinnedMeshComponent* Component);
	ANOMALYINJECTOR_API void ViewFrustumWithoutFarPlane(FConvexVolume& Out, const FMatrix& ViewProjection);
	ANOMALYINJECTOR_API FVector AxisVector(int32 Axis);
	ANOMALYINJECTOR_API bool IsSceneCaptureOnly(const UPrimitiveComponent* Component);
	FORCEINLINE double NearClipSmallNumber()
	{
#if ANOMALY_ENGINE_425
		return KINDA_SMALL_NUMBER;
#else
		return UE_KINDA_SMALL_NUMBER;
#endif
	}
	template <typename Signature>
	void ResetFunction(TFunction<Signature>& Function)
	{
#if ANOMALY_ENGINE_425
		Function = TFunction<Signature>();
#else
		Function.Reset();
#endif
	}
}
