// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#include "AnomalySelectorSubsystem.h"
#include "AnomalyInjectorLog.h"
#include "AnomalyInjectorSubsystem.h"   // public Apply/Revert surface (the injector stays untouched)
#include "AnomalyViewport.h"            // GetActiveViewInfo + FilterVisibleActors (composed; AnomalyViewport unchanged)

#include "EngineUtils.h"                // TActorIterator
#include "Engine/Engine.h"             // GEngine, GetSmallFont
#include "Engine/World.h"              // UWorld::GetFirstPlayerController / GetSubsystem
#include "Engine/Canvas.h"            // UCanvas (HUD draw)
#include "Debug/DebugDrawService.h"   // UDebugDrawService::Register/Unregister (host-blind HUD)
#include "DrawDebugHelpers.h"         // DrawDebugBox (selected-object highlight)
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"   // IsInputKeyDown / WasInputKeyJustPressed (raw key state)
#include "InputCoreTypes.h"           // EKeys, FKey
#include "HAL/IConsoleManager.h"      // FAutoConsoleCommandWithWorldAndArgs

namespace
{
	/** The object-scoped, primitive-backed anomalies the selector offers (default args only; missing_texture
	 *  has no args). NOTE: lod_corruption / lod_popping are intentionally HIDDEN from the UI for now — they
	 *  remain fully registered and applyable via `IAI.Apply <id>` (functionality intact); they are only
	 *  omitted from this selectable list. Globals (time_dilation, camera_clipping) + lighting_mismatch stay
	 *  console-only for v1. */
	const FName GAnomalyChoices[] =
	{
		FName(TEXT("missing_object")),
		FName(TEXT("flicker")),
		FName(TEXT("missing_texture")),
	};
	constexpr int32 GNumAnomalyChoices = UE_ARRAY_COUNT(GAnomalyChoices);

	/** Cap the on-screen visible list so a fully-visible scene cannot bury the screen. */
	constexpr int32 GMaxListedActors = 18;
}

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

void UAnomalySelectorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Default keybinds (S7 ruling). KeyPrev left invalid -> "previous" = Shift + KeyNext by default;
	// all are rebindable via IAI.SelectorBind to escape host/overlay collisions (e.g. the Steam overlay
	// grabs Shift+Tab in a Steam-launched build — fine in PIE; rebind escapes it; gotcha G26).
	KeyNext   = EKeys::Tab;
	KeyPrev   = FKey();          // none -> use Shift+Tab
	KeyCycle  = EKeys::C;
	KeyInject = EKeys::G;
	KeyRevert = EKeys::H;

	UE_LOG(LogAnomaly, Log, TEXT("Selector subsystem initialized for world '%s' (UI OFF; IAI.SelectorUI 1 to enable)."),
		*GetNameSafe(GetWorld()));
}

void UAnomalySelectorSubsystem::Deinitialize()
{
	// Delegate hygiene: a dangling debug-draw delegate is a crash risk. Always unregister on teardown.
	UnregisterHUD();
	Super::Deinitialize();
}

bool UAnomalySelectorSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

TStatId UAnomalySelectorSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UAnomalySelectorSubsystem, STATGROUP_Tickables);
}

void UAnomalySelectorSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Dormant when disabled -> existing gates byte-identical (no input poll, no refresh, no draw).
	if (!bUIEnabled)
	{
		return;
	}

	// Input thin-shell: map raw keys -> the explicit-core methods.
	PollInput();

	// Throttled visible-set refresh so the HUD list stays current between key presses (Advance/Prev
	// also refresh on-demand). Only-when-enabled + ~0.1s bounds the synchronous occlusion-trace cost.
	RefreshAccumulator += DeltaTime;
	if (RefreshAccumulator >= RefreshIntervalSeconds)
	{
		RefreshVisibleSet();
	}

	// World-space highlight on the selected object (dev-only; ENABLE_DRAW_DEBUG).
	if (AActor* Actor = SelectedActor.Get())
	{
		FVector Origin = FVector::ZeroVector;
		FVector Extent = FVector::ZeroVector;
		Actor->GetActorBounds(/*bOnlyCollidingComponents=*/false, Origin, Extent);
		DrawDebugBox(GetWorld(), Origin, Extent, FColor::Yellow, /*bPersistent=*/false, /*LifeTime=*/-1.0f, /*DepthPriority=*/0, /*Thickness=*/2.0f);
	}
}

