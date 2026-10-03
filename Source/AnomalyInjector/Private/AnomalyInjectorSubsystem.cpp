#include "AnomalyInjectorSubsystem.h"
#include "Engine/EngineBaseTypes.h"
#if __has_include("Stats/LightweightStats.h")
#include "Stats/LightweightStats.h"
#endif
#include "Stats/Stats.h"
#if __has_include("Stats/StatsSystemTypes.h")
#include "Stats/StatsSystemTypes.h"
#endif
#include "Subsystems/SubsystemCollection.h"
#include "AnomalyInjectorLog.h"
#include "AnomalyHiddenClass.h"
#include "AnomalyBenchGate.h"

#include "EngineUtils.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "HAL/IConsoleManager.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"

#include "AnomalyDefaults.h"
#include "AnomalyViewport.h"
#include "AnomalyTargeting.h"
#include "AnomalyTargetPolicy.h"
#include "Components/PrimitiveComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/PlayerController.h"
#include "MaterialShared.h"
#include "SceneTypes.h"

#include "Anomalies/Anomaly_MissingObject.h"
#include "Anomalies/Anomaly_Blinking.h"
#include "Anomalies/Anomaly_TimeDilation.h"
#include "Anomalies/Anomaly_LightingMismatch.h"
#include "Anomalies/Anomaly_LodCorruption.h"
#include "Anomalies/Anomaly_LodPopping.h"
#include "Anomalies/Anomaly_CameraClipping.h"
#include "Anomalies/Anomaly_MissingTexture.h"
#include "Anomalies/Anomaly_CorruptedTexture.h"
#include "Anomalies/Anomaly_StuckLowMip.h"
#include "Anomalies/Anomaly_ChangeCase.h"
#include "Anomalies/Anomaly_TexCorrupt.h"
#include "AnomalyTexCorrupt.h"
#include "Engine/Texture2D.h"

static constexpr uint64 GAnomalyHeartbeatKey = 0x47445048;

#if !UE_BUILD_SHIPPING
namespace
{
	bool GBenchDestroyArmed = false;
	FString GBenchDestroyQuery;
	int32 GBenchDestroyHoldFrames = 2;
	int32 GBenchDestroyAnomalousTicks = 0;
}
#endif

UAnomalyInjectorSubsystem::~UAnomalyInjectorSubsystem() = default;

UAnomalyInjectorSubsystem::UAnomalyInjectorSubsystem()
{
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> CheckerFinder(
		TEXT("/AnomalyInjector/Materials/M_MissingTexture_Checker.M_MissingTexture_Checker"));
	MissingTextureChecker = CheckerFinder.Object;

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> PinkFinder(
		TEXT("/AnomalyInjector/Materials/M_CorruptedTexture_Pink.M_CorruptedTexture_Pink"));
	CorruptedTexturePink = PinkFinder.Object;

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> TexCorruptUvFinder(
		TEXT("/AnomalyInjector/Materials/M_CorruptTex_UV.M_CorruptTex_UV"));
	TexCorruptUv = TexCorruptUvFinder.Object;

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> TexCorruptNormalFinder(
		TEXT("/AnomalyInjector/Materials/M_CorruptTex_Normal.M_CorruptTex_Normal"));
	TexCorruptNormal = TexCorruptNormalFinder.Object;

	static ConstructorHelpers::FObjectFinder<UTexture2D> TexCorruptNoiseFinder(
		TEXT("/AnomalyInjector/Textures/T_CorruptTex_NoiseN.T_CorruptTex_NoiseN"));
	TexCorruptNoise = TexCorruptNoiseFinder.Object;
}

UMaterialInterface* UAnomalyInjectorSubsystem::GetMissingTextureMaterial() const
{
	return MissingTextureChecker;
}

UMaterialInterface* UAnomalyInjectorSubsystem::GetCorruptedTextureMaterial() const
{
	return CorruptedTexturePink;
}

UMaterialInterface* UAnomalyInjectorSubsystem::GetTexCorruptUvMaterial() const
{
	return TexCorruptUv;
}

UMaterialInterface* UAnomalyInjectorSubsystem::GetTexCorruptNormalMaterial() const
{
	return TexCorruptNormal;
}

UTexture2D* UAnomalyInjectorSubsystem::GetTexCorruptNoiseTexture() const
{
	return TexCorruptNoise;
}

void UAnomalyInjectorSubsystem::TexCorruptHold(UObject* Object)
{
	if (Object)
	{
		TexCorruptStrongRefs.Add(Object);
	}
}

void UAnomalyInjectorSubsystem::TexCorruptLetGo(UObject* Object)
{
	if (Object)
	{
		TexCorruptStrongRefs.RemoveSingleSwap(Object);
	}
}

namespace
{
	FAnomalyArgSpec FloatArg(const TCHAR* Name, const TCHAR* Default, double Min, double Max, bool bRequired = false)
	{
		FAnomalyArgSpec A;
		A.Name = Name; A.Type = EAnomalyArgType::Float; A.Default = Default; A.bRequired = bRequired;
		A.bHasMin = true; A.Min = Min; A.bHasMax = true; A.Max = Max;
		return A;
	}
	FAnomalyArgSpec IntArg(const TCHAR* Name, const TCHAR* Default, double Min, bool bRequired = false)
	{
		FAnomalyArgSpec A;
		A.Name = Name; A.Type = EAnomalyArgType::Int; A.Default = Default; A.bRequired = bRequired;
		A.bHasMin = true; A.Min = Min;
		return A;
	}
	FAnomalyArgSpec StringArg(const TCHAR* Name, const TCHAR* Default)
	{
		FAnomalyArgSpec A;
		A.Name = Name; A.Type = EAnomalyArgType::String; A.Default = Default;
		return A;
	}

	bool IsTargetableId(const FName& Id, EAnomalyScope Scope)
	{
		if (Scope != EAnomalyScope::Global)
		{
			return true;
		}
		return Id == FName(TEXT("camera_clipping"));
	}

	void GetAuthoredSpec(const FName& Id, EAnomalyScope& OutScope, TArray<FAnomalyArgSpec>& OutArgs)
	{
		OutArgs.Reset();
		if (Id == FName(TEXT("missing_object")))
		{
			OutScope = EAnomalyScope::Object;
		}
		else if (Id == FName(TEXT("blinking")))
		{
			OutScope = EAnomalyScope::Object;
			OutArgs.Add(IntArg(TEXT("half_period_frames"), TEXT("3"), 1.0));
		}
		else if (Id == FName(TEXT("lod_corruption")))
		{
			OutScope = EAnomalyScope::Object;
			OutArgs.Add(IntArg(TEXT("lod-index"), TEXT(""), 1.0));
		}
		else if (Id == FName(TEXT("lod_popping")))
		{
			OutScope = EAnomalyScope::Object;
			OutArgs.Add(IntArg(TEXT("half_period_frames"), TEXT("8"), 1.0));
		}
		else if (Id == FName(TEXT("time_dilation")))
		{
			OutScope = EAnomalyScope::Global;
			OutArgs.Add(FloatArg(TEXT("scale"), TEXT("0.5"), 0.0, 20.0,  true));
		}
		else if (Id == FName(TEXT("camera_clipping")))
		{
			OutScope = EAnomalyScope::Global;
			OutArgs.Add(FloatArg(TEXT("near"), TEXT("100"), 1.0, 100000.0));
		}
		else if (Id == FName(TEXT("lighting_mismatch")))
		{
			OutScope = EAnomalyScope::Component;
			FAnomalyArgSpec Mode;
			Mode.Name = TEXT("mode"); Mode.Type = EAnomalyArgType::Enum; Mode.Default = TEXT("dim");
			Mode.Options = { TEXT("off"), TEXT("dim"), TEXT("recolor"), TEXT("noshadow") };
			OutArgs.Add(Mode);
			OutArgs.Add(StringArg(TEXT("params"), TEXT("")));
		}
		else if (Id == FName(TEXT("missing_texture")))
		{
			OutScope = EAnomalyScope::Object;
		}
		else if (Id == FName(TEXT("corrupted_texture")))
		{
			OutScope = EAnomalyScope::Object;
		}
		else if (Id == FName(TEXT("null_effect")) || Id == FName(TEXT("solid_swap")))
		{
			OutScope = EAnomalyScope::Object;
			OutArgs.Add(IntArg(TEXT("delay"), TEXT("0"), 0));
		}
		else if (Id == FName(TEXT("stuck_low_mip")))
		{
			OutScope = EAnomalyScope::Object;
			OutArgs.Add(IntArg(TEXT("mip_levels"), TEXT("-1"), (double)AnomalyDefaults::StuckMipLevelsMin));
		}
		else if (Id == FName(TEXT("uv_corruption")))
		{
			OutScope = EAnomalyScope::Object;
			FAnomalyArgSpec Mode;
			Mode.Name = TEXT("mode"); Mode.Type = EAnomalyArgType::Enum; Mode.Default = TEXT("tile"); Mode.bRequired = true;
			Mode.Options = { TEXT("tile"), TEXT("scramble") };
			OutArgs.Add(Mode);
		}
		else if (Id == FName(TEXT("normal_corruption")))
		{
			OutScope = EAnomalyScope::Object;
			FAnomalyArgSpec Mode;
			Mode.Name = TEXT("mode"); Mode.Type = EAnomalyArgType::Enum; Mode.Default = TEXT("invert"); Mode.bRequired = true;
			Mode.Options = { TEXT("invert"), TEXT("green_flip") };
			OutArgs.Add(Mode);
		}
		else
		{
			OutScope = EAnomalyScope::Object;
			UE_LOG(LogAnomaly, Warning, TEXT("GetAnomalyCatalog: no authored arg-spec for '%s' (default object/no-args)."), *Id.ToString());
		}
	}
}


void UAnomalyInjectorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	auto Register = [this](TUniquePtr<IAnomaly> Anomaly)
	{
		const FName Id = Anomaly->GetId();
		Anomalies.Add(Id, MoveTemp(Anomaly));
	};
	Register(MakeUnique<FAnomaly_MissingObject>());
	Register(MakeUnique<FAnomaly_Blinking>());
	Register(MakeUnique<FAnomaly_TimeDilation>());
	Register(MakeUnique<FAnomaly_LightingMismatch>());
	Register(MakeUnique<FAnomaly_LodCorruption>());
	Register(MakeUnique<FAnomaly_LodPopping>());
	Register(MakeUnique<FAnomaly_CameraClipping>());
	Register(MakeUnique<FAnomaly_MissingTexture>());
	Register(MakeUnique<FAnomaly_CorruptedTexture>());
	Register(MakeUnique<FAnomaly_StuckLowMip>());
	Register(MakeUnique<AnomalyTexCorrupt::FAnomaly_TexCorrupt>(FName(TEXT("uv_corruption")), AnomalyTexCorrupt::EFamily::UV));
	Register(MakeUnique<AnomalyTexCorrupt::FAnomaly_TexCorrupt>(FName(TEXT("normal_corruption")), AnomalyTexCorrupt::EFamily::Normal));
#if !UE_BUILD_SHIPPING
	if (AnomalyBenchGate::IsEnabled())
	{
		Register(MakeUnique<FAnomaly_ChangeCase>(FName(TEXT("null_effect")), false));
		Register(MakeUnique<FAnomaly_ChangeCase>(FName(TEXT("solid_swap")), true));
	}
#endif
	AnomalyBenchGate::SweepUngatedLevers(TEXT("subsystem_init"));

	SynthPreActorTickHandle = FWorldDelegates::OnWorldPreActorTick.AddUObject(
		this, &UAnomalyInjectorSubsystem::OnWorldPreActorTickSynth);

	UE_LOG(LogAnomaly, Log, TEXT("Subsystem initialized for world '%s'. %d anomaly type(s) registered."),
		*GetNameSafe(GetWorld()), Anomalies.Num());

	UE_LOG(LogAnomaly, Log,
		TEXT("TEXCORRUPT-ASSETS M_CorruptTex_UV resolved=%d (%s); M_CorruptTex_Normal resolved=%d (%s); T_CorruptTex_NoiseN ")
		TEXT("resolved=%d (%s). m53's corruptors and noise texture are hard references on this subsystem's CDO, so they cook with ")
		TEXT("the plugin. Any 0 here means every uv_corruption / normal_corruption fire is REFUSED assets_unavailable: the ")
		TEXT("anomaly fails closed and never draws without them."),
		TexCorruptUv ? 1 : 0, *GetPathNameSafe(TexCorruptUv), TexCorruptNormal ? 1 : 0, *GetPathNameSafe(TexCorruptNormal),
		TexCorruptNoise ? 1 : 0, *GetPathNameSafe(TexCorruptNoise));
}

void UAnomalyInjectorSubsystem::Deinitialize()
{
	if (SynthPreActorTickHandle.IsValid())
	{
		FWorldDelegates::OnWorldPreActorTick.Remove(SynthPreActorTickHandle);
		SynthPreActorTickHandle.Reset();
	}
	const int32 Reverted = RevertAllActive();
	if (Reverted > 0)
	{
		UE_LOG(LogAnomaly, Log, TEXT("Subsystem deinitializing; reverted %d active anomaly(ies)."), Reverted);
	}

	for (const FTargetWatch& W : TargetWatches)
	{
		if (AActor* Actor = W.Actor.Get())
		{
			Actor->OnEndPlay.RemoveDynamic(this, &UAnomalyInjectorSubsystem::OnWatchedTargetEndPlay);
		}
	}
	TargetWatches.Reset();

	for (const TPair<FName, TUniquePtr<IAnomaly>>& Pair : Anomalies)
	{
		if (Pair.Value)
		{
			Pair.Value->OnWorldTeardown();
		}
	}

	AnomalyTexCorrupt::RestoreBenchAssetSlotMid(TEXT("world teardown"));
	AnomalyTexCorrupt::EndWarmDraw();
	TexCorruptStrongRefs.Reset();
	TrailProvider.Reset();

	Super::Deinitialize();
}

#if !UE_BUILD_SHIPPING
void UAnomalyInjectorSubsystem::ServiceBenchDestroyLatch()
{
	if (!GBenchDestroyArmed)
	{
		return;
	}

	bool bAnyAnomalous = false;
	for (const TPair<FName, TUniquePtr<IAnomaly>>& Pair : Anomalies)
	{
		if (Pair.Value && Pair.Value->IsActive() && Pair.Value->IsCurrentlyAnomalous())
		{
			bAnyAnomalous = true;
			break;
		}
	}
	if (!bAnyAnomalous)
	{
		return;
	}

	++GBenchDestroyAnomalousTicks;
	if (GBenchDestroyAnomalousTicks <= GBenchDestroyHoldFrames)
	{
		return;
	}

	const TArray<TWeakObjectPtr<AActor>> Matches = AnomalyTargeting::FindActorsMatching(GetWorld(), GBenchDestroyQuery);
	AActor* Victim = nullptr;
	for (const TWeakObjectPtr<AActor>& Weak : Matches)
	{
		if (AActor* Candidate = Weak.Get())
		{
			Victim = Candidate;
			break;
		}
	}

	if (!Victim)
	{
		UE_LOG(LogAnomaly, Warning,
			TEXT("IAI.Bench.DestroyTarget: the latch fired after %d anomalous tick(s) but '%s' matched NO live actor, ")
			TEXT("so NOTHING WAS DESTROYED and the latch stays armed. A lever that silently matches nothing is a clean ")
			TEXT("null, so this says so instead."),
			GBenchDestroyAnomalousTicks, *GBenchDestroyQuery);
		return;
	}

	GBenchDestroyArmed = false;
	const FString VictimName = Victim->GetName();
	UE_LOG(LogAnomaly, Warning,
		TEXT("IAI.Bench.DestroyTarget: DESTROYING '%s' now - it matched '%s' and an anomaly has been measurably ")
		TEXT("anomalous for %d tick(s), so this destruction lands MID-WINDOW, which is the only condition that tests ")
		TEXT("the target-destroyed exit. BENCH DEVICE: it exists so the exit path can be MEASURED rather than argued ")
		TEXT("for structurally."),
		*VictimName, *GBenchDestroyQuery, GBenchDestroyAnomalousTicks);
	Victim->Destroy();
	UE_LOG(LogAnomaly, Warning,
		TEXT("IAI.Bench.DestroyTarget: Destroy() returned for '%s'; the latch is now DISARMED."), *VictimName);
}
#endif

void UAnomalyInjectorSubsystem::WatchTargetForAnomaly(AActor* Actor, const FName& Id)
{
	if (!Actor)
	{
		return;
	}
	for (const FTargetWatch& W : TargetWatches)
	{
		if (W.Actor.Get() == Actor && W.AnomalyId == Id)
		{
			return;
		}
	}
	Actor->OnEndPlay.AddDynamic(this, &UAnomalyInjectorSubsystem::OnWatchedTargetEndPlay);
	FTargetWatch& New = TargetWatches.AddDefaulted_GetRef();
	New.Actor = Actor;
	New.AnomalyId = Id;
}

