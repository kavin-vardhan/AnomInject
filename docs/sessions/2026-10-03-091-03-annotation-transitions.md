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

## Provisional stop before the ruling: NEEDS-DECISION, no push

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

## Resumed after the orchestrator's 15:05 IST ruling

Before the final report was delivered, `office-reads.txt` directed the implementer to `orchestrator-ruling-1.txt`.
The ruling defines AA-off as zero temporal-AA reasons and zero temporal windows, preserving every independent
transition reason. PIE proxy retains one `pie_end_settle` per fire; staged retains none absent other reasons.
This supersedes the acceptance conflict and provisional stop above. Engine proof resumed behind the idle gate.

The complete retained TSR consumer fixture (1728 hash-verified image copies, not a new candidate capture) exposed
two reader gaps: the kit rejected proxy-route PIE flags, and dropping the permitted first post-label frame broke
its temporal-tail continuity check. Recorded proxy route now permits the existing PIE exclusion. Only a validated
first post-label PIE exclusion bridges continuity; it remains outside AA excuses and references. Decay is checked
at the next measured tail frame against the last labelled frame. Generic and proxy controls cover flagged decay,
a missing later flag and an undecayed tail. Hold-route/type/placement/unpaired guards remain active. No pixel
threshold changed. The expanded kit passes 93 checks with identical results from both decoders. The first
87-check run failed an aggregate expected-count assertion after three fixtures were added; all per-event cases
passed. The count was corrected, then the six temporal/PIE cases added. All failure logs remain in `_r913/logs`.

The first final proxy TSR PIE leg launched only after the gate passed (no foreign editor/game/heavy GPU process,
human idle 130.5 s, headroom 33.3 GiB). It captured 8 fires, 384 labelled frames with zero missing measured masks,
and exact labels/annotation equality on 152 transition frames. Captured view-method bits 0x10, upscaler 0,
unknown 0, default 4. Tail coverage still requires the independent null capture; this is not a completed proof.
The editor exited 0. A redundant same-content DLL restoration copy returned errno 13 immediately after exit;
all five live DLL hashes exactly matched the prior receipt, so restoration was verified without rewriting them.
The receipt-owned temporary profile was removed. The local harness now saves capture results before restoration,
avoids redundant copies and always attempts receipt-owned profile cleanup. No security setting changed; no
foreign process was touched. No security-block event was found in the read-only event query.

## Strict-tail measurements and revised candidate, before final rebuild

The original 16-window candidate is an actual in-engine can-fail: strict target RGB MAE above an independent
clean-to-clean null leaves 164 unflagged frames across eight proxy TSR PIE fires (maximum +44), 173 across seven
staged fires (+46), 144 across eight UV TSR PIE fires (+45), and one across eight normal TSR PIE fires (+18).
Annotation and labels agree exactly; coverage is the failure. The office kit still passes the proxy TSR leg 8/8
at its historical half-strength 0/0, illustrating why that gate alone cannot establish the new requirement.

Null calibration was revised openly after failures: median reference, a second identity null, a pool-disabled
null, raw reference, then maximum pairwise RGB MAE over fixed first-128 post-warmup samples from independent clean
captures. Reference index 192, fixed target region, no numeric floor or multiplier in the tail gate. Every
preceding failed result remains in `_r913`. Held-out clean frames and live pre-fire windows must validate the
bound. TAA PIE with 192 post frames still had a decaying excess at +190; a three-fire 512-post diagnostic found
the last excess at +203, with later pre-fire windows clean. This is a finite measurement, not a persistent-floor
claim. A single staged TAA null failed holdout validation (0.756620 versus 0.756451); an additional clean run
after 600 deferred engine frames is planned under the same algorithm to test startup convergence. It remains
unjudged until validated. These calibrations are measurement changes, not silently green results.

The MainWorld hold leg was not judgeable under this reference method: null 8.5708 and insufficient visible change;
the old E1 reader censored every end. Restricting a diagnostic to the measured target silhouette did not remove
the clean variation. A copied static fixture now gives the target its own duplicated detailed texture/material,
so the actual hold route is eligible. Original assets are hashed unchanged. Cook output, staging scratch and
logs are isolated under `_r913`; fixture creation and restoration receipts are retained.

