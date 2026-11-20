# 095-02 receipts (Claude Code, read-only planning brief, 2026-11-13)

Outputs:
- D:\IntrusiveAnomalies\_reviews\095-02-plan-A1-capture-continuity.md
- D:\IntrusiveAnomalies\_reviews\095-02-plan-C-terrain-blink-subtype.md

State at start and end: plugin repo checkout `master` = ebc34ed, no tracked change; the 15 known untracked files untouched.
No source edit, branch, commit, push, stash, build, editor launch or capture. No Codex. Other refs read via git objects only.
`git apply --check` (writes nothing) was run by a sub-reviewer for 6713f51.

Files in this folder:
- europa-port-diff-touched-files.txt — `git diff --shortstat master feat/europa-port -- <file>` for every file the plans touch.
- europa-port-subsystem-hunks.txt — hunk headers of AnomalyCaptureSubsystem.cpp on feat/europa-port vs master.

Evidence read today (not copied):
- D:\IntrusiveAnomalies\_m47_legs\AE1_CT_AEON\session_20260903-182615 (banked 90-frame CB_GateLevel leg):
  frame_index 1..120, deltas 74x1 and 15x3 (at session 3->4, 11->12, 15->16, ...), t_wall steps 93-100 ms at the gaps,
  anomaly_present onsets 3, 15, 27, ..., run_summary ticks_per_captured_frame 1.344 (121/90), 64.9 MB for 156 files.
- Disk free at the time: C 20.21 GiB, D 53.09 GiB, E 22.09 GiB.

Sub-reviewers (read-only, three): reader inventory; terrain path on master + UE 5.1.1; settle history, 6713f51 and drop paths.
Claims the plans depend on were re-checked by the main session and are marked [V] in the plans; the rest are marked [R].
