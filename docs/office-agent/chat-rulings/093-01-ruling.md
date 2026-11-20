# 093-01 — Chat ruling: the natural-cause feasibility study is ACCEPTED; pilot plan and design rules

**Date:** 2026-10-13, about 14:45 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- `_reviews\093-01-codex-natural-causes-feasibility.md`: Codex `gpt-6-astra`/`max`, 3,981 s.
- `_reviews\093-01-code-verification.md`: Claude Code checked 53 claims (46 confirmed, 4 corrected, 3 unverifiable).
- Mailbox report 093-01: GREEN, 76 min, $17.75 API-equivalent.
- The design draft `_reviews\drafts\093-00-natural-causes-design.DRAFT.md`, r2.

## Verdict
**ACCEPTED as research.**
- Both documents are thorough. Where Code corrected Codex, Code's corrections are upheld (R9).
- This ruling **supersedes the draft** wherever they conflict.
- Nothing here changes the frozen release, `release/2026-10-05` @ `d7eee97`.

## Rulings

### R1. Pilot order (chat owns it, at the client's request)
1. **Foundation first** (R4, R7, R8): the natural-attempt lifecycle, the probes and the additive label keys.
2. **MISS-1** ships alongside the foundation as a **controlled R1 calibration**: the engine's own default material, swapped in by us. It is never described as a shader failure.
3. **MISS-2** (natural): the engine's `DefaultTexture` bound to a colour parameter of the host's own material, through a MID on the m53 route.
4. **BLUR-1** (natural): the release hold lever on the **shared** texture itself, with every visible user labelled (R6).
5. **GONE-1** (natural): the draw distance set too short. **Gated on the reference-mask proof (R5).**
6. **NRM-3** follows MISS-2. It uses `/Engine/EngineMaterials/FlatNormal` with an explicit plugin cook reference, not `DefaultNormal`.

Deferred or dropped:
- **BLUR-2** is research only and deferred: the streamer protects visible textures first, so it cannot be aimed.
- **MISS-3** is dropped: a null MID texture parameter is a no-op.
- Motion-dependent entries wait for automatic movement (the M4 idea).

### R2. Modes, type ids and the existing anomalies
- **`injection_mode`:**
  - `natural` means the event came through the natural-attempt lifecycle (R7): cause first, effect measured, no-effect attempts not counted, collateral labelled.
  - `controlled` means everything else, including every shipped anomaly and MISS-1.
- **Anomaly type ids do not change.** A natural missing texture is still `missing_texture`; the mode is a separate field, so the bug class the model learns stays the same.
  - The plan proposes how the owner or client picks controlled vs natural per type (console and dashboard). The UI shape is the owner's product call.
- **Shipped anomalies stay `controlled`.** Where the shipped mechanism really is a real cause, they get optional cause metadata only:

  | Shipped anomaly | Cause id | Cause |
  |---|---|---|
  | `lod_corruption` | LOD-1 | content: forced LOD left on |
  | `normal_corruption` green_flip | NRM-1 | content: normal map in the wrong convention |
  | `camera_clipping` | CAM-1 | config: near plane set too large |
  | `blinking`, **mode 0 only** | GONE-3 | code: visibility toggled wrongly |

- No other shipped anomaly gets a cause id.
- `time_dilation` and `lighting_mismatch` stay out: they are registered in the source but not qualified in the client's menu.

### R3. Module policy (resolves the "three-module" conflict)
The draft's "Core/CoreUObject/Engine only" was stale: the release already depends on more. The rule from now on:
- **Forbidden:**
  - any host-game module;
  - any engine source edit;
  - any private-header use beyond what the capture module already uses.
- **Allowed:** the engine modules already listed in the release's `Build.cs` files (RenderCore, RHI, Renderer with its existing private-header use, AnomalyShaders, InputCore, Foliage, and UnrealEd on editor-only paths).
- **Any new module dependency** is named in the plan with its reason; chat approves it.

