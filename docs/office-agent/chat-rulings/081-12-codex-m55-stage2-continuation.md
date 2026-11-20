# 081-12 — Moving twins and both legacy comparisons PASS; NoHold recipe needs a map/fixture ruling

2026-09-23. Owner supplied `081-11-chat-ruling-nohold-target-finish-stage-2.md`.
Codex implements; Chat orchestrates; Code independently reviews after Stage 3.
**14 captures, all first-attempt valid: two moving twins and twelve legacy legs.**
No source implementation changed, no rebuild/cook, and no NoHold attempt spent.
The accepted 22 final-build measurement legs and both 5/5 guard qualifications stand.

**Chat review REQUIRED** for the remaining NoHold recipe incompatibility and this
round's disposition. **Chat review NOT REQUIRED** for the completed authorized
motion/legacy checks and safe restoration. Next mandatory checkpoint: a compatible
NoHold map/fixture recipe; then its held N2/S3 attempts, followed by R5 in 081-13.
Stage 3 remains held because R2–R4 is not GREEN and R5 is unresolved.

## 1. NoHold: proven target found, but unchanged placement cannot run on its map

The m52 evidence identifies this eligible target precisely:

- `docs/sessions/2026-09-20-080-04-m52-ratio-destroy-vt.md:28` declares **MainWorld**;
  line34 records three NoHold events, held false, empty injected frames; line42
  accepts G11 and the targeted G3/G10/G4e gates.
- `M52E_M52E_G3_HOLD_NAT/session_20260920-214315/annotation.json` identifies
  `StaticMeshActor_UAID_A036BC6AB247EBF902_2044254803`, asset **SM_rock_02**, in
  `/Game/StackOBot/Maps/MainWorld.MainWorld:PersistentLevel`.
- That bank's `anomaly_log.txt:223` records **T_grunge_mask ratio_at_pick=20.48**.
  This supplies the exact ratio the journal's accepted G3/G11 entry summarizes.
- The different SM_rock actor ending `2048592804` is **refused at ratio6.27** in
  journal lines74–82. It must not be described as a ratio≥8 target.

The qualified fixture's `AnomalyBenchModule.cpp:38` and typed bridge
`AnomalyControlServerSubsystem.cpp:388` accept **CB_GateLevel/L_ShooterGym only**.
MainWorld cannot execute either input lock or placement. CB's map reference table
contains basic shape meshes and BasicShapeMaterial/WorldGridMaterial; it contains
no SM_rock_02 reference. No m52 journal evidence establishes an eligible textured
neighbour in CB. The original actor49 Cube had zero candidate textures.

Therefore the requested combination—this m52 target and input lock/placement
unchanged—has no established executable recipe. Camera-only B1 does not remove
the command map gate or supply a MainWorld calibrated placement. I have not changed
the guard, teleported a new actor, invented a calibration, or spent an attempt
on a known absent target. `nohold-recipe-precondition.json` preserves map hash,
asset references, actor identity, ratio lines and source locations.

**NoHold_N_A1 stays FAILED-PRECONDITION(recipe). Natural2 and synthetic3 remain.**
This round adds a pre-launch incompatibility finding, not another failed capture.
No zero-label event evidence or producer defect is claimed from the absent launch.

### Concrete decision needed

Please select the compatible fixture route: either extend the bench-only MainWorld
map gates/placement using a declared MainWorld reference pose and independent fresh
camera check, or authorize a transient textured target in CB using the proven cooked
SM_rock_02 asset while retaining CB input lock and placement. The latter preserves
the existing camera fixture but changes target construction and needs an explicit
recipe. A build change would need the applicable requalification before captures.
No allowance reset is requested. The required ≥3 actual fires, held:false, ≥3
phase_count0/no_labelled_frames finals, zero pairs and zero labelled legacy rows
remain unchanged, including any veto accounting.

## 2. Natural moving twins PASS

Predictions were written before launch in the 081-12 session journal and plan
amendment. Qualified **A699E9EE**, built yaw-motion fixture, target actor49, seed777,
720p, Config2/4/16/4/0, 90 frames. Input lock and one placement precede fresh strict
B1; B1 applies at settle only. No optional synthetic moving launches were needed.

