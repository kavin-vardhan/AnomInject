// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#include "AnomalyControlServerSubsystem.h"

#include "AnomalyControlServerLog.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "Misc/Guid.h"
#include "HAL/PlatformTime.h"
#include "HAL/IConsoleManager.h"     // FAutoConsoleCommandWithWorldAndArgs (project convention)
#include "Containers/StringConv.h"   // FTCHARToUTF8 / FUTF8ToTCHAR

#if ANOMALY_CONTROL_SERVER
#include "AnomalyPreviewCapture.h"
#include "Widgets/SWindow.h"
#include "Modules/ModuleManager.h"
#include "IWebSocketNetworkingModule.h"
#include "IWebSocketServer.h"
#include "INetworkingWebSocket.h"
#include "WebSocketNetworkingDelegates.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#endif

namespace
{
	constexpr int32 DefaultControlPort = 8077;

	UAnomalyControlServerSubsystem* GetServerSubsystem(UWorld* World)
	{
		return World ? World->GetSubsystem<UAnomalyControlServerSubsystem>() : nullptr;
	}
}

// ----------------------------------------------------------------------------------------------------
// IAI.Server.* console surface (module-scoped, resolves the subsystem from the console's world; G7 guard)
// ----------------------------------------------------------------------------------------------------

static FAutoConsoleCommandWithWorldAndArgs GServerStartCmd(
	TEXT("IAI.Server.Start"),
	TEXT("Start the Anomaly control WebSocket server. Optional arg: port (default 8077). Localhost only."),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
	{
		if (UAnomalyControlServerSubsystem* Sub = GetServerSubsystem(World))
		{
			const int32 P = (Args.Num() > 0) ? FCString::Atoi(*Args[0]) : DefaultControlPort;
			Sub->StartListening(P);
		}
		else
		{
			UE_LOG(LogAnomalyServer, Warning, TEXT("IAI.Server.Start: no control subsystem (run inside a Game/PIE world)."));
		}
	}));

static FAutoConsoleCommandWithWorldAndArgs GServerStopCmd(
	TEXT("IAI.Server.Stop"),
	TEXT("Stop the Anomaly control WebSocket server (closes all connections)."),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& /*Args*/, UWorld* World)
	{
		if (UAnomalyControlServerSubsystem* Sub = GetServerSubsystem(World))
		{
			Sub->StopListening();
		}
	}));

static FAutoConsoleCommandWithWorldAndArgs GServerStatusCmd(
	TEXT("IAI.Server.Status"),
	TEXT("Log the Anomaly control server state (listening / port / token / connections)."),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& /*Args*/, UWorld* World)
	{
		if (UAnomalyControlServerSubsystem* Sub = GetServerSubsystem(World))
		{
			Sub->LogStatus();
		}
	}));

// ----------------------------------------------------------------------------------------------------
// Subsystem
// ----------------------------------------------------------------------------------------------------

UAnomalyControlServerSubsystem::~UAnomalyControlServerSubsystem() = default;

bool UAnomalyControlServerSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

TStatId UAnomalyControlServerSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UAnomalyControlServerSubsystem, STATGROUP_Tickables);
}

void UAnomalyControlServerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
#if ANOMALY_CONTROL_SERVER
	UE_LOG(LogAnomalyServer, Log, TEXT("AnomalyControlServer subsystem initialized (listener dormant — use IAI.Server.Start)."));
#else
	UE_LOG(LogAnomalyServer, Log, TEXT("AnomalyControlServer subsystem initialized (compiled out: ANOMALY_CONTROL_SERVER=0)."));
#endif
}

void UAnomalyControlServerSubsystem::Deinitialize()
{
	StopListening();
	Super::Deinitialize();
}

