#include "AnomalyMaskReduceShader.h"

#if ANOMALY_SHADERS

IMPLEMENT_GLOBAL_SHADER(FAnomalyMaskReduceCS, "/Plugin/AnomalyInjector/Private/AnomalyMaskReduce.usf", "MainCS", SF_Compute);

#endif
