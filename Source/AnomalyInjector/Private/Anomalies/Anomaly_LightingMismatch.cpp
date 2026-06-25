#include "Anomalies/Anomaly_LightingMismatch.h"

#include "AnomalyTargeting.h"
#include "AnomalyArgs.h"
#include "AnomalyInjectorLog.h"
#include "Components/LightComponent.h"

bool FAnomaly_LightingMismatch::Apply(UWorld* World, const TArray<FString>& Args)
{
	if (!World)
	{
		return false;
	}
	if (Args.Num() == 0 || Args[0].IsEmpty())
	{
		UE_LOG(LogAnomaly, Warning, TEXT("lighting_mismatch: usage <substring> [off | dim <f> | recolor <r g b> | noshadow]"));
		return false;
	}

	if (bActive)
	{
		Revert();
	}

	const FString& Substring = Args[0];

	FString Mode = AnomalyArgs::GetString(Args, 1, TEXT("dim")).ToLower();
	if (Mode != TEXT("off") && Mode != TEXT("dim") && Mode != TEXT("recolor") && Mode != TEXT("noshadow"))
	{
		UE_LOG(LogAnomaly, Warning, TEXT("lighting_mismatch: unknown mode '%s'; using 'dim'."), *Mode);
		Mode = TEXT("dim");
	}

	float DimFactor = 0.1f;
	FLinearColor RecolorColor(1.0f, 0.0f, 1.0f);
	if (Mode == TEXT("dim"))
	{
		DimFactor = AnomalyArgs::GetFloat(Args, 2, 0.1f, 0.0f, 1.0f);
	}
	else if (Mode == TEXT("recolor"))
	{
		const float R = AnomalyArgs::GetFloat(Args, 2, 1.0f, 0.0f, 1.0f);
		const float G = AnomalyArgs::GetFloat(Args, 3, 0.0f, 0.0f, 1.0f);
		const float B = AnomalyArgs::GetFloat(Args, 4, 1.0f, 0.0f, 1.0f);
		RecolorColor = FLinearColor(R, G, B);
	}

	const TArray<TWeakObjectPtr<ULightComponent>> Lights = AnomalyTargeting::FindComponentsMatching<ULightComponent>(World, Substring);
	if (Lights.Num() == 0)
	{
		UE_LOG(LogAnomaly, Log, TEXT("lighting_mismatch: matched 0 light component(s) for '%s'."), *Substring);
		return false;
	}

	for (const TWeakObjectPtr<ULightComponent>& Weak : Lights)
	{
		ULightComponent* Light = Weak.Get();
		if (!Light)
		{
			continue;
		}

		FCapturedLight Record;
		Record.Light = Light;
		Record.Intensity = Light->Intensity;
		Record.Color = Light->GetLightColor();
		Record.bVisible = Light->GetVisibleFlag();
		Record.bCastShadows = (Light->CastShadows != 0);
		Captured.Add(Record);

		if (Mode == TEXT("off"))
		{
			Light->SetVisibility(false);
		}
		else if (Mode == TEXT("recolor"))
		{
			Light->SetLightColor(RecolorColor);
		}
		else if (Mode == TEXT("noshadow"))
		{
			Light->SetCastShadows(false);
		}
		else
		{
			Light->SetIntensity(Record.Intensity * DimFactor);
		}
	}

	bActive = Captured.Num() > 0;
	UE_LOG(LogAnomaly, Log, TEXT("lighting_mismatch: mode '%s' on %d light component(s) for '%s'."),
		*Mode, Captured.Num(), *Substring);
	return bActive;
}

void FAnomaly_LightingMismatch::Revert()
{
	for (const FCapturedLight& Record : Captured)
	{
		ULightComponent* Light = Record.Light.Get();
		if (!Light)
		{
			continue;
		}
		Light->SetIntensity(Record.Intensity);
		Light->SetLightColor(Record.Color);
		Light->SetVisibility(Record.bVisible);
		Light->SetCastShadows(Record.bCastShadows);
	}

	Captured.Reset();
	bActive = false;
}
