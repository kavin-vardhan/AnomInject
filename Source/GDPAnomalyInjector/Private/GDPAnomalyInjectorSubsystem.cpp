// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#include "GDPAnomalyInjectorSubsystem.h"
#include "GDPAnomalyInjectorLog.h"

#include "EngineUtils.h"            // TActorIterator
#include "Engine/Engine.h"         // GEngine
#include "Engine/World.h"          // UWorld
#include "GameFramework/Actor.h"
#include "HAL/IConsoleManager.h"   // FAutoConsoleCommandWithWorldAndArgs

#include "Anomalies/GDPAnomaly_MissingObject.h"
#include "Anomalies/GDPAnomaly_Flicker.h"
#include "Anomalies/GDPAnomaly_TimeDilation.h"
#include "Anomalies/GDPAnomaly_LightingMismatch.h"
#include "Anomalies/GDPAnomaly_LodCorruption.h"
#include "Anomalies/GDPAnomaly_LodPopping.h"
#include "Anomalies/GDPAnomaly_CameraClipping.h"

/** Fixed on-screen-message key ("GDPH") so the heartbeat refreshes in place instead of stacking. */
static constexpr uint64 GGDPHeartbeatKey = 0x47445048;

// Out-of-line dtor defined where IGDPAnomaly is a complete type (gotcha G9).
UGDPAnomalyInjectorSubsystem::~UGDPAnomalyInjectorSubsystem() = default;

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

void UGDPAnomalyInjectorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Explicit registration — one instance per type, keyed by GetId(). No self-registration
	// macros (keeps it readable and free of static-init-order hazards).
	auto Register = [this](TUniquePtr<IGDPAnomaly> Anomaly)
	{
		const FName Id = Anomaly->GetId();
		Anomalies.Add(Id, MoveTemp(Anomaly));
	};
	Register(MakeUnique<FGDPAnomaly_MissingObject>());
	Register(MakeUnique<FGDPAnomaly_Flicker>());
	Register(MakeUnique<FGDPAnomaly_TimeDilation>());
	Register(MakeUnique<FGDPAnomaly_LightingMismatch>());   // M2: component-targeting (A1) + per-target capture
	Register(MakeUnique<FGDPAnomaly_LodCorruption>());      // M2/M3: component-targeting, static + skeletal (GDPLod)
	Register(MakeUnique<FGDPAnomaly_LodPopping>());         // M3: ticking LOD pop (GDPLod), static + skeletal
	Register(MakeUnique<FGDPAnomaly_CameraClipping>());     // M2: global near-clip capture/restore

	UE_LOG(LogGDPAnomaly, Log, TEXT("Subsystem initialized for world '%s'. %d anomaly type(s) registered."),
		*GetNameSafe(GetWorld()), Anomalies.Num());
}

void UGDPAnomalyInjectorSubsystem::Deinitialize()
{
	// Generalized auto-restore-on-teardown: revert anything still active before the world ends.
	const int32 Reverted = RevertAllActive();
	if (Reverted > 0)
	{
		UE_LOG(LogGDPAnomaly, Log, TEXT("Subsystem deinitializing; reverted %d active anomaly(ies)."), Reverted);
	}
	Super::Deinitialize();
}

bool UGDPAnomalyInjectorSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	// Only real game worlds: standalone game and Play-In-Editor. Never the editor world.
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

TStatId UGDPAnomalyInjectorSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UGDPAnomalyInjectorSubsystem, STATGROUP_Tickables);
}

void UGDPAnomalyInjectorSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Drive the active anomalies (static ones inherit a no-op Tick).
	for (const TPair<FName, TUniquePtr<IGDPAnomaly>>& Pair : Anomalies)
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
				GGDPHeartbeatKey,
				2.5f,
				FColor::Green,
				FString::Printf(TEXT("[GDP] AnomalyInjector ticking (active: %d/%d)"), ActiveCount, Anomalies.Num()));
		}
		UE_LOG(LogGDPAnomaly, Verbose, TEXT("Heartbeat; active anomalies: %d/%d"), ActiveCount, Anomalies.Num());
	}
}

// ---------------------------------------------------------------------------
// Targeting aid
// ---------------------------------------------------------------------------

void UGDPAnomalyInjectorSubsystem::ListActors() const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	int32 Count = 0;
	UE_LOG(LogGDPAnomaly, Log, TEXT("--- GDP.ListActors (world '%s') ---"), *GetNameSafe(World));
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
		UE_LOG(LogGDPAnomaly, Log, TEXT("  %s | %s | %s"),
			*Actor->GetClass()->GetName(),
			*Actor->GetName(),
			*Label);
		++Count;
	}
	UE_LOG(LogGDPAnomaly, Log, TEXT("--- %d actor(s) ---"), Count);
}

// ---------------------------------------------------------------------------
// Anomaly manager API
// ---------------------------------------------------------------------------

void UGDPAnomalyInjectorSubsystem::ListAnomalies() const
{
	// Deterministic, sorted output (AMB-5). FName::operator< is index-order, not lexical,
	// so sort by the string form.
	TArray<FName> Ids;
	Anomalies.GetKeys(Ids);
	Ids.Sort([](const FName& A, const FName& B) { return A.ToString() < B.ToString(); });

	UE_LOG(LogGDPAnomaly, Log, TEXT("--- GDP.ListAnomalies (%d) ---"), Ids.Num());
	for (const FName& Id : Ids)
	{
		const TUniquePtr<IGDPAnomaly>& Anomaly = Anomalies[Id];
		UE_LOG(LogGDPAnomaly, Log, TEXT("  %s - %s - %s"),
			*Anomaly->GetId().ToString(), *Anomaly->GetDescription(), *Anomaly->GetUsage());
	}
	UE_LOG(LogGDPAnomaly, Log, TEXT("--- %d anomaly(ies) ---"), Ids.Num());
}

