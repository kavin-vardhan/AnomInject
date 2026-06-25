#include "Modules/ModuleManager.h"
#include "AnomalyInjectorLog.h"

DEFINE_LOG_CATEGORY(LogAnomaly);

class FAnomalyInjectorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override
	{
		UE_LOG(LogAnomaly, Log, TEXT("AnomalyInjector module started."));
	}

	virtual void ShutdownModule() override
	{
		UE_LOG(LogAnomaly, Log, TEXT("AnomalyInjector module shut down."));
	}
};

IMPLEMENT_MODULE(FAnomalyInjectorModule, AnomalyInjector)
