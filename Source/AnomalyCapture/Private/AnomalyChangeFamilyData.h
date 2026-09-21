#pragma once

#include "AnomalyChangeStage.h"
#if ANOMALY_CAPTURE
#include "SceneView.h"

// UE copies this shared extension data with FSceneViewFamily into FViewFamilyInfo.
// Thus it follows the actual family, unlike FrameNumber (shared by multiple families).
class FAnomalyChangeFamilyData : public ISceneViewFamilyExtentionData
{
public:
	static const TCHAR* GSubclassIdentifier;
	virtual const TCHAR* GetSubclassIdentifier() const override { return GSubclassIdentifier; }
	FAnomalyChangeIssuePtr Issue;
	uint64 FamilyId = 0;
	// Render-thread-only claims; not part of the immutable issued identity/receipt.
	bool bColourServed = false;
	bool bMaskServed = false;
	bool Claim(const FSceneView& View, bool bMask);
	static FAnomalyChangeFamilyData* Find(const FSceneViewFamily* Family)
	{
		return Family ? const_cast<FSceneViewFamily*>(Family)->GetExtentionData<FAnomalyChangeFamilyData>() : nullptr;
	}
	static uint64 NextId();
};
#endif