The next candidate uses offset 256 and cap 256, rounding the reproducible 203 measurement upward with margin.
Onset 3, hide 1, and confirmed-AA-off 0/0/0 are unchanged. Two existing CVar help strings are corrected for this
value and capture-time detection; this is a disclosed exception to fields/log-only string additions, with no new
CVar. The pure suite passes 315 checks; short-default and short-cap mutants join the four detection mutants.
Review also found the new generic fire-window registry was not carried across captures. It now transfers its
rebased track and frozen geometry like the held-window history. A capture-boundary diagnostic and an AA 0->4->0
diagnostic are required before final close. Final builds, final-source proof and pushes remain pending.

One harness waiter read the intermediate CAPTURED record before FILE-CHECKS-PASS and stopped before launching
authoring. Waiters now wait through that state and owned JSON receipts publish atomically. The waiting cook
helper was restarted by its recorded PID to isolate cooked/staged paths; no engine process or foreign PID was killed.

## 091-03 ruling 2: unchanged E1 gate and measured t10 coverage

The orchestrator's 18:05 IST ruling supersedes the strict-null acceptance above. The unchanged 084-09 E1 t50
reader remains the release gate; the window must additionally cover its t10 diagnostic plus a small margin.
Initial 16-window measurements: proxy PIE TSR t10 +22..26 / TAA +36, staged TSR +23..24 / TAA +36; all t50
events pass with zero unexcused frames. UV TSR PIE t10 +5..6 and normal +1..2, both t50 0/0. This justifies a
40-frame common offset: maximum 36 plus four. The common value also safely covers a run that changes AA family;
the existing temporal state deliberately retains evidence across such changes. The cap returns to 64.
The provisional 256 build is retained but superseded; no source or proof gate silently treats it as delivered.
Residuals above null but below t10 are diagnostics only, including staged TAA's repeated very small +507 excess.
All failed/revised calibration records remain. The final matrix and static hold measurement are pending and
must cover t10 within 40; an overrun requires another measured correction, never acceptance by annotation alone.

Cause analysis uses a separately hashed temporary observer build: existing render-resource sampling at the
capture's SVE callback, source texture resident mips/first mip/resource identity, no streaming state writes.
It is gated by -IAIBench plus its diagnostic command-line flag and excluded from Shipping. The patch stays in
the evidence folder and is restored by byte receipt, not merged into release. Matched AA-off captures and the
unchanged E1 reader distinguish temporal tails from source re-streaming; no residency behavior change is authorized.

## Final defining-header review

The 40-window builds c43f538/f9fac4e/96e7a32 passed all normal builds and proxy strict builds at 0/0,
with 68 independent header units per strict target. Lever audits 34/52/53 pass; string review has zero
unresolved entries and all 11 current snapshots are archived/re-hashed. A final direct-header review adds
SceneUtils.h for EAntiAliasingMethod and the upscaler definition directly: TemporalUpscaler.h on 5.7,
PostProcess/TemporalAA.h on 5.1 via __has_include. The prior opaque pointer comparison compiled, but this
keeps the brief's explicit include discipline. No behavior or window changes. New final builds/proof are
required. Waiting launch helpers were stopped by verified owned PID; no engine or foreign process was killed.


## Closing state after the disk-floor refusal: NEEDS-DECISION, no push

Final Source builds are fix1413a73 (EXE3D437AB3), m53e874e28 (75A9553C), proxy58386be (C7FB5212).
All normal Editor/Game builds passed 0 errors / 0 warnings. Proxy strict Editor/Game passed 0/0 with 68
independent header units each; strict settings were restored and normal targets rebuilt 0/0. Lever audits
34/52/53 pass. Metadata-aware ASCII/UTF-16 review has zero unresolved strings. Fourteen snapshots (84 binary
entries), including superseded candidates and the separate observer, are archived and re-hashed including ZIP
members. The inventory records 237 exe/DLL copies and 117 distinct hashes. Full hashes are in the report/ledger.
The two changed existing CVar help strings are an explicit fields/log-only string-scan exception; no new CVar.

