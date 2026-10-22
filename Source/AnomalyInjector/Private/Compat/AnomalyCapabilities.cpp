#include "Compat/AnomalyCapabilities.h"
#include "AnomalyBenchGate.h"

bool AnomalyCompat::SupportsAnomaly(FName Id)
{
#if ANOMALY_ENGINE_425
	if (Id == FName(TEXT("missing_object"))) { return AnomalyBenchGate::IsEnabled(); }
	return Id == FName(TEXT("blinking")) || Id == FName(TEXT("lighting_mismatch")) || Id == FName(TEXT("lod_corruption"));
#else
	static const FName Delivered[] = {
		FName(TEXT("missing_object")), FName(TEXT("blinking")), FName(TEXT("time_dilation")),
		FName(TEXT("lighting_mismatch")), FName(TEXT("lod_corruption")), FName(TEXT("lod_popping")),
		FName(TEXT("camera_clipping")), FName(TEXT("missing_texture")), FName(TEXT("corrupted_texture")),
		FName(TEXT("stuck_low_mip")), FName(TEXT("uv_corruption")), FName(TEXT("normal_corruption")) };
	for (const FName& Supported : Delivered) { if (Id == Supported) { return true; } }
	return AnomalyBenchGate::IsEnabled() && (Id == FName(TEXT("null_effect")) || Id == FName(TEXT("solid_swap")));
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
