---
name: europa-phaseb-texture-clip-facts
description: "Phase B (camera_clipping, corrupted/missing_texture on 4.25): release code behind ANOMALY_ENGINE_51; 5.1 uassets unloadable; session-global hold; outline colour == pink; 094-44r G3 failed on bloom halo; 094-44c fixture fix passes G1-G3 + can-fails, stopped at G5 (no live r.CustomDepth replies in PIE)"
metadata:
  node_type: memory
  type: project
  originSessionId: <uuid>
  modified: 2026-11-05T06:05:40.979Z
---

Plan: `_reviews\094-43-phaseB-plan.md` (2026-11-04).
- **Which 5.1 version:** `master` b5f15a3 and m51 53bf725 are OLDER than release d7eee97, which has 289 more lines in the three anomalies (090-09 installed-state/full revert, 086-02/084-07 slab rule). The port branch already carries the release files, compiled out by `#if ANOMALY_ENGINE_51`. On 4.25, `SupportsAnomaly` (Compat/AnomalyCapabilities.cpp:5) lists only four ids.
- **Assets:** the plugin's `M_CorruptedTexture_Pink`/`M_MissingTexture_Checker` headers read legacy −8 / UE4 522 / UE5 1008. 4.25 zeroes the versions for legacy < −7 (PackageFileSummary.cpp:95-100) and the linker fails (LinkerLoad.cpp:1175). They must be re-authored in 4.25 with every usage flag set; a missing flag means grey in `-game` (Material.cpp:1643).
- **4.25 compile gaps:** `OverrideMaterials[i].Get()` (raw pointer array, `:69`, `:285` in each texture file) and `GetCameraCacheView` (CameraClipping.cpp:159).
- **camera_clipping:** it is a session global. TryFireOnce skips it (AutoInjector:318), so the 4.25 C5 driver must hold it from Start to Stop, as 5.1's `ApplySessionGlobals` does. `FAnomalyMasks425::Labelled` returns false with no target (:317); add a whole-frame path. AnomalyBench (`cc_v1`) is a stub on 4.25.
- **Fixtures:** the Outline425 outline is float3(1,0,1), the same as the pink LinearColor(1,0,1), so leak checks must be paired same-seed "new magenta" counts. EA_Core has no skeletal mesh; 4.25 ships `/Engine/EngineMeshes/SkeletalCube`.

**Why:** 094-44 will be built from this plan.
**How to apply:** verify these against the source again before citing them. Related: [[europa-custom-depth-leak-094-41]], [[europa-425-editor-runtime-facts]], [[europa-port-51-base-is-release]].

**094-44 (2026-11-04, 05:02-05:31, NEEDS-DECISION):**
- The fixture editor authored both 4.25 materials. Local commit `9c87836` (unpushed) holds the assets, the py27 script, docs and 7 lines of unbuilt `#elif ANOMALY_ENGINE_425` finders. I did not push: the native change never compiled.
- **The stop:** Codex added a fresh-process readback launch. The kit stopped it with `FE3 unreadable UnrealBuildTool command line`, because the editor's startup UBT child exited in under 267 ms, before the first poll. When the kit did read it (authoring launch), it was `-Mode=ValidatePlatforms -allplatforms`. This can recur at any editor launch.
- **My byte-level readback (no editor):** `D:\IA_Europa\Temp\CodeReview09444\uasset44.py` parses 4.25 packages (names, imports, exports, tagged properties). `matcheck44.py` checks the flags and walks the checker/pink graphs against d7eee97's script. Its can-fail runs on the fixture's M_Surface, M_Masked and M_Outline.

**094-44r (2026-11-04, 05:49-07:31, NEEDS-DECISION):**
- FE3b (ae82fe7, `tools/europa/runtime_creation.py`, 10 ms owned-tree poll) caught every editor UBT child at creation (6 sessions, all ValidatePlatforms). Self-test `runtime_creation_tests.py <newdir>` re-runs read-only in ~10 s.
- GF4 passed by my readers: fixture EA_Skeletal and EA_Instanced (3 instances) IoU 1.0, 0 new magenta. The fixture now has 9 assets (adds M_LiveSurface and M_MissingTexture_NoSkeletal, the can-fail mutant).
- Texture port = d7eee97 plus only the OverrideMaterial compat helper (8e59a56). Its first gate run failed G3 (whole-frame IoU 0.25-0.26): a bloom halo around a saturated target on a black background, ~3x the mask area. Every mask pixel changed and both edges were clean. No static fire either: OF_LOD_A covers 27.9%, above the 25% census ceiling.
- **Readers:** `D:\IA_Europa\Temp\CodeReview09444r\texrev44r.py` (4.25 mask PNGs are RGBA; the tag is in R), plus iou44r/newmag44r/children44r.

