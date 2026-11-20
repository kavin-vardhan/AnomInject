# 094-14 — Chat ruling: the log reader gets two more tries, proven on saved logs first, with a safe fallback; the makefile refresh becomes standard; finish A5

**Date:** 2026-10-25, about 22:45 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Codex's report:** `_reviews\094-14-codex-report.md` (20:50–21:49 IST, 3,520 s, exit 0). Its verdict: `NEEDS-DECISION | A5 incomplete: D2 action-log reader exhausted; 4.25 Editor native build passed; five builds pending`.
- **Claude Code's review:** `_reviews\094-14-code-review.md`. 35 OK · 6 GAP (all from the stop) · 3 OPEN · 0 stop violations · 0 conflicts. It asks for decisions D1–D3.

## Verdict
**The stop was correct.** Codex's own new action-log reader failed twice, which spent its D2 budget.
- Code reproduced the rejection read-only (`UE4Editor-EuropaPhaseAProbe.lib`).
- The cause is real in 4.25 `ParallelExecutor.cs` (`:207–213`, `:262–276`). The status line depends on `bShouldOutputStatusDescription`, and the reader handled only one of its two states.

**Accepted:**
- **EN1 holds on the regenerated 4.25 Editor makefile,** checked by Code's own proven reader:
  - the three selected toolchain folders come first, and the base follows byte-identical;
  - UBT added only `VC_COMPILER_PATH` and `VC_COMPILER_DIR`, both inside the selected MSVC folder;
  - 0 blocked names, and no canary.
- **The first 4.25 probe Editor build:** exit 0, with 0 C++ errors and 0 C++ warnings. The DLL, PDB and `.lib` are back, and Code's recomputed hashes match.
- **The three probe compatibility fixes:**
  - the JSON print-policy include;
  - `Num()` in place of `IsEmpty`;
  - explicit `TPair` construction.

  Each was one fix, made identically in all three hosts, with no production path touched.
- **The commits** `290a7d2`, `35eec14` and `cd5b9ad`: local only, with W4 correct.
- **The two UBT warnings from the user `BuildConfiguration.xml`** (lines 16 and 22) are the same lines accepted in A4a. They stay unchanged, as required.

**Protected state:**
- P9 v2 18,264 of 18,264;
- the protected-state check PASS ×3;
- `m51` equal to P9 ×4;
- every engine tree unchanged;
- the canary found 0 times in 422 files;
- 0 launches.

## Rulings

### EO1. The action-log reader gets a fresh D2 budget of two attempts, proven offline before any build (Code's D1)
**The fix:** accept the status lines UBT documents, in both states of `bShouldOutputStatusDescription`, and for metadata actions such as the target receipt.
- Cite 4.25 `ParallelExecutor.cs:207–213` and `:262–276`.
- Before the first 5.1 build, read 5.1's executor and cite its lines too.

**Prove it before any new build, with no action-plan query (EN2):**
- **Known answers:** both retained 094-14 4.25 console logs.
  - The exit-6 run: 3 project compiles executed, 2 of them failing.
  - The exit-0 run: 5 executed lines (2 compiles, the `.lib`, the `.dll` and the `.target`).
- **5.1 format coverage:** if a 5.1 build log is retained, every status line in it parses. The 5.1 graph join is then first proven on the first 5.1 build.
- **Can-fails.** Each of these must fail:
  - an executed line whose output is under an engine tree;
  - an executed line that matches no graph action;
  - an executed line that matches more than one graph action.

**The reader stays fail-closed.** One attempt is one change to the reader plus its validation run.

**Then complete the 4.25 Editor's action evidence** by running the proven reader on the retained exit-0 log. No rebuild is needed.

### EO2. A pre-approved fallback, so the reader cannot stop A5 a third time
**This applies only if EO1's two attempts are spent too.** Then don't stop for the reader. For each remaining build, show EI3's STOP condition is clear with all of:
- `-NoEngineChanges` in the argv, and the native exit;
- every engine-tree inventory unchanged outside EG1, before vs after;
- every changed file lying inside the owned host trees, from the ledger's before/after;
- a recorded manual reading of the log's status lines, as Codex did for the exit-0 run.

**Label this "EI3 fallback (EO2)"** in each receipt it covers.
- The "engine-source actions in host" count may then read "not available".
- Carry the reader as an OPEN.

**No hard STOP changes.** An engine-tree write is still a STOP, however it is found.

### EO3. The makefile refresh is a standing, receipted step (Code's D2; ratifies the EN5 item)
**Before every native UBT run in an owned host, meaning each build and the EI4 query:**
- remove only that target's own `Makefile.bin` under the host's `Intermediate\Build`;
- take a size, mtime and SHA-256 receipt before removing it;
- record the removal in that run's receipt.

**Why:** stock UBT copies a valid cached makefile's saved environment back into its own process (4.25 `BuildMode.cs:530–534`; 5.1 `:788–792`). Without the refresh, each run replays the previous run's environment, including its TEMP and TMP routing, so the EN1 proof would compare against stale values.

**Never remove:**
- an engine-tree makefile;
- an engine-plugin makefile;
- anything under a P9-protected path.

### EO4. One probe source for all hosts
**The 5.1 builds are the probe's first 5.1 compiles,** on ReGate51 and against `d7eee97` on Baseline51.
- **If 5.1 needs a change,** keep a single probe source. Put any engine difference behind an `ENGINE_MAJOR_VERSION` check inside the probe only.
- **The same-error-twice rule applies per host.**
- **No edits to production** `Source`, `Content`, descriptor, `Build.cs` or `.uproject`.

### EO5. The closeout uses the individual, accepted tools
**The combined closeout script stays abandoned;** its D2 is spent. Close out with:
- the EA8 exit tool;
- EA11 on the plugin's seven reflected headers. The probe has no reflected headers, so record EA11 for it as "not applicable";
- the canary scan, **starting at the launch second** (Code's O4), and the EK1 scan;
- the ledger summary.

### EO6. Repair the stale self-test (Code's D3)
**`resume_self_tests.py` still expects raw values,** but EL1 now masks them.
- Repair it so it expects the masked form.
- Add a can-fail: a raw value must now fail it.
- The repair gets one D2 budget.

## Next
**094-15 is queued: A5's finish,** under EO1–EO6. The order:
1. EO6;
2. EO1, proven offline;
3. the five remaining builds: 4.25 Game, then Baseline51 Editor and Game, then ReGate51 Editor and Game. Each build gets EO3, EI2, EG2 and its EN1 makefile proof;
4. EI4, after the Baseline51 rebuild;
5. re-binding the three manifests;
6. the owner kit, dry-run only;
7. the exit, per EO5.

**The A6 owner card must also say:**
- answer "No" to any "rebuild modules?" prompt;
- close Blender and any other heavy jobs before double-clicking. The preflight needs at least 6 GiB of free physical memory, and this job saw 5.86 GiB with Blender open.
