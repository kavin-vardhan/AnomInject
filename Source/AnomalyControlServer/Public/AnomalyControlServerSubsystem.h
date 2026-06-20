// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "AnomalyControlServerSubsystem.generated.h"

class IWebSocketServer;
class INetworkingWebSocket;
class FJsonObject;

/**
 * One tracked control-server connection. The socket is owned by the WebSocketNetworking server; we hold a
 * non-owning pointer + our auth / loopback / subscription state.
 */
struct FControlConn
{
	INetworkingWebSocket* Socket = nullptr;
	bool bAuthed = false;       // passed the hello{token} handshake
	bool bRejected = false;     // non-loopback / bad token / auth-timeout — never serviced
	double ConnectTime = 0.0;
	bool bSubSnapshot = false;  // subscribed to read-back snapshots
	bool bSubFrames = false;    // subscribed to preview frames
};

/**
 * UAnomalyControlServerSubsystem (Tier-2 external control dashboard — Slice 1: the manual inject/monitor
 * loop + live preview, server side).
 *
 * A SEPARATE world subsystem (Game + PIE only, gotcha G7). Owns a WebSocketNetworking standalone
 * IWebSocketServer and, while clients are subscribed, pushes:
 *  - read-back SNAPSHOTS (WS TEXT/JSON) on a cadence: view + renderable-visible set (with screen-rects) +
 *    active anomalies (id/target/args/source/time) + auto-injector state + session flags + fps; and
 *  - preview FRAMES (WS BINARY: 16-byte header + JPEG) captured game-view-only via FViewport::ReadPixels.
 * It dispatches commands (list_anomalies / subscribe / inject / revert / revert_all / set_viewport_scoping /
 * set_hud / request_frame) to the injector / selector / auto-injector public surfaces.
 *
 * Security v1: listener DORMANT by default (start via IAI.Server.Start); STRICT loopback (non-127.0.0.1/::1
 * and empty peers refused service — INetworkingWebSocket exposes no hard close) + token handshake. Compiled
 * OUT when ANOMALY_CONTROL_SERVER=0 (Shipping). Game-agnostic: public UE APIs only, never host types.
 */
UCLASS()
class ANOMALYCONTROLSERVER_API UAnomalyControlServerSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	// Out-of-line dtor: the TUniquePtr<IWebSocketServer> member needs the complete type at destruction,
	// available in the .cpp (gotcha G9 pattern).
	virtual ~UAnomalyControlServerSubsystem();

	// --- USubsystem / UWorldSubsystem ---
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// --- FTickableGameObject ---
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

	// --- Console-driven control (dormant by default) ---
	bool StartListening(int32 InPort);
	void StopListening();
	void LogStatus() const;
	bool IsListening() const { return bListening; }

protected:
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

private:
#if ANOMALY_CONTROL_SERVER
	// WebSocketNetworking callbacks (game thread, inside Server->Tick()).
	void OnClientConnected(INetworkingWebSocket* Socket);
	void OnReceive(INetworkingWebSocket* Socket, void* Data, int32 Size);
	void OnSocketClosed(INetworkingWebSocket* Socket);

	// Command dispatch + replies.
	void HandleMessage(FControlConn& Conn, const TSharedPtr<FJsonObject>& Msg);
	void SendJson(INetworkingWebSocket* Socket, const TSharedRef<FJsonObject>& Obj) const;
	void SendRawText(INetworkingWebSocket* Socket, const FString& Text) const;
	void SendAck(INetworkingWebSocket* Socket, const FString& ReType) const;

	// Cadence push.
	void PushSnapshots();
	void PushFrames(bool bForce);

	FControlConn* FindConn(INetworkingWebSocket* Socket);
	static bool IsLoopbackAddr(const FString& Addr);

	TUniquePtr<IWebSocketServer> Server;
	TArray<FControlConn> Conns;

	FString Token;
	int32 Port = 8077;

	// Cadence (seconds). Defaults: snapshots ~5 Hz, frames ~2 Hz (modest, to bound the ReadPixels flush).
	double SnapshotIntervalSec = 0.2;
	double FrameIntervalSec = 0.5;
	double LastSnapshotTime = 0.0;
	double LastFrameTime = 0.0;
	bool bWantOneFrame = false;     // request_frame one-shot

	uint32 FrameCounter = 0;
	uint32 ViewEpoch = 0;           // bumps when the viewport resolution changes (frame<->snapshot correlation)
	int32 LastViewportX = 0;
	int32 LastViewportY = 0;

	static constexpr double AuthTimeoutSeconds = 5.0;
#endif // ANOMALY_CONTROL_SERVER

	bool bListening = false;
};