void UAnomalyInjectorSubsystem::ClearTargetWatchForAnomaly(const FName& Id)
{
	for (int32 i = TargetWatches.Num() - 1; i >= 0; --i)
	{
		if (TargetWatches[i].AnomalyId != Id)
		{
			continue;
		}
		AActor* Actor = TargetWatches[i].Actor.Get();
		TargetWatches.RemoveAt(i);

		if (!Actor)
		{
			continue;
		}
		int32 StillWatched = 0;
		for (const FTargetWatch& W : TargetWatches)
		{
			if (W.Actor.Get() == Actor)
			{
				++StillWatched;
			}
		}
		if (StillWatched == 0)
		{
			Actor->OnEndPlay.RemoveDynamic(this, &UAnomalyInjectorSubsystem::OnWatchedTargetEndPlay);
		}
	}
}

void UAnomalyInjectorSubsystem::OnWatchedTargetEndPlay(AActor* Actor, EEndPlayReason::Type EndPlayReason)
{
	if (!Actor)
	{
		return;
	}

	const bool bWorldEnding =
		EndPlayReason == EEndPlayReason::LevelTransition ||
		EndPlayReason == EEndPlayReason::EndPlayInEditor ||
		EndPlayReason == EEndPlayReason::Quit;

	TArray<FName> Ids;
	for (int32 i = TargetWatches.Num() - 1; i >= 0; --i)
	{
		if (TargetWatches[i].Actor.Get() == Actor)
		{
			Ids.AddUnique(TargetWatches[i].AnomalyId);
			TargetWatches.RemoveAt(i);
		}
	}

	Actor->OnEndPlay.RemoveDynamic(this, &UAnomalyInjectorSubsystem::OnWatchedTargetEndPlay);

	for (const FName& Id : Ids)
	{
		TUniquePtr<IAnomaly>* Found = Anomalies.Find(Id);
		if (Found && Found->IsValid())
		{
			(*Found)->OnTargetLost(Actor, bWorldEnding);
		}
	}
}

bool UAnomalyInjectorSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

TStatId UAnomalyInjectorSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UAnomalyInjectorSubsystem, STATGROUP_Tickables);
}

void UAnomalyInjectorSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GEngine && AnomalyViewport::AreOverlaysSuppressed())
	{
		GEngine->RemoveOnScreenDebugMessage(GAnomalyHeartbeatKey);
	}

	if (!bSynthTickOrder)
	{
		DispatchAnomalyTicks(DeltaTime);
	}

#if !UE_BUILD_SHIPPING
	ServiceBenchDestroyLatch();
#endif

	HeartbeatAccumulator += DeltaTime;
	if (HeartbeatAccumulator >= 2.0f)
	{
		HeartbeatAccumulator = 0.0f;
		const int32 ActiveCount = GetActiveAnomalyCount();
		if (GEngine && !AnomalyViewport::AreOverlaysSuppressed())
		{
			GEngine->AddOnScreenDebugMessage(
				GAnomalyHeartbeatKey,
				2.5f,
				FColor::Green,
				FString::Printf(TEXT("[IAI] AnomalyInjector ticking (active: %d/%d, scoping: %s)"),
					ActiveCount, Anomalies.Num(), bViewportScopingEnabled ? TEXT("ON") : TEXT("OFF")));
		}
		UE_LOG(LogAnomaly, Verbose, TEXT("Heartbeat; active anomalies: %d/%d; scoping: %s"),
			ActiveCount, Anomalies.Num(), bViewportScopingEnabled ? TEXT("ON") : TEXT("OFF"));
	}
}


void UAnomalyInjectorSubsystem::DispatchAnomalyTicks(float DeltaTime)
{
	for (const TPair<FName, TUniquePtr<IAnomaly>>& Pair : Anomalies)
	{
		if (Pair.Value && Pair.Value->IsActive())
		{
			Pair.Value->Tick(DeltaTime);
		}
	}
	for (const TPair<FName, TUniquePtr<IAnomaly>>& Pair : Anomalies)
	{
		if (Pair.Value)
		{
			Pair.Value->TickAlways(DeltaTime);
		}
	}
}

void UAnomalyInjectorSubsystem::OnWorldPreActorTickSynth(UWorld* World, ELevelTick TickType, float DeltaSeconds)
{
	if (!bSynthTickOrder || World != GetWorld())
	{
		return;
	}
	DispatchAnomalyTicks(DeltaSeconds);
}

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

void UAnomalyInjectorSubsystem::TestVisibility(const TArray<FString>& Args) const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const FString Substring = Args[0];

	FAnomalyViewInfo View;
	View.Origin   = FVector(FCString::Atod(*Args[1]), FCString::Atod(*Args[2]), FCString::Atod(*Args[3]));
	View.Rotation = FRotator(FCString::Atod(*Args[4]), FCString::Atod(*Args[5]), FCString::Atod(*Args[6]));
	View.HorizontalFOVDeg = Args.IsValidIndex(7) ? FCString::Atof(*Args[7]) : 90.0f;
	View.AspectRatio      = Args.IsValidIndex(8) ? FCString::Atof(*Args[8]) : (16.0f / 9.0f);
	View.bValid = true;

	UE_LOG(LogAnomaly, Log, TEXT("--- IAI.TestVisibility '%s' from O=(%s) R=(%s) fov=%.1f aspect=%.3f ---"),
		*Substring, *View.Origin.ToString(), *View.Rotation.ToString(), View.HorizontalFOVDeg, View.AspectRatio);

	const TArray<TWeakObjectPtr<AActor>> Actors = AnomalyTargeting::FindActorsMatching(World, Substring);
	int32 TotalComps = 0;
	int32 VisibleComps = 0;
	for (const TWeakObjectPtr<AActor>& Weak : Actors)
	{
		AActor* Actor = Weak.Get();
		if (!Actor)
		{
			continue;
		}

		TArray<UPrimitiveComponent*> Prims;
		Actor->GetComponents<UPrimitiveComponent>(Prims);
		for (const UPrimitiveComponent* Prim : Prims)
		{
			if (!Prim)
			{
				continue;
			}
			const bool bFrustum = AnomalyViewport::IsComponentInFrustum(View, Prim);
			const bool bVisible = AnomalyViewport::IsComponentVisible(View, World, Prim);
			const bool bUnoccluded = bFrustum && bVisible;
			++TotalComps;
			if (bVisible)
			{
				++VisibleComps;
			}
			UE_LOG(LogAnomaly, Log, TEXT("  '%s' (actor '%s') frustum=%d unoccluded=%d visible=%d"),
				*Prim->GetName(), *Actor->GetName(), bFrustum ? 1 : 0, bUnoccluded ? 1 : 0, bVisible ? 1 : 0);
		}
	}
	UE_LOG(LogAnomaly, Log, TEXT("--- IAI.TestVisibility: %d visible / %d primitive component(s) for '%s' ---"),
		VisibleComps, TotalComps, *Substring);
}


void UAnomalyInjectorSubsystem::DumpCatalog() const
{
	const TArray<FAnomalyCatalogEntry> Catalog = GetAnomalyCatalog();
	UE_LOG(LogAnomaly, Log, TEXT("--- IAI.DumpCatalog (%d) ---"), Catalog.Num());
	for (const FAnomalyCatalogEntry& Entry : Catalog)
	{
		FString ArgsStr;
		for (const FAnomalyArgSpec& Arg : Entry.Args)
		{
			FString Bounds;
			if (Arg.Type == EAnomalyArgType::Enum)
			{
				Bounds = FString::Printf(TEXT("{%s}"), *FString::Join(Arg.Options, TEXT("|")));
			}
			else if (Arg.bHasMin || Arg.bHasMax)
			{
				Bounds = FString::Printf(TEXT("[%s..%s]"),
					Arg.bHasMin ? *FString::SanitizeFloat(Arg.Min) : TEXT("-"),
					Arg.bHasMax ? *FString::SanitizeFloat(Arg.Max) : TEXT("-"));
			}
			ArgsStr += FString::Printf(TEXT(" %s:%s%s%s=%s"),
				*Arg.Name, ToString(Arg.Type), Arg.bRequired ? TEXT("!") : TEXT(""),
				*Bounds, Arg.Default.IsEmpty() ? TEXT("(none)") : *Arg.Default);
		}
		UE_LOG(LogAnomaly, Log, TEXT("  %s | scope=%s | usage='%s' | args:%s"),
			*Entry.Id.ToString(), ToString(Entry.Scope), *Entry.Usage,
			ArgsStr.IsEmpty() ? TEXT(" (none)") : *ArgsStr);
	}
	UE_LOG(LogAnomaly, Log, TEXT("--- %d entry(ies) ---"), Catalog.Num());
}

