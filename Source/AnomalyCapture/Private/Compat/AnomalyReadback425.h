#pragma once
#include "Compat/AnomalyEngineVersion.h"
#if ANOMALY_ENGINE_425 && ANOMALY_CAPTURE
#include "RHIGPUReadback.h"
#include "RHICommandList.h"
#include "AnomalyPrivatePolicy425.h"

namespace AnomalyReadback425
{
    constexpr bool Accept(bool HasData, int W, int H, int Pitch, int MappedH, int BytesPerPixel=4)
    {
        return AnomalyPrivatePolicy425::Layout(HasData,W,H,Pitch,MappedH,BytesPerPixel);
    }
    static_assert(Accept(true, 64, 48, 64, 48), "Exact map must pass");
    static_assert(Accept(true, 64, 48, 80, 48), "Padded rows are copied by pitch");
    static_assert(!Accept(true, 64, 48, 63, 48), "Short pitch must fail");
    static_assert(!Accept(true, 64, 48, 64, 47), "Short height must fail");
    static_assert(!Accept(true, 64, 48, 0, 0), "Zero map must fail");
    static_assert(!Accept(false, 64, 48, 64, 48), "Null map must fail");
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
