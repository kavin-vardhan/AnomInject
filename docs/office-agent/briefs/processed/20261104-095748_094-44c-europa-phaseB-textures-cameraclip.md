# ACK-VERDICT: 094-44c NEEDS-DECISION is resolved by _reviews\094-44c-ruling.md (GH1–GH6): FE3b, the loaders and GF4 are accepted and pushed (8df75de); G3 failed on the overexposed, bloom-on bench scene, so fix the fixture (bloom off, fixed exposure, contrast checks) and keep the whole-frame 0.98 bar; bring a static target into the 0.5–25% band by pose or scale; push the default-route change only with passing gates; mutant materials go in fixture Content only; 094-44c resumes from 2c99a6f.
# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 094-44c Europa Phase B (resume from 2c99a6f, fixture exposure fix): missing_texture, corrupted_texture and camera_clipping on 4.25 (from 5.1 release d7eee97), live skeletal/ISM coverage, gates on fixture and Paris D3D11/D3D12, scan and push

# 094-44c — Codex fixes the bench scene's exposure, re-runs the texture gates, then the Paris regression and camera_clipping; Code reviews, scans and pushes

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

**Context:**
- **Where we are:** FZ2 is in, and 094-44r pushed FE3b, the 4.25 materials and GF4 (`8df75de`). The texture port is local (`2c99a6f`); its G3 failed only on the overexposed, bloom-on bench scene.
- **This job:** Phase B, the owner's top priority. missing_texture (checkered), corrupted_texture (pink) and camera_clipping, ported **unchanged** from the 5.1 release line `d7eee97`.
- **The design:** `_reviews\094-43-phaseB-plan.md`, accepted by chat (GE3, GE4, GF4, GF5).
- **The owner may use the PC in the morning:** respect the kit's admission gates and GA5's 3 h bound.

## Start (quick)
- **The watcher starts you inside the protected `m51` checkout.** Every write uses an absolute path. Receipts go in `D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\code-receipts-094-44c\`.
- **Check `m51` against P9** with read-only git (`GIT_OPTIONAL_LOCKS=0`): HEAD `53bf725`, index `526aba7f`, 0 tracked changes, the 15 untracked entries. Any difference: stop with NEEDS-DECISION.
- **Check `feat/europa-port`:** local at `2c99a6f`, origin at `8df75de`, clean. If not, stop with NEEDS-DECISION.
- **Read only:**
  - `_reviews\094-44r-ruling.md` (GH1–GH6);
  - `_reviews\094-44-ruling.md` (GG1–GG3);
  - `_reviews\094-44-request.md`;
  - `_reviews\094-44r-code-review.md` (my previous review; reuse my readers);
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
  - Write `resume-prompt-094-44c.txt` (UTF-8, no BOM), with the request verbatim, then GH1–GH6 from `094-44r-ruling.md` verbatim, short Code-added context, and the GDP implementer block as used in 094-43r.
  - Resume thread `01a14334`, detached, with W4, plus `-OwnerApprovedFullAccess`.
  - If the resume fails twice, start a fresh full-access thread.

## Poll
- **Cadence:** every 2–3 minutes while a build or session is likely; otherwise about 5 minutes.
- **One line per poll** in `code-receipts-094-44c\midjob-notes.txt`:
  - free space and commit;
  - build processes;
  - our editor, game, crash-monitor and ShaderCompileWorker PIDs;
  - how long any admission gate has held;
  - the current step.
- **Use UTC epoch seconds.**
- **Never end the run while Codex runs.** The cap is 24 h. **Never steer Codex.**
- **While you wait,** prepare your readers for G1–G5 (texture region change, mask against the colour-change silhouette, paired new-magenta, and camera_clipping against its same-seed control).

## After Codex exits: the review, `_reviews\094-44c-code-review.md`, one to two pages
0. **GH2/GH3:** the bench settings as recorded; the pre-fire contrast checks (checker greys ≥ 40 levels apart, pink R,B ≥ 200 and G ≤ 80); the static target's coverage in the band. Every FE3b child recorded this run, with its mode.
1. **The port diff against `d7eee97`:**
   - behaviour, names, label format, row and summary keys, and config keys unchanged;
   - only compat-level differences, with citations.
2. **The materials:** hash-equal to `8df75de`, and the 5.1 `.uasset`s hash-equal. Any new mutant material is in fixture Content only, with its hash (GH5).
3. **G1 split:** at least 3 fires of each texture anomaly on a static target and at least 3 on the skeletal target. The runtime can-fails (the grey usage-flag mutant fails G2, the one-frame-early label fails G3) shown failing.
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
1. **Write the pre-push scan** of `8df75de..<tip>` to `_reviews\094-44c-prepush-scan.md`, as in 094-43r.
2. **If it is clean, push `feat/europa-port`** under the 094-39 rules: that branch only, no tags, backup refs or force, a fast-forward only, confirmed with `ls-remote`.
3. **Update the office card** to `_reviews\094-44c-office-card-europa.md`:
   - the new commit;
   - the default pool now includes the two textures;
   - how to request camera_clipping for a held session;
   - numbers-only reads.

**If a gate failed, push the passing earlier steps only if they stand alone and the scan is clean, and never the default-route change without its passing gates (GH4).** Say exactly what was pushed.

**Where you may write:**
- `code-receipts-094-44c\`;
- the resume prompt;
- the preserved report outputs;
- the `_reviews\094-44c-*` files;
- `D:\IA_Europa\Receipts\CodeReview09444c\`;
- `D:\IA_Europa\Temp\CodeReview09444c\`;
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
   - **GREEN:** all three anomalies pass by your own checks, the Paris regression passes on both RHIs, the scan is clean, the push is confirmed, and the card is updated.
   - **NEEDS-DECISION:** a gate is short, or GA5's bound was hit. Say what was pushed, if anything.
   - **RED:** a second crash, an upload sign, a change to the Paris host's own Content or Config, a changed 5.1 `.uasset`, or a non-fast-forward push.
2. **A plain summary for the owner** (4 lines): do the checkered and pink textures work and label correctly, does camera clipping work, does Paris still pass, and is it pushed with an updated card?
3. **The fixture fix; per anomaly, with numbers; the Paris regression; the push and the card.**
4. **Safety,** or "none".
5. **Decisions chat must make,** or "none".