Completed initial-16-source pixel evidence from the unchanged 084-09 E1 reader:

| Leg | Fires | t50 tail | t10 tail | t50 unflagged | t10 unflagged |
|---|---:|---:|---:|---:|---:|
| Proxy TSR PIE | 8 | 7..8 | 22..26 | 0 | 61 |
| Proxy TSR staged | 7 | 7..8 | 23..24 | 0 | 52 |
| Proxy TAA PIE | 8 | 15 | 36 | 0 | 160 |
| Proxy TAA staged | 7 | 11 | 36 | 0 | 140 |
| UV TSR PIE | 8 | 0 | 5..6 | 0 | 0 |
| Normal TSR PIE | 8 | 0 | 1..2 | 0 | 0 |

All those capture files agree between annotation and labels; their labelled frames have measured masks.
The short-window failures are retained can-fails, not passes of the new t10 requirement. The current 40 has
not yet been measured in-engine. MainWorld hold was unjudgeable; the copied static unique-texture hold fixture
was authored/cooked but not captured. Original fixture hashes remain unchanged. Above-null diagnostic lengths
are TSR proxy44/46, UV45, normal18, TAA PIE203; staged TAA recurs through507 in a512-post run. The staged clean
validation initially failed; the additional600-deferred clean run and all failed calibrations remain recorded.

The passive observer (temporary patch SHA dbfce7d56316aa85307c4273f9ddad07bdbc5d205afec33e67bf1a1b97fccbe4,
EXE81ED86AB) completed proxy PIE TSR/TAA/off, three fires each. Every sampled original-source record retains
eight resident mips, first mip1 and the same resource identity within its leg. E1 t10 tails are24..26,36,0.
AA off passes strict/t10/t50 at exact0/0, with zero temporal reasons/windows and three independent PIE settle
entries. A one-frame shift makes unchanged E1 fail3/3, six unexcused frames. This supports history rather than
source re-streaming for those proxy PIE legs only. No residency change was implemented. Observer code is not
merged; its source was restored before the final production build.

The resumed UV diagnostic gate stopped without launching at20:07 IST: D:38.9 GiB, memory headroom0.9 GiB,
human idle795.4s, foreign UE5.7 editor PID110408. D: remains below the50 GiB floor. All remaining launch/reader
waiters were stopped by verified own PID, with receipts. No foreign PID or security setting was changed.
Final proxy TSR/TAA/off PIE/staged, final UV/normal and static hold, m53 section0, AA0->4->0 and run-boundary
carry controls are NOT RUN. No new final-source pixel result or release-readiness claim is made; nothing pushed.

The final queue was prepared before launch with eight attempts for seven-fire legs, four staged attempts for
three-fire legs (staged warm-up previously consumed one), and512 post frames on TAA to retain below-t10
diagnostics. The unchanged E1 gate and40 declaration were not relaxed. The final reader plans are saved but
have produced no final results. Resume requires a new disk/idle gate and explicit handling of the retained
NOT-QUIET record; never overwrite the failed receipt.

Office reads were rechecked:171 entries, off16, no EFFECTIVE line supplied. Those totals establish temporal
detection and flag emission, not the identity of the owner's two pixels/frames. All original staged files and
the temporary device profile were restored by receipt; the host now has the final normal proxy binaries and
clean Source. Harness originals and original fixture assets are unchanged. Settings, owned launches, transient
restoration errors, gate adaptations, archives and all unpushed commits are enumerated in the full report.

## Required-first resume: repeated startup failure, RED, no push

The 20:49 resume accepted the schema, detection, 3/40/1 policy, office-kit corrections and proxy PIE history
cause evidence. It attributed the earlier D: dip to the owner's growing page file; that attribution comes
from the reviewer, not a measurement by this job. D: was now 60.4 GiB free and E: 345.1 GiB.
Optional residency, AA-switch and carry controls were removed from the critical path. A new, predeclared
sixteen-leg queue ordered six final proxy TSR/TAA/off PIE/staged legs first, then proxy UV/normal TSR and
m53 UV/normal section-0 legs in both modes, then static Hold0913 TSR PIE/staged. Previous queues and refusal
records were retained. No source, build, window or pixel threshold changed.

