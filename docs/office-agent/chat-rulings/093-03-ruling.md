# 093-03 — Chat ruling: Part 0 ACCEPTED; J1 NOT YET QUALIFIED, continue in 093-04

**Date:** 2026-10-13, about 23:20 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- Codex's report: `_relay\runs\2026-10-13-093-03-j1-natural-base\report.md` (`gpt-6-astra`/`max`, full access, 4,434 s, VERDICT NEEDS-DECISION).
- Claude Code's review: `_reviews\093-03-code-review.md` (15 OK · 5 GAP).
- Mailbox report 093-03: NEEDS-DECISION, 86 min, $11.02 API-equivalent.

## Rulings

### C1. Part 0, the relay guard: ACCEPTED
- `-OrchestratorBrief` and `-GuardCheckOnly` were shown working both ways (exit 6 / exit 0, with no writes).
- Code's addition of `-Sandbox danger-full-access` is correct: owner decision 090-12s applies, and every implementer run uses it.

### C2. J1 status: not qualified
**Accepted as evidence** (Code recounted or rehashed each):
- the merge (one conflict, `CLAUDE.md`, resolved by keeping both dated blocks);
- defaults 0/0/0;
- the source delta equal to the release's own change;
- normal and strict builds, 0 errors / 0 warnings;
- the CPU tests and mutants;
- the F1 UV multi-viewport PIE leg (1,152 images, 4 events, all edges +0, every labelled frame with a measured mask);
- its can-fail: the shipped release captures the wrong view.

**Not qualified yet.** Most PIE gates and every staged gate are unrun. Nothing is pushed, which is correct.

### C3. F6: Code's diagnosis is upheld
- **Why the stop happened.** Both the old and the new checker run the per-row self-consistency test (`event_id == id@start_frame|target_name`) on every row, before the transition filter. A mutant that changes only `event_id` trips that test in both. The stop was right under the stop rule; the explanation ("leftover transitions") was wrong.
- **The continuation uses a self-consistent wrong event** on an existing capture: `start_frame`, `target_name` and `event_id` changed together, on a row with no transition.
  - Frozen expectation: the **old checker misses it, the new one rejects it** ("labels event missing from annotation").
  - Code reproduced this on scratch copies (`code-receipts\f6diag\`).
  - Run it in PIE now, and on a staged capture later.
- Codex's `event_id`-only mutant stays as a second, documented control: the self-consistency test catches it in both versions.
- **No transition-free capture is needed.**

### C4. Docs before any push
- Correct the F6 explanation in the gotcha, the `CLAUDE.md` status block and the journal.
- Renumber the gotcha from G465 to the next free number across **all** refs: G477 at review time; re-check at commit.

### C5. Wording
- The existing `verify_capture.py --label-pixel-gate` exits 0 on the wrong-view control and reports UNASSESSABLE. The rejection comes from the job's marker and mask checks. The report and docs say exactly that.
- **Follow-up, for the regular delivery line and not this job:** an all-UNASSESSABLE run should not print "NO FAILURE FOUND". This is logged for the tooling backlog.

### C6. Continuation 093-04
**Preferred route:** resume Codex's J1 session (`relay_resume_codex_impl_v1.ps1`) with the continuation prompt, so it keeps its harness context. Otherwise launch a new implementer job.

**Scope:**
- the corrected F6 control;
- the docs fix;
- a bench cook with the fixtures the remaining gates need;
- the full remaining matrix, PIE AND staged, from the **unchanged** predictions file:
  - F1 shared-proxy mode, plus staged;
  - F2 C1/C2 scope and tag retirement;
  - F3 alpha pair;
  - dropped-mask accounting with deliberate drops;
  - m52 hold;
  - shared proxy;
  - normal and co-user;
  - the label-pixel gate with measured masks.

**Rules:**
- **No source change** is allowed. `E41ABF93` stays the candidate binary. Any needed source change means STOP and NEEDS-DECISION.
- Push `fix/followup-capture` (fast-forward) only when every gate is green and the docs are corrected.

### C7. Capture size
The PNGs are 1123×870, while the viewport metadata reads 1280×992 (from a 1280×1024 request). Explain this in one line in the continuation. It is not a gate: the label-pixel gate passed, with masks matching the pictures.

## Next
- 093-04 is released (Codex continues J1; Code reviews).
- Then J2, the office-robust F1, on the qualified `fix/followup-capture`.
- Branch policy P10 is unchanged.
