# 079-09 — Codex implementation report

**Disposition: NEEDS-DECISION.** Implemented and pushed **`fix/verifier-field-validity` at `7d5b6e919f9bca3506b9057a56a4349ea300ae64`**. The original **m51 checkout is restored**. The fixed computation rules are implemented; three supplied expectations cannot all hold together: P1's lighting result, P2's claimed bench coverage, and P6's absolute interpretation of NO-TRACE. P4 produces no derived cap. The original predictions are preserved, not rewritten to match these results.

Authority: [owner-authorized 079-09 spec](D:/IntrusiveAnomalies/_reviews/079-09-spec-for-codex-consistency-semantics.md). **Codex implemented; Claude Code independently reviews afterward; Chat orchestrates.** This report is implementation evidence, not an independent review or merge approval. No brief or mailbox message was sent.

**Chat review REQUIRED** for the conflicts below, the independent Code review and the evidence before merge. That is the next mandatory checkpoint. Chat review was **NOT REQUIRED** for this already-authorized implementation and feature push. Section G, client and m51 campaign holds remain.

## 1. Commits, scope and review copies

| Commit | Purpose |
|---|---|
| `2a38d159a5402a363736d7854bcee287c2aec711` | Predictions and interpretations committed before implementation; dated pointer in the old predictions file. |
| `7d5b6e919f9bca3506b9057a56a4349ea300ae64` | Consistency observations, actual per-pair regions, whole-span checks, run eligibility, permanent regressions, documentation and session journal. |

Remote `refs/heads/fix/verifier-field-validity` was verified at the implementation SHA after push. No merge or tag. Master/origin-master remain `ac13700d99fe116056f5aa11611f8931a563a3f2`; m51 remains `53bf725bd406f8ce3e38d6ed153b7ff77e3a3e67`. `git diff 1cbc07e 7d5b6e919f9bca3506b9057a56a4349ea300ae64 -- Source Shaders tools/measure_label_offset.py` is empty. Both repository tracked trees are clean after restoration; the pre-existing ten plugin handoffs and eight CaptureBench untracked files remain.

CaptureBench remains `472a409dc70ae78d01a38d08fc3bddb15fe41c58`. **Only its pre-existing untracked `tools/m079_shift_recovery.py` changed.** It has no Git remote; the scorer is not included in the plugin push. The local working file and [frozen scorer v3](D:/IntrusiveAnomalies/_reviews/079-09-frozen/m079_shift_recovery.py) are retained for the review, with matching hashes. No Source/Shader change, build, cook, capture, office-data transfer or owner-app change occurred.

Read the [predeclared predictions](D:/IntrusiveAnomalies/_reviews/079-09-frozen/2026-09-20-verifier-consistency-semantics.md), [session journal](D:/IntrusiveAnomalies/_reviews/079-09-frozen/2026-09-20-079-09-codex-implementation.md) and [frozen source manifest](D:/IntrusiveAnomalies/_reviews/079-09-frozen/manifest.json). Source links below point to the reviewed feature copies because the live plugin checkout is m51 again.

## 2. 079-08 findings: implementation and limits

