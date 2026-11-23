#include "AnomalyTargeting.h"

#include "AnomalyTargetPolicy.h"

#include "EngineUtils.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

namespace
{
	FString ShortNameOfPath(const FString& Path)
	{
		int32 Dot = INDEX_NONE;
		int32 Colon = INDEX_NONE;
		Path.FindLastChar(TEXT('.'), Dot);
		Path.FindLastChar(TEXT(':'), Colon);
		const int32 Cut = FMath::Max(Dot, Colon);
		return Cut == INDEX_NONE ? Path : Path.RightChop(Cut + 1);
	}
}

namespace AnomalyTargeting
{
	TArray<TWeakObjectPtr<AActor>> FindActorsMatching(UWorld* World, const FString& Query)
	{
		TArray<TWeakObjectPtr<AActor>> Result;
		if (!World || Query.IsEmpty())
		{
			return Result;
		}

		const AnomalyTargetPolicy::ETargetToken Kind = AnomalyTargetPolicy::ClassifyTargetToken((int)Query[0]);
		const bool bPath = Kind == AnomalyTargetPolicy::ETargetToken::Path;
		const bool bExact = Kind == AnomalyTargetPolicy::ETargetToken::ExactName;
		const FString Needle = (bPath || bExact) ? Query.RightChop(1) : Query;
		if (Needle.IsEmpty())
		{
			return Result;
		}

		if (bPath)
		{
			const FString ShortName = ShortNameOfPath(Needle);
			for (TActorIterator<AActor> It(World); It; ++It)
			{
				AActor* Actor = *It;
				if (Actor && Actor->GetName().Equals(ShortName, ESearchCase::IgnoreCase)
					&& Actor->GetPathName().Equals(Needle, ESearchCase::IgnoreCase))
				{
					Result.Add(Actor);
					break;
				}
			}
			return Result;
		}

		for (TActorIterator<AActor> It(World); It; ++It)
		{
			AActor* Actor = *It;
			if (!Actor)
			{
				continue;
			}

			bool bMatch;
			if (bExact)
			{
				bMatch = Actor->GetName().Equals(Needle, ESearchCase::IgnoreCase)
					|| Actor->GetClass()->GetName().Equals(Needle, ESearchCase::IgnoreCase);
			}
			else
			{
				bMatch = Actor->GetName().Contains(Needle)
					|| Actor->GetClass()->GetName().Contains(Needle);
			}

			if (bMatch)
			{
				Result.Add(Actor);
			}
		}
		return Result;
	}

	int32 CountActorsNamed(UWorld* World, const FString& Name, TArray<FString>* OutPaths, int32 MaxPaths)
	{
		int32 Count = 0;
		if (!World || Name.IsEmpty())
		{
			return 0;
		}
		for (TActorIterator<AActor> It(World); It; ++It)
		{
			AActor* Actor = *It;
			if (Actor && Actor->GetName().Equals(Name, ESearchCase::IgnoreCase))
			{
				++Count;
				if (OutPaths && OutPaths->Num() < MaxPaths)
				{
					OutPaths->Add(Actor->GetPathName());
				}
			}
		}
		return Count;
	}

	FString MakePathToken(const AActor* Actor)
	{
		return Actor ? FString(TEXT("@")) + Actor->GetPathName() : FString();
	}

	FString DescribeFirstPaths(const TArray<TWeakObjectPtr<AActor>>& Actors, int32 MaxPaths)
	{
		TArray<FString> Paths;
		for (const TWeakObjectPtr<AActor>& Weak : Actors)
		{
			if (Paths.Num() >= MaxPaths)
			{
				break;
			}
			if (const AActor* Actor = Weak.Get())
			{
				Paths.Add(Actor->GetPathName());
			}
		}
		return FString::Join(Paths, TEXT(" | "));
	}
}
