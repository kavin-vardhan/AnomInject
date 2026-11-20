# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 095-06b Review fixes for m56, m58 and the bench scripts, then one integration branch of m56 plus m58 - light, no engine legs, no Codex

# 095-06b — apply the ruled review fixes, then build the branch the gates will run on

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

**Where things stand:** two units were written on 2026-11-13 and neither has run in the engine: m56 capture continuity (`fix/m56-capture-continuity` = `b4017ea`) and m58 terrain blink subtype (`feat/m58-terrain-blink-subtype` = `a948046`). A fresh session reviewed both, read-only. Chat has ruled on every finding and every note. You apply the rulings.

**The machine is shared** and the owner's work comes first. This brief is **light**: code, throttled Editor compiles, pure selftests, Python tests. **No Game build, no editor or game launch, no capture leg, no cook.**

**Read first, in this order (binding):**
1. `D:\IntrusiveAnomalies\_reviews\095-06-ruling.md`: chat's ruling on each finding and note. **This is your work list.** Do what it says "apply" or "document"; leave what it says "leave".
2. `D:\IntrusiveAnomalies\_reviews\095-06-independent-review.md`: the review, with the `file:line`, the scenarios and the merge result.
3. `D:\IntrusiveAnomalies\_reviews\095-02-plan-A1-capture-continuity.md` §2 to §5 and §7, and `095-02-ruling.md`, `095-05-ruling.md`.
4. The journals on both branches for what was built.

