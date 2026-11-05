#if defined(ANOMALY_CONTROL_TRANSPORT_SELFTEST)
#include "Compat/AnomalyControlWire.h"
#include <cstdio>
#include <cstdlib>
static int Checks=0;
static void Require(bool Value) { ++Checks; if (!Value) { std::fprintf(stderr,"FAILED check %d\n",Checks); std::exit(1); } }
int main()
{
    using namespace AnomalyControlWire;
    const std::vector<uint8_t> Json={'{','"','v','"',':','1','}'};
    FPeer Peer; std::vector<uint8_t> Complete;
    Require(Peer.Send(Json.data(),Json.size()));
    Require(*Peer.Front()==Json && Peer.QueuedBytes()==Json.size());
    Peer.Sent(); Require(!Peer.Front() && Peer.QueuedBytes()==0);
    Require(Peer.Feed(Json.data(),2,false,0,Complete) && Complete.empty());
    Require(Peer.Feed(Json.data()+2,2,true,3,Complete) && Complete.empty());
    Require(Peer.Feed(Json.data()+4,3,true,0,Complete) && Complete==Json);
    std::vector<uint8_t> Block(MaxInbound,42);
    Require(Peer.Feed(Block.data(),Block.size(),false,0,Complete));
    Require(!Peer.Feed(Block.data(),1,true,0,Complete));
    Peer.Stop(); Peer.Stop(); Require(!Peer.Feed(Json.data(),Json.size(),true,0,Complete));
    Require(!Peer.Send(Json.data(),Json.size()) && !Peer.Front() && Peer.QueuedBytes()==0);
    FPeer Queue;
    for (size_t I=0;I<MaxOutbound/MaxInbound;++I) { Require(Queue.Send(Block.data(),Block.size())); }
    Require(Queue.QueuedBytes()==MaxOutbound && !Queue.Send(Json.data(),1));
    Queue.Sent(); Require(Queue.Send(Json.data(),Json.size()));
    Queue.Stop(); Require(Queue.QueuedBytes()==0 && !Queue.Front());
    FPeer Tiny;
    for (size_t I=0;I<MaxMessages;++I) { Require(Tiny.Send(Json.data(),1)); }
    Require(!Tiny.Send(Json.data(),1));
    Require(AcceptPeer(7) && !AcceptPeer(8));
    std::printf("PASS %d framing, fragmentation, teardown and bounds checks\n",Checks);
    return 0;
}
#endif
