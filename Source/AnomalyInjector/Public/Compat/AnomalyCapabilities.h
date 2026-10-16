#pragma once

#include "CoreMinimal.h"
#include "Compat/AnomalyEngineVersion.h"

namespace AnomalyCompat
{
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
        return { ANOMALY_ENGINE_51 && !UE_BUILD_SHIPPING,
            ANOMALY_ENGINE_51 && !UE_BUILD_SHIPPING,
            ANOMALY_ENGINE_51 && !UE_BUILD_SHIPPING,
            ANOMALY_ENGINE_51 && !UE_BUILD_SHIPPING,
            ANOMALY_ENGINE_51 && !UE_BUILD_SHIPPING,
            ANOMALY_PROFILE_BENCH != 0 };
    }
}
