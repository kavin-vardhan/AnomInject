#pragma once

#include "CoreMinimal.h"

struct FAnomalyTelemetryFields
{
	TArray<TPair<FString, int32>> Ints;
	TArray<TPair<FString, bool>> Bools;
	TArray<TPair<FString, FString>> Strings;

	bool IsEmpty() const
	{
		return Ints.Num() == 0 && Bools.Num() == 0 && Strings.Num() == 0;
	}

	void Reset()
	{
		Ints.Reset();
		Bools.Reset();
		Strings.Reset();
	}

	void AddInt(const FString& Key, int32 Value)
	{
		Ints.Emplace(Key, Value);
	}

	void AddBool(const FString& Key, bool bValue)
	{
		Bools.Emplace(Key, bValue);
	}

	void AddString(const FString& Key, const FString& Value)
	{
		Strings.Emplace(Key, Value);
	}
};

struct FAnomalyTelemetry : public FAnomalyTelemetryFields
{
	TArray<TPair<FString, TArray<FAnomalyTelemetryFields>>> Arrays;

	bool IsEmpty() const
	{
		return FAnomalyTelemetryFields::IsEmpty() && Arrays.Num() == 0;
	}

	void Reset()
	{
		FAnomalyTelemetryFields::Reset();
		Arrays.Reset();
	}

	FAnomalyTelemetryFields& AddArrayEntry(const FString& Key)
	{
		for (TPair<FString, TArray<FAnomalyTelemetryFields>>& KV : Arrays)
		{
			if (KV.Key == Key)
			{
				return KV.Value.AddDefaulted_GetRef();
			}
		}
		TPair<FString, TArray<FAnomalyTelemetryFields>>& New = Arrays.Emplace_GetRef(Key, TArray<FAnomalyTelemetryFields>());
		return New.Value.AddDefaulted_GetRef();
	}
};
