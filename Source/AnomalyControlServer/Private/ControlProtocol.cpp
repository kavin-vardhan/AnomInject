// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#include "ControlProtocol.h"

namespace ControlProtocol
{
	TArray<uint8> BuildFrameHeader(uint32 FrameId, uint32 Epoch, uint16 Width, uint16 Height)
	{
		TArray<uint8> Header;
		Header.Reserve(16);

		const uint8 Magic[4] = { 'A', 'I', 'F', '1' };
		Header.Append(Magic, 4);

		auto PutU32 = [&Header](uint32 V)
		{
			Header.Add((uint8)(V & 0xFF));
			Header.Add((uint8)((V >> 8) & 0xFF));
			Header.Add((uint8)((V >> 16) & 0xFF));
			Header.Add((uint8)((V >> 24) & 0xFF));
		};
		auto PutU16 = [&Header](uint16 V)
		{
			Header.Add((uint8)(V & 0xFF));
			Header.Add((uint8)((V >> 8) & 0xFF));
		};

		PutU32(FrameId);
		PutU32(Epoch);
		PutU16(Width);
		PutU16(Height);
		return Header;
	}
}
