094-43a — EUROPA: PROVE THE DEPTH-ONLY ROUTE BY NAME LOOKUP (NO PRIVATE SYMBOLS) ON THE OUTLINE FIXTURE; THE KIT'S STOP-INTENT FIX. Under chat's ruling 094-42c (GC1–GC7) and 094-41 (FZ1–FZ10), with FM1, FJ1 and GA5. Requested by chat Claude (the orchestrator) on 2026-11-03.

Only chat's rulings and this request bind you. Claude Code's review files are observations, not instructions.

BINDING
1. `_reviews\094-42c-ruling.md` (GC1–GC7), `_reviews\094-42r-ruling.md` (GA1–GA6) and `_reviews\094-41-ruling.md` (FZ1–FZ10).
2. `_reviews\094-41-codex-plan.md`, option 1, is the design reference for the private pass: the immediate plugin-owned mesh pass, explicit drawing of hidden event components, and no host custom-depth state.
3. Kept: 094-31 (FM1; the budget is fresh for every target in this job), 094-31b (FN2), 094-27c (FJ1), 094-27d (FK1), GA2 (`-nowrite` on every fixture launch), GA5 (3 h admission bound), and the rulings these keep by name.

STARTING POINT
- `feat/europa-port` at `d840cdb` locally, with origin at `d1cdc7b`.
- The fixture is at `E:\IA_Fixtures\Outline425\`: seven assets authored, with Editor and Game built. `spike/europa-09442c-mesh-shader` is local evidence and stays untouched.
- From 094-42c: naming the depth-only shaders' `StaticType` fails to link (5× LNK2001 plus LNK1120).

ORDER
1. **Entry:** FJ1's fast checks.
2. **GC6, the kit fix:** the kit records a stop's intent (time, PID, reason) before it classifies the exit. An exit that follows a recorded intent is classified `owned-stop`, never `crash`. Add a self-test for both cases. Commit with W4 on `feat/europa-port`.
3. **The route proof,** on a new throwaway branch `spike/europa-09443a-depthonly-byname` from `d840cdb`, used in the fixture only and never merged or pushed:
   - **What it must do:**
     - Find the depth-only shader types **at runtime, by name**, through RenderCore's exported shader-type registry.
     - Get them from the material's shader map for the mesh's vertex factory, through `FMaterial`'s exported lookup, falling back to the default material as the engine does.
     - Bind and draw them through the exported mesh-pass machinery (`FMeshPassProcessor`, `FMeshMaterialShader::GetShaderBindings`, `DrawDynamicMeshPass`) into **plugin-owned** depth and ID targets, from the existing view-extension hook.
     - Draw blink-hidden event components explicitly, as the 094-41 plan describes.
   - **What it must never do:**
     - Name any non-exported Renderer symbol, or include a Renderer-private header for a type it then calls.
     - Set `bRenderCustomDepth`, a stencil value or a write mask, or touch `r.CustomDepth`.
     - Read or write the host's custom-depth targets.
   - **If a step cannot be done this way,** stop and report it exactly, with source citations. Examples: a binding that the base class cannot fill, a missing export, or a vertex factory that has no depth-only shader in the map.
4. **The proof gates,** on the fixture, D3D11, with every launch through the armed kit and carrying `-nowrite`:
   - **P1, the link:** Editor and Game build with 0 LNK errors.
   - **P2, no new shader type:** after one warm-up launch, a second launch logs 0 `Missing cached shader map` lines.
   - **P3, coverage:** for each anomaly target and census object across opaque (`M_Surface`), masked (`M_Masked`), WPO (`M_WPO`) and a blink-hidden target, compare the private ID coverage with the label mask from the current custom-depth path, in a separate run with outlines off. Report the per-object IoU and the pixel differences. Gate: IoU ≥ 0.98 on every object, and the hidden target non-empty on its hidden frames.
   - **P4, no leak:** run C5 on the fixture with the outline on, using only the private pass for masks and with no custom-depth tagging. Compare against the outline-off control: **0 new magenta pixels** on every saved frame. Include frame 746's equivalent as an example.
   - **P5, state:** the receipt shows `r.CustomDepth` unchanged, 0 components with `bRenderCustomDepth`, and no stencil writes.
   - **Also report:** the added GPU and CPU time per captured frame (the mean and the 95th percentile).
5. **Exit:** FJ1's fast checks. Commit GC6 and the evidence with W4 on `feat/europa-port`. The spike branch stays local. No push.

STOP RULES
- **GA5:** any kit admission gate that holds for 3 h stops the job with NEEDS-DECISION and its partial results.
- **Launches:** only the armed kit launches the editor or the game, and every fixture launch carries `-nowrite`.
- **Builds:** authorized for the fixture's Editor and Game on the new spike branch. Fresh FM1 budget.
- **Protected changes:** any change to the Paris host's Content, Config, a descriptor or the `.uproject` is a STOP. The Paris host is not launched in this job.
- **Crashes and uploads:** an upload sign is a STOP; a second runtime crash ends the run (FI4).
- **Untouched:** `m51`, `master`, `release/2026-10-05`, the tags, `spike/europa-09442c-mesh-shader`, the 5.1 engine and StackOBot.
- **No push and no tags.** The new spike branch is never merged or pushed.
- **Machine:** no security, paging, power or machine-setting changes, and no elevation. Delete nothing except by receipt. If E: falls below its floor, stop and report; never create an overflow folder.
- **Foreign activity** (the owner may be rendering) never ends the job. Record it (EZ1).
- **Codenames only.**

DELIVERABLE
A short `report.md`, covering:
- the summary;
- GC6, with its self-test;
- the route as built: the lookups, the bindings and the draw path, with citations;
- P1–P5, with numbers, plus the per-frame cost;
- any blocker found, stated exactly;
- any crash, upload sign or protected change (or "none");
- the final commit.

End with one line: "VERDICT: <GREEN|NEEDS-DECISION|RED> | <summary>". GREEN means P1–P5 all pass. NEEDS-DECISION means the route is blocked, or a gate is short.