// ---------------------------------------------------------------------------
// Activation
// ---------------------------------------------------------------------------

void UAnomalySelectorSubsystem::SetUIEnabled(bool bEnabled)
{
	if (bEnabled == bUIEnabled)
	{
		UE_LOG(LogAnomaly, Log, TEXT("IAI.SelectorUI: already %s."), bEnabled ? TEXT("ON") : TEXT("OFF"));
		return;   // idempotent -> never double-registers the HUD delegate
	}

	bUIEnabled = bEnabled;
	if (bEnabled)
	{
		RegisterHUD();
		RefreshAccumulator = 0.0f;
		RefreshVisibleSet();   // populate the list immediately so the first frame already shows it
	}
	else
	{
		UnregisterHUD();
		SelectedActor.Reset();
		VisibleActors.Reset();
		LastInjectResult.Reset();
	}

	UE_LOG(LogAnomaly, Log, TEXT("IAI.SelectorUI -> %s."), bEnabled ? TEXT("ON") : TEXT("OFF"));
}

// ---------------------------------------------------------------------------
// Explicit core: selection model
// ---------------------------------------------------------------------------

void UAnomalySelectorSubsystem::AdvanceSelection()
{
	RefreshVisibleSet();   // on-demand fresh set (S4)
	if (VisibleActors.Num() == 0)
	{
		SelectedActor.Reset();
		UE_LOG(LogAnomaly, Log, TEXT("Selector.Next: no visible actors to select."));
		return;
	}

	const int32 Current = IndexOfSelected();          // INDEX_NONE (-1) when nothing selected
	const int32 Next = (Current + 1) % VisibleActors.Num();   // -1 -> 0 (first)
	SelectedActor = VisibleActors[Next];
	UE_LOG(LogAnomaly, Log, TEXT("Selector.Next: selected '%s' (%d/%d)."),
		*GetSelectedActorName(), Next + 1, VisibleActors.Num());
}

void UAnomalySelectorSubsystem::SelectPrevious()
{
	RefreshVisibleSet();
	if (VisibleActors.Num() == 0)
	{
		SelectedActor.Reset();
		UE_LOG(LogAnomaly, Log, TEXT("Selector.Prev: no visible actors to select."));
		return;
	}

	int32 Current = IndexOfSelected();
	if (Current < 0)
	{
		Current = 0;   // no selection -> step back from the first -> wraps to the last
	}
	const int32 Prev = (Current - 1 + VisibleActors.Num()) % VisibleActors.Num();
	SelectedActor = VisibleActors[Prev];
	UE_LOG(LogAnomaly, Log, TEXT("Selector.Prev: selected '%s' (%d/%d)."),
		*GetSelectedActorName(), Prev + 1, VisibleActors.Num());
}

void UAnomalySelectorSubsystem::CycleAnomalyChoice()
{
	AnomalyChoiceIndex = (AnomalyChoiceIndex + 1) % GNumAnomalyChoices;
	UE_LOG(LogAnomaly, Log, TEXT("Selector.Cycle: anomaly -> '%s'."), *GetAnomalyChoice().ToString());
}

bool UAnomalySelectorSubsystem::InjectSelected()
{
	AActor* Actor = SelectedActor.Get();
	if (!Actor)
	{
		UE_LOG(LogAnomaly, Warning, TEXT("Selector.Inject: nothing selected."));
		return false;
	}

	UWorld* World = GetWorld();
	UAnomalyInjectorSubsystem* Injector = World ? World->GetSubsystem<UAnomalyInjectorSubsystem>() : nullptr;
	if (!Injector)
	{
		UE_LOG(LogAnomaly, Warning, TEXT("Selector.Inject: injector subsystem not present."));
		return false;
	}

	const FName Id = GetAnomalyChoice();
	// "=" exact-match sentinel: target ONLY this actor, never a same-prefixed sibling (S1 / verify-5).
	const FString Token = FString(TEXT("=")) + Actor->GetName();
	const bool bApplied = Injector->ApplyAnomaly(Id, TArray<FString>{ Token });
	if (bApplied)
	{
		LastInjectedId = Id;
		LastInjectResult = FString::Printf(TEXT("inject %s on %s: applied"), *Id.ToString(), *Actor->GetName());
	}
	else
	{
		// AMB-2 zero-match (e.g. an LOD anomaly on a pure-VFX actor): surface it on the HUD (R4), not just the log.
		LastInjectResult = FString::Printf(TEXT("inject %s on %s: 0 matched (try a mesh / different anomaly)"),
			*Id.ToString(), *Actor->GetName());
	}
	UE_LOG(LogAnomaly, Log, TEXT("Selector.Inject: '%s' on '%s' -> %s."),
		*Id.ToString(), *Actor->GetName(), bApplied ? TEXT("applied") : TEXT("not applied"));
	return bApplied;
}