bool UAnomalyControlServerSubsystem::StartListening(int32 InPort)
{
#if ANOMALY_CONTROL_SERVER
	if (bListening)
	{
		UE_LOG(LogAnomalyServer, Warning, TEXT("Control: already listening on port %d."), Port);
		return true;
	}

	IWebSocketNetworkingModule* Module = FModuleManager::LoadModulePtr<IWebSocketNetworkingModule>(TEXT("WebSocketNetworking"));
	if (!Module)
	{
		UE_LOG(LogAnomalyServer, Error, TEXT("Control: WebSocketNetworking module unavailable (is the plugin enabled?)."));
		return false;
	}

	Server = Module->CreateServer();
	if (!Server.IsValid())
	{
		UE_LOG(LogAnomalyServer, Error, TEXT("Control: CreateServer() failed."));
		return false;
	}

	Port = (InPort > 0) ? InPort : DefaultControlPort;
	Token = FGuid::NewGuid().ToString(EGuidFormats::Digits);

	FWebSocketClientConnectedCallBack ConnectedCb =
		FWebSocketClientConnectedCallBack::CreateUObject(this, &UAnomalyControlServerSubsystem::OnClientConnected);

	if (!Server->Init((uint32)Port, ConnectedCb))
	{
		UE_LOG(LogAnomalyServer, Error, TEXT("Control: server Init failed on port %d."), Port);
		Server.Reset();
		return false;
	}

	if (!Preview.IsValid())
	{
		Preview = MakeUnique<FAnomalyPreviewCapture>();
	}
	Preview->Start();

	bListening = true;
	LastFrameRequestTime = 0.0;
	FrameCounter = 0;

	UE_LOG(LogAnomalyServer, Log, TEXT("=== Anomaly Control Server LISTENING on ws://127.0.0.1:%d ==="), Port);
	UE_LOG(LogAnomalyServer, Log, TEXT("=== Control server token: %s ==="), *Token);
	UE_LOG(LogAnomalyServer, Log, TEXT("    (NOTE: lws binds all interfaces; non-loopback peers are refused service. Token required.)"));
	return true;
#else
	UE_LOG(LogAnomalyServer, Warning, TEXT("Control: server compiled out (ANOMALY_CONTROL_SERVER=0)."));
	return false;
#endif
}

void UAnomalyControlServerSubsystem::StopListening()
{
#if ANOMALY_CONTROL_SERVER
	if (Preview.IsValid())
	{
		Preview->Stop();
	}
	Conns.Reset();
	Server.Reset(); // destroys the server -> closes all connections
	if (bListening)
	{
		UE_LOG(LogAnomalyServer, Log, TEXT("Control: server stopped."));
	}
#endif
	bListening = false;
}

void UAnomalyControlServerSubsystem::LogStatus() const
{
#if ANOMALY_CONTROL_SERVER
	if (!bListening)
	{
		UE_LOG(LogAnomalyServer, Log, TEXT("Control: NOT listening."));
		return;
	}
	int32 Authed = 0;
	for (const FControlConn& C : Conns)
	{
		if (C.bAuthed)
		{
			++Authed;
		}
	}
	UE_LOG(LogAnomalyServer, Log, TEXT("Control: listening ws://127.0.0.1:%d | token=%s | %d connection(s), %d authed | preview=%s"),
		Port, *Token, Conns.Num(), Authed, (Preview.IsValid() && Preview->IsRunning()) ? TEXT("on") : TEXT("off"));
#else
	UE_LOG(LogAnomalyServer, Log, TEXT("Control: compiled out (ANOMALY_CONTROL_SERVER=0)."));
#endif
}

void UAnomalyControlServerSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
#if ANOMALY_CONTROL_SERVER
	if (!bListening || !Server.IsValid())
	{
		return;
	}

	// Service libwebsockets — connect / receive / close callbacks fire synchronously here (game thread).
	Server->Tick();

	// Auth timeout: reject (stop servicing) any peer that never sent a valid hello within the window.
	const double Now = FPlatformTime::Seconds();
	for (FControlConn& C : Conns)
	{
		if (!C.bAuthed && !C.bRejected && (Now - C.ConnectTime) > AuthTimeoutSeconds)
		{
			C.bRejected = true;
			UE_LOG(LogAnomalyServer, Warning, TEXT("Control: auth timeout — ignoring an un-authenticated peer."));
		}
	}

	PushFrameIfReady();
#endif
}

#if ANOMALY_CONTROL_SERVER

FControlConn* UAnomalyControlServerSubsystem::FindConn(INetworkingWebSocket* Socket)
{
	return Conns.FindByPredicate([Socket](const FControlConn& C) { return C.Socket == Socket; });
}

TWeakPtr<SWindow> UAnomalyControlServerSubsystem::GetGameWindow() const
{
	if (GEngine && GEngine->GameViewport)
	{
		return GEngine->GameViewport->GetWindow();
	}
	return nullptr;
}

bool UAnomalyControlServerSubsystem::IsLoopbackAddr(const FString& Addr)
{
	return Addr.StartsWith(TEXT("127."))
		|| Addr == TEXT("::1")
		|| Addr == TEXT("0:0:0:0:0:0:0:1")
		|| Addr.StartsWith(TEXT("::ffff:127."))
		|| Addr.Equals(TEXT("localhost"), ESearchCase::IgnoreCase);
}

