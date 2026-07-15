#include "AnomalyCaptureSubsystem.h"

#include "AnomalyCaptureLog.h"
#include "Engine/World.h"
#include "Engine/GameViewportClient.h"
#include "UnrealClient.h"
#include "HAL/IConsoleManager.h"
#include "Misc/Paths.h"
#include "Misc/DateTime.h"
#include "Misc/ConfigCacheIni.h"
#include "CoreGlobals.h"

#if ANOMALY_CAPTURE
#include "AnomalyLabelWriter.h"
#include "AnomalyPreviewCapture.h"
#include "AnomalyAutoInjectorSubsystem.h"
#include "AnomalyInjectorSubsystem.h"
#include "AnomalyFrameCapturer.h"
#include "AnomalyAsyncWriter.h"
#include "HAL/PlatformProcess.h"
#include "HAL/PlatformTime.h"
#include "RenderingThread.h"
#include "Modules/ModuleManager.h"

#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Misc/EngineVersion.h"
#include "Misc/App.h"

#include "Framework/Application/SlateApplication.h"
#include "Widgets/SWindow.h"
#include "Widgets/SViewport.h"
#include "Layout/WidgetPath.h"
#include "Layout/ArrangedWidget.h"
#include "Layout/ArrangedChildren.h"

#if WITH_EDITOR
#include "Settings/LevelEditorPlaySettings.h"
#endif
#endif

#if ANOMALY_CAPTURE
struct FSessionEventAccum
{
	FName Id = NAME_None;
	FString Target;
	uint64 StartFrame = 0;

	TArray<int32> AffectedFrames;
	double CoverageSum = 0.0;
	int32 CoverageCount = 0;

	int32 AnchorIndex = MAX_int32;
	FVector CamPos = FVector::ZeroVector;
	FRotator CamRot = FRotator::ZeroRotator;
	float CamFov = 0.0f;
	float CamAspect = 0.0f;
	float CamNear = 0.0f;
	FString CamPath;
	int64 TicksMsec = 0;
	FString NodeName;
	FString NodePath;
	FVector NodePos = FVector::ZeroVector;

	int32 VisibleFrames = 0;
	int32 HiddenFrames = 0;
	int32 Transitions = 0;
	int32 LastHidden = -1;
	TArray<int32> HiddenIndices;
};
#endif

struct FAnomalyCaptureAsyncState
{
#if ANOMALY_CAPTURE
	TSharedPtr<FAnomalyFrameCapturer, ESPMode::ThreadSafe> Capturer;
	TSharedPtr<FAnomalyAsyncWriter, ESPMode::ThreadSafe> Writer;
	TMap<uint64, AnomalyLabel::FCaptureSnapshot> PendingSnapshots;
	TArray<FSessionEventAccum> SessionEvents;
#endif
};

#if ANOMALY_CAPTURE
namespace
{
	constexpr float GAnomalyDefaultFarPlane = 1000000.0f;

	constexpr double GFpsStampTolerance = 0.02;
	constexpr double GPaceCoarseSleepMarginSec = 0.0015;
	constexpr int32 GEarlyPacingWarnMinFrames = 30;

	FString ResolveCameraPath(UWorld* World)
	{
		if (APlayerController* PC = World ? World->GetFirstPlayerController() : nullptr)
		{
			if (AActor* VT = PC->GetViewTarget())
			{
				return VT->GetPathName();
			}
			if (PC->PlayerCameraManager)
			{
				return PC->PlayerCameraManager->GetPathName();
			}
		}
		return FString();
	}

	void MapAnomalyToClient(FName Id, int32 Transitions, FString& OutType, FString& OutSubtype)
	{
		if (Id == FName(TEXT("blinking")))
		{
			OutType = TEXT("blink");
			OutSubtype = (Transitions <= 2) ? TEXT("disappear_reappear") : TEXT("flicker");
		}
		else
		{
			OutType = Id.ToString();
			OutSubtype = FString();
		}
	}
}
#endif


UAnomalyCaptureSubsystem::UAnomalyCaptureSubsystem()
{
	Async = MakeUnique<FAnomalyCaptureAsyncState>();
}

UAnomalyCaptureSubsystem::~UAnomalyCaptureSubsystem() = default;

bool UAnomalyCaptureSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

TStatId UAnomalyCaptureSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UAnomalyCaptureSubsystem, STATGROUP_Tickables);
}

void UAnomalyCaptureSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
#if ANOMALY_CAPTURE
	bool bConfigDelivery = false;
	if (GConfig && GConfig->GetBool(TEXT("AnomalyCapture"), TEXT("bDeliveryModeDefault"), bConfigDelivery, GGameIni))
	{
		bDeliveryMode = bConfigDelivery;
	}
	FString ConfigClock;
	if (GConfig && GConfig->GetString(TEXT("AnomalyCapture"), TEXT("ContentClockDefault"), ConfigClock, GGameIni))
	{
		if (ConfigClock.Equals(TEXT("game"), ESearchCase::IgnoreCase)) { ContentClock = EContentClock::Game; }
		else if (ConfigClock.Equals(TEXT("wall"), ESearchCase::IgnoreCase)) { ContentClock = EContentClock::Wall; }
	}
	bool bConfigFocusGate = false;
	if (GConfig && GConfig->GetBool(TEXT("AnomalyCapture"), TEXT("bFocusGateDefault"), bConfigFocusGate, GGameIni))
	{
		bFocusGate = bConfigFocusGate;
	}
	UE_LOG(LogAnomalyCapture, Log, TEXT("AnomalyCapture subsystem initialized (idle — use IAI.Capture.Start). Delivery mode: %s. Content clock: %s. Focus gate: %s."),
		bDeliveryMode ? TEXT("ON (client-facing output only)") : TEXT("off (full fidelity)"),
		ContentClock == EContentClock::Game ? TEXT("game (stamp target fps)") : TEXT("wall (stamp sustained on slow runs)"),
		bFocusGate ? TEXT("on (start waits for game-window focus)") : TEXT("off (start begins immediately)"));
#if WITH_EDITOR
	if (ULevelEditorPlaySettings* PlaySettings = GetMutableDefault<ULevelEditorPlaySettings>())
	{
		bSavedShowMouseControlLabel = PlaySettings->ShowMouseControlLabel;
		PlaySettings->ShowMouseControlLabel = false;
		bMouseLabelOverridden = true;
	}
#endif
#else
	UE_LOG(LogAnomalyCapture, Log, TEXT("AnomalyCapture subsystem initialized (compiled out: ANOMALY_CAPTURE=0)."));
#endif
}

void UAnomalyCaptureSubsystem::Deinitialize()
{
	bDeinitializing = true;
	StopRun();
#if ANOMALY_CAPTURE
	if (Async.IsValid())
	{
		Async->PendingSnapshots.Empty();
		if (Async->Capturer.IsValid())
		{
			Async->Capturer->UnregisterBackbufferHook();
			Async->Capturer.Reset();
		}
		if (Async->Writer.IsValid())
		{
			Async->Writer->FlushPending(2.0);
			Async->Writer.Reset();
		}
	}
#if WITH_EDITOR
	if (bMouseLabelOverridden)
	{
		if (ULevelEditorPlaySettings* PlaySettings = GetMutableDefault<ULevelEditorPlaySettings>())
		{
			PlaySettings->ShowMouseControlLabel = bSavedShowMouseControlLabel;
		}
		bMouseLabelOverridden = false;
	}
#endif
#endif
	Super::Deinitialize();
}

void UAnomalyCaptureSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
#if ANOMALY_CAPTURE
	if (!bRunning)
	{
		return;
	}

	if (Phase == ECapturePhase::ArmedPending)
	{
		UWorld* World = GetWorld();
		if (!HasGameWindow(World) || IsGameWindowFocused(World))
		{
			BeginActualRun();
			return;
		}
		const double NowWall = FPlatformTime::Seconds();
		if (NowWall - LastArmWaitLogWall >= 2.0)
		{
			LastArmWaitLogWall = NowWall;
			UE_LOG(LogAnomalyCapture, Log, TEXT("Capture armed — still waiting for game-window focus (%.0fs)."), NowWall - ArmWaitStartWall);
		}
		if (FocusWaitTimeoutSeconds > 0.0 && NowWall - ArmWaitStartWall >= FocusWaitTimeoutSeconds)
		{
			UE_LOG(LogAnomalyCapture, Warning, TEXT("Capture armed — game-window focus not acquired after %.0fs; starting anyway (IAI.Capture.Stop to cancel)."), FocusWaitTimeoutSeconds);
			BeginActualRun();
		}
		return;
	}

	PaceThisTick();

	SampleViewThisTick();

	if (bAsyncCapture)
	{
		ProcessCompletedFrames();
	}

	if (FrameCap > 0 && SessionFrameIndex >= FrameCap
		&& Phase != ECapturePhase::Idle && Phase != ECapturePhase::DrainTail)
	{
		if (bAsyncCapture)
		{
			Phase = ECapturePhase::DrainTail;
			PhaseFramesLeft = FMath::Max(10, ViewLagFrames + 4);
		}
		else
		{
			FinishRun(true);
			return;
		}
	}

	switch (Phase)
	{
	case ECapturePhase::LeadIn:
		if (PhaseFramesLeft > 0) { CaptureCurrentFrame(); --PhaseFramesLeft; }
		if (PhaseFramesLeft <= 0) { BeginFire(); }
		break;

	case ECapturePhase::SettleAfterFire:
		if (PhaseFramesLeft > 0) { --PhaseFramesLeft; }
		if (PhaseFramesLeft <= 0) { Phase = ECapturePhase::Positives; PhaseFramesLeft = PositiveFrames; }
		break;

	case ECapturePhase::Positives:
		if (PhaseFramesLeft > 0) { CaptureCurrentFrame(); --PhaseFramesLeft; }
		if (PhaseFramesLeft <= 0) { BeginRevert(); }
		break;

	case ECapturePhase::SettleAfterRevert:
		if (PhaseFramesLeft > 0) { --PhaseFramesLeft; }
		if (PhaseFramesLeft <= 0) { Phase = ECapturePhase::PostGap; PhaseFramesLeft = PostFrames; }
		break;

	case ECapturePhase::PostGap:
		if (PhaseFramesLeft > 0) { CaptureCurrentFrame(); --PhaseFramesLeft; }
		if (PhaseFramesLeft <= 0)
		{
			++BurstsDone;
			if (BurstCount > 0 && BurstsDone >= BurstCount)
			{
				if (bAsyncCapture)
				{
					Phase = ECapturePhase::DrainTail;
					PhaseFramesLeft = FMath::Max(10, ViewLagFrames + 4);
				}
				else
				{
					FinishRun(true);
				}
			}
			else
			{
				BeginFire();
			}
		}
		break;

	case ECapturePhase::DrainTail:
		if ((Async.IsValid() && Async->PendingSnapshots.Num() == 0) || PhaseFramesLeft <= 0)
		{
			FinishRun(true);
		}
		else
		{
			--PhaseFramesLeft;
		}
		break;

	default:
		break;
	}

	FinalizeArmedLabel();
#endif
}


void UAnomalyCaptureSubsystem::SetBurstConfig(int32 K, int32 Pre, int32 Positive, int32 Post, int32 Bursts)
{
	if (bRunning)
	{
		UE_LOG(LogAnomalyCapture, Warning, TEXT("IAI.Capture.Config: ignored mid-run (stop first)."));
		return;
	}
	SettleFrames    = FMath::Max(0, K);
	PreFrames       = FMath::Max(0, Pre);
	PositiveFrames  = FMath::Max(0, Positive);
	PostFrames      = FMath::Max(0, Post);
	BurstCount      = FMath::Max(0, Bursts);
	UE_LOG(LogAnomalyCapture, Log, TEXT("IAI.Capture.Config: K=%d pre=%d positive=%d post=%d bursts=%d."),
		SettleFrames, PreFrames, PositiveFrames, PostFrames, BurstCount);
}

void UAnomalyCaptureSubsystem::SetViewLag(int32 L)
{
	if (bRunning)
	{
		UE_LOG(LogAnomalyCapture, Warning, TEXT("IAI.Capture.ViewLag: ignored mid-run (stop first)."));
		return;
	}
	ViewLagFrames = FMath::Max(0, L);
	UE_LOG(LogAnomalyCapture, Log, TEXT("IAI.Capture.ViewLag: L=%d frame(s)."), ViewLagFrames);
}

void UAnomalyCaptureSubsystem::SetAsyncCapture(bool bInAsync)
{
	if (bRunning)
	{
		UE_LOG(LogAnomalyCapture, Warning, TEXT("IAI.Capture.Async: ignored mid-run (stop first)."));
		return;
	}
	bAsyncCapture = bInAsync;
	UE_LOG(LogAnomalyCapture, Log, TEXT("IAI.Capture.Async: %s (%s)."),
		bAsyncCapture ? TEXT("ON") : TEXT("OFF"),
		bAsyncCapture ? TEXT("backbuffer readback, game UI included") : TEXT("synchronous ReadPixels fallback"));
}

void UAnomalyCaptureSubsystem::SetCaptureFps(int32 InFps)
{
	if (bRunning)
	{
		UE_LOG(LogAnomalyCapture, Warning, TEXT("IAI.Capture.Fps: ignored mid-run (stop first)."));
		return;
	}
	VideoFps = FMath::Clamp(InFps, 1, 240);
	UE_LOG(LogAnomalyCapture, Log, TEXT("IAI.Capture.Fps: %d (fixed timestep 1/%d s per frame during runs)."),
		VideoFps, VideoFps);
}

void UAnomalyCaptureSubsystem::SetCapturePace(bool bInPace)
{
	if (bRunning)
	{
		UE_LOG(LogAnomalyCapture, Warning, TEXT("IAI.Capture.Pace: ignored mid-run (stop first)."));
		return;
	}
	bPaceCapture = bInPace;
	UE_LOG(LogAnomalyCapture, Log, TEXT("IAI.Capture.Pace: %s (%s)."),
		bPaceCapture ? TEXT("ON") : TEXT("OFF"),
		bPaceCapture
			? TEXT("each captured frame is held to >= 1/fps of wall time; game == wall == video clock")
			: TEXT("engine free-runs during capture; video.fps stamping stays honest"));
}

void UAnomalyCaptureSubsystem::SetCaptureDelivery(bool bInDelivery)
{
	if (bRunning)
	{
		UE_LOG(LogAnomalyCapture, Warning, TEXT("IAI.Capture.Delivery: ignored mid-run (stop first)."));
		return;
	}
	bDeliveryMode = bInDelivery;
	UE_LOG(LogAnomalyCapture, Log, TEXT("IAI.Capture.Delivery: %s (%s)."),
		bDeliveryMode ? TEXT("ON") : TEXT("OFF"),
		bDeliveryMode
			? TEXT("client-facing output only: Actual_Frames + Video_Clip + run_summary.json + annotation.json (no labels.jsonl, no run.json)")
			: TEXT("full fidelity: all capture artifacts written"));
}

void UAnomalyCaptureSubsystem::SetContentClock(EContentClock InClock)
{
	if (bRunning)
	{
		UE_LOG(LogAnomalyCapture, Warning, TEXT("IAI.Capture.ContentClock: ignored mid-run (stop first)."));
		return;
	}
	ContentClock = InClock;
	UE_LOG(LogAnomalyCapture, Log, TEXT("IAI.Capture.ContentClock: %s (%s)."),
		ContentClock == EContentClock::Game ? TEXT("game") : TEXT("wall"),
		ContentClock == EContentClock::Game
			? TEXT("content advances on the GAME clock (StackOBot etc.): video.fps stamped at TARGET; a slow run is a capture-time perf issue, not a video defect")
			: TEXT("content advances on the WALL clock (sequencer/real-time titles): a slow run stamps the sustained rate so the video plays at true speed"));
}

void UAnomalyCaptureSubsystem::SetFocusGate(bool bInGate)
{
	if (bRunning)
	{
		UE_LOG(LogAnomalyCapture, Warning, TEXT("IAI.Capture.FocusGate: ignored mid-run (stop first)."));
		return;
	}
	bFocusGate = bInGate;
	UE_LOG(LogAnomalyCapture, Log, TEXT("IAI.Capture.FocusGate: %s (%s)."),
		bFocusGate ? TEXT("ON") : TEXT("OFF"),
		bFocusGate
			? TEXT("Start waits for game-window focus before the first frame")
			: TEXT("Start begins immediately regardless of focus"));
}

