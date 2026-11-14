#pragma once

#include "CoreMinimal.h"

class FStaticMeshRenderData;

struct FAnomalyLodRenderProbe
{
	int32 FireIndex = -1;
	const FStaticMeshRenderData* RenderData = nullptr;
	FVector Origin = FVector::ZeroVector;
	float Radius = 0.0f;
	int32 MinLod = 0;
	int32 NumLods = 0;
	float FactorScale = 1.0f;
};

struct FAnomalyLodRenderResult
{
	int32 FireIndex = -1;
	int32 EngineLod = -1;
	int32 TemporalLod0 = -1;
	int32 TemporalLod1 = -1;
};
