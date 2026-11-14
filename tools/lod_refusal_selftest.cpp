#ifndef LOD_POLICY_HEADER
#define LOD_POLICY_HEADER "../Source/AnomalyInjector/Public/AnomalyLodPolicy.h"
#endif
#include LOD_POLICY_HEADER

#include <cstdio>
#include <cstring>
#include <string>

using namespace AnomalyLodPolicy;

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

static void CheckDecide(bool bHism, bool bSkeletal, bool bAllowSkeletal, int NumLods, EForcedLodRefusal Want)
{
	const EForcedLodRefusal Got = DecideForcedLod(bHism, bSkeletal, bAllowSkeletal, NumLods);
	Check(Got == Want, std::string("decide hism=") + (bHism ? "1" : "0") + " skel=" + (bSkeletal ? "1" : "0")
		+ " allow=" + (bAllowSkeletal ? "1" : "0") + " lods=" + std::to_string(NumLods)
		+ " want=" + std::to_string((int)Want) + " got=" + std::to_string((int)Got));
}

static void CheckDominant(int Hism, int Single, int Skeletal, int Other, const char* Want)
{
	const char* Got = DominantRefusal(Hism, Single, Skeletal, Other);
	Check(std::strcmp(Got, Want) == 0, "dominant " + std::to_string(Hism) + "/" + std::to_string(Single) + "/"
		+ std::to_string(Skeletal) + "/" + std::to_string(Other) + " want='" + Want + "' got='" + Got + "'");
}

static void CheckUsable(int NumLods, int MinLod, unsigned int WithVertices, int WantFirst, int WantCount)
{
	const int GotFirst = FirstUsableLod(NumLods, MinLod, WithVertices);
	const int GotCount = UsableLodCount(NumLods, MinLod, WithVertices);
	Check(GotFirst == WantFirst && GotCount == WantCount, "usable lods=" + std::to_string(NumLods) + " min=" + std::to_string(MinLod)
		+ " mask=" + std::to_string(WithVertices) + " want=" + std::to_string(WantFirst) + "/" + std::to_string(WantCount)
		+ " got=" + std::to_string(GotFirst) + "/" + std::to_string(GotCount));
}

static void CheckUsableRefusal(bool bSkeletal, bool bAllow, int NumLods, int MinLod, unsigned int WithVertices, EForcedLodRefusal Want)
{
	const EForcedLodRefusal Got = DecideForcedLod(false, bSkeletal, bAllow, UsableLodCount(NumLods, MinLod, WithVertices));
	Check(Got == Want, "usable-refusal skel=" + std::to_string(bSkeletal ? 1 : 0) + " allow=" + std::to_string(bAllow ? 1 : 0)
		+ " lods=" + std::to_string(NumLods) + " min=" + std::to_string(MinLod) + " mask=" + std::to_string(WithVertices)
		+ " want=" + std::to_string((int)Want) + " got=" + std::to_string((int)Got));
}

static void RunUsableLodChecks()
{
	CheckUsable(0, 0, 0u, 0, 0);
	CheckUsable(1, 0, 1u, 0, 1);
	CheckUsable(2, 0, 3u, 0, 2);
	CheckUsable(2, 1, 3u, 1, 1);
	CheckUsable(2, 0, 2u, 1, 1);
	CheckUsable(3, 1, 7u, 1, 2);
	CheckUsable(3, 2, 7u, 2, 1);
	CheckUsable(3, 0, 4u, 2, 1);
	CheckUsable(3, 0, 6u, 1, 2);
	CheckUsable(4, 5, 15u, 3, 1);
	CheckUsable(4, -1, 15u, 0, 4);
	CheckUsable(3, 0, 0u, 3, 0);
	CheckUsable(4, 1, 13u, 2, 2);
	CheckUsable(8, 0, 255u, 0, 8);
	CheckUsable(8, 6, 255u, 6, 2);

	CheckUsableRefusal(false, false, 2, 0, 3u, EForcedLodRefusal::None);
	CheckUsableRefusal(false, false, 2, 1, 3u, EForcedLodRefusal::SingleLod);
	CheckUsableRefusal(false, false, 2, 0, 2u, EForcedLodRefusal::SingleLod);
	CheckUsableRefusal(false, false, 3, 1, 7u, EForcedLodRefusal::None);
	CheckUsableRefusal(false, false, 3, 2, 7u, EForcedLodRefusal::SingleLod);
	CheckUsableRefusal(false, false, 3, 0, 4u, EForcedLodRefusal::SingleLod);
	CheckUsableRefusal(false, false, 3, 0, 0u, EForcedLodRefusal::SingleLod);
	CheckUsableRefusal(true, true, 2, 1, 3u, EForcedLodRefusal::SingleLod);
	CheckUsableRefusal(true, true, 3, 0, 6u, EForcedLodRefusal::None);
	CheckUsableRefusal(true, false, 3, 0, 7u, EForcedLodRefusal::SkeletalUnmeasured);
}

