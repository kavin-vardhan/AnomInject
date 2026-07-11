#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "AnomalyControlServerSubsystem.generated.h"

class IWebSocketServer;
class INetworkingWebSocket;
class FJsonObject;

struct FControlConn
{
	INetworkingWebSocket* Socket = nullptr;
	bool bAuthed = false;
	bool bRejected = false;
	double ConnectTime = 0.0;
	bool bSubSnapshot = false;
	bool bSubFrames = false;
};

UCLASS()
class ANOMALYCONTROLSERVER_API UAnomalyControlServerSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual ~UAnomalyControlServerSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

	bool StartListening(int32 InPort);
	void StopListening();
	void LogStatus() const;
	bool IsListening() const { return bListening; }

protected:
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

private:
#if ANOMALY_CONTROL_SERVER
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

	TUniquePtr<IWebSocketServer> Server;
	TArray<FControlConn> Conns;

	FString Token;
	int32 Port = 8077;

	double SnapshotIntervalSec = 0.2;
	double FrameIntervalSec = 0.5;
	double LastSnapshotTime = 0.0;
	double LastFrameTime = 0.0;
	bool bWantOneFrame = false;

	uint32 FrameCounter = 0;
	uint32 ViewEpoch = 0;
	int32 LastViewportX = 0;
	int32 LastViewportY = 0;

	static constexpr double AuthTimeoutSeconds = 5.0;
#endif

	bool bListening = false;
};