| Reading | Null | Solid |
|---|---:|---:|
| Final events / window pairs | 5 / 20 | 5 / 20 |
| cam_moved AND cam_drot_deg≠0 | 20/20 | 20/20 |
| Measured pairs | 18/20 | 17/20 |
| Measured onset gt8/n range | .027775–.044246 | .999168–1.000000 |
| Foreground samples | 87/87 | 85/85 |
| Mean control gt8/n, measured window pairs | .042017 | .047071 |
| Static natural control mean for comparison | .009714 | .016055 |

Both settled snapshots pass at sample0, rotation0/0/0, coverage100%, calibrated
origin(-1500,0,260). Both motion fractions are100%, exceeding the declared90% gate;
quantized cam_drot_deg is1–3 on those pairs. All ten onsets measured. Null stays far
below solid; control rises with motion and is reported separately, never subtracted.

Refused pairs remain explicit: null2 budget_exceeded; solid2 budget_exceeded and1
predecessor_missing. 142 frozen-mask CRCs verified, high-water≤16,588,800 bytes,
retained0; denominator/histogram/reference-onset/persistence/late-mutation/drop
counters0. No100% arithmetic coverage or independent saved-colour oracle claim.
Read `suite-v2-results.json`, each full fixture/audit/log, and `motion-readings.json`.

## 3. Mandatory legacy comparisons PASS on both maps

Verified predecessor archive **AEBD09EA** before staging. Six legs **per map**:
predecessor×2, candidate evidence OFF/ON/ON/OFF. The same startup CLI recipe ran on
both binaries; the predecessor lacks the new fixture commands. MainWorld uses the
banked rock target, CB uses actor49. Seed777, blinking, Config2/4/8/4/0,90 frames,
720p, AA0, AE off. No result-based retries; all twelve physical launches passed
their first fixture attempt. Candidate restaged in finally and verified afterward.

- All **30 within-map pairwise comparisons** passed. Each launch has8 events,
  30 masks and onset indices3/15/27/39/51/63/75/87.
- **MainWorld:** mask files byte-identical, plus identical counts, MASK-TIE tuples,
  camera rows, onsets, label/annotation/legacy-summary key sets and event ranges.
- **CB:** identical mask occupancy, and every nonzero byte equals its own row's
  mask_value. Counts/cameras/onsets/key sets/event ranges agree. MASK-TIE values
  agree after excluding only the run-local stencil tag from cross-run comparison;
  per-run tag validity remains checked against mask bytes.
- Foreground is valid on all12,54–65 measurement samples per launch, bracketed
  around first/last captured timestamps. CB's scoped B1 passes. MainWorld is outside
  actor49 bbox calibration, explicitly declared before launch; its full camera rows
  are compared across runs. No handled-ensure.
- Actual OFF artifacts have no sidecar; the four ON artifacts have Stage2 sidecars
  and90 issued captures. Thus OFF/ON is verified from artifacts, not switch alone.

Read `legacy-comparison.json` in full, `legacy-qualification.json`, command/runner
logs and banked `_focus.json`/`_leg_game.log`. Accepted bank copies and every physical
attempt are both preserved; they are not counted as additional launches.

One pre-launch helper fault was fixed: PowerShell5 returns exit1 for an empty
Get-Process match despite SilentlyContinue. Array-wrapped enumeration fixes that
idle check. No binary staging or capture preceded it; original helper preserved
as `legacy-before-idle-fix.py`. No rebuild or capture allowance consumed.

Carry **FPS 21.5 vs 30 on evidence-OFF MAIN4**. This round's roughly30fps legacy
runs do not replace the pending Stage3 game-thread/writer cost gates.

## Coverage limits: historical text preserved verbatim


