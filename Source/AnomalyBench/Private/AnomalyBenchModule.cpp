#include "Modules/ModuleManager.h"
#include "AnomalyCaptureSubsystem.h"
#include "AnomalyViewport.h"
#include "Camera/PlayerCameraManager.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpectatorPawn.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"
#include "EngineUtils.h"
#include "HAL/IConsoleManager.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"

class FAnomalyBenchModule final : public IModuleInterface
{
	TUniquePtr<FAutoConsoleCommandWithWorldAndArgs> Place;
	TUniquePtr<FAutoConsoleCommandWithWorldAndArgs> Lock;
	TUniquePtr<FAutoConsoleCommandWithWorldAndArgs> SceneFixtureCommand;
	TWeakObjectPtr<AStaticMeshActor> Occluder;
	TWeakObjectPtr<UStaticMeshComponent> OcclusionTarget;
	bool bMotionArmed = false;
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
			PC->SetIgnoreLookInput(false); PC->SetIgnoreMoveInput(false);
		}
		UE_LOG(LogTemp, Log, TEXT("IAI-INPUT-LOCK OFF reason=%s controller=%s lookIgnored=%d moveIgnored=%d gfc=%llu"),
			Reason, *GetNameSafe(PC), PC && PC->IsLookInputIgnored(), PC && PC->IsMoveInputIgnored(), GFrameCounter);
		bOwnInputLock = false; bSawCapture = false; LockedController.Reset(); LockedWorld.Reset(); SessionAtLock.Reset();
		FlushLog();
	}
	void WorldTick(UWorld* World, ELevelTick, float DeltaSeconds)
	{
		if (!bOwnInputLock || LockedWorld.Get() != World) { return; }
		if (!LockedController.IsValid() || World->GetFirstPlayerController() != LockedController.Get())
		{
			ReleaseInput(TEXT("controller_changed")); return;
		}
		auto* Cap = World->GetSubsystem<UAnomalyCaptureSubsystem>();
		if (!Cap) { ReleaseInput(TEXT("capture_subsystem_lost")); return; }
		if (Cap->IsCaptureActive())
		{
			bSawCapture = true;
			if (bMotionArmed)
			{
				auto* PC = LockedController.Get();
				FRotator Rotation = PC->GetControlRotation(); Rotation.Yaw -= 3.0f * DeltaSeconds;
				PC->SetControlRotation(Rotation);
			}
			if (Occluder.IsValid() && OcclusionTarget.IsValid())
			{
				FAnomalyViewInfo View;
				if (AnomalyViewport::GetActiveViewInfo(World, View))
				{
					FHitResult Hit; FCollisionQueryParams Params(SCENE_QUERY_STAT(IAIBenchOcclusion), true);
					Params.AddIgnoredActor(LockedController->GetViewTarget());
					World->LineTraceSingleByChannel(Hit, View.Origin, OcclusionTarget->Bounds.Origin, ECC_Visibility, Params);
					UE_LOG(LogTemp, Log, TEXT("IAI-OCCLUSION TRACE gfc=%llu hit=%s occluder_hit=%d"),
						GFrameCounter, *ReadyName(Hit.GetActor()), Hit.GetActor() == Occluder.Get());
				}
			}
		}
		else if (bSawCapture || Cap->GetSessionId() != SessionAtLock)
		{
			ReleaseInput(TEXT("run_end"));
			bMotionArmed = false;
		}
	}
	void WorldCleanup(UWorld* World, bool, bool)
	{
		if (LockedWorld.Get() == World) { ReleaseInput(TEXT("world_cleanup")); }
		if (PlacedWorld.Get() == World) { PlacedWorld.Reset(); }
		if (Occluder.IsValid() && Occluder->GetWorld() == World) { Occluder.Reset(); OcclusionTarget.Reset(); }
		bMotionArmed = false;
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
		SceneFixtureCommand = MakeUnique<FAutoConsoleCommandWithWorldAndArgs>(TEXT("IAI.Bench.SceneFixture"),
			TEXT("Explicit CB fixture: occluder duplicates the loaded target mesh; motion arms capture-only -3deg/sec yaw."),
			FConsoleCommandWithWorldAndArgsDelegate::CreateRaw(this, &FAnomalyBenchModule::SceneFixture));
		TickHandle = FWorldDelegates::OnWorldPostActorTick.AddRaw(this, &FAnomalyBenchModule::WorldTick);
		CleanupHandle = FWorldDelegates::OnWorldCleanup.AddRaw(this, &FAnomalyBenchModule::WorldCleanup);
	}
	void ShutdownModule() override
	{
		FWorldDelegates::OnWorldPostActorTick.Remove(TickHandle);
		FWorldDelegates::OnWorldCleanup.Remove(CleanupHandle);
		ReleaseInput(TEXT("module_shutdown")); Place.Reset(); Lock.Reset(); SceneFixtureCommand.Reset();
		if (Occluder.IsValid()) { Occluder->Destroy(); }
		Occluder.Reset(); OcclusionTarget.Reset(); bMotionArmed = false;
	}
	void SceneFixture(const TArray<FString>& Args, UWorld* World)
	{
		auto* Cap = World ? World->GetSubsystem<UAnomalyCaptureSubsystem>() : nullptr;
		auto* PC = World ? World->GetFirstPlayerController() : nullptr;
		LogReadiness(World, Cap, PC);
		if (const TCHAR* Failure = FixtureFailure(World, Cap, PC)) { Refuse(TEXT("IAI-SCENE"), Failure); return; }
		if (!World->GetMapName().Contains(TEXT("CB_GateLevel"))) { Refuse(TEXT("IAI-SCENE"), TEXT("cb_only")); return; }
		if (!bOwnInputLock || LockedController.Get() != PC || PlacedWorld.Get() != World) { Refuse(TEXT("IAI-SCENE"), TEXT("locked_placement_required")); return; }
		if (Args.Num() != 1 || (Args[0] != TEXT("motion") && Args[0] != TEXT("occluder"))) { Refuse(TEXT("IAI-SCENE"), TEXT("invalid_mode")); return; }
		if (bMotionArmed || Occluder.IsValid()) { Refuse(TEXT("IAI-SCENE"), TEXT("already_configured")); return; }
		if (Args[0] == TEXT("motion"))
		{
			bMotionArmed = true;
			UE_LOG(LogTemp, Log, TEXT("IAI-SCENE MOTION armed=1 yaw_rate=-3 capture_only=1 gfc=%llu"), GFrameCounter); FlushLog(); return;
		}
		UStaticMeshComponent* Target = nullptr;
		for (TActorIterator<AStaticMeshActor> It(World); It; ++It)
		{
			if (It->GetName() == TEXT("StaticMeshActor_49")) { Target = It->GetStaticMeshComponent(); break; }
		}
		FAnomalyViewInfo View;
		if (!Target || !Target->GetStaticMesh()) { Refuse(TEXT("IAI-SCENE"), TEXT("no_loaded_target_mesh")); return; }
		if (!AnomalyViewport::GetActiveViewInfo(World, View)) { Refuse(TEXT("IAI-SCENE"), TEXT("no_active_view")); return; }
		FActorSpawnParameters Spawn; Spawn.Name = TEXT("IAIBenchOccluder"); Spawn.ObjectFlags |= RF_Transient;
		Spawn.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		auto* Actor = World->SpawnActor<AStaticMeshActor>(Spawn);
		if (!Actor) { Refuse(TEXT("IAI-SCENE"), TEXT("spawn_failed")); return; }
		auto* Mesh = Actor->GetStaticMeshComponent(); Mesh->SetMobility(EComponentMobility::Movable);
		Mesh->SetStaticMesh(Target->GetStaticMesh());
		for (int32 I = 0; I < Target->GetNumMaterials(); ++I) { Mesh->SetMaterial(I, Target->GetMaterial(I)); }
		const FVector Scale = Target->GetComponentScale() * 0.75;
		const FQuat Rotation = Target->GetComponentQuat();
		const FVector Center = (View.Origin + Target->Bounds.Origin) * 0.5;
		Actor->SetActorTransform(FTransform(Rotation, Center - Rotation.RotateVector(Scale * Target->GetStaticMesh()->GetBounds().Origin), Scale));
		Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly); Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		Occluder = Actor; OcclusionTarget = Target;
		UE_LOG(LogTemp, Log, TEXT("IAI-SCENE OCCLUDER actor=%s source=%s asset=%s center=%s scale=%s gfc=%llu"),
			*Actor->GetName(), *Target->GetOwner()->GetName(), *Target->GetStaticMesh()->GetPathName(), *Center.ToString(), *Scale.ToString(), GFrameCounter);
		FlushLog();
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
		const FVector Desired = bStack ? FVector(-1500, 0, 260) : FVector(-443.9807434082031, -70.0, 212.00010681152344);
		const FRotator Rotation = bStack ? FRotator::ZeroRotator : FRotator(0, 0, -0.08256798918660047);
		const FVector OldOwner = Owner->GetActorLocation();
		const FVector LocalOffset = PC->GetControlRotation().Quaternion().UnrotateVector(View.Origin - OldOwner);
		const FVector NewOwner = Desired - Rotation.Quaternion().RotateVector(LocalOffset);
		PC->SetControlRotation(Rotation);
		const bool Moved = Owner->SetActorLocation(NewOwner, false, nullptr, ETeleportType::TeleportPhysics);
		PlacedWorld = World;
		UE_LOG(LogTemp, Log, TEXT("IAI-L2 PLACED request=%s ok=%d owner=%s oldOwner=%s newOwner=%s oldCamera=%s desiredCamera=%s desiredRot=%s gfc=%llu"),
			*Args[0], Moved, *Owner->GetName(), *OldOwner.ToString(), *NewOwner.ToString(), *View.Origin.ToString(), *Desired.ToString(), *Rotation.ToString(), GFrameCounter);
		FlushLog();
	}
};
IMPLEMENT_MODULE(FAnomalyBenchModule, AnomalyBench)
