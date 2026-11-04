#pragma once
#include <string>
#include <vector>
#include <algorithm>

namespace AnomalyCaptureAdmission425
{
    enum class Depth { None, Writer, Reader, Unknown };
    inline Depth Classify(int Readers,int Writers,int Unknown)
    {
        return Readers ? Depth::Reader : Unknown ? Depth::Unknown : Writers ? Depth::Writer : Depth::None;
    }
    inline bool Allowed(Depth Result,bool Bench,bool Bypass)
    {
        return (Result!=Depth::Reader && Result!=Depth::Unknown) || (Bench && Bypass);
    }
    template<class Known> bool Routes(const std::string& Text,bool Explicit,Known InPool,std::vector<std::string>& Out)
    {
        Out.clear();
        const std::string Input=Explicit ? Text : "blinking,lod_corruption,missing_texture,corrupted_texture";
        std::size_t Start=0;
        do
        {
            const auto End=Input.find(',',Start);
            const auto Id=Input.substr(Start,End==std::string::npos ? End : End-Start);
            if (Id.empty() || !InPool(Id) || std::find(Out.begin(),Out.end(),Id)!=Out.end()) { Out.clear(); return false; }
            Out.push_back(Id);
            if (End==std::string::npos) { break; }
            Start=End+1;
        } while (Start<=Input.size());
        return !Out.empty();
    }
}
