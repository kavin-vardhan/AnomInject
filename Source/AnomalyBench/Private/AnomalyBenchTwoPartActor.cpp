#include "AnomalyBenchTwoPartActor.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"

AAnomalyBenchTwoPartActor::AAnomalyBenchTwoPartActor()
{
	PrimaryActorTick.bCanEverTick = false;
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	Root->SetMobility(EComponentMobility::Movable);
	RootComponent = Root;
	PartEligible = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PartEligible"));
	PartEligible->SetupAttachment(Root);
	PartEligible->SetMobility(EComponentMobility::Movable);
	PartIneligible = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PartIneligible"));
	PartIneligible->SetupAttachment(Root);
	PartIneligible->SetMobility(EComponentMobility::Movable);
}
