# 081-41 — Chat ruling on 081-40 GREEN: Stage 3 cost PASS (paced, 720p + 1080p); docs and the consolidated Stage 3 report released

Date: 2026-09-26 06:50 IST · Author: chat Claude (orchestrator) · Build 3 (`85c99b3`, Game 002805CF) · Feature head `ab634f5`

## Ruling 1 — cost accepted
**Headline, paced at 30 fps (production-like): PASS at 720p and 1080p.**
- Game-thread Δ upper bound: −0.072 ms at 720p, +0.110 ms at 1080p. The budget is +1 ms.
- The writer keeps up; pacing held.
- 0 drops.
- Every B leg measured 120/120 required pairs.
- m55 high-water 20 MB / 68 MB against the 256 MiB cap.
- The oracle exit 0 on all 13 B legs.

**Stress, pacing off (labelled; not the headline):**
- 720p PASS at yield 0.17–0.20.
- 1080p UNRESOLVED: writer saturation hit the cap, and the budget refusals were honest.

Findings recorded:
- **G302:** under pacing-off load, `view_mismatch` / `unsupported_delivery` collapse the yield. This is the m51 pairing defect; m55 refuses those pairs correctly.
- **G304:** the run log costs about 1 ms of game-thread CPU per frame. It is a debug feature and was off in the gated legs; the cause is not established. Clients are told to leave it off for production captures.
- **The FPS-21.5 prior is closed as PARTIAL.** It was a one-off 1.56 s start-of-run stall that did not reproduce in n = 2.
- **Instrument limitation:** the game process GPU counter reads 0.0 %.
- **Prediction misses:** the diagnostic cycles and the arm-rate band. Both are reported as misses and neither was refitted.

## Ruling 2 — 081-41 docs (bench-free, high), per 081-03 §Stage 3 and every carry-forward
Client-facing docs use codenames only, with **no internal review identifiers** (no brief numbers, no G-numbers, no "chat"/"Codex"/"Code"; the 079-14 lesson). Internal docs may cite them.

1. **Field reference** (`docs/change-evidence-fields.md`, update in place):
   - every new field, the sidecar `change_evidence.jsonl` schema, and the closed 14-value refusal enum with what each one means;
   - the `ref_*` pre-onset comparison, `cam_delta`, the control and the histogram;
   - "measurements only, no verdict in v1; `observable` unchanged".
2. **Client readme** (`docs/client-readme.md`, a new m55 section):
   - **What it is.** Per-frame, per-target measured pixel change with a whole-frame control. **No verdict in v1.** Readers decide; the tool reports.
   - **The delay=3 paragraph.** Gradual arrival shows up in the pre-onset `ref_*` comparison, not in adjacent pairs.
   - **Memory envelope.** It covers m55's admitted payload only, not total process RAM.
     - Hard cap 256 MiB; the closure-bound formula.
     - Measured paced high-water: about 20 MB at 720p and about 68 MB at 1080p.
     - 1440p ≈ 195 MB is within budget but unexercised.
     - At 4K ≈ 440 MB, expect `budget_exceeded` unless the knob is raised.
     - Refusals are honest: never wrong numbers.
   - **Cost.** At paced 30 fps there is no measurable game-thread cost at 720p or 1080p (bounds < +0.12 ms), the writer is unaffected and the workload is full. **Use paced capture.** With pacing off at 1080p the writer saturates and m55 refuses pairs (`budget_exceeded`). Under heavy unpaced load, expect `view_mismatch` / `unsupported_delivery` refusals.
   - **The `positive_frames` caveat.** It counts unmeasured deferred rows.
   - **Oracle usage** (`verify_capture.py --change-oracle`). Exit codes 0 / 1 / 3. It validates arithmetic and transport only.
   - **Known limits (pre-existing, not m55):**
     - an occasional unmeasured frame when a mask render slips;
     - a shader-readiness marker that can over-flag frames in packaged builds;
     - `coverage_pct` is timing-sampled;
     - leave the run log off for production captures.
3. **Catalogue note.** The bench twins `null_effect` / `solid_swap` are bench-only and excluded from the auto pool.
4. **Consolidated Stage 3 report** (`_reviews\081-41-evidence\stage3-report.md`):
   - the oracle (081-19);
   - cost, with paced as the headline and the stress readings labelled;
   - the requalification summary (c0–c3 and AMENDMENTS 1–6);
   - the priors table;
   - the c0 long-run reading;
   - the **UNEXERCISED block verbatim**;
   - the product findings list for m51/M3 (G295, G298, G302, G304, `coverage_pct`).
5. **Merge-prep inventory (list only, no action):**
   - the comment-stripper scope over the Stage 1–2 source (Codex-authored; about 66 lines flagged earlier) and any Code-authored comments;
   - `AnomalyBench` excluded from client packages (M3 checklist, `docs/PRE-DELIVERY-CHECKLIST.md`);
   - the exact file list of every Code-authored source change since build 2, with commits, for the Codex review.
6. **Docs:** journal, CLAUDE.md status block (Stage 3 complete pending review), gotchas. Commit and push.

## After 081-41
**Codex reviews every Code-authored change.** The owner pastes chat's request (`_reviews\081-42-codex-review-request.md`) into Codex. Scope: build 2, build 3, the oracle, the requalification and cost harnesses, and the Stage 3 docs.

Then:
- the comment stripper;
- the merge by Code (no-ff onto master, **no tag unless the owner asks**);
- the handoff.

After that: m53 flicker → m54 UV/normal corruption → m51 pairing → M3 packaging.
