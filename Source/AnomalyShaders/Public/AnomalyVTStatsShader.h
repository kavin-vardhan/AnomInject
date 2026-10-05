#pragma once

#include "CoreMinimal.h"

#if ANOMALY_SHADERS
#include "GlobalShader.h"
#include "ShaderParameterStruct.h"

class FAnomalyVTStatsCS : public FGlobalShader
{
public:
    DECLARE_EXPORTED_SHADER_TYPE(FAnomalyVTStatsCS, Global, ANOMALYSHADERS_API);
    SHADER_USE_PARAMETER_STRUCT(FAnomalyVTStatsCS, FGlobalShader);

    BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
        SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, PhysicalTexture)
        SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<uint4>, PageTable)
        SHADER_PARAMETER_RDG_BUFFER_UAV(RWBuffer<uint>, OutStats)
        SHADER_PARAMETER(FIntPoint, SourceSize)
        SHADER_PARAMETER(FIntPoint, PageOrigin)
        SHADER_PARAMETER(uint32, TileSize)
        SHADER_PARAMETER(uint32, BorderSize)
        SHADER_PARAMETER(uint32, CoordinateBits)
    END_SHADER_PARAMETER_STRUCT()

    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters&) { return true; }
};
#endif
