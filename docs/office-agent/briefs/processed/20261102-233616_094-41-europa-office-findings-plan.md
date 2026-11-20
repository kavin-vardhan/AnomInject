# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 094-41 Europa office findings: plan only (custom-depth outlines in saved frames, resolution, D3D12, ExecCmds)

# 094-41 — Plan only. Codex diagnoses why our capture shows as outlines on Europa's game and plans the fix; then Code reviews the plan

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

**Context:** 094-40 was GREEN and the office ran its capture card on Europa's real game. Counts passed, but the game uses custom depth itself, so our census and mask tagging appear as changing outlines in the saved frames (self-check `polluted=1`). This job is **read-only planning**: no source edits, builds, launches, commits or pushes, by anyone.

## Start (quick)
- **The watcher starts you inside the protected `m51` checkout.** Every write uses an absolute path. Receipts go in `D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\code-receipts-094-41\`.
- **Check `m51` against P9 with read-only git** (`GIT_OPTIONAL_LOCKS=0`): HEAD `53bf725`, index `526aba7f`, 0 tracked changes, the 15 untracked entries. Any difference: stop with NEEDS-DECISION.
- **Read only:** `_reviews\094-41-request.md`, `_reviews\094-40-ruling.md`, `_reviews\094-40-code-review.md`, `_reviews\094-40-office-card-europa-capture.md`.
- **Check `feat/europa-port` is at `d1cdc7b`** (read-only git). If not, stop with NEEDS-DECISION.
- **The resume:**
  - Preserve the run folder's previous report outputs.
  - Write `resume-prompt-094-41.txt` (UTF-8, no BOM): the request verbatim; short Code-added context (`m51` status, feature HEAD); the GDP implementer block, with this job's no-edit, no-build, no-launch rule stated first.
  - Resume thread `01a14334`, detached, with W4: same `-Run` and `-Workdir`. **No `-OwnerApprovedFullAccess`** for this job (read-only).
  - Confirm a new turn started. If the resume fails twice, start a fresh thread.

## Poll
- A light poll, about every 10 minutes, one compact line in `code-receipts-094-41\midjob-notes.txt`.
- **Never end the run while Codex runs.** Cap 6 h. **Never steer Codex.**
- If Codex launches the editor or game, or edits a source file, record it and report RED.

## After Codex exits: the review, `_reviews\094-41-code-review.md`, about one page
1. **Diagnosis check:** open every source location Codex cites at `d1cdc7b` and confirm or refute each. Say plainly whether hold-time mask tagging can outline the labelled object in a saved frame (label leakage).
2. **The options:** is the recommended option buildable on 4.25 without engine edits, on D3D11 and D3D12? Any option Codex missed?
3. **The home repro:** does it truly avoid touching the Paris host's Content and Config?
4. **The smaller findings** (resolution, D3D12, -ExecCmds, `client`): agree or disagree, with citations.
5. **Safety:** no edits, builds, launches, commits or pushes happened; `m51` unchanged by git.

**Where you may write:** `code-receipts-094-41\`, the resume prompt, the preserved report outputs, `_reviews\094-41-*`.

## Hard rules
- **No source edits, builds, launches, commits, pushes or history rewrites, by you or Codex.**
- **Delete nothing. Kill nothing,** except, if necessary, the relay's Codex process, by PID.
- **No security, paging, power or machine-setting changes. No elevation.**
- **Never print or copy a secret value.**
- **Don't touch:** `m51`, `master`, `release/2026-10-05`, tags, `ToCodex\`, `E:\<personal-folder>`, `D:\IA_BankOverflow`, `E:\UEProjects\LyraStarterGame`, `E:\Unreal Projects\StylizedParisStreet`.
- **Codenames only:** "Europa" and "Europa's fork". Never name the game, its levels or classes; there is no office data at home and none may be requested.

## Final message
1. **`VERDICT: GREEN | NEEDS-DECISION | RED`.** GREEN: the plan is complete, names a recommended option, and your review agrees. NEEDS-DECISION: the plan is incomplete or you disagree on something that matters. RED: any edit, build, launch, commit or push happened.
2. **A plain summary for the owner** (3 lines): what causes the outlines, and the proposed fix.
3. **Diagnosis** (confirmed or refuted, including label leakage), **the recommended option** and the runners-up, **the home repro**, **the smaller findings**.
4. **Safety,** or "none".
5. **Decisions chat must make,** or "none".
