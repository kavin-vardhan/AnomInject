# REPORT for 071-02-m49-step1-label-pixel-verifier.md
brief: 071-02-m49-step1-label-pixel-verifier.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: medium
started: 2026-09-04 13:14:46 | finished: 2026-09-04 13:36:03 | duration: 21 min | exit code: 0
context at last turn: 566925 tokens (57% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\071-02-m49-step1-label-pixel-verifier.live.txt

---- FINAL MESSAGE (Claude Code) ----
Both pushed and in sync. Everything the brief asked for is complete.

```
# 071-02 REPORT — m49 step 1: the label-vs-pixel verifier
Python only. No C++ change, no build, no cook, no CaptureBench lever, no G-EDGE leg. Phase A untouched and still QUEUED.

## 1. Plain-language rundown
1. verify_capture.py gains --label-pixel-gate. Per event it checks the two things the client checks by eye: is the first labelled frame the first frame whose pixels change, and is the frame after end_frame the first clean one.
2. It imports measure_label_offset.py for the region/baseline/threshold code instead of copying it, and refuses to run without it rather than running degraded.
3. It proves it can fail before any result is read: seven self-test cases, both edges, both directions, plus a mask case and a NOT-VISIBLE case.
4. Five known-good banked sessions read PASS with zero shifts, across both artifact eras and both tick orders.
5. It fires on both known-bad targets (the m26 measured-zero spline, the H5 foliage over-claim) with NOT-VISIBLE. Both directions are now proven on real data, not just synthetic.
6. The known-answer runs found THREE defects in my own instrument, all fixed before any verdict was read; the readings they produced were withdrawn.
7. NOT-MEASURABLE is reported as an unread surface, never as a pass, with the reason on the event's own line.
8. Ship path done: the manifest now carries measure_label_offset.py, the checklist has a host-side gate box, client-delivery.md has the client command. client-readme.md untouched — schema v2 lands with Phase A.

## 2. Self-test (python tools/verify_capture.py --label-pixel-gate --selftest, exit 0)
| case | expected | read |
|---|---|---|
| clean | PASS | PASS |
| clean_masked (mask-region path) | PASS | PASS |
| label_late_1 | ONSET-SHIFT(-1) | ONSET-SHIFT(-1) |
| label_early_1 | ONSET-SHIFT(+1) | ONSET-SHIFT(+1) |
| end_late_1 | END-SHIFT(-1) | END-SHIFT(-1) |
| end_early_1 | END-SHIFT(+1) | END-SHIFT(+1) |
| blank_region | NOT-VISIBLE | NOT-VISIBLE |
Plus real-data shifts (--selftest --dir on the m45 leg; labels+annotation copied, frames symlinked, bank never written): +1 reads back ONSET-SHIFT(-1) on 5 of 6 events, -1 reads ONSET-SHIFT(+1) on 5 of 6; the sixth is the truncated final event, NOT-MEASURABLE in both directions and in the control.
The m47 black-frame self-test is unchanged (grey -> PASS, black -> FAIL, exit 0), so the existing checklist box reads as before.

## 3. Known-answer runs (read-only on the bank)
| session | masks? | result | exit |
|---|---|---|---|
| B20_M45_FULL_NAT | yes (m45, native) | PASS 5 · SHIFT 0 · NOT-VIS 0 · NOT-MEAS 1 | 0 |
| B20_M45_FULL_SYN | yes (m45, synth tick order) | PASS 5 · SHIFT 0 · NOT-VIS 0 · NOT-MEAS 1 | 0 |
| B28_..._m46_ctex_native | yes (m46) | PASS 7 · SHIFT 0 · NOT-VIS 0 · NOT-MEAS 1 | 0 |
| CM_CM_CTRL49 | no (pre-m43, bbox-only) | PASS 7 · SHIFT 0 · NOT-VIS 0 · NOT-MEAS 1 | 0 |
| H4_H4_CTRL_49 | no (pre-m43, bbox-only) | PASS 7 · SHIFT 0 · NOT-VIS 0 · NOT-MEAS 1 | 0 |
| CM_CM_SPLINE2 (known bad: m26 measured-zero) | no | NOT-VISIBLE 3 · SHIFT 4 · NOT-MEAS 1 | 2 |
| H5_H5_FOLIAGE_NOMARK (known bad: H5 over-claim) | no | NOT-VISIBLE 4 · SHIFT 2 · PASS 1 · NOT-MEAS 1 | 2 |
Every NOT-MEASURABLE in the top five is the same reason: the session's last event ends on the final frame, so its end edge has no frame after it (A50 TRUNCATED). The two pre-m43 legs are 14 blink edge pairs each, all aligned, in bbox-only mode — the mode the M2 bundle's sessions run in.
WARNING on the two known-bad rows: every row there is LOW confidence with a CONTAMINATED baseline, which is the instrument correctly flagging its own baseline as polluted. The trustworthy signal is NOT-VISIBLE; their SHIFT rows are LOW-confidence readings on a contaminated baseline and are NOT findings about label timing.

Three instrument defects found and fixed before any verdict was read (journal §11.3): the ceiling counted the session tail as a clean gap (±0, made everything NOT-MEASURABLE); "first frame above tau" read the temporal-AA ghost instead of the transition; and an edge search reached into another event's transition on the same actor eight frames later (argmax picked 42 over 40 by 0.8932 vs 0.8899). A fourth was in my fixture, not the gate. Every product-side reading they produced was withdrawn — the m45 leg has 0 shifts.

## 4. Client command, and what a v1 session can conclude
python host-tools/verify_capture.py --label-pixel-gate --dir <sessionDir>
Prove-it-can-fail first: python host-tools/verify_capture.py --label-pixel-gate --selftest
Exit 0 = nothing disagrees; 2 = any shift or NOT-VISIBLE; 3 = unreadable session. --report-only always exits 0.
On a v1 (M2 bundle, no masks) session it CAN read onset and end shifts per event from the bbox region and say nothing inside the claimed box changes near the claim (the F5/F7 shape). It CANNOT separate a sliver from an occluded target from a mip-collapsed checker (that is target_pixels, Phase A), judge per-frame observability inside a window (F4's "leaves frame at 69" shows only as d collapsing), or hold its threshold steady under camera motion — a fast pan raises tau and the event reads NOT-MEASURABLE rather than passing silently.

## 5. Commits / checkout
Plugin repo (AnomInject), branch master, pushed 9682b2c..ae54812:
  b062832  feat(tools): m49 step 1 - label-vs-pixel verifier with selftest
  ae54812  docs(071): m49 step 1 - verifier results, delivery box, client command
AnomDash repo, branch master, pushed be5b151..618b8df:
  618b8df  chore(bundle): ship measure_label_offset.py beside verify_capture.py
Checkout: switched europa-e1 (1f5e305, == origin/europa-e1, no tracked modifications) -> master; pulled --ff-only (already up to date, 9682b2c). LEFT ON master at ae54812 == origin/master, as instructed. The eight untracked CHAT-HANDOFF files were never staged. Comment stripper: 0 changed / 95 no-change (the new code is docstrings only, no '#' comments).
Journal: docs/sessions/2026-09-04-071-concorde-feedback-triage.md §11 (appended to the 071 file rather than a new 071-02 file — same investigation, and §11 cites §3.7/§4.2/§7/§8 directly).

## 6. Open questions / NEEDS-DECISION
NEEDS-DECISION: none. Nothing blocked; the instrument is built, self-proven and green.
Open, unchanged and not mine to close:
1. Q7 — the Bates delivery window (owner's, pending).
2. Q1/Q2/Q8 — with the client (F1's selection route; MP4-vs-PNG and viewer base; mask.provided for idx 9/44).
3. Reported, not fixed: on banked m45 legs a row can carry mask_value 0 while its mask PNG carries the event's real tag (si=27 reads mask_value 0 against a PNG containing only 222). The gate absorbs it (falls back to the PNG's sole non-zero value, refuses when several are present). Phase A's target_pixels join must not inherit that gap.
4. Next per the approved sequencing: the G-EDGE leg (both orders) using this instrument, then Phase A. Neither started.
```
api-equivalent cost (not billed on Max): $20.70 | turns: 66 | running total across all briefs: $709.81