void UAnomalyCaptureSubsystem::StartRun(const FString& BaseDir, bool bPng, int32 InSeed, int32 InFrameCap,
	const FString& InTargetAnomaly, const FString& InTargetActor)
{
#if ANOMALY_CAPTURE
	if (bRunning)
	{
		UE_LOG(LogAnomalyCapture, Warning, TEXT("IAI.Capture.Start: already running (stop first)."));
		return;
	}

	UWorld* World = GetWorld();
	UAnomalyAutoInjectorSubsystem* Auto = ResolveAuto();
	if (!World || !Auto)
	{
		UE_LOG(LogAnomalyCapture, Warning, TEXT("IAI.Capture.Start: no world / no auto-injector (run inside a Game/PIE world)."));
		return;
	}
	UAnomalyInjectorSubsystem* Injector = World->GetSubsystem<UAnomalyInjectorSubsystem>();

	bAutoWasRunning = Auto->IsRunning();
	if (bAutoWasRunning)
	{
		Auto->SetRunning(false);
		UE_LOG(LogAnomalyCapture, Log, TEXT("IAI.Capture.Start: paused the auto-injector's Run for the capture (will resume on finish)."));
	}

	Seed = (InSeed >= 0) ? InSeed : Auto->GetSeed();
	Auto->SetSeed(Seed);

	Auto->RevertAllLiveFires();
	if (Injector)
	{
		Injector->RevertAllActive();
	}

	const bool bHasAnomaly = !InTargetAnomaly.IsEmpty();
	const bool bHasActor = !InTargetActor.IsEmpty();
	bTargetedMode = bHasAnomaly && bHasActor;
	TargetAnomalyId = bTargetedMode ? FName(*InTargetAnomaly) : NAME_None;
	TargetActorName = bTargetedMode ? InTargetActor : FString();
	if ((bHasAnomaly || bHasActor) && !bTargetedMode)
	{
		UE_LOG(LogAnomalyCapture, Warning,
			TEXT("IAI.Capture.Start: targeted mode needs BOTH an anomaly and a target actor (got only one) — falling back to auto-pool."));
	}

	bFormatPng = bPng;
	FrameCap = FMath::Max(0, InFrameCap);

	const FString Base = BaseDir.IsEmpty()
		? FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("AnomalyCaptures"))
		: BaseDir;
	const FString Stamp = FDateTime::Now().ToString(TEXT("%Y%m%d-%H%M%S"));
	const FString BaseId = FString::Printf(TEXT("session_%s"), *Stamp);
	SessionId = BaseId;
	RunDir = FPaths::Combine(Base, SessionId);
	for (int32 Disamb = 2; IFileManager::Get().DirectoryExists(*RunDir); ++Disamb)
	{
		SessionId = FString::Printf(TEXT("%s-%d"), *BaseId, Disamb);
		RunDir = FPaths::Combine(Base, SessionId);
	}

	IFileManager::Get().MakeDirectory(*FPaths::Combine(RunDir, TEXT("Actual_Frames")), true);

	int32 VW = 0, VH = 0;
	if (UGameViewportClient* GV = World->GetGameViewport())
	{
		FVector2D Size = FVector2D::ZeroVector;
		GV->GetViewportSize(Size);
		VW = (int32)Size.X;
		VH = (int32)Size.Y;
	}
	ViewportW = VW;
	ViewportH = VH;

	const FEngineVersion& EV = FEngineVersion::Current();
	EngineVersion = FString::Printf(TEXT("%u.%u"), (uint32)EV.GetMajor(), (uint32)EV.GetMinor());
	EngineProject = FApp::GetProjectName();

	BurstsDone = 0;
	FramesWritten = 0;
	PositiveFramesWritten = 0;
	ZeroMatchBursts = 0;
	SessionFrameIndex = 0;
	FirstFrameTimeSeconds = -1.0;
	LastFrameTimeSeconds = -1.0;
	FirstArmWallSeconds = -1.0;
	LastArmWallSeconds = -1.0;
	bPaceInitialized = false;
	NextPaceWallTarget = 0.0;
	bEarlyRatioWarned = false;
	LastRunPacing = FLastRunPacing();
	ViewRing.Reset();
	if (Async.IsValid())
	{
		Async->SessionEvents.Reset();
	}

	if (bAsyncCapture)
	{
		EnsureCapturer();
		if (Async.IsValid())
		{
			Async->PendingSnapshots.Empty();
			if (Async->Writer.IsValid())
			{
				Async->Writer->ResetCounters();
			}
		}
	}

	bRunning = true;
	bRunBegun = false;

	if (bFocusGate && HasGameWindow(World) && !IsGameWindowFocused(World))
	{
		Phase = ECapturePhase::ArmedPending;
		ArmWaitStartWall = FPlatformTime::Seconds();
		LastArmWaitLogWall = ArmWaitStartWall;
		UE_LOG(LogAnomalyCapture, Log,
			TEXT("=== Capture ARMED: %s | waiting for game-window focus before the first frame (Start held; focus the game window to begin, or IAI.Capture.Stop to cancel) ==="),
			*RunDir);
	}
	else
	{
		BeginActualRun();
	}
#else
	UE_LOG(LogAnomalyCapture, Warning, TEXT("IAI.Capture.Start: capture compiled out (ANOMALY_CAPTURE=0)."));
#endif
}

void UAnomalyCaptureSubsystem::BeginActualRun()
{
#if ANOMALY_CAPTURE
	StartFrame = GFrameCounter;

	AnomalyLabel::FRunManifest M;
	M.Seed = Seed;
	M.SettleFrames = SettleFrames;
	M.ViewLagFrames = ViewLagFrames;
	M.PreFrames = PreFrames;
	M.PositiveFrames = PositiveFrames;
	M.PostFrames = PostFrames;
	M.BurstCount = BurstCount;
	M.FrameCap = FrameCap;
	M.SessionId = SessionId;
	M.ViewportW = ViewportW;
	M.ViewportH = ViewportH;
	M.Format = bFormatPng ? TEXT("png") : TEXT("jpeg");
	M.StartFrame = StartFrame;
	M.StartTimeUtc = FDateTime::UtcNow().ToIso8601();
	M.Mode = bTargetedMode ? TEXT("targeted") : TEXT("auto_pool");
	M.TargetAnomaly = bTargetedMode ? TargetAnomalyId.ToString() : FString();
	M.TargetActor = bTargetedMode ? TargetActorName : FString();
	M.TargetFps = VideoFps;
	M.bPaced = bPaceCapture;
	if (!bDeliveryMode)
	{
		AnomalyLabel::WriteRunManifest(RunDir, M);
	}

	bSavedUseFixedTimeStep = FApp::UseFixedTimeStep();
	SavedFixedDeltaTime = FApp::GetFixedDeltaTime();
	FApp::SetUseFixedTimeStep(true);
	FApp::SetFixedDeltaTime(1.0 / (double)VideoFps);
	bFixedTimeStepOverridden = true;

	AnomalyViewport::SetOverlaysSuppressed(true);

	Phase = ECapturePhase::LeadIn;
	PhaseFramesLeft = PreFrames;
	bRunBegun = true;

	UE_LOG(LogAnomalyCapture, Log,
		TEXT("=== Capture run STARTED: %s | mode=%s | delivery=%s | clock=%s | seed=%d fmt=%s capture=%s fps=%d(fixed-step%s) | K=%d L=%d pre=%d positive=%d post=%d bursts=%s frameCap=%s ==="),
		*RunDir,
		bTargetedMode ? *FString::Printf(TEXT("targeted[%s on %s]"), *TargetAnomalyId.ToString(), *TargetActorName) : TEXT("auto-pool"),
		bDeliveryMode ? TEXT("on") : TEXT("off"),
		ContentClock == EContentClock::Game ? TEXT("game") : TEXT("wall"),
		Seed, *M.Format, bAsyncCapture ? TEXT("async/backbuffer") : TEXT("sync"), VideoFps,
		bPaceCapture ? TEXT(", paced") : TEXT(", unpaced"),
		SettleFrames, ViewLagFrames, PreFrames, PositiveFrames, PostFrames,
		BurstCount > 0 ? *FString::FromInt(BurstCount) : TEXT("until-stop"),
		FrameCap > 0 ? *FString::FromInt(FrameCap) : TEXT("none"));
#endif
}