bool UAnomalySelectorSubsystem::RevertSelected()
{
	if (LastInjectedId.IsNone())
	{
		LastInjectResult = TEXT("revert: nothing injected by the selector yet");
		UE_LOG(LogAnomaly, Log, TEXT("Selector.Revert: nothing injected by the selector yet."));
		return false;
	}

	UWorld* World = GetWorld();
	UAnomalyInjectorSubsystem* Injector = World ? World->GetSubsystem<UAnomalyInjectorSubsystem>() : nullptr;
	if (!Injector)
	{
		UE_LOG(LogAnomaly, Warning, TEXT("Selector.Revert: injector subsystem not present."));
		return false;
	}

	const FString RevertedId = LastInjectedId.ToString();
	const bool bReverted = Injector->RevertAnomaly(LastInjectedId);
	LastInjectResult = FString::Printf(TEXT("revert %s: %s"), *RevertedId, bReverted ? TEXT("reverted") : TEXT("not active"));
	UE_LOG(LogAnomaly, Log, TEXT("Selector.Revert: '%s' -> %s."),
		*RevertedId, bReverted ? TEXT("reverted") : TEXT("not active"));
	if (bReverted)
	{
		LastInjectedId = NAME_None;
	}
	return bReverted;
}

// ---------------------------------------------------------------------------
// Explicit core: readbacks
// ---------------------------------------------------------------------------

FString UAnomalySelectorSubsystem::GetSelectedActorName() const
{
	const AActor* Actor = SelectedActor.Get();
	return Actor ? Actor->GetName() : FString(TEXT("(none)"));
}

TArray<FString> UAnomalySelectorSubsystem::GetVisibleActorNames() const
{
	TArray<FString> Names;
	Names.Reserve(VisibleActors.Num());
	for (const TWeakObjectPtr<AActor>& Weak : VisibleActors)
	{
		if (const AActor* Actor = Weak.Get())
		{
			Names.Add(Actor->GetName());
		}
	}
	return Names;
}

FName UAnomalySelectorSubsystem::GetAnomalyChoice() const
{
	return GAnomalyChoices[AnomalyChoiceIndex];
}

void UAnomalySelectorSubsystem::LogStatus() const
{
	const TArray<FString> Names = GetVisibleActorNames();
	UE_LOG(LogAnomaly, Log, TEXT("--- IAI.Selector.Status ---"));
	UE_LOG(LogAnomaly, Log, TEXT("  UI: %s | anomaly: '%s' | selected: '%s'"),
		bUIEnabled ? TEXT("ON") : TEXT("OFF"), *GetAnomalyChoice().ToString(), *GetSelectedActorName());
	UE_LOG(LogAnomaly, Log, TEXT("  visible (%d):"), Names.Num());
	for (const FString& Name : Names)
	{
		UE_LOG(LogAnomaly, Log, TEXT("    %s%s"),
			(SelectedActor.IsValid() && Name == GetSelectedActorName()) ? TEXT("> ") : TEXT("  "), *Name);
	}
	UE_LOG(LogAnomaly, Log, TEXT("--- %d visible actor(s) ---"), Names.Num());
}

// ---------------------------------------------------------------------------
// Configurable keybinds
// ---------------------------------------------------------------------------

