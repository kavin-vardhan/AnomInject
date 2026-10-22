#include "Compat/AnomalyTickableWorldSubsystem.h"
#include "Compat/AnomalyEngineVersion.h"
#include "AnomalyBenchGate.h"
#include "AnomalyInjectorLog.h"
#include "CoreGlobals.h"
#include "Engine/World.h"
#include "HAL/IConsoleManager.h"
#include "Subsystems/SubsystemCollection.h"

#if !UE_BUILD_SHIPPING
static int32 GAnomalyTickAudit = 0;
static FAutoConsoleVariableRef GAnomalyTickAuditVar(TEXT("IAI.Bench.TickAudit"), GAnomalyTickAudit,
	TEXT("Default-off subsystem lifetime audit. Requires -IAIBench. Never enable in timing or pixel regression legs."));
#endif

UAnomalyTickableWorldSubsystem::UAnomalyTickableWorldSubsystem() = default;

UAnomalyTickableWorldSubsystem::UAnomalyTickableWorldSubsystem(FVTableHelper& Helper) : Super(Helper) {}

bool UAnomalyTickableWorldSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!Super::ShouldCreateSubsystem(Outer))
	{
		return false;
	}
	const UWorld* World = Cast<UWorld>(Outer);
	return World && (World->WorldType == EWorldType::Game || World->WorldType == EWorldType::PIE);
}

ETickableTickType UAnomalyTickableWorldSubsystem::GetTickableTickType() const
{
	return IsTemplate() ? ETickableTickType::Never : FTickableGameObject::GetTickableTickType();
}

bool UAnomalyTickableWorldSubsystem::IsTickable() const
{
#if ANOMALY_ENGINE_425
	return !IsTemplate() && bInitialized;
#else
	return FTickableGameObject::IsTickable();
#endif
}

bool UAnomalyTickableWorldSubsystem::IsAllowedToTick() const
{
	return !IsTemplate() && bInitialized;
}

void UAnomalyTickableWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	check(!bInitialized);
	bInitialized = true;
#if !UE_BUILD_SHIPPING
	if (GAnomalyTickAudit) { Audit(TEXT("initialize")); }
#endif
}

void UAnomalyTickableWorldSubsystem::Deinitialize()
{
	check(bInitialized);
	bInitialized = false;
#if !UE_BUILD_SHIPPING
	if (GAnomalyTickAudit) { Audit(TEXT("deinitialize")); }
#endif
}

void UAnomalyTickableWorldSubsystem::Tick(float DeltaTime)
{
	checkf(IsInitialized(), TEXT("Ticking should have been disabled for an uninitialized subsystem : remember to call IsInitialized in the subsystem's IsTickable, IsTickableInEditor and/or IsTickableWhenPaused implementation"));
#if !UE_BUILD_SHIPPING
	if (GAnomalyTickAudit) { Audit(TEXT("tick")); }
#endif
}

#if !UE_BUILD_SHIPPING
void UAnomalyTickableWorldSubsystem::Audit(const TCHAR* Operation) const
{
	if (!AnomalyBenchGate::IsEnabled()) { return; }
	const UWorld* World = GetWorld();
	UE_LOG(LogAnomaly, Display, TEXT("EUROPA-TICK op=%s instance=%p class=%s world=%p world_name=%s frame=%llu world_time=%.9f initialized=%d template=%d"),
		Operation, this, *GetClass()->GetName(), World, *GetNameSafe(World), (unsigned long long)GFrameCounter,
		World ? (double)World->GetTimeSeconds() : -1.0, bInitialized ? 1 : 0, IsTemplate() ? 1 : 0);
}
#endif
