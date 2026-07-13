#include "AnomalyControlServerSubsystem.h"

#include "AnomalyControlServerLog.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "Misc/Guid.h"
#include "HAL/PlatformTime.h"
#include "HAL/IConsoleManager.h"
#include "Containers/StringConv.h"
#include "Misc/ConfigCacheIni.h"
#include "CoreGlobals.h"

#if ANOMALY_CONTROL_SERVER
#include "AnomalyPreviewCapture.h"
#include "ControlProtocol.h"
#include "ControlSnapshot.h"
#include "AnomalyInjectorSubsystem.h"
#include "AnomalySelectorSubsystem.h"
#include "AnomalyAutoInjectorSubsystem.h"
#include "AnomalyViewport.h"
#include "AnomalyCaptureSubsystem.h"
#include "Modules/ModuleManager.h"
#include "IWebSocketNetworkingModule.h"
#include "IWebSocketServer.h"
#include "INetworkingWebSocket.h"
#include "WebSocketNetworkingDelegates.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Serialization/JsonReader.h"
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
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>&  , UWorld* World)
	{
		if (UAnomalyControlServerSubsystem* Sub = GetServerSubsystem(World))
		{
			Sub->StopListening();
		}
	}));

static FAutoConsoleCommandWithWorldAndArgs GServerStatusCmd(
	TEXT("IAI.Server.Status"),
	TEXT("Log the Anomaly control server state (listening / port / token / connections)."),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>&  , UWorld* World)
	{
		if (UAnomalyControlServerSubsystem* Sub = GetServerSubsystem(World))
		{
			Sub->LogStatus();
		}
	}));


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

	FString ConfigToken;
	const bool bHasConfigToken = GConfig
		&& GConfig->GetString(TEXT("AnomalyControlServer"), TEXT("Token"), ConfigToken, GGameIni)
		&& !ConfigToken.IsEmpty();
	Token = bHasConfigToken ? ConfigToken : FGuid::NewGuid().ToString(EGuidFormats::Digits);

	FWebSocketClientConnectedCallBack ConnectedCb =
		FWebSocketClientConnectedCallBack::CreateUObject(this, &UAnomalyControlServerSubsystem::OnClientConnected);

	if (!Server->Init((uint32)Port, ConnectedCb))
	{
		UE_LOG(LogAnomalyServer, Error, TEXT("Control: server Init failed on port %d."), Port);
		Server.Reset();
		return false;
	}

	bListening = true;
	LastSnapshotTime = 0.0;
	LastFrameTime = 0.0;
	FrameCounter = 0;
	ViewEpoch = 0;
	LastViewportX = 0;
	LastViewportY = 0;
	bWantOneFrame = false;

	UE_LOG(LogAnomalyServer, Log, TEXT("=== Anomaly Control Server LISTENING on ws://127.0.0.1:%d ==="), Port);
	UE_LOG(LogAnomalyServer, Log, TEXT("=== Control server token: %s (%s) ==="), *Token,
		bHasConfigToken ? TEXT("from DefaultGame.ini [AnomalyControlServer] Token") : TEXT("random per-session"));
	return true;
#else
	UE_LOG(LogAnomalyServer, Warning, TEXT("Control: server compiled out (ANOMALY_CONTROL_SERVER=0)."));
	return false;
#endif
}

