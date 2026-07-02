#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
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


	// InFrameCap > 0 caps the session at exactly that many written frames, then finalizes (0 = run until
	// StopRun / the burst-count schedule, legacy behavior).
	void StartRun(const FString& BaseDir, bool bPng, int32 InSeed, int32 InFrameCap);

	void StopRun();

	void LogStatus() const;

	bool IsRunning() const { return bRunning; }

	void GetStatus(bool& bOutRunning, int32& OutFrames, FString& OutRunDir, int32& OutSeed) const;

	// Session-capture accessors (surfaced in the control-server snapshot).
	int32 GetFrameCap() const { return FrameCap; }
	FString GetSessionId() const { return SessionId; }

	void SetBurstConfig(int32 K, int32 Pre, int32 Positive, int32 Post, int32 Bursts);

	void SetViewLag(int32 L);

	// Async capture toggle (default ON). When OFF, falls back to the synchronous ReadPixels grab.
	void SetAsyncCapture(bool bInAsync);
	bool IsAsyncCapture() const { return bAsyncCapture; }

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

	// Async capture helpers.
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

	// Session-local 0-based ordinal of the next frame to arm/write (== count armed so far). Doubles as
	// the frame-cap counter: on reaching FrameCap the run stops arming and finalizes.
	int32 SessionFrameIndex = 0;
	int32 FrameCap = 0;

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
	// Saved/restored around a capture run so the editor's PIE "Shift+F1 for Mouse Cursor" hint does
	// not contaminate captured frames (transient in-memory only — never SaveConfig'd).
	bool bSavedShowMouseControlLabel = false;
	bool bMouseLabelOverridden = false;
#endif
};