static void CheckForcedLod(int Model, int FirstUsable, int NumLods, int FirstResident, int Want)
{
	const int Got = EffectiveForcedLod(Model, FirstUsable, NumLods, FirstResident);
	Check(Got == Want, "forced model=" + std::to_string(Model) + " first=" + std::to_string(FirstUsable) + " lods="
		+ std::to_string(NumLods) + " resident=" + std::to_string(FirstResident) + " want=" + std::to_string(Want) + " got=" + std::to_string(Got));
}

static void CheckShownLod(int Predicted, int FirstUsable, int NumLods, int FirstResident, int Want)
{
	const int Got = EffectiveShownLod(Predicted, FirstUsable, NumLods, FirstResident);
	Check(Got == Want, "shown predicted=" + std::to_string(Predicted) + " first=" + std::to_string(FirstUsable) + " lods="
		+ std::to_string(NumLods) + " resident=" + std::to_string(FirstResident) + " want=" + std::to_string(Want) + " got=" + std::to_string(Got));
}

static void CheckScenario(const char* Name, bool bKnown, int Predicted, int Model, int NumLods, int MinLod, unsigned int WithVertices,
	int FirstResident, EForcedLodRefusal Want)
{
	const int First = FirstUsableLod(NumLods, MinLod, WithVertices);
	const int Resident = FirstResident < First ? First : FirstResident;
	const EForcedLodRefusal Got = DecideContrast(bKnown, EffectiveShownLod(Predicted, First, NumLods, Resident),
		EffectiveForcedLod(Model, First, NumLods, Resident));
	Check(Got == Want, std::string("scenario ") + Name + " want=" + std::to_string((int)Want) + " got=" + std::to_string((int)Got));
}

static void CheckDominantOf(int Hism, int Single, int Skeletal, int NotCoarser, int Undetermined, int Other, const char* Want)
{
	const int Counts[NumRefusalKinds] = { 0, Hism, Single, Skeletal, NotCoarser, Undetermined };
	const char* Got = DominantRefusalOf(Counts, NumRefusalKinds, Other);
	Check(std::strcmp(Got, Want) == 0, "dominant-of " + std::to_string(Hism) + "/" + std::to_string(Single) + "/" + std::to_string(Skeletal)
		+ "/" + std::to_string(NotCoarser) + "/" + std::to_string(Undetermined) + "/" + std::to_string(Other) + " want='" + Want + "' got='" + Got + "'");
}

