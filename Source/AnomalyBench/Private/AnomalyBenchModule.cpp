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
#include "AnomalyBenchGate.h"

namespace
{
	struct FCcPose
	{
		FVector Location = FVector::ZeroVector;
		float Yaw = 0.0f;
		const TCHAR* Segment = TEXT("");
		int32 Designed = 0;
	};

	static constexpr int32 CcTotalFrames = 200;
	static constexpr double CcWallFaceX = -1400.0;
	static constexpr double CcEyeZ = 260.0;

	FCcPose CcWallPose(double Distance, const TCHAR* Segment)
	{
		FCcPose P;
		P.Location = FVector(CcWallFaceX - Distance, 0.0, CcEyeZ);
		P.Yaw = 0.0f;
		P.Segment = Segment;
		P.Designed = Distance < 100.0 ? 1 : 0;
		return P;
	}

	FCcPose CcFixedPose(const FVector& Location, const TCHAR* Segment, int32 Designed)
	{
		FCcPose P;
		P.Location = Location;
		P.Yaw = 180.0f;
		P.Segment = Segment;
		P.Designed = Designed;
		return P;
	}

	FCcPose CcPoseAt(int32 FrameIndex, bool bPawnMesh)
	{
		int32 I = FMath::Clamp(FrameIndex, 0, CcTotalFrames - 1);
		const FVector Control(-1700.0, 0.0, CcEyeZ);
		if (I < 30) { return CcFixedPose(Control, TEXT("control_start"), 0); }
		I -= 30;
		if (I < 10) { return CcWallPose(150.0, TEXT("wall_far")); }
		I -= 10;
		if (I < 20) { return CcWallPose(147.5 - 5.0 * I, TEXT("wall_in")); }
		I -= 20;
		if (I < 10) { return CcWallPose(50.0, TEXT("wall_near")); }
		I -= 10;
		if (I < 20) { return CcWallPose(52.5 + 5.0 * I, TEXT("wall_out")); }
		I -= 20;
		if (I < 10) { return CcWallPose(150.0, TEXT("wall_far_2")); }
		I -= 10;
		if (I < 20) { return CcFixedPose(FVector(CcWallFaceX - 50.0, 0.0, CcEyeZ), TEXT("behind"), 0); }
		I -= 20;
		if (I < 20) { return CcFixedPose(FVector(-1700.0, -300.0, CcEyeZ), TEXT("beside"), 0); }
		I -= 20;
		if (I < 20) { return CcFixedPose(FVector(-1700.0, 500.0, CcEyeZ), TEXT("prop_noncolliding"), 1); }
		I -= 20;
		if (I < 20) { return CcFixedPose(FVector(-1700.0, -700.0, CcEyeZ), TEXT("pawn_mesh"), bPawnMesh ? 1 : 0); }
		return CcFixedPose(Control, TEXT("control_end"), 0);
	}
}

