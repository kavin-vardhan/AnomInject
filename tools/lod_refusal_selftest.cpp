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

int main()
{
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
