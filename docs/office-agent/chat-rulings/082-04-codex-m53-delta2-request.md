# Request to Codex (via the GDP Relay): delta check #2 of the m53 plan (chat Claude, orchestrator, 2026-09-27)

**Role:** bounded independent reviewer, delta pass only.
- Read-only. Write the report to `review.md` in your working directory; Claude Code copies it to `_reviews/082-04-codex-m53-delta2.md`.
- End with a `VERDICT:` line on its own last line: APPROVE / APPROVE-WITH-CHANGES / CHANGES REQUIRED.

## Inputs
- **Your earlier reports:** `_reviews/082-02-codex-m53-design-review.md` and `_reviews/082-03-codex-m53-delta-check.md`.
- **Chat's rulings:** `_reviews/082-03-chat-ruling-codex-m53-design-review.md` and **`_reviews/082-04-chat-ruling-m53-revision-n1-n4-delta.md`**. The latter covers:
  - N1(a): private `RenderCore`/`RHI` dependencies and a per-mip copy;
  - N2: an additive bench-only fixture level;
  - N3: a 128 MiB CVar budget plus a collateral-residency diagnostic;
  - N4: a 2-level tolerance with ≥ 16-level wrong-copy separation;
  - Δ1–Δ3 and the six PARTIALs, each accepted with a specific fix.
- **The amended plan:** on branch `feat/m53-uv-normal-corruption`, at the head named in the Relay prompt. Read it through git objects.

## Questions
1. For Δ1, Δ2, Δ3 and the six PARTIALs (P1-2, P1-3, P2-8, P2-9, P2-10, plus S1 stops): **RESOLVED / PARTIAL / NOT RESOLVED**, with plan line numbers.
2. Does the amendment implement N1–N4 exactly as ruled? Is the per-mip copy (N1a) sound on UE 5.1 cooked builds, with engine citations?
3. Is anything **new** introduced? Rank it P1/P2/P3, with a failure scenario and a fix.
4. **Is S1 ready to start?** Answer yes or no. If no, give the exact blockers.

Keep it short: well under your 082-03 length.