- **UNEXERCISED — same-size camera cut:** accepted v1 limitation. Camera delta is a caveat, never an identity-validity rule.
- **UNEXERCISED — natural letterbox/viewport resize:** this fixture has no usable camera component for that change. G9 alters receipt geometry and tests the refusal, not a real viewport resize.
- **UNEXERCISED — dedicated projection/FOV-only producer leg:** quantised fields are exported; no claim that a dedicated optical change was forced.
- **UNEXERCISED — deliberate clean hitch:** invalid handled-ensure hitches do not count as coverage.
- **UNEXERCISED — producer-side gradual ramp:** requires asset/cook work outside v1. Accepted delay-3 end-to-end and `0,3,6,9,12` selftest belong to Stages 2 and 3 and have not run.
- **UNEXERCISED — clean actual synchronous fallback:** its attempted device hit a D3D12 ensure. G13 covers an explicitly synthetic registration gap while normal delivery continues.
- **UNEXERCISED — standalone teardown before ordinary FinishRun and separate-run stale-work transition:** code performs best-effort teardown persistence; measured gates cover run-end and within-run epoch reset. Stage 2 teardown/phase/event finalization gates remain mandatory.
- **UNEXERCISED — real owner viewport/world replacement:** G15 tests a synthetic epoch transition with real unserved arms; G7 tests actual in-flight old-generation drains. Neither is a real world replacement.
- **UNEXERCISED — JPEG/resampled/backbuffer delivery refusals as dedicated clean runtime legs:** the support checks exist, but final accepted legs use native PNG SVE delivery. Actual multi-view GPU rendering is also unexercised.
- **UNEXERCISED — the other three supported colour formats:** runtime supplied `PF_A2B10G10R10` (18), masks `PF_R8_UINT` (57).
- G8 exercises a real throwaway GT family and owner check, plus duplicate RT claim; no second GPU-rendered family or whole duplicate callback invocation is claimed.
- G11 is a nine-frame lifetime fault leg, not three-event campaign evidence. All ordinary final gate legs must meet the independent three-event rule.
- Stage 2 arithmetic, phases, event records and bench twins, and Stage 3 oracle/cost/release gates are **UNRUN**. Stage 1 sidecar rows include unlabelled indices; absence of a mask there is an explicit refusal.


### Dated amendments, 2026-09-22 and 2026-09-23

The accepted Stage2 implementation/static/delay3/hide/occlusion/blink/short/drop/
run-end/actual OpenLevel teardown coverage from081-11 stands. Actual teardown is
covered in both orders; separate-run stale-work and real owner replacement remain
unexercised. This round adds natural moving twins and both legacy comparisons.
NoHold remains unresolved, scoped Lyra R5 remains UNRUN, and Stage3 oracle/selftest/
cost remain UNRUN. Stage1 accepted36; its other24 Lyra G1–G12/order legs remain
DROPPED-BY-OWNER-DECISION. Historical invalid attempts remain uncredited.

Delay3 is accepted as the end-to-end gradual-arrival counterexample: first three
measured window samples below.1, index3 above.9, onsets near null, and pre-onset
ref_* comparisons expose the accumulating change that adjacent pairs cannot.
Carry this into the Stage3 client-readme paragraph. It does not claim that the
separate producer ramp or pending 0,3,6,9,12 arithmetic selftest has run.

## Boundary, read order, and next step

Read this full report,081-11 ruling, frozen081-12 journal, final-ledger.json,
nohold-recipe-precondition.json and cited m52 raw evidence, then moving and legacy
full audits/logs. Preflight, bank/evidence manifests, frozen manifest, restoration
and postflight establish state. Helpers and reports are under D:/IntrusiveAnomalies/
_reviews; session locations are explicit in the audits. Previous artifacts and all
4,463 prior081-11 bank files are rehashed at the boundary, not overwritten.

Only the prediction amendment and session journal are committed/pushed to the
feature branch. Exact feature hash is in postflight.json; built source remains
cb14f3109d56d9b6b25ab4ccb72472b5c0f2eae8. Game built/staged/archive SHA256 remains
`a699e9ee46d43a860912977cc8ed721cf8233958398f9cab9b0f2b43bf36a46d`.
Main source restored m51 `53bf725bd406f8ce3e38d6ed153b7ff77e3a3e67`; main Editor
and Game binaries remain candidate despite m51 source. Lyra source stays detached
`caa68c6799db023f7a24c0d6cfc56124ce27b542`, original9 modules unchanged; no Lyra
staging/launch/restore was needed this round. Master/m51/CaptureBench, archives,
containers, and existing untracked files preserved. CURRENT's entire previous byte
sequence retained, including unrelated Blender/video histories. No owned work in
flight, no merge/tag/cook/office transfer/owner-app changes. No fresh session or
context-setting change recommended; this remains the same workstream.

Chat review REQUIRED for the NoHold map/fixture decision and this report. Keep
N2/S3 held until that recipe is executable; then complete NoHold before R5, with
its existing first-leg three-invalid auto-drop rule. Chat review NOT REQUIRED for
the completed authorized motion/legacy work or safe boundary documentation.
Stage3 is not released by this partial disposition.

VERDICT: NEEDS-DECISION