bool UAnomalyCaptureSubsystem::HasGameWindow(UWorld* World) const
{
	UGameViewportClient* GV = World ? World->GetGameViewport() : nullptr;
	return GV && GV->Viewport;
}

bool UAnomalyCaptureSubsystem::IsGameWindowFocused(UWorld* World) const
{
	UGameViewportClient* GV = World ? World->GetGameViewport() : nullptr;
	if (GV && GV->Viewport)
	{
		return GV->Viewport->IsForegroundWindow();
	}
	return true;
}

void UAnomalyCaptureSubsystem::StopRun()
{
#if ANOMALY_CAPTURE
	if (!bRunning)
	{
		return;
	}
	FinishRun(true);
#endif
}

void UAnomalyCaptureSubsystem::GetStatus(bool& bOutRunning, int32& OutFrames, FString& OutRunDir, int32& OutSeed) const
{
	bOutRunning = bRunning;
	OutFrames = FramesWritten;
	OutRunDir = RunDir;
	OutSeed = Seed;
}

void UAnomalyCaptureSubsystem::LogStatus() const
{
#if ANOMALY_CAPTURE
	if (!bRunning)
	{
		UE_LOG(LogAnomalyCapture, Log,
			TEXT("Capture: idle. Config K=%d L=%d pre=%d positive=%d post=%d bursts=%s capture=%s fps=%d pace=%s delivery=%s clock=%s focusgate=%s. Start: IAI.Capture.Start [dir] [png|jpeg] [seed]."),
			SettleFrames, ViewLagFrames, PreFrames, PositiveFrames, PostFrames,
			BurstCount > 0 ? *FString::FromInt(BurstCount) : TEXT("until-stop"),
			bAsyncCapture ? TEXT("async/backbuffer") : TEXT("sync"),
			VideoFps, bPaceCapture ? TEXT("on") : TEXT("off"),
			bDeliveryMode ? TEXT("on") : TEXT("off"),
			ContentClock == EContentClock::Game ? TEXT("game") : TEXT("wall"),
			bFocusGate ? TEXT("on") : TEXT("off"));
		return;
	}
	UE_LOG(LogAnomalyCapture, Log,
		TEXT("Capture: RUNNING %s | seed=%d | burst %d%s | frames=%d (positive=%d) | zero-match bursts=%d"),
		*RunDir, Seed, BurstsDone + 1,
		BurstCount > 0 ? *FString::Printf(TEXT("/%d"), BurstCount) : TEXT(""),
		FramesWritten, PositiveFramesWritten, ZeroMatchBursts);
#else
	UE_LOG(LogAnomalyCapture, Log, TEXT("Capture: compiled out (ANOMALY_CAPTURE=0)."));
#endif
}

#if ANOMALY_CAPTURE

namespace
{
	bool ComputeGameViewportCapture(UWorld* World, SWindow*& OutWindow, FIntRect& OutRect)
	{
		OutWindow = nullptr;
		OutRect = FIntRect(0, 0, 0, 0);

		UGameViewportClient* GVC = World ? World->GetGameViewport() : nullptr;
		if (!GVC || !FSlateApplication::IsInitialized())
		{
			return false;
		}

		TSharedPtr<SViewport> ViewportWidget = GVC->GetGameViewportWidget();
		if (!ViewportWidget.IsValid())
		{
			return false;
		}

		TSharedPtr<SWindow> Window = FSlateApplication::Get().FindWidgetWindow(ViewportWidget.ToSharedRef());
		if (!Window.IsValid())
		{
			return false;
		}
		OutWindow = Window.Get();

		const FGeometry InnerWindowGeometry = Window->GetWindowGeometryInWindow();
		FArrangedChildren JustWindow(EVisibility::Visible);
		JustWindow.AddWidget(FArrangedWidget(Window.ToSharedRef(), InnerWindowGeometry));

		FWidgetPath WidgetPath(Window.ToSharedRef(), JustWindow);
		if (!WidgetPath.ExtendPathTo(FWidgetMatcher(ViewportWidget.ToSharedRef()), EVisibility::Visible))
		{
			return false;
		}

		const FArrangedWidget ArrangedWidget =
			WidgetPath.FindArrangedWidget(ViewportWidget.ToSharedRef()).Get(FArrangedWidget::GetNullWidget());
		const FVector2D Position = ArrangedWidget.Geometry.GetAbsolutePosition();
		const FVector2D Size = ArrangedWidget.Geometry.GetAbsoluteSize();

		OutRect = FIntRect(
			FMath::RoundToInt(Position.X),
			FMath::RoundToInt(Position.Y),
			FMath::RoundToInt(Position.X + Size.X),
			FMath::RoundToInt(Position.Y + Size.Y));

		return OutRect.Width() > 0 && OutRect.Height() > 0;
	}
}


UAnomalyAutoInjectorSubsystem* UAnomalyCaptureSubsystem::ResolveAuto() const
{
	UWorld* World = GetWorld();
	return World ? World->GetSubsystem<UAnomalyAutoInjectorSubsystem>() : nullptr;
}

void UAnomalyCaptureSubsystem::EnsureCapturer()
{
	if (!Async.IsValid())
	{
		return;
	}
	if (!Async->Capturer.IsValid())
	{
		Async->Capturer = MakeShared<FAnomalyFrameCapturer, ESPMode::ThreadSafe>();
		Async->Capturer->RegisterBackbufferHook();
	}
	if (!Async->Writer.IsValid())
	{
		Async->Writer = MakeShared<FAnomalyAsyncWriter, ESPMode::ThreadSafe>();
	}
	FModuleManager::Get().LoadModule(TEXT("ImageWrapper"));
}

void UAnomalyCaptureSubsystem::ProcessCompletedFrames()
{
	if (!Async.IsValid() || !Async->Capturer.IsValid() || !Async->Writer.IsValid())
	{
		return;
	}

	Async->Capturer->EnqueueDrain();

	const AnomalyPreview::EImageFormat Format =
		bFormatPng ? AnomalyPreview::EImageFormat::PNG : AnomalyPreview::EImageFormat::JPEG;
	const TCHAR* Ext = bFormatPng ? TEXT("png") : TEXT("jpg");

	FAnomalyCapturedFrame Frame;
	while (Async->Capturer->PopCompleted(Frame))
	{
		const AnomalyLabel::FCaptureSnapshot* Snap = Async->PendingSnapshots.Find(Frame.RequestId);
		if (!Snap)
		{
			UE_LOG(LogAnomalyCapture, Verbose, TEXT("Capture(async): completed frame id=%llu has no pending snapshot (dropped)."),
				Frame.RequestId);
			continue;
		}

		const FString ImageName = FString::Printf(TEXT("Actual_Frames/frame_%05d.%s"), Snap->SessionIndex, Ext);
		int32 NumLabels = 0;
		const FString Record = AnomalyLabel::BuildLabelRecordForSnapshot(*Snap, Frame.Width, Frame.Height, ImageName, NumLabels);

		AccumulateFrameEvents(Snap->Fires, Snap->FireHidden, Snap->FirePos, Snap->View, Snap->NearClip,
			Snap->SessionIndex, Snap->TimeSeconds);

		FAnomalyAsyncWriter::FJob Job;
		Job.OutputDir = RunDir;
		Job.OutFormat = Format;
		Job.RawBytes = MoveTemp(Frame.RawBytes);
		Job.SrcFormat = Frame.Format;
		Job.BytesPerPixel = Frame.BytesPerPixel;
		Job.Width = Frame.Width;
		Job.Height = Frame.Height;
		Job.ImageRelPath = ImageName;
		Job.Record = Record;
		Job.bPositive = Snap->Fires.Num() > 0;
		Job.bWriteLabels = !bDeliveryMode;
		Async->Writer->Enqueue(MoveTemp(Job));

		Async->PendingSnapshots.Remove(Frame.RequestId);
	}

	FramesWritten = Async->Writer->GetFramesWritten();
	PositiveFramesWritten = Async->Writer->GetPositiveWritten();
}

