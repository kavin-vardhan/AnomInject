#pragma once
#include "CoreMinimal.h"
#if ANOMALY_SHADERS && ANOMALY_SHADERS425
#include "GlobalShader.h"
#include "ShaderParameterStruct.h"
#include "SceneRenderTargetParameters.h"
class FAnomalyMask425CS : public FGlobalShader
{
public:
    DECLARE_EXPORTED_SHADER_TYPE(FAnomalyMask425CS, Global, ANOMALYSHADERS_API);
    SHADER_USE_PARAMETER_STRUCT(FAnomalyMask425CS, FGlobalShader);
    BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
        SHADER_PARAMETER_STRUCT_REF(FSceneTexturesUniformParameters, SceneTextures)
        SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<uint4>, Evidence)
        SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<uint>, Mask)
        SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<uint>, Drawn)
        SHADER_PARAMETER(FIntPoint, OutputSize)
        SHADER_PARAMETER(FIntPoint, InternalMin)
        SHADER_PARAMETER(FIntPoint, InternalSize)
        SHADER_PARAMETER(uint32, TargetTag)
        SHADER_PARAMETER(float, DepthBias)
    END_SHADER_PARAMETER_STRUCT()
    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters&) { return true; }
};
#endif
