#pragma once

namespace AnomalyMaskServe
{
	inline bool ArmBelongsToFamily(unsigned long long ArmSeq, bool bFamilyHasBound, unsigned long long FamilyBound)
	{
		return !bFamilyHasBound || ArmSeq <= FamilyBound;
	}

	inline unsigned long long BoundForFamily(unsigned long long NextArmSeq)
	{
		return NextArmSeq > 0 ? NextArmSeq - 1 : 0;
	}
}
