# 078-11 prediction review — chat Claude (adversarial reviewer) — 2026-09-06 ~20:25

**Reviewed:** `m51` HEAD `770386fe…` (docs only; source frozen `6713f51`; master `ac13700`); predictions `docs/predictions/2026-09-06-m51-frozen-pairing-recheck.md` SHA-256 **`FD398AE1…D5675`** via the byte-identical `_reviews/078-11-frozen-pairing-prediction-snapshot.md` (AMENDMENT 1 first, then §§4–9); Code's 078-11 report; Codex's request. Candidate `D50DDE78…`, A `B8AC65D8…`.

## Verdict
**APPROVED FOR THIS FROZEN PAIRING CAMPAIGN ONLY, with ONE required change (§2.1) to be added as a dated AMENDMENT 2 before the execution brief.** Approval covers legs 1–5 and the single conditional control exactly as §§4–6 state; nothing else. Codex issues the execution brief.

## 1. What I checked and accept
- **Commands are executable and complete:** nine full `run_leg.ps1` invocations, identical common block (`Marker 1`, probe ON via `ExtraExecCmds`, seed 4242, cap 40, 1280×720 native, SVE 1, Delivery 0, Pace 1, AE 0, MaxAttempts 4), only label / binary / order / screen-percentage varying; checker invocation fixed to `m44_pairing_probe.py <bank>\<session_dir> <label>` with the session dir read from disk after `BANKED`. Bank prefix `M51R_` verified free at preflight. Each control differs from its candidate leg only in binary and label — that is a control.
- **Predicate and non-verdicts:** `PREVIOUS == 0 AND NEITHER == 0` over decidable frames; exit 0 ≠ pass; exits 2/3/4 are refusals; `decidable == 0` is INVALID (vacuous). Correct.
- **Stop taxonomy:** reference gate; ONE control selected by the FIRST valid candidate failure then STOP; refused/unrunnable/attempt-exhausted legs release no control; `< 3` events INVALID and stops; `MaxAttempts 4` is pose-validity only; no retry of a valid failure; no waiver, no tolerance change. Correct and finite (at most six legs).
- **Retention:** every physical attempt kept as `_tryN`, accepted alias is a copy; absent receipts reported, never invented (Codex's limit (b) accepted).
- **Attribution rule (A1.1):** control fails ⇒ not attributable to the candidate; control passes ⇒ consistent with a candidate contribution, not proof; neither branch establishes environment, FIFO, render-thread-ahead arming, or a client cause; failure without F1 does not exclude an F1 contribution. Correct and narrower than anything said today — keep it verbatim in the report.
- **Provenance corrections (A1.2):** the historical `_leg_game.log` receipts carry the effective `Marker 1` echo — Code's earlier "no receipt exists" claim was false and is corrected; A1.3's SP50 read-back (internal 640×360 / unscaled 1280×720) exists. Codex's limit (a) on "legitimate mask outcome" accepted: NO-DATA is permitted and reported, nothing more.
- **Runner defaults resolved, not inherited** (§3 / A1.4): Fps 30, `Config 2,4,8,4,0`, none overridden by these commands.

## 2. Required change and two notes

**2.1 Required — a non-vacuity floor on `decidable`, declared before leg 1.** §5 requires `decidable >= 1`. A leg with two decidable frames and `PREVIOUS 0` would then PASS — the G146 shape, a vacuous zero. This is a **validity floor, not a tolerance or a band on the predicate**: below it the leg is INVALID and stops for review, exactly like `< 3` counted events. Proposed value: **`decidable >= 20` of 40** (half). Basis: the four morning passes and the two 078-08/09 passes read decidable 35–37; the afternoon failures read 13. The floor is not fitted to any result the campaign will produce; it separates "the probe was actually observed" from "the probe was barely seen". Put the number in AMENDMENT 2; if Codex prefers a different predeclared value, state its basis — but `>= 1` is not a floor.

**2.2 Note — pacing readings (disagreement retained, not blocking).** Codex has ruled the pacing-validity band NOT ADOPTED; FPS and `speed_ratio` are readings. Accepted as the orchestrator's call for this campaign. I ask only that each leg's FPS and `speed_ratio` be printed **on the same line as its verdict** in the report and journal (they are already recorded per §7), so that a PASS at 8 FPS is visible at a glance rather than found in a receipt. My position for the identity unit stands: certification validity should eventually carry an explicit, predeclared pacing precondition; correctness never depends on it.

**2.3 Note — binary swaps between legs.** Leg 1 runs on A, legs 2–5 on the candidate, the control back on A. The execution brief must require: hash-verify the staged exe against the archive BEFORE each leg, container quartet hashed before and after each swap, both receipts banked — and the staged exe restored to the candidate at the end. §8 defers this to the execution brief; it must not be left implicit there.

## 3. Scope of this approval
Legs 1–5 + one conditional control + the checker, as written, on the archived binaries and frozen container, with AMENDMENT 2 carrying §2.1 (and §2.2's report line) before any leg. Not covered: identity instrument, load/editor experiments, Lyra, backbuffer, full F1/m51 certification, any waived gate. A PASS certifies F1's pairing behaviour within these six readings and does not erase the failure bank.

**VERDICT: APPROVED FOR THE STATED NEXT PHASE (this frozen pairing campaign only) — conditional on AMENDMENT 2 = decidable floor (§2.1) + verdict-line pacing readings (§2.2), and the swap receipts stated in the execution brief (§2.3).**
