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
	Check(UsesTransitions(true, 3, 16));
	Check(UsesTransitions(true, 0, 16));
	Check(UsesTransitions(true, 3, 0));
	Check(!UsesTransitions(true, 0, 0));
	Check(!UsesTransitions(false, 3, 16));
	AnomalyLabelSync::FEventTransitionTrack Track;
	for (int Si = 0; Si < 48; ++Si)
	{
		bool On = false, Off = false;
		Track.Observe(Si, Si < 20, 3, 16, On, Off);
		Check(On == (Si < 3));
		Check(Off == (Si >= 20 && Si < 36));
	}
	Track.Rebase(24);
	Check(Track.LastMember() == -5);
	Check(!Track.OffWindowPassed(0, 16));
	Check(Track.OffWindowPassed(12, 16));
	std::printf("proxy blur %d checks, %d failures\n", Checks, Failed);
	return Failed ? 1 : 0;
}