Natural-cause levers should stay Engine-level (component, material and cvar setters). Measurement reuses the capture module.

### R4. Evidence contract (resolves "measured vs exact renderer telemetry")
**No engine instrumentation.** A natural effect event is valid only with all three of these:
1. **Mechanism state**, read back on the render thread and keyed to the capture request:
   - MISS: the material and uniform texture;
   - BLUR: resident mips against the pre-arm render-thread baseline;
   - GONE: the effective cull values, with the proxy recreated.
2. **A measured mask**: nonzero, measured, on the same frame key.
3. **A measured visible change** in that footprint against clean-noise controls (the m55 machinery, extended to an attempt stage).

This is the evidence standard the shipped anomalies already meet. An exact per-draw receipt is not required, and we never claim one.

### R5. GONE-1 reference-mask contract (Codex's Q9 route, with Code's corrections)

**Scope v1:**
- an ordinary non-Nanite `UStaticMeshComponent`, opaque or masked;
- no LOD parent or HLOD, and not `bNeverDistanceCull`;
- no host use of custom depth on the original.

**The companion:**
- A plugin-owned transient component sharing the original's mesh and materials.
- Main pass, depth, shadows, GI/DF, ray tracing, decals and collision all off; custom depth only.
- No distance cull. `bIgnoreInstanceForTextureStreaming` is set before registration.

**Equivalence, proven per target before arming, while the original still draws:**
- The companion's silhouette matches the original's measured mask in the owner view, at or above a declared agreement threshold. The plan proposes and justifies the threshold.
- The captured colour is unchanged with vs without the companion, within clean noise.
- A target that fails is **not eligible**: its label/mask can't be measured (rule 1 of the four).

**During the effect:**
- The companion's RT0 support maps to `target_pixels`, which keeps the shipped "would-be silhouette" meaning.
- RT1 is never filled from the companion.
- New additive key: `mask_measurement.method = "equivalent_reference_surface"`.
- Zero companion support is a **measurement failure, not a disappearance**. m26 stays on.

**Aiming:**
- Use the distance from the owner view to the **bounds origin**.
- Scale = `ViewDistanceScale × FOV factor`.
- Use a positive cull value below the scaled threshold.
- The 10 m fade margin applies **only** to `DistanceCullFade` materials.

**Restore:** order RD: changed flags → `SetCullDistance(savedLD)` → `SetCachedMaxDrawDistance(savedCached)`, then verify the proxy. Observe the object's return through the same companion before destroying it.

**Can-fail:** a deliberately offset or scaled companion must fail the equivalence check.

### R6. BLUR-1 restore: the scoped `StreamIn` repair is allowed
- **Why this is allowed:** A4 forbids forcing host residency or changing host quality to *pass a control or manufacture eligibility*. Putting back the host's own pre-arm state after our cause is a **revert** (rule 4, revert residue), not a quality change.
- **Bounds on the repair:**
  - only on the textures this cause changed;
  - only up to the pre-arm baseline, never above it;
  - only inside the owned restore window, with bounded retries;
  - released when recovery is observed or the window expires;
  - never a persistent force-resident flag, never a global setting.
- **Labels:** recovery is measured per affected user, and labels end when the picture recovers, not when the knob is reset.
- **Every visible user is labelled.** Any per-event user cap is a declared measurement or resource bound (masks), stated in the plan. It is never a purity preference.

### R7. Attempt outcomes (three-way, label-safe)

| Outcome | What happens |
|---|---|
| **Effect observed** on the aimed target | Counted. One event for the target and one for every visibly affected collateral object |
| **No effect**: change within the clean-noise bound in the footprint, within the budget | Reverted, not counted, diagnostics only |
| **Ambiguous** (above noise, below the visible threshold) or **unknown** (missing receipt) | Reverted. The frames are flagged (excluded or transition), **never silently labelled clean**. Diagnostics |
| **Missed aim, visible collateral** | The collateral is labelled truthfully and never erased. The attempt counts as a missed aim in diagnostics |

