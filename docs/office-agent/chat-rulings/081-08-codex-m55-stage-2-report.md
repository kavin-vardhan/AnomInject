# 081-08 — m55 Stage 2 implementation and partial qualification

2026-09-21. Authority: 081-07 scoped Stage 2 release, 081-03 Stage 2, and 081-04/05 amendments.
Codex implements; Chat orchestrates; Code independently reviews after Stage 3. No mailbox brief.

## Disposition

**NEEDS-DECISION.** Stage 2 code and all three build targets are complete. Four StackOBot static-camera
twin legs passed their available checks on candidate **2C21EDE5**, five events per leg. Qualification
stopped at `solid_swap delay=3`, natural order, after **three INVALID-FIXTURE attempts** under the
existing B1 check. This leg has no measurement verdict. No fourth attempt, relaxed fixture check,
subsequent capture, or Stage 3 work followed. The four-hour limit was not the stop trigger.

**Chat review REQUIRED** for the exhausted B1 allowance/fixture continuation and Stage 2 disposition.
**Chat review NOT REQUIRED** for the completed authorized implementation, builds and boundary
documentation. The next mandatory checkpoint is Chat's ruling on this report. Stage 3 remains
held; only a future GREEN Stage 2 report releases it automatically under 081-07.

Stage 1 is **ACCEPTED/CLOSED on 36 legs**: StackOBot G1–G15 both orders (30), Lyra G13–G15 both
orders (6). All 24 Lyra G1–G12/order legs are **DROPPED-BY-OWNER-DECISION** because the host chooses
spawn per launch and the accepted Stage 1 candidate had no placement lever. They are neither
UNEXERCISED nor FAIL. The three historical G1 L1-invalid attempts and the original G14-synthetic
failed-precondition-undefined attempt remain preserved. `081-08-evidence/stage1-ledger.json` and
`final-ledger.json`, the feature plan/journal and the Stage 3 carry-forward record state this.

## Implementation and source identity

Built implementation commit: `989da8b12ccf9fb7b70409f08a6321d5f7d78087` on
`feat/m55-change-evidence`. Final documentation commit and remote/boundary verification are in
`081-08-evidence/postflight.json` and `081-08-frozen/manifest.json`.

- Added worker measurements over immutable canonical colour and filtered/frozen delivered masks:
  max RGB byte difference, strict `>8`, eight bins, sums, guarded target/control means, K=4 phase
  windows, retained pre-onset reference, running maxima, and final event/phase records.
  Control is mask byte zero, excluding the union of all target tags. Alpha is ignored.
- `chg_measured == chg_eligible` in v1. The 14 refusal reasons remain closed. Unmeasured values
  remain explicit; `prev_target_pixels=-1` at onset does not assert prior visibility.
- Phase metadata comes from the same sampled FireLabelled snapshot used by legacy labels.
  BeginRevert after issue but before end-of-tick sampling defers closure through that snapshot.
  Closure uses the last-index watermark and the eight-capture/two-second bound. Phase details
  cap at eight while retaining actual phase count. Zero-phase events use `no_labelled_frames`.
- Pixel scans run outside the admission mutex. Unique canonical-buffer reservations count shared
  reference ownership once, with three buffers/64 MiB bounds. Reference and adjacent comparisons
  both check identity and geometry. Future late completions affect run counters, not frozen events.
- Bench-only `null_effect` and `solid_swap` share a class/lifecycle. The positive twin delegates
  to existing shipped Lit magenta material application/revert. Delay counts captured labelled
  frames. Registration is non-Shipping; Apply additionally requires the explicit fixture flag,
  supported bench map and default-off enable. No auto-pool/selector additions.
- Added editor-target-only `AnomalyBench` module for L2, using measured camera-to-pawn offset,
  one pawn placement and control rotation, preserving the host camera/movement implementation.
  It logs camera and pawn origins; typed fixture-gated control messages invoke placement and
  flush a log barrier. Fresh independent L1 follows. The runner audits the full timestamped
  placement-to-capture log interval, including buffered possession/respawn entries.
- Added a default-off CB fixture command that requests actual OpenLevel teardown with a live
  event. **Built but UNRUN**; its existence does not close the teardown gate.

Source scope inspection confirms no changes to shaders, content, verifier tools or automatic
selection. Existing label serialization and observable assignment code were not changed;
capture integration supplies sidecar metadata. **Runtime legacy invariance is still UNRUN**,
so this source check cannot establish unchanged runtime behaviour. See `source-checks.json`.

## Builds, staging and checker corrections

StackOBot Game, StackOBotEditor and LyraEditor Win64 Development built successfully for each of
three source revisions (nine builds). No cook, asset/shader change or Lyra host-source change.
The intermediate v2 build was not staged. Preliminary v1 twin readings are retained separately;
they are not qualification evidence for revised source.

