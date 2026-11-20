# 093-08: targeted object admission and ordered J2 qualification

## Design frozen before product changes

L2-L6 authorize this continuation on `fix/followup-capture`, from local
`9514242` (product `f9b9328`). Remote remains `1bf7054` until all gates pass.
Worktree: `E:/IA_BuildCache/_r933/AnomalyInjector`; new evidence: `_r938`.
The preserved 093-06/093-07 reports and `_r936`/`_r937` captures are controls,
not qualification of this candidate. No natural-cause or delivery work.

All externally reachable object applications converge on
`UAnomalyInjectorSubsystem::ApplyAnomaly`, immediately before the registered
anomaly's `Apply` can mutate the world. The admission belongs there:

| Entry path | Existing on-screen protection | New common admission |
|---|---|---|
| Capture `BeginFire -> TryFireSpecific -> ApplyAnomaly` | None for the named target; later mask veto only | Required |
| Automatic `TryFireOnce -> ApplyAnomaly` | Visible/census candidate selection, potentially stale | Re-check at application |
| Selector `InjectSelected -> ApplyAnomaly`, including selector UI/key/console | Cached visible selection, no fresh application check | Required |
| Console `IAI.Apply -> ApplyAnomaly` | None | Required |
| Dashboard control-server `inject -> ApplyAnomaly` | None | Required |
| Public C++ `ApplyAnomaly` caller | Caller-dependent | Required |
| Capture `ApplySessionGlobals -> ApplyAnomaly` | Global anomaly eligibility | Retained; not an object fire |

`StuckLowMip`'s internal proxy `Apply` and the bench `ChangeCase`'s internal
material application implement an already admitted parent effect, including
its deferred onset. They are not independent named-target fire entry points.
Their ongoing timing and mask veto are unchanged.

Object scope covers missing_object, blinking, lod_corruption, lod_popping,
missing_texture, corrupted_texture, stuck_low_mip, uv_corruption,
normal_corruption, and bench null_effect/solid_swap. The authored global
time_dilation/camera_clipping and component lighting_mismatch eligibility
are unchanged. Unknown anomaly IDs and missing/empty target arguments keep
their existing error paths. An object query with no matching actor keeps the
existing no-match refusal; it is not counted as a projection refusal.

For a nonempty object query, resolve actors with the same FindActorsMatching
name/class rules as application (including `=` exact queries). Before any
mutation, query `AnomalyViewport::GetActiveViewInfo` on the game thread. This
is the 093-07 unique local-player/world/real-viewport resolution and public
ULocalPlayer::GetProjectionData route: no first-wins/global viewport fallback.
Missing or ambiguous projection refuses the entire call and increments
`targeted_refused_no_projection` once. It is not added to persistent RefusedIds.

Use the engine view-projection matrix to extract the frustum, with the same
near-plane/infinite-far policy as the existing viewport frustum helper.
Intersect each matched actor's union of renderable primitive bounds with it.
An invalid geometry bounds or wholly outside bounds refuses the entire call
and increments `targeted_refused_off_screen` once. A mixed visible/off-screen
query never partially applies. No occlusion trace, coverage threshold,
poll-radius test, FOV/aspect reconstruction, or mask read is added here.
Partially visible/occluded bounds can pass; existing anomaly eligibility and
the measured-mask veto remain responsible for their current checks.

Store the last transient admission reason per anomaly ID so existing
TryFireSpecific/Auto.Yield diagnostics report the refusal. Clear it on the
next application attempt and at run reset. Returning false preserves the
existing retry cadence and prevents fire records, event IDs and mutations.
An already active effect is left intact by a refused re-application.

Reset the two world-local counters at capture start, alongside exclusion
stats. Export their numeric values at the root of run_summary.json, including
zeros. They count refused calls, not actors or samples. They never enter
labels, annotation, catalog schema, or capture_ownership. Console/dashboard
calls during a run are included. Add only public engine frustum includes;
no engine edits, Build.cs changes, new dependencies or private headers.

L3 accepts the last completed camera for selection/admission. Frozen serving
view geometry continues to drive labels, masks and visibility. The existing
mask veto remains the backstop at camera changes; K6 measures its behavior.
The earlier timing question is closed by L3, not a new decision item.

## Harness work planned before engine legs

Copy owned harness/specifications to `_r938`; keep retained evidence immutable.
Give the helper a new version stamp, install with fresh source timestamps,
build both hosts and verify the full Switches command in both Editor/Game
binaries before any leg. No source is edited while a build reads it.

Camera drop readings include logged unpaired/stale retirements of samples
issued before a switch, as well as missing subsequent samples; de-duplicate
by session index and retain actual receipt/reason lines. Report attribution
windows and unattributed drops. These are readings, never added gate floors.

Run the prepared archived f608d56 Lyra 942x869 masked replay for the 0.813
negative. The retained null-mask stop cannot supply a measured box scale.
Generate the healthy office I PASS from a new candidate engine capture, and
retain all four reader lines under actual Python 3.10 and the home interpreter.

The predictions file freezes changes and reasons before any leg. K7 order,
D2 stops, two attempts per own harness problem, receipts, builds and hygiene
requirements remain binding. No current result is claimed by this design.

