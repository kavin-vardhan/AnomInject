# REPORT for 079-04-verifier-cap-ruling-and-pre-review-freeze.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 079-04-verifier-cap-ruling-and-pre-review-freeze.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: high
started: 2026-09-15 12:04:32 | finished: 2026-09-15 12:14:59 | duration: 10 min | exit code: 0
context at last turn: 669282 tokens (67% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\079-04-verifier-cap-ruling-and-pre-review-freeze.live.txt

---- FINAL MESSAGE (Claude Code) ----
All clean and restored. Writing the report.

---

# 079-04 — REPORT

## Outcome

**All six re-readings match AMENDMENT 3** (committed `f064f0f` *before* the constant was touched). **One constant changed, nothing else in the gate.**

| | prediction | reading | |
|---|---|---|---|
| **P1** | selftest 17/17 `OK` | **17/17, `SELFTEST: OK`**, no `*** BROKEN ***` | ✅ |
| **P2** | six bench legs byte-identical to 079-03 | **`PASS 4 / SHIFT 0 / NOT-VISIBLE 0 / NOT-MEASURABLE 0`**, `tau=0.0040`, `M_med=0.0005`, `cap=0.0400` — all six | ✅ |
| **P3** | `A2L_LEGA` full 6/6 × 3 | **6/6 at δ = 0, +1, −1 — FULL RECOVERY** | ✅ |
| **P5** | `M50L_LG9` unchanged | **6 refusals, zero NOT-VISIBLE, zero SHIFT** | ✅ |
| **P6** | batch == individual | **identical** | ✅ |
| **P8** | `LYRA_SMOKE_01` + `A1L_LEGA` refused | **0 PASS / 0 SHIFT / 0 NOT-VISIBLE** | ✅ |

**Zero NOT-VISIBLE and zero SHIFT across all ten banked sessions; every session `VERDICT PASS`, exit 0.**

**P8 reason split** — exactly the departure AMENDMENT 3 declared in advance:
```
A1L_LEGA       idx0..4 camera motion   idx5 manifested-false-or-empty
LYRA_SMOKE_01  idx0..5 camera motion
M50L_LG9       3x camera motion  2x threshold unsatisfiable  1x region covers the picture
```
`A1L_LEGA idx=5` is `manifested: false`, and that check precedes the motion check — correct behaviour, not a miss.

**A2L_LEGA** now sits **~8× inside** the cap (`M_med 0.0049` vs `cap 0.0400`) instead of exactly on it — `PASS 6 / NOT-MEASURABLE 1`. The fragility ruling 1 cites is **observed, not hypothetical**: A2L's own `M_med` moved between **0.0043 and 0.0049** across the three shift runs, so the old cap would have refused the session that set it.

## Header block (the ruled lines, verbatim)

```
    M_med       = median over clean frames of the WHOLE-FRAME changed-pixel fraction
    MOTION_CAP  = 0.040

    cap=0.040 (2x margin below first incomplete recovery, LYRA_SMOKE_01 M_med=0.0801;
    only full recovery observed at 0.0049, A2L_LEGA; ruling 079-04)
```
Followed by the real-session recovery table; the synthetic ladder marked *"DELIBERATELY NOT the binding number… and it sets nothing"*; **"WHY 0.040 AND NOT 0.0049"**; and **"THERE IS NO MIDDLE TIER, AND THE REASON IS THE COMPLAINT THIS TOOL EXISTS FOR"** carrying the zero-false-shifts-to-0.085 measurement as the stated reason it was refused.

## One coherence fix I made beyond the brief

`m079_shift_recovery.py` still printed `CAP = 0.0049` — its own 079-03 `min()` rule — which now **contradicts the shipped constant**. It now prints **both bounds**, names ruling 079-04, and echoes `verify_capture.MOTION_CAP`. A calibration tool reporting a different number from the thing it calibrated is a trap for the next reader. New sha256 `9FA26D4F4172F887`.

## §0.5 — CaptureBench

**`git status` clean, tracked-clean at `472a409`**, verified before and after this brief. **`G261`** minted: the stripper is plugin-repo-only; never run it in CaptureBench or the Lyra worktree — `A53`-protected files live there, and untracked files there have no git copy to restore from.

## Standing limits (unchanged, restated)

Rule (a) met — **no correct label is called NOT-VISIBLE anywhere.** **Rule (b) is not met:** a cap at 0.040 still refuses most real gameplay footage, so the tool is honest and largely silent there. Only **one** real session has ever recovered fully, so the cap rests on a thin base and should be re-derived when more masked moving-camera sessions exist. `M50L_LG9` remains **not** a known-good session.

## State

`fix/verifier-field-validity` = **`57e91cd`** (pushed; `f064f0f` predictions → `57e91cd` implementation). `master` == `origin/master` == **`ac13700`**. `m51` = `53bf725`. **No merge** — frozen for Codex's merge-gate review. `measure_label_offset.py` byte-unchanged; `Source/`/`Shaders/` untouched; no capture, build or cook. Stripper run against `AnomalyInjector` only: **0 changed / 97 no-change**. Evidence `M079_VERIFIER_EVIDENCE\079-04\` (14 files). Temp junction roots removed; checkout restored to `m51`, all trees clean.

**Owed:** the office re-run on the M2 captures — predeclared **zero NOT-VISIBLE**; transcribe the header's `M_med`/`cap` and each event's `bbox=… obs=…`. If those sessions read `M_med` above 0.040 they will be refused for camera motion, which is rule (b) failing on the real data — the decision already in front of chat.

**VERDICT: GREEN**
api-equivalent cost (not billed on Max): $14.63 | turns: 24 | running total across all briefs: $1,499.39
