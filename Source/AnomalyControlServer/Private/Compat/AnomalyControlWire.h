#pragma once
#include <cstddef>
#include <cstdint>
#include <deque>
#include <vector>
#include <utility>
namespace AnomalyControlWire
{
    constexpr size_t MaxInbound=1024*1024,MaxOutbound=16*1024*1024,MaxConnections=8,MaxMessages=512;
    inline bool AcceptPeer(size_t Count) { return Count<MaxConnections; }
    class FPeer
    {
    public:
        bool Feed(const void* Data,size_t Size,bool Final,size_t Remaining,std::vector<uint8_t>& Complete)
        {
            Complete.clear();
            if (!Active || Size>MaxInbound-Inbound.size() || (Size && !Data)) { return false; }
            if (Size) { const auto* Bytes=static_cast<const uint8_t*>(Data); Inbound.insert(Inbound.end(),Bytes,Bytes+Size); }
            if (Final && Remaining==0) { Complete=std::move(Inbound); Inbound.clear(); }
            return true;
        }
        bool Send(const uint8_t* Data,size_t Size)
        {
            if (!Active || !Data || !Size || Size>MaxOutbound-Queued || Outbound.size()>=MaxMessages) { return false; }
            Outbound.emplace_back(Data,Data+Size); Queued+=Size; return true;
        }
        const std::vector<uint8_t>* Front() const { return Active && !Outbound.empty() ? &Outbound.front() : nullptr; }
        void Sent() { if (Front()) { Queued-=Outbound.front().size(); Outbound.pop_front(); } }
        void Stop() { Active=false; Inbound.clear(); Outbound.clear(); Queued=0; }
        size_t QueuedBytes() const { return Queued; }
    private:
        bool Active=true;
        size_t Queued=0;
        std::vector<uint8_t> Inbound;
        std::deque<std::vector<uint8_t>> Outbound;
    };
}
