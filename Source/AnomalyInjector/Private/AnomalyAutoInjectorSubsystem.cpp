// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#include "AnomalyAutoInjectorSubsystem.h"
#include "AnomalyInjectorLog.h"
#include "AnomalyInjectorSubsystem.h"    // public Apply/Revert + IsViewportScopingEnabled (injector untouched)
#include "AnomalySelectorSubsystem.h"    // IsUIEnabled (coexistence warning only; selector untouched)
#include "AnomalyViewport.h"             // GetVisibleRenderableActors (the shared renderable-visible set)

#include "Engine/Engine.h"              // GEngine, GetSmallFont
#include "Engine/World.h"               // UWorld::GetFirstPlayerController / GetSubsystem
#include "Engine/Canvas.h"              // UCanvas (HUD draw)
#include "Debug/DebugDrawService.h"     // UDebugDrawService::Register/Unregister (host-blind HUD, G25)
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"   // WasInputKeyJustPressed (raw key state, G26)
#include "InputCoreTypes.h"             // EKeys, FKey
#include "HAL/IConsoleManager.h"        // FAutoConsoleCommandWithWorldAndArgs
#include "HAL/PlatformTime.h"           // FPlatformTime::Cycles (default time-based seed)
#include "CoreGlobals.h"                // GFrameCounter (fire start-frame stamp, m7 capture/labeling)

namespace
{
	/** v1 pool = the five object-scoped, primitive-backed anomalies (R-POOL). Globals (time_dilation,
	 *  camera_clipping) + lighting_mismatch are a future non-object track. The fixed ORDER is load-bearing:
	 *  Eligible is built in this order so Stream.RandHelper indexing is reproducible — APPEND new ids at the
	 *  END so existing seeds' early draws stay stable. */
	const FName GAutoPool[] =
	{
		FName(TEXT("missing_object")),
		FName(TEXT("flicker")),
		FName(TEXT("lod_corruption")),
		FName(TEXT("lod_popping")),
		FName(TEXT("missing_texture")),
	};
	constexpr int32 GNumAutoPool = UE_ARRAY_COUNT(GAutoPool);
	static_assert(GNumAutoPool == UAnomalyAutoInjectorSubsystem::NumPoolKeys, "pool size must match the keybind count");

	/** Resolve the injector subsystem from World (the only thing the auto-injector calls into). */
	UAnomalyInjectorSubsystem* ResolveInjector(UWorld* World)
	{
		return World ? World->GetSubsystem<UAnomalyInjectorSubsystem>() : nullptr;
	}
}

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

void UAnomalyAutoInjectorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Default keybinds: 1/2/3/4/5 toggle the five pool ids, J start/stop the run, K reseed. Distinct from
	// the selector's Tab/C/G/H so the two eyeball shells never react to one press (gotcha G26). Rebindable.
	KeyPool[0] = EKeys::One;
	KeyPool[1] = EKeys::Two;
	KeyPool[2] = EKeys::Three;
	KeyPool[3] = EKeys::Four;
	KeyPool[4] = EKeys::Five;
	KeyRun     = EKeys::J;
	KeyReseed  = EKeys::K;

	// Default seed is time-based (R-SEED: the seed VALUE may be time-derived; the scheduler DRAWS go
	// through the stream, never global FMath::Rand). Console-settable via IAI.Auto.Seed.
	Seed = static_cast<int32>(FPlatformTime::Cycles());
	Stream.Initialize(Seed);

	// Default enable-set: all four on, so a fresh run fires variety out of the box.
	for (const FName& Id : GAutoPool)
	{
		EnabledIds.Add(Id);
	}

	UE_LOG(LogAnomaly, Log,
		TEXT("AutoInjector subsystem initialized for world '%s' (Enable OFF; IAI.Auto.Enable 1 to show the UI, IAI.Auto.Run 1 to fire)."),
		*GetNameSafe(GetWorld()));
}

void UAnomalyAutoInjectorSubsystem::Deinitialize()
{
	// Mirror the selector: only delegate hygiene here. Do NOT call into the injector during teardown
	// (subsystem teardown order is unspecified) — the injector's own Deinitialize reverts everything
	// still active, so the world is restored regardless.
	UnregisterHUD();
	LiveFires.Reset();
	Super::Deinitialize();
}

bool UAnomalyAutoInjectorSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

TStatId UAnomalyAutoInjectorSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UAnomalyAutoInjectorSubsystem, STATGROUP_Tickables);
}

void UAnomalyAutoInjectorSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Dormant when neither switch is on -> existing gates byte-identical (no poll, no schedule, no draw).
	if (!bEnabled && !bRunning)
	{
		return;
	}

	if (bEnabled)
	{
		PollInput();   // eyeball shell: raw keys -> toggle pool / run / reseed
	}
	if (bRunning)
	{
		AdvanceTime(DeltaTime);   // auto-feed the deterministic core with the frame delta
	}
}