Current Game SHA256:
`2c21ede58f54b947e014f29e7c81181872c90a8334eea8f004bb0c1f4ab6b392` (241,705,984 bytes).
Built, staged and archive copies agree. Archive:
`D:/IntrusiveAnomalies/_binary_baselines/StackOBot.exe.m55-stage2-2C21EDE5`.
Lyra's 11 candidate DLL/PDB/manifest files are archived at
`D:/IntrusiveAnomalies/_binary_baselines/m55-stage2-lyra-cece3c18`.
Full hashes, source identity and A44 scan results: `081-08-evidence/binary-v3.json`.
All five cooked containers and both older predecessor archives are unchanged.

The initial v1 A44 script incorrectly searched for literal `chg_hist`, which is constructed from
prefix/suffix at runtime. It stopped before any leg. I corrected the probe to literal
`chg_gt8_max_sofar`, retained ASCII/UTF-16 scans, two positive controls and an invented absent
token, verified the partially staged files and resumed staging. This was a checker defect.

## Valid current-candidate evidence

Each accepted bank is under `D:/IntrusiveAnomalies/_bench_sessions_bank/M55S2_<label>`;
all attempts, including rejected ones, remain in the corresponding `_tryN` directories.
`bank-manifest.json` records 2,469 files across the current-candidate banks and attempts.

| Label (prefix M55S2V3_) | Accepted attempt | Events / phases | Measured / required pairs | Explicit refused pairs |
|---|---:|---:|---:|---|
| NULL_N | 3 | 5 / 5 | 18 / 20 | 1 budget, 1 predecessor missing |
| NULL_S | 2 | 5 / 5 | 15 / 20 | 3 budget, 2 predecessor missing |
| SOLID_N | 1 | 5 / 5 | 20 / 20 | None |
| SOLID_S | 2 | 5 / 5 | 13 / 20 | 4 budget, 3 predecessor missing |

All 20 onsets were measured. Later refusal rows were neither replaced nor counted as zero.
The 66 measured rows conserve histograms/counts and normalized means, join legacy target_pixels,
and match adjacent/reference onset measurements. Every leg has 71 frozen-mask CRC matches to
written PNG bytes (284 total), correct window/final-record counts, no audit errors, zero retained
bytes and zero denominator/histogram/reference-onset/persistence/late-mutation/drop counters.
High-water allocation is 15,667,200 bytes per leg. Transport counters cover all issued captures;
measurement counters cover event windows and must not be conflated.

Measured onset target fractions (`gt8/n`):

| Order | Null target range | Solid target range | Solid control range |
|---|---|---|---|
| Natural | 0.000119694–0.000987477 | 1.0–1.0 | 0.017606050–0.021311170 |
| Synthetic | 0.000194503–0.000433891 | 1.0–1.0 | 0.018049448–0.021331059 |

Solid exceeds both its control and every matched-order null onset without threshold tuning.
Null is not forced to match control numerically. These are B1-qualified static fixture readings;
moving-camera twins remain unrun. `audit-m55s2v3_*.json` and `twins-ordering.json` contain the
readings and final events. Checks here establish consistency/joins/conservation, **not** the
independent saved-image arithmetic oracle required in Stage 3. No observable-change verdict
or cost/release conclusion follows from these counts.

Ordinary twins include `run_end` final records for their last events. Dedicated run-end and
actual teardown qualification remain separately open.

## Qualification stop and withdrawn unrun recipe

`M55S2V3_DELAY_N`, all at requested 1280x720 and valid focus:

| Attempt | Settle index | Modal bbox | Modal coverage / distinct | Existing B1 result |
|---|---:|---|---|---|
| 1 | 0 | (0, 485.2, 306.1, 234.8) | 85.9% / 3 | INVALID: below 90% minimum despite matching pose |
| 2 | 48 | (18.1, 441.3, 324.5, 252.4) | 94.1% / 2 | INVALID: calibrated pose mismatch |
| 3 | 29 | (0, 516.1, 7.4, 203.9) | 100% / 1 | INVALID: calibrated pose mismatch |

`a54_oracle.py:a56_check` requires modal coverage >=0.90, at most three distinct boxes and a
calibrated bbox match. No condition was relaxed. Cause of the observed variation is **NOT
ESTABLISHED**. Full log: `081-08-evidence/leg-m55s2v3_delay_n.log`. All three attempts are banked;
none received a Stage 2 arithmetic verdict. Runner exit2 means exhausted fixture attempts.

