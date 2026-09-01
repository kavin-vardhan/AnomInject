#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "InputCoreTypes.h"
#include "Math/RandomStream.h"
#include "AnomalyCensusProvider.h"
#include "AnomalyAutoInjectorSubsystem.generated.h"

class UCanvas;
class APlayerController;
class AActor;

struct FAutoLiveFire
{
	FName Id = NAME_None;
	TWeakObjectPtr<AActor> Target;
	FString TargetName;
	float SecondsRemaining = 0.0f;
	uint64 StartFrame = 0;
	bool bWholeFrameExtent = false;
};

struct FAutoLiveFireInfo
{
	FName Id;
	FString Target;
	TWeakObjectPtr<AActor> TargetActor;
	float SecondsRemaining = 0.0f;
	uint64 StartFrame = 0;
	bool bWholeFrameExtent = false;
};

UCLASS()
class ANOMALYINJECTOR_API UAnomalyAutoInjectorSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	static constexpr int32 NumPoolKeys = 6;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;


	void SetEnabled(bool bInEnabled);
	bool IsEnabled() const { return bEnabled; }

	void SetRunning(bool bInRunning);
	bool IsRunning() const { return bRunning; }


	void AdvanceTime(float DeltaSeconds);

	bool TryFireOnce();

	bool TryFireSpecific(FName Id, const FString& ActorName, const TArray<FString>& ExtraArgs = TArray<FString>());

	void SetCensusProvider(FAnomalyCensusQueryFn InQuery, FAnomalyCensusReadyFn InReady,
		FAnomalyCensusFireReportFn InFireReport, int32 InWaitBudgetTicks);
	void ClearCensusProvider();
	bool HasCensusProvider() const { return (bool)CensusQuery; }

	int32 RevertAllLiveFires();


	bool SetAnomalyEnabled(FName Id, bool bInEnabled);

	void SetAllAnomaliesEnabled(bool bInEnabled);

	void SetSeed(int32 InSeed);

	void SetIntervalRange(float MinSeconds, float MaxSeconds);

	void SetHoldRange(float MinSeconds, float MaxSeconds);

	void SetMaxConcurrent(int32 InMax);

	void SetPersist(bool bInPersist);


	TArray<FString> GetEnabledIds() const;

	bool IsAnomalyEnabled(FName Id) const;

	int32 GetLiveFireCount() const { return LiveFires.Num(); }

	TArray<FString> GetLiveFireSummaries() const;

	int32 GetSeed() const { return Seed; }

	void GetIntervalRange(float& OutMin, float& OutMax) const { OutMin = IntervalMin; OutMax = IntervalMax; }
	void GetHoldRange(float& OutMin, float& OutMax) const { OutMin = HoldMin; OutMax = HoldMax; }
	int32 GetMaxConcurrent() const { return MaxConcurrent; }
	bool GetPersist() const { return bPersist; }

	TArray<FAutoLiveFireInfo> GetLiveFires() const;

	void LogStatus() const;


	bool SetKeyBinding(FName Action, FKey Key);

protected:
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

private:
	void PollInput();
	void DrawHUD(UCanvas* Canvas, APlayerController* PC);
	void RegisterHUD();
	void UnregisterHUD();
	int32 ServiceReverts(float DeltaSeconds);
	void WarnOnCoexistence() const;
	bool IsIdLive(FName Id) const;
	bool IsActorLive(const AActor* Actor) const;

	TArray<FAutoLiveFire> LiveFires;

	TSet<FName> EnabledIds;

	FRandomStream Stream;

	int32 Seed = 0;

	float IntervalMin = 4.0f;
	float IntervalMax = 9.0f;
	float HoldMin = 3.0f;
	float HoldMax = 6.0f;
	int32 MaxConcurrent = 4;
	bool bPersist = false;

	float FireTimer = 0.0f;

	FString LastFireResult;

	bool bEnabled = false;

	bool bRunning = false;

	FDelegateHandle DebugDrawHandle;

	FAnomalyCensusQueryFn CensusQuery;
	FAnomalyCensusReadyFn CensusReady;
	FAnomalyCensusFireReportFn CensusFireReport;
	int32 CensusWaitBudgetTicks = 12;
	int32 CensusWaitTicksUsed = 0;
	bool bCensusFirstFireResolved = false;

	FKey KeyPool[NumPoolKeys];
	FKey KeyRun;
	FKey KeyReseed;
};
