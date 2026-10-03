#include "../Source/AnomalyInjector/Public/AnomalyLabelSync.h"
#include <cstdio>

int main()
{
	using namespace AnomalyLabelSync;
	int Failed = 0;
	auto Check = [&Failed](bool Good, const char* Name) { if (!Good) { ++Failed; std::printf("FAIL %s\n", Name); } };
	const auto Applied = AppliedBit(201);
	Check(DecideScopeExit(false, true, 201, true, Applied) == ERetireAction::RestoreValueAndFlag, "excluded tracked component releases our census tag");
	Check(DecideScopeExit(true, true, 201, true, Applied) == ERetireAction::Leave, "installed component keeps its tag");
	Check(DecideScopeExit(false, true, 201, false, Applied) == ERetireAction::RestoreValueKeepHostFlag, "host-disabled custom depth stays disabled");
	Check(DecideScopeExit(false, false, 201, true, Applied) == ERetireAction::Leave, "untracked host component is untouched");
	Check(DecideScopeExit(false, true, 17, true, Applied) == ERetireAction::Leave, "host replacement value is untouched");
	Check(DecideScopeExit(false, true, 201, true, 0) == ERetireAction::Leave, "unowned reserved value is untouched");
	std::printf("stencil_scope_selftest: 6 checks, %d failures\n", Failed);
	return Failed ? 1 : 0;
}
