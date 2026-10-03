#include "../Source/AnomalyCapture/Private/AnomalyCaptureOwnership.h"
#include "../Source/AnomalyCapture/Private/AnomalyMaskServe.h"
#include <cstdio>

using namespace AnomalyCaptureOwnership;
static int Checks = 0;
static int Failures = 0;
static void Check(bool Value, const char* Name)
{
	++Checks;
	if (!Value) { ++Failures; std::printf("FAIL %s\n", Name); }
}

int main()
{
	int GameScene = 1, EditorScene = 2, GameViewport = 3, OtherViewport = 4;
	Check(IsOwner(&GameScene, &GameViewport, &GameScene, &GameViewport), "owner admitted");
	Check(!IsOwner(&GameScene, &GameViewport, &EditorScene, &GameViewport), "different world rejected");
	Check(!IsOwner(&GameScene, &GameViewport, &GameScene, &OtherViewport), "same world different viewport rejected");
	Check(!IsOwner(nullptr, &GameViewport, nullptr, &GameViewport), "missing scene fails closed");
	Check(!IsOwner(&GameScene, nullptr, &GameScene, nullptr), "missing viewport fails closed");
	Check(!IsOwner(&GameScene, &GameViewport, nullptr, nullptr), "scene capture without owner fails closed");
	for (int OwnerFirst = 0; OwnerFirst < 2; ++OwnerFirst)
	{
		bool ColourPending = true, MaskPending = true;
		int ColourOwner = 0, MaskOwner = 0, Foreign = 0;
		for (int I = 0; I < 2; ++I)
		{
			const bool Owner = (I == 0) == (OwnerFirst != 0);
			const bool Accepted = IsOwner(&GameScene, &GameViewport, Owner ? &GameScene : &EditorScene, Owner ? &GameViewport : &OtherViewport);
			if (Accepted && ColourPending && IsSampleFrame(10, 10))
			{
				ColourPending = false;
				if (Owner) { ++ColourOwner; } else { ++Foreign; }
			}
			if (Accepted && MaskPending && AnomalyMaskServe::ArmBelongsToFamily(1, true, 1) && IsSampleFrame(10, 10))
			{
				MaskPending = false;
				if (Owner) { ++MaskOwner; } else { ++Foreign; }
			}
		}
		Check(ColourOwner == 1 && MaskOwner == 1 && Foreign == 0, "review counterexample: both callback orders keep owner colour and mask");
	}
	Check(!AnomalyMaskServe::ArmBelongsToFamily(1, false, 1), "missing family bound rejects mask");
	Check(!AnomalyMaskServe::ArmBelongsToFamily(2, true, 1), "later arm cannot use earlier render");
	Check(IsSampleFrame(10, 10), "lagged owner render uses its frozen sample frame");
	Check(!IsSampleFrame(10, 11), "skipped sample cannot use later owner render");
	Check(!IsSampleFrame(11, 10), "future sample cannot use earlier owner render");
	Check(HasExpired(10, 11), "skipped owner request becomes unpaired");
	Check(!HasExpired(11, 11), "current sample remains live");
	Check(IsSampleFrame(11, 11), "fresh sample after skipped render is admitted");
	Check(MatchesFamilyKey(1, 10, 1, 10), "same family key matches");
	Check(!MatchesFamilyKey(1, 10, 1, 11), "equal scene frame numbers do not alias distinct families");
	Check(!MatchesFamilyKey(1, 10, 2, 10), "corrupted scene frame still misses");
	std::printf("capture_ownership_selftest: %d checks, %d failures\n", Checks, Failures);
	return Failures ? 1 : 0;
}