| Finding | Change and location | Evidence / remaining limit |
|---|---|---|
| **F1 — lighting is not anomaly cause** | [verify_capture.py:904](D:/IntrusiveAnomalies/_reviews/079-09-frozen/verify_capture.py:904), [verify_capture.py:957](D:/IntrusiveAnomalies/_reviews/079-09-frozen/verify_capture.py:957): CONSISTENT carries “does not establish cause”; OFFSET-NOTE requests frame inspection and never fails. Ring fraction is diagnostic only. | Accurate-mask lighting and occlusion now exit 0. Late-label lighting is CONSISTENT under the fixed rule, contradicting P1; no label truth is claimed. |
| **F2 — fixed-mask reuse / missing coverage** | [verify_capture.py:663](D:/IntrusiveAnomalies/_reviews/079-09-frozen/verify_capture.py:663), [verify_capture.py:791](D:/IntrusiveAnomalies/_reviews/079-09-frozen/verify_capture.py:791): both actual masks form each pair's union, including baseline pairs; no snapshot substitution. Whole-span validation in `_run_outcome`. Endpoint extent in [verify_capture.py:1004](D:/IntrusiveAnomalies/_reviews/079-09-frozen/verify_capture.py:1004). | Missing mask 61 refuses the onset; missing interior mask 55 prevents NO-TRACE. Moving-mask contract measures d=0.5 from a pixel only in the previous mask. RGB dimension and endpoint-loss regressions pass. Strict coverage loses all 64 pinned edges. |
| **F3 — no signal promoted to agreement** | [verify_capture.py:904](D:/IntrusiveAnomalies/_reviews/079-09-frozen/verify_capture.py:904), [verify_capture.py:942](D:/IntrusiveAnomalies/_reviews/079-09-frozen/verify_capture.py:942): only two zero-delta TRANSITION observations produce CONSISTENT; zero-delta transition plus NO-TRANSITION is PARTIAL. Run counts are printed. | `no_detected_end` is PARTIAL. A span-level UNASSESSABLE run can have a PARTIAL event because its two NO-TRANSITION edges were observed; the run remains ineligible for scoring. |
| **F4 — transition association / bbox bounds** | [verify_capture.py:860](D:/IntrusiveAnomalies/_reviews/079-09-frozen/verify_capture.py:860): label-only bounds calculated before detection; target/mode groups separate bbox and mask runs; duplicate candidates invalidate every claimant. | 15→16 fixture makes both competing edges UNASSESSABLE at k=15, no wrong delta. Inclusive bounds also make short aligned controls unassessable; coverage cost is explicit. |
| **F5 — explicit mask-ID mismatch accepted** | [verify_capture.py:674](D:/IntrusiveAnomalies/_reviews/079-09-frozen/verify_capture.py:674): explicit nonzero absent IDs refuse the pair; only ID 0/unresolved may use a sole value. Fallback is tagged and excludes NO-TRACE. | Foreign-value fixture prints `mask id 222 not present`; unresolved blank fixture does not fail. |
| **F6 — scorer ignores run eligibility** | [verify_capture.py:1043](D:/IntrusiveAnomalies/_reviews/079-09-frozen/verify_capture.py:1043), [m079_shift_recovery.py:225](D:/IntrusiveAnomalies/_reviews/079-09-frozen/m079_shift_recovery.py:225), [m079_shift_recovery.py:369](D:/IntrusiveAnomalies/_reviews/079-09-frozen/m079_shift_recovery.py:369). READING/UNASSESSABLE runs return zero scored cells; their keys stay in an unscored ledger. | `mixed_run`: 0 scored / 2 unscored. Tiny-run denominator: 26 keys including 8 unsupported. All 572 bank keys reconcile in both guard states; no positive support or usable cap follows. |

Documentation: [client-readme.md:150](D:/IntrusiveAnomalies/_reviews/079-09-frozen/client-readme.md:150), [verify_capture.py:102](D:/IntrusiveAnomalies/_reviews/079-09-frozen/verify_capture.py:102), [office-rdp-card.md:1264](D:/IntrusiveAnomalies/_reviews/079-09-frozen/office-rdp-card.md:1264), [gotchas.md:6878](D:/IntrusiveAnomalies/_reviews/079-09-frozen/gotchas.md:6878), [gotchas.md:6907](D:/IntrusiveAnomalies/_reviews/079-09-frozen/gotchas.md:6907). The previous G263 and historical Section G prediction remain as history; their causal/coverage claims are expressly withdrawn by the current notes.

NO-TRACE's class allowlist is `missing_object`, `blink`/`blinking`, `missing_texture`, `corrupted_texture`, `lod_popping`, `lod_corruption`. [Producer evidence](D:/IntrusiveAnomalies/_reviews/079-09-frozen/producer-class-excerpts.txt) records GetId declarations, `MapAnomalyToClient`, `ResolveAnomalyActiveSource` and the active-frame predicate from the unchanged producer. Hidden blink phases are separate active runs. Time dilation, lighting mismatch, camera clipping and unknown classes are excluded. **Class membership is a producer contract, not an independent proof that every injected instance must create visible pixels.** The output's negative remains threshold-qualified.

