#pragma once
#include "AnomalyCaptureFeatures.h"
#if ANOMALY_CAPTURE_FRAMES425
#include "Compat/AnomalyCaptureBackend.h"
#include "AnomalyFrameCapturer.h"
#include "AnomalyViewMetadata425.h"
class FAnomalyCaptureBackend425 : public IAnomalyCaptureBackend
{
public:
    bool RequestFrame(UWorld* World, uint64 Id) override;
    bool PollFrame(FAnomalyCapturedFrame& Frame) override;
    void Stop() override;
    int32 Dropped() const;
    FAnomalyReadbackLayout Layout() const;
    ~FAnomalyCaptureBackend425() override { Stop(); }
private:
    TSharedPtr<FAnomalyFrameCapturer, ESPMode::ThreadSafe> Capturer;
    TSharedPtr<FAnomalyViewMetadata425, ESPMode::ThreadSafe> View;
    TWeakObjectPtr<UWorld> Owner;
    TMap<uint64, int32> PendingAA;
    int32 FinalDrops = 0;
    FAnomalyReadbackLayout FinalLayout;
};
#endif
