# 091-03 — Annotation transitions and capture-time temporal evidence

Goal: exclude settling frames in the client's session annotation, cover every existing transition reason and
detect temporal AA/upscaling on the actual captured view. Implement in fix/m52-label-timing and merge forward to
m53 and proxy. No push until the in-engine proof and deferred 091-02 section 0 pass.

Work and predeclared predictions: `E:\IA_BuildCache\_r913`. Original 090-12/091-02 harnesses remain unchanged.
The old clean fix checkout was detached at 8b86f43; the branch now has its worktree under `_r913/fix_src`.

## Source findings and implementation

The old annotation writer omitted all transition fields. Schema 2.1 retains major `label_schema: 2`, adds
`label_schema_minor: 1`, event identity, sorted unique event `transition_frames`, entry counts by reason and the
root union count. Live, detached and synchronous transition entries use the same emission and reason helpers
as the label writer. Exclusion-only events remain even when the event's positive membership was vetoed.

Exact engine: `D:\UESource\UnrealEngine\Engine\Build\Build.version` = 5.1.1, compatible changelist 23058290.
Source locations below are relative to `Engine/Source/Runtime`:

| Source | Engine evidence | Capture handling |
|---|---|---|
| Desktop TAA/TSR | Engine/Private/SceneUtils.cpp:41, SceneView.cpp:952 | Read the rendered view's method on every captured SVE frame; methods 2 and 4 are temporal. |
| Third-party temporal upscaler | Renderer/Private/SceneRendering.cpp:2552 forces TAA when forking the family upscaler; PostProcess/TemporalAA.cpp:1002 selects the registered interface when enabled | Read family upscaler presence after rendering setup at the same postprocess callback that copies color. Conservatively temporal even if the upscaler setting subsequently disables use. |
| Mobile | SceneUtils.cpp:45 reads r.Mobile.AntiAliasing; mobile HDR changes supported methods; PostProcess/PostProcessing.cpp:1922,2264 uses the mobile view and upscaler | Capture-view evidence covers the mobile choice; defaults re-read through the engine feature-level API. Missing capture evidence is temporal. |
| Forward/deferred MSAA | SceneUtils.cpp:83 checks forward/mobile-deferred support and MSAA count; unsupported TSR falls back to TAA | Actual view TAA is temporal; known non-temporal methods remain non-temporal absent other evidence. |
| Per-view overrides | SceneView.cpp:952 applies show flags, realtime support and view state; SceneRendering.cpp:2770 can disable AA later | Observe final postprocess view, after family extension and renderer overrides, instead of relying on the startup default. |
| Runtime changes | Engine/Private/GameUserSettings.cpp:429,466,857 and Scalability.cpp:466 set quality groups/CVars; subsequent views recompute their method | Re-read defaults while sampling and completing frames; inspect each capture receipt. Once temporal, retain protection to run end so pending tails survive a later change to off. |
| Unknown | Missing family/receipt or unknown method is not proof of AA off | Unknown evidence is temporal. Backbuffer/sync paths have no authoritative view receipt and fail safe. |

Stock SetAntiAliasingQuality changes the quality group, not necessarily the algorithm; a host scalability section
or game can write method CVars at runtime. AA history detection is distinct from unrelated GI/denoiser histories.
The API boundary is corroborated by Epic's [FSceneView documentation](https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Runtime/Engine/FSceneView)
and [temporal-upscaler interface](https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Runtime/Renderer/ITemporalUpscaler);
the version-specific conclusions above come from the local 5.1.1 source, not current web documentation.

`label_temporal_source` records observed view-method bits, third-party upscaler, unknown evidence, latest engine
default/feature level and retained temporal state. UV/normal and other fire-window types now have temporal onset
and offset tracking; their earlier protection covered only one PIE settle frame. Defaults stay 3/16/1 pending
pixel measurement; no measured tail length is claimed from source.

## Validation and outstanding proof

File comparison covers sorted/exact event frames, each reason count, root union, distinct event identity and
missing events. Doctored missing/extra/duplicate/shuffled/wrong-event frames, reasons, count and schema fail.
Verifier checks run automatically for the new schema; legacy schema remains readable. Office kit 1.5 refuses
a new-schema transition disagreement. Final build results, engine proof tables, archives and decisions are in
the job report; this journal will be updated at close.

Initial idle gate refused: foreign Blender approximately 96% GPU, human idle 232 seconds. No engine launch.
The copied Windows GPU query had doubled escapes; only this job's copy was corrected, with the same thresholds.
First normal build exited 0xC00000FD with `Stack overflow.` before a compiler diagnostic; a retry is retained
separately. No security setting or foreign process was touched.

The R4 source gate initially failed its literal requirement that hide transitions run before sample geometry is
frozen. Capture-time AA requires resolving hide transitions at completion instead. The revised gate still requires
geometry frozen at the tick-end sample, adds both transition functions to the completion-time live-read ban, and
requires detached tails to use stored sampled geometry. Two new live-geometry mutants must fail. This is a declared
post-failure gate adaptation to the implementation's ordering, not a relaxed pixel/edge/mask acceptance threshold.

Full report destination:
`D:\IntrusiveAnomalies\_relay\runs\2026-10-03-091-03-annotation-transitions\report.md`.
