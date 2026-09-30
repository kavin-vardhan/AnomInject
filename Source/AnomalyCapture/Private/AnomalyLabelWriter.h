#pragma once

#include "CoreMinimal.h"
#include "PixelFormat.h"
#include "AnomalyPreviewCapture.h"
#include "AnomalyTelemetry.h"
#include "AnomalyViewport.h"
#include "AnomalyAutoInjectorSubsystem.h"

class UWorld;
struct FAnomalyCensusCounters;
struct FCameraClipRunAccum;
class FJsonObject;

namespace AnomalyLabel
{
	struct FCameraClipFrameDiag
	{
		bool bPresent = false;
		bool bSlab = false;
		bool bSphereProxy = false;
		bool bNearOverridden = false;
		int32 SlabPrimitives = 0;
		int32 EyeInsideBox = 0;
		bool bSatPositive = false;
		bool bUnconfirmed = false;
		float ClippedRayFraction = 0.0f;
		int32 ConfirmTraces = 0;
		int32 ConfirmHits = 0;
		int32 ConfirmMisses = 0;
		int32 ConfirmUnresolved = 0;
	};

	static constexpr int32 SchemaVersion = 1;

	enum class EAnomalyMaskState : uint8
	{
		Unmeasured = 0,
		Empty      = 1,
		Present    = 2
	};

	inline const TCHAR* DescribeMaskState(EAnomalyMaskState State)
	{
		switch (State)
		{
		case EAnomalyMaskState::Present: return TEXT("present");
		case EAnomalyMaskState::Empty:   return TEXT("empty");
		default:                         return TEXT("unmeasured");
		}
	}

	struct FCaptureSnapshot
	{
		uint64 FrameCounter = 0;
		int32  SessionIndex = 0;
		double TimeSeconds = 0.0;
		double WallSeconds = 0.0;
		float  NearClip = 0.0f;
		FAnomalyViewInfo View;
		TArray<FAutoLiveFireInfo> Fires;
		bool bTargetMask = false;
		int32 ShadersPending = 0;
		int32 AnomalyMaterialsIncomplete = 0;
		FString MaskFileRel;
		TArray<int32> MaskValues;
		EAnomalyMaskState MaskState = EAnomalyMaskState::Unmeasured;
		TArray<uint8>   FireActive;
		TArray<uint8>   FirePolicy;
		TArray<uint8>   FireOnScreen;
		TArray<FVector> FirePos;
		bool bExposureDip = false;
		bool bExposureDipScopeExcluded = false;
		TArray<int32>   TargetPixels;
		TArray<int32>   TargetDrawnPixels;
		TArray<uint8>   FireLabelled;
		TArray<uint8>   ConditionHeld;
		TArray<uint8>   Observable;
		TArray<FIntRect> DrawnBounds;
		TArray<FAnomalyTelemetry> Telemetry;

		struct FRenderTruthWatch
		{
			FName Id;
			FString Target;
			uint64 StartFrame = 0;
			FString TextureName;
			int32 Baseline = 0;
			int32 HeldLevel = 0;
			uint64 BaselineResourceId = 0;
		};
		TArray<FRenderTruthWatch> RenderWatch;
		bool bRenderWatchArmed = false;
		TArray<uint8> Trailing;
		TArray<uint8> RenderSettled;

		TArray<uint8> EntryEmit;
		TArray<uint8> EntryTransition;
		TArray<FAutoLiveFireInfo> TransitionFires;
		TArray<uint8> TransitionFireReasons;
		TArray<FAutoLiveFireInfo> TransitionCandidates;

		bool bViewGlobalPending = false;
		bool bGlobalEarlyPositive = false;
		FCameraClipFrameDiag CameraClip;
	};

	struct FLabelEntryCounts
	{
		bool bPresent = false;
		int32 Suppressed = 0;
		int32 TransitionEntries = 0;
	};

	FLabelEntryCounts CountLabelEntries(const FCaptureSnapshot& Snapshot);

