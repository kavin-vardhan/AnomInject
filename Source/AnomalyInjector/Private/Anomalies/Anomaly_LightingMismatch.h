#pragma once

#include "CoreMinimal.h"
#include "IAnomaly.h"

class UWorld;
class ULightComponent;

class FAnomaly_LightingMismatch final : public IAnomaly
{
public:
	virtual FName   GetId() const override { return FName(TEXT("lighting_mismatch")); }
	virtual FString GetDescription() const override { return TEXT("Mismatch lights on matching actors (off / dim / recolor / noshadow)."); }
	virtual FString GetUsage() const override { return TEXT("<substring> [off | dim <f> | recolor <r g b> | noshadow]"); }

	virtual bool Apply(UWorld* World, const TArray<FString>& Args) override;
	virtual void Revert() override;
	virtual bool IsActive() const override { return bActive; }

private:
	struct FCapturedLight
	{
		TWeakObjectPtr<ULightComponent> Light;
		float        Intensity = 0.0f;
		FLinearColor Color = FLinearColor::White;
		bool         bVisible = true;
		bool         bCastShadows = true;
	};

	TArray<FCapturedLight> Captured;
	bool bActive = false;
};
