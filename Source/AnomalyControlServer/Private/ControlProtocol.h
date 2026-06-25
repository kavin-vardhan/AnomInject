#pragma once

#include "CoreMinimal.h"

namespace ControlProtocol
{
	static constexpr int32 Version = 1;

	TArray<uint8> BuildFrameHeader(uint32 FrameId, uint32 Epoch, uint16 Width, uint16 Height);
}