void UAnomalyControlServerSubsystem::StopListening()
{
#if ANOMALY_CONTROL_SERVER
	Conns.Reset();
	Server.Reset();
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
	UE_LOG(LogAnomalyServer, Log, TEXT("Control: listening ws://127.0.0.1:%d | token=%s | %d connection(s), %d authed | snapHz~%.1f frameHz~%.1f"),
		Port, *Token, Conns.Num(), Authed,
		SnapshotIntervalSec > 0 ? 1.0 / SnapshotIntervalSec : 0.0,
		FrameIntervalSec > 0 ? 1.0 / FrameIntervalSec : 0.0);
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

	Server->Tick();

	const double Now = FPlatformTime::Seconds();
	for (FControlConn& C : Conns)
	{
		if (!C.bAuthed && !C.bRejected && (Now - C.ConnectTime) > AuthTimeoutSeconds)
		{
			C.bRejected = true;
			UE_LOG(LogAnomalyServer, Warning, TEXT("Control: auth timeout — ignoring an un-authenticated peer."));
		}
	}

	if (Now - LastSnapshotTime >= SnapshotIntervalSec)
	{
		LastSnapshotTime = Now;
		PushSnapshots();
	}
	if (bWantOneFrame || (Now - LastFrameTime >= FrameIntervalSec))
	{
		LastFrameTime = Now;
		PushFrames(bWantOneFrame);
		bWantOneFrame = false;
	}
#endif
}

#if ANOMALY_CONTROL_SERVER

