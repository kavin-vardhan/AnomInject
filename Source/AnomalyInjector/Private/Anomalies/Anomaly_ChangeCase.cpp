#include "Anomalies/Anomaly_ChangeCase.h"
#include "AnomalyInjectorLog.h"
#include "AnomalyLod.h"
#include "AnomalyViewport.h"
#include "AnomalyInjectorSubsystem.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "HAL/IConsoleManager.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"

#if !UE_BUILD_SHIPPING
static TAutoConsoleVariable<int32> GChangeCases(TEXT("IAI.Bench.ChangeEvidenceCases"), 0,
	TEXT("Bench twins only; also requires -IAIBenchFixture and CB_GateLevel or L_ShooterGym. Never auto-pooled."));
#endif
bool FAnomaly_ChangeCase::Apply(UWorld* World, const TArray<FString>& Args)
{
#if UE_BUILD_SHIPPING
	return false;
#else
	if (!World || Args.IsEmpty() || GChangeCases.GetValueOnGameThread() != 1 ||
		!FParse::Param(FCommandLine::Get(), TEXT("IAIBenchFixture")) ||
		!(World->GetMapName().Contains(TEXT("CB_GateLevel")) || World->GetMapName().Contains(TEXT("L_ShooterGym"))))
	{
		UE_LOG(LogAnomaly, Warning, TEXT("%s REFUSED: fixture-only; requires IAI.Bench.ChangeEvidenceCases 1 and -IAIBenchFixture on a named bench map."), *Id.ToString());
		return false;
	}
	Revert();
	auto Meshes = AnomalyLod::ResolveLodComponents(World, Args[0]);
	FAnomalyViewInfo View;
	if (AnomalyViewport::GetActiveViewInfo(World, View)) { Meshes = AnomalyViewport::FilterVisibleComponents(View, World, Meshes); }
	bool bStatic = false;
	for (const auto& M : Meshes) { if (Cast<UStaticMeshComponent>(M.Get())) { bStatic = true; } }
	if (!bStatic) { return false; }
	Delay = 0;
	for (int32 I = 1; I < Args.Num(); ++I)
	{
		if (Args[I].StartsWith(TEXT("delay="))) { Delay = FMath::Max(0, FCString::Atoi(*Args[I].Mid(6))); }
		else { Delay = FMath::Max(0, FCString::Atoi(*Args[I])); }
	}
	SavedArgs = {Args[0]}; ActiveWorld = World; Captured = 0; LastNotedFrame = 0; bActive = true;
	if (Delay == 0) { bApplied = !bSwap || MaterialSwap.Apply(World, SavedArgs); if (!bApplied) { Revert(); return false; } }
	UE_LOG(LogAnomaly, Log, TEXT("ChangeCase: id=%s target=%s delay=%d swap=%d held=1"), *Id.ToString(), *Args[0], Delay, bSwap);
	return true;
#endif
}
void FAnomaly_ChangeCase::NoteCapturedFrame(bool bLabelled)
{
	if (bActive && bLabelled) { ++Captured; LastNotedFrame = GFrameCounter; }
}
void FAnomaly_ChangeCase::TickAlways(float)
{
	if (bActive && !bApplied && Captured >= Delay && GFrameCounter != LastNotedFrame)
	{
		bApplied = !bSwap || MaterialSwap.Apply(ActiveWorld.Get(), SavedArgs);
		UE_LOG(LogAnomaly, Log, TEXT("ChangeCase: transition id=%s afterLabelled=%d applied=%d gfc=%llu"), *Id.ToString(), Captured, bApplied, GFrameCounter);
		if (!bApplied) { Revert(); }
	}
}
void FAnomaly_ChangeCase::Revert()
{
	if (bSwap && MaterialSwap.IsActive()) { MaterialSwap.Revert(); }
	bActive = false; bApplied = false; ActiveWorld.Reset(); SavedArgs.Reset();
}
