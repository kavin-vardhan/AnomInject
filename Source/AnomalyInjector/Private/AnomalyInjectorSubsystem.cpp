#include "AnomalyInjectorSubsystem.h"
#include "AnomalyInjectorLog.h"

#include "EngineUtils.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "HAL/IConsoleManager.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"

#include "AnomalyViewport.h"
#include "AnomalyTargeting.h"
#include "Components/PrimitiveComponent.h"

#include "Anomalies/Anomaly_MissingObject.h"
#include "Anomalies/Anomaly_Flicker.h"
#include "Anomalies/Anomaly_TimeDilation.h"
#include "Anomalies/Anomaly_LightingMismatch.h"
#include "Anomalies/Anomaly_LodCorruption.h"
#include "Anomalies/Anomaly_LodPopping.h"
#include "Anomalies/Anomaly_CameraClipping.h"
#include "Anomalies/Anomaly_MissingTexture.h"

static constexpr uint64 GAnomalyHeartbeatKey = 0x47445048;

UAnomalyInjectorSubsystem::~UAnomalyInjectorSubsystem() = default;

UAnomalyInjectorSubsystem::UAnomalyInjectorSubsystem()
{
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> CheckerFinder(
		TEXT("/AnomalyInjector/Materials/M_MissingTexture_Checker.M_MissingTexture_Checker"));
	MissingTextureChecker = CheckerFinder.Object;
}

UMaterialInterface* UAnomalyInjectorSubsystem::GetMissingTextureMaterial() const
{
	return MissingTextureChecker;
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

	void GetAuthoredSpec(const FName& Id, EAnomalyScope& OutScope, TArray<FAnomalyArgSpec>& OutArgs)
	{
		OutArgs.Reset();
		if (Id == FName(TEXT("missing_object")))
		{
			OutScope = EAnomalyScope::Object;
		}
		else if (Id == FName(TEXT("flicker")))
		{
			OutScope = EAnomalyScope::Object;
			OutArgs.Add(FloatArg(TEXT("hz"), TEXT("5"), 0.0, 60.0));
		}
		else if (Id == FName(TEXT("lod_corruption")))
		{
			OutScope = EAnomalyScope::Object;
			OutArgs.Add(IntArg(TEXT("lod-index"), TEXT(""), 1.0));
		}
		else if (Id == FName(TEXT("lod_popping")))
		{
			OutScope = EAnomalyScope::Object;
			OutArgs.Add(FloatArg(TEXT("hz"), TEXT("2"), 0.0, 30.0));
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
	Register(MakeUnique<FAnomaly_Flicker>());
	Register(MakeUnique<FAnomaly_TimeDilation>());
	Register(MakeUnique<FAnomaly_LightingMismatch>());
	Register(MakeUnique<FAnomaly_LodCorruption>());
	Register(MakeUnique<FAnomaly_LodPopping>());
	Register(MakeUnique<FAnomaly_CameraClipping>());
	Register(MakeUnique<FAnomaly_MissingTexture>());

	UE_LOG(LogAnomaly, Log, TEXT("Subsystem initialized for world '%s'. %d anomaly type(s) registered."),
		*GetNameSafe(GetWorld()), Anomalies.Num());
}

void UAnomalyInjectorSubsystem::Deinitialize()
{
	const int32 Reverted = RevertAllActive();
	if (Reverted > 0)
	{
		UE_LOG(LogAnomaly, Log, TEXT("Subsystem deinitializing; reverted %d active anomaly(ies)."), Reverted);
	}
	Super::Deinitialize();
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

	for (const TPair<FName, TUniquePtr<IAnomaly>>& Pair : Anomalies)
	{
		if (Pair.Value && Pair.Value->IsActive())
		{
			Pair.Value->Tick(DeltaTime);
		}
	}

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
				FString::Printf(TEXT("[IAI] AnomalyInjector ticking (active: %d/%d, scoping: %s)"),
					ActiveCount, Anomalies.Num(), bViewportScopingEnabled ? TEXT("ON") : TEXT("OFF")));
		}
		UE_LOG(LogAnomaly, Verbose, TEXT("Heartbeat; active anomalies: %d/%d; scoping: %s"),
			ActiveCount, Anomalies.Num(), bViewportScopingEnabled ? TEXT("ON") : TEXT("OFF"));
	}
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
		UE_LOG(LogAnomaly, Log, TEXT("  [%d] %s | %s | %s | dist=%.0f | rect=%s [%.3f,%.3f - %.3f,%.3f]"),
			i, *Info.ActorName, *Info.ClassName, *Info.ComponentType, Info.Distance,
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

	const bool bApplied = (*Found)->Apply(GetWorld(), Args);

	if (bApplied)
	{
		FActiveRecord Record;
		Record.Args = Args;
		Record.ApplyTimeSeconds = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0;
		ActiveRecords.Add(Id, MoveTemp(Record));
	}
	else
	{
		ActiveRecords.Remove(Id);
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
	ActiveRecords.Empty();
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
