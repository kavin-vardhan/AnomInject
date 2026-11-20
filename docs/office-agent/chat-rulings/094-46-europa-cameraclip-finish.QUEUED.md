# ACK-VERDICT: 094-45r NEEDS-DECISION is resolved by _reviews\094-45r-ruling.md (GN1–GN5): Stage 1 (textures) is accepted and pushed (c5d0919); camera_clipping's behaviour is right, and D4's "unlabelled frames match" is judged against a measured session noise floor (GN2); the Paris regression runs the default routes plus camera_clipping on three legs and checks the shared path is additive-only (GN3); port the missing unconfirmed counter (GN4).
# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 094-46 Europa camera_clipping: noise-floor control pair, D4 on D3D11/D3D12, Paris regression (default + camera_clipping) on three legs, scan and push

# 094-46 — Codex finishes camera_clipping; Code reviews, scans and pushes

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

**Context:**
- **Where we are:** origin `feat/europa-port` = `c5d0919`. That's FZ2 plus the Phase B textures, in the default set. Local `c706b7c` is the camera_clipping port.
- **094-45r's D4 run:** the three slab spans were labelled exactly, and the baseline-near can-fail labelled 0. Unlabelled frames differed from the control by 1–3 px of ±3 levels, which looks like session noise.
- **This job (GN5):**
  1. GN4: port `label_transition_camera_clipping_unconfirmed_entries` as in `d7eee97`.
  2. GN2: a control pair (two baseline-near sessions, same seed and bench settings) to measure the noise floor. Record its thresholds before comparing.
  3. D4 judged against the floor, on D3D11 1280×720 and D3D12 1280×720.
  4. GN3: the Paris regression with the default routes **plus** camera_clipping requested, on D3D11 1280×720, D3D12 1280×720 and D3D12 1279×719. H4, texture G1–G3, D4 under GN2, and a default run without camera_clipping additive-only against `c5d0919`'s outputs.
  5. Your scan, push and card update.
- **The PC belongs to the owner.** Respect the kit's admission gates and GA5's 3 h bound.

## Start (quick)
- **The watcher starts you inside the protected `m51` checkout.** Every write uses an absolute path. Receipts go in `D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\code-receipts-094-46\`.
- **Check `m51` against P9** with read-only git (`GIT_OPTIONAL_LOCKS=0`): HEAD `53bf725`, index `526aba7f`, 0 tracked changes, the 15 untracked entries. Any difference: stop with NEEDS-DECISION.
- **Check `feat/europa-port`:** local at `c706b7c`, origin at `c5d0919`, clean. If not, stop with NEEDS-DECISION.
- **Read only:**
  - `_reviews\094-45r-ruling.md` (GN1–GN5);
  - `_reviews\094-45r-code-review.md` (reuse your D4 and Paris readers);
  - `_reviews\094-44-request.md` (steps 7–10);
  - `_reviews\094-43-ruling.md` (GE3: D4, D5);
  - `_reviews\094-45r-office-card-europa.md`.
- **Take your own hashes** of:
  - the Paris host's own `Content`, `Config`, descriptor and `.uproject`;
  - the fixture's assets;
  - the 4.25 and 5.1 material `.uasset`s.

  Record C:'s free space.
- **The resume:**
  - Preserve the previous report outputs.
  - Write `resume-prompt-094-46.txt` (UTF-8, no BOM), with GN1–GN5 verbatim, steps 7–10 of `094-44-request.md`, the job list above, short Code-added context, and the GDP implementer block as used in 094-43r.
  - Resume thread `01a14334`, detached, with W4, plus `-OwnerApprovedFullAccess`.
  - If the resume fails twice, start a fresh full-access thread.

## Poll
- **Cadence:** every 2–3 minutes while a build or session is likely; otherwise about 5 minutes.
- **One line per poll** in `code-receipts-094-46\midjob-notes.txt`:
  - free space (C:, D:, E:) and commit;
  - build processes;
  - our editor, game, crash-monitor and ShaderCompileWorker PIDs;
  - how long any admission gate has held;
  - the current step.
- **Use UTC epoch seconds.**
- **Never end the run while Codex runs.** The cap is 24 h. **Never steer Codex.**

## After Codex exits: the review, `_reviews\094-46-code-review.md`
1. **GN4:** the counter's port, against `d7eee97`.
2. **GN2:** the control pair's noise floor (max |Δ|, and the max count at that level, per frame), recorded before the comparison. D4 on both RHIs, recomputed by you: every unlabelled frame within the floor, every labelled span far above it, the can-fail labels 0, and the near plane restored.
3. **GN3:** the three Paris legs recomputed: H4, texture G1–G3 (strict), and D4. The additive-only diff of a default run against `c5d0919`'s outputs (no key removed or changed). The card's read commands still work.
4. **D5:** camera_clipping is never in the auto pool, and the defaults are unchanged.

## Push (yours), only if every gate passes by your own review
1. **Write the pre-push scan** of `c5d0919..<tip>` to `_reviews\094-46-prepush-scan.md`.
2. **Push `feat/europa-port`** under the 094-39 rules: that branch only, no tags, backup refs or force, a fast-forward only, confirmed with `ls-remote`.
3. **Update the card** to `_reviews\094-46-office-card-europa.md`: the new commit, the defaults unchanged, and how to request camera_clipping for a held session.

## Safety
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
- `code-receipts-094-46\`;
- the resume prompt;
- the preserved report outputs;
- the `_reviews\094-46-*` files;
- `D:\IA_Europa\Receipts\CodeReview09446\`;
- `D:\IA_Europa\Temp\CodeReview09446\`;
- and the push.

## Hard rules
- **No work by you** beyond the review, the scan, the push and the card: no source edits, builds, launches, commits or history rewrites. Codex does the work.
- **Never touch the editor, the game, their windows or their dialogs.** Never touch the owner's processes.
- **Delete nothing.** No reparse points.
- **Kill nothing,** except, if necessary, the relay's Codex process, by PID.
- **Never print or copy a secret value.**
- **No security, paging, power or machine-setting changes. No elevation.**
- **Don't touch:** `m51`, `master`, `release/2026-10-05`, tags, `ToCodex\`, `E:\<personal-folder>`, `E:\UEProjects\LyraStarterGame`, `E:\Unreal Projects\StylizedParisStreet`.
- **Codenames only.**

## Final message
1. **`VERDICT: GREEN | NEEDS-DECISION | RED`.**
   - **GREEN:** GN2's D4 passes on both RHIs, GN3's three legs pass, the scan is clean, the push is confirmed, and the card is updated.
   - **NEEDS-DECISION:** a gate is short or GA5's bound was hit.
   - **RED:** a second crash, an upload sign, a change to the Paris host's own Content or Config, a changed material `.uasset`, or a non-fast-forward push.
2. **A plain summary for the owner** (2 lines): does camera clipping pass and is it pushed, and which office card to use.
3. **GN4; GN2's floor and D4; GN3's legs; the push and the card.**
4. **Safety,** or "none".
5. **Decisions chat must make,** or "none".
