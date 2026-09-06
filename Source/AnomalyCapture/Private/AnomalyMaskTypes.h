#pragma once

#include "CoreMinimal.h"

struct FAnomalyMaskTagResult
{
	int32 Count = 0;
	int32 MinX = MAX_int32;
	int32 MinY = MAX_int32;
	int32 MaxX = MIN_int32;
	int32 MaxY = MIN_int32;
};

struct FAnomalyMaskResult
{
	uint32 RenderFrame = 0;
	TMap<uint8, FAnomalyMaskTagResult> TagResults;
	FIntPoint ViewRectSize = FIntPoint::ZeroValue;
	int32 TotalMaskedPixels = 0;
	bool bSawUnassignedReservedTag = false;
	uint8 FirstUnassignedTag = 0;
	int32 UnassignedTagCount = 0;
	int32 CustomDepthModeAtPass = -1;
	FIntPoint CustomStencilExtent = FIntPoint::ZeroValue;
	TArray<uint8> MaskPixels;
};

enum class EAnomalyMaskState : uint8
{
	NotMeasured = 0,
	MeasuredZero = 1,
	MeasuredNonZero = 2
};

inline const TCHAR* LexToStringAnomalyMaskState(EAnomalyMaskState State)
{
	switch (State)
	{
	case EAnomalyMaskState::MeasuredZero:    return TEXT("MEASURED_ZERO");
	case EAnomalyMaskState::MeasuredNonZero: return TEXT("MEASURED_NONZERO");
	default:                                 return TEXT("NOT_MEASURED");
	}
}
