# 081-13 — NoHold runs demonstrate the expected semantics; camera-reference alignment needs a ruling

2026-09-23. Authority: `081-12-chat-ruling-nohold-on-mainworld.md` (the supplied .m
path resolved to this unique .md). No build or implementation change; A699E9EE.
**Natural A2/A3 failed the predeclared pose comparison. Neither is credited.**
The bounded cause read establishes an index/reference mismatch, not camera drift.
Synthetic and Lyra remain unrun. All prior accepted measurements/legacy gates stand.

**Chat review REQUIRED** for the reference choice and requalification of the same
artifacts. **Chat review NOT REQUIRED** for the completed authorized captures,
bounded diagnosis and safe restoration. Next mandatory checkpoint: the reference
ruling described below, before synthetic continuation or R5. Stage 3 remains held.

## What ran

Recovered the exact m52 G3 banked CLI from
`M52E_M52E_G3_HOLD_NAT_try1/_leg_game.log:207`: MainWorld, windowed1280×720,
seed777,90frames, Config2/4/8/4/0, AEoff, marker1, stall0, SVE1, delivery0, pace1,
fps30, targeted stuck_low_mip on `StaticMeshActor_UAID_A036BC6AB247EBF902_2044254803`.
Added NoHold1 and natural tick order0; unique output label. No AA/timeout override,
lock, placement, guard change or fixture flag. Change evidence was enabled from
the compiled default, confirmed by both run summaries. The initial startup Apply
found no streamable texture; three later fires applied and are counted separately.

Before launch I declared the camera check against G3's **saved `session_index`**,
the index used by images and the previous legacy comparison. All90 index values
must match, with each camera within1cm Euclidean origin /0.1° wrapped rotation.
This interpretation is the source of the rejection described below. It is recorded
in the prediction amendment and journal, not selected after seeing the measurements.

| Attempt | Frames | Foreground | Failed saved indices | Max origin / rotation delta |
|---|---:|---:|---|---:|
| NOHOLD_N_A2 | 90 | 54/54 | 43,44,45 | 1.184082cm /0° |
| NOHOLD_N_A3 | 90 | 49/49 | 43,44,45 | 1.184082cm /0° |

Both have no handled-ensure and pass the runner's focus/startup qualification.
The subsequent full-row pose check rejects them before measurement qualification.
The runner's `ACCEPTED` message/bank copy refers only to its lower-level checks;
`nohold-results.json` and each fixture-result are the final classifications.
Original N_A1 remains FAILED-PRECONDITION(recipe). **Natural allowance exhausted;
synthetic3 unused.** No fourth attempt or result-based retry. No Lyra attempt, so
its automatic-drop condition has not occurred.

## Cause: hold/nohold save different engine-frame subsets

At saved indices43/44/45 the new runs contain engineframes50/51/52, while G3 has
52/53/54. The corresponding origin deltas are1.184082/1.105133/1.031464cm; all
rotation deltas are0. Remaining87 saved-row comparisons pass.

Offline comparisons of the same untouched captures establish:

| Reference / join | Rows compared per attempt | Max origin delta | Max rotation delta |
|---|---:|---:|---:|
| Named G3 hold / saved session_index | 90 | 1.184082cm | 0° |
| Named G3 hold / engine frame_index | 80 | **0cm** | **0°** |
| Existing G2 NoHold / saved session_index | 90 | **0cm** | **0°** |
| Existing G2 NoHold / engine frame_index | 90 | **0cm** | **0°** |

The existing NoHold bank is
`M52E_M52E_G2_CANFAIL/session_20260920-214247`; its archived CLI is exactly G3 plus
NoHold1. This is an old, independently banked reference, not either new run used as
its own reference. `camera-reference-cause.json` records reference hashes, every
comparison and the exact unmatched engine indices. Ten current rows have no G3
engine-frame counterpart, so80 equal rows do not establish90-row G3 coverage.

**My saved-index interpretation was over-restrictive for this cross-recipe check.**
The ruling says “same frame indices,” which also supports literal `frame_index`;
the previous legacy runs compared identical capture recipes. Saved-frame sequence
position is not engine time when hold/nohold changes the saved subset. These data
do not establish a MainWorld camera instability or a producer regression.

I have retained the original rejected classifications. Switching reference to G2,
or accepting80/90 engine-frame coverage, changes the executed precondition. No
threshold was relaxed and no reference was silently substituted after the outcome.

## Unqualified NoHold semantics: both raw captures pass the diagnostic

These readings are **not credited gate passes** while pose qualification is open.
Full audits: each attempt's `diagnostic-unqualified-audit.json`.

- Three `Auto.FireSpecific ... -> applied` log lines at engineframes20/58/96,
  matching three deferred telemetry identities and three final sidecar identities.
