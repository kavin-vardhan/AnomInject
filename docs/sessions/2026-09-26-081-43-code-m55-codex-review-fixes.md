# 081-43 — m55: Codex 081-42 findings fixed (F1–F6, N1, N2, oracle scope note), proven both ways, whole bank replayed (Claude Code, Opus 5.5, 2026-09-26, bench-free)

## 1. Goal

Chat's disposition `_reviews/081-43-chat-ruling-codex-review-disposition.md` of Codex's review `_reviews/081-42-codex-review.md`
(CHANGES REQUIRED; no C++ defect): fix the six P2 findings and three P3 notes as ruled, prove every fix both ways, replay the oracle over
every banked m55 session and every legacy comparison and G270 evaluation from the raw bank, re-run each AMENDMENT 1–6 proof, grep the
changed client text. Bench-free: no game, build or capture; no `Source/` change. Worked from a scratch worktree `D:\IntrusiveAnomalies\_wt_081-43`
on `feat/m55-change-evidence` (`eafa654` == origin). Evidence under `_reviews/081-43-evidence/`.

## 2. What was done

### F1 — a contradicted `empty_region` refusal fails

- **Oracle** (`tools/verify_capture.py`): an `empty_region` refusal whose delivered mask holds target and control pixels is counted in a new
  summary key `empty_region_disagrees` and makes the exit **1** (printed `MISMATCH DISAGREES si=… tag=…`). A missing or all-zero mask PNG
  stays **unverifiable** (the writer writes no PNG for an all-zero mask); the summary also carries `empty_region_agrees` / `_unverifiable`.
- **Harness** (`_reviews/081-23-lib.py`): `oracle_disagreement_error(osum)` is read by **every** gate that accepts `empty_region` or reads
  the oracle — `lyra_g270_eval` (a new first branch **FAIL**, above NEEDS-DECISION), `measure_eval` (the `empty` case), `lyra_twin_eval`,
  `nohold_eval` and the legacy evidence-ON legs. Each rejects on the count itself, not only through the exit code. The lib now runs the
  new oracle from a frozen copy `_reviews/081-43-frozen/verify_capture.py` (sha `b399e884…`, asserted in `run_oracle`); the old lib keeps
  `081-23-frozen` (`7f544896…`).

### F2 — per-leg invariants before any excusal; candidate-only singletons are NEEDS-DECISION (revises 081-35's G297 ruling)

- `leg_invariants` runs on each leg's **raw** reading and flat paths, before any coalesced / M47 / tag-bijection view and before any noise
  excusal. Nine invariants, each read from the source and surveyed on the whole bank before being gated on (section 4). A violation is
  **FAIL** and outranks NEEDS-DECISION (a proven defect outranks an open question).
- `candidate_singletons`: a self-excused path singleton on a candidate (non-A-side) leg that is outside the frozen historical run-unique
  set, outside the 081-22 timing-sampled set and **not resolved by an invariant** is NEEDS-DECISION. "Resolved by an invariant" is narrow:
  a `target_pixels` or `bbox_drawn_px` path of a row that has a mask PNG, when every invariant holds on every leg — the value is then a
  derived copy of its own decoded mask, and the decision rests on the mask path, which is evaluated separately (a mask difference only the
  candidate shows is itself a singleton and NEEDS-DECISION). Field-level singletons: resolved only when every covering path is resolved;
  `mask_ties` (no path) only through INV-9. The frozen sets, the M47 and A/B/C excusals and the G297 recurrence report are unchanged.
- **Verdict precedence:** invariant FAIL > the earlier-ruled NEEDS-DECISION items (coalesced and M47 caps, the G297 tripwire) > FAIL >
  candidate-only singleton NEEDS-DECISION > PASS. The candidate rule never lets a singleton PASS and never masks a proven FAIL; the first
  version put it above FAIL, which turned ten AMENDMENT 6 negatives from FAIL into NEEDS-DECISION (caught by the proof re-run, section 3.1).

### F3 and the oracle scope note — input classification and contradictory measured rows

- The sidecar is read at an input boundary (`_oracle_read_sidecar`): each line is decoded as UTF-8 and parsed; a malformed line or a
  non-object line is **uninterpretable** with its line number. A measured row whose pair ids, `mask_value` or `tau_px` are not integers,
  whose `receipt` is not an object (`[1]` and `[]` included) or whose `receipt.rect` is not an array, a non-boolean `chg_measured`, or a
  row the recomputation raises on, is uninterpretable too. A `tau_px` outside 0–255 is a mismatch (it used to read UNAVAILABLE, exit 0). **Precedence 1 > 3 > 0**; the summary line always carries `mismatched` and `uninterpretable`; an
  `EXIT n` line states why. `main()` catches everything: an internal error is exit 3, never a traceback; a failed `--oracle-json` write is
  3 unless the result is 1. A well-formed session with nothing compared stays 0.
