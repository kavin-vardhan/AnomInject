#pragma once

#include "CoreMinimal.h"
#if __has_include("RHIShaderPlatform.h")
#include "RHIShaderPlatform.h"
#endif

#if ANOMALY_CAPTURE

#include "RHIDefinitions.h"

class AActor;
class UPrimitiveComponent;
class UStaticMeshComponent;

namespace AnomalyMeasurability
{
	enum class EReason : uint8
	{
		None,
		Nanite,
	};

	const TCHAR* LexToString(EReason Reason);

	bool ComponentRendersAsNanite(const UStaticMeshComponent* SMC, EShaderPlatform ShaderPlatform);

	bool ComponentDrawsNanite(const UPrimitiveComponent* Component);

	bool IsKnownUnmeasurable(const AActor* Actor, EShaderPlatform ShaderPlatform, EReason& OutReason);
}

#endif
