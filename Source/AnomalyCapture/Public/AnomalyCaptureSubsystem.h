#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "HAL/FileManager.h"
#include "Engine/EngineBaseTypes.h"
#include "AnomalyViewport.h"
#include "AnomalyCaptureSubsystem.generated.h"

struct FAnomalyCaptureAsyncState;
class FAnomalyPreviewTee;

UCLASS()
class ANOMALYCAPTURE_API UAnomalyCaptureSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	UAnomalyCaptureSubsystem();
	virtual ~UAnomalyCaptureSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;


	void StartRun(const FString& BaseDir, bool bPng, int32 InSeed, int32 InFrameCap,
		const FString& InTargetAnomaly = FString(), const FString& InTargetActor = FString(),
		const TArray<FString>& InTargetArgs = TArray<FString>(), int32 InOutputHeight = -1);

	void StopRun();

	void LogStatus() const;

	bool IsRunning() const { return bRunning; }

	bool IsCaptureActive() const { return bRunning; }

	void PreviewPump();
	void PreviewArm(uint32 InViewEpoch);
	bool PreviewPoll(TArray<uint8>& OutJpeg, int32& OutW, int32& OutH, uint32& OutEpoch);

	void SetFocusGate(bool bInGate);
	bool IsFocusGated() const { return bFocusGate; }

	void GetStatus(bool& bOutRunning, int32& OutFrames, FString& OutRunDir, int32& OutSeed) const;

	int32 GetFrameCap() const { return FrameCap; }
	FString GetSessionId() const { return SessionId; }

	void SetBurstConfig(int32 K, int32 Pre, int32 Positive, int32 Post, int32 Bursts);

	void SetViewLag(int32 L);

	void SetAsyncCapture(bool bInAsync);
	bool IsAsyncCapture() const { return bAsyncCapture; }

	void SetCaptureFps(int32 InFps);

	void SetCapturePace(bool bInPace);
	bool IsCapturePaced() const { return bPaceCapture; }

	void SetCaptureDelivery(bool bInDelivery);
	bool IsDeliveryMode() const { return bDeliveryMode; }

	void SetSveCapture(bool bInSve);
	bool IsSveCapture() const { return bSveCapture; }

	void SetMaskMeasure(bool bInMask);
	bool IsMaskMeasure() const { return bMaskMeasure; }

	void SetMaskProbe(bool bInProbe);
	bool IsMaskProbe() const { return bMaskProbe; }

	void SetTickPin(bool bInPin);
	bool IsTickPinEnabled() const { return bTickPinEnabled; }
	const TCHAR* DescribeTickPinSource() const;
	static bool IsTickPinCompiled();

	void SetOutputHeightOverride(int32 InHeight);
	int32 GetOutputHeightOverride() const { return OutputHeightOverride; }
	int32 GetEffectiveOutputHeight() const { return EffectiveOutputHeight; }

	enum class EContentClock : uint8 { Wall, Game };
	void SetContentClock(EContentClock InClock);
	EContentClock GetContentClock() const { return ContentClock; }

	struct FLastRunPacing
	{
		bool bValid = false;
		bool bPaced = true;
		double TargetFps = 0.0;
		double SustainedWallFps = 0.0;
		double SpeedRatio = 1.0;
		double StampedFps = 0.0;
	};
	const FLastRunPacing& GetLastRunPacing() const { return LastRunPacing; }

protected:
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

