#pragma once

#include "CoreMinimal.h"

#if ANOMALY_CONTROL_SERVER
#include "IWebSocketServer.h"
class INetworkingWebSocket;
struct FControlConn
{
	INetworkingWebSocket* Socket = nullptr;
	bool bAuthed = false;
	bool bRejected = false;
	double ConnectTime = 0.0;
	bool bSubSnapshot = false;
	bool bSubFrames = false;
};

struct FAnomalyControlTransportState
{
	TUniquePtr<IWebSocketServer> Server;
	TArray<FControlConn> Conns;

	FString Token;
	int32 Port = 8077;

	double SnapshotIntervalSec = 0.2;
	double FrameIntervalSec = 0.5;
	double LastSnapshotTime = 0.0;
	double LastFrameTime = 0.0;
	bool bWantOneFrame = false;

	uint32 FrameCounter = 0;
	uint32 ViewEpoch = 0;
	int32 LastViewportX = 0;
	int32 LastViewportY = 0;

};
#else
struct FAnomalyControlTransportState {};
#endif
