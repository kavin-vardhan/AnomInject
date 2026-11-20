# ACK-VERDICT: 094-44f NEEDS-DECISION is resolved by _reviews\094-44f-ruling.md (GL1–GL5): −5.0 EV is accepted; the bench locks its view to EA_Camera (or the sweep pose) and re-qualifies GK2 in -game before any fire, with a -game sweep allowed if −5.0 EV fails; runtime_exit stops counting an owned stop with a recorded intent as a crash; C: is recorded per session; 094-45 runs Stage 1 (texture Paris legs, push) and then Stage 2 (camera_clipping, push).
# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 094-45 Europa Phase B: Stage 1 texture Paris regression (view lock, -game re-qualify) and push; Stage 2 camera_clipping port, gates and push

# 094-45 — Codex finishes the textures on Paris, then ports camera_clipping; Code reviews, scans and pushes after each stage

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

**Context:**
- **The textures:** origin is at `8df75de`. Local `983f429` has the texture port: fixture G1–G5 and G4 pass (GI1, GJ1), and Paris D3D11's H4 and texture G1/G2 pass (GJ2). The bench exposure is settled at −5.0 EV (GL1). 094-44f's Paris leg stopped because the `-game` camera wasn't looking at the targets (GL2).
- **This job has two gated stages:**
  - **Stage 1, the textures:**
    1. GL3, the `runtime_exit` fix;
    2. GL2's view lock and `-game` re-qualification;
    3. the Paris legs on D3D11 1280×720, D3D12 1280×720 and D3D12 1279×719: H4, texture G1–G3 whole-frame at 0.98 with clean edges, and G5;
    4. **your scan, push and card.**
  - **Stage 2, camera_clipping:** runs only after Stage 1 is pushed. Steps 7–8 of `_reviews\094-44-request.md` under GE3's D4/D5 and the Phase B plan, then **your scan, push and card update.**
- **The owner's PC is free from 15:15 IST.** Respect the kit's admission gates and GA5's 3 h bound.

