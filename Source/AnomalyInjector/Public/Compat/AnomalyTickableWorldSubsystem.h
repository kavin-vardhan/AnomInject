#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Tickable.h"
#include "Compat/AnomalyStatsIncludes.h"
#include "AnomalyTickableWorldSubsystem.generated.h"

UCLASS(Abstract)
class ANOMALYINJECTOR_API UAnomalyTickableWorldSubsystem : public UWorldSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	UAnomalyTickableWorldSubsystem();
	UAnomalyTickableWorldSubsystem(FVTableHelper& Helper);
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual UWorld* GetTickableGameObjectWorld() const override { return GetWorld(); }
	virtual ETickableTickType GetTickableTickType() const override;
	virtual bool IsTickable() const override;
	virtual bool IsAllowedToTick() const final;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override PURE_VIRTUAL(UAnomalyTickableWorldSubsystem::GetStatId, return TStatId(););
	bool IsInitialized() const { return bInitialized; }

private:
	void Audit(const TCHAR* Operation) const;
	bool bInitialized = false;
};
