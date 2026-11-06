#include "AnomalyControlServerSubsystem.h"
#include "Compat/AnomalyControlTransportState.h"
#if __has_include("Stats/LightweightStats.h")
#include "Stats/LightweightStats.h"
#endif
#include "Stats/Stats.h"
#if __has_include("Stats/StatsSystemTypes.h")
#include "Stats/StatsSystemTypes.h"
#endif
#include "Subsystems/SubsystemCollection.h"

#include "AnomalyControlServerLog.h"
#include "AnomalyBenchGate.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "Misc/Guid.h"
#include "HAL/PlatformTime.h"
#include "HAL/IConsoleManager.h"
#include "Containers/StringConv.h"
#include "Misc/ConfigCacheIni.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "CoreGlobals.h"
#include "Misc/Paths.h"
#include "Misc/DateTime.h"
#include "HAL/FileManager.h"

#if ANOMALY_CONTROL_SERVER
#include "AnomalyPreviewCapture.h"
#include "ControlProtocol.h"
#include "ControlSnapshot.h"
#include "Modules/ModuleManager.h"
#include "AnomalyInjectorSubsystem.h"
#include "AnomalySelectorSubsystem.h"
#include "AnomalyAutoInjectorSubsystem.h"
#include "AnomalyViewport.h"
#include "AnomalyCaptureSubsystem.h"
#include "Modules/ModuleManager.h"
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


UAnomalyControlServerSubsystem::UAnomalyControlServerSubsystem()
    : Transport(MakeUnique<FAnomalyControlTransportState>())
{
}

UAnomalyControlServerSubsystem::UAnomalyControlServerSubsystem(FVTableHelper& Helper)
    : Super(Helper), Transport(MakeUnique<FAnomalyControlTransportState>())
{
}

UAnomalyControlServerSubsystem::~UAnomalyControlServerSubsystem() = default;


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
		UE_LOG(LogAnomalyServer, Warning, TEXT("Control: already listening on port %d."), Transport->Port);
		return true;
	}

	Transport->Server = MakeAnomalyControlTransport();
	if (!Transport->Server) { return false; }

	Transport->Port = (InPort > 0) ? InPort : DefaultControlPort;

	FString ConfigToken;
	const bool bHasConfigToken = GConfig
		&& GConfig->GetString(TEXT("AnomalyControlServer"), TEXT("Token"), ConfigToken, GGameIni)
		&& !ConfigToken.IsEmpty();
	Transport->Token = bHasConfigToken ? ConfigToken : FGuid::NewGuid().ToString(EGuidFormats::Digits);

    FAnomalyControlCallbacks Callbacks;
    Callbacks.Open=[this](uint64 Id,const FString& Remote) { OnClientConnected(Id,Remote); };
    Callbacks.Message=[this](uint64 Id,const TArray<uint8>& Bytes) { OnReceive(Id,Bytes.GetData(),Bytes.Num()); };
    Callbacks.Close=[this](uint64 Id) { OnSocketClosed(Id); };
    if (!Transport->Server->Start(Transport->Port,MoveTemp(Callbacks)))
    {
        UE_LOG(LogAnomalyServer,Error,TEXT("Control: server Init failed on port %d."),Transport->Port);
        Transport->Server.Reset(); return false;
    }

	bListening = true;
	Transport->LastSnapshotTime = 0.0;
	Transport->LastFrameTime = 0.0;
	Transport->FrameCounter = 0;
	Transport->ViewEpoch = 0;
	Transport->LastViewportX = 0;
	Transport->LastViewportY = 0;
	Transport->bWantOneFrame = false;

	UE_LOG(LogAnomalyServer, Log, TEXT("=== Anomaly Control Server LISTENING on ws://127.0.0.1:%d ==="), Transport->Port);
	UE_LOG(LogAnomalyServer, Log, TEXT("=== Control server token: %s (%s) ==="), *Transport->Token,
		bHasConfigToken ? TEXT("from DefaultGame.ini [AnomalyControlServer] Token") : TEXT("random per-session"));
	if (GLog) { GLog->FlushThreadedLogs(); GLog->Flush(); }
	return true;
#else
	UE_LOG(LogAnomalyServer, Warning, TEXT("Control: server compiled out (ANOMALY_CONTROL_SERVER=0)."));
	return false;
#endif
}

