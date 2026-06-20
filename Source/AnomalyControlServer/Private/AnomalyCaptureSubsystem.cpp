// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#include "AnomalyCaptureSubsystem.h"

#include "AnomalyControlServerLog.h"
#include "Engine/World.h"
#include "Engine/GameViewportClient.h"
#include "HAL/IConsoleManager.h"
#include "Misc/Paths.h"
#include "Misc/DateTime.h"
#include "CoreGlobals.h"                 // GFrameCounter

#if ANOMALY_CONTROL_SERVER
#include "AnomalyLabelWriter.h"
#include "AnomalyPreviewCapture.h"
#include "AnomalyAutoInjectorSubsystem.h"
#endif

// ----------------------------------------------------------------------------------------------------
// USubsystem / UWorldSubsystem / FTickableGameObject
// ----------------------------------------------------------------------------------------------------

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
#if ANOMALY_CONTROL_SERVER
	UE_LOG(LogAnomalyServer, Log, TEXT("AnomalyCapture subsystem initialized (idle — use IAI.Capture.Start)."));
#else
	UE_LOG(LogAnomalyServer, Log, TEXT("AnomalyCapture subsystem initialized (compiled out: ANOMALY_CONTROL_SERVER=0)."));
#endif
}

void UAnomalyCaptureSubsystem::Deinitialize()
{
	StopRun();   // clean teardown: revert any in-flight fire + finalize the run (S5)
	Super::Deinitialize();
}

void UAnomalyCaptureSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
#if ANOMALY_CONTROL_SERVER
	if (!bRunning)
	{
		return;
	}

	// Sample the current view EVERY running tick (incl. pre-roll + settle frames) so the ring is continuously
	// populated; a captured frame then projects with the view from L ticks ago (S3 — the render trails the
	// game thread, so that older view matches the pixels ReadPixels just returned).
	SampleViewThisTick();

	// One frame per tick. Settle phases skip capture; the rest capture exactly one labeled frame. The fire/
	// revert actions happen at phase boundaries (BeginFire / BeginRevert) so the symmetric K-frame settle
	// brackets BOTH transitions (S1).
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
				FinishRun(/*bLogLine=*/true);
			}
			else
			{
				// The post-roll just emitted doubles as the next burst's pre-roll (S3 shared gap).
				BeginFire();
			}
		}
		break;

	default:
		break;
	}
#endif // ANOMALY_CONTROL_SERVER
}

// ----------------------------------------------------------------------------------------------------
// Console-driven control
// ----------------------------------------------------------------------------------------------------

void UAnomalyCaptureSubsystem::SetBurstConfig(int32 K, int32 Pre, int32 Positive, int32 Post, int32 Bursts)
{
	if (bRunning)
	{
		UE_LOG(LogAnomalyServer, Warning, TEXT("IAI.Capture.Config: ignored mid-run (stop first)."));
		return;
	}
	SettleFrames    = FMath::Max(0, K);
	PreFrames       = FMath::Max(0, Pre);
	PositiveFrames  = FMath::Max(0, Positive);
	PostFrames      = FMath::Max(0, Post);
	BurstCount      = FMath::Max(0, Bursts);
	UE_LOG(LogAnomalyServer, Log, TEXT("IAI.Capture.Config: K=%d pre=%d positive=%d post=%d bursts=%d."),
		SettleFrames, PreFrames, PositiveFrames, PostFrames, BurstCount);
}

void UAnomalyCaptureSubsystem::SetViewLag(int32 L)
{
	if (bRunning)
	{
		UE_LOG(LogAnomalyServer, Warning, TEXT("IAI.Capture.ViewLag: ignored mid-run (stop first)."));
		return;
	}
	ViewLagFrames = FMath::Max(0, L);
	UE_LOG(LogAnomalyServer, Log, TEXT("IAI.Capture.ViewLag: L=%d frame(s)."), ViewLagFrames);
}

