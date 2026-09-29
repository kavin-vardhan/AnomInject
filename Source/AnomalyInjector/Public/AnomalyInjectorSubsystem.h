#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "Subsystems/WorldSubsystem.h"
#include "IAnomaly.h"
#include "AnomalyCatalogTypes.h"
#include "AnomalyExclusion.h"
#include "Templates/Function.h"
#include "AnomalyInjectorSubsystem.generated.h"

class AActor;
class UMaterialInterface;

using FAnomalyTrailProviderFn = TFunction<AnomalyExclusion::ETrail(FName, FString&)>;

struct FAnomalyPartnerExclusion
{
	bool bExcluded = false;
	bool bAdmittedAfterUnresolved = false;
	FName Partner = NAME_None;
	AnomalyExclusion::EState PartnerState = AnomalyExclusion::EState::Idle;
	FString EventKey;

	FString Describe() const
	{
		return FString::Printf(TEXT("%s:%s"), *Partner.ToString(), ANSI_TO_TCHAR(AnomalyExclusion::DescribeState(PartnerState)));
	}
};

struct FAnomalyExclusionStats
{
	int32 ExcludedUvCorruption = 0;
	int32 ExcludedNormalCorruption = 0;
	int32 ExcludedStuckLowMip = 0;
	int32 AdmittedAfterUnresolved = 0;
};

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

	UMaterialInterface* GetTexCorruptUvMaterial() const;

	UMaterialInterface* GetTexCorruptNormalMaterial() const;

	class UTexture2D* GetTexCorruptNoiseTexture() const;

	void TexCorruptHold(UObject* Object);

	void TexCorruptLetGo(UObject* Object);


	void ListAnomalies() const;

	bool ApplyAnomaly(const FName& Id, const TArray<FString>& Args);

	bool RevertAnomaly(const FName& Id);

	int32 RevertAllActive();

	int32 GetActiveAnomalyCount() const;

	bool IsAnomalyCurrentlyAnomalous(const FName& Id) const;
	bool IsAnomalyVisualConditionHeld(const FName& Id) const;
	bool GetAnomalyTelemetry(const FName& Id, FAnomalyTelemetry& Out) const;

	bool GetCameraClippingFrameEvaluation(struct FAnomalyNearClipSlabResult& OutSlab, bool& bOutSphereProxy) const;

	bool DoesAnomalyHaveDeferredOnset(const FName& Id) const;

	bool DoesAnomalyUseRenderTruth(const FName& Id) const;

	bool GetAnomalyRenderTruthTextures(const FName& Id, TArray<FAnomalyRenderTruthTexture>& Out) const;

	bool GetAnomalyRestoringTextures(const FName& Id, TArray<FAnomalyRenderTruthTexture>& Out) const;

	bool ConsumeAnomalyContamination(const FName& Id, FString& OutReason);

	void SetAnomalyRefusal(const FName& Id, const FString& Reason);

	void ClearAnomalyRefusals();

	bool GetAnomalyRefusal(const FName& Id, FString& OutReason) const;

	void SetActorReserved(AActor* Actor, bool bReserved);

	void ClearReservedActors();

	bool IsActorReserved(const AActor* Actor) const;

	bool IsExcludedByPartner(FName Id, FString& OutWhy) const;

	FAnomalyPartnerExclusion EvaluatePartnerExclusion(FName Id) const;

	void NoteExclusion(FName Candidate, const FAnomalyPartnerExclusion& Verdict, const FString& Attempt);

	void SetTrailProvider(FAnomalyTrailProviderFn InProvider);

	void ClearTrailProvider();

	bool HasTrailProvider() const { return (bool)TrailProvider; }

	void ResetExclusionStats();

	FAnomalyExclusionStats GetExclusionStats() const { return ExclusionStats; }

	void NoteAnomalyCapturedFrame(const FName& Id, bool bAnomalousThisFrame);

	void WatchTargetForAnomaly(AActor* Actor, const FName& Id);

	void ClearTargetWatchForAnomaly(const FName& Id);


	TArray<FAnomalyCatalogEntry> GetAnomalyCatalog() const;

	TArray<FActiveAnomalyInfo> GetActiveAnomalies() const;

protected:
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

private:
	void DispatchAnomalyTicks(float DeltaTime);

	void OnWorldPreActorTickSynth(UWorld* World, ELevelTick TickType, float DeltaSeconds);

	void ServiceBenchDestroyLatch();

	bool IsIdFireLive(const FName& Id) const;

	bool IsStuckMipRestoring() const;

	FString PartnerEventKey(const FName& Id) const;

	FAnomalyTrailProviderFn TrailProvider;

	FAnomalyExclusionStats ExclusionStats;

	TMap<FName, uint64> LastApplyFrame;

	FName LastTexCorruptRevertId = NAME_None;

	UFUNCTION()
	void OnWatchedTargetEndPlay(AActor* Actor, EEndPlayReason::Type EndPlayReason);

	struct FTargetWatch
	{
		TWeakObjectPtr<AActor> Actor;
		FName AnomalyId;
	};
	TArray<FTargetWatch> TargetWatches;

	TMap<FName, TUniquePtr<IAnomaly>> Anomalies;

	struct FActiveRecord
	{
		TArray<FString> Args;
		double ApplyTimeSeconds = 0.0;
	};
	TMap<FName, FActiveRecord> ActiveRecords;

	TMap<FName, FString> RefusedIds;

	TArray<TWeakObjectPtr<AActor>> ReservedActors;

	float HeartbeatAccumulator = 0.0f;

	bool bViewportScopingEnabled = false;

	bool bAutoPoolSelection = false;

	bool bSynthTickOrder = false;

	FDelegateHandle SynthPreActorTickHandle;

	UPROPERTY()
	TObjectPtr<UMaterialInterface> MissingTextureChecker;

	UPROPERTY()
	TObjectPtr<UMaterialInterface> CorruptedTexturePink;

	UPROPERTY()
	TObjectPtr<UMaterialInterface> TexCorruptUv;

	UPROPERTY()
	TObjectPtr<UMaterialInterface> TexCorruptNormal;

	UPROPERTY()
	TObjectPtr<class UTexture2D> TexCorruptNoise;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UObject>> TexCorruptStrongRefs;
};
