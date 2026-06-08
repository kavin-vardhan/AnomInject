// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#include "GDPAnomalyInjectorSubsystem.h"
#include "GDPAnomalyInjectorLog.h"

#include "EngineUtils.h"            // TActorIterator
#include "Engine/Engine.h"         // GEngine
#include "Engine/World.h"          // UWorld
#include "GameFramework/Actor.h"
#include "HAL/IConsoleManager.h"   // FAutoConsoleCommandWithWorldAndArgs

/** Fixed on-screen-message key ("GDPH") so the heartbeat refreshes in place instead of stacking. */
static constexpr uint64 GGDPHeartbeatKey = 0x47445048;

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

void UGDPAnomalyInjectorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogGDPAnomaly, Log, TEXT("Subsystem initialized for world '%s'."), *GetNameSafe(GetWorld()));
}

void UGDPAnomalyInjectorSubsystem::Deinitialize()
{
	// Restore anything we hid before the world tears down.
	const int32 Restored = ShowAllHidden();
	if (Restored > 0)
	{
		UE_LOG(LogGDPAnomaly, Log, TEXT("Subsystem deinitializing; restored %d hidden actor(s)."), Restored);
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

	// Heartbeat: proves the subsystem ticks in PIE. On-screen every ~2s (visible),
	// plus a Verbose log line for those watching the Output Log.
	HeartbeatAccumulator += DeltaTime;
	if (HeartbeatAccumulator >= 2.0f)
	{
		HeartbeatAccumulator = 0.0f;
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				GGDPHeartbeatKey,
				2.5f,
				FColor::Green,
				FString::Printf(TEXT("[GDP] AnomalyInjector ticking (hidden: %d)"), HiddenActors.Num()));
		}
		UE_LOG(LogGDPAnomaly, Verbose, TEXT("Heartbeat; hidden actors: %d"), HiddenActors.Num());
	}
}

// ---------------------------------------------------------------------------
// M0 anomaly operations
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

int32 UGDPAnomalyInjectorSubsystem::HideActorsMatching(const FString& Substring)
{
	UWorld* World = GetWorld();
	if (!World || Substring.IsEmpty())
	{
		return 0;
	}

	int32 HiddenCount = 0;
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (!Actor)
		{
			continue;
		}

		// Match by Name or Class name only (case-insensitive). Never the editor label.
		const bool bNameMatch = Actor->GetName().Contains(Substring);
		const bool bClassMatch = Actor->GetClass()->GetName().Contains(Substring);
		if (bNameMatch || bClassMatch)
		{
			Actor->SetActorHiddenInGame(true);
			HiddenActors.AddUnique(Actor);
			++HiddenCount;
			UE_LOG(LogGDPAnomaly, Log, TEXT("Hid actor '%s' (class '%s')."),
				*Actor->GetName(), *Actor->GetClass()->GetName());
		}
	}
	return HiddenCount;
}

int32 UGDPAnomalyInjectorSubsystem::ShowAllHidden()
{
	int32 RestoredCount = 0;
	for (const TWeakObjectPtr<AActor>& WeakActor : HiddenActors)
	{
		if (AActor* Actor = WeakActor.Get())
		{
			Actor->SetActorHiddenInGame(false);
			++RestoredCount;
		}
	}
	HiddenActors.Reset();
	return RestoredCount;
}

// ---------------------------------------------------------------------------
// Console command surface
//
// Module-scoped FAutoConsoleCommandWithWorldAndArgs objects: registered at module
// load, alive for the module's lifetime, decoupled from subsystem lifetime. Each
// resolves the subsystem from the world the console passes in, and null-guards
// gracefully when invoked outside a game world (the subsystem only exists in
// Game/PIE worlds — see DoesSupportWorldType).
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

static FAutoConsoleCommandWithWorldAndArgs GGDPHideActorCmd(
	TEXT("GDP.HideActor"),
	TEXT("Hide actors whose Name or Class contains <substring>. Usage: GDP.HideActor <substring>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() == 0)
			{
				UE_LOG(LogGDPAnomaly, Warning, TEXT("Usage: GDP.HideActor <substring>"));
				return;
			}
			if (UGDPAnomalyInjectorSubsystem* Subsystem = ResolveSubsystem(World))
			{
				const int32 Count = Subsystem->HideActorsMatching(Args[0]);
				UE_LOG(LogGDPAnomaly, Log, TEXT("GDP.HideActor '%s' -> hid %d actor(s)."), *Args[0], Count);
			}
		}));

static FAutoConsoleCommandWithWorldAndArgs GGDPShowAllActorsCmd(
	TEXT("GDP.ShowAllActors"),
	TEXT("Restore every actor hidden via GDP.HideActor."),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (UGDPAnomalyInjectorSubsystem* Subsystem = ResolveSubsystem(World))
			{
				const int32 Count = Subsystem->ShowAllHidden();
				UE_LOG(LogGDPAnomaly, Log, TEXT("GDP.ShowAllActors -> restored %d actor(s)."), Count);
			}
		}));
