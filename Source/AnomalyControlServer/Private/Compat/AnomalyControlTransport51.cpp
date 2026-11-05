#include "AnomalyControlTransport.h"
#if ANOMALY_CONTROL_SERVER && ANOMALY_ENGINE_51
#include "Modules/ModuleManager.h"
#include "IWebSocketNetworkingModule.h"
#include "IWebSocketServer.h"
#include "INetworkingWebSocket.h"
#include "WebSocketNetworkingDelegates.h"
class FAnomalyControlTransport51 final : public IAnomalyControlTransport
{
    TUniquePtr<IWebSocketServer> Server;
    TMap<uint64,INetworkingWebSocket*> Peers;
    FAnomalyControlCallbacks Callbacks;
    uint64 NextId=1;
    bool Stopping=true;
    void Closed(uint64 Id)
    {
        if (!Stopping && Peers.Remove(Id) && Callbacks.Close) { Callbacks.Close(Id); }
    }
    void Connected(INetworkingWebSocket* Socket)
    {
        if (Stopping || !Socket) { return; }
        const uint64 Id=NextId++; Peers.Add(Id,Socket);
        if (Callbacks.Open) { Callbacks.Open(Id,Socket->RemoteEndPoint(false)); }
        Socket->SetReceiveCallBack(FWebSocketPacketReceivedCallBack::CreateLambda([this,Id](void* Data,int32 Size)
        {
            if (!Stopping && Peers.Contains(Id) && Data && Size>0 && Callbacks.Message)
            {
                TArray<uint8> Bytes; Bytes.Append(static_cast<const uint8*>(Data),Size); Callbacks.Message(Id,Bytes);
            }
        }));
        Socket->SetSocketClosedCallBack(FWebSocketInfoCallBack::CreateLambda([this,Id]() { Closed(Id); }));
        Socket->SetErrorCallBack(FWebSocketInfoCallBack::CreateLambda([this,Id]() { Closed(Id); }));
    }
public:
    ~FAnomalyControlTransport51() override { Stop(); }
    bool Start(int32 Port,FAnomalyControlCallbacks InCallbacks) override
    {
        auto* Module=FModuleManager::LoadModulePtr<IWebSocketNetworkingModule>(TEXT("WebSocketNetworking"));
        if (!Module) { return false; }
        Server=Module->CreateServer(); if (!Server) { return false; }
        Callbacks=MoveTemp(InCallbacks); Stopping=false;
        if (!Server->Init(uint32(Port),FWebSocketClientConnectedCallBack::CreateRaw(this,&FAnomalyControlTransport51::Connected))) { Stop(); return false; }
        return true;
    }
    void Tick() override { if (Server && !Stopping) { Server->Tick(); } }
    bool Send(uint64 Id,const uint8* Data,int32 Size) override
    {
        auto* Socket=Peers.Find(Id); if (Stopping || !Socket || !Data || Size<=0) { return false; }
        (*Socket)->Send(Data,uint32(Size),false); return true;
    }
    void Stop() override
    {
        Stopping=true; Callbacks=FAnomalyControlCallbacks();
        for (const auto& Pair:Peers)
        {
            Pair.Value->SetReceiveCallBack(FWebSocketPacketReceivedCallBack());
            Pair.Value->SetSocketClosedCallBack(FWebSocketInfoCallBack());
            Pair.Value->SetErrorCallBack(FWebSocketInfoCallBack());
        }
        Peers.Reset(); Server.Reset();
    }
};
TUniquePtr<IAnomalyControlTransport> MakeAnomalyControlTransport() { return MakeUnique<FAnomalyControlTransport51>(); }
#endif
