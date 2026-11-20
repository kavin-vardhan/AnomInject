<!-- Collected by Claude Code from runs\2026-10-16-094-01-europa-phase-a-plan\report.md (thread 01a14334, gpt-6-astra, max, danger-full-access resume, exit 0, 2414 s, 11:11:05-11:51:20 IST). Body below is byte-identical to report.md, sha256 927681ADEADAADDA2441C61FFE00B94B389069ED0898D3771B57F07BD7E80CAA. -->
094-42r: the guard/routes work, both Paris builds and the EA_Core regression pass. The separate fixture builds, but its authoring session hit FF2's Saved-config stop before the recipe ran. Repro, guard and shader-cost measurements remain incomplete.

Step 2 was rechecked at `4fdfe84`: native reader/writer/unknown admission, default and explicit CSV routes, invalid-route negatives, refusal-before-tagging, fixture scope, zero-weight reader parity, and the launch macro tests all pass. The code restricts planted states to recognized `-IAIBench` fixtures and their `EA_` actors. C4/C5 refuse reader or unknown before tagging without disabling the effect. Defaults are blinking plus LOD; lighting and missing-object require explicit selection. Routes are logged and receipted. The port follows the 5.1 scanner at `d7eee97`, `AnomalyCaptureSubsystem.cpp:3179–3313`; 4.25 uses `GetCameraCachePOV` (`PlayerCameraManager.h:312–313`).

Builds used below-normal priority, two parallel actions and FM1's 8/3 GiB commit lines. No memory stop occurred.

| Target | Attempt | Result | Seconds |
|---|---:|---|---:|
| Paris Editor Development | 1 | PASS, zero errors | 162.6 |
| Paris Game Development | 1 | PASS, zero errors | 106.3 |
| Fixture Editor Development | 1 | FAIL, six cascaded diagnostics | 154.7 |
| Fixture Editor Development | 2 | PASS | 4.8 |
| Fixture Game Development | 1 | PASS | 139.5 |

The fixture-only compile defect was `R.ToSharedRef()` on a value already returned as `TSharedRef`. Serializing `R` directly fixes it; the failed compile and successful rebuild provide the negative and positive evidence. Paris source remains qualified at `4fdfe84`.

EA_Core slot 01 passes every required gate, with explicit four-route selection, seed 4242 and the fixture-only 0.5% selection floor. It writes 2,352 paired 1280×720 frames, format 18/D3D11, zero drops and a normal exit.

| Anomaly | Fires | Affected frames with masks |
|---|---:|---:|
| blinking | 22 | 132 |
| lighting_mismatch | 13 | 157 |
| lod_corruption | 19 | 247 |
| missing_object | 15 | 195 |

All 731 affected frames have masks; all 1,621 clean frames have none. Every LOD event changes target pixels: 14,455–14,475 changed pixels across its 247 frames. Schema 2.1, affected/injected sets, far plane and coverage values pass. Exact restore of 14 components and 2,352 mid-session samples pass; reserved 203 is never allocated and host custom-depth states stay untouched. Player-view agreement, eight exact UI-marker pairs and marker absence in every captured frame pass. Census exhaustion, lost, no-pass and polluted counts are zero. The street-only census-over-55 and wall-strip gates are waived by the EA_Core profile. Scanner result is `none`, no bypass, zero tags before/after its scan. [Full smoke receipt](D:/IA_Europa/Receipts/09442/c5-attempts/C5/01/reader.json).

Fixture: `E:/IA_Fixtures/Outline425`, with a detached plugin worktree at `4fdfe84`. The kept authoring script prepares the required meshes, two multi-LOD assets, movable light, player start and CustomDepth outline material. Its Python 2.7 harness and keyword-Rotator positive/negative checks pass. Script SHA-256: `6e4174a1886925076c352f32578d0e7cd4c10df7c17aa055c2be9f6fbd44891c`.

Author slot 01 started at 11:39:46 IST. At 11:39:51 IST, Unreal created `E:/IA_Fixtures/Outline425/Saved/Config/Windows/Engine.ini` (1,502 bytes). The authoring argv omitted `-nowrite`. FF2/FG2 therefore stopped the owned editor and adopted monitor, exit 125. The recipe never ran and zero declared assets exist. The generated file and terminal result are preserved; no retry was launched. Repro pixel differences, reader-refusal runtime proof and shader-cost numbers are **not measured**. The next authorized authoring run needs the missing switch and a fresh qualified slot. [Stop receipt](E:/IA_Fixtures/Outline425/Receipts/09442/author/01/result.json).

FZ6(a), source-only: existing desktop depth permutations can support a private depth/stencil target. Opaque position-only geometry uses `TDepthOnlyVS<true>` with the default surface material; masked/PDO uses the original material, `TDepthOnlyVS<false>` and `FDepthOnlyPS<false>`; WPO retains the original material/full vertex stream, with hull/domain stages when tessellated. Sources: installed 4.25 `Renderer/Private/DepthRendering.cpp:96–169,973–998` and `Shaders/Private/DepthOnlyVertexShader.usf:119–145`.

Lookup can use exported `FindShaderTypeByName`, the material's vertex-factory mesh shader map and `GetShader(type,0)` (`RenderCore/Public/Shader.h:1765–1771,2195`; `Engine/Public/MaterialShared.h:1193,1764`). Bind material/pass/view and vertex-factory/primitive data (`Renderer/Private/ShaderBaseClasses.cpp:565–631`), preserving pipeline fallback and material permutations. Allocate a private depth-stencil target with shader-read in TargetableFlags (`Renderer/Private/PostProcess/SceneRenderTargets.cpp:3113–3142`; `RenderCore/Public/RendererInterface.h:109–110`). Renderer-private depth helpers are not DLL-exported; a plugin processor must reproduce their selection through accessible APIs. This is feasibility evidence, not a qualified private pass. [Full citations and limitations](D:/IA_Europa/Receipts/09442/shader-route-source.md). FZ6(b) was not run; no spike branch was created, merged or pushed.

No crash or upload sign occurred. The fixture's generated INI is the protected-file violation; the Paris Content, Config, descriptors, project and post-build binaries remain unchanged. Entry/exit m51 and protected refs pass; all 27 rebound files match. No manual deletion during this resume, new archive, push or tag. Full binary hashes are retained in `D:/IA_Europa/Receipts/09442/c5-425-binaries-01.json` and the fixture build result files under `E:/IA_Fixtures/Outline425/Receipts/09442/builds/`.

Final commit: `f7b32f0c0b13ea963f0c116b7b23479ae706d463` on `feat/europa-port`; worktree clean. W4 closeout, receipts and append-only gotcha G516 are committed. The seven preceding local implementation commits remain unpushed; origin stays at `d1cdc7b`.

VERDICT: NEEDS-DECISION | EA_Core passes; fixture authoring hit FF2 before the recipe, leaving repro/guard and shader-cost measurements unrun.
