# 093-12 — Chat ruling: the camera-cut crash is a J2 regression, not a shipped bug; fix it, re-qualify on the new binary with a scoped re-run backed by an identity proof, and measure TSR half-resolution boxes on a proper fixture

**Date:** 2026-10-15, about 23:55 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- Codex's continuation: `report.md` in the J2 run folder. Same thread, one try, 20:42:01–23:08:09 IST, 8,768 s, exit 0, VERDICT NEEDS-DECISION. Evidence `E:\IA_BuildCache\_r9312`.
- Claude Code's review: `_reviews\093-12-code-review.md`. 29 OK · 0 GAP · 1 OPEN (work the stop left unrun); observations A–F.
- Mailbox report 093-12b: NEEDS-DECISION, 161 min, $14.09 API-equivalent.
- **Process note.** Mailbox briefs 093-12 and 093-12R never started: chat put a `%` in the NAME header ("50% proof"). The watcher writes NAME into a `.cmd` line, and cmd's percent expansion stripped everything from the `%` to `D:`, including the stdin redirect, so Claude received no prompt. The same brief ran as 093-12b.

## Verdict
**J2 is still not qualified, and nothing is pushed.**
- Remote `fix/followup-capture` stays at `1bf7054`.
- Local head is `7049614` (docs only); the Z2 freeze is `cdef048`.
- Candidate `eff797c` is unchanged: 48/48 rehash identical across three banks.
- Archive: `followup-capture-J2-tsr-09312-eff797c-57F83797-C6D74FED-UNQUALIFIED`.

**Accepted on review:**
- The Z2 freeze, committed at 20:47:54, before the first fresh leg at 20:57.
- **The TSR half-resolution proof:** parts 1, 2 and 4 pass on all four fresh 50 % legs, both fixtures:
  - every frame rendered at 640×496 inside 1280×992;
  - one size throughout;
  - pairing passes, and every shifted copy rejects.
- Part 3 (box agreement) had **no usable rows** (observation A); see AA4.
- **All pairing at the historic recipe:** 16 runs across 093-11 and 093-12, with PREVIOUS = 0 everywhere and no Lyra spill stop.
- **The controls and the probe:**
  - the exact J1 delayed control writes 0 of 360, with the J1 DLLs loaded;
  - the pre-F1 control shows the wrong view on all 360 frames;
  - both admission probes pass (360/360, refusal counters 0/1).
- Harness fixes within budget, the docs, the restoration, and the no-push.
- **The stop was right:** a product assertion is a D2 stop.

## Rulings

### AA1. The crash: a J2 regression, found before shipping
**What happens, per Codex's diagnosis and Code's independent check:**
1. A camera cut at sample 60 changes the capture owner (generation 2).
2. The ownership reset at `AnomalyCaptureSubsystem.cpp:4731-4733` empties the exposure-exclusion mask but keeps its width, height and pixel count.
3. The cut keeps the picture at 1280×992, so the size check at `:1486` sees no change and skips allocation.
4. `FoldExposureExclusion` at `:1524` then reads the empty array. The result is an assertion and an editor crash.

**Where it came from:**
- `git blame`: that reset block came in with J2's `e13ddc52` (2026-10-14).
- It is absent from the pushed J1 (`1bf7054`) and from the release (`d7eee97`).
- **No delivered build and no client data is affected,** so there is no product call for the owner.

**Why resize passed:** a resize goes through the same reset but changes the size, so it reallocates (observation B). Among the J2 cases, a camera cut is the only owner change that keeps the size. That is exactly what K6 was added to catch.

### AA2. The fix (a product change on `fix/followup-capture`, design note first)
**Design note, committed before any code. It covers:**
- every site that resets or reallocates the exposure-exclusion state;
- every code path the fix can alter (its "reach");
- the identity-proof legs and their comparator (AA3);
- the CPU self-test, if the existing self-test harness can host it.

**The fix:**
1. **One reset, everywhere.** Mask, folded mask, width, height and `ExposureExclusionPx` are reset together through one helper, at every reset site. That also clears the stale pixel count (observation C).
2. **Allocation follows the array, not the cache.** Allocation at `:1486` is keyed on the array's actual element count against width × height, not on a cached size change. An emptied array therefore always reallocates before it is read.

**Scope:**
- Only the exposure-exclusion state in `AnomalyCaptureSubsystem.cpp`, plus its header if the helper needs a declaration.
- Each of these is a STOP: a change anywhere else; an engine edit; a new module dependency; a private header beyond what capture already uses.

**Can-fails:**
- The retained 093-12 crash is the old-binary failure, run with the same leg spec. It is not re-run.
- If the self-test harness can host it, a CPU self-test that changes the owner at the same size and then folds: the old logic must fail it, and the new logic must pass.

