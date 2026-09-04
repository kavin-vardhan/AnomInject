#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "IAnomaly.h"
#include "AnomalyCatalogTypes.h"
#include "AnomalyInjectorSubsystem.generated.h"

class UMaterialInterface;

UCLASS()
class ANOMALYINJECTOR_API UAnomalyInjectorSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	UAnomalyInjectorSubsystem();

	virtual ~UAnomalyInjectorSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

	void ListActors() const;

	void TestVisibility(const TArray<FString>& Args) const;


	void DumpCatalog() const;

	void DumpActiveAnomalies() const;

	void DumpVisibleRenderableInfos() const;


	void SetViewportScoping(bool bEnabled);

	bool IsViewportScopingEnabled() const { return bViewportScopingEnabled; }

	static bool IsViewportScopingEnabled(UWorld* World);


	void SetAutoPoolSelection(bool bInAutoPool);

	bool IsAutoPoolSelection() const { return bAutoPoolSelection; }

	static bool IsAutoPoolSelection(UWorld* World);


	void SetSynthTickOrder(bool bEnabled);

	bool IsSynthTickOrderEnabled() const { return bSynthTickOrder; }

	static bool IsSynthTickOrderEnabled(UWorld* World);


	UMaterialInterface* GetMissingTextureMaterial() const;

	UMaterialInterface* GetCorruptedTextureMaterial() const;


	void ListAnomalies() const;

	bool ApplyAnomaly(const FName& Id, const TArray<FString>& Args);

	bool RevertAnomaly(const FName& Id);

	int32 RevertAllActive();

	int32 GetActiveAnomalyCount() const;

	bool IsAnomalyCurrentlyAnomalous(const FName& Id) const;
	bool IsAnomalyVisualConditionHeld(const FName& Id) const;


	TArray<FAnomalyCatalogEntry> GetAnomalyCatalog() const;

	TArray<FActiveAnomalyInfo> GetActiveAnomalies() const;

protected:
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

private:
	void DispatchAnomalyTicks(float DeltaTime);

	void OnWorldPreActorTickSynth(UWorld* World, ELevelTick TickType, float DeltaSeconds);

	TMap<FName, TUniquePtr<IAnomaly>> Anomalies;

	struct FActiveRecord
	{
		TArray<FString> Args;
		double ApplyTimeSeconds = 0.0;
	};
	TMap<FName, FActiveRecord> ActiveRecords;

	float HeartbeatAccumulator = 0.0f;

	bool bViewportScopingEnabled = false;

	bool bAutoPoolSelection = false;

	bool bSynthTickOrder = false;

	FDelegateHandle SynthPreActorTickHandle;

	UPROPERTY()
	TObjectPtr<UMaterialInterface> MissingTextureChecker;

	UPROPERTY()
	TObjectPtr<UMaterialInterface> CorruptedTexturePink;
};
