#pragma once
#include "Compat/AnomalyEngineVersion.h"
#if ANOMALY_ENGINE_425 && ANOMALY_CAPTURE
#include "RHIGPUReadback.h"
#include "RHICommandList.h"

namespace AnomalyReadback425
{
    constexpr bool Accept(bool HasData, int W, int H, int Pitch, int MappedH)
    {
        return HasData && W > 0 && H > 0 && Pitch == W && MappedH == H;
    }
    static_assert(Accept(true, 1280, 720, 1280, 720), "Exact map must pass");
    static_assert(!Accept(true, 1280, 720, 1344, 720), "Padded map must fail");
    static_assert(!Accept(true, 1280, 720, 1279, 720), "Short pitch must fail");
    static_assert(!Accept(true, 1280, 720, 1280, 719), "Short height must fail");
    static_assert(!Accept(true, 1280, 720, 0, 0), "Zero map must fail");
    static_assert(!Accept(false, 1280, 720, 1280, 720), "Null map must fail");
}

class FAnomalyReadback425 : public FRHIGPUMemoryReadback
{
public:
    explicit FAnomalyReadback425(FName Name) : FRHIGPUMemoryReadback(Name) {}
    void EnqueueCopy(FRHICommandList& Cmd, FRHITexture* Source, FResolveRect Rect = FResolveRect()) override;
    void* Lock(uint32 NumBytes) override;
    void* Lock(int32& Pitch, int32* Height);
    void Unlock() override;
private:
    FTexture2DRHIRef Staging;
};
#endif