bool UAnomalySelectorSubsystem::SetKeyBinding(FName Action, FKey Key)
{
	if      (Action == FName(TEXT("next")))   { KeyNext = Key; }
	else if (Action == FName(TEXT("prev")))   { KeyPrev = Key; }
	else if (Action == FName(TEXT("cycle")))  { KeyCycle = Key; }
	else if (Action == FName(TEXT("inject"))) { KeyInject = Key; }
	else if (Action == FName(TEXT("revert"))) { KeyRevert = Key; }
	else
	{
		UE_LOG(LogAnomaly, Warning, TEXT("IAI.SelectorBind: unknown action '%s' (use next/prev/cycle/inject/revert)."),
			*Action.ToString());
		return false;
	}

	UE_LOG(LogAnomaly, Log, TEXT("IAI.SelectorBind: '%s' -> '%s'."), *Action.ToString(), *Key.ToString());
	return true;
}

// ---------------------------------------------------------------------------
// Thin shells + internals
// ---------------------------------------------------------------------------

void UAnomalySelectorSubsystem::PollInput()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	APlayerController* PC = World->GetFirstPlayerController();   // reachable each tick (gotcha G23)
	if (!PC)
	{
		return;   // no local PC (e.g. very early frames) -> nothing to poll
	}

	const bool bShift = PC->IsInputKeyDown(EKeys::LeftShift) || PC->IsInputKeyDown(EKeys::RightShift);

	// Next / Prev. Default prev gesture = Shift + KeyNext; a dedicated KeyPrev (if rebound) also works.
	if (KeyNext.IsValid() && PC->WasInputKeyJustPressed(KeyNext))
	{
		if (bShift) { SelectPrevious(); } else { AdvanceSelection(); }
	}
	else if (KeyPrev.IsValid() && PC->WasInputKeyJustPressed(KeyPrev))
	{
		SelectPrevious();
	}

	if (KeyCycle.IsValid()  && PC->WasInputKeyJustPressed(KeyCycle))  { CycleAnomalyChoice(); }
	if (KeyInject.IsValid() && PC->WasInputKeyJustPressed(KeyInject)) { InjectSelected(); }
	if (KeyRevert.IsValid() && PC->WasInputKeyJustPressed(KeyRevert)) { RevertSelected(); }
}

void UAnomalySelectorSubsystem::RefreshVisibleSet()
{
	RefreshAccumulator = 0.0f;

	// Renderable-visible set (m5 follow-on): visible AND actually renders geometry (static / skeletal /
	// VFX) — non-rendering primitives (collision boxes, capsules, RVT bounds, landscape, debug/streaming
	// actors) are excluded so they are never injectable targets. Single source of truth in AnomalyViewport,
	// shared with future auto-injection. On no resolvable view it returns empty (offer nothing, never
	// blind) — deliberately distinct from the anomaly finders' treat-as-unscoped (per G23 a Play/Simulate
	// session resolves a view anyway).
	UWorld* World = GetWorld();
	VisibleActors = World ? AnomalyViewport::GetVisibleRenderableActors(World)
	                      : TArray<TWeakObjectPtr<AActor>>();

	// Name-sort for a deterministic, bridge-testable cycle order (v1). Screen-X ordering is the
	// intended next UX polish (see architecture.md / journal 009).
	VisibleActors.Sort([](const TWeakObjectPtr<AActor>& A, const TWeakObjectPtr<AActor>& B)
	{
		const AActor* AA = A.Get();
		const AActor* BB = B.Get();
		if (!AA) { return false; }
		if (!BB) { return true; }
		return AA->GetName() < BB->GetName();
	});

	// Reconcile: clear the selection if the selected actor left the visible set (or was destroyed).
	if (!SelectedActor.IsValid() || !VisibleActors.Contains(SelectedActor))
	{
		SelectedActor.Reset();
	}
}

int32 UAnomalySelectorSubsystem::IndexOfSelected() const
{
	if (!SelectedActor.IsValid())
	{
		return INDEX_NONE;
	}
	return VisibleActors.IndexOfByKey(SelectedActor);
}

void UAnomalySelectorSubsystem::RegisterHUD()
{
	if (DebugDrawHandle.IsValid())
	{
		return;   // already registered (guard against double-register)
	}
	// Register under the "Game" engine show flag, which is ON for any non-editor view
	// (ShowFlags.h: SetGame(InitMode != ESFIM_Editor ...)). Drawn by GameViewportClient::Draw with
	// ZERO dependence on the host's HUD/GameMode class (gotcha G25).
	DebugDrawHandle = UDebugDrawService::Register(
		TEXT("Game"),
		FDebugDrawDelegate::CreateUObject(this, &UAnomalySelectorSubsystem::DrawHUD));
}

