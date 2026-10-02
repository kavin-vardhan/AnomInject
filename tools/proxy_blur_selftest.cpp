#include "AnomalyProxyBlurPolicy.h"
#include <cstdio>

int main()
{
	using namespace AnomalyProxyBlur;
	using P = AnomalyLabelSync::EAnnotationPolicy;
	int Failed = 0;
	int Checks = 0;
	auto Check = [&](bool Ok) { ++Checks; if (!Ok) { ++Failed; } };
	Check(SelectRoute(ERoute::Auto, false) == ERoute::Hold);
	Check(SelectRoute(ERoute::Auto, true) == ERoute::Proxy);
	Check(SelectRoute(ERoute::Hold, true) == ERoute::Hold);
	Check(SelectRoute(ERoute::Hold, false) == ERoute::Hold);
	Check(SelectRoute(ERoute::Proxy, false) == ERoute::Proxy);
	Check(SelectRoute(ERoute::Proxy, true) == ERoute::Proxy);
	Check(TargetMips(12, 7, 12, -1, 4) == 7);
	Check(TargetMips(9, 7, 11, -1, 4) == 7);
	Check(TargetMips(12, 7, 12, 2, 4) == 10);
	Check(TargetMips(9, 7, 11, 15, 4) == 7);
	Check(TargetMips(7, 7, 12, -1, 4) == 7);
	Check(TargetMips(8, 2, 8, -1, 4) == 4);
	Check(CopyDrop(9, 7) == 2);
	Check(CopyDrop(12, 7) == 5);
	Check(CopyDrop(7, 7) == 0);
	Check(CopyDrop(7, 9) == 0);
	Check(Policy(false, P::RenderHeldWindow) == P::RenderHeldWindow);
	Check(Policy(false, P::AnomalyState) == P::AnomalyState);
	Check(Policy(true, P::RenderHeldWindow) == P::FireWindow);
	Check(Policy(true, P::AnomalyState) == P::FireWindow);
	Check(AnomalyLabelSync::IsAnnotationMember(Policy(true, P::AnomalyState), false, true, true));
	Check(!AnomalyLabelSync::IsAnnotationMember(Policy(true, P::AnomalyState), true, true, false));
	Check(!AnomalyLabelSync::IsAnnotationMember(Policy(true, P::AnomalyState), true, false, true));
	Check(AnomalyLabelSync::IsEffectInterrupted(Policy(true, P::AnomalyState), false));
	Check(!AnomalyLabelSync::IsEffectInterrupted(Policy(false, P::RenderHeldWindow), false));
	std::printf("proxy blur %d checks, %d failures\n", Checks, Failed);
	return Failed ? 1 : 0;
}