// ---------------------------------------------------------------------------
// Activation
// ---------------------------------------------------------------------------

void UAnomalyAutoInjectorSubsystem::SetEnabled(bool bInEnabled)
{
	if (bInEnabled == bEnabled)
	{
		UE_LOG(LogAnomaly, Log, TEXT("IAI.Auto.Enable: already %s."), bInEnabled ? TEXT("ON") : TEXT("OFF"));
		return;   // idempotent -> never double-registers the HUD delegate
	}

	bEnabled = bInEnabled;
	if (bEnabled)
	{
		WarnOnCoexistence();   // e.g. selector UI already on (gate C: SelectorUI 1 then Auto.Enable 1)
		RegisterHUD();
	}
	else
	{
		if (bRunning)
		{
			SetRunning(false);   // stop firing + revert live fires before going dormant
		}
		UnregisterHUD();
		LastFireResult.Reset();
	}

	UE_LOG(LogAnomaly, Log, TEXT("IAI.Auto.Enable -> %s."), bEnabled ? TEXT("ON") : TEXT("OFF"));
}

void UAnomalyAutoInjectorSubsystem::SetRunning(bool bInRunning)
{
	if (bInRunning == bRunning)
	{
		UE_LOG(LogAnomaly, Log, TEXT("IAI.Auto.Run: already %s."), bInRunning ? TEXT("ON") : TEXT("OFF"));
		return;
	}
	if (bInRunning && !bEnabled)
	{
		UE_LOG(LogAnomaly, Warning, TEXT("IAI.Auto.Run: enable the auto-injector first (IAI.Auto.Enable 1)."));
		return;   // Run is forced OFF when !Enabled (ruling a)
	}

	bRunning = bInRunning;
	if (bRunning)
	{
		RevertAllLiveFires();     // clean slate (e.g. any leftover FireOnce test fires)
		WarnOnCoexistence();      // selector-on / scoping-on warnings at run-start
		Stream.Initialize(Seed);  // reproducible from the seed
		// Arm the first interval (NOT 0): an interval draw precedes EVERY fire window, including the first
		// (uniform draw protocol + a calmer start).
		FireTimer = Stream.FRandRange(IntervalMin, IntervalMax);
		UE_LOG(LogAnomaly, Log, TEXT("IAI.Auto.Run -> ON (seed %d, first fire in %.2fs)."), Seed, FireTimer);
	}
	else
	{
		const int32 Reverted = RevertAllLiveFires();
		UE_LOG(LogAnomaly, Log, TEXT("IAI.Auto.Run -> OFF (reverted %d live fire(s))."), Reverted);
	}
}

// ---------------------------------------------------------------------------
// Explicit deterministic core
// ---------------------------------------------------------------------------

void UAnomalyAutoInjectorSubsystem::AdvanceTime(float DeltaSeconds)
{
	// 1) Service auto-reverts first so a freed slot can be reused by a fire this same call.
	ServiceReverts(DeltaSeconds);

	// 2) One timed fire window per call (frame-semantics). When the timer elapses, attempt one fire and
	//    arm the next interval — the interval draw happens here regardless of whether the attempt fired,
	//    so it is tied to the WINDOW, not to success (keeps the stream position deterministic).
	FireTimer -= DeltaSeconds;
	if (FireTimer <= 0.0f)
	{
		TryFireOnce();
		FireTimer = Stream.FRandRange(IntervalMin, IntervalMax);
	}
}

