#pragma once
#include <cstddef>

namespace AnomalyPrivatePolicy425
{
    constexpr float DepthTolerance = 4.0f / 16777215.0f;
    enum class Kind { Static, Instanced, Hierarchical, Skeletal, Unsupported };
    constexpr bool Supported(Kind Type, bool Registered, bool Surface, bool Translucent, bool Tessellated)
    {
        return Type != Kind::Unsupported && Registered && Surface && !Translucent && !Tessellated;
    }
    constexpr bool Admit(bool HiddenEvent, float PrivateZ, float SceneZ)
    {
        return PrivateZ > 0 && (HiddenEvent ? PrivateZ >= SceneZ - DepthTolerance :
            PrivateZ >= SceneZ - DepthTolerance && PrivateZ <= SceneZ + DepthTolerance);
    }
    constexpr bool Layout(bool Data, int Width, int Height, int Pitch, int Rows, int BytesPerPixel)
    {
        return Data && Width > 0 && Height > 0 && Pitch >= Width && Rows >= Height && BytesPerPixel > 0 &&
            static_cast<unsigned long long>(Pitch) * Rows * BytesPerPixel <= 0x7fffffffULL;
    }
    template<typename Copy>
    bool CopyRows(void* Destination, const void* Source, int Width, int Height, int Pitch, int Rows, int BytesPerPixel, Copy CopyBytes)
    {
        if (!Destination || !Layout(Source != nullptr, Width, Height, Pitch, Rows, BytesPerPixel)) { return false; }
        auto* To = static_cast<unsigned char*>(Destination);
        const auto* From = static_cast<const unsigned char*>(Source);
        for (int Y = 0; Y < Height; ++Y)
        {
            CopyBytes(To + static_cast<std::size_t>(Y) * Width * BytesPerPixel,
                From + static_cast<std::size_t>(Y) * Pitch * BytesPerPixel, static_cast<std::size_t>(Width) * BytesPerPixel);
        }
        return true;
    }
}
