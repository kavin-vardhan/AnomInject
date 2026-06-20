// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "AnomalyViewport.h"   // FAnomalyViewInfo (the per-tick view ring for the L-frame delayed projection)
#include "AnomalyCaptureSubsystem.generated.h"

/**
 * UAnomalyCaptureSubsystem (m7 capture/labeling, Stage 2) — the burst-orchestration capture run.
 *
 * A SEPARATE world subsystem (Game + PIE only, gotcha G7), dormant by default, in the control-server module
 * (Q2 housing; reuses the capture primitive + label writer + ImageWrapper; compiled with the rest of the
 * module under ANOMALY_CONTROL_SERVER — dataset capture is a dev/research activity, never a retail Shipping
 * build). It produces an ML-friendly labeled image sequence by driving the m6 auto-injector's DETERMINISTIC
 * core directly and capturing the game viewport each frame.
 *
 * Burst shape (per S1 — the K-frame settle is SYMMETRIC, because the render trails the game thread by >=1
 * frame, r.OneFrameThreadLag, so BOTH the fire and the revert boundaries have a lag window where game-thread
 * ground truth and on-screen pixels disagree):
 *
 *   [pre-roll: M clean negatives] -> FireOnce -> [settle K: skipped] -> [positives: P frames]
 *      -> RevertAllLiveFires -> [settle K: skipped] -> [post-roll: M clean negatives]
 *
 * A run REPEATS bursts (S3); consecutive bursts SHARE the gap — one burst's post-roll doubles as the next
 * burst's pre-roll, so the lead-in pre-roll is emitted once. BurstCount 0 = loop until Stop.
 *
 * Each captured frame: synchronous game-viewport capture + the auto-injector's GetLiveFires() snapshot, taken
 * on the SAME game-thread tick and stamped with one GFrameCounter (exact image<->label alignment). The label
 * (anomaly_present + per-fire bbox) falls out of GetLiveFires() directly — settle frames are simply not
 * captured, so positives see the live fire over corrupted pixels and post-roll negatives see an empty set over
 * clean pixels.
 *
 * Lifecycle ownership (S2): capture OWNS the fire/revert lifecycle. It commands the auto-injector via
 * TryFireOnce / RevertAllLiveFires and reads via GetLiveFires ONLY — it never drives AdvanceTime/Step (which
 * could fire a second interval-anomaly and break one-per-burst). It uses RevertAllLiveFires (not the injector's
 * RevertAll) so GetLiveFires stays accurate and post-roll labels clean.
 *
 * Coexistence (A2): during a capture run the auto-injector's Run must be OFF (capture owns firing). If Run is
 * ON at Start it is WARNED, not blocked (m6 precedent).
 *
 * Reproducibility (S4): the run seeds the auto-injector stream at Start; same seed + same visible-set sequence
 * (a fixed operator vantage) => identical SEQUENCE of fired (id, target) and therefore identical bbox-target.
 * NOT pixel-identity (the live scene has ambient motion — a capture/replay concern).
 */
UCLASS()
class ANOMALYCONTROLSERVER_API UAnomalyCaptureSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	// --- USubsystem / UWorldSubsystem ---
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// --- FTickableGameObject ---
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

	// --- Console-driven control (dormant by default) ---

	/** Start a capture run. BaseDir empty => <ProjectSaved>/AnomalyCaptures. bPng false => JPEG. InSeed < 0 =>
	 *  use the auto-injector's current seed. Creates run_<seed>_<timestamp>/ + run.json, then begins the burst
	 *  loop. No-op (warns) if already running or no auto-injector. */
	void StartRun(const FString& BaseDir, bool bPng, int32 InSeed);

	/** Stop the run: revert any in-flight fire (RevertAllLiveFires), write run_summary.json, leave the world clean. */
	void StopRun();

	/** Log run state + config + counters (IAI.Capture.Status). */
	void LogStatus() const;

	bool IsRunning() const { return bRunning; }

	/** Read-only status for the control-surface read-back (additive; ZERO behavior change to capture logic).
	 *  Added by the dashboard/control-server track for the WS snapshot + capture_stopped reply. */
	void GetStatus(bool& bOutRunning, int32& OutFrames, FString& OutRunDir, int32& OutSeed) const;

	/** Set the burst schedule (clamped: K>=0, others>=0, BurstCount>=0). Ignored mid-run (warns). */
	void SetBurstConfig(int32 K, int32 Pre, int32 Positive, int32 Post, int32 Bursts);

	/** Set the projection view-lag L (frames the bbox-projection view trails the captured pixels; S3). Default 1
	 *  (r.OneFrameThreadLag). Tune at the moving gate: box trails the object => L too small, leads => too large.
	 *  Conceptually DISTINCT from the settle-K. Ignored mid-run (warns). */
	void SetViewLag(int32 L);

