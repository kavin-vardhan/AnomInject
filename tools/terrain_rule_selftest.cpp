#include "AnomalyTerrainRule.h"

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

using namespace AnomalyTerrainRule;

using FTxt = TText<wchar_t>;
using FPartW = TPart<wchar_t>;
using FObjW = TObject<wchar_t>;

static int GFailures = 0;
static int GChecks = 0;

static void Check(bool bOk, const char* What)
{
	++GChecks;
	if (!bOk)
	{
		++GFailures;
		std::printf("FAIL %s\n", What);
	}
}

static FTxt T(const std::wstring& S)
{
	FTxt Out;
	Out.Ptr = S.c_str();
	Out.Len = (int)S.size();
	return Out;
}

struct FPartSpec
{
	std::wstring Name;
	std::wstring Asset;
	std::vector<std::wstring> Chain;
};

struct FSubject
{
	std::wstring Name;
	std::vector<std::wstring> Chain;
	std::vector<FPartSpec> Parts;

	std::vector<std::vector<FTxt>> PartChains;
	std::vector<FPartW> PartViews;
	std::vector<FTxt> Chain2;
	FObjW Obj;

	const FObjW& Build()
	{
		Chain2.clear();
		for (const std::wstring& C : Chain)
		{
			Chain2.push_back(T(C));
		}
		PartChains.assign(Parts.size(), std::vector<FTxt>());
		PartViews.assign(Parts.size(), FPartW());
		for (size_t i = 0; i < Parts.size(); ++i)
		{
			for (const std::wstring& C : Parts[i].Chain)
			{
				PartChains[i].push_back(T(C));
			}
			PartViews[i].Name = T(Parts[i].Name);
			PartViews[i].Asset = T(Parts[i].Asset);
			PartViews[i].Chain = PartChains[i].empty() ? nullptr : PartChains[i].data();
			PartViews[i].ChainNum = (int)PartChains[i].size();
		}
		Obj.Name = T(Name);
		Obj.Chain = Chain2.empty() ? nullptr : Chain2.data();
		Obj.ChainNum = (int)Chain2.size();
		Obj.Parts = PartViews.empty() ? nullptr : PartViews.data();
		Obj.NumParts = (int)PartViews.size();
		return Obj;
	}
};

static FVerdict Run(FSubject& S, bool bClassActive, const std::vector<std::wstring>& Patterns)
{
	std::vector<FTxt> P;
	for (const std::wstring& X : Patterns)
	{
		P.push_back(T(X));
	}
	return Classify(S.Build(), bClassActive, P.empty() ? nullptr : P.data(), (int)P.size());
}

static FPartSpec StaticMeshPart(const std::wstring& Name, const std::wstring& Asset)
{
	return FPartSpec{ Name, Asset, { L"StaticMeshComponent", L"MeshComponent", L"PrimitiveComponent", L"SceneComponent", L"ActorComponent", L"Object" } };
}

static FPartSpec LandscapePart(const std::wstring& Name)
{
	return FPartSpec{ Name, L"", { L"LandscapeComponent", L"PrimitiveComponent", L"SceneComponent", L"ActorComponent", L"Object" } };
}

static FPartSpec GrassPart(const std::wstring& Name, const std::wstring& Asset)
{
	return FPartSpec{ Name, Asset, { L"GrassInstancedStaticMeshComponent", L"HierarchicalInstancedStaticMeshComponent",
		L"InstancedStaticMeshComponent", L"StaticMeshComponent", L"MeshComponent", L"PrimitiveComponent", L"SceneComponent",
		L"ActorComponent", L"Object" } };
}

static FSubject StaticMeshActor(const std::wstring& Name, const std::wstring& Asset)
{
	FSubject S;
	S.Name = Name;
	S.Chain = { L"StaticMeshActor", L"Actor", L"Object" };
	S.Parts = { StaticMeshPart(L"StaticMeshComponent0", Asset) };
	return S;
}

static FSubject StreamingProxy()
{
	FSubject S;
	S.Name = L"LandscapeStreamingProxy_3_2_0";
	S.Chain = { L"LandscapeStreamingProxy", L"LandscapeProxy", L"PartitionActor", L"Actor", L"Object" };
	S.Parts = { LandscapePart(L"LandscapeComponent_0"), GrassPart(L"GrassInstancedStaticMeshComponent_0", L"SM_GrassClump") };
	return S;
}

static FSubject LandscapeActor()
{
	FSubject S;
	S.Name = L"Landscape_0";
	S.Chain = { L"Landscape", L"LandscapeProxy", L"PartitionActor", L"Actor", L"Object" };
	return S;
}

