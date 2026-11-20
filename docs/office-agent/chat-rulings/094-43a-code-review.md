# 094-43a — Code's review: the name-lookup route links and adds no shader type, but labels an object one frame before the picture shows it (P3 fails); P4 was not run; GC6 is sound

**Date:** 2026-11-04, about 01:50 IST · **Author:** Claude Code (mailbox launcher and reviewer) · **These are observations, not rulings.**

**How it ran:**
- Resumed thread `01a14334` with `-OwnerApprovedFullAccess`, detached, W4 applied (no `GIT_*_DATE` in the child).
- Ran 23:36:21 → 01:07:01 (5,439 s), exit 0. Codex verdict: NEEDS-DECISION.
- Report collected byte-identical as `_reviews\094-43a-codex-report.md`.
- Receipts: `code-receipts-094-43a\`. My readers: `D:\IA_Europa\Temp\CodeReview09443a\`.

## 1. The route as built
Spike `c1193bd` (native `1cf84a0`), file `Source/AnomalyCapture/Private/Compat/AnomalyDepthOnlyByName425.cpp`.
- **Shader lookup by name:**
  - L39 `FindShaderTypeByName` (RENDERCORE_API, `Shader.h:2195`);
  - L40 `FMaterial::GetRenderingThreadShaderMap` (ENGINE_API, `MaterialShared.h:1764`);
  - L41 `GetMeshShaderMap` (inline, `:1193`);
  - L42 `GetShader` on `FShaderMapContent` (class RENDERCORE_API, `Shader.h:1725`).
  - Names used: `TDepthOnlyVS<true/false>` and `FDepthOnlyPS<false>`.
- **Fallback:** L61 `GetMaterialWithFallback`; L72 the default material for opaque, non-WPO meshes (the engine's depth-pass rule).
- **Draw:**
  - the `FMeshPassProcessor` constructor (RENDERER_API, `MeshPassProcessor.h:1550`);
  - the `Compute*` statics (`:1574-1576`);
  - `InitializeMeshMaterialData` at L84 (RENDERER_API, `MeshMaterialShader.h:29`);
  - `BuildMeshDrawCommands` at L85;
  - `DrawDynamicMeshPass` at L96/L207;
  - depth state at L54-55: `DepthNearOrEqual` and `DepthWrite_StencilNop`;
  - its own depth texture (L161) and a scene-depth-only uniform buffer (L159). Host custom depth is never bound.
- **No private symbols:**
  - Every include resolves to a 4.25 `Public` folder (checked file by file).
  - The only Renderer type touched is `FPrimitiveSceneInfo` (`Renderer/Public/PrimitiveSceneInfo.h:215`), and only by data member, reached through `FSceneInterface` virtuals.
  - P1's clean link confirms that no non-exported symbol is named.
  - `Build.cs:364` still adds the Renderer `Private` path, but only for the legacy path.
- **No custom-depth writes:** in private mode, `AnomalyMasks425.cpp:55/76/84/124/312` and `AnomalyStreetBench425.cpp:49` skip or return before every `SetRenderCustomDepth`, stencil-value, write-mask or `r.CustomDepth` call. `Tag()` only records the actor.
- **Spike shortcuts that will matter on Europa:**
  - only exact `UStaticMeshComponent` (L135, cast at L203);
  - one clear and one compute pass per object;
  - a linear search of all scene primitives per object (L192-199);
  - a new depth texture every frame;
  - a blocking flush and query wait on every captured frame (L238-240).

## 2. Gates, by my own checks
- **P1: PASS.**
  - Editor-01 (link probe): 0 LNK. Editor-02: 6 compile errors, 0 LNK. Editor-03: exit 0, 0 LNK. Game-01: exit 0, 0 LNK.
  - All ran BelowNormal with 2 actions. Binaries changed only while UBT ran.
- **P2: PASS.**
  - 0 `Missing cached shader map` lines in `warm\01`, and also 0 in `warmup\02` and `private-off\01`.
  - The two warm launches lacked `-IAIDepthOnlyByName`; the private run, which had it, also reads 0. Shader types register at module load, so this does not weaken the gate.
  - The can-fail is 094-42c's 53 lines.
- **P3: FAIL, reproduced exactly.** My grouped reader decodes the ASP1 byte-planar evidence (`p3review43a.py`). Reference\02 vs private-off\01: 2,352 pairs, 0 signature mismatches.
  - Floor: minimum IoU 0.99992 (27 px).
  - Hide, LOD_A and LOD_B, including forced LOD: 1.0.
  - Hidden blink: 54 + 78 frames, all non-empty, IoU 1.0.
  - Lighting group: 26 frames, 1.0.
  - All 731 event masks: IoU 1.0.
  - **Masked: 7 frames at IoU 0 (4,700 px). WPO: 12 frames at IoU 0 (4,962 px).**
- **Why the 19 frames fail** (`p3diag43a.py`):
  - Every failing pixel is in the private output only.
  - Scene depth is empty on 4,673 of 4,700 and 4,962 of 4,962 of them.
  - The next frame's legacy mask covers all of them, and 4,088–4,538 of those pixels change colour into that frame.
  - Example: `_reviews\094-43a-example-frame708-private-early.png`. The WPO panel is absent from 708's picture and present in 709. **The private pass labels the object one frame early.**
  - Codex's occlusion-lag mechanism fits, but it was not instrumented: cause not established.
- **P4: not run** (GC4 stop). No outline-on frame exists for this route.
- **P5: PASS.**
  - Private-off: 2,352 of 2,352 rows read on_count 0, tagged 0, unexpected 0. Depth-before equals depth-after (14 candidates, all off).
  - Receipt: `custom_depth_mode 1`, `private_depth_byname true`.
  - Three runtime echoes read `r.CustomDepth = "1"  LastSetBy: Constructor`.
  - `private-pass.jsonl`: objects equal resolved on all 2,352 rows. Its host-read and stencil-write fields are declarative, not measured.
- **Cost per captured frame** (my recompute; 7 objects, 7 draws):

  | Measure | Mean | p95 |
  |---|---:|---:|
  | GPU | 0.486 ms | 0.628 ms |
  | Render-thread submit | 0.213 ms | 0.333 ms |
  | Submit plus blocking wait | 2.239 ms | 2.557 ms (max 9.1 ms) |

  Game-thread cost was not measured.
- **GC6: committed `576975d`; 7 of 7 self-tests pass on my own `git archive` copy.**
  - The intent `{utc, pid, reason, root_running}` is persisted before the stop. If it cannot be saved, nothing is stopped.
  - An intent counts only for the same PID, while the root is running, inside the run's time window. So a post-crash cleanup intent leaves the crash a crash.
  - My run of the classifier on the real receipts: `reference\01` (exit 125, intent 19:00:30.859, root running) → owned-stop; `warmup\01` (exit 3) → crash; the other four → normal. No WinError 5.

## 3. Safety: one crash, nothing else
- **Crash and upload:**
  - 1 FI4 crash: `warmup\01`, the D3D11 typed-UAV global-shader fatal. No crash folder.
  - 1 owned stop (`reference\01`): the UBT ValidatePlatforms child exited before it could be read.
  - 0 upload lines in 12 logs. All 6 fixture launches used `-nowrite` and D3D11.
- **Paris and fixture:**
  - Paris Content (1,456), plugin Content (5), Config (18), Binaries (16 + 11), the descriptor and the `.uproject` are hash-equal before and after. `Saved` is unchanged. Paris was never launched or built.
  - The fixture's seven assets and the preserved `Engine.ini` are hash-equal.
- **Git:**
  - `m51` PASS at start, prelaunch and exit; `master`, `release`, 30 tags and the tag refs are unchanged.
  - Binaries changed only in builds, all under `E:\IA_Fixtures\Outline425`.
- **Deletions:** none by command. Three tracked 42c spike files left the fixture worktree when it switched branch; they are still in `740220c`.
- **Branches and push:**
  - The new spike `c1193bd` is local and is not an ancestor of feat. The old spike is unchanged at `740220c`.
  - Feat is at `75ca580` (no Source or Shaders change, no merges).
  - `ls-remote` shows origin feat at `d1cdc7b` and no spike branch. No push, no tags.
- **Foreign activity:** the owner was active about 00:06 (Task Manager, Snipping Tool). Admission waits stayed at 11 minutes or less, so GA5 never came near.

## 4. Decisions for chat
1. **The cheap route fails only on visibility timing.**
   - Offline on this run: if the pass counted shown objects only where its depth equals scene depth (the "drawn" bit it already computes), and kept the counterfactual test for hidden and missing targets, P3 passes on every object.
   - Masked and WPO reach 1.0; the hidden targets (119 + 208 frames) are 1.0 and non-empty; the minimum IoU is 0.99992 (`p3sim_drawn43a_v2.py`).
   - This is one fixture run, not a proof. Rule whether a spike re-test of that rule (then P4) is worth one more night, or whether to take GC3's recompile fallback.
2. **FZ9** (`_reviews\094-43a-fz9-findings.md`): no 5.1 office or client dataset can be judged from home records. The only recorded 5.1 reader is Lyra's outline material, on home bench data only.