## Implementation before qualification

Design/predictions committed at `7f4329c`. Product changes are confined to
AnomalyInjectorSubsystem.h/.cpp and AnomalyCaptureSubsystem.cpp. The first
adds the common admission/transient reasons and counters; capture resets
and exports the counters. The only new include is public ConvexVolume.h.
G482 was free across 71 refs before the implementation commit. No new
product lever, module dependency, engine/private header or label key.

## Scope stop before the first engine leg

At 2026-10-14 16:47 UTC, recorded `_r938/scope-stop.json`. Reading Code's
parallel review R8 revealed an inherited 093-07 metadata regression. Independently
checked the candidate, pushed J1 source and installed UE 5.1 engine source:
the old path used PlayerCameraManager::GetFOVAngle(), whose result is LockedFOV
when positive, otherwise the camera-cache FOV. The current projector copies
Camera.FOV directly. ULocalPlayer::GetViewPoint itself uses GetFOVAngle().
Thus a host-locked FOV changes the meaning/value of fovDeg even though the
engine-derived boxes are unaffected. A metadata repair belongs in
AnomalyViewport.cpp, outside this continuation's admission/two-counter scope.

Code explicitly calls the finding minor and says it does not invalidate the
legs; Code did not order or trigger this stop. The implementer's scope stop
follows the still-binding 093-07 Part A.3 no-key-meaning-change requirement and
093-08 Part A.3 scope restriction. No threshold or prediction failed or moved.
No new engine process was launched; no PF-1 positive, PF-2..PF-4, J2,
camera-switch, cook, staged, archived 0.813 replay or healthy I PASS ran.
The already-started build/hygiene checks and receipt restoration continue.
The admission is committed at 41c1a0c, unqualified and unpushed.

Before the stop, the revised PF-1 checker rejected both immutable controls:
093-07 has four fires and zero false labelled rows; 093-06 has four fires and
96 false rows. Session files rehash unchanged. The pending-pre-switch drop
reader's chronological test includes si59 at the cut60, which the old window
reader missed. CPU runner: 69 successful steps, including 55 Python tests;
family predicates baseline plus eight mutants and five ownership mutants pass.
Both actual Python 3.10.11 and 3.13 retained reader tests agree: null-mask FAIL
m=96, StackOBot box PASS b=999, Lyra box FAIL b=967. No healthy I line is invented.

## Closure evidence

Both fixtures pass normal/strict Editor and Game builds, zero errors/warnings,
71 standalone header TUs per target. The rebuilt helpers pass all 10 runtime
and 15 Editor full command names, including Lyra's Switches, with version
09308_r1_targeted_admission. Source/binary lever audits pass. Strings versus
the frozen f9b9328 binaries: StackOBot 1509 additions, Lyra 1507, all attributed;
unknown ASCII/wide sentinel controls reject. No new product lever or default.

Frozen product 41c1a0c has Source tree 5363827c7ac678d185a4ceecc9cd071870ed9c01
and unchanged Shaders tree 64e18bf3d238c0c10817c34ed03fcb24f5903f06.
EXEs: StackOBot 5f75eca808a001430ba643c12f7520807c3182d6900e014773d8f93fdce38ccb;
Lyra ad40da9909a115e7a6c99b3d253632c6d860910e2bc7df2e7625536468b29779.
Every candidate DLL/helper hash is in `_r938/candidate/*/manifest.json`.

Archive:
`D:/IntrusiveAnomalies/_binary_baselines/followup-capture-J2-admission-09308-41c1a0c-5F75ECA8-AD40DA99-UNQUALIFIED`.
436 payload files / 535545573 bytes rehash; a byte-mutated copy is rejected
and its archived original remains unchanged. Final docs/report are preserved
under closure with their own manifest. No cooked containers or engine proof.

Restoration: StackOBot detached 23098fa, 201 original files rehashed; Lyra
detached caa68c6, 183 originals rehashed and 19 new build files removed by
receipt. Original content is unchanged; receipt-owned helper/fixture roots
removed. All 5764 live-stage hashes match. Strict Build.cs settings and 71
generated TUs per host were restored/removed before normal builds. No engine
bench setting, host config, security setting or foreign process was changed.

Own harness corrections, one repair attempt each: decode an existing Python
BOM with utf-8-sig in the parser preflight; use a Unicode escape after a
PowerShell pipe changed the gotcha title's em dash; rebuild/timestamp/version
the missing Switches helper; include pending pre-switch drops. Before any leg,
remove the copied older resize/camera completeness exception and enforce L5
w=r for every healthy positive. No observed result was retuned. The VC env
script's vswhere lookup diagnostic did not prevent MSVC14.38 or any test.

No push; remote remains 1bf7054. Required decision: authorize the inherited
GetFOVAngle/LockedFOV metadata restoration outside admission scope, then restart
K7 at PIE PF-1. Every new engine leg and both admission counters' runtime
readings remain unavailable. This is not an idle/security/disk or engine-gate
failure, and Code did not order the stop. L3 timing is not reopened.
