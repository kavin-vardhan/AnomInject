# 081-23 predictions — m55 build 3 requalification campaign (Claude Code, Opus 5.5)

Written 2026-09-24, bench-free, before any leg of this campaign. Authority: `_reviews/081-22-chat-ruling-legacy-d-accepted-requal-prep.md`
(governing; timing-sampled set), `081-16-chat-ruling-memory-criterion-and-handover.md` (memory gate), `081-16-codex-budget-bound-prebuild-review.md`
§4 (the 75-leg inventory), `081-13`/`081-14` rulings (NoHold G2 pose policy, G270 §3.4 branches), `081-21` ruling and this plan's dated
amendment 081-21 §D (gate-device consequences on build 3). No build, no source change, no leg ran while this was written.
Missed predictions will be reported as missed, never refitted.

- **Build under test:** build 3 = source `85c99b3` (+ oracle `c27024d`), Game exe **002805CF** (built = staged = `_binary_baselines\StackOBot.exe.m55-build3-002805CF`),
  Lyra modules `m55-build3-lyra-cf0b38cb`. **Legacy A-side:** pre-m55 `_binary_baselines\StackOBot.exe.m52-veto-arm-window-0844220E` (source = master `031a103`).
- **Oracle:** `tools/verify_capture.py --change-oracle`, frozen byte-for-byte from this branch into `_reviews/081-23-frozen/verify_capture.py`
  (git blob and SHA-256 in `_reviews/081-23-evidence/harness-derivation.json`).
- **Harness:** `_reviews/081-23-*` (derivations by asserted single-occurrence replacements from the 081-22 / 081-13 runners, recorded in
  `harness-derivation.json`; evaluators in `081-23-lib.py`; one driver per chunk `081-23-chunk0..3.py`; orchestrator `081-23-window.py`).
  Every harness file's SHA-256 is fixed in `_reviews/081-23-evidence/prep-boundary.json`; a window's preflight refuses to start if any changed.

## 0. Validity, stop and retry rules (fixed now)

- A window starts only after two consecutive quiet checks (no `UnrealEditor` of any engine, no game process, no UBT/SCW of ours), up to 9 min
  of waiting per invocation; otherwise the window is not used and nothing ran.
- Every runner re-checks for a foreign `UnrealEditor` at startup, before `capture_start` and once per second during capture (legacy/NoHold
  runners: in the session-wait loop). A hit stops our PID only and marks the leg **INVALID-FOREIGN**; the chunk stops and resumes at that leg in
  a later window (ledgers are resumable; decided legs are never re-run).
- Pre-capture fixture failures (focus, lock/placement, B1/L1/L2, startup) and whole-leg foreground loss are **INVALID** and retried, at most
  **3 attempts per leg**. Three invalid attempts: **NOT-RUN** (stop the chunk), except Lyra G270 → **DROPPED/UNRESOLVED, non-blocking** (081-14 §3.4).
- **Fail-fast:** the first **fixture-valid** failed gate stops the chunk (NEEDS-DECISION). A valid failure is never retried. Readings are never gates.
- Staged exe is re-hashed before every leg (002805CF), swapped only to the pre-m55 archive for legacy A-side legs and restored in a `finally`.

## 1. Inventory — 92 legs (chunk 0: 6 readings; requalification: 86)

Codex's 75 (081-16 §4) adjusted: **−1** (Stack 1080 `solid1080` natural = 081-22 (a), already PASS on build 3), **+4** (G8 and G9 both orders:
build 3 changed exactly the family/attach code they exercise — `AnomalySceneViewExtension.cpp` gate 8 and `AnomalySveCapturer.cpp` gate 9 — and
§D predicts a changed G8 outcome), **+8** legacy (the brief asks for both tick orders and the evidence-ON halves; 081-22 (d) supplies MainWorld
evidence-OFF both orders and is credited, not re-run). 75 − 1 + 4 + 8 = **86**, plus the 6 long-run readings.

