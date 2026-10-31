#pragma once

#include "CoreMinimal.h"
#include "Compat/AnomalyEngineVersion.h"

namespace AnomalyCompat
{
	ANOMALYINJECTOR_API bool SupportsAnomaly(FName Id);
	ANOMALYINJECTOR_API const TCHAR* UnsupportedAnomalyReason();

    struct FBackendCapabilities
    {
        bool Capture;
        bool LabelsAndMasks;
        bool ControlTransport;
        bool PluginShaders;
        bool TextureAdapters;
        bool BenchFixtures;
    };

    constexpr FBackendCapabilities PhaseABackends()
    {
        return { (ANOMALY_ENGINE_51 || ANOMALY_ENGINE_425) && !UE_BUILD_SHIPPING,
            ANOMALY_ENGINE_51 && !UE_BUILD_SHIPPING,
            ANOMALY_ENGINE_51 && !UE_BUILD_SHIPPING,
            (ANOMALY_ENGINE_51 || ANOMALY_ENGINE_425) && !UE_BUILD_SHIPPING,
            ANOMALY_ENGINE_51 && !UE_BUILD_SHIPPING,
            ANOMALY_PROFILE_BENCH != 0 };
    }
}
