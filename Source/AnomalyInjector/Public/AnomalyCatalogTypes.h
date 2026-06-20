// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * Structured read-back types for the control surface (Slice 1, A1/A2). Plain C++ (not USTRUCT — these
 * are serialized manually by the control-server module; the project uses reflection only where needed,
 * mirroring IAnomaly). Carried so the dashboard/server render generically and a NEW anomaly needs zero
 * control-stack change: it adds its authored {scope, args} spec next to its Register() line.
 *
 * IAnomaly is LOCKED and untouched — scope + arg schema are sourced from a registration-time authored
 * table in the injector, NOT from parsing GetUsage() (a human hint; see gotcha G31).
 */

/** Targeting kind. Drives the inject flow: a target picker is shown only for non-Global scopes. */
enum class EAnomalyScope : uint8
{
	Object,     // targets an on-screen actor by name (the "=<name>" exact-match pool): missing_object, flicker, lod_*
	Component,  // targets matching components by name, not from the renderable visible set: lighting_mismatch
	Global      // no target (whole-frame / world): time_dilation, camera_clipping
};

/** Argument value kind, so the dashboard builds the right control (slider / dropdown / checkbox / text). */
enum class EAnomalyArgType : uint8
{
	Float,
	Int,
	Enum,
	Bool,
	String
};

/** One tunable argument of an anomaly. Numeric bounds apply to Float/Int; Options to Enum. */
struct FAnomalyArgSpec
{
	FString Name;
	EAnomalyArgType Type = EAnomalyArgType::String;
	bool bRequired = false;
	FString Default;                 // string form; the client coerces by Type

	bool bHasMin = false;
	double Min = 0.0;
	bool bHasMax = false;
	double Max = 0.0;

	TArray<FString> Options;         // Enum choices (empty otherwise)
};

/** A catalog entry: identity + human help (from IAnomaly) + authored targeting scope and arg schema. */
struct FAnomalyCatalogEntry
{
	FName Id;
	FString Description;
	FString Usage;                   // human hint, kept as a help/fallback
	EAnomalyScope Scope = EAnomalyScope::Object;
	TArray<FAnomalyArgSpec> Args;
};

/** A currently-active anomaly, for the Active panel read-back. Target/source are composed server-side
 *  (target from Args[0] "=<name>" for object scope or from the auto-injector's live fires; source by
 *  cross-referencing the auto-injector) — the injector reports only what it knows. */
struct FActiveAnomalyInfo
{
	FName Id;
	TArray<FString> Args;            // exactly what ApplyAnomaly was called with
	double SecondsActive = 0.0;
};

/** Wire-name helpers (for the control server's JSON). */
inline const TCHAR* ToString(EAnomalyScope Scope)
{
	switch (Scope)
	{
	case EAnomalyScope::Object:    return TEXT("object");
	case EAnomalyScope::Component: return TEXT("component");
	case EAnomalyScope::Global:    return TEXT("global");
	}
	return TEXT("object");
}

inline const TCHAR* ToString(EAnomalyArgType Type)
{
	switch (Type)
	{
	case EAnomalyArgType::Float:  return TEXT("float");
	case EAnomalyArgType::Int:    return TEXT("int");
	case EAnomalyArgType::Enum:   return TEXT("enum");
	case EAnomalyArgType::Bool:   return TEXT("bool");
	case EAnomalyArgType::String: return TEXT("string");
	}
	return TEXT("string");
}