void UAnomalyCaptureSubsystem::StartRun(const FString& BaseDir, bool bPng, int32 InSeed)
{
#if ANOMALY_CONTROL_SERVER
	if (bRunning)
	{
		UE_LOG(LogAnomalyServer, Warning, TEXT("IAI.Capture.Start: already running (stop first)."));
		return;
	}

	UWorld* World = GetWorld();
	UAnomalyAutoInjectorSubsystem* Auto = ResolveAuto();
	if (!World || !Auto)
	{
		UE_LOG(LogAnomalyServer, Warning, TEXT("IAI.Capture.Start: no world / no auto-injector (run inside a Game/PIE world)."));
		return;
	}

	// A2: capture OWNS firing via the deterministic core. If the auto-injector's own Run loop is on it would
	// fire/revert concurrently and break one-per-burst. Warn, do NOT block (m6 precedent).
	if (Auto->IsRunning())
	{
		UE_LOG(LogAnomalyServer, Warning,
			TEXT("IAI.Capture.Start: the auto-injector's Run is ON — capture drives firing itself; "
			     "'capture run + Auto.Run' is UNSUPPORTED. Recommend IAI.Auto.Run 0."));
	}

	// S4 reproducibility: seed the stream at run-start. InSeed < 0 keeps the auto-injector's current seed but
	// re-initializes the stream to its start (SetSeed re-inits), so two same-seed fixed-vantage runs match.
	Seed = (InSeed >= 0) ? InSeed : Auto->GetSeed();
	Auto->SetSeed(Seed);

	// Clean slate so the lead-in pre-roll is genuinely negative.
	Auto->RevertAllLiveFires();

	bFormatPng = bPng;

	const FString Base = BaseDir.IsEmpty()
		? FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("AnomalyCaptures"))
		: BaseDir;
	const FString Stamp = FDateTime::Now().ToString(TEXT("%Y%m%d-%H%M%S"));
	RunDir = FPaths::Combine(Base, FString::Printf(TEXT("run_%d_%s"), Seed, *Stamp));

	int32 VW = 0, VH = 0;
	if (UGameViewportClient* GV = World->GetGameViewport())
	{
		FVector2D Size = FVector2D::ZeroVector;
		GV->GetViewportSize(Size);
		VW = (int32)Size.X;
		VH = (int32)Size.Y;
	}

	StartFrame = GFrameCounter;

	AnomalyLabel::FRunManifest M;
	M.Seed = Seed;
	M.SettleFrames = SettleFrames;
	M.ViewLagFrames = ViewLagFrames;
	M.PreFrames = PreFrames;
	M.PositiveFrames = PositiveFrames;
	M.PostFrames = PostFrames;
	M.BurstCount = BurstCount;
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
	ViewRing.Reset();   // fresh per run

	bRunning = true;
	Phase = ECapturePhase::LeadIn;
	PhaseFramesLeft = PreFrames;

	UE_LOG(LogAnomalyServer, Log,
		TEXT("=== Capture run STARTED: %s | seed=%d fmt=%s | K=%d L=%d pre=%d positive=%d post=%d bursts=%s ==="),
		*RunDir, Seed, *M.Format, SettleFrames, ViewLagFrames, PreFrames, PositiveFrames, PostFrames,
		BurstCount > 0 ? *FString::FromInt(BurstCount) : TEXT("until-stop"));
#else
	UE_LOG(LogAnomalyServer, Warning, TEXT("IAI.Capture.Start: capture compiled out (ANOMALY_CONTROL_SERVER=0)."));
#endif
}

void UAnomalyCaptureSubsystem::StopRun()
{
#if ANOMALY_CONTROL_SERVER
	if (!bRunning)
	{
		return;
	}
	FinishRun(/*bLogLine=*/true);
#endif
}

