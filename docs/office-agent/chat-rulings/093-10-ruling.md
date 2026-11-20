# 093-10 — Chat ruling: the bench recovered and 32 J2 cases pass; the pairing stop is the probe being unlit under the arbiter recipe, with no pairing slip in evidence; prove it with a one-factor control, then finish J2

**Date:** 2026-10-15, about 11:55 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- Codex's continuation: `report.md` in the J2 run folder. Same thread, one try, 07:17–09:59 IST, 9,746 s, exit 0, VERDICT NEEDS-DECISION.
- Claude Code's review: `_reviews\093-10-code-review.md`.
  - Part A (the 093-09 portion): 11 OK · 0 GAP.
  - Part B (093-10): 17 OK · 1 GAP · 2 OPEN (the work the stop left unrun).
  - Observations A–E.
- Mailbox report 093-10: NEEDS-DECISION, 175 min, $18.96 API-equivalent.
- Chat's own look at Code's overlay (`code-receipts-093-10\pairing-si68-69-mask-outline.png`, zoomed) and `pairing-hue-diagnostic.txt`.
- The pairing instrument's history: `docs/predictions/2026-09-06-m51-pairing-instrument-diagnosis.md`, and G228 (the AA-off arbiter).

## Verdict
**J2 is still not qualified, and nothing is pushed.**
- Remote `fix/followup-capture` stays at `1bf7054`. Local head `7d5e5a4` (docs only).
- Compiled candidate `eff797c` (StackOBot EXE `57F83797`, Lyra EXE `C6D74FED`) rehashed byte-identical after the crash.
- Archive: `followup-capture-J2-recovery-09310-eff797c-57F83797-C6D74FED-UNQUALIFIED`.

**Accepted on review, for candidate `eff797c`:**
- The crash recovery and the 093-09 portion. X1's three conditions are met: the binaries are identical, Code confirms the gates, and the repairs were harness-only with thresholds unchanged.
- PF-1; PF-2 12/12; the 0.813 can-fail.
- PF-3 at 0.0 px on both legs; PF-4; PF-5 with an observed can-fail (cache 90 against the locked 70).
- 32 J2 cases PASS across both fixtures, including resize under W2 (360 = 358 + 2 named drops) and every refusal case.
- The restoration, and the no-push.

**The stop was right** under D2: a frozen gate failed its prediction, and no harness defect was proven. Codex didn't relax the tolerance and didn't retry. Code's diagnosis is upheld as a reading (Y1).

## Rulings

### Y1. The pairing stop: the probe can't be seen under the arbiter lighting; there is no pairing slip in evidence
**What the reader assumes.** The pairing reader (the m44 probe):
- finds the probe's bright-magenta pixels;
- takes their centroid;
- asks whether it sits on this frame's mask (CURRENT) or the previous frame's (PREVIOUS).

That works only if the probe shows magenta over its silhouette.

**What happened.** This was the first pairing leg ever run at the AA-off arbiter recipe:
- `r.AntiAliasingMethod 0`;
- `r.Lumen.DiffuseIndirect.Allow 0`;
- `r.DynamicGlobalIlluminationMethod 0`;
- `r.ReflectionMethod 0`.

Every earlier pairing leg ran at the bench's normal lighting. What that did to the probe:
- With no indirect light, the probe cube's camera-facing face renders **black against a black sky**.
- Only its inner side face, in direct sun, is bright magenta: about 150–2,070 of the mask's 40,938 px, plus bloom.
- So the reader's centroid lands 70–80 px toward the screen centre from the mask centre. That gives NEITHER on all 56 decidable frames.

**Why there is no pairing slip in the evidence:**
- In the overlay, the mask outline sits on the cube **at its current position** in both frames.
- The magenta is always on the current side. It never appears at the previous position, which is about 566 px away. A one-frame slip is exactly what PREVIOUS would show, and PREVIOUS reads 0.
- Motion blur is visible too. The probe jumps about 566 px between frames, so in one of the two frames the lit side face is smeared about ±30 px. The smear is centred on the mask, so it widens the magenta but does not move its centroid. C2 (Y2) checks this factor.
- The rest of the leg is clean: 80/80 written, 0 dropped or pending, admission 0/0.

**But a reading isn't a gate.** A control establishes the cause, and the gate passes only by running. Y2 does both.

**The arbiter and the pairing gate ask different questions.**
- G228 says any gate asking "did this change what renders" runs at the AA-off arbiter.
- The pairing gate asks whether the mask comes from the same frame as the picture. Its reader was calibrated at the bench's normal lighting, and its pass history is there.
- Carrying the arbiter recipe over to the pairing legs broke the reader's precondition. That is a test-recipe problem, not a product finding.

### Y2. Prove the cause with a one-factor control, then run every pairing leg at the corrected recipe
**Freeze first.** Before any pairing leg, commit (docs/predictions):
- the amendment below, with its reasons;
- the decision table;
- the readings.

The failed leg stays as retained evidence, labelled "instrument precondition not met (probe unlit)". These do not change:
- the pairing reader;
- the 40 px tolerance;
- the predicate (PREVIOUS = NEITHER = 0);
- the decidable-frame floor, if the frozen spec has one.

