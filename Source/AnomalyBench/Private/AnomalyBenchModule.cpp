#include "Modules/ModuleManager.h"
#include "AnomalyCaptureSubsystem.h"
#include "AnomalyViewport.h"
#include "Camera/PlayerCameraManager.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpectatorPawn.h"
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
	static const TCHAR* FixtureFailure(UWorld* World, UAnomalyCaptureSubsystem* Cap, APlayerController* PC)
	{
		if (!World) { return TEXT("no_world"); }
		if (!FParse::Param(FCommandLine::Get(), TEXT("IAIBenchFixture"))) { return TEXT("fixture_flag_missing"); }
		if (!World->GetMapName().Contains(TEXT("CB_GateLevel")) && !World->GetMapName().Contains(TEXT("L_ShooterGym"))) { return TEXT("map_not_allowed"); }
		if (!Cap) { return TEXT("no_capture_subsystem"); }
		if (Cap->IsCaptureActive()) { return TEXT("capture_active"); }
		if (!PC) { return TEXT("no_controller"); }
		return nullptr;
	}
	static FString ReadyName(const UObject* Object) { return Object ? Object->GetName() : TEXT("null"); }
	static void LogReadiness(UWorld* World, UAnomalyCaptureSubsystem* Cap, APlayerController* PC)
	{
		UE_LOG(LogTemp, Log, TEXT("IAI-BENCH READY pc=%s pawn=%s spectator=%s viewtarget=%s capture_active=%d map=%s fixture=%d"),
			*ReadyName(PC), *ReadyName(PC ? PC->GetPawn() : nullptr), *ReadyName(PC ? PC->GetSpectatorPawn() : nullptr),
			*ReadyName(PC ? PC->GetViewTarget() : nullptr), Cap && Cap->IsCaptureActive(),
			World ? *World->GetMapName() : TEXT("null"), FParse::Param(FCommandLine::Get(), TEXT("IAIBenchFixture")));
	}
	static void Refuse(const TCHAR* Command, const TCHAR* Reason)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s REFUSED reason=%s"), Command, Reason); FlushLog();
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
		LogReadiness(World, Cap, PC);
		if (const TCHAR* Failure = FixtureFailure(World, Cap, PC))
		{
			Refuse(TEXT("IAI-INPUT-LOCK"), Failure); return;
		}
		if (Args.Num() != 1 || (Args[0] != TEXT("0") && Args[0] != TEXT("1")))
		{
			Refuse(TEXT("IAI-INPUT-LOCK"), TEXT("invalid_argument")); return;
		}
		if (Args[0] == TEXT("0"))
		{
			if (bOwnInputLock && LockedWorld.Get() != World)
			{
				Refuse(TEXT("IAI-INPUT-LOCK"), TEXT("different_world")); return;
			}
			ReleaseInput(TEXT("command")); return;
		}
		if (bOwnInputLock)
		{
			if (LockedController.Get() != PC || LockedWorld.Get() != World)
			{
				Refuse(TEXT("IAI-INPUT-LOCK"), TEXT("different_lock_owner")); return;
			}
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
		auto* Cap = World ? World->GetSubsystem<UAnomalyCaptureSubsystem>() : nullptr;
		auto* PC = World ? World->GetFirstPlayerController() : nullptr;
		LogReadiness(World, Cap, PC);
		if (const TCHAR* Failure = FixtureFailure(World, Cap, PC))
		{
			Refuse(TEXT("IAI-L2"), Failure); return;
		}
		if (PlacedWorld.Get() == World) { Refuse(TEXT("IAI-L2"), TEXT("already_placed")); return; }
		if (Args.Num() != 1) { Refuse(TEXT("IAI-L2"), TEXT("invalid_request")); return; }
		// Spectators derive from APawn. Prefer the actual view target before the controller's fallback.
		APawn* Owner = Cast<APawn>(PC->GetViewTarget());
		if (!Owner) { Owner = PC->GetPawnOrSpectator(); }
		if (!Owner) { Refuse(TEXT("IAI-L2"), TEXT("no_view_owner")); return; }
		FAnomalyViewInfo View;
		if (!AnomalyViewport::GetActiveViewInfo(World, View))
		{
			Refuse(TEXT("IAI-L2"), TEXT("no_active_view")); return;
		}
		const bool bStack = World->GetMapName().Contains(TEXT("CB_GateLevel"));
		if (bStack && (!bOwnInputLock || LockedWorld.Get() != World || LockedController.Get() != PC))
		{
			Refuse(TEXT("IAI-L2"), TEXT("input_lock_required")); return;
		}
		// CB calibration: a54_oracle.py CALIB_BBOX and 081-09 R1V3_A1's independent view snapshot.
		const FVector Desired = bStack ? FVector(-1500, 0, 260) : FVector(-443.9807434082031, -70.0, 212.00010681152344);
		const FRotator Rotation = bStack ? FRotator::ZeroRotator : FRotator(0, 0, -0.08256798918660047);
		const FVector OldOwner = Owner->GetActorLocation();
		// Translate the resolved owner by the camera-origin error after rotating its measured offset.
		// No replacement camera, ghost mode, velocity reset or persistent pose pin. Input lock is separate.
		const FVector LocalOffset = PC->GetControlRotation().Quaternion().UnrotateVector(View.Origin - OldOwner);
		const FVector NewOwner = Desired - Rotation.Quaternion().RotateVector(LocalOffset);
		PC->SetControlRotation(Rotation);
		const bool Moved = Owner->SetActorLocation(NewOwner, false, nullptr, ETeleportType::TeleportPhysics);
		PlacedWorld = World; // A failed one-shot is still an attempted placement, never secretly retried.
		UE_LOG(LogTemp, Log, TEXT("IAI-L2 PLACED request=%s ok=%d owner=%s oldOwner=%s newOwner=%s oldCamera=%s desiredCamera=%s desiredRot=%s gfc=%llu"),
			*Args[0], Moved, *Owner->GetName(), *OldOwner.ToString(), *NewOwner.ToString(), *View.Origin.ToString(), *Desired.ToString(), *Rotation.ToString(), GFrameCounter);
		FlushLog();
	}
};
IMPLEMENT_MODULE(FAnomalyBenchModule, AnomalyBench)
