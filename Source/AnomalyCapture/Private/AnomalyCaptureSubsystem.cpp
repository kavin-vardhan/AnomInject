#include "AnomalyCaptureSubsystem.h"

#include "AnomalyCaptureLog.h"
#include "Engine/World.h"
#include "Engine/GameViewportClient.h"
#include "HAL/IConsoleManager.h"
#include "Misc/Paths.h"
#include "Misc/DateTime.h"
#include "CoreGlobals.h"

#if ANOMALY_CAPTURE
#include "AnomalyLabelWriter.h"
#include "AnomalyPreviewCapture.h"
#include "AnomalyAutoInjectorSubsystem.h"
#include "AnomalyFrameCapturer.h"
#include "AnomalyAsyncWriter.h"
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

// Async capture state (PImpl) -- keeps the frame-capturer + render/Slate types out of the Public
// subsystem header. Always a complete type so the TUniquePtr member destructs in any build config;
// its members exist only when ANOMALY_CAPTURE is enabled.
#if ANOMALY_CAPTURE
// Per-event accumulator (one per fire = id + target + start_frame), built across the run and serialized
// into annotation.json at finalize. Anchor* fields are captured at the event's SMALLEST session index
// (its injection moment) — robust to async frames completing out of order.
struct FSessionEventAccum
{
	FName Id = NAME_None;
	FString Target;
	uint64 StartFrame = 0;

	TArray<int32> AffectedFrames;   // session-local indices where the fire was live AND bbox_valid
	double CoverageSum = 0.0;
	int32 CoverageCount = 0;

	int32 AnchorIndex = MAX_int32;  // smallest session index seen while live -> anchors camera/engine/node
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
	TArray<int32> HiddenIndices;    // session-indices where the actor was render-hidden (the OUT frames)
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
	// Reversed-Z gives no finite far plane; this documented placeholder only matters for depth
	// un-normalization, and depth is provided:false for now (populate the field, don't block).
	constexpr float GAnomalyDefaultFarPlane = 1000000.0f;   // 10 km in cm

	// A stable identifier for the active camera: the view-target actor's path (falls back to the camera
	// manager). Used for the annotation's camera.path.
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

