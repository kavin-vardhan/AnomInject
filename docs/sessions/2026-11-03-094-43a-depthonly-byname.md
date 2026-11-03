# 094-43a - Stop intent and depth-only name lookup

Entry passes against 094-42c: m51 and protected refs, Paris protected files and all 27 bound binaries, the seven fixture assets and preserved Saved ini. Origin remains d1cdc7b and the previous spike remains 740220c.

GC6 records intent before termination, carries it through execution.json, and classifies a subsequent matching root exit as owned-stop. A root crash followed by cleanup remains a crash. Seven focused tests pass, including missing intent, wrong PID, late intent, persistence failure and termination failure. No editor/game is launched by these tests. No native code changes on the feature branch; FJ1 therefore calls for no rebuild or strict-include sweep for GC6.

The requested route is under source review. A new fixture-only branch will start at d840cdb; no change is made to the prior spike. No push or tags.

094-43a runtime scope now admits only this fixture job and enforces GA5 admission at three hours. Focused scope/argv tests pass, including missing -nowrite and an E: 49 GiB negative. The final experimental Editor build passes attempt 3; Game passes attempt 1. No runtime session has started. Tools are sealed before arming.