**The amended pairing recipe.** This is attempt 1 of a new D2 harness problem, "pairing recipe".
- **Which legs:** every pairing leg, meaning native and synthetic, 100 % and 50 %, both fixtures, PIE and staged.
- **What changes:** they run the J2 recipe **minus the three lighting overrides**. `r.Lumen.DiffuseIndirect.Allow`, `r.DynamicGlobalIlluminationMethod` and `r.ReflectionMethod` are left at the host's own values.
- **What stays as frozen:** everything else, including AA off, auto-exposure off, the screen-percentage settings and the marker setting.
- This changes one factor group from the failed leg, so a pass names the cause.

**Legs, in order:**
1. **C1, the gate:** StackOBot PIE native pairing 100 % at the amended recipe, with its shifted-mask control.
2. **C2, a reading, not a gate:** the failed leg's exact recipe with motion blur off (for example `r.MotionBlurQuality 0`; record the command used). It shows whether motion blur plays any part. The expected result is still NEITHER.

**Readings on every pairing leg (never gates):**
- **Probe coverage:** the reader's own bright-magenta pixels inside the current mask, divided by the mask area; median over decidable frames.
  - Also take it on the failed leg.
  - Also take it on one retained historic passing pairing bank, if one is on disk.
- **`speed_ratio`,** as m51 defined it (m51's PREVIOUS failures went with loaded throughput). Write "unavailable" if the logs can't give it.

**Decision table (frozen before C1):**

| C1 result | Meaning | Action |
|---|---|---|
| PREVIOUS = NEITHER = 0, floor met, shifted-mask control rejects | Lighting was the cause | Adopt the amended recipe for every pairing leg and continue |
| Any PREVIOUS > 0 | A real pairing slip | **STOP** (product gate). Never retried |
| NEITHER > 0, PREVIOUS = 0 | Lighting alone isn't enough | Attempt 2: the historic pairing recipe exactly (no arbiter overrides at all), same leg and control. A pass means adopt it for every pairing leg. Otherwise **STOP** and report C1, C2 and attempt 2 |

- A later pairing leg (Lyra, 50 %, synthetic or staged) that reads NEITHER at the adopted recipe counts against the same two-attempt budget.
- Any PREVIOUS > 0 on any pairing leg, at any time, is a STOP.
- **Not authorised:**
  - a fixture content change (an emissive probe, a moved light);
  - a reader, tolerance or predicate change;
  - re-running a leg until it passes;
  - any product source change.

### Y3. Code's GAP B19: accepted; correct the record
- **The error:** the report says the healthy reader line came from the marker-corrected session. It came from the original marker-ON session.
- **No result changes:** the line holds on both sessions; Code ran it on each.
- **Fix:** Codex corrects the journal and the final report in the next docs commit.

### Y4. Code's observations B–E
**B. `owner_render_missing` reads 0 in the rejections map, though both resize drops carry that reason.**
- W2 holds: the drops are named in the log and counted in `dropped`, `unpaired` and `capture_unpaired_dropped`. The map counts family checks only.
- **No product change now.** It would force a rebuild and a full re-run for a diagnostics label.
- **Docs now:** state the counter's scope where `run_summary.json` is documented, and add a gotcha.
- **Tracked follow-up:** count the drop reason (or remove the key) with the next product change on this line of work.

**C. The label-pixel gate can't judge edges after a resize.** The J2 box-crop reader covers it.
- For the office: no window resize during a checkpoint session.
- Codex adds that one sentence to `tools\office_checkpoint_i.md`. Text only; the `.py` files don't change.

**D. Exit-code numbering.** On this resume the script renames the 093-10 `exit-code.txt` to `exit-code-attempt4.txt`, though it was the fifth attempt.
- Accepted. Code records the mapping in its receipts. No script edit.

**E. Process handling: noted.** The `UnrealTraceServer` daemon (PID 36544) isn't attributable to the job, so it stays untouched.

### Y5. The marker-barcode budget is spent
Both attempts for the marker problem are used. If a camera-switch leg fails for a barcode or marker reason, Codex stops and reports NEEDS-DECISION. There is no third attempt.

### Y6. Order (K7, continuing from the stop)
1. **Pre-flight:**
   - rehash the candidate against the `eff797c` manifest;
   - check hosts, staging and content against the 093-10 restoration receipt.
   - Any mismatch means rebuild, and every gate re-runs (as X2).
2. Freeze and commit Y2. The Y3 and Y4 docs may go in the same commit.
3. C1 and C2, then the decision table.
4. **The rest of the PIE matrix:**
   - StackOBot pairing: native 50 %, synthetic 100 % and 50 %;
   - Lyra synthetic alignment ×2 and Lyra pairing ×4;
   - the exact J1 delayed and pre-F1 wrong-view engine controls;
   - the supplemental admission probes.
5. The camera switch (K6), both fixtures.
6. The cook.
7. Every staged gate.
8. The push (fast-forward, `fix/followup-capture` only) when everything is green.

D2 is otherwise unchanged. The 32 passed cases stand, because the binary is the same.

### Y7. For the owner
- **The 04:29 shutdown (X4):** Code's check shows the PC booted again at 04:34:05, five minutes after the job died, with nobody at it.
  - So it restarted itself. An automatic Windows update is the likely cause; that is not established.
  - If overnight runs should survive, how to prevent it (for example, pausing updates or setting active hours) is the owner's call. Nobody here changes system settings.
- **Still open from K8:** the `AspectRatioAxisConstraint` read on both office hosts.

## Next
- **093-11 is queued:** Codex resumes its J2 thread, and Code reviews after Codex exits (W3).
- **After J2 qualifies at home:**
  1. office checkpoint I (second office host required, first optional);
  2. then `research/natural-causes` and J3.
