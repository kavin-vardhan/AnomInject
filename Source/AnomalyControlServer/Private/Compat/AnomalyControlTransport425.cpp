#include "AnomalyControlTransport.h"
#if ANOMALY_CONTROL_SERVER && ANOMALY_ENGINE_425
#include "AnomalyControlWire.h"
#include "Windows/AllowWindowsPlatformTypes.h"
#define UI UI_ST
THIRD_PARTY_INCLUDES_START
#include "libwebsockets.h"
#include <iphlpapi.h>
THIRD_PARTY_INCLUDES_END
#undef UI
#include "Windows/HideWindowsPlatformTypes.h"

class FAnomalyControlTransport425 final : public IAnomalyControlTransport
{
    struct FPeer
    {
        lws* Socket=nullptr;
        AnomalyControlWire::FPeer Wire;
    };
    struct FSession { uint64 Id; };
    struct FEvent
    {
        uint64 Id=0;
        int32 Kind=0;
        FString Remote;
        TArray<uint8> Bytes;
    };
    lws_context* Context=nullptr;
    lws_protocols Protocols[2]{};
    TMap<uint64,TUniquePtr<FPeer>> Peers;
    TArray<FEvent> Events;
    FAnomalyControlCallbacks Callbacks;
    uint64 NextId=1;
    size_t QueuedInbound=0;
    bool Stopping=true;

    static int32 OwnedListeners(int32 Port,bool& LoopbackOnly)
    {
        LoopbackOnly=true;
        DWORD Size=0;
        DWORD Result=GetExtendedTcpTable(nullptr,&Size,false,AF_INET,TCP_TABLE_OWNER_PID_LISTENER,0);
        for (int32 Attempt=0;Attempt<3 && Result==ERROR_INSUFFICIENT_BUFFER;++Attempt)
        {
            if (!Size || Size>16*1024*1024) { return -1; }
            TArray<uint8> Buffer; Buffer.SetNumUninitialized(int32(Size));
            Result=GetExtendedTcpTable(Buffer.GetData(),&Size,false,AF_INET,TCP_TABLE_OWNER_PID_LISTENER,0);
            if (Result==NO_ERROR)
            {
                const auto* Table=reinterpret_cast<const MIB_TCPTABLE_OWNER_PID*>(Buffer.GetData());
                int32 Count=0;
                for (DWORD I=0;I<Table->dwNumEntries;++I)
                {
                    const auto& Row=Table->table[I];
                    if (Row.dwOwningPid==GetCurrentProcessId() && ntohs(u_short(Row.dwLocalPort))==Port)
                    {
                        ++Count;
                        LoopbackOnly=LoopbackOnly && ntohl(Row.dwLocalAddr)==0x7f000001;
                    }
                }
                return Count;
            }
        }
        return -1;
    }

