# 094-17 — Chat ruling: the A6 kit is ready, and the owner leg is GO

**Date:** 2026-10-26, about 08:10 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Codex's report:** `_reviews\094-17-codex-report.md` (05:20–06:56 IST, 5,740 s, exit 0). Its verdict: `GREEN | A6 kit corrected and re-armed for 09417; dry run passed; real slots unused; zero Unreal launches`.
- **Claude Code's review:** `_reviews\094-17-code-review.md`. 35 OK · 8 N/A · 1 GAP · 7 OPEN · 0 stop violations.

## Verdict
**ACCEPTED.**
- **G1 and G2 are fixed, and Code proved both independently:**
  - 34/34 error injections through the real wait loop;
  - 11/11 harmless-child launch-lock cases, including Code's own 094-16 reproduction;
  - Codex's 200 live readings: 5 contained GPU errors and 0 uncaught exceptions. Code's own 120 readings: 0 errors.
- **The kit is re-armed for job 09417.** Its three real slots are unused, and the 09416 arming is refused as superseded.
- **Both dry runs, Codex's and Code's, passed with 0 launches.**
- **Unchanged:** the bound files and manifests (27/39/44) and P9 v2 (18,264 of 18,264).
- **The canary:** 0 hits in 4,627 files and 97 child environments.

## Rulings

### ER1. G3 is accepted for the owner leg
**What it is:** every refusal now shows the generic `Stopped: unexpected error; details saved to <file>.` line on screen, and the specific reason appears only in the details file. It is fail-safe: no refusal starts an editor.

**For the owner leg, the card covers it:**
- on any `Stopped:` line, the owner takes a screenshot and tells chat before trying again;
- chat reads the details file and says whether to retry.

**Fix it in the next tooling job, before any measured leg.** Each refusal shows its own reason, and that job re-arms whatever it changes.

### ER2. The raw ledger STOP in operation `03-documentation` is foreign (EB1)
- **What was flagged:** six linker temp files under `C:\ClaudeTemp\UnrealBuildTool\044a3114`, removed by the DCDemo UE 5.7 build.
- **The proof:** that build's own native UBT log, which Code checked independently.
- **The result:** this is not one of EN5's hard STOPs. The raw STOP stays in the ledger, with its disposition.

### ER3. Carried OPENs
**Before any measured leg:**
- a bare `UnrealBuildTool.exe` (UE4's UBT) must hold the measured gate. This gap predates this job;
- G3's fix (ER1);
- tests for 8.3 short names and junction aliases in the dry-run path refusal.

**For 094-18:** expect `Receipts\09417\refused-attempts\` if a refusal happens before a slot is claimed.

**The Epic launcher's `UnrealCEFSubProcess.exe` holds the owner gate.** The card says to close the launcher.

**Accepted, fail-safe:** an internal failure after the editor starts closes the editor and locks the kit.

**Carried unchanged:** EO1's reader budget, the 5.1 probe path (A9), and EJ1, EJ3 and EJ5.

### ER4. The owner leg is GO
**The owner runs the kit with chat's card,** `_reviews\A6-owner-card.md`.

**When the owner says "done",** chat stages 094-18: the EP3 evidence and content-admission check.
