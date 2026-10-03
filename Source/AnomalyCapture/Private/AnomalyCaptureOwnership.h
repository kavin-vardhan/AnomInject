#pragma once

namespace AnomalyCaptureOwnership
{
	inline bool IsOwner(const void* OwnerScene, const void* OwnerTarget, const void* Scene, const void* Target)
	{
		return OwnerScene && OwnerTarget && Scene == OwnerScene && Target == OwnerTarget;
	}

	inline bool IsSampleFrame(unsigned long long SampleFrame, unsigned long long FamilyFrame)
	{
		return SampleFrame == FamilyFrame;
	}

	inline bool HasExpired(unsigned long long SampleFrame, unsigned long long BeforeFrame)
	{
		return SampleFrame < BeforeFrame;
	}

	inline bool MatchesFamilyKey(unsigned int Frame, unsigned long long Id, unsigned int OtherFrame, unsigned long long OtherId)
	{
		return Frame == OtherFrame && Id == OtherId;
	}
}
