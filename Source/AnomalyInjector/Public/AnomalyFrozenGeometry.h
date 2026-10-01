#pragma once

namespace AnomalyFrozenGeometry
{
	enum class ESource : unsigned char
	{
		None = 0,
		WholeFrame = 1,
		Box = 2
	};

	inline const char* DescribeSource(ESource Source)
	{
		switch (Source)
		{
		case ESource::WholeFrame: return "whole_frame";
		case ESource::Box:        return "box";
		default:                  return "none";
		}
	}

	inline ESource DecideAtSample(bool bWholeFrameExtent, bool bActorAlive, bool bTargetNamed, bool bBoxFound)
	{
		if (bWholeFrameExtent || (!bActorAlive && !bTargetNamed))
		{
			return ESource::WholeFrame;
		}
		return (bActorAlive && bBoxFound) ? ESource::Box : ESource::None;
	}

	template <class TBox>
	struct TFrozen
	{
		bool bSampled = false;
		ESource Source = ESource::None;
		TBox Box;
	};

	template <class TBox>
	inline TFrozen<TBox> Freeze(bool bWholeFrameExtent, bool bActorAlive, bool bTargetNamed, bool bBoxFound,
		const TBox& BoxAtSample)
	{
		TFrozen<TBox> G;
		G.bSampled = true;
		G.Source = DecideAtSample(bWholeFrameExtent, bActorAlive, bTargetNamed, bBoxFound);
		if (G.Source == ESource::Box)
		{
			G.Box = BoxAtSample;
		}
		return G;
	}

	template <class TBox, class TView, class TVec2, class TProjectBox>
	inline bool Resolve(const TFrozen<TBox>& G, const TView& FrozenView, TProjectBox&& ProjectBox, TVec2& OutMin, TVec2& OutMax)
	{
		OutMin = TVec2(0.0, 0.0);
		OutMax = TVec2(0.0, 0.0);
		if (!G.bSampled)
		{
			return false;
		}
		switch (G.Source)
		{
		case ESource::WholeFrame:
			OutMax = TVec2(1.0, 1.0);
			return true;
		case ESource::Box:
			return ProjectBox(FrozenView, G.Box, OutMin, OutMax);
		default:
			return false;
		}
	}
}
