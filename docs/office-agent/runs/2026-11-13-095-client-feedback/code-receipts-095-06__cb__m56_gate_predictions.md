# m56 (capture continuity) - bench predictions per CaptureBench tool (095-05b)

Plan: `_reviews\095-02-plan-A1-capture-continuity.md` sections 3.2 and 7; frozen numbers:
`code-receipts-095-05\predictions-095-05.txt` (P1-P20). Nothing here has run on an engine leg yet.

## What m56 changes for every bench reader
- The K settle ticks after every onset and offset are now CAPTURED (flagged `transition_reason: settle` on the row
  and on every entry of that row). At `IAI.Capture.Config 2 4 8 4 0` one cycle is 16 frames for 16 ticks; the release
  wrote 12 frames for the same 16 ticks.
- Rows gain `tick_index` and `t_wall_run`; `run_summary` gains `capture_span_ticks`, `frames_armed`, `frames_dropped`,
  `frames_dropped_ticks`(`_truncated`), `frames_dropped_by_cause`, `orphan_completions`, `label_rows_failed`,
  `settle_frames`, `label_transition_settle_entries` and the writer keys; `ticks_per_captured_frame` = span / written.
- `session_index` of the same tick DIFFERS between a release leg and an m56 leg (the release skipped ticks), so no
  release-vs-m56 comparison may join rows by `session_index`. Join by tick offset (`frame_index` minus the first
  `frame_index`): `m56_nonsettle_compare.py`.

## Per tool
| Tool | Status | m56 prediction / rule | Tested here |
|---|---|---|---|
| `run_leg.ps1` | updated: `-CaptureConfig` (default `2 4 8 4 0`, same ExecCmds) and `predicted_*` in `_leg_geometry.json` | 90 frames: m56 5 complete events, onsets at si 3 (mod 16), 22 settle frames; release 7 events. 300 frames: m56 18 events, 74 settle frames | prediction arithmetic only (parsed, block evaluated); never launched |
| `m56_continuity_check.py` | new (G1) | healthy m56 leg: every clause PASS, `ticks_per_captured_frame` 1.000, `frames_dropped` 0, settle rows exactly at the schedule; release leg (G4a): FAIL, 15 `frame_index` jumps of 3 per 90 frames, `ticks_per_captured_frame` ~1.344 | selftest 9 cases; banked release session FAIL; doctored continuous copy PASS |
| `m56_nonsettle_compare.py` | new (G5) | release vs m56, pose-matched, native, AA-off and delivered: every release row equals the m56 row at the same tick offset except `tick_index`, `t_wall_run`, renumbering keys and the run-unique set of a same-binary control pair | selftest 6 cases; banked release vs doctored copy PASS; one changed `bbox_px` FAIL |
| `a54_oracle.py` | updated: a flank frame carrying `settle` is skipped outward | m56 legs: same verdicts as the release on fire-window types (their flanks are not settle frames); `blinking` native onset flank (onset+1) is a settle frame and is skipped. Margins must be re-gated on m56 legs (P7) | byte-identical old vs new on 4 banked sessions x 3 shifts; the skip itself is exercised by construction only |
| `subset_gate.py` | no code change | release vs m56: NOT applicable (rows join by `session_index`); use `m56_nonsettle_compare.py`. m56 vs m56 control pairs: the run-unique set is measured as before; new keys are present on both sides | not run |
| `m50_pc7v3.py` | no code change | its DECLARED_* sets are m50's; an m56-vs-release pair is not comparable by `session_index`; for m56-vs-m56 the declared additions would be `tick_index`, `t_wall_run` (labels) and the run_summary keys above | not run |
| `m44_gates.py` | no code change | ONSET: the onset frame is unchanged by m56, first-mask = first-label = first-changed frame still holds. G7: after-onset settle frames are labelled and carry masks; after-offset settle frames carry only a transition-only entry and no mask, so "masks only on labelled frames" still holds. Onset si move from 3 (mod 12) to 3 (mod 16) | not run (needs engine legs) |
| `m44_pairing_probe.py` | no code change | NEITHER 0 / PREVIOUS 0 unchanged; more decidable frames per leg (16 per cycle) | not run |
| `p9_hidden_set.py` | no code change | the 12-frame shapes are artefacts of the skipped ticks. m56, blinking half-period 3 ticks, onset at si n: native hidden si = n + {2,3,4,8,9} (n+2 is a settle frame, labelled), synth hidden si = n + {3,4,5,9}. claimed == observed in tick space; the old `{n,n+1,n+5,n+6}` vs `{n,n+1,n+2,n+6}` comparison must not be reused | unit cases unchanged; not run on a leg |

## Could not be tested without an engine leg
- Every number above that describes a real capture: event counts, settle positions on a real host, masks on settle
  frames, the a54 flank skip on real pixels, pairing, blink hidden sets, and the settle-exactness table (plan section 8).
- The continuity check's after-offset window for `stuck_low_mip` bursts (deferred onset / restore trail) is reported
  NOT JUDGED by design; whether that is enough is a G2 reading.
