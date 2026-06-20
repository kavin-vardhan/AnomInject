// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "InputCoreTypes.h"      // FKey members (configurable keybinds)
#include "Math/RandomStream.h"   // FRandomStream (the one seeded scheduler RNG, R-SEED)
#include "AnomalyAutoInjectorSubsystem.generated.h"

class UCanvas;
class APlayerController;
class AActor;

/**
 * One live auto-fired anomaly the scheduler is tracking: the anomaly id, its target actor (weak ->
 * GC-safe), the target's name cached for HUD/log (survives the actor's destruction), and the seconds
 * left before auto-revert (R-LIFE). Plain C++ bookkeeping (not a USTRUCT), mirroring the anomalies'
 * own FPoppingTarget. Note: there is intentionally NO conflict-group field — the v1 scheduler invariant
 * is ONE-ANOMALY-PER-ACTOR (OVERRIDE-1), which subsumes both conflict groups, so the guard never needs
 * to read a group.
 */
struct FAutoLiveFire
{
	FName Id = NAME_None;
	TWeakObjectPtr<AActor> Target;
	FString TargetName;
	float SecondsRemaining = 0.0f;
};

/** A live auto-fire for the control-surface read-back (Slice 1, A3): no weak ptr — the cached name only. */
struct FAutoLiveFireInfo
{
	FName Id;
	FString Target;
	float SecondsRemaining = 0.0f;
};

/**
 * UAnomalyAutoInjectorSubsystem  (milestone m6 — automatic injection)
 *
 * A SEPARATE world subsystem from the injector and the selector. At gameplay start a UI selects which
 * anomaly types are enabled; while running, those anomalies fire RANDOMLY on whatever renderable objects
 * are on-screen at that moment, then auto-revert after a randomized hold. Live-autonomous — distinct from
 * the deferred capture/replay pipeline. It calls ONLY the injector's public ApplyAnomaly/RevertAnomaly;
 * IAnomaly, the injector subsystem, the anomalies, and the leaf helpers are all untouched.
 *
 * Concurrent but COLLISION-FREE BY CONSTRUCTION (no ref-count coordinator), via two invariants:
 *  (i)  One live fire per id. The injector registry holds ONE instance per id (re-Apply of a live id
 *       reverts-then-reapplies), so the scheduler never re-fires an id that is still live — clean revert
 *       accounting and the natural concurrency ceiling (max live <= distinct enabled-id count).
 *  (ii) ONE ANOMALY PER ACTOR (OVERRIDE-1). The scheduler never targets an actor that already hosts ANY
 *       live fire. This single invariant subsumes BOTH conflict groups — bHidden (missing_object/flicker)
 *       AND forced-LOD (lod_corruption/lod_popping) — and also prevents a hide masking a LOD change (an
 *       invisible/mislabeled sample, the exact failure the viewport layer exists to prevent). The deferred
 *       ref-count "hidden-by" coordinator (G12) remains the path for DELIBERATE compound/stacked anomalies.
 *
 * Explicit-core / thin-shell split (mirrors m4/m5):
 *  - The EXPLICIT, DETERMINISTIC CORE is AdvanceTime() (service auto-reverts + at most one timed fire
 *    window per call) and TryFireOnce() (force one fire attempt), plus the readbacks. It is a pure
 *    function of (seeded stream, enable-set, cadence, the renderable-visible set) and is driveable over
 *    the MCP bridge WITHOUT real time and WITHOUT the eyeball shell — IAI.Auto.Step <sec> -> AdvanceTime,
 *    IAI.Auto.FireOnce -> TryFireOnce. This is the m4 "the core is fully bridge-gatable" principle.
 *  - Two THIN SHELLS drive it: (1) the IAI.Auto.* console commands (the bridge gate), and (2) per-tick
 *    raw-key polling + an immediate-mode UDebugDrawService HUD (the owner's real-Play eyeball).
 *
 * THREE independent states, deliberately separated:
 *  - Enable (SetEnabled): the eyeball shell only — registers the HUD + polls keys. Default OFF -> dormant
 *    (Tick early-returns, no delegate) so every existing M0-m5 gate stays byte-identical.
 *  - Run (SetRunning): the auto-tick auto-feed — Tick -> AdvanceTime(DeltaTime). Forced OFF when !Enabled.
 *  - Step / FireOnce: direct manual core drive — work regardless of Enable/Run (given a configured
 *    enable-set + seed). This is how the bridge gates the scheduler deterministically.
 *
 * Determinism (R-SEED): all randomness comes from ONE FRandomStream, seeded once per run (console-settable
 * seed; default time-based). The draw protocol is FIXED and independent of ApplyAnomaly's result (see the
 * .cpp). The honest limit: the seed reproduces the CHOICES (id / target / hold / interval) given the same
 * sequence of visible sets (and the same Step granularity); full run reproducibility with fixed visible
 * sets is a capture/replay-pipeline concern, not v1.
 *
 * Self-scoping (R-CAD): targets are drawn from AnomalyViewport::GetVisibleRenderableActors directly; this
 * path does NOT use IAI.SetViewportScoping (keeping it ON would make the "=" apply redundantly re-test
 * visibility and could drop a target between pick and apply). No view -> fire nothing this window (never
 * inject blind). v1 pool = the 4 object-scoped anomalies only (globals + lighting_mismatch are a future
 * non-object track).
 *
 * Coexistence (R-COEXIST): manual selector/console injection of a pool id during an auto run is
 * UNSUPPORTED (it would clobber via the registry's one-instance-per-id; the auto-injector can only track
 * its own fires). Detected cases (selector UI on; viewport scoping on) are WARNED, not blocked.
 *
 * Game + PIE worlds only. Stays game-agnostic: public UE APIs only, never host types.
 */
