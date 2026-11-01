#pragma once
#include <cstdint>
namespace AnomalyCensusBatch425
{
    constexpr std::uint64_t LostAfterTicks=8;
    enum class EOutcome { Pending, Credit, NoPass, Polluted, Lost };
    inline EOutcome Outcome(std::uint64_t Now,std::uint64_t Armed,bool Resolved,bool Pass,bool Polluted)
    {
        if (Resolved) { return !Pass ? EOutcome::NoPass : Polluted ? EOutcome::Polluted : EOutcome::Credit; }
        return Now>Armed+LostAfterTicks ? EOutcome::Lost : EOutcome::Pending;
    }
    inline float AnnotationFar(float) { return 1000000.0f; }
}
