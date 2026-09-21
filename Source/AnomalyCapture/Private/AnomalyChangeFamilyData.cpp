#include "AnomalyChangeFamilyData.h"
#if ANOMALY_CAPTURE
#include "HAL/ThreadSafeCounter64.h"

const TCHAR* FAnomalyChangeFamilyData::GSubclassIdentifier = TEXT("AnomalyChangeFamilyData.m55");
uint64 FAnomalyChangeFamilyData::NextId()
{
	static FThreadSafeCounter64 Serial;
	return (uint64)Serial.Increment();
}
bool FAnomalyChangeFamilyData::Claim(const FSceneView& View, bool bMask)
{
	auto Stage = Issue.IsValid() ? Issue->Stage.Pin() : nullptr;
	if (!Stage.IsValid()) { return false; }
	// Claim only the actual family/view here. Generation is checked before publishing a
	// receipt at drain, so expiration of this additional consumer cannot drop a legacy PNG.
	if (!View.Family || View.Family->Views.Num() != 1 || View.Family->Views[0] != &View
		|| View.Family->RenderTarget != Issue->OwnerTarget || View.Family->Scene != Issue->OwnerScene)
	{
		Stage->Diagnostic(TEXT("view_rejected"));
		return false;
	}
	bool& Served = bMask ? bMaskServed : bColourServed;
	if (Served) { Stage->Diagnostic(TEXT("duplicate_callback")); return false; }
	Served = true;
	return true;
}
#endif