void UAnomalyControlServerSubsystem::StopListening()
{
#if ANOMALY_CONTROL_SERVER
	if (Transport->Server) { Transport->Server->Stop(); }
	Transport->Conns.Reset();
	Transport->Server.Reset();
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
	for (const FControlConn& C : Transport->Conns)
	{
		if (C.bAuthed)
		{
			++Authed;
		}
	}
	UE_LOG(LogAnomalyServer, Log, TEXT("Control: listening ws://127.0.0.1:%d | token=%s | %d connection(s), %d authed | snapHz~%.1f frameHz~%.1f"),
		Transport->Port, *Transport->Token, Transport->Conns.Num(), Authed,
		Transport->SnapshotIntervalSec > 0 ? 1.0 / Transport->SnapshotIntervalSec : 0.0,
		Transport->FrameIntervalSec > 0 ? 1.0 / Transport->FrameIntervalSec : 0.0);
#else
	UE_LOG(LogAnomalyServer, Log, TEXT("Control: compiled out (ANOMALY_CONTROL_SERVER=0)."));
#endif
}

void UAnomalyControlServerSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
#if ANOMALY_CONTROL_SERVER
	if (!bListening || !Transport->Server.IsValid())
	{
		return;
	}

#if ANOMALY_ENGINE_425
    for (FControlConn& Conn:Transport->Conns)
    {
        if (!Conn.bPendingCaptureStop) { continue; }
        auto* World=GetWorld();
        auto* Cap=World ? World->GetSubsystem<UAnomalyCaptureSubsystem>() : nullptr;
        bool Running=false; int32 Frames=0,Seed=0; FString Directory;
        if (Cap) { Cap->GetStatus(Running,Frames,Directory,Seed); }
        if (Running) { continue; }
        Conn.bPendingCaptureStop=false;
        if (!Cap || Cap->GetSessionId()!=Conn.PendingStopSession) { continue; }
        const TSharedRef<FJsonObject> Reply=MakeShared<FJsonObject>();
        Reply->SetStringField(TEXT("type"),TEXT("capture_stopped"));
        Reply->SetBoolField(TEXT("running"),false);
        Reply->SetStringField(TEXT("runDir"),Directory);
        Reply->SetStringField(TEXT("sessionId"),Cap->GetSessionId());
        Reply->SetNumberField(TEXT("frames"),Frames);
        Reply->SetNumberField(TEXT("maxFrames"),Cap->GetFrameCap());
        Reply->SetNumberField(TEXT("seed"),Seed);
        const auto Pacing=Cap->GetLastRunPacing();
        if (Pacing.bValid)
        {
            Reply->SetNumberField(TEXT("targetFps"),Pacing.TargetFps);
            Reply->SetNumberField(TEXT("stampedFps"),Pacing.StampedFps);
            Reply->SetNumberField(TEXT("speedRatio"),Pacing.SpeedRatio);
            Reply->SetBoolField(TEXT("paced"),Pacing.bPaced);
        }
        SendJson(Conn.Id,Reply);
    }
#endif

	Transport->Server->Tick();

	if (UWorld* PumpWorld = GetWorld())
	{
		if (UAnomalyCaptureSubsystem* Cap = PumpWorld->GetSubsystem<UAnomalyCaptureSubsystem>())
		{
			Cap->PreviewPump();
		}
	}

	const double Now = FPlatformTime::Seconds();
	for (FControlConn& C : Transport->Conns)
	{
		if (!C.bAuthed && !C.bRejected && (Now - C.ConnectTime) > AuthTimeoutSeconds)
		{
			C.bRejected = true;
			UE_LOG(LogAnomalyServer, Warning, TEXT("Control: auth timeout — ignoring an un-authenticated peer."));
		}
	}

	if (Now - Transport->LastSnapshotTime >= Transport->SnapshotIntervalSec)
	{
		Transport->LastSnapshotTime = Now;
		PushSnapshots();
	}
	if (Transport->bWantOneFrame || (Now - Transport->LastFrameTime >= Transport->FrameIntervalSec))
	{
		Transport->LastFrameTime = Now;
		PushFrames(Transport->bWantOneFrame);
		Transport->bWantOneFrame = false;
	}
#endif
}

#if ANOMALY_CONTROL_SERVER

FControlConn* UAnomalyControlServerSubsystem::FindConn(uint64 Id)
{
	return Transport->Conns.FindByPredicate([Id](const FControlConn& C) { return C.Id == Id; });
}

