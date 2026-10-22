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
	USceneComponent* Root = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnomalyBench")
	UStaticMeshComponent* PartEligible = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnomalyBench")
	UStaticMeshComponent* PartIneligible = nullptr;
};