	bool ProjectFireBox(const FAutoLiveFireInfo& F, const FAnomalyViewInfo& View, FVector2D& OutMin, FVector2D& OutMax);

	bool IsFireInstalledAt(const TArray<uint8>* FireInstalled, int32 FireIndex);

	bool IsFireInAnnotation(const TArray<uint8>* FirePolicy, const TArray<uint8>* FireActive, const TArray<uint8>* FireOnScreen,
		const TArray<uint8>* FireInstalled, int32 FireIndex);

	bool IsSnapshotEntryLabelled(const FCaptureSnapshot& Snapshot, int32 FireIndex);

	int32 MarkInterruptedEffects(FCaptureSnapshot& Snapshot);

	static constexpr int32 GTargetPixelsUnmeasured = -1;

	enum class EObservable : uint8
	{
		False = 0,
		True = 1,
		Unmeasured = 2,
	};

	void ConvertTightToBGRA(EPixelFormat Format, int32 BytesPerPixel, const TArray<uint8>& RawBytes,
		int32 W, int32 H, TArray<FColor>& OutPixels);

	double ComputeSubsampledMeanLuma(EPixelFormat Format, int32 BytesPerPixel, const TArray<uint8>& RawBytes,
		int32 W, int32 H, int32 Stride);

	void ComputeSubsampledMeanLumaSplit(EPixelFormat Format, int32 BytesPerPixel, const TArray<uint8>& RawBytes,
		int32 W, int32 H, int32 Stride, const TArray<uint8>* ExclusionMask,
		double& OutLumaAll, double& OutLumaExcl);

	void DeriveOutputSize(int32 SrcW, int32 SrcH, int32 TargetH, int32& OutW, int32& OutH, bool& bOutNeedsResample);

	bool ResampleAndEncodeBGRA(AnomalyPreview::EImageFormat Format, const TArray<FColor>& Pixels,
		int32 SrcW, int32 SrcH, int32 OutW, int32 OutH, TArray<uint8>& OutBytes, bool& bOutResampled);

	bool CaptureLabeledShot(UWorld* World, const FString& OutputDir, AnomalyPreview::EImageFormat Format,
		const FAnomalyViewInfo& ProjectionView, const FString& ImageRelName, int32 SessionIndex,
		double WallSeconds, int32 TargetOutputHeight, FString& OutImagePath, FString& OutSidecarPath,
		int32& OutNumLabels, int32& OutNativeW, int32& OutNativeH, int32& OutWrittenW, int32& OutWrittenH,
		bool& bOutResampled, bool bLog = true, bool bWriteLabels = true, const FCaptureSnapshot* SyncFrame = nullptr);

	FString BuildLabelRecordForSnapshot(const FCaptureSnapshot& Snapshot, int32 Width, int32 Height,
		const FString& ImageName, int32& OutNumLabels);

	bool EncodeAndWriteFrame(const FString& OutputDir, AnomalyPreview::EImageFormat OutFormat,
		const TArray<uint8>& RawBytes, EPixelFormat SrcFormat, int32 BytesPerPixel, int32 Width, int32 Height,
		int32 OutWidth, int32 OutHeight, const FString& ImageRelPath, const FString& Record,
		FCriticalSection& JsonlLock, bool bWriteLabels, bool& bOutResampled,
		TSharedPtr<const TArray<FColor>, ESPMode::ThreadSafe>* CanonicalPixels = nullptr);

	struct FRunManifest
	{
		int32 Seed = 0;
		int32 SettleFrames = 0;
		int32 ViewLagFrames = 0;
		int32 PreFrames = 0;
		int32 PositiveFrames = 0;
		int32 PostFrames = 0;
		int32 BurstCount = 0;
		int32 FrameCap = 0;
		FString SessionId;
		int32 ViewportW = 0;
		int32 ViewportH = 0;
		FString Format;
		uint64 StartFrame = 0;
		FString StartTimeUtc;
		FString Mode;
		FString TargetAnomaly;
		FString TargetActor;
		int32 TargetFps = 30;
		bool bPaced = true;
	};

