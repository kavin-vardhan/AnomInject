#include "Modules/ModuleManager.h"
#include "AnomalyCaptureLog.h"
#include "AnomalyViewport.h"
#include "AnomalyMeasurability.h"

DEFINE_LOG_CATEGORY(LogAnomalyCapture);

class FAnomalyCaptureModule : public IModuleInterface
{
public:
	virtual void StartupModule() override
	{
#if ANOMALY_CAPTURE
		AnomalyViewport::SetNaniteComponentProbe(&AnomalyMeasurability::ComponentDrawsNanite);
		UE_LOG(LogAnomalyCapture, Log, TEXT("AnomalyCapture module started (idle — use IAI.Capture.Start)."));
#else
		UE_LOG(LogAnomalyCapture, Log, TEXT("AnomalyCapture module started (compiled out: ANOMALY_CAPTURE=0)."));
#endif
	}

	virtual void ShutdownModule() override
	{
#if ANOMALY_CAPTURE
		AnomalyViewport::SetNaniteComponentProbe(nullptr);
#endif
		UE_LOG(LogAnomalyCapture, Log, TEXT("AnomalyCapture module shut down."));
	}
};

IMPLEMENT_MODULE(FAnomalyCaptureModule, AnomalyCapture)
