## Dated amendment 081-17 (2026-09-24): build 2 — 256 MiB payload cap + ownership census — written and committed BEFORE the build and BEFORE any build-2 result

Authority: `_reviews/081-16-chat-ruling-memory-criterion-and-handover.md` (read in full) and brief
081-17R. Implementer from this round: Claude Code (Opus 5.5); Codex's m55 implementation ledger closed
at 081-16. Codex's 081-16 source counterexample is accepted: `11C + 9M` is **not** a structural bound
on 5649a6d (one closing phase 3..18, delayed head, completions 4..15 before 5 s retains 13C + 16M =
62,668,800 B at 720p / 141,004,800 B at 1080p with every reservation legal). **No structural bound is
claimed anywhere in this amendment.**

### A. Memory criterion — supersedes every earlier budget / high-water predicate

1. **Hard payload cap:** compiled default of `IAI.Capture.ChangeMaxBytes` = **268,435,456 (256 MiB)**
   (was 67,108,864). The byte cap stays the only admission limit. `ReserveLocked`, the completion
   clock (8 later completions / 5 s wall / 4-completion gap outside closure) and every refusal
   semantic are exactly 5649a6d's.
2. **Scope:** the cap bounds **m55's admitted payload only** — colour allocations (a shared
   phase-reference owner counted once) and retained masks reserved by the change stage. It is not a
   process-RAM ceiling: the writer pool, receipts, rows, compression and metadata are outside it.
3. **Gate on every qualification leg:** **zero `budget_exceeded` anywhere in the run** and **full
   required-pair yield** wherever 081-14 / 081-15 demand it. There is no structural guarantee of full
   yield at any resolution; a leg that refuses is a valid failure, never retried.
4. **High-water is an explained reading, never a gate.** No bound, no `change_bytes_closure_bound`,
   no formula-based WARN. The only numeric memory check left is the pre-existing auditor invariant
   `change_bytes_high_water <= change_max_bytes`, which admission makes true by construction.
5. **Superseded, prospectively, historical text and readings untouched:** 081-15 Ruling 1b (bound,
   WARN, "high-water <= logged bound" — never implemented); the 081-15 amendment's "64 MiB is the sole
   admission cap"; the field reference's 64 MiB wording; the "64 MiB resolution envelope"
   carry-forward, which becomes Ruling 2's payload-only statement. The 081-15 E8E2E43F readings keep
   their binary and are credited to nothing on build 2.

### B. Build 2 = the 256 MiB default + diagnostics (i)/(ii), nothing else

- **(i) At every budget refusal** — a failed colour or mask reservation in `Colour()` / `Mask()`,
  including bench gate 10's forced colour refusal — one `Capture(m55): BUDGET-EXCEEDED` log line and a
  `budget_census` array on the refused index's sidecar pair row(s). Only indices that own window rows
  have sidecar rows; for any other index the log line is the record.
- **Census fields:** `kind` (`colour` | `mask`), `si`, `requested_bytes`, `max_bytes`, `bytes_held`;
  `colours` = distinct admitted colour allocations, split `colours_pending` / `colours_previous` /
  `colours_phase_ref` / `colours_in_hand` with priority pending > previous > phase_ref > in_hand, so an
  allocation shared by Previous and a phase reference is counted once; `in_hand` is the serial
  worker's current item (moved out of Pending while it is finalized and measured); `colour_bytes`;
  `masks`, `mask_bytes`; `unaccounted_bytes` = bytes_held − colour_bytes − mask_bytes; `cursor`,
  `latest_index`; `open_phases` [{event, ordinal, first, last, closing}]; `head` {si, state
  `pending` | `in_hand` | `none`, `waits_on` ⊆ [colour, mask, observed, sealed]}.
