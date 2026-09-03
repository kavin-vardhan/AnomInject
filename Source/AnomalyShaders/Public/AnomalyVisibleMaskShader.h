#pragma once

#include "CoreMinimal.h"

#if ANOMALY_SHADERS

#include "GlobalShader.h"
#include "ShaderParameterStruct.h"
#include "SceneTexturesConfig.h"

class FAnomalyVisibleMaskPS : public FGlobalShader
{
public:
	DECLARE_EXPORTED_SHADER_TYPE(FAnomalyVisibleMaskPS, Global, ANOMALYSHADERS_API);
	SHADER_USE_PARAMETER_STRUCT(FAnomalyVisibleMaskPS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT_INCLUDE(FSceneTextureShaderParameters, SceneTextures)
		SHADER_PARAMETER(float, DepthBias)
		SHADER_PARAMETER(uint32, ReservedBase)
		SHADER_PARAMETER(FIntPoint, ViewRectMin)
		SHADER_PARAMETER(FIntPoint, InternalRectMin)
		SHADER_PARAMETER(FIntPoint, InternalRectSize)
		SHADER_PARAMETER(FIntPoint, OutputRectSize)
		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters&) { return true; }
};

#endif
