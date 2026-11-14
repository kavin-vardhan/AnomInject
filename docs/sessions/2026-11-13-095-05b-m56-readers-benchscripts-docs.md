# 095-05b — m56: the readers learn settle and schema 2.2, the bench check scripts, the docs

**Date:** 2026-11-13 · **Session:** Claude Code (Opus 5.5, xhigh), headless, light brief (095-03b E4: no build, no
launch, no capture, no cook). **Branch:** `fix/m56-capture-continuity` (plugin); `m56-gates` (CaptureBench, local).
**Spec:** `_reviews\095-02-plan-A1-capture-continuity.md` §3, §7, §8, §10, §11; rulings 095-02 (A-1), 095-03b
(E2, E4), 095-05. **Receipts:** `D:\IntrusiveAnomalies\_relay\runs\2026-11-13-095-client-feedback\code-receipts-095-05b\`.

## State in one paragraph

Plan commits 5 and 6 are on `fix/m56-capture-continuity`; the CaptureBench scripts are on its `m56-gates` branch.
Every plugin-repo reader now reads schema 2.2 (`settle`, `tick_index`, the drop accounting) and still reads older
sessions with the same verdicts. The two new bench scripts are proven both ways offline on a banked release-era
session and a doctored copy. **No engine leg has run on m56; the Game target is not built; the dashboard's encode
step and the client's bundled kit are not updated.** No merge, no tag.

## Commits

| Repo | Hash | Subject |
|---|---|---|
| plugin | `325a3e7` | chore(tools): m56 readers learn settle and schema 2.2 |
| plugin | (this commit) | docs: m56 capture continuity |
| CaptureBench (local, no remote) | `1d1e119` on `m56-gates` | tools(m56): 095-05b continuity check, non-settle comparator, a54 settle flank rule, run_leg schedule predictions |

## Part 1 — the plugin readers (plan §3.2)

- **`verify_capture.py`.** Annotation check: `label_schema_minor` 1 or 2; a row whose only transition is the
  row-level `settle` counts in `transition_frame_count`; `settle` in a 2.1 file is refused; transition entries of
  events missing from `annotation.json` are tolerated up to `run_summary.vetoed_events` (G575). Label rule: `settle`
  joins the reason list as a reason of every type (handled like `capture_unpaired`, `LABEL_RULE_ANY_TYPE`), labelled
  or not, never temporal-only; `transition_present` must equal "a flagged entry or a row-level settle"; row-level
  reasons must be `capture_unpaired` / `settle`. New checks: `SETTLE-INCONSISTENT` (a settle row's entries all carry
  settle, and only settle rows carry it), `SETTLE-MISPLACED` (`_lr_settle_placement`: the K ticks after every onset
  row and after every revert, in tick space, K from `run.json` else inferred; stuck_low_mip after-onset only; strays
  allowed only with zero-match bursts), `SETTLE-COUNT`, `TICK-CONTINUITY`, `DROP-COUNT` (`_lr_continuity`: missing
  ticks = `frames_dropped_ticks` + failed label rows; `frames_dropped` = Σ causes; written + dropped = span).
- **`label_sync_check.py`** (office kit **1.6**, evaluator 095-05b): `settle` known; the transition gate excuses a
  settle frame in both directions and keeps it out of the anti-aliasing decay rule; `brk`/`brk52` read `tick_index`
  when present (G576); report gains a settle count per type and a session line; no longer refuses 2.2 (it called the
  annotation check, which refused minor 2).
- **`measure_label_offset.py`:** transition and settle frames never enter the baseline, whatever the guard relaxes to.
- **`exclusion_gate.py`:** the Python rule (labelled or any transition) already equals the C++ counter's rule after
  m56, because `NoteTexCorruptM52Overlap` runs after `StepSettleFlags` and counts any reason bit. **No C++ change, so
  no compile.** Three selftest cases pin settle entries.
- **`r4_pairing_gate.py`** re-run on the m56 source: PASS; selftest 12 OK.
- **`OFFICE-CHECK.md`:** the 42-frame cycle at `2 40 8 30 0` (about 70 anomalies per 3000 frames, was about 78), the
  `settle frames` line to read back, kit 1.6.

### Reader tests (baseline at `586a0fd` → after)

| Test | Before | After |
|---|---|---|
| `verify_capture --label-rule --selftest` | 65 cases OK | **83 OK** (+18 m56) |
| `verify_capture --label-pixel-gate --selftest` | 98 OK | **101 OK** (+3 schema-2.2) |
| `verify_capture --change-oracle --selftest` | 35 OK | 35 OK |
| `test_annotation_transitions` | 5 OK | **8 OK** (+3) |
| `test_verify_capture_consistency`, `test_verify_capture_change_oracle` | OK | OK |
| `label_sync_check --selftest` | 93 checks OK | **102 OK** (+9) |
| `measure_label_offset --selftest` | PASS | PASS (+ baseline exclusion check) |
| `exclusion_gate --selftest` | OK | OK (+3 cases) |
| `r4_pairing_gate`, `--selftest` | PASS, 12 OK | PASS, 12 OK |

Receipts: `reader-tests-baseline-586a0fd.txt`, `reader-tests-after-part1.txt`.

### Schema-2.2 fixtures: healthy passes, mutants fail the reader that should catch them (G577)

| Mutant | Reader that catches it | Result |
|---|---|---|
| healthy 2.2 session with settle frames | label rule, annotation check, kit, pixel gate | clean / exact / PASS 0/0 / CONSISTENT |
| settle flag moved one tick later; offset flag moved onto the revert tick | label rule `SETTLE-MISPLACED`; continuity check C7 | FAIL (both) |
| entry settle without the row flag; settle row without `transition_present` | label rule `SETTLE-INCONSISTENT` / `TRANSITION-PRESENT-MISMATCH` | FAIL |
| label shifted by one frame (labels vs annotation) | label rule `LABELLED-EXTRA` | FAIL |
| label shifted by one frame, settle flags present (pixels) | kit (`settle_label_late_1` / `_early_1`) | FAIL; pixel gate OFFSET-NOTE by design |
| residue on unflagged frames / same residue on settle frames | kit | FAIL / PASS (raw FAIL) |
| tick missing, `run_summary` claims no drop | label rule `DROP-COUNT`; continuity check C3/C6 | FAIL; the honestly counted drop is clean in the label rule |
| settle count wrong; settle in a 2.1 file; unknown row reason | label rule `SETTLE-COUNT` / `ANNOTATION-TRANSITIONS` / `REASON-UNKNOWN` | FAIL |
| vetoed event's settle entries unlisted | annotation check | clean with `vetoed_events` 1, FAIL with 0 |

### Old sessions read the same

`old-vs-new-readers.txt`: the `586a0fd` readers and the edited readers give identical verdict lines on two banked
m47-era sessions (`AE1_CT_AEON`, `AE6_MT_AEOFF`: label rule NO MISMATCH, OLD rule; annotation legacy; kit READ BACK
identical) and two 4.25-port schema-2.1 home sessions (one clean, one failing identically). ⚠ **No 5.1 release-era
(2.1) session remains on disk** (the bank is empty and the 091-03 evidence was removed in earlier E: cleanups), so the
2.1 proof rests on sessions written by the 4.25 port of the same writer.

## Part 2 — CaptureBench (`m56-gates`, `1d1e119`, local; the checkout was returned to `master`)

- **`m56_continuity_check.py`** (G1, clauses C1–C8). Selftest 9 cases. **Banked release-era
  `_m47_legs\AE1_CT_AEON\session_20260903-182615` (read-only): FAIL** — 90 of 90 rows without `tick_index`,
  `frame_index` steps `{1: 74, 3: 15}` (15 jumps of 3, first at si 3→4, 11→12, 15→16), `ticks_per_captured_frame`
  1.3444, `capture_span_ticks` / `frames_dropped` absent, 30 expected settle ticks with no row (first 4, 5, 14, 15).
  **Doctored continuous copy** (`doctored-continuous-AE1`, built in the receipts by `make_doctored_continuous.py`:
  120 rows, 90 real rows unchanged apart from renumbering, 30 settle rows at the skipped ticks): **PASS**, all
  eight clauses.
- **`m56_nonsettle_compare.py`** (G5). Selftest 6 cases. Banked AE1 vs its doctored copy: **PASS** (span 0..119,
  90 release rows joined, 30 settle rows without a partner by design). The same copy with one `bbox_px` changed at
  si 9: **FAIL** (`anomalies/<entry>/bbox_px x1, first offset 9`).
- **`a54_oracle.py`:** a flank frame carrying the row-level settle flag is skipped outward. Old vs new byte-identical
  on 4 banked sessions × 3 shifts (`a54-identity.txt`; AE1/AE2 ALL-ALIGNED at shift 0, SHIFTED at ±1). Its certified
  controls (R30, HF1) are gone from disk, so the A53 re-verification was replaced by this identity; the skip itself is
  exercised by construction only.
- **`run_leg.ps1`:** `-CaptureConfig` (default `2 4 8 4 0`, same ExecCmds) and `predicted_*` for both build families in
  `_leg_geometry.json`: 90 frames → m56 5 complete events, 22 settle frames, onsets at si 3 (mod 16); release 7. 300
  frames → m56 18. Parsed and the block evaluated; never launched.
- **`m56_gate_predictions.md`:** the m56 prediction per bench tool (`subset_gate.py`, `m50_pc7v3.py`, `m44_gates.py`,
  `m44_pairing_probe.py`, `p9_hidden_set.py` need no code change; release-vs-m56 must never join by `session_index`).

## Part 3 — docs

`client-readme.md` (§8.2 root/video fields, new §8.5a 2.1 → 2.2 changelog, §8.6 `tick_index` / `t_wall_run` /
row-level `transition_reason` and the `t_wall` correction, §8.6a `settle` rows, new §8.6c capture continuity with
every new `run_summary` key and every drop cause, an amber-overlay note), `architecture.md`, `capture-fps.md`,
`PRE-DELIVERY-CHECKLIST.md` (the stale "68 keys" literal replaced by a categorical check plus an m56 box), this journal,
gotchas G575–G578, the status block.

## Could not be tested without an engine leg

Everything that describes a real capture: continuity on a real session, settle positions with real fires, masks on
settle frames, the kit on real settle pixels, the a54 flank skip on real pixels, event counts per leg, blink hidden
sets in tick space, the non-settle comparison on a real release/m56 pair, the drop levers, and the settle-exactness
table (plan §8). The continuity check's after-offset window for stuck_low_mip bursts is NOT JUDGED by design.

## Deviations

1. No C++ change for the exclusion counter: it already applies the same rule after m56 (source read above).
2. `annotation_transition_check` tolerates vetoed events' transition entries (not in the plan; without it every 2.2
   session with a veto is refused, G575).
3. `verify_capture.py` gained structural settle and continuity checks beyond the plan's list, so the moved-flag and
   missing-tick mutants fail a plugin-repo reader.
4. a54 skips only settle-flagged flank frames (other reasons unchanged), so prior readings stay byte-identical.
5. The kit's READ BACK line is unchanged; the settle count is a separate session line (the office read gains a line).
6. Old-schema proof on m47-era and 4.25-port sessions (no 5.1 release-era session remains).
7. CaptureBench has no remote: committed locally, not pushed.
8. The pre-push scrub of `verify_capture.py` reported terms present at `586a0fd` already: a two-letter local variable
   in the m55 change-oracle selftest matches the scrub's abbreviation rule (found by bisection, line numbers only;
   earlier scans covered only changed files). Renamed in its own commit `338e4cb`; the 35-case selftest and its unit
   tests still pass. G578 applies to reading the scrub's output.

## Next

095-06 (independent read-only review of both branches), then 095-07 on the owner's word (Game build, one cook, every
gate including the settle-exactness table), then chat's K ruling, the dashboard brief and a re-bundle (A-R4).
