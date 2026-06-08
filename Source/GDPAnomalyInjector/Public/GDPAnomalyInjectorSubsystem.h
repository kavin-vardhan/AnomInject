// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GDPAnomalyInjectorSubsystem.generated.h"

/**
 * UGDPAnomalyInjectorSubsystem
 *
 * M0 walking skeleton: a world-scoped, auto-ticking subsystem that proves the full
 * inject/restore loop end-to-end with ONE hardcoded anomaly (hide an actor).
 *
 * No anomaly abstraction/registry yet — that gets factored once we have several
 * concrete anomalies. This type stays game-agnostic: it references only public UE
 * APIs and must never depend on the host game module.
 */
UCLASS()
class GDPANOMALYINJECTOR_API UGDPAnomalyInjectorSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	// --- USubsystem / UWorldSubsystem ---
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// --- FTickableGameObject ---
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

	// --- M0 anomaly operations (called by the console command surface) ---

	/** Enumerate actors in the current world; log "Class | Name | Label" for each. */
	void ListActors() const;

	/**
	 * Hide every actor whose Name OR Class name contains Substring (case-insensitive).
	 * Deliberately never matches the editor label (GetActorLabel is editor-only and
	 * absent in cooked builds). Returns the number of actors hidden this call.
	 */
	int32 HideActorsMatching(const FString& Substring);

	/** Restore every actor we have hidden. Returns the number restored. */
	int32 ShowAllHidden();

protected:
	/**
	 * Restrict this subsystem to real game worlds (standalone Game + Play-In-Editor).
	 * It must never instantiate or tick in the editor preview/editing world — we never
	 * inject anomalies into the world being authored.
	 */
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

private:
	/** Actors we have hidden, tracked weakly so destroyed actors never dangle. */
	TArray<TWeakObjectPtr<AActor>> HiddenActors;

	/** Seconds accumulated since the last heartbeat, used to throttle it. */
	float HeartbeatAccumulator = 0.0f;
};