static void RunContrastChecks()
{
	CheckForcedLod(1, 0, 2, 0, 0);
	CheckForcedLod(2, 0, 2, 0, 1);
	CheckForcedLod(1, 1, 2, 1, 1);
	CheckForcedLod(5, 0, 3, 0, 2);
	CheckForcedLod(1, 0, 3, 2, 2);
	CheckForcedLod(2, 1, 4, 1, 1);
	CheckForcedLod(1, 2, 4, 2, 2);
	CheckForcedLod(3, 2, 4, 0, 2);
	CheckForcedLod(4, 0, 4, 0, 3);
	CheckForcedLod(1, 1, 3, 0, 1);
	CheckForcedLod(2, 2, 4, 0, 2);

	CheckShownLod(0, 0, 2, 0, 0);
	CheckShownLod(0, 1, 3, 1, 1);
	CheckShownLod(5, 0, 3, 0, 2);
	CheckShownLod(0, 0, 2, 1, 1);
	CheckShownLod(-1, 0, 2, 0, 0);
	CheckShownLod(1, 0, 3, 2, 2);
	CheckShownLod(2, 1, 4, 1, 2);

	Check(DecideContrast(false, 0, 1) == EForcedLodRefusal::CurrentLodUndetermined, "contrast unknown refuses");
	Check(DecideContrast(false, -1, -1) == EForcedLodRefusal::CurrentLodUndetermined, "contrast unknown sentinel refuses");
	Check(DecideContrast(true, 0, 1) == EForcedLodRefusal::None, "contrast coarser admits");
	Check(DecideContrast(true, 0, 3) == EForcedLodRefusal::None, "contrast much coarser admits");
	Check(DecideContrast(true, 0, 0) == EForcedLodRefusal::NotCoarser, "contrast equal refuses");
	Check(DecideContrast(true, 1, 1) == EForcedLodRefusal::NotCoarser, "contrast equal 1 refuses");
	Check(DecideContrast(true, 2, 1) == EForcedLodRefusal::NotCoarser, "contrast finer refuses");

	CheckScenario("explicit-index-1-at-lod0", true, 0, 1, 2, 0, 3u, 0, EForcedLodRefusal::NotCoarser);
	CheckScenario("default-worst-at-lod0", true, 0, 2, 2, 0, 3u, 0, EForcedLodRefusal::None);
	CheckScenario("default-worst-already-worst", true, 1, 2, 2, 0, 3u, 0, EForcedLodRefusal::NotCoarser);
	CheckScenario("explicit-index-2-of-4-at-lod0", true, 0, 2, 4, 0, 15u, 0, EForcedLodRefusal::None);
	CheckScenario("explicit-index-2-of-4-at-lod2", true, 2, 2, 4, 0, 15u, 0, EForcedLodRefusal::NotCoarser);
	CheckScenario("minlod1-explicit-lod1-predicted0", true, 0, 2, 3, 1, 7u, 0, EForcedLodRefusal::NotCoarser);
	CheckScenario("minlod1-worst-predicted0", true, 0, 3, 3, 1, 7u, 0, EForcedLodRefusal::None);
	CheckScenario("lod0-streamed-out-worst-of-2", true, 0, 2, 2, 0, 3u, 1, EForcedLodRefusal::NotCoarser);
	CheckScenario("lod0-streamed-out-worst-of-3", true, 0, 3, 3, 0, 7u, 1, EForcedLodRefusal::None);
	CheckScenario("stripped-lod0-worst-of-3-predicted0", true, 0, 3, 3, 0, 6u, 0, EForcedLodRefusal::None);
	CheckScenario("current-unknown", false, 0, 2, 2, 0, 3u, 0, EForcedLodRefusal::CurrentLodUndetermined);
	CheckScenario("game-forced-worst-already", true, 1, 2, 2, 0, 3u, 0, EForcedLodRefusal::NotCoarser);

	Check(std::strcmp(DescribeRefusal(EForcedLodRefusal::NotCoarser), "not_coarser_than_current") == 0, "name not coarser");
	Check(std::strcmp(DescribeRefusal(EForcedLodRefusal::CurrentLodUndetermined), "current_lod_undetermined") == 0, "name undetermined");

	CheckDominantOf(0, 0, 0, 2, 0, 0, "not_coarser_than_current");
	CheckDominantOf(0, 0, 0, 0, 1, 0, "current_lod_undetermined");
	CheckDominantOf(0, 1, 0, 1, 0, 0, "single_lod");
	CheckDominantOf(0, 0, 0, 1, 2, 0, "current_lod_undetermined");
	CheckDominantOf(1, 0, 0, 3, 0, 0, "not_coarser_than_current");
	CheckDominantOf(0, 0, 0, 2, 0, 2, "");
	CheckDominantOf(0, 0, 0, 0, 0, 0, "");
}