void UAnomalyCaptureSubsystem::DrainAsyncToCompletion()
{
	if (!Async.IsValid() || !Async->Capturer.IsValid() || !Async->Writer.IsValid())
	{
		return;
	}

	for (int32 Iter = 0; Iter < 8 && Async->PendingSnapshots.Num() > 0; ++Iter)
	{
		Async->Capturer->EnqueueDrain();
		FlushRenderingCommands();
		ProcessCompletedFrames();
	}

	if (Async->PendingSnapshots.Num() > 0)
	{
		UE_LOG(LogAnomalyCapture, Warning, TEXT("Capture(async): %d frame(s) did not resolve by run end (dropped)."),
			Async->PendingSnapshots.Num());
		Async->PendingSnapshots.Empty();
	}

	Async->Writer->FlushPending(5.0);
	FramesWritten = Async->Writer->GetFramesWritten();
	PositiveFramesWritten = Async->Writer->GetPositiveWritten();
	if (Async->Writer->GetDropped() > 0)
	{
		UE_LOG(LogAnomalyCapture, Warning, TEXT("Capture(async): %d frame(s) failed to encode/write."),
			Async->Writer->GetDropped());
	}
}

FAnomalyViewInfo UAnomalyCaptureSubsystem::ProjectionView() const
{
	const int32 Idx = ViewRing.Num() - 1 - ViewLagFrames;
	if (ViewRing.IsValidIndex(Idx))
	{
		return ViewRing[Idx];
	}
	return ViewRing.Num() > 0 ? ViewRing.Last() : FAnomalyViewInfo{};
}

void UAnomalyCaptureSubsystem::SampleViewThisTick()
{
	FAnomalyViewInfo V;
	AnomalyViewport::GetActiveViewInfo(GetWorld(), V);
	ViewRing.Add(V);

	const int32 MaxDepth = ViewLagFrames + 2;
	while (ViewRing.Num() > MaxDepth)
	{
		ViewRing.RemoveAt(0);
	}
}

void UAnomalyCaptureSubsystem::BeginFire()
{
	UAnomalyAutoInjectorSubsystem* Auto = ResolveAuto();
	const bool bFired = Auto
		? (bTargetedMode ? Auto->TryFireSpecific(TargetAnomalyId, TargetActorName) : Auto->TryFireOnce())
		: false;
	if (!bFired)
	{
		++ZeroMatchBursts;
		UE_LOG(LogAnomalyCapture, Log, TEXT("Capture: burst %d fired nothing (zero-match / empty) — negatives only."),
			BurstsDone + 1);
	}
	Phase = ECapturePhase::SettleAfterFire;
	PhaseFramesLeft = SettleFrames;
}

void UAnomalyCaptureSubsystem::BeginRevert()
{
	if (UAnomalyAutoInjectorSubsystem* Auto = ResolveAuto())
	{
		Auto->RevertAllLiveFires();
	}
	Phase = ECapturePhase::SettleAfterRevert;
	PhaseFramesLeft = SettleFrames;
}

void UAnomalyCaptureSubsystem::CaptureCurrentFrame()
{
	UWorld* World = GetWorld();
	const AnomalyPreview::EImageFormat Format =
		bFormatPng ? AnomalyPreview::EImageFormat::PNG : AnomalyPreview::EImageFormat::JPEG;

	const FAnomalyViewInfo ProjView = ProjectionView();

	if (bAsyncCapture && Async.IsValid() && Async->Capturer.IsValid())
	{
		SWindow* TargetWindow = nullptr;
		FIntRect CaptureRect;
		if (ComputeGameViewportCapture(World, TargetWindow, CaptureRect))
		{
			AnomalyLabel::FCaptureSnapshot Snap;
			Snap.FrameCounter = GFrameCounter;
			Snap.SessionIndex = SessionFrameIndex;
			Snap.TimeSeconds = World ? World->GetTimeSeconds() : 0.0;
			Snap.NearClip = GNearClippingPlane;
			Snap.View = ProjView;
			if (FirstFrameTimeSeconds < 0.0)
			{
				FirstFrameTimeSeconds = Snap.TimeSeconds;
			}
			LastFrameTimeSeconds = Snap.TimeSeconds;
			Snap.WallSeconds = FPlatformTime::Seconds();
			StampArmWallClock(Snap.WallSeconds);
			Async->PendingSnapshots.Add(Snap.FrameCounter, MoveTemp(Snap));
			Async->Capturer->ArmForCapture(GFrameCounter, TargetWindow, CaptureRect);
			ArmedLabelFrameId = GFrameCounter;
			bHasArmedLabel = true;
			++SessionFrameIndex;
			CheckEarlyPacingWarning();
			return;
		}

		UE_LOG(LogAnomalyCapture, Verbose,
			TEXT("Capture(async): could not resolve the game-viewport rect this tick — falling back to sync grab."));
	}

	const UAnomalyAutoInjectorSubsystem* Auto = ResolveAuto();
	const bool bPositive = Auto && Auto->GetLiveFireCount() > 0;

	const TCHAR* Ext = bFormatPng ? TEXT("png") : TEXT("jpg");
	const FString ImageName = FString::Printf(TEXT("Actual_Frames/frame_%05d.%s"), SessionFrameIndex, Ext);

	FString ImagePath, SidecarPath;
	int32 NumLabels = 0;
	const double NowWall = FPlatformTime::Seconds();
	if (AnomalyLabel::CaptureLabeledShot(World, RunDir, Format, ProjView, ImageName, SessionFrameIndex, NowWall, ImagePath, SidecarPath, NumLabels, false, !bDeliveryMode))
	{
		TArray<FAutoLiveFireInfo> Fires;
		if (Auto) { Fires = Auto->GetLiveFires(); }
		TArray<uint8> Hidden;
		TArray<FVector> Pos;
		Hidden.Reserve(Fires.Num());
		Pos.Reserve(Fires.Num());
		for (const FAutoLiveFireInfo& F : Fires)
		{
			const AActor* FActor = F.TargetActor.Get();
			Hidden.Add((FActor && FActor->IsHidden()) ? 1 : 0);
			Pos.Add(FActor ? FActor->GetActorLocation() : FVector::ZeroVector);
		}
		const double NowT = World ? World->GetTimeSeconds() : 0.0;
		AccumulateFrameEvents(Fires, Hidden, Pos, ProjView, GNearClippingPlane, SessionFrameIndex, NowT);
		if (FirstFrameTimeSeconds < 0.0)
		{
			FirstFrameTimeSeconds = NowT;
		}
		LastFrameTimeSeconds = NowT;
		StampArmWallClock(NowWall);

		++SessionFrameIndex;
		++FramesWritten;
		if (bPositive)
		{
			++PositiveFramesWritten;
		}
		CheckEarlyPacingWarning();
	}
}

void UAnomalyCaptureSubsystem::FinalizeArmedLabel()
{
	if (!bHasArmedLabel)
	{
		return;
	}
	bHasArmedLabel = false;

	if (!Async.IsValid())
	{
		return;
	}
	AnomalyLabel::FCaptureSnapshot* Snap = Async->PendingSnapshots.Find(ArmedLabelFrameId);
	if (!Snap)
	{
		return;
	}

	if (const UAnomalyAutoInjectorSubsystem* Auto = ResolveAuto())
	{
		Snap->Fires = Auto->GetLiveFires();
	}
	Snap->FireHidden.Reset();
	Snap->FirePos.Reset();
	Snap->FireHidden.Reserve(Snap->Fires.Num());
	Snap->FirePos.Reserve(Snap->Fires.Num());
	for (const FAutoLiveFireInfo& F : Snap->Fires)
	{
		const AActor* FActor = F.TargetActor.Get();
		Snap->FireHidden.Add((FActor && FActor->IsHidden()) ? 1 : 0);
		Snap->FirePos.Add(FActor ? FActor->GetActorLocation() : FVector::ZeroVector);
	}
}

void UAnomalyCaptureSubsystem::PaceThisTick()
{
	if (!bPaceCapture || VideoFps <= 0)
	{
		return;
	}
	const double Period = 1.0 / (double)VideoFps;
	const double Now = FPlatformTime::Seconds();
	if (!bPaceInitialized)
	{
		bPaceInitialized = true;
		NextPaceWallTarget = Now + Period;
		return;
	}
	if (Now < NextPaceWallTarget)
	{
		const double Coarse = NextPaceWallTarget - Now - GPaceCoarseSleepMarginSec;
		if (Coarse > 0.0)
		{
			FPlatformProcess::SleepNoStats((float)Coarse);
		}
		while (FPlatformTime::Seconds() < NextPaceWallTarget)
		{
			FPlatformProcess::SleepNoStats(0.0f);
		}
		NextPaceWallTarget += Period;
	}
	else
	{
		NextPaceWallTarget = Now + Period;
	}
}

