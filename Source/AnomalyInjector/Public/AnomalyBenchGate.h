#pragma once

#include "CoreMinimal.h"

struct IConsoleCommand;

namespace AnomalyBenchGate
{
	ANOMALYINJECTOR_API bool IsEnabled();

	ANOMALYINJECTOR_API FString DescribeFlags();

	ANOMALYINJECTOR_API int32 SweepUngatedLevers(const TCHAR* Where);

	ANOMALYINJECTOR_API IConsoleCommand* FindLeverCommand(const TCHAR* Name);
}
