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
