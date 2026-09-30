#pragma once

#include "AnomalyLabelSync.h"

namespace AnomalyActiveSource
{
	enum class ESource : unsigned char
	{
		FireWindow = 0,
		ActorHidden = 1,
		AnomalyState = 2
	};

	struct FEntry
	{
		const char* Id;
		ESource Source;
	};

	inline const FEntry* Table(int& OutNum)
	{
		static const FEntry Entries[] =
		{
			{ "blinking",          ESource::ActorHidden },
			{ "missing_object",    ESource::ActorHidden },
			{ "lod_popping",       ESource::AnomalyState },
			{ "missing_texture",   ESource::FireWindow },
			{ "corrupted_texture", ESource::FireWindow },
			{ "null_effect",       ESource::FireWindow },
			{ "solid_swap",        ESource::FireWindow },
			{ "lighting_mismatch", ESource::FireWindow },
			{ "lod_corruption",    ESource::FireWindow },
			{ "camera_clipping",   ESource::AnomalyState },
			{ "stuck_low_mip",     ESource::AnomalyState },
			{ "uv_corruption",     ESource::FireWindow },
			{ "normal_corruption", ESource::FireWindow },
			{ "time_dilation",     ESource::FireWindow }
		};
		OutNum = (int)(sizeof(Entries) / sizeof(Entries[0]));
		return Entries;
	}

	inline bool SameId(const char* A, const char* B)
	{
		if (!A || !B)
		{
			return false;
		}
		while (*A && *A == *B)
		{
			++A;
			++B;
		}
		return *A == *B;
	}

	inline bool Resolve(const char* Id, ESource& Out)
	{
		int Num = 0;
		const FEntry* Entries = Table(Num);
		for (int i = 0; i < Num; ++i)
		{
			if (SameId(Entries[i].Id, Id))
			{
				Out = Entries[i].Source;
				return true;
			}
		}
		Out = ESource::FireWindow;
		return false;
	}

	inline AnomalyLabelSync::EAnnotationPolicy PolicyFor(ESource Source, bool bRenderTruthFire)
	{
		if (bRenderTruthFire)
		{
			return AnomalyLabelSync::EAnnotationPolicy::RenderHeldWindow;
		}
		switch (Source)
		{
		case ESource::ActorHidden:  return AnomalyLabelSync::EAnnotationPolicy::ActorHidden;
		case ESource::AnomalyState: return AnomalyLabelSync::EAnnotationPolicy::AnomalyState;
		default:                    return AnomalyLabelSync::EAnnotationPolicy::FireWindow;
		}
	}

	inline bool IsLabelledMember(ESource Source, bool bRenderTruthFire, bool bActive, bool bOnScreen, bool bInstalled)
	{
		return AnomalyLabelSync::IsAnnotationMember(PolicyFor(Source, bRenderTruthFire), bActive, bOnScreen, bInstalled);
	}
}
