# 094-26c — Chat ruling (pre-ruled from the live evidence at 05:50 IST): the 4.25 editor authored EA_Core, but our managed-file rule stopped on an editor bookkeeping file; 094-26d widens the rule and re-authors both maps

**Date:** 2026-10-30, about 05:55 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Live evidence:** Codex's messages and final summary for 094-26c, and Claude Code's midjob notes. Code's review was still being written. 094-26d reads it, and stops at entry if it shows a protected-state change, a crash or an upload.
- **The facts:**
  - FE1 and FE2 were implemented and committed. The 09426c kit was re-armed without dry runs.
  - **The smoke session passed 51 of 51 bindings.**
  - **EA_Core authored its two packages** (`SM_EA_TwoLod.uasset`, `EA_Core.umap`), and the editor exited normally (code 0).
  - **No crash, no upload, no build.**
  - Unreal also wrote `Saved\Config\WorldState\1202994220.json` (50 B), from `FActorFolders::OnWorldSaved`, and a `Saved\Backup` copy of the map.
  - The kit treated the WorldState file as a config change: it stopped, quarantined both packages by receipt, and refused EA_Travel.
  - Existing Content, the project's `Config\`, the build outputs and marker 31124 are unchanged.
  - The docs commit is `aecff71`.

## Verdict
**The authoring works on 4.25.** The editor built EA_Core from our recipe and closed cleanly. That is the first real authored output of the port.

**The STOP is mine, again:** FC5 listed only the editor-managed files we had already seen. The editor's normal save also writes world-state and backup files.

## Rulings

### FF1. Accepted as evidence
- EA_Core's run proves the launcher, the recipe and the save path on 4.25.
- The two quarantined packages stay in quarantine as evidence.
- 094-26d re-authors both recipes fresh, so A6's outputs come from one clean, accepted run.

### FF2. The editor's `Saved\` folder is editor-managed, except its ini files (widens FC5)
- **Recorded, never a STOP:** everything the editor writes under the host's `Saved\`, including:
  - `Saved\Config\WorldState\*.json`;
  - `Saved\Backup\*`;
  - `Saved\Autosaves\*`;
  - `Saved\Logs\*`;
  - the crash-client configs.
- **Still a STOP:**
  - a change to any `*.ini` under `Saved\Config\` (`-nowrite` should prevent it);
  - any change to the project's `Config\` folder, the descriptors or the `.uproject`.
- **Unchanged:**
  - Content outside the declared outputs stays protected;
  - FC5's `Intermediate\` caches stay recorded.

### FF3. No smoke session in 094-26d
- The recipe and the launcher are unchanged since the 51-of-51 smoke. FC2–FC4 are proven in the real editor.
- 094-26d's change is only in the kit's post-session classification, covered by its tests.

### FF4. 094-26d: widen the rule, re-arm, then EA_Core and EA_Travel when the PC is idle
**In order:**
1. FF2, with its tests and can-fails:
   - a WorldState json, a Backup map and an Autosave are recorded;
   - an ini under `Saved\Config\` and a file in the project's `Config\` stop.
2. **Re-arm for 09426d:**
   - fresh slots for EA_Core and EA_Travel;
   - EW2's scope otherwise;
   - 09426c and older refused;
   - no dry runs.
3. One live, observe-only gate reading.
4. **The real run, under FA5's idle rule:**
   - **the window:** until 06:30 IST on 10-31, which covers today's idle time and tonight;
   - **the order:** EA_Core, then EA_Travel.
5. **Evidence** per EY7, with FF2's classes. A lean exit (FA6, a milestone).

### FF5. For 094-27: one preflight per run, not per leg
- Each session's preflight re-hashes about 104 GB, which takes about 15 minutes. With many legs, that cost dominates.
- 094-27's kit runs the full protected and Content preflight once per run, before its first leg. Between legs it runs only the fast checks: Content hashes, the bound files and routing. The post-run checks stay in full.
- FF2's `Saved\` class applies to its editor and game legs.

## For the owner
- **Nothing to do now.**
- **The test maps get built** the next time your PC is idle for 10 minutes: today if you're out, otherwise tonight.
- **If you see an Unreal window open,** leave it alone. It closes itself in about a minute.