void UAnomalyInjectorSubsystem::DumpActiveAnomalies() const
{
	const TArray<FActiveAnomalyInfo> Active = GetActiveAnomalies();
	UE_LOG(LogAnomaly, Log, TEXT("--- IAI.DumpActive (%d) ---"), Active.Num());
	for (const FActiveAnomalyInfo& Info : Active)
	{
		UE_LOG(LogAnomaly, Log, TEXT("  %s | active %.2fs | args=[%s]"),
			*Info.Id.ToString(), Info.SecondsActive, *FString::Join(Info.Args, TEXT(" ")));
	}
	UE_LOG(LogAnomaly, Log, TEXT("--- %d active ---"), Active.Num());
}

void UAnomalyInjectorSubsystem::DumpVisibleRenderableInfos() const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const TArray<TWeakObjectPtr<AActor>> Actors = AnomalyViewport::GetVisibleRenderableActors(World);
	const TArray<FRenderableActorInfo> Infos = AnomalyViewport::GetVisibleRenderableActorInfos(World);

	bool bMatch = (Actors.Num() == Infos.Num());
	if (bMatch)
	{
		for (int32 i = 0; i < Actors.Num(); ++i)
		{
			if (Actors[i].Get() != Infos[i].Actor.Get())
			{
				bMatch = false;
				break;
			}
		}
	}

	UE_LOG(LogAnomaly, Log, TEXT("--- IAI.DumpVisible: infos=%d vs GetVisibleRenderableActors=%d | byte-identical(set+order): %s ---"),
		Infos.Num(), Actors.Num(), bMatch ? TEXT("MATCH") : TEXT("MISMATCH"));
	for (int32 i = 0; i < Infos.Num(); ++i)
	{
		const FRenderableActorInfo& Info = Infos[i];
		UE_LOG(LogAnomaly, Log, TEXT("  [%d] %s | asset=%s | %s | %s | %s | dist=%.0f | rect=%s [%.3f,%.3f - %.3f,%.3f]"),
			i, *Info.ActorName,
			Info.AssetName.IsEmpty() ? TEXT("(none)") : *Info.AssetName,
			Info.ComponentClass.IsEmpty() ? TEXT("(none)") : *Info.ComponentClass,
			*Info.ClassName, *Info.ComponentType, Info.Distance,
			Info.bRectValid ? TEXT("ok") : TEXT("invalid"),
			Info.ScreenMin.X, Info.ScreenMin.Y, Info.ScreenMax.X, Info.ScreenMax.Y);
	}
}


void UAnomalyInjectorSubsystem::SetViewportScoping(bool bEnabled)
{
	bViewportScopingEnabled = bEnabled;
	UE_LOG(LogAnomaly, Log, TEXT("IAI.SetViewportScoping -> %s."), bEnabled ? TEXT("ON") : TEXT("OFF"));
}

bool UAnomalyInjectorSubsystem::IsViewportScopingEnabled(UWorld* World)
{
	if (World)
	{
		if (const UAnomalyInjectorSubsystem* Subsystem = World->GetSubsystem<UAnomalyInjectorSubsystem>())
		{
			return Subsystem->bViewportScopingEnabled;
		}
	}
	return false;
}

void UAnomalyInjectorSubsystem::SetSynthTickOrder(bool bEnabled)
{
	bSynthTickOrder = bEnabled;
#if !UE_BUILD_SHIPPING
	if (bEnabled)
	{
		UE_LOG(LogAnomaly, Warning,
			TEXT("IAI.Bench.SynthTickOrder -> ON. BENCH-ONLY SYNTHESIS. The injector's anomaly dispatch now runs at ")
			TEXT("OnWorldPreActorTick, i.e. BEFORE the capture subsystem, for every world tick. This SYNTHESISES THE ")
			TEXT("SYMPTOM of a host on which the two subsystems tick in the opposite relative order; it does NOT ")
			TEXT("reproduce that host's cause and is NOT evidence about it. Labels and pixels will disagree by design. ")
			TEXT("NEVER ship a capture taken with this ON."));
	}
	else
	{
		UE_LOG(LogAnomaly, Log,
			TEXT("IAI.Bench.SynthTickOrder -> OFF. The injector's anomaly dispatch is back in its own Tick, i.e. in ")
			TEXT("whatever order this host ticks the subsystems."));
	}
#endif
}

bool UAnomalyInjectorSubsystem::IsSynthTickOrderEnabled(UWorld* World)
{
	if (World)
	{
		if (const UAnomalyInjectorSubsystem* Subsystem = World->GetSubsystem<UAnomalyInjectorSubsystem>())
		{
			return Subsystem->bSynthTickOrder;
		}
	}
	return false;
}

void UAnomalyInjectorSubsystem::SetAutoPoolSelection(bool bInAutoPool)
{
	bAutoPoolSelection = bInAutoPool;
}

bool UAnomalyInjectorSubsystem::IsAutoPoolSelection(UWorld* World)
{
	if (World)
	{
		if (const UAnomalyInjectorSubsystem* Subsystem = World->GetSubsystem<UAnomalyInjectorSubsystem>())
		{
			return Subsystem->bAutoPoolSelection;
		}
	}
	return false;
}


void UAnomalyInjectorSubsystem::ListAnomalies() const
{
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

	if (const FString* Refusal = RefusedIds.Find(Id))
	{
		UE_LOG(LogAnomaly, Warning,
			TEXT("IAI.Apply '%s' REFUSED %s - no fire is recorded and nothing is changed."),
			*Id.ToString(), **Refusal);
		return false;
	}

	{
		EAnomalyScope Scope = EAnomalyScope::Object;
		TArray<FAnomalyArgSpec> Spec;
		GetAuthoredSpec(Id, Scope, Spec);
		if (Scope == EAnomalyScope::Object && Args.Num() > 0 && !Args[0].IsEmpty() && !AnomalyDefaults::GetAllowNaniteTargets())
		{
			const TArray<TWeakObjectPtr<AActor>> Matches = AnomalyTargeting::FindActorsMatching(GetWorld(), Args[0]);
			int32 Refused = 0;
			for (const TWeakObjectPtr<AActor>& Weak : Matches)
			{
				if (AnomalyViewport::RefuseNaniteTarget(Weak.Get(), TEXT("apply")))
				{
					++Refused;
				}
			}
			if (Refused > 0)
			{
				UE_LOG(LogAnomaly, Warning,
					TEXT("IAI.Apply '%s' '%s' REFUSED %s - %d of %d matched actor(s) draw a Nanite primitive, so the target ")
					TEXT("mask could not carry them. Nothing is applied and no fire is recorded (setting %s)."),
					*Id.ToString(), *Args[0], UTF8_TO_TCHAR(AnomalyTargetPolicy::DescribeNaniteReason()), Refused,
					Matches.Num(), *AnomalyDefaults::DescribeAllowNaniteTargets());
				return false;
			}
		}
	}

	{
		const FAnomalyPartnerExclusion Excl = EvaluatePartnerExclusion(Id);
		if (Excl.bExcluded)
		{
			NoteExclusion(Id, Excl, TEXT("-"));
			UE_LOG(LogAnomaly, Warning,
				TEXT("IAI.Apply '%s' REFUSED excluded_partner_live:%s - its m52/m53 partner can still be in the picture, ")
				TEXT("so no fire is recorded and nothing is changed."),
				*Id.ToString(), *Excl.Describe());
			return false;
		}
		if (Excl.bAdmittedAfterUnresolved)
		{
			++ExclusionStats.AdmittedAfterUnresolved;
			UE_LOG(LogAnomaly, Log,
				TEXT("Auto.AdmitUnresolved candidate=%s partner=%s state=%s event=%s - admitted because the partner's restore ")
				TEXT("trail is past restore_unresolved; its blur may still be in the picture (counted in ")
				TEXT("run_summary.texcorrupt_admitted_after_unresolved)."),
				*Id.ToString(), *Excl.Partner.ToString(), ANSI_TO_TCHAR(AnomalyExclusion::DescribeState(Excl.PartnerState)),
				*Excl.EventKey);
		}
	}

	const bool bApplied = (*Found)->Apply(GetWorld(), Args);

	switch (AnomalyExclusion::RecordAfterApply(bApplied, (*Found)->IsActive()))
	{
	case AnomalyExclusion::ERecordAction::Replace:
	{
		FActiveRecord Record;
		Record.Args = Args;
		Record.ApplyTimeSeconds = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0;
		ActiveRecords.Add(Id, MoveTemp(Record));
		LastApplyFrame.Add(Id, GFrameCounter);
		break;
	}
	case AnomalyExclusion::ERecordAction::Keep:
		UE_LOG(LogAnomaly, Warning,
			TEXT("IAI.Apply '%s' was refused while its previous application is still installed - the running effect and ")
			TEXT("its active record are KEPT (a refused re-Apply never drops the record of an effect that is on screen), so ")
			TEXT("the m52/m53 exclusion still sees it live. Revert it with IAI.Revert %s."),
			*Id.ToString(), *Id.ToString());
		break;
	case AnomalyExclusion::ERecordAction::Drop:
		ActiveRecords.Remove(Id);
		break;
	}

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
	ActiveRecords.Remove(Id);
	if (AnomalyTexCorrupt::IsTexCorruptId(Id))
	{
		LastTexCorruptRevertId = Id;
	}
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
			if (AnomalyTexCorrupt::IsTexCorruptId(Pair.Key))
			{
				LastTexCorruptRevertId = Pair.Key;
			}
			++Count;
		}
	}
	ActiveRecords.Empty();
	return Count;
}

