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
#include "AnomalyDefaults.h"
#include "AnomalyInjectorLog.h"
#include "AnomalyInjectorSubsystem.h"
#include "AnomalyRunLog.h"
#include "AnomalyFrameCapturer.h"
#include "AnomalySveCapturer.h"
#include "AnomalySceneViewExtension.h"
#include "AnomalyMaskSceneViewExtension.h"
#include "AnomalyMaskMeasure.h"
#include "AnomalyStencilTag.h"
#include "AnomalyCensus.h"
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
#include "EngineUtils.h"
#include "Misc/EngineVersion.h"
#include "Misc/App.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SkinnedMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"
#include "MaterialShared.h"
#include "Engine/PostProcessVolume.h"
#include "Camera/PlayerCameraManager.h"
#include "SceneInterface.h"
#include "RHI.h"

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
namespace AnomalyTickPin
{
#if ANOMINJECT_FW_TICKPIN
	static constexpr bool bCompiled = true;

	static bool Read()
	{
		return FApp::sUseFixedGameTickWithVariableRenderTick_Net;
	}

	static void Write(bool bValue)
	{
		FApp::sUseFixedGameTickWithVariableRenderTick_Net = bValue;
	}

#if defined(FW_BUILD_FIXEDTICKWITHVARIABLERENDER)
	static constexpr bool bForkDefineVisible = true;
#else
	static constexpr bool bForkDefineVisible = false;
#endif

#else
	static constexpr bool bCompiled = false;
	static constexpr bool bForkDefineVisible = false;

	static bool Read()
	{
		return false;
	}

	static void Write(bool)
	{
	}
#endif
}

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

	int32 InactiveFrames = 0;
	int32 ActiveFrames = 0;
	TMap<int32, uint8> ActiveByIndex;
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
	FAnomalyStencilTagLedger TagLedger;
	FAnomalyCensus Census;
	TMap<TWeakObjectPtr<UPrimitiveComponent>, int32> PreRunStencilSnapshot;
	TArray<FAnomalyCapturedFrame> TargetMaskHeldFrames;
#endif
};

#if ANOMALY_CAPTURE
namespace
{
	constexpr int32 GTargetMaskMaxHoldTicks = 4;

	constexpr int32 GMaskPairingProbeTag = 250;
	const FVector GMaskPairingProbePosA(-900.0, -250.0, 260.0);
	const FVector GMaskPairingProbePosB(-900.0,  250.0, 260.0);

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

