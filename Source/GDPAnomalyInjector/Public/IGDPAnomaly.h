// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class UWorld;

/**
 * IGDPAnomaly — the locked M1 anomaly abstraction.
 *
 * A plain C++ polymorphic interface (NOT a UCLASS — the registry/dispatch needs no
 * reflection). One instance per anomaly type lives in the subsystem's registry and
 * carries its own active/inactive state. Anomalies cache whatever they need for Tick
 * (resolved target weak-ptrs, a world weak-ptr) inside Apply, so Tick needs only
 * DeltaSeconds. GC-safety comes from holding TWeakObjectPtr to any UObject.
 *
 * Contract:
 *  - Apply returns true iff it applied an observable effect (e.g. an actor anomaly that
 *    matched zero actors returns false and stays inactive).
 *  - Re-applying an already-active anomaly must revert-then-reapply (no state leak).
 *  - Revert undoes everything Apply did and leaves IsActive() == false.
 */
class IGDPAnomaly
{
public:
	virtual ~IGDPAnomaly() = default;

	/** Stable snake_case identifier, e.g. "missing_object". */
	virtual FName GetId() const = 0;

	/** One-line human description for GDP.ListAnomalies. */
	virtual FString GetDescription() const = 0;

	/** Argument usage hint, e.g. "<name-substring> [hz]". */
	virtual FString GetUsage() const = 0;

	/** Apply the anomaly to World using Args. Returns true iff an effect was applied. */
	virtual bool Apply(UWorld* World, const TArray<FString>& Args) = 0;

	/** Per-frame update for ticking anomalies; no-op for static ones. */
	virtual void Tick(float DeltaSeconds) {}

	/** Undo everything Apply did. Must leave IsActive() == false. */
	virtual void Revert() = 0;

	/** True between a successful Apply and its Revert. */
	virtual bool IsActive() const = 0;
};