UCLASS()
class ANOMALYINJECTOR_API UAnomalyAutoInjectorSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	/** Number of pool ids (and the matching count of toggle keybinds). The .cpp static_asserts the pool
	 *  array length against this. */
	static constexpr int32 NumPoolKeys = 4;

	// --- USubsystem / UWorldSubsystem ---
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// --- FTickableGameObject ---
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

	// --- Activation: two independent switches (ruling a) ---

	/** Eyeball shell on/off: registers/unregisters the HUD draw delegate + enables key polling. Idempotent.
	 *  Default OFF -> dormant -> every existing gate byte-identical. Disabling also stops a run (reverts live). */
	void SetEnabled(bool bInEnabled);
	bool IsEnabled() const { return bEnabled; }

	/** Auto-tick firing on/off (Tick -> AdvanceTime). Forced OFF when !Enabled (enable first). Starting a run
	 *  re-seeds the stream from the current seed and arms the first interval; stopping reverts all live fires. */
	void SetRunning(bool bInRunning);
	bool IsRunning() const { return bRunning; }

	// --- Explicit deterministic core (bridge-driveable WITHOUT Enable/Run) ---

	/** Advance the scheduler by DeltaSeconds: service auto-reverts (one pass) then, if the inter-fire timer
	 *  has elapsed, attempt exactly one fire and arm the next interval. Frame-semantics (<= 1 fire window per
	 *  call) — drive multi-fire timing with repeated Step calls. IAI.Auto.Step routes here. */
	void AdvanceTime(float DeltaSeconds);

	/** Force one fire attempt now, ignoring the inter-fire timer. Returns true iff an anomaly was applied.
	 *  Honours both invariants + the cap + the no-blind-fire rule, and the fixed draw protocol. IAI.Auto.FireOnce
	 *  routes here. Uses the current stream position (call IAI.Auto.Seed first for a reproducible sequence). */
	bool TryFireOnce();

	// --- Enable-set + cadence config (console-settable; sane defaults in Initialize) ---

	/** Enable/disable one of the four pool ids. Returns false on an id outside the pool. */
	bool SetAnomalyEnabled(FName Id, bool bInEnabled);

	/** Enable or disable all four pool ids at once. */
	void SetAllAnomaliesEnabled(bool bInEnabled);

	/** Set the run seed and re-initialize the stream immediately (so a subsequent FireOnce/Step is reproducible). */
	void SetSeed(int32 InSeed);

	/** Randomized inter-fire interval range [Min,Max] seconds (clamped: Min > 0, Max >= Min). */
	void SetIntervalRange(float MinSeconds, float MaxSeconds);

	/** Randomized per-fire hold range [Min,Max] seconds before auto-revert (clamped: Min > 0, Max >= Min). */
	void SetHoldRange(float MinSeconds, float MaxSeconds);

	/** Max concurrent live fires (clamped >= 1). Naturally also bounded by the distinct enabled-id count. */
	void SetMaxConcurrent(int32 InMax);

	/** If true, fires PERSIST until manual revert / run-stop / teardown (no auto-revert). Default false (R-LIFE). */
	void SetPersist(bool bInPersist);

	// --- Readbacks (the bridge state-gate assertions) ---

	/** Enabled pool ids in fixed pool order. */
	TArray<FString> GetEnabledIds() const;

	/** Is this pool id currently enabled? */
	bool IsAnomalyEnabled(FName Id) const;

	/** Number of live (fired, not-yet-reverted) anomalies. */
	int32 GetLiveFireCount() const { return LiveFires.Num(); }

	/** Live fires as "id target Ns" (or "id target persist"), for status assertions. */
	TArray<FString> GetLiveFireSummaries() const;

	/** The current run seed. */
	int32 GetSeed() const { return Seed; }

	// Cadence read-back (Slice 1, A3) — the control-surface analogue of LogStatus's printout.
	void GetIntervalRange(float& OutMin, float& OutMax) const { OutMin = IntervalMin; OutMax = IntervalMax; }
	void GetHoldRange(float& OutMin, float& OutMax) const { OutMin = HoldMin; OutMax = HoldMax; }
	int32 GetMaxConcurrent() const { return MaxConcurrent; }
	bool GetPersist() const { return bPersist; }

	/** Live fires as structured {id, target-name, seconds-remaining} (the read-back analogue of GetLiveFireSummaries). */
	TArray<FAutoLiveFireInfo> GetLiveFires() const;

	/** Log enable/run state, seed, cadence, the enabled set, and the live fires (IAI.Auto.Status). */
	void LogStatus() const;

	// --- Configurable keybinds ---

	/** Rebind an action key. Action is one of pool1/pool2/pool3/pool4/run/reseed. Returns false on an
	 *  unknown action (lets keys escape host/overlay collisions, and avoid the selector's Tab/C/G/H). */
	bool SetKeyBinding(FName Action, FKey Key);

