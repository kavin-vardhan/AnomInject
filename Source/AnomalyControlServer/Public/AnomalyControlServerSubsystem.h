#pragma once

#include "CoreMinimal.h"
#include "Compat/AnomalyStatsIncludes.h"
#include "Compat/AnomalyTickableWorldSubsystem.h"
#include "AnomalyControlServerSubsystem.generated.h"

class FJsonObject;

struct FControlConn;
struct FAnomalyControlTransportState;

UCLASS()
class ANOMALYCONTROLSERVER_API UAnomalyControlServerSubsystem : public UAnomalyTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	UAnomalyControlServerSubsystem();
	UAnomalyControlServerSubsystem(FVTableHelper& Helper);
	virtual ~UAnomalyControlServerSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

	bool StartListening(int32 InPort);
	void StopListening();
	void LogStatus() const;
	bool IsListening() const { return bListening; }

private:
	void OnClientConnected(uint64 Id, const FString& Remote);
	void OnReceive(uint64 Id, const void* Data, int32 Size);
	void OnSocketClosed(uint64 Id);

	void HandleMessage(FControlConn& Conn, const TSharedPtr<FJsonObject>& Msg);
	void SendJson(uint64 Id, const TSharedRef<FJsonObject>& Obj) const;
	void SendRawText(uint64 Id, const FString& Text) const;
	void SendAck(uint64 Id, const FString& ReType) const;

	void PushSnapshots();
	void PushFrames(bool bForce);

	FControlConn* FindConn(uint64 Id);
	static bool IsLoopbackAddr(const FString& Addr);

	TUniquePtr<FAnomalyControlTransportState> Transport;

	static constexpr double AuthTimeoutSeconds = 5.0;

	bool bListening = false;
};
