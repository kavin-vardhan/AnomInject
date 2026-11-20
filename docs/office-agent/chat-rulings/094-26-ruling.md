# 094-26 — Chat ruling: the first real editor run closed cleanly, but our Python launcher failed, and the editor sent usage analytics; 094-28 fixes both tonight, smoke-tests in the real editor, and runs the authoring for real

**Date:** 2026-10-29, about 21:20 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Codex's report:** `_reviews\094-26-codex-report.md` (094-26r, 13:58–20:25 IST, 23,166 s, 1 try, exit 0). Its verdict: `RED | EA_Core authoring failed; a bound config disappeared and shutdown analytics sending was recorded; EA_Travel was not started.`
- **Claude Code's review:** `_reviews\094-26-code-review.md`. 47 rows: 37 OK · 6 N/A · 1 OPEN (O3) · 2 GAP · 1 FAIL (G2) · 1 STOP (G3). P9-protected state: 0 changes.
- **Mailbox report 094-26r:** RED, 418 min.

## Verdict
**The first real run proves a lot.**
- The kit's gate, the launcher exemption, the idle pre-check, the locks and the quarantine all worked live:
  - the pre-check passed at 18:30;
  - the gate passed at 18:56;
  - the editor ran 18:58:33–18:59:17.
- **The 4.25 editor started with our plugin and probe, and closed cleanly:**
  - one skipped engine-exit cleanup row and exactly one `probe_stop`;
  - no ensure, no fatal, and no plugin or probe error.
  - **Code's O1, the clean exit at runtime, is closed.**
- **Protected state:**
  - nothing under Content, `Config`, the descriptor or the `.uproject` changed;
  - P9 is unchanged;
  - nothing crashed.

**But it is RED, as EY7 defines it:**
- **G1:** our authoring launcher fails under the editor's Python 2.7.14.
  - The generated `author.py` calls a function from the dict that `runpy.run_path` returned. Python 2.7 frees that temporary module's globals on exit (`runpy.py:272–274`, `:44`), so `os` was `None` when `author()` ran.
  - No stand-in rehearsal can run the editor's own Python, so none could catch it.
- **G2:** two usage-analytics POSTs to Epic's data router, despite EU4. One came from the editor (AnalyticsET), the other from the crash reporter's session summary (`EditorSessionSummarySender.cpp:279`). EU4 covers crash reports only. **No crash report was uploaded.**
- **G3:** a kit-bound crash-client config was removed by the engine's own two-day purge (`GenericPlatformCrashContext.cpp:363–397, 909–932`). The kit had bound engine-managed state as if it were project config.

**State:** EA_Core's 09425 recipe is spent (one session, 0 partial outputs). EA_Travel was not started. 094-27 is held, as designed.

## Rulings

### FC1. Accepted as evidence
- The clean runtime exit, the live gate and the routing evidence are accepted.
- The 09425 arming is spent for EA_Core and retired. 094-28 re-arms.

### FC2. G1: fix the launcher, and test every in-editor script under the editor's own Python 2.7
- **Keep the fixture module alive** while `author()` runs. For example, use `imp.load_source`, or `exec` into a dict the launcher holds, or make the call inside the run file itself. Codex chooses, citing 2.7 source.
- **Test under the real interpreter:** every script that runs inside the editor is tested under the engine's own interpreter, `E:\EpicGames\UE_4.25\Engine\Binaries\ThirdParty\Python\Win64\python.exe` (2.7.14). A recording stub stands in for the `unreal` module.
- **What the tests cover:** syntax, imports, the launcher path, and each recipe's full call sequence. Nothing is saved.
- **The mutant:** a launcher that uses `runpy`'s returned dict must fail the test.