static void TestPatternCommandAndAllTerrainWarning()
{
	Check(ClassifyPatternCommand(0, nullptr) == EPatternCommand::Usage, "no argument prints the usage");
	Check(ClassifyPatternCommand(1, "clear") == EPatternCommand::BackToIni, "'clear' goes back to the ini list");
	Check(ClassifyPatternCommand(1, "CLEAR") == EPatternCommand::BackToIni, "'CLEAR' is case-insensitive");
	Check(ClassifyPatternCommand(1, "none") == EPatternCommand::None, "'none' sets no patterns");
	Check(ClassifyPatternCommand(1, "None") == EPatternCommand::None, "'None' is case-insensitive");
	Check(ClassifyPatternCommand(1, "Ground") == EPatternCommand::Set, "a word is a pattern");
	Check(ClassifyPatternCommand(1, "nonesuch") == EPatternCommand::Set, "'nonesuch' is a pattern, not 'none'");
	Check(ClassifyPatternCommand(2, "clear") == EPatternCommand::Set, "'clear' with a second word is a pattern list");
	Check(ClassifyPatternCommand(2, "none") == EPatternCommand::Set, "'none' with a second word is a pattern list");
	Check(!WarnEveryBlinkWasTerrain(4, 4), "four of four terrain blink events do not warn (fewer than five)");
	Check(WarnEveryBlinkWasTerrain(5, 5), "five of five terrain blink events warn");
	Check(WarnEveryBlinkWasTerrain(12, 12), "twelve of twelve warn");
	Check(!WarnEveryBlinkWasTerrain(12, 11), "eleven of twelve do not warn");
	Check(!WarnEveryBlinkWasTerrain(0, 0), "no blink event does not warn");
}

