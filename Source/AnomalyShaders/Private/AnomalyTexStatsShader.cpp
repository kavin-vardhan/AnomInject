#include "AnomalyTexStatsShader.h"

#if ANOMALY_SHADERS && !ANOMALY_SHADERS425

IMPLEMENT_GLOBAL_SHADER(FAnomalyTexStatsCS, "/Plugin/AnomalyInjector/Private/AnomalyTexStats.usf", "MainCS", SF_Compute);

#endif
