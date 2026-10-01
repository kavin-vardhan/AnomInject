#include "../Source/AnomalyInjector/Private/Anomalies/TexCorruptPure.h"

#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>

#ifndef READY_MUTANT
#define READY_MUTANT 0
#endif

#ifndef SOURCE_TREE
#define SOURCE_TREE "../Source/AnomalyInjector/Private/Anomalies/TexCorruptTree.cpp"
#endif

using namespace TexCorruptPure;

static int GChecks = 0;
static int GFailures = 0;

static void Check(bool bOk, const char* What)
{
	++GChecks;
	if (!bOk)
	{
		++GFailures;
		std::printf("FAIL %s\n", What);
	}
}

struct FFacts
{
	bool bWholeMapComplete;
	bool bCompileFinished;
	bool bVertexFactoryShaders;
	int AnyShaders;
	int BasePassVs;
	int BasePassPs;
};

static EDrawReadiness Judge(const FFacts& F)
{
	if constexpr (READY_MUTANT == 1)
	{
		return F.bWholeMapComplete ? EDrawReadiness::Ready : EDrawReadiness::CompilePending;
	}
	if constexpr (READY_MUTANT == 2)
	{
		if (F.bWholeMapComplete)
		{
			return EDrawReadiness::Ready;
		}
		return F.bCompileFinished && F.AnyShaders > 0 ? EDrawReadiness::Ready
			: (F.bCompileFinished ? EDrawReadiness::NoVertexFactoryShaders : EDrawReadiness::CompilePending);
	}
	if constexpr (READY_MUTANT == 3)
	{
		if (!F.bCompileFinished)
		{
			return EDrawReadiness::CompilePending;
		}
		return F.bVertexFactoryShaders && F.AnyShaders > 0 ? EDrawReadiness::Ready : EDrawReadiness::NoVertexFactoryShaders;
	}
	return JudgeDrawReadiness(F.bCompileFinished, F.bVertexFactoryShaders, F.BasePassVs, F.BasePassPs);
}

static void TestJudge()
{
	Check(Judge({ false, true, true, 12, 1, 2 }) == EDrawReadiness::Ready,
		"healthy partial map: whole map incomplete (editor, on-demand), compile finished, base-pass VS and PS for the VF -> admitted");
	Check(Judge({ true, true, false, 0, 0, 0 }) == EDrawReadiness::NoVertexFactoryShaders,
		"stale whole-map flag: cached complete=1 but the component's VF has no shaders (usage set after compile) -> draw_shaders_missing");
	Check(Judge({ false, true, true, 3, 0, 0 }) == EDrawReadiness::NoBasePassVertexShader,
		"VF map holds only non-base-pass shaders (e.g. depth only) -> draw_shaders_missing, not admitted on 'any shader'");
	Check(Judge({ false, true, true, 4, 1, 0 }) == EDrawReadiness::NoBasePassPixelShader,
		"base-pass VS without PS -> draw_shaders_missing");
	Check(Judge({ false, false, true, 12, 1, 2 }) == EDrawReadiness::CompilePending,
		"compile still running -> shader_map_incomplete (compile_pending), retried later");
	Check(Judge({ true, true, true, 20, 2, 4 }) == EDrawReadiness::Ready, "cooked game: complete map with base pass -> admitted");
	Check(ClassifyBasePassShaderTypeName("TBasePassVSFNoLightMapPolicy") == 1, "TBasePassVS* is a base-pass vertex shader");
	Check(ClassifyBasePassShaderTypeName("TBasePassPSFNoLightMapPolicySkylight") == 2, "TBasePassPS* is a base-pass pixel shader");
	Check(ClassifyBasePassShaderTypeName("TMobileBasePassPSFNoLightMapPolicyHDRLinear64") == 2, "mobile base-pass pixel shader");
	Check(ClassifyBasePassShaderTypeName("TDepthOnlyVS<false>") == 0, "depth-only VS is not a base-pass shader");
	Check(ClassifyBasePassShaderTypeName("TBasePassCSFNoLightMapPolicy") == 0, "base-pass compute shader is not a VS or PS");
	Check(ClassifyBasePassShaderTypeName(nullptr) == 0, "null name");
	Check(std::string(LexDrawReadiness(EDrawReadiness::NoVertexFactoryShaders)) == "no_vertex_factory_shaders", "lex");
}

static std::string ReadFile(const char* Path)
{
	std::ifstream In(Path, std::ios::binary);
	std::stringstream S;
	S << In.rdbuf();
	return S.str();
}

static bool IdentityBeforeReadiness(const std::string& Src)
{
	const size_t Fn = Src.find("void EvaluateSlot(UWorld* World, const FTreeInputs& In, FSlot& S)");
	if (Fn == std::string::npos)
	{
		return false;
	}
	const size_t Usage = Src.find("FindUsageRefusal(S, Root, UsageSub)", Fn);
	const size_t Ready = Src.find("Why::DrawShadersMissing", Fn);
	const size_t Meta = Src.find("Why::ShaderMapUnavailable", Fn);
	return Meta != std::string::npos && Usage != std::string::npos && Ready != std::string::npos && Meta < Usage && Usage < Ready;
}

static void TestSourceOrder()
{
	std::string Src = ReadFile(SOURCE_TREE);
	if constexpr (READY_MUTANT == 4)
	{
		const std::string A = "FindUsageRefusal(S, Root, UsageSub)";
		const size_t P = Src.find(A);
		if (P != std::string::npos)
		{
			Src.erase(P, A.size());
			const size_t Q = Src.find("Why::DrawShadersMissing");
			Src.insert(Q == std::string::npos ? Src.size() : Q + 30, A);
		}
	}
	Check(!Src.empty(), "source: TexCorruptTree.cpp readable");
	Check(IdentityBeforeReadiness(Src),
		"source: EvaluateSlot checks metadata (S5), then identity (usage -> default material, S7), then draw readiness (S6)");
}

int main()
{
	TestJudge();
	TestSourceOrder();
	std::printf("texcorrupt readiness selftest (mutant %d): %d checks, %d failures\n", READY_MUTANT, GChecks, GFailures);
	return GFailures == 0 ? 0 : 1;
}