void UAnomalyCaptureSubsystem::StampArmWallClock(double NowWall)
{
	if (FirstArmWallSeconds < 0.0)
	{
		FirstArmWallSeconds = NowWall;
	}
	LastArmWallSeconds = NowWall;
}

void UAnomalyCaptureSubsystem::CheckEarlyPacingWarning()
{
	if (bEarlyRatioWarned || SessionFrameIndex < GEarlyPacingWarnMinFrames)
	{
		return;
	}
	const double GameSpan = LastFrameTimeSeconds - FirstFrameTimeSeconds;
	const double WallSpan = LastArmWallSeconds - FirstArmWallSeconds;
	if (GameSpan <= KINDA_SMALL_NUMBER || WallSpan <= KINDA_SMALL_NUMBER)
	{
		return;
	}
	const double Ratio = WallSpan / GameSpan;
	if (Ratio > 1.0 + GFpsStampTolerance)
	{
		bEarlyRatioWarned = true;
		if (ContentClock == EContentClock::Game)
		{
			UE_LOG(LogAnomalyCapture, Warning,
				TEXT("Capture: live capture running slow (~%.1f of %d fps, ratio %.2f) — the video will be stamped at target %d and plays natural; this is a capture-time perf issue only. Lower IAI.Capture.Fps or run packaged to speed the live capture."),
				(double)VideoFps / Ratio, VideoFps, Ratio, VideoFps);
		}
		else
		{
			UE_LOG(LogAnomalyCapture, Warning,
				TEXT("Capture: sustaining ~%.1f of %d fps (ratio %.2f) — the video will be stamped at the true rate; lower IAI.Capture.Fps or run a packaged build."),
				(double)VideoFps / Ratio, VideoFps, Ratio);
		}
	}
}

void UAnomalyCaptureSubsystem::ComputeRunPacing()
{
	LastRunPacing = FLastRunPacing();
	LastRunPacing.bPaced = bPaceCapture;
	LastRunPacing.TargetFps = (double)VideoFps;
	LastRunPacing.SustainedWallFps = (double)VideoFps;
	LastRunPacing.SpeedRatio = 1.0;
	LastRunPacing.StampedFps = (double)VideoFps;

	const double GameSpan = LastFrameTimeSeconds - FirstFrameTimeSeconds;
	const double WallSpan = LastArmWallSeconds - FirstArmWallSeconds;
	if (SessionFrameIndex < 2 || FirstFrameTimeSeconds < 0.0 || FirstArmWallSeconds < 0.0
		|| GameSpan <= KINDA_SMALL_NUMBER || WallSpan <= KINDA_SMALL_NUMBER)
	{
		return;
	}

	LastRunPacing.bValid = true;
	LastRunPacing.SpeedRatio = WallSpan / GameSpan;
	LastRunPacing.SustainedWallFps = (double)VideoFps / LastRunPacing.SpeedRatio;

	if (ContentClock == EContentClock::Game)
	{
		LastRunPacing.StampedFps = (double)VideoFps;
		if (LastRunPacing.SpeedRatio > 1.0 + GFpsStampTolerance)
		{
			UE_LOG(LogAnomalyCapture, Warning,
				TEXT("Capture: live capture ran slow (sustained %.3f of target %d fps, ratio %.3f) — video.fps stamped at target %d and plays natural; the slowness is a capture-time performance issue, not a video defect."),
				LastRunPacing.SustainedWallFps, VideoFps, LastRunPacing.SpeedRatio, VideoFps);
		}
	}
	else
	{
		if (LastRunPacing.SpeedRatio > 1.0 + GFpsStampTolerance)
		{
			LastRunPacing.StampedFps = FMath::RoundToDouble(LastRunPacing.SustainedWallFps * 1000.0) / 1000.0;
			UE_LOG(LogAnomalyCapture, Warning,
				TEXT("Capture: could not hold %d fps wall-clock (sustained %.3f fps, ratio %.3f) — video.fps stamped at the true rate %.3f."),
				VideoFps, LastRunPacing.SustainedWallFps, LastRunPacing.SpeedRatio, LastRunPacing.StampedFps);
		}
		else if (LastRunPacing.SpeedRatio < 1.0 - GFpsStampTolerance)
		{
			UE_LOG(LogAnomalyCapture, Log,
				TEXT("Capture: ran faster than %d fps wall-clock (ratio %.3f, pace=%s) — video.fps stays %d."),
				VideoFps, LastRunPacing.SpeedRatio, bPaceCapture ? TEXT("on") : TEXT("off"), VideoFps);
		}
	}

	UE_LOG(LogAnomalyCapture, Log,
		TEXT("Capture: pacing=%s | clock=%s | target=%d fps | sustained=%.3f fps | ratio=%.3f | stamped=%.3f."),
		bPaceCapture ? TEXT("on") : TEXT("off"),
		ContentClock == EContentClock::Game ? TEXT("game") : TEXT("wall"), VideoFps,
		LastRunPacing.SustainedWallFps, LastRunPacing.SpeedRatio, LastRunPacing.StampedFps);
}

void UAnomalyCaptureSubsystem::FinishRun(bool bLogLine)
{
	if (UAnomalyAutoInjectorSubsystem* Auto = ResolveAuto())
	{
		Auto->RevertAllLiveFires();
	}

	if (bRunBegun)
	{
		if (bAsyncCapture)
		{
			DrainAsyncToCompletion();
		}

		ComputeRunPacing();

		WriteSessionAnnotationFile();

		AnomalyLabel::WriteRunSummary(RunDir, FramesWritten, PositiveFramesWritten, BurstsDone, ZeroMatchBursts, GFrameCounter,
			VideoFps, LastRunPacing.SustainedWallFps, LastRunPacing.SpeedRatio, LastRunPacing.StampedFps, bPaceCapture, bDeliveryMode,
			ContentClock == EContentClock::Game ? TEXT("game") : TEXT("wall"));

		if (bLogLine)
		{
			UE_LOG(LogAnomalyCapture, Log,
				TEXT("=== Capture run FINISHED: %s | %d frame(s) (positive=%d) | %d burst(s), %d zero-match ==="),
				*RunDir, FramesWritten, PositiveFramesWritten, BurstsDone, ZeroMatchBursts);
		}
	}
	else
	{
		IFileManager::Get().DeleteDirectory(*RunDir, false, true);
		if (bLogLine)
		{
			UE_LOG(LogAnomalyCapture, Log,
				TEXT("=== Capture run CANCELLED before focus: %s | no frames written (armed-pending run stopped) ==="),
				*RunDir);
		}
	}

	bRunBegun = false;
	bRunning = false;
	Phase = ECapturePhase::Idle;
	PhaseFramesLeft = 0;

	bTargetedMode = false;
	TargetAnomalyId = NAME_None;
	TargetActorName.Reset();

	if (bFixedTimeStepOverridden)
	{
		FApp::SetUseFixedTimeStep(bSavedUseFixedTimeStep);
		FApp::SetFixedDeltaTime(SavedFixedDeltaTime);
		bFixedTimeStepOverridden = false;
	}

	AnomalyViewport::SetOverlaysSuppressed(false);

	if (bAutoWasRunning)
	{
		if (!bDeinitializing)
		{
			if (UAnomalyAutoInjectorSubsystem* Auto = ResolveAuto())
			{
				Auto->SetRunning(true);
				UE_LOG(LogAnomalyCapture, Log, TEXT("Capture: resumed the auto-injector's Run (paused for the capture)."));
			}
		}
		bAutoWasRunning = false;
	}
}

