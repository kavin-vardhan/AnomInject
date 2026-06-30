#include "Modules/ModuleManager.h"
#include "AnomalyCaptureLog.h"

DEFINE_LOG_CATEGORY(LogAnomalyCapture);

class FAnomalyCaptureModule : public IModuleInterface
{
public:
	virtual void StartupModule() override
	{
#if ANOMALY_CAPTURE
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