FControlConn* UAnomalyControlServerSubsystem::FindConn(INetworkingWebSocket* Socket)
{
	return Conns.FindByPredicate([Socket](const FControlConn& C) { return C.Socket == Socket; });
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

	const FString Remote = Socket->RemoteEndPoint( false);

	FControlConn Conn;
	Conn.Socket = Socket;
	Conn.ConnectTime = FPlatformTime::Seconds();

	if (!IsLoopbackAddr(Remote))
	{
		Conn.bRejected = true;
		UE_LOG(LogAnomalyServer, Warning, TEXT("Control: refusing non-loopback peer '%s' (no service)."),
			Remote.IsEmpty() ? TEXT("(empty)") : *Remote);
	}
	else
	{
		UE_LOG(LogAnomalyServer, Log, TEXT("Control: client connected from '%s' (awaiting hello)."), *Remote);
	}

	Conns.Add(Conn);

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
		return;
	}

	const FUTF8ToTCHAR Conv(reinterpret_cast<const ANSICHAR*>(Data), Size);
	const FString Text(Conv.Length(), Conv.Get());

	TSharedPtr<FJsonObject> Msg;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Text);
	if (!FJsonSerializer::Deserialize(Reader, Msg) || !Msg.IsValid())
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

	if (Type == TEXT("hello"))
	{
		FString Tok;
		Msg->TryGetStringField(TEXT("token"), Tok);
		if (Tok == Token)
		{
			Conn.bAuthed = true;
			const TSharedRef<FJsonObject> Welcome = MakeShared<FJsonObject>();
			Welcome->SetStringField(TEXT("type"), TEXT("welcome"));
			Welcome->SetStringField(TEXT("server"), TEXT("AnomalyControlServer/slice1"));
			Welcome->SetNumberField(TEXT("v"), ControlProtocol::Version);
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
		return;
	}

	UWorld* World = GetWorld();

	if (Type == TEXT("list_anomalies"))
	{
		SendRawText(Conn.Socket, ControlSnapshot::BuildCatalogJson(World));
		return;
	}

	if (Type == TEXT("subscribe"))
	{
		Conn.bSubSnapshot = false;
		Conn.bSubFrames = false;
		const TArray<TSharedPtr<FJsonValue>>* Channels = nullptr;
		if (Msg->TryGetArrayField(TEXT("channels"), Channels) && Channels)
		{
			for (const TSharedPtr<FJsonValue>& V : *Channels)
			{
				const FString Channel = V->AsString();
				if (Channel == TEXT("snapshot")) { Conn.bSubSnapshot = true; }
				else if (Channel == TEXT("frames")) { Conn.bSubFrames = true; }
			}
		}
		double Hz = 0.0;
		if (Msg->TryGetNumberField(TEXT("snapshotHz"), Hz) && Hz > 0.0)
		{
			SnapshotIntervalSec = 1.0 / FMath::Clamp(Hz, 1.0, 20.0);
		}
		if (Msg->TryGetNumberField(TEXT("frameHz"), Hz) && Hz > 0.0)
		{
			FrameIntervalSec = 1.0 / FMath::Clamp(Hz, 0.5, 10.0);
		}
		SendAck(Conn.Socket, TEXT("subscribe"));
		return;
	}

	if (Type == TEXT("inject"))
	{
		FString Anomaly, Target;
		Msg->TryGetStringField(TEXT("anomaly"), Anomaly);
		Msg->TryGetStringField(TEXT("target"), Target);

		TArray<FString> ApplyArgs;
		if (!Target.IsEmpty())
		{
			ApplyArgs.Add(Target);
		}
		const TArray<TSharedPtr<FJsonValue>>* ArgsArr = nullptr;
		if (Msg->TryGetArrayField(TEXT("args"), ArgsArr) && ArgsArr)
		{
			for (const TSharedPtr<FJsonValue>& V : *ArgsArr)
			{
				ApplyArgs.Add(V->AsString());
			}
		}

		bool bApplied = false;
		if (UAnomalyInjectorSubsystem* Inj = World ? World->GetSubsystem<UAnomalyInjectorSubsystem>() : nullptr)
		{
			bApplied = Inj->ApplyAnomaly(FName(*Anomaly), ApplyArgs);
		}

		const TSharedRef<FJsonObject> Ack = MakeShared<FJsonObject>();
		Ack->SetStringField(TEXT("type"), TEXT("ack"));
		Ack->SetStringField(TEXT("re"), TEXT("inject"));
		Ack->SetStringField(TEXT("anomaly"), Anomaly);
		Ack->SetBoolField(TEXT("applied"), bApplied);
		SendJson(Conn.Socket, Ack);
		return;
	}

	if (Type == TEXT("revert"))
	{
		FString Anomaly;
		Msg->TryGetStringField(TEXT("anomaly"), Anomaly);
		if (UAnomalyInjectorSubsystem* Inj = World ? World->GetSubsystem<UAnomalyInjectorSubsystem>() : nullptr)
		{
			Inj->RevertAnomaly(FName(*Anomaly));
		}
		SendAck(Conn.Socket, TEXT("revert"));
		return;
	}

	if (Type == TEXT("revert_all"))
	{
		if (UAnomalyInjectorSubsystem* Inj = World ? World->GetSubsystem<UAnomalyInjectorSubsystem>() : nullptr)
		{
			Inj->RevertAllActive();
		}
		SendAck(Conn.Socket, TEXT("revert_all"));
		return;
	}

	if (Type == TEXT("set_viewport_scoping"))
	{
		bool bEnabled = false;
		Msg->TryGetBoolField(TEXT("enabled"), bEnabled);
		if (UAnomalyInjectorSubsystem* Inj = World ? World->GetSubsystem<UAnomalyInjectorSubsystem>() : nullptr)
		{
			Inj->SetViewportScoping(bEnabled);
		}
		SendAck(Conn.Socket, TEXT("set_viewport_scoping"));
		return;
	}

	if (Type == TEXT("set_hud"))
	{
		FString Which;
		bool bEnabled = false;
		Msg->TryGetStringField(TEXT("which"), Which);
		Msg->TryGetBoolField(TEXT("enabled"), bEnabled);
		if (Which == TEXT("selector"))
		{
			if (UAnomalySelectorSubsystem* Sel = World ? World->GetSubsystem<UAnomalySelectorSubsystem>() : nullptr)
			{
				Sel->SetUIEnabled(bEnabled);
			}
		}
		else if (Which == TEXT("auto"))
		{
			if (UAnomalyAutoInjectorSubsystem* Auto = World ? World->GetSubsystem<UAnomalyAutoInjectorSubsystem>() : nullptr)
			{
				Auto->SetEnabled(bEnabled);
			}
		}
		SendAck(Conn.Socket, TEXT("set_hud"));
		return;
	}

	if (Type == TEXT("request_frame"))
	{
		bWantOneFrame = true;
		return;
	}

	if (Type == TEXT("set_poll_radius"))
	{
		double Cm = 0.0;
		Msg->TryGetNumberField(TEXT("cm"), Cm);
		AnomalyViewport::SetPollRadius((float)Cm);
		SendAck(Conn.Socket, TEXT("set_poll_radius"));
		return;
	}

	if (Type == TEXT("set_min_screen_coverage"))
	{
		double Pct = 0.0;
		Msg->TryGetNumberField(TEXT("pct"), Pct);
		AnomalyViewport::SetMinScreenCoveragePct((float)Pct);
		SendAck(Conn.Socket, TEXT("set_min_screen_coverage"));
		return;
	}

	if (Type == TEXT("auto_config"))
	{
		if (UAnomalyAutoInjectorSubsystem* Auto = World ? World->GetSubsystem<UAnomalyAutoInjectorSubsystem>() : nullptr)
		{
			const TSharedPtr<FJsonObject>* Pool = nullptr;
			if (Msg->TryGetObjectField(TEXT("pool"), Pool) && Pool && Pool->IsValid())
			{
				for (const TPair<FString, TSharedPtr<FJsonValue>>& KV : (*Pool)->Values)
				{
					if (KV.Value.IsValid())
					{
						Auto->SetAnomalyEnabled(FName(*KV.Key), KV.Value->AsBool());
					}
				}
			}
			double MinV = 0.0, MaxV = 0.0;
			const bool bHasIntMin = Msg->TryGetNumberField(TEXT("intervalMin"), MinV);
			double MaxTmp = 0.0;
			const bool bHasIntMax = Msg->TryGetNumberField(TEXT("intervalMax"), MaxTmp);
			if (bHasIntMin || bHasIntMax)
			{
				float CurMin = 0.0f, CurMax = 0.0f;
				Auto->GetIntervalRange(CurMin, CurMax);
				Auto->SetIntervalRange(bHasIntMin ? (float)MinV : CurMin, bHasIntMax ? (float)MaxTmp : CurMax);
			}
			const bool bHasHoldMin = Msg->TryGetNumberField(TEXT("holdMin"), MinV);
			const bool bHasHoldMax = Msg->TryGetNumberField(TEXT("holdMax"), MaxTmp);
			if (bHasHoldMin || bHasHoldMax)
			{
				float CurMin = 0.0f, CurMax = 0.0f;
				Auto->GetHoldRange(CurMin, CurMax);
				Auto->SetHoldRange(bHasHoldMin ? (float)MinV : CurMin, bHasHoldMax ? (float)MaxTmp : CurMax);
			}
			double NumV = 0.0;
			if (Msg->TryGetNumberField(TEXT("maxConcurrent"), NumV)) { Auto->SetMaxConcurrent((int32)NumV); }
			bool bPersist = false;
			if (Msg->TryGetBoolField(TEXT("persist"), bPersist)) { Auto->SetPersist(bPersist); }
			if (Msg->TryGetNumberField(TEXT("seed"), NumV)) { Auto->SetSeed((int32)NumV); }
		}
		SendAck(Conn.Socket, TEXT("auto_config"));
		return;
	}

	if (Type == TEXT("auto_run"))
	{
		bool bRunning = false;
		Msg->TryGetBoolField(TEXT("running"), bRunning);
		if (UAnomalyAutoInjectorSubsystem* Auto = World ? World->GetSubsystem<UAnomalyAutoInjectorSubsystem>() : nullptr)
		{
			if (bRunning) { Auto->SetEnabled(true); }
			Auto->SetRunning(bRunning);
		}
		SendAck(Conn.Socket, TEXT("auto_run"));
		return;
	}

	if (Type == TEXT("auto_step"))
	{
		double Seconds = 0.0;
		Msg->TryGetNumberField(TEXT("seconds"), Seconds);
		if (UAnomalyAutoInjectorSubsystem* Auto = World ? World->GetSubsystem<UAnomalyAutoInjectorSubsystem>() : nullptr)
		{
			Auto->AdvanceTime((float)Seconds);
		}
		SendAck(Conn.Socket, TEXT("auto_step"));
		return;
	}

	if (Type == TEXT("auto_fire_once"))
	{
		if (UAnomalyAutoInjectorSubsystem* Auto = World ? World->GetSubsystem<UAnomalyAutoInjectorSubsystem>() : nullptr)
		{
			Auto->TryFireOnce();
		}
		SendAck(Conn.Socket, TEXT("auto_fire_once"));
		return;
	}

	if (Type == TEXT("capture_start"))
	{
		FString Dir, Format, Anomaly, TargetActor;
		Msg->TryGetStringField(TEXT("dir"), Dir);
		Msg->TryGetStringField(TEXT("format"), Format);
		Msg->TryGetStringField(TEXT("anomaly"), Anomaly);
		Msg->TryGetStringField(TEXT("target"), TargetActor);
		double SeedV = -1.0;
		Msg->TryGetNumberField(TEXT("seed"), SeedV);
		double MaxFramesV = 0.0;
		Msg->TryGetNumberField(TEXT("maxFrames"), MaxFramesV);
		const bool bPng = !Format.Equals(TEXT("jpeg"), ESearchCase::IgnoreCase);
		if (UAnomalyCaptureSubsystem* Cap = World ? World->GetSubsystem<UAnomalyCaptureSubsystem>() : nullptr)
		{
			Cap->StartRun(Dir, bPng, (int32)SeedV, (int32)MaxFramesV, Anomaly, TargetActor);
		}
		SendAck(Conn.Socket, TEXT("capture_start"));
		return;
	}

	if (Type == TEXT("capture_stop"))
	{
		bool bRun = false;
		int32 Frames = 0, Seed = 0, FrameCap = 0;
		FString RunDir, SessionId;
		UAnomalyCaptureSubsystem::FLastRunPacing Pacing;
		if (UAnomalyCaptureSubsystem* Cap = World ? World->GetSubsystem<UAnomalyCaptureSubsystem>() : nullptr)
		{
			Cap->StopRun();
			Cap->GetStatus(bRun, Frames, RunDir, Seed);
			FrameCap = Cap->GetFrameCap();
			SessionId = Cap->GetSessionId();
			Pacing = Cap->GetLastRunPacing();
		}
		const TSharedRef<FJsonObject> Reply = MakeShared<FJsonObject>();
		Reply->SetStringField(TEXT("type"), TEXT("capture_stopped"));
		Reply->SetBoolField(TEXT("running"), bRun);
		Reply->SetStringField(TEXT("runDir"), RunDir);
		Reply->SetStringField(TEXT("sessionId"), SessionId);
		Reply->SetNumberField(TEXT("frames"), Frames);
		Reply->SetNumberField(TEXT("maxFrames"), FrameCap);
		Reply->SetNumberField(TEXT("seed"), Seed);
		if (Pacing.bValid)
		{
			Reply->SetNumberField(TEXT("targetFps"), Pacing.TargetFps);
			Reply->SetNumberField(TEXT("stampedFps"), Pacing.StampedFps);
			Reply->SetNumberField(TEXT("speedRatio"), Pacing.SpeedRatio);
			Reply->SetBoolField(TEXT("paced"), Pacing.bPaced);
		}
		SendJson(Conn.Socket, Reply);
		return;
	}

	if (Type == TEXT("capture_status"))
	{
		bool bRun = false;
		int32 Frames = 0, Seed = 0, FrameCap = 0;
		FString RunDir, SessionId;
		UAnomalyCaptureSubsystem::FLastRunPacing Pacing;
		if (UAnomalyCaptureSubsystem* Cap = World ? World->GetSubsystem<UAnomalyCaptureSubsystem>() : nullptr)
		{
			Cap->GetStatus(bRun, Frames, RunDir, Seed);
			FrameCap = Cap->GetFrameCap();
			SessionId = Cap->GetSessionId();
			Pacing = Cap->GetLastRunPacing();
		}
		const int32 Remaining = FrameCap > 0 ? FMath::Max(0, FrameCap - Frames) : -1;
		const TSharedRef<FJsonObject> Reply = MakeShared<FJsonObject>();
		Reply->SetStringField(TEXT("type"), TEXT("capture_status"));
		Reply->SetBoolField(TEXT("running"), bRun);
		Reply->SetStringField(TEXT("runDir"), RunDir);
		Reply->SetStringField(TEXT("sessionId"), SessionId);
		Reply->SetNumberField(TEXT("frames"), Frames);
		Reply->SetNumberField(TEXT("maxFrames"), FrameCap);
		Reply->SetNumberField(TEXT("framesRemaining"), Remaining);
		Reply->SetNumberField(TEXT("seed"), Seed);
		if (Pacing.bValid)
		{
			Reply->SetNumberField(TEXT("targetFps"), Pacing.TargetFps);
			Reply->SetNumberField(TEXT("stampedFps"), Pacing.StampedFps);
			Reply->SetNumberField(TEXT("speedRatio"), Pacing.SpeedRatio);
			Reply->SetBoolField(TEXT("paced"), Pacing.bPaced);
		}
		SendJson(Conn.Socket, Reply);
		return;
	}

	SendAck(Conn.Socket, Type);
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
	Socket->Send(reinterpret_cast<const uint8*>(Utf8.Get()), (uint32)Utf8.Length(),  false);
}

