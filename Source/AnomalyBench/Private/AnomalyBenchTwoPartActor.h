#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AnomalyBenchTwoPartActor.generated.h"

class USceneComponent;
class UStaticMeshComponent;

UCLASS(NotBlueprintable)
class AAnomalyBenchTwoPartActor : public AActor
{
	GENERATED_BODY()

public:
	AAnomalyBenchTwoPartActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnomalyBench")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnomalyBench")
	TObjectPtr<UStaticMeshComponent> PartEligible;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnomalyBench")
	TObjectPtr<UStaticMeshComponent> PartIneligible;
};
