#include "../../Source/AnomalyCapture/Private/Compat/AnomalyPrivatePolicy425.h"
#include <algorithm>
#include <cassert>
#include <cstring>
#include <vector>

int main()
{
    using namespace AnomalyPrivatePolicy425;
    for (Kind K:{Kind::Static,Kind::Instanced,Kind::Hierarchical,Kind::Skeletal})
    {
        assert(Supported(K,true,true,false,false));
        assert(!Supported(K,false,true,false,false));
        assert(!Supported(K,true,false,false,false));
        assert(!Supported(K,true,true,true,false));
        assert(!Supported(K,true,true,false,true));
    }
    assert(!Supported(Kind::Unsupported,true,true,false,false));
    assert(Admit(false,0.25f,0.25f));
    assert(!Admit(false,0.5f,0.25f));
    assert(Admit(true,0.5f,0.25f));
    assert(!Admit(true,0.25f,0.5f));
    assert(!Admit(true,0,0));
    for (int Bpp:{4,16})
    {
        for (int Width:{1,1279,1280,1600})
        {
            const int Height=7,Pitch=((Width*Bpp+255)/256)*256/Bpp;
            std::vector<unsigned char> Source(Pitch*(Height+1)*Bpp,0xcd),Destination(Width*Height*Bpp+32,0xaa),Expected=Destination;
            for (int Y=0;Y<Height;++Y)
            {
                for (int X=0;X<Width*Bpp;++X) { Source[Y*Pitch*Bpp+X]=Expected[Y*Width*Bpp+X]=static_cast<unsigned char>((Y*17+X)%251); }
            }
            assert(CopyRows(Destination.data(),Source.data(),Width,Height,Pitch,Height+1,Bpp,std::memcpy));
            assert(Destination==Expected);
            auto Wrong=Destination; std::memcpy(Wrong.data(),Source.data(),Width*Height*Bpp);
            if (Pitch!=Width) { assert(Wrong!=Expected); }
            for (const auto Bad:{0,Width-1})
            {
                assert(!Layout(true,Width,Height,Bad,Height,Bpp));
                auto Before=Destination;
                assert(!CopyRows(Destination.data(),Source.data(),Width,Height,Bad,Height,Bpp,std::memcpy));
                assert(Destination==Before);
            }
            assert(!CopyRows(Destination.data(),Source.data(),Width,Height,Pitch,Height-1,Bpp,std::memcpy));
            assert(!CopyRows(Destination.data(),nullptr,Width,Height,Pitch,Height,Bpp,std::memcpy));
        }
    }
    assert(!Layout(true,1,1,0x7fffffff,0x7fffffff,16));
    return 0;
}