bool UAnomalyInjectorSubsystem::IsIdFireLive(const FName& Id) const
{
	const TUniquePtr<IAnomaly>* Found = Anomalies.Find(Id);
	return Found && Found->IsValid() && AnomalyExclusion::IsFireLiveForExclusion((*Found)->IsActive());
}

bool UAnomalyInjectorSubsystem::IsStuckMipRestoring() const
{
	const TUniquePtr<IAnomaly>* Found = Anomalies.Find(FName(ANSI_TO_TCHAR(AnomalyExclusion::M52Id)));
	if (!Found || !Found->IsValid())
	{
		return false;
	}
	return static_cast<const FAnomaly_StuckLowMip*>(Found->Get())->IsRestoringAny();
}

bool UAnomalyInjectorSubsystem::IsStuckMipRestoringPastTimeout() const
{
	const TUniquePtr<IAnomaly>* Found = Anomalies.Find(FName(ANSI_TO_TCHAR(AnomalyExclusion::M52Id)));
	if (!Found || !Found->IsValid())
	{
		return false;
	}
	return static_cast<const FAnomaly_StuckLowMip*>(Found->Get())->IsRestoringPastTimeout(AnomalyDefaults::GetStuckMipRestoreTimeout());
}

FString UAnomalyInjectorSubsystem::PartnerEventKey(const FName& Id) const
{
	if (const uint64* Frame = LastApplyFrame.Find(Id))
	{
		return FString::Printf(TEXT("%s@%llu"), *Id.ToString(), (unsigned long long)*Frame);
	}
	return FString::Printf(TEXT("%s@-"), *Id.ToString());
}

FAnomalyPartnerExclusion UAnomalyInjectorSubsystem::EvaluatePartnerExclusion(FName Id) const
{
	static const FName M52Name(ANSI_TO_TCHAR(AnomalyExclusion::M52Id));
	static const FName UvName(ANSI_TO_TCHAR(AnomalyExclusion::M53UvId));
	static const FName NormalName(ANSI_TO_TCHAR(AnomalyExclusion::M53NormalId));

	FAnomalyPartnerExclusion Out;
	AnomalyExclusion::EFamily Candidate = AnomalyExclusion::EFamily::Other;
	if (Id == M52Name)
	{
		Candidate = AnomalyExclusion::EFamily::M52;
	}
	else if (Id == UvName || Id == NormalName)
	{
		Candidate = AnomalyExclusion::EFamily::M53;
	}
	if (Candidate == AnomalyExclusion::EFamily::Other)
	{
		return Out;
	}

	if (Candidate == AnomalyExclusion::EFamily::M53)
	{
		AnomalyExclusion::FM52Signals S;
		S.bFireLive = IsIdFireLive(M52Name);
		FString TrailKey;
		if (TrailProvider)
		{
			S.bProvider = true;
			S.Trail = TrailProvider(M52Name, TrailKey);
		}
		S.bRestoringSet = IsStuckMipRestoring();
		S.bRestoringPastTimeout = S.bRestoringSet && IsStuckMipRestoringPastTimeout();
		const AnomalyExclusion::EState M52 = AnomalyExclusion::M52State(S);
		const AnomalyExclusion::FVerdict V = AnomalyExclusion::Evaluate(Candidate, M52, AnomalyExclusion::EState::Idle);
		Out.bExcluded = V.bExcluded;
		Out.bAdmittedAfterUnresolved = V.bAdmittedAfterUnresolved;
		Out.Partner = M52Name;
		Out.PartnerState = V.PartnerState;
		const bool bTrailState = S.bProvider && S.Trail != AnomalyExclusion::ETrail::None && !S.bFireLive;
		Out.EventKey = (bTrailState && !TrailKey.IsEmpty()) ? TrailKey : PartnerEventKey(M52Name);
		return Out;
	}

	AnomalyExclusion::FM53Signals S;
	FName LivePartner = NAME_None;
	if (IsIdFireLive(UvName))
	{
		LivePartner = UvName;
	}
	else if (IsIdFireLive(NormalName))
	{
		LivePartner = NormalName;
	}
	S.bFireLive = !LivePartner.IsNone();
	S.bRevertSettling = !S.bFireLive && AnomalyTexCorrupt::IsRevertSettling(GetWorld());
	FString UvKey;
	FString NormalKey;
	FName TrailPartner = NAME_None;
	if (TrailProvider)
	{
		const AnomalyExclusion::ETrail UvTrail = TrailProvider(UvName, UvKey);
		const AnomalyExclusion::ETrail NormalTrail = TrailProvider(NormalName, NormalKey);
		S.Trail = AnomalyExclusion::CombineTrail(UvTrail, NormalTrail);
		TrailPartner = S.Trail == UvTrail ? UvName : NormalName;
	}
	const AnomalyExclusion::EState M53 = AnomalyExclusion::M53State(S);
	const AnomalyExclusion::FVerdict V = AnomalyExclusion::Evaluate(Candidate, AnomalyExclusion::EState::Idle, M53);
	Out.bExcluded = V.bExcluded;
	const bool bFromTrail = !S.bFireLive && !S.bRevertSettling && V.bExcluded;
	Out.Partner = S.bFireLive ? LivePartner
		: (bFromTrail ? TrailPartner : (LastTexCorruptRevertId.IsNone() ? UvName : LastTexCorruptRevertId));
	Out.PartnerState = V.PartnerState;
	const FString& TrailKey = TrailPartner == UvName ? UvKey : NormalKey;
	Out.EventKey = (bFromTrail && !TrailKey.IsEmpty()) ? TrailKey : PartnerEventKey(Out.Partner);
	return Out;
}

bool UAnomalyInjectorSubsystem::IsExcludedByPartner(FName Id, FString& OutWhy) const
{
	const FAnomalyPartnerExclusion Excl = EvaluatePartnerExclusion(Id);
	OutWhy = Excl.bExcluded ? Excl.Describe() : FString();
	return Excl.bExcluded;
}

void UAnomalyInjectorSubsystem::NoteExclusion(FName Candidate, const FAnomalyPartnerExclusion& Verdict, const FString& Attempt)
{
	if (!Verdict.bExcluded)
	{
		return;
	}
	if (Candidate == FName(ANSI_TO_TCHAR(AnomalyExclusion::M53UvId)))
	{
		++ExclusionStats.ExcludedUvCorruption;
	}
	else if (Candidate == FName(ANSI_TO_TCHAR(AnomalyExclusion::M53NormalId)))
	{
		++ExclusionStats.ExcludedNormalCorruption;
	}
	else if (Candidate == FName(ANSI_TO_TCHAR(AnomalyExclusion::M52Id)))
	{
		++ExclusionStats.ExcludedStuckLowMip;
	}
	UE_LOG(LogAnomaly, Log, TEXT("Auto.Exclude candidate=%s partner=%s state=%s event=%s attempt=%s"),
		*Candidate.ToString(), *Verdict.Partner.ToString(), ANSI_TO_TCHAR(AnomalyExclusion::DescribeState(Verdict.PartnerState)),
		*Verdict.EventKey, *Attempt);
}

void UAnomalyInjectorSubsystem::SetTrailProvider(FAnomalyTrailProviderFn InProvider)
{
	TrailProvider = MoveTemp(InProvider);
	UE_LOG(LogAnomaly, Log,
		TEXT("Auto: TRAIL PROVIDER REGISTERED - a family stays live for the m52/m53 exclusion until its last labelled frame ")
		TEXT("and its last transition-flagged tail frame are emitted, no receipt is outstanding for it and no detached ")
		TEXT("trail can reopen it (live, gating, reopenable, adopted, carried or tail-only), not only until its revert. ")
		TEXT("A trail past restore_unresolved is admitted and counted."));
}

