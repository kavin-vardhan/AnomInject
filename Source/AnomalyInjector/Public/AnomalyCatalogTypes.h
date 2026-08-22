#pragma once

#include "CoreMinimal.h"


enum class EAnomalyScope : uint8
{
	Object,
	Component,
	Global
};

enum class EAnomalyArgType : uint8
{
	Float,
	Int,
	Enum,
	Bool,
	String
};

struct FAnomalyArgSpec
{
	FString Name;
	EAnomalyArgType Type = EAnomalyArgType::String;
	bool bRequired = false;
	FString Default;

	bool bHasMin = false;
	double Min = 0.0;
	bool bHasMax = false;
	double Max = 0.0;

	TArray<FString> Options;
};

struct FAnomalyCatalogEntry
{
	FName Id;
	FString Description;
	FString Usage;
	EAnomalyScope Scope = EAnomalyScope::Object;
	bool bTargetable = true;
	TArray<FAnomalyArgSpec> Args;
};

struct FActiveAnomalyInfo
{
	FName Id;
	TArray<FString> Args;
	double SecondsActive = 0.0;
};

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
