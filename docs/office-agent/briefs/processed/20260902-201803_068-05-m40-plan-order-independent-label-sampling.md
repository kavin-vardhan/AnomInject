# EFFORT: high

SESSION 068, BRIEF 5 — rulings on 068-04, then the m40 PLAN + PREDICTIONS. ⛔ PLAN ONLY: no source edits, no build, no leg. Headless: final message = report; never wait; NEEDS-DECISION for real forks. Codename discipline absolute.

VERDICT ON 068-04 (commit 623ca24): ACCEPTED in full. The bench control (Δ=+2 on all 8 bursts) against Bates (Δ=+3) is the cleanest measurement this ledger has; "consistent with, not asserted" stands as the ledger's language until a fix build is validated on Bates. Option 2 is APPROVED as the fix: sample the label's active state at FWorldDelegates::OnWorldTickEnd (post all tickables, pre-draw), making the injector/capture tick order irrelevant to labels and leaving pixels untouched on every host.

RULINGS:
 R1. Prove-it-can-fail = ROUTE A. A bench-only, default-OFF lever that synthesises Δ=+3 (the SYMPTOM, not the cause — the gate file and the StartRun echo must both say so, A48-shape echo). Choose the mechanically simplest faithful synthesis (e.g. the injector does not count the tick on which Apply ran, guarded by the lever) and justify it in the plan. ⛔ Byte-inert when OFF (P-C7 must hold on the new binary with the lever off); never client-facing (no ini key, console only, name carries "Bench"/"Synth"); named in every read-back.
 R2. Sync fallback (AnomalyCaptureSubsystem.cpp:2439): NOT moved in m40 — documented as a known limitation in the ledger + a follow-up item, because no gate exercises that path today and one-variable-at-a-time applies. The fallback already announces itself; note in the plan how a reader would detect it (SVE-WANT-SUMMARY marksIssued vs framesWritten, per 068-03).
 R3. Milestone number = m40 ("order-independent label sampling"). m39 stays honest bbox. m40 MAY SHIP BEFORE m39 — numbers are identities, not order; say so in CLAUDE.md's milestone map. Office batch tag sequence gains m40 when it lands (m31→…→m38→m40; m39 slots in when it ships).

TASK — write the m40 plan (journal 068 §10) and the predictions file (docs/predictions/2026-09-02-m40-order-independent-label-sampling.md), then STOP and report; implementation is the next brief after chat approves.
 The plan must contain:
  1. File-by-file changes (exact functions; the OnWorldTickEnd hook registration/unregistration lifecycle mirroring m26's proven pattern at :334/:704/:707; the World guard; what SampleDeferredActiveState becomes; where the sampled value is consumed by FireActive/frame_indices; the lever and its echo).
  2. THE FOUR LEGS, pre-declared with p9_hidden_set.py as the reader (SEP_RATIO 5.0 frozen, never retuned):
     L1 pre-fix binary (F2FA6BCD), lever OFF → 16/16 ALIGNED (control, must reproduce the certified cadence byte-identical).
     L2 pre-fix binary, lever ON → P9-SHAPE on every readable event, observed hidden {n,n+1,n+2,n+6} vs claimed {n,n+1,n+5,n+6}, Δ=+3 in the toggle lines (anomaly_log.txt with RunLogVerbose) — THE BENCH REPRODUCTION OF P9 (B). If L2 does not reproduce, the lever is wrong, not the theory: stop and report.
     L3 post-fix binary, lever ON → 16/16 ALIGNED, and labels now read {n,n+1,n+2,n+6} matching pixels, with toggle lines still Δ=+3 (proves the fix, not the lever, moved the labels).
     L4 post-fix binary, lever OFF → 16/16 ALIGNED AND frame_indices / labels.jsonl byte-identical to L1 (proven inert where the order already agrees). P-C7 re-anchor rides L4.
     Plus: m20's pixel-vs-label measurement re-run on the post-fix binary; P6 48/48 keyset; census-OFF client-inert unchanged (no new client keys; the lever is console-only).
  3. Predictions: per leg, the exact reader verdicts and counts you expect, written BEFORE code; the abort/teardown behaviour of the new delegate (no dangling handle across world transitions — the lifecycle risk).
  4. Bates validation plan for the next build update: pre-declared pass condition per 068-04's Route-B wording (labels equal the eye whether Δ reads +2 or +3), the C-3 bundle re-run as the instrument, blinking stays unticked on Bates until then.
  5. What the m40 commit shape is (predictions commit → feat(capture) commit; scope tag; no tag).
 Also: ledger §8.6a gains "FIX APPROVED: option 2, m40" line; card Standing block unchanged.
Commit: `docs(m40): plan and predictions - order-independent label sampling`. Push.

FINAL MESSAGE: 1. status + SHA. 2. the file-by-file table. 3. the four legs with predicted verdicts. 4. the lever's exact mechanism + echo text. 5. NEEDS-DECISION or "none". Then STOP — do not implement.