- `held:false` and `bench_no_hold:true` on every deferred row; bias suppression
  explicitly logged. `stuck_mip_frames_held=0`, two hold timeouts; final fire ends
  at run cap. The first picked T_grunge_mask retains the established target recipe.
- Exactly3 final events, each phase_count0, top-level `no_labelled_frames`, empty
  phases; **zero pairs**. Two finalize on fire removal, one on run end.
- Legacy3 events, veto0, injected frame_count0 for every event:3+0=3 accounting.
  There are **67 raw legacy anomaly telemetry rows and summary positive_frames67**;
  these are unmeasured deferred rows (`target_pixels=-1`, `observable=null`, heldfalse),
  not injected/labelled positive windows. Zero injected frames is not a claim of
  zero raw anomaly rows. NoHold never arms a labelled mask in these captures.
- Canonical-byte high-water7,372,800, retained0; arithmetic/persistence/late-mutation/
  dropped-row counters0. No mask CRC or saved-colour arithmetic claim for these
  zero-pair captures.

The first offline diagnostic additionally required3 M23 first-arm lines. That was
too narrow:081-11 allows ARM/fire and latest081-12 explicitly requires3fires.
Corrected it to the actual applied-fire log lines, joined to heldfalse telemetry
and final identities. Original helper/audits are retained under
`*-before-fire-log-correction*`. This correction changes no camera result and
does not promote either attempt or consume another launch.

## Requested ruling and concrete continuation

Prefer the existing **m52 G2 CANFAIL NoHold bank** as the pose reference: it uses
the same NoHold recipe and provides complete90-row comparisons, with no tolerance
change. Authorize a dated requalification of these **same A2/A3 artifacts**; preserve
the original failed G3 saved-index audits. No new natural launch/reset is requested.
Then run synthetic within its untouched3attempt allowance using that declared
reference, followed by scoped R5 per081-07. Alternatively, specify a G3 engine-index
comparison policy for the ten rows absent from the reference; I have not invented one.

The source/module/binary state needs no change for either reference route. R5 is
held behind NoHold qualification, not dropped. Its first-leg3invalid auto-drop
rule remains. Stage3 auto-release still requires GREEN NoHold plus R5 resolved.

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


### Dated amendments through 2026-09-23

Prior24 final-build Stage2 measurement legs remain accepted, including actual
OpenLevel teardown both orders, plus both six-leg legacy comparisons. Actual
teardown is covered; separate-run stale work and real owner replacement remain
unexercised. This round adds unqualified natural NoHold artifacts only. NoHold
synthetic, scoped R5 and Stage3 oracle/selftest/cost remain UNRUN. Stage1 accepted36;
its other24 Lyra G1–G12/order legs remain DROPPED-BY-OWNER-DECISION, with prior
invalids preserved. This is separate from scoped R5, which has not been attempted.

Delay3 remains the accepted gradual-arrival counterexample: first three measured
samples<.1, index3>.9, onset near null; pre-onset ref_* exposes accumulated change.
Cite it in the Stage3 client readme. Producer ramp and0,3,6,9,12 selftest remain
unrun. Carry **FPS 21.5 vs 30 on evidence-OFF MAIN4**, prior high-water17.5MB
StackOBot/19.4MB Lyra; no pending cost gate is cleared by these NoHold runs.

## Boundary and read order

Read this full report, latest081-12 ruling, frozen081-13 journal and final ledger;
then original pose-comparison/fixture-result/nohold-results, camera-reference-cause,
unqualified diagnostic audits, all logs and the two cited m52 references. Raw
banks contain both physical attempts; duplicate runner bank copies are not extra
launches. Manifests hash all current evidence and recheck prior artifacts/banks.

Only the prediction amendment and session journal are committed/pushed on
feat/m55-change-evidence; exact commit and restoration proofs are in postflight.json.
Built source cb14f3109d56d9b6b25ab4ccb72472b5c0f2eae8 unchanged. Game built/staged/
archive SHA256 a699e9ee46d43a860912977cc8ed721cf8233958398f9cab9b0f2b43bf36a46d.
Main source restored m51 53bf725bd406f8ce3e38d6ed153b7ff77e3a3e67 with candidate
Editor/Game binaries still in place. Lyra remains detachedcaa68c6799db023f7a24c0d6cfc56124ce27b542
with original9modules: no Lyra staging/launch this round. Master/m51/CaptureBench,
archives/containers and existing untracked files—including the newly supplied
CHAT-HANDOFF-081-m55-stage-2.md—are preserved. CURRENT retains every prior byte,
including unrelated histories. No owned work in flight; no merge/tag/cook/office
transfer/owner-app change. Same workstream; no fresh Codex session recommended.

Chat review REQUIRED for the reference/requalification decision, then synthetic
and R5 continuation. Chat review NOT REQUIRED for completed authorized execution,
bounded cause read and safe boundary restoration. No release readiness claim.

VERDICT: NEEDS-DECISION