| id | host / recipe | order | frames | evidence | proves | pass rule | compared against |
|---|---|---|---|---|---|---|---|
| C0 L1–L4 | Lyra `L_ShooterGym` 081-17R G270 CLI, 1080p | N | 1800 | ON, OFF, OFF, ON | long-run writer latency, retention growth, ON vs OFF | **readings only** — 081-18 rules R1–R4, predictions P1–P12, P20 unchanged | ON vs OFF (ABBA) |
| C0 S1–S2 | Stack CB 081-17R `solid1080` CLI | N | 1800 | ON, OFF | same on the light host | readings — P13–P19 | ON vs OFF |
| C1 G8 ×2 | Stack CB, `corrupted_texture` actor49, Config `2 8 16 4 0`, gate 8 | N, S | 90 | ON | throwaway foreign family under the legacy-exact rule | M-common + §D: SI8 `unsupported_delivery` (no receipt), SI9 `predecessor_undelivered`, `view_rejected` ≥ 1, `throwaway_family_constructed` 1, SI8 PNG absent | §D prediction |
| C1 G9 ×2 | same, gate 9 | N, S | 90 | ON | receipt geometry refusal after the AfterPass rewrite | M-common + SI8 `extent_mismatch` | — |
| C1 G2 ×2 | same, gate 2 | N, S | 90 | ON | dropped arm, mask now served legacy-style | M-common + arm_dropped 1, predecessor_missing > 0, §D: SI8 `out_of_order_timeout`, SI9 `predecessor_missing` | §D (pre-build-3 shape SI9 `mask_payload_missing` must not recur) |
| C1 G15 ×2 | same, gate 15 | N, S | 90 | ON | unserved-arm reset via the mask-path bench hold | M-common + 3 cancel/defer counters > 0, SI8 `closure_timeout`, SI9 `epoch_reset`, 89 labels | — |
| C1 LOW ×2 | Stack CB `corrupted_texture`, `2 4 16 4 0`, `IAI.Capture.ChangeMaxBytes 1` | N, S | 90 | ON | **refusal census (diagnostic i) at runtime — never run** | refusals > 0; high-water 0, retained 0; ≥ 1 `BUDGET-EXCEEDED` line and ≥ 1 row `budget_census`; every census entry closed (bytes_held 0, unaccounted 0); EFFECTIVE `maxBytes=1(from console)`; oracle exit 0, compared 0 | 081-05 LOW control |
| C1 G10 ×2 | gate 10 (forced capacity at SI 8) | N, S | 90 | ON | refusal census on a real forced refusal | M-common except refusals exactly at SI 8; SI8 `budget_exceeded`; census shown and closed | — |
| C1 COUNT/WALL ×4 | Stack CB `null_effect` `2 4 2 4 0`, gate 16 / 17 | N, S | 90 | ON | completion clock can-fails (count path, wall path) | 081-15 predicates (count: SI3 `closure_timeout` via `clock=completions` ≥ 8 < 5 s; wall: `clock=wall` 5–10 s, 0 completions, nothing measured) + M-common | 081-15 A-side |
| C1 1080 ×3 | Stack CB `null1080` N/S, `solid1080` S, `2 4 16 4 0`, 1920×1080 | N,S / S | 90 | ON | full yield under the 256 MiB cap | M-common + **20/20** required pairs, 5 events, onset separation (null < .1, solid > .9) | 081-17 build-2 twins |
| C1 legacy CB-N, CB-S | Stack CB `blinking` actor49 seed 777 `2 4 8 4 0` 720p AA0 | N / S | 90 ×6 each | pre, OFF, ON, ON, OFF, pre | legacy never depends on m55 (ON and OFF) | L-rules (§2.6) with CB field set (occupancy, own-tag) | pre-m55 0844220E |
| C1 legacy MAIN-ON-N, -S | MainWorld `blinking` SM_rock_02 seed 777 `2 4 8 4 0` 720p AA0 | N / S | 90 ×4 each | pre, ON, ON, pre | evidence-ON legacy identity on MainWorld | L-rules with MAIN field set (mask bytes) | pre-m55 0844220E |
| C2 static ×22 | Stack CB: null, solid, delay 3, hide, hide_depth, occluded (`empty`), blink (360 f), short, drop (45 f), run-end (20 f), teardown (at 30) | N, S | per 081-15 | ON | Stage-2 measurement/finalisation on build 3 | M-common + the 081-15 case predicate of each recipe | 081-11 accepted legs (readings only) |
| C2 moving ×2 | Stack CB motion scene null / solid | N | 90 | ON | camera-motion caveat, 90 % `cam_moved` | M-common + motion ≥ 90 % | 081-12 |
| C2 G1,3,4,5,6,7,11,12,13 ×18 | Stack CB identity gates, `2 8 16 4 0` (G11: 9 frames, floor 1) | N, S | 90 | ON | order / delivery / coalescing / epoch / late / stale / gap | M-common + 081-15 identity predicate of each gate (unchanged by build 3 per §D) | Stage-1 V6 (predicates only) |
| C3 NoHold ×2 | MainWorld `stuck_low_mip` SM_rock_02 `StuckMipNoHold 1` | N, S | 90 | ON | zero-labelled deferred onset | G2 pose policy (1 cm / 0.1°, join session_index N / frame_index S vs `M52E_M52E_G2_CANFAIL`), ≥ 3 fires, `no_labelled_frames`, zero pairs, 0 injected frames, bias suppression line, zero budget refusals, oracle exit 0 compared 0 | 081-13/081-14 |
| C3 Lyra G270 | Lyra 081-17R CLI, 300 frames, 1080p | N | 300 | ON | observation under full-yield bar | 081-14 §3.4 branches + zero `budget_exceeded` anywhere + census closed + oracle exit 0 mismatched 0 | 081-17 build 2 (PASS) |
| C3 Lyra 720 twins ×2 | Lyra `null_effect` / `solid_swap`, `2 4 16 4 0`, 720p | N | 90 | ON | the accepted scoped Lyra twins on build 3 | 081-08 audit (normal) + zero refusals + census closed + oracle compared = measured, mismatched 0 | 081-14 (A699) |

