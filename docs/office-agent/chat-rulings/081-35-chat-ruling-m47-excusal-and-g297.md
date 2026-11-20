# 081-35 — Chat ruling on 081-34 NEEDS-DECISION: AMENDMENT 6 (m47 shader-readiness keys excused under their own log signature, capped; G297 self-excused singletons made visible with a recurrence tripwire); CB-S re-evaluated from the bank

Date: 2026-09-26 (IST) · Author: chat Claude (orchestrator) · Build under test: 3 (`85c99b3`, Game 002805CF) · Branch head `64bdb64`

## Background
`CB_S2` (build 3, evidence **OFF**) carried m47's conditional shader-readiness keys on all 90 frames. Its own log shows the cause: `PREWARM shaders materials=2 incomplete=1 … pendingBefore=0 pendingAfter=0`, then `SHADERS pending=0 incomplete=1` on every frame. Every mask, label, pixel count, bbox and event matches pre-m55 `CB_S1`. The other 11 legacy legs read 0. The bank shows one earlier non-zero, a Lyra smoke run, in 972 sessions.

`git diff 031a103 85c99b3 -- Source` touches no m47 line, and the leg ran with evidence OFF. So this is pre-m55 behaviour, and the key-presence rule fired correctly on a conditional diagnostic.

## Ruling 1: the m47 keys stay in scope, excused only under their own log signature, symmetric and capped (same shape as ruling A)
A leg's m47 keys are **M47-EXCUSED** only if **all** of the following hold.
1. **Signature.** The leg's own log has `Capture(m47): PREWARM shaders materials=M incomplete=N …` with N ≥ 1.
2. **Closed key set.** The only differing keys and values are m47's conditional emissions. Enumerate them from the m47 writer source and freeze the list in the amendment. Expected: per-row `render_state`, `shader_jobs_pending`, `anomaly_materials_incomplete`; run_summary `frames_shaders_pending`, `shader_prewarm_incomplete`. Any key outside the frozen list → the ordinary rules, so FAIL.
3. **Internal consistency.**
   - `shader_prewarm_incomplete` equals the log's N;
   - every row's `anomaly_materials_incomplete` equals that row's `SHADERS … incomplete=` log value;
   - `frames_shaders_pending` equals the number of rows with `render_state="shaders_pending"`.

   Any mismatch → FAIL.
4. **Everything else passes the existing rules unchanged**, including AMENDMENT 5 as released.
5. **Symmetric** across binaries.
6. **Cap.**
   - At most 1 M47-EXCUSED leg per group.
   - Across the rest of the requalification, a **second** build-3 leg that needs the excusal, while pre-m55 legs show none, is NEEDS-DECISION. The bank prior is about 1 in 972, so recurrence would be a signal.
7. **Reported** as `m47_excused: [{leg, N, frames_marked, prewarm_line}]`.

**Pre-existing product finding (not m55; recorded, FUTURE for m51/M3).**
- m47's log claims the incomplete count is "STRUCTURALLY ZERO in a packaged build". This measurement disproves that.
- The run-wide marker labelled all 90 frames `shaders_pending`, although no shader job was pending and the active anomaly (`blinking`) swaps no material. So its pixels could not have been affected.
- For a client, that is a false-positive quality flag that could make them discard good frames.
- FUTURE fixes:
  - tie the frame marker to the materials that active anomalies actually use on that frame;
  - log which material is incomplete;
  - correct the "structurally zero" wording.

**Mechanism:** NOT ESTABLISHED. Record it as a gotcha.

## Ruling 2: G297 self-excused singletons become a reported reading with a recurrence tripwire, not a new gate
The cross clause learns variance from control pairs, so one deviating leg can excuse itself through its own pair.

Against a **systematic** m55 effect the gate's power is intact: that would show on both legs of a pair and fail. A one-leg deviation cannot be told apart from noise with n = 2 per binary. Tightening the gate to fail on singletons would false-fail genuine noise about 2/3 of the time. So the ruling is to make the blind spot **visible**, not to gate on it:

1. **Reading.** For every legacy group (native and synth order), list each **self-excused singleton**: a field where exactly one leg differs from the other five, and that difference is excused only through that leg's own control pair. Give the field, the leg, its binary and evidence state, the values, and whether the field is in the historical run-unique set. Write the list to the comparison JSON and the report.
2. **Tripwire.** Across all legacy groups, banked and remaining: if a field **not** in the historical run-unique set has self-excused singletons on **build-3** legs in **≥ 2 groups** and **never** on pre-m55 legs → NEEDS-DECISION.
3. **Bank pre-check (bench-free, before anything else).** Compute the reading over every banked legacy comparison (081-11/12/15/22, c0, CB-N, CB-S). If the tripwire already fires, stop there with NEEDS-DECISION and the list.

## Proof required, both directions (bench-free)
1. **M47 negatives:**
   - keys present with no PREWARM line → FAIL;
   - the log's N differs from `shader_prewarm_incomplete` → FAIL;
   - `frames_shaders_pending` differs from the number of marked rows → FAIL;
   - a row's `anomaly_materials_incomplete` differs from its log line → FAIL;
   - m47 keys plus one non-m47 difference → FAIL;
   - a key outside the frozen list → FAIL;
   - two M47 legs in one group → NEEDS-DECISION;
   - a second build-3 M47 leg across the campaign with pre-m55 at none → NEEDS-DECISION.
2. **M47 positive:** the banked `CB_S2` → M47-EXCUSED, with the lines quoted.
3. **Singleton reading.**
   - A synthetic singleton is listed.
   - The tripwire fires on the ≥ 2-groups, build-3-only case.
   - It does not fire when pre-m55 legs show the same field, when the field is in the run-unique set, or when there is only one group.
   - Report the real reading over the bank.
4. **No flips.** Re-evaluate every earlier legacy comparison: verdicts unchanged apart from the intended CB-S FAIL → PASS, and readings unchanged on existing keys. An earlier PASS turning FAIL is a stop.
5. **CB-S re-evaluation from the bank** (081-26/33 mechanism, dated, citing this ruling) → **PASS-REEVALUATED**. If it does not come out that way, stop and report; do not bend it.
6. **Boundary re-issue** and a full dry run, all green. c1 resumes at `MAINON_N1`; give the exact invocation.

## Sequencing
- **081-35**, bench-free, xhigh:
  1. bank pre-check;
  2. declare AMENDMENT 6 (legacy groups only; no stack predicate, threshold or prediction changed) and implement it;
  3. proofs;
  4. CB-S commit;
  5. docs: gotchas for the m47 finding and G297 (now visible);
  6. commit docs **before** the boundary re-issue, then re-issue the boundary;
  7. push.
- **081-36**, bench, high: only if 081-35 is GREEN. Runs c1 from `MAINON_N1` (MAIN-N, MAIN-S), then c2, then c3.
