#pragma once

#include "CoreMinimal.h"

class USkinnedMeshComponent;
class UStaticMesh;
class UPrimitiveComponent;
class UBodySetup;
class APlayerCameraManager;
struct FStaticMeshRenderData;
struct FMinimalViewInfo;

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
}