void UAnomalyInjectorSubsystem::ClearTrailProvider()
{
	const bool bHad = (bool)TrailProvider;
	TrailProvider.Reset();
	if (bHad)
	{
		UE_LOG(LogAnomaly, Log,
			TEXT("Auto: trail provider CLEARED - with no capture run, m52 stays live for the exclusion while its game-thread ")
			TEXT("restoring set is non-empty, bounded by the same restore_unresolved timeout as a capture: once every texture ")
			TEXT("in it has waited StuckMipRestoreTimeoutFrames ticks, m53 is admitted and counted."));
	}
}

void UAnomalyInjectorSubsystem::ResetExclusionStats()
{
	ExclusionStats = FAnomalyExclusionStats();
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

bool UAnomalyInjectorSubsystem::IsAnomalyVisualConditionHeld(const FName& Id) const
{
	const TUniquePtr<IAnomaly>* Found = Anomalies.Find(Id);
	if (!Found || !Found->IsValid() || !(*Found)->IsActive())
	{
		return false;
	}
	return (*Found)->IsVisualConditionHeld();
}

uint8 UAnomalyInjectorSubsystem::GetAnomalyVisualConditionState(const FName& Id) const
{
	const TUniquePtr<IAnomaly>* Found = Anomalies.Find(Id);
	if (!Found || !Found->IsValid() || !(*Found)->IsActive())
	{
		return 0;
	}
	return (*Found)->GetVisualConditionState();
}

bool UAnomalyInjectorSubsystem::GetAnomalyTelemetry(const FName& Id, FAnomalyTelemetry& Out) const
{
	const TUniquePtr<IAnomaly>* Found = Anomalies.Find(Id);
	if (!Found || !Found->IsValid() || !(*Found)->IsActive())
	{
		return false;
	}
	return (*Found)->GetTelemetry(Out);
}

FString UAnomalyInjectorSubsystem::GetAnomalyLastRefusalReason(const FName& Id) const
{
	const TUniquePtr<IAnomaly>* Found = Anomalies.Find(Id);
	if (!Found || !Found->IsValid())
	{
		return FString();
	}
	return (*Found)->GetLastRefusalReason();
}

bool UAnomalyInjectorSubsystem::GetCameraClippingFrameEvaluation(FAnomalyNearClipSlabResult& OutSlab,
	bool& bOutSphereProxy) const
{
	const TUniquePtr<IAnomaly>* Found = Anomalies.Find(FName(TEXT("camera_clipping")));
	if (!Found || !Found->IsValid() || !(*Found)->IsActive())
	{
		return false;
	}
	return static_cast<const FAnomaly_CameraClipping*>(Found->Get())->GetFrameEvaluation(OutSlab, bOutSphereProxy);
}

bool UAnomalyInjectorSubsystem::IsAnomalyCurrentlyAnomalous(const FName& Id) const
{
	const TUniquePtr<IAnomaly>* Found = Anomalies.Find(Id);
	if (!Found || !Found->IsValid() || !(*Found)->IsActive())
	{
		return false;
	}
	return (*Found)->IsCurrentlyAnomalous();
}

bool UAnomalyInjectorSubsystem::DoesAnomalyHaveDeferredOnset(const FName& Id) const
{
	const TUniquePtr<IAnomaly>* Found = Anomalies.Find(Id);
	if (!Found || !Found->IsValid())
	{
		return false;
	}
	return (*Found)->HasDeferredOnset();
}

bool UAnomalyInjectorSubsystem::DoesAnomalyUseRenderTruth(const FName& Id) const
{
	const TUniquePtr<IAnomaly>* Found = Anomalies.Find(Id);
	if (!Found || !Found->IsValid())
	{
		return false;
	}
	return (*Found)->UsesRenderResidencyTruth();
}

bool UAnomalyInjectorSubsystem::GetAnomalyRenderTruthTextures(const FName& Id, TArray<FAnomalyRenderTruthTexture>& Out) const
{
	const TUniquePtr<IAnomaly>* Found = Anomalies.Find(Id);
	if (!Found || !Found->IsValid() || !(*Found)->IsActive())
	{
		return false;
	}
	return (*Found)->GetRenderTruthTextures(Out);
}

bool UAnomalyInjectorSubsystem::GetAnomalyRestoringTextures(const FName& Id, TArray<FAnomalyRenderTruthTexture>& Out) const
{
	const TUniquePtr<IAnomaly>* Found = Anomalies.Find(Id);
	if (!Found || !Found->IsValid())
	{
		return false;
	}
	return (*Found)->GetRestoringRenderTruthTextures(Out);
}

bool UAnomalyInjectorSubsystem::ConsumeAnomalyContamination(const FName& Id, FString& OutReason)
{
	TUniquePtr<IAnomaly>* Found = Anomalies.Find(Id);
	if (!Found || !Found->IsValid())
	{
		return false;
	}
	return (*Found)->ConsumeHoldContamination(OutReason);
}

void UAnomalyInjectorSubsystem::SetAnomalyRefusal(const FName& Id, const FString& Reason)
{
	if (Reason.IsEmpty())
	{
		RefusedIds.Remove(Id);
		return;
	}
	RefusedIds.Add(Id, Reason);
	UE_LOG(LogAnomaly, Warning, TEXT("IAI: '%s' is REFUSED from now on - %s"), *Id.ToString(), *Reason);
}

void UAnomalyInjectorSubsystem::ClearAnomalyRefusals()
{
	RefusedIds.Reset();
}

bool UAnomalyInjectorSubsystem::GetAnomalyRefusal(const FName& Id, FString& OutReason) const
{
	if (const FString* Reason = RefusedIds.Find(Id))
	{
		OutReason = *Reason;
		return true;
	}
	return false;
}

void UAnomalyInjectorSubsystem::SetActorReserved(AActor* Actor, bool bReserved)
{
	ReservedActors.RemoveAll([Actor](const TWeakObjectPtr<AActor>& Weak) { return !Weak.IsValid() || Weak.Get() == Actor; });
	if (bReserved && Actor)
	{
		ReservedActors.Add(Actor);
	}
}

void UAnomalyInjectorSubsystem::ClearReservedActors()
{
	ReservedActors.Reset();
}

bool UAnomalyInjectorSubsystem::IsActorReserved(const AActor* Actor) const
{
	if (!Actor)
	{
		return false;
	}
	for (const TWeakObjectPtr<AActor>& Weak : ReservedActors)
	{
		if (Weak.Get() == Actor)
		{
			return true;
		}
	}
	return false;
}

void UAnomalyInjectorSubsystem::NoteAnomalyCapturedFrame(const FName& Id, bool bAnomalousThisFrame)
{
	TUniquePtr<IAnomaly>* Found = Anomalies.Find(Id);
	if (!Found || !Found->IsValid() || !(*Found)->IsActive())
	{
		return;
	}
	(*Found)->NoteCapturedFrame(bAnomalousThisFrame);
}

TArray<FAnomalyCatalogEntry> UAnomalyInjectorSubsystem::GetAnomalyCatalog() const
{
	TArray<FName> Ids;
	Anomalies.GetKeys(Ids);
	Ids.Sort([](const FName& A, const FName& B) { return A.ToString() < B.ToString(); });

	TArray<FAnomalyCatalogEntry> Out;
	Out.Reserve(Ids.Num());
	for (const FName& Id : Ids)
	{
		const TUniquePtr<IAnomaly>& Anomaly = Anomalies[Id];
		if (!Anomaly)
		{
			continue;
		}
		FAnomalyCatalogEntry Entry;
		Entry.Id = Anomaly->GetId();
		Entry.Description = Anomaly->GetDescription();
		Entry.Usage = Anomaly->GetUsage();
		GetAuthoredSpec(Entry.Id, Entry.Scope, Entry.Args);
		Entry.bTargetable = IsTargetableId(Entry.Id, Entry.Scope);
		Out.Add(MoveTemp(Entry));
	}
	return Out;
}

TArray<FActiveAnomalyInfo> UAnomalyInjectorSubsystem::GetActiveAnomalies() const
{
	const double Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0;

	TArray<FName> Ids;
	Anomalies.GetKeys(Ids);
	Ids.Sort([](const FName& A, const FName& B) { return A.ToString() < B.ToString(); });

	TArray<FActiveAnomalyInfo> Out;
	for (const FName& Id : Ids)
	{
		const TUniquePtr<IAnomaly>& Anomaly = Anomalies[Id];
		if (!Anomaly || !Anomaly->IsActive())
		{
			continue;
		}
		FActiveAnomalyInfo Info;
		Info.Id = Id;
		if (const FActiveRecord* Rec = ActiveRecords.Find(Id))
		{
			Info.Args = Rec->Args;
			Info.SecondsActive = FMath::Max(0.0, Now - Rec->ApplyTimeSeconds);
		}
		Out.Add(MoveTemp(Info));
	}
	return Out;
}


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

static FAutoConsoleCommandWithWorldAndArgs GSetViewportScopingCmd(
	TEXT("IAI.SetViewportScoping"),
	TEXT("Toggle viewport-visibility scoping for object-scoped anomalies (default OFF). Usage: IAI.SetViewportScoping <0|1>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() == 0)
			{
				UE_LOG(LogAnomaly, Warning, TEXT("Usage: IAI.SetViewportScoping <0|1>"));
				return;
			}
			if (UAnomalyInjectorSubsystem* Subsystem = ResolveSubsystem(World))
			{
				Subsystem->SetViewportScoping(FCString::Atoi(*Args[0]) != 0);
			}
		}));

