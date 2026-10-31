#pragma once
#include "AnomalyCaptureFeatures.h"
#if ANOMALY_CAPTURE_FRAMES425
#include "CoreMinimal.h"
#include "PixelFormat.h"
namespace AnomalyPixel425
{
    struct FPixel { uint8 R; uint8 G; uint8 B; bool Valid; };
    constexpr bool Accept(EPixelFormat Format)
    {
        return Format == PF_B8G8R8A8 || Format == PF_R8G8B8A8 || Format == PF_A2B10G10R10;
    }
    constexpr uint8 Channel10(uint32 V) { return uint8((V * 255u + 511u) / 1023u); }
    constexpr uint32 Word(const uint8* P)
    {
        return uint32(P[0]) | (uint32(P[1]) << 8) | (uint32(P[2]) << 16) | (uint32(P[3]) << 24);
    }
    constexpr FPixel Decode(EPixelFormat Format, uint32 V)
    {
        return Format == PF_A2B10G10R10 ? FPixel{Channel10(V & 1023u), Channel10((V >> 10) & 1023u), Channel10((V >> 20) & 1023u), true} :
            Format == PF_B8G8R8A8 ? FPixel{uint8(V >> 16), uint8(V >> 8), uint8(V), true} :
            Format == PF_R8G8B8A8 ? FPixel{uint8(V), uint8(V >> 8), uint8(V >> 16), true} : FPixel{0, 0, 0, false};
    }
    constexpr uint8 Synthetic10[] = {0, 0, 0, 0, 255, 3, 8, 192, 1, 252, 247, 63};
    static_assert(Decode(PF_A2B10G10R10, Word(Synthetic10)).R == 0, "10-bit zero");
    static_assert(Decode(PF_A2B10G10R10, Word(Synthetic10 + 4)).R == 255, "10-bit red maximum");
    static_assert(Decode(PF_A2B10G10R10, Word(Synthetic10 + 4)).G == 128, "10-bit green midpoint");
    static_assert(Decode(PF_A2B10G10R10, Word(Synthetic10 + 4)).B == 0, "10-bit blue zero; alpha ignored");
    static_assert(Decode(PF_A2B10G10R10, Word(Synthetic10 + 8)).R == 0, "10-bit one rounds down");
    static_assert(Decode(PF_A2B10G10R10, Word(Synthetic10 + 8)).G == 127, "10-bit 511 rounds to 127");
    static_assert(Decode(PF_A2B10G10R10, Word(Synthetic10 + 8)).B == 255, "10-bit blue maximum");
    static_assert(!Accept(PF_FloatRGBA) && !Decode(PF_FloatRGBA, 0xffffffffu).Valid, "Unsupported format refused");
    static_assert(Decode(PF_B8G8R8A8, 0x12345678u).R == 52 && Decode(PF_R8G8B8A8, 0x12345678u).R == 120, "8-bit channels retained");
}
#endif
