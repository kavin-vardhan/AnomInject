#pragma once

#include "CoreMinimal.h"

class UWorld;

namespace ControlSnapshot
{
	FString BuildSnapshotJson(UWorld* World, uint32 Epoch);

	FString BuildCatalogJson(UWorld* World);
}
