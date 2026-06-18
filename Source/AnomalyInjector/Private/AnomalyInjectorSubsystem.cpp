// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#include "AnomalyInjectorSubsystem.h"
#include "AnomalyInjectorLog.h"

#include "EngineUtils.h"            // TActorIterator
#include "Engine/Engine.h"         // GEngine
#include "Engine/World.h"          // UWorld
#include "GameFramework/Actor.h"
#include "HAL/IConsoleManager.h"   // FAutoConsoleCommandWithWorldAndArgs

#include "Anomalies/Anomaly_MissingObject.h"
#include "Anomalies/Anomaly_Flicker.h"
#include "Anomalies/Anomaly_TimeDilation.h"
#include "Anomalies/Anomaly_LightingMismatch.h"
#include "Anomalies/Anomaly_LodCorruption.h"
#include "Anomalies/Anomaly_LodPopping.h"
#include "Anomalies/Anomaly_CameraClipping.h"

/** Fixed on-screen-message key ("GDPH") so the heartbeat refreshes in place instead of stacking. */
static constexpr uint64 GAnomalyHeartbeatKey = 0x47445048;

// Out-of-line dtor defined where IAnomaly is a complete type (gotcha G9).
UAnomalyInjectorSubsystem::~UAnomalyInjectorSubsystem() = default;

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

void UAnomalyInjectorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Explicit registration — one instance per type, keyed by GetId(). No self-registration
	// macros (keeps it readable and free of static-init-order hazards).
	auto Register = [this](TUniquePtr<IAnomaly> Anomaly)
	{
		const FName Id = Anomaly->GetId();
		Anomalies.Add(Id, MoveTemp(Anomaly));
	};
	Register(MakeUnique<FAnomaly_MissingObject>());
	Register(MakeUnique<FAnomaly_Flicker>());
	Register(MakeUnique<FAnomaly_TimeDilation>());
	Register(MakeUnique<FAnomaly_LightingMismatch>());   // M2: component-targeting (A1) + per-target capture
	Register(MakeUnique<FAnomaly_LodCorruption>());      // M2/M3: component-targeting, static + skeletal (AnomalyLod)
	Register(MakeUnique<FAnomaly_LodPopping>());         // M3: ticking LOD pop (AnomalyLod), static + skeletal
	Register(MakeUnique<FAnomaly_CameraClipping>());     // M2: global near-clip capture/restore

	UE_LOG(LogAnomaly, Log, TEXT("Subsystem initialized for world '%s'. %d anomaly type(s) registered."),
		*GetNameSafe(GetWorld()), Anomalies.Num());
}

void UAnomalyInjectorSubsystem::Deinitialize()
{
	// Generalized auto-restore-on-teardown: revert anything still active before the world ends.
	const int32 Reverted = RevertAllActive();
	if (Reverted > 0)
	{
		UE_LOG(LogAnomaly, Log, TEXT("Subsystem deinitializing; reverted %d active anomaly(ies)."), Reverted);
	}
	Super::Deinitialize();
}

bool UAnomalyInjectorSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	// Only real game worlds: standalone game and Play-In-Editor. Never the editor world.
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

TStatId UAnomalyInjectorSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UAnomalyInjectorSubsystem, STATGROUP_Tickables);
}

void UAnomalyInjectorSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Drive the active anomalies (static ones inherit a no-op Tick).
	for (const TPair<FName, TUniquePtr<IAnomaly>>& Pair : Anomalies)
	{
		if (Pair.Value && Pair.Value->IsActive())
		{
			Pair.Value->Tick(DeltaTime);
		}
	}

	// Heartbeat: proves the subsystem ticks in PIE. On-screen every ~2s plus a Verbose log,
	// now reporting the active-anomaly count.
	HeartbeatAccumulator += DeltaTime;
	if (HeartbeatAccumulator >= 2.0f)
	{
		HeartbeatAccumulator = 0.0f;
		const int32 ActiveCount = GetActiveAnomalyCount();
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				GAnomalyHeartbeatKey,
				2.5f,
				FColor::Green,
				FString::Printf(TEXT("[IAI] AnomalyInjector ticking (active: %d/%d)"), ActiveCount, Anomalies.Num()));
		}
		UE_LOG(LogAnomaly, Verbose, TEXT("Heartbeat; active anomalies: %d/%d"), ActiveCount, Anomalies.Num());
	}
}