| Attempt | Start / exit IST | Own engine PID | Gate: person idle / commit headroom | Engine result |
|---|---|---:|---|---|
| F_P_proxy_tsr_58386be | 20:58:11 / 20:58:39 | 102896 | 2580.0 s / 16.6 GiB | exit 3 before capture |
| F_P_proxy_tsr_58386be_r1 | 21:02:06 / 21:02:29 | 106640 | 2815.6 s / 15.5 GiB | exit 3 before capture |

Both launches passed three consecutive fresh samples: no foreign game/build/GPU-heavy process, GPU below
10%, disk floors above 50 GiB. Foreign editor PID70836 was idle and untouched. Both used the exact final
58386be editor DLLs, whose hashes match the archived C7FB5212 build, identical map/AA/capture settings and
no window override. The first failed at D3D12 initial Present with DXGI_ERROR_DEVICE_REMOVED and Aftermath
Timeout. One unchanged retry was declared before launch; it failed identically. Both processes exited
themselves. Neither produced a capture session or anomaly fire. These are failed startup attempts, not E1
passes or zero-tail measurements. All sixteen required pixel results and section 0 remain incomplete.

The engine logs report 1834.89 and 737.48 MiB physical memory free at exit; the gate measures commit
headroom, a different quantity. The device-loss cause is not established. No driver/TDR/render-path/security
change, foreign process action, or further engine launch followed the repeated failure. The wrapper returned
zero but the engine exit receipt recorded 3; the missing-capture check correctly stopped each queue.

Both receipt-owned temporary profiles were removed; five editor DLLs were hash-restored to the final normal
baseline. No staged leg ran. The six staged originals, original harnesses and fixture assets are rechecked at
close. Archives and binary inventory are re-hashed; ten new backup copies introduce no new executable hash.
The earlier report is retained as report-before-resume.md. Full failed launch receipts, logs, gate readings,
queue plans and errors live under E:\IA_BuildCache\_r913. The typed office reads are unchanged (171 entries,
off16, no EFFECTIVE line). Only this documentation is committed and merged forward; nothing is pushed.

Release outcome: RED because required final-source engine proof cannot start. No schema/window policy
decision is newly needed. Resolve the repeated startup failure in a safe bench window, then execute required
items 1-3 in the resume order; optional diagnostics must not block delivery. Preserve both failed attempts.


## Resume 2 completed: required final proof passes, GREEN

The reviewer ruled the two first-Present failures environmental VRAM/device contention with the foreign editor, not a plugin defect. This is the reviewer's attribution. No product source changed. The new job gate forbids any foreign UnrealEditor at every sample and immediately before launch. All final legs passed it; no device removal recurred. A replay of the retained old foreign-editor gate is rejected by the actual new quiet function, and a mutant removing that condition is detected. Predictions preceded the launches.

| Build / anomaly | Mode / AA | Fires | t50 tail | t10 tail | Exported tail | Labelled / missing masks |
|---|---|---:|---:|---:|---|---|
| 58386be / proxy | pie / TSR | 8 | 7..8 | 23..25 | 40 | 384 / 0 |
| 58386be / proxy | pie / TAA | 8 | 15 | 37 | 40 | 384 / 0 |
| 58386be / proxy | pie / off | 8 | 0 | 0 | 1 (independent PIE settle only) | 384 / 0 |
| 58386be / proxy | staged / TSR | 7 | 7..8 | 23..24 | 40 | 336 / 0 |
| 58386be / proxy | staged / TAA | 7 | 11 | 36 | 40 | 336 / 0 |
| 58386be / proxy | staged / off | 7 | 0 | 0 | 0 (no transitions) | 336 / 0 |
| 58386be / uv | pie / TSR | 3 | 0 | 4..5 | 40 | 144 / 0 |
| 58386be / normal | pie / TSR | 3 | 0 | 1..2 | 40 | 144 / 0 |
| 58386be / uv | staged / TSR | 4 | 0 | 3 | 40 | 192 / 0 |
| 58386be / normal | staged / TSR | 4 | 0 | 0 | 40 | 192 / 0 |
| e874e28 / uv | pie / off | 3 | 0 | 0 | 1 (independent PIE settle only) | 144 / 0 |
| e874e28 / normal | pie / off | 3 | 0 | 0 | 1 (independent PIE settle only) | 144 / 0 |
| e874e28 / uv | staged / off | 4 | 0 | 0 | 0 (no transitions) | 192 / 0 |
| e874e28 / normal | staged / off | 4 | 0 | 0 | 0 (no transitions) | 192 / 0 |
| 58386be / hold_static | pie / TSR | 3 | 7..8 | 24..25 | 40 | 165 / 0 |
| 58386be / hold_static | staged / TSR | 4 | 7..8 | 23..24 | 40 | 196 / 0 |

