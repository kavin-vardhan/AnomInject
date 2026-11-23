#ifdef TARGET_POLICY_HEADER
#include TARGET_POLICY_HEADER
#else
#include "../Source/AnomalyInjector/Public/AnomalyTargetPolicy.h"
#endif

#include <cstdio>
#include <cstring>
#include <string>

using namespace AnomalyTargetPolicy;

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

static void TokenCases()
{
	Check(ClassifyTargetToken('@') == ETargetToken::Path, "@ is a path token");
	Check(ClassifyTargetToken('=') == ETargetToken::ExactName, "= is an exact-name token");
	Check(ClassifyTargetToken('S') == ETargetToken::Substring, "a bare token is a substring query");
	Check(ClassifyTargetToken('/') == ETargetToken::Substring, "a bare path without @ is a substring query");
	Check(ClassifyTargetToken(0) == ETargetToken::Substring, "an empty token is a substring query");
	Check(std::strcmp(DescribeTargetToken(ETargetToken::Path), "path") == 0, "path described");
	Check(std::strcmp(DescribeTargetToken(ETargetToken::ExactName), "exact_name") == 0, "exact name described");
	Check(std::strcmp(DescribeTargetToken(ETargetToken::Substring), "substring") == 0, "substring described");
	Check(std::strcmp(DescribeAmbiguousReason(), "target_ambiguous") == 0, "ambiguous reason name");
	Check(AmbiguousPathsLogged == 3, "three paths are logged");
}

static void ApplyAmbiguityCases()
{
	Check(!IsApplyTokenAmbiguous(ETargetToken::ExactName, 0), "apply: no actor of that name is not ambiguous");
	Check(!IsApplyTokenAmbiguous(ETargetToken::ExactName, 1), "apply: one actor of that name is not ambiguous");
	Check(IsApplyTokenAmbiguous(ETargetToken::ExactName, 2), "apply: two actors of that name are ambiguous");
	Check(IsApplyTokenAmbiguous(ETargetToken::ExactName, 402), "apply: many namesakes are ambiguous");
	Check(IsApplyTokenAmbiguous(ETargetToken::Substring, 2), "apply: a bare token equal to two names is ambiguous");
	Check(!IsApplyTokenAmbiguous(ETargetToken::Substring, 0), "apply: a substring that is no actor's exact name keeps its multi-actor meaning");
	Check(!IsApplyTokenAmbiguous(ETargetToken::Substring, 1), "apply: a bare unique name is not ambiguous");
	Check(!IsApplyTokenAmbiguous(ETargetToken::Path, 2), "apply: a path token is never ambiguous");
	Check(!IsApplyTokenAmbiguous(ETargetToken::Path, 1000), "apply: a path token is never ambiguous, whatever the name count");
}

static void TargetedAmbiguityCases()
{
	Check(!IsTargetedNameAmbiguous(ETargetToken::Substring, 0), "targeted: no match is not ambiguous (0 matched)");
	Check(!IsTargetedNameAmbiguous(ETargetToken::Substring, 1), "targeted: a unique name keeps working");
	Check(IsTargetedNameAmbiguous(ETargetToken::Substring, 2), "targeted: two matches are refused");
	Check(IsTargetedNameAmbiguous(ETargetToken::Substring, 7), "targeted: a class name with seven actors is refused");
	Check(IsTargetedNameAmbiguous(ETargetToken::ExactName, 3), "targeted: an =name with three matches is refused");
	Check(!IsTargetedNameAmbiguous(ETargetToken::ExactName, 1), "targeted: an =name with one match keeps working");
	Check(!IsTargetedNameAmbiguous(ETargetToken::Path, 1), "targeted: a path is never ambiguous");
	Check(!IsTargetedNameAmbiguous(ETargetToken::Path, 2), "targeted: a path is never ambiguous even if two were reported");
}

int main()
{
	TokenCases();
	ApplyAmbiguityCases();
	TargetedAmbiguityCases();
	std::printf("target_identity_selftest: %d checks, %d failures\n", GChecks, GFailures);
	return GFailures == 0 ? 0 : 1;
}
