# 094-17 owner leg — Chat ruling: the A6 owner leg ran, our probe crashed the editor at exit, and 094-18 checks the evidence and fixes the probe

**Date:** 2026-10-27 · **Author:** chat Claude (orchestrator)

**Input:** the owner's report after running the kit with `_reviews\A6-owner-card.md`, as authorized by 094-17-ruling ER4.

## What happened
**Everything the card asked for worked:**
- the editor opened through the kit;
- shader compilation finished;
- the mesh preview of `SM_WindowHouse3` stayed open for a minute;
- the owner closed the editor with File > Exit.

**About a minute after Exit, the editor crashed.** The crash-report window showed:
- `Assertion failed: !CurrentActor->IsUnreachable()`, at 4.25 `Engine/Source/Runtime/Engine/Public/EngineUtils.h:248`, with `WorldSettings None.None:None.None`;
- the call chain:
  - a world-cleanup delegate (`TBaseRawMethodDelegateInstance<…FEuropaPhaseAWorldProbe, void(UWorld*, bool, bool)>`);
  - → `FEuropaPhaseAWorldProbe::Cleanup()` (`EuropaPhaseAWorldProbe.cpp:157`);
  - → `Snapshot()` (`EuropaPhaseAWorldProbe.cpp:281`);
  - → `SnapshotState()` (`EuropaPhaseAProbeDriver.cpp:282`);
  - → the assertion.
- The frames below the delegate run through Engine, UnrealEd and CoreUObject, which is consistent with world teardown during the final garbage collection at exit.

## Rulings

### ES1. The A6 owner leg is DONE, pending 094-18's evidence check
- The kit's single authorized session is spent.
- The leg is **not re-run** unless 094-18 shows that its evidence was lost in the crash.

### ES2. The crash is a defect in our probe, not a STOP
**The cause:** our host-only probe walks the level's actors with an actor iterator while the world is being torn down at exit, after garbage collection has already marked those actors unreachable. 4.25's iterator asserts on that.

**Scope:** the production plugin is not involved.

**The fix, in the probe only:**
- **In every cleanup or teardown path, never use `TActorIterator` or `TActorRange`.** Walk each level's actor list instead, skipping null, pending-kill and unreachable actors and counting them.
- **When the world is torn down at engine exit or during garbage collection,** record a cleanup row saying the actor snapshot was skipped, and why.
- **One probe source for all hosts (EO4).** Put any engine difference behind `ENGINE_MAJOR_VERSION`.
- **Check every other probe path for the same pattern:** in 5.1 too, and at the end of Play-in-Editor.
- **Can-fail:** a static check that refuses any actor iterator in a cleanup path. A mutant that puts one back must fail it.

**The clean exit is proven at the next runtime leg.** A crash there is a STOP.

### ES3. Crash-report facts
**From the crash reporter's own logs, establish:**
- whether a report was uploaded;
- whether the editor was restarted from the crash window. A restart would be a second session outside the kit: record it;
- that no editor process of ours is still running;
- every file the crash reporter wrote outside D: (EA4).

**Future owner cards** will say: on a crash window, choose "Close Without Sending".

### ES4. Rebuild and re-bind after the fix
- **6 rebuilds,** with EO3, EI2, EG2 and the EN1 makefile proofs.
- **EI4,** after the Baseline51 rebuild.
- **Re-bind the three manifests,** with the new probe hashes and source labels.

**This amends 094-15's "A6–A8 use exactly these binaries":**
- A6's owner-leg evidence stays bound to the 094-15 binaries;
- A7 onward uses the re-bound binaries.

### ES5. No re-arm now
- The 09417 kit stays spent.
- The next runtime leg's arming comes with that leg's own job, together with ER1's G3 fix and ER3's bare-UBT gap.

## Next
**094-18 is queued, with no launch.** In order:
1. the EP3 evidence check, including ES3;
2. the crash's root cause in source;
3. the ES2 fix;
4. the ES4 rebuilds and re-binding.

**If 094-18's evidence shows any Paris or plugin content changed,** it stops before the fix and reports.
