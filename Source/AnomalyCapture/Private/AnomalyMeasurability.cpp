#include "AnomalyMeasurability.h"

#if ANOMALY_CAPTURE

#include "AnomalyViewport.h"

#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "RenderUtils.h"

namespace AnomalyMeasurability
{
	const TCHAR* LexToString(EReason Reason)
	{
		switch (Reason)
		{
		case EReason::Nanite: return TEXT("nanite");
		default:              return TEXT("none");
		}
	}

	bool ComponentRendersAsNanite(const UStaticMeshComponent* SMC, EShaderPlatform ShaderPlatform)
	{
		if (!SMC || SMC->bDisallowNanite)
		{
			return false;
		}
#if WITH_EDITORONLY_DATA
		if (SMC->bDisplayNaniteFallbackMesh)
		{
			return false;
		}
#endif
		const UStaticMesh* Mesh = SMC->GetStaticMesh();
		return Mesh && Mesh->HasValidNaniteData() && UseNanite(ShaderPlatform);
	}

	bool IsKnownUnmeasurable(const AActor* Actor, EShaderPlatform ShaderPlatform, EReason& OutReason)
	{
		OutReason = EReason::None;
		if (!Actor)
		{
			return false;
		}

		TInlineComponentArray<UPrimitiveComponent*> Prims;
		const_cast<AActor*>(Actor)->GetComponents(Prims);

		int32 Renderable = 0;
		int32 NaniteOnly = 0;
		for (const UPrimitiveComponent* Prim : Prims)
		{
			if (!AnomalyViewport::IsRenderableComponent(Prim))
			{
				continue;
			}
			++Renderable;
			if (const UStaticMeshComponent* SMC = Cast<UStaticMeshComponent>(Prim))
			{
				if (ComponentRendersAsNanite(SMC, ShaderPlatform))
				{
					++NaniteOnly;
				}
			}
		}

		if (Renderable > 0 && NaniteOnly > 0)
		{
			OutReason = EReason::Nanite;
			return true;
		}
		return false;
	}
}

#endif
