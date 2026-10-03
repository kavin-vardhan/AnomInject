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

Typed office readings arrived in the run folder: 171 transition entries, off-window 16. This confirms temporal
detection in that run; aggregate counts alone do not prove which particular two frames were flagged.

**Acceptance conflict:** retained home AA-off PIE proxy session
`D:\IA_BankOverflow\_r912\out\P_e1_663a99b\session_20261002-204552` reports temporal false, off-window 0,
8 transition entries and 8 PIE settle frames. Thus exporting all existing reasons exactly and requiring an empty
AA-off PIE `transition_frames` cannot both hold while preserving behavior. The implementation preserves exclusions.
No gate was relaxed to call this green. Required decision: accept zero *temporal-AA* transitions for AA-off while
retaining independent uncertainty reasons, or explicitly authorize a separate change to the PIE settle policy.

A retained proxy TSR capture also exposed a pre-existing verifier omission: its reason whitelist did not permit
PIE settle on a proxy-routed stuck_low_mip event. The reader now uses recorded route evidence to distinguish the
proxy fire-window policy from hold; a hold-route mutation must still fail. This does not change pixel thresholds.

## Close: NEEDS-DECISION, no push

Normal Editor/Game builds passed with 0 errors and 0 warnings for final Source commits fix `1df4d26`, m53 `3f4f1c7`
and proxy `8131578`. Proxy strict Editor/Game also passed 0/0 with 68 independent header TUs per target; its normal
configuration was restored and rebuilt 0/0. Exe SHA prefixes are `7B943716`, `2468CB8D`, `516CBFDF`. The preliminary
fix `e151208` / `479BA61E` build was superseded by completion-time hide tracking and is archived separately.

Two build failures remain on record: the initial fix UBT process exited 0xC00000FD with stack overflow before a
compiler diagnostic; the initial m53 Game build failed with C1002, compiler out of heap space in pass 2. Unchanged
retries passed. No compiler setting, security setting or foreign process was changed to obtain those passes.

Annotation tests: 5 pass, including 10 doctored variants. Verifier unit tests: 44; label-rule selftest: 65; office
kit: 84; temporal/window C++ checks: 313 plus four actual failing mutants; R4 source guard: 12 including the new
completion live-geometry mutants. Lever selftest: 12; branch source/binary audits: fix 34, m53 52, proxy 53, no failures.
A retained TSR capture was copied into a synthetic schema-2.1 consumer fixture: eight events, 152 unique transition
frames. New-schema equality and verifier pass; missing-tail, wrong-reason and wrong-total mutations fail both and
are refused by the kit. Its positive kit result is only past the schema guard: the JSON-only fixture is then refused
for no frames. It is not a candidate engine capture or a pixel proof. Old annotation fails the strict new checker.

ASCII/UTF-16 scan versus each 091-02 baseline found only intended field/evidence text after reviewing exported-symbol,
debug and FH4 unwind records and matching unchanged constant data across PE pointer relocations. An appended foreign
semantic-string mutant fails. All six binaries in each of four successful snapshots (including the superseded fix)
were copied into archives, re-hashed and checked again inside ZIPs. Full hashes and paths are in `_r913/archives.json`
and the report. Scratch PE review helpers do not ship in the plugin.

The final read-only idle gate refused after 40.6 seconds: foreign UE 5.7 editor PID 71812, GPU up to 15.31%, human
idle 30 seconds, commit headroom 11.8 GiB. Initial/mid-run refusals are retained too. The job launched no editor/game,
changed no bench CVar, and made no staging/profile/fixture writes. Our harness copy additionally refuses missing idle
or memory measurements. Originals are unchanged. Final Source trees are preserved in the local merged heads.

Required proxy TSR/TAA/AA-off PIE/staged pixel legs, hold and UV/normal TSR tail legs, and deferred 091-02 section 0
are all NOT RUN. Measured tails, zero unflagged tails and zero labelled frames without masks cannot be asserted.
The declared off-window remains 16; no pixel threshold or transition window was retuned. The acceptance conflict
above and the refused idle gate prevent a green release. No feature branch was pushed, as the brief requires proof
first. Resolve the AA-off criterion and resume in a quiet bench window; rerun proof on these actual Source trees.

Full report destination:
`D:\IntrusiveAnomalies\_relay\runs\2026-10-03-091-03-annotation-transitions\report.md`.
