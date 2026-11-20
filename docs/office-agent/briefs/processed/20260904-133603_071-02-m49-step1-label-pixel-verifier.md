# EFFORT: medium
# MODEL: claude-opus-5

# 071-02 — m49 step 1: the label-vs-pixel verifier (`verify_capture.py --label-pixel-gate`) with self-test and known-answer runs
Implementation against the approved plan: journal 071 §7 (verifier spec) and §8.1 (the `tools/verify_capture.py` + `measure_label_offset.py` entry). Python only — no C++ edits, no build, no cook.

Headless. Nobody answers questions. Your FINAL MESSAGE is the only thing chat reads; structure in §4. On a real fork or a failed known-answer run: stop and mark NEEDS-DECISION. Codename discipline applies.

## 0. Chat rulings on the 071-01 open questions (record them in the journal; they bind m49)
- Q3: `observable` is `true | false | null` (`null` = unmeasured, `target_pixels == -1`); event gains `unmeasured_frame_count`. If EVERY frame of an event is unmeasured, `affected_frames` falls back to the injected subset and the event carries `observability_measured: false`; otherwise `affected_frames` = frames with `observable == true`.
- Q4: an event with an empty observable set STAYS in the file (`observable_frame_count: 0`, `injected_frames` populated). That is the client's injected_in_scene vs visually_observable split.
- Q5: T3(i) picker-level exclusion of translucent-only targets for ALL types, default ON, knob to restore.
- Q6: root key `label_schema: 2`; the README carries a v1→v2 field table.
- m39 FOLDED INTO m49 (approved). G-EDGE leg (§3.7) approved as an m49 gate, both orders, run once the verifier exists (this brief builds the instrument; the leg runs in a later brief).
- Q7 (Bates window) is the owner's — pending. Q1/Q2/Q8 go to the client — chat has asked.
- Sequencing ruling: the verifier comes FIRST because it must exist when the client's session folder arrives; Phase A implementation is QUEUED behind it (do not start).

## 1. Tree
Main checkout is on `europa-e1` (== origin). Shared-tree rule 1 permits the switch: verify `git status` shows no tracked modifications, record both SHAs, `git checkout master`, `git pull --ff-only` (expect `9682b2c`). LEAVE the checkout on master afterwards — Europa is parked and the m49 campaign builds from master; state this in the report. Path-scoped adds only.

## 2. Build exactly what §7 specifies
1. `tools/verify_capture.py --label-pixel-gate <session_dir>` reusing `tools/measure_label_offset.py`'s region/baseline/threshold code (import, do not copy). Per event: ONSET and END readings (`d(k)` = fraction of region pixels differing > 8/255 from k-1; baseline median + 6·MAD with the 0.004 floor; baseline-contamination banner; MEASURABLE-RANGE ceiling); region = `bbox_px`, or the mask when `mask_file` is present; NOT-VISIBLE = mask count < N on any positive frame, or (bbox-only) no pixel change anywhere near the claim; checker/magenta contrast REPORTED, never judged. One line per event: `PASS | ONSET-SHIFT(±n) | END-SHIFT(±n) | NOT-VISIBLE | NOT-MEASURABLE(<why>)`, plus a session summary line with counts. Exit code: 0 all PASS/NOT-MEASURABLE, 2 any shift or NOT-VISIBLE, 3 on a malformed session. Works on v1 sessions (the M2 bundle shape: no masks, `labels.jsonl` keyed by `session_index`, unsorted) and on master's v1.5 sessions with masks; must not require any key that v1 lacks.
2. `--selftest`: synthesise ±1 onset and ±1 end shifts on a banked session (edit a copy of the labels, never the bank) and prove the tool reads them back as `ONSET-SHIFT(+1)`, `(-1)`, `END-SHIFT(±1)`; prove a NOT-VISIBLE case by pointing an event at a region with no change; prove a clean session reads PASS. Both directions, G96/G142.
3. Known-answer runs (read-only on the bank): (a) a master-era banked session WITH masks (expect PASS 0/0 on every event); (b) a pre-m43 banked session WITHOUT masks (bbox-only mode; expect PASS or NOT-MEASURABLE with the reason); (c) if the owner's 2026-08-22 Concorde-lineage session or any pinned-m32-era session is banked, run it and report. Quote every line.
4. Ship path: add `measure_label_offset.py` beside `verify_capture.py` in AnomDash `bundle_manifest.txt`; `PRE-DELIVERY-CHECKLIST.md` gains the box "verifier run on the HOST's own smoke session, exit 0"; `docs/client-delivery.md` gains the one-line client command. Do not touch `client-readme.md` yet (schema v2 text lands with Phase A).
5. Journal `docs/sessions/2026-09-04-071-concorde-feedback-triage.md` §11 (this brief) or a new 071-02 file — your call, say which. Commit `feat(tools): m49 step 1 - label-vs-pixel verifier with selftest` + docs commit; push master when green.

## 3. Do NOT
No C++ changes, no CaptureBench levers, no G-EDGE leg run, no Phase A files. Those are QUEUED.

## 4. Final message structure
1. Plain-language rundown (8 lines max).
2. Self-test table (case | expected | read).
3. Known-answer table (session | masks? | per-event lines | exit code).
4. Exact client command line and what a v1 (M2) session can and cannot conclude.
5. Commits/SHAs, checkout state (branch + SHA, and the two SHAs at the switch), journal path.
6. Open questions / NEEDS-DECISION.
