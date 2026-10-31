#include "AnomalyVisibleMaskShader.h"

#if ANOMALY_SHADERS && !ANOMALY_SHADERS425

IMPLEMENT_GLOBAL_SHADER(FAnomalyVisibleMaskPS, "/Plugin/AnomalyInjector/Private/AnomalyVisibleMask.usf", "MainPS", SF_Pixel);

#endif
