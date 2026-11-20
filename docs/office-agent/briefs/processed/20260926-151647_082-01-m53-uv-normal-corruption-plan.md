# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 082 m53 UV/normal corruption plan (Code, Opus 5.5)

# 082-01 — m53 UV / normal-map texture corruption: PLAN ONLY (source read, design, predictions); no code, no build, no bench

You are Claude Code, running **headless** from the GDP mailbox. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

## How your turns end — standing instruction from the owner, the owner
A message with no tool call ends the whole run. Do not end a turn to report progress, to announce the next step, to offer options, or because a milestone is done or the run has been long — put status notes in the same message as your next tool call and keep going. End the run only when (a) every step below is done, (b) a stop condition below fires, or (c) something you need is deliberately protected from you or needs the owner. Every final message follows the structure at the end and starts with a `VERDICT:` line. If the Bash tool stalls on this box, use the PowerShell tool.

## Context
m55 (measured change evidence) is merged: `master` `4283fc8`, staged bench exe `E0BE6F0A`. The owner reordered the queue on 2026-09-26: **m53 = UV / normal-map texture corruption comes first**, then m54 = flicker. This is a high-priority client anomaly. Today texture corruption covers only `missing_texture` (checker) and `corrupted_texture` (pink).

**Shape agreed with the owner on 2026-09-22.** Treat it as the starting design, and challenge it with evidence where the source says otherwise:
- **Reuse the takeover.** Build on the `corrupted_texture` material takeover, using two plugin materials, `M_CorruptedUV` and `M_CorruptedNormal`.
- **Pull the host's own textures.** Take them from the target's material with `GetUsedTextures`: the base colour is the sRGB texture, the normal map is the one with compression `TC_Normalmap`. Create one dynamic material instance per target.
- **UV modes:** tile, drift, U↔V swap, lerp to TexCoord[1].
- **Normal modes:** green-channel flip, invert, drop (flat), noise.
- **Label keys:** modes are written as additive keys. No schema break.
- **One cook for both materials.**
- **Targets:** opaque, single-sided targets only; refuse everything else.
- **Rejected:** mesh-level UV rewrite, because render data is shared and CPU vertex data is stripped.

**Invariants.**
- The plugin stays game-agnostic: Core, CoreUObject and Engine only, no host types, and targets are matched by name or class.
- Built and validated on UE 5.1.
- It must work on the office hosts (Concorde, Bates) as well as StackOBot and Lyra.

## Read first
1. **The current texture anomalies.**
   - `Source/AnomalyInjector/Private/Anomalies/Anomaly_CorruptedTexture.cpp` (+ `missing_texture`, and the pink material asset and how it is cooked).
   - The m29/G50 lesson: unlit emissive glows onto neighbours.
   - The m47 shader-readiness path, including G298's packaged-build false positive.
   - The m52 `stuck_low_mip` plan and gates, for texture-streaming interplay.
2. **m55's change evidence.** `docs/change-evidence-fields.md` and `docs/client-readme.md` §9. m53's visibility evidence should be built on it.
3. **The viewport-visibility layer, auto-injection pool, catalogue and label schema docs.** Also `docs/architecture.md`, `docs/gotchas.md` (texture, material, cook, Nanite and PSO entries) and `docs/PRE-DELIVERY-CHECKLIST.md`.
4. **Earlier plans, for format.** `docs/predictions/2026-09-20-m55-change-evidence.md`, and the m52 plan.

## The plan must answer (with engine source citations, UE 5.1)
1. **Texture discovery.**
   - How reliable `GetUsedTextures` is in a **cooked** build for material instances, parameter overrides, layered materials, virtual textures and streaming (mip residency).
   - How to pick base vs normal robustly: sRGB flag, compression setting, name heuristics as a fallback only.
   - What happens with 0, 1 or several candidates; the refusal reasons.
2. **Material design.**
   - The graph for `M_CorruptedUV` and `M_CorruptedNormal`: parameters, how each mode is expressed, and how lighting stays plausible, so it isn't glowing or flat unless that is the mode.
   - **Usage flags**: static, skeletal, instanced, Nanite, and whether each is needed or refused. Cooked permutations.
   - The **first-use shader and PSO hitch** in a packaged build, and how m47 readiness interacts with it.
   - What must be cooked, and the implications for the office hosts' builds.
3. **Takeover and restore.** Slot selection and multi-material meshes, MID lifetime, exact restore, destroy/revert, and sharing: can one MID serve targets that share a material?
4. **Visibility evidence.** Using m55, the per-target changed-pixel count at onset against the control.
   - Which modes are expected to be visually strong, which are weak (e.g. tiny drift, green flip under flat lighting), and how the plan handles weak cases. That handling could be a floor, a refusal, or a label caveat; choose and justify.
   - No verdict word: m55 v1 is measurements only.
5. **Gates and fixtures.** Declared predictions for StackOBot (MainWorld and/or CB_GateLevel) and Lyra.
   - Which targets qualify on each: opaque, single-sided, with base and/or normal textures.
   - Null and positive twins, if needed.
   - Both tick orders where relevant.
   - Restore identity (pixels after revert match before).
   - Yield.
6. **Auto-injection.** Pool inclusion and default ON/OFF. That is the owner's product call, so give a recommendation plus the evidence he would need.
7. **Cost and risk.** Expected cost, the risks, and anything in the agreed shape the source contradicts.
8. **Implementation stages and briefs.** A file-by-file plan, a stage split with stop points, and a cook/build count estimate.

## Hard rules
**PLAN ONLY.**
- No source change, no build, no cook, no game launch, no bench.
- Write the plan as `docs/predictions/2026-09-26-m53-uv-normal-corruption.md` on a new branch `feat/m53-uv-normal-corruption` off `master` `4283fc8`, using a scratch worktree. Add a short journal. Commit and push. No tag.
- Never kill anything that is not yours, and never by name.
- Never touch `m51`, `master` (other than branching from it), tags, cooked containers, CaptureBench or `ToCodex\`.
- Codenames only.

## Final message structure
1. `VERDICT: GREEN | NEEDS-DECISION | RED`. GREEN = the plan is written, pushed and answers 1–8.
2. **Plain summary for the owner**, 4–6 short lines: what the new anomaly will look like, and what's risky.
3. The answers to 1–8, condensed, with source citations.
4. Where the source contradicts the agreed shape, if anywhere.
5. The owner's product calls, with your recommendation.
6. Stage and brief plan, with estimates.
7. Commits pushed.
8. NEEDS-DECISION items, or "none".
