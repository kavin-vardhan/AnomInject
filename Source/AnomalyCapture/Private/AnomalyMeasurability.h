#pragma once

#include "CoreMinimal.h"

#if ANOMALY_CAPTURE

#include "RHIDefinitions.h"

class AActor;
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

	bool IsKnownUnmeasurable(const AActor* Actor, EShaderPlatform ShaderPlatform, EReason& OutReason);
}

#endif