void UAnomalySelectorSubsystem::UnregisterHUD()
{
	if (DebugDrawHandle.IsValid())
	{
		UDebugDrawService::Unregister(DebugDrawHandle);
		DebugDrawHandle.Reset();
	}
}

void UAnomalySelectorSubsystem::DrawHUD(UCanvas* Canvas, APlayerController* /*PC*/)
{
	if (!bUIEnabled || !Canvas)
	{
		return;
	}
	UFont* Font = GEngine ? GEngine->GetSmallFont() : nullptr;
	if (!Font)
	{
		return;
	}

	const float X = 50.0f;
	float Y = 80.0f;
	const float LineH = 16.0f;

	Canvas->SetDrawColor(FColor::White);
	Canvas->DrawText(Font, TEXT("[IAI] Object Selector  —  Tab / Shift+Tab: cycle object   C: anomaly   G: inject   H: revert"), X, Y);
	Y += LineH * 1.5f;

	// List 1 — visible actors (selected one marked).
	Canvas->SetDrawColor(FColor::White);
	Canvas->DrawText(Font, FString::Printf(TEXT("Visible objects (%d):"), VisibleActors.Num()), X, Y);
	Y += LineH;

	int32 Listed = 0;
	for (const TWeakObjectPtr<AActor>& Weak : VisibleActors)
	{
		const AActor* Actor = Weak.Get();
		if (!Actor)
		{
			continue;
		}
		if (Listed >= GMaxListedActors)
		{
			Canvas->SetDrawColor(FColor(140, 140, 140));
			Canvas->DrawText(Font, FString::Printf(TEXT("   … (+%d more)"), VisibleActors.Num() - Listed), X, Y);
			Y += LineH;
			break;
		}
		const bool bSel = (Weak == SelectedActor);
		Canvas->SetDrawColor(bSel ? FColor::Yellow : FColor(180, 180, 180));
		Canvas->DrawText(Font, FString::Printf(TEXT("  %s %s"), bSel ? TEXT(">") : TEXT(" "), *Actor->GetName()), X, Y);
		Y += LineH;
		++Listed;
	}

	Y += LineH * 0.5f;

	// List 2 — the injectable anomalies (chosen one marked).
	Canvas->SetDrawColor(FColor::White);
	Canvas->DrawText(Font, TEXT("Anomaly:"), X, Y);
	Y += LineH;
	for (int32 i = 0; i < GNumAnomalyChoices; ++i)
	{
		const bool bSel = (i == AnomalyChoiceIndex);
		Canvas->SetDrawColor(bSel ? FColor::Green : FColor(180, 180, 180));
		Canvas->DrawText(Font, FString::Printf(TEXT("  %s %s"), bSel ? TEXT(">") : TEXT(" "), *GAnomalyChoices[i].ToString()), X, Y);
		Y += LineH;
	}

	// Last inject/revert result (R4) — surfaces the AMB-2 zero-match case in real Play, not log-only.
	if (!LastInjectResult.IsEmpty())
	{
		Y += LineH * 0.5f;
		Canvas->SetDrawColor(FColor(120, 200, 255));
		Canvas->DrawText(Font, FString::Printf(TEXT("Last: %s"), *LastInjectResult), X, Y);
		Y += LineH;
	}

	// On-screen name label anchored to the selected actor.
	if (const AActor* Actor = SelectedActor.Get())
	{
		const FVector Screen = Canvas->Project(Actor->GetActorLocation());
		if (Screen.Z > 0.0f)   // in front of the camera
		{
			Canvas->SetDrawColor(FColor::Yellow);
			Canvas->DrawText(Font, Actor->GetName(), Screen.X, Screen.Y);
		}
	}
}

// ---------------------------------------------------------------------------
// Console command surface (the bridge thin-shell -> the explicit-core methods)
//
// Module-scoped FAutoConsoleCommandWithWorldAndArgs, mirroring the injector's pattern: each resolves
// the selector subsystem from the console's world and null-guards outside Game/PIE.
// ---------------------------------------------------------------------------

