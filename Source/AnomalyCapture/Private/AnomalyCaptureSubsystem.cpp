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
#include "AnomalyPreviewTee.h"
#include "AnomalyAutoInjectorSubsystem.h"
#include "AnomalyInjectorSubsystem.h"
#include "AnomalyFrameCapturer.h"
#include "AnomalySveCapturer.h"
#include "AnomalySceneViewExtension.h"
#include "AnomalyMaskSceneViewExtension.h"
#include "AnomalyMaskMeasure.h"
#include "AnomalyStencilTag.h"
#include "AnomalySveKeyRing.h"
#include "AnomalyAsyncWriter.h"
#include "Misc/CoreDelegates.h"
#include "SceneViewExtension.h"
#include "HAL/PlatformProcess.h"
#include "HAL/PlatformTime.h"
#include "RenderingThread.h"
#include "Modules/ModuleManager.h"

#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Misc/EngineVersion.h"
#include "Misc/App.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SkinnedMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"
#include "MaterialShared.h"

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

	FString NodeAssetName;
	FString NodeComponentClass;
	FVector NodeBoundsOrigin = FVector::ZeroVector;
	FVector NodeBoundsExtent = FVector::ZeroVector;

	FSelectionProvenance Provenance;

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
	TMap<int32, uint8> HiddenByIndex;
};
#endif

struct FAnomalyCaptureAsyncState
{
#if ANOMALY_CAPTURE
	TSharedPtr<FAnomalyFrameCapturer, ESPMode::ThreadSafe> Capturer;
	TSharedPtr<FAnomalySveCapturer, ESPMode::ThreadSafe> SveCapturer;
	TSharedPtr<FAnomalySceneViewExtension, ESPMode::ThreadSafe> SveExtension;
	TSharedPtr<FAnomalyMaskSceneViewExtension, ESPMode::ThreadSafe> MaskExtension;
	TSharedPtr<FAnomalyAsyncWriter, ESPMode::ThreadSafe> Writer;
	TMap<uint64, AnomalyLabel::FCaptureSnapshot> PendingSnapshots;
	TArray<FSessionEventAccum> SessionEvents;
	FAnomalyMaskMeasure MaskMeasure;
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

	void ResolveNodeIdentity(const AActor* Actor, FString& OutAssetName, FString& OutComponentClass,
		FVector& OutBoundsOrigin, FVector& OutBoundsExtent)
	{
		OutAssetName.Reset();
		OutComponentClass.Reset();
		OutBoundsOrigin = FVector::ZeroVector;
		OutBoundsExtent = FVector::ZeroVector;
		if (!Actor)
		{
			return;
		}

		TArray<UMeshComponent*> Meshes;
		Actor->GetComponents<UMeshComponent>(Meshes);
		for (UMeshComponent* Mesh : Meshes)
		{
			if (!Mesh || !Mesh->IsVisible())
			{
				continue;
			}
			if (const UStaticMeshComponent* SM = Cast<UStaticMeshComponent>(Mesh))
			{
				if (const UStaticMesh* Asset = SM->GetStaticMesh())
				{
					OutAssetName = Asset->GetName();
				}
			}
			else if (const USkinnedMeshComponent* SK = Cast<USkinnedMeshComponent>(Mesh))
			{
				if (const UObject* Asset = SK->GetSkinnedAsset())
				{
					OutAssetName = Asset->GetName();
				}
			}
			OutComponentClass = Mesh->GetClass()->GetName();
			if (!OutAssetName.IsEmpty())
			{
				break;
			}
		}

		const FBox Box = Actor->GetComponentsBoundingBox(true);
		if (Box.IsValid)
		{
			OutBoundsOrigin = Box.GetCenter();
			OutBoundsExtent = Box.GetExtent();
		}
	}

	void MapAnomalyToClient(FName Id, FString& OutType, FString& OutSubtype)
	{
		if (Id == FName(TEXT("blinking")))
		{
			OutType = TEXT("blink");
			OutSubtype = TEXT("disappear_reappear");
		}
		else
		{
			OutType = Id.ToString();
			OutSubtype = Id.ToString();
		}
	}