void UAnomalyControlServerSubsystem::OnClientConnected(INetworkingWebSocket* Socket)
{
	if (!Socket)
	{
		return;
	}

	const FString Remote = Socket->RemoteEndPoint(/*bAppendPort=*/false);

	FControlConn Conn;
	Conn.Socket = Socket;
	Conn.ConnectTime = FPlatformTime::Seconds();

	// A hard close is not exposed by INetworkingWebSocket; we enforce loopback by REFUSING SERVICE
	// (never Send, ignore recv). SPIKE FINDING TO REPORT: what string RemoteEndPoint() returns for a local
	// browser. Clearly-routable addresses are refused; an empty/unrecognized string is served-with-warning
	// (so the smoke test isn't blocked by an unknown format) — Slice 1 hardens this once the format is known.
	if (Remote.IsEmpty())
	{
		UE_LOG(LogAnomalyServer, Warning,
			TEXT("Control: RemoteEndPoint() returned EMPTY — servicing anyway for the spike. REPORT THIS."));
	}
	else if (!IsLoopbackAddr(Remote))
	{
		Conn.bRejected = true;
		UE_LOG(LogAnomalyServer, Warning, TEXT("Control: refusing non-loopback peer '%s' (no service)."), *Remote);
	}
	else
	{
		UE_LOG(LogAnomalyServer, Log, TEXT("Control: client connected from '%s' (awaiting hello)."), *Remote);
	}

	Conns.Add(Conn);

	// Per-socket callbacks. Lambdas capture the raw socket; the server outlives these until close/teardown,
	// and StopListening() resets the server (dropping all sockets) before this subsystem is destroyed.
	Socket->SetReceiveCallBack(FWebSocketPacketReceivedCallBack::CreateLambda(
		[this, Socket](void* Data, int32 Size) { OnReceive(Socket, Data, Size); }));
	Socket->SetSocketClosedCallBack(FWebSocketInfoCallBack::CreateLambda(
		[this, Socket]() { OnSocketClosed(Socket); }));
	Socket->SetErrorCallBack(FWebSocketInfoCallBack::CreateLambda(
		[this, Socket]() { UE_LOG(LogAnomalyServer, Warning, TEXT("Control: socket error.")); OnSocketClosed(Socket); }));
}

void UAnomalyControlServerSubsystem::OnSocketClosed(INetworkingWebSocket* Socket)
{
	const int32 Removed = Conns.RemoveAll([Socket](const FControlConn& C) { return C.Socket == Socket; });
	if (Removed > 0)
	{
		UE_LOG(LogAnomalyServer, Log, TEXT("Control: connection closed; %d remain."), Conns.Num());
	}
}

void UAnomalyControlServerSubsystem::OnReceive(INetworkingWebSocket* Socket, void* Data, int32 Size)
{
	if (!Data || Size <= 0)
	{
		return;
	}
	FControlConn* Conn = FindConn(Socket);
	if (!Conn || Conn->bRejected)
	{
		return; // never service rejected/unknown peers
	}

	const uint8* Bytes = static_cast<const uint8*>(Data);

	auto BytesToString = [](const uint8* P, int32 N) -> FString
	{
		FUTF8ToTCHAR Conv(reinterpret_cast<const ANSICHAR*>(P), N);
		return FString(Conv.Length(), Conv.Get());
	};

	auto ParseJson = [](const FString& In, TSharedPtr<FJsonObject>& Out) -> bool
	{
		const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(In);
		return FJsonSerializer::Deserialize(Reader, Out) && Out.IsValid();
	};

	// Defensive: try a clean UTF-8 parse first; if that fails and the payload is long enough, retry skipping
	// a possible 4-byte length prefix (the UE netdriver framing convention). Which path wins is a SPIKE FINDING.
	TSharedPtr<FJsonObject> Msg;
	FString Text = BytesToString(Bytes, Size);
	bool bParsed = ParseJson(Text, Msg);
	if (!bParsed && Size > 4)
	{
		Text = BytesToString(Bytes + 4, Size - 4);
		bParsed = ParseJson(Text, Msg);
		if (bParsed)
		{
			UE_LOG(LogAnomalyServer, Warning, TEXT("Control: incoming had a 4-byte size prefix (lws framing) — stripped."));
		}
	}

	if (!bParsed)
	{
		UE_LOG(LogAnomalyServer, Warning, TEXT("Control: unparseable message (%d bytes) — ignored."), Size);
		return;
	}

	HandleMessage(*Conn, Msg);
}

