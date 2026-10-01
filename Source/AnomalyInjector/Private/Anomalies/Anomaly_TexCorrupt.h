#pragma once

#include "CoreMinimal.h"
#include "IAnomaly.h"
#include "Anomalies/TexCorruptCore.h"

class AActor;
class UWorld;
class UMeshComponent;
class UMaterialInterface;
class UMaterialInstanceDynamic;
class UTexture2D;
class UTextureRenderTarget2D;
class UAnomalyInjectorSubsystem;

namespace AnomalyTexCorrupt
{
class FAnomaly_TexCorrupt final : public IAnomaly
{
public:
	FAnomaly_TexCorrupt(FName InId, EFamily InFamily);
	virtual ~FAnomaly_TexCorrupt();

	virtual FName   GetId() const override { return Id; }
	virtual FString GetDescription() const override;
	virtual FString GetUsage() const override { return TEXT("<substring> [mode]"); }

	virtual bool Apply(UWorld* World, const TArray<FString>& Args) override;
	virtual void TickAlways(float DeltaSeconds) override;
	virtual void Revert() override;
	virtual bool IsActive() const override { return bActive; }
	virtual bool IsVisualConditionHeld() const override;
	virtual unsigned char GetVisualConditionState() const override;
	virtual void NoteCapturedFrame(bool bAnomalousThisFrame) override;
	virtual bool GetTelemetry(FAnomalyTelemetry& Out) const override;
	virtual bool WantsTargetLostNotification() const override { return true; }
	virtual void OnTargetLost(AActor* Actor, bool bWorldEnding) override;
	virtual void OnWorldTeardown() override;

	bool IsRevertSettlingIn(const UWorld* World) const;

private:
	struct FOutput
	{
		UTexture2D* Source = nullptr;
		FString SourceName;
		EClass Class = EClass::None;
		int32 M = 0;
		int32 W = 0;
		int32 H = 0;
		int32 FirstMip = 0;
		int32 CookedM = 0;
		int32 Drop = 0;
		int32 RW = 0;
		int32 RH = 0;
		int32 RM = 0;
		UTextureRenderTarget2D* Target = nullptr;
		TArray<UMaterialInstanceDynamic*> LevelMids;
		int64 Bytes = 0;
		FScratchKey Key;
	};

	struct FScratchSet
	{
		FScratchKey Key;
		int32 M = 0;
		TArray<UTextureRenderTarget2D*> Levels;
		int64 Bytes = 0;
		bool bLive = false;
	};

	struct FHostMid
	{
		UMaterialInterface* Resolved = nullptr;
		UMaterialInstanceDynamic* Mid = nullptr;
		TArray<TPair<FMaterialParameterInfo, UTextureRenderTarget2D*>> Bound;
	};

	struct FOwnedSlot
	{
		TWeakObjectPtr<UMeshComponent> Comp;
		FName CompName = NAME_None;
		TWeakObjectPtr<AActor> Owner;
		int32 SlotIndex = 0;
		UMaterialInterface* Raw = nullptr;
		int32 RawArrayLen = 0;
		UMaterialInterface* Asset = nullptr;
		UMaterialInterface* Resolved = nullptr;
		UMaterialInstanceDynamic* HostMid = nullptr;
		bool bCommitted = false;
		int32 OverrideLenAfter = 0;
	};

	struct FTexRecord
	{
		FString Name;
		FString Param;
		FString Association;
		int32 LayerIndex = -1;
		FString Class;
		FString PixelFormat;
		int32 W = 0;
		int32 H = 0;
		int32 M = 0;
		int32 FirstMip = 0;
		int32 CookedM = 0;
		int64 RtBytes = 0;
		bool bNonSpatialExempt = false;
	};

	struct FUntouched
	{
		FString Slot;
		FString Reason;
	};

	struct FCollateral
	{
		TWeakObjectPtr<UTexture2D> Texture;
		FString Path;
		int32 ApplyLevel = -1;
	};

	void Hold(UObject* Obj);
	void LetGoAll();
	bool FailAt(int32 Step, const TCHAR* Reason, const FString& Detail);
	void ReleaseAllTargets();
	void ReleaseScratchSet(FScratchSet& S);
	bool AllocateAll(FString& OutDetail);
	bool SetupLevelMid(UWorld* World, FOutput& O, int32 Level, UMaterialInterface* Corruptor, UTexture2D* Noise, FString& OutFail);
	bool EnqueueOutput(UWorld* World, FOutput& O, bool bClear);
	int32 SourceMipFor(const FOutput& O, int32 Level) const;
	FScratchSet* FindScratch(const FScratchKey& Key);
	void GatherCollateral(UWorld* World);
	int32 CountCollateralDrops(int32* OutUnknownNow = nullptr) const;
	int32 CollateralIncomplete() const;
	void LogCollateral(const TCHAR* Kind) const;
	void TakePostRevertSample();
	TexCorruptPure::EHeld EvaluateCondition(int32& OutFirstBad) const;
	void Redraw();
	void RegisterTargetWatch(UAnomalyInjectorSubsystem* Injector);
	void ReleaseTargetWatch();
	void ResetEventState();
	int32 CommitSlots();
	void RestoreAndRelease(double StartSeconds, int32 DelayedBy);
	void FinishPendingRestore(const TCHAR* Context);
	void ClearPartScope();

	FName Id;
	EFamily Family;

	bool bActive = false;
	EMode Mode = EMode::None;
	int32 TileN = 1;
	FAttemptInfo Attempt;
	bool bCommitPending = false;
	uint64 CommitDueFrame = 0;
	bool bRestorePending = false;
	uint64 RestoreDueFrame = 0;
	int32 RestoreDelayedBy = 0;
	EWrongCopy Fault = EWrongCopy::None;
	int32 NoApply = 0;
	uint64 ApplyFrame = 0;
	int64 RequiredBytes = 0;
	TexCorruptPure::FEventAccount Account;
	int32 SlotsCorrupted = 0;
	int32 SlotsTotal = 0;
	int32 TicksSinceApply = 0;
	bool bForeignReplacePending = false;

	TWeakObjectPtr<UWorld> EventWorld;
	TWeakObjectPtr<UAnomalyInjectorSubsystem> Holder;
	TArray<UObject*> Held;
	TArray<TWeakObjectPtr<AActor>> Owners;
	TWeakObjectPtr<AActor> PrimaryOwner;
	FString PrimaryName;

	TArray<FOutput> Outputs;
	TArray<FScratchSet> Scratch;
	TArray<FHostMid> HostMids;
	TArray<FOwnedSlot> Slots;
	TArray<FTexRecord> TexRecords;
	TArray<FUntouched> Untouched;
	TArray<FString> PartsCorrupted;
	TArray<TWeakObjectPtr<AActor>> ScopedOwners;
	int32 ComponentsCorrupted = 0;
	int32 ComponentsSkipped = 0;

	TArray<FCollateral> Collateral;
	bool bCollateralTruncated = false;
	int32 CollateralDroppedByCap = 0;
	int32 CollateralUnmeasuredMaterials = 0;
	int32 CollateralUnresolved = 0;
	int32 CollateralNullSlots = 0;
	int32 CollateralUnknownAtApply = 0;
	int32 CollateralRenderedPrimitives = 0;
	bool bCollateralTaken = false;
	uint64 CollateralApplyFrame = 0;
	int32 CollateralNoApply = 0;
	uint64 RevertFrame = 0;
	uint64 PostRevertFrame = 0;
	bool bTerminalRollback = false;
};
}