	bool WriteRunManifest(const FString& RunDir, const FRunManifest& Manifest);

	struct FRingTelemetry
	{
		int32 Published = 0;
		int32 Consumed = 0;
		int32 Missed = 0;
		int32 Wrapped = 0;
		int32 Corrupted = 0;
		int32 WantedMatches = 0;
	};

	struct FTickPinTelemetry
	{
		bool bCompiled = false;
		bool bApplied = false;
		int32 Saved = -1;
		int32 Reasserts = 0;
		int32 GameTicks = 0;
	};

	struct FReadbackLayoutTelemetry
	{
		int32 SourceExtentX = 0;
		int32 SourceExtentY = 0;
		int32 RectMinX = 0;
		int32 RectMinY = 0;
		int32 RectMaxX = 0;
		int32 RectMaxY = 0;
		int32 W = 0;
		int32 H = 0;
		int32 BufferHeight = 0;
		int32 RowPitchInPixels = 0;
		int32 Format = 0;
	};

	bool WriteRunSummary(const FString& RunDir, int32 TotalFrames, int32 PositiveFrames, int32 BurstsDone,
		int32 ZeroMatchBursts, uint64 EndFrame,
		int32 TargetFps, double SustainedWallFps, double SpeedRatio, double StampedFps, double GameClockSpeedRatio, bool bPaced, bool bDeliveryMode,
		const FString& ContentClock, int32 NonManifestedEvents, const FString& CapturePath,
		const FRingTelemetry* Ring = nullptr,
		int32 MaskProbeArms = 0, int32 MaskResidualDiscards = 0, int32 MaskNoPassDiscards = 0,
		int32 VetoedEvents = 0, int32 TranslucentVetoes = 0, int32 TranslucencyUnknownVetoes = 0,
		const FTickPinTelemetry* TickPin = nullptr, int32 PatternExcludedTargets = 0,
		const FReadbackLayoutTelemetry* ReadbackLayout = nullptr,
		const ::FAnomalyCensusCounters* Census = nullptr,
		const struct FTargetMaskTelemetry* TargetMask = nullptr,
		const struct FShaderReadinessTelemetry* ShaderReadiness = nullptr,
		int32 FramesExposureDip = 0,
		const struct FObservabilityTelemetry* Observability = nullptr,
		int32 TranslucentOnlyExcludedTargets = 0,
		int32 UnmeasurableTargetsAdmitted = 0,
		int32 TargetDrawnPixelsMeasured = 0, int32 FramesDrawnUnexpected = 0,
		int32 FramesExposureDipSuppressed = 0,
		const struct FStuckMipTelemetry* StuckMip = nullptr, const TSharedPtr<FJsonObject>& ChangeSummary = nullptr,
		const TSharedPtr<FJsonObject>& TexCorruptSummary = nullptr,
		const struct FLabelSyncTelemetry* LabelSync = nullptr, const ::FCameraClipRunAccum* CameraClip = nullptr,
		int32 RefusedNaniteTargets = 0, const FString& NaniteTargetPolicy = FString());

	struct FLabelSyncTelemetry
	{
		FString AaMethod;
		int32 AaMethodValue = 0;
		bool bTemporalAa = false;
		int32 OnFramesConfigured = -1;
		int32 OffFramesConfigured = -1;
		int32 HideFramesConfigured = -1;
		int32 OnFrames = 0;
		int32 OffFrames = 0;
		int32 HideFrames = 0;
		int32 TransitionEntries = 0;
		int32 TransitionFrames = 0;
		int32 SuppressedEntries = 0;
		int32 OutOfOrderFrames = 0;
		int32 MaskTagRecycles = 0;
		int32 MaskTagPeakLive = 0;
		int32 MaskTagExhausted = 0;
		int32 MaskTagRetireQuarantined = 0;
		int32 MaskTagRetireHostFlagKept = 0;
		int32 MaskPriorCollisions = 0;
		int32 MaskPriorCollisionQuarantined = 0;
		int32 ReasonEntries[6] = { 0, 0, 0, 0, 0, 0 };
		int32 CarriedTransitionTracks = 0;
		int32 CarriedHideTracks = 0;
		int32 UnlabelledActiveEntries = 0;
		int32 SyncFramesWritten = 0;
	};