void UAnomalyCaptureSubsystem::AccumulateFrameEvents(const TArray<FAutoLiveFireInfo>& Fires,
	const TArray<uint8>& FireHidden, const TArray<FVector>& FirePos, const FAnomalyViewInfo& View,
	float NearClip, int32 SessionIndex, double TimeSeconds)
{
	if (!Async.IsValid())
	{
		return;
	}
	UWorld* World = GetWorld();

	for (int32 i = 0; i < Fires.Num(); ++i)
	{
		const FAutoLiveFireInfo& F = Fires[i];

		FSessionEventAccum* Ev = Async->SessionEvents.FindByPredicate(
			[&F](const FSessionEventAccum& E){ return E.Id == F.Id && E.StartFrame == F.StartFrame && E.Target == F.Target; });
		if (!Ev)
		{
			FSessionEventAccum NewEv;
			NewEv.Id = F.Id;
			NewEv.Target = F.Target;
			NewEv.StartFrame = F.StartFrame;
			Ev = &Async->SessionEvents[Async->SessionEvents.Add(MoveTemp(NewEv))];
		}

		if (SessionIndex < Ev->AnchorIndex)
		{
			Ev->AnchorIndex = SessionIndex;
			Ev->CamPos = View.Origin;
			Ev->CamRot = View.Rotation;
			Ev->CamFov = View.HorizontalFOVDeg;
			Ev->CamAspect = View.AspectRatio;
			Ev->CamNear = NearClip;
			Ev->CamPath = ResolveCameraPath(World);
			Ev->TicksMsec = (int64)FMath::RoundToDouble(TimeSeconds * 1000.0);
			Ev->NodeName = F.Target;
			if (const AActor* FActor = F.TargetActor.Get())
			{
				Ev->NodePath = FActor->GetPathName();
			}
			Ev->NodePos = FirePos.IsValidIndex(i) ? FirePos[i] : FVector::ZeroVector;
		}

		if (const AActor* FActor = F.TargetActor.Get())
		{
			FVector2D Min(FVector2D::ZeroVector), Max(FVector2D::ZeroVector);
			if (AnomalyViewport::ProjectActorBoundsToScreenRect(View, FActor, Min, Max))
			{
				Ev->AffectedFrames.Add(SessionIndex);
				const double BoxW = FMath::Clamp((double)Max.X, 0.0, 1.0) - FMath::Clamp((double)Min.X, 0.0, 1.0);
				const double BoxH = FMath::Clamp((double)Max.Y, 0.0, 1.0) - FMath::Clamp((double)Min.Y, 0.0, 1.0);
				Ev->CoverageSum += FMath::Max(0.0, BoxW) * FMath::Max(0.0, BoxH);
				++Ev->CoverageCount;
			}
		}

		const int32 Hidden = (FireHidden.IsValidIndex(i) && FireHidden[i]) ? 1 : 0;
		if (Hidden) { ++Ev->HiddenFrames; Ev->HiddenIndices.Add(SessionIndex); } else { ++Ev->VisibleFrames; }
		if (Ev->LastHidden != -1 && Hidden != Ev->LastHidden) { ++Ev->Transitions; }
		Ev->LastHidden = Hidden;
	}
}

void UAnomalyCaptureSubsystem::WriteSessionAnnotationFile()
{
	if (!Async.IsValid())
	{
		return;
	}

	AnomalyLabel::FSessionAnnotation A;
	A.SessionId = SessionId;
	A.Video.FramesDir = TEXT("Actual_Frames");
	A.Video.VideoPath = FString::Printf(TEXT("Video_Clip/%s.mp4"), *SessionId);
	A.Video.ResolutionW = ViewportW;
	A.Video.ResolutionH = ViewportH;
	A.Video.TotalFrames = FramesWritten;

	A.Video.Fps = LastRunPacing.StampedFps > 0.0 ? LastRunPacing.StampedFps : (double)VideoFps;
	A.Video.TargetFps = VideoFps;

	for (FSessionEventAccum& Ev : Async->SessionEvents)
	{
		AnomalyLabel::FSessionEvent Out;
		MapAnomalyToClient(Ev.Id, Ev.Transitions, Out.AnomalyType, Out.AnomalySubtype);

		const bool bHideType = Ev.HiddenIndices.Num() > 0;
		TArray<int32> FrameIndices = bHideType ? Ev.HiddenIndices : Ev.AffectedFrames;
		FrameIndices.Sort();
		Out.FrameIndices = MoveTemp(FrameIndices);
		Out.CoverageRatio = Ev.CoverageCount > 0 ? (Ev.CoverageSum / (double)Ev.CoverageCount) : 0.0;

		AnomalyLabel::FSessionNode Node;
		Node.Name = Ev.NodeName;
		Node.Path = Ev.NodePath;
		Node.GlobalPosition = Ev.NodePos;
		Out.Nodes.Add(Node);
		Out.PrimaryIndex = 0;

		Out.CamPath = Ev.CamPath;
		Out.CamPosition = Ev.CamPos;
		Out.CamRotation = Ev.CamRot;
		Out.CamFovDeg = Ev.CamFov;
		Out.CamAspect = Ev.CamAspect;
		Out.CamNear = Ev.CamNear;
		Out.CamFar = GAnomalyDefaultFarPlane;

		Out.TicksMsec = Ev.TicksMsec;
		Out.EngineName = TEXT("UnrealEngine");
		Out.EngineVersion = EngineVersion;
		Out.EngineProject = EngineProject;

		A.Events.Add(MoveTemp(Out));
	}

	if (AnomalyLabel::WriteSessionAnnotation(RunDir, A))
	{
		UE_LOG(LogAnomalyCapture, Log, TEXT("Capture: wrote annotation.json (%d anomaly event(s))."), A.Events.Num());
	}
	else
	{
		UE_LOG(LogAnomalyCapture, Warning, TEXT("Capture: failed to write annotation.json."));
	}
}


namespace
{
	UAnomalyCaptureSubsystem* ResolveCapture(UWorld* World)
	{
		if (!World)
		{
			UE_LOG(LogAnomalyCapture, Warning, TEXT("No world for this command; run it from a Game/PIE world."));
			return nullptr;
		}
		UAnomalyCaptureSubsystem* Cap = World->GetSubsystem<UAnomalyCaptureSubsystem>();
		if (!Cap)
		{
			UE_LOG(LogAnomalyCapture, Warning, TEXT("AnomalyCapture subsystem not present for world '%s'."), *GetNameSafe(World));
		}
		return Cap;
	}
}

static FAutoConsoleCommandWithWorldAndArgs GCaptureStartCmd(
	TEXT("IAI.Capture.Start"),
	TEXT("Start a labeled burst-capture run. Usage: IAI.Capture.Start [outDir] [png|jpeg] [seed] [maxFrames] [anomaly] [targetActor]  ")
	TEXT("(default dir <ProjectSaved>/AnomalyCaptures; png; seed = auto-injector's current; maxFrames 0 = until "
	     "Stop / burst schedule). Pass BOTH [anomaly] and [targetActor] for a TARGETED run (fires only that anomaly on "
	     "only that actor each burst); omit both for AUTO-POOL (random mix from the enabled pool). Empty \"\" placeholders "
	     "resolve to defaults for the leading args. The auto-injector's Run is paused for the run and resumed on finish. "
	     "Configure bursts first with IAI.Capture.Config."),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (UAnomalyCaptureSubsystem* Cap = ResolveCapture(World))
			{
				auto Slot = [&Args](int32 Index) -> FString
				{
					if (!Args.IsValidIndex(Index)) { return FString(); }
					const FString& S = Args[Index];
					if (S.IsEmpty() || S == TEXT("\"\"") || S == TEXT("''")) { return FString(); }
					return S;
				};
				const FString Dir = Slot(0);
				const FString Fmt = Slot(1);
				const FString SeedStr = Slot(2);
				const FString MaxStr = Slot(3);
				const FString Anomaly = Slot(4);
				const FString TargetActor = Slot(5);
				const bool bPng = !(Fmt.Equals(TEXT("jpeg"), ESearchCase::IgnoreCase) || Fmt.Equals(TEXT("jpg"), ESearchCase::IgnoreCase));
				const int32 Seed = !SeedStr.IsEmpty() ? FCString::Atoi(*SeedStr) : -1;
				const int32 MaxFrames = !MaxStr.IsEmpty() ? FCString::Atoi(*MaxStr) : 0;
				Cap->StartRun(Dir, bPng, Seed, MaxFrames, Anomaly, TargetActor);
			}
		}));

static FAutoConsoleCommandWithWorldAndArgs GCaptureStopCmd(
	TEXT("IAI.Capture.Stop"),
	TEXT("Stop the capture run (reverts any in-flight fire, writes run_summary.json)."),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>&  , UWorld* World)
		{
			if (UAnomalyCaptureSubsystem* Cap = ResolveCapture(World)) { Cap->StopRun(); }
		}));

static FAutoConsoleCommandWithWorldAndArgs GCaptureStatusCmd(
	TEXT("IAI.Capture.Status"),
	TEXT("Log capture run state, config, and counters."),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>&  , UWorld* World)
		{
			if (UAnomalyCaptureSubsystem* Cap = ResolveCapture(World)) { Cap->LogStatus(); }
		}));