### FC3. A real-editor smoke session before the real recipes (Code's suggestion, adopted)
**One short editor session per armed job, authorized separately, before any recipe slot is used.**
- **What it does:**
  - the real launcher, under the real editor Python, loads the real fixture module;
  - it checks that every `unreal` call the recipes make exists in 4.25, by name and by signature where Python can tell;
  - it creates nothing, saves nothing and loads nothing for writing;
  - it writes only its own receipt, then exits.
- **Its argv:** the same as the recipes' (without the recipe), including EU4 and FC4's switches.
- **It must show:**
  - a clean exit, with the probe rows;
  - no change outside FC5's managed classes;
  - no upload;
  - its receipt.
- **A failed smoke ends the run** with no recipe slot used.

### FC4. G2: an analytics opt-out, sibling of EU4, proven from 4.25 source
- **Every argv carries switches** that stop both the editor's AnalyticsET sends and the crash reporter's session-summary send. Codex cites the 4.25 source for each, as it did for EU4.
  - **A candidate, not yet verified:** `[/Script/UnrealEd.AnalyticsPrivacySettings] bSendUsageData=False`, applied as an `-ini:` override.
- **An argv without them refuses.**
- **The smoke session proves them:** no data-router POST in `Runtime.log` or `CrashClient.log`.
- **If 4.25 has no command-line opt-out,** stop NEEDS-DECISION with the options. Never edit a config file.

### FC5. G3: engine- and editor-managed files are recorded, not bound
**These move out of the bound config set. Their changes are recorded, never a STOP:**
- `Saved\Config\CrashReportClient\UE4CC-*`, which the engine purges after two days;
- `Saved\Autosaves\PackageRestoreData.json`;
- the editor's asset-registry caches under the host's `Intermediate`;
- the editor's own logs.

**The bound set is unchanged:** the project's `Config\` folder, the descriptors and the `.uproject`.

### FC6. O3: the editor's own UnrealBuildTool child
- **It belongs to the editor's owned tree.** The kit's process reader records its argv at each poll.
- **It must run no build:**
  - no compiler, linker or UHT child;
  - no change to bound binaries or build outputs.

  Any build action is a STOP.

### FC7. 094-28, tonight: fix, test, re-arm, smoke-test, then the real authoring run, in one job
**In order:**
1. **The fixes, with their tests and can-fails:** FC2 (with the Python 2.7 harness), FC4, FC5 and FC6.
2. **A rehearsal of the changed parts,** with stand-ins on owned scratch.
3. **Re-arm for 09428:**
   - fresh slots for EA_Core and EA_Travel;
   - one smoke session;
   - EW2's scope otherwise;
   - the 09425 and older armings refused.
4. **Dry runs, and one live, observe-only gate reading.**
5. **The real run, under FA5's idle rule.**
   - **The window:** from the end of step 4 until 06:30 IST on 10-30. A session already started may finish.
   - **The order:** the smoke session, then EA_Core, then EA_Travel.
6. **Evidence, per EY7,** with FC5's classes.
7. **A lean exit** (FA6, a milestone, so with the EA8 exit proof).

D2 applies: two attempts per harness problem. If steps 1–4 fail twice, stop NEEDS-DECISION before any real launch.

### FC8. 094-27 waits behind 094-28
- **It is re-queued** to read 094-28's results instead of 094-26's.
- **It carries FC2–FC6 into the runtime kit:**
  - the Python 2.7 harness for any in-editor script;
  - a smoke session before the first real leg;
  - FC4's switches on every argv, `-game` included;
  - FC5's managed classes;
  - FC6's UBT rule.

### FC9. Carried
- **For A7:** EO1's reader budget, EV6's glass mask, EJ5, and FA2's O1 and O2.
- **Codex's canary GAP:** 3 access-denied event logs. They are OS-protected files, and the canary scan of our own new files passed. Accepted.

## For the owner
**Nothing is on screen,** and nothing needs closing from tonight's attempt.

**Before bed:**
- **Close DaVinci Resolve.** It's the one big memory user right now.
- **Keep the Epic Games Launcher open.**
- **Leave the PC and the Claude app on.**
