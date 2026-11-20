# REPORT for 093-01-natural-causes-feasibility.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 093-01-natural-causes-feasibility.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-10-13 12:46:30 | finished: 2026-10-13 14:02:05 | duration: 76 min | exit code: 0
context at last turn: 619306 tokens (62% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\093-01-natural-causes-feasibility.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN. Codex's review is collected unedited and ledgered, and I checked 53 of its claims against the UE 5.1 source and the release code.

**Plain summary for the owner**
1. **Soon, with a still camera:** swap an object's colour texture for the engine's own placeholder (MISS-2), and blur a texture through its real "resolution too low" setting, even when several objects share it (BLUR-1). Making an object vanish because its draw distance is too short (GONE-1) also works, but only once we can still draw its outline for the label. The engine's grey-grid fallback (MISS-1) works too, but as a controlled copy of the look, not a real failure.
2. **Needs the camera or player to move:** late-loading textures, objects popping in late, LOD and light pops at distance, the camera going into walls, objects passing through each other.
3. **Needs engine changes:** nothing on the list needs them just to cause the bug. They would only be needed for perfect per-frame proof of what the GPU drew, per-object detail on virtual textures, or faking real driver/GPU-memory bugs, which no plugin can do honestly. Squeezing all texture memory (BLUR-2) can't be aimed at one object, so it's parked.

**The Codex job**
- Run folder: `_relay\runs\2026-10-13-093-01-natural-causes-feasibility\`. `run-meta.txt` shows `gpt-6-astra` at effort `max`, matching what was requested.
- Duration 3,981 s (12:48:57–13:55:18), exit 0.
- I stopped my own first launch about a minute in, by PID. The 2-hour background-task limit could have killed Codex mid-run, so I relaunched it detached in the same folder. No `-r2` folder was needed. The two other `codex.exe` processes (started 07:55) weren't mine and I left them alone.
- Its VERDICT line: `MISS-2 and BLUR-1 FEASIBLE-WITH-CONDITIONS plugin-only | MISS-1 FEASIBLE as R1 controlled calibration | GONE-1 FEASIBLE-WITH-CONDITIONS (equivalent measured reference mask) | BLUR-2 deferred | engine changes: none intrinsically for bounded catalog levers; required for literal universal actual-draw/binding/VT-sampling telemetry | strict three-module constraint conflicts with existing capture/copy infrastructure`

**Pilot candidates**
- **MISS-1: feasible, but as an honest R1 (forced) version.**
  - Trigger: assign `UMaterial::GetDefaultMaterial(MD_Surface)` (the configured WorldGrid) to the target's slots.
  - Probe: the material actually bound on the render thread (`GetMaterialWithFallback`), plus picture and mask evidence. `CheckMaterialUsage_Concurrent` can predict a usage-flag fallback before arming.
  - Reliably visible: the whole material changes. Reject targets that already look like the default.
  - Revert: restore each raw material slot exactly, including empty/inherited ones.
  - Main risk: calling it a real shader failure. It is a forced swap.
- **MISS-2: feasible with conditions.**
  - Trigger: a temporary dynamic material instance (the m53 route-B machinery) binds `GEngine->DefaultTexture` to a genuinely visible colour texture parameter. Normal and roughness inputs stay real.
  - Probe: the texture bound on the render thread, plus a picture change measured against a clean reference.
  - Revert: restore the original slot.
  - Main risks: picking the true colour parameter (the binding tree only knows "is a normal map"), packed or reused parameters, and virtual-texture slots, where an ordinary texture is skipped with a warning.
- **BLUR-1: feasible with conditions.**
  - Trigger: the release hold lever (`NumCinematicMipLevels` plus `UpdateCachedLODBias`) applied to the shared texture itself, labelling every visible user.
  - Probe: the release's render-thread residency observer (resident mips against a pre-arm baseline, keyed per capture), widened to every user of the texture.
  - Reliably visible only on ordinary streamable, detailed textures. It can't touch virtual textures, which ignore this setting, so the virtual-texture-heavy first office host gets only a subset.
  - Revert: restore the saved value and watch the mips stream back. Recovery is asynchronous.
  - Main risks: users of the shared texture changing during the event, and in PIE, editor views blurring too.
- **BLUR-2: not reliably aimable; deferred.**
  - Trigger: shrink `r.Streaming.PoolSize`. The engine protects visible, small textures first.
  - Revert trap: restoring the value `-1` can leave the squeezed budget in place. Live resizing also needs `UseFixedPoolSize=0`.
  - The over-budget on-screen warning never reaches the default SVE-captured frames, but would appear on the backbuffer capture path.
- **GONE-1: feasible with conditions.**
  - Trigger: `SetCullDistance` below the camera-to-bounds-origin distance. It pops out instantly unless the material uses the engine's distance-fade feature.
  - Revert: restore both the draw-distance setting and the cached cull distance. Restoring only the setting can wipe a cull-volume value.
  - It does nothing on components marked never-cull or under an HLOD parent.
  - Main risk: a culled object writes no stencil mask. The m26 veto would then read zero and delete every event, so it needs the reference-surface mask Codex proposes, which still has to be proven on the bench. The visibility layer is fine as the "should be visible" check, since it doesn't depend on "was rendered".

**My verification:** 53 claims checked: 46 confirmed, 4 corrected, 3 unverifiable. The corrections:
- **PIE and missing usage flags:** neither the draft ("PIE auto-fixes usage flags") nor the review ("includes ordinary PIE operation") is right. While PIE ticks or draws, the engine treats itself as a game, so it shows the default material without repairing (App.h:157; EditorEngine.cpp:1837, 2137; Material.cpp:1749). Repair, and dirtying the asset, happens only for checks made outside PIE.
- **Flat normal asset:** the review names `/Engine/EngineMaterials/DefaultNormal`, but that is the default material's 1.9 MB normal map. The flat one is `/Engine/EngineMaterials/FlatNormal` (4 KB), which isn't a startup package and so needs an explicit cook reference.
- **GONE-1 distance scale:** it is `ViewDistanceScale` times an FOV factor (SceneVisibility.cpp:937), not `ViewDistanceScale` alone. The FOV factor is 1 unless the host configures it.
- **GONE-1 fade margin:** the review's 10 m safety margin is only needed for materials that use distance fade (StaticMeshRender.cpp:1924). Applied to everything, it would wrongly rule out most targets nearer than 10 m.

**Engine-change entries:** none of the 40 catalog IDs needs an engine edit just to trigger the bug. Engine instrumentation would only be needed for:
- an exact per-frame record of what each object drew, with which material and textures (the literal Q2 ask, most acute for MISS-1 and GONE-1/2/4);
- per-object virtual-texture sampling detail (BLUR-4 and VT versions of BLUR-1/2);
- forcing a real shader failure on host cooked materials (MISS-1's literal cause).

A faithful driver or GPU-memory fault (CORR-3's driver reading) is out of reach even with engine changes.

**Pilot order and label keys:** the review amends the draft's §8 order:
- New natural work goes MISS-2 → BLUR-1 → GONE-1, after the label foundation, with GONE-1 gated on proving the reference mask.
- MISS-1 runs alongside as the R1 calibration, not as a caused failure.
- BLUR-2 becomes research only, not a pilot.
- LOD-1, NRM-1 and GONE-3 already count as real setting, data or code mistakes and need no rework. NRM-3 can follow MISS-2.

Its label-key proposal is all additive:
- `natural_metadata_version`;
- `injection_mode` (controlled/natural) on events and label entries;
- optional `cause_id`, `cause_family` and `cause_event_id` linking several ordinary per-object events;
- a root `natural_causes[]` record (mechanism, parameters, scope, aimed target, arm/revert/recovery frames, restore status);
- `effect_observation` (observed/no_effect/unknown/not_applicable);
- a typed `severity` (for example mip deficit, or missing reference pixels) and `effect_mask` provenance;
- new `cause_*` counts in the run summary only.

No existing key changes meaning; `target_drawn_pixels` is never filled with reference-surface pixels, and a cause with nothing visible adds no event.

**NEEDS-DECISION:** none for this brief. Inputs for chat's ruling:
- whether the three-module rule gives way to the existing capture/copy infrastructure;
- the reference-mask contract for GONE-1;
- whether BLUR-1's restore may use the scoped `StreamIn` repair under the A4 ruling;
- MISS-1 as R1 only.

One note: the review refers to the office hosts by their codenames, as the relay rules block tells it to, rather than "first/second office host". No game title appears. I left the content unedited as required.

Files are in D:\IntrusiveAnomalies\_reviews:
- 093-01-codex-natural-causes-feasibility.md
- 093-01-code-verification.md
api-equivalent cost (not billed on Max): $17.75 | turns: 161 | running total across all briefs: $4,449.56
