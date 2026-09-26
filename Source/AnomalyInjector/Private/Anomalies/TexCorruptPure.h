#pragma once

namespace TexCorruptPure
{
	namespace Rank
	{
		constexpr int S1 = 1;
		constexpr int S2 = 2;
		constexpr int S3 = 3;
		constexpr int S4 = 4;
		constexpr int S5 = 5;
		constexpr int S6 = 6;
		constexpr int S7 = 7;
		constexpr int S8 = 8;
		constexpr int TBase = 10;
		constexpr int A2 = 31;
		constexpr int A3 = 32;
		constexpr int A5 = 35;
		constexpr int A6 = 36;

		inline int ForBindingStep(int TStep)
		{
			return TBase + TStep;
		}
	}

	enum class EFmt : int
	{
		DXT1,
		DXT5,
		BC7,
		BGRA8,
		BC4,
		G8,
		BC5,
		Other
	};

	enum class EClassP : int
	{
		None,
		Colour,
		Data,
		Normal
	};

	enum class EClassWhy : int
	{
		Ok,
		NormalMapNotBc5,
		Bc5NotNormalMap,
		SrgbSingleChannel,
		FormatNotAdmitted
	};

	inline int MaxInt(int A, int B)
	{
		return A > B ? A : B;
	}

	inline int FullChainLength(int W, int H)
	{
		int Longest = MaxInt(W, H);
		int Levels = 1;
		while (Longest > 1)
		{
			Longest >>= 1;
			++Levels;
		}
		return Levels;
	}

	inline long long ChainBytes(int W, int H, int M)
	{
		long long Sum = 0;
		const int Levels = M < 1 ? 1 : M;
		for (int m = 0; m < Levels; ++m)
		{
			Sum += 4LL * (long long)MaxInt(1, W >> m) * (long long)MaxInt(1, H >> m);
		}
		return Sum;
	}

	inline long long ScratchBytes(int W, int H, int M)
	{
		if (M <= 1)
		{
			return 0;
		}
		return ChainBytes(W, H, M) - 4LL * (long long)W * (long long)H;
	}

	inline bool IsAdmittedChainShape(int M, int W, int H, const int* LevelW, const int* LevelH)
	{
		if (M < 1 || W <= 0 || H <= 0)
		{
			return false;
		}
		if (M == 1)
		{
			return true;
		}
		if (M != FullChainLength(W, H))
		{
			return false;
		}
		for (int m = 0; m < M; ++m)
		{
			if (LevelW[m] != MaxInt(1, W >> m) || LevelH[m] != MaxInt(1, H >> m))
			{
				return false;
			}
		}
		return true;
	}

	inline EClassP ClassifyFormat(EFmt Fmt, bool bSRGB, bool bNormalMap, EClassWhy& OutWhy)
	{
		OutWhy = EClassWhy::Ok;
		if (bNormalMap)
		{
			if (Fmt == EFmt::BC5)
			{
				return EClassP::Normal;
			}
			OutWhy = EClassWhy::NormalMapNotBc5;
			return EClassP::None;
		}
		switch (Fmt)
		{
		case EFmt::BC5:
			OutWhy = EClassWhy::Bc5NotNormalMap;
			return EClassP::None;
		case EFmt::DXT1:
		case EFmt::DXT5:
		case EFmt::BC7:
		case EFmt::BGRA8:
			return bSRGB ? EClassP::Colour : EClassP::Data;
		case EFmt::BC4:
		case EFmt::G8:
			if (bSRGB)
			{
				OutWhy = EClassWhy::SrgbSingleChannel;
				return EClassP::None;
			}
			return EClassP::Data;
		default:
			OutWhy = EClassWhy::FormatNotAdmitted;
			return EClassP::None;
		}
	}

	struct FReqTex
	{
		long long Id = 0;
		int W = 0;
		int H = 0;
		int M = 0;
		bool bSRGB = false;
	};

	struct FRequirement
	{
		long long Chains = 0;
		long long Scratch = 0;
		int DistinctTextures = 0;
		int ScratchClasses = 0;

		long long Total() const { return Chains + Scratch; }
	};

	inline FRequirement EventRequirement(const FReqTex* Tex, int N)
	{
		FRequirement R;
		for (int i = 0; i < N; ++i)
		{
			bool bSeen = false;
			for (int j = 0; j < i && !bSeen; ++j)
			{
				bSeen = Tex[j].Id == Tex[i].Id;
			}
			if (bSeen)
			{
				continue;
			}
			++R.DistinctTextures;
			R.Chains += ChainBytes(Tex[i].W, Tex[i].H, Tex[i].M);
			if (Tex[i].M <= 1)
			{
				continue;
			}
			bool bClassSeen = false;
			for (int j = 0; j < i && !bClassSeen; ++j)
			{
				bClassSeen = Tex[j].M > 1 && Tex[j].W == Tex[i].W && Tex[j].H == Tex[i].H && Tex[j].bSRGB == Tex[i].bSRGB;
			}
			if (!bClassSeen)
			{
				++R.ScratchClasses;
				R.Scratch += ScratchBytes(Tex[i].W, Tex[i].H, Tex[i].M);
			}
		}
		return R;
	}

	inline bool Fits(long long Required, long long Cap, long long Live, long long Pending)
	{
		return Required <= Cap - Live - Pending;
	}

	inline int PickFirstFailing(const int* Steps, const bool* Required, int N)
	{
		int Best = -1;
		for (int i = 0; i < N; ++i)
		{
			if (!Required[i] || Steps[i] == 0)
			{
				continue;
			}
			if (Best < 0 || Steps[i] < Steps[Best])
			{
				Best = i;
			}
		}
		return Best;
	}

	inline int PickEarliestSlot(const int* Ranks, const bool* Untouched, const bool* Qualified, int N)
	{
		int Best = -1;
		for (int i = 0; i < N; ++i)
		{
			if (Qualified[i] || Untouched[i])
			{
				continue;
			}
			if (Best < 0 || Ranks[i] < Ranks[Best])
			{
				Best = i;
			}
		}
		return Best;
	}

	inline bool IsNonSpatial(int M, int W, int H)
	{
		return M == 1 && W == 1 && H == 1;
	}

	inline int SourceMipFor(int Level, int M, int TileN, bool bMipShift)
	{
		int Mip = Level;
		if (TileN > 1)
		{
			int Shift = 0;
			int N = TileN;
			while (N > 1)
			{
				N >>= 1;
				++Shift;
			}
			Mip += Shift;
		}
		if (bMipShift)
		{
			Mip += 1;
		}
		const int Last = M > 1 ? M - 1 : 0;
		return Mip < 0 ? 0 : (Mip > Last ? Last : Mip);
	}

	inline bool StreamingBudgetPossible(bool bSupportsStreaming, int UsePerTextureBias, float MipBias)
	{
		return bSupportsStreaming && UsePerTextureBias != 0 && MipBias > 0.0f;
	}

	inline bool GlobalStreamingBias(int UsePerTextureBias, float MipBias)
	{
		return MipBias > 0.0f && UsePerTextureBias == 0;
	}
}