bool UAnomalyControlServerSubsystem::IsLoopbackAddr(const FString& Addr)
{
	return Addr.StartsWith(TEXT("127."))
		|| Addr == TEXT("::1")
		|| Addr == TEXT("0:0:0:0:0:0:0:1")
		|| Addr.StartsWith(TEXT("::ffff:127."))
		|| Addr.Equals(TEXT("localhost"), ESearchCase::IgnoreCase);
}

void UAnomalyControlServerSubsystem::OnClientConnected(uint64 Id,const FString& Remote)
{
	if (!Id)
	{
		return;
	}


	FControlConn Conn;
	Conn.Id = Id;
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

	Transport->Conns.Add(Conn);

}

void UAnomalyControlServerSubsystem::OnSocketClosed(uint64 Id)
{
	const int32 Removed = Transport->Conns.RemoveAll([Id](const FControlConn& C) { return C.Id == Id; });
	if (Removed > 0)
	{
		UE_LOG(LogAnomalyServer, Log, TEXT("Control: connection closed; %d remain."), Transport->Conns.Num());
	}
}

void UAnomalyControlServerSubsystem::OnReceive(uint64 Id, const void* Data, int32 Size)
{
	if (!Data || Size <= 0)
	{
		return;
	}
	FControlConn* Conn = FindConn(Id);
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
		if (Tok == Transport->Token)
		{
			Conn.bAuthed = true;
			const TSharedRef<FJsonObject> Welcome = MakeShared<FJsonObject>();
			Welcome->SetStringField(TEXT("type"), TEXT("welcome"));
			Welcome->SetStringField(TEXT("server"), TEXT("AnomalyControlServer/slice1"));
			Welcome->SetNumberField(TEXT("v"), ControlProtocol::Version);
			SendJson(Conn.Id, Welcome);
			UE_LOG(LogAnomalyServer, Log, TEXT("Control: client authenticated."));
		}
		else
		{
			const TSharedRef<FJsonObject> Err = MakeShared<FJsonObject>();
			Err->SetStringField(TEXT("type"), TEXT("error"));
			Err->SetStringField(TEXT("code"), TEXT("bad_token"));
			Err->SetStringField(TEXT("message"), TEXT("token rejected"));
			SendJson(Conn.Id, Err);
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
#if ANOMALY_ENGINE_425
    if (const auto* Capture=World ? World->GetSubsystem<UAnomalyCaptureSubsystem>() : nullptr)
    {
        const bool ReadOnly=Type==TEXT("list_anomalies") || Type==TEXT("subscribe") || Type==TEXT("request_frame") ||
            Type==TEXT("capture_status") || Type==TEXT("capture_stop");
        if (Capture->IsCaptureActive() && !ReadOnly)
        {
            auto Reply=MakeShared<FJsonObject>(); Reply->SetStringField(TEXT("type"),TEXT("error"));
            Reply->SetStringField(TEXT("code"),TEXT("capture_active")); Reply->SetStringField(TEXT("message"),TEXT("Capture transaction is active"));
            SendJson(Conn.Id,Reply); return;
        }
    }
#endif

#if !UE_BUILD_SHIPPING
	if (Type == TEXT("bench_place_view") || Type == TEXT("bench_input_lock") || Type == TEXT("bench_log_barrier") || Type == TEXT("bench_scene_fixture"))
	{
		auto Reply = MakeShared<FJsonObject>();
		Reply->SetStringField(TEXT("type"), Type);
		const auto* Cap = World ? World->GetSubsystem<UAnomalyCaptureSubsystem>() : nullptr;
		const bool Allowed = FParse::Param(FCommandLine::Get(), TEXT("IAIBenchFixture")) && World &&
			(World->GetMapName().Contains(TEXT("L_ShooterGym")) || World->GetMapName().Contains(TEXT("CB_GateLevel"))) && Cap && !Cap->IsCaptureActive();
		bool Executed = false;
		double Request = 0; Msg->TryGetNumberField(TEXT("request"), Request);
		if (Allowed)
		{
			if (Type != TEXT("bench_log_barrier"))
			{
				FModuleManager::Get().LoadModulePtr<IModuleInterface>(TEXT("AnomalyBench"));
			}
			if (Type == TEXT("bench_input_lock"))
			{
				bool Enabled = false;
				if (Msg->TryGetBoolField(TEXT("enabled"), Enabled))
				{
					if (IConsoleCommand* Command = AnomalyBenchGate::FindLeverCommand(TEXT("IAI.Bench.InputLock")))
					{
						Command->Execute({Enabled ? TEXT("1") : TEXT("0")}, World, *GLog); Executed = true;
					}
				}
			}
			else if (Type == TEXT("bench_place_view"))
			{
				if (IConsoleCommand* Command = AnomalyBenchGate::FindLeverCommand(TEXT("IAI.Bench.PlaceView")))
				{
					Command->Execute({FString::FromInt((int32)FMath::Clamp(Request, 0.0, 1000000000.0))}, World, *GLog);
					Executed = true;
				}
			}
			else if (Type == TEXT("bench_scene_fixture"))
			{
				FString Mode; Msg->TryGetStringField(TEXT("mode"), Mode);
				if (IConsoleCommand* Command = AnomalyBenchGate::FindLeverCommand(TEXT("IAI.Bench.SceneFixture")))
				{
					Command->Execute({Mode}, World, *GLog); Executed = true;
				}
			}
			else { Executed = true; }
			GLog->FlushThreadedLogs(); GLog->Flush();
		}
		Reply->SetBoolField(TEXT("executed"), Executed);
		SendJson(Conn.Id, Reply); return;
	}
#endif

	if (Type == TEXT("list_anomalies"))
	{
		SendRawText(Conn.Id, ControlSnapshot::BuildCatalogJson(World));
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
			Transport->SnapshotIntervalSec = 1.0 / FMath::Clamp(Hz, 1.0, 20.0);
		}
		if (Msg->TryGetNumberField(TEXT("frameHz"), Hz) && Hz > 0.0)
		{
			Transport->FrameIntervalSec = 1.0 / FMath::Clamp(Hz, 0.5, 10.0);
		}
		SendAck(Conn.Id, TEXT("subscribe"));
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
		SendJson(Conn.Id, Ack);
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
		SendAck(Conn.Id, TEXT("revert"));
		return;
	}

	if (Type == TEXT("revert_all"))
	{
		if (UAnomalyInjectorSubsystem* Inj = World ? World->GetSubsystem<UAnomalyInjectorSubsystem>() : nullptr)
		{
			Inj->RevertAllActive();
		}
		SendAck(Conn.Id, TEXT("revert_all"));
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
		SendAck(Conn.Id, TEXT("set_viewport_scoping"));
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
		SendAck(Conn.Id, TEXT("set_hud"));
		return;
	}

	if (Type == TEXT("request_frame"))
	{
		Transport->bWantOneFrame = true;
		return;
	}

	if (Type == TEXT("set_poll_radius"))
	{
		double Cm = 0.0;
		Msg->TryGetNumberField(TEXT("cm"), Cm);
		AnomalyViewport::SetPollRadius((float)Cm);
		SendAck(Conn.Id, TEXT("set_poll_radius"));
		return;
	}

	if (Type == TEXT("set_min_screen_coverage"))
	{
		double Pct = 0.0;
		Msg->TryGetNumberField(TEXT("pct"), Pct);
		AnomalyViewport::SetMinScreenCoveragePct((float)Pct);
		SendAck(Conn.Id, TEXT("set_min_screen_coverage"));
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
		SendAck(Conn.Id, TEXT("auto_config"));
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
		SendAck(Conn.Id, TEXT("auto_run"));
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
		SendAck(Conn.Id, TEXT("auto_step"));
		return;
	}

	if (Type == TEXT("auto_fire_once"))
	{
		if (UAnomalyAutoInjectorSubsystem* Auto = World ? World->GetSubsystem<UAnomalyAutoInjectorSubsystem>() : nullptr)
		{
			Auto->TryFireOnce();
		}
		SendAck(Conn.Id, TEXT("auto_fire_once"));
		return;
	}

	if (Type == TEXT("capture_start"))
	{
#if ANOMALY_ENGINE_425
        FString Directory,Format,Anomaly,Target,Error;
        Msg->TryGetStringField(TEXT("dir"),Directory); Msg->TryGetStringField(TEXT("format"),Format);
        Msg->TryGetStringField(TEXT("anomaly"),Anomaly); Msg->TryGetStringField(TEXT("target"),Target);
        double Seed=0,MaxFrames=0,Height=0;
        const bool HasSeed=Msg->HasTypedField<EJson::Number>(TEXT("seed")) && Msg->TryGetNumberField(TEXT("seed"),Seed);
        const bool HasFrames=Msg->HasTypedField<EJson::Number>(TEXT("maxFrames")) && Msg->TryGetNumberField(TEXT("maxFrames"),MaxFrames);
        Msg->TryGetNumberField(TEXT("outputHeight"),Height);
        auto* Capture=World ? World->GetSubsystem<UAnomalyCaptureSubsystem>() : nullptr;
        auto* Auto=World ? World->GetSubsystem<UAnomalyAutoInjectorSubsystem>() : nullptr;
        if (!HasSeed && Auto) { Seed=Auto->GetSeed(); }
        TArray<FString> Routes=Auto ? Auto->GetEnabledIds() : TArray<FString>();
        if (!Capture || !Auto) { Error=TEXT("Capture subsystem unavailable"); }
        else if ((!Format.IsEmpty() && !Format.Equals(TEXT("png"),ESearchCase::IgnoreCase)) ||
            !Anomaly.IsEmpty() || !Target.IsEmpty() || Height!=0 || Msg->HasField(TEXT("targetArgs")) ||
            Msg->HasField(TEXT("outputWidth"))) { Error=TEXT("4.25 requires native PNG auto capture"); }
        else if (!FMath::IsFinite(Seed) || Seed<MIN_int32 || Seed>MAX_int32 || Seed!=double(int32(Seed))) { Error=TEXT("Invalid seed"); }
        else if (!FMath::IsFinite(MaxFrames) || MaxFrames<0 || MaxFrames>MAX_int32 || MaxFrames!=double(int32(MaxFrames)) ||
            (Msg->HasField(TEXT("maxFrames")) && !HasFrames) ||
            (Msg->HasField(TEXT("seed")) && !HasSeed)) { Error=TEXT("Invalid capture frame count or seed type"); }
        else if (Directory.IsEmpty() || FPaths::IsRelative(Directory)) { Error=TEXT("Absolute capture base directory required"); }
        else
        {
            const FString Session=Directory/FString::Printf(TEXT("session_%s_%lld"),*FDateTime::Now().ToString(TEXT("%Y%m%d_%H%M%S")),FDateTime::UtcNow().GetTicks());
            Capture->StartFreePlayRun(Session,int32(Seed),int32(MaxFrames),Routes,Error);
        }
        if (!Error.IsEmpty())
        {
            auto Reply=MakeShared<FJsonObject>(); Reply->SetStringField(TEXT("type"),TEXT("error"));
            Reply->SetStringField(TEXT("code"),TEXT("capture_refused")); Reply->SetStringField(TEXT("message"),Error);
            SendJson(Conn.Id,Reply); return;
        }
        SendAck(Conn.Id,TEXT("capture_start")); return;
#else
		FString Dir, Format, Anomaly, TargetActor;
		Msg->TryGetStringField(TEXT("dir"), Dir);
		Msg->TryGetStringField(TEXT("format"), Format);
		Msg->TryGetStringField(TEXT("anomaly"), Anomaly);
		Msg->TryGetStringField(TEXT("target"), TargetActor);
		double SeedV = -1.0;
		Msg->TryGetNumberField(TEXT("seed"), SeedV);
		double MaxFramesV = 0.0;
		Msg->TryGetNumberField(TEXT("maxFrames"), MaxFramesV);
		double OutputHeightV = -1.0;
		const bool bHaveOutputHeight = Msg->TryGetNumberField(TEXT("outputHeight"), OutputHeightV);
		const int32 OutputHeight = bHaveOutputHeight ? FMath::Max(0, (int32)OutputHeightV) : -1;

		bool bPng = true;
		if (Format.IsEmpty() || Format.Equals(TEXT("png"), ESearchCase::IgnoreCase))
		{
			bPng = true;
		}
		else if (Format.Equals(TEXT("jpeg"), ESearchCase::IgnoreCase) || Format.Equals(TEXT("jpg"), ESearchCase::IgnoreCase))
		{
			bPng = false;
		}
		else
		{
			bPng = true;
			UE_LOG(LogAnomalyServer, Warning,
				TEXT("capture_start: UNRECOGNISED format '%s' - expected 'png', 'jpeg' or 'jpg'. FALLING BACK TO ")
				TEXT("PNG for this run. The written frames will be .png regardless of what the caller intended, ")
				TEXT("and run.json will record \"png\". Before m28 this fallback was SILENT."),
				*Format);
		}

		if (UAnomalyCaptureSubsystem* Cap = World ? World->GetSubsystem<UAnomalyCaptureSubsystem>() : nullptr)
		{
#if !UE_BUILD_SHIPPING
			if (FParse::Param(FCommandLine::Get(), TEXT("IAIBenchFixture")))
			{
				UE_LOG(LogAnomalyServer, Log, TEXT("IAI-L2 CAPTURE-START gfc=%llu"), GFrameCounter);
				GLog->FlushThreadedLogs(); GLog->Flush();
			}
#endif
			TArray<FString> TargetArgs;
#if !UE_BUILD_SHIPPING
			double BenchDelay = 0;
			if (Msg->TryGetNumberField(TEXT("benchDelayFrames"), BenchDelay) && BenchDelay == 3 &&
				Anomaly == TEXT("solid_swap") && FParse::Param(FCommandLine::Get(), TEXT("IAIBenchFixture")) &&
				(World->GetMapName().Contains(TEXT("CB_GateLevel")) || World->GetMapName().Contains(TEXT("L_ShooterGym"))))
			{
				TargetArgs.Add(TEXT("delay=3"));
			}
#endif
			Cap->StartRun(Dir, bPng, (int32)SeedV, (int32)MaxFramesV, Anomaly, TargetActor, TargetArgs, OutputHeight);
		}
		SendAck(Conn.Id, TEXT("capture_start"));
		return;
#endif
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

#if ANOMALY_ENGINE_425
        if (bRun)
        {
            Conn.bPendingCaptureStop=true;
            Conn.PendingStopSession=SessionId;
            return;
        }
#endif
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
		SendJson(Conn.Id, Reply);
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
		SendJson(Conn.Id, Reply);
		return;
	}

	SendAck(Conn.Id, Type);
}