bool UAnomalyAutoInjectorSubsystem::TryFireOnce()
{
	// LOCKED DRAW PROTOCOL (R-SEED): stream draws happen on a FIXED schedule independent of ApplyAnomaly's
	// result, so a zero-match never shifts stream position. Skip-paths consume ZERO draws (cap, empty
	// Eligible, empty V); the Candidates-empty skip consumes exactly the Id draw; a real attempt draws
	// Id, Target, Hold (in that order) THEN applies and registers on success only.

	UWorld* World = GetWorld();
	UAnomalyInjectorSubsystem* Injector = ResolveInjector(World);
	if (!World || !Injector)
	{
		return false;
	}

	// Cap (0 draws). Also naturally bounded by the distinct enabled-id count via invariant (i).
	if (LiveFires.Num() >= MaxConcurrent)
	{
		return false;
	}

	// Eligible = pool order, enabled, NOT currently live (invariant i). (0 draws.)
	TArray<FName> Eligible;
	Eligible.Reserve(GNumAutoPool);
	for (const FName& Id : GAutoPool)
	{
		if (EnabledIds.Contains(Id) && !IsIdLive(Id))
		{
			Eligible.Add(Id);
		}
	}
	if (Eligible.Num() == 0)
	{
		return false;
	}

	// Renderable-visible set; empty on no view => never inject blind (R-CAD / R6). (0 draws.) Name-sort
	// for deterministic RandHelper indexing (the set is unsorted by contract).
	TArray<TWeakObjectPtr<AActor>> Visible = AnomalyViewport::GetVisibleRenderableActors(World);
	if (Visible.Num() == 0)
	{
		return false;
	}
	Visible.Sort([](const TWeakObjectPtr<AActor>& A, const TWeakObjectPtr<AActor>& B)
	{
		const AActor* AA = A.Get();
		const AActor* BB = B.Get();
		if (!AA) { return false; }
		if (!BB) { return true; }
		return AA->GetName() < BB->GetName();
	});

	// Draw 1 — Id.
	const FName Id = Eligible[Stream.RandHelper(Eligible.Num())];

	// Candidates = Visible - {actors hosting ANY live fire} (invariant ii, OVERRIDE-1: one anomaly per actor).
	TArray<AActor*> Candidates;
	Candidates.Reserve(Visible.Num());
	for (const TWeakObjectPtr<AActor>& Weak : Visible)
	{
		AActor* Actor = Weak.Get();
		if (Actor && !IsActorLive(Actor))
		{
			Candidates.Add(Actor);
		}
	}
	if (Candidates.Num() == 0)
	{
		// All visible actors already host a fire -> skip this window (the Id draw is consumed; locked).
		return false;
	}

	// Draw 2 — Target. Draw 3 — Hold.
	AActor* Target = Candidates[Stream.RandHelper(Candidates.Num())];
	const float Hold = Stream.FRandRange(HoldMin, HoldMax);

	// Apply to ONLY this actor via the "=" exact-match token (never a prefix-sibling). Register on success.
	const FString TargetName = Target->GetName();
	const FString Token = FString(TEXT("=")) + TargetName;
	const bool bApplied = Injector->ApplyAnomaly(Id, TArray<FString>{ Token });
	if (bApplied)
	{
		FAutoLiveFire Fire;
		Fire.Id = Id;
		Fire.Target = Target;
		Fire.TargetName = TargetName;
		Fire.SecondsRemaining = Hold;
		Fire.StartFrame = GFrameCounter;   // capture/labeling: the fire's start frame (m7)
		LiveFires.Add(Fire);
		LastFireResult = FString::Printf(TEXT("fire %s on %s (hold %.1fs)"), *Id.ToString(), *TargetName, Hold);
	}
	else
	{
		// Zero-match (e.g. an LOD id drawn onto a pure-VFX actor that is legitimately in the visible set):
		// surface it, don't register, move on. The draws above are already spent (protocol locked).
		LastFireResult = FString::Printf(TEXT("fire %s on %s: 0 matched (skipped)"), *Id.ToString(), *TargetName);
	}
	UE_LOG(LogAnomaly, Log, TEXT("Auto.Fire: '%s' on '%s' -> %s."),
		*Id.ToString(), *TargetName, bApplied ? TEXT("applied") : TEXT("0 matched"));
	return bApplied;
}

// ---------------------------------------------------------------------------
// Enable-set + cadence config
// ---------------------------------------------------------------------------

bool UAnomalyAutoInjectorSubsystem::SetAnomalyEnabled(FName Id, bool bInEnabled)
{
	bool bInPool = false;
	for (const FName& PoolId : GAutoPool)
	{
		if (PoolId == Id) { bInPool = true; break; }
	}
	if (!bInPool)
	{
		UE_LOG(LogAnomaly, Warning,
			TEXT("IAI.Auto.Pool: '%s' is not a v1 pool id (missing_object/flicker/lod_corruption/lod_popping/missing_texture)."),
			*Id.ToString());
		return false;
	}

	if (bInEnabled) { EnabledIds.Add(Id); } else { EnabledIds.Remove(Id); }
	UE_LOG(LogAnomaly, Log, TEXT("IAI.Auto.Pool: '%s' -> %s."), *Id.ToString(), bInEnabled ? TEXT("ON") : TEXT("OFF"));
	return true;
}

void UAnomalyAutoInjectorSubsystem::SetAllAnomaliesEnabled(bool bInEnabled)
{
	EnabledIds.Reset();
	if (bInEnabled)
	{
		for (const FName& Id : GAutoPool)
		{
			EnabledIds.Add(Id);
		}
	}
	UE_LOG(LogAnomaly, Log, TEXT("IAI.Auto.Pool all -> %s."), bInEnabled ? TEXT("ON") : TEXT("OFF"));
}