static void RunScopeChecks()
{
	Check(!ShouldScopeToForced(0, 0), "scope none forced none other");
	Check(!ShouldScopeToForced(0, 2), "scope none forced");
	Check(!ShouldScopeToForced(1, 0), "scope whole actor forced keeps actor box");
	Check(!ShouldScopeToForced(3, 0), "scope three of three forced keeps actor box");
	Check(ShouldScopeToForced(1, 1), "scope one of two forced");
	Check(ShouldScopeToForced(2, 1), "scope two of three forced");
	Check(ShouldScopeToForced(1, 3), "scope one of four forced");
}

int main()
{
	RunUsableLodChecks();
	RunContrastChecks();
	RunScopeChecks();

	for (int Lods = 0; Lods <= 8; ++Lods)
	{
		CheckDecide(true, false, false, Lods, EForcedLodRefusal::HierarchicalInstanced);
		CheckDecide(true, false, true, Lods, EForcedLodRefusal::HierarchicalInstanced);
	}
	for (int Allow = 0; Allow <= 1; ++Allow)
	{
		CheckDecide(false, false, Allow != 0, 0, EForcedLodRefusal::SingleLod);
		CheckDecide(false, false, Allow != 0, 1, EForcedLodRefusal::SingleLod);
		CheckDecide(false, false, Allow != 0, 2, EForcedLodRefusal::None);
		CheckDecide(false, false, Allow != 0, 3, EForcedLodRefusal::None);
		CheckDecide(false, false, Allow != 0, 8, EForcedLodRefusal::None);
	}

	for (int Lods = 0; Lods <= 8; ++Lods)
	{
		CheckDecide(false, true, false, Lods, EForcedLodRefusal::SkeletalUnmeasured);
	}
	CheckDecide(false, true, true, 0, EForcedLodRefusal::SingleLod);
	CheckDecide(false, true, true, 1, EForcedLodRefusal::SingleLod);
	CheckDecide(false, true, true, 2, EForcedLodRefusal::None);
	CheckDecide(false, true, true, 4, EForcedLodRefusal::None);

	Check(std::strcmp(DescribeRefusal(EForcedLodRefusal::HierarchicalInstanced), "hism_ignores_forced_lod") == 0, "name hism");
	Check(std::strcmp(DescribeRefusal(EForcedLodRefusal::SingleLod), "single_lod") == 0, "name single");
	Check(std::strcmp(DescribeRefusal(EForcedLodRefusal::SkeletalUnmeasured), "skeletal_unmeasured") == 0, "name skeletal");
	Check(std::strcmp(DescribeRefusal(EForcedLodRefusal::None), "") == 0, "name none");

	CheckDominant(0, 0, 0, 0, "");
	CheckDominant(1, 0, 0, 0, "hism_ignores_forced_lod");
	CheckDominant(0, 1, 0, 0, "single_lod");
	CheckDominant(0, 0, 0, 3, "");
	CheckDominant(2, 2, 0, 0, "hism_ignores_forced_lod");
	CheckDominant(1, 3, 0, 0, "single_lod");
	CheckDominant(1, 0, 0, 2, "");
	CheckDominant(0, 2, 0, 1, "single_lod");
	CheckDominant(0, 1, 0, 1, "");
	CheckDominant(3, 1, 0, 2, "hism_ignores_forced_lod");
	CheckDominant(1, 0, 0, 1, "");
	CheckDominant(2, 1, 0, 2, "");
	CheckDominant(0, 0, 1, 0, "skeletal_unmeasured");
	CheckDominant(0, 1, 2, 0, "skeletal_unmeasured");
	CheckDominant(0, 2, 2, 0, "single_lod");
	CheckDominant(1, 0, 3, 0, "skeletal_unmeasured");
	CheckDominant(0, 0, 2, 2, "");
	CheckDominant(0, 0, 2, 1, "skeletal_unmeasured");

	std::printf("lod_refusal_selftest: %d checks, %d failures\n", GChecks, GFailures);
	return GFailures == 0 ? 0 : 1;
}
