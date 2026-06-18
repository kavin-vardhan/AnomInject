// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#include "Modules/ModuleManager.h"
#include "AnomalyInjectorLog.h"

DEFINE_LOG_CATEGORY(LogAnomaly);

/**
 * Runtime module for the Anomaly Injector plugin.
 * Minimal for M0 — the work lives in UAnomalyInjectorSubsystem and the console
 * command surface. This is just a home for the log category and a future init hook.
 */
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
