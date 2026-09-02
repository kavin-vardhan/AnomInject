#include "AnomalyCensusProvider.h"

const TCHAR* LexToStringAnomalyCensusDecision(EAnomalyCensusDecision Decision)
{
	switch (Decision)
	{
	case EAnomalyCensusDecision::Eligible:            return TEXT("ELIGIBLE");
	case EAnomalyCensusDecision::ExcludedZero:        return TEXT("EXCLUDED-zero");
	case EAnomalyCensusDecision::ExcludedBelowFloor:  return TEXT("EXCLUDED-below_floor");
	case EAnomalyCensusDecision::ExcludedAboveCeiling: return TEXT("EXCLUDED-above_ceiling");
	case EAnomalyCensusDecision::ExcludedTranslucent: return TEXT("EXCLUDED-translucent");
	case EAnomalyCensusDecision::FallbackBounds:      return TEXT("FALLBACK-bounds");
	default:                                          return TEXT("NO-OPINION");
	}
}
