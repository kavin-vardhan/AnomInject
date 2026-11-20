# 081-06 — m55 Stage 1 Lyra qualification

**NEEDS-DECISION — G14 synthetic qualified; G1 native exhausted three L1-invalid attempts before capture. The remaining 24 Lyra legs are unrun. Stage 2 remains held.**

Authority: [Chat's qualification rulings](D:/IntrusiveAnomalies/_reviews/081-05-chat-rulings-stage-1-qualification.md), with 081-03/081-04 otherwise unchanged. Codex implements; Chat orchestrates; Code independently reviews after Stage 3. No mailbox brief. This round changes only qualification harnesses and documentation; the accepted implementation and binary remain unchanged.

## 1. Artifact rulings applied

- MainWorld's banked strict reduce/mask-byte/MASK-TIE/onset/camera/key-set gate is accepted as passed.
- CB_GateLevel's cross-run invariant is now **occupancy-identical AND every nonzero byte equals its own row's `mask_value`**. The previous 71/71 decoded comparisons pass. Raw stencil-tag numbers are not cross-run identities; G274 records Chat's rule and its predecessor-only evidence.
- The bounded allocator read names `FAnomalyMaskMeasure::AllocateTag` (first ledger-free value scanning from `NextTagOffset`, then advance the offset) and `FAnomalyCensus::ArmNextBatch` (claims from the same ledger/range). Their available values depend on claim/release order. This is one source explanation, not a new timing investigation.
- Whole-artifact EXTRAS are accepted as timing/float noise. Carry **FPS 21.5 vs 30 on evidence-OFF MAIN4** verbatim into Stage 3. Any cost leg at that FPS needs an explanation before the gate is read. Retained-byte high-water priors are **17.5 MB StackOBot / 19.4 MB Lyra**. No Stage 3 cost acceptance is inferred.

The [prior report](D:/IntrusiveAnomalies/_reviews/081-05-codex-m55-stage-1-report.md) and original evidence remain unchanged. The old failed G14 synthetic record is retained as `failed-precondition-undefined`, not credited or deleted.

## 2. L1 definition and bounded cause read

**Reference pose:** older visible healthy `M55S1V5_LYRA_G14_S`, [labels.jsonl line 1](D:/IntrusiveAnomalies/_bench_sessions_bank/M55S1V5_LYRA_G14_S/session_20260921-141401/labels.jsonl:1), session index 0: origin `(-443.9807434082031,-70,212.00010681152344)`, pitch/yaw/roll `(0,0,-0.08256798918660047)`. This is the line-oriented pose record in the capture artifacts; the UE text log does not print that exact pose. [Reference and source hash](D:/IntrusiveAnomalies/_reviews/081-06-evidence/l1-reference.json).

Before `capture_start`, the external runner subscribes to one fresh control-server snapshot. Its `view` uses `AnomalyViewport::GetActiveViewInfo`, the same camera source as capture labels. It requires position distance **≤150 cm** and shortest quaternion rotation distance **≤10°**. Snapshot validity, finite numbers, 1280×720 viewport and capture-not-running are also checked. Protocol/geometry faults halt; they are not pose retries. The runner unsubscribes before capture; it never moves the pawn or sets the camera. Motion after the initial check is allowed and reported as camera caveats.

Pose failure is `INVALID-FIXTURE` before any capture request. Save the raw snapshot, computed distances, decision and runtime log; stop only the owned PID. There are at most **three total attempts per leg**, shared with the existing handled-ensure exception. A passing L1 followed by an ordinary leg with fewer than three events or a vacuous G14 mask check is a real FAIL, with no retry. G11 retains its accepted nine-frame lifetime-only exception. The original 35 accepted gate legs are carried forward under Chat's acceptance, without pretending they had L1; all 25 new legs apply L1 in either order.

**Bounded cause read:** the older visible and failed G14 synthetic logs have the same map and effective launch arguments apart from output/log names. Both load `/ShooterCore/Maps/L_ShooterGym` and show pawn-avatar/respawn events before and after capture. The runner has no pose setter and uses the same 45-second settle. Neither log identifies a selected PlayerStart or establishes a stale/saved-camera cause. Why the starting view changed is **not established**; no runner configuration difference was found to fix. L1 is enforced instead. [Exact excerpts and line numbers](D:/IntrusiveAnomalies/_reviews/081-06-evidence/bounded-cause-read.json). No further cause investigation was performed.

Predictions and the rule were committed before reruns at `1efc1f8`. The L1 helper checks cover the reference, distance refusal, rotation refusal, ±180° wrapping, angular threshold and nonfinite values. PowerShell parsing passed. Exact commands, logs and per-attempt audit files are in [081-06 evidence](D:/IntrusiveAnomalies/_reviews/081-06-evidence).

## 3. Candidate and qualification results

The candidate is unchanged: source `3dc1ddaa355093f05610c8eac94d7cf15c48c7f0`, Game SHA-256 `aebd09ea195b16317ad2d4c09ad621facfa12f0588636d9f5400f2331bf44d58`, archived Lyra modules `m55-stage1-lyra-74598de2`. The original entry module set was checked before replacement; every candidate module is checked against its archived hash before each leg. **No rebuild or cook.** [Restaging manifest](D:/IntrusiveAnomalies/_reviews/081-06-evidence/candidate-restaged.json).

Run order: G14 synthetic, then G1–G12 native/synthetic. The prior 30 StackOBot and five Lyra accepted final-source legs remain credited. Results:

| Leg / attempt | Starting distance | Starting rotation difference | Disposition |
|---|---:|---:|---|
| G14 synthetic A1 | 32.505 cm | 6.402° | PASS-L1 and PASS gate |
| G1 native A1 | 2,137.344 cm | 30.120° | INVALID-FIXTURE; no capture |
| G1 native A2 | 257.783 cm | 59.434° | INVALID-FIXTURE; no capture |
| G1 native A3 | 360.000 cm | 0.083° | INVALID-FIXTURE; no capture; limit reached |

G14 synthetic retained **3 events**, wrote all **90 colour images**, and verified **49/49 delivered nonempty mask PNGs** against frozen buffer CRCs. It produced 45 identity-valid pairs, 20 `empty_region`, 3 `budget_exceeded`, 3 `predecessor_missing`, 18 `mask_payload_missing`, and 1 `first_frame` refusal. High-water was **18,432,000 bytes**, with zero held after closure. No handled ensure, persistence failure or identity audit failure occurred. This is transport/lifetime evidence; it does not claim pixel-change arithmetic or full pair coverage. [Gate audit](D:/IntrusiveAnomalies/_reviews/081-06-evidence/audit-m55s1q1_lyra_g14_s_a1.json).

All three G1 native attempts failed L1 before the `capture_start` branch. Their banked directories contain the raw pre-capture snapshot, decision and runtime log, **no capture-start marker and no session directory**. Independent recomputation reproduces every stored distance and classification. The third view is `(-443.9807434082031,-430,212.00010681152344)`, rotation `(0,0,0)`: its almost matching rotation does not satisfy the 150 cm position limit. No fourth attempt, retry-count reset, pose adjustment, target replacement or threshold relaxation occurred. [L1 verification](D:/IntrusiveAnomalies/_reviews/081-06-evidence/l1-verification.json), [all attempts](D:/IntrusiveAnomalies/_reviews/081-06-evidence/qualification-attempts.json).

These are **INVALID-FIXTURE**, neither product PASS nor FAIL. Qualification halted at the allowed limit. G1–G12 in both orders remain **UNRUN** (24 legs); G1 native's three launches did not become capture legs. The combined accepted matrix is **36/60**: 30 StackOBot, six Lyra (G13–G15 both orders). The new L1-qualified G14 synthetic result does not erase the original failed-precondition-undefined result. [Combined ledger](D:/IntrusiveAnomalies/_reviews/081-06-evidence/final-ledger.json), [matrix](D:/IntrusiveAnomalies/_reviews/081-06-evidence/matrix.md).

## 4. Boundary and carry-forwards

Documentation commits `1efc1f8` (rulings/predictions before legs) and **`4662e6ffd498b0ec789330789dbf8dde242a8a65`** (outcome/carry-forwards) are pushed to `feat/m55-change-evidence`; local and origin match. No plugin source, shader, asset or verifier-tool change was made; no new build was produced. [Frozen source/docs manifest](D:/IntrusiveAnomalies/_reviews/081-06-frozen/manifest.json), [journal](D:/IntrusiveAnomalies/_reviews/081-06-frozen/docs/sessions/2026-09-21-081-06-codex-m55-lyra-qualification.md), [updated active plan](D:/IntrusiveAnomalies/_reviews/081-06-frozen/docs/predictions/2026-09-20-m55-change-evidence.md).

Main is restored to **m51 `53bf725bd406f8ce3e38d6ed153b7ff77e3a3e67`**, tracked clean with all 11 prior untracked handoffs preserved. Lyra is restored to detached **`caa68c6799db023f7a24c0d6cfc56124ce27b542`** with all nine entry module/PDB/manifest files hash-verified. CaptureBench and its eight pre-existing untracked files are unchanged. Master local/origin remains `031a103b18b90b2ce9dd626b22e9498717c60815`; no merge/tag or master/m51 ref change. Staged StackOBot remains **AEBD09EA**, explicitly not an m51 binary despite the restored checkout; archived predecessor and all containers are unchanged. No owned process remains running. [Restoration](D:/IntrusiveAnomalies/_reviews/081-06-evidence/restoration.json), [boundary hashes](D:/IntrusiveAnomalies/_reviews/081-06-evidence/boundary.json), [postflight](D:/IntrusiveAnomalies/_reviews/081-06-evidence/postflight.json).

CURRENT is updated and its previous contents are preserved byte-for-byte. The previous Stage 1 report and frozen/banked evidence remain unchanged. This round stopped on the declared three-attempt L1 limit, not the four-hour bound. No fresh session is recommended for this continuing workstream.

 Stage 2 teardown/phase/event finalization remains required. The prior coverage-limit block is [preserved verbatim](D:/IntrusiveAnomalies/_reviews/081-06-evidence/coverage-limits-verbatim.md) for the Stage 3 report. No accepted limitation is silently closed by this qualification.

## Questions for chat

**Fixture policy decision:** three fresh G1 native launches could not meet L1, so another retry under the same exhausted allowance would be outside the released rule. Authorize a deterministic, fixture-only initial spawn/view placement at the recorded reference, followed by the unchanged L1 check, host rendering defaults and allowed in-leg motion, or choose a different qualification setup? No such placement was implemented or applied in this round.

**Chat review REQUIRED before further qualification or Stage 2.** The matrix is incomplete, so GREEN's automatic Stage 2 release condition is unmet. **Chat review NOT REQUIRED** for the already released and completed attempts. The next mandatory checkpoint is Chat's disposition of the exhausted L1 qualification. Stage 2 remains unstarted; Code's independent review stays after Stage 3.

VERDICT: NEEDS-DECISION
