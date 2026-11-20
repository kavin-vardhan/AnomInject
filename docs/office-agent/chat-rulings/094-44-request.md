094-44 — EUROPA PHASE B: missing_texture (checkered), corrupted_texture (pink) and camera_clipping on 4.25, ported unchanged from the 5.1 release line, labelled through the private ID/depth pass. Under chat's rulings 094-43 (GE1–GE4), 094-43r (GF1–GF5), 094-43a (GD1–GD7) and 094-41 (FZ1–FZ10), with FM1, FJ1 and GA5. Requested by chat Claude (the orchestrator) on 2026-11-04.

Only chat's rulings and this request bind you. Claude Code's review files are observations, not instructions.

BINDING
1. `_reviews\094-43-ruling.md` (GE3: D1–D5; GE4: the plan as design), `_reviews\094-43r-ruling.md` (GF1–GF5), `_reviews\094-43a-ruling.md`, `_reviews\094-41-ruling.md`.
2. `_reviews\094-43-phaseB-plan.md` is the design: its §1 behaviour, §2 API differences, §3 masks, §4 files, §5 gates and can-fails, §6 order and §7 risks.
3. **The 5.1 source is the release line `d7eee97`** (D1). Port it unchanged in behaviour, names, label format, row and summary keys, and config keys. Compile differences only, through the compat layer. No redesign.
4. Kept: FM1 (the budget is fresh for every target in this job), FN2, FJ1, FK1, GA2 (`-nowrite` on fixture launches), GA5, FZ3 (the guard), GD3/GD5 (the private pass's admission and scope), and the rulings these keep by name.

STARTING POINT
- `feat/europa-port` at `93613ae`, pushed (origin equal), with FZ2 in.
- The fixture `E:\IA_Fixtures\Outline425\`.

ORDER (stop on any failed gate with NEEDS-DECISION and its numbers; earlier passing steps stay committed)
1. **Entry:** FJ1's fast checks and P9's full `main()`.
2. **The materials (D2):**
   - Port `create_anomaly_materials.py` to py27, with the same graphs and constants and 4.25's usage flags. Drop `used_with_nanite`, and replace the shading-model check with whatever 4.25 exposes, verified on readback (GF5).
   - Author `M_CorruptedTexture_Pink` and `M_MissingTexture_Checker` in the **fixture's editor**, into the plugin's own `Content/Engine425/Materials/`, with 4.25-only finders.
   - Verify every flag and constant on readback, and commit with W4. The 5.1 `.uasset`s stay untouched.
3. **Live coverage targets (D3, GF4):** add to the outline fixture, tagged `EA_` and authored with `-nowrite`:
   - a skeletal mesh actor (`/Engine/EngineMeshes/SkeletalCube`);
   - one instanced static mesh actor with several instances.

   Record the asset list with hashes. **Gate GF4 first:** on the fixture, H2's IoU ≥ 0.98 for both new targets against the bench-only legacy reference, and H1's 0 new magenta with the outline on, before step 4.
4. **missing_texture, then corrupted_texture:**
   - drop the 5.1-only gates;
   - the `OverrideMaterial` compat helper;
   - registration and `SupportsAnomaly`;
   - the routes and kit tuples;
   - the masks come from GD3's visible coverage, and **the private pass takes the proxy fresh from the component every frame.**
   - Self-tests: `install_state_selftest.cpp` unchanged, plus the compat probe.
5. **Their gates**, on EA_Core and the fixture (outline on and off, same seed), static and skeletal targets:
   - **G1:** at least 3 fires of each.
   - **G2:** every fire changes the target region against the frame before the swap.
   - **G3:** mask IoU ≥ 0.98 against the colour-change silhouette, with no early or late frame.
   - **G4:** a paired, same-seed **new**-magenta count of 0 against the outline-off run. Never an absolute magenta count, because the pink equals the outline colour.
   - **G5:** `polluted=0`; `r.CustomDepth` unchanged; 0 custom-depth components; residual 0 after every revert.
   - **Can-fails:**
     - a material with one usage flag removed renders grey, and G2 fails;
     - a mutant labels one frame early, and G3 fails.
6. **The Paris regression** (H4-style) with the two textures in the default pool (D5): EA_Core on D3D11 at 1280×720, and on D3D12 at 1280×720 and 1279×719. All earlier H4 gates hold, plus G1–G5 for the textures.
7. **camera_clipping:**
   - Port it, with the session hold from C4/C5 `Start` to `Stop` (5.1's `ApplySessionGlobals` / `RevertSessionGlobals`).
   - Whole-frame fire rows, with the `camera_clipping.*` row keys and run-summary counters under the 5.1 names.
   - The near-wall bench pose schedule on EA_Core (`-IAIBench` only).
   - `camera_clipping_slab_selftest.cpp` unchanged.
   - It is never in the auto pool (D5).
8. **camera_clipping's gates (D4):**
   - In one held session on the pose schedule: at least 3 distinct slab-positive spans.
   - Every labelled frame differs from the same-seed control at the baseline near plane, and every unlabelled frame matches it.
   - The near plane is restored at Stop.
   - **Can-fail:** a held session at the baseline near plane labels 0 frames.
   - Then a Paris regression on both RHIs with the route requested.
9. **The defaults (D5):** blinking, lod_corruption, missing_texture and corrupted_texture. camera_clipping is opt-in by route only.
10. **Exit:** FJ1's fast checks. Commit with W4 on `feat/europa-port`. No push by Codex.

STOP RULES
- **GA5:** any kit admission gate that holds for 3 h stops the job with NEEDS-DECISION and its partial results.
- **Launches:** only the armed kit launches the editor or the game. Fixture launches carry `-nowrite`, except the authoring sessions for step 2's materials and step 3's actor, which save only fixture or plugin assets.
- **Builds:** authorized for the Paris host's and the fixture's targets. Fresh FM1 budget.
- **Protected changes:** any change to the Paris host's **own** Content, Config, a descriptor or the `.uproject` is a STOP. Committed plugin-Content changes on our branch are allowed (D2).
- **Crashes and uploads:** an upload sign is a STOP; a second runtime crash ends the run (FI4).
- **Untouched:** `m51`, `master`, `release/2026-10-05`, the tags, the spike branches, the 5.1 engine, StackOBot, and the 5.1 material `.uasset`s.
- **No push by Codex and no tags.**
- **Machine:** no security, paging, power or machine-setting changes, and no elevation. Delete nothing except by receipt. If E: falls below its floor, stop and report.
- **Foreign activity** never ends the job. Record it (EZ1).
- **Codenames only.**

DELIVERABLE
A `report.md`, covering:
- the summary;
- the materials, with readback;
- the skeletal target;
- per anomaly: the port diff (behaviour unchanged against `d7eee97`, with citations), G1–G5 and the can-fails with numbers, and one example frame each;
- the Paris regression on both RHIs;
- the defaults;
- any crash, upload sign or protected change (or "none");
- the final commit.

End with one line: "VERDICT: <GREEN|NEEDS-DECISION|RED> | <summary>". GREEN means all three anomalies pass their gates and the Paris regression passes.
