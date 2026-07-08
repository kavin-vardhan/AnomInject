#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "HAL/FileManager.h"
#include "AnomalyViewport.h"
#include "AnomalyCaptureSubsystem.generated.h"

struct FAnomalyCaptureAsyncState;

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


	void StartRun(const FString& BaseDir, bool bPng, int32 InSeed, int32 InFrameCap);

	void StopRun();

	void LogStatus() const;

	bool IsRunning() const { return bRunning; }

	void GetStatus(bool& bOutRunning, int32& OutFrames, FString& OutRunDir, int32& OutSeed) const;

	int32 GetFrameCap() const { return FrameCap; }
	FString GetSessionId() const { return SessionId; }

	void SetBurstConfig(int32 K, int32 Pre, int32 Positive, int32 Post, int32 Bursts);

	void SetViewLag(int32 L);

	void SetAsyncCapture(bool bInAsync);
	bool IsAsyncCapture() const { return bAsyncCapture; }

	void SetCaptureFps(int32 InFps);

protected:
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

private:
	enum class ECapturePhase : uint8
	{
		Idle,
		LeadIn,
		SettleAfterFire,
		Positives,
		SettleAfterRevert,
		PostGap,
		DrainTail
	};

	void BeginFire();
	void BeginRevert();
	void CaptureCurrentFrame();
	void FinishRun(bool bLogLine);
	void SampleViewThisTick();
	FAnomalyViewInfo ProjectionView() const;
	class UAnomalyAutoInjectorSubsystem* ResolveAuto() const;

	void AccumulateFrameEvents(const TArray<struct FAutoLiveFireInfo>& Fires, const TArray<uint8>& FireHidden,
		const TArray<FVector>& FirePos, const FAnomalyViewInfo& View, float NearClip, int32 SessionIndex, double TimeSeconds);
	void WriteSessionAnnotationFile();

	void EnsureCapturer();
	void ProcessCompletedFrames();
	void DrainAsyncToCompletion();

	bool bRunning = false;
	ECapturePhase Phase = ECapturePhase::Idle;
	int32 PhaseFramesLeft = 0;
	FString RunDir;
	FString SessionId;
	int32 Seed = 0;
	bool bFormatPng = true;
	uint64 StartFrame = 0;

	int32 BurstsDone = 0;
	int32 FramesWritten = 0;
	int32 PositiveFramesWritten = 0;
	int32 ZeroMatchBursts = 0;

	int32 SessionFrameIndex = 0;
	int32 FrameCap = 0;

	int32 ViewportW = 0;
	int32 ViewportH = 0;

	int32 VideoFps = 30;

	bool bSavedUseFixedTimeStep = false;
	double SavedFixedDeltaTime = 0.0;
	bool bFixedTimeStepOverridden = false;

	double FirstFrameTimeSeconds = -1.0;
	double LastFrameTimeSeconds = -1.0;
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
	TUniquePtr<FAnomalyCaptureAsyncState> Async;

#if WITH_EDITOR
	bool bSavedShowMouseControlLabel = false;
	bool bMouseLabelOverridden = false;
#endif
};