- **(ii) At every new high-water** the same census is recomputed **after** the admitted buffer's
  ownership is recorded and kept as the peak census. A `Capture(m55): HIGH-WATER` line is logged only
  when the peak has risen by at least one colour allocation (the first admitted colour's size) since
  the last such line; one `Capture(m55): HIGH-WATER-PEAK` line at each closure. The run summary adds
  exactly one key, `change_bytes_peak_census` (object: the census fields plus `high_water`).
- Field reference updated for these fields. No other source, schema, clock, admission, enum, writer,
  identity, observable, shader or cook change.

### C. Build-2 predictions (readings are predictions, not gates)

- **Build:** one set — StackOBot Game, StackOBot Editor, LyraEditor — all exit 0. The A44 scan of the
  staged exe finds the new UTF-16 tokens `BUDGET-EXCEEDED`, `HIGH-WATER-PEAK`,
  `change_bytes_peak_census`, `budget_census`, and an invented control token is absent.
- **Every leg's EFFECTIVE line:** `maxBytes=268435456(from compiled)`.
- **Step D — Stack 1080p twins, both orders, 4 legs, fail-fast:** exactly 081-15's `null1080` /
  `solid1080` recipes (CB_GateLevel 1920x1080, 90 frames, seed 777, Config 2 4 16 4 0, target
  StaticMeshActor_49, SVE 1 / Delivery 0 / Pace 1 / Fps 30 / AA 0 / AE off, ChangeEvidenceCases 1,
  ChangeGate 14, input lock + placement + B1 with the declared 1080p→720p normalisation), in the order
  null N, null S, solid N, solid S. Only the binary differs. Up to 3 attempts per leg for
  fixture-invalid captures only; the first fixture-valid capture is the reading.
  **Pass per leg:** 20/20 required window pairs measured; exactly 5 final events;
  `change_reason_budget_exceeded == 0`, zero `BUDGET-EXCEEDED` lines, zero rows carrying
  `budget_census`; invariants 0 (retained 0, high-water <= max, denominator / histogram / ref-onset
  mismatch 0, persist_failed 0, late_record_mutations 0, rows_dropped 0); 081-15's retained twin
  checks (>= 3 measured onsets; null onset gt8/n < 0.1, solid > 0.9). **The first fixture-valid failure
  stops the round (NEEDS-DECISION) and step E does not run.**
- **Expected high-water (prediction only):** 1080p twins **67–120 MB, central ~85 MB**, peak census
  dominated by pending masks ahead of a late head (≈ 10–17 masks) plus 3–7 colour allocations
  (previous, one phase reference, out-of-order completions). Expected zero refusals: the source-legal
  single-phase state 13C + 16M = 141.0 MB fits 256 MiB — which is **not** a bound. Latency readings as
  081-15 (p50 400–550 ms, p95 <= 650 ms).
- **Diagnostic self-check (reported, not a product gate, does not stop the legs):** peak census
  present on every leg; `peak.high_water == change_bytes_high_water == peak.bytes_held`;
  `unaccounted_bytes == 0`. A failure is a defect in build 2's diagnostic and makes the round
  NEEDS-DECISION at report time.
- **(i) is expected NOT to fire in D or E.** Its runtime proof is therefore **UNEXERCISED this
  round**; it rides the next brief's G10 (forced refusal) and `ChangeMaxBytes 1` controls, both
  orders, which must show the line and the row field. A zero `BUDGET-EXCEEDED` count here is a
  reading, not proof the line can fire (G96); the zero-refusal gate rests primarily on the
  pre-existing `change_reason_budget_exceeded` counter, which fired 3–6 times per 1080p twin on
  E8E2E43F.
- **Step E — Lyra G270, 300 frames:** A2's exact CLI (`081-14-evidence/command-LYRA_G3_N_A2.json`)
  with `-MaxFrames 300` and the build-2 Lyra modules; 081-14 fixture sequence unchanged (input lock,
  45 s settle, one L2 placement, independent L1 within 150 cm / 10°, respawn audit pre and post,
  foreground throughout). Fixture allowance 3; the first fixture-valid capture is the reading, never
  retried. Branches (081-14 §3.4; "measured" includes zero `budget_exceeded` in required windows):
  **PASS** = >= 3 events, every final phase `pairs_measured == pairs_required`, every window row
  measured, invariants 0. **OBSERVED-BELOW-EVENT-FLOOR** = fixture-valid, >= 1 event with >= 1 phase,
  every phase fully measured, < 3 events (does not hold Stage 3). **G270 DROPPED for m55** = 3
  fixture-invalid attempts (UNRESOLVED observation). **NEEDS-DECISION (stop)** = fixture-valid and any
  required pair refused by `closure_timeout` or `budget_exceeded`; declared conservatively here, a
  required pair refused for any other reason, or zero events / zero phases (a vacuous reading, G146),
  is also not a PASS and stops. Expected: 3–5 events (A2 gave 2 in 150 frames), zero budget refusals,
  zero closure_timeout, high-water 60–140 MB (prediction only).
- **Harness:** Codex's 081-15 runners are reused under 081-17 names; only the evidence root, the bank
  prefix (`M55S2R17_`), the binary manifest and the added budget / event / census predicates change.
  A harness self-fault before sampling is HARNESS-INVALID, declared, not counted, at most two fix
  iterations per step.
- **Inventory after this round:** of Codex's 75 candidate legs, this round runs 4 + G270; 70 remain
  for the next brief (66 inventory + 4 count/wall can-fails), all on build 2. Stage 3 stays HELD.

