#include "Anomalies/Anomaly_TimeDilation.h"

#include "AnomalyInjectorLog.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

bool FAnomaly_TimeDilation::Apply(UWorld* World, const TArray<FString>& Args)
{
	if (!World)
	{
		return false;
	}
	if (Args.Num() == 0 || Args[0].IsEmpty())
	{
		UE_LOG(LogAnomaly, Warning, TEXT("time_dilation: usage <scale>"));
		return false;
	}
	if (!Args[0].IsNumeric())
	{
		UE_LOG(LogAnomaly, Warning, TEXT("time_dilation: '%s' is not a number."), *Args[0]);
		return false;
	}
	const float Scale = FCString::Atof(*Args[0]);

	if (bActive)
	{
		Revert();
	}

	WorldWeak = World;
	PreviousDilation = UGameplayStatics::GetGlobalTimeDilation(World);
	UGameplayStatics::SetGlobalTimeDilation(World, Scale);

	const float Applied = UGameplayStatics::GetGlobalTimeDilation(World);
	if (!FMath::IsNearlyEqual(Applied, Scale))
	{
		UE_LOG(LogAnomaly, Warning,
			TEXT("time_dilation: requested %.4f but clamped to %.4f (WorldSettings Min/MaxGlobalTimeDilation)."),
			Scale, Applied);
	}

	bActive = true;
	UE_LOG(LogAnomaly, Log, TEXT("time_dilation: baseline %.4f -> %.4f."), PreviousDilation, Applied);
	return true;
}

void FAnomaly_TimeDilation::Revert()
{
	if (UWorld* World = WorldWeak.Get())
	{
		UGameplayStatics::SetGlobalTimeDilation(World, PreviousDilation);
		UE_LOG(LogAnomaly, Log, TEXT("time_dilation: restored baseline %.4f."), PreviousDilation);
	}
	WorldWeak.Reset();
	PreviousDilation = 1.0f;
	bActive = false;
}