		FBox Box(ForceInit);
		if (AnomalyViewport::GetActorRenderableBounds(Actor, Box))
		{
			OutBoundsOrigin = Box.GetCenter();
			OutBoundsExtent = Box.GetExtent();
		}
		else
		{
			UE_LOG(LogAnomalyCapture, Warning,
				TEXT("Capture: '%s' has NO renderable geometry component (no static or skinned mesh), so node.bounds is ")
				TEXT("left at zero rather than reporting a collision or visualisation primitive. An event on a target ")
				TEXT("that draws nothing is worth looking at."),
				*GetNameSafe(Actor));
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

	enum class EAnomalyActiveSource : uint8
	{
		FireWindow,
		ActorHidden,
		AnomalyState
	};

	EAnomalyActiveSource ResolveAnomalyActiveSource(FName Id, bool& bOutKnownId)
	{
		static const TMap<FName, EAnomalyActiveSource> SourceById = {
			{ FName(TEXT("blinking")),          EAnomalyActiveSource::ActorHidden },
			{ FName(TEXT("missing_object")),    EAnomalyActiveSource::ActorHidden },
			{ FName(TEXT("lod_popping")),       EAnomalyActiveSource::AnomalyState },
			{ FName(TEXT("missing_texture")),   EAnomalyActiveSource::FireWindow },
			{ FName(TEXT("corrupted_texture")), EAnomalyActiveSource::FireWindow },
			{ FName(TEXT("lighting_mismatch")), EAnomalyActiveSource::FireWindow },
			{ FName(TEXT("lod_corruption")),    EAnomalyActiveSource::FireWindow },
			{ FName(TEXT("camera_clipping")),   EAnomalyActiveSource::AnomalyState },
			{ FName(TEXT("time_dilation")),     EAnomalyActiveSource::FireWindow }
		};
		const EAnomalyActiveSource* Found = SourceById.Find(Id);
		bOutKnownId = (Found != nullptr);
		return Found ? *Found : EAnomalyActiveSource::FireWindow;
	}

	const TCHAR* DescribeActiveSource(EAnomalyActiveSource Source)
	{
		switch (Source)
		{
		case EAnomalyActiveSource::ActorHidden:  return TEXT("actor-hidden");
		case EAnomalyActiveSource::AnomalyState: return TEXT("anomaly-state");
		default:                                 return TEXT("fire-window");
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

#if ANOMALY_CAPTURE
static EAnomalyMaskReduceMode GMaskReduceMode = EAnomalyMaskReduceMode::Gpu;
static bool GMaskReduceFromIni = false;
static bool GMaskReduceFromConsole = false;

static bool ParseMaskReduceMode(const FString& In, EAnomalyMaskReduceMode& Out)
{
	if (In.Equals(TEXT("gpu"), ESearchCase::IgnoreCase)) { Out = EAnomalyMaskReduceMode::Gpu; return true; }
	if (In.Equals(TEXT("cpu"), ESearchCase::IgnoreCase)) { Out = EAnomalyMaskReduceMode::Cpu; return true; }
	if (In.Equals(TEXT("both"), ESearchCase::IgnoreCase)) { Out = EAnomalyMaskReduceMode::Both; return true; }
	return false;
}

static const TCHAR* DescribeMaskReduceSource()
{
	if (GMaskReduceFromConsole)
	{
		return TEXT("IAI.Capture.MaskReduce (console)");
	}
	if (GMaskReduceFromIni)
	{
		return TEXT("DefaultGame.ini [AnomalyCapture] MaskReduceDefault");
	}
	return TEXT("COMPILED DEFAULT (gpu)");
}
#endif

void UAnomalyCaptureSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
#if ANOMALY_CAPTURE
	MaskEndFrameHandle = FCoreDelegates::OnEndFrame.AddUObject(this, &UAnomalyCaptureSubsystem::OnEndFrameMaskSample);
	MaskWorldTickEndHandle = FWorldDelegates::OnWorldTickEnd.AddUObject(this, &UAnomalyCaptureSubsystem::OnWorldTickEndMask);
	SampleWorldTickEndHandle = FWorldDelegates::OnWorldTickEnd.AddUObject(this, &UAnomalyCaptureSubsystem::OnWorldTickEndSample);
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
	bool bConfigCensus = false;
	if (GConfig && GConfig->GetBool(TEXT("AnomalyCapture"), TEXT("bSelectionCensusDefault"), bConfigCensus, GGameIni))
	{
		bCensus = bConfigCensus;
		bCensusFromIni = true;
	}
	float ConfigCensusFloor = 0.0f;
	if (GConfig && GConfig->GetFloat(TEXT("AnomalyCapture"), TEXT("CensusMinDrawnCoveragePctDefault"), ConfigCensusFloor, GGameIni))
	{
		CensusFloorPct = FMath::Clamp(ConfigCensusFloor, 0.0f, 100.0f);
		bCensusFloorFromIni = true;
	}
	float ConfigCensusCeiling = 0.0f;
	if (GConfig && GConfig->GetFloat(TEXT("AnomalyCapture"), TEXT("CensusMaxDrawnCoveragePctDefault"), ConfigCensusCeiling, GGameIni))
	{
		CensusCeilingPct = FMath::Clamp(ConfigCensusCeiling, -1.0f, 100.0f);
		bCensusCeilingFromIni = true;
	}
	int32 ConfigCensusAge = 0;
	if (GConfig && GConfig->GetInt(TEXT("AnomalyCapture"), TEXT("CensusMaxVerdictAgeTicksDefault"), ConfigCensusAge, GGameIni))
	{
		CensusMaxVerdictAgeTicks = FMath::Clamp(ConfigCensusAge, 0, 600);
		bCensusMaxAgeFromIni = true;
	}
	bool bConfigCensusTranslucent = true;
	if (GConfig && GConfig->GetBool(TEXT("AnomalyCapture"), TEXT("bCensusExcludeTranslucentDefault"), bConfigCensusTranslucent, GGameIni))
	{
		bCensusExcludeTranslucent = bConfigCensusTranslucent;
		bCensusTranslucentFromIni = true;
	}
	bool bConfigTargetMask = true;
	if (GConfig && GConfig->GetBool(TEXT("AnomalyCapture"), TEXT("bTargetMaskDefault"), bConfigTargetMask, GGameIni))
	{
		bTargetMask = bConfigTargetMask;
		bTargetMaskFromIni = true;
	}
	bool bConfigCensusTranslucentWriters = false;
	if (GConfig && GConfig->GetBool(TEXT("AnomalyCapture"), TEXT("bCensusIncludeTranslucentCustomDepthWritersDefault"), bConfigCensusTranslucentWriters, GGameIni))
	{
		bCensusIncludeTranslucentWriters = bConfigCensusTranslucentWriters;
		bCensusTranslucentWritersFromIni = true;
	}
	if (!GMaskReduceFromConsole)
	{
		GMaskReduceMode = EAnomalyMaskReduceMode::Gpu;
		GMaskReduceFromIni = false;
		FString ConfigMaskReduce;
		if (GConfig && GConfig->GetString(TEXT("AnomalyCapture"), TEXT("MaskReduceDefault"), ConfigMaskReduce, GGameIni))
		{
			EAnomalyMaskReduceMode ParsedMode;
			if (ParseMaskReduceMode(ConfigMaskReduce, ParsedMode))
			{
				GMaskReduceMode = ParsedMode;
				GMaskReduceFromIni = true;
			}
			else
			{
				UE_LOG(LogAnomalyCapture, Warning,
					TEXT("Capture(mask): m34 MaskReduceDefault '%s' is not one of gpu|cpu|both and is REFUSED ")
					TEXT("(G144: an unrecognised value never silently becomes a default). The compiled default ")
					TEXT("gpu stands."),
					*ConfigMaskReduce);
			}
		}
	}
	int32 ConfigOutputHeight = 0;
	if (GConfig && GConfig->GetInt(TEXT("AnomalyCapture"), TEXT("CaptureOutputHeightDefault"), ConfigOutputHeight, GGameIni))
	{
		OutputHeightIni = ConfigOutputHeight;
		bOutputHeightFromIni = true;
	}
	bool bConfigTickPin = false;
	if (GConfig && GConfig->GetBool(TEXT("AnomalyCapture"), TEXT("bTickModePinDefault"), bConfigTickPin, GGameIni))
	{
		bTickPinEnabled = bConfigTickPin;
		bTickPinFromIni = true;
	}
	bool bConfigLabelsInDelivery = false;
	if (GConfig && GConfig->GetBool(TEXT("AnomalyCapture"), TEXT("bWriteLabelsInDeliveryDefault"), bConfigLabelsInDelivery, GGameIni))
	{
		bLabelsInDelivery = bConfigLabelsInDelivery;
		bLabelsInDeliveryFromIni = true;
	}
	int32 ConfigRunLog = -1;
	if (GConfig && GConfig->GetInt(TEXT("AnomalyCapture"), TEXT("RunLogDefault"), ConfigRunLog, GGameIni))
	{
		RunLogIni = FMath::Clamp(ConfigRunLog, -1, 1);
		bRunLogFromIni = true;
	}
	bool bConfigRunLogVerbose = false;
	if (GConfig && GConfig->GetBool(TEXT("AnomalyCapture"), TEXT("bRunLogVerboseDefault"), bConfigRunLogVerbose, GGameIni))
	{
		bRunLogVerbose = bConfigRunLogVerbose;
		bRunLogVerboseFromIni = true;
	}
	UE_LOG(LogAnomalyCapture, Log, TEXT("AnomalyCapture subsystem initialized (idle Ã¢â‚¬â€ use IAI.Capture.Start). Delivery mode: %s. Content clock: %s. Focus gate: %s. Grab point: %s (%s), default from %s."),
		bDeliveryMode ? TEXT("ON (client-facing output only)") : TEXT("off (full fidelity)"),
		ContentClock == EContentClock::Game ? TEXT("game (stamp target fps)") : TEXT("wall (stamp sustained on slow runs)"),
		bFocusGate ? TEXT("on (start waits for game-window focus)") : TEXT("off (start begins immediately)"),
		DescribeGrabPoint(),
		bSveCapture ? TEXT("scene colour, pre-Slate Ã¢â‚¬â€ UI EXCLUDED") : TEXT("presented backbuffer Ã¢â‚¬â€ UI INCLUDED"),
		bSveFromIni
			? TEXT("DefaultGame.ini [AnomalyCapture] bSveCaptureDefault")
			: TEXT("S4 COMPILED-IN DEFAULT (SVE, UI-free); no ini key present; IAI.Capture.SVE 0 selects the backbuffer/UI-on path"));
	UE_LOG(LogAnomalyCapture, Log,
		TEXT("Capture(mask): m27/m41 EFFECTIVE AT INIT - mask %s, from %s. m41 flipped the COMPILED default ON: ")
		TEXT("a missing ini key now DOWNGRADES PROVENANCE rather than silently restoring m25 labelling, which ")
		TEXT("is the failure G139 exists to make visible. Mask off means the m26 H5 cure is INACTIVE and this ")
		TEXT("build labels exactly as m25 did."),
		bMaskMeasure ? TEXT("ON (measure, report and veto)") : TEXT("off"),
		DescribeMaskSource());
	UE_LOG(LogAnomalyCapture, Log,
		TEXT("Capture(mask): m34 REDUCE AT INIT - maskReduce=%s, from %s. gpu reduces the visible mask to a ")
		TEXT("5 KB per-tag table ON THE GPU and reads back the table, not the surface; cpu is the NAMED BISECT ")
		TEXT("(the pre-m34 full-surface readback + render-thread scan); both runs the two side by side and ")
		TEXT("compares per armed frame (MASK-REDUCE COMPARE). The reduction is integer-atomic and BIT-EXACT ")
		TEXT("across modes. Inert while the mask itself is off."),
		LexToStringAnomalyMaskReduceMode(GMaskReduceMode), DescribeMaskReduceSource());
	UE_LOG(LogAnomalyCapture, Log,
		TEXT("Capture(census): m36/m37/m41 AT INIT - census %s (from %s), floor=%.2f%% (from %s), ")
		TEXT("maxVerdictAgeTicks=%d, excludeTranslucent=%d, includeTranslucentWriters=%d (from %s). m41 ")
		TEXT("flipped the COMPILED default ON. The census measures DRAWN PIXELS per selection candidate ")
		TEXT("through the m26 mask + m34 reduce, UPSTREAM of selection; the armed-frame measurement and the ")
		TEXT("zero-only veto are unchanged and remain the backstop. It requires the mask and async capture; ")
		TEXT("the EFFECTIVE value for a run is echoed at IAI.Capture.Start."),
		bCensus ? TEXT("ON") : TEXT("off"), DescribeCensusSource(),
		CensusFloorPct, DescribeCensusFloorSource(),
		CensusMaxVerdictAgeTicks, bCensusExcludeTranslucent ? 1 : 0,
		bCensusIncludeTranslucentWriters ? 1 : 0, DescribeCensusTranslucentWritersSource());
	UE_LOG(LogAnomalyCapture, Log,
		TEXT("Capture(m28): OUTPUT HEIGHT AT INIT - ini level %s. 0 means NATIVE and the written frames are ")
		TEXT("byte-identical to a pre-m28 build. This is the INI LEVEL ONLY; a console override or a per-run ")
		TEXT("argument can still beat it, and the EFFECTIVE value for a run is echoed at IAI.Capture.Start."),
		bOutputHeightFromIni
			? *FString::Printf(TEXT("%d, from DefaultGame.ini [AnomalyCapture] CaptureOutputHeightDefault"), OutputHeightIni)
			: TEXT("not set; no ini key present, so the compiled default 0 (native) stands unless overridden"));
	{
		FString RunLogInitSource;
		const bool bRunLogInitEffective = ResolveRunLogEffective(RunLogInitSource);
		UE_LOG(LogAnomalyCapture, Log,
			TEXT("Capture(runlog): m38 AT INIT - run log would be %s (%s), verboseKnob=%d (from %s). The run log is a ")
			TEXT("run-scoped copy of LogAnomaly + LogAnomalyCapture written to anomaly_log.txt beside annotation.json, ")
			TEXT("so a capture carries its own explanation and survives a log rotation. Three-state: -1 auto (mirrors ")
			TEXT("run.json - on when delivery is off), 0 off, 1 on. This is the INIT LEVEL ONLY; the EFFECTIVE value ")
			TEXT("for a run is echoed at IAI.Capture.Start."),
			bRunLogInitEffective ? TEXT("ON") : TEXT("OFF"), *RunLogInitSource,
			bRunLogVerbose ? 1 : 0,
			bRunLogVerboseFromIni
				? TEXT("DefaultGame.ini [AnomalyCapture] bRunLogVerboseDefault")
				: TEXT("COMPILED DEFAULT (off); IAI.Capture.RunLogVerbose 1 raises LogAnomaly to Verbose FOR ONE RUN and restores it"));
	}
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
	EndRunLog();
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
	if (SampleWorldTickEndHandle.IsValid())
	{
		FWorldDelegates::OnWorldTickEnd.Remove(SampleWorldTickEndHandle);
		SampleWorldTickEndHandle.Reset();
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

	if (bRunBegun)
	{
		++CaptureGameTicks;
		if (bTickPinApplied)
		{
			if (AnomalyTickPin::Read())
			{
				++TickPinReasserts;
			}
			AnomalyTickPin::Write(false);
		}
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
			UE_LOG(LogAnomalyCapture, Log, TEXT("Capture armed Ã¢â‚¬â€ still waiting for game-window focus (%.0fs)."), NowWall - ArmWaitStartWall);
		}
		if (FocusWaitTimeoutSeconds > 0.0 && NowWall - ArmWaitStartWall >= FocusWaitTimeoutSeconds)
		{
			UE_LOG(LogAnomalyCapture, Warning, TEXT("Capture armed Ã¢â‚¬â€ game-window focus not acquired after %.0fs; starting anyway (IAI.Capture.Stop to cancel)."), FocusWaitTimeoutSeconds);
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

void UAnomalyCaptureSubsystem::OnWorldTickEndSample(UWorld* World, ELevelTick TickType, float DeltaSeconds)
{
#if ANOMALY_CAPTURE
	if (World != GetWorld() || !bRunning)
	{
		return;
	}
	SampleDeferredActiveState();
#endif
}

void UAnomalyCaptureSubsystem::OnWorldTickEndMask(UWorld* World, ELevelTick TickType, float DeltaSeconds)
{
#if ANOMALY_CAPTURE
	if (World != GetWorld() || !bMaskMeasure || !bRunning || !Async.IsValid() || !Async->MaskExtension.IsValid())
	{
		return;
	}

	TSet<uint8> ExtraTags =
		(bCensusEffective && Async->Census.IsActive()) ? Async->Census.GetLegitTags() : TSet<uint8>();
	if (bBenchMaskPairingProbe && MaskPairingProbe.IsValid())
	{
		ExtraTags.Add((uint8)GMaskPairingProbeTag);
	}
	Async->MaskMeasure.SetExtraAssignedTags(ExtraTags);
	Async->MaskMeasure.VerifyPendingTags();
	Async->MaskExtension->EnqueueDrain();
	ReleaseTargetMaskSelfTags();
	ServiceTargetMask();
	Async->MaskMeasure.CollectResults(Async->MaskExtension.Get());

	const bool bCapturedThisTick = bTargetMaskEffective && (TargetMaskArmedTick == GFrameCounter)
		&& (TargetMaskArmedSessionIndex >= 0);
	const bool bArmedNormal = Async->MaskMeasure.ArmIfMeasurable(Async->MaskExtension.Get(), GFrameCounter, false);

	if (bCapturedThisTick)
	{
		EnsureMaskRecordsForCapturedFrame();
		if (!ArmTargetMaskOwn(TargetMaskArmedSessionIndex))
		{
			TargetMaskOutcome.Add(TargetMaskArmedSessionIndex, (uint8)AnomalyLabel::EAnomalyMaskState::Unmeasured);
			++TargetMaskUnavailable;
		}
		TargetMaskArmedSessionIndex = -1;
	}

	if (!bArmedNormal && bMaskProbe && !bMaskProbeFiredThisRun && !bDeliveryMode)
	{
		if (Async->MaskMeasure.ArmProbeOnHidden(Async->MaskExtension.Get(), GFrameCounter))
		{
			bMaskProbeFiredThisRun = true;
		}
	}

	if (bCensusEffective && Async->Census.IsActive())
	{
		Async->Census.Tick(World, Async->MaskExtension.Get(), bArmedNormal, Async->MaskMeasure.BuildBaseTagSet());
		if (Async->Census.ConsumeCycleJustCompleted())
		{
			RunStencilHygieneCheck(false);
		}
	}
#endif
}

static constexpr uint64 GTargetMaskRequestBit = 1ull << 61;

void UAnomalyCaptureSubsystem::EnqueueTargetMaskPng(int32 SessionIndex, const TArray<uint8>& Gray, int32 W, int32 H)
{
	if (!Async.IsValid() || !Async->Writer.IsValid() || SessionIndex < 0 || W <= 0 || H <= 0)
	{
		return;
	}
	FAnomalyAsyncWriter::FJob Job;
	Job.bGrayMask = true;
	Job.OutputDir = RunDir;
	Job.ImageRelPath = FString::Printf(TEXT("target_mask/frame_%05d.png"), SessionIndex);
	Job.RawBytes = Gray;
	Job.Width = W;
	Job.Height = H;
	Async->Writer->Enqueue(MoveTemp(Job));
}

void UAnomalyCaptureSubsystem::ReleaseTargetMaskSelfTags()
{
	if (TargetMaskSelfTagged.Num() == 0 || TargetMaskSelfTaggedTick == GFrameCounter)
	{
		return;
	}
	for (const TWeakObjectPtr<AActor>& Weak : TargetMaskSelfTagged)
	{
		if (AActor* Actor = Weak.Get())
		{
			AnomalyStencilTag::RestoreActor(Actor);
			++TargetMaskTagFlips;
		}
	}
	TargetMaskSelfTagged.Reset();
}

void UAnomalyCaptureSubsystem::EnsureMaskRecordsForCapturedFrame()
{
	if (!Async.IsValid() || !bMaskMeasure)
	{
		return;
	}
	const UAnomalyAutoInjectorSubsystem* Auto = ResolveAuto();
	if (!Auto)
	{
		return;
	}
	for (const FAutoLiveFireInfo& F : Auto->GetLiveFires())
	{
		Async->MaskMeasure.FindOrAddRecord(F.Id, F.Target, F.StartFrame, const_cast<AActor*>(F.TargetActor.Get()));
	}
}

bool UAnomalyCaptureSubsystem::ArmTargetMaskOwn(int32 SessionIndex)
{
	if (!Async.IsValid() || !Async->MaskExtension.IsValid())
	{
		return false;
	}

	const UAnomalyAutoInjectorSubsystem* Auto = ResolveAuto();
	if (!Auto)
	{
		return false;
	}

	TArray<AActor*> Visible;
	TArray<uint8> Tags;
	TSet<uint8> LiveTags;
	int32 ScanFires = 0;
	int32 ScanLabelled = 0;
	int32 ScanWithRecord = 0;
	for (const FAutoLiveFireInfo& F : Auto->GetLiveFires())
	{
		++ScanFires;
		AActor* Actor = F.TargetActor.Get();
		if (!Actor)
		{
			continue;
		}
		if (!IsFireLabelledThisFrame(F))
		{
			continue;
		}
		++ScanLabelled;
		for (const FAnomalyMaskRecord& R : Async->MaskMeasure.GetRecords())
		{
			if (R.Id == F.Id && R.Target == F.Target && R.StartFrame == F.StartFrame && R.Tag != 0)
			{
				++ScanWithRecord;
				LiveTags.Add(R.Tag);
				if (!Actor->IsHidden())
				{
					Visible.Add(Actor);
					Tags.Add(R.Tag);
				}
				break;
			}
		}
	}

	bool bProbeForcesArm = false;
	if (bBenchMaskPairingProbe && MaskPairingProbe.IsValid())
	{
		LiveTags.Add((uint8)GMaskPairingProbeTag);
		bProbeForcesArm = true;
	}

	if (Visible.Num() == 0 && !bProbeForcesArm)
	{
		if (ScanFires > 0)
		{
			UE_LOG(LogAnomalyCapture, Log,
				TEXT("Capture(m44): TARGET MASK NOT ARMED session_index=%d scanned fires=%d labelled=%d ")
				TEXT("withRecord=%d visible=%d. A bare 'not armed' cannot be diagnosed; these counts say ")
				TEXT("WHICH stage refused. Silent when there are no live fires, because not arming is ")
				TEXT("then the correct behaviour."),
				SessionIndex, ScanFires, ScanLabelled, ScanWithRecord, Visible.Num());
		}
		return false;
	}

	int32 TaggedCount = 0;
	for (int32 i = 0; i < Visible.Num(); ++i)
	{
		if (AnomalyStencilTag::IsAnyComponentTagged(Visible[i]))
		{
			++TaggedCount;
			continue;
		}
		if (AnomalyStencilTag::TagActor(Visible[i], (int32)Tags[i]) > 0)
		{
			TargetMaskSelfTagged.Add(Visible[i]);
				TargetMaskSelfTaggedTick = GFrameCounter;
			++TaggedCount;
			++TargetMaskTagFlips;
		}
	}

	if (TaggedCount == 0 && !bProbeForcesArm)
	{
		return false;
	}

	const uint64 RequestId = GTargetMaskRequestBit | (++TargetMaskOwnSerial);
	Async->MaskExtension->SetAssignedTags(Async->MaskMeasure.BuildAssignedTagSet());
	Async->MaskExtension->ArmMask(RequestId, true);
	TargetMaskPendingSessionIndex.Add(RequestId, SessionIndex);
	TargetMaskPendingTags.Add(RequestId, LiveTags);
	return true;
}

void UAnomalyCaptureSubsystem::ServiceTargetMask()
{
	if (!bTargetMaskEffective || !Async.IsValid() || !Async->MaskExtension.IsValid()
		|| TargetMaskPendingSessionIndex.Num() == 0)
	{
		return;
	}

	TArray<uint64> Ready;
	for (const TPair<uint64, int32>& Pair : TargetMaskPendingSessionIndex)
	{
		FAnomalyMaskResult Result;
		if (!Async->MaskExtension->TakeMaskResult(Pair.Key, Result, true))
		{
			continue;
		}
		Ready.Add(Pair.Key);

		const TSet<uint8>* LiveTagsPtr = TargetMaskPendingTags.Find(Pair.Key);
		const TSet<uint8> EventTags = LiveTagsPtr ? *LiveTagsPtr : TSet<uint8>();

		const int32 W = Result.ViewRectSize.X;
		const int32 H = Result.ViewRectSize.Y;
		if (W <= 0 || H <= 0 || Result.MaskPixels.Num() < (int64)W * (int64)H)
		{
			++TargetMaskUnavailable;
			TargetMaskOutcome.Add(Pair.Value, (uint8)AnomalyLabel::EAnomalyMaskState::Unmeasured);
			UE_LOG(LogAnomalyCapture, Warning,
				TEXT("Capture(m43): TARGET MASK UNAVAILABLE for session_index %d (id=%llu, %dx%d, pixels=%d). ")
				TEXT("The labels row for this frame carries mask_file:null - NOT MEASURED, which is a different ")
				TEXT("fact from a blank mask."),
				Pair.Value, Pair.Key, W, H, Result.MaskPixels.Num());
			continue;
		}

		TargetMaskW = W;
		TargetMaskH = H;

		TArray<uint8> Gray = MoveTemp(Result.MaskPixels);
		const int32 N = W * H;
		int32 KeptPixels = 0;
		for (int32 i = 0; i < N; ++i)
		{
			const uint8 V = Gray[i];
			if (V != 0 && !EventTags.Contains(V))
			{
				Gray[i] = 0;
			}
			else if (V != 0)
			{
				++KeptPixels;
			}
		}

		for (uint8 Tag : EventTags)
		{
			if (const FAnomalyMaskTagResult* R = Result.TagResults.Find(Tag))
			{
				if (R->Count > 0)
				{
					if (!TargetMaskFirstFrame.Contains(Tag))
					{
						TargetMaskFirstFrame.Add(Tag, Pair.Value);
					}
					TargetMaskLastFrame.Add(Tag, Pair.Value);
				}
			}
		}

		for (uint8 Tag : EventTags)
		{
			const FAnomalyMaskTagResult* R = Result.TagResults.Find(Tag);
			const int32 TableCount = R ? R->Count : 0;
			int32 PngCount = 0;
			for (int32 i = 0; i < N; ++i)
			{
				if (Gray[i] == Tag) { ++PngCount; }
			}
			UE_LOG(LogAnomalyCapture, Log,
				TEXT("Capture(m43): MASK-TIE session_index=%d tag=%d tableCount=%d pngCount=%d %s - the ")
				TEXT("delivered mask is counted against the SAME reduce table the veto reads, so the shipped ")
				TEXT("silhouette is provably the one the labels were judged on."),
				Pair.Value, (int32)Tag, TableCount, PngCount,
				(TableCount == PngCount) ? TEXT("MATCH") : TEXT("*** MISMATCH ***"));
		}

		if (KeptPixels > 0)
		{
			EnqueueTargetMaskPng(Pair.Value, Gray, W, H);
			++TargetMaskMeasured;
			TargetMaskOutcome.Add(Pair.Value, (uint8)AnomalyLabel::EAnomalyMaskState::Present);
		}
		else
		{
			++TargetMaskHiddenBlank;
			TargetMaskOutcome.Add(Pair.Value, (uint8)AnomalyLabel::EAnomalyMaskState::Empty);
			UE_LOG(LogAnomalyCapture, Log,
				TEXT("Capture(m44): TARGET MASK EMPTY for session_index %d - the mask was MEASURED and the ")
				TEXT("target contributed zero pixels, so NO file is written and the labels row reads ")
				TEXT("mask_state:\"empty\" with mask_file:null. That is a different fact from ")
				TEXT("mask_state:\"unmeasured\", which means no measurement exists."),
				Pair.Value);
		}
	}

	for (uint64 Id : Ready)
	{
		TargetMaskPendingSessionIndex.Remove(Id);
		TargetMaskPendingTags.Remove(Id);
	}
}

void UAnomalyCaptureSubsystem::RunStencilHygieneCheck(bool bFinal)
{
#if ANOMALY_CAPTURE
	if (!Async.IsValid())
	{
		return;
	}
	TMap<TWeakObjectPtr<UPrimitiveComponent>, int32> Now;
	AnomalyStencilTag::SnapshotCustomDepthEnabled(GetWorld(), Now);
	TSet<TWeakObjectPtr<UPrimitiveComponent>> Exclude;
	if (!bFinal)
	{
		AnomalyStencilTag::GetTaggedComponents(Exclude);
	}
	FString FirstDiff;
	const int32 Diffs = AnomalyStencilTag::DiffCustomDepthSnapshots(
		Async->PreRunStencilSnapshot, Now, bFinal ? nullptr : &Exclude, FirstDiff);
	if (Diffs == 0)
	{
		UE_LOG(LogAnomalyCapture, Log,
			TEXT("Census: CENSUS-HYGIENE %s identical=1 excluded=%d - the set of components with ")
			TEXT("bRenderCustomDepth and their stencil values matches the pre-run snapshot%s."),
			bFinal ? TEXT("final") : TEXT("cycle"), Exclude.Num(),
			bFinal ? TEXT("") : TEXT(" (components currently and legitimately tagged are excluded from the cycle check)"));
	}
	else
	{
		UE_LOG(LogAnomalyCapture, Warning,
			TEXT("Census: CENSUS-HYGIENE %s DIFF n=%d first=%s - host-visible custom-depth state does NOT match ")
			TEXT("the pre-run snapshot. With the leak probe ON this is the G96 proof firing as designed; ")
			TEXT("otherwise it is a hygiene defect and the leg FAILS P-C6."),
			bFinal ? TEXT("final") : TEXT("cycle"), Diffs, *FirstDiff);
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
	bMaskMeasureFromConsole = true;
	UE_LOG(LogAnomalyCapture, Log,
		TEXT("IAI.Capture.Mask: %s - THE BISECT SWITCH, and it takes effect BETWEEN RUNS, not mid-run. It gates ")
		TEXT("m26 slices 1+2+3 together: MEASURE, mask{provided}, and the VETO. Setting it 0 and re-capturing ")
		TEXT("returns the build to m25 labelling behaviour in about thirty seconds, with no rebuild. It overrides ")
		TEXT("DefaultGame.ini [AnomalyCapture] bMaskMeasureDefault for this session. The full banner prints at ")
		TEXT("IAI.Capture.Start."),
		bMaskMeasure ? TEXT("ON") : TEXT("off"));
}

void UAnomalyCaptureSubsystem::SetCensus(bool bInCensus)
{
	if (bRunning)
	{
		UE_LOG(LogAnomalyCapture, Warning, TEXT("IAI.Capture.Census: ignored - a capture run is in progress."));
		return;
	}
	bCensus = bInCensus;
	bCensusFromIni = false;
	bCensusFromConsole = true;
	UE_LOG(LogAnomalyCapture, Log,
		TEXT("IAI.Capture.Census: %s - takes effect BETWEEN RUNS. Overrides DefaultGame.ini [AnomalyCapture] ")
		TEXT("bSelectionCensusDefault for this session. It requires the mask (IAI.Capture.Mask) and async capture; ")
		TEXT("read the EFFECTIVE value and its provenance from the StartRun echo, never from here."),
		bCensus ? TEXT("ON") : TEXT("off"));
}

void UAnomalyCaptureSubsystem::SetCensusFloorPct(float InPct)
{
	if (bRunning)
	{
		UE_LOG(LogAnomalyCapture, Warning, TEXT("IAI.Capture.CensusFloor: ignored - a capture run is in progress."));
		return;
	}
	if (InPct < 0.0f || InPct > 100.0f)
	{
		UE_LOG(LogAnomalyCapture, Warning,
			TEXT("IAI.Capture.CensusFloor: %.2f is outside [0,100] and is REFUSED (out-of-range is refused, ")
			TEXT("never clamped). Current value unchanged (%.2f)."), InPct, CensusFloorPct);
		return;
	}
	CensusFloorPct = InPct;
	bCensusFloorFromIni = false;
	bCensusFloorFromConsole = true;
	UE_LOG(LogAnomalyCapture, Log,
		TEXT("IAI.Capture.CensusFloor: %.2f%% - the census eligibility floor on MEASURED DRAWN coverage ")
		TEXT("(drawn_px/frame_px). DELIBERATELY a separate knob from IAI.SetMinScreenCoverage, whose operand ")
		TEXT("stays the BOUNDS rect and which still governs the NOT_MEASURABLE fallback path - one knob driving ")
		TEXT("two operands would couple the fallback to the future floor decision. Takes effect BETWEEN RUNS."),
		CensusFloorPct);
}

void UAnomalyCaptureSubsystem::SetCensusMaxVerdictAgeTicks(int32 InTicks)
{
	if (bRunning)
	{
		UE_LOG(LogAnomalyCapture, Warning, TEXT("IAI.Capture.CensusMaxAge: ignored - a capture run is in progress."));
		return;
	}
	if (InTicks < 0 || InTicks > 600)
	{
		UE_LOG(LogAnomalyCapture, Warning,
			TEXT("IAI.Capture.CensusMaxAge: %d is outside [0,600] and is REFUSED. Current value unchanged (%d)."),
			InTicks, CensusMaxVerdictAgeTicks);
		return;
	}
	CensusMaxVerdictAgeTicks = InTicks;
	bCensusMaxAgeFromIni = false;
	bCensusMaxAgeFromConsole = true;
	UE_LOG(LogAnomalyCapture, Log,
		TEXT("IAI.Capture.CensusMaxAge: %d tick(s) - a census verdict older than this at fire time is EXPIRED ")
		TEXT("and the candidate re-measures before regaining eligibility. 0 makes every verdict expired, which ")
		TEXT("is the P-C11 loud-inert control. Takes effect BETWEEN RUNS."),
		CensusMaxVerdictAgeTicks);
}

void UAnomalyCaptureSubsystem::SetCensusExcludeTranslucent(bool bInExclude)
{
	if (bRunning)
	{
		UE_LOG(LogAnomalyCapture, Warning, TEXT("IAI.Capture.CensusTranslucent: ignored - a capture run is in progress."));
		return;
	}
	bCensusExcludeTranslucent = bInExclude;
	bCensusTranslucentFromIni = false;
	bCensusTranslucentFromConsole = true;
	UE_LOG(LogAnomalyCapture, Log,
		TEXT("IAI.Capture.CensusTranslucent: %s - R2 ruling: a candidate whose EVERY renderable slot is ")
		TEXT("translucent without AllowTranslucentCustomDepthWrites is EXCLUDED from selection (owner ruled ")
		TEXT("such targets unusable; they also cannot write custom depth, so a census measurement of them ")
		TEXT("would be a false zero - the H6 route-e shape). OFF exists so the route-e v2 experiment needs ")
		TEXT("no cook. Takes effect BETWEEN RUNS."),
		bCensusExcludeTranslucent ? TEXT("ON (exclude)") : TEXT("off (translucent candidates are measured)"));
}

void UAnomalyCaptureSubsystem::SetCensusIncludeTranslucentWriters(bool bInInclude)
{
	if (bRunning)
	{
		UE_LOG(LogAnomalyCapture, Warning, TEXT("IAI.Capture.CensusTranslucentWriters: ignored - a capture run is in progress."));
		return;
	}
	bCensusIncludeTranslucentWriters = bInInclude;
	bCensusTranslucentWritersFromIni = false;
	bCensusTranslucentWritersFromConsole = true;
	UE_LOG(LogAnomalyCapture, Log,
		TEXT("IAI.Capture.CensusTranslucentWriters: %s - m41. TWO KNOBS, ONE QUESTION EACH: ")
		TEXT("IAI.Capture.CensusTranslucent decides WHETHER translucent-only candidates are excluded at all; ")
		TEXT("this one decides WHETHER a translucent slot that opts into custom-depth writes still counts as ")
		TEXT("translucent-only. It is consulted only when the first is ON. m41 default OFF: a translucent-only ")
		TEXT("candidate is EXCLUDED regardless of AllowTranslucentCustomDepthWrites, because the opt-in makes ")
		TEXT("the census measure a GEOMETRIC SILHOUETTE for a target that contributes no visible colour - and ")
		TEXT("the armed-frame veto reads THE SAME custom-depth silhouette, so on that class the two are not ")
		TEXT("independent checks. ON restores the pre-m41 behaviour for hosts that want glass-type targets. ")
		TEXT("Takes effect BETWEEN RUNS."),
		bCensusIncludeTranslucentWriters
			? TEXT("ON (custom-depth-writing translucents are MEASURED, pre-m41 behaviour)")
			: TEXT("off (translucent-only is EXCLUDED regardless of the opt-in)"));
}

void UAnomalyCaptureSubsystem::SetBenchMaskPairingProbe(bool bInOn)
{
	if (bRunning)
	{
		UE_LOG(LogAnomalyCapture, Warning, TEXT("IAI.Bench.MaskPairingProbe: ignored - a capture run is in progress."));
		return;
	}
	bBenchMaskPairingProbe = bInOn;
	UE_LOG(LogAnomalyCapture, Warning,
		TEXT("IAI.Bench.MaskPairingProbe -> %s. BENCH DEVICE, console only, no ini key, never in a client ")
		TEXT("payload. ON spawns a MOVABLE magenta cube in front of the settled bench camera, tags it ONCE ")
		TEXT("at spawn (so no render-state recreate is ever involved) and alternates its position every ")
		TEXT("captured tick. The picture is the trusted reference (m31-paired); the mask centroid for the ")
		TEXT("probe tag is compared against it. If the mask shows the PREVIOUS tick's position, the mask ")
		TEXT("arm is being served by the previous render. NEVER ship a capture taken with this ON."),
		bBenchMaskPairingProbe ? TEXT("ON") : TEXT("OFF"));
}

void UAnomalyCaptureSubsystem::SpawnMaskPairingProbe()
{
	if (!bBenchMaskPairingProbe || MaskPairingProbe.IsValid())
	{
		return;
	}
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	UStaticMesh* Cube = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
	UMaterialInterface* Pink = LoadObject<UMaterialInterface>(
		nullptr, TEXT("/AnomalyInjector/Materials/M_CorruptedTexture_Pink.M_CorruptedTexture_Pink"));
	if (!Cube)
	{
		UE_LOG(LogAnomalyCapture, Error,
			TEXT("Capture(bench): MASK-PAIRING PROBE REFUSED - /Engine/BasicShapes/Cube did not load. ")
			TEXT("Nothing was spawned and no fixture was improvised."));
		return;
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AStaticMeshActor* Probe = World->SpawnActor<AStaticMeshActor>(
		AStaticMeshActor::StaticClass(), FTransform(GMaskPairingProbePosA), Params);
	if (!Probe)
	{
		UE_LOG(LogAnomalyCapture, Error, TEXT("Capture(bench): MASK-PAIRING PROBE REFUSED - spawn failed."));
		return;
	}

	UStaticMeshComponent* Comp = Probe->GetStaticMeshComponent();
	Comp->SetMobility(EComponentMobility::Movable);
	Comp->SetStaticMesh(Cube);
	if (Pink)
	{
		Comp->SetMaterial(0, Pink);
	}
	Comp->SetWorldScale3D(FVector(1.5f, 1.5f, 1.5f));
	Comp->SetRenderCustomDepth(true);
	Comp->SetCustomDepthStencilValue(GMaskPairingProbeTag);

	MaskPairingProbe = Probe;
	MaskPairingProbePos = 0;
	UE_LOG(LogAnomalyCapture, Warning,
		TEXT("Capture(bench): MASK-PAIRING PROBE SPAWNED tag=%d posA=(%s) posB=(%s) - tagged ONCE at spawn, ")
		TEXT("Movable, position alternates per captured tick via SetActorLocation (a transform update, NOT a ")
		TEXT("render-state recreate)."),
		GMaskPairingProbeTag, *GMaskPairingProbePosA.ToString(), *GMaskPairingProbePosB.ToString());
}

void UAnomalyCaptureSubsystem::StepMaskPairingProbe(int32 SessionIndex)
{
	if (!bBenchMaskPairingProbe)
	{
		return;
	}
	AActor* Probe = MaskPairingProbe.Get();
	if (!Probe)
	{
		return;
	}
	MaskPairingProbePos = (SessionIndex % 2);
	const FVector Where = (MaskPairingProbePos == 0) ? GMaskPairingProbePosA : GMaskPairingProbePosB;
	Probe->SetActorLocation(Where);
	UE_LOG(LogAnomalyCapture, Log,
		TEXT("Capture(bench): MASK-PAIRING PROBE STEP session_index=%d pos=%s y=%.1f gameFrame=%llu"),
		SessionIndex, (MaskPairingProbePos == 0) ? TEXT("A") : TEXT("B"), Where.Y, (uint64)GFrameCounter);
}

void UAnomalyCaptureSubsystem::DestroyMaskPairingProbe()
{
	if (AActor* Probe = MaskPairingProbe.Get())
	{
		Probe->Destroy();
	}
	MaskPairingProbe = nullptr;
}

void UAnomalyCaptureSubsystem::SetBenchCensusFixedExpiry(bool bInFixed)
{
	if (bRunning)
	{
		UE_LOG(LogAnomalyCapture, Warning, TEXT("IAI.Bench.CensusFixedExpiry: ignored - a capture run is in progress."));
		return;
	}
	bBenchCensusFixedExpiry = bInFixed;
	UE_LOG(LogAnomalyCapture, Warning,
		TEXT("IAI.Bench.CensusFixedExpiry -> %s. BENCH DEVICE, console only, no ini key, never in a client ")
		TEXT("payload. ON forces the PRE-m41 FIXED expiry window (the knob alone, cycle length ignored) so ")
		TEXT("D-G1's A-side and B-side can run on ONE binary. NEVER ship a capture taken with this ON."),
		bBenchCensusFixedExpiry ? TEXT("ON") : TEXT("OFF"));
}

void UAnomalyCaptureSubsystem::SetBenchCensusBatchCap(int32 InCap)
{
	if (bRunning)
	{
		UE_LOG(LogAnomalyCapture, Warning, TEXT("IAI.Bench.CensusBatchCap: ignored - a capture run is in progress."));
		return;
	}
	if (InCap < 0 || InCap > 55)
	{
		UE_LOG(LogAnomalyCapture, Warning,
			TEXT("IAI.Bench.CensusBatchCap: %d is outside [0,55] and is REFUSED. Current value unchanged (%d)."),
			InCap, BenchCensusBatchCap);
		return;
	}
	BenchCensusBatchCap = InCap;
	UE_LOG(LogAnomalyCapture, Warning,
		TEXT("IAI.Bench.CensusBatchCap -> %d. BENCH DEVICE, console only, no ini key, never in a client ")
		TEXT("payload. 0 = off. A small cap stretches a census cycle over many more ticks, which is how ")
		TEXT("D-G1 manufactures a cycle longer than the expiry knob without needing a 300-candidate host."),
		BenchCensusBatchCap);
}

void UAnomalyCaptureSubsystem::SetBenchCensusDropEveryNth(int32 InN)
{
	if (bRunning)
	{
		UE_LOG(LogAnomalyCapture, Warning, TEXT("IAI.Bench.CensusDropEntry: ignored - a capture run is in progress."));
		return;
	}
	if (InN < 0 || InN > 64)
	{
		UE_LOG(LogAnomalyCapture, Warning,
			TEXT("IAI.Bench.CensusDropEntry: %d is outside [0,64] and is REFUSED. Current value unchanged (%d)."),
			InN, BenchCensusDropEveryNth);
		return;
	}
	BenchCensusDropEveryNth = InN;
	UE_LOG(LogAnomalyCapture, Warning,
		TEXT("IAI.Bench.CensusDropEntry -> %d. BENCH DEVICE, console only, no ini key, never in a client ")
		TEXT("payload. 0 = off. N omits every Nth prefiltered actor from the census's own candidate list ")
		TEXT("while leaving it visible to the fire path, so census_fires_unseen_candidates is PROVEN ABLE TO ")
		TEXT("BE NON-ZERO (E-G2, G96) - without that its zero on a client host would be blindness, not a ")
		TEXT("reading. NEVER ship a capture taken with this ON."),
		BenchCensusDropEveryNth);
}

void UAnomalyCaptureSubsystem::SetCensusReservation(bool bInReserve)
{
	if (bRunning)
	{
		UE_LOG(LogAnomalyCapture, Warning, TEXT("IAI.Capture.CensusReservation: ignored - a capture run is in progress."));
		return;
	}
	bCensusReservation = bInReserve;
	UE_LOG(LogAnomalyCapture, Warning,
		TEXT("IAI.Capture.CensusReservation: %s - R10 host stencil reservation. OFF is a GATE LEVER for the ")
		TEXT("P-C12 companion ONLY (proving the instrument can fail, G96); it must be ON in any real leg, or ")
		TEXT("host-set custom-depth pixels can be COUNTED under a plugin tag. Takes effect BETWEEN RUNS."),
		bCensusReservation ? TEXT("ON") : TEXT("OFF"));
}

void UAnomalyCaptureSubsystem::SetCensusLeakProbe(bool bInProbe)
{
	if (bRunning)
	{
		UE_LOG(LogAnomalyCapture, Warning, TEXT("IAI.Capture.CensusLeakProbe: ignored - a capture run is in progress."));
		return;
	}
	bCensusLeakProbe = bInProbe;
	UE_LOG(LogAnomalyCapture, Warning,
		TEXT("IAI.Capture.CensusLeakProbe: %s - GATE ARTEFACT, default OFF, MUST be OFF in any build that ships. ")
		TEXT("ON: the census DELIBERATELY leaves exactly one tagged component un-restored so the final ")
		TEXT("CENSUS-HYGIENE check is proven able to report a DIFF (P-C6 companion, G96). Takes effect BETWEEN RUNS."),
		bCensusLeakProbe ? TEXT("ON") : TEXT("off"));
}

void UAnomalyCaptureSubsystem::SetCensusCoArm(bool bInCoArm)
{
	if (bRunning)
	{
		UE_LOG(LogAnomalyCapture, Warning, TEXT("IAI.Capture.CensusCoArm: ignored - a capture run is in progress."));
		return;
	}
	bCensusCoArm = bInCoArm;
	UE_LOG(LogAnomalyCapture, Warning,
		TEXT("IAI.Capture.CensusCoArm: %s - GATE ARTEFACT, default OFF. ON: the census arms ONLY on ticks where ")
		TEXT("the event mask also armed, forcing the census arm to queue BEHIND the event arm and pop on the ")
		TEXT("NEXT view family - the P-C10 delayed-pop attribution control (R4). Takes effect BETWEEN RUNS."),
		bCensusCoArm ? TEXT("ON") : TEXT("off"));
}

const TCHAR* UAnomalyCaptureSubsystem::DescribeCensusSource() const
{
	if (bCensusFromConsole)
	{
		return TEXT("IAI.Capture.Census (console)");
	}
	if (bCensusFromIni)
	{
		return TEXT("DefaultGame.ini [AnomalyCapture] bSelectionCensusDefault");
	}
	return TEXT("COMPILED DEFAULT (on)");
}

const TCHAR* UAnomalyCaptureSubsystem::DescribeMaskSource() const
{
	if (bMaskMeasureFromConsole)
	{
		return TEXT("IAI.Capture.Mask (console)");
	}
	if (bMaskMeasureFromIni)
	{
		return TEXT("DefaultGame.ini [AnomalyCapture] bMaskMeasureDefault");
	}
	return TEXT("COMPILED DEFAULT (on)");
}

const TCHAR* UAnomalyCaptureSubsystem::DescribeTargetMaskSource() const
{
	if (bTargetMaskFromConsole)
	{
		return TEXT("IAI.Capture.TargetMask (console)");
	}
	if (bTargetMaskFromIni)
	{
		return TEXT("DefaultGame.ini [AnomalyCapture] bTargetMaskDefault");
	}
	return TEXT("COMPILED DEFAULT (on)");
}

void UAnomalyCaptureSubsystem::SetTargetMask(bool bInOn)
{
	if (bRunning)
	{
		UE_LOG(LogAnomalyCapture, Warning, TEXT("IAI.Capture.TargetMask: ignored - a capture run is in progress."));
		return;
	}
	bTargetMask = bInOn;
	bTargetMaskFromIni = false;
	bTargetMaskFromConsole = true;
	UE_LOG(LogAnomalyCapture, Log,
		TEXT("IAI.Capture.TargetMask: %s - m43. Takes effect BETWEEN RUNS. It writes one 8-bit grayscale PNG ")
		TEXT("per captured frame at target_mask/frame_NNNNN.png (numbered by SESSION INDEX, so it sorts with ")
		TEXT("Actual_Frames), whose non-zero values are the stencil tags of the ANOMALY TARGETS visible in ")
		TEXT("that frame. It reuses the m26 visible-mask pass - no new shader, no new render pass - and it ")
		TEXT("does NOT touch the m26 measurement or the veto. Requires the mask (IAI.Capture.Mask)."),
		bTargetMask ? TEXT("ON") : TEXT("off"));
}

const TCHAR* UAnomalyCaptureSubsystem::DescribeCensusTranslucentWritersSource() const
{
	if (bCensusTranslucentWritersFromConsole)
	{
		return TEXT("IAI.Capture.CensusTranslucentWriters (console)");
	}
	if (bCensusTranslucentWritersFromIni)
	{
		return TEXT("DefaultGame.ini [AnomalyCapture] bCensusIncludeTranslucentCustomDepthWritersDefault");
	}
	return TEXT("COMPILED DEFAULT (off)");
}

int32 UAnomalyCaptureSubsystem::ScanHostPostProcessCustomDepthReaders(UWorld* World) const
{
	int32 Volumes = 0;
	int32 CameraBlends = 0;
	int32 BlendableEntries = 0;
	int32 MaterialsSeen = 0;
	int32 Readers = 0;
	int32 SceneTextureUsers = 0;
	FString Names;

	const ERHIFeatureLevel::Type FeatureLevel =
		(World && World->Scene) ? World->Scene->GetFeatureLevel() : GMaxRHIFeatureLevel;

	auto ConsiderBlendable = [&](UObject* Object)
	{
		UMaterialInterface* MI = Cast<UMaterialInterface>(Object);
		if (!MI)
		{
			return;
		}
		++MaterialsSeen;

		const FMaterialResource* Resource = MI->GetMaterialResource(FeatureLevel);
		if (!Resource)
		{
			return;
		}
		FMaterialShaderMap* ShaderMap = Resource->GetGameThreadShaderMap();
		if (!ShaderMap)
		{
			return;
		}

		if (ShaderMap->UsesSceneTexture(PPI_PostProcessInput0)
			|| ShaderMap->UsesSceneTexture(PPI_SceneColor)
			|| ShaderMap->UsesSceneTexture(PPI_SceneDepth))
		{
			++SceneTextureUsers;
		}

		const bool bDepth = ShaderMap->UsesSceneTexture(PPI_CustomDepth);
		const bool bStencil = ShaderMap->UsesSceneTexture(PPI_CustomStencil);
		if (bDepth || bStencil)
		{
			++Readers;
			Names += FString::Printf(TEXT(" %s(%s)"), *MI->GetName(),
				(bDepth && bStencil) ? TEXT("depth+stencil") : (bDepth ? TEXT("depth") : TEXT("stencil")));
		}
	};

	auto ConsiderSettings = [&](const FPostProcessSettings& Settings)
	{
		for (const FWeightedBlendable& Blendable : Settings.WeightedBlendables.Array)
		{
			++BlendableEntries;
			ConsiderBlendable(Blendable.Object);
		}
	};

	if (World)
	{
		for (TActorIterator<APostProcessVolume> It(World); It; ++It)
		{
			if (APostProcessVolume* Volume = *It)
			{
				++Volumes;
				ConsiderSettings(Volume->Settings);
			}
		}

		if (APlayerController* PC = World->GetFirstPlayerController())
		{
			if (APlayerCameraManager* CameraManager = PC->PlayerCameraManager)
			{
				TArray<FPostProcessSettings> const* BlendSettings = nullptr;
				TArray<float> const* BlendWeights = nullptr;
				CameraManager->GetCachedPostProcessBlends(BlendSettings, BlendWeights);
				if (BlendSettings)
				{
					for (const FPostProcessSettings& Settings : *BlendSettings)
					{
						++CameraBlends;
						ConsiderSettings(Settings);
					}
				}

				++CameraBlends;
				ConsiderSettings(CameraManager->GetCameraCacheView().PostProcessSettings);
			}
		}
	}

	if (Readers > 0)
	{
		UE_LOG(LogAnomalyCapture, Warning,
			TEXT("Capture(census): HOST-PP CUSTOM-DEPTH READERS = %d [%s ] (scanned %d volume(s), %d camera ")
			TEXT("blend(s), %d blendable entr(ies), %d material(s), %d of which read some scene texture). The census tags candidates ")
			TEXT("with CUSTOM DEPTH on captured frames, so a host post-process that READS custom depth or ")
			TEXT("custom stencil can tint or outline census-tagged objects in frames the labels call CLEAN - ")
			TEXT("an unlabelled artifact no counter can see. THIS IS A 'LOOK AT THIS', NOT A DEFECT: the ")
			TEXT("bitmask says the material SAMPLES the texture, never that the sample changes a pixel."),
			Readers, *Names, Volumes, CameraBlends, BlendableEntries, MaterialsSeen, SceneTextureUsers);
	}
	else
	{
		UE_LOG(LogAnomalyCapture, Log,
			TEXT("Capture(census): HOST-PP CUSTOM-DEPTH READERS = 0 (scanned %d volume(s), %d camera blend(s), ")
			TEXT("%d blendable entr(ies), %d material(s), %d of which read some scene texture). ")
			TEXT("THE DISCRIMINATOR IS 'blendable entries' vs 'materials': entries 0 means the scanned ")
			TEXT("post-process settings CARRY NO BLENDABLE AT ALL (they only tune exposure/bloom-style ")
			TEXT("values), which is the ordinary case and makes a 0 here a TRUE zero; entries > 0 with ")
			TEXT("materials 0 would mean the walk found blendables it could not resolve, which would be a ")
			TEXT("DEFECT IN THIS SCAN. READ THE SCANNED COUNTS, NOT JUST THE ")
			TEXT("ZERO: a zero with nothing scanned is BLINDNESS, not a clean read (G96) - a synthetic level ")
			TEXT("with no post-process volume and a camera-less pawn reads 0/0/0 legitimately, and the ")
			TEXT("instrument for that case is IAI.Bench.ProbeSceneTextureUsage, not this line. The three ")
			TEXT("sources scanned are the engine's own three (LocalPlayer.cpp:866-881): post-process VOLUMES, ")
			TEXT("the camera manager's CACHED BLENDS (camera modifiers), and the view target's CAMERA POV ")
			TEXT("OVERRIDE. Source of truth per material is its SERIALIZED UsedSceneTextures bitmask, so this ")
			TEXT("works in a cooked build with no editor-only data. ")
			TEXT("LIMITS: it cannot see a reader OUTSIDE the material system (a host scene-view extension, a ")
			TEXT("custom pass, Niagara, UMG, a decal), and it is a snapshot taken now, so a blendable added ")
			TEXT("later is missed. ZERO DOES NOT MEAN 'NOTHING ON THIS HOST READS CUSTOM DEPTH'."),
			Volumes, CameraBlends, BlendableEntries, MaterialsSeen, SceneTextureUsers);
	}

	return Readers;
}

const TCHAR* UAnomalyCaptureSubsystem::DescribeCensusFloorSource() const
{
	if (bCensusFloorFromConsole)
	{
		return TEXT("IAI.Capture.CensusFloor (console)");
	}
	if (bCensusFloorFromIni)
	{
		return TEXT("DefaultGame.ini [AnomalyCapture] CensusMinDrawnCoveragePctDefault");
	}
	return TEXT("COMPILED DEFAULT (0.5)");
}

const TCHAR* UAnomalyCaptureSubsystem::DescribeCensusCeilingSource() const
{
	if (bCensusCeilingFromConsole)
	{
		return TEXT("IAI.Capture.CensusCeiling (console)");
	}
	if (bCensusCeilingFromIni)
	{
		return TEXT("DefaultGame.ini [AnomalyCapture] CensusMaxDrawnCoveragePctDefault");
	}
	return TEXT("COMPILED DEFAULT (25.0)");
}

void UAnomalyCaptureSubsystem::SetCensusCeilingPct(float InPct)
{
	if (bRunning)
	{
		UE_LOG(LogAnomalyCapture, Warning, TEXT("IAI.Capture.CensusCeiling: ignored - a capture run is in progress."));
		return;
	}
	if (InPct > 100.0f)
	{
		UE_LOG(LogAnomalyCapture, Warning,
			TEXT("IAI.Capture.CensusCeiling: %.2f is above 100 and is REFUSED (out-of-range is refused, never ")
			TEXT("clamped). Current value unchanged (%.2f)."), InPct, CensusCeilingPct);
		return;
	}
	CensusCeilingPct = InPct;
	bCensusCeilingFromIni = false;
	bCensusCeilingFromConsole = true;
	if (InPct > 0.0f)
	{
		UE_LOG(LogAnomalyCapture, Log,
			TEXT("IAI.Capture.CensusCeiling: %.2f%% - the census eligibility CEILING on MEASURED DRAWN coverage. ")
			TEXT("The band is INCLUSIVE: a candidate is eligible iff floor <= coverage <= ceiling. A MEASURED ")
			TEXT("NON-ZERO candidate ABOVE the ceiling is EXCLUDED CATEGORICALLY, because at scenery scale the ")
			TEXT("LABEL is unusable - not because the anomaly fails. Takes effect BETWEEN RUNS."),
			CensusCeilingPct);
	}
	else
	{
		UE_LOG(LogAnomalyCapture, Warning,
			TEXT("IAI.Capture.CensusCeiling: %.2f - the ceiling is DISABLED (<= 0). NO upper bound is applied and ")
			TEXT("scenery-scale targets ARE eligible. This is a real setting, not an error, and StartRun says so ")
			TEXT("out loud so a disabled ceiling can never read like a healthy one."),
			CensusCeilingPct);
	}
}

const TCHAR* UAnomalyCaptureSubsystem::DescribeTickPinSource() const
{
	if (bTickPinFromConsole)
	{
		return TEXT("IAI.Capture.TickPin (console override, beats the ini)");
	}
	if (bTickPinFromIni)
	{
		return TEXT("DefaultGame.ini [AnomalyCapture] bTickModePinDefault");
	}
	return TEXT("COMPILED DEFAULT (on where the fork is detected)");
}

FString UAnomalyCaptureSubsystem::DescribeLabelsInDelivery() const
{
	const TCHAR* Source = bLabelsInDeliveryFromConsole
		? TEXT("console")
		: (bLabelsInDeliveryFromIni ? TEXT("ini") : TEXT("COMPILED DEFAULT"));
	return FString::Printf(TEXT("%s(%s)"), bLabelsInDelivery ? TEXT("on") : TEXT("off"), Source);
}

void UAnomalyCaptureSubsystem::SetLabelsInDelivery(bool bInWrite)
{
#if ANOMALY_CAPTURE
	if (bRunning)
	{
		UE_LOG(LogAnomalyCapture, Warning, TEXT("IAI.Capture.DeliveryLabels: ignored mid-run (stop first)."));
		return;
	}
	bLabelsInDelivery = bInWrite;
	bLabelsInDeliveryFromConsole = true;
	UE_LOG(LogAnomalyCapture, Log,
		TEXT("IAI.Capture.DeliveryLabels: %s. This decides whether labels.jsonl is written WHEN DELIVERY MODE IS ON; ")
		TEXT("with delivery OFF the file is written regardless and this setting changes nothing. It ADDS a file to the ")
		TEXT("delivered set and nothing else - annotation.json's field set does not move and run.json stays suppressed. ")
		TEXT("The overlay inspection tool reads labels.jsonl, so turning this OFF makes that tool inert on a delivered ")
		TEXT("session."),
		bLabelsInDelivery ? TEXT("ON (labels.jsonl IS written in delivery mode)") : TEXT("off (pre-m32 minimal file set)"));
#endif
}

bool UAnomalyCaptureSubsystem::IsTickPinCompiled()
{
#if ANOMALY_CAPTURE
	return AnomalyTickPin::bCompiled;
#else
	return false;
#endif
}

void UAnomalyCaptureSubsystem::SetTickPin(bool bInPin)
{
#if ANOMALY_CAPTURE
	if (bRunning)
	{
		UE_LOG(LogAnomalyCapture, Warning, TEXT("IAI.Capture.TickPin: ignored mid-run (stop first)."));
		return;
	}
	bTickPinEnabled = bInPin;
	bTickPinFromConsole = true;
	if (!AnomalyTickPin::bCompiled)
	{
		UE_LOG(LogAnomalyCapture, Warning,
			TEXT("IAI.Capture.TickPin: %s RECORDED but this build is TICKPIN not-compiled (no decoupled-tick ")
			TEXT("fork was detected at build time), so NOTHING is pinned whatever you set. The command exists on ")
			TEXT("every build on purpose - a silently missing command on the host that matters is worse than a ")
			TEXT("command that says it can do nothing."),
			bTickPinEnabled ? TEXT("ON") : TEXT("off"));
		return;
	}
	UE_LOG(LogAnomalyCapture, Log,
		TEXT("IAI.Capture.TickPin: %s - THE BISECT SWITCH FOR THE PIN, and it takes effect BETWEEN RUNS, not ")
		TEXT("mid-run. It overrides DefaultGame.ini [AnomalyCapture] bTickModePinDefault for this session, which ")
		TEXT("matters because a loose ini beside a package is a no-op (G88) - the cooked config wins, so without ")
		TEXT("this command the unpinned control leg would need a second COOK. Set it 0 and re-capture for the ")
		TEXT("unpinned control, 1 for the pinned leg; the full provenance prints at IAI.Capture.Start."),
		bTickPinEnabled ? TEXT("ON") : TEXT("off"));
#endif
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
			TEXT("IAI.Capture.Start: CAP-RUNDIR-REFUSED outDir %s still contains a quote character after unwrapping Ã¢â‚¬â€ ")
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
			TEXT("IAI.Capture.Start: targeted mode needs BOTH an anomaly and a target actor (got only one) Ã¢â‚¬â€ falling back to auto-pool."));
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
			TEXT("IAI.Capture.Start: CAP-RUNDIR-REFUSED could not create run directory %s Ã¢â‚¬â€ REFUSING TO START ")
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

	StartRunLog();

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
	TicksAtFirstArm = -1;
	TicksAtLastArm = -1;
	GameClockSpeedRatio = 1.0;
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

	if (Async.IsValid() && Async->Capturer.IsValid())
	{
		Async->Capturer->ResetReadbackLayout();
	}

	UE_LOG(LogAnomalyCapture, Log,
		TEXT("=== Capture(mask): EFFECTIVE FOR THIS RUN - mask %s, default from %s, maskReduce=%s(from %s) === ")
		TEXT("READ THIS LINE, NOT THE ")
		TEXT("INI. Mask off means the m26 H5 cure is INACTIVE and this session labels exactly as m25 did. In a ")
		TEXT("packaged build the ini that counts is the COOKED DefaultGame.ini - a loose ini beside the package ")
		TEXT("is a SILENT NO-OP (G88), which is why this line reports the EFFECTIVE value and not the file."),
		bMaskMeasure ? TEXT("ON (measure, report and veto)") : TEXT("off"),
		DescribeMaskSource(),
		LexToStringAnomalyMaskReduceMode(GMaskReduceMode), DescribeMaskReduceSource());

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

	bTargetMaskEffective = bTargetMask && bMaskMeasure && bAsyncCapture && (EffectiveOutputHeight == 0);
	TargetMaskMeasured = 0;
	TargetMaskHiddenBlank = 0;
	TargetMaskUnavailable = 0;
	TargetMaskPendingSessionIndex.Reset();
	TargetMaskPendingTags.Reset();
	TargetMaskFirstFrame.Reset();
	TargetMaskLastFrame.Reset();
	TargetMaskOutcome.Reset();
	TargetMaskHoldTicks = 0;
	if (Async.IsValid())
	{
		Async->TargetMaskHeldFrames.Reset();
	}
	TargetMaskSelfTagged.Reset();
	TargetMaskOwnSerial = 0;
	TargetMaskTagFlips = 0;
	TargetMaskW = 0;
	TargetMaskH = 0;
	TargetMaskArmedSessionIndex = -1;
	{
		const TCHAR* Why = TEXT("");
		if (!bTargetMask)                 { Why = TEXT(" (requested off)"); }
		else if (!bMaskMeasure)           { Why = TEXT(" (THE MASK IS OFF - the target mask reuses the m26 visible-mask pass, so it cannot run without it)"); }
		else if (!bAsyncCapture)          { Why = TEXT(" (async capture is off)"); }
		else if (EffectiveOutputHeight != 0) { Why = TEXT(" (REFUSED: IAI.Capture.OutputHeight is non-zero. The mask is view-rect sized while m28 RESAMPLES the written frame, so the two would disagree in size, and a LABEL MASK MUST NEVER BE FILTERED - bilinear would invent stencil values that were never assigned to anything. Nearest-neighbour mask resampling is a named follow-up, NOT built. Set the output height to 0 to get masks.)"); }
		UE_LOG(LogAnomalyCapture, Log,
			TEXT("=== Capture(m43): TARGET MASK %s FOR THIS RUN%s - requested %s, from %s, output dir '%s/target_mask' === ")
			TEXT("READ THIS LINE, NOT THE INI. One 8-bit grayscale PNG per captured frame, numbered by SESSION ")
			TEXT("INDEX; non-zero pixel values are the stencil tags of the ANOMALY TARGETS visible in that frame ")
			TEXT("and 0 is background. mask_map.json maps value+event to target and anomaly type. A BLANK png ")
			TEXT("means MEASURED AND NOTHING VISIBLE (a hidden blinking target); mask_file:null in labels.jsonl ")
			TEXT("means NOT MEASURED - they are different facts. It reuses the m26 pass and does NOT change the ")
			TEXT("m26 measurement, the veto, or annotation.json. Delivery mode does NOT suppress it."),
			bTargetMaskEffective ? TEXT("ON") : TEXT("OFF"), Why,
			bTargetMask ? TEXT("on") : TEXT("off"), DescribeTargetMaskSource(), *RunDir);
	}

	bCensusEffective = bCensus && bMaskMeasure && bAsyncCapture;
	UE_LOG(LogAnomalyCapture, Log,
		TEXT("=== Capture(census): EFFECTIVE FOR THIS RUN - census %s (requested %s, from %s), floor=%.2f%%(from %s), ")
		TEXT("ceiling=%s(from %s) [band is INCLUSIVE: eligible iff floor <= coverage <= ceiling], ")
		TEXT("maxVerdictAgeTicks=%d(%s, m41: the FLOOR of the freshness window, not a fixed age), ")
		TEXT("excludeTranslucent=%d(%s), includeTranslucentWriters=%d(%s), reservation=%d ")
		TEXT("=== READ THIS LINE, NOT THE INI. ")
		TEXT("The census measures DRAWN PIXELS per selection candidate (m26 mask + m34 reduce, rolling batches ")
		TEXT("upstream of selection); the armed-frame measurement and the ZERO-ONLY veto are unchanged and remain ")
		TEXT("the backstop; annotation.json's field set does not move."),
		bCensusEffective ? TEXT("ON") : TEXT("off"),
		bCensus ? TEXT("on") : TEXT("off"), DescribeCensusSource(),
		CensusFloorPct, DescribeCensusFloorSource(),
		(CensusCeilingPct > 0.0f)
			? *FString::Printf(TEXT("%.2f%%"), CensusCeilingPct)
			: TEXT("DISABLED (<=0; NO upper bound is applied and scenery-scale targets ARE eligible)"),
		DescribeCensusCeilingSource(),
		CensusMaxVerdictAgeTicks,
		bCensusMaxAgeFromConsole ? TEXT("console") : (bCensusMaxAgeFromIni ? TEXT("ini") : TEXT("compiled")),
		bCensusExcludeTranslucent ? 1 : 0,
		bCensusTranslucentFromConsole ? TEXT("console") : (bCensusTranslucentFromIni ? TEXT("ini") : TEXT("compiled")),
		bCensusIncludeTranslucentWriters ? 1 : 0, DescribeCensusTranslucentWritersSource(),
		bCensusReservation ? 1 : 0);
	if (bCensus && !bCensusEffective)
	{
		UE_LOG(LogAnomalyCapture, Warning,
			TEXT("Capture(census): THE MASK IS OFF, so the census is INACTIVE for this run - it requires the ")
			TEXT("mask (currently %d, from %s) and async capture (currently %d). m41 ships BOTH compiled ")
			TEXT("defaults ON, so reaching this line means something TURNED THE MASK OFF: console ")
			TEXT("(IAI.Capture.Mask 0) or an ini key setting bMaskMeasureDefault=False. It is never the ")
			TEXT("shipped default and must never read as routine noise. A census that silently did not run ")
			TEXT("would look like a clean null, which is why this is a WARNING."),
			bMaskMeasure ? 1 : 0, DescribeMaskSource(), bAsyncCapture ? 1 : 0);
	}

	if (bMaskMeasure && Async.IsValid())
	{
		if (Async->MaskExtension.IsValid())
		{
			Async->MaskExtension->Reset();
			Async->MaskExtension->SetReduceMode(GMaskReduceMode);
		}
		Async->TagLedger.Reset();
		if (bCensusReservation)
		{
			Async->TagLedger.HostReserved = AnomalyStencilTag::SnapshotHostReservedValues(World);
		}
		{
			TArray<uint8> ReservedSorted = Async->TagLedger.HostReserved.Array();
			ReservedSorted.Sort();
			FString ReservedList;
			for (uint8 V : ReservedSorted)
			{
				ReservedList += FString::Printf(TEXT(" %d"), (int32)V);
			}
			UE_LOG(LogAnomalyCapture, Log,
				TEXT("Capture(mask): M36 STENCIL RESERVATION %s - reserved=%d [%s ]. Host-set bRenderCustomDepth ")
				TEXT("with a stencil value in 200..254, snapshotted at StartRun, is never assigned by the census ")
				TEXT("OR the event allocator this run: hygiene restores host values AFTER, this prevents host ")
				TEXT("pixels being COUNTED under a plugin tag DURING. No per-cycle rescan in v1."),
				bCensusReservation ? TEXT("ON") : TEXT("OFF (gate lever - P-C12 companion only)"),
				Async->TagLedger.HostReserved.Num(), *ReservedList);
		}
		AnomalyStencilTag::SnapshotCustomDepthEnabled(World, Async->PreRunStencilSnapshot);
		Async->MaskMeasure.BeginRun(&Async->TagLedger);
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

bool UAnomalyCaptureSubsystem::ResolveRunLogEffective(FString& OutSource) const
{
#if ANOMALY_CAPTURE
	if (RunLogOverride >= 0)
	{
		OutSource = FString::Printf(TEXT("forced %s, from IAI.Capture.RunLog"),
			RunLogOverride != 0 ? TEXT("ON") : TEXT("OFF"));
		return RunLogOverride != 0;
	}
	if (bRunLogFromIni && RunLogIni >= 0)
	{
		OutSource = FString::Printf(TEXT("forced %s, from DefaultGame.ini [AnomalyCapture] RunLogDefault"),
			RunLogIni != 0 ? TEXT("ON") : TEXT("OFF"));
		return RunLogIni != 0;
	}
	OutSource = FString::Printf(TEXT("auto, from delivery=%s"), bDeliveryMode ? TEXT("on") : TEXT("off"));
	return !bDeliveryMode;
#else
	OutSource = TEXT("compiled out");
	return false;
#endif
}

void UAnomalyCaptureSubsystem::StartRunLog()
{
#if ANOMALY_CAPTURE
	FString Source;
	const bool bWant = ResolveRunLogEffective(Source);

	if (bWant)
	{
		const FString Path = FPaths::Combine(RunDir, TEXT("anomaly_log.txt"));
		const FString Header = FString::Printf(
			TEXT("# anomaly_log.txt - LogAnomaly + LogAnomalyCapture for THIS RUN ONLY. session=%s dir=%s ")
			TEXT("| timestamps are forced UTC and the bracketed number is GFrameCounter modulo 1000, regardless of ")
			TEXT("this build's log.Timestamp setting, so a line here joins to labels.jsonl frame_index. ")
			TEXT("| written and flushed ONE LINE AT A TIME, so a process killed mid-run still leaves everything ")
			TEXT("logged before the kill."),
			*SessionId, *RunDir);

		RunLog = MakeUnique<FAnomalyRunLog>();
		if (RunLog->Open(Path, Header))
		{
			GLog->AddOutputDevice(RunLog.Get());
			UE_LOG(LogAnomalyCapture, Log,
				TEXT("Capture(runlog): EFFECTIVE FOR THIS RUN - run log ON (%s) -> %s"), *Source, *Path);
			UE_LOG(LogAnomalyCapture, Log,
				TEXT("Capture(runlog): this file carries LogAnomaly and LogAnomalyCapture ONLY, for this run only. ")
				TEXT("It is CLOSED at FinishRun, so any async-writer tail lines emitted after that point are in the ")
				TEXT("MAIN log ONLY and will not appear here."));
		}
		else
		{
			RunLog.Reset();
			UE_LOG(LogAnomalyCapture, Warning,
				TEXT("Capture(runlog): EFFECTIVE FOR THIS RUN - run log was ON (%s) but the file could NOT be opened ")
				TEXT("at %s, so NO anomaly_log.txt will be written. Reported rather than silently degraded: a run ")
				TEXT("with no run log must never read like a run that was not asked for one."),
				*Source, *Path);
		}
	}
	else
	{
		UE_LOG(LogAnomalyCapture, Log,
			TEXT("Capture(runlog): EFFECTIVE FOR THIS RUN - run log OFF (%s) - NO anomaly_log.txt will be written."),
			*Source);
	}

	if (bRunLogVerbose && !bRunLogVerbosityRaised)
	{
		RunLogSavedVerbosity = (uint8)LogAnomaly.GetVerbosity();
		LogAnomaly.SetVerbosity(ELogVerbosity::Verbose);
		bRunLogVerbosityRaised = true;
		UE_LOG(LogAnomalyCapture, Log,
			TEXT("Capture(runlog): VERBOSITY RAISED - LogAnomaly %s(%d) -> Verbose(%d) FOR THIS RUN ONLY, and it is ")
			TEXT("RESTORED at FinishRun. The blinking toggle line is Verbose (Anomaly_Blinking.cpp:95) and is absent ")
			TEXT("from any run that did not ask for it. The device FILTERS; this knob is the only thing that RAISES, ")
			TEXT("and it never leaves the level changed."),
			::ToString((ELogVerbosity::Type)RunLogSavedVerbosity), (int32)RunLogSavedVerbosity,
			(int32)ELogVerbosity::Verbose);
		UE_LOG(LogAnomaly, Verbose, TEXT("Capture(runlog): RUNLOG-VERBOSE-PROBE raised=1"));
	}
#endif
}

void UAnomalyCaptureSubsystem::EndRunLog()
{
#if ANOMALY_CAPTURE
	if (bRunLogVerbosityRaised)
	{
		LogAnomaly.SetVerbosity((ELogVerbosity::Type)RunLogSavedVerbosity);
		bRunLogVerbosityRaised = false;
		UE_LOG(LogAnomaly, Verbose, TEXT("Capture(runlog): RUNLOG-VERBOSE-PROBE raised=0"));
		UE_LOG(LogAnomalyCapture, Log,
			TEXT("Capture(runlog): VERBOSITY RESTORED - LogAnomaly is back to %s(%d). The paired probe is the proof ")
			TEXT("BOTH WAYS: RUNLOG-VERBOSE-PROBE raised=1 is present because the raise took, and raised=0 is ABSENT ")
			TEXT("because the restore took. A restore never shown to suppress anything is not a restore."),
			::ToString((ELogVerbosity::Type)RunLogSavedVerbosity), (int32)RunLogSavedVerbosity);
	}

	if (!RunLog.IsValid())
	{
		return;
	}

	const FString Path = RunLog->GetFilePath();
	const int32 Lines = RunLog->GetLinesWritten();

	GLog->RemoveOutputDevice(RunLog.Get());
	RunLog->Close(TEXT("# closed at FinishRun; any writer tail lines after this point are in the main log only."));
	RunLog.Reset();

	UE_LOG(LogAnomalyCapture, Log,
		TEXT("Capture(runlog): CLOSED %s (%d line(s) plus the close marker). This line itself is in the MAIN log only ")
		TEXT("- the device was already removed when it was emitted, which is the same reason the writer's tail lines ")
		TEXT("are."),
		*Path, Lines);
#endif
}

void UAnomalyCaptureSubsystem::SetRunLog(int32 InState)
{
#if ANOMALY_CAPTURE
	if (bRunning)
	{
		UE_LOG(LogAnomalyCapture, Warning, TEXT("IAI.Capture.RunLog: ignored - a capture run is in progress."));
		return;
	}
	if (InState < -1 || InState > 1)
	{
		UE_LOG(LogAnomalyCapture, Warning,
			TEXT("IAI.Capture.RunLog: %d is outside [-1..1] and is REFUSED (out-of-range is refused, never clamped). ")
			TEXT("Current value unchanged (%d)."),
			InState, RunLogOverride);
		return;
	}
	RunLogOverride = InState;
	FString Source;
	const bool bEff = ResolveRunLogEffective(Source);
	UE_LOG(LogAnomalyCapture, Log,
		TEXT("IAI.Capture.RunLog: EFFECTIVE READ-BACK = %s (%s). -1 auto mirrors run.json (ON when delivery is off), ")
		TEXT("0 off, 1 on. Takes effect at the next IAI.Capture.Start, not mid-run. It adds ONE FILE, ")
		TEXT("anomaly_log.txt; no artifact field moves."),
		bEff ? TEXT("ON") : TEXT("OFF"), *Source);
#endif
}

void UAnomalyCaptureSubsystem::SetRunLogVerbose(bool bInVerbose)
{
#if ANOMALY_CAPTURE
	if (bRunning)
	{
		UE_LOG(LogAnomalyCapture, Warning, TEXT("IAI.Capture.RunLogVerbose: ignored - a capture run is in progress."));
		return;
	}
	bRunLogVerbose = bInVerbose;
	UE_LOG(LogAnomalyCapture, Log,
		TEXT("IAI.Capture.RunLogVerbose: EFFECTIVE READ-BACK = %d. ON raises LogAnomaly to Verbose FOR THE DURATION ")
		TEXT("OF ONE RUN and RESTORES the prior level at FinishRun, echoing both transitions. It is SEPARATE from ")
		TEXT("the run log itself, which only ever FILTERS and never raises. Takes effect at the next ")
		TEXT("IAI.Capture.Start."),
		bRunLogVerbose ? 1 : 0);
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

	AnomalyViewport::ResetTargetExclusionStats();
	PatternExcludedTargets = 0;

	TickPinReasserts = 0;
	CaptureGameTicks = 0;
	bTickPinApplied = false;
	TickPinSaved = -1;
	if (AnomalyTickPin::bCompiled && bTickPinEnabled)
	{
		TickPinSaved = AnomalyTickPin::Read() ? 1 : 0;
		AnomalyTickPin::Write(false);
		bTickPinApplied = true;
		UE_LOG(LogAnomalyCapture, Log,
			TEXT("Capture(tickpin): TICKPIN active saved=%d (decoupled sim/render tick forced OFF for the ")
			TEXT("duration of this capture, re-applied every capture tick, restored at finish; default from %s%s)."),
			TickPinSaved,
			DescribeTickPinSource(),
			AnomalyTickPin::bForkDefineVisible ? TEXT("; fork build define visible to this TU") : TEXT(""));
	}
	else if (AnomalyTickPin::bCompiled)
	{
		UE_LOG(LogAnomalyCapture, Log,
			TEXT("Capture(tickpin): TICKPIN disabled-by-ini (the fork WAS detected at build time; the pin is off ")
			TEXT("for this session, default from %s). This is the UNPINNED CONTROL leg."),
			DescribeTickPinSource());
	}
	else
	{
		UE_LOG(LogAnomalyCapture, Log,
			TEXT("Capture(tickpin): TICKPIN not-compiled (no decoupled-tick fork detected) Ã¢â‚¬â€ this build never ")
			TEXT("touches the engine tick mode and behaves exactly as it did before the pin existed."));
	}

	UE_LOG(LogAnomalyCapture, Log,
		TEXT("=== Capture(G-M9): EFFECTIVE FOR THIS RUN - dualPathReadback=%s. off is the shipped path ")
		TEXT("and is byte-for-byte what it is without the instrument. on enqueues a SECOND readback in ")
		TEXT("the PRE-m35 form alongside the m35 owned-copy readback ON THE SAME FRAME and byte-compares ")
		TEXT("the two drained pictures; the comparison is sound ONLY because both passes are added ")
		TEXT("consecutively and are read-only with respect to scene colour, so they observe identical ")
		TEXT("contents. A non-zero diff has TWO causes and the output line names both - check ADJACENCY ")
		TEXT("first. Mode 2 corrupts the legacy picture on purpose so the comparator can be proven to ")
		TEXT("fire, and never touches what is written to disk. ==="),
		*AnomalyReadback::DescribeDualPathReadback());

	AnomalyViewport::SetOverlaysSuppressed(true);

	Phase = ECapturePhase::LeadIn;
	PhaseFramesLeft = PreFrames;
	bRunBegun = true;

	SpawnMaskPairingProbe();

	if (bCensusEffective && Async.IsValid() && Async->MaskExtension.IsValid())
	{
		FAnomalyCensusParams CensusParams;
		CensusParams.FloorPct = CensusFloorPct;
		CensusParams.CeilingPct = CensusCeilingPct;
		CensusParams.MaxVerdictAgeTicks = CensusMaxVerdictAgeTicks;
		CensusParams.bExcludeTranslucent = bCensusExcludeTranslucent;
		CensusParams.bIncludeTranslucentCustomDepthWriters = bCensusIncludeTranslucentWriters;
		CensusParams.bLeakProbe = bCensusLeakProbe && !bDeliveryMode;
		CensusParams.bCoArmOnly = bCensusCoArm && !bDeliveryMode;
		CensusParams.bBenchFixedExpiry = bBenchCensusFixedExpiry;
		CensusParams.BenchBatchCap = BenchCensusBatchCap;
		CensusParams.BenchDropEveryNth = BenchCensusDropEveryNth;
		Async->Census.Begin(GetWorld(), &Async->TagLedger, CensusParams);

		Async->Census.NoteHostPpReaders(ScanHostPostProcessCustomDepthReaders(GetWorld()));

		if (UAnomalyAutoInjectorSubsystem* Auto = ResolveAuto())
		{
			TWeakObjectPtr<UAnomalyCaptureSubsystem> WeakSelf(this);
			Auto->SetCensusProvider(
				[WeakSelf](const AActor* Actor) -> FAnomalyCensusOpinion
				{
					if (UAnomalyCaptureSubsystem* Self = WeakSelf.Get())
					{
						if (Self->Async.IsValid())
						{
							return Self->Async->Census.QueryActor(Actor);
						}
					}
					return FAnomalyCensusOpinion();
				},
				[WeakSelf]() -> bool
				{
					if (UAnomalyCaptureSubsystem* Self = WeakSelf.Get())
					{
						if (Self->Async.IsValid())
						{
							return Self->Async->Census.HasCompletedACycle();
						}
					}
					return true;
				},
				[WeakSelf](int32 Consulted, int32 Fallback, int32 Unseen)
				{
					if (UAnomalyCaptureSubsystem* Self = WeakSelf.Get())
					{
						if (Self->Async.IsValid())
						{
							Self->Async->Census.NoteFire(Consulted, Fallback, Unseen);
						}
					}
				},
				CensusMaxVerdictAgeTicks);
		}
	}

	ApplySessionGlobals();

	UE_LOG(LogAnomalyCapture, Log,
		TEXT("=== Capture run STARTED: %s | mode=%s | delivery=%s | clock=%s | seed=%d fmt=%s capture=%s fps=%d(fixed-step%s) | K=%d L=%d pre=%d positive=%d post=%d bursts=%s frameCap=%s | blinkHalf=%s lodHalf=%s lodMaxDist=%s lodMinCov=%s lodHighestOnly=%s clipRadius=%s | excludePatterns=%s | labelsInDelivery=%s ==="),
		*RunDir,
		bTargetedMode ? *FString::Printf(TEXT("targeted[%s on %s]"), *TargetAnomalyId.ToString(), *TargetActorName) : TEXT("auto-pool"),
		bDeliveryMode ? TEXT("on") : TEXT("off"),
		ContentClock == EContentClock::Game ? TEXT("game") : TEXT("wall"),
		Seed, *M.Format, DescribeGrabPoint(), VideoFps,
		bPaceCapture ? TEXT(", paced") : TEXT(", unpaced"),
		SettleFrames, ViewLagFrames, PreFrames, PositiveFrames, PostFrames,
		BurstCount > 0 ? *FString::FromInt(BurstCount) : TEXT("until-stop"),
		FrameCap > 0 ? *FString::FromInt(FrameCap) : TEXT("none"),
		*AnomalyDefaults::DescribeBlinkingHalfPeriod(),
		*AnomalyDefaults::DescribeLodPoppingHalfPeriod(),
		*AnomalyDefaults::DescribeLodPoppingMaxDistance(),
		*AnomalyDefaults::DescribeLodPoppingMinCoverage(),
		*AnomalyDefaults::DescribeLodPoppingRequireHighestLod(),
		*AnomalyDefaults::DescribeCameraClippingTriggerRadius(),
		*AnomalyDefaults::DescribeExcludedTargetPatterns(),
		*DescribeLabelsInDelivery());

	UE_LOG(LogAnomalyCapture, Log,
		TEXT("Capture(bench): m40 SYNTH TICK ORDER = %s (compiled default off). When ON, the injector's anomaly ")
		TEXT("dispatch is relocated to OnWorldPreActorTick to synthesise the SYMPTOM of a reversed subsystem tick ")
		TEXT("order. It is a bench device: console-only, no ini key, never in a client payload. A session captured ")
		TEXT("with it ON has labels that deliberately disagree with its pixels."),
		UAnomalyInjectorSubsystem::IsSynthTickOrderEnabled(GetWorld()) ? TEXT("ON") : TEXT("off"));
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
		Async->MaskExtension->SetReduceMode(GMaskReduceMode);
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

	if (bTargetMaskEffective && Async->MaskExtension.IsValid())
	{
		Async->MaskExtension->EnqueueDrain();
		ServiceTargetMask();
	}

	TArray<FAnomalyCapturedFrame> Batch = MoveTemp(Async->TargetMaskHeldFrames);
	Async->TargetMaskHeldFrames.Reset();
	{
		FAnomalyCapturedFrame Popped;
		while (bUseSve ? Async->SveCapturer->PopCompleted(Popped) : Async->Capturer->PopCompleted(Popped))
		{
			Batch.Add(MoveTemp(Popped));
		}
	}

	for (int32 BatchIndex = 0; BatchIndex < Batch.Num(); ++BatchIndex)
	{
		FAnomalyCapturedFrame& Frame = Batch[BatchIndex];
		if (!bRectDeltaLogged)
		{
			bRectDeltaLogged = true;
			SWindow* DeltaWindow = nullptr;
			FIntRect SlateRect;
			const bool bHaveSlateRect = ComputeGameViewportCapture(GetWorld(), DeltaWindow, SlateRect);
			UE_LOG(LogAnomalyCapture, Log,
				TEXT("Capture: RESOLUTION DELTA (3-rect) Ã¢â‚¬â€ grab=%s | grabbed %dx%d | slate-window %s | viewport-size %dx%d ")
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
		AnomalyLabel::FCaptureSnapshot* Snap = Async->PendingSnapshots.Find(Frame.RequestId);
		if (!Snap)
		{
			UE_LOG(LogAnomalyCapture, Warning,
				TEXT("Capture(async): CAP-PAIR-DROP completed frame id=%llu has no pending snapshot Ã¢â‚¬â€ frame DROPPED, ")
				TEXT("never labelled by guess (pendingSnapshots=%d). This drop used to log at Verbose, which made a ")
				TEXT("broken pairing indistinguishable from a path that never submitted."),
				Frame.RequestId, Async->PendingSnapshots.Num());
			continue;
		}

		if (Snap->bTargetMask)
		{
			const uint8* Outcome = TargetMaskOutcome.Find(Snap->SessionIndex);
			if (!Outcome && TargetMaskHoldTicks < GTargetMaskMaxHoldTicks)
			{
				++TargetMaskHoldTicks;
				for (int32 k = BatchIndex; k < Batch.Num(); ++k)
				{
					Async->TargetMaskHeldFrames.Add(MoveTemp(Batch[k]));
				}
				break;
			}
			TargetMaskHoldTicks = 0;
			if (Outcome)
			{
				Snap->MaskState = (AnomalyLabel::EAnomalyMaskState)*Outcome;
			}
			else
			{
				Snap->MaskState = AnomalyLabel::EAnomalyMaskState::Unmeasured;
				++TargetMaskUnavailable;
				UE_LOG(LogAnomalyCapture, Warning,
					TEXT("Capture(m44): TARGET MASK NEVER RESOLVED for session_index %d after %d held ticks - ")
					TEXT("the row reads mask_state:\"unmeasured\", which is the honest reading. It is NOT ")
					TEXT("\"empty\": no measurement exists for this frame."),
					Snap->SessionIndex, GTargetMaskMaxHoldTicks);
			}
			TargetMaskOutcome.Remove(Snap->SessionIndex);
		}

		int32 OutW = Frame.Width;
		int32 OutH = Frame.Height;
		bool bNeedsResample = false;
		AnomalyLabel::DeriveOutputSize(Frame.Width, Frame.Height, EffectiveOutputHeight, OutW, OutH, bNeedsResample);
		LogFirstFrameMeasuredLine(Frame.Width, Frame.Height, OutW, OutH, bNeedsResample);

		const FString ImageName = FString::Printf(TEXT("Actual_Frames/frame_%05d.%s"), Snap->SessionIndex, Ext);
		int32 NumLabels = 0;
		const FString Record = AnomalyLabel::BuildLabelRecordForSnapshot(*Snap, OutW, OutH, ImageName, NumLabels);

		AccumulateFrameEvents(Snap->Fires, Snap->FireActive, Snap->FirePos, Snap->View, Snap->NearClip,
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
		Job.bWriteLabels = !bDeliveryMode || bLabelsInDelivery;
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
	if (bTargetGlobalHeld)
	{
		Phase = ECapturePhase::SettleAfterFire;
		PhaseFramesLeft = SettleFrames;
		return;
	}

	UAnomalyAutoInjectorSubsystem* Auto = ResolveAuto();
	const bool bFired = Auto
		? (bTargetedMode ? Auto->TryFireSpecific(TargetAnomalyId, TargetActorName, TargetAnomalyArgs) : Auto->TryFireOnce())
		: false;
	if (!bFired)
	{
		++ZeroMatchBursts;
		UE_LOG(LogAnomalyCapture, Log, TEXT("Capture: burst %d fired nothing (zero-match / empty) Ã¢â‚¬â€ negatives only."),
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
			Snap.bTargetMask = bTargetMask;
			if (bTargetMaskEffective)
			{
				Snap.MaskFileRel = FString::Printf(TEXT("target_mask/frame_%05d.png"), Snap.SessionIndex);
				TargetMaskArmedTick = GFrameCounter;
				TargetMaskArmedSessionIndex = Snap.SessionIndex;
				StepMaskPairingProbe(Snap.SessionIndex);
			}
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
			TEXT("Capture(async): could not resolve the game-viewport rect this tick Ã¢â‚¬â€ falling back to sync grab."));
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
		false, !bDeliveryMode || bLabelsInDelivery))
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
			if (AppendSessionGlobalFires(Fires))
			{
				++SessionGlobalPositiveFrames;
			}
			else
			{
				++SessionGlobalNegativeFrames;
			}
		}
		const UAnomalyInjectorSubsystem* SyncInjector = World ? World->GetSubsystem<UAnomalyInjectorSubsystem>() : nullptr;
		TArray<uint8> ActiveNow;
		TArray<FVector> Pos;
		ActiveNow.Reserve(Fires.Num());
		Pos.Reserve(Fires.Num());
		for (const FAutoLiveFireInfo& F : Fires)
		{
			const AActor* FActor = F.TargetActor.Get();
			bool bKnownId = false;
			const EAnomalyActiveSource Source = ResolveAnomalyActiveSource(F.Id, bKnownId);
			if (Source == EAnomalyActiveSource::AnomalyState)
			{
				ActiveNow.Add(!FActor
					? 1
					: ((SyncInjector && SyncInjector->IsAnomalyCurrentlyAnomalous(F.Id)) ? 1 : 0));
			}
			else
			{
				ActiveNow.Add((FActor && FActor->IsHidden()) ? 1 : 0);
			}
			Pos.Add(FActor ? FActor->GetActorLocation() : FVector::ZeroVector);
		}
		const double NowT = World ? World->GetTimeSeconds() : 0.0;
		AccumulateFrameEvents(Fires, ActiveNow, Pos, ProjView, GNearClippingPlane, SessionFrameIndex, NowT);
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
		if (AppendSessionGlobalFires(Snap->Fires))
		{
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

	DeferredActiveRequestId = ArmedLabelRequestId;
	bHasDeferredActive = true;
}

void UAnomalyCaptureSubsystem::ApplySessionGlobals()
{
	ActiveSessionGlobals.Reset();
	SessionGlobalPositiveFrames = 0;
	SessionGlobalNegativeFrames = 0;
	SessionGlobalBaselineNearClip = GNearClippingPlane;
	bTargetGlobalHeld = false;

	UWorld* World = GetWorld();
	UAnomalyInjectorSubsystem* Injector = World ? World->GetSubsystem<UAnomalyInjectorSubsystem>() : nullptr;
	if (!Injector)
	{
		return;
	}

	if (bTargetedMode)
	{
		bool bGlobalScope = false;
		for (const FAnomalyCatalogEntry& Entry : Injector->GetAnomalyCatalog())
		{
			if (Entry.Id == TargetAnomalyId)
			{
				bGlobalScope = (Entry.Scope == EAnomalyScope::Global);
				break;
			}
		}
		if (!bGlobalScope)
		{
			return;
		}

		TArray<FString> Args;
		Args.Reserve(1 + TargetAnomalyArgs.Num());
		Args.Add(FString(TEXT("=")) + TargetActorName);
		Args.Append(TargetAnomalyArgs);

		if (Injector->ApplyAnomaly(TargetAnomalyId, Args))
		{
			ActiveSessionGlobals.Add(TargetAnomalyId);
			bTargetGlobalHeld = true;
			UE_LOG(LogAnomalyCapture, Log,
				TEXT("=== Capture(session-global): TARGETED '%s' on '%s' IS HELD FOR THE WHOLE SESSION - applied once here, ")
				TEXT("reverted at finish, and NEVER fired per burst, so the effect no longer switches off and on with the ")
				TEXT("burst cycle. While it is held the anomaly's own proximity trigger drives it: the near clip is pushed ")
				TEXT("whenever the player is within range of the target and restored when the player leaves. Baseline ")
				TEXT("near-clip was %.3f, it is now %.3f. A frame is labelled positive ONLY when the near clip is pushed ")
				TEXT("AND geometry is actually within the near-clip radius - standing inside the trigger radius is NOT by ")
				TEXT("itself a positive frame. ==="),
				*TargetAnomalyId.ToString(), *TargetActorName, SessionGlobalBaselineNearClip, GNearClippingPlane);
		}
		else
		{
			UE_LOG(LogAnomalyCapture, Warning,
				TEXT("Capture(session-global): TARGETED '%s' on '%s' returned false from Apply - it is NOT active for this ")
				TEXT("session, and because global ids are held rather than fired per burst, no burst will fire it either."),
				*TargetAnomalyId.ToString(), *TargetActorName);
		}
		return;
	}

	UAnomalyAutoInjectorSubsystem* Auto = ResolveAuto();
	if (!Auto)
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
	bTargetGlobalHeld = false;

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
		TEXT("Frames labelled positive: %d, negative: %d - the split is each id's OWN per-frame anomalous test, not the ")
		TEXT("session flag; for camera_clipping that means the near clip was pushed AND geometry was actually inside the ")
		TEXT("near-clip radius on that frame. ==="),
		ActiveSessionGlobals.Num(), GNearClippingPlane, SessionGlobalBaselineNearClip,
		SessionGlobalPositiveFrames, SessionGlobalNegativeFrames);

	ActiveSessionGlobals.Reset();
}

bool UAnomalyCaptureSubsystem::AppendSessionGlobalFires(TArray<FAutoLiveFireInfo>& InOutFires) const
{
	UWorld* World = GetWorld();
	const UAnomalyInjectorSubsystem* Injector = World ? World->GetSubsystem<UAnomalyInjectorSubsystem>() : nullptr;

	bool bAny = false;
	for (const FName& Id : ActiveSessionGlobals)
	{
		if (!Injector || !Injector->IsAnomalyCurrentlyAnomalous(Id))
		{
			continue;
		}
		FAutoLiveFireInfo F;
		F.Id = Id;
		F.Target = FString();
		F.TargetActor = nullptr;
		F.SecondsRemaining = 0.0f;
		F.StartFrame = (uint64)StartFrame;
		F.bWholeFrameExtent = true;
		InOutFires.Add(F);
		bAny = true;
	}
	return bAny;
}

void UAnomalyCaptureSubsystem::SampleDeferredActiveState()
{
	if (!bHasDeferredActive)
	{
		return;
	}
	bHasDeferredActive = false;

	if (!Async.IsValid())
	{
		return;
	}
	AnomalyLabel::FCaptureSnapshot* Snap = Async->PendingSnapshots.Find(DeferredActiveRequestId);
	if (!Snap)
	{
		return;
	}

	Snap->FireActive.Reset();
	Snap->FireActive.Reserve(Snap->Fires.Num());
	for (const FAutoLiveFireInfo& F : Snap->Fires)
	{
		Snap->FireActive.Add(ComputeFireActive(F));
	}

	if (Snap->bTargetMask)
	{
		Snap->MaskValues.Reset();
		Snap->MaskValues.Reserve(Snap->Fires.Num());
		for (const FAutoLiveFireInfo& F : Snap->Fires)
		{
			int32 Value = 0;
			for (const FAnomalyMaskRecord& R : Async->MaskMeasure.GetRecords())
			{
				if (R.Id == F.Id && R.Target == F.Target && R.StartFrame == F.StartFrame)
				{
					Value = (int32)R.Tag;
					break;
				}
			}
			Snap->MaskValues.Add(Value);
		}
	}
}

bool UAnomalyCaptureSubsystem::IsFireLabelledThisFrame(const FAutoLiveFireInfo& F) const
{
	bool bKnownId = false;
	const EAnomalyActiveSource Source = ResolveAnomalyActiveSource(F.Id, bKnownId);
	const AActor* FActor = F.TargetActor.Get();

	switch (Source)
	{
	case EAnomalyActiveSource::ActorHidden:
		return FActor && FActor->IsHidden();
	case EAnomalyActiveSource::AnomalyState:
	{
		UWorld* World = GetWorld();
		const UAnomalyInjectorSubsystem* Injector =
			World ? World->GetSubsystem<UAnomalyInjectorSubsystem>() : nullptr;
		return Injector && Injector->IsAnomalyCurrentlyAnomalous(F.Id);
	}
	default:
		return true;
	}
}

uint8 UAnomalyCaptureSubsystem::ComputeFireActive(const FAutoLiveFireInfo& F) const
{
	UWorld* World = GetWorld();
	const UAnomalyInjectorSubsystem* Injector = World ? World->GetSubsystem<UAnomalyInjectorSubsystem>() : nullptr;

	const AActor* FActor = F.TargetActor.Get();
	bool bKnownId = false;
	const EAnomalyActiveSource Source = ResolveAnomalyActiveSource(F.Id, bKnownId);

	if (Source == EAnomalyActiveSource::AnomalyState)
	{
		return !FActor
			? 1
			: ((Injector && Injector->IsAnomalyCurrentlyAnomalous(F.Id)) ? 1 : 0);
	}
	return (FActor && FActor->IsHidden()) ? 1 : 0;
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
	if (TicksAtFirstArm < 0)
	{
		TicksAtFirstArm = CaptureGameTicks;
	}
	TicksAtLastArm = CaptureGameTicks;
}

double UAnomalyCaptureSubsystem::ComputeNominalGameSpan() const
{
	if (VideoFps <= 0 || TicksAtFirstArm < 0 || TicksAtLastArm <= TicksAtFirstArm)
	{
		return 0.0;
	}
	return (double)(TicksAtLastArm - TicksAtFirstArm) * (1.0 / (double)VideoFps);
}

void UAnomalyCaptureSubsystem::CheckEarlyPacingWarning()
{
	if (bEarlyRatioWarned || SessionFrameIndex < GEarlyPacingWarnMinFrames)
	{
		return;
	}
	const double GameSpan = ComputeNominalGameSpan();
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
				TEXT("Capture: live capture running slow (~%.1f of %d fps, ratio %.2f) Ã¢â‚¬â€ the video will be stamped at target %d and plays natural; this is a capture-time perf issue only. Lower IAI.Capture.Fps or run packaged to speed the live capture."),
				(double)VideoFps / Ratio, VideoFps, Ratio, VideoFps);
		}
		else
		{
			UE_LOG(LogAnomalyCapture, Warning,
				TEXT("Capture: sustaining ~%.1f of %d fps (ratio %.2f) Ã¢â‚¬â€ the video will be stamped at the true rate; lower IAI.Capture.Fps or run a packaged build."),
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
	GameClockSpeedRatio = 1.0;

	const double GameSpan = ComputeNominalGameSpan();
	const double WallSpan = LastArmWallSeconds - FirstArmWallSeconds;
	const double GameClockSpan = LastFrameTimeSeconds - FirstFrameTimeSeconds;
	if (SessionFrameIndex < 2 || FirstFrameTimeSeconds < 0.0 || FirstArmWallSeconds < 0.0
		|| GameSpan <= KINDA_SMALL_NUMBER || WallSpan <= KINDA_SMALL_NUMBER)
	{
		return;
	}

	LastRunPacing.bValid = true;
	LastRunPacing.SpeedRatio = WallSpan / GameSpan;
	LastRunPacing.SustainedWallFps = (double)VideoFps / LastRunPacing.SpeedRatio;
	if (GameClockSpan > KINDA_SMALL_NUMBER)
	{
		GameClockSpeedRatio = WallSpan / GameClockSpan;
	}

	if (ContentClock == EContentClock::Game)
	{
		LastRunPacing.StampedFps = (double)VideoFps;
		if (LastRunPacing.SpeedRatio > 1.0 + GFpsStampTolerance)
		{
			UE_LOG(LogAnomalyCapture, Warning,
				TEXT("Capture: live capture ran slow (sustained %.3f of target %d fps, ratio %.3f) Ã¢â‚¬â€ video.fps stamped at target %d and plays natural; the slowness is a capture-time performance issue, not a video defect."),
				LastRunPacing.SustainedWallFps, VideoFps, LastRunPacing.SpeedRatio, VideoFps);
		}
	}
	else
	{
		if (LastRunPacing.SpeedRatio > 1.0 + GFpsStampTolerance)
		{
			LastRunPacing.StampedFps = FMath::RoundToDouble(LastRunPacing.SustainedWallFps * 1000.0) / 1000.0;
			UE_LOG(LogAnomalyCapture, Warning,
				TEXT("Capture: could not hold %d fps wall-clock (sustained %.3f fps, ratio %.3f) Ã¢â‚¬â€ video.fps stamped at the true rate %.3f."),
				VideoFps, LastRunPacing.SustainedWallFps, LastRunPacing.SpeedRatio, LastRunPacing.StampedFps);
		}
		else if (LastRunPacing.SpeedRatio < 1.0 - GFpsStampTolerance)
		{
			UE_LOG(LogAnomalyCapture, Log,
				TEXT("Capture: ran faster than %d fps wall-clock (ratio %.3f, pace=%s) Ã¢â‚¬â€ video.fps stays %d."),
				VideoFps, LastRunPacing.SpeedRatio, bPaceCapture ? TEXT("on") : TEXT("off"), VideoFps);
		}
	}

	UE_LOG(LogAnomalyCapture, Log,
		TEXT("Capture: pacing=%s | clock=%s | target=%d fps | sustained=%.3f fps | ratio=%.3f | stamped=%.3f | legacyRatio=%.7f | armTicks=%d."),
		bPaceCapture ? TEXT("on") : TEXT("off"),
		ContentClock == EContentClock::Game ? TEXT("game") : TEXT("wall"), VideoFps,
		LastRunPacing.SustainedWallFps, LastRunPacing.SpeedRatio, LastRunPacing.StampedFps,
		GameClockSpeedRatio, TicksAtLastArm - TicksAtFirstArm);
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
	if (ResolveAnomalyActiveSource(Ev.Id, bKnownId) == EAnomalyActiveSource::FireWindow)
	{
		return true;
	}
	for (const TPair<int32, uint8>& Pair : Ev.ActiveByIndex)
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
	SampleDeferredActiveState();

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
			Async->MaskExtension->EnqueueDrain(true);
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

		PatternExcludedTargets = AnomalyViewport::GetTargetExclusionCount();
		if (PatternExcludedTargets > 0)
		{
			UE_LOG(LogAnomalyCapture, Log,
				TEXT("Capture: %d distinct actor(s) were REFUSED as injection candidates by the target-exclusion ")
				TEXT("patterns (%s) during this run Ã¢â‚¬â€ see the EXCLUDED-TARGET lines above for which, and which ")
				TEXT("pattern matched each. This counts DISTINCT ACTORS refused at the selection chokepoint, not ")
				TEXT("anomalies that would otherwise have fired."),
				PatternExcludedTargets, *AnomalyDefaults::DescribeExcludedTargetPatterns());
		}

		AnomalyLabel::FTickPinTelemetry TickPinReport;
		TickPinReport.bCompiled = AnomalyTickPin::bCompiled;
		TickPinReport.bApplied = bTickPinApplied;
		TickPinReport.Saved = TickPinSaved;
		TickPinReport.Reasserts = TickPinReasserts;
		TickPinReport.GameTicks = CaptureGameTicks;

		FAnomalyReadbackLayout DrainLayout;
		if (Async.IsValid())
		{
			if (bSveCapture && Async->SveCapturer.IsValid())
			{
				DrainLayout = Async->SveCapturer->GetReadbackLayout();
			}
			else if (Async->Capturer.IsValid())
			{
				DrainLayout = Async->Capturer->GetReadbackLayout();
			}
		}

		AnomalyLabel::FReadbackLayoutTelemetry LayoutReport;
		if (DrainLayout.bValid)
		{
			LayoutReport.SourceExtentX = DrainLayout.SourceExtent.X;
			LayoutReport.SourceExtentY = DrainLayout.SourceExtent.Y;
			LayoutReport.RectMinX = DrainLayout.Rect.Min.X;
			LayoutReport.RectMinY = DrainLayout.Rect.Min.Y;
			LayoutReport.RectMaxX = DrainLayout.Rect.Max.X;
			LayoutReport.RectMaxY = DrainLayout.Rect.Max.Y;
			LayoutReport.W = DrainLayout.W;
			LayoutReport.H = DrainLayout.H;
			LayoutReport.BufferHeight = DrainLayout.BufferHeight;
			LayoutReport.RowPitchInPixels = DrainLayout.RowPitchInPixels;
			LayoutReport.Format = DrainLayout.Format;
		}

		DestroyMaskPairingProbe();

		AnomalyLabel::FTargetMaskTelemetry TargetMaskReport;
		if (bTargetMaskEffective)
		{
			if (Async.IsValid() && Async->MaskExtension.IsValid())
			{
				ReleaseTargetMaskSelfTags();
				for (int32 Attempt = 0; Attempt < 8 && TargetMaskPendingSessionIndex.Num() > 0; ++Attempt)
				{
					Async->MaskExtension->EnqueueDrain(true);
					FlushRenderingCommands();
					ServiceTargetMask();
				}
			}
			ProcessCompletedFrames();
			if (TargetMaskPendingSessionIndex.Num() > 0)
			{
				UE_LOG(LogAnomalyCapture, Warning,
					TEXT("Capture(m43): %d target-mask readback(s) were STILL PENDING after the bounded final ")
					TEXT("drain (8 attempts, each EnqueueDrain(final) + FlushRenderingCommands). Those frames ")
					TEXT("carry mask_file:null - NOT MEASURED, which is a different fact from a blank mask. ")
					TEXT("The number is reported rather than forced: holding a handle into teardown to chase it ")
					TEXT("is the leak shape m38 gate (iii) exists to catch."),
					TargetMaskPendingSessionIndex.Num());
			}
			for (const TPair<uint64, int32>& Pair : TargetMaskPendingSessionIndex)
			{
				UE_LOG(LogAnomalyCapture, Warning,
					TEXT("Capture(m43): UNAVAILABLE session_index %d (id=%llu) - its mask readback never ")
					TEXT("arrived. Its labels row says mask_file:null - NOT MEASURED, which is a different ")
					TEXT("fact from a blank mask."),
					Pair.Value, Pair.Key);
			}
			TargetMaskUnavailable += TargetMaskPendingSessionIndex.Num();
			TargetMaskPendingSessionIndex.Reset();
			TargetMaskPendingTags.Reset();

			const int32 Residual = FramesWritten - (TargetMaskMeasured + TargetMaskHiddenBlank + TargetMaskUnavailable);
			if (Residual > 0)
			{
				TargetMaskUnavailable += Residual;
				UE_LOG(LogAnomalyCapture, Warning,
					TEXT("Capture(m43): %d captured frame(s) never reached a target-mask decision at all - a sync ")
					TEXT("fallback grab, or a captured frame whose tick did not run the mask block. Counted as ")
					TEXT("UNAVAILABLE, never as blank: a blank asserts MEASURED AND NOTHING VISIBLE, and asserting ")
					TEXT("that about a frame we did not measure is worse than writing no file at all."),
					Residual);
			}

			TArray<AnomalyLabel::FTargetMaskMapEntry> MapEntries;
			if (Async.IsValid())
			{
				for (const FAnomalyMaskRecord& R : Async->MaskMeasure.GetRecords())
				{
					AnomalyLabel::FTargetMaskMapEntry E;
					E.MaskValue = (int32)R.Tag;
					E.EventId = FString::Printf(TEXT("%s@%llu"), *R.Id.ToString(), R.StartFrame);
					E.TargetName = R.Target;
					E.AnomalyType = R.Id.ToString();
					E.FirstFrame = TargetMaskFirstFrame.Contains(R.Tag) ? TargetMaskFirstFrame[R.Tag] : -1;
					E.LastFrame = TargetMaskLastFrame.Contains(R.Tag) ? TargetMaskLastFrame[R.Tag] : -1;
					MapEntries.Add(E);
				}
			}
			AnomalyLabel::WriteTargetMaskMap(RunDir, MapEntries);

			TargetMaskReport.Measured = TargetMaskMeasured;
			TargetMaskReport.HiddenBlank = TargetMaskHiddenBlank;
			TargetMaskReport.Unavailable = TargetMaskUnavailable;

			UE_LOG(LogAnomalyCapture, Log,
				TEXT("Capture(m43): TARGET MASK SUMMARY measured=%d hiddenBlank=%d unavailable=%d tagFlips=%d ")
				TEXT("(measured + hiddenBlank should equal the captured frame count; unavailable is frames whose ")
				TEXT("readback never arrived and whose labels row says mask_file:null). tagFlips counts the ")
				TEXT("target mask's OWN stencil tag applications and restores on live targets - one flip queues a ")
				TEXT("deferred render-proxy recreate. NAMED LIMITATION of m43: that churn is per fire-active ")
				TEXT("frame, and m42 (persist tags, rotate values in place) is its fix. Its effect on PIXELS is ")
				TEXT("UNMEASURED - the count is reported so the cost is visible, not so it can be claimed harmless."),
				TargetMaskMeasured, TargetMaskHiddenBlank, TargetMaskUnavailable, TargetMaskTagFlips);
		}
		else if (bTargetMask)
		{
			TargetMaskReport.Unavailable = FramesWritten;
		}

		AnomalyLabel::WriteRunSummary(RunDir, FramesWritten, PositiveFramesWritten, BurstsDone, ZeroMatchBursts, GFrameCounter,
			VideoFps, LastRunPacing.SustainedWallFps, LastRunPacing.SpeedRatio, LastRunPacing.StampedFps, GameClockSpeedRatio, bPaceCapture, bDeliveryMode,
			ContentClock == EContentClock::Game ? TEXT("game") : TEXT("wall"), NonManifestedEvents,
			bSveCapture ? TEXT("sve") : TEXT("backbuffer"),
			bSveCapture ? &RingTelemetry : nullptr,
			MaskProbeArms, MaskResidualDiscards, MaskNoPassDiscards, VetoedEvents,
			TranslucentVetoes, TranslucencyUnknownVetoes, &TickPinReport, PatternExcludedTargets,
			DrainLayout.bValid ? &LayoutReport : nullptr,
			(bCensusEffective && Async.IsValid()) ? &Async->Census.GetCounters() : nullptr,
			bTargetMask ? &TargetMaskReport : nullptr);

		if (bSveCapture)
		{
			const AnomalySveKeyRing::FCounters Ring = AnomalySveKeyRing::GetCounters();
			UE_LOG(LogAnomalyCapture, Log,
				TEXT("Capture(sve): key ring Ã¢â‚¬â€ published=%d consumed=%d missed=%d wrapped=%d corrupted=%d (forceMiss=%d)."),
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
					TEXT("and framesWritten agree; a gap between ADJACENT numbers names the stage that missed Ã¢â‚¬â€ ")
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
		EndRunLog();
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
			Async->MaskExtension->EnqueueDrain(true);
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
		if (bCensusEffective)
		{
			if (UAnomalyAutoInjectorSubsystem* Auto = ResolveAuto())
			{
				Auto->ClearCensusProvider();
			}
			Async->Census.End(GetWorld());
		}
		Async->MaskMeasure.EndRun();
		RunStencilHygieneCheck(true);
	}
	bCensusEffective = false;

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

	if (bTickPinApplied)
	{
		AnomalyTickPin::Write(TickPinSaved != 0);
		bTickPinApplied = false;
		UE_LOG(LogAnomalyCapture, Log,
			TEXT("Capture(tickpin): TICKPIN restored=%d reasserts=%d gameTicks=%d. If the process dies mid-capture ")
			TEXT("this restore never runs and the host re-asserts its own mode on the next round entry."),
			TickPinSaved, TickPinReasserts, CaptureGameTicks);
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

	EndRunLog();
}

void UAnomalyCaptureSubsystem::AccumulateFrameEvents(const TArray<FAutoLiveFireInfo>& Fires,
	const TArray<uint8>& FireActive, const TArray<FVector>& FirePos, const FAnomalyViewInfo& View,
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

		if (F.bWholeFrameExtent || ActiveSessionGlobals.Contains(F.Id))
		{
			Ev->AffectedFrames.Add(SessionIndex);
			Ev->CoverageSum += 1.0;
			++Ev->CoverageCount;
		}
		else if (const AActor* FActor = F.TargetActor.Get())
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

		const int32 Active = (FireActive.IsValidIndex(i) && FireActive[i]) ? 1 : 0;
		if (Active) { ++Ev->ActiveFrames; } else { ++Ev->InactiveFrames; }
		Ev->ActiveByIndex.Add(SessionIndex, (uint8)Active);
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
		TArray<int32> ActiveKeys;
		Ev.ActiveByIndex.GetKeys(ActiveKeys);
		ActiveKeys.Sort();

		TArray<int32> ActiveIdx;
		for (int32 Key : ActiveKeys)
		{
			if (Ev.ActiveByIndex[Key]) { ActiveIdx.Add(Key); }
		}

		AnomalyLabel::FSessionEvent Out;
		MapAnomalyToClient(Ev.Id, Out.AnomalyType, Out.AnomalySubtype);

		bool bKnownId = false;
		const EAnomalyActiveSource Source = ResolveAnomalyActiveSource(Ev.Id, bKnownId);
		if (!bKnownId)
		{
			UE_LOG(LogAnomalyCapture, Warning,
				TEXT("Capture: anomaly id '%s' is not registered in the active-state table Ã¢â‚¬â€ falling back to the whole fire ")
				TEXT("window. If this anomaly TOGGLES its anomalous state inside its own fire window it MUST be added to ")
				TEXT("ResolveAnomalyActiveSource, or every frame of the window is labelled positive whether or not the ")
				TEXT("anomaly was actually showing."),
				*Ev.Id.ToString());
		}

		TArray<int32> FrameIndices;
		if (Source != EAnomalyActiveSource::FireWindow)
		{
			Out.bManifested = ActiveIdx.Num() > 0;
			if (Out.bManifested)
			{
				FrameIndices = MoveTemp(ActiveIdx);
			}
			else
			{
				++NonManifestedEvents;
				UE_LOG(LogAnomalyCapture, Warning,
					TEXT("Capture: '%s' event on '%s' NEVER MANIFESTED Ã¢â‚¬â€ no captured frame sampled the anomaly as active ")
					TEXT("(state source: %s). Writing zero positive frames and manifested=false (previously this emitted %d ")
					TEXT("on-screen frames as positives)."),
					*Ev.Id.ToString(), *Ev.NodeName, DescribeActiveSource(Source), Ev.AffectedFrames.Num());
			}
		}
		else
		{
			FrameIndices = Ev.AffectedFrames;
		}
		FrameIndices.Sort();
		Out.FrameIndices = MoveTemp(FrameIndices);
		if (Source != EAnomalyActiveSource::FireWindow && Out.bManifested)
		{
			UE_LOG(LogAnomalyCapture, Log,
				TEXT("Capture: TOGGLING-SUBSET id=%s target=%s source=%s positives=%d of %d fire-active frame(s) Ã¢â‚¬â€ ")
				TEXT("annotation.json carries the ACTIVE SUBSET (gapped), never the whole fire window. labels.jsonl still ")
				TEXT("covers the whole window, which is why the overlay tool shows AMBER OUTSIDE-SUBSET boxes there."),
				*Ev.Id.ToString(), *Ev.NodeName, DescribeActiveSource(Source),
				Out.FrameIndices.Num(), Ev.ActiveByIndex.Num());
		}
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
	TEXT("Toggle client-delivery mode (default OFF). ON: a run writes ONLY the client-facing artifacts Ã¢â‚¬â€ ")
	TEXT("Actual_Frames/ + Video_Clip/ + run_summary.json + annotation.json Ã¢â‚¬â€ and suppresses labels.jsonl ")
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

static FAutoConsoleCommandWithWorldAndArgs GCaptureDeliveryLabelsCmd(
	TEXT("IAI.Capture.DeliveryLabels"),
	TEXT("Whether labels.jsonl is written WHEN DELIVERY MODE IS ON. COMPILED DEFAULT is ON. With delivery OFF the ")
	TEXT("file is written regardless and this changes nothing. It exists because the overlay INSPECTION tool draws ")
	TEXT("its boxes from labels.jsonl, and delivery mode used to suppress that file while still COMPUTING the data - ")
	TEXT("so the tool could not run in the config the client actually ships. This ADDS one small text file to the ")
	TEXT("delivered set and nothing else: annotation.json's field set does NOT move and run.json stays suppressed, ")
	TEXT("so the seed is still withheld and the session is still not client-reproducible. Turn it off to restore the ")
	TEXT("pre-m32 minimal file set. The packaged default is read at startup from DefaultGame.ini [AnomalyCapture] ")
	TEXT("bWriteLabelsInDeliveryDefault; this command overrides it for the session, BETWEEN RUNS. Read the effective ")
	TEXT("value and its provenance from the StartRun echo (labelsInDelivery=...), never from this help text. ")
	TEXT("Usage: IAI.Capture.DeliveryLabels <0|1>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() < 1)
			{
				UE_LOG(LogAnomalyCapture, Warning, TEXT("Usage: IAI.Capture.DeliveryLabels <0|1>"));
				return;
			}
			if (UAnomalyCaptureSubsystem* Cap = ResolveCapture(World)) { Cap->SetLabelsInDelivery(FCString::Atoi(*Args[0]) != 0); }
		}));

static FAutoConsoleCommandWithWorldAndArgs GCaptureRunLogCmd(
	TEXT("IAI.Capture.RunLog"),
	TEXT("m38 THE RUN-SCOPED SESSION LOG. Writes anomaly_log.txt beside annotation.json holding this run's ")
	TEXT("LogAnomaly and LogAnomalyCapture lines and NOTHING ELSE, so a capture carries its own explanation and ")
	TEXT("survives the engine rotating its main log on the next launch. THREE-STATE: -1 AUTO, 0 OFF, 1 ON. AUTO ")
	TEXT("MIRRORS run.json - on when delivery mode is off, off when it is on - so a client-facing capture stays ")
	TEXT("minimal unless you force it. Out-of-range is REFUSED, never clamped. It adds ONE FILE and moves NO ")
	TEXT("artifact field: annotation.json and run_summary.json keysets are unchanged. The device FILTERS by ")
	TEXT("category and NEVER changes verbosity - IAI.Capture.RunLogVerbose is the separate knob that does that. ")
	TEXT("The packaged default is read at startup from DefaultGame.ini [AnomalyCapture] RunLogDefault; this ")
	TEXT("command overrides it for the session, BETWEEN RUNS. Read the effective value and its provenance from ")
	TEXT("the StartRun echo (Capture(runlog): EFFECTIVE FOR THIS RUN ...), never from this help text. ")
	TEXT("Usage: IAI.Capture.RunLog <-1|0|1>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() < 1)
			{
				UE_LOG(LogAnomalyCapture, Warning, TEXT("Usage: IAI.Capture.RunLog <-1|0|1>  (-1 auto, 0 off, 1 on)"));
				return;
			}
			if (UAnomalyCaptureSubsystem* Cap = ResolveCapture(World)) { Cap->SetRunLog(FCString::Atoi(*Args[0])); }
		}));

static FAutoConsoleCommandWithWorldAndArgs GCaptureRunLogVerboseCmd(
	TEXT("IAI.Capture.RunLogVerbose"),
	TEXT("m38 RAISE LogAnomaly TO Verbose FOR THE DURATION OF ONE RUN, AND RESTORE IT AT FinishRun. Default OFF. ")
	TEXT("This is DELIBERATELY SEPARATE from IAI.Capture.RunLog: the run log only ever FILTERS lines that are ")
	TEXT("already being emitted, and a run that silently changed global log verbosity and left it changed would be ")
	TEXT("a defect, not a feature. Both transitions are echoed - a VERBOSITY RAISED line naming the level being ")
	TEXT("saved, and a VERBOSITY RESTORED line naming the level put back - and a paired RUNLOG-VERBOSE-PROBE line ")
	TEXT("proves it BOTH WAYS inside the same run: raised=1 appears, raised=0 does not. The reason to want it is ")
	TEXT("the blinking toggle line, which is Verbose (Anomaly_Blinking.cpp:95) and is therefore absent from every ")
	TEXT("run that did not ask for it. Takes effect at the next IAI.Capture.Start, not mid-run. The packaged ")
	TEXT("default is read at startup from DefaultGame.ini [AnomalyCapture] bRunLogVerboseDefault. ")
	TEXT("Usage: IAI.Capture.RunLogVerbose <0|1>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() < 1)
			{
				UE_LOG(LogAnomalyCapture, Warning, TEXT("Usage: IAI.Capture.RunLogVerbose <0|1>"));
				return;
			}
			if (UAnomalyCaptureSubsystem* Cap = ResolveCapture(World)) { Cap->SetRunLogVerbose(FCString::Atoi(*Args[0]) != 0); }
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

static FAutoConsoleCommandWithWorldAndArgs GCaptureMaskReduceCmd(
	TEXT("IAI.Capture.MaskReduce"),
	TEXT("m34 - HOW the visible mask is reduced to per-tag counts+bounds. gpu (the COMPILED DEFAULT): a compute ")
	TEXT("shader reduces the mask to a 5 KB per-tag table on the GPU and the CPU reads back the TABLE, not the ")
	TEXT("full surface - this removes the per-armed-frame render-thread W*H scan (the owner-bisected capture ")
	TEXT("hitch, journal 054 s7). cpu: THE NAMED BISECT (the IAI.Capture.SVE 0 / TickPin precedent) - the ")
	TEXT("pre-m34 full-surface readback + render-thread scan, one setting away, no rebuild, no re-cook. both: ")
	TEXT("run the two side by side and compare EVERY tag's count and bounds on EVERY armed frame, one greppable ")
	TEXT("line each (MASK-REDUCE COMPARE id=... IDENTICAL | FIRST-DIFF tag=... field=...), result fed from the ")
	TEXT("gpu table. The reduction is INTEGER-ATOMIC, so gpu and cpu are BIT-EXACT equal - any difference is a ")
	TEXT("defect, no tolerance. WHAT is measured, the veto rule, FAnomalyMaskResult and every artifact are ")
	TEXT("UNCHANGED across modes; inert while the mask itself is off. Takes effect BETWEEN RUNS. PRECEDENCE: ")
	TEXT("this console override beats DefaultGame.ini [AnomalyCapture] MaskReduceDefault, which beats the ")
	TEXT("compiled default (G88: a loose ini beside a package is a silent no-op, the cooked config wins). Read ")
	TEXT("the effective value and its provenance from the StartRun echo (maskReduce=...), never from this help ")
	TEXT("text. Usage: IAI.Capture.MaskReduce <gpu|cpu|both>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() < 1)
			{
				UE_LOG(LogAnomalyCapture, Warning, TEXT("Usage: IAI.Capture.MaskReduce <gpu|cpu|both>"));
				return;
			}
			EAnomalyMaskReduceMode Parsed;
			if (!ParseMaskReduceMode(Args[0], Parsed))
			{
				UE_LOG(LogAnomalyCapture, Warning,
					TEXT("IAI.Capture.MaskReduce: '%s' is not one of gpu|cpu|both - REFUSED (G144: an ")
					TEXT("unrecognised value never silently becomes a default). Current value unchanged (%s)."),
					*Args[0], LexToStringAnomalyMaskReduceMode(GMaskReduceMode));
				return;
			}
			if (UAnomalyCaptureSubsystem* Cap = ResolveCapture(World))
			{
				if (Cap->IsCaptureActive())
				{
					UE_LOG(LogAnomalyCapture, Warning,
						TEXT("IAI.Capture.MaskReduce: ignored - a capture run is in progress."));
					return;
				}
				GMaskReduceMode = Parsed;
				GMaskReduceFromConsole = true;
				UE_LOG(LogAnomalyCapture, Log,
					TEXT("IAI.Capture.MaskReduce: EFFECTIVE READ-BACK = %s (source=%s). Takes effect at the ")
					TEXT("next IAI.Capture.Start."),
					LexToStringAnomalyMaskReduceMode(GMaskReduceMode), DescribeMaskReduceSource());
			}
		}));

static FAutoConsoleCommandWithWorldAndArgs GCaptureTickPinCmd(
	TEXT("IAI.Capture.TickPin"),
	TEXT("THE BISECT SWITCH FOR THE CAPTURE-TIME ENGINE TICK-MODE PIN, in the IAI.Capture.SVE 0 tradition: one ")
	TEXT("setting that reaches the other behaviour with no rebuild and NO RE-COOK. On a decoupled-tick fork the ")
	TEXT("pin forces the fixed-sim/variable-render mode OFF for the duration of a capture and restores it at ")
	TEXT("finish. PRECEDENCE: this console override beats DefaultGame.ini [AnomalyCapture] bTickModePinDefault, ")
	TEXT("which beats the compiled default (on where the fork is detected). The override exists because a loose ")
	TEXT("ini beside a package is a NO-OP (G88) - the cooked config wins - so without it the unpinned control leg ")
	TEXT("would cost a second COOK, and the pinned-vs-unpinned A/B is the whole measurement. Settable BOTH ways: ")
	TEXT("0 for the unpinned control leg, 1 for the pinned leg. On a build where the pin compiled out this ")
	TEXT("command STILL EXISTS and says so - a silently missing command on the host that matters is the failure ")
	TEXT("mode we refuse. Mid-run changes are ignored (stop first). The effective value and its source print in ")
	TEXT("the single TICKPIN line at IAI.Capture.Start. Usage: IAI.Capture.TickPin <0|1>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() < 1)
			{
				UE_LOG(LogAnomalyCapture, Warning, TEXT("Usage: IAI.Capture.TickPin <0|1>"));
				return;
			}
			if (UAnomalyCaptureSubsystem* Cap = ResolveCapture(World))
			{
				Cap->SetTickPin(FCString::Atoi(*Args[0]) != 0);
				UE_LOG(LogAnomalyCapture, Log,
					TEXT("IAI.Capture.TickPin: EFFECTIVE READ-BACK = %d (compiled=%d, source=%s)."),
					Cap->IsTickPinEnabled() ? 1 : 0,
					UAnomalyCaptureSubsystem::IsTickPinCompiled() ? 1 : 0,
					Cap->DescribeTickPinSource());
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
	TEXT("Select the capture grab point (default OFF = backbuffer). ON: capture via a SceneViewExtension Ã¢â‚¬â€ scene ")
	TEXT("colour after tonemap and BEFORE Slate composites the UI, so the frame is UI-free, and the frame/state key ")
	TEXT("is recovered by IDENTITY through the view-family ring instead of by arm-to-present ORDER. OFF: the m21 ")
	TEXT("backbuffer path Ã¢â‚¬â€ the presented frame including game UI. Mid-run changes are ignored (stop first). The ")
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
	TEXT("step) Ã¢â‚¬â€ every frame is an exact 1/target game-time slice, so video.fps is stamped at TARGET at any ")
	TEXT("ratio and plays natural; a slow run is a capture-time perf issue, not a video defect. wall: content ")
	TEXT("follows the WALL clock (sequencer/real-time titles) Ã¢â‚¬â€ a run slower than target stamps the sustained ")
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
				UE_LOG(LogAnomalyCapture, Warning, TEXT("IAI.Capture.ContentClock: unknown token '%s' Ã¢â‚¬â€ expected 'game' or 'wall'. No change."), *Args[0]);
			}
		}));

static FAutoConsoleCommandWithWorldAndArgs GCaptureFocusGateCmd(
	TEXT("IAI.Capture.FocusGate"),
	TEXT("Gate the first captured frame on game-window focus (default ON). ON: a capture Start ARMS immediately ")
	TEXT("but holds the first frame until the game window has foreground focus Ã¢â‚¬â€ so clicking Start in the ")
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

static FAutoConsoleCommandWithWorldAndArgs GCaptureCensusCmd(
	TEXT("IAI.Capture.Census"),
	TEXT("m36/m41 SELECTION CENSUS - candidate selection based on PIXELS ACTUALLY DRAWN, not bounds. m41 ")
	TEXT("flipped the COMPILED default to ON (with the mask's, since the census requires it); ")
	TEXT("[AnomalyCapture] bSelectionCensusDefault in DefaultGame.ini remains as an explicit provenance ")
	TEXT("readout, and this switch overrides whichever applies, for the session, BETWEEN RUNS. Read the ")
	TEXT("effective value and its provenance from the StartRun echo, never from this help text. ON: during a ")
	TEXT("capture run the m26 mask pass + m34 per-tag reduce run as a rolling multi-target CENSUS over the ")
	TEXT("prefiltered candidate set (frustum + poll radius + renderable type + exclusion patterns - NO bounds ")
	TEXT("occlusion trace, NO bounds coverage), up to 55 tags per census frame, batched until every candidate ")
	TEXT("is measured. Selection rule: MEASURED_ZERO is excluded categorically; MEASURED_NONZERO is eligible ")
	TEXT("iff measured drawn coverage >= IAI.Capture.CensusFloor; NOT_MEASURABLE falls back to the bounds path ")
	TEXT("per reason (nanite/tag_failed/hidden/not_yet_measured), except translucent-without-opt-in which is ")
	TEXT("EXCLUDED (IAI.Capture.CensusTranslucent). Requires the mask (IAI.Capture.Mask) and async capture. ")
	TEXT("The armed-frame measurement and the ZERO-ONLY veto are UNCHANGED and remain the backstop; ")
	TEXT("annotation.json's field set does not move. Usage: IAI.Capture.Census <0|1>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() < 1)
			{
				UE_LOG(LogAnomalyCapture, Warning, TEXT("Usage: IAI.Capture.Census <0|1>"));
				return;
			}
			if (UAnomalyCaptureSubsystem* Cap = ResolveCapture(World))
			{
				Cap->SetCensus(FCString::Atoi(*Args[0]) != 0);
				UE_LOG(LogAnomalyCapture, Log, TEXT("IAI.Capture.Census: EFFECTIVE READ-BACK = %d."),
					Cap->IsCensus() ? 1 : 0);
			}
		}));

static FAutoConsoleCommandWithWorldAndArgs GCaptureCensusFloorCmd(
	TEXT("IAI.Capture.CensusFloor"),
	TEXT("m36 - the census eligibility floor, as a PERCENT of the viewport area of MEASURED DRAWN pixels ")
	TEXT("(drawn_px/frame_px), compiled default 6.0. A MEASURED_NONZERO candidate below the floor is not ")
	TEXT("selectable. DELIBERATELY a separate knob from IAI.SetMinScreenCoverage (whose operand stays the ")
	TEXT("BOUNDS rect and which still governs the NOT_MEASURABLE fallback path). Out-of-range is REFUSED, ")
	TEXT("never clamped. PRECEDENCE: console > DefaultGame.ini [AnomalyCapture] ")
	TEXT("CensusMinDrawnCoveragePctDefault > compiled 0.5 (m37 lowered it from 6.0). Takes effect BETWEEN ")
	TEXT("RUNS; the effective value echoes at StartRun. Usage: IAI.Capture.CensusFloor <pct>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() < 1)
			{
				UE_LOG(LogAnomalyCapture, Warning, TEXT("Usage: IAI.Capture.CensusFloor <pct>"));
				return;
			}
			if (UAnomalyCaptureSubsystem* Cap = ResolveCapture(World))
			{
				Cap->SetCensusFloorPct(FCString::Atof(*Args[0]));
			}
		}));

static FAutoConsoleCommandWithWorldAndArgs GCaptureCensusCeilingCmd(
	TEXT("IAI.Capture.CensusCeiling"),
	TEXT("m37 - the census eligibility CEILING, as a PERCENT of the view-rect area of MEASURED DRAWN pixels ")
	TEXT("(drawn_px/frame_px), compiled default 25.0. The band is INCLUSIVE: a candidate is eligible iff ")
	TEXT("floor <= coverage <= ceiling. A MEASURED_NONZERO candidate ABOVE the ceiling is EXCLUDED ")
	TEXT("CATEGORICALLY - at scenery scale the LABEL is unusable, which is NOT the same as the anomaly ")
	TEXT("failing (a landscape hide blacks the frame and boxes half of it). SET <= 0 TO DISABLE the ceiling ")
	TEXT("entirely; StartRun says DISABLED out loud so a disabled ceiling can never read like a healthy one. ")
	TEXT("Above 100 is REFUSED, never clamped. PRECEDENCE: console > DefaultGame.ini [AnomalyCapture] ")
	TEXT("CensusMaxDrawnCoveragePctDefault > compiled 25.0. Takes effect BETWEEN RUNS; the effective value ")
	TEXT("echoes at StartRun. Usage: IAI.Capture.CensusCeiling <pct>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() < 1)
			{
				UE_LOG(LogAnomalyCapture, Warning, TEXT("Usage: IAI.Capture.CensusCeiling <pct>"));
				return;
			}
			if (UAnomalyCaptureSubsystem* Cap = ResolveCapture(World))
			{
				Cap->SetCensusCeilingPct(FCString::Atof(*Args[0]));
			}
		}));

static FAutoConsoleCommandWithWorldAndArgs GCaptureCensusMaxAgeCmd(
	TEXT("IAI.Capture.CensusMaxAge"),
	TEXT("m36/m41 - the FLOOR OF THE FRESHNESS WINDOW for a census verdict at fire time, in GAME TICKS ")
	TEXT("(compiled default 12, ini [AnomalyCapture] CensusMaxVerdictAgeTicksDefault). m41 CHANGED THE ")
	TEXT("SEMANTICS: the window is max(this knob, lastCompletedCycleTicks + 8), so it can never be shorter ")
	TEXT("than one census cycle. A fixed 12 on a host whose cycle exceeds 12 ticks expires the ")
	TEXT("EARLIEST-MEASURED candidates before the cycle even closes, biasing selection toward whatever was ")
	TEXT("measured last, and the only loud signal fires on ALL-fallback so a PARTIAL bias was silent. A ")
	TEXT("verdict older than the window is EXPIRED: the candidate falls back to the bounds path meanwhile. ")
	TEXT("0 STILL expires everything and is still the P-C11 loud-inert control (special-cased, so m41 does ")
	TEXT("not retire that lever by accident): every fire then logs the all-fallback WARNING and increments ")
	TEXT("census_fires_fallback_all. Read the EFFECTIVE window from the StartRun echo and from each ")
	TEXT("'Auto.Fire: census consulted=' line. Out-of-range is REFUSED. Takes effect BETWEEN ")
	TEXT("RUNS. Usage: IAI.Capture.CensusMaxAge <ticks>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() < 1)
			{
				UE_LOG(LogAnomalyCapture, Warning, TEXT("Usage: IAI.Capture.CensusMaxAge <ticks>"));
				return;
			}
			if (UAnomalyCaptureSubsystem* Cap = ResolveCapture(World))
			{
				Cap->SetCensusMaxVerdictAgeTicks(FCString::Atoi(*Args[0]));
			}
		}));

static FAutoConsoleCommandWithWorldAndArgs GCaptureCensusTranslucentCmd(
	TEXT("IAI.Capture.CensusTranslucent"),
	TEXT("m36 R2 - whether a candidate whose EVERY renderable slot is translucent WITHOUT ")
	TEXT("AllowTranslucentCustomDepthWrites is EXCLUDED from selection (compiled default ON, ini ")
	TEXT("[AnomalyCapture] bCensusExcludeTranslucentDefault). Such a target cannot write custom depth ")
	TEXT("(CustomDepthRendering.cpp ignores translucent materials without the opt-in), so measuring it would ")
	TEXT("produce a FALSE ZERO - the H6 route-e shape - and the owner ruled such targets unusable anyway ")
	TEXT("(the decal case, journal 060). OFF measures them instead (the route-e v2 experiment, no cook ")
	TEXT("needed). Counted in census_excluded_translucent. Takes effect BETWEEN RUNS. ")
	TEXT("Usage: IAI.Capture.CensusTranslucent <0|1>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() < 1)
			{
				UE_LOG(LogAnomalyCapture, Warning, TEXT("Usage: IAI.Capture.CensusTranslucent <0|1>"));
				return;
			}
			if (UAnomalyCaptureSubsystem* Cap = ResolveCapture(World))
			{
				Cap->SetCensusExcludeTranslucent(FCString::Atoi(*Args[0]) != 0);
			}
		}));

static FAutoConsoleCommandWithWorldAndArgs GCaptureCensusTranslucentWritersCmd(
	TEXT("IAI.Capture.CensusTranslucentWriters"),
	TEXT("m41 - whether a translucent slot that opts into AllowTranslucentCustomDepthWrites still counts as ")
	TEXT("translucent-only (compiled default OFF, ini [AnomalyCapture] ")
	TEXT("bCensusIncludeTranslucentCustomDepthWritersDefault). TWO KNOBS, ONE QUESTION EACH: ")
	TEXT("IAI.Capture.CensusTranslucent decides WHETHER translucent-only candidates are excluded at all; this ")
	TEXT("one decides WHETHER the custom-depth opt-in rescues them. It is consulted only when the first is ON. ")
	TEXT("OFF (the m41 default) EXCLUDES translucent-only candidates REGARDLESS of the opt-in: with the opt-in ")
	TEXT("the census measures a GEOMETRIC SILHOUETTE for a target that contributes no visible colour (a fog ")
	TEXT("card), it passes the floor, it is selected, it is hidden, and nothing visibly changes - and the ")
	TEXT("armed-frame veto reads THE SAME custom-depth silhouette, so on that class the census and the veto ")
	TEXT("are NOT independent checks. ON restores the pre-m41 behaviour for hosts that want glass-type ")
	TEXT("targets. Counted in census_excluded_translucent. Takes effect BETWEEN RUNS. ")
	TEXT("Usage: IAI.Capture.CensusTranslucentWriters <0|1>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() < 1)
			{
				UE_LOG(LogAnomalyCapture, Warning, TEXT("Usage: IAI.Capture.CensusTranslucentWriters <0|1>"));
				return;
			}
			if (UAnomalyCaptureSubsystem* Cap = ResolveCapture(World))
			{
				Cap->SetCensusIncludeTranslucentWriters(FCString::Atoi(*Args[0]) != 0);
			}
		}));

static FAutoConsoleCommandWithWorldAndArgs GCaptureTargetMaskCmd(
	TEXT("IAI.Capture.TargetMask"),
	TEXT("m43 TARGET ID MASK - one 8-bit grayscale PNG per captured frame at target_mask/frame_NNNNN.png, ")
	TEXT("numbered by SESSION INDEX so it sorts with Actual_Frames. Non-zero pixel values are the stencil ")
	TEXT("tags of the ANOMALY TARGETS visible in that frame; 0 is background. mask_map.json maps ")
	TEXT("value+event to target name and anomaly type; labels.jsonl gains mask_file on the frame row and ")
	TEXT("mask_value on each anomaly row. THIS IS NOT A MASK OF EVERY OBJECT IN THE SCENE - only anomaly ")
	TEXT("targets appear, by design. A BLANK png means MEASURED AND NOTHING VISIBLE (a hidden blinking ")
	TEXT("target, which is real ground truth); mask_file:null means NOT MEASURED. It reuses the m26 ")
	TEXT("visible-mask pass - no new shader, no new render pass - and does NOT change the m26 measurement, ")
	TEXT("the veto or annotation.json. Requires IAI.Capture.Mask. REFUSED when IAI.Capture.OutputHeight is ")
	TEXT("non-zero, because a label mask must never be filtered. COMPILED default ON; ini ")
	TEXT("[AnomalyCapture] bTargetMaskDefault. Takes effect BETWEEN RUNS; delivery mode does NOT suppress ")
	TEXT("it. Usage: IAI.Capture.TargetMask <0|1>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() < 1)
			{
				UE_LOG(LogAnomalyCapture, Warning, TEXT("Usage: IAI.Capture.TargetMask <0|1>"));
				return;
			}
			if (UAnomalyCaptureSubsystem* Cap = ResolveCapture(World))
			{
				Cap->SetTargetMask(FCString::Atoi(*Args[0]) != 0);
			}
		}));

static FAutoConsoleCommandWithWorldAndArgs GBenchProbeSceneTextureUsageCmd(
	TEXT("IAI.Bench.ProbeSceneTextureUsage"),
	TEXT("BENCH DEVICE, console only - no ini key, never in a client payload, touches no level and no ")
	TEXT("capture. Resolves ONE named material through EXACTLY the code path the m41 host-PP preflight ")
	TEXT("uses (GetMaterialResource -> GetGameThreadShaderMap -> UsesSceneTexture) and prints the scene ")
	TEXT("textures it finds. It exists because a synthetic level with no post-process volume and a ")
	TEXT("camera-less pawn makes the preflight read 0 volumes / 0 blends / 0 materials, which is a TRUE ")
	TEXT("answer on that fixture and simultaneously proves nothing about the query - blindness and a clean ")
	TEXT("read are the same number there (G96). This probe separates them WITHOUT depending on any level. ")
	TEXT("Usage: IAI.Bench.ProbeSceneTextureUsage <object path>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() < 1)
			{
				UE_LOG(LogAnomalyCapture, Warning,
					TEXT("Usage: IAI.Bench.ProbeSceneTextureUsage <object path>"));
				return;
			}

			const FString Path = Args[0];
			UMaterialInterface* MI = LoadObject<UMaterialInterface>(nullptr, *Path);
			if (!MI)
			{
				UE_LOG(LogAnomalyCapture, Warning,
					TEXT("Bench.ProbeSceneTextureUsage: '%s' -> NOT PRESENT IN THIS CONTAINER. That is a ")
					TEXT("MEASUREMENT, not a failure, and no substitute material is chosen for you."),
					*Path);
				return;
			}

			const ERHIFeatureLevel::Type FeatureLevel =
				(World && World->Scene) ? World->Scene->GetFeatureLevel() : GMaxRHIFeatureLevel;

			const FMaterialResource* Resource = MI->GetMaterialResource(FeatureLevel);
			FMaterialShaderMap* ShaderMap = Resource ? Resource->GetGameThreadShaderMap() : nullptr;
			if (!ShaderMap)
			{
				UE_LOG(LogAnomalyCapture, Warning,
					TEXT("Bench.ProbeSceneTextureUsage: '%s' -> resource=%d shaderMap=0. The material is ")
					TEXT("present but carries no game-thread shader map at this feature level, so the query ")
					TEXT("has nothing to read. Reported, not worked around."),
					*Path, Resource ? 1 : 0);
				return;
			}

			struct FProbeId { const TCHAR* Name; ESceneTextureId Id; };
			static const FProbeId Ids[] =
			{
				{ TEXT("CustomDepth"),       PPI_CustomDepth },
				{ TEXT("CustomStencil"),     PPI_CustomStencil },
				{ TEXT("PostProcessInput0"), PPI_PostProcessInput0 },
				{ TEXT("SceneColor"),        PPI_SceneColor },
				{ TEXT("SceneDepth"),        PPI_SceneDepth },
				{ TEXT("WorldNormal"),       PPI_WorldNormal },
				{ TEXT("Velocity"),          PPI_Velocity },
			};

			FString Found;
			int32 NumFound = 0;
			for (const FProbeId& Probe : Ids)
			{
				if (ShaderMap->UsesSceneTexture((uint32)Probe.Id))
				{
					Found += FString::Printf(TEXT(" %s"), Probe.Name);
					++NumFound;
				}
			}

			UE_LOG(LogAnomalyCapture, Warning,
				TEXT("Bench.ProbeSceneTextureUsage: '%s' -> domain=%d blendMode=%d sceneTextures=%d [%s ] ")
				TEXT("customDepth=%d customStencil=%d. Read through the SAME path as the preflight, so a ")
				TEXT("bit that lights here is a bit the preflight can see."),
				*Path, (int32)MI->GetMaterial()->MaterialDomain, (int32)MI->GetBlendMode(),
				NumFound, Found.IsEmpty() ? TEXT(" none") : *Found,
				ShaderMap->UsesSceneTexture(PPI_CustomDepth) ? 1 : 0,
				ShaderMap->UsesSceneTexture(PPI_CustomStencil) ? 1 : 0);
		}));

static FAutoConsoleCommandWithWorldAndArgs GBenchMaskPairingProbeCmd(
	TEXT("IAI.Bench.MaskPairingProbe"),
	TEXT("BENCH DEVICE, default OFF, console only - no ini key, never in a client payload. ON spawns a ")
	TEXT("MOVABLE magenta cube in front of the settled bench camera, tags it ONCE at spawn (so no ")
	TEXT("render-state recreate can confound the reading) and alternates its position every captured tick. ")
	TEXT("The picture is the trusted reference because the capturer has the m31 frame handshake; the mask ")
	TEXT("centroid for the probe's tag is compared against it. A mask that shows the PREVIOUS tick's ")
	TEXT("position is a mask arm served by the PREVIOUS render. Takes effect BETWEEN RUNS. ")
	TEXT("Usage: IAI.Bench.MaskPairingProbe <0|1>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() < 1)
			{
				UE_LOG(LogAnomalyCapture, Warning, TEXT("Usage: IAI.Bench.MaskPairingProbe <0|1>"));
				return;
			}
			if (UAnomalyCaptureSubsystem* Cap = ResolveCapture(World))
			{
				Cap->SetBenchMaskPairingProbe(FCString::Atoi(*Args[0]) != 0);
			}
		}));

static FAutoConsoleCommandWithWorldAndArgs GBenchCensusFixedExpiryCmd(
	TEXT("IAI.Bench.CensusFixedExpiry"),
	TEXT("BENCH DEVICE, default OFF, console only - no ini key, never in a client payload. ON forces the ")
	TEXT("PRE-m41 FIXED census expiry window (IAI.Capture.CensusMaxAge alone, cycle length ignored) instead ")
	TEXT("of m41's max(knob, lastCompletedCycleTicks + 8). It exists so D-G1's A-side and B-side run on ONE ")
	TEXT("BINARY - m40's L2 leg needed an unarchived intermediate build for exactly this reason and that is ")
	TEXT("not repeated. Takes effect BETWEEN RUNS. Usage: IAI.Bench.CensusFixedExpiry <0|1>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() < 1)
			{
				UE_LOG(LogAnomalyCapture, Warning, TEXT("Usage: IAI.Bench.CensusFixedExpiry <0|1>"));
				return;
			}
			if (UAnomalyCaptureSubsystem* Cap = ResolveCapture(World))
			{
				Cap->SetBenchCensusFixedExpiry(FCString::Atoi(*Args[0]) != 0);
			}
		}));

static FAutoConsoleCommandWithWorldAndArgs GBenchCensusBatchCapCmd(
	TEXT("IAI.Bench.CensusBatchCap"),
	TEXT("BENCH DEVICE, default 0 (off), console only - no ini key, never in a client payload. Caps the ")
	TEXT("number of candidates tagged per census batch, which stretches one census cycle over many more ")
	TEXT("ticks. D-G1 uses it to manufacture a cycle longer than the expiry knob without needing a host with ")
	TEXT("hundreds of candidates. Out-of-range is REFUSED. Takes effect BETWEEN RUNS. ")
	TEXT("Usage: IAI.Bench.CensusBatchCap <0..55>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() < 1)
			{
				UE_LOG(LogAnomalyCapture, Warning, TEXT("Usage: IAI.Bench.CensusBatchCap <0..55>"));
				return;
			}
			if (UAnomalyCaptureSubsystem* Cap = ResolveCapture(World))
			{
				Cap->SetBenchCensusBatchCap(FCString::Atoi(*Args[0]));
			}
		}));

static FAutoConsoleCommandWithWorldAndArgs GBenchCensusDropEntryCmd(
	TEXT("IAI.Bench.CensusDropEntry"),
	TEXT("BENCH DEVICE, default 0 (off), console only - no ini key, never in a client payload. N omits every ")
	TEXT("Nth prefiltered actor from the census's own candidate list while leaving it fully visible to the ")
	TEXT("FIRE path, so those actors reach selection with NO census entry and must read 'unseen'. This is ")
	TEXT("E-G2, the positive control for census_fires_unseen_candidates: without it a zero on a client host ")
	TEXT("would be blindness rather than a reading (G96). Out-of-range is REFUSED. Takes effect BETWEEN RUNS. ")
	TEXT("Usage: IAI.Bench.CensusDropEntry <0..64>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() < 1)
			{
				UE_LOG(LogAnomalyCapture, Warning, TEXT("Usage: IAI.Bench.CensusDropEntry <0..64>"));
				return;
			}
			if (UAnomalyCaptureSubsystem* Cap = ResolveCapture(World))
			{
				Cap->SetBenchCensusDropEveryNth(FCString::Atoi(*Args[0]));
			}
		}));

static FAutoConsoleCommandWithWorldAndArgs GCaptureCensusReservationCmd(
	TEXT("IAI.Capture.CensusReservation"),
	TEXT("m36 R10 - host stencil reservation (default ON). At StartRun, every stencil value in 200..254 found ")
	TEXT("on a component with bRenderCustomDepth that the plugin did not tag is RESERVED for the run and never ")
	TEXT("assigned by the census or the event allocator, so host pixels cannot be COUNTED under a plugin tag. ")
	TEXT("OFF is a GATE LEVER for the P-C12 companion only (proving the instrument can fail, G96) and must be ")
	TEXT("ON in any real leg. Takes effect BETWEEN RUNS. Usage: IAI.Capture.CensusReservation <0|1>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() < 1)
			{
				UE_LOG(LogAnomalyCapture, Warning, TEXT("Usage: IAI.Capture.CensusReservation <0|1>"));
				return;
			}
			if (UAnomalyCaptureSubsystem* Cap = ResolveCapture(World))
			{
				Cap->SetCensusReservation(FCString::Atoi(*Args[0]) != 0);
			}
		}));