**Dropped:** none. **Not added:** G14 (the healthy control is every measurement leg), Lyra G1–G12 (DROPPED-BY-OWNER-DECISION 081-07),
Stage-3 cost legs (brief: do not start).

## 2. Pass rules

### 2.1 M-common (every Stack measurement leg; implemented in `081-23-lib.measure_eval`)

1. **Invariants** = the unchanged 081-08 audit (`081-08-audit.py`): stage version 2, retained bytes 0, high-water ≤ cap, denominator /
   histogram / ref-onset / persist / late-mutation / rows-dropped counters 0, no duplicate or unordered pair, every phase's required rows present,
   onset never repaired, measured pairs adjacent with matching tokens/family/epoch/geometry, histogram conservation, legacy denominator join,
   event floor (3; recipes with a declared floor 1 keep it). The "no measured window pair" diagnostic is expected only on `empty`, `wall_fail`
   and `LOW`, as in 081-15.
2. **Memory (081-16 Ruling 1):** `change_reason_budget_exceeded` 0 and no `BUDGET-EXCEEDED` line anywhere in the run; exceptions are exactly
   G10 (refusals only at SI 8) and LOW (refusals required). `change_max_bytes` = 268,435,456 (LOW: 1). Full required-pair yield where demanded:
   the 1080 twins (20/20) and G270. **High-water is a reading, never a gate.**
3. **Census:** the peak census is closed (peak = high-water = bytes held, unaccounted 0, unowned 0, colour split sums, colour + mask bytes =
   bytes held, exactly one `HIGH-WATER-PEAK` line equal to the summary); every refusal census (log line and row field) is closed the same way,
   agrees with its log line, and every `budget_exceeded` row carries one.
4. **Build 3:** one `EFFECTIVE` line reading `maxBytes=268435456(from compiled) gate=<requested>(from console)` (14 for non-gate legs, the
   requested gate otherwise); no `BENCH-GATE-REFUSED`, no `STAGE-DISCARDED`; `change_multi_view_families` 0, `change_unsupported_completion` 0,
   `change_view_rejected` 0 (except G8); no event record carries `late_results` (F6).
5. **Oracle** (frozen `--change-oracle`, every ON leg): exit **0**; `mismatched` 0; `compared` = measured rows ≥ 1 where rows are expected to
   measure, `compared` 0 where none is (`empty`, `wall_fail`, LOW, NoHold). Exit 1 or 3 fails the leg.
6. **Fixture validity** (decided before any gate is read): whole-leg foreground, one lock / one placement / one release with restoration, no
   handled ensure, captured-label B1 (not for moving/NoHold).

### 2.2 Case predicates (081-15 suite, unchanged; restated in `081-23-lib.CASES` / `measure_eval`)

delay: ≥ 3 window-3 transitions > 0.9 and every earlier window < 0.1 · hide_depth: drawn = count on every measured row · empty: every onset
`empty_region` with target_pixels 0 and an independent occluder trace hit, nothing measured, legacy + veto = finals · motion: ≥ 90 % of pairs
`cam_moved` · blink: a phase count > 8 truncated to 8 · short: a phase with `pairs_required` < 4 · teardown: `change_teardown_flush` 1 and a
`teardown` final · drop: a `fire_removed_or_target_lost` final · run-end: a `run_end` final.

### 2.3 Identity predicates (081-15, plus §D for build 3)

G1 two-ready reverse drain (receipt proof) · **G2** arm_dropped 1, predecessor_missing > 0, **§D: SI8 `out_of_order_timeout`, SI9
`predecessor_missing`** · G3/G4/G5 SI8 `current_undelivered` (G3/G4: SI9 `predecessor_undelivered`) · G6 served ≥ 2 with a
`mask_payload_missing` refusal · G7 epoch-rejected colour and mask drains, SI9 `epoch_reset` · **G8 (§D)** SI8 `unsupported_delivery` without a
receipt, SI9 `predecessor_undelivered`, `view_rejected` ≥ 1, `throwaway_family_constructed` 1, SI8's legacy PNG absent (fixture-only fault,
declared before build 3; the RT duplicate-claim check is UNEXERCISED on build 3) · G9 SI8 `extent_mismatch` · G10 SI8 `budget_exceeded` · G11
late results > 0, closure_timeout > 0, positive `LATE-FROZEN … mutations=0` · G12 SI8 predecessor = 6, `predecessor_missing` · G13 SI8 untraced,
SI9 `predecessor_missing`, unregistered capture 1, 90 labels and 90 PNGs · G15 as in the table. Every gate: the verbose PAIR trace finalises
every issued index.

