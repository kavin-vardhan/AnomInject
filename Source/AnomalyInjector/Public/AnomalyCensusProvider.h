#pragma once

#include "CoreMinimal.h"
#include "Templates/Function.h"

class AActor;

enum class EAnomalyCensusDecision : uint8
{
	NoOpinion = 0,
	Eligible,
	ExcludedZero,
	ExcludedBelowFloor,
	ExcludedAboveCeiling,
	ExcludedTranslucent,
	FallbackBounds
};

ANOMALYINJECTOR_API const TCHAR* LexToStringAnomalyCensusDecision(EAnomalyCensusDecision Decision);

struct FAnomalyCensusOpinion
{
	EAnomalyCensusDecision Decision = EAnomalyCensusDecision::NoOpinion;
	int32 AgeTicks = -1;
	float DrawnPct = -1.0f;
	const TCHAR* Reason = TEXT("none");
	int32 WindowTicks = -1;
	bool bExpired = false;
	bool bUnseen = false;
};

using FAnomalyCensusQueryFn = TFunction<FAnomalyCensusOpinion(const AActor*)>;
using FAnomalyCensusReadyFn = TFunction<bool()>;
using FAnomalyCensusFireReportFn = TFunction<void(int32, int32, int32)>;
