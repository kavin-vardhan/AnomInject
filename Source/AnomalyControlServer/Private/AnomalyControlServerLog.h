// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#pragma once

#include "Logging/LogMacros.h"

/**
 * Dedicated log category for the control-server module. NOT the core module's LogAnomaly: UE log
 * categories are not exported across module DLL boundaries (DEFINE_LOG_CATEGORY emits an un-decorated
 * symbol with no module API macro), so a dependent module cannot link the core's category. Reusing
 * LogAnomaly here produced an LNK2001 in the Slice-0 build — each module owns its own category.
 */
DECLARE_LOG_CATEGORY_EXTERN(LogAnomalyServer, Log, All);
