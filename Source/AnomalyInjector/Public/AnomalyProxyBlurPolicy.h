#pragma once

#include "AnomalyLabelSync.h"

namespace AnomalyProxyBlur
{
	enum class ERoute : unsigned char { Auto, Hold, Proxy };

	inline ERoute SelectRoute(ERoute Setting, bool bShared)
	{
		return Setting == ERoute::Auto ? (bShared ? ERoute::Proxy : ERoute::Hold) : Setting;
	}

	inline int TargetMips(int Resident, int Floor, int Maximum, int Levels, int Minimum)
	{
		const int Target = Levels < 0 ? (Floor > Minimum ? Floor : Minimum) : Resident - Levels;
		return Target < Floor ? Floor : (Target > Maximum ? Maximum : Target);
	}

	inline int CopyDrop(int Resident, int Target)
	{
		return Resident > Target && Target > 0 ? Resident - Target : 0;
	}

	inline AnomalyLabelSync::EAnnotationPolicy Policy(bool bProxy, AnomalyLabelSync::EAnnotationPolicy HoldPolicy)
	{
		return bProxy ? AnomalyLabelSync::EAnnotationPolicy::FireWindow : HoldPolicy;
	}
}
