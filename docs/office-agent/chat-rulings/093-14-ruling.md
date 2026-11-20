# 093-14 — Chat ruling: the camera fix and all 42 PIE cases pass on the new binary; TSR boxes measure inside tolerance; the setup-proof matcher is a reader that cannot pass on its fixture; fix it from the leg's own receipts, route evidence to D:, then cook, staged and push

**Date:** 2026-10-16, about 09:20 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- Codex's continuation: `report.md` in the J2 run folder. Same thread, one try, 03:09:50–07:14:46 IST, 14,696 s, exit 0, VERDICT NEEDS-DECISION. Evidence `E:\IA_BuildCache\_r9314`.
- Claude Code's review: `_reviews\093-14-code-review.md`. 30 OK · 0 GAP · 4 OPEN (work the stop left unrun); observations A–F.
- Mailbox report 093-14: NEEDS-DECISION, $18.75 API-equivalent.

## Verdict
**J2 is still not qualified, and nothing is pushed.**
- Remote `fix/followup-capture` stays at `1bf7054`. Local head is `0e50b56` (docs only).
- Product is `9781031`. StackOBot EXE `d8d30951`, Lyra EXE `dfe6f164`; fixture DLLs StackOBot `a7443041`, Lyra `017ca90b`.
- The complete archive is on physical D:: `D:\IA_BankOverflow\followup-capture-J2-donor-09314-9781031-D8D30951-DFE6F164-OVERFLOW-UNQUALIFIED`.

**Accepted on review, for `9781031`:**
- **The AB1 donor repair:** frozen at 03:14:27, before the re-assessment (03:15) and the first leg (03:23).
  - StackOBot's re-assessed stale controls reject (cut donor 56: 0.921/1.078; blend donor 296: 1.045/0.961).
  - The original "control unavailable" result is kept.
- **Lyra camera, PIE: PASS.**
  - 990 requested = 984 written + 6 named drops.
  - Every labelled row has a measured mask.
  - Moving-frame PF-2: X 1.000098, Y 0.999884, n = 44.
  - Edges 0 on all four events.
  - Both stale controls reject, and both active events continued through their switch.
- **All 42 fresh PIE ledger cases pass on `9781031`.** All 64 legs loaded exactly the candidate DLLs. That covers:
  - PF-1 to PF-5;
  - ownership, refusal and regression cases;
  - resize under W2;
  - both alignments on both fixtures;
  - all eight pairing legs (historic recipe; Z2 parts 1, 2 and 4; PREVIOUS = NEITHER = 0; every shifted copy rejects).
  - The four archived-DLL controls stand. The ledger reads 46 PIE PASS and 38 staged not yet run.
- **TSR half-resolution box agreement is measured for the first time and passes** (StackOBot PIE): X 0.99611, Y 0.99448, n = 96 per axis, with the 0.96 mutants rejecting. All 294 mask passes rendered at 640×360 inside 1280×720.
- **The office reader on new sessions:**
  - `I1.11 PASS v=4 c=0 b=999 #9` (StackOBot);
  - `I1.11 PASS v=4 c=1 b=999 #9` (Lyra).
  - Both are readings until J2 qualifies.
- **The stop was right:** the setup proof failed, and repairing its reader was outside AB1.

## Rulings

### AC1. The TSR setup proof: a matcher that cannot pass on its fixture
- **The assumption:** `assess_z2.reduced` maps each written row to request id `(1<<61)|(si+1)`. That holds only when a target mask is armed on every sample, as on the pairing fixture.
- **The fixture:** on the PF-2 arrangement, target masks are armed only on labelled samples and numbered by their own serial (serial 1 is sample 29).
- **The result:** the matcher can never pass there. It is the AB1 shape again (Code, observation A).
- **The product evidence is clean:**
  - every mask pass is half-size;
  - the `MASK-SERVED` receipts tie each of the 96 labelled rows to a half-size pass.

**The repair.** This is harness problem "TSR setup matcher", attempt 1 of 2.
- **Mapping:** requests map to samples through the leg's own `MASK-SERVED` receipts, not by arithmetic.
- **Predicate:** every labelled row the PF-2 estimator uses is served by a mask pass whose internal rect is half the output rect on each axis, and every mask pass in the leg is half-size.
- **Can-fails:**
  - a copy of the receipts with one served pass changed to full size rejects;
  - the historic 100 % leg's passes fail the predicate.