	bool IsHideTypeAnomaly(FName Id, bool& bOutKnownId)
	{
		static const TSet<FName> HideTypeIds = {
			FName(TEXT("blinking")),
			FName(TEXT("missing_object"))
		};
		static const TSet<FName> NonHideTypeIds = {
			FName(TEXT("missing_texture")),
			FName(TEXT("corrupted_texture")),
			FName(TEXT("lighting_mismatch")),
			FName(TEXT("lod_corruption")),
			FName(TEXT("lod_popping")),
			FName(TEXT("camera_clipping")),
			FName(TEXT("time_dilation"))
		};
		bOutKnownId = HideTypeIds.Contains(Id) || NonHideTypeIds.Contains(Id);
		return HideTypeIds.Contains(Id);
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
	MaskEndFrameHandle = FCoreDelegates::OnEndFrame.AddUObject(this, &UAnomalyCaptureSubsystem::OnEndFrameMaskSample);
	MaskWorldTickEndHandle = FWorldDelegates::OnWorldTickEnd.AddUObject(this, &UAnomalyCaptureSubsystem::OnWorldTickEndMask);
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
	bool bConfigSve = false;
	bool bSveFromIni = false;
	if (GConfig && GConfig->GetBool(TEXT("AnomalyCapture"), TEXT("bSveCaptureDefault"), bConfigSve, GGameIni))
	{
		bSveCapture = bConfigSve;
		bSveFromIni = true;
	}
	bool bConfigMask = false;
	if (GConfig && GConfig->GetBool(TEXT("AnomalyCapture"), TEXT("bMaskMeasureDefault"), bConfigMask, GGameIni))
	{
		bMaskMeasure = bConfigMask;
		bMaskMeasureFromIni = true;
	}
	int32 ConfigOutputHeight = 0;
	if (GConfig && GConfig->GetInt(TEXT("AnomalyCapture"), TEXT("CaptureOutputHeightDefault"), ConfigOutputHeight, GGameIni))
	{
		OutputHeightIni = ConfigOutputHeight;
		bOutputHeightFromIni = true;
	}
	UE_LOG(LogAnomalyCapture, Log, TEXT("AnomalyCapture subsystem initialized (idle — use IAI.Capture.Start). Delivery mode: %s. Content clock: %s. Focus gate: %s. Grab point: %s (%s), default from %s."),
		bDeliveryMode ? TEXT("ON (client-facing output only)") : TEXT("off (full fidelity)"),
		ContentClock == EContentClock::Game ? TEXT("game (stamp target fps)") : TEXT("wall (stamp sustained on slow runs)"),
		bFocusGate ? TEXT("on (start waits for game-window focus)") : TEXT("off (start begins immediately)"),
		DescribeGrabPoint(),
		bSveCapture ? TEXT("scene colour, pre-Slate — UI EXCLUDED") : TEXT("presented backbuffer — UI INCLUDED"),
		bSveFromIni
			? TEXT("DefaultGame.ini [AnomalyCapture] bSveCaptureDefault")
			: TEXT("S4 COMPILED-IN DEFAULT (SVE, UI-free); no ini key present; IAI.Capture.SVE 0 selects the backbuffer/UI-on path"));
	UE_LOG(LogAnomalyCapture, Log,
		TEXT("Capture(mask): m27 EFFECTIVE AT INIT - mask %s, default from %s. Mask off means the m26 H5 cure ")
		TEXT("is INACTIVE and this build labels exactly as m25 did."),
		bMaskMeasure ? TEXT("ON (measure, report and veto)") : TEXT("off"),
		bMaskMeasureFromIni
			? TEXT("DefaultGame.ini [AnomalyCapture] bMaskMeasureDefault")
			: TEXT("COMPILED DEFAULT (off); no ini key present; IAI.Capture.Mask 1 enables it for the session"));
	UE_LOG(LogAnomalyCapture, Log,
		TEXT("Capture(m28): OUTPUT HEIGHT AT INIT - ini level %s. 0 means NATIVE and the written frames are ")
		TEXT("byte-identical to a pre-m28 build. This is the INI LEVEL ONLY; a console override or a per-run ")
		TEXT("argument can still beat it, and the EFFECTIVE value for a run is echoed at IAI.Capture.Start."),
		bOutputHeightFromIni
			? *FString::Printf(TEXT("%d, from DefaultGame.ini [AnomalyCapture] CaptureOutputHeightDefault"), OutputHeightIni)
			: TEXT("not set; no ini key present, so the compiled default 0 (native) stands unless overridden"));
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
	if (MaskEndFrameHandle.IsValid())
	{
		FCoreDelegates::OnEndFrame.Remove(MaskEndFrameHandle);
		MaskEndFrameHandle.Reset();
	}
	if (MaskWorldTickEndHandle.IsValid())
	{
		FWorldDelegates::OnWorldTickEnd.Remove(MaskWorldTickEndHandle);
		MaskWorldTickEndHandle.Reset();
	}
	PreviewTee.Reset();
	if (Async.IsValid())
	{
		Async->PendingSnapshots.Empty();
		if (Async->Capturer.IsValid())
		{
			Async->Capturer->UnregisterBackbufferHook();
			Async->Capturer.Reset();
		}
		if (Async->SveCapturer.IsValid())
		{
			Async->SveCapturer->SetActive(false);
		}
		if (Async->SveExtension.IsValid())
		{
			Async->SveExtension.Reset();
			FlushRenderingCommands();
		}
		Async->SveCapturer.Reset();
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

	SampleDeferredHidden();

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

void UAnomalyCaptureSubsystem::OnWorldTickEndMask(UWorld* World, ELevelTick TickType, float DeltaSeconds)
{
#if ANOMALY_CAPTURE
	if (World != GetWorld() || !bMaskMeasure || !bRunning || !Async.IsValid() || !Async->MaskExtension.IsValid())
	{
		return;
	}

	Async->MaskMeasure.VerifyPendingTags();
	Async->MaskExtension->EnqueueDrain();
	Async->MaskMeasure.CollectResults(Async->MaskExtension.Get());
	const bool bArmedNormal = Async->MaskMeasure.ArmIfMeasurable(Async->MaskExtension.Get(), GFrameCounter);

	if (!bArmedNormal && bMaskProbe && !bMaskProbeFiredThisRun && !bDeliveryMode)
	{
		if (Async->MaskMeasure.ArmProbeOnHidden(Async->MaskExtension.Get(), GFrameCounter))
		{
			bMaskProbeFiredThisRun = true;
		}
	}
#endif
}

void UAnomalyCaptureSubsystem::OnEndFrameMaskSample()
{
#if ANOMALY_CAPTURE
	if (!bMaskMeasure || !bRunning || !Async.IsValid())
	{
		return;
	}
	Async->MaskMeasure.SampleEndOfFrame();
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

void UAnomalyCaptureSubsystem::SetSveCapture(bool bInSve)
{
	if (bRunning)
	{
		UE_LOG(LogAnomalyCapture, Warning, TEXT("IAI.Capture.SVE: ignored mid-run (stop first)."));
		return;
	}
	bSveCapture = bInSve;
	UE_LOG(LogAnomalyCapture, Log, TEXT("IAI.Capture.SVE: %s (effective=%d) (%s)."),
		bSveCapture ? TEXT("ON") : TEXT("OFF"),
		bSveCapture ? 1 : 0,
		bSveCapture
			? TEXT("B' scene-view-extension grab: scene colour after tonemap and BEFORE Slate, so the frame is UI-free; frame/state keyed by identity through the view-family ring, not by arm-to-present order")
			: TEXT("backbuffer grab (default): the presented frame including game UI, keyed by arm-to-present pairing"));
}

void UAnomalyCaptureSubsystem::SetMaskMeasure(bool bInMask)
{
	if (bRunning)
	{
		UE_LOG(LogAnomalyCapture, Warning, TEXT("IAI.Capture.Mask: ignored - a capture run is in progress."));
		return;
	}
	bMaskMeasure = bInMask;
	bMaskMeasureFromIni = false;
	UE_LOG(LogAnomalyCapture, Log,
		TEXT("IAI.Capture.Mask: %s - THE BISECT SWITCH, and it takes effect BETWEEN RUNS, not mid-run. It gates ")
		TEXT("m26 slices 1+2+3 together: MEASURE, mask{provided}, and the VETO. Setting it 0 and re-capturing ")
		TEXT("returns the build to m25 labelling behaviour in about thirty seconds, with no rebuild. It overrides ")
		TEXT("DefaultGame.ini [AnomalyCapture] bMaskMeasureDefault for this session. The full banner prints at ")
		TEXT("IAI.Capture.Start."),
		bMaskMeasure ? TEXT("ON") : TEXT("off"));
}

void UAnomalyCaptureSubsystem::SetMaskProbe(bool bInProbe)
{
	if (bRunning)
	{
		UE_LOG(LogAnomalyCapture, Warning, TEXT("IAI.Capture.MaskProbe: ignored - a capture run is in progress."));
		return;
	}
	bMaskProbe = bInProbe;
	UE_LOG(LogAnomalyCapture, Warning,
		TEXT("IAI.Capture.MaskProbe: %s (GATE ARTEFACT, default OFF - one deliberate KNOWN-HIDDEN arm per run ")
		TEXT("to prove the 255 detector, the end-of-frame confirmation and the frame-scoped discard are LIVE. ")
		TEXT("LOCK-1 is bypassed for that ONE arm. INERT IN DELIVERY MODE BY GUARD regardless of this flag. ")
		TEXT("MUST be OFF in any build that ships - see docs/PRE-DELIVERY-CHECKLIST.md)."),
		bMaskProbe ? TEXT("ON") : TEXT("off"));
}

void UAnomalyCaptureSubsystem::SetOutputHeightOverride(int32 InHeight)
{
	if (bRunning)
	{
		UE_LOG(LogAnomalyCapture, Warning, TEXT("IAI.Capture.OutputHeight: ignored mid-run (stop first)."));
		return;
	}
	OutputHeightOverride = (InHeight < 0) ? -1 : InHeight;
	UE_LOG(LogAnomalyCapture, Log,
		TEXT("IAI.Capture.OutputHeight: EFFECTIVE READ-BACK = %s. This is the BETWEEN-RUNS override (precedence ")
		TEXT("level 2). It BEATS the ini and is BEATEN by a per-run argument. -1 means CLEARED, which is NOT the ")
		TEXT("same as 0: cleared falls through to the ini or the compiled default, while 0 is a deliberate ")
		TEXT("request for NATIVE that overrides the ini."),
		(OutputHeightOverride < 0)
			? TEXT("-1 (cleared - falls through to the ini / compiled default)")
			: *FString::Printf(TEXT("%d"), OutputHeightOverride));
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
	const FString& InTargetAnomaly, const FString& InTargetActor, const TArray<FString>& InTargetArgs,
	int32 InOutputHeight)
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

	FString CleanBaseDir = BaseDir.TrimStartAndEnd();
	if (CleanBaseDir.Len() >= 2
		&& ((CleanBaseDir.StartsWith(TEXT("\"")) && CleanBaseDir.EndsWith(TEXT("\"")))
			|| (CleanBaseDir.StartsWith(TEXT("'")) && CleanBaseDir.EndsWith(TEXT("'")))))
	{
		CleanBaseDir = CleanBaseDir.Mid(1, CleanBaseDir.Len() - 2).TrimStartAndEnd();
	}
	if (CleanBaseDir.Contains(TEXT("\"")))
	{
		UE_LOG(LogAnomalyCapture, Error,
			TEXT("IAI.Capture.Start: CAP-RUNDIR-REFUSED outDir %s still contains a quote character after unwrapping — ")
			TEXT("REFUSING TO START rather than failing silently at annotation-write time. Pass the path without ")
			TEXT("embedded quotes."),
			*CleanBaseDir);
		return;
	}

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
	TargetAnomalyArgs = bTargetedMode ? InTargetArgs : TArray<FString>();
	if ((bHasAnomaly || bHasActor) && !bTargetedMode)
	{
		UE_LOG(LogAnomalyCapture, Warning,
			TEXT("IAI.Capture.Start: targeted mode needs BOTH an anomaly and a target actor (got only one) — falling back to auto-pool."));
	}

	bFormatPng = bPng;
	FrameCap = FMath::Max(0, InFrameCap);

	if (InOutputHeight >= 0)
	{
		EffectiveOutputHeight = InOutputHeight;
		OutputHeightSource = EOutputHeightSource::PerRun;
	}
	else if (OutputHeightOverride >= 0)
	{
		EffectiveOutputHeight = OutputHeightOverride;
		OutputHeightSource = EOutputHeightSource::Override;
	}
	else if (bOutputHeightFromIni)
	{
		EffectiveOutputHeight = OutputHeightIni;
		OutputHeightSource = EOutputHeightSource::Ini;
	}
	else
	{
		EffectiveOutputHeight = 0;
		OutputHeightSource = EOutputHeightSource::CompiledDefault;
	}
	EffectiveOutputHeight = FMath::Max(0, EffectiveOutputHeight);

	const FString Base = CleanBaseDir.IsEmpty()
		? FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("AnomalyCaptures"))
		: CleanBaseDir;
	const FString Stamp = FDateTime::Now().ToString(TEXT("%Y%m%d-%H%M%S"));
	const FString BaseId = FString::Printf(TEXT("session_%s"), *Stamp);
	SessionId = BaseId;
	RunDir = FPaths::Combine(Base, SessionId);
	for (int32 Disamb = 2; IFileManager::Get().DirectoryExists(*RunDir); ++Disamb)
	{
		SessionId = FString::Printf(TEXT("%s-%d"), *BaseId, Disamb);
		RunDir = FPaths::Combine(Base, SessionId);
	}

	if (!IFileManager::Get().MakeDirectory(*FPaths::Combine(RunDir, TEXT("Actual_Frames")), true))
	{
		UE_LOG(LogAnomalyCapture, Error,
			TEXT("IAI.Capture.Start: CAP-RUNDIR-REFUSED could not create run directory %s — REFUSING TO START ")
			TEXT("rather than failing silently at annotation-write time. Check the outDir argument."),
			*RunDir);
		RunDir.Reset();
		SessionId.Reset();
		if (bAutoWasRunning)
		{
			Auto->SetRunning(true);
			bAutoWasRunning = false;
		}
		return;
	}
	LastRunDir = RunDir;

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
	NonManifestedEvents = 0;
	VetoedEvents = 0;
	SessionFrameIndex = 0;
	SyncResamplesPerformed = 0;
	SyncFirstWrittenW = 0;
	SyncFirstWrittenH = 0;
	SyncDimMismatches = 0;
	bLoggedFirstFrameMeasuredLine = false;
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
	bRectDeltaLogged = false;

	if (bSveCapture && Async.IsValid() && Async->SveCapturer.IsValid())
	{
		AnomalySveKeyRing::Reset();
		Async->SveCapturer->Reset();
		Async->SveCapturer->SetActive(true);
	}

	UE_LOG(LogAnomalyCapture, Log,
		TEXT("=== Capture(mask): EFFECTIVE FOR THIS RUN - mask %s, default from %s === READ THIS LINE, NOT THE ")
		TEXT("INI. Mask off means the m26 H5 cure is INACTIVE and this session labels exactly as m25 did. In a ")
		TEXT("packaged build the ini that counts is the COOKED DefaultGame.ini - a loose ini beside the package ")
		TEXT("is a SILENT NO-OP (G88), which is why this line reports the EFFECTIVE value and not the file."),
		bMaskMeasure ? TEXT("ON (measure, report and veto)") : TEXT("off"),
		bMaskMeasureFromIni
			? TEXT("DefaultGame.ini [AnomalyCapture] bMaskMeasureDefault")
			: TEXT("COMPILED DEFAULT (off) or IAI.Capture.Mask"));

	UE_LOG(LogAnomalyCapture, Log,
		TEXT("=== Capture(m28): EFFECTIVE FOR THIS RUN - requested output height %d, from %s === READ THIS LINE, ")
		TEXT("NOT THE INI. 0 means NATIVE: no resample runs and the written frames are byte-identical to a ")
		TEXT("pre-m28 build. The render is ALWAYS native - this resamples at the WRITE step only, so it cannot ")
		TEXT("affect selection, labelling geometry or the m26/m27 mask veto. Width is NEVER specified: it is ")
		TEXT("derived from each frame's own aspect, so a non-aspect-preserving output is unrepresentable. A ")
		TEXT("request at or above the frame's own height is NOT an upscale - it yields native. In a packaged ")
		TEXT("build the ini that counts is the COOKED DefaultGame.ini - a loose ini beside the package is a ")
		TEXT("SILENT NO-OP (G88), which is why this line reports the EFFECTIVE value and not the file. The ")
		TEXT("ACTUAL output size is measured from the first written frame and logged separately."),
		EffectiveOutputHeight, DescribeOutputHeightSource());

	if (bMaskMeasure && Async.IsValid())
	{
		if (Async->MaskExtension.IsValid())
		{
			Async->MaskExtension->Reset();
		}
		Async->MaskMeasure.BeginRun();
		bMaskProbeFiredThisRun = false;
		UE_LOG(LogAnomalyCapture, Log,
			TEXT("Capture(mask): m26 SLICES 1+2+3 ACTIVE - MEASURE, REPORT AND VETO. annotation.json's ")
			TEXT("ALREADY-SHIPPING mask{provided} carries the measurement tri-state's bool (NOT_MEASURED -> ")
			TEXT("false, MEASURED_ZERO/MEASURED_NONZERO -> true); NO sub-fields are added and depth{} is ")
			TEXT("untouched. THE VETO IS ZERO-ONLY: an event is removed from annotation.json IF AND ONLY IF it ")
			TEXT("is manifested AND its target was MEASURED at ZERO drawn pixels. NOT_MEASURED is never ")
			TEXT("vetoed, and a measured NON-ZERO count is never vetoed however small a fraction of its ")
			TEXT("claimed extent it is - there is NO ratio and NO threshold. The captured frames are NOT ")
			TEXT("un-written. Reserved stencil base %d, max %d arms/event."),
			AnomalyStencilTag::ReservedStencilBase, FAnomalyMaskMeasure::MaxArmsPerEvent);
		UE_LOG(LogAnomalyCapture, Log,
			TEXT("Capture(mask): probe EFFECTIVE=%d (flag=%d, deliveryMode=%d - the probe is INERT in ")
			TEXT("delivery mode by GUARD regardless of the flag; default OFF; gate use only)."),
			(bMaskProbe && !bDeliveryMode) ? 1 : 0, bMaskProbe ? 1 : 0, bDeliveryMode ? 1 : 0);
	}

	UE_LOG(LogAnomalyCapture, Log,
		TEXT("Capture: grab point EFFECTIVE = %s (sve=%d, sveCapturer=%d, sveExtension=%d, backbufferCapturer=%d, forceMiss=%d)."),
		bSveCapture ? TEXT("SVE/scene-colour (UI-free)") : TEXT("backbuffer (UI included)"),
		bSveCapture ? 1 : 0,
		(Async.IsValid() && Async->SveCapturer.IsValid()) ? 1 : 0,
		(Async.IsValid() && Async->SveExtension.IsValid()) ? 1 : 0,
		(Async.IsValid() && Async->Capturer.IsValid()) ? 1 : 0,
		AnomalySveKeyRing::GetForceMissMode());

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

	ApplySessionGlobals();

	UE_LOG(LogAnomalyCapture, Log,
		TEXT("=== Capture run STARTED: %s | mode=%s | delivery=%s | clock=%s | seed=%d fmt=%s capture=%s fps=%d(fixed-step%s) | K=%d L=%d pre=%d positive=%d post=%d bursts=%s frameCap=%s ==="),
		*RunDir,
		bTargetedMode ? *FString::Printf(TEXT("targeted[%s on %s]"), *TargetAnomalyId.ToString(), *TargetActorName) : TEXT("auto-pool"),
		bDeliveryMode ? TEXT("on") : TEXT("off"),
		ContentClock == EContentClock::Game ? TEXT("game") : TEXT("wall"),
		Seed, *M.Format, DescribeGrabPoint(), VideoFps,
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
	OutRunDir = LastRunDir;
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
			DescribeGrabPoint(),
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

const TCHAR* UAnomalyCaptureSubsystem::DescribeGrabPoint() const
{
	if (bSveCapture)
	{
		return TEXT("sve/scene-colour");
	}
	return bAsyncCapture ? TEXT("async/backbuffer") : TEXT("sync");
}

const TCHAR* UAnomalyCaptureSubsystem::DescribeOutputHeightSource() const
{
	switch (OutputHeightSource)
	{
	case EOutputHeightSource::PerRun:
		return TEXT("PER-RUN ARGUMENT (dashboard outputHeight / console Start oh=)");
	case EOutputHeightSource::Override:
		return TEXT("IAI.Capture.OutputHeight (between-runs override)");
	case EOutputHeightSource::Ini:
		return TEXT("DefaultGame.ini [AnomalyCapture] CaptureOutputHeightDefault");
	default:
		break;
	}
	return TEXT("COMPILED DEFAULT (0 = native); no ini key present, no override set, no per-run argument");
}

void UAnomalyCaptureSubsystem::LogFirstFrameMeasuredLine(int32 SrcW, int32 SrcH, int32 OutW, int32 OutH, bool bResampled)
{
	if (bLoggedFirstFrameMeasuredLine)
	{
		return;
	}
	bLoggedFirstFrameMeasuredLine = true;
	UE_LOG(LogAnomalyCapture, Log,
		TEXT("Capture(m28): MEASURED FROM THE FIRST WRITTEN FRAME - native %dx%d -> output %dx%d, resample %s. ")
		TEXT("This is the authoritative pair; it is what labels.jsonl width/height, every bbox_px and ")
		TEXT("annotation.video.resolution are computed from."),
		SrcW, SrcH, OutW, OutH,
		bResampled ? TEXT("YES") : TEXT("no - native, path unchanged"));
}

void UAnomalyCaptureSubsystem::NoteSyncWrittenSize(int32 W, int32 H, const FString& ImageRelPath)
{
	if (SyncFirstWrittenW <= 0 || SyncFirstWrittenH <= 0)
	{
		SyncFirstWrittenW = W;
		SyncFirstWrittenH = H;
		return;
	}
	if (SyncFirstWrittenW != W || SyncFirstWrittenH != H)
	{
		++SyncDimMismatches;
		UE_LOG(LogAnomalyCapture, Warning,
			TEXT("Capture(m28): FRAME DIMENSIONS CHANGED MID-RUN - '%s' was written at %dx%d but the FIRST written ")
			TEXT("frame of this session was %dx%d. annotation.json video.resolution reports the FIRST frame's ")
			TEXT("pair, so it does NOT describe this frame."),
			*ImageRelPath, W, H, SyncFirstWrittenW, SyncFirstWrittenH);
	}
}

void UAnomalyCaptureSubsystem::EnsureCapturer()
{
	if (!Async.IsValid())
	{
		return;
	}
	if (bSveCapture)
	{
		if (!Async->SveCapturer.IsValid())
		{
			Async->SveCapturer = MakeShared<FAnomalySveCapturer, ESPMode::ThreadSafe>();
		}
		if (!Async->SveExtension.IsValid())
		{
			Async->SveExtension = FSceneViewExtensions::NewExtension<FAnomalySceneViewExtension>(Async->SveCapturer);
		}
	}
	else if (!Async->Capturer.IsValid())
	{
		Async->Capturer = MakeShared<FAnomalyFrameCapturer, ESPMode::ThreadSafe>();
		Async->Capturer->RegisterBackbufferHook();
	}
	if (bMaskMeasure && !Async->MaskExtension.IsValid())
	{
		Async->MaskExtension = FSceneViewExtensions::NewExtension<FAnomalyMaskSceneViewExtension>();
	}
	if (!Async->Writer.IsValid())
	{
		Async->Writer = MakeShared<FAnomalyAsyncWriter, ESPMode::ThreadSafe>();
	}
	FModuleManager::Get().LoadModule(TEXT("ImageWrapper"));
}

void UAnomalyCaptureSubsystem::ProcessCompletedFrames()
{
	if (!Async.IsValid() || !Async->Writer.IsValid())
	{
		return;
	}

	const bool bUseSve = bSveCapture && Async->SveCapturer.IsValid();
	if (!bUseSve && !Async->Capturer.IsValid())
	{
		return;
	}

	if (bUseSve)
	{
		Async->SveCapturer->EnqueueDrain();
	}
	else
	{
		Async->Capturer->EnqueueDrain();
	}

	const AnomalyPreview::EImageFormat Format =
		bFormatPng ? AnomalyPreview::EImageFormat::PNG : AnomalyPreview::EImageFormat::JPEG;
	const TCHAR* Ext = bFormatPng ? TEXT("png") : TEXT("jpg");

	FAnomalyCapturedFrame Frame;
	while (bUseSve ? Async->SveCapturer->PopCompleted(Frame) : Async->Capturer->PopCompleted(Frame))
	{
		if (!bRectDeltaLogged)
		{
			bRectDeltaLogged = true;
			SWindow* DeltaWindow = nullptr;
			FIntRect SlateRect;
			const bool bHaveSlateRect = ComputeGameViewportCapture(GetWorld(), DeltaWindow, SlateRect);
			UE_LOG(LogAnomalyCapture, Log,
				TEXT("Capture: RESOLUTION DELTA (3-rect) — grab=%s | grabbed %dx%d | slate-window %s | viewport-size %dx%d ")
				TEXT("| dW_slate=%s dH_slate=%s | dW_vp=%d dH_vp=%d. ")
				TEXT("GRABBED is the delivered image (SVE takes the VIEW rect, backbuffer takes the Slate WINDOW rect); ")
				TEXT("VIEWPORT-SIZE is GetViewportSize() and is what annotation.video.resolution and run.json viewport report, ")
				TEXT("so a non-zero dW_vp/dH_vp means the reported resolution disagrees with the delivered pixels."),
				DescribeGrabPoint(),
				Frame.Width, Frame.Height,
				bHaveSlateRect ? *FString::Printf(TEXT("%dx%d"), SlateRect.Width(), SlateRect.Height()) : TEXT("unresolved"),
				ViewportW, ViewportH,
				bHaveSlateRect ? *FString::Printf(TEXT("%d"), Frame.Width - SlateRect.Width()) : TEXT("?"),
				bHaveSlateRect ? *FString::Printf(TEXT("%d"), Frame.Height - SlateRect.Height()) : TEXT("?"),
				Frame.Width - ViewportW,
				Frame.Height - ViewportH);
		}
		const AnomalyLabel::FCaptureSnapshot* Snap = Async->PendingSnapshots.Find(Frame.RequestId);
		if (!Snap)
		{
			UE_LOG(LogAnomalyCapture, Warning,
				TEXT("Capture(async): CAP-PAIR-DROP completed frame id=%llu has no pending snapshot — frame DROPPED, ")
				TEXT("never labelled by guess (pendingSnapshots=%d). This drop used to log at Verbose, which made a ")
				TEXT("broken pairing indistinguishable from a path that never submitted."),
				Frame.RequestId, Async->PendingSnapshots.Num());
			continue;
		}

		int32 OutW = Frame.Width;
		int32 OutH = Frame.Height;
		bool bNeedsResample = false;
		AnomalyLabel::DeriveOutputSize(Frame.Width, Frame.Height, EffectiveOutputHeight, OutW, OutH, bNeedsResample);
		LogFirstFrameMeasuredLine(Frame.Width, Frame.Height, OutW, OutH, bNeedsResample);

		const FString ImageName = FString::Printf(TEXT("Actual_Frames/frame_%05d.%s"), Snap->SessionIndex, Ext);
		int32 NumLabels = 0;
		const FString Record = AnomalyLabel::BuildLabelRecordForSnapshot(*Snap, OutW, OutH, ImageName, NumLabels);

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
		Job.OutWidth = OutW;
		Job.OutHeight = OutH;
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
	if (!Async.IsValid() || !Async->Writer.IsValid())
	{
		return;
	}

	const bool bUseSve = bSveCapture && Async->SveCapturer.IsValid();
	if (!bUseSve && !Async->Capturer.IsValid())
	{
		return;
	}

	const int32 PendingAtDrainEntry = Async->PendingSnapshots.Num();
	int32 IterationsConsumed = 0;

	for (int32 Iter = 0; Iter < 8 && Async->PendingSnapshots.Num() > 0; ++Iter)
	{
		if (bUseSve)
		{
			Async->SveCapturer->EnqueueDrain();
		}
		else
		{
			Async->Capturer->EnqueueDrain();
		}
		FlushRenderingCommands();
		ProcessCompletedFrames();
		++IterationsConsumed;
	}

	UE_LOG(LogAnomalyCapture, Log,
		TEXT("Capture(drain): M1 pendingAtDrainEntry=%d flushIterationsConsumed=%d pendingAfter=%d ")
		TEXT("(drainTailBudget=%d, stencilBranchHeldBudget=12)."),
		PendingAtDrainEntry, IterationsConsumed, Async->PendingSnapshots.Num(),
		FMath::Max(10, ViewLagFrames + 4));

	if (bUseSve && Async->SveCapturer.IsValid())
	{
		const FAnomalyReadbackLatencyStats Stats = Async->SveCapturer->GetLatencyStats();
		FString HistText;
		TArray<int32> Keys;
		Stats.Histogram.GetKeys(Keys);
		Keys.Sort();
		for (int32 Key : Keys)
		{
			HistText += FString::Printf(TEXT("%d:%d "), Key, Stats.Histogram[Key]);
		}
		UE_LOG(LogAnomalyCapture, Log,
			TEXT("Capture(drain): M1 readbackLatencyFrames samples=%d min=%d max=%d mean=%.3f notReadyPolls=%d hist=[%s]"),
			Stats.Samples,
			Stats.Samples > 0 ? Stats.MinFrames : -1,
			Stats.Samples > 0 ? Stats.MaxFrames : -1,
			Stats.Samples > 0 ? (double)Stats.SumFrames / (double)Stats.Samples : -1.0,
			Stats.NotReadyPolls,
			*HistText);
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
		? (bTargetedMode ? Auto->TryFireSpecific(TargetAnomalyId, TargetActorName, TargetAnomalyArgs) : Auto->TryFireOnce())
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

	const bool bUseSve = bSveCapture && Async.IsValid() && Async->SveCapturer.IsValid();

	if (bAsyncCapture && Async.IsValid() && (Async->Capturer.IsValid() || bUseSve))
	{
		SWindow* TargetWindow = nullptr;
		FIntRect CaptureRect;
		if (bUseSve || ComputeGameViewportCapture(World, TargetWindow, CaptureRect))
		{
			const uint64 RequestId = ++CaptureRequestSerial;
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
			Async->PendingSnapshots.Add(RequestId, MoveTemp(Snap));
			if (bUseSve)
			{
				Async->SveCapturer->ArmWanted(RequestId);
			}
			else
			{
				Async->Capturer->ArmForCapture(RequestId, TargetWindow, CaptureRect);
			}
			ArmedLabelRequestId = RequestId;
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
	int32 NativeW = 0, NativeH = 0, WrittenW = 0, WrittenH = 0;
	bool bResampled = false;
	const double NowWall = FPlatformTime::Seconds();
	if (AnomalyLabel::CaptureLabeledShot(World, RunDir, Format, ProjView, ImageName, SessionFrameIndex, NowWall,
		EffectiveOutputHeight, ImagePath, SidecarPath, NumLabels, NativeW, NativeH, WrittenW, WrittenH, bResampled,
		false, !bDeliveryMode))
	{
		if (bResampled)
		{
			++SyncResamplesPerformed;
		}
		LogFirstFrameMeasuredLine(NativeW, NativeH, WrittenW, WrittenH, bResampled);
		NoteSyncWrittenSize(WrittenW, WrittenH, ImageName);

		TArray<FAutoLiveFireInfo> Fires;
		if (Auto) { Fires = Auto->GetLiveFires(); }
		if (ActiveSessionGlobals.Num() > 0)
		{
			if (IsNearClipSlicingNow())
			{
				AppendSessionGlobalFires(Fires);
				++SessionGlobalPositiveFrames;
			}
			else
			{
				++SessionGlobalNegativeFrames;
			}
		}
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

void UAnomalyCaptureSubsystem::PreviewPump()
{
#if ANOMALY_CAPTURE
	if (PreviewTee.IsValid())
	{
		PreviewTee->Pump(IsCaptureActive());
	}
#endif
}

void UAnomalyCaptureSubsystem::PreviewArm(uint32 InViewEpoch)
{
#if ANOMALY_CAPTURE
	if (IsCaptureActive())
	{
		return;
	}

	UWorld* World = GetWorld();
	SWindow* TargetWindow = nullptr;
	FIntRect CaptureRect;
	if (!ComputeGameViewportCapture(World, TargetWindow, CaptureRect))
	{
		return;
	}

	if (!PreviewTee.IsValid())
	{
		PreviewTee = MakeUnique<FAnomalyPreviewTee>();
		UE_LOG(LogAnomalyCapture, Log, TEXT("Preview(tee): backbuffer preview armed for the first time (hook registered)."));
	}
	if (PreviewTee->IsBusy())
	{
		return;
	}
	PreviewTee->Arm(TargetWindow, CaptureRect, InViewEpoch);
#endif
}

bool UAnomalyCaptureSubsystem::PreviewPoll(TArray<uint8>& OutJpeg, int32& OutW, int32& OutH, uint32& OutEpoch)
{
#if ANOMALY_CAPTURE
	if (!PreviewTee.IsValid())
	{
		return false;
	}
	if (IsCaptureActive())
	{
		PreviewTee->DiscardReady();
		return false;
	}
	return PreviewTee->PollJpeg(OutJpeg, OutW, OutH, OutEpoch);
#else
	return false;
#endif
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
	AnomalyLabel::FCaptureSnapshot* Snap = Async->PendingSnapshots.Find(ArmedLabelRequestId);
	if (!Snap)
	{
		return;
	}

	if (const UAnomalyAutoInjectorSubsystem* Auto = ResolveAuto())
	{
		Snap->Fires = Auto->GetLiveFires();
	}
	if (ActiveSessionGlobals.Num() > 0)
	{
		if (IsNearClipSlicingNow())
		{
			AppendSessionGlobalFires(Snap->Fires);
			++SessionGlobalPositiveFrames;
		}
		else
		{
			++SessionGlobalNegativeFrames;
		}
	}
	Snap->FirePos.Reset();
	Snap->FirePos.Reserve(Snap->Fires.Num());
	for (const FAutoLiveFireInfo& F : Snap->Fires)
	{
		const AActor* FActor = F.TargetActor.Get();
		Snap->FirePos.Add(FActor ? FActor->GetActorLocation() : FVector::ZeroVector);
	}

	DeferredHiddenRequestId = ArmedLabelRequestId;
	bHasDeferredHidden = true;
}

void UAnomalyCaptureSubsystem::ApplySessionGlobals()
{
	ActiveSessionGlobals.Reset();
	SessionGlobalPositiveFrames = 0;
	SessionGlobalNegativeFrames = 0;
	SessionGlobalBaselineNearClip = GNearClippingPlane;

	if (bTargetedMode)
	{
		return;
	}

	UWorld* World = GetWorld();
	UAnomalyInjectorSubsystem* Injector = World ? World->GetSubsystem<UAnomalyInjectorSubsystem>() : nullptr;
	UAnomalyAutoInjectorSubsystem* Auto = ResolveAuto();
	if (!Injector || !Auto)
	{
		return;
	}

	for (const FAnomalyCatalogEntry& Entry : Injector->GetAnomalyCatalog())
	{
		if (Entry.Scope != EAnomalyScope::Global || !Auto->IsAnomalyEnabled(Entry.Id))
		{
			continue;
		}
		if (Injector->ApplyAnomaly(Entry.Id, TArray<FString>()))
		{
			ActiveSessionGlobals.Add(Entry.Id);
		}
		else
		{
			UE_LOG(LogAnomalyCapture, Warning,
				TEXT("Capture(session-global): '%s' is enabled in the pool but its Apply returned false - it is NOT active for this session."),
				*Entry.Id.ToString());
		}
	}

	if (ActiveSessionGlobals.Num() > 0)
	{
		TArray<FString> Names;
		for (const FName& Id : ActiveSessionGlobals) { Names.Add(Id.ToString()); }
		UE_LOG(LogAnomalyCapture, Log,
			TEXT("=== Capture(session-global): APPLIED FOR THE WHOLE SESSION: %s. Baseline near-clip was %.3f, it is now %.3f. ")
			TEXT("These are held from StartRun to FinishRun and NEVER routed through the auto-injector's per-burst fire path, ")
			TEXT("so no target actor is drawn and no '=ActorName' token is ever built for them. ")
			TEXT("A frame is labelled positive ONLY when geometry is actually within the near-clip radius - the near plane being ")
			TEXT("wrong is not the same as the viewer seeing anything wrong. ==="),
			*FString::Join(Names, TEXT(", ")), SessionGlobalBaselineNearClip, GNearClippingPlane);
	}
}

void UAnomalyCaptureSubsystem::RevertSessionGlobals()
{
	if (ActiveSessionGlobals.Num() == 0)
	{
		return;
	}

	UWorld* World = GetWorld();
	UAnomalyInjectorSubsystem* Injector = World ? World->GetSubsystem<UAnomalyInjectorSubsystem>() : nullptr;
	if (Injector)
	{
		for (const FName& Id : ActiveSessionGlobals)
		{
			Injector->RevertAnomaly(Id);
		}
	}

	UE_LOG(LogAnomalyCapture, Log,
		TEXT("=== Capture(session-global): REVERTED %d id(s). Near-clip is now %.3f (baseline at StartRun was %.3f). ")
		TEXT("Frames labelled positive: %d, negative: %d - the split is the near-clip PROXIMITY QUERY, not the session flag. ==="),
		ActiveSessionGlobals.Num(), GNearClippingPlane, SessionGlobalBaselineNearClip,
		SessionGlobalPositiveFrames, SessionGlobalNegativeFrames);

	ActiveSessionGlobals.Reset();
}

bool UAnomalyCaptureSubsystem::IsNearClipSlicingNow() const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	FAnomalyViewInfo View;
	if (!AnomalyViewport::GetActiveViewInfo(World, View))
	{
		return false;
	}

	const float Radius = GNearClippingPlane;
	if (Radius <= 0.0f)
	{
		return false;
	}

	FCollisionQueryParams Params(FName(TEXT("AnomalyNearClipProbe")), false);
	if (const APlayerController* PC = World->GetFirstPlayerController())
	{
		if (const APawn* Pawn = PC->GetPawn())
		{
			Params.AddIgnoredActor(Pawn);
		}
	}

	return World->OverlapAnyTestByChannel(View.Origin, FQuat::Identity, ECC_Visibility,
		FCollisionShape::MakeSphere(Radius), Params);
}

void UAnomalyCaptureSubsystem::AppendSessionGlobalFires(TArray<FAutoLiveFireInfo>& InOutFires) const
{
	for (const FName& Id : ActiveSessionGlobals)
	{
		FAutoLiveFireInfo F;
		F.Id = Id;
		F.Target = FString();
		F.TargetActor = nullptr;
		F.SecondsRemaining = 0.0f;
		F.StartFrame = (uint64)StartFrame;
		InOutFires.Add(F);
	}
}

void UAnomalyCaptureSubsystem::SampleDeferredHidden()
{
	if (!bHasDeferredHidden)
	{
		return;
	}
	bHasDeferredHidden = false;

	if (!Async.IsValid())
	{
		return;
	}
	AnomalyLabel::FCaptureSnapshot* Snap = Async->PendingSnapshots.Find(DeferredHiddenRequestId);
	if (!Snap)
	{
		return;
	}

	Snap->FireHidden.Reset();
	Snap->FireHidden.Reserve(Snap->Fires.Num());
	for (const FAutoLiveFireInfo& F : Snap->Fires)
	{
		const AActor* FActor = F.TargetActor.Get();
		Snap->FireHidden.Add((FActor && FActor->IsHidden()) ? 1 : 0);
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

#if ANOMALY_CAPTURE
static bool MaskStateProvidesMeasurement(EAnomalyMaskState State)
{
	switch (State)
	{
	case EAnomalyMaskState::MeasuredZero:    return true;
	case EAnomalyMaskState::MeasuredNonZero: return true;
	default:                                 return false;
	}
}

static bool MaskStateVetoes(EAnomalyMaskState State)
{
	return State == EAnomalyMaskState::MeasuredZero;
}

static bool AnyTranslucentSlotOnTaggedComponents(const AActor* Actor, int32& OutTranslucentSlots, int32& OutTotalSlots, bool& bOutKnown)
{
	OutTranslucentSlots = 0;
	OutTotalSlots = 0;
	bOutKnown = false;
	if (!Actor)
	{
		return false;
	}

	int32 RenderableComponents = 0;
	TInlineComponentArray<UPrimitiveComponent*> Prims;
	const_cast<AActor*>(Actor)->GetComponents(Prims);
	for (const UPrimitiveComponent* Prim : Prims)
	{
		if (!AnomalyViewport::IsRenderableComponent(Prim))
		{
			continue;
		}
		++RenderableComponents;
		const int32 NumSlots = Prim->GetNumMaterials();
		for (int32 SlotIndex = 0; SlotIndex < NumSlots; ++SlotIndex)
		{
			const UMaterialInterface* Mat = Prim->GetMaterial(SlotIndex);
			if (!Mat)
			{
				continue;
			}
			++OutTotalSlots;
			if (IsTranslucentBlendMode(Mat->GetBlendMode()))
			{
				++OutTranslucentSlots;
			}
		}
	}

	bOutKnown = (RenderableComponents > 0) && (OutTotalSlots > 0);
	return bOutKnown && OutTranslucentSlots > 0;
}

static bool AccumEventManifested(const FSessionEventAccum& Ev)
{
	bool bKnownId = false;
	if (!IsHideTypeAnomaly(Ev.Id, bKnownId))
	{
		return true;
	}
	for (const TPair<int32, uint8>& Pair : Ev.HiddenByIndex)
	{
		if (Pair.Value)
		{
			return true;
		}
	}
	return false;
}
#endif

void UAnomalyCaptureSubsystem::FinishRun(bool bLogLine)
{
	SampleDeferredHidden();

	if (UAnomalyAutoInjectorSubsystem* Auto = ResolveAuto())
	{
		Auto->RevertAllLiveFires();
	}

	const bool bWroteSession = bRunBegun;

	if (bWroteSession)
	{
		if (bAsyncCapture)
		{
			DrainAsyncToCompletion();
		}

		RevertSessionGlobals();

		ComputeRunPacing();

		int32 MaskProbeArms = 0;
		int32 MaskResidualDiscards = 0;
		int32 MaskNoPassDiscards = 0;
		if (bMaskMeasure && Async.IsValid() && Async->MaskExtension.IsValid())
		{
			Async->MaskExtension->EnqueueDrain();
			FlushRenderingCommands();
			Async->MaskMeasure.CollectResults(Async->MaskExtension.Get());
			MaskProbeArms = Async->MaskMeasure.TotalProbeArms();
			MaskResidualDiscards = Async->MaskMeasure.TotalResidualDiscards();
			MaskNoPassDiscards = Async->MaskMeasure.TotalNoPassDiscards();
		}

		{
			const int32 AsyncResamples = (Async.IsValid() && Async->Writer.IsValid())
				? Async->Writer->GetResamplesPerformed() : 0;
			const int32 AsyncMismatches = (Async.IsValid() && Async->Writer.IsValid())
				? Async->Writer->GetDimMismatches() : 0;
			int32 FirstW = 0, FirstH = 0;
			if (Async.IsValid() && Async->Writer.IsValid())
			{
				Async->Writer->GetFirstWrittenSize(FirstW, FirstH);
			}
			if (FirstW <= 0 || FirstH <= 0)
			{
				FirstW = SyncFirstWrittenW;
				FirstH = SyncFirstWrittenH;
			}
			UE_LOG(LogAnomalyCapture, Log,
				TEXT("Capture(m28): RESAMPLE SUMMARY resamples_performed=%d framesWritten=%d ")
				TEXT("requestedOutputHeight=%d source=%s firstWrittenFrame=%dx%d dimMismatches=%d. ")
				TEXT("resamples_performed is INTERNAL/LOG ONLY and is deliberately NOT written to ")
				TEXT("run_summary.json. On a NATIVE run it must be EXACTLY 0; on a downscaled run it must ")
				TEXT("EXACTLY equal framesWritten."),
				AsyncResamples + SyncResamplesPerformed, FramesWritten,
				EffectiveOutputHeight, DescribeOutputHeightSource(),
				FirstW, FirstH, AsyncMismatches + SyncDimMismatches);
		}

		VetoedEvents = 0;
		TranslucentVetoes = 0;
		TranslucencyUnknownVetoes = 0;
		int32 CountedEventsBefore = Async.IsValid() ? Async->SessionEvents.Num() : 0;
		if (bMaskMeasure && Async.IsValid())
		{
			for (int32 i = Async->SessionEvents.Num() - 1; i >= 0; --i)
			{
				const FSessionEventAccum& Ev = Async->SessionEvents[i];
				if (!AccumEventManifested(Ev))
				{
					continue;
				}
				const FAnomalyMaskRecord* Rec =
					Async->MaskMeasure.FindRecord(Ev.Id, Ev.Target, Ev.StartFrame);
				if (!Rec || !MaskStateVetoes(Rec->State))
				{
					continue;
				}
				int32 TranslucentSlots = 0;
				int32 TotalSlots = 0;
				bool bTranslucencyKnown = false;
				const bool bAnyTranslucent = AnyTranslucentSlotOnTaggedComponents(
					Rec->TargetActor.Get(), TranslucentSlots, TotalSlots, bTranslucencyKnown);
				if (!bTranslucencyKnown)
				{
					++TranslucencyUnknownVetoes;
				}
				else if (bAnyTranslucent)
				{
					++TranslucentVetoes;
				}

				UE_LOG(LogAnomalyCapture, Warning,
					TEXT("VETOED-OBJECT target=%s asset=%s componentClass=%s state=%s maxCount=%d ")
					TEXT("translucency=%s translucentSlots=%d/%d anomaly=%s startFrame=%llu"),
					*Ev.Target,
					Ev.NodeAssetName.IsEmpty() ? TEXT("(none)") : *Ev.NodeAssetName,
					Ev.NodeComponentClass.IsEmpty() ? TEXT("(none)") : *Ev.NodeComponentClass,
					LexToStringAnomalyMaskState(Rec->State),
					Rec->MaxCount,
					bTranslucencyKnown ? (bAnyTranslucent ? TEXT("TRANSLUCENT") : TEXT("opaque")) : TEXT("UNKNOWN"),
					TranslucentSlots, TotalSlots,
					*Ev.Id.ToString(), Ev.StartFrame);

				UE_LOG(LogAnomalyCapture, Warning,
					TEXT("Capture(mask): M26S3 VETO id=%s target=%s startFrame=%llu state=%s - the target was ")
					TEXT("MEASURED and contributed ZERO drawn pixels, so this event is removed from ")
					TEXT("annotation.json before it is written. The captured FRAMES are NOT un-written (L1). ")
					TEXT("NOT_MEASURED is never vetoed, and a measured NON-ZERO count is never vetoed however ")
					TEXT("small it is - the rule is zero-only and there is no ratio or threshold."),
					*Ev.Id.ToString(), *Ev.Target, Ev.StartFrame,
					LexToStringAnomalyMaskState(Rec->State));
				Async->SessionEvents.RemoveAt(i);
				++VetoedEvents;
			}
		}

		WriteSessionAnnotationFile();

		if (bMaskMeasure && Async.IsValid())
		{
			UE_LOG(LogAnomalyCapture, Log,
				TEXT("Capture(mask): M27 VETO SUMMARY vetoedEvents=%d translucentVetoes=%d ")
				TEXT("translucencyUnknownVetoes=%d - one line per removed object was logged above, each prefixed ")
				TEXT("with the single grep token named in client-delivery.md; ")
				TEXT("a vetoed event leaves NO trace in annotation.json, so those lines and vetoed_events are the ")
				TEXT("only record. translucentVetoes counts vetoes whose target carried at least one TRANSLUCENT ")
				TEXT("material slot: on UE 5.1 such a target cannot write custom depth unless its material ticks ")
				TEXT("Allow Custom Depth Writes, so its zero may mean the mask could not see it rather than that ")
				TEXT("it drew nothing. UNKNOWN is counted SEPARATELY and is NOT the same as opaque. Both counters ")
				TEXT("are DIAGNOSTIC - they feed nothing, gate nothing, and must never become a filter."),
				VetoedEvents, TranslucentVetoes, TranslucencyUnknownVetoes);
			UE_LOG(LogAnomalyCapture, Log,
				TEXT("Capture(mask): M26S3 G-11 countedEventsBefore=%d countedEventsAfter=%d vetoedEvents=%d ")
				TEXT("nonManifestedEvents=%d - the two counters are DISJOINT by construction: manifested is ")
				TEXT("evaluated FIRST and a non-manifested event is NEVER vetoed. non_manifested_events means ")
				TEXT("'the hide never showed in pixels'; vetoed_events means 'the target contributed no pixels ")
				TEXT("to hide'. A CERTIFYING leg with fewer than 3 counted events AFTER the veto is INVALID and ")
				TEXT("must be reported as invalid, never graded on the reduced set. A demonstration leg that ")
				TEXT("vetoes everything is the veto WORKING, not a validity failure - the distinction is the ")
				TEXT("leg's ROLE, fixed before it runs."),
				CountedEventsBefore, Async->SessionEvents.Num(), VetoedEvents, NonManifestedEvents);
		}

		AnomalyLabel::FRingTelemetry RingTelemetry;
		if (bSveCapture)
		{
			const AnomalySveKeyRing::FCounters Ring = AnomalySveKeyRing::GetCounters();
			RingTelemetry.Published = Ring.Published;
			RingTelemetry.Consumed = Ring.Consumed;
			RingTelemetry.Missed = Ring.Missed;
			RingTelemetry.Wrapped = Ring.Wrapped;
			RingTelemetry.Corrupted = Ring.Corrupted;
			RingTelemetry.WantedMatches = Ring.WantedMatches;
		}

		AnomalyLabel::WriteRunSummary(RunDir, FramesWritten, PositiveFramesWritten, BurstsDone, ZeroMatchBursts, GFrameCounter,
			VideoFps, LastRunPacing.SustainedWallFps, LastRunPacing.SpeedRatio, LastRunPacing.StampedFps, bPaceCapture, bDeliveryMode,
			ContentClock == EContentClock::Game ? TEXT("game") : TEXT("wall"), NonManifestedEvents,
			bSveCapture ? TEXT("sve") : TEXT("backbuffer"),
			bSveCapture ? &RingTelemetry : nullptr,
			MaskProbeArms, MaskResidualDiscards, MaskNoPassDiscards, VetoedEvents,
			TranslucentVetoes, TranslucencyUnknownVetoes);

		if (bSveCapture)
		{
			const AnomalySveKeyRing::FCounters Ring = AnomalySveKeyRing::GetCounters();
			UE_LOG(LogAnomalyCapture, Log,
				TEXT("Capture(sve): key ring — published=%d consumed=%d missed=%d wrapped=%d corrupted=%d (forceMiss=%d)."),
				Ring.Published, Ring.Consumed, Ring.Missed, Ring.Wrapped, Ring.Corrupted,
				AnomalySveKeyRing::GetForceMissMode());

			if (Async.IsValid() && Async->SveCapturer.IsValid())
			{
				const FAnomalySveHandshakeStats Shake = Async->SveCapturer->GetHandshakeStats();
				UE_LOG(LogAnomalyCapture, Log,
					TEXT("Capture(sve): SVE-WANT-SUMMARY marksIssued=%d publishesSeen=%d wantedMatches=%d ")
					TEXT("submitsIssued=%d framesWritten=%d pendingWantedAtEnd=%d maxPendingDepth=%d ")
					TEXT("(traced %d arm / %d publish event(s) of first %d each; the per-event lines above carry ")
					TEXT("their own token). The handshake is CONNECTED when marksIssued, wantedMatches, submitsIssued ")
					TEXT("and framesWritten agree; a gap between ADJACENT numbers names the stage that missed — ")
					TEXT("marks>matches: arms never met an eligible publish; matches>submits: the render pass dropped ")
					TEXT("keyed frames; submits>frames: readback or snapshot pairing lost them (pairing losses now ")
					TEXT("warn per frame with their own token)."),
					Shake.ArmsIssued, Ring.Published, Ring.WantedMatches,
					Shake.SubmitsIssued, FramesWritten, Shake.PendingNow, Shake.MaxPendingDepth,
					Shake.TracedArms, Shake.TracedPublishes, (int32)FAnomalySveCapturer::HandshakeTraceLimit);
			}
		}

		if (bLogLine)
		{
			UE_LOG(LogAnomalyCapture, Log,
				TEXT("=== Capture run FINISHED: %s | %d frame(s) (positive=%d) | %d burst(s), %d zero-match ==="),
				*RunDir, FramesWritten, PositiveFramesWritten, BurstsDone, ZeroMatchBursts);
		}
	}

	if (!bWroteSession)
	{
		IFileManager::Get().DeleteDirectory(*RunDir, false, true);
		if (bLogLine)
		{
			UE_LOG(LogAnomalyCapture, Log,
				TEXT("=== Capture run CANCELLED before focus: %s | no frames written (armed-pending run stopped) ==="),
				*RunDir);
		}
	}

	if (bMaskMeasure && Async.IsValid())
	{
		if (!bWroteSession && Async->MaskExtension.IsValid())
		{
			Async->MaskExtension->EnqueueDrain();
			FlushRenderingCommands();
			Async->MaskMeasure.CollectResults(Async->MaskExtension.Get());
		}

		const TArray<FAnomalyMaskRecord>& Recs = Async->MaskMeasure.GetRecords();
		UE_LOG(LogAnomalyCapture, Log,
			TEXT("Capture(mask): M26S1 SUMMARY events=%d notMeasured=%d"),
			Recs.Num(), Async->MaskMeasure.NumUnmeasured());
		for (const FAnomalyMaskRecord& R : Recs)
		{
			const double PctOfFrame = (R.ViewportPixels > 0)
				? (100.0 * (double)R.MaxCount / (double)R.ViewportPixels) : -1.0;
			UE_LOG(LogAnomalyCapture, Log,
				TEXT("Capture(mask): M26S1 EVENT id=%s target=%s startFrame=%llu tag=%d state=%s ")
				TEXT("maxCount=%d viewportPx=%d pctOfFrame=%.4f arms=%d resolved=%d framesDiscarded=%d ")
				TEXT("framesResidual=%d framesUnconfirmed=%d framesNoPass=%d framesContributed=%d probeArms=%d ")
				TEXT("skippedHidden=%d collisions=%d tagFailed=%d%s%s"),
				*R.Id.ToString(), *R.Target, R.StartFrame, (int32)R.Tag,
				LexToStringAnomalyMaskState(R.State),
				R.MaxCount, R.ViewportPixels, PctOfFrame,
				R.ArmsIssued, R.ArmsResolved, R.FramesDiscarded,
				R.FramesResidualDiscarded, R.FramesUnconfirmed, R.FramesNoPass,
				R.FramesContributed, R.ProbeArms,
				R.SkippedHidden,
				R.CollisionHits, R.bTagFailed ? 1 : 0,
				R.FirstCollisionDetail.IsEmpty() ? TEXT("") : TEXT(" detail="),
				*R.FirstCollisionDetail);

			if (R.State == EAnomalyMaskState::NotMeasured)
			{
				UE_LOG(LogAnomalyCapture, Warning,
					TEXT("Capture(mask): M26S1 NOT_MEASURED for '%s' on '%s' - no clean frame ")
					TEXT("(armsIssued=%d resolved=%d framesDiscarded=%d framesResidual=%d ")
					TEXT("framesUnconfirmed=%d framesNoPass=%d probeArms=%d skippedHidden=%d tagFailed=%d ")
					TEXT("collisions=%d). Slice 3 MUST ADMIT this event: never-measured is not measured-zero.%s"),
					*R.Id.ToString(), *R.Target, R.ArmsIssued, R.ArmsResolved, R.FramesDiscarded,
					R.FramesResidualDiscarded, R.FramesUnconfirmed, R.FramesNoPass, R.ProbeArms,
					R.SkippedHidden, R.bTagFailed ? 1 : 0, R.CollisionHits,
					(R.FramesNoPass > 0 && R.FramesNoPass == R.ArmsResolved)
						? TEXT(" EVERY resolved frame lacked the custom-depth pass for this target. framesNoPass ")
						  TEXT("is NOT a Nanite counter: the causes include NANITE geometry (G134), FRUSTUM ")
						  TEXT("CULLING, and any other route by which the target is absent from the view's ")
						  TEXT("relevant set. Distinguish them by whether the target projected on screen at all.")
						: TEXT(""));
			}
		}
		Async->MaskMeasure.EndRun();
	}

	if (Async.IsValid() && Async->SveCapturer.IsValid())
	{
		Async->SveCapturer->SetActive(false);
	}

	bRunBegun = false;
	bRunning = false;
	Phase = ECapturePhase::Idle;
	PhaseFramesLeft = 0;
	RunDir.Reset();

	bTargetedMode = false;
	TargetAnomalyId = NAME_None;
	TargetActorName.Reset();
	TargetAnomalyArgs.Reset();

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
				ResolveNodeIdentity(FActor, Ev->NodeAssetName, Ev->NodeComponentClass,
					Ev->NodeBoundsOrigin, Ev->NodeBoundsExtent);
				AnomalyViewport::EvaluateSelectionProvenance(World, FActor, Ev->Provenance);
			}
			Ev->NodePos = FirePos.IsValidIndex(i) ? FirePos[i] : FVector::ZeroVector;
		}

		if (bMaskMeasure)
		{
			Async->MaskMeasure.FindOrAddRecord(F.Id, F.Target, F.StartFrame, const_cast<AActor*>(F.TargetActor.Get()));
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
		else if (ActiveSessionGlobals.Contains(F.Id))
		{
			Ev->AffectedFrames.Add(SessionIndex);
			Ev->CoverageSum += 1.0;
			++Ev->CoverageCount;
		}

		const int32 Hidden = (FireHidden.IsValidIndex(i) && FireHidden[i]) ? 1 : 0;
		if (Hidden) { ++Ev->HiddenFrames; } else { ++Ev->VisibleFrames; }
		Ev->HiddenByIndex.Add(SessionIndex, (uint8)Hidden);
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
	int32 AsyncW = 0, AsyncH = 0;
	if (Async->Writer.IsValid())
	{
		Async->Writer->GetFirstWrittenSize(AsyncW, AsyncH);
	}

	int32 ResolvedW = 0, ResolvedH = 0;
	if (AsyncW > 0 && AsyncH > 0)
	{
		ResolvedW = AsyncW;
		ResolvedH = AsyncH;
		if (SyncFirstWrittenW > 0 && SyncFirstWrittenH > 0
			&& (SyncFirstWrittenW != AsyncW || SyncFirstWrittenH != AsyncH))
		{
			UE_LOG(LogAnomalyCapture, Warning,
				TEXT("Capture(m28): THIS SESSION WROTE FRAMES ON BOTH PATHS AND THEY DISAGREE - async first-written ")
				TEXT("%dx%d vs sync-fallback first-written %dx%d. video.resolution reports the ASYNC pair. The ")
				TEXT("session's frames are NOT all one size."),
				AsyncW, AsyncH, SyncFirstWrittenW, SyncFirstWrittenH);
		}
	}
	else if (SyncFirstWrittenW > 0 && SyncFirstWrittenH > 0)
	{
		ResolvedW = SyncFirstWrittenW;
		ResolvedH = SyncFirstWrittenH;
	}
	else
	{
		UE_LOG(LogAnomalyCapture, Warning,
			TEXT("Capture(m28): NO FRAME WAS WRITTEN THIS SESSION, so annotation.json video.resolution is [0,0]. ")
			TEXT("It is sourced from the FIRST WRITTEN FRAME by design (m28/D4) and a session with no pixels ")
			TEXT("describes no pixels. It is deliberately NOT falling back to GetViewportSize(), which is the ")
			TEXT("quantity survey S0 found does not describe the delivered image."));
	}

	A.Video.ResolutionW = ResolvedW;
	A.Video.ResolutionH = ResolvedH;
	A.Video.TotalFrames = FramesWritten;

	A.Video.Fps = LastRunPacing.StampedFps > 0.0 ? LastRunPacing.StampedFps : (double)VideoFps;
	A.Video.TargetFps = VideoFps;

	for (FSessionEventAccum& Ev : Async->SessionEvents)
	{
		TArray<int32> HiddenKeys;
		Ev.HiddenByIndex.GetKeys(HiddenKeys);
		HiddenKeys.Sort();

		TArray<int32> HiddenIdx;
		for (int32 Key : HiddenKeys)
		{
			if (Ev.HiddenByIndex[Key]) { HiddenIdx.Add(Key); }
		}

		AnomalyLabel::FSessionEvent Out;
		MapAnomalyToClient(Ev.Id, Out.AnomalyType, Out.AnomalySubtype);

		bool bKnownId = false;
		const bool bHideType = IsHideTypeAnomaly(Ev.Id, bKnownId);
		if (!bKnownId)
		{
			UE_LOG(LogAnomalyCapture, Warning,
				TEXT("Capture: anomaly id '%s' is not registered in the hide-type table — falling back to on-screen frames. ")
				TEXT("If this is a hide-type anomaly it MUST be added to IsHideTypeAnomaly, or a non-manifesting event will be ")
				TEXT("labelled positive on every frame the actor was visible."),
				*Ev.Id.ToString());
		}

		TArray<int32> FrameIndices;
		if (bHideType)
		{
			Out.bManifested = HiddenIdx.Num() > 0;
			if (Out.bManifested)
			{
				FrameIndices = MoveTemp(HiddenIdx);
			}
			else
			{
				++NonManifestedEvents;
				UE_LOG(LogAnomalyCapture, Warning,
					TEXT("Capture: '%s' event on '%s' NEVER MANIFESTED — no captured frame sampled the target hidden. ")
					TEXT("Writing zero positive frames and manifested=false (previously this emitted %d on-screen frames as positives)."),
					*Ev.Id.ToString(), *Ev.NodeName, Ev.AffectedFrames.Num());
			}
		}
		else
		{
			FrameIndices = Ev.AffectedFrames;
		}
		FrameIndices.Sort();
		Out.FrameIndices = MoveTemp(FrameIndices);
		Out.CoverageRatio = Ev.CoverageCount > 0 ? (Ev.CoverageSum / (double)Ev.CoverageCount) : 0.0;
		Out.CoveragePct = Ev.Provenance.CoveragePct;

		AnomalyLabel::FSessionNode Node;
		Node.Name = Ev.NodeName;
		Node.Path = Ev.NodePath;
		Node.GlobalPosition = Ev.NodePos;
		Node.AssetName = Ev.NodeAssetName;
		Node.ComponentClass = Ev.NodeComponentClass;
		Node.BoundsOrigin = Ev.NodeBoundsOrigin;
		Node.BoundsExtent = Ev.NodeBoundsExtent;
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

		Out.bMaskProvided = false;
		if (bMaskMeasure)
		{
			if (const FAnomalyMaskRecord* Rec = Async->MaskMeasure.FindRecord(Ev.Id, Ev.Target, Ev.StartFrame))
			{
				Out.bMaskProvided = MaskStateProvidesMeasurement(Rec->State);
				UE_LOG(LogAnomalyCapture, Log,
					TEXT("Capture(mask): M26S2 MAP id=%s target=%s startFrame=%llu state=%s -> mask.provided=%s ")
					TEXT("(NOT_MEASURED maps to false and MUST BE ADMITTED; MEASURED_ZERO maps to TRUE and is a ")
					TEXT("measurement, not an absence - the two zeros never share a representation)"),
					*Ev.Id.ToString(), *Ev.Target, Ev.StartFrame,
					LexToStringAnomalyMaskState(Rec->State),
					Out.bMaskProvided ? TEXT("true") : TEXT("false"));
			}
			else
			{
				UE_LOG(LogAnomalyCapture, Warning,
					TEXT("Capture(mask): M26S2 MAP id=%s target=%s startFrame=%llu - NO MASK RECORD for this event ")
					TEXT("-> mask.provided=false (never measured, MUST BE ADMITTED)"),
					*Ev.Id.ToString(), *Ev.Target, Ev.StartFrame);
			}
		}

		A.Events.Add(MoveTemp(Out));
	}

	if (!bDeliveryMode)
	{
		TArray<AnomalyLabel::FProvenanceRecord> Prov;
		for (const FSessionEventAccum& Ev : Async->SessionEvents)
		{
			AnomalyLabel::FProvenanceRecord R;
			R.AnomalyId = Ev.Id.ToString();
			R.Target = Ev.Target;
			R.AnchorIndex = Ev.AnchorIndex == MAX_int32 ? -1 : Ev.AnchorIndex;
			R.CoveragePct = Ev.Provenance.CoveragePct;
			R.OcclusionSamplesPassed = Ev.Provenance.OcclusionSamplesPassed;
			R.OcclusionSamplesTotal = Ev.Provenance.OcclusionSamplesTotal;
			R.PollDistance = Ev.Provenance.PollDistance;
			R.bValid = Ev.Provenance.bValid;
			Prov.Add(MoveTemp(R));
		}
		AnomalyLabel::WriteSelectionProvenance(RunDir, Prov);
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
	TEXT("Start a labeled burst-capture run. Usage: IAI.Capture.Start [outDir] [png|jpeg] [seed] [maxFrames] [anomaly] [targetActor] [anomalyArgs...]  ")
	TEXT("(default dir <ProjectSaved>/AnomalyCaptures; png; seed = auto-injector's current; maxFrames 0 = until "
	     "Stop / burst schedule). Pass BOTH [anomaly] and [targetActor] for a TARGETED run (fires only that anomaly on "
	     "only that actor each burst); omit both for AUTO-POOL (random mix from the enabled pool). Empty \"\" placeholders "
	     "resolve to defaults for the leading args. The auto-injector's Run is paused for the run and resumed on finish. "
	     "Configure bursts first with IAI.Capture.Config. ")
	TEXT("OUTPUT RESOLUTION: an optional NON-POSITIONAL token oh=<height> may appear ANYWHERE in the argument list. ")
	TEXT("It is the PER-RUN output height (precedence level 1 - it beats IAI.Capture.OutputHeight and the ini) and is ")
	TEXT("STRIPPED before the anomaly args are collected. oh=0 requests NATIVE explicitly; omitting it entirely falls ")
	TEXT("through to the override, then the ini, then the compiled default. It is deliberately NOT positional because ")
	TEXT("args 6+ are forwarded to the anomaly VERBATIM and there is no free slot."),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (UAnomalyCaptureSubsystem* Cap = ResolveCapture(World))
			{
				int32 PerRunOutputHeight = -1;
				TArray<FString> Positional;
				Positional.Reserve(Args.Num());
				for (const FString& A : Args)
				{
					if (A.StartsWith(TEXT("oh="), ESearchCase::IgnoreCase))
					{
						const FString Value = A.Mid(3);
						PerRunOutputHeight = Value.IsEmpty() ? -1 : FMath::Max(0, FCString::Atoi(*Value));
						continue;
					}
					Positional.Add(A);
				}

				auto Slot = [&Positional](int32 Index) -> FString
				{
					if (!Positional.IsValidIndex(Index)) { return FString(); }
					const FString& S = Positional[Index];
					if (S.IsEmpty() || S == TEXT("\"\"") || S == TEXT("''")) { return FString(); }
					return S;
				};
				const FString Dir = Slot(0);
				const FString Fmt = Slot(1);
				const FString SeedStr = Slot(2);
				const FString MaxStr = Slot(3);
				const FString Anomaly = Slot(4);
				const FString TargetActor = Slot(5);
				TArray<FString> TargetArgs;
				for (int32 i = 6; Positional.IsValidIndex(i); ++i)
				{
					TargetArgs.Add(Positional[i]);
				}
				const bool bPng = !(Fmt.Equals(TEXT("jpeg"), ESearchCase::IgnoreCase) || Fmt.Equals(TEXT("jpg"), ESearchCase::IgnoreCase));
				const int32 Seed = !SeedStr.IsEmpty() ? FCString::Atoi(*SeedStr) : -1;
				const int32 MaxFrames = !MaxStr.IsEmpty() ? FCString::Atoi(*MaxStr) : 0;
				Cap->StartRun(Dir, bPng, Seed, MaxFrames, Anomaly, TargetActor, TargetArgs, PerRunOutputHeight);
			}
		}));

static FAutoConsoleCommandWithWorldAndArgs GCaptureOutputHeightCmd(
	TEXT("IAI.Capture.OutputHeight"),
	TEXT("Set the capture OUTPUT HEIGHT for subsequent runs (default 0 = NATIVE). This is a DOWNSCALE ON WRITE: the ")
	TEXT("render is always native and only the WRITTEN frame is resampled, so it cannot affect selection, labelling ")
	TEXT("geometry or the m26/m27 mask veto - the mask counts at the view's render resolution and never sees the ")
	TEXT("written file. There is NO width parameter by design: width is derived from each frame's own aspect and both ")
	TEXT("are snapped to even, so a non-aspect-preserving output cannot be requested. A value at or above the frame's ")
	TEXT("own height is NOT an upscale - it yields native and no resample runs. Mid-run changes are ignored (stop ")
	TEXT("first). This is the BETWEEN-RUNS override, precedence level 2: it BEATS DefaultGame.ini [AnomalyCapture] ")
	TEXT("CaptureOutputHeightDefault and is BEATEN by a per-run argument (dashboard outputHeight, console Start oh=). ")
	TEXT("Pass -1 to CLEAR it, which is NOT the same as 0. Usage: IAI.Capture.OutputHeight <height|0|-1>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() < 1)
			{
				UE_LOG(LogAnomalyCapture, Warning, TEXT("Usage: IAI.Capture.OutputHeight <height|0|-1>"));
				return;
			}
			if (UAnomalyCaptureSubsystem* Cap = ResolveCapture(World))
			{
				Cap->SetOutputHeightOverride(FCString::Atoi(*Args[0]));
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

static FAutoConsoleCommandWithWorldAndArgs GCaptureMaskCmd(
	TEXT("IAI.Capture.Mask"),
	TEXT("m26 SLICES 1+2+3 - MEASURE, REPORT AND VETO. The COMPILED default is OFF, but that is only the ")
	TEXT("fallback: [AnomalyCapture] bMaskMeasureDefault in DefaultGame.ini carries delivered behaviour, and ")
	TEXT("this switch overrides whichever of the two applies, for the session, BETWEEN RUNS. Read the effective ")
	TEXT("value and its provenance from the StartRun echo, never from this help text. ON: tag each fired target into custom stencil using ")
	TEXT("the SHARED renderable predicate (AnomalyViewport::IsRenderableComponent), rasterise an ")
	TEXT("occlusion-correct silhouette mask after tonemap, and reduce it to a per-event SURVIVING-PIXEL COUNT. ")
	TEXT("A frame contributes only on POSITIVE evidence the custom-depth pass ran and the target rendered ")
	TEXT("visible. The mask is armed only on a tick where the target is KNOWN NOT HIDDEN, so a hide-type target ")
	TEXT("is measured post-revert; an event with no qualifying frame stays NOT_MEASURED. SLICE 2 REPORTING: ")
	TEXT("annotation.json's already-shipping mask{provided} carries the tri-state's bool - NOT_MEASURED false ")
	TEXT("(never measured, MUST be admitted), MEASURED_ZERO and MEASURED_NONZERO true. NO sub-fields are added ")
	TEXT("under mask and depth{} is untouched. SLICE 3 VETO, ZERO-ONLY: an event is removed from ")
	TEXT("annotation.json IF AND ONLY IF it is manifested AND its target was MEASURED at ZERO drawn pixels. ")
	TEXT("NOT_MEASURED is NEVER vetoed; a measured NON-ZERO count is NEVER vetoed however small a fraction of ")
	TEXT("its claimed extent it is - there is NO ratio and NO threshold. Removed events are counted in ")
	TEXT("run_summary vetoed_events, which stays DISJOINT from non_manifested_events; the captured frames are ")
	TEXT("NOT un-written. With this OFF no event has a record, so mask{provided} is false everywhere, nothing ")
	TEXT("is vetoed and the output is unchanged. Mid-run changes are ignored (stop first). ")
	TEXT("Usage: IAI.Capture.Mask <0|1>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() < 1)
			{
				UE_LOG(LogAnomalyCapture, Warning, TEXT("Usage: IAI.Capture.Mask <0|1>"));
				return;
			}
			if (UAnomalyCaptureSubsystem* Cap = ResolveCapture(World))
			{
				Cap->SetMaskMeasure(FCString::Atoi(*Args[0]) != 0);
				UE_LOG(LogAnomalyCapture, Log, TEXT("IAI.Capture.Mask: EFFECTIVE READ-BACK = %d."),
					Cap->IsMaskMeasure() ? 1 : 0);
			}
		}));

static FAutoConsoleCommandWithWorldAndArgs GCaptureMaskProbeCmd(
	TEXT("IAI.Capture.MaskProbe"),
	TEXT("GATE ARTEFACT - default OFF, MUST be OFF in any build that ships (docs/PRE-DELIVERY-CHECKLIST.md). ")
	TEXT("ON: during a mask-measure run, issue ONE deliberate arm on a KNOWN-HIDDEN tick to prove the ")
	TEXT("255/StencilDummy detector, the end-of-frame confirmation and the frame-scoped discard are all LIVE ")
	TEXT("on this binary (F-6 item 5, G96 both ways). LOCK-1 is bypassed for that ONE arm only; the probe ")
	TEXT("frame is bucketed PROBE and can never contribute to a measurement. INERT IN DELIVERY MODE by a ")
	TEXT("guard at the fire site, regardless of this flag. Mid-run changes are ignored (stop first). ")
	TEXT("Usage: IAI.Capture.MaskProbe <0|1>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() < 1)
			{
				UE_LOG(LogAnomalyCapture, Warning, TEXT("Usage: IAI.Capture.MaskProbe <0|1>"));
				return;
			}
			if (UAnomalyCaptureSubsystem* Cap = ResolveCapture(World))
			{
				Cap->SetMaskProbe(FCString::Atoi(*Args[0]) != 0);
				UE_LOG(LogAnomalyCapture, Log, TEXT("IAI.Capture.MaskProbe: EFFECTIVE READ-BACK = %d."),
					Cap->IsMaskProbe() ? 1 : 0);
			}
		}));

static FAutoConsoleCommandWithWorldAndArgs GCaptureSveCmd(
	TEXT("IAI.Capture.SVE"),
	TEXT("Select the capture grab point (default OFF = backbuffer). ON: capture via a SceneViewExtension — scene ")
	TEXT("colour after tonemap and BEFORE Slate composites the UI, so the frame is UI-free, and the frame/state key ")
	TEXT("is recovered by IDENTITY through the view-family ring instead of by arm-to-present ORDER. OFF: the m21 ")
	TEXT("backbuffer path — the presented frame including game UI. Mid-run changes are ignored (stop first). The ")
	TEXT("packaged default is read at startup from DefaultGame.ini [AnomalyCapture] bSveCaptureDefault; this command ")
	TEXT("overrides it for the session. Usage: IAI.Capture.SVE <0|1>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() < 1)
			{
				UE_LOG(LogAnomalyCapture, Warning, TEXT("Usage: IAI.Capture.SVE <0|1>"));
				return;
			}
			if (UAnomalyCaptureSubsystem* Cap = ResolveCapture(World))
			{
				Cap->SetSveCapture(FCString::Atoi(*Args[0]) != 0);
				UE_LOG(LogAnomalyCapture, Log, TEXT("IAI.Capture.SVE: EFFECTIVE READ-BACK = %d."),
					Cap->IsSveCapture() ? 1 : 0);
			}
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
