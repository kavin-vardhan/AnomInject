# ACK-VERDICT: 094-46 NEEDS-DECISION is resolved by _reviews\094-46-ruling.md (GO1–GO4): GN4 and D3D11's combined GN3 sessions are accepted; D4 additionally seals a near-plane floor (a held near-100 session with no near holds against a near-10 control) and an unlabelled frame matches within the larger of the two floors; one default-only run (D3D12 1279×719) satisfies GN3's key diff; 094-46r continues from eea2acd.
# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 094-46r Europa camera_clipping: near-plane floor (GO2), D4 on D3D11/D3D12, D3D12 legs, default-only key diff, scan and push

# 094-46r — Codex finishes camera_clipping; Code reviews, scans and pushes

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

**Context:**
- **Where we are:** origin `feat/europa-port` = `c5d0919`. Local `eea2acd` has the camera_clipping port plus GN4's counter and the combined bench schedule.
- **094-46:** D3D11's session floor is sealed (max 1 level), and D3D11's three combined sessions pass H4, the strict texture gates and G5 (GO1). D4's spans are exact, but unlabelled frames in the positive show ±2-level speckle, apparently from the near-plane change itself.
- **This job (GO4):**
  1. **GO2's near-plane floor on D3D11:** a held near-100 session with **no near holds** (a guarded bench switch, `-IAIBench` on EA_Core only, so nothing can be clipped; its labels must read 0), paired against a near-10 control with the same seed. Seal its max |Δ| and its max count at that level before judging. Then judge D3D11's existing positive: every unlabelled frame within the **larger** of the session floor and the near-plane floor, and the labelled spans far above both.
  2. **D3D12 1280×720:** the session floor pair, the near-plane floor pair and the positive. Seal both floors first, then judge D4 the same way.
  3. **D3D12 1279×719:** the combined leg (H4 and strict texture G1–G3).
  4. **GO3's default-only run** (D3D12 1279×719) against `c5d0919`'s outputs: no key removed, no type changed, and the card's read commands work.
  5. **Your scan, push and card update.**

  Reuse 094-46's accepted D3D11 sessions; don't re-run them.
- **The PC belongs to the owner.** Respect the kit's admission gates and GA5's 3 h bound.

## Start (quick)
- **The watcher starts you inside the protected `m51` checkout.** Every write uses an absolute path. Receipts go in `D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\code-receipts-094-46r\`.
- **Check `m51` against P9** with read-only git (`GIT_OPTIONAL_LOCKS=0`): HEAD `53bf725`, index `526aba7f`, 0 tracked changes, the 15 untracked entries. Any difference: stop with NEEDS-DECISION.
- **Check `feat/europa-port`:** local at `eea2acd`, origin at `c5d0919`, clean. If not, stop with NEEDS-DECISION.
- **Read only:**
  - `_reviews\094-46-ruling.md` (GO1–GO4);
  - `_reviews\094-45r-ruling.md` (GN1–GN5);
  - `_reviews\094-46-code-review.md` (reuse your floor, D4, H4 and key-diff readers);
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
  - Write `resume-prompt-094-46r.txt` (UTF-8, no BOM), with GN1–GN5 and GO1–GO4 verbatim, steps 7–10 of `094-44-request.md`, the job list above, short Code-added context, and the GDP implementer block as used in 094-43r.
  - Resume thread `01a14334`, detached, with W4, plus `-OwnerApprovedFullAccess`.
  - If the resume fails twice, start a fresh full-access thread.

## Poll
- **Cadence:** every 2–3 minutes while a build or session is likely; otherwise about 5 minutes.
- **One line per poll** in `code-receipts-094-46r\midjob-notes.txt`:
  - free space (C:, D:, E:) and commit;
  - build processes;
  - our editor, game, crash-monitor and ShaderCompileWorker PIDs;
  - how long any admission gate has held;
  - the current step.
- **Use UTC epoch seconds.**
- **Never end the run while Codex runs.** The cap is 24 h. **Never steer Codex.**

## After Codex exits: the review, `_reviews\094-46r-code-review.md`
1. **GO2 on both RHIs:**
   - both floors, recorded before each positive (max |Δ| and max count at that level, per frame);
   - the near-plane floor session labels 0 and has no near holds;
   - D4 recomputed by you: every unlabelled frame within the larger floor, every labelled span far above it, the baseline can-fail labels 0, and the near plane restored.
2. **The D3D12 legs,** recomputed: H4 and strict texture G1–G3 (IoU 1.000, 0 spill, clean edges, no revert residue), and G5.
3. **GO3:** the default-only key diff against `c5d0919` (no key removed, no type changed), and the card's read commands run on it.
4. **D5:** camera_clipping is never in the auto pool, and the defaults are unchanged.

## Push (yours), only if every gate passes by your own review
1. **Write the pre-push scan** of `c5d0919..<tip>` to `_reviews\094-46r-prepush-scan.md`.
2. **Push `feat/europa-port`** under the 094-39 rules: that branch only, no tags, backup refs or force, a fast-forward only, confirmed with `ls-remote`.
3. **Update the card** to `_reviews\094-46r-office-card-europa.md`: the new commit, the defaults unchanged, and how to request camera_clipping for a held session.

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
- `code-receipts-094-46r\`;
- the resume prompt;
- the preserved report outputs;
- the `_reviews\094-46r-*` files;
- `D:\IA_Europa\Receipts\CodeReview09446r\`;
- `D:\IA_Europa\Temp\CodeReview09446r\`;
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
   - **GREEN:** GO2's D4 passes on both RHIs, the D3D12 legs pass, GO3's key diff passes, the scan is clean, the push is confirmed, and the card is updated.
   - **NEEDS-DECISION:** a gate is short or GA5's bound was hit.
   - **RED:** a second crash, an upload sign, a change to the Paris host's own Content or Config, a changed material `.uasset`, or a non-fast-forward push.
2. **A plain summary for the owner** (2 lines): does camera clipping pass and is it pushed, and which office card to use.
3. **GO2's floors and D4 per RHI; the D3D12 legs; GO3; the push and the card.**
4. **Safety,** or "none".
5. **Decisions chat must make,** or "none".
