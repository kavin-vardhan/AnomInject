#include "Compat/AnomalyCapabilities.h"

bool AnomalyCompat::SupportsAnomaly(FName Id)
{
#if ANOMALY_ENGINE_425
	return Id == FName(TEXT("missing_object")) || Id == FName(TEXT("blinking")) || Id == FName(TEXT("lighting_mismatch")) || Id == FName(TEXT("lod_corruption"));
#else
	return true;
#endif
}

const TCHAR* AnomalyCompat::UnsupportedAnomalyReason()
{
#if ANOMALY_ENGINE_425
	return TEXT("unsupported_engine_phase_a");
#else
	return TEXT("unsupported_anomaly");
#endif
}