	// internal anomaly id -> client vocabulary. Only blinking maps to a client clip today; others are
	// recorded in the session (type = internal id) but the slicer emits no client clip for them yet.
	//
	// STAGE 5 (slicer) CARRY-FORWARD — the native session is a SUPERSET; the slicer transforms, not copies:
	//  1. Client affected_frames = the OUT frames only (per-event _debug.hidden_frame_list), NOT our full
	//     live/bbox_valid affected_frames span. Client coverage_ratio then averages over those SAME
	//     out-frames. (PENDING owner confirmation with client: their prose "frames that contain the blink"
	//     is ambiguous vs their 1-out-frame example.)
	//  2. Client camera is global_position + additionalProperties:false -> slicer DROPS our bonus camera
	//     fields (rotation/fov_deg/aspect) + source_id + _debug + schema_version + engine name/version/project.
	//  3. bbox_norm=[x0,y0,x1,y1] CORNERS vs bbox_px=[x,y,w,h] ORIGIN+SIZE (labels.jsonl) — do not conflate.
	//  4. Clip window must include >=1 trailing post-revert VISIBLE frame so the delivered clip actually
	//     shows the "reappear" the subtype claims (the object is still hidden at the last OUT frame).
	void MapAnomalyToClient(FName Id, int32 Transitions, FString& OutType, FString& OutSubtype)
	{
		if (Id == FName(TEXT("blinking")))
		{
			OutType = TEXT("blink");
			// One disappear->reappear cycle = visible->hidden->visible = 2 transitions. More = sustained.
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
	UE_LOG(LogAnomalyCapture, Log, TEXT("AnomalyCapture subsystem initialized (idle — use IAI.Capture.Start)."));
#if WITH_EDITOR
	// The PIE "Shift+F1 for Mouse Cursor" hint is shown ONCE at PIE start and self-fades; the setting is
	// a one-shot SHOW-gate, NOT a live toggle (flipping it later cannot hide an already-shown label). So
	// disable it here at PIE-world init — before the level viewport's StartPlayInEditor shows it — and
	// restore on teardown. Keeps it out of EVERY captured frame for the PIE session (transient; never
	// SaveConfig'd; editor-PIE chrome, absent in packaged builds).
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

	SampleViewThisTick();

	if (bAsyncCapture)
	{
		ProcessCompletedFrames();
	}

	// Hard N-frame cap: once exactly FrameCap frames have been armed, stop arming and finalize cleanly —
	// no matter which cadence phase we're in (a truncated trailing burst is honest; its live fire is
	// reverted by FinishRun). Never strands a half-burst: we always land in DrainTail -> FinishRun.
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
					// Let the last-armed frames present + their GPU readbacks resolve before finalizing
					// (the final arm's backbuffer presents only NEXT frame). ProcessCompletedFrames at the
					// top of Tick drains them each tick -> a clean run drops ZERO frames.
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

void UAnomalyCaptureSubsystem::StartRun(const FString& BaseDir, bool bPng, int32 InSeed, int32 InFrameCap)
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

	if (Auto->IsRunning())
	{
		UE_LOG(LogAnomalyCapture, Warning,
			TEXT("IAI.Capture.Start: the auto-injector's Run is ON — capture drives firing itself; "
			     "'capture run + Auto.Run' is UNSUPPORTED. Recommend IAI.Auto.Run 0."));
	}

	Seed = (InSeed >= 0) ? InSeed : Auto->GetSeed();
	Auto->SetSeed(Seed);

	Auto->RevertAllLiveFires();

	bFormatPng = bPng;
	FrameCap = FMath::Max(0, InFrameCap);

	const FString Base = BaseDir.IsEmpty()
		? FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("AnomalyCaptures"))
		: BaseDir;
	const FString Stamp = FDateTime::Now().ToString(TEXT("%Y%m%d-%H%M%S"));
	SessionId = FString::Printf(TEXT("session_%s_s%d"), *Stamp, Seed);
	RunDir = FPaths::Combine(Base, SessionId);

	// Frames live under Actual_Frames/ (session-local frame_%05d) so the host-side ffmpeg %05d glob and the
	// client slicer both see contiguous 0-based numbering; created up-front so an empty run still has it.
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
	M.ViewportW = VW;
	M.ViewportH = VH;
	M.Format = bFormatPng ? TEXT("png") : TEXT("jpeg");
	M.StartFrame = StartFrame;
	M.StartTimeUtc = FDateTime::UtcNow().ToIso8601();
	AnomalyLabel::WriteRunManifest(RunDir, M);

	BurstsDone = 0;
	FramesWritten = 0;
	PositiveFramesWritten = 0;
	ZeroMatchBursts = 0;
	SessionFrameIndex = 0;
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

	// Suppress ONLY our own dev overlays (poll-radius sphere, selector HUD/box, auto HUD, the
	// on-screen heartbeat) for the whole run -> the captured frame is the real player view with the
	// GAME UI intact, minus our scaffolding. Applies to both the async + sync paths.
	AnomalyViewport::SetOverlaysSuppressed(true);

	Phase = ECapturePhase::LeadIn;
	PhaseFramesLeft = PreFrames;

	UE_LOG(LogAnomalyCapture, Log,
		TEXT("=== Capture run STARTED: %s | seed=%d fmt=%s capture=%s | K=%d L=%d pre=%d positive=%d post=%d bursts=%s frameCap=%s ==="),
		*RunDir, Seed, *M.Format, bAsyncCapture ? TEXT("async/backbuffer") : TEXT("sync"),
		SettleFrames, ViewLagFrames, PreFrames, PositiveFrames, PostFrames,
		BurstCount > 0 ? *FString::FromInt(BurstCount) : TEXT("until-stop"),
		FrameCap > 0 ? *FString::FromInt(FrameCap) : TEXT("none"));
#else
	UE_LOG(LogAnomalyCapture, Warning, TEXT("IAI.Capture.Start: capture compiled out (ANOMALY_CAPTURE=0)."));
#endif
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
			TEXT("Capture: idle. Config K=%d L=%d pre=%d positive=%d post=%d bursts=%s capture=%s. Start: IAI.Capture.Start [dir] [png|jpeg] [seed]."),
			SettleFrames, ViewLagFrames, PreFrames, PositiveFrames, PostFrames,
			BurstCount > 0 ? *FString::FromInt(BurstCount) : TEXT("until-stop"),
			bAsyncCapture ? TEXT("async/backbuffer") : TEXT("sync"));
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
	// Resolve the game viewport's window + its pixel rect within that window's backbuffer
	// (FFrameGrabber pattern). Clips capture to the PIE viewport region even when docked inside the
	// editor window -> editor chrome is never captured.
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
	// Preload ImageWrapper on the game thread so the encode workers never trigger a module load.
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