**094-44c (2026-11-04, 07:53-09:49, NEEDS-DECISION, local feat 90d4275, nothing pushed):**
- **The fixture fix (GH2/GH3), runtime-only:** bloom 0; `r.EyeAdaptationQuality 2` + `r.EyeAdaptation.MethodOverride 3` + physical-camera exposure off + `r.ExposureOffset -2.1875` (quality 0 forces exposure compensation 1, so ExposureOffset did nothing before: ShowFlags.cpp:367, PostProcessEyeAdaptation.cpp:240/307); point light 0 + six shadowless fills; FOV 30, distance 1300; baselines swapped to WorldGridMaterial. The static target is now `OF_LOD_B` (cube) at scale 1.3. Drawn share: static 7.149 %, skeletal 6.685 %. The annotation's bounds-based `coverage_pct` says 31.74 for the skeletal.
- **Result:** both textures pass G1-G3 (static 13/5, skeletal 9/8 fires; IoU 1.0; 0 px outside; clean edges). Grey mutant: 29 skeletal events with 0 changed px (log: "missing bUsedWithSkeletalMesh ... Default Material will be used in game"; PIE counts as game in 4.25). The early-label can-fail is a reader-side mask shift. GH2 margins are thin: baseline 51.2-51.7 against a 51.0 floor, pink B about 202.
- **Codex stopped at G5:** the kit's console probe types into the window, and PIE never routes it to the console, so only the startup `r.CustomDepth "1" Constructor` was logged. G4, Paris and camera_clipping are unrun.
- **The gate legs ran in the fixture editor's PIE via a py driver** (`Scripts\texture_pose09444c.py`). The first PIE launch auto-started the Epic Games Launcher + EOS (31 descendants; Codex closed 12 after exit). Later legs used `-BENCHMARK`, which skips the launcher start (EditorEngine.cpp:619) but also forces a fixed step, a fixed seed, no sound and no ini writes.
- **Readers:** `D:\IA_Europa\Temp\CodeReview09444c\`: texrev44c (adds contrast and pink checks), children44c (roots as args), kids44c (one creation log), crash44c, example44c, runlow44c.ps1 (BelowNormal runner).

**094-45 (2026-11-04, 15:15-16:08, NEEDS-DECISION, local feat 2d93e67, nothing pushed):**
- GL3 fixed (empty-reason owned stop = owned-stop; my mutants fail). GL2 view lock: EA_Camera (-550,0,480) P-12 FOV50; plugin bench probe `IAI.Capture.BenchGeometry` (-IAIBench, EA_Core). Drawn shares 8.5059/8.5059/12.8707 %, EA_Floor 23.492 % (never previewed; C5 fired it anyway).
- D3D11 H4 passes, but G3 fails: checker on the cubes 0.49 (one checker grey renders within 2 levels of the original at -5 EV), floor checker 0.4065, floor pink 0.8131 (cube shadows are (0,0,0) under any material). 0 changed px outside masks, edges clean. The -game previews already show checker change fraction 0.49, so a pre-fire "swap changes >=98 % of silhouette" check would catch it.
- Evidence layout changed: session `D:\IA_Europa\Receipts\09445\H4\<leg>\02\session`, frames `E:\IA_BuildCache\09445-evidence\<leg>\02\plugin-frames\epoch01`; geometry/preview folders under `attempt-01`. Census tags live in evidence field 0 (M/D are 0 without fires); visible = custom >= scene - 4/(2^24-1).
- Readers: `D:\IA_Europa\Temp\CodeReview09445\` (h4check45/edges45 take the frames dir as an extra arg, geom45, gk2game45, diag45/45b, gl3review45, crash45, bg45.ps1 detached runner).

**094-45r (2026-11-04, 16:23-19:18, NEEDS-DECISION; Stage 1 PUSHED c5d0919, Stage 2 local c706b7c):**
- GM2 bench (host helper only): blue BasicShapeMaterial MID (0.05,0.2,0.5) on all 4 EA_Core targets incl. EA_Floor, EA_Light 0 + six shadowless fills; recalibrated to -1.875 EV (old -5 EV now too dark). Swap visibility 1.0 everywhere, so no explained-pixel fallback; all 3 legs strict IoU 1.000, 0 spill, 140/140 clean edges.
- Stage 2: release camera_clipping enabled via `Compat/AnomalyCameraSession425.cpp` (held Start-Stop, 5.1 row/summary keys; adds run_summary.json + `whole_frame` key on EVERY C5 run). Bench switches -IAICameraSlabBench/-IAICameraBaseline: 624 far + six 288-frame near/far holds, near eye 50 cm before EA_LOD. D3D11: 3 spans (624-911,1200-1487,1776-2063), labelled frames change >=902,398 px, baseline labels 0, near restored; D4 FAILS strictly: 914 unlabelled frames differ by 1-3 px at delta 3 (8 fixed pixel spots), +/-1-2 levels on ~40 % of every frame; no same-near control exists. Codex's camera legs used camera-only routes (no default-route co-run).
- Readers: `D:\IA_Europa\Temp\CodeReview09445r\` (cal45r = calibration/prefire recompute, cc45r = D4 pair reader, ccdiag45r, keys45r = 5.1 key parity, sum45r, cspace45r, h4check45r/edges45r).

**094-46r (2026-11-05, 09:37-11:25, NEEDS-DECISION, local feat 886e1a0, nothing pushed):**
- GO2 near-plane floor = `-IAICameraFarOnly` near-100 session (2,352 default-section frames, no tail) vs control A by index. D3D11 2/255 (reused 094-46 control; rebuild adds nothing: new vs old near-100 max 1 level); D3D12 2/240. D3D11 positive max 254 at 2 -> PASS; D3D12 positive 255 on 6 frames (one repeated state, 7 apart) -> FAIL count only, 0 px above 2. Info: far-only vs control B = 2/254 on both RHIs (a single pair's max is a sample max). 1279 legs, GO3 default-only, card reads unrun.
- Codex now `compact`-compresses job-created receipts to keep the 3 GiB reserve (hashes unchanged; I rehashed 13, 0 mismatches).
- Readers: D:\IA_Europa\Temp\CodeReview09446r\ (floor46r = pose-aware pair floor with near-hold check, d4_46r = D4 vs two floors, d4fail46r = failing-frame and distribution report, sum46r, gn4_46r, crash46r, bg46r.ps1; h4check46/edges46 copies). Receipt toolkit: code-receipts-094-46r (clone_scripts46r.py pattern).

**GE3 check, 094-43r (read-only, 4.25 source):**
- `/Engine/EngineMeshes/SkeletalCube` is a real SkeletalMesh, an old package (UE4 version 288) that loads. The only other engine skeletal mesh is the editor-only `EditorMeshes/SkeletalMesh/DefaultSkeletalMesh`.
- **py27 port of `create_anomaly_materials.py`:** every MEL, EAL and AssetTools call, expression property and enum exists in 4.25, except two:
  - `used_with_nanite` does not exist; drop it.
  - `unreal.MaterialShadingModel` is probably not exposed. `EMaterialShadingModel` is `UENUM()` without BlueprintType, and its only Blueprint reference is in BlueprintMaterialTextureNodes, which the fixture does not enable. So `_verify`'s shading-model check must change.

**094-44d (2026-11-04, 10:09-10:50, NEEDS-DECISION, local feat 4cc8b45, nothing pushed):**
- G4 PASS: outline-on (driver texture_pose09444d, Blendable 1, frozen settings byte-equal, fixture binaries = 44c archive C) vs 44c's texture-off: 0 new magenta on 2,352 pairs, diffs are grain only (<=2 levels). Pair by anomaly tuples/pose/camera; grain_phase, sample frames and census tag values differ by design. My reader: `CodeReview09444d\newmag44d.py` (`--selftest` injects magenta rings, 40/40 caught).
- G5 PASS: in-process `get_console_variable_int_value('r.CustomDepth')` from a slate post-tick callback (`Scripts\capture_depth_read09444d.py`), 1 at startup and at saved 81/2300.
- Paris D3D11 (EA_Core, -game): H4 gates pass (`h4check44d.py`), textures fire (22/13 on three cubes), G2 pass, **G3 fails (IoU 0.088-0.096)**: EA_Core is overexposed (33% of frame saturated, targets 0.95-0.98 of full scale) with host bloom; changes start/end exactly with the swap. D3D12 legs unrun. I recommended GH2's bench-only fix on EA_Core.
- Kit change: Paris launch reserve 3 -> 2 GiB (host-capacity-rule.json), floor 50 unchanged.
- Lesson: run readers detached (`bg44d.ps1`) and keep polling; a foreground reader run hid the whole Paris leg from my sightings (10:26-10:35 gap).

**094-44e (2026-11-04, 11:10-11:52, NEEDS-DECISION, local feat d33689f, nothing pushed):**
- The EA_Core bench helper is host Source `Source\EuropaPhaseAProbe\EuropaTextureBench09444e.cpp` (`Europa.TextureBench <dir> calibrate|run <EV>`, `-IAIBench` + EA_Core + !Shipping). It sets bloom 0, eye quality 2/method 3, ExposureOffset, physical-camera exposure off and pitch −12. Calibration: 0..−8 EV grid, 195 previews, at `E:\IA_BuildCache\09444e-evidence\Calibration\01`.
- **No exposure passes GJ3:** the cubes' pink reaches R,B≥200 only at −3.0..−3.75, but about 17 % of their pixels saturate until −5.0/−5.125. EA_LOD's pink passes only at −5.25..−6.0 (it sits in the tonemapper's white shoulder at −3). I recommended GI1's shadowless-fill lighting on EA_Core (unmeasured).
- **Readers:** `D:\IA_Europa\Temp\CodeReview09444e\`: calib44e.py (recheck from preview frames), prefire44e.py (pre-fire checks + four edge frames per texture event; it fails 094-44d's leg as it should), h4check44e.py (= h4check44d). Call `bg44e.ps1`/`runlow44e.ps1` in-process with `-PyArgs @(...)`; through `powershell -File` the array arrives comma-joined.

**094-46 (2026-11-05, 08:01-09:19, NEEDS-DECISION, local feat eea2acd, nothing pushed):**
- GN4 counter ported (sums the 4.25 camera_clipping_unconfirmed transition counts; 0 in every session); all 56 5.1 camera_clipping keys now emitted.
- Combined bench (-IAICameraRegression): the 2,352-frame default section at EA_Camera, then six 48-frame near/far holds; 2,640 frames; auto pool stops at 2328. D3D11 control pair (near 10) floor = max |delta| 1, max count 560,155; positive (near 100) unlabelled frames reach 2 levels on <=254 scattered px (4,704 positions, lower 60 % of frame, mean signed ~0), in 2,440 of 2,496 frames, so GN2 fails by one level; labelled frames change all 921,600 px; 0 unlabelled px >2. H4/G1-G3/G5 pass on all three D3D11 sessions. D3D12 and the default-only additive run unrun.
- Readers: D:\IA_Europa\Temp\CodeReview09446\ (floor46, d4_46, d4diag46, h4check46/edges46 take N=2640 and restrict refs to the default section, keys46 = additive key-path diff, gn4_46, keys51_46, crash46, bg46.ps1).

**094-46s (2026-11-05, 11:46-12:58, NEEDS-DECISION, local feat f3fbba7, nothing pushed):**
- GP2 (level bound only, count info) passes D3D11 and D3D12 1280x720 from existing frames (0 px above 2 on 2,496 unlabelled frames each).
- D3D12 1279x719 (new near-10 control + near-100 positive, 1280 floors shared): D4 fails on all 131 EA_Floor texture frames (far default section, no clip); exceedances are the floor's farthest visible row (373) plus a few px of 374-375, 99.8 % inside the floor texture mask (masks identical), max 117; one pixel (375,239) just outside the mask reaches 4. Same pixel = 1 px within-session texture spill on the positive (IoU 0.999995); control spills 0. Codex judged G3 against the 0.98 bar, not GN1's strict 1.000. Unrun: 1279 far-only session, GO3, card reads.
- Codex compact-compressed 24 archived EXE/DLL/PDB copies (09446/09446r binary-archive) plus 33 receipts; hashes and mtimes unchanged (my compcheck46s.py).
- Readers: D:\IA_Europa\Temp\CodeReview09446s\ (d4_46s.py = D4 under GP2; d4diag46s/d4out46s localise failing pixels vs other masks; tex1px46s = out-of-mask texture pixel finder; crop46s; compcheck46s; gn4_46s; crash46s; bg46s.ps1).