// ---------------------------------------------------------------------------
// Targeting aid
// ---------------------------------------------------------------------------

void UAnomalyInjectorSubsystem::ListActors() const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	int32 Count = 0;
	UE_LOG(LogAnomaly, Log, TEXT("--- IAI.ListActors (world '%s') ---"), *GetNameSafe(World));
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (!Actor)
		{
			continue;
		}

		FString Label;
#if WITH_EDITOR
		Label = Actor->GetActorLabel();
#else
		Label = TEXT("(no-label)");
#endif
		UE_LOG(LogAnomaly, Log, TEXT("  %s | %s | %s"),
			*Actor->GetClass()->GetName(),
			*Actor->GetName(),
			*Label);
		++Count;
	}
	UE_LOG(LogAnomaly, Log, TEXT("--- %d actor(s) ---"), Count);
}

// ---------------------------------------------------------------------------
// Anomaly manager API
// ---------------------------------------------------------------------------

void UAnomalyInjectorSubsystem::ListAnomalies() const
{
	// Deterministic, sorted output (AMB-5). FName::operator< is index-order, not lexical,
	// so sort by the string form.
	TArray<FName> Ids;
	Anomalies.GetKeys(Ids);
	Ids.Sort([](const FName& A, const FName& B) { return A.ToString() < B.ToString(); });

	UE_LOG(LogAnomaly, Log, TEXT("--- IAI.ListAnomalies (%d) ---"), Ids.Num());
	for (const FName& Id : Ids)
	{
		const TUniquePtr<IAnomaly>& Anomaly = Anomalies[Id];
		UE_LOG(LogAnomaly, Log, TEXT("  %s - %s - %s"),
			*Anomaly->GetId().ToString(), *Anomaly->GetDescription(), *Anomaly->GetUsage());
	}
	UE_LOG(LogAnomaly, Log, TEXT("--- %d anomaly(ies) ---"), Ids.Num());
}

bool UAnomalyInjectorSubsystem::ApplyAnomaly(const FName& Id, const TArray<FString>& Args)
{
	TUniquePtr<IAnomaly>* Found = Anomalies.Find(Id);
	if (!Found || !Found->IsValid())
	{
		UE_LOG(LogAnomaly, Warning, TEXT("Unknown anomaly '%s'. Try IAI.ListAnomalies."), *Id.ToString());
		return false;
	}

	// Re-entrancy (revert-then-reapply) is handled inside each anomaly's Apply.
	const bool bApplied = (*Found)->Apply(GetWorld(), Args);
	UE_LOG(LogAnomaly, Log, TEXT("IAI.Apply '%s' -> %s."), *Id.ToString(), bApplied ? TEXT("applied") : TEXT("not applied"));
	return bApplied;
}

bool UAnomalyInjectorSubsystem::RevertAnomaly(const FName& Id)
{
	TUniquePtr<IAnomaly>* Found = Anomalies.Find(Id);
	if (!Found || !Found->IsValid())
	{
		UE_LOG(LogAnomaly, Warning, TEXT("Unknown anomaly '%s'. Try IAI.ListAnomalies."), *Id.ToString());
		return false;
	}
	if (!(*Found)->IsActive())
	{
		UE_LOG(LogAnomaly, Log, TEXT("IAI.Revert '%s': not active."), *Id.ToString());
		return false;
	}

	(*Found)->Revert();
	UE_LOG(LogAnomaly, Log, TEXT("IAI.Revert '%s' -> reverted."), *Id.ToString());
	return true;
}

int32 UAnomalyInjectorSubsystem::RevertAllActive()
{
	int32 Count = 0;
	for (const TPair<FName, TUniquePtr<IAnomaly>>& Pair : Anomalies)
	{
		if (Pair.Value && Pair.Value->IsActive())
		{
			Pair.Value->Revert();
			++Count;
		}
	}
	return Count;
}

