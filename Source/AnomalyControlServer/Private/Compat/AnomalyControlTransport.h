#pragma once
#include "CoreMinimal.h"
#if ANOMALY_CONTROL_SERVER
struct FAnomalyControlCallbacks
{
    TFunction<void(uint64,const FString&)> Open;
    TFunction<void(uint64,const TArray<uint8>&)> Message;
    TFunction<void(uint64)> Close;
};
class IAnomalyControlTransport
{
public:
    virtual ~IAnomalyControlTransport() = default;
    virtual bool Start(int32 Port,FAnomalyControlCallbacks Callbacks) = 0;
    virtual void Tick() = 0;
    virtual bool Send(uint64 Id,const uint8* Data,int32 Size) = 0;
    virtual void Stop() = 0;
};
TUniquePtr<IAnomalyControlTransport> MakeAnomalyControlTransport();
#endif