private:
	enum class ECapturePhase : uint8
	{
		Idle,
		ArmedPending,
		LeadIn,
		SettleAfterFire,
		Positives,
		SettleAfterRevert,
		PostGap,
		DrainTail
	};

	void BeginActualRun();
	void OnEndFrameMaskSample();
	void OnWorldTickEndMask(UWorld* World, ELevelTick TickType, float DeltaSeconds);
	bool HasGameWindow(UWorld* World) const;
	bool IsGameWindowFocused(UWorld* World) const;
	void BeginFire();
	void BeginRevert();
	void CaptureCurrentFrame();
	void FinalizeArmedLabel();
	void SampleDeferredHidden();
	void FinishRun(bool bLogLine);
	void PaceThisTick();
	void StampArmWallClock(double NowWall);
	void CheckEarlyPacingWarning();
	void ComputeRunPacing();
	void SampleViewThisTick();
	FAnomalyViewInfo ProjectionView() const;
	class UAnomalyAutoInjectorSubsystem* ResolveAuto() const;

	void AccumulateFrameEvents(const TArray<struct FAutoLiveFireInfo>& Fires, const TArray<uint8>& FireHidden,
		const TArray<FVector>& FirePos, const FAnomalyViewInfo& View, float NearClip, int32 SessionIndex, double TimeSeconds);
	void WriteSessionAnnotationFile();

	void ApplySessionGlobals();
	void RevertSessionGlobals();
	bool IsNearClipSlicingNow() const;
	void AppendSessionGlobalFires(TArray<struct FAutoLiveFireInfo>& InOutFires) const;

	TArray<FName> ActiveSessionGlobals;
	float SessionGlobalBaselineNearClip = 0.0f;
	int32 SessionGlobalPositiveFrames = 0;
	int32 SessionGlobalNegativeFrames = 0;

	const TCHAR* DescribeGrabPoint() const;
	void EnsureCapturer();
	void ProcessCompletedFrames();
	void DrainAsyncToCompletion();

	bool bRunning = false;
	bool bRunBegun = false;
	bool bFocusGate = true;
	double ArmWaitStartWall = 0.0;
	double LastArmWaitLogWall = 0.0;
	double FocusWaitTimeoutSeconds = 30.0;
	ECapturePhase Phase = ECapturePhase::Idle;
	int32 PhaseFramesLeft = 0;
	FString RunDir;
	FString LastRunDir;
	FString SessionId;
	int32 Seed = 0;
	bool bFormatPng = true;
	uint64 StartFrame = 0;

	enum class EOutputHeightSource : uint8 { CompiledDefault, Ini, Override, PerRun };

	int32 OutputHeightIni = 0;
	bool bOutputHeightFromIni = false;
	int32 OutputHeightOverride = -1;
	int32 EffectiveOutputHeight = 0;
	EOutputHeightSource OutputHeightSource = EOutputHeightSource::CompiledDefault;
	int32 SyncResamplesPerformed = 0;
	int32 SyncFirstWrittenW = 0;
	int32 SyncFirstWrittenH = 0;
	int32 SyncDimMismatches = 0;
	bool bLoggedFirstFrameMeasuredLine = false;

	const TCHAR* DescribeOutputHeightSource() const;
	void NoteSyncWrittenSize(int32 W, int32 H, const FString& ImageRelPath);
	void LogFirstFrameMeasuredLine(int32 SrcW, int32 SrcH, int32 OutW, int32 OutH, bool bResampled);

	int32 BurstsDone = 0;
	int32 FramesWritten = 0;
	int32 PositiveFramesWritten = 0;
	int32 ZeroMatchBursts = 0;
	int32 NonManifestedEvents = 0;
	int32 VetoedEvents = 0;
	int32 TranslucentVetoes = 0;
	int32 TranslucencyUnknownVetoes = 0;

	int32 SessionFrameIndex = 0;
	int32 FrameCap = 0;

	uint64 CaptureRequestSerial = 0;
	uint64 ArmedLabelRequestId = 0;
	bool bHasArmedLabel = false;
	uint64 DeferredHiddenRequestId = 0;
	bool bHasDeferredHidden = false;

	FName TargetAnomalyId = NAME_None;
	FString TargetActorName;
	TArray<FString> TargetAnomalyArgs;
	bool bTargetedMode = false;

	bool bAutoWasRunning = false;
	bool bDeinitializing = false;

	int32 ViewportW = 0;
	int32 ViewportH = 0;

	int32 VideoFps = 30;

	bool bSavedUseFixedTimeStep = false;
	double SavedFixedDeltaTime = 0.0;
	bool bFixedTimeStepOverridden = false;

	bool bTickPinEnabled = true;
	bool bTickPinFromIni = false;
	bool bTickPinFromConsole = false;
	bool bTickPinApplied = false;
	int32 TickPinSaved = -1;
	int32 TickPinReasserts = 0;
	int32 CaptureGameTicks = 0;

	double FirstFrameTimeSeconds = -1.0;
	double LastFrameTimeSeconds = -1.0;
	double FirstArmWallSeconds = -1.0;
	double LastArmWallSeconds = -1.0;
	bool bPaceCapture = true;
	bool bPaceInitialized = false;
	double NextPaceWallTarget = 0.0;
	bool bEarlyRatioWarned = false;
	FLastRunPacing LastRunPacing;
	FString EngineVersion;
	FString EngineProject;

	int32 SettleFrames = 2;
	int32 PreFrames = 4;
	int32 PositiveFrames = 8;
	int32 PostFrames = 4;
	int32 BurstCount = 0;

	int32 ViewLagFrames = 0;
	TArray<FAnomalyViewInfo> ViewRing;

	bool bAsyncCapture = true;
	bool bSveCapture = true;
	bool bMaskMeasure = false;
	bool bMaskMeasureFromIni = false;
	bool bMaskProbe = false;
	bool bMaskProbeFiredThisRun = false;
	FDelegateHandle MaskEndFrameHandle;
	FDelegateHandle MaskWorldTickEndHandle;
	bool bRectDeltaLogged = false;
	bool bDeliveryMode = false;
	bool bLabelsInDelivery = true;
	bool bLabelsInDeliveryFromIni = false;
	bool bLabelsInDeliveryFromConsole = false;
	int32 PatternExcludedTargets = 0;
	EContentClock ContentClock = EContentClock::Wall;
	TUniquePtr<FAnomalyCaptureAsyncState> Async;
	TUniquePtr<FAnomalyPreviewTee> PreviewTee;

#if WITH_EDITOR
	bool bSavedShowMouseControlLabel = false;
	bool bMouseLabelOverridden = false;
#endif
};