During closure review I also found my **unrun** empty-onset helper recipe was invalid:
TeleportTargetOffscreenAt3 executes before the first live BeginFire at that index, and offscreen
placement would not establish an already-occluded onset anyway. I withdrew the recipe and its
private `TargetGeometryFault` B1 exception from working helpers. No accepted or rejected run used
that exception. The journal preserves the original prediction and appends this correction.
A real occluder fixture and appropriate independently stated pose check remain necessary.

## Work remaining and Lyra scope

- Delay=3 natural: blocked, 3/3 fixture attempts used, zero remaining. Synthetic: UNRUN.
- Moving-camera twins both orders: UNRUN; a motion fixture/check must be declared before use.
- Normal hide, depth-silencing hide, already-occluded onset, blink (>8 phases), short event,
  target-drop, dedicated run-end, actual teardown, StuckMipNoHold: all UNRUN in both orders.
- Six-leg runtime legacy comparison against archived **AEBD09EA**: UNRUN. That archive has
  been verified unchanged; it remains the mandated predecessor, not older 0844220E.
- Lyra: only null/solid natural and G3 on G270 recipe are permitted. All three UNRUN.
  No L2 placement/runtime attempt has occurred; first-leg allowance remains 3/3. Log interval
  and independent pose checkers passed their selftests, which do not qualify the host.
  First-leg exhaustion must automatically drop Lyra entirely for m55 without another question;
  that condition has not occurred. No other Lyra Stage 2 or Stage 3 legs are permitted.
- Stage 3 verifier/selftest, StackOBot-only 720p/1080p cost A/B/B/A, final docs and independent
  Code review are UNRUN/held. No shipping or merge claim.

## Coverage limits carried forward

The prior limits block is copied byte-for-byte to `081-08-evidence/coverage-limits-verbatim.md`.
Its historical statement that all Stage 2 work is unrun is superseded only by the four static
twin legs above; all other listed limitations remain. `coverage-status-amendment.md` carries
this round's disposition forward, including all 24 **DROPPED-BY-OWNER-DECISION** Lyra legs.
The actual teardown limit remains open and mandatory, not waived.

Carry **FPS 21.5 vs 30 on evidence-OFF MAIN4** into Stage 3. Any cost reading there needs an
explanation before reading the gate. Earlier high-water priors remain 17.5 MB StackOBot and
19.4 MB Lyra; this round's readings are not a cost comparison. No Lyra cost runs.

## Restored boundary and evidence read order

The boundary record verifies the pushed feature, frozen source/docs, prior artifacts,
archives/containers and clean entry checkouts. Main source is restored to m51
`53bf725bd406f8ce3e38d6ed153b7ff77e3a3e67`; Lyra detached source to
`caa68c6799db023f7a24c0d6cfc56124ce27b542`, with all nine original module files restored
and only the two new AnomalyBench module files removed. CaptureBench remains
`b2ca7646e094b61d423f879dbad47af116f0ae10`. Master local/origin remains
`031a103b18b90b2ce9dd626b22e9498717c60815`; m51 and other branch refs are unchanged.
Eleven main and eight CaptureBench pre-existing untracked files are preserved.

**Source and binaries differ at this boundary:** built/staged Game remains candidate2C21EDE5,
and main StackOBotEditor plugin modules remain the rebuilt candidate. Lyra alone has its entry
modules restored. Do not infer binary identity from the restored m51 checkout.
No engine/game/build process or watcher remains owned/in flight. No merge, tag, cook or mailbox
write. All previous CURRENT bytes are preserved beneath the new handoff. No fresh-session switch
is recommended for this continuing workstream.

Read in order: this report; 081-07 ruling; frozen 081-08 journal and current field reference;
final-ledger/twins-ordering/four accepted audits; delay attempt log; binary-v3/source-checks;
bank-manifest; frozen manifest and postflight. Review helpers live under `_reviews/081-08-*`.
The old `081-08-boundary.py post` was copied from the preceding round and is **not** this round's
postflight checker; use `081-08-finish-boundary.py`. Do not rerun the suite or reset allowances
on the strength of prepared helpers alone.

## Questions for chat

1. Please rule on continuation after `solid_swap delay=3` natural exhausted the existing B1
   allowance. Keep the current three invalid attempts uncredited. Is a separately bounded,
   deterministic StackOBot fixture correction and fresh qualification allowance authorized?
   I recommend retaining the calibrated pose/coverage checks and establishing the fixture
   before new measurement verdicts; no threshold relaxation or fourth attempt was made.
2. The withdrawn offscreen recipe does not satisfy already-occluded onset. Please confirm a
   bench-only geometric occluder fixture with an independently declared pose/occlusion check
   as the continuation approach under the no-cook/no-host-source constraints. Moving-camera
   twins likewise need their own declared motion check, since static B1 is not that gate.

VERDICT: NEEDS-DECISION
