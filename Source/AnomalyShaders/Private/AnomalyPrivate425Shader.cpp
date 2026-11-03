#include "AnomalyPrivate425Shader.h"
#if ANOMALY_SHADERS && ANOMALY_SHADERS425
IMPLEMENT_GLOBAL_SHADER(FAnomalyPrivateDepth425VS, "/Plugin/AnomalyInjector/Private/AnomalyPrivate425.usf", "CopyVS", SF_Vertex);
IMPLEMENT_GLOBAL_SHADER(FAnomalyPrivateDepth425PS, "/Plugin/AnomalyInjector/Private/AnomalyPrivate425.usf", "CopyPS", SF_Pixel);
IMPLEMENT_GLOBAL_SHADER(FAnomalyPrivateIds425CS, "/Plugin/AnomalyInjector/Private/AnomalyPrivate425.usf", "IdsCS", SF_Compute);
#endif