void UAnomalyAutoInjectorSubsystem::SetSeed(int32 InSeed)
{
	Seed = InSeed;
	Stream.Initialize(Seed);   // re-initialize now so a subsequent FireOnce/Step is reproducible
	UE_LOG(LogAnomaly, Log, TEXT("IAI.Auto.Seed -> %d."), Seed);
}

void UAnomalyAutoInjectorSubsystem::SetIntervalRange(float MinSeconds, float MaxSeconds)
{
	IntervalMin = FMath::Max(0.01f, MinSeconds);
	IntervalMax = FMath::Max(IntervalMin, MaxSeconds);
	UE_LOG(LogAnomaly, Log, TEXT("IAI.Auto.Interval -> [%.2f, %.2f]s."), IntervalMin, IntervalMax);
}

void UAnomalyAutoInjectorSubsystem::SetHoldRange(float MinSeconds, float MaxSeconds)
{
	HoldMin = FMath::Max(0.01f, MinSeconds);
	HoldMax = FMath::Max(HoldMin, MaxSeconds);
	UE_LOG(LogAnomaly, Log, TEXT("IAI.Auto.Hold -> [%.2f, %.2f]s."), HoldMin, HoldMax);
}

void UAnomalyAutoInjectorSubsystem::SetMaxConcurrent(int32 InMax)
{
	MaxConcurrent = FMath::Max(1, InMax);
	UE_LOG(LogAnomaly, Log, TEXT("IAI.Auto.MaxConcurrent -> %d."), MaxConcurrent);
}

void UAnomalyAutoInjectorSubsystem::SetPersist(bool bInPersist)
{
	bPersist = bInPersist;
	UE_LOG(LogAnomaly, Log, TEXT("IAI.Auto.Persist -> %s (fires %s auto-revert)."),
		bPersist ? TEXT("ON") : TEXT("OFF"), bPersist ? TEXT("do NOT") : TEXT("do"));
}

// ---------------------------------------------------------------------------
// Readbacks
// ---------------------------------------------------------------------------

TArray<FString> UAnomalyAutoInjectorSubsystem::GetEnabledIds() const
{
	TArray<FString> Result;
	for (const FName& Id : GAutoPool)   // fixed pool order
	{
		if (EnabledIds.Contains(Id))
		{
			Result.Add(Id.ToString());
		}
	}
	return Result;
}

bool UAnomalyAutoInjectorSubsystem::IsAnomalyEnabled(FName Id) const
{
	return EnabledIds.Contains(Id);
}

TArray<FString> UAnomalyAutoInjectorSubsystem::GetLiveFireSummaries() const
{
	TArray<FString> Result;
	Result.Reserve(LiveFires.Num());
	for (const FAutoLiveFire& Fire : LiveFires)
	{
		Result.Add(bPersist
			? FString::Printf(TEXT("%s %s persist"), *Fire.Id.ToString(), *Fire.TargetName)
			: FString::Printf(TEXT("%s %s %.1fs"), *Fire.Id.ToString(), *Fire.TargetName, Fire.SecondsRemaining));
	}
	return Result;
}

TArray<FAutoLiveFireInfo> UAnomalyAutoInjectorSubsystem::GetLiveFires() const
{
	TArray<FAutoLiveFireInfo> Result;
	Result.Reserve(LiveFires.Num());
	for (const FAutoLiveFire& Fire : LiveFires)
	{
		FAutoLiveFireInfo Info;
		Info.Id = Fire.Id;
		Info.Target = Fire.TargetName;
		Info.TargetActor = Fire.Target;          // the fired actor, for bounds projection (m7)
		Info.SecondsRemaining = Fire.SecondsRemaining;
		Info.StartFrame = Fire.StartFrame;
		Result.Add(Info);
	}
	return Result;
}

void UAnomalyAutoInjectorSubsystem::LogStatus() const
{
	UE_LOG(LogAnomaly, Log, TEXT("--- IAI.Auto.Status ---"));
	UE_LOG(LogAnomaly, Log, TEXT("  enable: %s | run: %s | seed: %d | persist: %s"),
		bEnabled ? TEXT("ON") : TEXT("OFF"), bRunning ? TEXT("ON") : TEXT("OFF"), Seed, bPersist ? TEXT("ON") : TEXT("OFF"));
	UE_LOG(LogAnomaly, Log, TEXT("  interval: [%.2f,%.2f]s | hold: [%.2f,%.2f]s | maxConcurrent: %d | nextFire: %.2fs"),
		IntervalMin, IntervalMax, HoldMin, HoldMax, MaxConcurrent, FireTimer);

	const TArray<FString> Enabled = GetEnabledIds();
	UE_LOG(LogAnomaly, Log, TEXT("  enabled pool (%d): %s"), Enabled.Num(),
		Enabled.Num() ? *FString::Join(Enabled, TEXT(", ")) : TEXT("(none)"));

	const TArray<FString> Live = GetLiveFireSummaries();
	UE_LOG(LogAnomaly, Log, TEXT("  live fires (%d):"), Live.Num());
	for (const FString& Line : Live)
	{
		UE_LOG(LogAnomaly, Log, TEXT("    %s"), *Line);
	}
	UE_LOG(LogAnomaly, Log, TEXT("--- %d live fire(s) ---"), Live.Num());
}