## 3. P1 — fixture readings; one supplied expectation contradicted

The permanent self-test passes **83 cases** against the fixed semantics and disclosed interpretations. [All case outcomes](D:/IntrusiveAnomalies/_reviews/079-09-evidence/final/selftest.txt), [every exact observation line](D:/IntrusiveAnomalies/_reviews/079-09-evidence/final/selftest_full.txt), [structured edge/run data](D:/IntrusiveAnomalies/_reviews/079-09-evidence/final/selftest_details.json). This is not a claim that all original P1 predictions passed.

**c08_lighting_aligned**, exit 0:

```text
      run[60..67] OFFSET-NOTE(+1,+0) - a target transition sits off the label - possible label offset OR an unrelated change (lighting, occlusion, neighbouring event); a person must look at the listed frames k-1..k+1; does not establish cause
        onset TRANSITION 61 d=1.0000 tau=0.0040 label 60 delta=+1 ring d=1.0000 look at frames 60..62 note: whole-region change (lighting/camera?) m_edge=0.0000 base=24 pairs=9/9
        end   TRANSITION 68 d=0.0625 tau=0.0040 label 68 delta=+0 ring d=0.0000 look at frames 67..69 m_edge=0.0000 base=24 pairs=9/9
```

**c08_lighting_late_onset**, exit 0:

```text
      run[61..67] CONSISTENT - consistent with the label; does not establish cause
        onset TRANSITION 61 d=1.0000 tau=0.0040 label 61 delta=+0 ring d=1.0000 look at frames 60..62 note: whole-region change (lighting/camera?) m_edge=0.0000 base=24 pairs=9/9
        end   TRANSITION 68 d=0.0625 tau=0.0040 label 68 delta=+0 ring d=0.0000 look at frames 67..69 m_edge=0.0000 base=24 pairs=9/9
```

**c08_occluder_aligned**, exit 0:

```text
      run[60..67] OFFSET-NOTE(+1,-2) - a target transition sits off the label - possible label offset OR an unrelated change (lighting, occlusion, neighbouring event); a person must look at the listed frames k-1..k+1; does not establish cause
        onset TRANSITION 61 d=0.5000 tau=0.0040 label 60 delta=+1 ring d=0.0000 look at frames 60..62 m_edge=0.0000 base=24 pairs=9/9
        end   TRANSITION 66 d=0.5000 tau=0.0040 label 68 delta=-2 ring d=0.0000 look at frames 65..67 m_edge=0.0000 base=24 pairs=9/9
```

**Conflict:** the true texture onset is 60, the larger lighting transition is 61, and the late label starts at 61. Section 1 requires CONSISTENT for two selected zero-delta transitions; P1 forbids CONSISTENT here. Ring diagnostics cannot change the outcome under section 2. This conflict was recorded before implementation/results.

**c08_missing_mask61**, exit 0:

```text
      run[60..67] PARTIAL - one edge observed; the other is unassessable
        onset UNASSESSABLE(mask missing at frame 61) label 60 m_edge=n/a base=0 pairs=7/9
        end   TRANSITION 68 d=0.0625 tau=0.0040 label 68 delta=+0 ring d=0.0000 look at frames 67..69 m_edge=0.0000 base=24 pairs=9/9
```

**c08_no_detected_end**, exit 0:

```text
      run[60..67] PARTIAL - one zero-delta transition; the other edge has NO-TRANSITION
        onset TRANSITION 60 d=0.0625 tau=0.0040 label 60 delta=+0 ring d=0.0000 look at frames 59..61 m_edge=0.0000 base=24 pairs=9/9
        end   NO-TRANSITION [64..72] tau=0.0040 label 68 m_edge=0.0000 base=24 pairs=9/9
```

**c08_foreign_mask_value**, exit 0:

```text
      run[60..67] UNASSESSABLE - neither edge observed
        onset UNASSESSABLE(mask id 222 not present at frame 55) label 60 m_edge=n/a base=0 pairs=0/9
        end   UNASSESSABLE(mask id 222 not present at frame 63) label 68 m_edge=n/a base=0 pairs=0/9
```