    static int Callback(lws* Socket,lws_callback_reasons Reason,void* User,void* Input,size_t Size)
    {
        auto* Self=static_cast<FAnomalyControlTransport425*>(lws_context_user(lws_get_context(Socket)));
        return Self && !Self->Stopping ? Self->Service(Socket,Reason,static_cast<FSession*>(User),Input,Size) : 0;
    }
    int Service(lws* Socket,lws_callback_reasons Reason,FSession* Session,void* Input,size_t Size)
    {
        if (Reason==LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION)
        {
            char Path[256]{};
            lws_hdr_copy(Socket,Path,sizeof(Path),WSI_TOKEN_GET_URI);
            return !AnomalyControlWire::AcceptPeer(Peers.Num()) || FCStringAnsi::Strcmp(Path,"/")!=0 ? 1 : 0;
        }
        if (Reason==LWS_CALLBACK_ESTABLISHED)
        {
            if (!Session || !AnomalyControlWire::AcceptPeer(Peers.Num()) || Events.Num()>=1024) { return -1; }
            Session->Id=NextId++;
            auto Peer=MakeUnique<FPeer>(); Peer->Socket=Socket; Peers.Add(Session->Id,MoveTemp(Peer));
            char Address[128]{}; lws_get_peer_simple(Socket,Address,sizeof(Address));
            FEvent Event; Event.Id=Session->Id; Event.Kind=0; Event.Remote=ANSI_TO_TCHAR(Address); Events.Add(MoveTemp(Event));
        }
        else if (Reason==LWS_CALLBACK_RECEIVE)
        {
            auto* Peer=Session ? Peers.Find(Session->Id) : nullptr;
            if (!Peer) { return -1; }
            std::vector<uint8_t> Complete;
            if (!(*Peer)->Wire.Feed(Input,Size,lws_is_final_fragment(Socket)!=0,lws_remaining_packet_payload(Socket),Complete)) { return -1; }
            if (!Complete.empty())
            {
                if (Complete.size()>AnomalyControlWire::MaxInbound*AnomalyControlWire::MaxConnections-QueuedInbound || Events.Num()>=1024) { return -1; }
                FEvent Event; Event.Id=Session->Id; Event.Kind=1;
                Event.Bytes.Append(Complete.data(),int32(Complete.size())); QueuedInbound+=Complete.size(); Events.Add(MoveTemp(Event));
            }
        }
        else if (Reason==LWS_CALLBACK_SERVER_WRITEABLE)
        {
            auto* Peer=Session ? Peers.Find(Session->Id) : nullptr;
            if (!Peer) { return -1; }
            const auto* Message=(*Peer)->Wire.Front();
            if (Message)
            {
                TArray<uint8> Buffer; Buffer.SetNumUninitialized(int32(LWS_PRE+Message->size()));
                FMemory::Memcpy(Buffer.GetData()+LWS_PRE,Message->data(),Message->size());
                const int Written=lws_write(Socket,Buffer.GetData()+LWS_PRE,Message->size(),LWS_WRITE_BINARY);
                if (Written<0 || size_t(Written)!=Message->size()) { return -1; }
                (*Peer)->Wire.Sent();
                if ((*Peer)->Wire.Front()) { lws_callback_on_writable(Socket); }
            }
        }
        else if (Reason==LWS_CALLBACK_CLOSED)
        {
            if (Session && Peers.Remove(Session->Id))
            {
                FEvent Event; Event.Id=Session->Id; Event.Kind=2; Events.Add(MoveTemp(Event));
            }
        }
        else if (Reason==LWS_CALLBACK_HTTP) { return -1; }
        return 0;
    }
public:
    ~FAnomalyControlTransport425() override { Stop(); }
    bool Start(int32 Port,FAnomalyControlCallbacks InCallbacks) override
    {
        check(IsInGameThread());
        if (Context || Port<1 || Port>65535) { return false; }
        bool LoopbackOnly=false;
        if (OwnedListeners(Port,LoopbackOnly)!=0) { return false; }
        Callbacks=MoveTemp(InCallbacks); Stopping=false;
        Protocols[0].name="anomaly-control"; Protocols[0].callback=&Callback;
        Protocols[0].per_session_data_size=sizeof(FSession); Protocols[0].rx_buffer_size=64*1024;
        lws_context_creation_info Info{}; Info.port=Port; Info.iface="127.0.0.1";
        Info.protocols=Protocols; Info.gid=-1; Info.uid=-1; Info.user=this; Info.options=LWS_SERVER_OPTION_DISABLE_IPV6;
        Context=lws_create_context(&Info);
        if (!Context || OwnedListeners(Port,LoopbackOnly)!=1 || !LoopbackOnly) { Stop(); return false; }
        return true;
    }
    void Tick() override
    {
        check(IsInGameThread());
        if (!Context || Stopping) { return; }
        lws_service(Context,0);
        TArray<FEvent> Ready=MoveTemp(Events); Events.Reset(); QueuedInbound=0;
        for (const auto& Event:Ready)
        {
            if (Stopping) { break; }
            if (Event.Kind==0 && Callbacks.Open) { Callbacks.Open(Event.Id,Event.Remote); }
            else if (Event.Kind==1 && Callbacks.Message) { Callbacks.Message(Event.Id,Event.Bytes); }
            else if (Event.Kind==2 && Callbacks.Close) { Callbacks.Close(Event.Id); }
        }
    }
    bool Send(uint64 Id,const uint8* Data,int32 Size) override
    {
        check(IsInGameThread());
        auto* Peer=Peers.Find(Id);
        if (Stopping || Size<=0 || !Peer || !(*Peer)->Wire.Send(Data,size_t(Size))) { return false; }
        lws_callback_on_writable((*Peer)->Socket); return true;
    }
    void Stop() override
    {
        check(IsInGameThread());
        Stopping=true; Callbacks=FAnomalyControlCallbacks();
        if (Context) { lws_context_destroy(Context); Context=nullptr; }
        for (auto& Peer:Peers) { Peer.Value->Wire.Stop(); }
        Peers.Reset(); Events.Reset(); QueuedInbound=0;
    }
};
TUniquePtr<IAnomalyControlTransport> MakeAnomalyControlTransport() { return MakeUnique<FAnomalyControlTransport425>(); }
#endif