void UAnomalyControlServerSubsystem::SendAck(INetworkingWebSocket* Socket, const FString& ReType) const
{
	const TSharedRef<FJsonObject> Ack = MakeShared<FJsonObject>();
	Ack->SetStringField(TEXT("type"), TEXT("ack"));
	Ack->SetStringField(TEXT("re"), ReType);
	SendJson(Socket, Ack);
}

void UAnomalyControlServerSubsystem::PushSnapshots()
{
	bool bAny = false;
	for (const FControlConn& C : Conns)
	{
		if (C.bAuthed && C.bSubSnapshot)
		{
			bAny = true;
			break;
		}
	}
	if (!bAny)
	{
		return;
	}

	UWorld* World = GetWorld();

	int32 VX = 0, VY = 0;
	if (UGameViewportClient* GV = World ? World->GetGameViewport() : nullptr)
	{
		FVector2D Size = FVector2D::ZeroVector;
		GV->GetViewportSize(Size);
		VX = (int32)Size.X;
		VY = (int32)Size.Y;
	}
	if (VX != LastViewportX || VY != LastViewportY)
	{
		++ViewEpoch;
		LastViewportX = VX;
		LastViewportY = VY;
	}

	const FString Json = ControlSnapshot::BuildSnapshotJson(World, ViewEpoch);
	for (const FControlConn& C : Conns)
	{
		if (C.bAuthed && C.bSubSnapshot && C.Socket)
		{
			SendRawText(C.Socket, Json);
		}
	}
}

