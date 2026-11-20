# Request to Codex: independent review of every Code-authored m55 change before merge (chat Claude, orchestrator, 2026-09-26)

**Role:** independent reviewer only, bounded.
- Read-only. No repo writes, no commits, no game launches or bench legs.
- Offline probes on banked data are fine where a claim can't be judged without one.
- Write `_reviews/081-42-codex-review.md` and end it with a `VERDICT:` line: APPROVE / APPROVE-WITH-FIXES / CHANGES REQUIRED.

**Why you:** you implemented m55 Stages 1–2. Claude Code (Opus 5.5) took over from build 2 on and has not been reviewed. A fresh Code session already reviewed your Stages 1–2 (081-20: 0 P1). This is the other half of that swap. Merge only after this review.

## State
- Branch `feat/m55-change-evidence`, head **`eafa654`** (== origin).
- `master` is `031a103`. The live checkout is `m51` `53bf725`, so read the branch from git objects.
- Build 3 = source `85c99b3`, Game exe `002805CF`. It is final; there is no build 4.
- The inventory of Code's changes is in `_reviews/081-41-evidence/merge-prep-inventory.md`:

| Commit | What | Lines (+/−) |
|---|---|---|
| `9c75abe` | build 2: `AnomalyChangeStage.cpp/.h`, payload cap + ownership census | 146 / 5 |
| `ea9946a` | `--change-oracle` in `tools/verify_capture.py` + test | 969 / 0 |
| `85c99b3` | build 3: F1/F2/F3/F6/F7/F9 from the 081-20 review, six Source files | 138 / 43 |
| `c27024d` | oracle exit codes 0/1/3 | 30 / 14 |
| `eafa654` | Stage 3 docs: `docs/change-evidence-fields.md` front section, `docs/client-readme.md` §9, catalogue note | +615 / −9 across 6 files |

- **Harnesses (outside the repo):**
  - requalification: `_reviews/081-23-*.py` plus the PS runners, lib at its last boundary;
  - cost: `_reviews/081-37-cost-*.py` with `_reviews/081-37-evidence/cost-boundary.json`.
- **Results:**
  - Requalification c0–c3: 92 legs GREEN (081-36).
  - Cost, paced at 30 fps: PASS at 720p and 1080p. Game-thread upper bounds are −0.07 / +0.11 ms against +1 ms; the writer keeps up; B yield 120/120; 0 drops; high-water 20 / 68 MB.
  - Cost, pacing off: a labelled stress reading. 720p PASS at yield 0.17–0.20; 1080p UNRESOLVED (cap, honest `budget_exceeded`).
  - Reports: `_reviews/081-41-evidence/stage3-report.md`, `_reviews/081-40-evidence/stage3-cost-report.md`.
- **Chat rulings along the way:** `_reviews/081-17…081-41-chat-ruling-*.md`.
  - The harness amendments are AMENDMENTS 1–6 in `docs/predictions/2026-09-24-m55-081-23-requalification.md` and AMENDMENT 1 in `docs/predictions/2026-09-26-m55-stage3-cost.md`.
  - The scorecard is `_reviews/081-17-codex-vs-code-comparison.md`. Ignore it for the review, but it's there.

## Questions
Answer each with findings ranked P1 (blocks merge) / P2 (fix before merge) / P3 (note). For each finding give the file and line, a concrete failure scenario, and a suggested fix. "No finding" is a valid answer; say what you checked.

**R1 Build 2/3 source (`9c75abe`, `85c99b3`).** Are these correct against the three contracts (numeric, identity/lifetime, temporal) you designed?
- The 256 MiB payload cap and fail-fast admission.
- The ownership census.
- F1: a run that never started writes no folder.
- F2/F3: legacy colour and masks exactly pre-m55 whenever m55 can't attach (multi-view, disabled, non-viewport); the `change_multi_view_families` counter.
- F6: `late_results` removed.
- F7: `change_unsupported_completion`.
- F9: ChangeGate refused without `-IAIBenchFixture`.

Specifically: can any path now leak memory, double-release, mis-stamp a receipt, or change legacy output when evidence is OFF?

**R2 Oracle (`ea9946a`, `c27024d`).** Does `--change-oracle` validate exactly what it claims (arithmetic and transport only) and no more?
- Can it pass a sidecar whose pairs are internally inconsistent?
- Are exit codes 0/1/3 unambiguous?

**R3 Docs (`eafa654`).** Is every client-facing statement in `client-readme.md` §9 and the field-reference front section true to the source and to the measured evidence? Flag any overclaim, especially:
- "no measurable cost";
- the memory envelope (the closure formula shipped as a planning estimate, "not a bound", per 081-16);
- "refusals are honest";
- the delay=3 paragraph.

**R4 Requalification harness amendments (AMENDMENTS 1–6).** Can any of them let an m55-caused legacy change or measurement regression PASS? Look hardest at:
- **A**: coalesced-arm unmeasured frame, excused under a log signature + A.6 colour-file condition + a cap;
- **B**: mask tags compared modulo one leg-wide bijection;
- **C**: the cross-pair clause honouring the historical run-unique set;
- **M47**: m47 keys excused under their own PREWARM signature;
- **G297**: self-excused singletons are a reported reading with a recurrence tripwire, not a gate. Chat chose visibility over gating, reasoning that n = 2 per binary can't separate a one-leg deviation from noise. Do you agree?
- The **person-present gate**: can it ever reclassify a fixture-valid result?

**R5 Cost harness and statistic.** Is the gate sound?
- The statistic: one-sided 95 %, pooled A/A and B/B spread, t with 6 degrees of freedom.
- The external game-thread cycle sampler: G299 rejected OS thread time; G303 tested the pacer replica.
- The paced-vs-pacing-off framing, with paced as the headline and pacing off labelled as stress.

Is there a way the paced PASS could hide a real ≥ 1 ms cost? Is the 20 / 68 MB high-water representative?

**R6 Merge blockers.** Anything else that must be fixed before a no-ff merge to master?
- The comment stripper would change 65 lines, all from Stage 1–2 commits.
- `AnomalyBench` is excluded from client packages by the pre-delivery checklist.

## Bound
About 1 working session. If R4/R5 need more, report what you have with the remainder listed.