// ---------------------------------------------------------------------------
// Configurable keybinds
// ---------------------------------------------------------------------------

bool UAnomalyAutoInjectorSubsystem::SetKeyBinding(FName Action, FKey Key)
{
	if      (Action == FName(TEXT("pool1")))  { KeyPool[0] = Key; }
	else if (Action == FName(TEXT("pool2")))  { KeyPool[1] = Key; }
	else if (Action == FName(TEXT("pool3")))  { KeyPool[2] = Key; }
	else if (Action == FName(TEXT("pool4")))  { KeyPool[3] = Key; }
	else if (Action == FName(TEXT("pool5")))  { KeyPool[4] = Key; }
	else if (Action == FName(TEXT("run")))    { KeyRun = Key; }
	else if (Action == FName(TEXT("reseed"))) { KeyReseed = Key; }
	else
	{
		UE_LOG(LogAnomaly, Warning,
			TEXT("IAI.Auto.Bind: unknown action '%s' (use pool1/pool2/pool3/pool4/pool5/run/reseed)."), *Action.ToString());
		return false;
	}

	UE_LOG(LogAnomaly, Log, TEXT("IAI.Auto.Bind: '%s' -> '%s'."), *Action.ToString(), *Key.ToString());
	return true;
}

// ---------------------------------------------------------------------------
// Thin shells + internals
// ---------------------------------------------------------------------------

void UAnomalyAutoInjectorSubsystem::PollInput()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	APlayerController* PC = World->GetFirstPlayerController();   // reachable each tick (gotcha G23)
	if (!PC)
	{
		return;
	}

	for (int32 i = 0; i < NumPoolKeys; ++i)
	{
		if (KeyPool[i].IsValid() && PC->WasInputKeyJustPressed(KeyPool[i]))
		{
			SetAnomalyEnabled(GAutoPool[i], !IsAnomalyEnabled(GAutoPool[i]));
		}
	}
	if (KeyRun.IsValid()    && PC->WasInputKeyJustPressed(KeyRun))    { SetRunning(!bRunning); }
	if (KeyReseed.IsValid() && PC->WasInputKeyJustPressed(KeyReseed)) { SetSeed(static_cast<int32>(FPlatformTime::Cycles())); }
}

int32 UAnomalyAutoInjectorSubsystem::ServiceReverts(float DeltaSeconds)
{
	if (bPersist || LiveFires.Num() == 0)
	{
		return 0;   // persist -> fires never auto-expire (R-LIFE config flag)
	}

	UAnomalyInjectorSubsystem* Injector = ResolveInjector(GetWorld());
	int32 Reverted = 0;
	// Reverse iterate for safe in-place removal.
	for (int32 i = LiveFires.Num() - 1; i >= 0; --i)
	{
		LiveFires[i].SecondsRemaining -= DeltaSeconds;
		if (LiveFires[i].SecondsRemaining <= 0.0f)
		{
			if (Injector)
			{
				Injector->RevertAnomaly(LiveFires[i].Id);   // GC-safe even if the target actor is gone
			}
			UE_LOG(LogAnomaly, Log, TEXT("Auto.Revert: '%s' on '%s' (hold elapsed)."),
				*LiveFires[i].Id.ToString(), *LiveFires[i].TargetName);
			LiveFires.RemoveAt(i);
			++Reverted;
		}
	}
	return Reverted;
}

int32 UAnomalyAutoInjectorSubsystem::RevertAllLiveFires()
{
	UAnomalyInjectorSubsystem* Injector = ResolveInjector(GetWorld());
	const int32 Count = LiveFires.Num();
	for (const FAutoLiveFire& Fire : LiveFires)
	{
		if (Injector)
		{
			Injector->RevertAnomaly(Fire.Id);
		}
	}
	LiveFires.Reset();
	return Count;
}