## Start (quick)
- **The watcher starts you inside the protected `m51` checkout.** Every write uses an absolute path. Receipts go in `D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\code-receipts-094-45\`.
- **Check `m51` against P9** with read-only git (`GIT_OPTIONAL_LOCKS=0`): HEAD `53bf725`, index `526aba7f`, 0 tracked changes, the 15 untracked entries. Any difference: stop with NEEDS-DECISION.
- **Check `feat/europa-port`:** local at `983f429`, origin at `8df75de`, clean. If not, stop with NEEDS-DECISION.
- **Read only:**
  - `_reviews\094-44f-ruling.md` (GL1–GL5);
  - `_reviews\094-44e-ruling.md` (GK1–GK3);
  - `_reviews\094-44-request.md` (steps 6–10);
  - `_reviews\094-43-ruling.md` (GE3: D4, D5);
  - `_reviews\094-43-phaseB-plan.md` (camera_clipping: §1, §2, §4.6–4.8, §5, §7 R6);
  - `_reviews\094-44f-code-review.md` (reuse your readers).
- **Take your own hashes** of:
  - the Paris host's own `Content`, `Config`, descriptor and `.uproject`;
  - the fixture's assets and preserved `Engine.ini`;
  - the 4.25 and 5.1 material `.uasset`s.

  Record C:'s free space (GL4).
- **The resume:**
  - Preserve the previous report outputs.
  - Write `resume-prompt-094-45.txt` (UTF-8, no BOM). It holds:
    - the `094-44-request.md` text;
    - GL1–GL5 verbatim;
    - the line "This job runs GL5's Stage 1 (GL3, GL2, three Paris legs, then Code pushes), then Stage 2 (camera_clipping, steps 7–8; Stage 2 starts only after Code confirms the Stage 1 push)";
    - short Code-added context;
    - the GDP implementer block as used in 094-43r.
  - Resume thread `01a14334`, detached, with W4, plus `-OwnerApprovedFullAccess`.
  - If the resume fails twice, start a fresh full-access thread.
  - **How the Stage 1 push and Stage 2 hand over:**
    - Codex commits Stage 1 and writes `STAGE1-READY.txt` (the tip SHA) in the run folder, then waits for `STAGE1-PUSHED.txt` before Stage 2's first build.
    - You review Stage 1, scan and push. If Stage 1 fails, write `STAGE1-FAILED.txt` with the reason, and Codex stops.
    - Put this handshake in the resume prompt.

## Poll
- **Cadence:** every 2–3 minutes while a build or session is likely; otherwise about 5 minutes.
- **One line per poll** in `code-receipts-094-45\midjob-notes.txt`:
  - free space (C:, D:, E:) and commit;
  - build processes;
  - our editor, game, crash-monitor and ShaderCompileWorker PIDs;
  - how long any admission gate has held;
  - the current stage and step;
  - whether `STAGE1-READY.txt` exists.
- **Use UTC epoch seconds.**
- **Never end the run while Codex runs.** The cap is 24 h. **Never steer Codex** beyond the handshake files.

## Stage 1 review, scan and push (when `STAGE1-READY.txt` appears)
1. **GL3:** the diff and its can-fail tests (an empty-reason owned stop → `owned-stop`; a real exit with no intent → `crash`; another PID's intent ignored).
2. **GL2:**
   - the view lock, and the in-frame check of every target (centres and drawn share) on `-game` frames;
   - GK2's checks re-qualified at the exposure used, recomputed by you from the real frames;
   - any `-game` sweep.
3. **The three Paris legs,** recomputed with your readers:
   - H4's gates;
   - texture G1–G3, whole-frame IoU ≥ 0.98, with the frame before each fire and the frames after each revert at 0 pixels changed;
   - G5;
   - `unavailable` counts;
   - C: before and after each session.
4. **If all of them pass:**
   - the pre-push scan of `8df75de..<Stage 1 tip>` to `_reviews\094-45-stage1-prepush-scan.md`;
   - the push under the 094-39 rules: `feat/europa-port` only, no tags, backup refs or force, a fast-forward only, confirmed with `ls-remote`;
   - the card `_reviews\094-45-office-card-europa.md`: the new commit, the default pool (blinking, lod_corruption, missing_texture, corrupted_texture), and numbers-only reads;
   - then write `STAGE1-PUSHED.txt`.
5. **Otherwise** write `STAGE1-FAILED.txt` and push nothing.

## Stage 2 review, scan and push (after Codex exits)
1. **The port diff against `d7eee97`:** behaviour, names, the `camera_clipping.*` row keys, the run-summary counters and the config keys unchanged; only compat-level differences, with citations.
2. **D4:**
   - at least 3 distinct slab-positive spans in one held session on the near-wall pose schedule;
   - every labelled frame differs from the same-seed control at the baseline near plane, and every unlabelled frame matches it;
   - the near plane is restored at Stop;
   - **the can-fail:** a held session at the baseline near plane labels 0 frames.
3. **The Paris regression with the route requested,** on D3D11 and D3D12.
4. **D5:** camera_clipping is never in the auto pool, and the defaults are unchanged.
5. **If all of them pass:** the pre-push scan of `<Stage 1 tip>..<tip>` to `_reviews\094-45-stage2-prepush-scan.md`, the push under the same rules, and the card updated with how to request camera_clipping for a held session.

## Safety (both stages)
- any crash or upload sign;
- the Paris host's own Content, Config, descriptor and `.uproject` hash-equal;
- only our branch changed plugin Content;
- the 4.25 and 5.1 material `.uasset`s hash-equal;
- `m51` checked by git;
- binaries changed only in builds;
- nothing deleted except by receipt;
- the spikes local;
- C: per session.

**Where you may write:**
- `code-receipts-094-45\`;
- the resume prompt and the handshake files in the run folder;
- the preserved report outputs;
- the `_reviews\094-45-*` files;
- `D:\IA_Europa\Receipts\CodeReview09445\`;
- `D:\IA_Europa\Temp\CodeReview09445\`;
- and the two pushes.

## Hard rules
- **No work by you** beyond the reviews, the scans, the pushes, the cards and the handshake files: no source edits, builds, launches, commits or history rewrites. Codex does the work.
- **Never touch the editor, the game, their windows or their dialogs.** Never touch the owner's processes.
- **Delete nothing.** No reparse points.
- **Kill nothing,** except, if necessary, the relay's Codex process, by PID.
- **Never print or copy a secret value.**
- **No security, paging, power or machine-setting changes. No elevation.**
- **Don't touch:** `m51`, `master`, `release/2026-10-05`, tags, `ToCodex\`, `E:\<personal-folder>`, `E:\UEProjects\LyraStarterGame`, `E:\Unreal Projects\StylizedParisStreet`.
- **Codenames only.**

## Final message
1. **`VERDICT: GREEN | NEEDS-DECISION | RED`.**
   - **GREEN:** both stages pass by your own checks and both pushes are confirmed.
   - **NEEDS-DECISION:** a gate is short or GA5's bound was hit. Say which stage, and what was pushed.
   - **RED:** a second crash, an upload sign, a change to the Paris host's own Content or Config, a changed material `.uasset`, or a non-fast-forward push.
2. **A plain summary for the owner** (3 lines): do the textures pass on Paris and are they pushed in the default set; does camera clipping work; and which office card to use.
3. **Stage 1 with numbers; Stage 2 with numbers; the pushes and the card.**
4. **Safety,** or "none".
5. **Decisions chat must make,** or "none".
