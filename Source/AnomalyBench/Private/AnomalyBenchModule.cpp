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

// Explicit fixture commands only. Descriptor excludes Client/Server and Shipping targets.
class FAnomalyBenchModule final : public IModuleInterface
{
	TUniquePtr<FAutoConsoleCommandWithWorldAndArgs> Place;
	TUniquePtr<FAutoConsoleCommandWithWorldAndArgs> Lock;
	TWeakObjectPtr<UWorld> PlacedWorld;
	TWeakObjectPtr<UWorld> LockedWorld;
	TWeakObjectPtr<APlayerController> LockedController;
	FString SessionAtLock;
	bool bOwnInputLock = false;
	bool bSawCapture = false;
	FDelegateHandle TickHandle;
	FDelegateHandle CleanupHandle;
	static bool IsFixture(UWorld* World)
	{
		return World && FParse::Param(FCommandLine::Get(), TEXT("IAIBenchFixture")) &&
			(World->GetMapName().Contains(TEXT("CB_GateLevel")) || World->GetMapName().Contains(TEXT("L_ShooterGym")));
	}
	static void FlushLog() { GLog->FlushThreadedLogs(); GLog->Flush(); }
	void ReleaseInput(const TCHAR* Reason)
	{
		if (!bOwnInputLock) { return; }
		auto* PC = LockedController.Get();
		if (PC)
		{
			// UE stacks these flags. Remove only our increment; never reset the host's flags.
			PC->SetIgnoreLookInput(false); PC->SetIgnoreMoveInput(false);
		}
		UE_LOG(LogTemp, Log, TEXT("IAI-INPUT-LOCK OFF reason=%s controller=%s lookIgnored=%d moveIgnored=%d gfc=%llu"),
			Reason, *GetNameSafe(PC), PC && PC->IsLookInputIgnored(), PC && PC->IsMoveInputIgnored(), GFrameCounter);
		bOwnInputLock = false; bSawCapture = false; LockedController.Reset(); LockedWorld.Reset(); SessionAtLock.Reset();
		FlushLog();
	}
	void WorldTick(UWorld* World, ELevelTick, float)
	{
		if (!bOwnInputLock || LockedWorld.Get() != World) { return; }
		if (!LockedController.IsValid() || World->GetFirstPlayerController() != LockedController.Get())
		{
			ReleaseInput(TEXT("controller_changed")); return;
		}
		auto* Cap = World->GetSubsystem<UAnomalyCaptureSubsystem>();
		if (!Cap) { ReleaseInput(TEXT("capture_subsystem_lost")); return; }
		if (Cap->IsCaptureActive()) { bSawCapture = true; }
		else if (bSawCapture || Cap->GetSessionId() != SessionAtLock)
		{
			// The session-id check also handles a start+finish between two module ticks.
			ReleaseInput(TEXT("run_end"));
		}
	}
	void WorldCleanup(UWorld* World, bool, bool)
	{
		if (LockedWorld.Get() == World) { ReleaseInput(TEXT("world_cleanup")); }
		if (PlacedWorld.Get() == World) { PlacedWorld.Reset(); }
	}
public:
	void StartupModule() override
	{
		Place = MakeUnique<FAutoConsoleCommandWithWorldAndArgs>(TEXT("IAI.Bench.PlaceView"),
			TEXT("L2 one-shot camera-origin placement. -IAIBenchFixture, named bench maps; outside capture. CB requires InputLock."),
			FConsoleCommandWithWorldAndArgsDelegate::CreateRaw(this, &FAnomalyBenchModule::PlaceView));
		Lock = MakeUnique<FAutoConsoleCommandWithWorldAndArgs>(TEXT("IAI.Bench.InputLock"),
			TEXT("1 locks fixture look/move input before settle; 0 releases outside capture. Automatically released at run end."),
			FConsoleCommandWithWorldAndArgsDelegate::CreateRaw(this, &FAnomalyBenchModule::InputLock));
		TickHandle = FWorldDelegates::OnWorldPostActorTick.AddRaw(this, &FAnomalyBenchModule::WorldTick);
		CleanupHandle = FWorldDelegates::OnWorldCleanup.AddRaw(this, &FAnomalyBenchModule::WorldCleanup);
	}
	void ShutdownModule() override
	{
		FWorldDelegates::OnWorldPostActorTick.Remove(TickHandle);
		FWorldDelegates::OnWorldCleanup.Remove(CleanupHandle);
		ReleaseInput(TEXT("module_shutdown")); Place.Reset(); Lock.Reset();
	}
	void InputLock(const TArray<FString>& Args, UWorld* World)
	{
		auto* Cap = World ? World->GetSubsystem<UAnomalyCaptureSubsystem>() : nullptr;
		auto* PC = World ? World->GetFirstPlayerController() : nullptr;
		if (!IsFixture(World) || !Cap || Cap->IsCaptureActive() || !PC || !PC->GetPawn() ||
			Args.Num() != 1 || (Args[0] != TEXT("0") && Args[0] != TEXT("1")))
		{
			UE_LOG(LogTemp, Warning, TEXT("IAI-INPUT-LOCK REFUSED fixture/map/ready-controller/inactive-capture/argument gate")); FlushLog(); return;
		}
		if (Args[0] == TEXT("0"))
		{
			if (bOwnInputLock && LockedWorld.Get() != World)
			{
				UE_LOG(LogTemp, Warning, TEXT("IAI-INPUT-LOCK REFUSED different world")); FlushLog(); return;
			}
			ReleaseInput(TEXT("command")); return;
		}
		if (bOwnInputLock)
		{
			UE_LOG(LogTemp, Log, TEXT("IAI-INPUT-LOCK ALREADY owned=%d"), LockedController.Get() == PC && LockedWorld.Get() == World);
			FlushLog(); return;
		}
		const bool PriorLook = PC->IsLookInputIgnored(), PriorMove = PC->IsMoveInputIgnored();
		PC->SetIgnoreLookInput(true); PC->SetIgnoreMoveInput(true);
		bOwnInputLock = true; bSawCapture = false; LockedWorld = World; LockedController = PC; SessionAtLock = Cap->GetSessionId();
		UE_LOG(LogTemp, Log, TEXT("IAI-INPUT-LOCK ON controller=%s priorLook=%d priorMove=%d lookIgnored=%d moveIgnored=%d gfc=%llu"),
			*PC->GetName(), PriorLook, PriorMove, PC->IsLookInputIgnored(), PC->IsMoveInputIgnored(), GFrameCounter);
		FlushLog();
	}
	void PlaceView(const TArray<FString>& Args, UWorld* World)
	{
		if (!IsFixture(World) || PlacedWorld.Get() == World || Args.Num() != 1)
		{
			UE_LOG(LogTemp, Warning, TEXT("IAI-L2 REFUSED fixture/map/one-shot/request gate")); FlushLog(); return;
		}
		auto* Cap = World->GetSubsystem<UAnomalyCaptureSubsystem>();
		auto* PC = World->GetFirstPlayerController();
		auto* Pawn = PC ? PC->GetPawn() : nullptr;
		FAnomalyViewInfo View;
		if (!Cap || Cap->IsCaptureActive() || !Pawn || !AnomalyViewport::GetActiveViewInfo(World, View))
		{
			UE_LOG(LogTemp, Warning, TEXT("IAI-L2 REFUSED active capture or no possessed view")); FlushLog(); return;
		}
		const bool bStack = World->GetMapName().Contains(TEXT("CB_GateLevel"));
		if (bStack && (!bOwnInputLock || LockedWorld.Get() != World || LockedController.Get() != PC))
		{
			UE_LOG(LogTemp, Warning, TEXT("IAI-L2 REFUSED CB placement requires input lock before settle")); FlushLog(); return;
		}
		// CB calibration: a54_oracle.py CALIB_BBOX and 081-09 R1V3_A1's independent view snapshot.
		const FVector Desired = bStack ? FVector(-1500, 0, 260) : FVector(-443.9807434082031, -70.0, 212.00010681152344);
		const FRotator Rotation = bStack ? FRotator::ZeroRotator : FRotator(0, 0, -0.08256798918660047);
		const FVector OldPawn = Pawn->GetActorLocation();
		// Translate the pawn by the camera-origin error after rotating its measured camera offset.
		// No replacement camera, ghost mode, velocity reset or persistent pose pin. Input lock is separate.
		const FVector LocalOffset = PC->GetControlRotation().Quaternion().UnrotateVector(View.Origin - OldPawn);
		const FVector NewPawn = Desired - Rotation.Quaternion().RotateVector(LocalOffset);
		PC->SetControlRotation(Rotation);
		const bool Moved = Pawn->SetActorLocation(NewPawn, false, nullptr, ETeleportType::TeleportPhysics);
		PlacedWorld = World; // A failed one-shot is still an attempted placement, never secretly retried.
		UE_LOG(LogTemp, Log, TEXT("IAI-L2 PLACED request=%s ok=%d pawn=%s oldPawn=%s newPawn=%s oldCamera=%s desiredCamera=%s desiredRot=%s gfc=%llu"),
			*Args[0], Moved, *Pawn->GetName(), *OldPawn.ToString(), *NewPawn.ToString(), *View.Origin.ToString(), *Desired.ToString(), *Rotation.ToString(), GFrameCounter);
		FlushLog();
	}
};
IMPLEMENT_MODULE(FAnomalyBenchModule, AnomalyBench)