- Contradictory-field checks on every measured row, each a mismatch (summary `contradicted`): `reason` not null; `chg_eligible` or
  `pair_valid` not true; `chg_hist` / `chg_sum` / `ctl_hist` / `ctl_sum` absent; and (added, see section 5) `expected_prev_session_index`
  ≠ `session_index` − 1. A contradicted row whose images are unavailable is still a MISMATCH, with the unavailability shown.
- The module docstring, `--help`, the field reference and the readme state the scope: arithmetic and transport only — not renderer
  pairing, not phase-reference selection, not the full schema.
- Selftest 24 → **35** cases; `tools/test_verify_capture_change_oracle.py` 9 → **19** tests (the five ruled F3 cases through the CLI with a
  no-traceback assertion, wrong-type scalars and an out-of-range `tau_px`, F1, a contradictory row, an internal error, a nothing-compared
  session). `test_verify_capture_consistency.py`
  25/25, `--selftest` and `--label-pixel-gate --selftest` (98) unchanged. The stripper would change nothing in either tool file.

### F4, F5, F6, N1, N2 — documentation (`docs/client-readme.md` §9, `docs/change-evidence-fields.md` front section)

- **F4** the control excludes the target's own pixels but **can include** light, shadow and reflection spill; compare, never subtract,
  never read cause (readme §9.2; field reference "The measurement").
- **F5** refused rows: the target (`chg_*`) and reference (`ref_gt8`, `ref_mean`) statistics are sentinels; control statistics (on
  `empty_region`), `ref_session_index`, `prev_target_pixels`, running phase maxima, latency and bookkeeping can be real (readme §9.1 and
  the `ref_*` table row; field reference intro and "The closed refusal vocabulary", with the `ctl_n` 921,600 / `ctl_gt8` 4,579 example).
  The refusal survey behind it: on the bank, only `empty_region` rows carry `ctl_*`; `ref_session_index`, `prev_target_pixels`, the
  maxima and latency appear on every refusal class.
- **F6** cost and memory rewritten as a **dated result on one scene and machine**: static scene, `solid_swap`, `2 4 16 4 0`, 600-frame
  captures, steady si 60–599, four per side ABBA, run log off, pacing configured 30 fps / observed arm rate ≈ 25.07/s; statistic = the
  one-sided 95 % upper bound on the mean game-thread CPU-cycle-equivalent Δ per engine frame, **−0.0718 / +0.1100 ms**, blocked time
  excluded; 120/120 required pairs and 0 drops on every ON capture; writer "no detected loss of throughput at this load"; pacing-off stress
  kept; no structural guarantee of yield, no memory bound but the 256 MiB cap; the peaks are fixture-specific and a second test game reached
  134.8 MB (`M55B3R23_L4_LYRA_ON_A1`).
- **N1** the delay=3 sentence is split: adjacent windows 0–2 at most 63 of 66,837 (under 0.1 %); window 3 all 66,837 in both comparisons;
  `ref_*` on windows 1–2 had accumulated 156–819 (up to ≈ 1.2 %). Read from both `M55B3R23_C2_DELAY_{N,S}_A1` banks.
- **N2** `change_teardown_flush` removed from the "absent when zero" list (it is in the counter initialiser, `AnomalyChangeStage.cpp:78`).

## 3. Proofs and replays (ruling "Replay requirements" 1–4)

