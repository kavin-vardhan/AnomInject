#include "TexCorruptPure.h"

#include <cstdio>
#include <string>

using namespace TexCorruptPure;
using namespace TexCorruptPure::Uniform;

static int GFailures = 0;
static int GChecks = 0;

static void Check(bool bOk, const std::string& What)
{
	++GChecks;
	if (!bOk)
	{
		++GFailures;
		std::printf("FAIL %s\n", What.c_str());
	}
}

static unsigned int Q(float V)
{
	return (unsigned int)(V * 65535.0f + 0.5f);
}

static void Raw(unsigned int* R, const float* Mn, const float* Mx, unsigned int Texels)
{
	for (int C = 0; C < 4; ++C)
	{
		R[C] = ~Q(Mn[C]);
		R[4 + C] = Q(Mx[C]);
	}
	R[8] = Texels;
}

static FStats Stats(const float* Mn, const float* Mx, unsigned int Texels = 256)
{
	unsigned int R[9];
	Raw(R, Mn, Mx, Texels);
	FStats S;
	DecodeStats(R, Texels, S);
	return S;
}

int main()
{
	const float Tol = TolRaw;
	{
		const float Mn[4] = { 0.4f, 0.4f, 0.4f, 1.0f };
		const float Mx[4] = { 0.4f, 0.4f, 0.4f, 1.0f };
		const FStats S = Stats(Mn, Mx);
		Check(S.bValid, "a constant 16x16 colour decodes");
		Check(IsUniformUnder(ERule::AnyChannelRange, S, Tol), "constant colour: no spatial variation -> uniform for uv");
	}
	{
		const float Mn[4] = { 0.40f, 0.40f, 0.40f, 1.0f };
		const float Mx[4] = { 0.40f + 1.0f / 255.0f, 0.40f, 0.40f, 1.0f };
		Check(IsUniformUnder(ERule::AnyChannelRange, Stats(Mn, Mx), Tol), "one 8-bit step of noise is still uniform (tolerance 2 steps)");
	}
	{
		const float Mn[4] = { 0.40f, 0.40f, 0.40f, 1.0f };
		const float Mx[4] = { 0.40f + 3.0f / 255.0f, 0.40f, 0.40f, 1.0f };
		Check(!IsUniformUnder(ERule::AnyChannelRange, Stats(Mn, Mx), Tol), "three 8-bit steps in one channel vary -> admitted for uv");
	}
	{
		const float Mn[4] = { 0.4f, 0.4f, 0.4f, 0.0f };
		const float Mx[4] = { 0.4f, 0.4f, 0.4f, 1.0f };
		Check(!IsUniformUnder(ERule::AnyChannelRange, Stats(Mn, Mx), Tol), "varying alpha alone is spatial variation");
	}
	{
		const float Mn[4] = { 0.5f, 0.5f, 0.0f, 1.0f };
		const float Mx[4] = { 0.5f + 1.0f / 255.0f, 0.5f, 0.0f, 1.0f };
		const FStats S = Stats(Mn, Mx);
		Check(IsUniformUnder(ERule::NormalXY, S, Tol), "flat BC5 normal (0.5,0.5): invert cannot show");
		Check(IsUniformUnder(ERule::NormalY, S, Tol), "flat BC5 normal: green_flip cannot show");
	}
	{
		const float Mn[4] = { 0.7f, 0.7f, 0.0f, 1.0f };
		const float Mx[4] = { 0.7f, 0.7f, 0.0f, 1.0f };
		const FStats S = Stats(Mn, Mx);
		Check(IsUniformUnder(ERule::AnyChannelRange, S, Tol), "a constant TILTED normal has no spatial variation (uv cannot show)");
		Check(!IsUniformUnder(ERule::NormalXY, S, Tol), "a constant tilted normal is not flat: invert changes it");
		Check(!IsUniformUnder(ERule::NormalY, S, Tol), "a constant tilted normal: green_flip changes it");
	}
	{
		const float Mn[4] = { 0.2f, 0.5f, 0.0f, 1.0f };
		const float Mx[4] = { 0.8f, 0.5f, 0.0f, 1.0f };
		const FStats S = Stats(Mn, Mx);
		Check(!IsUniformUnder(ERule::NormalXY, S, Tol), "x varies: invert shows");
		Check(IsUniformUnder(ERule::NormalY, S, Tol), "only x varies and y is flat: green_flip cannot show");
	}
	{
		unsigned int R[9];
		const float Mn[4] = { 0.4f, 0.4f, 0.4f, 1.0f };
		Raw(R, Mn, Mn, 255);
		FStats S;
		Check(!DecodeStats(R, 256, S) && !S.bValid, "a texel count short of the extent is not a measurement");
		Check(!IsUniformUnder(ERule::AnyChannelRange, S, Tol), "an invalid measurement is never called uniform");
	}
	{
		unsigned int R[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 256 };
		FStats S;
		Check(!DecodeStats(R, 256, S), "an untouched min slot (no texel written) is rejected");
	}
	Check(RuleFor(EModeFamilyP::UV, EModeP::Tile, false) == ERule::AnyChannelRange, "tile -> no spatial variation rule");
	Check(RuleFor(EModeFamilyP::UV, EModeP::Scramble, false) == ERule::AnyChannelRange, "scramble -> no spatial variation rule");
	Check(RuleFor(EModeFamilyP::Normal, EModeP::Invert, false) == ERule::NormalXY, "invert -> flat normal rule");
	Check(RuleFor(EModeFamilyP::Normal, EModeP::GreenFlip, false) == ERule::NormalY, "green_flip -> flat y rule");
	Check(RuleFor(EModeFamilyP::UV, EModeP::None, false) == ERule::AnyChannelRange, "uv with no mode (census) -> family rule");
	Check(RuleFor(EModeFamilyP::Normal, EModeP::None, false) == ERule::NormalXY, "normal with no mode (census) -> family rule");
	Check(RuleFor(EModeFamilyP::UV, EModeP::Tile, true) == ERule::None, "bench identity modes are not checked");
	{
		const EVerdict A[] = { EVerdict::Uniform, EVerdict::Uniform };
		Check(JudgeSlot(A, 2) == EVerdict::Uniform, "every required texture uniform -> slot refused texture_uniform");
		const EVerdict B[] = { EVerdict::Uniform, EVerdict::Varying };
		Check(JudgeSlot(B, 2) == EVerdict::Varying, "one varying required texture -> slot admitted");
		const EVerdict C[] = { EVerdict::Uniform, EVerdict::Pending };
		Check(JudgeSlot(C, 2) == EVerdict::Pending, "uniform + pending -> pending (never admitted unmeasured)");
		const EVerdict D[] = { EVerdict::Pending, EVerdict::Varying };
		Check(JudgeSlot(D, 2) == EVerdict::Varying, "a measured varying texture admits even with another pending");
		const EVerdict E[] = { EVerdict::Uniform, EVerdict::Unmeasurable };
		Check(JudgeSlot(E, 2) == EVerdict::Unmeasurable, "uniform + unmeasurable -> refused unmeasurable, never assumed to vary");
		Check(JudgeSlot(A, 0) == EVerdict::NotChecked, "no measurable required texture -> not checked");
		const EVerdict F[] = { EVerdict::NotChecked };
		Check(JudgeSlot(F, 1) == EVerdict::NotChecked, "not checked stays not checked");
	}
	Check(Rank::A7 > Rank::A6 && Rank::A7 < Rank::TBase + 100, "texture_uniform ranks after map_set_over_cap");

	std::printf("texcorrupt uniform selftest: %d checks, %d failures\n", GChecks, GFailures);
	return GFailures == 0 ? 0 : 1;
}