Every row passes E1 t50 with unchanged pixel thresholds and zero unflagged frames, covers all measured t10 tail frames in both files, has exact annotation/labels equality, and has no labelled frame without a measured mask. The AA-off UV/normal reader compatibility correction is described below. Shared proxy co-users: 20 per event, zero changes. These rows also close deferred 091-02 section 0 on m53 and proxy. AA off has zero temporal windows/reasons and raw strict/t10/t50 edges 0/0; independent PIE-settle rows remain exported per ruling 1.

The selected window stays40. Initial t10 maximum36 selected four spare frames; final TAA PIE measured37, one above that prediction, leaving three spare frames. Final maximum across required legs is37, static hold25. No threshold/window was retuned after these results. Below-t10 E1 strict residuals remain diagnostics; full per-event results and prior independent-null failures/measurements are retained.

Final-source office kit on TSR PIE judged8/8 with start/end0 and zero failures/wrong objects. Per-session verifier/annotation/mask checks pass on all16 required legs. Earlier selftests, compiled mutants, missing-mask, wrong-object, old-window, old-export and archive/string can-fails remain applicable because product Source and tools did not change during this resume. Normal/strict builds, include audit, lever audit and string scan remain the recorded final-source passes.

Reader compatibility failure retained: raw084-09 UV diff rejected the first m53 AA-off PIE capture solely for its three valid pie_end_settle flags, despite raw strict/t10/t50 edges0/0 and zero pixel mismatches. This contradicts the already accepted ruling1. The AA-off UV/normal confirmation path now uses an exact copied existing090-10e successor; its13 tests pass, including negative controls and400 random no-PIE parity cases. For every affected final capture, raw084-09 and090-10e have identical visible-frame sets, depths and edges, with raw0/0. Original FAIL files remain. All proxy, temporal UV/normal and hold paths retain084-09; no pixel threshold or production source changed. The summary-level PIE validity guard is also honored.

Harness repairs are disclosed and retained: the shared reader initially received a missing default receipt; it was then given the existing hashed detail-author.json, with unchanged ROI/thresholds and no capture rerun. The status-only aggregator counted list-valued unexcused frames by length. The staging helper expects EXE SHA8, so its source-commit argument was replaced by the selected build's existing SHA8 alias, retaining full hash verification; the unlaunched failed attempt was restored and retained under its original label. The first completed staged capture's EXE restore returned errno13 at+2s; no game/editor remained or matching Defender block was reported, and the same ordinary copy later restored all six originals. Subsequent staged post-exit delay is10s; no ACL/security change or permission bypass. Saved captures were reused and all failures kept.

Required work completed in resume1 priority order. Optional remaining residency, runtime-switch and capture-boundary engine controls are deferred under the reviewer's scope; no extra diagnostic launches were added. Both original startup failures and earlier reports remain as superseded evidence. Profile/staging/editor/fixture restoration, archives and remotes are rechecked at close. Documentation changes are merged fix -> m53 -> proxy, with exact proved Source identity, before the authorized fast-forward-only push. The final report lists the exact resulting heads and push receipt. No outstanding product/schema/window decision.
