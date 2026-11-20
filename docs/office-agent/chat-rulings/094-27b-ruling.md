# 094-27b — Chat ruling (pre-ruled from the live evidence at 15:35 IST): the fixes worked, and the first test found a real bug in the A6 maps (a rolled camera, gotcha G410). Our script's error path then crashed the editor on exit. Fix both, re-author the two maps, and run again tonight (094-27c)

**Date:** 2026-10-30, about 15:40 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Codex's report:** run folder `report.md` (10:44–15:26 IST, exit 0). Its verdict: RED; smoke passed; A3 Load crashed after `Saved camera rotation differs`.
- **Claude Code's midjob notes.** Code's review was still being written when this was ruled. 094-27c reads it, and stops at entry if it shows a protected-state change or an upload.
- **Chat's own read of the A6 recipe source,** as printed in Codex's event log.

## Verdict
**The fixes all worked:**
- FH2's readers passed the 5 real probe files and rejected 115 mutations;
- 35 FH checks and 8 rehearsal cases passed, as did two 18-leg dry runs;
- Codex caught its own hard-stop classification gap before any launch (`ecfb3ce`);
- **FH5 worked in two real sessions:** the respawned crash monitor was adopted by `-MONITOR=<owned PID>` both times.

**The waiting was correct.** From 11:30 to 14:40 the gate held on FD3's memory lines and FA5's idle rule while the PC was in use. Also, D: dipped to 28.5 GiB when `pagefile.sys` grew to 45 GB under commit pressure. It recovered on its own.

**The sessions:**
- **Smoke:** 14:41 IST, exit 0. PASS.
- **A3 Load:** 14:43:40–14:43:53 IST.
  1. The driver's check raised `Saved camera rotation differs` (it expected pitch −12, yaw 0, roll 0).
  2. The log then shows three loads of EA_Core and three quit requests.
  3. Then the editor crashed during shutdown: an access violation reading `0xffffffff`, with Niagara at the top of the stack. Exit 3.
  4. The crash monitor logged `Editor/ExitCode:3`. There was no crash folder and no upload sign.

**Nothing protected changed:**
- the EA8 exit proof passed (23,033 files);
- Content, config, the descriptors, the `.uproject` and marker 31124 are equal;
- canary 0 hits.

**RED** here means only "a crash", which ended the run as FH3 ruled.

**The camera: the check was right, and the recipe was wrong.**
- The A6 recipe spawns the camera with `unreal.Rotator(-12,0,0)`.
- In UE's Python, positional Rotator arguments are **(roll, pitch, yaw)**. This plugin already documents it as gotcha **G410** (2026-09-30).
- So both maps' cameras are rolled by −12° instead of pitched down by 12°. The view is tilted and level, not the slightly downward view that the A5 manifest and the picture regions assume.
- **Why nobody saw it earlier:**
  - FC2's recording stub logs the arguments without modelling the signature;
  - 094-26d's review checked labels and mappings, not saved values (FG4 O5 sent that to the runtime run);
  - so the Load leg's read-back was the first check that could catch it, and it did.
- The recipe's other rotators are all `(0,0,0)`, which this doesn't affect.

**The crash:**
- **The likely cause is our own driver's error path.** The repeated load-and-quit sequence suggests that the check's exception left a repeating callback registered, which loaded the map and asked to quit again, until the engine crashed in Niagara while shutting down.
- This is a hypothesis until Codex confirms it from the source.
- **No anomaly code was involved.** None of the plugin's injection code had run yet.

**Part of this is mine:** G410 was already in our gotchas, and no request of mine required a check for it.

## Rulings

### FI1. 094-27b is accepted as evidence
- The 09427b runtime kit is spent.
- **Proven in real sessions:** FH2–FH8 and FH5's monitor adoption.

### FI2. Keyword-only rotators, and re-authoring A6 (an authorized Content change)
**The recipe fix:** the camera becomes `unreal.Rotator(pitch=-12.0, yaw=0.0, roll=0.0)`.
- **An in-session check:** before saving, the recipe reads the spawned camera's rotation back and requires pitch −12, yaw 0 and roll 0, within ±0.01°.
- **If that check fails,** the session saves nothing and asks to exit once.