int main()
{
	TestPatternCommandAndAllTerrainWarning();
	const std::vector<std::wstring> None;

	{
		FSubject S = StaticMeshActor(L"StaticMeshActor_49", L"Cube");
		const FVerdict V = Run(S, true, None);
		Check(!IsTerrain(V), "plain static mesh actor is not terrain");
		Check(V.Rule == ERule::None && V.Field == EField::None && V.Part == -1 && V.Pattern == -1, "plain verdict is the empty verdict");
	}
	{
		FSubject S = StreamingProxy();
		const FVerdict V = Run(S, true, None);
		Check(V.Rule == ERule::Class, "streaming proxy matches the class rule");
		Check(V.Field == EField::ActorClass, "streaming proxy matches on the actor class chain");
		Check(IsEngineLandscape(V), "streaming proxy is engine landscape");
	}
	{
		FSubject S = LandscapeActor();
		const FVerdict V = Run(S, true, None);
		Check(V.Rule == ERule::Class && V.Field == EField::ActorClass, "Landscape actor matches the class rule through its parent");
	}
	{
		FSubject S = StreamingProxy();
		const FVerdict V = Run(S, false, None);
		Check(!IsTerrain(V), "class rule inactive and no pattern: streaming proxy is not terrain");
	}
	{
		FSubject S;
		S.Name = L"BP_Island_C_1";
		S.Chain = { L"BP_Island_C", L"Actor", L"Object" };
		S.Parts = { StaticMeshPart(L"Rocks", L"SM_Rock"), LandscapePart(L"Ground") };
		const FVerdict V = Run(S, true, None);
		Check(V.Rule == ERule::Class && V.Field == EField::ComponentClass, "an actor owning a landscape component matches the class rule");
		Check(V.Part == 1, "the component class match names the matching part");
	}
	{
		FSubject S;
		S.Name = L"CustomGround_1";
		S.Chain = { L"CustomGroundActor", L"Actor", L"Object" };
		S.Parts = { FPartSpec{ L"Tile", L"", { L"MyLandscapeComponent", L"LandscapeComponent", L"PrimitiveComponent", L"Object" } } };
		const FVerdict V = Run(S, true, None);
		Check(V.Rule == ERule::Class && V.Part == 0, "a landscape component subclass matches through its parent");
	}
	{
		FSubject S;
		S.Name = L"Helper_1";
		S.Chain = { L"LandscapeProxyHelper", L"Actor", L"Object" };
		S.Parts = { FPartSpec{ L"Mesh", L"SM_Helper", { L"LandscapeComponentLike", L"StaticMeshComponent", L"PrimitiveComponent" } } };
		const FVerdict V = Run(S, true, None);
		Check(!IsTerrain(V), "the class rule is an exact class name, never a substring");
	}
	{
		FSubject S;
		S.Name = L"landscapeproxy_1";
		S.Chain = { L"landscapeproxy", L"Actor", L"Object" };
		const FVerdict V = Run(S, true, None);
		Check(!IsTerrain(V), "the class rule is case-exact");
	}
	{
		FSubject S = StaticMeshActor(L"StaticMeshActor_12", L"SM_SlopeWarpLandscape");
		const FVerdict V = Run(S, true, { L"slopewarp" });
		Check(V.Rule == ERule::Pattern && V.Field == EField::Asset, "a lower-case pattern matches a mixed-case mesh asset");
		Check(V.Part == 0 && V.Pattern == 0, "the asset match names its part and pattern");
		Check(!IsEngineLandscape(V), "a pattern match is not engine landscape");
	}
	{
		FSubject S = StaticMeshActor(L"BP_Ground_3", L"SM_Plane");
		S.Chain = { L"BP_Ground_C", L"Actor", L"Object" };
		const FVerdict V = Run(S, true, { L"BP_GROUND_3" });
		Check(V.Rule == ERule::Pattern && V.Field == EField::Actor && V.Part == -1, "an upper-case pattern matches the actor name");
	}
	{
		FSubject S = StaticMeshActor(L"Terrain_Instance_9", L"SM_Plane");
		S.Name = L"Instance_9";
		S.Chain = { L"BP_Terrain_C", L"Actor", L"Object" };
		const FVerdict V = Run(S, true, { L"terrain_c" });
		Check(V.Rule == ERule::Pattern && V.Field == EField::ActorClass, "a pattern matches the actor's own class name");
	}
	{
		FSubject S = StaticMeshActor(L"Prop_2", L"SM_Prop");
		S.Parts = { StaticMeshPart(L"GroundMesh", L"SM_Prop") };
		const FVerdict V = Run(S, true, { L"groundmesh" });
		Check(V.Rule == ERule::Pattern && V.Field == EField::Component && V.Part == 0, "a pattern matches a component name");
	}
	{
		FSubject S = StaticMeshActor(L"Prop_3", L"SM_Prop");
		S.Parts = { FPartSpec{ L"Comp", L"SM_Prop", { L"WorldTileComponent", L"StaticMeshComponent", L"PrimitiveComponent" } } };
		const FVerdict V = Run(S, true, { L"worldtile" });
		Check(V.Rule == ERule::Pattern && V.Field == EField::ComponentClass, "a pattern matches a component's own class name");
	}
	{
		FSubject S = StaticMeshActor(L"Prop_4", L"SM_Prop");
		const FVerdict V = Run(S, true, { L"PrimitiveComponent" });
		Check(!IsTerrain(V), "a pattern never matches a parent class name of a component");
		FSubject A = StaticMeshActor(L"Prop_5", L"SM_Prop");
		const FVerdict W = Run(A, true, { L"Object" });
		Check(!IsTerrain(W), "a pattern never matches a parent class name of the actor");
	}
	{
		FSubject S = StaticMeshActor(L"Prop_6", L"SM_Prop");
		const FVerdict V = Run(S, true, { L"" });
		Check(!IsTerrain(V), "an empty pattern matches nothing");
		const FVerdict W = Run(S, true, { L"", L"sm_prop" });
		Check(W.Rule == ERule::Pattern && W.Pattern == 1, "an empty pattern is skipped and the next one still applies");
	}
	{
		FSubject S = StaticMeshActor(L"Ground_7", L"SM_Rock");
		const FVerdict V = Run(S, true, { L"sm_rock", L"ground" });
		Check(V.Pattern == 0 && V.Field == EField::Asset, "patterns apply in list order: the first matching pattern wins");
	}
	{
		FSubject S = StreamingProxy();
		const FVerdict V = Run(S, true, { L"LandscapeStreaming" });
		Check(V.Rule == ERule::Class, "the class rule wins over a pattern that also matches");
	}
	{
		FSubject S = StreamingProxy();
		const FVerdict V = Run(S, false, { L"LandscapeStreaming" });
		Check(V.Rule == ERule::Pattern && V.Field == EField::Actor, "with the class rule inactive a pattern can still name landscape");
		Check(!RefusesBlink(V, true), "a pattern match on landscape is never refused");
	}
	{
		FSubject S;
		S.Name = L"BP_Courtyard_C_0";
		S.Chain = { L"BP_Courtyard_C", L"Actor", L"Object" };
		S.Parts = { StaticMeshPart(L"Bench", L"SM_Bench"), StaticMeshPart(L"Floor", L"SM_CourtyardGround") };
		const FVerdict V = Run(S, true, { L"courtyardground" });
		Check(V.Rule == ERule::Pattern && V.Field == EField::Asset && V.Part == 1, "a mixed actor is terrain when one part matches; the part is named");
	}
	{
		FSubject S = StaticMeshActor(L"Rock", L"SM_A");
		Check(!IsTerrain(Run(S, true, { L"SM_ABC" })), "a pattern longer than the field never matches");
		Check(IsTerrain(Run(S, true, { L"sm_a" })), "a pattern equal to the whole field matches");
	}
	{
		FVerdict C;
		C.Rule = ERule::Class;
		C.Field = EField::ActorClass;
		FVerdict P;
		P.Rule = ERule::Pattern;
		P.Field = EField::Asset;
		const FVerdict N;
		Check(RefusesBlink(C, true), "an engine-landscape verdict is refused when the refusal is on");
		Check(!RefusesBlink(C, false), "the bench lever turns the refusal off");
		Check(!RefusesBlink(P, true), "pattern-matched mesh terrain is never refused");
		Check(!RefusesBlink(N, true), "a non-terrain verdict is never refused");
	}
	{
		Check(std::strcmp(BlinkSubtypeFor(true), "terrain_disappear_reappear") == 0, "terrain subtype string is the client's");
		Check(std::strcmp(BlinkSubtypeFor(false), "disappear_reappear") == 0, "non-terrain blink keeps disappear_reappear");
		Check(std::strcmp(UnlabelableReason(), "terrain_unlabelable") == 0, "the refusal reason string");
		Check(std::strcmp(DescribeField(EField::ComponentClass), "component_class") == 0, "field names");
	}
	{
		const std::wstring Hay = L"SM_SlopeWarpLandscape";
		const std::wstring Up = L"SLOPEWARP";
		Check(ContainsIgnoreCase(T(Hay), T(Up)), "case-insensitive substring");
		const std::wstring Miss = L"slopewarq";
		Check(!ContainsIgnoreCase(T(Hay), T(Miss)), "a non-substring does not match");
		FTxt Nul;
		Check(!ContainsIgnoreCase(Nul, T(Up)) && !ContainsIgnoreCase(T(Hay), Nul), "an empty side never matches");
		const std::wstring Cls = L"LandscapeProxy";
		Check(EqualsAscii(T(Cls), "LandscapeProxy") && !EqualsAscii(T(Cls), "LandscapeProx") && !EqualsAscii(T(Cls), "LandscapeProxyX"),
			"exact ASCII class equality");
	}

	{
		Check(TerrainCeilingCompiledPct == 25.0f, "terrain ceiling compiled default is 25, equal to the census ceiling (no behaviour change)");
		Check(IsTerrainCeilingInRange(0.0f) && IsTerrainCeilingInRange(40.0f) && IsTerrainCeilingInRange(100.0f), "terrain ceiling range includes 0, 40, 100");
		Check(!IsTerrainCeilingInRange(-0.5f) && !IsTerrainCeilingInRange(100.5f), "terrain ceiling out of range is refused");
		Check(ReadmitAboveCeiling(true, true, true, 33.3f, 40.0f), "terrain blink at 33.3 % re-admitted under a 40 % terrain ceiling");
		Check(ReadmitAboveCeiling(true, true, true, 40.0f, 40.0f), "terrain blink exactly at the terrain ceiling is re-admitted");
		Check(!ReadmitAboveCeiling(true, true, true, 40.1f, 40.0f), "terrain blink above the terrain ceiling stays excluded");
		Check(!ReadmitAboveCeiling(true, true, true, 37.5f, 25.0f), "at the default 25 the ground at 37.5 % stays excluded (no change)");
		Check(!ReadmitAboveCeiling(true, false, true, 33.3f, 40.0f), "only blinking is re-admitted, other ids keep the census ceiling");
		Check(!ReadmitAboveCeiling(true, true, false, 33.3f, 40.0f), "only terrain is re-admitted, other actors keep the census ceiling");
		Check(!ReadmitAboveCeiling(false, true, true, 10.0f, 40.0f), "a verdict that is not above_ceiling is never touched");
		Check(!ReadmitAboveCeiling(true, true, true, -1.0f, 40.0f), "an unmeasured coverage is never re-admitted");
	}

	std::printf("terrain_rule_selftest: %d checks, %d failures\n", GChecks, GFailures);
	return GFailures == 0 ? 0 : 1;
}