**adjacent_late_second_onset**, exit 0:

```text
      run[10..11] UNASSESSABLE - neither edge observed
        onset UNASSESSABLE(ambiguous: best=0.0400 second=0.0400 ratio=1.00 < 1.50) label 10 m_edge=0.0000 base=24 pairs=7/7
        end   UNASSESSABLE(transition 15 claimed by two edges (2 claimants)) label 12 m_edge=0.0000 base=24 pairs=7/7
      run[16..16] PARTIAL - one edge observed; the other is unassessable
        onset UNASSESSABLE(transition 15 claimed by two edges (2 claimants)) label 16 m_edge=0.0000 base=24 pairs=6/6
        end   TRANSITION 17 d=0.3600 tau=0.0040 label 17 delta=+0 ring d=0.0000 look at frames 16..18 m_edge=0.0000 base=24 pairs=6/6
```

**c08_mixed_run**, exit 0:

```text
      run[60..67] READING (bbox-only)
        onset TRANSITION 60 d=0.0625 tau=0.0040 label 60 delta=+0 ring d=0.0000 look at frames 59..61 m_edge=0.0000 base=24 pairs=9/9 (bbox-only)
        end   TRANSITION 68 d=0.0625 tau=0.0040 label 68 delta=+0 ring d=0.0000 look at frames 67..69 m_edge=0.0000 base=24 pairs=9/9 (bbox-only)
```

**blank_whole_span**, exit 2:

```text
      run[40..70] NO-TRACE - the label claims a visible change on this target across frames 40..70; the pixels contain none (above tau=0.0040; pixel threshold applies)
        onset NO-TRANSITION [36..44] tau=0.0040 label 40 m_edge=0.0000 base=24 pairs=9/9
        end   NO-TRANSITION [67..75] tau=0.0040 label 71 m_edge=0.0000 base=24 pairs=9/9
```

**blank_missing_interior**, exit 0:

```text
      run[40..70] UNASSESSABLE - whole-span check: mask missing at frame 55
        onset NO-TRANSITION [36..44] tau=0.0040 label 40 m_edge=0.0000 base=24 pairs=9/9
        end   NO-TRANSITION [67..75] tau=0.0040 label 71 m_edge=0.0000 base=24 pairs=9/9
```

`missing_mask61` meets the predicted **edge** refusal; its observable end makes the run/event PARTIAL. The missing-interior case has an UNASSESSABLE **run** and a PARTIAL **event** under the event rule because both edges were observed. Neither becomes NO-TRACE. `mixed_run` supplies zero scorer cells, as required. All original blank-region negatives still produce NO-TRACE/2. Other retained review fixtures and exact known answers are in the 83-case evidence above.

### Additional coverage corrections found during implementation

The first/last RGB-file counterexamples initially produced NO-TRACE when the scan used only the existing files' extent. The final implementation uses both labelled and delivered indices, keeping the missing endpoint pairs in view. Before-fix evidence: [boundary_before.json](D:/IntrusiveAnomalies/_reviews/079-09-evidence/boundary_before.json).

**missing_first_rgb**, exit 0:

```text
      run[3..8] PARTIAL - one edge observed; the other is unassessable
        onset UNASSESSABLE(RGB missing for pair 0..1) label 3 m_edge=n/a base=0 pairs=6/7
        end   NO-TRANSITION [5..13] tau=0.0040 label 9 m_edge=0.0000 base=24 pairs=9/9
```

**missing_last_rgb**, exit 0:

```text
      run[90..97] PARTIAL - one edge observed; the other is unassessable
        onset NO-TRANSITION [86..94] tau=0.0040 label 90 m_edge=0.0000 base=24 pairs=9/9
        end   UNASSESSABLE(RGB missing for pair 98..99) label 98 m_edge=n/a base=0 pairs=5/6
```

Equal-luminance red/green transitions are now detected using RGB-channel differences; changing RGB dimensions invalidates the affected edge. These are permanent cases, alongside the previous-mask union contract.

## 4. P2 and P3 — ten bank sessions

