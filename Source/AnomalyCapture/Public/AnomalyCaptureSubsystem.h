#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "HAL/FileManager.h"
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
		const TArray<FString>& InTargetArgs = TArray<FString>());

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

	int32 BurstsDone = 0;
	int32 FramesWritten = 0;
	int32 PositiveFramesWritten = 0;
	int32 ZeroMatchBursts = 0;
	int32 NonManifestedEvents = 0;

	int32 SessionFrameIndex = 0;
	int32 FrameCap = 0;

	uint64 ArmedLabelFrameId = 0;
	bool bHasArmedLabel = false;
	uint64 DeferredHiddenFrameId = 0;
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
	bool bSveCapture = false;
	bool bSveRectLogged = false;
	bool bDeliveryMode = false;
	EContentClock ContentClock = EContentClock::Wall;
	TUniquePtr<FAnomalyCaptureAsyncState> Async;
	TUniquePtr<FAnomalyPreviewTee> PreviewTee;

#if WITH_EDITOR
	bool bSavedShowMouseControlLabel = false;
	bool bMouseLabelOverridden = false;
#endif
};
