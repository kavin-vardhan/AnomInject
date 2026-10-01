#include "AnomalyMeasurability.h"

#if ANOMALY_CAPTURE

#include "AnomalyViewport.h"
#include "AnomalyTargetPolicy.h"

#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "RenderUtils.h"
#include "RHI.h"

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
		if (!SMC)
		{
			return false;
		}
#if WITH_EDITORONLY_DATA
		const bool bFallbackDisplayed = SMC->bDisplayNaniteFallbackMesh;
#else
		const bool bFallbackDisplayed = false;
#endif
		const UStaticMesh* Mesh = SMC->GetStaticMesh();
		return AnomalyTargetPolicy::RouteIsNanite(true, SMC->IsA<USplineMeshComponent>(), SMC->bDisallowNanite,
			bFallbackDisplayed, Mesh && Mesh->HasValidNaniteData(), UseNanite(ShaderPlatform));
	}

	bool ComponentDrawsNanite(const UPrimitiveComponent* Component)
	{
		return ComponentRendersAsNanite(Cast<UStaticMeshComponent>(Component), GMaxRHIShaderPlatform);
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

		if (Renderable > 0 && NaniteOnly == Renderable)
		{
			OutReason = EReason::Nanite;
			return true;
		}
		return false;
	}
}

#endif