**P2 MISSED:** all six pinned legs have four UNASSESSABLE events, zero CONSISTENT events, 64/64 edges unassessable and unavailable tau. Exit 0 means no NO-TRACE, not observed agreement. P2's `tau=0.0040`/64 observed-edge claim is not reproduced with actual per-pair masks.

| Session | Exact final line | Edge observations | Exit |
|---|---|---|---|
| [M49_GEDGE_BL_NAT](D:/IntrusiveAnomalies/_reviews/079-09-evidence/gate_M49_GEDGE_BL_NAT.txt) | `VERDICT                  NO FAILURE FOUND (0 consistent, 0 offset-notes for human review, 0 partial, 4 unassessable, 0 readings)` | 16 UNASSESSABLE | 0 |
| [M49_GEDGE_CT_NAT](D:/IntrusiveAnomalies/_reviews/079-09-evidence/gate_M49_GEDGE_CT_NAT.txt) | `VERDICT                  NO FAILURE FOUND (0 consistent, 0 offset-notes for human review, 0 partial, 4 unassessable, 0 readings)` | 8 UNASSESSABLE | 0 |
| [M49_GEDGE_MO_NAT](D:/IntrusiveAnomalies/_reviews/079-09-evidence/gate_M49_GEDGE_MO_NAT.txt) | `VERDICT                  NO FAILURE FOUND (0 consistent, 0 offset-notes for human review, 0 partial, 4 unassessable, 0 readings)` | 8 UNASSESSABLE | 0 |
| [M49_GEDGE_MT_NAT](D:/IntrusiveAnomalies/_reviews/079-09-evidence/gate_M49_GEDGE_MT_NAT.txt) | `VERDICT                  NO FAILURE FOUND (0 consistent, 0 offset-notes for human review, 0 partial, 4 unassessable, 0 readings)` | 8 UNASSESSABLE | 0 |
| [M49_GEDGE_BL_SYN](D:/IntrusiveAnomalies/_reviews/079-09-evidence/gate_M49_GEDGE_BL_SYN.txt) | `VERDICT                  NO FAILURE FOUND (0 consistent, 0 offset-notes for human review, 0 partial, 4 unassessable, 0 readings)` | 16 UNASSESSABLE | 0 |
| [M49_GEDGE_MT_SYN](D:/IntrusiveAnomalies/_reviews/079-09-evidence/gate_M49_GEDGE_MT_SYN.txt) | `VERDICT                  NO FAILURE FOUND (0 consistent, 0 offset-notes for human review, 0 partial, 4 unassessable, 0 readings)` | 8 UNASSESSABLE | 0 |
| [A2L_LEGA](D:/IntrusiveAnomalies/_reviews/079-09-evidence/gate_A2L_LEGA.txt) | `VERDICT                  NO FAILURE FOUND (0 consistent, 0 offset-notes for human review, 0 partial, 7 unassessable, 0 readings)` | 16 UNASSESSABLE | 0 |
| [LYRA_SMOKE_01](D:/IntrusiveAnomalies/_reviews/079-09-evidence/gate_LYRA_SMOKE_01.txt) | `VERDICT                  NO FAILURE FOUND (0 consistent, 0 offset-notes for human review, 0 partial, 6 unassessable, 0 readings)` | 16 UNASSESSABLE | 0 |
| [A1L_LEGA](D:/IntrusiveAnomalies/_reviews/079-09-evidence/gate_A1L_LEGA.txt) | `VERDICT                  NO FAILURE FOUND (0 consistent, 0 offset-notes for human review, 0 partial, 6 unassessable, 0 readings)` | 12 UNASSESSABLE | 0 |
| [M50L_LG9](D:/IntrusiveAnomalies/_reviews/079-09-evidence/gate_M50L_LG9.txt) | `VERDICT                  NO FAILURE FOUND (0 consistent, 0 offset-notes for human review, 0 partial, 0 unassessable, 6 readings)` | 16 UNASSESSABLE | 0 |

Representative exact pinned lines (full logs linked in the table):

```text
      run[3..10] UNASSESSABLE - neither edge observed
        onset UNASSESSABLE(mask missing at frame 0) label 3 m_edge=n/a base=0 pairs=4/7 (mask id unresolved - sole value used)
        end   UNASSESSABLE(mask missing at frame 11) label 11 m_edge=n/a base=0 pairs=4/9
```

