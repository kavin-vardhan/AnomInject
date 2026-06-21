// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "InputCoreTypes.h"   // FKey members (configurable keybinds)
#include "AnomalySelectorSubsystem.generated.h"

class UCanvas;
class APlayerController;

/**
 * UAnomalySelectorSubsystem
 *
 * In-game object-selector + inject UI (milestone m5). A SEPARATE world subsystem from the
 * injector: it owns selection state, polls input, and draws an immediate-mode HUD, then calls
 * the existing UAnomalyInjectorSubsystem's public Apply/Revert. The injector subsystem, IAnomaly,
 * and the leaf helpers are untouched (no-core-change streak preserved).
 *
 * Explicit-core / thin-shell split (mirrors the m4 viewport layer):
 *  - The EXPLICIT CORE is the set of public, bridge-driveable methods below
 *    (AdvanceSelection / SelectPrevious / CycleAnomalyChoice / InjectSelected / RevertSelected
 *    + the readbacks). They mutate state deterministically and are gated over the MCP bridge in a
 *    Simulate session (which exposes a usable view, gotcha G23).
 *  - Two THIN SHELLS drive that core: (1) the IAI.Selector.* console commands (the bridge gate),
 *    and (2) per-tick raw-key input polling + the HUD draw (the owner's real-Play eyeball). The
 *    keys + HUD are NOT bridge-driveable; only the methods/commands are.
 *
 * The candidate set IS the m4 visible set (frustum AND occlusion), so this path is inherently
 * visible-scoped and does NOT touch IAI.SetViewportScoping. Inject targets exactly the selected
 * actor via the AnomalyTargeting "=" exact-match sentinel (InjectSelected passes "=" + GetName()).
 *
 * Activation is opt-in via IAI.SelectorUI <0|1> (default OFF). While OFF the subsystem is dormant
 * (Tick early-returns, no HUD delegate registered) so every existing M0-m4 gate is byte-identical.
 *
 * Game + PIE worlds only (gotcha G7). Stays game-agnostic: public UE APIs only, never host types.
 */
UCLASS()
class ANOMALYINJECTOR_API UAnomalySelectorSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	// --- USubsystem / UWorldSubsystem ---
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// --- FTickableGameObject ---
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

	// --- Activation ---

	/** Enable/disable the selector UI. Registers/unregisters the HUD draw delegate; idempotent
	 *  (calling with the current state is a no-op, so it never double-registers the delegate). */
	void SetUIEnabled(bool bEnabled);

	/** Is the selector UI currently enabled? */
	bool IsUIEnabled() const { return bUIEnabled; }

	// --- Explicit core: selection model (bridge-driveable; shared by keys + console) ---

	/** Refresh the visible set, then select the NEXT visible actor in name-sorted order (wraps;
	 *  picks the first when nothing is selected). No-ops if no actor is visible. */
	void AdvanceSelection();

	/** Refresh the visible set, then select the PREVIOUS visible actor in name-sorted order
	 *  (wraps; picks the last when nothing is selected). No-ops if no actor is visible. */
	void SelectPrevious();

	/** Advance the chosen anomaly to the next of the selectable object-scoped ids (wraps). */
	void CycleAnomalyChoice();

	/** Inject the chosen anomaly on the selected actor (default args) via the injector subsystem,
	 *  targeting ONLY that actor with the "=" exact-match token. Returns true iff applied. */
	bool InjectSelected();

	/** Revert the last anomaly this selector injected (by id) via the injector subsystem.
	 *  Returns true iff something was reverted. */
	bool RevertSelected();

	// --- Explicit core: readbacks (the bridge state-gate assertions) ---

	/** Name of the selected actor, or "(none)". */
	FString GetSelectedActorName() const;

	/** The current visible set as actor names, name-sorted (same order the HUD list shows). */
	TArray<FString> GetVisibleActorNames() const;

	/** The chosen anomaly id. */
	FName GetAnomalyChoice() const;

	/** Log the selected actor, the visible-set names, and the chosen anomaly (IAI.Selector.Status). */
	void LogStatus() const;

	// --- Configurable keybinds ---

	/** Rebind an action key. Action is one of next/prev/cycle/inject/revert. Returns false on an
	 *  unknown action. (Lets keys escape host/overlay collisions, e.g. Shift+Tab vs the Steam overlay.) */
	bool SetKeyBinding(FName Action, FKey Key);

protected:
	/** Game (standalone) + PIE only; never the editor preview/editing world (gotcha G7). */
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

private:
	// Thin shells + internals.
	void PollInput();                 // raw-key poll -> core methods (the eyeball shell's input half)
	void DrawHUD(UCanvas* Canvas, APlayerController* PC);   // immediate-mode HUD (the eyeball shell's draw half)
	void RegisterHUD();               // register the UDebugDrawService delegate (guarded against double-register)
	void UnregisterHUD();             // unregister it (idempotent) — called on disable AND teardown
	void RefreshVisibleSet();         // recompute the name-sorted visible set + reconcile the selection
	int32 IndexOfSelected() const;    // index of SelectedActor within VisibleActors, or INDEX_NONE

	/** The currently-selected actor, tracked by weak identity across refreshes (cleared if it
	 *  leaves the visible set or is destroyed). */
	TWeakObjectPtr<AActor> SelectedActor;

	/** Name-sorted snapshot of the visible actor set (frustum AND occlusion via AnomalyViewport). */
	TArray<TWeakObjectPtr<AActor>> VisibleActors;

	/** Index into the fixed four-anomaly choice list (see the .cpp). */
	int32 AnomalyChoiceIndex = 0;

	/** Id of the most recent anomaly this selector injected, for RevertSelected (one-instance-per-id). */
	FName LastInjectedId = NAME_None;

	/**
	 * Human-readable result of the most recent inject/revert, drawn on the HUD (R4). Surfaces the AMB-2
	 * zero-match case in real Play (e.g. "inject lod_corruption on <vfx>: 0 matched") which is otherwise
	 * only visible in the Output Log — so a non-applicable combo announces itself instead of looking silent.
	 */
	FString LastInjectResult;

	/** Throttle accumulator for the visible-set refresh (only while the UI is enabled). */
	float RefreshAccumulator = 0.0f;

	/** Selector UI master switch (IAI.SelectorUI). Default OFF -> dormant -> byte-identical gates. */
	bool bUIEnabled = false;

	/** Handle for the registered UDebugDrawService HUD delegate (invalid when not registered). */
	FDelegateHandle DebugDrawHandle;

	// Configurable keybinds (defaults set in Initialize). KeyPrev defaults to an invalid key:
	// the default "previous" gesture is Shift + KeyNext; rebinding prev assigns a dedicated key.
	FKey KeyNext;
	FKey KeyPrev;
	FKey KeyCycle;
	FKey KeyInject;
	FKey KeyRevert;

	/** Visible-set refresh cadence while the UI is enabled (also refreshed on-demand at Advance/Prev). */
	static constexpr float RefreshIntervalSeconds = 0.1f;
};
