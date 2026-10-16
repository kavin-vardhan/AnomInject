#include "Modules/ModuleManager.h"
#if !ANOMALY_SHADERS
#include "CoreMinimal.h"
DEFINE_LOG_CATEGORY_STATIC(LogAnomalyShadersStub, Log, All);
#endif

#if ANOMALY_SHADERS
#include "Misc/Paths.h"
#include "Interfaces/IPluginManager.h"
#include "ShaderCore.h"
#endif

class FAnomalyShadersModule : public IModuleInterface
{
public:
	virtual void StartupModule() override
	{
#if ANOMALY_SHADERS
		if (TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("AnomalyInjector")))
		{
			const FString ShaderDir = FPaths::Combine(Plugin->GetBaseDir(), TEXT("Shaders"));
			AddShaderSourceDirectoryMapping(TEXT("/Plugin/AnomalyInjector"), ShaderDir);
		}
#else
		UE_LOG(LogAnomalyShadersStub, Log, TEXT("AnomalyShaders compiled out (ANOMALY_SHADERS=0)."));
#endif
	}

	virtual void ShutdownModule() override
	{
	}
};

IMPLEMENT_MODULE(FAnomalyShadersModule, AnomalyShaders)