int32 UAnomalyInjectorSubsystem::GetActiveAnomalyCount() const
{
	int32 Count = 0;
	for (const TPair<FName, TUniquePtr<IAnomaly>>& Pair : Anomalies)
	{
		if (Pair.Value && Pair.Value->IsActive())
		{
			++Count;
		}
	}
	return Count;
}

// ---------------------------------------------------------------------------
// Console command surface
//
// Module-scoped FAutoConsoleCommandWithWorldAndArgs objects: registered at module load,
// alive for the module's lifetime, decoupled from subsystem lifetime. Each resolves the
// subsystem from the world the console passes in, and null-guards gracefully when invoked
// outside a game world (the subsystem only exists in Game/PIE worlds).
// ---------------------------------------------------------------------------

namespace
{
	UAnomalyInjectorSubsystem* ResolveSubsystem(UWorld* World)
	{
		if (!World)
		{
			UE_LOG(LogAnomaly, Warning, TEXT("No world for this command; run it from a game/PIE world."));
			return nullptr;
		}

		UAnomalyInjectorSubsystem* Subsystem = World->GetSubsystem<UAnomalyInjectorSubsystem>();
		if (!Subsystem)
		{
			UE_LOG(LogAnomaly, Warning,
				TEXT("AnomalyInjector subsystem not present for world '%s' (only active in Game/PIE worlds)."),
				*GetNameSafe(World));
		}
		return Subsystem;
	}

	/** Build a copy of Args with the first element (the id) removed. */
	TArray<FString> TailArgs(const TArray<FString>& Args)
	{
		TArray<FString> Tail;
		if (Args.Num() > 1)
		{
			Tail.Append(Args.GetData() + 1, Args.Num() - 1);
		}
		return Tail;
	}
}

static FAutoConsoleCommandWithWorldAndArgs GListActorsCmd(
	TEXT("IAI.ListActors"),
	TEXT("List actors in the current world as 'Class | Name | Label'."),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (UAnomalyInjectorSubsystem* Subsystem = ResolveSubsystem(World))
			{
				Subsystem->ListActors();
			}
		}));

static FAutoConsoleCommandWithWorldAndArgs GListAnomaliesCmd(
	TEXT("IAI.ListAnomalies"),
	TEXT("List registered anomalies as 'id - description - usage'."),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (UAnomalyInjectorSubsystem* Subsystem = ResolveSubsystem(World))
			{
				Subsystem->ListAnomalies();
			}
		}));

static FAutoConsoleCommandWithWorldAndArgs GApplyCmd(
	TEXT("IAI.Apply"),
	TEXT("Apply an anomaly by id. Usage: IAI.Apply <id> <args...>  (see IAI.ListAnomalies)"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() == 0)
			{
				UE_LOG(LogAnomaly, Warning, TEXT("Usage: IAI.Apply <id> <args...>  (see IAI.ListAnomalies)"));
				return;
			}
			if (UAnomalyInjectorSubsystem* Subsystem = ResolveSubsystem(World))
			{
				Subsystem->ApplyAnomaly(FName(*Args[0]), TailArgs(Args));
			}
		}));

static FAutoConsoleCommandWithWorldAndArgs GRevertCmd(
	TEXT("IAI.Revert"),
	TEXT("Revert one anomaly by id. Usage: IAI.Revert <id>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() == 0)
			{
				UE_LOG(LogAnomaly, Warning, TEXT("Usage: IAI.Revert <id>"));
				return;
			}
			if (UAnomalyInjectorSubsystem* Subsystem = ResolveSubsystem(World))
			{
				Subsystem->RevertAnomaly(FName(*Args[0]));
			}
		}));

static FAutoConsoleCommandWithWorldAndArgs GRevertAllCmd(
	TEXT("IAI.RevertAll"),
	TEXT("Revert all active anomalies."),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (UAnomalyInjectorSubsystem* Subsystem = ResolveSubsystem(World))
			{
				const int32 Count = Subsystem->RevertAllActive();
				UE_LOG(LogAnomaly, Log, TEXT("IAI.RevertAll -> reverted %d anomaly(ies)."), Count);
			}
		}));
