# 094-22 — Chat ruling: the authoring kit's design is accepted; fix the launcher gate and the in-session rules, then re-arm

**Date:** 2026-10-28, about 08:40 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Codex's report:** `_reviews\094-22-codex-report.md` (04:53–07:29 IST, 9,374 s, exit 0). Its verdict: `GREEN | EW2 fixture-authoring kit armed and dry-run; bindings, P9, scans and gates pass; zero Unreal launches`.
- **Claude Code's review:** `_reviews\094-22-code-review.md`. 42 OK · 6 N/A · 1 GAP (G4, blocking) · 1 decision · 2 OPEN (carried) · 0 STOP.

## Verdict
**The design is accepted.** Every 4.25 citation was verified by Code.

**The mechanism:**
- the ordinary `UE4Editor.exe` opens the Paris project;
- `-EnablePlugins=PythonScriptPlugin,EditorScriptingUtilities` turns the plugins on for that run only (`PluginManager.cpp:567–603`);
- `-ExecutePythonScript` runs one committed recipe (`EditorPythonExecuter.cpp:139–222`);
- `-nowrite` blocks config writes, and `-SkipCompile` stops the rebuild prompt;
- the editor closes itself on the next tick.

**No `.uproject` or config file changes.**

**The outputs, by exact path, with nothing ever overwritten:**
- `Content\EuropaPhaseA\SM_EA_TwoLod.uasset` and `EA_Core.umap` (EA_Core);
- `EA_Travel.umap` (EA_Travel).

All 1,458 existing Content files are re-hashed before and after.

**The limit:** 1,800 s per session, after which only our own editor tree is stopped, crash reporter included.

**The dry runs:**
- both recipes passed, with 0 launches;
- the bound files, the manifests and Paris Content are unchanged.

**Protected state:**
- P9 v2 18,264 of 18,264;
- the canary found 0 times;
- EK1 found 0 secret values;
- the command audit: 204 commands through the wrapper, 1 direct read-only, 0 direct write-capable.

**But the real run could never start (Code's G4).** The launcher's own overlay processes (`EOSOverlayRenderer-Win64-Shipping.exe`, children of `EpicGamesLauncher.exe`) count as "another Unreal or game program". The dry runs missed it because they use synthetic readings.

## Rulings

### EX1. The launcher's whole process tree is excluded (fixes G4)
**The foreign-Unreal check excludes every descendant of the running `EpicGamesLauncher.exe`.**
- It is judged by process ancestry, re-read at each poll.
- The excluded processes are recorded.
- Any Unreal or game process outside that tree still blocks.

**The dry run adds a live, observe-only gate reading,** using the real collector on the real processes. It must show:
- the launcher tree excluded;
- no other blocker misread.

### EX2. The in-session rules for the authoring leg (Code's D2)
**The start gate is unchanged:** the full measured profile.

**During the session, only three things act:**
- **a disk floor breach:** stop our editor tree. There is no pause in mid-session;
- **free commit below 6 GiB:** stop our editor tree (EI2's in-run stop);
- **the 1,800 s limit.**

**Recorded, but not acting:** physical memory, GPU, human input and foreign processes.
- **Why:** our own editor uses memory and GPU, and authoring is not a measurement.
- **Measured legs (A7) get their own in-session rules** when they are designed.

**One session per recipe, with no automatic retry,** takes precedence over EA6's rerun for this leg.

### EX3. Saves and partial outputs (Code's D3)
- **Repeated saves:** a session may save its declared packages more than once. This ratifies Codex's EN5 reading.
- **A failed session's partial outputs:**
  - only those at the declared paths;
  - are moved by receipt into a quarantine folder under that attempt's receipts;
  - before any retry.
- **A retry still needs chat's re-arm.**
- **Nothing outside the declared paths may change.**

### EX4. `-unattended` stays off (EW3's fallback, accepted)
- The Python runner's unattended-script mode already applies the default answers (`EditorPythonExecuter.cpp:145`).
- The only 4.25 prompts that default to Yes are import or overwrite prompts, and the recipes never reach them.

### EX5. Small items
- **O6:** fix the live config's job-template wording, so armed slots say "armed for EW2", not "preparation and dry-run only".
- **O2b:** the four extra code shapes are recorded. None is in the probe, so no change is needed.
- **O4:** the entry point strips inherited `EUROPA_*` variables from helper children. Keep it.
- **O7:** accepted. The ET2 class covers stock marker removal.

### EX6. Next
- **094-23:** EX1, EX2, EX3 and EX5, then re-arm, then the live gate reading. Dry-run only.
- **The real run (094-24)** follows in the next free overnight window, if the owner agrees to unattended overnight runs.

**For the owner, now (the watcher is idle):**
1. restart the PC;
2. double-click `start_watcher.bat`;
3. start the Epic Games Launcher normally, and leave it running;
4. tell chat when that's done, and 094-23 starts.