	struct FStuckMipTelemetry
	{
		int32 FiresApplied = 0;
		int32 TexturesHeld = 0;
		int32 FramesHeld = 0;
		int32 RefusedShared = 0;
		int32 RefusedNotStreamable = 0;
		int32 RefusedVirtual = 0;
		int32 RefusedImperceptible = 0;
		int32 RefusedTooSmallForRatio = 0;
		int32 RefusedNoEligibleTextures = 0;
		int32 RefusedNotRestored = 0;
		int32 RefusedAlreadyHeld = 0;
		int32 HoldTimeouts = 0;
		int32 RestoreTimeouts = 0;
		int32 RestoreFramesMax = 0;
		int32 TexturesAwaitingRestore = 0;
		int32 OnsetPrerollMax = -1;
		int32 RevertOnDestroy = 0;
		int32 UnverifiedAtTeardown = 0;
		FString LabelSource;
		int32 RenderRecordFrames = 0;
		int32 RenderHeldFrames = 0;
		int32 RenderUnknownFrames = 0;
		int32 RenderRecordMissingFrames = 0;
		int32 TrailingFrames = 0;
		int32 TrailingLabelledFrames = 0;
		int32 SettleTailFrames = 0;
		int32 SettleTailSetting = 0;
		int32 TrailsOpened = 0;
		int32 TrailsClosed = 0;
		int32 RestoreUnresolved = 0;
		int32 RestoreUnresolvedAtEnd = 0;
		int32 GtMirrorDisagreeFrames = 0;
		int32 MaskDeferredDropped = 0;
		int32 RefusedSharedWorld = 0;
		int32 RefusedBaselinePending = 0;
		int32 RestoreTrackedWhileBusy = 0;
		double PurityEnumerationMsMax = 0.0;
		int32 PurityInactiveLevelUsers = 0;
		int32 PurityUnregisteredUsers = 0;
		int32 HoldContaminations = 0;
		int32 ContaminatedFrames = 0;
		int32 ResourceReplacedFrames = 0;
		int32 TrailReopens = 0;
		int32 TrailMissingFrames = 0;
		int32 UnwatchedAfterClose = 0;
		int32 WatchRefrozenFrames = 0;
		int32 WatchMissingFrames = 0;
		int32 OrderHeldFrames = 0;
		int32 LiveWindowCut = 0;
		int32 RestoreCarriedAtEnd = 0;
		int32 RestoreInheritedAtStart = 0;
		int32 ObserveUnresolved = 0;
		int32 HoldMonitorScans = 0;
		double HoldMonitorMsMean = 0.0;
		double HoldMonitorMsP95 = 0.0;
		double HoldMonitorMsMax = 0.0;
		int32 HoldMonitorComponentsWalkedMax = 0;
		int32 HoldMonitorUnregisteredWatchedMax = 0;
		int32 HoldMonitorRegistrationRejudges = 0;
		int32 HoldMonitorDirtyRequeued = 0;
		int32 StreamerFences = 0;
		double StreamerFenceMsMax = 0.0;
		int32 StreamerFenceIncomplete = 0;
		int32 RestoreHeldForStreamerPlans = 0;
		int32 TrailDetaches = 0;
		int32 TrailDetachesAtNextFire = 0;
		int32 GraceFrames = 0;
		int32 ReopensInGrace = 0;
		int32 ReopensAfterDetach = 0;
		int32 ReopenUnrecoverableFrames = 0;
		int32 ReopenCrossTalkSuppressed = 0;
		int32 ForcedAuthorityFrames = 0;
		int32 LateReceiptAfterForce = 0;
		int32 InheritedMaskRecords = 0;
		int32 PartialFrames = 0;
		int32 PartialOnsetFrames = 0;
		int32 PartialMidFrames = 0;
		int32 PartialOffsetFrames = 0;
		int32 PartialMaxPerEdge = 0;
		int32 PartialEventsOverThree = 0;
		TArray<FString> PartialEvents;
		int32 UnresolvedFrames = 0;
		int32 UnresolvedEvents = 0;
	};