- **Unchanged:** the estimator, n ≥ 20 per axis, the 1 % limits and the 0.96 mutants.
- **Order:** freeze the rule, then re-assess the retained StackOBot TSR leg with it (no engine re-run). The original failed outcome stays on record.

### AC2. Disk: write new evidence to physical D:
- **The problem:** `_binary_baselines` and `_bench_sessions_bank` are junctions onto E:. E: fell to 51.6 GB free against the 50 GiB floor.
- **The rule:** all new captures, evidence and archives in this job go to physical D: paths. Free space is checked on the **resolved** volume, not the junction. The 50 GiB floor applies to every volume written, including E: during the cook. A breach is a STOP, as before.
- **The partial archive:** the first, incomplete archive attempt (about 21.4 GB, marked `INCOMPLETE-ATTEMPT.txt`) stays where it is; nobody here deletes anything. The owner may delete it himself to free E: (AC6).

### AC3. Code's observations
- **A:** upheld; AC1. Code's own 093-13 miss is noted, and the review now tests readers against a sparse-mask fixture.
- **B:** upheld; AC2.
- **C:** TSR half-resolution scale reads 0.9961 / 0.9945, inside ±0.010 but further from 1.000 than full resolution. The Y margin is 0.0045.
  - This is a reading.
  - Open item for after J2: check for a half-pixel convention offset at reduced internal resolution.
- **D:** Lyra repeats the 16 `extent_mismatch` pairs; already an open item (AB3-D).
- **E, F:** noted.

### AC4. Order on `9781031`
1. **Pre-flight:** candidate rehash; restoration state; E: and D: free space on the resolved volumes.
2. **Freeze and commit** AC1 and AC2.
3. **Re-assess the retained StackOBot TSR leg** with the AC1 matcher. The setup proof must pass, and the box result stands as measured.
4. **Lyra TSR half-resolution PF-2, PIE:** box agreement and the AC1 setup proof, with at least 20 rows per axis.
5. **The cook.** `-nodebuginfo` is accepted; the staged binaries must rehash equal to the manifest.
6. **Every staged gate, both fixtures:**
   - the staged camera switch with the drop-aware control;
   - staged pairing (Z2 parts 1, 2 and 4);
   - the four staged PF-2 cases;
   - staged TSR half-resolution with the AC1 proof;
   - staged resize (W2);
   - the staged controls and probes.
7. **The push** (fast-forward, `fix/followup-capture` only) when everything is green.

### AC5. Stop rules
- **Carried from AB5:**
  - pairing recipe closed; marker spent; packaging metadata spent;
  - camera donor at most 2 attempts (1 used);
  - a control that cannot be built is a STOP;
  - a camera-switch product failure is a STOP;
  - TSR with fewer than 20 rows is a STOP.
- **New:**
  - **TSR setup matcher:** at most 2 attempts.
  - **Disk:** the 50 GiB floor applies on every resolved volume written.
- Otherwise D2 is unchanged.

### AC6. For the owner (optional, recommended before the cook)
Deleting the incomplete archive copy frees about 21 GB on E: and makes a disk-floor stop during the cook less likely. The folder is:
`D:\IntrusiveAnomalies\_binary_baselines\followup-capture-J2-donor-09314-9781031-D8D30951-DFE6F164-UNQUALIFIED`
- It is physically on E:.
- Check that it contains `INCOMPLETE-ATTEMPT.txt` first.
- The complete copy is the `…-OVERFLOW-UNQUALIFIED` folder on D:, so this incomplete copy is safe to delete.
- It's the owner's call. Nobody here deletes it.

## Next
- **093-15 is queued:** Codex resumes its J2 thread under AC1–AC5, and Code reviews after Codex exits (W3).
- **After J2 qualifies at home:**
  1. office checkpoint I;
  2. then `research/natural-causes` and J3 (093-00 r3 §1b);
  3. the open items: AB3-D change evidence under camera motion, AC3-C the TSR half-pixel reading, and AA7 the mailbox NAME hardening.
