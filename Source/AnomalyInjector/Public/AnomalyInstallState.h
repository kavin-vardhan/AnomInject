#pragma once

namespace AnomalyInstall
{
	enum class EState : unsigned char
	{
		None = 0,
		Full = 1,
		Partial = 2
	};

	struct FSlotView
	{
		bool bComponentValid = false;
		bool bRegistered = false;
		bool bRenders = false;
		int SlotIndex = -1;
		int NumMaterials = 0;
		bool bResolvedIsOurs = false;
	};

	inline bool SlotRendersOurs(const FSlotView& S)
	{
		return S.bComponentValid && S.bRegistered && S.bRenders
			&& S.SlotIndex >= 0 && S.SlotIndex < S.NumMaterials
			&& S.bResolvedIsOurs;
	}

	inline EState Classify(int NumTargeted, int NumOurs)
	{
		if (NumTargeted <= 0 || NumOurs <= 0)
		{
			return EState::None;
		}
		return NumOurs >= NumTargeted ? EState::Full : EState::Partial;
	}

	inline EState ClassifySlots(const FSlotView* Slots, int Num)
	{
		int Ours = 0;
		for (int i = 0; Slots && i < Num; ++i)
		{
			if (SlotRendersOurs(Slots[i]))
			{
				++Ours;
			}
		}
		return Classify(Num, Ours);
	}

	inline bool IsInstalled(EState S)
	{
		return S != EState::None;
	}

	inline bool IsInstalledByte(unsigned char B)
	{
		return B != 0;
	}

	inline bool IsPartialByte(unsigned char B)
	{
		return B == (unsigned char)EState::Partial;
	}

	inline const char* DescribeState(EState S)
	{
		switch (S)
		{
		case EState::Full:    return "full";
		case EState::Partial: return "partial";
		default:              return "none";
		}
	}

	enum class ERevertSlot : unsigned char
	{
		Leave = 0,
		Restore = 1,
		Clear = 2
	};

	inline ERevertSlot DecideRevertSlot(bool bSlotHoldsOurs, bool bWasExplicitOverride, bool bOriginalAlive)
	{
		if (!bSlotHoldsOurs)
		{
			return ERevertSlot::Leave;
		}
		return (bWasExplicitOverride && bOriginalAlive) ? ERevertSlot::Restore : ERevertSlot::Clear;
	}

	inline int SweepExtent(int NumMaterials, int NumOverrides)
	{
		const int A = NumMaterials < 0 ? 0 : NumMaterials;
		const int B = NumOverrides < 0 ? 0 : NumOverrides;
		return A > B ? A : B;
	}

	inline int CountOwnedOverrides(const bool* OverrideIsOurs, int NumOverrides)
	{
		int N = 0;
		for (int i = 0; OverrideIsOurs && i < NumOverrides; ++i)
		{
			if (OverrideIsOurs[i])
			{
				++N;
			}
		}
		return N;
	}
}