	struct FObservabilityTelemetry
	{
		int32 ObservableFrames = 0;
		int32 FramesConditionLost = 0;
		int32 ObservableMinPixels = 1;
	};

	struct FShaderReadinessTelemetry
	{
		double PrewarmMs = -1.0;
		int32 PrewarmMaterials = 0;
		int32 PrewarmIncomplete = 0;
		int32 FramesShadersPending = 0;
	};

	struct FTargetMaskTelemetry
	{
		int32 Measured = 0;
		int32 HiddenBlank = 0;
		int32 Unavailable = 0;
	};

	struct FTargetMaskMapEntry
	{
		int32 MaskValue = 0;
		FString EventId;
		FString TargetName;
		FString AnomalyType;
		int32 FirstFrame = 0;
		int32 LastFrame = 0;
	};

	bool WriteTargetMaskMap(const FString& RunDir, const TArray<FTargetMaskMapEntry>& Entries);


	struct FSessionVideo
	{
		FString FramesDir;
		FString VideoPath;
		int32 ResolutionW = 0;
		int32 ResolutionH = 0;
		double Fps = 30.0;
		int32 TargetFps = 30;
		int32 TotalFrames = 0;
	};

	struct FProvenanceRecord
	{
		FString AnomalyId;
		FString Target;
		int32 AnchorIndex = 0;
		float CoveragePct = -1.0f;
		int32 OcclusionSamplesPassed = 0;
		int32 OcclusionSamplesTotal = 0;
		float PollDistance = -1.0f;
		bool bValid = false;
	};

	bool WriteSelectionProvenance(const FString& RunDir, const TArray<FProvenanceRecord>& Records);

	struct FSessionNode
	{
		FString Name;
		FString Path;
		FVector GlobalPosition = FVector::ZeroVector;
		FString AssetName;
		FString ComponentClass;
		FVector BoundsOrigin = FVector::ZeroVector;
		FVector BoundsExtent = FVector::ZeroVector;
	};

	struct FSessionEvent
	{
		FString AnomalyType;
		FString AnomalySubtype;
		TArray<int32> FrameIndices;
		TArray<int32> InjectedFrameIndices;
		int32 ObservableFrameCount = 0;
		int32 UnmeasuredFrameCount = 0;
		bool bObservabilityMeasured = true;
		FString BboxSource = TEXT("projected");
		bool bManifested = true;
		double CoverageRatio = 0.0;
		float CoveragePct = -1.0f;

		TArray<FSessionNode> Nodes;
		int32 PrimaryIndex = 0;

		FString CamPath;
		FVector CamPosition = FVector::ZeroVector;
		FRotator CamRotation = FRotator::ZeroRotator;
		float CamFovDeg = 0.0f;
		float CamAspect = 0.0f;
		float CamNear = 0.0f;
		float CamFar = 0.0f;

		int64 TicksMsec = 0;
		FString EngineName;
		FString EngineVersion;
		FString EngineProject;

		bool bMaskProvided = false;
	};

	struct FSessionAnnotation
	{
		FString SessionId;
		FSessionVideo Video;
		TArray<FSessionEvent> Events;
	};

	bool WriteSessionAnnotation(const FString& RunDir, const FSessionAnnotation& Annotation);
}
