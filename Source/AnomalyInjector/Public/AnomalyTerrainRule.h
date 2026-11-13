#pragma once

namespace AnomalyTerrainRule
{
	inline const char* LandscapeActorClass()
	{
		return "LandscapeProxy";
	}

	inline const char* LandscapeComponentClass()
	{
		return "LandscapeComponent";
	}

	inline const char* TerrainBlinkSubtype()
	{
		return "terrain_disappear_reappear";
	}

	inline const char* DefaultBlinkSubtype()
	{
		return "disappear_reappear";
	}

	inline const char* UnlabelableReason()
	{
		return "terrain_unlabelable";
	}

	enum class ERule : unsigned char
	{
		None = 0,
		Class = 1,
		Pattern = 2
	};

	enum class EField : unsigned char
	{
		None = 0,
		Actor = 1,
		ActorClass = 2,
		Component = 3,
		ComponentClass = 4,
		Asset = 5
	};

	inline const char* DescribeField(EField F)
	{
		switch (F)
		{
		case EField::Actor: return "actor";
		case EField::ActorClass: return "actor_class";
		case EField::Component: return "component";
		case EField::ComponentClass: return "component_class";
		case EField::Asset: return "asset";
		default: return "none";
		}
	}

	template <typename TChar>
	struct TText
	{
		const TChar* Ptr = nullptr;
		int Len = 0;
	};

	template <typename TChar>
	inline bool IsEmpty(const TText<TChar>& S)
	{
		return S.Ptr == nullptr || S.Len <= 0;
	}

	inline int FoldAscii(int C)
	{
		return (C >= 'A' && C <= 'Z') ? C + ('a' - 'A') : C;
	}

	template <typename TChar>
	inline bool ContainsIgnoreCase(const TText<TChar>& Hay, const TText<TChar>& Needle)
	{
		if (IsEmpty(Needle) || IsEmpty(Hay) || Needle.Len > Hay.Len)
		{
			return false;
		}
		for (int i = 0; i + Needle.Len <= Hay.Len; ++i)
		{
			int j = 0;
			while (j < Needle.Len && FoldAscii((int)Hay.Ptr[i + j]) == FoldAscii((int)Needle.Ptr[j]))
			{
				++j;
			}
			if (j == Needle.Len)
			{
				return true;
			}
		}
		return false;
	}

	template <typename TChar>
	inline bool EqualsAscii(const TText<TChar>& S, const char* A)
	{
		if (!A)
		{
			return false;
		}
		int i = 0;
		for (; A[i] != '\0'; ++i)
		{
			if (i >= S.Len || S.Ptr == nullptr || (int)S.Ptr[i] != (int)(unsigned char)A[i])
			{
				return false;
			}
		}
		return i == S.Len;
	}

	template <typename TChar>
	inline bool ChainHas(const TText<TChar>* Chain, int Num, const char* ClassName)
	{
		for (int i = 0; Chain && i < Num; ++i)
		{
			if (EqualsAscii(Chain[i], ClassName))
			{
				return true;
			}
		}
		return false;
	}

	template <typename TChar>
	struct TPart
	{
		TText<TChar> Name;
		TText<TChar> Asset;
		const TText<TChar>* Chain = nullptr;
		int ChainNum = 0;
	};

	template <typename TChar>
	struct TObject
	{
		TText<TChar> Name;
		const TText<TChar>* Chain = nullptr;
		int ChainNum = 0;
		const TPart<TChar>* Parts = nullptr;
		int NumParts = 0;
	};

	struct FVerdict
	{
		ERule Rule = ERule::None;
		EField Field = EField::None;
		int Part = -1;
		int Pattern = -1;
	};

	inline bool IsTerrain(const FVerdict& V)
	{
		return V.Rule != ERule::None;
	}

	inline bool IsEngineLandscape(const FVerdict& V)
	{
		return V.Rule == ERule::Class;
	}

	template <typename TChar>
	inline FVerdict Classify(const TObject<TChar>& O, bool bClassRuleActive, const TText<TChar>* Patterns, int NumPatterns)
	{
		FVerdict V;
		if (bClassRuleActive)
		{
			if (ChainHas(O.Chain, O.ChainNum, LandscapeActorClass()))
			{
				V.Rule = ERule::Class;
				V.Field = EField::ActorClass;
				return V;
			}
			for (int p = 0; O.Parts && p < O.NumParts; ++p)
			{
				if (ChainHas(O.Parts[p].Chain, O.Parts[p].ChainNum, LandscapeComponentClass()))
				{
					V.Rule = ERule::Class;
					V.Field = EField::ComponentClass;
					V.Part = p;
					return V;
				}
			}
		}
		for (int k = 0; Patterns && k < NumPatterns; ++k)
		{
			const TText<TChar>& Pat = Patterns[k];
			if (IsEmpty(Pat))
			{
				continue;
			}
			V.Pattern = k;
			V.Rule = ERule::Pattern;
			V.Part = -1;
			if (ContainsIgnoreCase(O.Name, Pat))
			{
				V.Field = EField::Actor;
				return V;
			}
			if (O.Chain && O.ChainNum > 0 && ContainsIgnoreCase(O.Chain[0], Pat))
			{
				V.Field = EField::ActorClass;
				return V;
			}
			for (int p = 0; O.Parts && p < O.NumParts; ++p)
			{
				const TPart<TChar>& P = O.Parts[p];
				V.Part = p;
				if (ContainsIgnoreCase(P.Name, Pat))
				{
					V.Field = EField::Component;
					return V;
				}
				if (P.Chain && P.ChainNum > 0 && ContainsIgnoreCase(P.Chain[0], Pat))
				{
					V.Field = EField::ComponentClass;
					return V;
				}
				if (ContainsIgnoreCase(P.Asset, Pat))
				{
					V.Field = EField::Asset;
					return V;
				}
			}
		}
		return FVerdict();
	}

	inline bool RefusesBlink(const FVerdict& V, bool bRefusalEnabled)
	{
		return bRefusalEnabled && IsEngineLandscape(V);
	}

	inline const char* BlinkSubtypeFor(bool bTerrain)
	{
		return bTerrain ? TerrainBlinkSubtype() : DefaultBlinkSubtype();
	}
}
