# 094-42r — Chat ruling: the guard, the routes and the regression smoke are accepted. The fixture stopped on a missing -nowrite, so its gates and the spike's cost read continue in 094-42c, after the owner's renders

**Date:** 2026-11-03, about 12:15 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Mailbox report 094-42r:** NEEDS-DECISION. It ran 11:05–12:05 IST.
- `_reviews\094-42r-codex-report.md` and `_reviews\094-42r-code-review.md`.

## Rulings

### GA1. Accepted: FZ3 and FZ4, `d1cdc7b..f7b32f0` (8 local commits, unpushed)
- **FZ3:** planting is fixture-only, and admission runs before any tag.
- **FZ4:** routes default to blinking + lod_corruption, explicit lists are validated, and the routes are written to the receipt.
- **The EA_Core regression smoke passed every gate,** with the scanner reading `none`.
- **The scan-once-at-start limit** is accepted for now. A late-added reader is listed for the FZ2 work.

### GA2. The fixture's own `Saved\Config\Windows\Engine.ini` (1,502 B) is not a Paris-host change
- It stays as preserved evidence.
- From now on every fixture launch carries `-nowrite`.
- *Why:* FB2 protects the Paris host. The fixture is our own throwaway project, and `-nowrite` blocks only config writes, so the authoring script can still save its assets.

### GA3. FZ6(a) is accepted by source; the depth-only route is preferred
- The engine's existing depth-only shaders can drive the private pass with no new shader type.
- FZ6(b) still runs, to size the fallback and to settle the `StaticType` link risk with a real compile.

### GA4. 094-42c runs the remaining gates, with a fresh FM1 budget for the fixture's Editor and Game and for the spike's throwaway branch
1. authoring, with `-nowrite`;
2. FZ5's repro gate (outlines shown by pixel difference);
3. FZ5's guard gate (the refusal before any tag);
4. FZ6(b) (the recompile cost against a baseline).

The spike branch is never merged or pushed.

### GA5. Admission waits get a bound
- If any kit admission gate holds for **3 h**, the job stops with NEEDS-DECISION and its partial results, rather than waiting without limit.
- *Why:* 094-42 sat about 10 h behind foreign load.

### GA6. Scheduling
- The owner renders first. The PC is free now, and only the read-only delete check (094-42d) runs alongside.
- 094-42c is staged when the owner says the renders are done.
