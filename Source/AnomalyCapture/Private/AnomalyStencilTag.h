#pragma once

#include "CoreMinimal.h"

class AActor;

namespace AnomalyStencilTag
{
	static constexpr int32 ReservedStencilBase = 200;
	static constexpr int32 ReservedStencilMax = 255;

	int32 TagActor(AActor* Actor, int32 StencilValue);
	void RestoreActor(AActor* Actor);
	void RestoreAll();
	bool IsAnyTagged();

	bool VerifyActorStillTagged(const AActor* Actor, int32 StencilValue, FString& OutDetail);

	void EnableCustomStencil();
	void DisableCustomStencil();
}
