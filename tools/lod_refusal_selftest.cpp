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

static void CheckDecide(bool bHism, int NumLods, EForcedLodRefusal Want)
{
	const EForcedLodRefusal Got = DecideForcedLod(bHism, NumLods);
	Check(Got == Want, std::string("decide hism=") + (bHism ? "1" : "0") + " lods=" + std::to_string(NumLods)
		+ " want=" + std::to_string((int)Want) + " got=" + std::to_string((int)Got));
}

static void CheckDominant(int Hism, int Single, int Other, const char* Want)
{
	const char* Got = DominantRefusal(Hism, Single, Other);
	Check(std::strcmp(Got, Want) == 0, "dominant " + std::to_string(Hism) + "/" + std::to_string(Single) + "/"
		+ std::to_string(Other) + " want='" + Want + "' got='" + Got + "'");
}

int main()
{
	for (int Lods = 0; Lods <= 8; ++Lods)
	{
		CheckDecide(true, Lods, EForcedLodRefusal::HierarchicalInstanced);
	}
	CheckDecide(false, 0, EForcedLodRefusal::SingleLod);
	CheckDecide(false, 1, EForcedLodRefusal::SingleLod);
	CheckDecide(false, 2, EForcedLodRefusal::None);
	CheckDecide(false, 3, EForcedLodRefusal::None);
	CheckDecide(false, 8, EForcedLodRefusal::None);

	Check(std::strcmp(DescribeRefusal(EForcedLodRefusal::HierarchicalInstanced), "hism_ignores_forced_lod") == 0, "name hism");
	Check(std::strcmp(DescribeRefusal(EForcedLodRefusal::SingleLod), "single_lod") == 0, "name single");
	Check(std::strcmp(DescribeRefusal(EForcedLodRefusal::None), "") == 0, "name none");

	CheckDominant(0, 0, 0, "");
	CheckDominant(1, 0, 0, "hism_ignores_forced_lod");
	CheckDominant(0, 1, 0, "single_lod");
	CheckDominant(0, 0, 3, "");
	CheckDominant(2, 2, 0, "hism_ignores_forced_lod");
	CheckDominant(1, 3, 0, "single_lod");
	CheckDominant(1, 0, 2, "");
	CheckDominant(0, 2, 1, "single_lod");
	CheckDominant(0, 1, 1, "");
	CheckDominant(3, 1, 2, "hism_ignores_forced_lod");
	CheckDominant(1, 0, 1, "");
	CheckDominant(2, 1, 2, "");

	std::printf("lod_refusal_selftest: %d checks, %d failures\n", GChecks, GFailures);
	return GFailures == 0 ? 0 : 1;
}
