// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * ControlProtocol (Slice 1) — the transport-stable wire helpers shared by the control server.
 * Control + read-back ride WS TEXT frames (JSON); preview rides WS BINARY frames (this header + JPEG).
 */
namespace ControlProtocol
{
	/** Protocol version carried on every JSON message (additive-stable: bump to evolve, never repurpose). */
	static constexpr int32 Version = 1;

	/**
	 * Build the 16-byte little-endian preview-frame header that prefixes the JPEG bytes in a binary frame:
	 *   magic "AIF1" (4) | uint32 frameId | uint32 epoch | uint16 width | uint16 height.
	 * The client sniffs the magic to tell a frame from a JSON message, and matches `epoch` to a snapshot.
	 */
	TArray<uint8> BuildFrameHeader(uint32 FrameId, uint32 Epoch, uint16 Width, uint16 Height);
}
