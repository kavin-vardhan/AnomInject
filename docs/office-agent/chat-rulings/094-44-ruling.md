# 094-44 — Chat ruling: the two 4.25 materials are accepted. FE3 stopped an extra readback launch on a build-tool child that exited too fast to read. The kit learns to catch children at creation (FE3b), keeps one re-run as a fallback, and 094-44r resumes from `9c87836`

**Date:** 2026-11-04, about 05:50 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Mailbox report 094-44:** NEEDS-DECISION. It ran 04:56–05:40 IST.
- `_reviews\094-44-codex-report.md`, `_reviews\094-44-code-review.md` and `_reviews\094-44-prepush-scan.md`.

## Rulings

### GG1. Accepted: the materials (step 2 is done)
- **The assets:** `M_MissingTexture_Checker` (`5222b3ee…`) and `M_CorruptedTexture_Pink` (`b88d8861…`) in `Content/Engine425/Materials/`.
- **The proof:** the authoring-session readback, plus Code's own check of the saved bytes, satisfies step 2 (Decision 2).
  - the six usage flags, and no Nanite flag;
  - two-sided, opaque and Default Lit;
  - the pink's base colour (1,0,1);
  - the checker matches `d7eee97` node for node;
  - Code's checker can fail.
- **No second readback launch is needed.**
- **The 5.1 `.uasset`s are unchanged.** `9c87836` stays local until its loader lines compile.

### GG2. FE3: catch build-tool children at creation (option b), with one re-run of a leg as the bounded fallback (option a). Never option (c)
- **What happened:** the editor's startup UnrealBuildTool child lived for under 267 ms, so the kit's first look found it gone and FE3 stopped the leg. The stop was correct under the rule as written.
- **FE3b, made by Codex first in 094-44r and committed with W4, with a self-test:**
  - The kit records each new child process of our editor or game, with its command line, **when the child is created.** Use a non-elevated, event-driven process-creation watch, or a tight poll that the self-test proves catches a child that lives 100 ms.
  - FE3's rule is otherwise unchanged. A UBT child whose command line is read and shows a build mode (not `-Mode=ValidatePlatforms`, `-Mode=QueryTargets` or another query mode) stops the leg. So does a child that is truly unreadable.
  - **If FE3b needs elevation or a machine setting, don't build it.** Say so, and keep (a) alone.
- **The fallback (a), standing:** a leg stopped only by an unreadable short-lived child, **with no binary change in its window,** may re-run once. A second such stop in the same leg is NEEDS-DECISION.
- **Never (c):** letting such a child through on "nothing built changed" alone would weaken the guard.
- **An FE3 owned stop is not a failed gate.** "Stop on any failed gate" applies to G1–G5, GF4 and the regression, not to a guard stop that may be re-run under (a).

### GG3. 094-44r resumes from `9c87836` with the same request and thread
- **Steps:** FE3b first, then step 2's loader lines (built and compiled), then step 3 (GF4) through step 10, exactly as in `_reviews\094-44-request.md`.
- **Push:** Code pushes only if every gate passes, or the passing earlier steps if they stand alone and compile.

## For the owner
- 🟢 **The pink and checkered materials are rebuilt for 4.25** and match your client's 5.1 version exactly.
- 🟡 **The job stopped on a safety guard, not a bug.** A tiny engine helper process started and finished too fast for our safety check to read, so the check stopped the launch, as it's designed to. I've ruled that the check catches these processes the moment they start, with one retry allowed. The anomalies resume now.
