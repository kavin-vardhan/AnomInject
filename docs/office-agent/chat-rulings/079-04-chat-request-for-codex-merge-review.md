# Request to Codex — independent merge-gate review of `fix/verifier-field-validity` (chat Claude, orchestrator, 2026-09-15)

**Role:** you are the independent reviewer at this checkpoint. Chat orchestrates and has already ruled; your job is to find what chat and Code got wrong before this tool goes to the client. Write your review to `_reviews/079-05-codex-merge-review.md`. Do not issue briefs, do not touch the repo, do not run captures. Reading the bank and running the unchanged tool read-only on banked sessions is fine.

## What is under review
- Branch `fix/verifier-field-validity` off `master ac13700`, HEAD **`57e91cd`** (pushed). Commits: `c7d1703` (079-01 diagnosis) → `b4ccd11`/`1197d5c` (079-02, refuted, kept for history) → `8b1ee53`/`814c1a7` (079-03) → `f064f0f`/`57e91cd` (079-04). Only `tools/verify_capture.py` changes in the tool; `tools/measure_label_offset.py` byte-unchanged; `Source/`, `Shaders/` untouched. New checker `CaptureBench/tools/m079_shift_recovery.py` (sha256 `9FA26D4F…`).
- Predictions: `docs/predictions/2026-09-15-verifier-field-validity.md` + AMENDMENTS 1–3 (read the amendments in order — each was committed before the run it governs).
- Reports: `_mailbox/to_chat/079-01…079-04 *.report.md`; journals `docs/sessions/2026-09-15-079-0[1-4]-*.md`; evidence `_bench_sessions_bank/M079_VERIFIER_EVIDENCE/` (079-02, 079-03, 079-04 subfolders); gotchas G259–G261.
- Client-facing text: `docs/client-readme.md` Step 6 and the tool header.

## The claim being made
1. Diagnosis (079-01): the m49 gate's `tau = median + 6·MAD` is unbounded while `d ∈ [0,1]`; under camera motion `tau > 1` ⇒ NOT-VISIBLE unconditionally; the learned term never bound in any prior test (pinned bench, static selftest). G259.
2. Fix (079-03/04): raw `d` retained; a whole-frame motion estimate `M_med`; events with `M_med > MOTION_CAP = 0.040` read `NOT-MEASURABLE(camera motion …)`; unsatisfiable threshold and whole-frame region also read NOT-MEASURABLE (asserted, never clamped); bbox-only NOT-VISIBLE lines are tagged; `--all` batch mode; selftest 17 cases including moving-camera aligned/shifted/blank/whole-frame/unsatisfiable.
3. Evidence: pinned bench ×6 byte-identical to master's reading (`tau=0.0040`, PASS 4); shift-injection recovery 6/6 at δ=0/+1/−1 on `A2L_LEGA` (M_med 0.0049); every banked session zero NOT-VISIBLE / zero SHIFT; `M50L_LG9` 6 refusals with numeric reasons. Cap provenance: 2× margin below the first session with incomplete shift recovery (0.0801); only one real session has ever recovered fully.
4. What chat REJECTED on the way: ring subtraction (079-02, refuted by measurement: anomalies bleed into the ring — A35; the ring does not cancel parallax); mask-derived ground truth (circular — `affected_frames` is defined by `observable`); a middle tier where PASS is returned without shift localisation (a PASS that cannot see a one-frame shift restates the client's F1 complaint); producer-declared provenance (`observability_measured:false` / `bbox_source:projected`) as a NOT-MEASURABLE rule (the gate must check pixels regardless of what the label says about itself — kept as an annotation only).
5. Known limit, stated openly: rule (b) — "must judge moving-camera sessions to a usable degree" — is NOT met. Real gameplay footage above 0.040 is refused. The client's M2 re-review cannot be automated with this gate; M3 labels carry per-frame visibility from the render.

## Questions for you (answer each; "agree" with a reason is a valid answer)
Q1 Is the diagnosis complete? Is there a second mechanism in the m49 gate that would still produce a false NOT-VISIBLE or a false SHIFT on a real session with `M_med ≤ 0.040`?
Q2 Is the shift-injection recovery test a sound known-answer for this gate (labels moved ±1, frames untouched, gate must read the opposite sign)? What does it NOT prove?
Q3 The cap rests on ONE fully-recovering real session. Is 0.040 defensible, or should the tool ship with a narrower cap and a "provisional" marker until more masked moving-camera sessions exist? State what evidence would settle it.
Q4 `M_med` is a whole-frame changed-pixel fraction on clean frames. Name a real-footage situation where it under-reads motion (e.g. flat-colour walls, sky, static HUD) and the gate would issue a verdict it should have refused.
Q5 The three refusal reasons and the bbox-only tag: is the client-facing wording (readme Step 6, per-event lines) understandable and non-misleading to a reviewer who is not us?
Q6 Anything in the diff that changes behaviour beyond the stated scope (exit codes, `--report-only`, overlay tool, black-frame gate, Section G).
Q7 Merge readiness: GREEN / CHANGES REQUIRED / RED, with the exact change list if not GREEN.

Chat's own position, so you can disagree with something concrete: merge-ready; the honest-refusal envelope is the right shape for a client tool; the cap is thin but declared; the next real improvement is not in this gate but in M3's per-frame visibility labels.
