#pragma once

#include "Runtime/Launch/Resources/Version.h"

#define ANOMALY_ENGINE_425 (ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION == 25)
#define ANOMALY_ENGINE_51 (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION == 1)

#if !ANOMALY_ENGINE_425 && !ANOMALY_ENGINE_51
#error AnomalyInjector supports exactly Unreal Engine 4.25 and 5.1.
#endif