### 2.4 NoHold, Lyra

NoHold: 081-15 `nohold.py` predicate on the 081-13/081-14 G2 policy (§1 table), plus zero budget refusals and oracle exit 0 compared 0.
Lyra G270: PASS = ≥ 3 events, every labelled phase fully measured, invariants 0; OBSERVED-BELOW-EVENT-FLOOR (all measured, < 3 events) and
DROPPED after 3 fixture-invalids are non-blocking; **any** `budget_exceeded`, any refused required pair, an open census, an oracle mismatch or
an invariant error ⇒ NEEDS-DECISION (stop). Lyra 720 twins: 081-08 audit (normal floor) + zero refusals + closed census + oracle.

### 2.5 Legacy comparisons (L-rules; `081-23-lib.legacy_compare`)

Per group (map × order): the 081-22 AMENDMENT 1 rules, plus the 081-22 ruling.
1. **Declared rule:** every JSON path (labels keyed by `session_index`, `annotation.json`, `run_summary.json`, `run.json`, mask file hashes)
   that differs between groups must also differ within a same-binary, same-evidence control pair (exact paths), **or** belong to the
   **timing-sampled set**: `annotation/anomalies[]/coverage_pct`, `run_summary/end_frame`, `capture_game_ticks`, `key_ring_published`,
   `key_ring_consumed`, `key_ring_wrapped`, `ticks_per_captured_frame` (ruling 081-22).
2. **Stable fields** (081-12 field set: per-index anomaly tuples, camera rows, onsets, label/annotation key structure, non-`change_*` summary
   keys, run keys, event ranges, MASK-TIE tuples, and MAIN mask bytes / CB mask occupancy) must be equal across groups whenever they are equal
   within every control pair; in **native order** they must be equal on **every** pair.
3. **Native strict rule:** every cross-group differing path must lie in the pre-m55 pair's own differences ∪ 081-22's measured historical
   run-unique set (19 patterns, `081-22-evidence/legacy-historical-run-unique.json`) ∪ the timing-sampled set.
4. **Key presence:** build-3 legs add no path the pre-m55 legs lack and remove none, except the **declared evidence-ON additions**: every
   `run_summary/change_*` key and the `change_evidence.jsonl` sidecar — required on ON legs, forbidden on OFF legs (OFF read back: engine echo
   `"0"`, no EFFECTIVE line, no sidecar, no `change_*` key).
5. **Per leg:** no handled ensure; every nonzero mask byte equals its row's own tag; MASK-TIE table = PNG counts; ON legs: zero
   `budget_exceeded`, `change_multi_view_families` 0, `change_unsupported_completion` 0, oracle exit 0 with compared = measured ≥ 1.
Colour frame bytes are a reading only (not reproducible run to run, G230/081-12).

## 3. Readings predicted (not gates)

- Stack 720 high-water 10–45 MB on every C1/C2 recipe (720 colour 3.6864 MB, mask 0.9216 MB; priors 13.8–19.4 MB were under the removed
  3-colour cap); 1080 twins 45–80 MB (build 2: 51.9 / 64.3 / 66.4 / 70.5 MB; 081-22 (a) 66.4 MB). G270: 120–260 MB (build 2: 197.0 MB),
  and ⚠ this is the leg most likely to meet the cap. Lyra 720 twins 15–60 MB (prior 19.4 MB under the 3-colour cap, which caused the 10/12
  refusals then; zero predicted now).
- Colour-completion latency p50: Stack 720 100–250 ms (count_fail 158 ms), Stack 1080 300–450 ms, Lyra 1080 0.9–1.6 s.
- `speed_ratio` 1.000–1.02 on Stack, 1.00–1.06 on Lyra.
- Onsets: null twins < 0.01 (Stack) / < 0.02 (Lyra 720); solid 1.0; G270 onsets as recorded by 081-17R (0.0001–0.58), no threshold.
- LOW census: every refusal `bytes_held 0`, `colours 0`, `masks 0`; roughly 2 refusals per issued index (colour + mask).
- Legacy: MAIN native 7 timing-sampled paths may differ (081-22 (d) measured exactly those); CB native: 0–7 of them; all other paths within the
  control pairs. Evidence ON vs OFF on the same binary: identical legacy fields, masks byte-identical in native order.
