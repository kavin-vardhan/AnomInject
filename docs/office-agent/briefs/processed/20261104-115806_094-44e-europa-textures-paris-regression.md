# ACK-VERDICT: 094-44d NEEDS-DECISION is resolved by _reviews\094-44d-ruling.md (GJ1–GJ5): G4/G5 and Paris D3D11's H4 and texture G1/G2 are accepted; texture G3 on Paris failed on EA_Core's overexposure and bloom, so apply GH2's bench-only fix to EA_Core (option a) and rerun the three Paris legs; the launch reserve goes back to 3 GiB; push the textures' default-route change on its own gates; camera_clipping stays 094-45.
# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 094-44e Europa Phase B textures: EA_Core bench exposure fix, Paris regression D3D11/D3D12/odd width, scan and push (hard stop 13:35 IST)

# 094-44e — Codex applies the bench exposure fix to EA_Core and reruns the Paris regression; Code reviews, scans and pushes

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

**Context:**
- **Where we are:** origin is at `8df75de`. Local `4cc8b45` has the texture port with G1–G5 passed on the fixture (GI1, GJ1), and Paris D3D11's H4 and texture G1/G2 passed (GJ2). Paris texture G3 failed only on EA_Core's overexposure and bloom.
- **This job:** GJ5 only.
  1. GJ3's EA_Core bench fix: bench-only, runtime-only, under `-IAIBench`, never saved, nothing in the Paris host's Content or Config. Bloom off, manual exposure with the targets' mean at 0.2–0.8 of full scale, and the pre-fire checks.
  2. The Paris regression on all three legs.
  3. Your scan, push and card.

  The launch reserve goes back to 3 GiB (GJ4). **camera_clipping is NOT in this job** (094-45).
- **The design:** `_reviews\094-43-phaseB-plan.md`, accepted by chat (GE3, GE4, GF4, GF5).
- **⛔ HARD STOP: the owner's laptop is in personal use 13:45–15:15 IST.**
  - No build or launch starts after **13:10 IST**.
  - All editor, game and build processes are closed by **13:35 IST**.
  - Your pixel readers finish by **13:35**; after that, only writing text.
  - Anything unfinished: NEEDS-DECISION with partial results. Never push anything that didn't pass.
  - Put the stop times in the resume prompt for Codex.

