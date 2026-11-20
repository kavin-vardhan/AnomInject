# REPORT for 093-02-natural-foundation-plan.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 093-02-natural-foundation-plan.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-10-13 14:36:08 | finished: 2026-10-13 15:33:22 | duration: 57 min | exit code: 0
context at last turn: 522763 tokens (52% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\093-02-natural-foundation-plan.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN — I collected Codex's plan, wrote my review against the ruling, and changed no repository or bench state.

**For the owner, in plain words**
1. First comes a combined base: the frozen delivered build, plus the October fixes, plus a finished version of the "capture the right game view" fix that failed at the office.
2. Codex proposes 11 implementation jobs in sequence, each needing a home test session. It estimates 40–62 hours; I expect more, because two of the jobs look too big for one session.
3. You would make two office visits, typing numbers from a fixed form on each office machine. The first is after job 2 (does capture work on your real setup?), the second after job 11 (do the two new missing-texture modes work?). That's about 450 numbers in total.

**Codex job**
- Run folder: `D:\IntrusiveAnomalies\_relay\runs\2026-10-13-093-02-natural-foundation-plan\`
- Model and effort from `run-meta.txt`: `gpt-6-astra` / `max`
- Duration: 2,741 s (14:37:43–15:23:25), exit 0
- Its verdict line: `VERDICT: PLAN READY FOR CHAT/OWNER APPROVAL | base=d7eee97 + six 091-05 implementation commits + completed 091-06 F1 | identity=a | jobs=11 (40–62 h) | office readings=I:11–14 after J2, N:21–24 after J11, per host, numbers only`
- Collected to `_reviews\093-02-codex-natural-foundation-plan.md` (one header line added, content unedited); ledger row appended.

**The plan**
- **Base:** a new branch from `d7eee97`, cherry-picking the six 091-05 commits by hand and keeping the 0/0/0 defaults.
- **Identity:** option (a). Finish the saved F1 work by freezing the owner into each request and refusing ambiguous views. Codex says plainly that the office cause is still unknown.
- **Foundation:** a provisional attempt inside the existing scheduler: clean census → arm → evidence (render-thread state, mask, picture, all on one key) → count or revert → measured recovery, with one cause at a time.
- **Picture check:** m55 extended with clean-noise controls and a visible threshold of `max(4, 4×noise)` in 8-bit code values. Up to 4 paired samples, a 2 s watchdog, and two recovered samples.
- **Publication:** a new barrier. Uncertain frames are **excluded** from the published sequence, so clips get shorter and published indices are renumbered.
- **MISS-1:** the controlled engine-default-material swap, still `missing_texture`, with an exact slot restore (nulls and array length included).
- **MISS-2:** natural. `DefaultTexture` is bound to one effective sRGB colour parameter on a private MID, using a sibling MID for host MIDs; virtual-texture bindings are skipped.
- **Labels:** exactly the R8 keys, with worked examples for both pilots.
- **Gates:** 15 gates, each with its own can-fail, PIE and staged, on StackOBot plus Lyra as the office-like fixture.
- **Jobs:** J1 base, J2 F1, J3 lifecycle, J4 evidence, J5 publication, J6 labels, J7 MISS-1, J8–J9 MISS-2, J10 UI, J11 qualification.

**My review** (`_reviews\093-02-code-plan-review.md`): 14 ruling rows — 9 OK, 4 GAP, 1 CONFLICT.
- **CONFLICT, R2:** CAM-1 is tagged `cause_family: content_error`; the ruling says CAM-1 is "config".
- **GAP, R11 base:**
  - The plan's reason for not merging (that it "reverses defaults to 3/40/1") only holds for a plain diff between the two heads; a real merge doesn't reverse them.
  - A read-only 3-way merge of `5d6352b` into `d7eee97` has one conflict, in `CLAUDE.md`, and keeps 0/0/0.
  - The F1 work-in-progress also applies cleanly on top of that merge, so chat could use the proven `5d6352b` code as it stands instead of re-picking six commits by hand.
- **GAP, R11 identity:**
  - The saved work adds a new rejection (`NotGameView`: needs a game show flag, a game-thread view and a controller id ≥ 0) even when the owner is known. That is a new way to drop every frame, and the plan doesn't mention it.
  - The plan's main can-fail for this fix relies on two things the 10-10 cleanup deleted. The archived `3ADC2EA4` and `C7FB5212` builds were in `_r915`. The delayed-viewport harness was in `_r916`. Job 2 has to rebuild both.
- **GAP, R7:** exclusion-only publication puts jump cuts right next to every natural onset and offset, which a model could learn instead of the anomaly. It also breaks the readme's promise that the video plays at natural speed. The "transition flag" alternative that R7 allows isn't costed.
- **GAP, R8:** the controlled MISS-1 record sits inside `natural_causes[]` with `cause_family: code_defect`, which reads as natural and as a defect claim.
- **Other findings:**
  - The office edge readings (checkpoint N) are trivially 0 unless the diagnostic source-frame window is turned on. It should be mandatory there.
  - Both example events are in one session but share `mask_value 200`, which can't happen.
  - Jobs J5 and J11 are unlikely to fit one bench session.
  - J5 has no gate proving that a controlled-only run's output is unchanged.
  - Blinking mode 0 is a bench-only lever, so the GONE-3 tag can never appear in client data.
- **F1 patch check:** `git apply --check` exits 0 on `1eaf205` and on `5d6352b`, and fails on `d7eee97`, which has no ownership guard.

**New module dependencies:** none proposed. I checked that the engine functions the probes need are exported or inline.

**Owner product questions**
1. Dashboard layout: shape A (a Controlled/Natural choice beside each pool row) or B (a details panel when a row is selected).
2. Is it acceptable that clips get shorter when uncertain frames are dropped?
3. Which office hosts the checkpoints should cover; the 10-10 cleanup note says the second host's M3 won't ship.
4. When to tell the client about the new label keys.

**NEEDS-DECISION:** none blocks this brief. Chat needs to rule on:
1. Merge `5d6352b` or cherry-pick six commits.
2. Exclusion or transition flag for unresolved natural frames.
3. Where the MISS-1 record goes and what family it gets.
4. CAM-1's family value.
5. Where job 2's can-fail comes from: a rebuilt guard build, and the shipped `218BA4C7` as the pre-F1 control.
6. Making the diagnostic window mandatory for checkpoint N.

My scratch worktree `E:\IA_BuildCache\_r932` was created for the patch check and removed at the end. `m51`, the release worktree and every branch head are as I found them; the two `codex.exe` processes that were already running (desktop app) were not touched.
api-equivalent cost (not billed on Max): $11.78 | turns: 85 | running total across all briefs: $4,461.34
