#pragma once

#include "CoreMinimal.h"

class UWorld;

namespace AnomalyTexCorrupt
{
	struct FTreeResult;

	struct FVTDiagnostics
	{
		int64 Components = 0;
		int64 Touching = 0;
		int64 Bindings = 0;
		int64 RequiredComponents = 0;
		int64 RequiredBindings = 0;
		int64 UnreadSlots = 0;
		int64 StreamingStacks = 0;
		int64 PreallocatedStacks = 0;
		int64 OnlyVT = 0;
		int64 OnlyVTUnassessed = 0;
		int64 OnlyStack = 0;
		int64 OnlyStackUnassessed = 0;
		int64 IncompleteFormats = 0;
		TMap<int32, int64> StackLayers;
		TMap<int32, int64> LayerFormats;
		TMap<int32, int64> BindingFormats;
		TMap<int32, int64> ComponentFormats;
		TMap<int32, int64> Tiles;
		TMap<int32, int64> Borders;
		TMap<int32, int64> AddressX;
		TMap<int32, int64> AddressY;
	};

	void AccumulateVTDiagnostics(UWorld* World, const FTreeResult& Result, FVTDiagnostics& Counts);
	void PrintVTDiagnostics(const FVTDiagnostics& Counts, int32 Family);
}