#if !UE_BUILD_SHIPPING
static FAutoConsoleCommandWithWorldAndArgs GSynthTickOrderCmd(
	TEXT("IAI.Bench.SynthTickOrder"),
	TEXT("BENCH DEVICE, default OFF, console only - no ini key, never in a client payload. "
		 "When ON, the injector's anomaly dispatch is relocated to OnWorldPreActorTick so it runs BEFORE the "
		 "capture subsystem on every world tick. That SYNTHESISES THE SYMPTOM of a host whose subsystems tick in "
		 "the opposite relative order; it does NOT reproduce that host's cause. A session captured with this ON "
		 "has labels that deliberately disagree with its pixels. Usage: IAI.Bench.SynthTickOrder <0|1>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() == 0)
			{
				UE_LOG(LogAnomaly, Warning, TEXT("Usage: IAI.Bench.SynthTickOrder <0|1>"));
				return;
			}
			if (UAnomalyInjectorSubsystem* Subsystem = ResolveSubsystem(World))
			{
				Subsystem->SetSynthTickOrder(FCString::Atoi(*Args[0]) != 0);
			}
		}));

static TWeakObjectPtr<AActor> GBenchTranslucentProbe;

static void BenchReportMaterialCandidate(const TCHAR* Path, UMaterialInterface* M)
{
	if (!M)
	{
		UE_LOG(LogAnomaly, Log,
			TEXT("Bench.TranslucentProbe: candidate '%s' -> NOT PRESENT in this container."), Path);
		return;
	}
	UE_LOG(LogAnomaly, Log,
		TEXT("Bench.TranslucentProbe: candidate '%s' -> present, blendMode=%d translucent=%d ")
		TEXT("writesCustomDepth=%d"),
		Path, (int32)M->GetBlendMode(),
		IsTranslucentBlendMode(M->GetBlendMode()) ? 1 : 0,
		M->IsTranslucencyWritingCustomDepth() ? 1 : 0);
}

static FAutoConsoleCommandWithWorldAndArgs GBenchHideModeCmd(
	TEXT("IAI.Bench.HideMode"),
	TEXT("BENCH DEVICE, console only - no ini key, never in a client payload. Selects how the ")
	TEXT("hidden-class anomalies (blinking, missing_object) hide their target. 1 = m45 DEFAULT: drop ")
	TEXT("the target from the main and depth passes and silence shadows, Lumen, distance fields, ray ")
	TEXT("tracing and decals, while KEEPING custom depth, so the would-be silhouette is still ")
	TEXT("measurable and hidden frames get a mask. 0 = the pre-m45 SetActorHiddenInGame, which removes ")
	TEXT("the target from custom depth too and so yields no mask. It exists so the IDENTITY gate can ")
	TEXT("run both legs on ONE binary: the picture must be byte-identical either way. ")
	TEXT("Usage: IAI.Bench.HideMode <0|1>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() < 1)
			{
				UE_LOG(LogAnomaly, Warning, TEXT("Usage: IAI.Bench.HideMode <0|1>"));
				return;
			}
			AnomalyHiddenClass::SetHideMode(FCString::Atoi(*Args[0]));
			UE_LOG(LogAnomaly, Warning,
				TEXT("IAI.Bench.HideMode -> %s. BENCH DEVICE, console only, never in a client payload."),
				AnomalyHiddenClass::DescribeHideMode());
		}));

static FAutoConsoleCommandWithWorldAndArgs GBenchHideOmitShadowCmd(
	TEXT("IAI.Bench.HideOmitShadowSilencing"),
	TEXT("BENCH DEVICE, console only, default OFF - never in a client payload. ON deliberately OMITS ")
	TEXT("the shadow-silencing half of the m45 hide, so the target keeps casting a shadow while being ")
	TEXT("absent from the main pass. It exists ONLY to prove the IDENTITY gate can FAIL (G96/G114): a ")
	TEXT("gate that has only ever passed is not evidence. Usage: IAI.Bench.HideOmitShadowSilencing <0|1>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() < 1)
			{
				UE_LOG(LogAnomaly, Warning, TEXT("Usage: IAI.Bench.HideOmitShadowSilencing <0|1>"));
				return;
			}
			AnomalyHiddenClass::SetOmitShadowSilencing(FCString::Atoi(*Args[0]) != 0);
			UE_LOG(LogAnomaly, Warning,
				TEXT("IAI.Bench.HideOmitShadowSilencing -> %s. BENCH DEVICE. This is the deliberate ")
				TEXT("mis-application the identity gate must CATCH."),
				AnomalyHiddenClass::IsOmitShadowSilencing() ? TEXT("ON") : TEXT("off"));
		}));

static FAutoConsoleCommandWithWorldAndArgs GBenchHideOmitDepthPassCmd(
	TEXT("IAI.Bench.HideOmitDepthPassSilencing"),
	TEXT("BENCH DEVICE, console only, default OFF - never in a client payload. ON deliberately OMITS ")
	TEXT("the DEPTH-PASS half of the m45 hide: bRenderInDepthPass stays true while the main pass is ")
	TEXT("off, so the target still writes the depth prepass and OCCLUDES what is behind it while ")
	TEXT("drawing nothing itself. That is deterministic wrong pixels wherever the target overlaps ")
	TEXT("background, in ANY fixture, at the AA-off identity arbiter. It exists ONLY to prove the ")
	TEXT("identity gate can FAIL (G96/G114). Usage: IAI.Bench.HideOmitDepthPassSilencing <0|1>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() < 1)
			{
				UE_LOG(LogAnomaly, Warning, TEXT("Usage: IAI.Bench.HideOmitDepthPassSilencing <0|1>"));
				return;
			}
			AnomalyHiddenClass::SetOmitDepthPassSilencing(FCString::Atoi(*Args[0]) != 0);
			UE_LOG(LogAnomaly, Warning,
				TEXT("IAI.Bench.HideOmitDepthPassSilencing -> %s. BENCH DEVICE. This is the deliberate ")
				TEXT("mis-application the identity gate must CATCH."),
				AnomalyHiddenClass::IsOmitDepthPassSilencing() ? TEXT("ON") : TEXT("off"));
		}));

static FAutoConsoleCommandWithWorldAndArgs GBenchDestroyTargetCmd(
	TEXT("IAI.Bench.DestroyTarget"),
	TEXT("BENCH DEVICE, console only, default DISARMED - never in a client payload. ARMS a one-shot latch that ")
	TEXT("destroys the first live actor matching <substring> as soon as some anomaly has been MEASURABLY ANOMALOUS ")
	TEXT("for [hold_frames] injector ticks (default 2). It is a LATCH and not an immediate destroy on purpose: an ")
	TEXT("unattended packaged leg can only type console commands at startup, and destroying the target then would ")
	TEXT("test 'destroyed before the fire' rather than the exit path that matters, which is DESTROYED MID-WINDOW. ")
	TEXT("If the substring matches no live actor when the latch fires, NOTHING is destroyed, the latch STAYS ARMED, ")
	TEXT("and it says so - a lever that silently matches nothing is a clean null indistinguishable from a clean ")
	TEXT("result. Usage: IAI.Bench.DestroyTarget <substring> [hold_frames]"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() < 1 || Args[0].IsEmpty())
			{
				UE_LOG(LogAnomaly, Warning,
					TEXT("Usage: IAI.Bench.DestroyTarget <substring> [hold_frames]  (currently %s%s)"),
					GBenchDestroyArmed ? TEXT("ARMED on '") : TEXT("DISARMED"),
					GBenchDestroyArmed ? *(GBenchDestroyQuery + TEXT("'")) : TEXT(""));
				return;
			}
			GBenchDestroyQuery = Args[0];
			GBenchDestroyHoldFrames = (Args.Num() >= 2 && Args[1].IsNumeric()) ? FMath::Max(0, FCString::Atoi(*Args[1])) : 2;
			GBenchDestroyAnomalousTicks = 0;
			GBenchDestroyArmed = true;
			UE_LOG(LogAnomaly, Warning,
				TEXT("IAI.Bench.DestroyTarget -> ARMED on '%s' after %d anomalous tick(s). BENCH DEVICE. Nothing is ")
				TEXT("destroyed until an anomaly is measurably anomalous, so the destruction lands inside a live ")
				TEXT("window. This is the lever the target-destroyed exit gate needs; 080-03 reported that exit UNRUN ")
				TEXT("because no such lever existed."),
				*GBenchDestroyQuery, GBenchDestroyHoldFrames);
		}));

