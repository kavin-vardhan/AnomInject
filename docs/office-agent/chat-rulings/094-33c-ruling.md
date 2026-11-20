# 094-33c — Chat ruling: C3 passes on stock 4.25. Frames, labels and masks now all line up. One added completeness term for lighting, then C4: capture through the real firing path (094-34)

**Date:** 2026-11-01, about 07:10 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Mailbox report 094-33c:** NEEDS-DECISION. It ran 05:33–06:08 IST.
- **Codex's report:** `_reviews\094-33c-codex-report.md`.
- **Code's review:** `_reviews\094-33c-code-review.md`. Code recomputed every number from the raw files.

## Verdict
**C3 passes FQ2 and FQ3 for all four anomalies, on one session, with the retry unused:**

| Anomaly | IoU | Precision | Gate | Spill |
|---|---|---|---|---|
| blinking | 1.000 | 0.939–0.940 | ≥ 0.90 | 0 |
| missing_object | 1.000 | 0.940–0.941 | ≥ 0.90 | 0 |
| lod_corruption | 1.000 | 0.979 | ≥ 0.80 | 0 |
| lighting_mismatch | 0 outside receivers | 1.000 | ≥ 0.80 | 0 |

**What else held:**
- **Clean frames:** all 93 carry 0 mask pixels, with no mask file (5.1's `m44`, citations checked);
- **the auto-exposure lock** (`r.EyeAdaptationQuality 0`) is cited and recorded on every row;
- **the one-frame shift** fails for all four;
- **the wrong target** fails for blinking, missing_object and LOD;
- **C2 holds** on the same session, by Code's independent label-free check: 0 mismatches, with all 24 edge-shift negatives failing;
- **safety:** no crash, no upload, the Paris host and `m51` unchanged.

**Two gaps:**
1. **Lighting's wrong-target negative passes,** because the gate has no completeness term: any subset of the lit objects scores precision 1.
2. **C2's reference images were re-picked after the session.** The exposure lock changed the picture.

## Rulings

### FR1. C1–C3 are accepted on stock 4.25
**Frames, labels and masks** agree with what is on screen, frame by frame, for the four smoke anomalies.

**Still to do:**
- the test-scene coupling (FR4);
- Europa's fork.

### FR2. Lighting gets a completeness term, re-scored from the kept 094-33c evidence (FH4)
- **The term:** the mask covers at least **0.95** of the union of all affected receivers' silhouettes.
- **On the kept evidence:**
  - the real mask scores 1.000;
  - the best single object scores 0.44, and leaving out the smallest scores 0.84.

  So the wrong-target negative now fails, as it should.
- **Why this isn't tuning after the fact:**
  - the real result passes at any line between 0.85 and 1.0, so the choice doesn't decide the pass;
  - 0.95 is stricter than Code's suggested 0.90;
  - it closes a hole without loosening anything.

Re-score it in 094-34 from the kept files, without a new session.

### FR3. C2's reference images are fixed before the run from now on
- **The rule:** the reader takes its clean references from the session's own pre-apply frames by a rule written before the session, never chosen after.
- **094-33c's C2:** accepted on Code's independent label-free check, which agrees.

### FR4. C4: capture through the real firing path, with the test-scene coupling removed (094-34)
**Bind targets from the live anomaly, as 5.1 does.** Cite the 5.1 code. Nothing in the plugin may name the fixtures. Specifically:
- no fixed per-anomaly stencil tags (128–131) in the plugin: tags come from the live anomaly's targets, using 5.1's stencil ledger;
- no `EA_*` actor tags or names in plugin source;
- no hard 1280×720.

**A static check:** the plugin's Source holds no fixture names and no fixed tags.

**The sessions:** two auto-injection capture sessions on EA_Core, using the real firing path:
- the auto-injector, with the selector's visible targets, seeded (4242);
- restricted to the four smoke anomalies;
- long enough for at least 3 events of each;
- one session at 1280×720, and one at 1600×900.

**The gates, both sessions:**
- C2 by a label-free check, with 0 mismatches;
- C3 by FQ2, FQ3 and FR2;
- both negatives fail;
- a re-run with the same seed gives the same sequence (A8's seeded check).

**The rest:**
- FM1's build gate and FJ1's lean profile apply;
- one retry slot per session;
- **the window:** until 07:00 IST on 11-02.

## For the owner
- **Nothing to do.**
- **Capture works on 4.25 in the test scene.**
- **Next:** make it work on whatever's on screen, the way it will at the office.