- Chunk 0: 081-18 predictions P1–P20 unchanged (`081-18-evidence/predictions.md`, sha256 `225cb4df…2d05`), now read on build 3.

## 4. Chunks (bench windows; resumable, fail-fast)

| chunk | legs | expected machine time | worst case (every leg 3 attempts is not budgeted; ~1.5× typical) |
|---|---|---|---|
| **C0** long run (081-18 Part 2) | 6 | ~27 min (Lyra 1800-frame leg ≈ 3 min per attempt, ~1.7 attempts; Stack 1800 ≈ 1.5 min; stage/restore, analysis) | ~45 min |
| **C1** build-3 risk first: G8, G9, G2, G15, LOW, G10, count/wall, 1080 ×3, then the 20 legacy legs | 39 | ~20 min (Stack leg ≈ 30 s incl. audit + oracle; legacy leg ≈ 20 s) | ~40 min |
| **C2** Stack 720 measurement 24 + remaining identity 18 | 42 | ~25 min | ~45 min |
| **C3** NoHold ×2 (MainWorld) + Lyra G270 + Lyra 720 twins | 5 | ~15 min (MainWorld attempt ≈ 45 s, Lyra attempt ≈ 1.6 min, ~1.7 attempts; stage/restore) | ~35 min |

Launch timings are from the banked ledgers: 081-22 legacy legs 10–18 s runner time; 081-22 (a) 25 s; 081-17 Stack 1080 19 s; Lyra 83–90 s per
attempt (081-17/081-18), 50 % first-attempt fixture-invalid on Lyra. Pre/postflight run outside the quiet window. Order: C0 (decides whether a
build-4 scheduling change is needed), then C1 (new code paths and the never-run refusal census first), C2, C3.

## 5. Dry run (bench-free, done before this campaign)

`_reviews/081-23-evidence/dryrun/dryrun-results.json`: every evaluator run on banked stand-ins (known-good must pass) and on at least one
known-bad or mutated case (must fail). 118 of 118 checks behaved as expected. Notably: 081-22 (d) native MainWorld evidence-OFF reproduces its
recorded FAIL exactly (9 paths, 3 outside the historical set) without the timing-sampled set and PASSES with it; Stage-1 G2/G8 legs fail the §D
clauses with the pre-build-3 shapes (G2 SI9 `mask_payload_missing`), as §D predicts. Leg types with no banked stand-in are listed there.

## AMENDMENT 1 — 081-26, 2026-09-25: the G8 "no receipt" clause is re-encoded; the prediction is unchanged (dated, post-hoc)

Authority: `_reviews/081-25-chat-ruling-g8-predicate-correction.md` (rulings 1–4). **Written after `C1_G8_N_A1` ran (081-25), so it is a
post-hoc amendment and counts as one in the campaign metrics.** It changes how one §2.3/§D clause is evaluated, not what it predicts.

