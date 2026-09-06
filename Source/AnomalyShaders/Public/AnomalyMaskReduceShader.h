#pragma once

#include "CoreMinimal.h"

#if ANOMALY_SHADERS

#include "GlobalShader.h"
#include "ShaderParameterStruct.h"

class FAnomalyMaskReduceCS : public FGlobalShader
{
public:
	DECLARE_EXPORTED_SHADER_TYPE(FAnomalyMaskReduceCS, Global, ANOMALYSHADERS_API);
	SHADER_USE_PARAMETER_STRUCT(FAnomalyMaskReduceCS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_RDG_TEXTURE(Texture2D<uint>, MaskTexture)
		SHADER_PARAMETER_RDG_TEXTURE(Texture2D<uint>, DrawnTexture)
		SHADER_PARAMETER_RDG_BUFFER_UAV(RWBuffer<uint>, OutTable)
		SHADER_PARAMETER(FIntPoint, MaskSize)
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters&) { return true; }
};

#endif
