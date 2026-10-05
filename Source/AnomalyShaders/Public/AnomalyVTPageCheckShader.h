#pragma once

#include "CoreMinimal.h"

#if ANOMALY_SHADERS
#include "GlobalShader.h"
#include "ShaderParameterStruct.h"

class FAnomalyVTPageCheckCS : public FGlobalShader
{
public:
	DECLARE_EXPORTED_SHADER_TYPE(FAnomalyVTPageCheckCS, Global, ANOMALYSHADERS_API);
	SHADER_USE_PARAMETER_STRUCT(FAnomalyVTPageCheckCS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<uint4>, PageTable)
		SHADER_PARAMETER_RDG_BUFFER_UAV(RWBuffer<uint>, Entries)
		SHADER_PARAMETER(FIntPoint, PageOrigin)
		SHADER_PARAMETER(FIntPoint, PageSize)
		SHADER_PARAMETER(uint32, PageLevel)
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters&) { return true; }
};
#endif