- **The clause, as now evaluated.** §2.3's G8 clause "SI8 … without a receipt" means SI8's `receipt` carries **no delivery side**:
  `t_drain_ms == -1` **and** `view_family_id == 0` **and** `serving_token == 0` **and** `format == 0` **and** `rect == []` **and**
  `extent == []` (`081-23-lib.receipt_without_delivery`). The 081-23 code tested for an absent `receipt` key, a form the serialiser never
  writes: `ChangeReceiptJson` always writes the issue side (`run_epoch`, `cut_counter`, `capture_token`, `t_submit_ms`) and fills the
  delivery side only when a receipt was built, else the sentinels above, and the pair row always carries `receipt`. The definition predates
  build 3 (field reference: "a failed current delivery may only have issue identity: drain -1, family 0, unknown format and empty geometry
  arrays"; "no receipt is built") and is visible in pre-build-3 banked rows the record already called "no receipt".
- **Proof both ways** (`_reviews/081-26-evidence/g8-proof.json`, 38 of 38 as expected). PASS on `C1_G8_N_A1` SI8 and on the Stage-1 SI8
  rows of Lyra V6 G15 N/S (`closure_timeout`), Lyra V5 G4 N/S (`current_undelivered`), StackOBot V6 G15 N/S and V6 G4 N/S; FAIL on six
  mutated copies of the SI8 receipt, each with exactly one delivery field taken from SI7's real receipt, and on the delivered SI7/SI9
  receipts. Over the whole bank (374 sessions, 74,257 receipts) every receipt with `t_drain_ms -1` carries the whole stub (801) and every
  delivered receipt fails the clause (73,456). The full G8 evaluator on each mutated session copy fails on this clause and on nothing else.
- **`C1_G8_N_A1` is PASS-REEVALUATED** from its banked evidence (never re-run): errors none, oracle re-run 17 compared = 17 matched,
  0 mismatched; its original FIXTURE-VALID-FAILURE row is kept beside it in `081-23-evidence/c1/stack-results.json`. Window c1 resumes at
  `C1_G8_S`.
- **Resume rule, stated because the 081-23 code did not enforce §0 on re-entry.** A leg with a recorded PASS or PASS-REEVALUATED is decided
  and skipped; a leg with a recorded fixture-valid failure, FAIL, NEEDS-DECISION or NOT-RUN now **stops the window again with its original
  code** instead of being skipped (the 081-23 code skipped it and carried on, so an unchanged re-run of c1 would have launched `C1_G8_S` past
  the recorded failure — not re-run `C1_G8_N`, as journal 081-25 said). Invalid attempts are still retried up to three. Proven offline by
  driving the unchanged chunk drivers with every launch stubbed (`_reviews/081-26-evidence/resume-proof.json`).
- **Harness boundary re-issued** for the new `081-23-lib.py` hash and the new feature head; no other locked hash changed
  (`_reviews/081-26-evidence/boundary-diff.json`). **Dry run re-run with the corrected lib: 134 of 134 as expected** — the 118 checks of §5
  with identical outcomes plus 16 new G8 checks — without writing any 081-23 evidence file.
- ⚠ **Not changed, and blocking — two PREDICTION-QUESTIONs from the predicate audit** (`_reviews/081-26-evidence/audit.md`; ruling 2 forbids
  changing a predicate whose prediction looks wrong):
  **PQ-A** — §2.1.4 "`change_view_rejected` 0 (except G8)": build 3 also counts it on **G2** (SI8's mask arm is served while its colour arm
  was dropped, so the family carries no m55 data) and on **G15** (the bench mask hold counts every held pass); Stage-1 V6 banked 1 on G2 N/S
  and G15 N/S. C1_G2 and C1_G15 would fail on it.
  **PQ-B** — §2.1.3 "exactly one `HIGH-WATER-PEAK` line": gates **7** and **15** reset the epoch at SI 9 and close twice, and the field
  reference (081-17) specifies one line per closure. C1_G15 and C2_G7 would fail on it.
  **Window c1 would therefore stop again at `C1_G2_N`**; neither question is answered here.

## AMENDMENT 2 — 081-27, 2026-09-25: three PRE-RUN prediction amendments — PQ-A, PQ-B and the G270 `empty_region` clause (dated, pre-run)

Authority: `_reviews/081-26-chat-ruling-prediction-amendments-pre-run.md` (rulings 1–3, chat Claude, 2026-09-25 12:35 IST). **Written
before any affected leg ran on build 3**: the campaign ledgers hold exactly two rows, both `C1_G8_N` (`FIXTURE-VALID-FAILURE`, then
`PASS-REEVALUATED`); `C1_G2`, `C1_G15`, `C2_G7` and C3's Lyra G270 have no row, and `081-23-evidence/c2` and `c3` are empty. These are
pre-run amendments, not refits, and they do not count as post-hoc. Unlike AMENDMENT 1 they change **what is predicted**, not only how a
clause is encoded. Everything not named here is unchanged, including §3.4's stop branch and every other §2 clause.

- **PQ-A — §2.1.4, replacing "`change_view_rejected` 0 (except G8)".** `change_view_rejected` is **0 on every leg except G8, G2 and G15,
  which must read ≥ 1**. Why: on G2 the dropped colour arm's mask is still served and finds no m55 family data
  (`AnomalyMaskSceneViewExtension.cpp:184`); on G15 the bench mask hold counts every held pass (`:160`); Stage-1 V6 banked 1 on G2 N/S and
  G15 N/S. Honest counting, not a defect. Encoded as `081-23-lib.view_rejected_error` (G8 keeps its own §D clause, "≥ 1", in
  `identity_checks`); a missing, non-integer or zero value on G2/G15 fails, as does any non-zero value on a gate that must be 0.
- **PQ-B — §2.1.3, replacing "exactly one `HIGH-WATER-PEAK` line equal to the summary".** **One `HIGH-WATER-PEAK` line per closure: the
  count equals 1 + `change_epoch_resets`, the last line equals the run summary's peak census (bytes and census), and every line's census is
  closed** (its `bytes_held` and `high_water` equal the line's bytes; unaccounted 0, unowned 0, colour split sums, colour + mask bytes =
  bytes held, max respected, cursor/latest/open phases/head present). Why: G7 and G15 reset the epoch at SI 9, `ResetEpoch` closes and
  persists, and run end closes again (`AnomalyChangeStage.cpp:655/664/669`); the 081-17 field reference already says one line per closure.
  The summary's own peak census is still checked as before. Encoded as `081-23-lib.peak_lines_errors`, called from `peak_census_check`.
- **G270 — §2.3's Lyra G270 line and 081-14 §3.4, replacing "every labelled phase fully measured".** **PASS = ≥ 3 events and every
  required pair either measured or honestly refused as `empty_region`** (the target's own mask has no pixels in that frame — occlusion or
  off-screen), **with none lost to `closure_timeout` or `budget_exceeded`**; invariants 0, census closed, oracle exit 0 mismatched 0 as
  before. Any required pair refused for any other reason, or any phase whose `pairs_measured` + `empty_region` refusals ≠ `pairs_required`
  or whose `reasons` name anything but `empty_region`, stays **NEEDS-DECISION (stop)**; any `budget_exceeded` anywhere is still the 081-16
  memory stop. §3.4's stop branch is unchanged. Chat's 081-14 wording was too strict for a gameplay host. Encoded in
  `081-23-lib.lyra_g270_eval` (`g270_phase_accounted`); the same pair rule applies to OBSERVED-BELOW-EVENT-FLOOR (< 3 events, non-blocking),
  which previously also required every phase fully measured. The result gains an `empty_region` count as a reading.

**Proof both ways** (`_reviews/081-27-evidence/amendment-proof.json`, 64 of 64 as expected):
- PQ-A (33): PASS on Stage-1 V6 G2 N/S and G15 N/S (StackOBot) and Lyra V6 G15 N/S / V5 G2 N/S (view_rejected 1), on Stage-1 G7 N/S (0),
  on the build-3 non-gate legs c0 S1 and 081-22 A (0), and through `measure_eval` on `C1_G8_N_A1` judged as G2/G15; FAIL on
  `C1_G8_N_A1`'s view_rejected 2 judged as a non-gate leg and as G1/3/7/9/10/13, on the full `measure_eval('g9')` of `C1_G8_N_A1`, and on
  G2/G15 summaries with view_rejected 0, missing, `true` or -1, and on a `C1_G8_N_A1` copy with 0 judged as G2/G15.
- PQ-B (19): PASS on every banked build-3 single-closure leg (`C1_G8_N_A1`, c0 S1, L1, L4, 081-22 A) and on a faithful two-closure
  synthetic — no two-closure build-3 log exists in the bank (none has two `HIGH-WATER-PEAK` lines; Stage-1 G7/G15 predate the peak census)
  — built from `C1_G8_N_A1`'s log with a first-closure line carrying its real 14,745,600-byte census and `change_epoch_resets` 1, alone and
  through the full `measure_eval('g8')` on a session copy; FAIL on a missing first or last line, the two lines in the wrong order, a last
  line whose bytes or census differ from the summary, a first-closure census with unaccounted bytes, an unowned colour, `bytes_held` or
  `high_water` off its line, three lines for one reset, two lines for zero resets and two lines for two resets.
- G270 (12): PASS on c0 L1 (1800 frames, 50 events, 179 measured + 14 `empty_region` of 193 required, oracle exit 0, the leg the old
  wording would have stopped), c0 L4 (42 events, all measured), 081-17 LYRA_G3_N_A2 (build 2) and an unmodified L1 copy; FAIL
  (NEEDS-DECISION) on L1 copies with one `empty_region` pair re-labelled `closure_timeout`, `budget_exceeded` (with and without a closed
  census and its log line — the latter stops on the memory branch), `current_undelivered`, on a row/phase disagreement either way, on a
  phase accounting one pair short, and on L4 with a `BUDGET-EXCEEDED` line.

**Dry run** (081-26 wrapper redirected to `_reviews/081-27-evidence/dryrun/`): **150 of 150 as expected** — the 134 checks of 081-26 with
identical outcomes (one `why` string changes wording: 081-17 G270 PASS) plus 16 new; 081-23 and 081-26 evidence byte-unchanged.
**Boundary** re-issued for exactly the changed hashes (`081-23-lib.py`, `feature_head`): `_reviews/081-27-evidence/boundary-diff.json`.

## AMENDMENT 3 — 081-28, 2026-09-25: the G270 vacuity guard (dated, pre-run)

Authority: `_reviews/081-27-chat-ruling-g270-vacuity-guard.md` (chat Claude, 2026-09-25 13:00 IST). **Written before C3's Lyra G270 leg
ran on build 3**: `081-23-evidence/c3` is empty and the campaign ledgers hold no C3 row. A pre-run amendment, not a refit. Everything not
named here is unchanged, including AMENDMENT 2's G270 pair rule and §3.4's stop branch.

- **G270 — adds to AMENDMENT 2's PASS rule.** PASS **additionally requires ≥ 3 events each with at least one measured required pair**
  (an event counts when the sum of `pairs_measured` over its phases is > 0). If the pipeline is clean — no `closure_timeout`, no
  `budget_exceeded`, no other refusal, phase accounting complete, invariants 0, census closed, oracle clean — but fewer than 3 events
  have a measured required pair, the result is the existing **non-blocking OBSERVED-BELOW-EVENT-FLOOR** branch of 081-14 §3.4, **never
  PASS**. Why: under AMENDMENT 2 alone a leg whose required pairs were all `empty_region` would PASS with nothing measured (the oracle
  compares 0 rows, and the audit's "no measured pair" error is filtered for G270) — `G146`'s vacuous zero. Every stop branch still
  precedes this floor. Encoded in `081-23-lib.lyra_g270_eval`; the result gains `measured_events` as a reading.

**Proof both ways** (`_reviews/081-28-evidence/guard-proof.json`, 12 of 12 as expected): PASS on c0 L1 (50 events, 45 with a measured
pair, oracle exit 0 mismatched 0), c0 L4 (42 of 42), 081-17 LYRA_G3_N_A2 (build 2, 5 of 5), an unmodified-content L1 copy, and an L1 copy
with exactly 3 events keeping a measured pair (the boundary); OBSERVED-BELOW-EVENT-FLOOR on an L1 copy with every required pair relabelled
`empty_region` (rows and phases, 0 measured events) and on a copy where only 2 events keep a measured pair; NEEDS-DECISION on the 081-27
`closure_timeout` and `budget_exceeded` mutations (with and without a closed census and its log line), on L4 with a `BUDGET-EXCEEDED`
line, and on the all-`empty_region` copy plus one `closure_timeout` (the stop precedes the floor).

**Dry run** (081-27 wrapper redirected to `_reviews/081-28-evidence/dryrun/`): **153 of 153 as expected** — the 150 checks of 081-27 with
identical outcomes (two G270 PASS `why` strings gain the measured-event count) plus 3 new; 081-23, 081-26 and 081-27 evidence
byte-unchanged. **Boundary** re-issued for exactly `081-23-lib.py` and `feature_head`: `_reviews/081-28-evidence/boundary-diff.json`.

## AMENDMENT 4 — 081-30, 2026-09-26: the person-present gate and ENV-VOIDED — scheduling only, no prediction changed (dated)

Authority: `_reviews/081-30-chat-ruling-person-present-gate.md` (rulings 1 and 2, chat Claude, 2026-09-25 23:48 IST). **This amends §0's
retry rule, which is fixture scheduling. No prediction, predicate, threshold or PASS rule changes** (ruling 2d): every evaluator is
byte-identical, and the dry run's 153 prior checks are unchanged.

- **Before every attempt** of every leg kind and **at window start**, the harness waits until input has been idle ≥ 60 s
  (`GetLastInputInfo`), polling every ≤ 15 s. The runners' own ALT-tap focus action is excluded by construction: an input inside the
  window of a runner whose attempt carried no person evidence is the runner's. This wait and the process-quiet wait share one **45-min
  budget per window** (`wait-budget.json`, per brief tag); when it is spent the window exits 4 (NOT-QUIET class). It never records NOT-RUN.
- A foreground-invalid attempt with **person evidence** inside its steady-state span — a shell process (`SearchHost.exe`,
  `StartMenuExperienceHost.exe`, `ShellExperienceHost.exe`, `explorer.exe`) took the foreground, the cursor moved away from any focus action,
  or non-harness input registered — is **ENV-INTERRUPTED**. It does not count against §0's 3 attempts; at most 5 per leg, then exit 4, never
  NOT-RUN. Without person evidence it stays INVALID and counts, as before.
- Fixture-valid attempts are never reclassified. `person_evidence` is recorded on every attempt row; a fixture-valid failure that carries it
  keeps its verdict and goes to chat as NEEDS-DECISION.
- **ENV-VOIDED** (ruling 1): a dated ledger row that clears a NOT-RUN and grants a fresh 3-attempt budget; it never clears FAIL,
  FIXTURE-VALID-FAILURE or NEEDS-DECISION. Applied once: `C1_G2_S` A1–A3 (081-29) are ENV-VOIDED as environmental, the rows stay for audit,
  and c1 resumes at `C1_G2_S_A4`.

Proofs: `_reviews/081-30-evidence/pp-proof.json` (42 of 42), `pp-walk.json` (all 86 remaining legs + window start, 8 of 8),
`resume-proof.json`; dry run 182 of 182 (the 153 of 081-28 identical + 29 new). Boundary: `_reviews/081-30-evidence/boundary-diff.json`.
