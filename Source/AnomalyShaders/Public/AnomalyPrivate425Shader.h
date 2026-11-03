#pragma once
#include "CoreMinimal.h"
#if ANOMALY_SHADERS && ANOMALY_SHADERS425
#include "GlobalShader.h"
#include "ShaderParameterStruct.h"
#include "SceneRenderTargetParameters.h"
#include "SceneView.h"
class FAnomalyPrivateDepth425VS : public FGlobalShader
{
public:
    DECLARE_EXPORTED_SHADER_TYPE(FAnomalyPrivateDepth425VS, Global, ANOMALYSHADERS_API);
    SHADER_USE_PARAMETER_STRUCT(FAnomalyPrivateDepth425VS, FGlobalShader);
    BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
    END_SHADER_PARAMETER_STRUCT()
    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters&) { return true; }
};
class FAnomalyPrivateDepth425PS : public FGlobalShader
{
public:
    DECLARE_EXPORTED_SHADER_TYPE(FAnomalyPrivateDepth425PS, Global, ANOMALYSHADERS_API);
    SHADER_USE_PARAMETER_STRUCT(FAnomalyPrivateDepth425PS, FGlobalShader);
    BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
        SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)
        SHADER_PARAMETER_STRUCT_REF(FSceneTexturesUniformParameters, SceneTextures)
        SHADER_PARAMETER(FIntPoint, OutputSize)
    END_SHADER_PARAMETER_STRUCT()
    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters&) { return true; }
};
class FAnomalyPrivateIds425CS : public FGlobalShader
{
public:
    DECLARE_EXPORTED_SHADER_TYPE(FAnomalyPrivateIds425CS, Global, ANOMALYSHADERS_API);
    SHADER_USE_PARAMETER_STRUCT(FAnomalyPrivateIds425CS, FGlobalShader);
    BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
        SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)
        SHADER_PARAMETER_STRUCT_REF(FSceneTexturesUniformParameters, SceneTextures)
        SHADER_PARAMETER_TEXTURE(Texture2D, PrivateDepth)
        SHADER_PARAMETER_SRV(Texture2D<uint2>, PrivateIds)
        SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<uint4>, Evidence)
        SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<uint>, Mask)
        SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<uint>, Drawn)
        SHADER_PARAMETER(FIntPoint, OutputSize)
        SHADER_PARAMETER(uint32, SelectedLo)
        SHADER_PARAMETER(uint32, SelectedHi)
        SHADER_PARAMETER(uint32, HiddenLo)
        SHADER_PARAMETER(uint32, HiddenHi)
        SHADER_PARAMETER(uint32, BaseTag)
        SHADER_PARAMETER(float, DepthBias)
    END_SHADER_PARAMETER_STRUCT()
    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters&) { return true; }
};
#endif
