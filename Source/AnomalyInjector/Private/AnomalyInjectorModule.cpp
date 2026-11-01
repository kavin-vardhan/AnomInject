#include "Modules/ModuleManager.h"
#include "Misc/CoreDelegates.h"
#include "AnomalyInjectorLog.h"
#include "AnomalyBenchGate.h"
#include "AnomalyViewport.h"
#include "Compat/AnomalyCapabilities.h"

DEFINE_LOG_CATEGORY(LogAnomaly);

class FAnomalyInjectorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override
	{
		UE_LOG(LogAnomaly, Log, TEXT("AnomalyInjector module started."));
#if ANOMALY_ENGINE_425
		AnomalyViewport::SetNaniteComponentProbe([](const UPrimitiveComponent*) { return false; });
		UE_LOG(LogAnomaly, Display, TEXT("Europa 4.25: stock-no-Nanite component probe; missing_object, blinking, lighting_mismatch, lod_corruption; texture/control backends unavailable."));
#endif
		PostEngineInitHandle = FCoreDelegates::OnPostEngineInit.AddLambda([]()
		{
			AnomalyBenchGate::SweepUngatedLevers(TEXT("post_engine_init"));
		});
	}

	virtual void ShutdownModule() override
	{
#if ANOMALY_ENGINE_425
		AnomalyViewport::SetNaniteComponentProbe(nullptr);
#endif
		FCoreDelegates::OnPostEngineInit.Remove(PostEngineInitHandle);
		PostEngineInitHandle.Reset();
		UE_LOG(LogAnomaly, Log, TEXT("AnomalyInjector module shut down."));
	}

private:
	FDelegateHandle PostEngineInitHandle;
};

IMPLEMENT_MODULE(FAnomalyInjectorModule, AnomalyInjector)
