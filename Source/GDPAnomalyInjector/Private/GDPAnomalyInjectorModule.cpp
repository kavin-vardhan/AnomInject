// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#include "Modules/ModuleManager.h"
#include "GDPAnomalyInjectorLog.h"

DEFINE_LOG_CATEGORY(LogGDPAnomaly);

/**
 * Runtime module for the GDP Anomaly Injector plugin.
 * Minimal for M0 — the work lives in UGDPAnomalyInjectorSubsystem and the console
 * command surface. This is just a home for the log category and a future init hook.
 */
class FGDPAnomalyInjectorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override
	{
		UE_LOG(LogGDPAnomaly, Log, TEXT("GDPAnomalyInjector module started."));
	}

	virtual void ShutdownModule() override
	{
		UE_LOG(LogGDPAnomaly, Log, TEXT("GDPAnomalyInjector module shut down."));
	}
};

IMPLEMENT_MODULE(FGDPAnomalyInjectorModule, GDPAnomalyInjector)
