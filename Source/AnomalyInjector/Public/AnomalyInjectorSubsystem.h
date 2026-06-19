// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "IAnomaly.h"
#include "AnomalyInjectorSubsystem.generated.h"

/**
 * UAnomalyInjectorSubsystem
 *
 * World-scoped, auto-ticking subsystem that OWNS the anomaly registry and acts as the
 * manager: it registers one instance of each anomaly type in Initialize, ticks the active
 * ones, dispatches Apply/Revert, and reverts everything on teardown. Restricted to
 * Game + PIE worlds (never the editor preview world).
 *
 * Stays game-agnostic: references only public UE APIs, never the host game module.
 */
UCLASS()
class ANOMALYINJECTOR_API UAnomalyInjectorSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	// Out-of-line dtor: the TMap<FName, TUniquePtr<IAnomaly>> deleter needs the complete
	// IAnomaly type, which is available in the .cpp translation unit (gotcha G9).
	virtual ~UAnomalyInjectorSubsystem();

	// --- USubsystem / UWorldSubsystem ---
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// --- FTickableGameObject ---
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

	// --- Targeting aid (not an anomaly) ---
	/** Enumerate actors in the current world; log "Class | Name | Label" for each. */
	void ListActors() const;

	/**
	 * Diagnostic (not an anomaly): test core viewport visibility against a SYNTHETIC view assembled
	 * from Args (no live player needed) and log per-component frustum / occlusion / visible. This is the
	 * deterministic synthetic-view state-gate driver. Args: <substring> <ox> <oy> <oz> <pitch> <yaw>
	 * <roll> [fovDeg] [aspect].
	 */
	void TestVisibility(const TArray<FString>& Args) const;

	// --- Viewport-visibility scoping (opt-in; default OFF) ---

	/** Enable/disable routing object-scoped anomaly targeting through AnomalyViewport. */
	void SetViewportScoping(bool bEnabled);

	/** Current scoping state for this world's subsystem. */
	bool IsViewportScopingEnabled() const { return bViewportScopingEnabled; }

	/** Static convenience for anomalies: resolve the subsystem from World and read its flag (false if none). */
	static bool IsViewportScopingEnabled(UWorld* World);

	// --- Anomaly manager API (called by the console command surface) ---

	/** Log each registered anomaly as "id - description - usage" (sorted by id). */
	void ListAnomalies() const;

	/**
	 * Look up Id and apply it with Args. Each anomaly reverts-then-reapplies internally if
	 * already active, so re-firing with new args never leaks state. Returns true if applied.
	 */
	bool ApplyAnomaly(const FName& Id, const TArray<FString>& Args);

	/** Revert one anomaly if it is active. Returns true iff it was active and got reverted. */
	bool RevertAnomaly(const FName& Id);

	/** Revert every active anomaly. Returns the number reverted. */
	int32 RevertAllActive();

	/** Number of currently-active anomalies. */
	int32 GetActiveAnomalyCount() const;

protected:
	/**
	 * Restrict this subsystem to real game worlds (standalone Game + Play-In-Editor).
	 * Never instantiates or ticks in the editor preview/editing world.
	 */
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

private:
	/**
	 * Anomaly registry: one owned instance per type, keyed by GetId(). Plain C++ (not a
	 * UPROPERTY — IAnomaly is not a UObject); GC-safety lives inside each anomaly via
	 * TWeakObjectPtr.
	 */
	TMap<FName, TUniquePtr<IAnomaly>> Anomalies;

	/** Seconds accumulated since the last heartbeat, used to throttle it. */
	float HeartbeatAccumulator = 0.0f;

	/**
	 * Opt-in viewport-visibility scoping. Default OFF so every existing gate is byte-identical
	 * (the regression gate). When ON, the four object-scoped anomalies route target resolution through
	 * AnomalyViewport. Toggled via IAI.SetViewportScoping <0|1>.
	 */
	bool bViewportScopingEnabled = false;
};
