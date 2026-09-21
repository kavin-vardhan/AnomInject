#include "Modules/ModuleManager.h"
#include "AnomalyCaptureSubsystem.h"
#include "AnomalyViewport.h"
#include "Camera/PlayerCameraManager.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "HAL/IConsoleManager.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"

// This module is target-allowlisted to Editor. It is never part of a packaged client.
class FAnomalyBenchModule final : public IModuleInterface
{
	TUniquePtr<FAutoConsoleCommandWithWorldAndArgs> Place;
	TWeakObjectPtr<UWorld> PlacedWorld;
public:
	void StartupModule() override
	{
		Place = MakeUnique<FAutoConsoleCommandWithWorldAndArgs>(TEXT("IAI.Bench.PlaceView"),
			TEXT("L2 one-shot camera-origin placement. Editor fixture only, -IAIBenchFixture, L_ShooterGym; outside capture."),
			FConsoleCommandWithWorldAndArgsDelegate::CreateRaw(this, &FAnomalyBenchModule::PlaceView));
	}
	void ShutdownModule() override { Place.Reset(); }
	void PlaceView(const TArray<FString>& Args, UWorld* World)
	{
		if (!World || !FParse::Param(FCommandLine::Get(), TEXT("IAIBenchFixture")) ||
			!World->GetMapName().Contains(TEXT("L_ShooterGym")) || PlacedWorld.Get() == World || Args.Num() != 1)
		{
			UE_LOG(LogTemp, Warning, TEXT("IAI-L2 REFUSED fixture/map/one-shot/request gate")); return;
		}
		auto* Cap = World->GetSubsystem<UAnomalyCaptureSubsystem>();
		auto* PC = World->GetFirstPlayerController();
		auto* Pawn = PC ? PC->GetPawn() : nullptr;
		FAnomalyViewInfo View;
		if (!Cap || Cap->IsCaptureActive() || !Pawn || !AnomalyViewport::GetActiveViewInfo(World, View))
		{
			UE_LOG(LogTemp, Warning, TEXT("IAI-L2 REFUSED active capture or no possessed view")); return;
		}
		const FVector Desired(-443.9807434082031, -70.0, 212.00010681152344);
		const FRotator Rotation(0, 0, -0.08256798918660047);
		const FVector OldPawn = Pawn->GetActorLocation();
		// Translate the pawn by the camera-origin error after rotating its measured camera offset.
		// No replacement camera, ghost mode, movement disable, velocity reset or persistent pin.
		const FVector LocalOffset = PC->GetControlRotation().Quaternion().UnrotateVector(View.Origin - OldPawn);
		const FVector NewPawn = Desired - Rotation.Quaternion().RotateVector(LocalOffset);
		PC->SetControlRotation(Rotation);
		const bool Moved = Pawn->SetActorLocation(NewPawn, false, nullptr, ETeleportType::TeleportPhysics);
		PlacedWorld = World; // A failed one-shot is still an attempted placement, never secretly retried.
		UE_LOG(LogTemp, Log, TEXT("IAI-L2 PLACED request=%s ok=%d pawn=%s oldPawn=%s newPawn=%s oldCamera=%s desiredCamera=%s desiredRot=%s gfc=%llu"),
			*Args[0], Moved, *Pawn->GetName(), *OldPawn.ToString(), *NewPawn.ToString(), *View.Origin.ToString(), *Desired.ToString(), *Rotation.ToString(), GFrameCounter);
		GLog->FlushThreadedLogs(); GLog->Flush();
	}
};
IMPLEMENT_MODULE(FAnomalyBenchModule, AnomalyBench)
