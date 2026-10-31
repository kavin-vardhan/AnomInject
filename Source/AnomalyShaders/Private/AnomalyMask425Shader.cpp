#include "AnomalyMask425Shader.h"
#if ANOMALY_SHADERS && ANOMALY_SHADERS425
IMPLEMENT_GLOBAL_SHADER(FAnomalyMask425CS, "/Plugin/AnomalyInjector/Private/AnomalyMask425.usf", "MainCS", SF_Compute);
#endif