bool UGDPAnomalyInjectorSubsystem::ApplyAnomaly(const FName& Id, const TArray<FString>& Args)
{
	TUniquePtr<IGDPAnomaly>* Found = Anomalies.Find(Id);
	if (!Found || !Found->IsValid())
	{
		UE_LOG(LogGDPAnomaly, Warning, TEXT("Unknown anomaly '%s'. Try GDP.ListAnomalies."), *Id.ToString());
		return false;
	}

	// Re-entrancy (revert-then-reapply) is handled inside each anomaly's Apply.
	const bool bApplied = (*Found)->Apply(GetWorld(), Args);
	UE_LOG(LogGDPAnomaly, Log, TEXT("GDP.Apply '%s' -> %s."), *Id.ToString(), bApplied ? TEXT("applied") : TEXT("not applied"));
	return bApplied;
}

bool UGDPAnomalyInjectorSubsystem::RevertAnomaly(const FName& Id)
{
	TUniquePtr<IGDPAnomaly>* Found = Anomalies.Find(Id);
	if (!Found || !Found->IsValid())
	{
		UE_LOG(LogGDPAnomaly, Warning, TEXT("Unknown anomaly '%s'. Try GDP.ListAnomalies."), *Id.ToString());
		return false;
	}
	if (!(*Found)->IsActive())
	{
		UE_LOG(LogGDPAnomaly, Log, TEXT("GDP.Revert '%s': not active."), *Id.ToString());
		return false;
	}

	(*Found)->Revert();
	UE_LOG(LogGDPAnomaly, Log, TEXT("GDP.Revert '%s' -> reverted."), *Id.ToString());
	return true;
}

int32 UGDPAnomalyInjectorSubsystem::RevertAllActive()
{
	int32 Count = 0;
	for (const TPair<FName, TUniquePtr<IGDPAnomaly>>& Pair : Anomalies)
	{
		if (Pair.Value && Pair.Value->IsActive())
		{
			Pair.Value->Revert();
			++Count;
		}
	}
	return Count;
}

int32 UGDPAnomalyInjectorSubsystem::GetActiveAnomalyCount() const
{
	int32 Count = 0;
	for (const TPair<FName, TUniquePtr<IGDPAnomaly>>& Pair : Anomalies)
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
	UGDPAnomalyInjectorSubsystem* ResolveSubsystem(UWorld* World)
	{
		if (!World)
		{
			UE_LOG(LogGDPAnomaly, Warning, TEXT("No world for this command; run it from a game/PIE world."));
			return nullptr;
		}

		UGDPAnomalyInjectorSubsystem* Subsystem = World->GetSubsystem<UGDPAnomalyInjectorSubsystem>();
		if (!Subsystem)
		{
			UE_LOG(LogGDPAnomaly, Warning,
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

static FAutoConsoleCommandWithWorldAndArgs GGDPListActorsCmd(
	TEXT("GDP.ListActors"),
	TEXT("List actors in the current world as 'Class | Name | Label'."),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (UGDPAnomalyInjectorSubsystem* Subsystem = ResolveSubsystem(World))
			{
				Subsystem->ListActors();
			}
		}));

static FAutoConsoleCommandWithWorldAndArgs GGDPListAnomaliesCmd(
	TEXT("GDP.ListAnomalies"),
	TEXT("List registered anomalies as 'id - description - usage'."),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (UGDPAnomalyInjectorSubsystem* Subsystem = ResolveSubsystem(World))
			{
				Subsystem->ListAnomalies();
			}
		}));

static FAutoConsoleCommandWithWorldAndArgs GGDPApplyCmd(
	TEXT("GDP.Apply"),
	TEXT("Apply an anomaly by id. Usage: GDP.Apply <id> <args...>  (see GDP.ListAnomalies)"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() == 0)
			{
				UE_LOG(LogGDPAnomaly, Warning, TEXT("Usage: GDP.Apply <id> <args...>  (see GDP.ListAnomalies)"));
				return;
			}
			if (UGDPAnomalyInjectorSubsystem* Subsystem = ResolveSubsystem(World))
			{
				Subsystem->ApplyAnomaly(FName(*Args[0]), TailArgs(Args));
			}
		}));

static FAutoConsoleCommandWithWorldAndArgs GGDPRevertCmd(
	TEXT("GDP.Revert"),
	TEXT("Revert one anomaly by id. Usage: GDP.Revert <id>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() == 0)
			{
				UE_LOG(LogGDPAnomaly, Warning, TEXT("Usage: GDP.Revert <id>"));
				return;
			}
			if (UGDPAnomalyInjectorSubsystem* Subsystem = ResolveSubsystem(World))
			{
				Subsystem->RevertAnomaly(FName(*Args[0]));
			}
		}));

static FAutoConsoleCommandWithWorldAndArgs GGDPRevertAllCmd(
	TEXT("GDP.RevertAll"),
	TEXT("Revert all active anomalies."),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (UGDPAnomalyInjectorSubsystem* Subsystem = ResolveSubsystem(World))
			{
				const int32 Count = Subsystem->RevertAllActive();
				UE_LOG(LogGDPAnomaly, Log, TEXT("GDP.RevertAll -> reverted %d anomaly(ies)."), Count);
			}
		}));