**A standing rule (G410's class):**
- **Keywords only:** every `unreal.Rotator(...)` in any script we run inside the editor or the game passes pitch, yaw and roll by keyword.
- **A static check in every kit's tests** fails any positional `unreal.Rotator(` call that has arguments.
- **The Python 2.7 recording stub models the real signature,** `Rotator(roll=0.0, pitch=0.0, yaw=0.0)`, so a positional call lands in roll first.
- **The can-fail:** the old recipe line must fail the camera-angle assertion.

**Re-authoring:**
1. Quarantine `SM_EA_TwoLod.uasset`, `EA_Core.umap` and `EA_Travel.umap` by receipt, as FF1 did.
2. Re-arm the A6 authoring kit for 09427c, with fresh EA_Core and EA_Travel slots. 09426d and older armings are refused.
3. Author both maps fresh.

There is no authoring smoke session. As in FF3, only a value changed: the bindings and the argv are the same.

**The authorized Content change** is only those three declared outputs, at their exact paths. Everything else stays protected.

### FI3. The driver's failure path is single-shot (the crash fix)
**On any check failure, in this order:**
1. record every actual value, and the expected value next to it;
2. unregister every callback;
3. request exit exactly once;
4. after an exit request, load nothing, spawn nothing, and do nothing else.

**The cause:**
- Codex establishes the crash cause from the Load log and the driver source: where the callback is registered, where the failure path runs, and why the map loaded three times. Cite the lines.
- The fix applies whether or not the cause is confirmed. Report the cause as confirmed or unconfirmed.

**The camera check:** compare the normalized angles within ±0.01°, and record actual and expected for every value.

**The can-fails (Python 2.7 harness):**
- a failing check gives exactly one exit request, and no load after it;
- a repeating callback that raises is unregistered on its first raise.

### FI4. In the runtime kit, the first crash fails its leg and a second ends the run (amends FB5 and FH3)
**Why:**
- each runtime leg is a separate session that saves nothing;
- today's crash came from our own error path and harmed nothing;
- one crash should not cost the other 16 legs' evidence.

**These still end the run:**
- a second crash in the run;
- **any crash in an authoring session,** because it writes Content (quarantine per EX3);
- an upload sign or a canary hit;
- a protected change;
- a broken smoke.

**After a first crash:**
- stop the leg's owned tree by PID, the adopted monitor included;
- record any new crash folder with its leg;
- the next leg runs its fast checks as usual.

### FI5. OS-held files in the canary scan
- **The class:** files under `C:\Windows\` that the OS holds locked join FE2's OS-protected class. Today that was three Windows Update datastore and log files.
- **Treatment:** recorded by name and size, and never counted as a coverage gap.
- **Why:** our unelevated processes cannot write under `C:\Windows\`.

### FI6. 094-27c, now: fix, re-author, re-arm, then run, all in one job
**In order:**
1. **Entry:**
   - the EA8 entry proof against v2;
   - ED1 space and the ED3 toolchains;
   - 094-27b's results: stop at entry if Code's review shows a protected change or an upload.
2. **The fixes, now, in the daytime:**
   - FI2's recipe change, static check and stub;
   - FI3's driver and camera check;
   - FI4's crash counting, and FI5's class;
   - the engine's Python 2.7.14 harness for every changed script, the can-fails, and the suites that cover changed code;
   - **qualify everything in the runtime kit that does not depend on the new packages.**

   D2 applies: two attempts.
3. **Rehearsal:** the runtime kit's eight cases, plus FI3's and FI4's new ones: a first crash continues the run, and a second ends it.
4. **Authoring:**
   - quarantine the three packages by receipt;
   - re-arm the A6 kit for 09427c, and take one live, observe-only gate reading;
   - then, under FA5's idle rule, author EA_Core and then EA_Travel.
5. **Runtime:**
   - after both maps exist, bind the new package hashes and arm the runtime kit for 09427c: fresh single-use slots for all 18 legs, the smoke included;
   - dry runs of every leg, and one live gate reading;
   - then run when idle, in FH9's order.

   **If any of steps 2–5 fails twice,** stop with NEEDS-DECISION before the next real launch.
6. **Evidence and contact sheets** per FB6, then a lean exit (FA6; a milestone).

**The window:** until 06:00 IST on 10-31.

## For the owner
- **Nothing to do.**
- **The test maps get rebuilt with the camera fixed,** and then all the tests run, the next time your PC is idle for 10 minutes: tonight, unless you're out before then.