protected:
	/** Game (standalone) + PIE only; never the editor preview/editing world (gotcha G7). */
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

private:
	/** Burst state machine phases. Settle phases are NOT captured; the others capture one frame per tick. */
	enum class ECapturePhase : uint8
	{
		Idle,
		LeadIn,            // M pre-roll negatives (emitted once at run start)
		SettleAfterFire,   // K skipped frames (let the fire reach the rendered frame)
		Positives,         // P captured frames while the fire is live
		SettleAfterRevert, // K skipped frames (let the revert reach the rendered frame)
		PostGap            // M negatives after a burst (= the next burst's shared pre-roll)
	};

	void BeginFire();              // TryFireOnce -> SettleAfterFire (tracks zero-match, A6)
	void BeginRevert();            // RevertAllLiveFires -> SettleAfterRevert
	void CaptureCurrentFrame();    // one labeled frame into RunDir (bLog=false), updates counters
	void FinishRun(bool bLogLine); // revert clean + run_summary.json + back to Idle
	void SampleViewThisTick();     // push the current view into the ring (called once per running tick)
	FAnomalyViewInfo ProjectionView() const;  // the view L frames back (the one matching the captured pixels)
	class UAnomalyAutoInjectorSubsystem* ResolveAuto() const;

	// Run state.
	bool bRunning = false;
	ECapturePhase Phase = ECapturePhase::Idle;
	int32 PhaseFramesLeft = 0;
	FString RunDir;
	int32 Seed = 0;
	bool bFormatPng = true;
	uint64 StartFrame = 0;

	// Counters (for Status + run_summary.json).
	int32 BurstsDone = 0;
	int32 FramesWritten = 0;
	int32 PositiveFramesWritten = 0;
	int32 ZeroMatchBursts = 0;

	// Burst schedule (console-settable; tuned for a clear gate — densify later).
	int32 SettleFrames = 2;     // K (>=1 for the r.OneFrameThreadLag window + margin)
	int32 PreFrames = 4;        // M lead-in
	int32 PositiveFrames = 8;   // P
	int32 PostFrames = 4;       // M post / shared gap
	int32 BurstCount = 0;       // 0 = loop until Stop

	// Delayed-view projection (S3): the bbox is projected with the view from L ring-entries ago. VALIDATED
	// DEFAULT L=0 — NOT "zero render lag": the capture subsystem (a UTickableWorldSubsystem) ticks BEFORE
	// UpdateCameraManager (LevelTick.cpp:1606 vs 1621), so GetActiveViewInfo at the capture tick already
	// returns the PREVIOUS frame's camera POV — exactly the view that rendered the pixels ReadPixels returns
	// (1-frame render lag, r.OneFrameThreadLag). The two 1-frame lags cancel, so L=0 matches; L=1 over-corrects
	// (box trails the object under motion). Frame-count relationship => FPS-invariant. The knob stays for the
	// future async-readback path, which has its OWN lag characteristic (re-derive there — do NOT assume L=0).
	int32 ViewLagFrames = 0;                 // L (validated default 0; see above)
	TArray<FAnomalyViewInfo> ViewRing;       // recent per-tick views, newest last; depth = L + margin
};