| # | What | Result | Evidence |
|---|---|---|---|
| 1 | Old vs new oracle over **every** banked m55 session folder (476 = 326 session names, alias and `_tryN` copies) | **476 × 0 → 0**, no exit change. Both read 4,981 measured rows compared and matched, 4,958 reference comparisons matched; new: 0 contradicted, 0 uninterpretable, `empty_region` 0 agree / **0 disagree** / 83 unverifiable | `replay-oracle-bank.json` |
| 2a | Every legacy comparison rebuilt from the raw bank (081-12 MAIN, CB; 081-22 D MAIN-N, MAIN-S; 081-22 C reading; c1 CB-N, CB-S, MAIN-N, MAIN-S with as-run priors) | **9/9 verdicts unchanged** (8 PASS, 081-22 C FAIL as in 081-35's pre-check — not a legacy_compare group). **0 invariant violations**; 0 candidate-only singletons; the one build-3 singleton (MAIN-S `coverage_pct`) resolved as timing-sampled | `replay-legacy.json` |
| 2b | Every G270 evaluation (c3 `C3_LYRA_G270_N_A1`; c0 L1, L4 and 081-17 `LYRA_G3_N_A2` as evaluated in the 081-27/081-28 proofs) and every other requalification evaluator that reads the oracle (62 stack legs c1+c2, 2 Lyra twins, 2 NoHold) | **70/70 verdicts unchanged**, oracle exit unchanged on all; the only recorded-vs-replay difference is C1_G8_N_A1's original FIXTURE-VALID-FAILURE, re-evaluated PASS under AMENDMENT 1 (as recorded) | `replay-gates.json` |
| 3 | Negatives both ways (Codex's F1/F2/F3 reproductions; each invariant; the singleton rule; gate consumption) | **38/38 as expected**; guarded bank sources byte-unchanged | `negatives.json` |
| 3 | AMENDMENT 1–6 proofs under the pre-081-43 and the 081-43 lib | see section 3.1 | `amproofs.json` |
| 4 | Client-facing identifier grep over the added lines of the readme and the field reference's client section | **0 hits** over 137 lines (65 + 72; the 13 lines added to the internal notes are excluded, they are internal); positive control 4/4 | `doc-grep.json` |

Negatives in detail (old lib + old oracle / new lib + new oracle):
- **F1** Codex's mutation of `M55B3R23_C3_LYRA_G270_N_A1` (si 21, window 3, `stuck_low_mip@4064` → `empty_region`, phase and summary
  counters adjusted): audit clean; **G270 PASS / FAIL**; oracle exit **0 / 1** (`DISAGREES si=21 tag=213 target 184933 / control
  1888667`). The same mutation with frame 21's mask PNG absent: oracle 0 / 0, unverifiable 5, disagrees 0. With the oracle stubbed to exit
  0 but `empty_region_disagrees` 1, `measure_eval` (C2_EMPTY_N), `lyra_twin_eval`, `nohold_eval` and `lyra_g270_eval` each PASS / reject.
- **F2** Codex's MAINON_S2 `labels/4/anomalies[0]/target_pixels` 20,949 → 20,950: **PASS, no problem, no tripwire / FAIL** (INV-2, and
  INV-8/INV-9 with it). Each of INV-1…INV-9 fires on its own mutation (PASS / FAIL). A candidate-only singleton no invariant covers
  (`seconds_remaining` on MAINON_S2): **PASS / NEEDS-DECISION**; the same on the pre-m55 leg MAINON_S1, and a candidate singleton on a
  timing-sampled path: PASS / PASS. Disk copies: MAINON_S2 +1 tag pixel at si 17 (mask, label, MASK-TIE and sidecar row consistent) with
  MAINON_S3 moving one pixel in the same frame → PASS / PASS (resolved by INV-2/INV-9, the mask noise shared); the same with S3 untouched →
  PASS / **NEEDS-DECISION** (the mask difference is itself candidate-only).