void UAnomalyControlServerSubsystem::HandleMessage(FControlConn& Conn, const TSharedPtr<FJsonObject>& Msg)
{
	FString Type;
	Msg->TryGetStringField(TEXT("type"), Type);

	// Handshake gate.
	if (Type == TEXT("hello"))
	{
		FString Tok;
		Msg->TryGetStringField(TEXT("token"), Tok);
		if (Tok == Token)
		{
			Conn.bAuthed = true;
			const TSharedRef<FJsonObject> Welcome = MakeShared<FJsonObject>();
			Welcome->SetStringField(TEXT("type"), TEXT("welcome"));
			Welcome->SetStringField(TEXT("server"), TEXT("AnomalyControlServer/slice0"));
			Welcome->SetNumberField(TEXT("v"), 1);
			SendJson(Conn.Socket, Welcome);
			UE_LOG(LogAnomalyServer, Log, TEXT("Control: client authenticated."));
		}
		else
		{
			Conn.bRejected = true;
			UE_LOG(LogAnomalyServer, Warning, TEXT("Control: bad token — peer rejected."));
		}
		return;
	}

	if (!Conn.bAuthed)
	{
		return; // ignore everything until authenticated
	}

	// --- Slice 0 protocol stub: echo + on-demand frame request. (Slice 1 replaces this with the real vocab.) ---
	if (Type == TEXT("echo"))
	{
		const TSharedRef<FJsonObject> Reply = MakeShared<FJsonObject>();
		Reply->SetStringField(TEXT("type"), TEXT("echo_reply"));
		const TSharedPtr<FJsonObject>* Payload = nullptr;
		if (Msg->TryGetObjectField(TEXT("payload"), Payload) && Payload)
		{
			Reply->SetObjectField(TEXT("payload"), *Payload);
		}
		SendJson(Conn.Socket, Reply);
		return;
	}

	if (Type == TEXT("frame_request"))
	{
		if (Preview.IsValid())
		{
			Preview->RequestCapture(GetGameWindow());
		}
		return;
	}

	// Default: ack so the client sees a JSON round-trip for any other type.
	const TSharedRef<FJsonObject> Ack = MakeShared<FJsonObject>();
	Ack->SetStringField(TEXT("type"), TEXT("ack"));
	Ack->SetStringField(TEXT("re"), Type);
	SendJson(Conn.Socket, Ack);
}

void UAnomalyControlServerSubsystem::SendJson(INetworkingWebSocket* Socket, const TSharedRef<FJsonObject>& Obj) const
{
	FString Out;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Out);
	FJsonSerializer::Serialize(Obj, Writer);
	SendRawText(Socket, Out);
}

void UAnomalyControlServerSubsystem::SendRawText(INetworkingWebSocket* Socket, const FString& Text) const
{
	if (!Socket)
	{
		return;
	}
	FTCHARToUTF8 Utf8(*Text);
	// bPrependSize=false: send the raw message body (no 4-byte UE length prefix) so a browser WebSocket
	// client receives clean bytes. THIS IS A KEY SPIKE ASSERTION.
	Socket->Send(reinterpret_cast<const uint8*>(Utf8.Get()), (uint32)Utf8.Length(), /*bPrependSize=*/false);
}

void UAnomalyControlServerSubsystem::PushFrameIfReady()
{
	if (!Preview.IsValid())
	{
		return;
	}

	// Only do work if at least one authenticated client is connected.
	bool bAnyAuthed = false;
	for (const FControlConn& C : Conns)
	{
		if (C.bAuthed)
		{
			bAnyAuthed = true;
			break;
		}
	}
	if (!bAnyAuthed)
	{
		return;
	}

	// Throttle frame requests (~2 fps for the spike).
	const double Now = FPlatformTime::Seconds();
	if (Now - LastFrameRequestTime > FrameIntervalSeconds)
	{
		LastFrameRequestTime = Now;
		Preview->RequestCapture(GetGameWindow());
	}

	// If a frame was captured + encoded, push it as a binary WS frame: 16-byte header + JPEG bytes.
	TArray<uint8> Jpeg;
	int32 W = 0;
	int32 H = 0;
	if (!Preview->TakeEncodedJpeg(Jpeg, W, H, /*Quality=*/60))
	{
		return;
	}

	TArray<uint8> Frame;
	Frame.Reserve(16 + Jpeg.Num());
	const uint8 Magic[4] = { 'A', 'I', 'F', '1' };
	Frame.Append(Magic, 4);
	auto PutU32 = [&Frame](uint32 V)
	{
		Frame.Add((uint8)(V & 0xFF));
		Frame.Add((uint8)((V >> 8) & 0xFF));
		Frame.Add((uint8)((V >> 16) & 0xFF));
		Frame.Add((uint8)((V >> 24) & 0xFF));
	};
	auto PutU16 = [&Frame](uint16 V)
	{
		Frame.Add((uint8)(V & 0xFF));
		Frame.Add((uint8)((V >> 8) & 0xFF));
	};
	PutU32(++FrameCounter);
	PutU32(0); // epoch (v1 spike: unused — best-effort overlay alignment lives in Slice 1)
	PutU16((uint16)FMath::Clamp(W, 0, 65535));
	PutU16((uint16)FMath::Clamp(H, 0, 65535));
	Frame.Append(Jpeg);

	for (const FControlConn& C : Conns)
	{
		if (C.bAuthed && C.Socket)
		{
			C.Socket->Send(Frame.GetData(), (uint32)Frame.Num(), /*bPrependSize=*/false);
		}
	}
}

#endif // ANOMALY_CONTROL_SERVER
