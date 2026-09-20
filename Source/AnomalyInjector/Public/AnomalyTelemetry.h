#pragma once

#include "CoreMinimal.h"

struct FAnomalyTelemetry
{
	TArray<TPair<FName, int32>> Ints;
	TArray<TPair<FName, bool>> Bools;
	TArray<TPair<FName, FString>> Strings;

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

	void AddInt(FName Key, int32 Value)
	{
		Ints.Emplace(Key, Value);
	}

	void AddBool(FName Key, bool bValue)
	{
		Bools.Emplace(Key, bValue);
	}

	void AddString(FName Key, const FString& Value)
	{
		Strings.Emplace(Key, Value);
	}
};
