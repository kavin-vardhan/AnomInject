# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 094-44 Europa Phase B: missing_texture, corrupted_texture and camera_clipping on 4.25 (from 5.1 release d7eee97), live skeletal/ISM coverage, gates on fixture and Paris D3D11/D3D12, scan and push

# 094-44 — Codex ports the three Phase B anomalies; Code reviews, scans and pushes

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

**Context:**
- **Where we are:** 094-43r was GREEN. FZ2's private ID/depth pass is in, and `feat/europa-port` = `93613ae` is pushed.
- **This job:** Phase B, the owner's top priority. missing_texture (checkered), corrupted_texture (pink) and camera_clipping, ported **unchanged** from the 5.1 release line `d7eee97`.
- **The design:** `_reviews\094-43-phaseB-plan.md`, accepted by chat (GE3, GE4, GF4, GF5).
- **The owner may use the PC in the morning:** respect the kit's admission gates and GA5's 3 h bound.

## Start (quick)
- **The watcher starts you inside the protected `m51` checkout.** Every write uses an absolute path. Receipts go in `D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\code-receipts-094-44\`.
- **Check `m51` against P9** with read-only git (`GIT_OPTIONAL_LOCKS=0`): HEAD `53bf725`, index `526aba7f`, 0 tracked changes, the 15 untracked entries. Any difference: stop with NEEDS-DECISION.
- **Check `feat/europa-port`:** local and origin both at `93613ae`, clean. If not, stop with NEEDS-DECISION.
- **Read only:**
  - `_reviews\094-44-request.md`;
  - `_reviews\094-43r-ruling.md`;
  - `_reviews\094-43-ruling.md`;
  - `_reviews\094-43-phaseB-plan.md`;
  - `_reviews\094-43r-code-review.md`.
- **Take your own hashes** of:
  - the Paris host's own `Content`, `Config`, descriptor and `.uproject`;
  - the fixture's assets and preserved `Engine.ini`;
  - the two 5.1 material `.uasset`s.
- **The resume:**
  - Preserve the previous report outputs.
  - Write `resume-prompt-094-44.txt` (UTF-8, no BOM), with the request verbatim, short Code-added context, and the GDP implementer block as used in 094-43r.
  - Resume thread `01a14334`, detached, with W4, plus `-OwnerApprovedFullAccess`.
  - If the resume fails twice, start a fresh full-access thread.

## Poll
- **Cadence:** every 2–3 minutes while a build or session is likely; otherwise about 5 minutes.
- **One line per poll** in `code-receipts-094-44\midjob-notes.txt`:
  - free space and commit;
  - build processes;
  - our editor, game, crash-monitor and ShaderCompileWorker PIDs;
  - how long any admission gate has held;
  - the current step.
- **Use UTC epoch seconds.**
- **Never end the run while Codex runs.** The cap is 24 h. **Never steer Codex.**
- **While you wait,** prepare your readers for G1–G5 (texture region change, mask against the colour-change silhouette, paired new-magenta, and camera_clipping against its same-seed control).

## After Codex exits: the review, `_reviews\094-44-code-review.md`, one to two pages
1. **The port diff against `d7eee97`:**
   - behaviour, names, label format, row and summary keys, and config keys unchanged;
   - only compat-level differences, with citations.
2. **The materials:** the readback of every flag and constant. The 5.1 `.uasset`s are hash-equal.
3. **GF4:** the skeletal and instanced targets, with their IoU and new magenta.
4. **Per anomaly:** G1–G5 recomputed with your own readers, the can-fails shown failing, and one example frame you describe.
5. **The Paris regression** on D3D11 and D3D12 (including 1279×719), with the textures in the default pool, and camera_clipping requested.
6. **The defaults:** blinking, lod_corruption, missing_texture and corrupted_texture. camera_clipping is opt-in only.
7. **Safety:**
   - any crash or upload sign;
   - the Paris host's own Content, Config, descriptor and `.uproject` hash-equal;
   - only our branch changed plugin Content;
   - `m51` checked by git;
   - binaries changed only in builds;
   - nothing deleted except by receipt;
   - the spikes local.

## Push (yours), only if every gate passes by your own review
1. **Write the pre-push scan** of `93613ae..<tip>` to `_reviews\094-44-prepush-scan.md`, as in 094-43r.
2. **If it is clean, push `feat/europa-port`** under the 094-39 rules: that branch only, no tags, backup refs or force, a fast-forward only, confirmed with `ls-remote`.
3. **Update the office card** to `_reviews\094-44-office-card-europa.md`:
   - the new commit;
   - the default pool now includes the two textures;
   - how to request camera_clipping for a held session;
   - numbers-only reads.

**If a gate failed, push the passing earlier steps only if they stand alone and the scan is clean.** Say exactly what was pushed.

**Where you may write:**
- `code-receipts-094-44\`;
- the resume prompt;
- the preserved report outputs;
- the `_reviews\094-44-*` files;
- `D:\IA_Europa\Receipts\CodeReview09444\`;
- `D:\IA_Europa\Temp\CodeReview09444\`;
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
   - **GREEN:** all three anomalies and GF4 pass by your own checks, the Paris regression passes on both RHIs, the scan is clean, the push is confirmed, and the card is updated.
   - **NEEDS-DECISION:** a gate is short, or GA5's bound was hit. Say what was pushed, if anything.
   - **RED:** a second crash, an upload sign, a change to the Paris host's own Content or Config, a changed 5.1 `.uasset`, or a non-fast-forward push.
2. **A plain summary for the owner** (4 lines): do the checkered and pink textures work and label correctly, does camera clipping work, does Paris still pass, and is it pushed with an updated card?
3. **Per anomaly, with numbers; GF4; the Paris regression; the push and the card.**
4. **Safety,** or "none".
5. **Decisions chat must make,** or "none".
