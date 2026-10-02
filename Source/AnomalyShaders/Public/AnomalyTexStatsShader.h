#pragma once

#include "CoreMinimal.h"

#if ANOMALY_SHADERS

#include "GlobalShader.h"
#include "ShaderParameterStruct.h"

class FAnomalyTexStatsCS : public FGlobalShader
{
public:
	DECLARE_EXPORTED_SHADER_TYPE(FAnomalyTexStatsCS, Global, ANOMALYSHADERS_API);
	SHADER_USE_PARAMETER_STRUCT(FAnomalyTexStatsCS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, SourceTexture)
		SHADER_PARAMETER_RDG_BUFFER_UAV(RWBuffer<uint>, OutStats)
		SHADER_PARAMETER(FIntPoint, SourceSize)
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters&) { return true; }
};

#endif
