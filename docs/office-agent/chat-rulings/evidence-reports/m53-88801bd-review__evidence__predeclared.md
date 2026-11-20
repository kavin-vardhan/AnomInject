# 090-10c pre-declaration (written 2026-10-02 ~02:15, before any C2 build or leg; the "before" census legs had run on C1 `d38bcc4`)

## Frozen home inventory
- Fixture `CB_TexCorruptLevel`, PIE: `inv\P_B_FIX.json` (82 renderable candidates; plain census uv 67/82, normal 8/82 on C1).
- MainWorld, PIE: `inv\P_B_MW.json` (343 candidates; plain census uv 25/343, normal 8/343 on C1; 266 refused nanite_unmaskable).
- Office-like stressors (scratch, by receipt): texture-group LODBias=1 for World/WorldNormalMap/WorldSpecular via a scratch
  `DefaultDeviceProfiles.ini` on the scratch host (PIE) or `Saved\Config\Windows\DeviceProfiles.ini` beside the staged build;
  `sg.TextureQuality 1` (r.Streaming.MipBias 1); `r.Streaming.PoolSize 300`; per-texture LODBias=1 on every 3rd mesh actor and
  NumCinematicMipLevels=2 on every 3rd+1 (in memory, nothing saved); `-DPCVars=r.ShaderCompiler.JobCache=1,r.ShaderCompiler.JobCacheDDC=1`.

## Declared positives (fixed now; tuning may not change them)
- **D1** fixture `StaticMeshActor_0` (uv_corruption) and `StaticMeshActor_12` (normal_corruption) under each variant V1..V6 (below), PIE
  and staged: eligible, applied, visibly corrupt (090-10b pixel rule), edges 0 frames off at both ends (AA off, >= 3 fires per type).
- **D2** identity mode on `StaticMeshActor_0` under V1, V2, V3, V5, V6: pixel-exact to the uncorrupted original (median drawn-box
  difference vs the last clean frame within the clean-to-clean null). Under V4 (global sampler bias): exact or the deviation stated.
- **D3** office-like census (fixture and MainWorld, PIE): after the fix, NO object is blocked by a handled key:
  `runtime_lod_bias/*`, `not_fully_resident` (other than `/unmappable`), `streaming_pending`, `partial_footprint/slot_translucent`,
  `partial_footprint/slot_empty`. They may appear as notes. **Zero eligible on office-like content = FAIL.**
- **D4** genuine refusals stay refused (controls): `TC_Nanite` nanite gate, `TC_Skeletal` default_material_path, VT / cube /
  R16F / excluded group / host_mid producers, `TC_NoMesh`.
- **D5** B: a multi-component object with one whole component and one failing component is admitted; the mask (`mask_file` tag
  pixels) covers only the admitted component's pixels (no tagged pixel on the skipped component's screen area); with
  `IAI.Bench.TexCorruptNoPartScope 1` the mask covers both (the can-fail).

## Variants
- V1 per_texture: asset LODBias=1 on the target's textures (PIE: in-memory edit; staged: `IAI.Bench.TexCorruptHostTexBias`).
- V2 texture group: scratch device profile World/WorldNormalMap LODBias=1.
- V3 cinematic: NumCinematicMipLevels=2 on the target's textures (as V1's routes).
- V4 global_sampler: `r.MipMapLODBias 1`.
- V5a streaming budget: `r.Streaming.MipBias 1`, `r.Streaming.UsePerTextureBias 1`; V5b global streaming: same with 0.
- V6 not fully resident: `r.Streaming.PoolSize` small + the fixture's far `TC_Stream2k`.

## Can-fail
`0a69ea3` (archived `m53-0910b-65607703`: exe + editor DLLs) on the same setups: refused with runtime_lod_bias/<sub> (or
not_fully_resident) and 0 events. Where 0a69ea3 has no route to a variant in the staged build (V1/V3 need the new lever or a
cook), that cell is reported as not runnable, not as a pass.