### AA3. Re-qualify on the new binary: a scoped re-run with an identity proof
Every gate on the final binary still holds (G201). The PIE re-run is scoped by what the fix can reach, and the rest is proven unchanged:

1. **Builds and audits:** all 8 builds with 0/0, the binary and string audits, a new manifest.
2. **Camera switch (K6), PIE, both fixtures:**
   - the gate: edges 0, every labelled frame with a measured mask, and PF-2 agreement on the moving frames;
   - the readings: drops per switch, and whether each event continued or ended, with the reason.
3. **The fix's reach, in full with the original predicates, both fixtures:** at minimum the delayed viewport and resize, plus any other PIE case the design note lists as reaching a reset site.
   - On resize, the only expected change is `ExposureExclusionPx` in `run_summary.json`, as the note predicts.
4. **Identity proof, PIE, AA-off arbiter, both fixtures.** Re-run the UV regression and the healthy single-view case, and compare each with its `eff797c` session.
   - **Gate:**
     - the PF-3 comparator (same events, targets and edges; identical `visible_positive`; boxes within 0.5 px);
     - masks byte-identical;
     - label rows field-identical;
     - `run_summary.json` identical except the fields the note names.
   - **Reading:** pictures byte-identical (G228 says this holds at the arbiter; a difference is reported, not gated).
   - With these passing, the other completed PIE cases stand on `eff797c`: PF-1 to PF-5, the refusal cases, pairing, alignment, the J1 and pre-F1 controls (archived DLLs) and the admission probes.
5. **TSR half-resolution box agreement (AA4), PIE.**
6. **The cook** on the new candidate. `-nodebuginfo` is accepted, provided the staged binaries rehash equal to the new manifest.
7. **Every staged gate**, both fixtures, all on the new binary. That includes:
   - the staged camera switch;
   - staged pairing (historic recipe; Z2 parts 1, 2 and 4);
   - the four staged PF-2 cases;
   - staged resize (W2);
   - the staged controls.
8. **The push** (fast-forward, `fix/followup-capture` only) when everything is green.

### AA4. TSR half-resolution boxes: measure them on a proper fixture
- **The problem:** Z2 part 3 was vacuous (G146's shape). The pairing target touches the left picture edge, so the frozen PF-2 estimator excludes every row.
- **The fix:** box agreement at reduced resolution moves to dedicated legs. Run the PF-2 arrangement (target well inside the frame) at the historic recipe (TSR) with `r.ScreenPercentage 50`, on both fixtures, PIE and staged.
- **Gate:** the frozen PF-2 estimator reads 1.000 ± 0.010 on each axis, with at least 20 qualifying rows per axis. The 0.96 mutant rejects.
- **If fewer than 20 rows qualify, it is a STOP, not a reading.** A vacuous pass is not allowed again.
- **Pairing legs:** Z2 for them is parts 1, 2 and 4 from now on. Part 3 is retired there, with this ruling as the reason.

### AA5. Stop rules (Z3, plus these)
- **Pairing recipe:** closed. **Marker:** spent. **Packaging metadata:** spent.
- **Camera:** a camera-switch failure on the new binary is a STOP; the fix is not retried.
- **Identity:** an identity-proof difference outside the note's named fields is a STOP.
- **TSR boxes:** a TSR half-resolution leg with fewer than 20 qualifying rows on an axis is a STOP.
- Otherwise D2 is unchanged.

### AA6. Code's observations
- **A:** upheld; AA4.
- **B:** upheld; AA1 and AA3.
- **C:** in the fix (AA2).
- **D:** the change-stage pairs read `extent_mismatch` before the cut on the identity-mode reference. That is a reading. Re-check it on the new binary's camera leg and report it.
- **E (the owner's apps):** PowerToys, WhatsApp, Voice Recorder and some Edge WebView processes were gone by the end of the run. No job command could have closed them. This is for the owner's awareness only.
- **F:** noted.

### AA7. Process
- Mailbox NAME headers carry only letters, digits, spaces and plain punctuation: no `%`, `^`, `&`, `|`, `<`, `>` or `"`.
- Chat confirms INIT in the live feed within about two minutes of dropping any brief.
- **FUTURE:** a small mailbox hardening, so run_brief strips those characters from NAME. It will be done when the mailbox is idle, as its own small brief.

## Next
- **093-13 is queued:** Codex resumes its J2 thread under AA2–AA5, and Code reviews after Codex exits (W3).
- **After J2 qualifies at home:**
  1. office checkpoint I (the second office host is required);
  2. then `research/natural-causes` and J3, carrying 093-00 r3 §1b.
