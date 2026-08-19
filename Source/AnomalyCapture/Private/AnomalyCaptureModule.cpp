#include "Modules/ModuleManager.h"
#include "AnomalyCaptureLog.h"

#if ANOMALY_CAPTURE
#include "Misc/Paths.h"
#include "Interfaces/IPluginManager.h"
#include "ShaderCore.h"
#endif

DEFINE_LOG_CATEGORY(LogAnomalyCapture);

class FAnomalyCaptureModule : public IModuleInterface
{
public:
	virtual void StartupModule() override
	{
#if ANOMALY_CAPTURE
		if (TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("AnomalyInjector")))
		{
			const FString ShaderDir = FPaths::Combine(Plugin->GetBaseDir(), TEXT("Shaders"));
			AddShaderSourceDirectoryMapping(TEXT("/Plugin/AnomalyInjector"), ShaderDir);
		}
		UE_LOG(LogAnomalyCapture, Log, TEXT("AnomalyCapture module started (idle — use IAI.Capture.Start)."));
#else
		UE_LOG(LogAnomalyCapture, Log, TEXT("AnomalyCapture module started (compiled out: ANOMALY_CAPTURE=0)."));
#endif
	}

	virtual void ShutdownModule() override
	{
		UE_LOG(LogAnomalyCapture, Log, TEXT("AnomalyCapture module shut down."));
	}
};

IMPLEMENT_MODULE(FAnomalyCaptureModule, AnomalyCapture)