namespace
{
	UAnomalySelectorSubsystem* ResolveSelector(UWorld* World)
	{
		if (!World)
		{
			UE_LOG(LogAnomaly, Warning, TEXT("No world for this command; run it from a game/PIE world."));
			return nullptr;
		}
		UAnomalySelectorSubsystem* Selector = World->GetSubsystem<UAnomalySelectorSubsystem>();
		if (!Selector)
		{
			UE_LOG(LogAnomaly, Warning,
				TEXT("AnomalySelector subsystem not present for world '%s' (only active in Game/PIE worlds)."),
				*GetNameSafe(World));
		}
		return Selector;
	}
}

static FAutoConsoleCommandWithWorldAndArgs GSelectorUICmd(
	TEXT("IAI.SelectorUI"),
	TEXT("Enable/disable the in-game object-selector + inject UI (default OFF). Usage: IAI.SelectorUI <0|1>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() == 0)
			{
				UE_LOG(LogAnomaly, Warning, TEXT("Usage: IAI.SelectorUI <0|1>"));
				return;
			}
			if (UAnomalySelectorSubsystem* Selector = ResolveSelector(World))
			{
				Selector->SetUIEnabled(FCString::Atoi(*Args[0]) != 0);
			}
		}));

static FAutoConsoleCommandWithWorldAndArgs GSelectorNextCmd(
	TEXT("IAI.Selector.Next"),
	TEXT("Select the next visible actor (name-sorted)."),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (UAnomalySelectorSubsystem* Selector = ResolveSelector(World))
			{
				Selector->AdvanceSelection();
			}
		}));

static FAutoConsoleCommandWithWorldAndArgs GSelectorPrevCmd(
	TEXT("IAI.Selector.Prev"),
	TEXT("Select the previous visible actor (name-sorted)."),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (UAnomalySelectorSubsystem* Selector = ResolveSelector(World))
			{
				Selector->SelectPrevious();
			}
		}));

static FAutoConsoleCommandWithWorldAndArgs GSelectorCycleCmd(
	TEXT("IAI.Selector.Cycle"),
	TEXT("Cycle the chosen anomaly (missing_object/flicker/missing_texture)."),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (UAnomalySelectorSubsystem* Selector = ResolveSelector(World))
			{
				Selector->CycleAnomalyChoice();
			}
		}));

static FAutoConsoleCommandWithWorldAndArgs GSelectorInjectCmd(
	TEXT("IAI.Selector.Inject"),
	TEXT("Inject the chosen anomaly on the selected actor (exact-name target, default args)."),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (UAnomalySelectorSubsystem* Selector = ResolveSelector(World))
			{
				Selector->InjectSelected();
			}
		}));

static FAutoConsoleCommandWithWorldAndArgs GSelectorRevertCmd(
	TEXT("IAI.Selector.Revert"),
	TEXT("Revert the last anomaly the selector injected."),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (UAnomalySelectorSubsystem* Selector = ResolveSelector(World))
			{
				Selector->RevertSelected();
			}
		}));

static FAutoConsoleCommandWithWorldAndArgs GSelectorStatusCmd(
	TEXT("IAI.Selector.Status"),
	TEXT("Log the selected actor, the visible-set names, and the chosen anomaly."),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (UAnomalySelectorSubsystem* Selector = ResolveSelector(World))
			{
				Selector->LogStatus();
			}
		}));

static FAutoConsoleCommandWithWorldAndArgs GSelectorBindCmd(
	TEXT("IAI.SelectorBind"),
	TEXT("Rebind a selector key. Usage: IAI.SelectorBind <next|prev|cycle|inject|revert> <KeyName> (e.g. Tab, C, F1)"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() < 2)
			{
				UE_LOG(LogAnomaly, Warning, TEXT("Usage: IAI.SelectorBind <next|prev|cycle|inject|revert> <KeyName>"));
				return;
			}
			const FName KeyName(*Args[1]);
			const FKey Key(KeyName);
			if (!EKeys::GetKeyDetails(Key).IsValid())
			{
				UE_LOG(LogAnomaly, Warning, TEXT("IAI.SelectorBind: '%s' is not a known key."), *Args[1]);
				return;
			}
			if (UAnomalySelectorSubsystem* Selector = ResolveSelector(World))
			{
				Selector->SetKeyBinding(FName(*Args[0]), Key);
			}
		}));