void UAnomalyControlServerSubsystem::PushFrames(bool bForce)
{
	if (UWorld* CapWorld = GetWorld())
	{
		if (UAnomalyCaptureSubsystem* Cap = CapWorld->GetSubsystem<UAnomalyCaptureSubsystem>())
		{
			if (Cap->IsCaptureActive())
			{
				return;
			}
		}
	}

	bool bAny = false;
	for (const FControlConn& C : Conns)
	{
		if (C.bAuthed && (C.bSubFrames || bForce))
		{
			bAny = true;
			break;
		}
	}
	if (!bAny)
	{
		return;
	}

	TArray<uint8> Jpeg;
	int32 W = 0, H = 0;
	if (!AnomalyPreview::CaptureGameViewportJpeg(GetWorld(), Jpeg, W, H,  60))
	{
		return;
	}

	TArray<uint8> Frame = ControlProtocol::BuildFrameHeader(++FrameCounter, ViewEpoch, (uint16)FMath::Clamp(W, 0, 65535), (uint16)FMath::Clamp(H, 0, 65535));
	Frame.Append(Jpeg);

	for (const FControlConn& C : Conns)
	{
		if (C.bAuthed && (C.bSubFrames || bForce) && C.Socket)
		{
			C.Socket->Send(Frame.GetData(), (uint32)Frame.Num(),  false);
		}
	}
}

#endif
