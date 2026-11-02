#include "AnomalyCaptureAdmission425.h"
#include <cassert>
#include <iostream>
#include <set>
int main()
{
    using namespace AnomalyCaptureAdmission425;
    assert(Classify(1,4,0)==Depth::Reader);
    assert(Classify(0,4,0)==Depth::Writer);
    assert(Classify(0,4,1)==Depth::Unknown);
    assert(Classify(0,0,0)==Depth::None);
    for (auto D:{Depth::Reader,Depth::Unknown})
    {
        assert(!Allowed(D,false,false)); assert(!Allowed(D,false,true)); assert(!Allowed(D,true,false));
        assert(Allowed(D,true,true));
    }
    assert(Allowed(Depth::Writer,false,false));
    std::set<std::string> Pool={"blinking","lod_corruption","lighting_mismatch","missing_object"};
    auto Known=[&](const std::string& Id) { return Pool.count(Id)>0; };
    std::vector<std::string> Out;
    assert(Routes("",false,Known,Out)); assert((Out==std::vector<std::string>{"blinking","lod_corruption"}));
    assert(Routes("blinking,lighting_mismatch,lod_corruption,missing_object",true,Known,Out) && Out.size()==4);
    for (const auto* Text:{"","unknown","blinking,","blinking,,lod_corruption","blinking,blinking","lighting_mismatch,unknown"})
    {
        assert(!Routes(Text,true,Known,Out)); assert(Out.empty());
    }
    std::cout<<"PASS: reader/writer/unknown admission, bench-only bypass, default/explicit routes and invalid lists\n";
}