void UAnomalyCaptureSubsystem::LogStatus() const
{
#if ANOMALY_CONTROL_SERVER
	if (!bRunning)
	{
		UE_LOG(LogAnomalyServer, Log,
			TEXT("Capture: idle. Config K=%d L=%d pre=%d positive=%d post=%d bursts=%s. Start: IAI.Capture.Start [dir] [png|jpeg] [seed]."),
			SettleFrames, ViewLagFrames, PreFrames, PositiveFrames, PostFrames,
			BurstCount > 0 ? *FString::FromInt(BurstCount) : TEXT("until-stop"));
		return;
	}
	UE_LOG(LogAnomalyServer, Log,
		TEXT("Capture: RUNNING %s | seed=%d | burst %d%s | frames=%d (positive=%d) | zero-match bursts=%d"),
		*RunDir, Seed, BurstsDone + 1,
		BurstCount > 0 ? *FString::Printf(TEXT("/%d"), BurstCount) : TEXT(""),
		FramesWritten, PositiveFramesWritten, ZeroMatchBursts);
#else
	UE_LOG(LogAnomalyServer, Log, TEXT("Capture: compiled out (ANOMALY_CONTROL_SERVER=0)."));
#endif
}

#if ANOMALY_CONTROL_SERVER

// ----------------------------------------------------------------------------------------------------
// Internals
// ----------------------------------------------------------------------------------------------------

UAnomalyAutoInjectorSubsystem* UAnomalyCaptureSubsystem::ResolveAuto() const
{
	UWorld* World = GetWorld();
	return World ? World->GetSubsystem<UAnomalyAutoInjectorSubsystem>() : nullptr;
}

void UAnomalyCaptureSubsystem::SampleViewThisTick()
{
	FAnomalyViewInfo V;
	AnomalyViewport::GetActiveViewInfo(GetWorld(), V);   // bValid may be false (no live view) — stored as-is
	ViewRing.Add(V);

	// Keep only L + a small margin (newest last). RemoveAt(0) is O(n) but n is tiny (~3).
	const int32 MaxDepth = ViewLagFrames + 2;
	while (ViewRing.Num() > MaxDepth)
	{
		ViewRing.RemoveAt(0);
	}
}

FAnomalyViewInfo UAnomalyCaptureSubsystem::ProjectionView() const
{
	// The view from L ticks back (matches the captured pixels). At run start the ring isn't yet L+1 deep —
	// those first frames fall back to the most recent view; they are pre-roll negatives (no fire => no bbox),
	// so the fallback never mislabels a positive box. Empty ring => default (invalid) => bbox_valid=false.
	const int32 Idx = ViewRing.Num() - 1 - ViewLagFrames;
	if (ViewRing.IsValidIndex(Idx))
	{
		return ViewRing[Idx];
	}
	return ViewRing.Num() > 0 ? ViewRing.Last() : FAnomalyViewInfo{};
}

void UAnomalyCaptureSubsystem::BeginFire()
{
	UAnomalyAutoInjectorSubsystem* Auto = ResolveAuto();
	const bool bFired = Auto ? Auto->TryFireOnce() : false;
	if (!bFired)
	{
		// A6: empty Eligible/Candidates/visible-set => this burst is negatives only. The m6 draw protocol
		// already spent its draws, so determinism holds; advance.
		++ZeroMatchBursts;
		UE_LOG(LogAnomalyServer, Log, TEXT("Capture: burst %d fired nothing (zero-match / empty) — negatives only."),
			BurstsDone + 1);
	}
	Phase = ECapturePhase::SettleAfterFire;
	PhaseFramesLeft = SettleFrames;
}

void UAnomalyCaptureSubsystem::BeginRevert()
{
	if (UAnomalyAutoInjectorSubsystem* Auto = ResolveAuto())
	{
		Auto->RevertAllLiveFires();   // S2: keep GetLiveFires accurate so the post-roll labels clean
	}
	Phase = ECapturePhase::SettleAfterRevert;
	PhaseFramesLeft = SettleFrames;
}