void UAnomalyAutoInjectorSubsystem::WarnOnCoexistence() const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// R-COEXIST (warn, not block): manual selector/console injection of a pool id during an auto run will
	// clobber via the registry's one-instance-per-id. The auto-injector can only track its OWN fires.
	if (const UAnomalySelectorSubsystem* Selector = World->GetSubsystem<UAnomalySelectorSubsystem>())
	{
		if (Selector->IsUIEnabled())
		{
			UE_LOG(LogAnomaly, Warning,
				TEXT("Auto-injector and the selector UI are BOTH enabled — UNSUPPORTED. Manual injection of a pool id "
				     "during an auto run will clobber via the injector's one-instance-per-id. Disable one (IAI.SelectorUI 0)."));
		}
	}

	// Self-scoping: if viewport scoping is ON, the "=" apply re-tests visibility (redundant; can drop a
	// target between pick and apply, m5 fact #3). Recommend OFF — but do NOT force it (would surprise a
	// manual user).
	if (UAnomalyInjectorSubsystem::IsViewportScopingEnabled(World))
	{
		UE_LOG(LogAnomaly, Warning,
			TEXT("IAI.SetViewportScoping is ON; the auto-injector is self-scoping (it draws from the renderable-visible "
			     "set), so its apply will REDUNDANTLY re-test visibility and may drop a target. Recommend IAI.SetViewportScoping 0."));
	}
}

bool UAnomalyAutoInjectorSubsystem::IsIdLive(FName Id) const
{
	for (const FAutoLiveFire& Fire : LiveFires)
	{
		if (Fire.Id == Id)
		{
			return true;
		}
	}
	return false;
}

bool UAnomalyAutoInjectorSubsystem::IsActorLive(const AActor* Actor) const
{
	for (const FAutoLiveFire& Fire : LiveFires)
	{
		if (Fire.Target.Get() == Actor)
		{
			return true;
		}
	}
	return false;
}

void UAnomalyAutoInjectorSubsystem::RegisterHUD()
{
	if (DebugDrawHandle.IsValid())
	{
		return;   // already registered (guard against double-register)
	}
	// "Game" engine show flag — ON for any non-editor view; drawn by GameViewportClient::Draw with ZERO
	// dependence on the host's HUD/GameMode class (gotcha G25). A second delegate alongside the selector's
	// is additive (independent handles, both fire).
	DebugDrawHandle = UDebugDrawService::Register(
		TEXT("Game"),
		FDebugDrawDelegate::CreateUObject(this, &UAnomalyAutoInjectorSubsystem::DrawHUD));
}

void UAnomalyAutoInjectorSubsystem::UnregisterHUD()
{
	if (DebugDrawHandle.IsValid())
	{
		UDebugDrawService::Unregister(DebugDrawHandle);
		DebugDrawHandle.Reset();
	}
}

void UAnomalyAutoInjectorSubsystem::DrawHUD(UCanvas* Canvas, APlayerController* /*PC*/)
{
	if (!bEnabled || !Canvas)
	{
		return;
	}
	UFont* Font = GEngine ? GEngine->GetSmallFont() : nullptr;
	if (!Font)
	{
		return;
	}

	// Right column, off the selector's top-left HUD (both-on is unsupported, but a distinct anchor keeps
	// it legible if the owner toggles both for a quick look).
	const float X = FMath::Max(50.0f, Canvas->SizeX - 460.0f);
	float Y = 80.0f;
	const float LineH = 16.0f;

	Canvas->SetDrawColor(FColor::White);
	Canvas->DrawText(Font, FString::Printf(TEXT("[IAI] Auto-Injector  —  Run: %s   (1-4: types   J: run   K: reseed)"),
		bRunning ? TEXT("ON") : TEXT("OFF")), X, Y);
	Y += LineH * 1.5f;

	Canvas->SetDrawColor(FColor(180, 180, 180));
	Canvas->DrawText(Font, FString::Printf(TEXT("seed %d   interval [%.1f,%.1f]s   hold [%.1f,%.1f]s   max %d%s"),
		Seed, IntervalMin, IntervalMax, HoldMin, HoldMax, MaxConcurrent, bPersist ? TEXT("   PERSIST") : TEXT("")), X, Y);
	Y += LineH * 1.5f;

	// Enable-set: the pool ids with on/off state (the gameplay-start "which types" selection).
	Canvas->SetDrawColor(FColor::White);
	Canvas->DrawText(Font, TEXT("Types:"), X, Y);
	Y += LineH;
	for (int32 i = 0; i < GNumAutoPool; ++i)
	{
		const bool bOn = EnabledIds.Contains(GAutoPool[i]);
		Canvas->SetDrawColor(bOn ? FColor::Green : FColor(120, 120, 120));
		Canvas->DrawText(Font, FString::Printf(TEXT("  [%d] %s %s"),
			i + 1, bOn ? TEXT("[x]") : TEXT("[ ]"), *GAutoPool[i].ToString()), X, Y);
		Y += LineH;
	}

	Y += LineH * 0.5f;

	// Live fires (id -> target, time left).
	Canvas->SetDrawColor(FColor::White);
	Canvas->DrawText(Font, FString::Printf(TEXT("Live (%d/%d):"), LiveFires.Num(), MaxConcurrent), X, Y);
	Y += LineH;
	for (const FAutoLiveFire& Fire : LiveFires)
	{
		Canvas->SetDrawColor(FColor::Yellow);
		Canvas->DrawText(Font, bPersist
			? FString::Printf(TEXT("  %s -> %s  (persist)"), *Fire.Id.ToString(), *Fire.TargetName)
			: FString::Printf(TEXT("  %s -> %s  (%.1fs)"), *Fire.Id.ToString(), *Fire.TargetName, Fire.SecondsRemaining),
			X, Y);
		Y += LineH;
	}

	if (!LastFireResult.IsEmpty())
	{
		Y += LineH * 0.5f;
		Canvas->SetDrawColor(FColor(120, 200, 255));
		Canvas->DrawText(Font, FString::Printf(TEXT("Last: %s"), *LastFireResult), X, Y);
	}
}