protected:
	/** Game (standalone) + PIE only; never the editor preview/editing world (gotcha G7). */
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

private:
	// Thin shells + internals.
	void PollInput();                                       // raw-key poll -> toggle pool / run / reseed
	void DrawHUD(UCanvas* Canvas, APlayerController* PC);   // immediate-mode HUD (right column; off the selector's)
	void RegisterHUD();                                     // register the UDebugDrawService delegate (double-register-guarded)
	void UnregisterHUD();                                   // unregister it (idempotent) — on disable AND teardown
	int32 ServiceReverts(float DeltaSeconds);              // auto-revert past-deadline fires; returns # reverted
	int32 RevertAllLive();                                  // revert + clear every live fire (run-stop / disable)
	void WarnOnCoexistence() const;                         // selector-UI-on / viewport-scoping-on warnings (warn, not block)
	bool IsIdLive(FName Id) const;                          // any live fire with this id (invariant i)
	bool IsActorLive(const AActor* Actor) const;            // any live fire on this actor (invariant ii, OVERRIDE-1)

	/** The live fires the scheduler is tracking (the live-instance / revert-deadline bookkeeping). */
	TArray<FAutoLiveFire> LiveFires;

	/** Which pool ids are enabled for firing (subset of the fixed four-id pool). */
	TSet<FName> EnabledIds;

	/** The one seeded RNG behind every choice (interval, id, target, hold). Re-initialized on SetSeed / run-start. */
	FRandomStream Stream;

	/** Current run seed (default time-based, set in Initialize; console-settable). */
	int32 Seed = 0;

	// Cadence (R-CAD defaults; all console-settable).
	float IntervalMin = 4.0f;
	float IntervalMax = 9.0f;
	float HoldMin = 3.0f;
	float HoldMax = 6.0f;
	int32 MaxConcurrent = 4;
	bool bPersist = false;

	/** Seconds until the next fire window. Armed (an interval draw) at run-start and after each window. */
	float FireTimer = 0.0f;

	/** Human-readable result of the most recent fire attempt, drawn on the HUD (surfaces the zero-match case). */
	FString LastFireResult;

	/** Eyeball shell master switch (IAI.Auto.Enable). Default OFF -> dormant -> byte-identical gates. */
	bool bEnabled = false;

	/** Auto-tick firing switch (IAI.Auto.Run). Forced OFF when !bEnabled. */
	bool bRunning = false;

	/** Handle for the registered UDebugDrawService HUD delegate (invalid when not registered). */
	FDelegateHandle DebugDrawHandle;

	// Configurable keybinds (defaults set in Initialize): 1/2/3/4 toggle the four pool ids, J start/stop
	// the run, K reseed. Deliberately distinct from the selector's Tab/C/G/H (gotcha G26 — both poll raw
	// key state off the same PC, so distinct keys avoid both shells reacting to one press).
	FKey KeyPool[NumPoolKeys];
	FKey KeyRun;
	FKey KeyReseed;
};
