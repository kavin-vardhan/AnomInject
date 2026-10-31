#include "AnomalyReadback425.h"
#if ANOMALY_ENGINE_425 && ANOMALY_CAPTURE
void FAnomalyReadback425::EnqueueCopy(FRHICommandList& Cmd, FRHITexture* Source, const FResolveRect& Rect)
{
    Fence->Clear();
    FRHITexture2D* Texture = Source->GetTexture2D();
    FRHIResourceCreateInfo Info(TEXT("AnomalyStaging425"));
    Staging = RHICreateTexture2D(Texture->GetSizeX(), Texture->GetSizeY(), Texture->GetFormat(), 1, 1,
        TexCreate_CPUReadback | TexCreate_HideInVisualizeTexture, Info);
    Cmd.TransitionResource(EResourceTransitionAccess::EWritable, Staging);
    Cmd.CopyToResolveTarget(Source, Staging, FResolveParams(Rect));
    Cmd.WriteGPUFence(Fence);
}
void* FAnomalyReadback425::Lock(uint32 NumBytes)
{
    int32 Pitch = 0, Height = 0;
    return Lock(Pitch, &Height);
}
void* FAnomalyReadback425::Lock(int32& Pitch, int32* Height)
{
    void* Data = nullptr;
    int32 H = 0;
    FRHICommandListExecutor::GetImmediateCommandList().MapStagingSurface(Staging, Fence, Data, Pitch, H);
    if (Height) { *Height = H; }
    return Data;
}
void FAnomalyReadback425::Unlock()
{
    FRHICommandListExecutor::GetImmediateCommandList().UnmapStagingSurface(Staging);
}
#endif
