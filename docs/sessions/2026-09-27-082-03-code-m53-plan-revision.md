# 082-03 — m53 plan revision 2 for Codex's 082-02 review, and the Codex delta check

**2026-09-27 · Code (Opus 5.5), headless · branch `feat/m53-uv-normal-corruption`, parent `946c1bf`, edited in
the scratch worktree `E:\IA_BuildCache\_082_03_m53_wt`.**
**Plan only.** No source change, no build, no cook, no editor or game launch, no bench leg, no tag. The
`m51` main checkout, `master`, the tags, the cooked containers, CaptureBench, `ToCodex\` and `E:\AmmaYT` were
not touched.

## Goal

Brief 082-03:

1. Revise the m53 plan for Codex's 082-02 design review (CHANGES REQUIRED: 5 P1, 6 P2, 1 P3) and chat's
   disposition `_reviews/082-03-chat-ruling-codex-m53-design-review.md` (every finding accepted; D1's
   residency and fallback revised, D7 reversed, D2 and D6 amended).
2. Run Codex's delta check through the GDP Relay (its first real use), and do not act on it.

## What was done

- Read the ruling, the review, the delta-check request, the Relay README and runner, and the v1 plan and
  journal.
- Four read-only source verifications, run in parallel:
  - active parameter bindings in a cooked build (the uniform-expression set, parameter vs constant,
    `…ByInfo`, the default-material fallback);
  - which texture settings a cooked build can read, how texture and render-target samplers are built, and
    how render targets generate mips;
  - residency state, material-slot accessors and Nanite substitution, `CopyParameterOverrides`, and
    render-command ordering;
  - the plugin's prewarm and run phases, fixture gates, bench levers and fire-path exclusions.
- A second offline content scan, reading `.uasset` tagged properties: imported dimensions, `LODBias`,
  `MaxTextureSize`, `MipGenSettings` and alpha-coverage flags (plan Appendix C). The scripts are outside
  the repo at `C:\ClaudeTemp\m53scan\`.
- Rewrote the plan as revision 2 (§R0–§R16, Appendix C). Revision 1 is kept verbatim under a SUPERSEDED
  fold. §R0 maps every finding to its section and line range.

## What the revision changes

- **Route B only.** No route-A fallback anywhere; a failure narrows scope or holds, and any narrowing
  goes to chat.
- **Bindings** are read from the compiled uniform-expression set of the resource the slot renders with,
  so parameter vs constant is known per entry. The alias case resolves by construction.
- **Encodings:** an allowlist on the pixel format. Float, 16-bit, BC6H and LQ are refused.
- **No downsampling.** The render target takes the full cooked top-mip dimensions; over budget refuses.
- **Sampler matched** by construction (LOD group, filter, address); a host `r.MipMapLODBias` is refused
  because a render target cannot follow it.
- **Residency:** already resident at the full cooked chain, or refuse, with 0 wait.
- **Host MIDs refused;** no clone path is built.
- **One ordered decision tree** with exactly one final reason, and atomic per-slot map sets.
- **Every draw clears first,** `drift` included.
- **A defined Apply transaction** with rollback, and a non-capturing warm-draw phase before the lead-in.
- **The gate table** marks each row qualification, diagnostic or owner decision, with matched controls,
  wrong-copy controls that must fail, and minimum counts.
- **`expected_strength_class`** is a documented mode prior.

## Findings of this session

- 🚨 **Authored mip chains cannot be refused, because they cannot be detected.** `MipGenSettings`,
  `bDoScaleMipsForAlphaCoverage` and `MaxTextureSize` are editor-only data, and within Engine a render
  target can only regenerate its mips. The content has such chains: 8 of 131 StackOBot textures
  (`T_Eyes_Atlas` on the Bot) and 22 of 324 Lyra textures, including every Manny and Quinn map. ⇒ **N1**,
  a counter-proposal to P1-2. Recommended: declared RenderCore + RHI and a per-mip draw.
- 🚨 **Under the stricter rules no StackOBot host target exercises the colour or normal encodings.**
  - The rock textures are 4096²: one map's full chain is 85.33 MiB, above the 64 MiB budget.
  - m52 measured those textures at resident 11–12 of 13 mips at the bench poses, so they also fail
    residency.
  - The Bot is a host MID.
  - Only `SM_FloorBase` (a 1024² mask, UV family) is predicted to fire.
  - ⇒ **N2**, a bench-only synthetic fixture level. It is also the only source of the alias, layer and
    reason fixtures Codex asked for.
- **The render-target sampler has no mip bias**, while ordinary textures follow `r.MipMapLODBias`, and the
  engine's sampler refresh skips render targets. A non-zero host bias would therefore break identity, so
  it is refused. It also means minification cannot be forced with that variable in a gate.
- **The m47 prewarm runs immediately before a captured lead-in frame.** A warm draw therefore needs its
  own non-capturing phase.
- **No public per-texture m52 held/restoring query exists today.** The plan adds one covering both lists.
- **`corrupted_texture` has no fixture gate**, so it serves as the matched positive control on MainWorld.
  m55's `solid_swap` cannot fire there.

## Deviations

None from the brief's hard rules. Two items go back to chat as counter-proposals or decisions instead of
being resolved as specified: the authored-mip refusal (N1), and the fixtures that host content cannot
supply (N2). N3 (the budget value) and N4 (`G-ID`'s tolerance) are decisions the revision needs but the
ruling did not set.

## State

Revision 2 committed and pushed on `feat/m53-uv-normal-corruption`. Codex's delta check ran through the
Relay; its report is `_reviews/082-03-codex-m53-delta-check.md`. Code did not act on it.

## Hand-off

Chat reviews the revision and the delta check together. S1 is blocked on N1 and N2.
