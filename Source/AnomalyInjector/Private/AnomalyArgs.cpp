// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#include "AnomalyArgs.h"

#include "AnomalyInjectorLog.h"

namespace AnomalyArgs
{
	float GetFloat(const TArray<FString>& Args, int32 Index, float Default, float Min, float Max)
	{
		if (!Args.IsValidIndex(Index))
		{
			return Default;   // omitted optional arg -> default, silently
		}

		const FString& Token = Args[Index];
		if (!Token.IsNumeric())
		{
			UE_LOG(LogAnomaly, Warning, TEXT("args[%d] '%s' is not a number; using default %.4f."),
				Index, *Token, Default);
			return Default;
		}

		const float Value = FCString::Atof(*Token);
		const float Clamped = FMath::Clamp(Value, Min, Max);
		if (!FMath::IsNearlyEqual(Value, Clamped))
		{
			UE_LOG(LogAnomaly, Warning, TEXT("args[%d] %.4f out of range [%.4f, %.4f]; clamped to %.4f."),
				Index, Value, Min, Max, Clamped);
		}
		return Clamped;
	}

	int32 GetInt(const TArray<FString>& Args, int32 Index, int32 Default, int32 Min, int32 Max)
	{
		if (!Args.IsValidIndex(Index))
		{
			return Default;
		}

		const FString& Token = Args[Index];
		if (!Token.IsNumeric())
		{
			UE_LOG(LogAnomaly, Warning, TEXT("args[%d] '%s' is not a number; using default %d."),
				Index, *Token, Default);
			return Default;
		}

		const int32 Value = FCString::Atoi(*Token);
		const int32 Clamped = FMath::Clamp(Value, Min, Max);
		if (Value != Clamped)
		{
			UE_LOG(LogAnomaly, Warning, TEXT("args[%d] %d out of range [%d, %d]; clamped to %d."),
				Index, Value, Min, Max, Clamped);
		}
		return Clamped;
	}

	FString GetString(const TArray<FString>& Args, int32 Index, const FString& Default)
	{
		return Args.IsValidIndex(Index) ? Args[Index] : Default;
	}
}