class FAnomalyBenchModule final : public IModuleInterface
{
	TUniquePtr<FAutoConsoleCommandWithWorldAndArgs> Place;
	TUniquePtr<FAutoConsoleCommandWithWorldAndArgs> Lock;
	TUniquePtr<FAutoConsoleCommandWithWorldAndArgs> SceneFixtureCommand;
	TUniquePtr<FAutoConsoleCommandWithWorldAndArgs> ScheduleCommand;
	bool bScheduleArmed = false;
	bool bSchedulePawnMesh = false;
	FVector ScheduleLocalOffset = FVector::ZeroVector;
	TWeakObjectPtr<UWorld> ScheduleWorld;
	TArray<TWeakObjectPtr<AActor>> ScheduleActors;
	TWeakObjectPtr<UStaticMeshComponent> SchedulePawnProbe;
	int32 SchedulePreIndex = -1;
	int32 ScheduleLastLogged = -1;
	FDelegateHandle PreTickHandle;
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
	void DisarmSchedule(const TCHAR* Reason)
	{
		if (!bScheduleArmed && ScheduleActors.Num() == 0 && !SchedulePawnProbe.IsValid()) { return; }
		for (const TWeakObjectPtr<AActor>& Weak : ScheduleActors)
		{
			if (AActor* Actor = Weak.Get()) { Actor->Destroy(); }
		}
		ScheduleActors.Reset();
		if (UStaticMeshComponent* Probe = SchedulePawnProbe.Get()) { Probe->DestroyComponent(); }
		SchedulePawnProbe.Reset();
		UE_LOG(LogTemp, Log, TEXT("IAI-CC-SCHEDULE DISARMED reason=%s last_logged_k=%d gfc=%llu"), Reason, ScheduleLastLogged, GFrameCounter);
		bScheduleArmed = false; bSchedulePawnMesh = false; ScheduleWorld.Reset(); SchedulePreIndex = -1; ScheduleLastLogged = -1;
		FlushLog();
	}
	void ApplySchedulePose(APlayerController* PC, int32 FrameIndex)
	{
		APawn* Owner = Cast<APawn>(PC->GetViewTarget());
		if (!Owner) { Owner = PC->GetPawnOrSpectator(); }
		if (!Owner) { return; }
		const FCcPose Pose = CcPoseAt(FrameIndex, bSchedulePawnMesh);
		const FRotator Rotation(0.0, Pose.Yaw, 0.0);
		PC->SetControlRotation(Rotation);
		Owner->SetActorLocation(Pose.Location - Rotation.Quaternion().RotateVector(ScheduleLocalOffset), false, nullptr,
			ETeleportType::TeleportPhysics);
	}
	void LogSchedulePose(APlayerController* PC, int32 FrameIndex)
	{
		const FCcPose Pose = CcPoseAt(FrameIndex, bSchedulePawnMesh);
		FVector Got = FVector::ZeroVector;
		FRotator GotRot = FRotator::ZeroRotator;
		if (PC->PlayerCameraManager)
		{
			const FMinimalViewInfo& Pov = PC->PlayerCameraManager->GetCameraCacheView();
			Got = Pov.Location;
			GotRot = Pov.Rotation;
		}
		const double ErrCm = FVector::Dist(Got, Pose.Location);
		const double ErrDeg = FMath::Abs(FRotator::NormalizeAxis(GotRot.Yaw - Pose.Yaw)) + FMath::Abs(FRotator::NormalizeAxis(GotRot.Pitch))
			+ FMath::Abs(FRotator::NormalizeAxis(GotRot.Roll));
		UE_LOG(LogTemp, Log,
			TEXT("IAI-CC-SCHEDULE FRAME k=%d seg=%s designed=%d pawn_mesh=%d want=(%.3f,%.3f,%.3f,yaw %.3f) got=(%.3f,%.3f,%.3f,yaw %.3f) err_cm=%.4f err_deg=%.4f exhausted=%d gfc=%llu"),
			FrameIndex, Pose.Segment, Pose.Designed, bSchedulePawnMesh, Pose.Location.X, Pose.Location.Y, Pose.Location.Z, Pose.Yaw,
			Got.X, Got.Y, Got.Z, GotRot.Yaw, ErrCm, ErrDeg, FrameIndex >= CcTotalFrames, GFrameCounter);
	}
	void WorldPreTick(UWorld* World, ELevelTick, float)
	{
		if (!bScheduleArmed || ScheduleWorld.Get() != World || !bOwnInputLock || LockedWorld.Get() != World) { return; }
		auto* PC = LockedController.Get();
		auto* Cap = World->GetSubsystem<UAnomalyCaptureSubsystem>();
		if (!PC || !Cap) { return; }
		SchedulePreIndex = Cap->IsCaptureActive() ? Cap->GetSessionFrameIndex() : 0;
		ApplySchedulePose(PC, SchedulePreIndex);
	}
	void ReleaseInput(const TCHAR* Reason)
	{
		DisarmSchedule(Reason);
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
			if (bScheduleArmed && ScheduleWorld.Get() == World && SchedulePreIndex >= 0
				&& Cap->GetSessionFrameIndex() > SchedulePreIndex && SchedulePreIndex != ScheduleLastLogged)
			{
				LogSchedulePose(LockedController.Get(), SchedulePreIndex);
				ScheduleLastLogged = SchedulePreIndex;
			}
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
		if (ScheduleWorld.Get() == World) { DisarmSchedule(TEXT("world_cleanup")); }
		if (LockedWorld.Get() == World) { ReleaseInput(TEXT("world_cleanup")); }
		if (PlacedWorld.Get() == World) { PlacedWorld.Reset(); }
		if (Occluder.IsValid() && Occluder->GetWorld() == World) { Occluder.Reset(); OcclusionTarget.Reset(); }
		bMotionArmed = false;
	}
public:
	void StartupModule() override
	{
		if (AnomalyBenchGate::IsEnabled())
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
			ScheduleCommand = MakeUnique<FAutoConsoleCommandWithWorldAndArgs>(TEXT("IAI.Bench.CameraSchedule"),
				TEXT("BENCH DEVICE (bench gate only: -IAIBench or -IAIBenchFixture; compiled out of Shipping). cc_v1 arms the B-CC camera_clipping pose schedule on CB_GateLevel: ")
				TEXT("spawns a wall, a colliding prop beside the lens path, a non-colliding prop and (if a pawn is possessed) a pawn-owned mesh, then ")
				TEXT("drives the view through 200 poses keyed by the capture session frame index. off disarms outside capture. Requires InputLock."),
				FConsoleCommandWithWorldAndArgsDelegate::CreateRaw(this, &FAnomalyBenchModule::CameraSchedule));
		}
		TickHandle = FWorldDelegates::OnWorldPostActorTick.AddRaw(this, &FAnomalyBenchModule::WorldTick);
		PreTickHandle = FWorldDelegates::OnWorldPreActorTick.AddRaw(this, &FAnomalyBenchModule::WorldPreTick);
		CleanupHandle = FWorldDelegates::OnWorldCleanup.AddRaw(this, &FAnomalyBenchModule::WorldCleanup);
	}
	void ShutdownModule() override
	{
		FWorldDelegates::OnWorldPostActorTick.Remove(TickHandle);
		FWorldDelegates::OnWorldPreActorTick.Remove(PreTickHandle);
		FWorldDelegates::OnWorldCleanup.Remove(CleanupHandle);
		ReleaseInput(TEXT("module_shutdown")); Place.Reset(); Lock.Reset(); SceneFixtureCommand.Reset(); ScheduleCommand.Reset();
		if (Occluder.IsValid()) { Occluder->Destroy(); }
		Occluder.Reset(); OcclusionTarget.Reset(); bMotionArmed = false;
	}
	static void ApplyFixtureCollision(UStaticMeshComponent* Mesh, bool bCollide)
	{
		if (bCollide)
		{
			Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
			Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		}
		else
		{
			Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
	AActor* SpawnScheduleCube(UWorld* World, const TCHAR* Name, const FVector& Center, const FVector& Scale,
		UStaticMeshComponent* Source, bool bCollide)
	{
		FActorSpawnParameters Spawn; Spawn.Name = FName(Name); Spawn.ObjectFlags |= RF_Transient;
		Spawn.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		auto* Actor = World->SpawnActor<AStaticMeshActor>(Spawn);
		if (!Actor) { return nullptr; }
		auto* Mesh = Actor->GetStaticMeshComponent(); Mesh->SetMobility(EComponentMobility::Movable);
		Mesh->SetStaticMesh(Source->GetStaticMesh());
		for (int32 I = 0; I < Source->GetNumMaterials(); ++I) { Mesh->SetMaterial(I, Source->GetMaterial(I)); }
		ApplyFixtureCollision(Mesh, bCollide);
		Actor->SetActorTransform(FTransform(FQuat::Identity, Center - Scale * Source->GetStaticMesh()->GetBounds().Origin, Scale));
		ScheduleActors.Add(Actor);
		UE_LOG(LogTemp, Log, TEXT("IAI-CC-SCHEDULE FIXTURE actor=%s center=%s scale=%s collide=%d bounds=%s"),
			*Actor->GetName(), *Center.ToString(), *Scale.ToString(), bCollide, *Mesh->Bounds.GetBox().ToString());
		return Actor;
	}
	void CameraSchedule(const TArray<FString>& Args, UWorld* World)
	{
		auto* Cap = World ? World->GetSubsystem<UAnomalyCaptureSubsystem>() : nullptr;
		auto* PC = World ? World->GetFirstPlayerController() : nullptr;
		LogReadiness(World, Cap, PC);
		if (Args.Num() == 1 && Args[0] == TEXT("off"))
		{
			if (Cap && Cap->IsCaptureActive()) { Refuse(TEXT("IAI-CC-SCHEDULE"), TEXT("capture_active")); return; }
			DisarmSchedule(TEXT("command")); return;
		}
		if (const TCHAR* Failure = FixtureFailure(World, Cap, PC)) { Refuse(TEXT("IAI-CC-SCHEDULE"), Failure); return; }
		if (!World->GetMapName().Contains(TEXT("CB_GateLevel"))) { Refuse(TEXT("IAI-CC-SCHEDULE"), TEXT("cb_only")); return; }
		if (!bOwnInputLock || LockedController.Get() != PC || LockedWorld.Get() != World) { Refuse(TEXT("IAI-CC-SCHEDULE"), TEXT("input_lock_required")); return; }
		if (Args.Num() != 1 || Args[0] != TEXT("cc_v1")) { Refuse(TEXT("IAI-CC-SCHEDULE"), TEXT("invalid_mode")); return; }
		if (bScheduleArmed || ScheduleActors.Num() > 0) { Refuse(TEXT("IAI-CC-SCHEDULE"), TEXT("already_armed")); return; }
		if (bMotionArmed || Occluder.IsValid()) { Refuse(TEXT("IAI-CC-SCHEDULE"), TEXT("scene_fixture_configured")); return; }
		UStaticMeshComponent* Source = nullptr;
		for (TActorIterator<AStaticMeshActor> It(World); It; ++It)
		{
			if (It->GetName() == TEXT("StaticMeshActor_49")) { Source = It->GetStaticMeshComponent(); break; }
		}
		if (!Source || !Source->GetStaticMesh()) { Refuse(TEXT("IAI-CC-SCHEDULE"), TEXT("no_loaded_source_mesh")); return; }
		APawn* Owner = Cast<APawn>(PC->GetViewTarget());
		if (!Owner) { Owner = PC->GetPawnOrSpectator(); }
		if (!Owner) { Refuse(TEXT("IAI-CC-SCHEDULE"), TEXT("no_view_owner")); return; }
		FAnomalyViewInfo View;
		if (!AnomalyViewport::GetActiveViewInfo(World, View)) { Refuse(TEXT("IAI-CC-SCHEDULE"), TEXT("no_active_view")); return; }
		ScheduleLocalOffset = PC->GetControlRotation().Quaternion().UnrotateVector(View.Origin - Owner->GetActorLocation());
		ScheduleWorld = World;
		SpawnScheduleCube(World, TEXT("IAIBenchCcWall"), FVector(CcWallFaceX + 5.0, 0.0, CcEyeZ), FVector(0.1, 12.0, 8.0), Source, true);
		SpawnScheduleCube(World, TEXT("IAIBenchCcBeside"), FVector(-1700.0, -370.0, CcEyeZ), FVector(0.2, 0.2, 0.2), Source, true);
		SpawnScheduleCube(World, TEXT("IAIBenchCcProp"), FVector(-1750.0, 500.0, CcEyeZ), FVector(0.2, 0.2, 0.2), Source, false);
		if (APawn* Possessed = PC->GetPawn())
		{
			UStaticMeshComponent* Probe = NewObject<UStaticMeshComponent>(Possessed, TEXT("IAIBenchCcPawnProbe"), RF_Transient);
			Probe->SetMobility(EComponentMobility::Movable);
			Probe->SetUsingAbsoluteLocation(true); Probe->SetUsingAbsoluteRotation(true); Probe->SetUsingAbsoluteScale(true);
			Probe->SetStaticMesh(Source->GetStaticMesh());
			for (int32 I = 0; I < Source->GetNumMaterials(); ++I) { Probe->SetMaterial(I, Source->GetMaterial(I)); }
			ApplyFixtureCollision(Probe, true);
			Probe->RegisterComponent();
			Possessed->AddInstanceComponent(Probe);
			const FVector Scale(0.2, 0.2, 0.2);
			Probe->SetWorldTransform(FTransform(FQuat::Identity, FVector(-1750.0, -700.0, CcEyeZ) - Scale * Source->GetStaticMesh()->GetBounds().Origin, Scale));
			SchedulePawnProbe = Probe;
			bSchedulePawnMesh = true;
		}
		bScheduleArmed = true; SchedulePreIndex = -1; ScheduleLastLogged = -1;
		UE_LOG(LogTemp, Log,
			TEXT("IAI-CC-SCHEDULE ARMED version=cc_v1 frames=%d wall_face_x=%.1f eye_z=%.1f pawn=%s pawn_mesh=%d view_owner=%s local_offset=%s gfc=%llu"),
			CcTotalFrames, CcWallFaceX, CcEyeZ, *ReadyName(PC->GetPawn()), bSchedulePawnMesh, *Owner->GetName(),
			*ScheduleLocalOffset.ToString(), GFrameCounter);
		for (int32 K = 0; K < CcTotalFrames; ++K)
		{
			const FCcPose Pose = CcPoseAt(K, bSchedulePawnMesh);
			if (K == 0 || FCString::Strcmp(Pose.Segment, CcPoseAt(K - 1, bSchedulePawnMesh).Segment) != 0)
			{
				UE_LOG(LogTemp, Log, TEXT("IAI-CC-SCHEDULE SEGMENT start_k=%d seg=%s"), K, Pose.Segment);
			}
		}
		ApplySchedulePose(PC, 0);
		FlushLog();
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
		if (bScheduleArmed) { Refuse(TEXT("IAI-SCENE"), TEXT("camera_schedule_armed")); return; }
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
