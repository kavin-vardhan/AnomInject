#include "Modules/ModuleManager.h"
#include "Misc/CoreDelegates.h"
#include "AnomalyInjectorLog.h"
#include "AnomalyBenchGate.h"

DEFINE_LOG_CATEGORY(LogAnomaly);

class FAnomalyInjectorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override
	{
		UE_LOG(LogAnomaly, Log, TEXT("AnomalyInjector module started."));
		PostEngineInitHandle = FCoreDelegates::OnPostEngineInit.AddLambda([]()
		{
			AnomalyBenchGate::SweepUngatedLevers(TEXT("post_engine_init"));
		});
	}

	virtual void ShutdownModule() override
	{
		FCoreDelegates::OnPostEngineInit.Remove(PostEngineInitHandle);
		PostEngineInitHandle.Reset();
		UE_LOG(LogAnomaly, Log, TEXT("AnomalyInjector module shut down."));
	}

private:
	FDelegateHandle PostEngineInitHandle;
};

IMPLEMENT_MODULE(FAnomalyInjectorModule, AnomalyInjector)