static FAutoConsoleCommandWithWorldAndArgs GCaptureCensusLeakProbeCmd(
	TEXT("IAI.Capture.CensusLeakProbe"),
	TEXT("GATE ARTEFACT - default OFF, MUST be OFF in any build that ships. ON: the census DELIBERATELY leaves ")
	TEXT("exactly one tagged component un-restored, so the final CENSUS-HYGIENE check is proven able to report ")
	TEXT("a DIFF (P-C6 companion, G96 - a clean pass from an instrument never proven able to fail is not a ")
	TEXT("pass). Inert in delivery mode by guard. Usage: IAI.Capture.CensusLeakProbe <0|1>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() < 1)
			{
				UE_LOG(LogAnomalyCapture, Warning, TEXT("Usage: IAI.Capture.CensusLeakProbe <0|1>"));
				return;
			}
			if (UAnomalyCaptureSubsystem* Cap = ResolveCapture(World))
			{
				Cap->SetCensusLeakProbe(FCString::Atoi(*Args[0]) != 0);
			}
		}));

static FAutoConsoleCommandWithWorldAndArgs GCaptureCensusCoArmCmd(
	TEXT("IAI.Capture.CensusCoArm"),
	TEXT("GATE ARTEFACT - default OFF, MUST be OFF in any build that ships. ON: the census arms ONLY on ticks ")
	TEXT("where the event mask also armed, forcing the census arm to queue BEHIND the event arm on the SVE and ")
	TEXT("pop on the NEXT view family - the P-C10 delayed-pop attribution control (R4: tags stay on until ")
	TEXT("collected, values never reused in flight, so a delayed pop must still attribute every count to the ")
	TEXT("right candidate). Inert in delivery mode by guard. Usage: IAI.Capture.CensusCoArm <0|1>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() < 1)
			{
				UE_LOG(LogAnomalyCapture, Warning, TEXT("Usage: IAI.Capture.CensusCoArm <0|1>"));
				return;
			}
			if (UAnomalyCaptureSubsystem* Cap = ResolveCapture(World))
			{
				Cap->SetCensusCoArm(FCString::Atoi(*Args[0]) != 0);
			}
		}));