## Start (quick)
- **The watcher starts you inside the protected `m51` checkout.** Every write uses an absolute path. Receipts go in `D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\code-receipts-094-44e\`.
- **Check `m51` against P9** with read-only git (`GIT_OPTIONAL_LOCKS=0`): HEAD `53bf725`, index `526aba7f`, 0 tracked changes, the 15 untracked entries. Any difference: stop with NEEDS-DECISION.
- **Check `feat/europa-port`:** local at `4cc8b45`, origin at `8df75de`, clean. If not, stop with NEEDS-DECISION.
- **Read only:**
  - `_reviews\094-44d-ruling.md` (GJ1–GJ5);
  - `_reviews\094-44c-ruling.md` (GI1–GI7);
  - `_reviews\094-44r-ruling.md` (GH1–GH6);
  - `_reviews\094-44-ruling.md` (GG1–GG3);
  - `_reviews\094-44-request.md`;
  - `_reviews\094-44d-code-review.md` (my previous review; reuse my readers, especially the Paris checker);
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
  - Write `resume-prompt-094-44e.txt` (UTF-8, no BOM), with the request verbatim, then GJ1–GJ5 from `094-44d-ruling.md` and GI7's stop times verbatim, and the line "This job covers GJ5's scope only: the EA_Core bench exposure fix and the Paris regression on three legs. Steps 7–8 (camera_clipping) are not in this job.", short Code-added context, and the GDP implementer block as used in 094-43r.
  - Resume thread `01a14334`, detached, with W4, plus `-OwnerApprovedFullAccess`.
  - If the resume fails twice, start a fresh full-access thread.

## Poll
- **Cadence:** every 2–3 minutes while a build or session is likely; otherwise about 5 minutes.
- **One line per poll** in `code-receipts-094-44e\midjob-notes.txt`:
  - free space and commit;
  - build processes;
  - our editor, game, crash-monitor and ShaderCompileWorker PIDs;
  - how long any admission gate has held;
  - the current step.
- **Use UTC epoch seconds.**
- **Never end the run while Codex runs.** The cap is 24 h. **Never steer Codex.**
- **While you wait,** prepare your readers for G1–G5 (texture region change, mask against the colour-change silhouette, paired new-magenta, and the Paris regression checks).

## After Codex exits: the review, `_reviews\094-44e-code-review.md`, one to two pages
0. **GJ3:** the EA_Core bench settings as recorded, and proof they are runtime-only and bench-only (Paris host Content and Config hash-equal). The pre-fire checks (target mean 0.2–0.8, checker greys ≥ 40 apart, pink R,B ≥ 200 and G ≤ 80), recomputed from the real frames. The launch reserve back at 3 GiB. Every FE3b child recorded this run.
1. **G5 on Paris, where the kit records it:** `r.CustomDepth` unchanged, 0 custom-depth components, `polluted=0`, residual 0.
2. **(Fixture G4 is already accepted, GJ1.)**
3. **The Paris regression** (`-game`) on D3D11 1280×720, D3D12 1280×720 and D3D12 1279×719, with the textures in the default pool:
   - all of H4's earlier gates;
   - G1–G3 for the textures: whole-frame G3 at IoU ≥ 0.98, with the edge frames clean;
   - `unavailable` counts.
4. **The defaults:** blinking, lod_corruption, missing_texture and corrupted_texture. camera_clipping is untouched and still compiled out.
5. **The stop times kept:** the last build or launch start, and when the last process closed (IST).
6. **Safety:**
   - any crash or upload sign;
   - the Paris host's own Content, Config, descriptor and `.uproject` hash-equal;
   - only our branch changed plugin Content;
   - `m51` checked by git;
   - binaries changed only in builds;
   - nothing deleted except by receipt;
   - the spikes local.

## Push (yours), only if every gate passes by your own review
1. **Write the pre-push scan** of `8df75de..<tip>` to `_reviews\094-44e-prepush-scan.md`, as in 094-43r.
2. **If it is clean, push `feat/europa-port`** under the 094-39 rules: that branch only, no tags, backup refs or force, a fast-forward only, confirmed with `ls-remote`.
3. **Update the office card** to `_reviews\094-44e-office-card-europa.md`:
   - the new commit;
   - the default pool now includes the two textures;
   - numbers-only reads.

**If a gate failed, push the passing earlier steps only if they stand alone and the scan is clean, and never the default-route change without its passing gates (GH4).** Say exactly what was pushed.

**Where you may write:**
- `code-receipts-094-44e\`;
- the resume prompt;
- the preserved report outputs;
- the `_reviews\094-44e-*` files;
- `D:\IA_Europa\Receipts\CodeReview09444e\`;
- `D:\IA_Europa\Temp\CodeReview09444e\`;
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
   - **GREEN:** the Paris regression passes on all three legs (H4 plus texture G1–G3), the scan is clean, the push is confirmed, and the card is updated.
   - **NEEDS-DECISION:** a gate is short, GA5's bound was hit, or the 13:10/13:35 stop cut the job short. Say what was pushed, if anything.
   - **RED:** a second crash, an upload sign, a change to the Paris host's own Content or Config, a changed 5.1 `.uasset`, or a non-fast-forward push.
2. **A plain summary for the owner** (2 lines): does Paris pass on D3D11 and D3D12 with the textures on by default, and is it pushed with an updated card?
3. **The EA_Core fix; the Paris regression, with numbers; the push and the card; the stop times.**
4. **Safety,** or "none".
5. **Decisions chat must make,** or "none".
