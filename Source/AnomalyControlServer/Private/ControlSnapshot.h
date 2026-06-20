// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class UWorld;

/**
 * ControlSnapshot (Slice 1) — assembles the game->dashboard read-back JSON from the injector / auto-injector
 * / selector / viewport read-backs. Pure read: it never mutates game state. Serialized to a WS TEXT frame.
 */
namespace ControlSnapshot
{
	/**
	 * The full read-back snapshot: view + renderable-visible set (with screen-rects, AS-IS — no server-side
	 * filtering) + active anomalies (id/target/args/source/time) + auto-injector state + session flags + fps.
	 * `Epoch` is echoed for frame<->snapshot overlay correlation.
	 */
	FString BuildSnapshotJson(UWorld* World, uint32 Epoch);

	/** The anomaly catalog (id/description/usage/scope/arg-schema) — the reply to a `list_anomalies` command. */
	FString BuildCatalogJson(UWorld* World);
}