// ---------------------------------------------------------------------------
// Console command surface (the bridge thin-shell -> the explicit-core methods)
//
// Module-scoped FAutoConsoleCommandWithWorldAndArgs, mirroring the injector/selector pattern: each
// resolves the auto-injector subsystem from the console's world and null-guards outside Game/PIE.
// ---------------------------------------------------------------------------

namespace
{
	UAnomalyAutoInjectorSubsystem* ResolveAuto(UWorld* World)
	{
		if (!World)
		{
			UE_LOG(LogAnomaly, Warning, TEXT("No world for this command; run it from a game/PIE world."));
			return nullptr;
		}
		UAnomalyAutoInjectorSubsystem* Auto = World->GetSubsystem<UAnomalyAutoInjectorSubsystem>();
		if (!Auto)
		{
			UE_LOG(LogAnomaly, Warning,
				TEXT("AnomalyAutoInjector subsystem not present for world '%s' (only active in Game/PIE worlds)."),
				*GetNameSafe(World));
		}
		return Auto;
	}
}

static FAutoConsoleCommandWithWorldAndArgs GAutoEnableCmd(
	TEXT("IAI.Auto.Enable"),
	TEXT("Enable/disable the auto-injector eyeball shell (HUD + keys). Default OFF. Usage: IAI.Auto.Enable <0|1>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() == 0) { UE_LOG(LogAnomaly, Warning, TEXT("Usage: IAI.Auto.Enable <0|1>")); return; }
			if (UAnomalyAutoInjectorSubsystem* Auto = ResolveAuto(World)) { Auto->SetEnabled(FCString::Atoi(*Args[0]) != 0); }
		}));

static FAutoConsoleCommandWithWorldAndArgs GAutoRunCmd(
	TEXT("IAI.Auto.Run"),
	TEXT("Start/stop the auto-tick firing loop (requires Enable). Usage: IAI.Auto.Run <0|1>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() == 0) { UE_LOG(LogAnomaly, Warning, TEXT("Usage: IAI.Auto.Run <0|1>")); return; }
			if (UAnomalyAutoInjectorSubsystem* Auto = ResolveAuto(World)) { Auto->SetRunning(FCString::Atoi(*Args[0]) != 0); }
		}));

static FAutoConsoleCommandWithWorldAndArgs GAutoSeedCmd(
	TEXT("IAI.Auto.Seed"),
	TEXT("Set the run seed (re-initializes the stream now). Usage: IAI.Auto.Seed <int>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() == 0) { UE_LOG(LogAnomaly, Warning, TEXT("Usage: IAI.Auto.Seed <int>")); return; }
			if (UAnomalyAutoInjectorSubsystem* Auto = ResolveAuto(World)) { Auto->SetSeed(FCString::Atoi(*Args[0])); }
		}));

static FAutoConsoleCommandWithWorldAndArgs GAutoPoolCmd(
	TEXT("IAI.Auto.Pool"),
	TEXT("Enable/disable a pool id (or 'all'). Usage: IAI.Auto.Pool <id|all> <0|1>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() < 2) { UE_LOG(LogAnomaly, Warning, TEXT("Usage: IAI.Auto.Pool <id|all> <0|1>")); return; }
			if (UAnomalyAutoInjectorSubsystem* Auto = ResolveAuto(World))
			{
				const bool bOn = FCString::Atoi(*Args[1]) != 0;
				if (Args[0].Equals(TEXT("all"), ESearchCase::IgnoreCase)) { Auto->SetAllAnomaliesEnabled(bOn); }
				else { Auto->SetAnomalyEnabled(FName(*Args[0]), bOn); }
			}
		}));

