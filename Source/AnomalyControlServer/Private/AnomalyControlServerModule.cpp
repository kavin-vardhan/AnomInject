// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#include "Modules/ModuleManager.h"
#include "AnomalyControlServerLog.h"

DEFINE_LOG_CATEGORY(LogAnomalyServer);

/**
 * Runtime module for the Anomaly control server (Slice 0 transport spike). Minimal — the work lives in
 * UAnomalyControlServerSubsystem and the IAI.Server.* console surface. This is just the module entry point.
 */
class FAnomalyControlServerModule : public IModuleInterface
{
public:
	virtual void StartupModule() override
	{
#if ANOMALY_CONTROL_SERVER
		UE_LOG(LogAnomalyServer, Log, TEXT("AnomalyControlServer module started (listener dormant — use IAI.Server.Start)."));
#else
		UE_LOG(LogAnomalyServer, Log, TEXT("AnomalyControlServer module started (compiled out: ANOMALY_CONTROL_SERVER=0)."));
#endif
	}

	virtual void ShutdownModule() override
	{
		UE_LOG(LogAnomalyServer, Log, TEXT("AnomalyControlServer module shut down."));
	}
};

IMPLEMENT_MODULE(FAnomalyControlServerModule, AnomalyControlServer)