		// Build the label record on the GAME THREAD (it projects the target actor's bounds = UObject
		// access). All heavy work (convert + encode + file write) is handed to the writer thread pool.
		// Session-local name so files are contiguous 0-based (ffmpeg %05d + client slicer); the label's
		// "image" field and the written file share this exact rel path.
		const FString ImageName = FString::Printf(TEXT("Actual_Frames/frame_%05d.%s"), Snap->SessionIndex, Ext);
		int32 NumLabels = 0;
		const FString Record = AnomalyLabel::BuildLabelRecordForSnapshot(*Snap, Frame.Width, Frame.Height, ImageName, NumLabels);

		// Fold this frame's submit-time fire state into the per-event session annotation accumulator.
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
		Async->Writer->Enqueue(MoveTemp(Job));

		Async->PendingSnapshots.Remove(Frame.RequestId);
	}

	// Mirror the worker counters back so status / summary have a single game-thread source.
	FramesWritten = Async->Writer->GetFramesWritten();
	PositiveFramesWritten = Async->Writer->GetPositiveWritten();
}

void UAnomalyCaptureSubsystem::DrainAsyncToCompletion()
{
	if (!Async.IsValid() || !Async->Capturer.IsValid() || !Async->Writer.IsValid())
	{
		return;
	}

	// End-of-run only (NOT during the per-frame loop): a bounded flush to resolve the last in-flight
	// readbacks (GPU) and hand them to the writer. The run framerate is unaffected (the prohibition
	// is on per-frame flushes during capture).
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

	// Wait for the encode/write worker jobs so run_summary reflects all writes.
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
	const bool bFired = Auto ? Auto->TryFireOnce() : false;
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
			if (const UAnomalyAutoInjectorSubsystem* Auto = ResolveAuto())
			{
				Snap.Fires = Auto->GetLiveFires();
			}
			// Sample the fast-toggling hidden flag + actor position NOW (submit time) — the async readback
			// resolves a few frames later, by which point the blink has toggled again.
			Snap.FireHidden.Reserve(Snap.Fires.Num());
			Snap.FirePos.Reserve(Snap.Fires.Num());
			for (const FAutoLiveFireInfo& F : Snap.Fires)
			{
				const AActor* FActor = F.TargetActor.Get();
				Snap.FireHidden.Add((FActor && FActor->IsHidden()) ? 1 : 0);
				Snap.FirePos.Add(FActor ? FActor->GetActorLocation() : FVector::ZeroVector);
			}
			Async->PendingSnapshots.Add(Snap.FrameCounter, MoveTemp(Snap));
			Async->Capturer->ArmForCapture(GFrameCounter, TargetWindow, CaptureRect);
			++SessionFrameIndex;
			return;
		}

		UE_LOG(LogAnomalyCapture, Verbose,
			TEXT("Capture(async): could not resolve the game-viewport rect this tick — falling back to sync grab."));
	}

	// Synchronous fallback (legacy ReadPixels grab on this tick). Same session-local Actual_Frames/ naming.
	const UAnomalyAutoInjectorSubsystem* Auto = ResolveAuto();
	const bool bPositive = Auto && Auto->GetLiveFireCount() > 0;

	const TCHAR* Ext = bFormatPng ? TEXT("png") : TEXT("jpg");
	const FString ImageName = FString::Printf(TEXT("Actual_Frames/frame_%05d.%s"), SessionFrameIndex, Ext);

	FString ImagePath, SidecarPath;
	int32 NumLabels = 0;
	if (AnomalyLabel::CaptureLabeledShot(World, RunDir, Format, ProjView, ImageName, SessionFrameIndex, ImagePath, SidecarPath, NumLabels, false))
	{
		// Accumulate this frame into the per-event session annotation (submit == capture tick here).
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
		AccumulateFrameEvents(Fires, Hidden, Pos, ProjView, GNearClippingPlane, SessionFrameIndex,
			World ? World->GetTimeSeconds() : 0.0);

		++SessionFrameIndex;
		++FramesWritten;
		if (bPositive)
		{
			++PositiveFramesWritten;
		}
	}
}