static FAutoConsoleCommandWithWorldAndArgs GCaptureConfigCmd(
	TEXT("IAI.Capture.Config"),
	TEXT("Set the burst schedule. Usage: IAI.Capture.Config <settleK> <preFrames> <positiveFrames> <postFrames> <burstCount>  (burstCount 0 = until Stop)"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() < 5)
			{
				UE_LOG(LogAnomalyCapture, Warning, TEXT("Usage: IAI.Capture.Config <settleK> <preFrames> <positiveFrames> <postFrames> <burstCount>"));
				return;
			}
			if (UAnomalyCaptureSubsystem* Cap = ResolveCapture(World))
			{
				Cap->SetBurstConfig(FCString::Atoi(*Args[0]), FCString::Atoi(*Args[1]), FCString::Atoi(*Args[2]),
					FCString::Atoi(*Args[3]), FCString::Atoi(*Args[4]));
			}
		}));

static FAutoConsoleCommandWithWorldAndArgs GCaptureViewLagCmd(
	TEXT("IAI.Capture.ViewLag"),
	TEXT("Set the bbox-projection view-lag L in frames (default 0). Legacy/sync-path projection knob; ")
	TEXT("the async stencil box (later stages) is frame-locked and does not use it. Usage: IAI.Capture.ViewLag <frames>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() < 1)
			{
				UE_LOG(LogAnomalyCapture, Warning, TEXT("Usage: IAI.Capture.ViewLag <frames>"));
				return;
			}
			if (UAnomalyCaptureSubsystem* Cap = ResolveCapture(World)) { Cap->SetViewLag(FCString::Atoi(*Args[0])); }
		}));

static FAutoConsoleCommandWithWorldAndArgs GCaptureAsyncCmd(
	TEXT("IAI.Capture.Async"),
	TEXT("Toggle async backbuffer readback (game UI included) vs the synchronous ReadPixels fallback. ")
	TEXT("Default ON. Usage: IAI.Capture.Async <0|1>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() < 1)
			{
				UE_LOG(LogAnomalyCapture, Warning, TEXT("Usage: IAI.Capture.Async <0|1>"));
				return;
			}
			if (UAnomalyCaptureSubsystem* Cap = ResolveCapture(World)) { Cap->SetAsyncCapture(FCString::Atoi(*Args[0]) != 0); }
		}));

static FAutoConsoleCommandWithWorldAndArgs GCaptureFpsCmd(
	TEXT("IAI.Capture.Fps"),
	TEXT("Set the native capture/playback rate (default 30). During a run the engine ticks on a FIXED ")
	TEXT("timestep of 1/fps, so every frame is an exact 1/fps slice of game time and the mp4 encodes at ")
	TEXT("exactly this rate. Usage: IAI.Capture.Fps <fps>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() < 1)
			{
				UE_LOG(LogAnomalyCapture, Warning, TEXT("Usage: IAI.Capture.Fps <fps>"));
				return;
			}
			if (UAnomalyCaptureSubsystem* Cap = ResolveCapture(World)) { Cap->SetCaptureFps(FCString::Atoi(*Args[0])); }
		}));

static FAutoConsoleCommandWithWorldAndArgs GCapturePaceCmd(
	TEXT("IAI.Capture.Pace"),
	TEXT("Toggle real-time frame pacing during capture runs (default ON). ON: each captured frame is held ")
	TEXT("to >= 1/fps of WALL time, so game time == wall time == video time and the live game runs at 1x ")
	TEXT("while capturing (slow-motion only if the machine cannot hold the rate). OFF: the engine free-runs ")
	TEXT("(old behavior); video.fps stamping stays honest either way. Usage: IAI.Capture.Pace <0|1>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() < 1)
			{
				UE_LOG(LogAnomalyCapture, Warning, TEXT("Usage: IAI.Capture.Pace <0|1>"));
				return;
			}
			if (UAnomalyCaptureSubsystem* Cap = ResolveCapture(World)) { Cap->SetCapturePace(FCString::Atoi(*Args[0]) != 0); }
		}));

static FAutoConsoleCommandWithWorldAndArgs GCaptureDeliveryCmd(
	TEXT("IAI.Capture.Delivery"),
	TEXT("Toggle client-delivery mode (default OFF). ON: a run writes ONLY the client-facing artifacts — ")
	TEXT("Actual_Frames/ + Video_Clip/ + run_summary.json + annotation.json — and suppresses labels.jsonl ")
	TEXT("and run.json (so the seed is not shipped and the session is not client-reproducible). Ground-truth ")
	TEXT("is still computed, just not written. OFF: full fidelity (all artifacts). The packaged default is ")
	TEXT("read at startup from DefaultGame.ini [AnomalyCapture] bDeliveryModeDefault; this command overrides ")
	TEXT("it for the session. Usage: IAI.Capture.Delivery <0|1>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() < 1)
			{
				UE_LOG(LogAnomalyCapture, Warning, TEXT("Usage: IAI.Capture.Delivery <0|1>"));
				return;
			}
			if (UAnomalyCaptureSubsystem* Cap = ResolveCapture(World)) { Cap->SetCaptureDelivery(FCString::Atoi(*Args[0]) != 0); }
		}));

static FAutoConsoleCommandWithWorldAndArgs GCaptureContentClockCmd(
	TEXT("IAI.Capture.ContentClock"),
	TEXT("Select which clock the game's visible content advances on, so the honest fps stamp picks the right ")
	TEXT("rate on a slow run (default WALL). game: content follows the GAME clock (StackOBot world under fixed ")
	TEXT("step) — every frame is an exact 1/target game-time slice, so video.fps is stamped at TARGET at any ")
	TEXT("ratio and plays natural; a slow run is a capture-time perf issue, not a video defect. wall: content ")
	TEXT("follows the WALL clock (sequencer/real-time titles) — a run slower than target stamps the sustained ")
	TEXT("rate so the video plays at true speed (the m11 office fix). Packaged default: DefaultGame.ini ")
	TEXT("[AnomalyCapture] ContentClockDefault=game|wall; this command overrides it for the session. ")
	TEXT("Usage: IAI.Capture.ContentClock <game|wall>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() < 1)
			{
				UE_LOG(LogAnomalyCapture, Warning, TEXT("Usage: IAI.Capture.ContentClock <game|wall>"));
				return;
			}
			UAnomalyCaptureSubsystem* Cap = ResolveCapture(World);
			if (!Cap) { return; }
			if (Args[0].Equals(TEXT("game"), ESearchCase::IgnoreCase))
			{
				Cap->SetContentClock(UAnomalyCaptureSubsystem::EContentClock::Game);
			}
			else if (Args[0].Equals(TEXT("wall"), ESearchCase::IgnoreCase))
			{
				Cap->SetContentClock(UAnomalyCaptureSubsystem::EContentClock::Wall);
			}
			else
			{
				UE_LOG(LogAnomalyCapture, Warning, TEXT("IAI.Capture.ContentClock: unknown token '%s' — expected 'game' or 'wall'. No change."), *Args[0]);
			}
		}));

static FAutoConsoleCommandWithWorldAndArgs GCaptureFocusGateCmd(
	TEXT("IAI.Capture.FocusGate"),
	TEXT("Gate the first captured frame on game-window focus (default ON). ON: a capture Start ARMS immediately ")
	TEXT("but holds the first frame until the game window has foreground focus — so clicking Start in the ")
	TEXT("external dashboard (browser) does not record idle frames during the click-and-move-back gap; focus the ")
	TEXT("game window to begin, or IAI.Capture.Stop to cancel the armed run. The gate is skipped when there is no ")
	TEXT("game window at all (headless / Simulate), and a safety timeout starts the run anyway if focus never ")
	TEXT("arrives. Packaged default: DefaultGame.ini [AnomalyCapture] bFocusGateDefault; this command overrides it ")
	TEXT("for the session. Usage: IAI.Capture.FocusGate <0|1>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() < 1)
			{
				UE_LOG(LogAnomalyCapture, Warning, TEXT("Usage: IAI.Capture.FocusGate <0|1>"));
				return;
			}
			if (UAnomalyCaptureSubsystem* Cap = ResolveCapture(World))
			{
				Cap->SetFocusGate(FCString::Atoi(*Args[0]) != 0);
			}
		}));

#endif
