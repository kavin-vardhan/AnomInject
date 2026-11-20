# 093-08 predictions: frozen before product changes and engine legs

The design/path inventory is in the 093-08 journal. All 093-07 estimator,
PF-2/PF-3/PF-4, camera, reader and 84-case J2 predictions remain binding,
except the explicit L2/L5 amendments below. No threshold is reduced.

| Amendment | Prediction | Reason frozen before any leg |
|---|---|---|
| PF-1 exact Lyra selected 942x869 MaintainYFOV | 360 requested/written, zero visible/labelled rows, zero applied fires, targeted_refused_off_screen >=1, targeted_refused_no_projection=0. Engine corners wholly left of picture. | New admission rejects before Apply rather than relying on the later mask veto. |
| PF-1 negative | Identical checker rejects retained `_r937/legs/lyra_P_pf1` with four applied fires; also retains rejection of `_r936`'s 96 false rows. | The previous projection-only candidate is the direct missing-admission can-fail. |
| Missing/ambiguous owner object attempts | No target applications while projection is unavailable; targeted_refused_no_projection >=1 if an attempt reaches admission. | Such attempts formerly could mutate before owner discovery. Refusal is retryable. |
| Ambiguous/split J2 refusal cases | 360 requested/0 written with the specified ownership refusal count. Target applications now zero, admission no-projection positive. | Their game-thread owner projection is not unique. This is an intentional refusal case, not a healthy capture. |
| Render-only NotGameView/stereo negatives | Existing 360/0 capture refusal and sub-reason assertions stay. Admission counters are recorded; do not require applied fires in a refused capture. | Render-view rejection need not make the game-thread projection unavailable. |
| Delayed viewport healthy case | 360/360, >=3 visible events, zero edges, every labelled row measured, wrong view zero. LateStart restores the viewport before rendering/first scheduled fire. | If an attempt actually precedes owner resolution it may be refused; require >=3 visible events, not an obsolete unconditional four. Record all attempts/refusals. |
| Other healthy cases, including PF-2 and camera | Existing >=3 visible-event floor, zero edges, measured paired masks and completeness remain. Record both admission counters per leg. | Admission may remove early invalid attempts; it cannot excuse missing visibility or pictures. |
| PF-3 StackOBot native X regression | Same events, targets, edges, visible_positive and bbox tolerance <=0.5px as f608d56. | Stable on-screen framing resolves before first fire; no admission change is expected here. |

W2 amendment, frozen in 093-09 before any new leg: healthy completeness is w=r
except resize and camera switches. Those legs require r=w+counted drops, with
every missing sample attributed to a named drop reason and zero stale or
mismatched frames accepted. This restores the original 093-06 resize accounting.
Camera readings include pending pre-switch samples; never count a retired sample
as written. Edges remain zero where events exist, masks measured, and PF-2
agreement required on written frames. Reason: W2 explicitly authorizes generation
retirement and supersedes 1071ca1's stricter reading of L5.

Supplemental admission probe, after PF-4 within J2: on each fixture PIE and
staged, use an on-screen framed target and temporarily remove the world's
viewport pointer during one console IAI.Apply attempt after capture starts,
restoring it before rendering. Expect exactly one no-projection refusal,
no target mutation from that attempt, then >=3 normal visible events on the
unchanged schedule, w=r=360 and zero edges/measured masks. Capture pre/post
active state and refusal reason. A copied zeroed counter or applied=true
receipt must be rejected. This lever is helper-only, IAIBench-gated and
Shipping-denied; no product bypass or timing control is introduced.

PF-2 estimator remains frozen verbatim in 093-07: median projected/mask box
centre displacement per axis about picture centre; all declared inclusion
rules; at least 20 distinct rows per axis; both raw scales [0.990,1.010];
worst-axis b rounded half up. No refitting, outlier trimming or gate tuning.
Old StackOBot masks pass ~1.000, Lyra 1123x870 rejects ~0.967, archived
Lyra 942x869 replay rejects ~0.813. Per-axis 0.96 mutants still fail.

Camera scripts retain cut index60, blend300/duration1.5 seconds, Config
0 30 180 60 4, expected990 samples, matching identity reference and PF-2
blend interval301..344. Count pending pre-switch sample retirements using
their log chronology, including indices below the switch index; de-duplicate
these with missing subsequent indices. Report all unmatched retirements too.
No change to zero-edge, mask, pose, visible-event or geometry thresholds.

Order: PIE PF-1, twelve PF-2 cases and controls, PF-3 UV/proxy, PF-4,
full J2 PIE matrix and admission probes, camera switches; then cook; every
staged gate including probes and camera; only all-green permits fast-forward
push. Rebuild/verify both helpers before engine legs. Reader healthy PASS,
retained null-mask FAIL, StackOBot box PASS and Lyra box FAIL are required.

Build normal/strict Editor/Game on both hosts, run prior meaningful suites,
lever audit/string attribution, source/comment/scope checks, archive rehash
and can-fail, restore hosts/staging by receipt. D2 stops remain unchanged.
