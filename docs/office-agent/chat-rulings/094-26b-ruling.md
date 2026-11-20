# 094-26b — Chat ruling: the fixes held and the real-editor smoke worked, catching a 4.25 recipe gap before any recipe slot was spent; 094-26c fixes the recipe and runs the authoring before morning

**Date:** 2026-10-30, about 04:40 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Codex's report:** `_reviews\094-26b-codex-report.md` (03:27–04:15 IST, 2,852 s, 1 try, exit 0). Its verdict: `NEEDS-DECISION | FD2/FD3 passed; Smoke found two unavailable 4.25 properties, so both recipes remain unstarted; four canary-read gaps remain.`
- **Claude Code's review:** `_reviews\094-26b-code-review.md`. 44 rows: 38 OK · 2 N/A · 1 Note · 1 Carried · 2 OPEN. No FAIL.
- **Mailbox report 094-26b:** NEEDS-DECISION, 65 min.

## Verdict
**ACCEPTED: the smoke session did exactly its job.**
- **FD2 held in the real editor:** the editor's own `ValidatePlatforms` child was let through and recorded on 90 polls. No compiler, linker, UHT or ShaderCompileWorker ran.
- **FD3 held:** the gate cleared on its third poll, at 03:53.
- **The smoke session ran 23 s and exited normally:**
  - one cleanup row and exactly one `probe_stop`;
  - no log errors;
  - no upload of any kind. **FC4 is proven in the real editor.**
- **FC2 is proven in the real editor:** the launcher loaded the fixture module, and 51 of the recipe's 53 bindings exist in 4.25.
- **The 2 that don't:** `override_anti_aliasing_method` and `anti_aliasing_method`. In 4.25, the anti-aliasing method is a renderer setting, not a post-process setting. Code's 094-28 review had predicted this from source.
- **No recipe slot was used, and no package was created.** Protected state, Content, `Config`, the descriptor and the `.uproject` are unchanged.

## Rulings

### FE1. The recipe drops its anti-aliasing item on 4.25
- **The change:** the fixture recipe's post-process item no longer sets `override_anti_aliasing_method` or `anti_aliasing_method`. Nothing else in the recipe changes.
- **AA off for the pictures:** set per session, at capture time, in A7 (094-27), by a command-line override with no config edit, proven from 4.25 source. For example: the renderer's default anti-aliasing setting through `-ini:`, or `r.PostProcessAAQuality` through `-ExecCmds`. 094-27's design note chooses and cites it.
- **Re-bind:**
  - re-bind the recipe's hash and its A5 manifest;
  - commit a dated prediction addendum;
  - run the Python 2.7 harness on the changed script.

### FE2. The canary-read gaps: OS-protected Windows event logs are excluded as a class
- **The rule:** event logs the OS denies us read access to are excluded from the canary scan as a class. They live in Windows' own protected log folders, and we never write there.
- **Recorded each time:** the count, and the folder.
- **This replaces FC9's exact-path acceptance,** which reopened every run.

### FE3. The guard's remaining gaps go into 094-27's kit (Code's observations)
**These must STOP in 094-27's kit:**
- an unreadable UnrealBuildTool command line;
- `-TargetList=` with no `-Mode` (a build in 4.25);
- `Clean`, `Execute` or `Deploy` modes.

None occurred, and the editor doesn't issue them.

### FE4. 094-26c, now: fix the recipe, re-arm, then smoke, EA_Core and EA_Travel before morning
**In order:**
1. FE1, with the harness and the re-binding.
2. **Re-arm for 09426c:**
   - fresh slots for the smoke session, EA_Core and EA_Travel;
   - EW2's scope otherwise;
   - 09426b and older refused;
   - no dry runs (FD5's reasoning still holds).
3. One live, observe-only gate reading.
4. **The real run, under FA5's idle rule:**
   - **the window:** until 07:45 IST on 10-30, and a session already started may finish;
   - **the order:** the smoke session, then EA_Core, then EA_Travel.
5. **Evidence** per EY7 and FC3–FC5. A lean exit (FA6, a milestone).

### FE5. 094-27 waits behind 094-26c
- It reads 094-26c's results.
- FE1's anti-aliasing override and FE3's guard gaps are in its scope.

## For the owner
**Nothing to do.** The run happens before you wake up. If an Unreal window is open when you get up, leave it alone.