void UAnomalyCaptureSubsystem::FinishRun(bool bLogLine)
{
	if (UAnomalyAutoInjectorSubsystem* Auto = ResolveAuto())
	{
		Auto->RevertAllLiveFires();
	}

	if (bAsyncCapture)
	{
		DrainAsyncToCompletion();
	}

	// All frames are now accumulated -> assemble the native multi-anomaly annotation.json.
	WriteSessionAnnotationFile();

	AnomalyLabel::WriteRunSummary(RunDir, FramesWritten, PositiveFramesWritten, BurstsDone, ZeroMatchBursts, GFrameCounter);

	if (bLogLine)
	{
		UE_LOG(LogAnomalyCapture, Log,
			TEXT("=== Capture run FINISHED: %s | %d frame(s) (positive=%d) | %d burst(s), %d zero-match ==="),
			*RunDir, FramesWritten, PositiveFramesWritten, BurstsDone, ZeroMatchBursts);
	}

	bRunning = false;
	Phase = ECapturePhase::Idle;
	PhaseFramesLeft = 0;

	AnomalyViewport::SetOverlaysSuppressed(false);
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

		// One event per (id, target, start_frame). Re-fires of the same id on a later burst are distinct.
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

		// Anchor = injection moment (smallest session index while live) -> camera / engine / node snapshot.
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

		// affected_frames = only frames where the projected box is valid; coverage = clamped box area.
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

		// Toggle observation (drives anomaly_subtype): count visibility transitions over the hold, and
		// record which session-indices were actually hidden (the OUT frames the slicer emits).
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
	A.Video.Fps = VideoFps;
	A.Video.TotalFrames = FramesWritten;

	for (FSessionEventAccum& Ev : Async->SessionEvents)
	{
		AnomalyLabel::FSessionEvent Out;
		MapAnomalyToClient(Ev.Id, Ev.Transitions, Out.AnomalyType, Out.AnomalySubtype);
		Out.SourceId = Ev.Id.ToString();

		Ev.AffectedFrames.Sort();
		Out.AffectedFrames = Ev.AffectedFrames;
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

		Out.VisibleFrames = Ev.VisibleFrames;
		Out.HiddenFrames = Ev.HiddenFrames;
		Out.Transitions = Ev.Transitions;
		Ev.HiddenIndices.Sort();
		Out.HiddenFrameList = Ev.HiddenIndices;

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
	TEXT("Start a labeled burst-capture run. Usage: IAI.Capture.Start [outDir] [png|jpeg] [seed] [maxFrames]  ")
	TEXT("(default dir <ProjectSaved>/AnomalyCaptures; png; seed = auto-injector's current; maxFrames 0 = until "
	     "Stop / burst schedule). The auto-injector's Run must be OFF (capture drives firing). Configure bursts "
	     "first with IAI.Capture.Config."),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (UAnomalyCaptureSubsystem* Cap = ResolveCapture(World))
			{
				const FString Dir = (Args.Num() > 0 && !Args[0].IsEmpty()) ? Args[0] : FString();
				bool bPng = true;
				if (Args.Num() > 1 && (Args[1].Equals(TEXT("jpeg"), ESearchCase::IgnoreCase) || Args[1].Equals(TEXT("jpg"), ESearchCase::IgnoreCase)))
				{
					bPng = false;
				}
				const int32 Seed = (Args.Num() > 2) ? FCString::Atoi(*Args[2]) : -1;
				const int32 MaxFrames = (Args.Num() > 3) ? FCString::Atoi(*Args[3]) : 0;
				Cap->StartRun(Dir, bPng, Seed, MaxFrames);
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

#endif
