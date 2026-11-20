# ACK-VERDICT: 082-01 NEEDS-DECISION ruled in _reviews\082-02-chat-ruling-m53-plan-decisions.md (route B provisional, D1-D7 ruled; m53 now waits on Codex's design review). This brief is unrelated read-only disk housekeeping.
# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: high
# NAME: 083 disk inventory IA build cache (Code, Opus 5.5)

# 083-01 — Disk inventory of E:\IA_BuildCache (and other GDP space users): what is it, what references it, what can go. READ-ONLY: delete nothing

You are Claude Code, running **headless** from the GDP mailbox. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

## How your turns end — standing instruction from the owner, the owner
A message with no tool call ends the whole run. Do not end a turn to report progress, to announce the next step, to offer options, or because a milestone is done or the run has been long — put status notes in the same message as your next tool call and keep going. End the run only when (a) every step below is done, (b) a stop condition below fires, or (c) something you need is deliberately protected from you or needs the owner. Every final message follows the structure at the end and starts with a `VERDICT:` line. Long scans outlive the tool timeout: run them in the background and poll with bounded PowerShell wait loops. If the Bash tool stalls on this box, use the PowerShell tool.

## Why
The owner reports that `E:\IA_BuildCache` is **over 350 GB** and he barely has space for anything else. Chat needs a precise, evidence-backed picture before proposing what to delete. **This brief only measures and classifies.** The deletion comes in a later brief, after the owner approves it.

## The work
1. **Measure.**
   - Free and total space on C:, D: and E:.
   - `E:\IA_BuildCache`: the total, every top-level folder, and the second level under anything over 5 GB, each with size, file count and oldest/newest modified dates.
   - The same for the other GDP space users you know of: the targets of the `_bench_sessions_bank` and `_binary_baselines` junctions, Lyra copies and bundles, `DerivedDataCache` locations (engine, project, shared), StackOBot `Intermediate`/`Saved`/`Binaries`, stray `_wt_*` or scratch worktrees, `C:\ClaudeTemp`, and cook outputs.
   - Use a fast method, such as .NET enumeration or `robocopy /L`, not a slow recursive `Get-ChildItem` over millions of files.
2. **Identify.** For each item, say:
   - what it is (from `CLAUDE.md`, journals, gotchas, the runbook, and 081-44's build notes);
   - which brief or milestone created it;
   - whether anything **references** it: ledgers, evidence JSON, boundary files, harness scripts, predictions, docs, the staged exe provenance or the live `m51` checkout;
   - whether it is **regenerable**, and at what cost in build or cook minutes.
3. **Classify:**
   - **A: safe to delete.** Regenerable scratch or intermediates, orphaned worktrees, leftover temp files, caches that rebuild. State the one-time cost (e.g. "the next build is a full rebuild, about 73 min").
   - **B: shrink.** Old bench sessions from **superseded** builds or invalid attempts, which could lose their PNG frames and keep labels, sidecars, logs and JSON. Or big folders that could be compressed.
     - Say exactly what would stop working, e.g. an oracle replay needs the PNGs.
     - Never propose shrinking sessions the **final accepted evidence** of a merged milestone depends on: the m55 c0–c3 requalification, the Stage 3 cost runs, the 081-43 replay, and the m52/m49 accepted gates. List those as C.
   - **C: keep.** Referenced by accepted evidence, the staged exe and its archives, anything the live checkout or the next m53 work needs.
   - **Unknown:** anything you can't place. List it, and don't guess.
4. **Propose, but do not execute:**
   - a GB total per class;
   - a deletion list and a shrink list as plain path files under `_reviews\083-01-evidence\`;
   - a **going-forward policy** so this doesn't grow back, e.g. scratch builds removed after archiving, a retention rule for bench sessions, a DDC location or size cap. Give the trade-off of each rule.

## Hard rules
- **READ-ONLY.** Delete, move, compress or rename nothing. Write only under `_reviews\083-01-evidence\`.
- Never kill anything that is not yours, and never by name.
- Don't touch `m51`, `master`, tags, cooked containers, CaptureBench or `ToCodex\`.
- Codenames only.

## Final message structure
1. `VERDICT: GREEN | NEEDS-DECISION | RED`. GREEN = everything measured, classified, and the lists written.
2. **Plain summary for the owner**, 4–6 short lines: where the space went and how much can safely come back.
3. Drive free/total table.
4. Size table: item, size, what it is, referenced by, regenerable/cost, class.
5. Totals per class, and the reclaimable estimate: A alone, then A+B.
6. What would break or slow down if A or B is removed.
7. The proposed going-forward policy.
8. NEEDS-DECISION items for the owner, or "none".