**Receipts:** `D:\IntrusiveAnomalies\_relay\runs\2026-11-13-095-client-feedback\code-receipts-095-06b\`. Python: `C:\Python313\python.exe`.

## Pre-flight: stop with NEEDS-DECISION if any fails
- The primary checkout is on `fix/m56-capture-continuity` = `b4017ea`, clean apart from the known untracked files, equal to origin. CaptureBench is on `master`, clean, with branch `m56-gates` = `1d1e119`.
- E: is at or above its 50 GiB floor.
- No Unreal or build process of ours is running. Never kill a foreign process.

## Part 1 — m56, on `fix/m56-capture-continuity`
One commit per item, each committed as soon as it is written. The ruling has the exact decision; in short:
- **S1:** a vetoed event never comes back into `annotation.json` through the transition-only loop.
- **S2:** `run_summary` gains the exact list of vetoed event ids (re-use an existing list if one already identifies them). The readers accept an orphan transition entry **only** when its event id is in that list, its only reason is `settle`, and the schema minor is 2 or later. **Schema-2.1 sessions are judged exactly as before this unit.** The reviewer's probe session (a lost real event beside one vetoed event) must now **fail**; add it as a test.
- **S3:** a frame is `settle` only after a real fire or a real revert. In held-global mode the settle-phase ticks are captured as normal frames. Add a targeted `camera_clipping` fixture that the label-rule check must pass.
- **S4:** the writer's wait is bounded by **progress**: no job completing for 120 s means stalled; the run stops, unfinished frames count as `writer_timeout`, the log is loud. Covers the back-pressure wait and the end-of-run and teardown flushes.
- **N2** (with S4): writer jobs carry a run generation; a late job of an old run never touches a new run's lists.
- **N1:** frames written by the sync fallback are counted in the frames-written figure the accounting reads.
- **S5:** the optional temporal transition windows start **after** the settle frames. Prove it with the pure window selftest: a table for K = 2 and K = 0, N in {1, 3, 40}, showing the same frames flagged as the release arithmetic. **Fallback, as the ruling says:** if it cannot be done inside the window arithmetic without touching a membership rule or a label path, stop that item, apply the documentation fix instead, and report it.
- **N7:** the label rule excuses at most 2K stray settle rows per zero-match burst, at schedule positions.
- **N10:** the indices of row-only settle frames go into `annotation.json`'s session-level transition list.
- **N3, N4, N5:** documentation and journal only, as ruled.

After the C++ commits: **Editor target only,** 2 parallel actions, start only with at least 4 GiB free **checked at launch**, 0 errors and 0 warnings in the plugin. Then the pure C++ selftests, every Python tool test, and the lever audit. Update the readme and journal for anything client-visible. Pre-push scan, push.

## Part 2 — m58, on `feat/m58-terrain-blink-subtype`
- **M1:** `IAI.SetTerrainPatterns clear` means "back to the ini list" and its help text says so; a new `none` means "no patterns".
- **M2:** one loud run-end warning when every blink event of a run was terrain and there were five or more.
- **M3:** a one-line note in the docs.
- Selftest updated, Editor compile under the gentle rules, lever audit, pre-push scan, push.

## Part 3 — the bench scripts, on CaptureBench `m56-gates`
- **B1:** `run_leg.ps1` never kills by name. It stops only the PID it started itself, after checking that PID's start time and image path, and it **refuses to start a leg** if any other StackOBot process is running, naming it. Remove both kill-by-name sites. Test the refusal and the PID path with a harmless stand-in process you start yourself (not the game).
- **S6:** amend the predictions **before any leg has run**, as a dated amendment with the reason, exactly as the ruling says. Prove the amended comparator both ways again.
- **N6:** a scripted check of settle-frame label content: per event, the count of labelled-and-flagged frames, and a measured mask on every onset-settle frame. Prove it both ways on doctored sessions.
- **N9:** the continuity check rejects zero-byte frames.
- **CB-N1:** the G5 reading states that the comparator reads `labels.jsonl` only.
- Commit on `m56-gates` (local; the repo has no remote). Leave CaptureBench on `master`.

## Part 4 — the integration branch
Chat's ruling: **the gates run on the build that ships.**
1. Create `integ/m56-m58` off `master`. Merge `feat/m58-terrain-blink-subtype` first, then `fix/m56-capture-continuity` (true merges, no squash, no rebase).
2. Resolve the conflicts the review listed: `WriteRunSummary` keeps **every** parameter from both sides in one consistent order at the declaration, the definition and every call site; `CLAUDE.md` and the gotchas file keep both sides' entries.
3. **M4:** the terrain subtype's readme row moves to the 2.1 → 2.2 changelog table.
4. Check the semantic overlap the review named: a vetoed terrain blink must not count in `blink_terrain_events`. Fix it here if it does, as its own commit.
5. Editor compile under the gentle rules: 0 errors, 0 warnings. All pure selftests, all Python tests, the lever audit, on the merged tip.
6. Pre-push scan, push `integ/m56-m58`. **No merge to `master`, no tag.**
7. **Leave the primary checkout on `integ/m56-m58`,** clean, with the Editor binaries built from its tip.

## Rules
- **No Codex.** No `--force`, `clean`, `reset --hard`, `stash`, squash or rebase. Delete nothing outside your own outputs.
- **Never touch the owner's processes. Never kill a process by name.** No machine settings.
- **Never touch:** the `m51` ref, `fix/followup-capture`, `feat/europa-port`, `D:\IA_Europa`, tags, the dashboard repo.
- Never print a token. Codenames only. Office data never comes home.
- **Commit each item as soon as it is done.** If your context passes 60 %, finish and commit the item you are on, push, and report what remains as NEEDS-DECISION.
- On a failed test you cannot fix within the item's scope: report and stop.
- **How your turn ends:** do not end while anything you started is still running, and do not end on a progress update or a question. End only with the final message below. Target: under 4 hours.

## Final message
- **The very first line is exactly `VERDICT: GREEN`, `VERDICT: NEEDS-DECISION` or `VERDICT: RED`.** Bare line, no code fence. GREEN means every ruled item is committed and tested, both unit branches and `integ/m56-m58` are pushed, and the merged tip compiled clean with every offline test passing.
- Then:
  1. **For the owner (3 lines, plain words).**
  2. **Per item (B1, S1 to S6, each applied note):** the commit, what changed, the test that now proves it.
  3. **The integration branch:** tip, how each conflict was resolved, compile and test results.
  4. **What the host's binaries match now.** Context used.
  5. **Exactly what 095-07 must run,** as a short list: the three exes to build (release from the archive, m58-only, integration), the cook's maps, the legs per gate and which exe each runs on, the frozen predictions file.
  6. **Safety,** or "none". **Decisions chat must make,** or "none".
