# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: high
# NAME: 095-01 Return to main - master fast-forwarded to the delivered release, primary checkout off m51, rebuild, baseline capture cadence readings - no Codex

# 095-01 — return to the main line, and take the "before" readings for the client's frame-skip topic

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

**Why:** the Europa port is delivered (`feat/europa-port` = `2828f6a`). The owner has moved the priority back to the 5.1 main line, to act on three topics from the client's annotation-quality review: frame skipping, LOD popping without a spatial trigger, and terrain as a blink subtype. This brief does only the first step: make `master` the real main line again, move the primary checkout onto it, rebuild, and measure today's capture cadence on the delivered code. **No product source changes. No Codex** (owner's instruction, 2026-11-13).

**Read first (both binding context):**
- `D:\IntrusiveAnomalies\_reviews\095-00-ruling.md`: chat's rulings. This brief implements §1 M1 and M2 and the baseline in §5 step 1.
- `D:\IntrusiveAnomalies\_reviews\095-00-code-capture-review-2026-11-11.md`: your own read-only review of 2026-11-11 (a different session; you don't have its context). Sections C1, C2 and 8 matter here.

**Heads-up:** the watcher starts you inside the primary checkout, which is still on `m51`. The `CLAUDE.md` you auto-loaded is m51's and is two months stale. Current state is in the ruling and in `git show release/2026-10-05:CLAUDE.md`.

**Receipts:** `D:\IntrusiveAnomalies\_relay\runs\2026-11-13-095-client-feedback\code-receipts-095-01\`. Every write outside the repo uses an absolute path.

## Part 1 — pre-flight (read-only)
Record, in `preflight.txt`:
- the primary checkout's branch and HEAD (expected `m51` = `53bf725`), `git status --porcelain` (expected: 15 untracked files, nothing modified or staged), and the index hash;
- after `git fetch origin`: local and remote tips of `master` (expected `b5f15a3` both), `release/2026-10-05` (expected `d7eee97` both), `m51`, `fix/followup-capture`, `feat/europa-port`; the worktree list;
- free GiB on C:, D: and E: (resolve the junctions), against the standing floors;
- whether any Unreal editor, game or build process is running, and from where.

**Stop here with NEEDS-DECISION, changing nothing, if any of these is true:**
- a tracked file in the primary checkout is modified or staged;
- `master` is not a strict ancestor of `d7eee97`, or `origin/master` is not `b5f15a3`;
- `master` is checked out in any worktree other than the primary;
- a foreign Unreal or build process is using the primary StackOBot host (never kill it);
- the rebuild or the capture legs would take a volume below its floor.

## Part 2 — `master` becomes the delivered release (ruling M1)
1. Fast-forward local `master` from `b5f15a3` to `d7eee97`. Fast-forward only; refuse anything else.
2. Push `master` to origin: fast-forward, no force, no tags. Confirm with `ls-remote`.
   - Every commit being published is already on origin under `release/2026-10-05`, so no new content leaves the PC. Say so in the receipt instead of running the full pre-push scan.

## Part 3 — move the primary checkout onto `master` (ruling M2)
1. **Copy** the 15 untracked files into the receipts folder, with SHA-256 hashes. They also stay where they are.
2. **Collisions:** if any untracked path is a tracked path on `master`, move that file into the receipts folder (after the copy) so the checkout can proceed. List every file moved. Delete nothing.
3. `git checkout master`: plain. **No** `--force`, `clean`, `reset --hard` or `stash`. If git refuses, stop and report its exact output.
4. Verify: HEAD = `d7eee97` on `master`; the tree is clean apart from the surviving untracked files; `m51` still points at `53bf725`; no other ref moved.
5. **The host around the plugin.** From the release's final-proof journals (091-03, 091-07), find which bench-fixture plugin revision and host configuration the release proof ran on. Compare with the primary StackOBot host (`Plugins\CaptureBench`, host config, the other plugins present). Report every difference.
   - Align by a plain checkout of an existing commit **only** when that tree is clean. Never by force. If it can't be aligned that way, leave it and report.

## Part 4 — rebuild
- Build the primary StackOBot host's Editor and Game targets (Development, Win64) per the runbook. Required: 0 errors. Record the warning count and the hashes of the game exe and the plugin DLLs.
- No cook and no packaging in this brief.

## Part 5 — baseline capture legs on `master` (the "before" for unit A)
**Admission:** the owner is at the office and started the watcher for this work, so the 15-minute input-idle wait is **waived** for this brief (`idle_wait_waived=true`). Keep the other checks: no foreign Unreal or build process, physical memory ≥ 6 GiB, GPU free. If admission still fails after 20 minutes of waiting, skip the legs and report.

**Before launching anything,** write `predictions-095-01.txt`:
- B1: if the settle skip (C1) behaves as read, `ticks_per_captured_frame` lands in 1.30–1.36 and the written frame count is about three quarters of the engine ticks.
- B2: if the mask-wait defect (C2) is reachable through the output-height setting, frames written fall well short of frames requested, and the "did not resolve by run end" line appears.

**Legs** (standalone `-game` on the standard bench map, release defaults, the default auto pool, paced, with no dashboard attached):
- **B1, required:** frame cap 600, native output height.
- **B2, required if B1 completes:** frame cap 300, output height set to a non-zero value through the same message or console route the dashboard's Setup panel uses. Everything else as B1.

**Readings per leg** (numbers only, in a table):
- from `run_summary.json`: `ticks_per_captured_frame`, `capture_game_ticks`, frames written against the cap, `speed_ratio`, `sustained_wall_fps`, `stamped_fps`, `capture_unpaired_frames` and any `capture_unpaired*` siblings, `target_mask_frames_unavailable`, the event count, and the effective settle / pre / positive / post values the run echoed;
- counted on disk: frame files, mask files and `labels.jsonl` rows;
- log line counts for: `TARGET MASK NEVER RESOLVED`, `did not resolve by run end`, `writer flush timed out`, `failed to encode`, `failed to write`;
- wall-clock duration of the leg.

These are **readings, not gates.** Report them as measured, next to the predictions, and say plainly where a prediction missed. Don't investigate a miss in this brief.

Keep both sessions under the usual bench retention and give their paths. Restore anything you changed on the host for the legs, by receipt.

## Part 6 — record it on `master`
- Add a short entry at the top of `CLAUDE.md`'s status block, and a session journal `docs/sessions/2026-11-13-095-01-return-to-main.md` with the branch move and the baseline readings.
- Check that milestone numbers m56, m57 and m58 are unused across docs on all refs; report the next free milestone number and the next free gotcha number.
- One docs commit on `master` (`docs: record return to main and baseline cadence readings (095-01)`). Run the pre-push scan on that commit, then push `master` (fast-forward) and confirm with `ls-remote`.

## Rules
- **No product source edits.** Nothing under `Source\`, `Shaders\`, `Content\` or `WebClient\` changes.
- **Never touch:** the `m51` ref (it stays where it is), `fix/followup-capture`, the J2 worktrees and hosts under `E:\IA_BuildCache\`, `feat/europa-port`, `D:\IA_Europa`, tags, the dashboard repo, `E:\<personal-folder>`, `E:\UEProjects\LyraStarterGame`, `E:\Unreal Projects\StylizedParisStreet`, `ToCodex\`.
- **No Codex,** no Relay.
- **Never kill a process by name.** No machine settings. Delete nothing. Never print a token. Codenames only.
- On any failed check or real fork: **report and stop.** Never iterate on a failure.
- **How your turn ends:** do not end while anything you started is still running, and do not end on a progress update or a question. If you are about to do either, keep working. End only with the final message below. Target: under 90 minutes.

## Final message
1. **`VERDICT: GREEN | NEEDS-DECISION | RED`.** GREEN means Parts 2–4 are done and verified, B1 ran and its readings are reported, and the docs commit is pushed.
2. **For the owner (3 lines, plain words):** is the main branch now the delivered code and is the main folder on it; did it build; what the baseline capture showed about skipped frames.
3. **Git:** a before/after table of the refs; push confirmations; what happened to the 15 untracked files.
4. **Build:** result, warning count, hashes.
5. **Baseline:** predictions against readings, per leg.
6. **Host and fixture differences** against the release proof, and what you aligned.
7. **Next free milestone and gotcha numbers.**
8. **Safety,** or "none". **Decisions chat must make,** or "none".
