#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "InputCoreTypes.h"
#include "AnomalySelectorSubsystem.generated.h"

class UCanvas;
class APlayerController;

UCLASS()
class ANOMALYINJECTOR_API UAnomalySelectorSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;


	void SetUIEnabled(bool bEnabled);

	bool IsUIEnabled() const { return bUIEnabled; }


	void AdvanceSelection();

	void SelectPrevious();

	void CycleAnomalyChoice();

	bool InjectSelected();

	bool RevertSelected();


	FString GetSelectedActorName() const;

	TArray<FString> GetVisibleActorNames() const;

	FName GetAnomalyChoice() const;

	void LogStatus() const;


	bool SetKeyBinding(FName Action, FKey Key);

protected:
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

private:
	void PollInput();
	void DrawHUD(UCanvas* Canvas, APlayerController* PC);
	void RegisterHUD();
	void UnregisterHUD();
	void RefreshVisibleSet();
	int32 IndexOfSelected() const;

	TWeakObjectPtr<AActor> SelectedActor;

	TArray<TWeakObjectPtr<AActor>> VisibleActors;

	int32 AnomalyChoiceIndex = 0;

	FName LastInjectedId = NAME_None;

	FString LastInjectResult;

	float RefreshAccumulator = 0.0f;

	bool bUIEnabled = false;

	FDelegateHandle DebugDrawHandle;

	FKey KeyNext;
	FKey KeyPrev;
	FKey KeyCycle;
	FKey KeyInject;
	FKey KeyRevert;

	static constexpr float RefreshIntervalSeconds = 0.1f;
};