**P3 matched:** none of the three masked gameplay sessions produces NO-TRACE. M50L_LG9 has six READING events and prints `coverage                 UNREAD-BBOX-ONLY`. All per-edge lines are retained in the linked logs, including unassessable bbox observations. Structured bank evidence: [bank.json](D:/IntrusiveAnomalies/_reviews/079-09-evidence/bank.json).

## 5. P4 — scorer v3; no cap can be derived

The immutable cohort printed **before scoring** was **572 keys**: A2L_LEGA 208, LYRA_SMOKE_01 208, A1L_LEGA 156 (44 original run-edges × 13 perturbation/variant combinations). Each guard state reconciles the identical unique key set.

| Guard state | Planned | Scored | Recovered | Wrong | No-transition | Unassessable scored | Unsupported | Unscored |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| OFF, cap=1.0 | 572 | 0 | 0 | 0 | 0 | 0 | 0 | 572 |
| ON, legacy cap=0.00014 | 572 | 0 | 0 | 0 | 0 | 0 | 0 | 572 |

In each state, 392 unscored keys belong to UNASSESSABLE runs and 180 to READING runs. The latter include **170 cells with baseline numbers that remain ineligible**. Zero wrong observations with zero scored cells supplies no validation. Rates retain 572 as their denominator; positive support is zero original edges.

Exact lines:

```text
PLANNED 572 = SCORED 0 + UNSCORED 572
DERIVED CAP UNDETERMINED; N=0 perturbation cells, not independent edges; NO ELIGIBLE CELL reached a baseline - nothing to derive from
GUARD ON uses 0.00014 (legacy configured value; NOT a v3 derivation)
POSITIVE SUPPORT: 0 original edges: []
```

[Full guard tables and refusal reasons](D:/IntrusiveAnomalies/_reviews/079-09-evidence/final/scorer_v3.txt); [all cells and source hashes](D:/IntrusiveAnomalies/_reviews/079-09-evidence/final/cohort_v3.json). **P4's numeric rederivation is unavailable.** The code records N=0 and an UNDETERMINED v3 result, retaining the existing numeric default without claiming a new empirical boundary.

The complete 78-group sweep used verifier SHA-256 `749f21cd72b0ee34a10453f33f7bb3c5a9f2ceecfdfb670c04be10b48e576410` in [the retained calibration snapshot](D:/IntrusiveAnomalies/_reviews/079-09-evidence/calibration-tools/verify_capture.py). Its only subsequent runtime change was the missing-endpoint extent correction. The [exact source diff](D:/IntrusiveAnomalies/_reviews/079-09-evidence/final/boundary-hardening.diff) and [39 rebuilt-shadow checks](D:/IntrusiveAnomalies/_reviews/079-09-evidence/final/complete_checks.json) establish that old and final bounds match for **every** calibration input, so no sweep window or reading changes. The final candidate separately reran all ten unperturbed sessions and all fixture/contracts. A full sweep after that last hardening was not repeated; this distinction is explicit.

## 6. P5 — batch and exit contract preserved

[Four contract tests](D:/IntrusiveAnomalies/_reviews/079-09-evidence/final/contracts.txt) pass: both flattened-path collision reports survive with opposite consistent/NO-TRACE readings; a forced key collision returns 3 before writing any report; mixed successful and missing-label sessions return 3 even with report-only; and the previous-mask union pixel is measured. The focused batch tests stub only the already-separately-run image self-test.

Actual CLI subprocess checks return **NO-TRACE=2**, **NO-TRACE with report-only=0**, and **execution error with report-only=3**. [Commands’ captured results](D:/IntrusiveAnomalies/_reviews/079-09-evidence/final/complete_checks.json). Scorer class controls produce `recovered/recovered`, `wrong/recovered`, `unassessable/recovered`, and `no-transition/no-transition`; the tiny case retains eight unsupported keys. [Scorer self-test](D:/IntrusiveAnomalies/_reviews/079-09-evidence/final/scorer_selftest.txt) passes, also rerun successfully against the frozen tools after m51 restoration. `git diff --check` passes.

