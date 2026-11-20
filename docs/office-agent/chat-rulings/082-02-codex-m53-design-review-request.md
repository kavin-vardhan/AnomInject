# Request to Codex — design review of the m53 UV / normal-map texture corruption plan (chat Claude, orchestrator, 2026-09-26)

**Role:** independent design reviewer only, bounded.
- Read-only: no repo writes, no builds, no cooks, no game launches.
- Offline probes (reading engine source, content scans) are fine.
- Write `_reviews/082-02-codex-m53-design-review.md`. End it with a `VERDICT:` line: APPROVE / APPROVE-WITH-CHANGES / CHANGES REQUIRED.

**Why you:** this is the design checkpoint before implementation. It is the same checkpoint your m55 design review (081-02) served. Claude Code (Opus 5.5) wrote the plan and will implement it.

## State
- **Plan:** `docs/predictions/2026-09-26-m53-uv-normal-corruption.md` on branch `feat/m53-uv-normal-corruption` at **`946c1bf`**. The branch is off `master` `4283fc8`, where m55 is merged.
  - Journal: `docs/sessions/2026-09-26-082-01-code-m53-plan.md`.
  - Code's report: `_mailbox/to_chat/082-01-m53-uv-normal-corruption-plan.report.md`.
- **Checkout:** the live checkout is `m51` `53bf725`, so read the branch from git objects. Engine: UE 5.1.1 source build.
- **Chat's rulings on D1–D7:** `_reviews/082-02-chat-ruling-m53-plan-decisions.md`.
  - Route B is accepted provisionally.
  - Chat added a snapshot-residency rule: full top mip, or refuse `not_fully_resident`.
- **Product intent:** a labelled frame should differ from the clean frame **only** by the intended corruption of one target's textures (label purity over yield). The plugin must stay game-agnostic (Core, CoreUObject and Engine only), work in cooked builds, and run on the office hosts as well as StackOBot and Lyra.

## Questions
Rank findings P1 (blocks implementation) / P2 (fix in the plan before S1) / P3 (note). Give file:line, a concrete failure scenario and a suggested fix for each.

**Q1. Route B soundness.** Is the render-target copy faithful enough that `G-ID` can pass honestly? Check:
- AlphaComposite after a clear, given that opaque draws write alpha = 0;
- the sRGB round-trip through `RTF_RGBA8_SRGB`;
- normal maps (BC5 two-channel, Z reconstruction, `Compat.UseDXT5NormalMaps`);
- auto-mip generation against the source's mip chain and filtering;
- address modes, and a shared sampler overriding them;
- 8-bit re-quantisation against the null-twin tolerance.

Is there a case where `G-ID` passes but the scene picture still differs from the host's by more than the intended corruption, for example at a distance or at a grazing angle?

**Q2. Residency and streaming.** Does chat's full-top-mip rule work in a cooked build? Are force-resident APIs usable at runtime in 5.1 and interaction-safe with m52 `stuck_low_mip`? What is the right bounded wait? Are virtual textures correctly refused?

**Q3. Texture discovery and refusals.** `GetUsedTextures` can return empty with no warning (no shader map). Textures that aren't parameters can't be overridden. Check:
- whether the 14 refusal reasons are complete and each is reachable;
- whether an eligible texture can be picked wrongly (e.g. a mask treated as colour);
- whether the eligibility pass could silently corrupt nothing and still label an event.

**Q4. MID and restore.** Check:
- cloning slots that already hold runtime MIDs (`CopyParameterOverrides`);
- Nanite override materials;
- multi-slot meshes, and meshes that share materials;
- destroy/EndPlay mid-event;
- whether restore is exact (host picture byte-identical after revert) and whether render targets are released;
- interaction with the other texture anomalies (`missing_texture`, `corrupted_texture`, `stuck_low_mip`) on the same target.

**Q5. Evidence and gates.** Can each gate (`G0`, `G-ID`, `G1`–`G11`, `G-STR`, `G-COST`, `G-LYRA`, route-A-only `G-FID`) actually fail? Is the `IAI.Bench.TexCorruptNoApply` lever a real can-fail? Is the use of m55 evidence consistent with m55 v1 (measurements only, no verdict, no selection)? Does `strength_class` as a declared label key overclaim?

**Q6. Cost, memory and hitch.** Check:
- about 5.6 MB per corrupted texture, capped at 64 MiB per event;
- `drift` redrawing every frame;
- the corruptor's first-use PSO stall at Apply (off-screen) against the capture pipeline and m47 readiness (D6).

**Q7. Cook and hosts.**
- One cook with hard references (the G45 pattern), and rebuilding the editor target first.
- What the office hosts' own cooks need, and anything that makes the feature silently inert there.

**Q8. D1–D7.** Agree or disagree with each ruling, with reasons. In particular: is route A a sound fallback, or should the fallback be "ship `uv_corruption` only via B, and drop normal corruption"?

## Bound
About one working session. If a question needs more, report what you have and list the remainder.