static FAutoConsoleCommandWithWorldAndArgs GCaptureCensusHostTagCmd(
	TEXT("IAI.Capture.CensusHostTag"),
	TEXT("GATE ARTEFACT - default absent, MUST NOT be used outside a gate leg. Sets bRenderCustomDepth plus ")
	TEXT("the given CustomDepthStencilValue on every primitive component of the exact-named actor, DIRECTLY ")
	TEXT("and OUTSIDE the plugin's tag map - simulating HOST-set custom depth for the P-C12 reservation ")
	TEXT("control. The plugin will treat that value as host-owned: with reservation ON it must appear in the ")
	TEXT("StartRun reserved set and never be assigned. This command does NOT restore anything; the state ")
	TEXT("persists for the process. Usage: IAI.Capture.CensusHostTag <ActorName> <value>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() < 2 || !World)
			{
				UE_LOG(LogAnomalyCapture, Warning, TEXT("Usage: IAI.Capture.CensusHostTag <ActorName> <value>"));
				return;
			}
			const int32 Value = FMath::Clamp(FCString::Atoi(*Args[1]), 0, 255);
			int32 Touched = 0;
			for (TActorIterator<AActor> It(World); It; ++It)
			{
				AActor* Actor = *It;
				if (!Actor || !Actor->GetName().Equals(Args[0], ESearchCase::IgnoreCase))
				{
					continue;
				}
				TInlineComponentArray<UPrimitiveComponent*> Prims;
				Actor->GetComponents(Prims);
				for (UPrimitiveComponent* Prim : Prims)
				{
					Prim->SetCustomDepthStencilValue(Value);
					Prim->SetRenderCustomDepth(true);
					++Touched;
				}
				break;
			}
			UE_LOG(LogAnomalyCapture, Warning,
				TEXT("IAI.Capture.CensusHostTag: actor '%s' value %d -> %d component(s) set OUTSIDE the plugin ")
				TEXT("tag map (GATE USE ONLY - simulates host-set custom depth; 0 components means the actor was ")
				TEXT("not found by exact name)."),
				*Args[0], Value, Touched);
		}));

#endif
