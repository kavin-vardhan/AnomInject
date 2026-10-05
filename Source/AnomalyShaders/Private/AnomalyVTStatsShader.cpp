#include "AnomalyVTStatsShader.h"

#if ANOMALY_SHADERS
IMPLEMENT_GLOBAL_SHADER(FAnomalyVTStatsCS, "/Plugin/AnomalyInjector/Private/AnomalyVTStats.usf", "MainCS", SF_Compute);
#endif