void UAnomalyCaptureSubsystem::CaptureCurrentFrame()
{
	UWorld* World = GetWorld();
	const AnomalyPreview::EImageFormat Format =
		bFormatPng ? AnomalyPreview::EImageFormat::PNG : AnomalyPreview::EImageFormat::JPEG;

	// Classify the frame by the game-thread ground truth at THIS tick (accurate even on a zero-match burst,
	// where the "positive" phase has no live fire).
	const UAnomalyAutoInjectorSubsystem* Auto = ResolveAuto();
	const bool bPositive = Auto && Auto->GetLiveFireCount() > 0;

	// Project the bbox with the view from L frames ago (matches the ReadPixels frame), NOT the current view.
	const FAnomalyViewInfo ProjView = ProjectionView();

	FString ImagePath, SidecarPath;
	int32 NumLabels = 0;
	if (AnomalyLabel::CaptureLabeledShot(World, RunDir, Format, ProjView, ImagePath, SidecarPath, NumLabels, /*bLog=*/false))
	{
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
		Auto->RevertAllLiveFires();   // leave the world clean (S5)
	}

	AnomalyLabel::WriteRunSummary(RunDir, FramesWritten, PositiveFramesWritten, BurstsDone, ZeroMatchBursts, GFrameCounter);

	if (bLogLine)
	{
		UE_LOG(LogAnomalyServer, Log,
			TEXT("=== Capture run FINISHED: %s | %d frame(s) (positive=%d) | %d burst(s), %d zero-match ==="),
			*RunDir, FramesWritten, PositiveFramesWritten, BurstsDone, ZeroMatchBursts);
	}

	bRunning = false;
	Phase = ECapturePhase::Idle;
	PhaseFramesLeft = 0;
}

// ----------------------------------------------------------------------------------------------------
// IAI.Capture.* console surface (module-scoped, mirroring the injector/auto pattern)
// ----------------------------------------------------------------------------------------------------

namespace
{
	UAnomalyCaptureSubsystem* ResolveCapture(UWorld* World)
	{
		if (!World)
		{
			UE_LOG(LogAnomalyServer, Warning, TEXT("No world for this command; run it from a Game/PIE world."));
			return nullptr;
		}
		UAnomalyCaptureSubsystem* Cap = World->GetSubsystem<UAnomalyCaptureSubsystem>();
		if (!Cap)
		{
			UE_LOG(LogAnomalyServer, Warning, TEXT("AnomalyCapture subsystem not present for world '%s'."), *GetNameSafe(World));
		}
		return Cap;
	}
}

static FAutoConsoleCommandWithWorldAndArgs GCaptureStartCmd(
	TEXT("IAI.Capture.Start"),
	TEXT("Start a labeled burst-capture run. Usage: IAI.Capture.Start [outDir] [png|jpeg] [seed]  ")
	TEXT("(default dir <ProjectSaved>/AnomalyCaptures; png; seed = auto-injector's current). The auto-injector's "
	     "Run must be OFF (capture drives firing). Configure bursts first with IAI.Capture.Config."),
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
				Cap->StartRun(Dir, bPng, Seed);
			}
		}));

static FAutoConsoleCommandWithWorldAndArgs GCaptureStopCmd(
	TEXT("IAI.Capture.Stop"),
	TEXT("Stop the capture run (reverts any in-flight fire, writes run_summary.json)."),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& /*Args*/, UWorld* World)
		{
			if (UAnomalyCaptureSubsystem* Cap = ResolveCapture(World)) { Cap->StopRun(); }
		}));

static FAutoConsoleCommandWithWorldAndArgs GCaptureStatusCmd(
	TEXT("IAI.Capture.Status"),
	TEXT("Log capture run state, config, and counters."),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& /*Args*/, UWorld* World)
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
				UE_LOG(LogAnomalyServer, Warning, TEXT("Usage: IAI.Capture.Config <settleK> <preFrames> <positiveFrames> <postFrames> <burstCount>"));
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
	TEXT("Set the bbox-projection view-lag L in frames (default 1 = r.OneFrameThreadLag; corrects the box ")
	TEXT("offset under camera motion). Tune: box trails the object => raise L, leads => lower L. Usage: IAI.Capture.ViewLag <frames>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() < 1)
			{
				UE_LOG(LogAnomalyServer, Warning, TEXT("Usage: IAI.Capture.ViewLag <frames>"));
				return;
			}
			if (UAnomalyCaptureSubsystem* Cap = ResolveCapture(World)) { Cap->SetViewLag(FCString::Atoi(*Args[0])); }
		}));

#endif // ANOMALY_CONTROL_SERVER