static FAutoConsoleCommandWithWorldAndArgs GAutoIntervalCmd(
	TEXT("IAI.Auto.Interval"),
	TEXT("Set the inter-fire interval range. Usage: IAI.Auto.Interval <minSec> <maxSec>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() < 2) { UE_LOG(LogAnomaly, Warning, TEXT("Usage: IAI.Auto.Interval <minSec> <maxSec>")); return; }
			if (UAnomalyAutoInjectorSubsystem* Auto = ResolveAuto(World)) { Auto->SetIntervalRange(FCString::Atof(*Args[0]), FCString::Atof(*Args[1])); }
		}));

static FAutoConsoleCommandWithWorldAndArgs GAutoHoldCmd(
	TEXT("IAI.Auto.Hold"),
	TEXT("Set the per-fire hold (auto-revert) range. Usage: IAI.Auto.Hold <minSec> <maxSec>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() < 2) { UE_LOG(LogAnomaly, Warning, TEXT("Usage: IAI.Auto.Hold <minSec> <maxSec>")); return; }
			if (UAnomalyAutoInjectorSubsystem* Auto = ResolveAuto(World)) { Auto->SetHoldRange(FCString::Atof(*Args[0]), FCString::Atof(*Args[1])); }
		}));

static FAutoConsoleCommandWithWorldAndArgs GAutoMaxConcurrentCmd(
	TEXT("IAI.Auto.MaxConcurrent"),
	TEXT("Set the max concurrent live fires. Usage: IAI.Auto.MaxConcurrent <n>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() == 0) { UE_LOG(LogAnomaly, Warning, TEXT("Usage: IAI.Auto.MaxConcurrent <n>")); return; }
			if (UAnomalyAutoInjectorSubsystem* Auto = ResolveAuto(World)) { Auto->SetMaxConcurrent(FCString::Atoi(*Args[0])); }
		}));

static FAutoConsoleCommandWithWorldAndArgs GAutoPersistCmd(
	TEXT("IAI.Auto.Persist"),
	TEXT("Toggle persist-until-manual (off = auto-revert after hold). Usage: IAI.Auto.Persist <0|1>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() == 0) { UE_LOG(LogAnomaly, Warning, TEXT("Usage: IAI.Auto.Persist <0|1>")); return; }
			if (UAnomalyAutoInjectorSubsystem* Auto = ResolveAuto(World)) { Auto->SetPersist(FCString::Atoi(*Args[0]) != 0); }
		}));

static FAutoConsoleCommandWithWorldAndArgs GAutoStepCmd(
	TEXT("IAI.Auto.Step"),
	TEXT("Advance the scheduler by N seconds (deterministic core drive; no Enable/Run needed). Usage: IAI.Auto.Step <seconds>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() == 0) { UE_LOG(LogAnomaly, Warning, TEXT("Usage: IAI.Auto.Step <seconds>")); return; }
			if (UAnomalyAutoInjectorSubsystem* Auto = ResolveAuto(World)) { Auto->AdvanceTime(FCString::Atof(*Args[0])); }
		}));

static FAutoConsoleCommandWithWorldAndArgs GAutoFireOnceCmd(
	TEXT("IAI.Auto.FireOnce"),
	TEXT("Force one fire attempt now (deterministic core drive; no Enable/Run needed)."),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (UAnomalyAutoInjectorSubsystem* Auto = ResolveAuto(World)) { Auto->TryFireOnce(); }
		}));

static FAutoConsoleCommandWithWorldAndArgs GAutoStatusCmd(
	TEXT("IAI.Auto.Status"),
	TEXT("Log enable/run state, seed, cadence, the enabled set, and the live fires."),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (UAnomalyAutoInjectorSubsystem* Auto = ResolveAuto(World)) { Auto->LogStatus(); }
		}));

static FAutoConsoleCommandWithWorldAndArgs GAutoBindCmd(
	TEXT("IAI.Auto.Bind"),
	TEXT("Rebind an auto-injector key. Usage: IAI.Auto.Bind <pool1|pool2|pool3|pool4|run|reseed> <KeyName>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() < 2) { UE_LOG(LogAnomaly, Warning, TEXT("Usage: IAI.Auto.Bind <pool1|pool2|pool3|pool4|run|reseed> <KeyName>")); return; }
			const FName KeyName(*Args[1]);
			const FKey Key(KeyName);
			if (!EKeys::GetKeyDetails(Key).IsValid())
			{
				UE_LOG(LogAnomaly, Warning, TEXT("IAI.Auto.Bind: '%s' is not a known key."), *Args[1]);
				return;
			}
			if (UAnomalyAutoInjectorSubsystem* Auto = ResolveAuto(World)) { Auto->SetKeyBinding(FName(*Args[0]), Key); }
		}));
