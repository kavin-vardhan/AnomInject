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

	// Capture/playback rate (default 30). During a run the engine ticks on a FIXED timestep of 1/fps,
	// so every captured frame is an exact 1/fps slice of game time (offline-render pattern).
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

	// Session-annotation assembly (Stage 2): fold one captured frame's live fires into the per-event
	// accumulator, and serialize the native multi-anomaly annotation.json at finalize.
	void AccumulateFrameEvents(const TArray<struct FAutoLiveFireInfo>& Fires, const TArray<uint8>& FireHidden,
		const TArray<FVector>& FirePos, const FAnomalyViewInfo& View, float NearClip, int32 SessionIndex, double TimeSeconds);
	void WriteSessionAnnotationFile();

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

	// Session-annotation metadata (captured at StartRun; serialized at finalize).
	int32 ViewportW = 0;
	int32 ViewportH = 0;

	// The NATIVE capture rate: during a run the engine runs on a fixed timestep of 1/VideoFps, so each
	// captured frame IS exactly 1/VideoFps of game time and the mp4 encodes at exactly this rate (natural
	// pacing on any machine; wall-clock render speed becomes irrelevant). IAI.Capture.Fps to change.
	int32 VideoFps = 30;

	// Fixed-timestep engagement state (saved at StartRun, restored at FinishRun).
	bool bSavedUseFixedTimeStep = false;
	double SavedFixedDeltaTime = 0.0;
	bool bFixedTimeStepOverridden = false;

	// World-time of the first/last ARMED frame (same clock as labels.jsonl "t"). Diagnostic only: under
	// the fixed timestep the measured rate must track VideoFps (settle gaps read it slightly low); a big
	// deviation means the fixed step didn't hold.
	double FirstFrameTimeSeconds = -1.0;
	double LastFrameTimeSeconds = -1.0;
	FString EngineVersion;           // e.g. "5.1"
	FString EngineProject;           // e.g. "StackOBot"

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
