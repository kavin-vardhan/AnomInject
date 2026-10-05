#include "AnomalyVTPageCheckShader.h"

#if ANOMALY_SHADERS
IMPLEMENT_GLOBAL_SHADER(FAnomalyVTPageCheckCS, "/Plugin/AnomalyInjector/Private/AnomalyVTPageCheck.usf", "MainCS", SF_Compute);
#endif
