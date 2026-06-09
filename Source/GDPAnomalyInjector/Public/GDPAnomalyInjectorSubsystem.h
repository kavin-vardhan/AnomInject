// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "IGDPAnomaly.h"
#include "GDPAnomalyInjectorSubsystem.generated.h"

/**
 * UGDPAnomalyInjectorSubsystem
 *
 * World-scoped, auto-ticking subsystem that OWNS the anomaly registry and acts as the
 * manager: it registers one instance of each anomaly type in Initialize, ticks the active
 * ones, dispatches Apply/Revert, and reverts everything on teardown. Restricted to
 * Game + PIE worlds (never the editor preview world).
 *
 * Stays game-agnostic: references only public UE APIs, never the host game module.
 */
UCLASS()
class GDPANOMALYINJECTOR_API UGDPAnomalyInjectorSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	// Out-of-line dtor: the TMap<FName, TUniquePtr<IGDPAnomaly>> deleter needs the complete
	// IGDPAnomaly type, which is available in the .cpp translation unit (gotcha G9).
	virtual ~UGDPAnomalyInjectorSubsystem();

	// --- USubsystem / UWorldSubsystem ---
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// --- FTickableGameObject ---
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

	// --- Targeting aid (not an anomaly) ---
	/** Enumerate actors in the current world; log "Class | Name | Label" for each. */
	void ListActors() const;

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
	 * UPROPERTY — IGDPAnomaly is not a UObject); GC-safety lives inside each anomaly via
	 * TWeakObjectPtr.
	 */
	TMap<FName, TUniquePtr<IGDPAnomaly>> Anomalies;

	/** Seconds accumulated since the last heartbeat, used to throttle it. */
	float HeartbeatAccumulator = 0.0f;
};