## 7. P6 — independent review pending; threshold contradiction remains

Claude Code has not yet independently reviewed this implementation. Its fixture gate is pending. The supplied absolute statement “a wrong NO-TRACE (pixels did change …)” is incompatible with section 1's `d > tau` condition. The existing accurately masked single-pixel change has **d=0.0004 < tau=0.0040** and therefore yields:

**c07_below_tau_masked**, exit 2:

```text
      run[60..67] NO-TRACE - the label claims a visible change on this target across frames 60..67; the pixels contain none (above tau=0.0040; pixel threshold applies)
        onset NO-TRANSITION [56..64] tau=0.0040 label 60 m_edge=0.0000 base=24 pairs=9/9
        end   NO-TRANSITION [64..72] tau=0.0040 label 68 m_edge=0.0000 base=24 pairs=9/9
```

This is the specified threshold outcome, but a false negative under P6's absolute reading. It was predeclared and was not hidden by deleting the fixture or changing its threshold. The wording adds an explicit threshold qualification. Chat must choose whether the intended negative is thresholded absence or literal absence; the current implementation cannot honestly promise both. Producer class membership also does not prove a visible effect for every individual injection.

## 8. Reproduction and final boundary

The live plugin is restored to m51. To reproduce the feature tool without changing it, use these **frozen** copies (PowerShell):

```powershell
python -B D:\IntrusiveAnomalies\_reviews\079-09-frozen\verify_capture.py --label-pixel-gate --selftest
python -B D:\IntrusiveAnomalies\_reviews\079-09-frozen\test_verify_capture_consistency.py
$env:IAI_VERIFIER_TOOLS='D:\IntrusiveAnomalies\_reviews\079-09-frozen'
python -B D:\IntrusiveAnomalies\_reviews\079-09-frozen\m079_shift_recovery.py --selftest
# Full sweep, if independently repeating it; write a new output file:
python -B D:\IntrusiveAnomalies\_reviews\079-09-frozen\m079_shift_recovery.py --json D:\IntrusiveAnomalies\_reviews\079-09-independent-cohort.json
Remove-Item Env:IAI_VERIFIER_TOOLS
```

| Artifact | SHA-256 |
|---|---|
| [verify_capture.py](D:/IntrusiveAnomalies/_reviews/079-09-frozen/verify_capture.py) | `f00a5ad698cf0583925224a66947da8be6b10ce32d5333b1997946285a78043b` |
| [measure_label_offset.py](D:/IntrusiveAnomalies/_reviews/079-09-frozen/measure_label_offset.py) | `15538658dda9c14df791b6d4b1e746c4ffef94519ac9f7b59989db62ab9933d1` |
| [m079_shift_recovery.py](D:/IntrusiveAnomalies/_reviews/079-09-frozen/m079_shift_recovery.py) | `f07df15aefc7766b16ffb1cc401cfab79b98a5b4b411a3053c1bec6abfbe384d` |

All implementation tests and calibration processes have finished; their owned scratch links/directories were cleaned. No watcher or new work owner was started. Existing stale watcher/migration records and unrelated scratch content were left intact. A fresh Codex session or context-setting change is not warranted for this coherent workstream. **Claude Code owns the next independent review; Chat owns disposition. No merge is authorized by this report.**

## 9. Time and cost

First recorded task clock: **2026-09-20 05:42:55 UTC / 11:12:55 IST**. Report-generation clock: **2026-09-20 06:39:03 UTC**. Measured interval: **0:56:08** of wall time, covering source review, implementation, two full calibration sweeps, targeted validation, documentation, commit/push and checkout restoration. Any work before the first recorded clock is outside this measurement. It is not CPU time.

**Monetary cost/token billing is unavailable to this task; no dollar amount is invented.** No paid external service, Unreal build or capture was launched.

**Chat review REQUIRED** before merge: resolve P1/P2/P4/P6 and the disclosed boundary-mask/run-ledger interpretations, then assess Claude Code's independent review. Implementation and feature push are complete; release readiness is not established.

VERDICT: NEEDS-DECISION
