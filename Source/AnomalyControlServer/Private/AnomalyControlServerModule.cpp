#include "Modules/ModuleManager.h"
#include "AnomalyControlServerLog.h"

DEFINE_LOG_CATEGORY(LogAnomalyServer);

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
