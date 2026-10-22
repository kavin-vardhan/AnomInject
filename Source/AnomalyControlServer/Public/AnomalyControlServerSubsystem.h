#pragma once

#include "CoreMinimal.h"
#include "Compat/AnomalyStatsIncludes.h"
#include "Compat/AnomalyTickableWorldSubsystem.h"
#include "AnomalyControlServerSubsystem.generated.h"

class INetworkingWebSocket;
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
	void OnClientConnected(INetworkingWebSocket* Socket);
	void OnReceive(INetworkingWebSocket* Socket, void* Data, int32 Size);
	void OnSocketClosed(INetworkingWebSocket* Socket);

	void HandleMessage(FControlConn& Conn, const TSharedPtr<FJsonObject>& Msg);
	void SendJson(INetworkingWebSocket* Socket, const TSharedRef<FJsonObject>& Obj) const;
	void SendRawText(INetworkingWebSocket* Socket, const FString& Text) const;
	void SendAck(INetworkingWebSocket* Socket, const FString& ReType) const;

	void PushSnapshots();
	void PushFrames(bool bForce);

	FControlConn* FindConn(INetworkingWebSocket* Socket);
	static bool IsLoopbackAddr(const FString& Addr);

	TUniquePtr<FAnomalyControlTransportState> Transport;

	static constexpr double AuthTimeoutSeconds = 5.0;

	bool bListening = false;
};
