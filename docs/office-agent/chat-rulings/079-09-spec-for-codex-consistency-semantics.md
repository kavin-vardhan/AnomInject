# 079-09 — Implementation spec for CODEX: the verifier reports consistency and readings, never label truth (chat Claude, orchestrator, 2026-09-20)

**Role change for this unit (owner's decision):** Codex IMPLEMENTS this spec on `fix/verifier-field-validity`; Claude Code will be the independent reviewer afterwards (roles swapped). Chat remains orchestrator. Codex's disposition of its own 079-08 review: all six findings accepted by chat (`_reviews/079-08-chat-disposition.md`); this spec is the correction.

## 0. Ground rules
- Work ONLY on branch `fix/verifier-field-validity` from `1cbc07e`. Do not check out, edit, merge into or rebase `m51` or `master`. `Source/`, `Shaders/`, `tools/measure_label_offset.py` byte-unchanged. CaptureBench: only `tools/m079_shift_recovery.py` may change. Push the branch; no merge, no tag.
- Predictions before results: write `docs/predictions/2026-09-20-verifier-consistency-semantics.md` (a NEW file; the old predictions file gets a dated pointer) with the predeclared readings in §5 BEFORE any implementation commit. Every fixture you built in 079-05/07/08 becomes a permanent selftest case with its known answer under the new semantics.
- Conventional Commits; journal `docs/sessions/2026-09-20-079-09-codex-implementation.md`; a finding-by-finding closure note `_reviews/079-09-codex-implementation-report.md`. No mailbox use. Restore the checkout to whatever it was when you started.

## 1. The semantics (fixed by chat — implement exactly; argue in the report, not in the code)
The tool never asserts that a label is correct or incorrect. It reports pixel evidence and one sound negative.

Per edge (onset, end) of every run, the OBSERVATION is one of:
- `TRANSITION k d=… tau=…` — the largest above-threshold, dominant change on the target within the search window, at frame k;
- `NO-TRANSITION [a..b]` — the whole window was observed and nothing on the target exceeded tau;
- `UNASSESSABLE(<reason>)` — see §2 for the reasons.
There is no per-edge verdict, no "aligned", no "shift". The label's frame is printed beside it: `label s Δ=k−s`.

Per run:
- `CONSISTENT` — both edges observed, each TRANSITION at Δ=0. Wording in output: "consistent with the label; does not establish cause".
- `OFFSET-NOTE(Δonset, Δend)` — both edges observed, at least one TRANSITION with Δ≠0. Wording: "a target transition sits off the label by Δ — possible label offset OR an unrelated change (lighting, occlusion, neighbouring event); a person must look at frames k−1..k+1". NOT a failure; exit code unaffected.
- `NO-TRACE` — the ONE sound negative. Conditions, all required: mask mode with a valid mask for EVERY frame of the run's span plus both edge windows; every frame pair observed; no above-threshold change on the target at ANY frame of the span or the windows; the run's class is one whose injection necessarily changes the target's pixels (hide, texture, LOD, colour/material — cite the class list from the producer and exclude classes that need not, e.g. time-dilation/flicker-off frames if applicable). Wording: "the label claims a visible change on this target across frames s..e; the pixels contain none". This is the only outcome that makes the session FAIL (exit 2 unless `--report-only`).
- `PARTIAL` — one edge observed, the other UNASSESSABLE. Never promoted.
- `UNASSESSABLE` — neither edge observed.
- `READING` — bbox-only run (no masks): print the same observation lines computed on the bbox, tagged `(bbox-only)`, and nothing else; never CONSISTENT/NO-TRACE.

Per event: counts of its runs by outcome; the event token is `NO-TRACE` if any run is NO-TRACE, else `OFFSET-NOTE` if any, else `CONSISTENT` if ALL runs CONSISTENT, else `PARTIAL` if any run has an observed edge, else `UNASSESSABLE` / `READING`. Session summary: `CONSISTENT n  OFFSET-NOTE n  NO-TRACE n  PARTIAL n  UNASSESSABLE n  READING n  (of N events)`; VERDICT line: `NO-TRACE on n event(s) — labels claim changes the pixels do not contain` ⇒ FAIL; else `NO FAILURE FOUND (c consistent, o offset-notes for human review, p partial, u unassessable, r readings)` — never the word PASS.

## 2. Attribution and coverage rules (closes 079-08 F1–F5)
- **Per-pair masks (F2):** every compared frame pair (t−1, t) uses the masks of BOTH frames; the region is their union (a moving target is compared over both silhouettes). A missing, empty, or unreadable mask on either frame of any pair in the window ⇒ that edge is `UNASSESSABLE(mask missing at frame t)`. For NO-TRACE the requirement extends to every frame of the span.
- **Identity (F5):** the label's `mask_value` must be present in the mask PNG of every used frame; an explicit non-zero id absent from a frame ⇒ `UNASSESSABLE(mask id v not present at frame t)`. The sole-value fallback is allowed ONLY when the label's `mask_value` is 0/unresolved, and then the observation is tagged `(mask id unresolved — sole value used)` and the run can be at most OFFSET-NOTE/CONSISTENT, never NO-TRACE.
- **Lighting / occlusion / animation (F1):** not attributable by this method — that is why CONSISTENT and OFFSET-NOTE carry the "does not establish cause" wording and why no verdict exists. Add one diagnostic to help the human: for each TRANSITION, also print the change fraction of a ring around the mask at the same frame (`ring d=…`); a high ring change on the same frame is printed as `note: whole-region change (lighting/camera?)`. Diagnostic only — no rule depends on it.
- **Association across runs and edges (F4):** a detected transition frame may serve at most ONE edge. Search windows for consecutive edges are bounded by the LABELLED frames only (no clipping by detected frames); if two edges' windows both select the same transition frame, BOTH become `UNASSESSABLE(transition k claimed by two edges)`. Bbox-only runs impose no bounds on masked runs.
- **Aggregation (F3):** an edge counts as observed only with a TRANSITION or NO-TRANSITION; UNASSESSABLE never contributes agreement; a run with NO-TRANSITION on one edge and TRANSITION Δ=0 on the other is `PARTIAL` (not CONSISTENT) — NO-TRANSITION at an edge is only meaningful inside NO-TRACE's whole-span condition.
- **Existing rules kept:** local baseline per edge; regional-motion reading `m_edge` with the cap now meaning "assessable" (`m_edge > cap` ⇒ `UNASSESSABLE(regional change m=… > cap)`); unsatisfiable threshold ⇒ UNASSESSABLE; whole-frame region ⇒ UNASSESSABLE; dominance required for a TRANSITION (non-dominant ⇒ `UNASSESSABLE(ambiguous: best=… second=…)`); frame coverage (RGB) required.

## 3. Scorer v3 (closes F6, adapts to semantics)
Run-level eligibility from the gate (a READING/UNASSESSABLE run contributes no cells). Classes per cell: `recovered` (TRANSITION at the expected Δ), `wrong` (TRANSITION at another Δ), `no-transition`, `unassessable(<reason>)`, `unsupported(<reason>)`. Cap derivation unchanged in mechanism, re-run on the mask-mode cohort; header wording: "two-significant-figure cut-off strictly below the smallest observed wrong-cell m_edge (N cells = perturbation cells, not independent edges)"; "three masked sessions", not four.

## 4. Wording (closes F1/F2/F5's doc claims, the four-sessions error, and G263)
- README Step 6 rewritten under the new semantics: what CONSISTENT means and does not mean; what OFFSET-NOTE asks a person to do; what NO-TRACE proves; that no output certifies a label as correct; M3 per-frame `observable` flags are the producer's own evidence and are not confirmed by this tool.
- Header: same; remove every "attribution"/"masks resolve attribution" claim; G263 corrected by dated append ("masks identify the pixels, not the cause — no verdict; consistency and readings only"), new G-number for the F4 lesson (a transition serves one edge).
- Section G card note updated to the new outcome names; the historical predeclared reading stays as history.

## 5. Predeclared readings (write these into the predictions file first)
P1 Selftest: every 079-05/07/08 fixture with its new known answer — in particular `lighting_aligned` ⇒ OFFSET-NOTE (or CONSISTENT if the lighting step is on the same frame as the label) with the ring note, never FAIL; `lighting_late_onset` ⇒ OFFSET-NOTE, never CONSISTENT; `occluder_aligned` ⇒ CONSISTENT or OFFSET-NOTE, never FAIL; `missing_mask61` ⇒ UNASSESSABLE(mask missing at 61); `no_detected_end` ⇒ PARTIAL; `foreign_mask_value` ⇒ UNASSESSABLE(mask id 222 not present); the F4 `15→16` ordering fixture ⇒ both competing edges UNASSESSABLE(claimed by two edges) or correct Δ, never a wrong Δ; `mixed_run` ⇒ READING and zero scorer cells; the old NOT-VISIBLE synthetic cases ⇒ NO-TRACE (they satisfy the whole-span condition) — and one new fixture where the span has a mask missing on ONE interior frame ⇒ UNASSESSABLE, not NO-TRACE.
P2 Pinned bench ×6: all 4 events CONSISTENT, `NO FAILURE FOUND (4 consistent …)`, `tau=0.0040`, 64 edges observed, exit 0.
P3 `A2L_LEGA`, `LYRA_SMOKE_01`, `A1L_LEGA`: no NO-TRACE; outcomes as the data dictate — print the full per-edge lines; `M50L_LG9`: 6 READING, `UNREAD-BBOX-ONLY`.
P4 Scorer v3 tables on the three masked sessions, immutable denominator first; cap re-derived.
P5 Batch unchanged (collision + exit contract tests still pass; exit 2 only from NO-TRACE).
P6 Claude Code's review will run its own fixtures; predeclare that any fixture with accurate masks and known RGB timing must yield CONSISTENT / OFFSET-NOTE / PARTIAL / UNASSESSABLE / NO-TRACE per §1 — and that a wrong NO-TRACE (pixels did change on the target within the span) is the only remaining P1 class.

## 6. Report
`_reviews/079-09-codex-implementation-report.md`: per-P readings with exact lines; every 079-08 finding → what changed, where (file:line), the fixture that proves it; commits; branch SHA; the cost/time you spent (the owner is measuring). End with one line: `VERDICT: GREEN` / `NEEDS-DECISION` / `RED`.