static FAutoConsoleCommandWithWorldAndArgs GBenchSpawnTranslucentProbeCmd(
	TEXT("IAI.Bench.SpawnTranslucentProbe"),
	TEXT("BENCH DEVICE, default absent, console only - no ini key, never in a client payload. Spawns ONE ")
	TEXT("probe actor in front of the current view carrying a TRANSLUCENT material, so B-G1 can read the m41 "
	     "translucent rule off a real candidate without touching the FROZEN CB_GateLevel (G99). It prints a "
	     "CANDIDATE TABLE first - every material it considered, with that material's blend mode and its "
	     "IsTranslucencyWritingCustomDepth() - because the ON direction of B-G1 needs a material that is "
	     "translucent AND opts into custom-depth writes, and that flag is a COMPILE-TIME UMaterial property "
	     "a UMaterialInstanceDynamic inherits and cannot change. If no such material exists in the staged "
	     "container the lever says so BY NAME and the ON direction is UNOBTAINABLE on this bench - declared, "
	     "not a pass and not a failure; it rides the next cook with C-G1b. ")
	TEXT("Usage: IAI.Bench.SpawnTranslucentProbe <0|1>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() == 0)
			{
				UE_LOG(LogAnomaly, Warning, TEXT("Usage: IAI.Bench.SpawnTranslucentProbe <0|1>"));
				return;
			}
			if (!World)
			{
				return;
			}

			if (FCString::Atoi(*Args[0]) == 0)
			{
				if (AActor* Existing = GBenchTranslucentProbe.Get())
				{
					Existing->Destroy();
					UE_LOG(LogAnomaly, Warning, TEXT("Bench.TranslucentProbe: DESPAWNED."));
				}
				GBenchTranslucentProbe.Reset();
				return;
			}

			static const TCHAR* Candidates[] =
			{
				TEXT("/AnomalyInjector/Materials/M_MissingTexture_Checker.M_MissingTexture_Checker"),
				TEXT("/AnomalyInjector/Materials/M_CorruptedTexture_Pink.M_CorruptedTexture_Pink"),
				TEXT("/Engine/EngineMaterials/WorldGridMaterial.WorldGridMaterial"),
				TEXT("/Engine/EngineDebugMaterials/VertexColorMaterial.VertexColorMaterial"),
				TEXT("/Engine/EngineMaterials/EditorBrushMaterial.EditorBrushMaterial"),
			};

			UMaterialInterface* Chosen = nullptr;
			UMaterialInterface* ChosenWriter = nullptr;
			const TCHAR* ChosenPath = nullptr;
			const TCHAR* ChosenWriterPath = nullptr;
			for (const TCHAR* Path : Candidates)
			{
				UMaterialInterface* M = LoadObject<UMaterialInterface>(nullptr, Path);
				BenchReportMaterialCandidate(Path, M);
				if (!M || !IsTranslucentBlendMode(M->GetBlendMode()))
				{
					continue;
				}
				if (!Chosen)
				{
					Chosen = M;
					ChosenPath = Path;
				}
				if (!ChosenWriter && M->IsTranslucencyWritingCustomDepth())
				{
					ChosenWriter = M;
					ChosenWriterPath = Path;
				}
			}

			if (!Chosen)
			{
				UE_LOG(LogAnomaly, Warning,
					TEXT("Bench.TranslucentProbe: REFUSED - NO TRANSLUCENT MATERIAL was found in this staged ")
					TEXT("container among the %d candidates listed above. B-G1 is UNOBTAINABLE on this bench in ")
					TEXT("BOTH directions and rides the next cook alongside C-G1b. This refusal is the ")
					TEXT("measurement; nothing was spawned and no fixture was improvised."),
					(int32)UE_ARRAY_COUNT(Candidates));
				return;
			}

			UMaterialInterface* Use = ChosenWriter ? ChosenWriter : Chosen;
			const TCHAR* UsePath = ChosenWriter ? ChosenWriterPath : ChosenPath;

			UStaticMesh* Mesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
			if (!Mesh)
			{
				UE_LOG(LogAnomaly, Warning,
					TEXT("Bench.TranslucentProbe: REFUSED - /Engine/BasicShapes/Cube is not in this container, ")
					TEXT("so the probe has no mesh. Nothing spawned."));
				return;
			}

			FVector Origin(0.0f);
			FRotator Rotation(0.0f);
			if (APlayerController* PC = World->GetFirstPlayerController())
			{
				PC->GetPlayerViewPoint(Origin, Rotation);
			}
			const FVector Location = Origin + Rotation.Vector() * 400.0f;

			if (AActor* Existing = GBenchTranslucentProbe.Get())
			{
				Existing->Destroy();
			}

			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			AActor* Probe = World->SpawnActor<AActor>(AActor::StaticClass(), Location, FRotator::ZeroRotator, SpawnParams);
			if (!Probe)
			{
				UE_LOG(LogAnomaly, Warning, TEXT("Bench.TranslucentProbe: REFUSED - SpawnActor failed."));
				return;
			}

			UStaticMeshComponent* Comp = NewObject<UStaticMeshComponent>(Probe);
			Probe->SetRootComponent(Comp);
			Comp->RegisterComponent();
			Comp->SetStaticMesh(Mesh);
			Comp->SetMaterial(0, Use);
			Comp->SetWorldScale3D(FVector(2.0f));
			Comp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			GBenchTranslucentProbe = Probe;

			UE_LOG(LogAnomaly, Warning,
				TEXT("Bench.TranslucentProbe: SPAWNED '%s' at %s with material '%s' (translucent=1 ")
				TEXT("writesCustomDepth=%d). %s NEVER ship a capture taken with this probe present."),
				*Probe->GetName(), *Location.ToCompactString(), UsePath,
				Use->IsTranslucencyWritingCustomDepth() ? 1 : 0,
				ChosenWriter
					? TEXT("BOTH directions of B-G1 are testable on this container.")
					: TEXT("NO custom-depth-writing translucent material exists in this container, so ONLY the "
					       "OFF direction of B-G1 is testable here; the ON direction is UNOBTAINABLE and rides "
					       "the next cook with C-G1b. Declared, not a pass and not a failure."));
		}));
#endif

static FAutoConsoleCommandWithWorldAndArgs GTestVisibilityCmd(
	TEXT("IAI.TestVisibility"),
	TEXT("Diagnostic: test core visibility against a SYNTHETIC view (no live player needed). "
	     "Usage: IAI.TestVisibility <substring> <ox> <oy> <oz> <pitch> <yaw> <roll> [fovDeg] [aspect]"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() < 7)
			{
				UE_LOG(LogAnomaly, Warning,
					TEXT("Usage: IAI.TestVisibility <substring> <ox> <oy> <oz> <pitch> <yaw> <roll> [fovDeg] [aspect]"));
				return;
			}
			if (UAnomalyInjectorSubsystem* Subsystem = ResolveSubsystem(World))
			{
				Subsystem->TestVisibility(Args);
			}
		}));

static FAutoConsoleCommandWithWorldAndArgs GDumpCatalogCmd(
	TEXT("IAI.DumpCatalog"),
	TEXT("Diagnostic: log the structured anomaly catalog (id | scope | usage | arg schema)."),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (UAnomalyInjectorSubsystem* Subsystem = ResolveSubsystem(World))
			{
				Subsystem->DumpCatalog();
			}
		}));

static FAutoConsoleCommandWithWorldAndArgs GDumpActiveCmd(
	TEXT("IAI.DumpActive"),
	TEXT("Diagnostic: log the active anomalies (id | seconds-active | applied args)."),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (UAnomalyInjectorSubsystem* Subsystem = ResolveSubsystem(World))
			{
				Subsystem->DumpActiveAnomalies();
			}
		}));

static FAutoConsoleCommandWithWorldAndArgs GDumpVisibleCmd(
	TEXT("IAI.DumpVisible"),
	TEXT("Diagnostic: log the renderable-visible actor infos (name|class|comp|dist|screen-rect) and assert "
	     "set+order match GetVisibleRenderableActors (the A4 rect gate + the byte-identical regression gate)."),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (UAnomalyInjectorSubsystem* Subsystem = ResolveSubsystem(World))
			{
				Subsystem->DumpVisibleRenderableInfos();
			}
		}));
