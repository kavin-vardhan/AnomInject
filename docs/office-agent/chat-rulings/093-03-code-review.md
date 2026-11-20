# 093-03 — Claude Code review of Codex's J1 implementation (follow-up capture base)

**Date:** 2026-10-13, about 21:35 IST · **Author:** Claude Code (mailbox brief `093-03-relay-guard-and-j1-base.md`), reviewer only.

**Inputs:**
- Codex's report: `_relay\runs\2026-10-13-093-03-j1-natural-base\report.md` (`gpt-6-astra`/`max`, 4,434 s, VERDICT NEEDS-DECISION).
- The local branch `fix/followup-capture` in `E:\IA_BuildCache\_r933\AnomalyInjector`: merge `72f3eb3`, docs head `72edc48`. **Not pushed.**
- Evidence under `E:\IA_BuildCache\_r933`.

**Method:** reading and recounting only. No build, no editor or game launch, no git write. My scratch files, recount scripts and outputs are in `_relay\runs\2026-10-13-093-03-j1-natural-base\code-receipts\`. The saved sessions were unchanged by my recount: file count, bytes and latest mtime were identical before and after.

## Counts
**15 OK · 5 GAP.**

## Gate table

| # | Gate | Codex result | My check | Verdict |
|---|---|---|---|---|
| 1 | Merge carries only the `CLAUDE.md` resolution (P1) | One conflict, `CLAUDE.md`; no source conflicts | Parents `d7eee97` + `5d6352b`. 30 one-side paths: the merge blob equals the changing parent's blob for all 30. 0 extra paths, 0 dropped paths. Three both-side files (`AnomalyCaptureSubsystem.cpp`, `AnomalyLabelSync.h`, `docs/client-readme.md`): `git merge-file` gives 0 conflicts, and the merge bytes equal the auto-merge bytes. `CLAUDE.md`: the 13-line prefix and 8,351-line suffix equal the auto-merge. The 34-line resolved region contains the whole release block (7 lines), the whole follow-up block (17 lines) and a dated 093-03 header | OK |
| 2 | Defaults read 0/0/0 | 0/0/0 | `DefaultOn/Off/HideFramesTemporal = 0/0/0` at `72f3eb3`. The docs head changes no Source, Shaders or tools path | OK |
| 3 | `git diff 5d6352b <head> -- Source Shaders tools` equals the release's `d660b5d..d7eee97` delta | Equal after removing blob-index lines | 136/136 lines. Only the 2 `index` lines differ: these are `AnomalyCaptureSubsystem.cpp` and `AnomalyLabelSync.h`, whose pre-images carry `5d6352b`'s own edits. Hunks, line offsets and `--stat` (4 files, +26/−27) are identical | OK |
| 4 | Explicit 3/40/1 override test | PASS, 316 checks | Receipt `pure-gate-controls-driver.log`: the `ignore_explicit_override` and `old_defaults` controls each fail 3 assertions (detected). Not re-run | OK (receipt) |
| 5 | Normal and strict Editor/Game builds | 4 builds, 0 errors / 0 warnings | Not rebuilt (outside a review). The archived binaries equal the pre-launch `build-manifest.json`, and the PIE `launch.json` binds that manifest | OK (receipt) |
| 6 | Unit/pure tests and mutants | PASS; 18 mutants + 5 controls rejected | `pure-mutants-3-driver.log`: 33/33 entries pass, 15/15 mutants exit nonzero. Plus 3 `active_source` mutants in the first driver log, and 5 default/override/ownership controls detected. The earlier failed-compile mutant runs are the disclosed harness errors | OK |
| 7 | String scan / lever audit | PASS | Read the lever-selftest receipt only (planted second `-IAIBench` gate → FAILED as expected) | OK (receipt) |
| 8 | Archive hash | 6/6 rehash | My rehash of `_binary_baselines\followup-capture-J1-72f3eb3-E41ABF93`: 6/6 equal the report and the build manifest. Release archive `218BA4C7`: 6/6. The corruption copy hashes `1495e3ce…`, not `589602b0…`, so it was detected. Index `_binary_baselines\README.md` is untouched (02-10) | OK |
| 9 | F1 owner guard, PIE UV multi-viewport (candidate) | Pass: 1,152 images, 4 fires, 8 edges +0, 192 masks | **Recount:** `verify_capture.py --label-pixel-gate` (head copy) gives 4/4 CONSISTENT; all 8 onset/recovery edges are +0. 1,152 rows = 1,152 images, `session_index` 0..1151, image ids = row ids. All 192 labelled rows have a nonzero mask PNG, and its pixel count equals `target_pixels` on every row. 192 measured + 960 unavailable = 1,152. Frame 200, viewed: the game view with the barcode marker row | OK (UV only; the proxy mode is unrun) |
| 10 | F1 can-fail: shipped release `218BA4C7` | Fails as expected (marker missing on all 1,152) | Masks: 0 measured, 192 `empty`. All 4 events have empty observable `affected_frames` (48 injected each). Frame 200, viewed: a distant editor view, with no marker row | OK |
| 10b | Which detector rejects the release control | Report: "wrong-view release capture is rejected" (in the label-pixel row) | The existing `verify_capture.py --label-pixel-gate` **exits 0** on it: 4 UNASSESSABLE, "NO FAILURE FOUND". Codex's own `assessment.json` also records `label_pixel_exit = 0`. The rejection comes from the job's own assessment script (missing marker, 192 missing measured masks), not from the existing reader | GAP (wording) |
| 11 | F6 wrong-event-id can-fail | **STOP:** "both checkers reject because the copy still contains four `pie_end_settle` transitions" | **That explanation is wrong.** Old and new `annotation_transition_check` run the same self-consistency test on **every** row, before the transition filter: `event_id == id@start_frame\|target_name`. Codex's copy changed only `event_id`, so it trips that test in both versions. 091-05's change only adds the cross-check "labels event missing from annotation" for non-transition rows. Diagnostic on scratch copies of the same capture (row 191 has no transition; the transitions are on rows 239/479/719/959): changing only `event_id` → old and new both report `event_id mismatch`. A **self-consistent** wrong event (`start_frame` 999999, `target_name` WrongTarget, matching `event_id`) → **old `[]` (misses), new `labels event missing from annotation`** (rejects). That is the frozen expectation, on this capture. This is a diagnostic only, not a gate result: the gate is still unqualified | GAP |
| 12 | F2 C1/C2, F3 alpha pair, dropped-mask drops, m52 hold, shared proxy, normal, and their can-fails | Not run (stopped) | — | GAP (unrun) |
| 13 | Every staged gate, and the staged recount this review was asked for | Not run; the retained container lacks the C1/C2, alpha and hold fixtures, so a bench cook is needed | No staged leg exists, so none could be recounted | GAP (unrun) |
| 14 | Push and protected refs | Nothing pushed | `origin` has no `fix/followup-capture`. `m51` `53bf725`; `master` `b5f15a3` = origin; `release/2026-10-05` `d7eee97` = origin; 30 tags. `_r933` is clean at `72edc48`. The main checkout is untouched | OK |
| 15 | Build-host restore | 201 files match | My rehash of all 201 receipt entries: 201/201 match. Host detached at `23098fa`, clean | OK |
| 16 | Staging untouched | No write, cook or staged launch | 0 files under `StackOBot\Builds` written since launch (19:48) | OK |
| 17 | Docs on the local head (status block, journal, gotcha) | Committed locally (`72edc48`) | **(a)** `G465` collides: `feat/vt-blur-stacks` (to G476) and `feat/vt-proxy-blur` (to G469) already use G465; the next free number over all refs is G477. **(b)** `G465`'s text, the `CLAUDE.md` status block and Codex's NEEDS-DECISION #1 record the transition explanation that row 11 contradicts. They derive "needs a transition-free capture" from it, which is the scope-from-unverified-mechanism shape `CLAUDE.md`'s invariant forbids | GAP |
| 18 | Predictions before the first leg | Yes | I saw `docs/predictions/093-natural-foundation-miss.md` untracked in `_r933` at 20:10. Its pre-leg binding is 14:51 UTC (20:21 IST); the first session started 20:31:57. It covers every step-6 gate, with a can-fail each | OK |
| 19 | Process and safety hygiene | Own PIDs exited 0; no foreign PID touched | No UnrealEditor, UBT or StackOBot process remains. The owner's `codex` desktop processes were present throughout and left alone | OK |

## What is wrong, with evidence
1. **The F6 stop reason is misdiagnosed (row 11).**
   - Evidence: `verify_capture.py` around line 3276 in both `_r917` (release) and `_r933` (head). The `event_id != key` check precedes `if entry.get('transition') != 1: continue`.
   - Receipt: `code-receipts\f6_diag.py` and its three scratch variants under `code-receipts\f6diag\`.
   - The stop itself was correct under the brief's stop rule: the declared expectation did fail. But the failure belongs to the mutant's construction, not to the capture's `pie_end_settle` transitions. A continuation does **not** need a transition-free capture.
2. **The local docs repeat that misdiagnosis and use a colliding gotcha number (row 17).** Nothing is pushed, so a correction costs nothing yet.
3. **One line overstates the existing reader (row 10b).** The F1 can-fail is real, but `--label-pixel-gate` alone does not fail the wrong-view capture. It reports it as unassessable and exits 0.
4. **Most of J1's matrix is unrun (rows 12–13).** That includes every staged gate, which needs a bench cook first. J1 is not qualified, and J2 cannot start from it yet.

Nothing was fixed. Chat rules.
