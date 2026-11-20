---
name: m55-requal-prep-boundary-locks-feature-head
description: "Bench-window preflights pin feat/m55-change-evidence head AND every harness hash (081-23 prep-boundary; 081-37 cost-boundary). Any docs commit on that branch after a re-issue blocks the next window. As of 081-43 the lib is 1c4ae649 (new oracle frozen in 081-43-frozen) and BOTH boundaries pin the old lib f5876b05."
metadata:
  node_type: memory
  type: project
  originSessionId: <uuid>
  modified: 2026-09-26T06:40:22.412Z
---

`_reviews/081-23-preflight.py` (hash-locked) checks `git rev-parse feat/m55-change-evidence` (the LOCAL ref) == prep-boundary.json
`feature_head` and the SHA-256 of 34 harness files (incl. `081-23-lib.py` and `081-23-window.py`) before every bench window c0-c3.

State after 081-35 (2026-09-26): AMENDMENT 6 (m47 excusal + G297 reading/tripwire) in the lib; lib `f5876b05…` (081-33 form `9901be63`
kept at `_reviews/081-35-evidence/081-23-lib.pre-081-35.py`); window runner `56e86f2e` unchanged; boundary re-issued (`b034d5c5` →
`d809c49c`) pinning feature head `03adc99` (pushed, local == origin). c1 ledger carries LEGACY_GROUP_CB_N and _CB_S PASS-REEVALUATED.
Preflight replay green c1/c2/c3; dry run 209/209. Next: 081-36 `python D:\IntrusiveAnomalies\_reviews\081-23-window.py c1 540 081-36`
resumes at `R23_MAINON_N1`. Latest wrappers to copy+redirect: `081-35-evidence/dryrun-081-35.py <tag>`, `boundary-reissue.py`,
`preflight-replay.py`, `resume-proof.py`, `reevaluate-cb-s.py` (group re-evaluation from the bank).

**081-36 (2026-09-26):** c1 (MAIN-N/S), c2 (42) and c3 (5) all PASS — requalification c0–c3 COMPLETE on build 3. The journal
commit moved the feature head to `2125159` WITHOUT a re-issue (boundary still pins `03adc99`), so any further window needs one first.

**081-37 (2026-09-26):** Stage 3 cost harness has its OWN boundary `_reviews/081-37-evidence/cost-boundary.json` pinning feature head
`ae56e17` (pushed), the committed predictions `docs/predictions/2026-09-26-m55-stage3-cost.md` sha `fe52e4fa…` and 15 harness files
(081-37 window/lib/sampler + the reused 081-23 lib/common/measure.ps1/b1/pose + a54_oracle). The 081-23 prep-boundary stays at `03adc99`
(c0–c3 closed). 081-38 runs `python _reviews\081-37-cost-window.py 081-38`; evidence lands in `_reviews/081-37-evidence/cost/`, bank
`M55B3R23_S3_*`. Any docs commit on the branch before 081-38 needs a cost-boundary re-issue (`081-37-evidence/boundary-issue.py`).

**081-38 → 081-39 (2026-09-26):** 081-38 ran pacing OFF (720p PASS as stress, 1080p LEG-FAILURE on vacuity). 081-39 declared AMENDMENT 1
(paced block P `S3_P_720_*`/`S3_P_1080_*`, 600 frames, gate P incl. pacing-held + yield ≥ 0.9; the 18 pacing-off legs RETIRED, their
LEG-FAILURE no longer halts). Cost boundary re-issued `ae56e17` → **`805f317`** (pushed), predictions `6230506a…` (evidence copy now
`081-39-evidence/predictions.md`), lib `e5f0d9a2`, window `3a9d5fa4`; previous boundary kept as `cost-boundary.before-081-39.json`.
081-40 runs `C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\081-37-cost-window.py 081-40` against the SAME ledger
(`081-37-evidence/cost/`). 081-39 wrappers: `081-39-evidence/{boundary-issue,dryrun-081-39,window-walk,proof-pacer,proof-readings-paced}.py`.
Again: any docs commit on the branch before 081-40 needs a re-issue.

**081-40 (2026-09-26):** block P ran (tag `081-40`, ~20 min): gate P 720p PASS, 1080p PASS, overall PASS; diagnostic pair run (G304:
run log ON costs ≈ +1 ms game-thread CPU/engine frame). Report `_reviews/081-40-evidence/stage3-cost-report.md` (generator `make_report.py`
there). Journal commit moved the feature head to **`ab634f5`** WITHOUT a re-issue (cost boundary still pins `805f317`) → any further cost
window needs a re-issue first. 081-41 is bench-free (client docs).

**081-43 (2026-09-26, bench-free, Codex 081-42 fixes):** the 081-23 lib is now `1c4ae649…` (F1 disagreement gating, F2 invariants
INV-1..9 + candidate-singleton NEEDS-DECISION); pre-081-43 lib `f5876b05` kept at `_reviews/081-43-evidence/081-23-lib.pre-081-43.py`.
The lib runs the NEW oracle from `_reviews/081-43-frozen/verify_capture.py` (`b399e884…`, asserted in `run_oracle`); old stays in
`081-23-frozen`. BOTH the prep and cost boundaries still pin `f5876b05` → any bench window needs a re-issue. Cached legacy readings
(pre-081-43 oracle summaries) FAIL under the new lib (missing `empty_region_disagrees`) — rebuild readings from the raw bank
(`081-43-evidence/replay-legacy.py`). The `_bench_sessions_bank` is a junction to `E:\IA_BuildCache\...` → hardlinks from the bank
must target E: (scratch `E:\IA_BuildCache\_r43_neg_scratch`). Proof re-run wrapper: `081-43-evidence/amproofs.py` (patches lib path +
output dir, runs each AMENDMENT proof under both libs; `--resume-as-of-081-26 resume-proof.py` rebuilds the 081-26-era c1 ledger).

**081-44 (2026-09-26): m55 MERGED to master (`38f3376`, docs `4283fc8`, no tag); feature head `6591669` (strip); staged exe now `E0BE6F0A` (code-identical to build 3 `002805CF`). The boundaries below are historical unless m55 bench work reopens.**

**Why:** a brief can say "docs commits on the feature branch" while the harness it runs forbids moving that branch; moving it
silently blocks the next window.

**How to apply:** when a boundary must be re-issued anyway, commit+push docs FIRST (from a scratch worktree outside the StackOBot tree),
then re-issue pinning that pushed head (081-26..081-35 did this), and replay the preflight offline. A docs-only commit on the feature
branch WITHOUT a re-issue (e.g. a bench brief's journal) moves the head and makes the next window's preflight refuse — plan a re-issue
with it. Proof mutants of banked sessions must copy `Actual_Frames` and the attempt log. Resume-proof checkers: `run_legacy_group`'s
finally block calls `quiet()` even when a group is evaluated without a launch (G293 shape). See [[brief-gate-beats-standing-shared-tree-rule]].
