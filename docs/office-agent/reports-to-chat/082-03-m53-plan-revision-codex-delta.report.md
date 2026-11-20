# REPORT for 082-03-m53-plan-revision-codex-delta.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 082-03-m53-plan-revision-codex-delta.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-27 00:20:54 | finished: 2026-09-27 01:04:04 | duration: 43 min | exit code: 0
context at last turn: 635067 tokens (64% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\082-03-m53-plan-revision-codex-delta.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN. The revision is pushed, every finding is resolved or has a counter-proposal, and the Codex delta check is collected. Codex's own verdict on the revision is **CHANGES REQUIRED**, including one new P1 defect that my revision introduced.

**Plain summary for the owner**
- The new anomaly keeps the game's own material and only corrupts copies of textures it already uses. The backup plan of swapping in our own material is gone.
- It is much stricter now. It refuses any texture not fully loaded, refuses anything too big for the memory budget instead of shrinking it, refuses materials the game created at runtime, and only corrupts a material if all of its textures can be moved together.
- The cost is yield: on StackOBot probably only one target (the floor) still qualifies, and only for UV corruption. The rocks are 4096² and not fully loaded at the bench camera, and the Bot is refused. Lyra keeps the gym cubes (both families) and the weapons (normal only).
- Two things could not be done as ruled, so chat must decide. The game's hand-tuned lower mips can't be detected in a packaged build, so they can't be refused (N1). The real levels can't test the colour and normal paths at all, so a small bench-only test level is needed (N2).
- Codex's check (9 minutes): 6 findings fully fixed and 6 partly. It found three new problems in the revision. The biggest is that a runtime material stored on the mesh itself, rather than on the component, gets past the refusal.

**Resolution table (condensed; line numbers are in the plan at `b118a66`)**

| finding | resolution |
|---|---|
| P1-1 | Bindings come from the compiled shader map's texture list for the world's feature level and active quality, so parameter vs constant is known per entry. Constants are refused. Overrides use the full parameter info (name, association, index). Codex's alias case resolves by construction. |
| P1-2 | **Counter-proposal N1.** Pixel-format allowlist; float, 16-bit, BC6H and LQ refused. Full dimensions, no downsampling. Sampler matched; `r.MipMapLODBias ≠ 0` refused. Authored or sharpened mip chains are editor-only settings, and inside Engine a render target can only regenerate its mips. Found on 8/131 StackOBot textures and 22/324 Lyra textures, including every Manny/Quinn map. |
| P1-3 | Already fully resident (`NumResidentLODs == MaxNumLODs`) or refuse, with 0 wait. No force-resident call and no `WaitForStreaming`; `snapshot_mip` is recorded. |
| P1-4 | Slots holding a runtime material instance are refused. No clone path is built; `host_mid_cloned` is reserved. |
| P1-5 | Route A removed. A failure defers the normal family or stops; any narrowing goes to chat. |
| P2-6 | Every draw clears to (0,0,0,1), then draws, then regenerates mips. New gate G-RD, with a lever that skips the clear and must fail. |
| P2-7 | Each slot's raw, asset, resolved and effective material is captured. Nanite is inspected from the resolved material. Originals are held strongly. A slot is restored only while it still holds our material. |
| P2-8 | A slot's whole map set is transformed or the slot is refused. The cap refuses instead of trimming. The 64 px floor becomes a named policy (`below_size_policy`). |
| P2-9 | One ordered decision tree (event → slot → texture → aggregation) with exactly one final reason. Every reason has a named fixture, or is marked UNEXERCISED. |
| P2-10 | Every gate is marked qualification, diagnostic or owner decision. Matched controls: NoApply as the null, `corrupted_texture` as the positive, and wrong-copy levers that must fail. Minimum counts; G-LYRA needs one successful event per id. |
| P2-11 | The prewarm list is split from the per-frame check. A defined Apply transaction with rollback. A new non-capturing warm-draw phase before the lead-in. Cold-first-fire cost is measured, and accepting it is the owner's call. |
| P3-12 | Renamed `expected_strength_class`, documented as a mode prior. m55 stays out of runtime decisions. |

**Revised stages**
- **S1** (route B core, identity only, the test level if N2 is approved): 2 builds and 1 cook. Blocked on N1 and N2.
- **S2** (all modes, both ids, drift only if G-RD passes): 2 builds, 0 cooks.
- **S3** (yield, strength, cost, Lyra): 0–1 bench builds plus a Lyra build.
- **S4** (docs and merge): 1 build pair.
- Totals: about 8 bench builds, 1 cook, 5 sessions. v1 said 4 sessions.

**Yield impact at 64 MiB with the stricter refusals (predicted, not measured)**
- **StackOBot:** only `SM_FloorBase` (UV, data class) is expected to fire.
  - The rocks fail residency: m52 measured them at 11–12 of 13 mips at the bench poses. Even if resident, one 4096² map is 85.33 MiB, over the budget.
  - The Bot is refused as a runtime material. The Nanite modular kit is over budget.
- **Lyra:** cube UV fits at about 48 MiB, but goes over (69 MiB) if the opacity map is active. Cube normal fits. Weapons get normal only.
- **Drift** is over budget everywhere except the floor.

**Codex delta check** (`gpt-6-astra`, effort max, 556 s, exit 0): `VERDICT: CHANGES REQUIRED`
- **RESOLVED:** P1-1, P1-5, P2-6, P2-7, P2-11, P3-12.
- **PARTIAL:**
  - P1-2: N1 is not yet operative.
  - P1-3: admitting `snapshot_mip > 0` under a runtime bias is ambiguous.
  - P1-4: see Δ1.
  - P2-8: an excluded LOD-group map inside a selected slot can stay fixed while the others move.
  - P2-9: a virtual entry fails the `Standard2D` step first, so it reports `unsupported_type` instead of `virtual_texture`.
  - P2-10: Δ2, Δ3, and the test matrix doesn't list every admitted linear format.
- **NOT RESOLVED:** none.
- **New findings:**
  - **Δ1 (P1)** — the runtime-material check reads only the component override. A runtime material in the mesh asset's slot passes and would be used as a parent.
  - **Δ2 (P2)** — the sRGB wrong-copy lever may not actually be wrong, so a valid instrument could be declared invalid.
  - **Δ3 (P2)** — G-BIND needs a non-identity mode inside identity-only S1, and G-RD's criterion would reject correct drift.
- Codex also says S1 needs N4 ruled, and every S1 qualification failure listed as a stop. I have not acted on any of this.

**Commits pushed:** `b118a66` on `feat/m53-uv-normal-corruption` (`946c1bf..b118a66`, origin == local). It contains the revised plan with v1 verbatim under a SUPERSEDED fold, the journal `docs/sessions/2026-09-27-082-03-code-m53-plan-revision.md`, and the branch status block. No tag. `m51` is still at `53bf725`, `master` == `origin/master` == `b5f15a3`, and the scratch worktree is removed.

**Relay outputs:** the report is `_reviews\082-03-codex-m53-delta-check.md` (one header line, then the body byte-identical to `review.md`). The ledger has a new row.

**NEEDS-DECISION** (plan §R15):
- **N1** — how to handle authored mip chains: (a) declared RenderCore and RHI dependencies plus a per-mip draw, recommended; (b) stay inside Engine and accept a class we can't detect; or (c) hold m53. Blocks S1.
- **N2** — a bench-only synthetic test level authored by a CaptureBench tool. It's the only way to get the alias, layer, reason and encoding fixtures, and it touches CaptureBench. Blocks S1.
- **N3** — the render-target budget. 64 MiB refuses every 4096² source; the plan has the numbers for 64, 128 and 256 MiB.
- **N4** — G-ID's tolerance: 2 levels per 8-bit channel with an 8× separation from the wrong-copy controls, or exact match only.
- Plus Codex's Δ1–Δ3 and the six PARTIALs, for chat to rule on alongside these.

Note: several connectors (Gmail, Google Calendar, some plugin servers) need authorising in claude.ai settings or `/mcp`, and the pdf-viewer server failed to connect. None was needed for this brief.
api-equivalent cost (not billed on Max): $27.48 | turns: 79 | running total across all briefs: $2,492.94