Decision budgets are experiment settings to measure on the bench, not accepted thresholds. Examples from the review: 4 paired samples for direct swaps; 30 samples or 2 s for streaming.

### R8. Label keys v1 (additive; Codex Q8, with a smaller first set)
- **Keys:**
  - `natural_metadata_version`;
  - `injection_mode`;
  - `cause_id`, `cause_family` and `cause_event_id`;
  - a root `natural_causes[]`, only for causes that emitted effects;
  - `effect_observation`;
  - a typed `severity`;
  - `mask_measurement.method`;
  - `cause_*` counts in `run_summary`.
- `effect_mask`, for footprints other than the object such as missing shadows, is deferred until LIGHT-5.
- **No existing key changes meaning.** `target_drawn_pixels` never takes companion pixels.
- Cause metadata is optional and never gates anything (the client: "good to have").
- The client is told about the new keys before the first natural build ships. That is an owner action at that time.

### R9. Corrections to the draft (upheld; the plan uses these)
- **PIE and missing usage flags.** While PIE ticks or draws, the engine takes the in-game branch: the default material, no repair. Repair, and dirtying the package, happens only outside PIE scope (verification row 3).
- **Flat normal.** It is `/Engine/EngineMaterials/FlatNormal`. That is not a startup package, so it needs an explicit cook reference.
- **GONE-1 aiming.** The scale includes the FOV factor, and the fade margin applies only to `DistanceCullFade` materials.
- **`stuck_low_mip` shared route.** It is a private **reduced-mip copy**: R0 as a streaming look, or R2 only as "wrong-resolution replacement asset". It is not "cached bias on a private copy".
- **`missing_texture` / `corrupted_texture`.** Both are R0. UE 5.1 *does* use hot pink for VT decode errors, so "never Unreal's colour" was wrong.
- **`time_dilation` / `lighting_mismatch`.** Both are registered in the release source. "Unreleased" means not in the client's supported menu.
- **TIME-1.** A one-frame delta spike is a different mechanism, not a faithful hitch under fixed-step capture.
- **Custom-depth tagging.** Plugin tagging makes the original "definitely unoccluded". Never tag the original for GONE-2.

### R10. Parked research (not scheduled)
- **MISS-1b:** a plugin-owned material deliberately lacking a usage flag. In PIE and cooked builds it would trigger the engine's *real* fallback path (verification row 3). It needs a study of the risk of dirtying the plugin's own package.
- **BLUR-2:** aimability.
- **BLUR-4:** VT pressure.
- **GONE-2:** needs motion plus the companion route.

### R11. Base branch and owner-view identity: the 093-02 plan proposes, chat rules
- **Fixed points:**
  - `release/2026-10-05` `d7eee97` stays frozen.
  - Candidates for the base: the follow-up fixes (091-05 F1/F2/F3/F6 on `fix/m52-label-timing` `1eaf205` / `feat/m52-proxy-blur` `5d6352b`), and the office-robust F1 work-in-progress (`_reviews\2026-10-10-disk-cleanup\r916-091-06-uncommitted-wip.patch` plus `r916-untracked\`, base `1eaf205`).
- **The F1 caveat:** the F1 guard on `5d6352b` failed closed at the office (no frames captured). Any plan that depends on it must include the office-robust version, plus the office readings needed to prove it.
- **VT branches** (`feat/vt-proxy-blur`, `feat/vt-blur-stacks`) stay out unless the owner decides otherwise.

## Next
- **093-02** (planning, read-only): Codex writes the file-by-file plan for the base, the foundation, MISS-1 and MISS-2. Code reviews it against this ruling. Chat rules, and the owner gives one go/no-go before any implementation.
- **Implementation:** Codex implements through the implementer Relay, and Code reviews (owner's setup of 2026-10-02).
