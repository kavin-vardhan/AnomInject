// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "AnomalyControlServerSubsystem.generated.h"

class IWebSocketServer;
class INetworkingWebSocket;
class FAnomalyPreviewCapture;
class FJsonObject;
class SWindow;

/**
 * One tracked control-server connection (Slice 0 spike bookkeeping). Plain C++; the socket is owned by
 * the WebSocketNetworking server, we only hold a non-owning pointer + our auth/loopback state.
 */
struct FControlConn
{
	INetworkingWebSocket* Socket = nullptr;
	bool bAuthed = false;     // passed the hello{token} handshake
	bool bRejected = false;   // non-loopback peer, bad token, or auth-timeout — never serviced (no Send, ignore recv)
	double ConnectTime = 0.0;
};

/**
 * UAnomalyControlServerSubsystem  (Tier-2 external control dashboard — Slice 0: TRANSPORT SPIKE)
 *
 * A SEPARATE world subsystem (Game + PIE only, gotcha G7) from the injector/selector/auto-injector. It
 * owns a WebSocketNetworking standalone IWebSocketServer and, on a throttle, pushes a JPEG preview frame
 * (captured via FSlateRenderer::OnBackBufferReadyToPresent) to connected loopback clients.
 *
 * SCOPE (Slice 0 only): stand up the transport and de-risk the one Experimental dependency. It does NOT
 * yet drive injection — no command vocabulary beyond a hello/echo/frame_request stub. Slice 1 adds the
 * real command dispatch + read-back snapshot against the injector/selector/auto-injector.
 *
 * Security (v1): listener DORMANT by default (start only via IAI.Server.Start); loopback-only peers
 * (non-127.0.0.1/::1 refused service — a hard close is not exposed by INetworkingWebSocket); token
 * handshake (hello{token} within a timeout or the peer is ignored). Compiled OUT when
 * ANOMALY_CONTROL_SERVER=0 (Shipping).
 *
 * Stays game-agnostic: public UE APIs only, never host types.
 */
UCLASS()
class ANOMALYCONTROLSERVER_API UAnomalyControlServerSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	// Out-of-line dtor: TUniquePtr<IWebSocketServer>/<FAnomalyPreviewCapture> members need the complete
	// type at destruction, which is available in the .cpp translation unit (gotcha G9 pattern).
	virtual ~UAnomalyControlServerSubsystem();

	// --- USubsystem / UWorldSubsystem ---
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// --- FTickableGameObject ---
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

	// --- Console-driven control (dormant by default) ---

	/** Start the WebSocket listener on InPort (default 8077). Generates + logs a fresh token. Returns true on success. */
	bool StartListening(int32 InPort);

	/** Stop the listener; destroys the server (closing all connections) and unregisters the preview capture. */
	void StopListening();

	/** Log listening state, port, token, and connection count (IAI.Server.Status). */
	void LogStatus() const;

	bool IsListening() const { return bListening; }

protected:
	/** Game (standalone) + PIE only; never the editor preview/editing world (gotcha G7). */
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

private:
#if ANOMALY_CONTROL_SERVER
	// WebSocketNetworking callbacks (fire on the game thread, inside Server->Tick()).
	void OnClientConnected(INetworkingWebSocket* Socket);
	void OnReceive(INetworkingWebSocket* Socket, void* Data, int32 Size);
	void OnSocketClosed(INetworkingWebSocket* Socket);

	// Protocol stub + helpers.
	void HandleMessage(FControlConn& Conn, const TSharedPtr<FJsonObject>& Msg);
	void SendJson(INetworkingWebSocket* Socket, const TSharedRef<FJsonObject>& Obj) const;
	void SendRawText(INetworkingWebSocket* Socket, const FString& Text) const;
	void PushFrameIfReady();
	FControlConn* FindConn(INetworkingWebSocket* Socket);
	TWeakPtr<SWindow> GetGameWindow() const;
	static bool IsLoopbackAddr(const FString& Addr);

	TUniquePtr<IWebSocketServer> Server;
	TUniquePtr<FAnomalyPreviewCapture> Preview;
	TArray<FControlConn> Conns;

	FString Token;
	int32 Port = 8077;
	double LastFrameRequestTime = 0.0;
	uint32 FrameCounter = 0;

	// Spike tunables.
	static constexpr double AuthTimeoutSeconds = 5.0;
	static constexpr double FrameIntervalSeconds = 0.5;   // ~2 fps preview for the spike
#endif // ANOMALY_CONTROL_SERVER

	bool bListening = false;
};
