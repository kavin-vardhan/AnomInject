#include "Compat/AnomalyCapabilities.h"
#include "AnomalyBenchGate.h"

bool AnomalyCompat::SupportsAnomaly(FName Id)
{
#if ANOMALY_ENGINE_425
	if (Id == FName(TEXT("missing_object"))) { return AnomalyBenchGate::IsEnabled(); }
	return Id == FName(TEXT("blinking")) || Id == FName(TEXT("lighting_mismatch")) || Id == FName(TEXT("lod_corruption"));
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
