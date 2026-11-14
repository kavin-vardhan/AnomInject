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

int main()
{
	RunUsableLodChecks();

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