- **F3** on a copy of `M55B3R23_C2_SOLID_N_A1`, through each oracle's CLI: `{bad json` only **0 / 3**; one valid measured row then `{bad
  json` **0 / 3**; a mismatch plus `{bad json` **1 / 1**; `receipt = [1]` **traceback (exit 1) / 3**; `receipt = []` **0 / 3**; the unmutated
  sidecar 0 / 0. Scope note: the contradictory row (eligible/valid false, `budget_exceeded`, expected prev 71) **0 / 1**; hist and sum
  omitted **0 / 1**; a reference index moved to an identical image 0 / 0 (the documented limitation).

### 3.1 AMENDMENT 1–6 proofs

Each proof script was copied with only its lib path and output directory rewritten (`amproofs.py`) and run today under the pre-081-43
lib (`f5876b05` + the old oracle) and the 081-43 lib (`1c4ae649` + the new oracle); cases compared one by one, and with the banked original.

| Amendment | Proof | pre-081-43 / 081-43 / original (as expected, of cases) | new ≠ pre |
|---|---|---|---|
| 1 | `081-26/g8-proof.py` | 38 / 38 / 38 of 38 | 0 |
| 1 | `081-26/resume-proof.py` on its 081-26 ledger | 7 / 7 / 7 of 7 | 0 |
| 2 | `081-27/amendment-proof.py` | 64 / 64 / 64 of 64 | 0 |
| 3 | `081-28/guard-proof.py` | 12 / 12 / 12 of 12 | 0 |
| 4 | `081-30/pp-proof.py` on its 081-30 ledger | 42 / 42 / 42 of 42 | 0 |
| 5 | `081-32/am5-proof.py` | 31 / 31 / 35 of 36 | 0 |
| 5 A.6 | `081-33/a6-proof.py` | 49 / 49 / 49 of 49 | 0 |
| 6 | `081-35/am6-proof.py` | 33 / **30** / 33 of 33 | **3** |

- **The two ledger-reading proofs** read the live c1 ledger, which has moved on (c1 completed after 081-30). Run on today's ledger they fail
  the same way under both libs (resume-proof hits its own "no copy" guard as c1 resumes into the legacy groups; pp-proof's proof 7
  finds C1_G2_S's latest row PASS, not NOT-RUN). Re-run on the ledger as it stood — c1 rows up to C1_G8_N's PASS-REEVALUATED for 081-26,
  up to C1_G2_S's NOT-RUN for 081-30 — both match their originals exactly under both libs (`amproofs-resume-as-of-081-26.json`,
  `amproofs-pp-as-of-081-30.json`).
- **am5-proof's five** are the cases 081-33 superseded (A.6: its mutants lack colour files, and the colour-pixel negative was withdrawn);
  identical under both libs, and a6-proof, which replaced them, is 49/49.
- **am6-proof's three** are exactly the G297 cases that pass a synthetic candidate-only singleton (`CB_S5` si 3 `bbox_px[0]` +1, synthetic
  order) with a reading: "is listed", "one group only (no prior entry)", "a pre-m55 leg shows the same field in a third group". Ruling
  F2.2 revokes that PASS; they now read NEEDS-DECISION, and **the recurrence tripwire's output is identical in every case** (it fires on
  `labels/*/anomalies[]/bbox_px[]` exactly where it fired before). The first run of the new lib changed 13 cases: the other ten were
  M47 negatives expected FAIL that the candidate decision had masked as NEEDS-DECISION — fixed by the precedence rule in section 2.

## 4. The invariants (F2.1)

Read from the writer (`AnomalyCaptureSubsystem.cpp` :1368–1426 mask outcome, :4022–4105 label arrays and observable, `AnomalyLabelWriter.cpp`
:120–131 `bbox_drawn_px`) and surveyed on every banked m55 session with labels (382 sessions, 59,923 rows, 45,047 entries): 0 violations
of INV-2…INV-6 and INV-8; INV-7 holds as written (≥); MASK-TIE (INV-9) on 34,210 entries in 352 + 198 logged sessions: 0 violations.
**INV-1 fired on 12 sessions, every one the G5 identity gate** (a deliberately failed mask write at si 8, in every era): the writer then
names a file it did not write and says so (`AnomalyAsyncWriter.cpp`, "TARGET MASK WRITE FAILED … names a file that does not exist"). That
is a real delivery failure, not a clean comparison, so a legacy leg carrying one FAILs; no legacy leg has one. Surveys:
`invariant-survey.json`, `inv9-survey.json`.

| Id | Invariant | Proof it fires |
|---|---|---|
| INV-1 | a non-null `mask_file` names an existing 8-bit grayscale PNG that is not all-zero, on a `present` row; a `present` row names a file | mask_file → a missing PNG |
| INV-2 | `target_pixels` = the decoded count of the entry's tag in that frame's mask; `-1` ⇒ the tag is absent from it (the ruled minimum) | Codex's +1 |
| INV-3 | rows without a mask PNG: `unmeasured` ⇒ `target_pixels` −1; `empty` ⇒ 0 or −1 | 5 on an unmeasured row |
| INV-4 | `bbox_drawn_px` = the tag's bounding box `[x, y, w, h]` in the decoded mask when counted, else null | width +1 |
| INV-5 | `observable` null ⇔ `target_pixels` −1; `observable` true ⇒ `target_pixels` ≥ `observable_min_pixels` | null on a counted entry |
| INV-6 | `target_drawn_pixels` −1 when `target_pixels` −1, else −1 or 0 ≤ drawn ≤ count | drawn > count |
| INV-7 | `target_mask_frames_measured` ≥ present rows, `_hidden_blank` ≥ empty rows, `observable_frames` ≥ observable entries, `target_drawn_pixels_measured` ≥ drawn-measured entries | `observable_frames` 0 |
| INV-8 | evidence-ON: each measured pair joins exactly one label entry (`id@start_frame`) with the same `mask_value` and `chg_n` = `target_pixels` | `start_frame` +1 |
| INV-9 | each counted entry on a present/empty row has exactly one MASK-TIE log line with tableCount = pngCount = `target_pixels` and drawnCount = `target_drawn_pixels`; the comparator's cached tuples equal the log's | log line +1; cached tuples edited |

Considered and **not** asserted (the source does not guarantee them): `target_mask_frames_unavailable` against unmeasured rows (topped up by a
run-end residual); `observable` false vs true and `frames_drawn_unexpected` (depend on the labelled state, which no artifact records); INV-7 as
equalities (fail on 24–51 identity-gate fault legs, where a processed frame's row is dropped — every violation had summary > rows).

## 5. Deviations

- **Three AMENDMENT 6 proof cases no longer read as the 081-35 proof expected** (section 3.1): they assert the PASS that ruling F2.2
  revokes. The proof file is not edited (it is a record); the change is the ruled one, and the tripwire behaves as before.
- **INV-9 and the `mask_ties` resolution were added beyond the ruled minimum.** On a MAIN group a label count difference also moves the
  MASK-TIE log field, which has no flat path; without INV-9 every count singleton would be raised twice and could never be resolved.
- **A fourth contradictory-field check** (`expected_prev_session_index` = `session_index` − 1, when present), because Codex's P3 example
  carried `71`; it holds on all 4,981 banked measured rows.
- **More F3 classes than listed:** non-integer pair ids (previously UNAVAILABLE "pair ids unreadable"), a non-integer `mask_value` or
  `tau_px` (previously a mismatch or UNAVAILABLE), an absent `receipt` or absent `receipt.rect` on a measured row, a non-boolean
  `chg_measured`, a non-UTF-8 line, a row the recomputation raises on; and a `tau_px` outside 0–255 is now a mismatch (it read UNAVAILABLE,
  exit 0 — found while reviewing the diff). None occurs in the bank. `receipt.rect` that is an array of the wrong length stays UNAVAILABLE.
- **The candidate rule covers every non-A-side leg,** which in build-3 groups is exactly the build-3 legs; in the stage-era 081-12 groups it
  also covers the stage-2 candidate (its only self-excused singletons are historical run-unique paths, so nothing changes).
- **N2 extended.** The same sentence also listed `change_pending_*` as absent when zero. `change_pending_colour_cancelled` and
  `change_pending_mask_cancelled` are added by `Diagnostic` on every issued capture (`AnomalyCaptureSubsystem.cpp:4488–4489`) and read 0 on
  104 of 106 build-3 ON summaries; only `change_pending_family_deferred` is absent until it fires. Corrected with the measurement.
- **N1 corrected a range.** 081-41 wrote "9–63" changed pixels on windows 0–2; the banks read 0–63 (N) and 2–56 (S). Now "at most 63".
- **Proof scratch on E:.** `_bench_sessions_bank` is a junction to `E:\IA_BuildCache`, so the mutated copies were built with hard links in
  `E:\IA_BuildCache\_r43_neg_scratch`; every mutated or copied (non-linked) file was mirrored to `081-43-evidence/neg/mutated-copies/` and the
  scratch deleted.
- **The prep and cost boundaries now pin a superseded lib.** `081-23-lib.py` is `1c4ae649…` (was `f5876b05…`, kept at
  `081-43-evidence/081-23-lib.pre-081-43.py`); both boundaries hash the lib, so any further requalification or cost window needs a re-issue
  first. None is planned; no boundary was re-issued.
- The oracle JSON detail gained keys (`uninterpretable`, `exit`, per-row `line`, `contradictions`); the summary kept every old key.
- **Cached readings are not re-usable under the new lib.** A legacy reading cached by a window before 081-43 carries an oracle summary
  without `empty_region_disagrees`; the new lib treats the absent count as a problem rather than as zero (strict on purpose — tolerating
  it would reopen F1). Any re-evaluation must rebuild readings from the raw bank, as `replay-legacy.py` does.

## 6. State

- `feat/m55-change-evidence` carries this commit (tools + docs, pushed). Build 3 `85c99b3` (Game `002805CF`) unchanged; **no `Source/`
  change**. The main checkout is still `m51` `53bf725`; `master` `031a103`, tags, containers, CaptureBench and `ToCodex\` untouched.
- NOT MERGED, NOT TAGGED. **Pending chat's review of these replays → comment strip → build checks → no-ff merge.**