void UAnomalyControlServerSubsystem::SendJson(uint64 Id, const TSharedRef<FJsonObject>& Obj) const
{
	FString Out;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Out);
	FJsonSerializer::Serialize(Obj, Writer);
	SendRawText(Id, Out);
}

void UAnomalyControlServerSubsystem::SendRawText(uint64 Id, const FString& Text) const
{
	if (!Id)
	{
		return;
	}
	FTCHARToUTF8 Utf8(*Text);
	Transport->Server->Send(Id,reinterpret_cast<const uint8*>(Utf8.Get()),Utf8.Length());
}

void UAnomalyControlServerSubsystem::SendAck(uint64 Id, const FString& ReType) const
{
	const TSharedRef<FJsonObject> Ack = MakeShared<FJsonObject>();
	Ack->SetStringField(TEXT("type"), TEXT("ack"));
	Ack->SetStringField(TEXT("re"), ReType);
	SendJson(Id, Ack);
}

void UAnomalyControlServerSubsystem::PushSnapshots()
{
	bool bAny = false;
	for (const FControlConn& C : Transport->Conns)
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
	if (VX != Transport->LastViewportX || VY != Transport->LastViewportY)
	{
		++Transport->ViewEpoch;
		Transport->LastViewportX = VX;
		Transport->LastViewportY = VY;
	}

	const FString Json = ControlSnapshot::BuildSnapshotJson(World, Transport->ViewEpoch);
	for (const FControlConn& C : Transport->Conns)
	{
		if (C.bAuthed && C.bSubSnapshot && C.Id)
		{
			SendRawText(C.Id, Json);
		}
	}
}

void UAnomalyControlServerSubsystem::PushFrames(bool bForce)
{
	UAnomalyCaptureSubsystem* Cap = nullptr;
	if (UWorld* CapWorld = GetWorld())
	{
		Cap = CapWorld->GetSubsystem<UAnomalyCaptureSubsystem>();
	}
	if (!Cap)
	{
		return;
	}
	if (Cap->IsCaptureActive())
	{
		return;
	}

	bool bAny = false;
	for (const FControlConn& C : Transport->Conns)
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
	uint32 FrameEpoch = 0;
	if (Cap->PreviewPoll(Jpeg, W, H, FrameEpoch))
	{
		TArray<uint8> Frame = ControlProtocol::BuildFrameHeader(++Transport->FrameCounter, FrameEpoch, (uint16)FMath::Clamp(W, 0, 65535), (uint16)FMath::Clamp(H, 0, 65535));
		Frame.Append(Jpeg);

		for (const FControlConn& C : Transport->Conns)
		{
			if (C.bAuthed && (C.bSubFrames || bForce) && C.Id)
			{
				Transport->Server->Send(C.Id,Frame.GetData(),Frame.Num());
			}
		}
	}

	Cap->PreviewArm(Transport->ViewEpoch);
}

#endif
