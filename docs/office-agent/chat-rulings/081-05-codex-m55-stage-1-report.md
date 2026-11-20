# 081-05 — Codex m55 Stage 1 implementation report

**NEEDS-DECISION — implementation built; Stage 1 acceptance is incomplete.** Final candidate: 30/30 StackOBot gate legs accepted; Lyra 5 accepted, 1 failed, 24 unrun after the suite stopped. Stage 2 has not started. Both checkouts are restored; the feature implementation and full evidence are preserved.

Authority: [081-03 implementation request](D:/IntrusiveAnomalies/_reviews/081-03-codex-implement-m55-v1.md) and [081-04 Stage 1 release](D:/IntrusiveAnomalies/_reviews/081-04-chat-rulings-release-stage-1.md). Codex implements, Chat orchestrates, Claude Code independently reviews after Stage 3. No mailbox brief was sent. Stage 1 contains identity and lifetime only; no pixel-difference arithmetic, phases, event measurements, bench twins or oracle.

## 0. Delta check of v2

The original precheck correctly stopped before implementation. Chat accepted D1–D7 and released this stage. The first commit, `46c6597`, added the authoritative v2.1 amendments and predictions before source work. Nothing below converts design acceptance into runtime acceptance.

The line numbers below refer to the frozen **original active v2** at `ae2a967`, [plan snapshot](D:/IntrusiveAnomalies/_reviews/081-04-evidence/snapshot/docs/predictions/2026-09-20-m55-change-evidence.md). The superseded fold is excluded and remains unchanged.

| Finding | Original v2 status / lines | Disposition after 081-04 |
|---|---|---|
| F1 onset / gradual change | RESOLVED; 7–12, 400–452, 822–823 | RESOLVED as a measurements-only design. Phase/reference implementation belongs to Stage 2; producer ramp remains UNEXERCISED. |
| F2 expired metadata / identity | NOT RESOLVED; 256–305, 345–351, 810–820 | RESOLVED by D1–D3: issue-time generation/token, selected owner, unique serving family, and forced old colour AND mask work/coalescing/duplicate cases. Implemented here. |
| F3 progressive/final records | PARTIAL; 307–343, 460–478, 483–558 | RESOLVED by D4–D5: watermark, bounded resolution, immutable finalisation, persistence before reset; event reason and sidecar-only detail. Run lifetime implemented here; phase/event records remain Stage 2. |
| F4 reachable null control | RESOLVED; 667–737, 794–803 | RESOLVED; eligible bench twins remain Stage 2. No-labelled event schema clarified by D5. |
| F5 calibration / histogram claim | PARTIAL; 741–768, 831–835 | RESOLVED by D6: exact strict-> thresholds recoverable from bins are 0,2,4,8,16,32,64; arbitrary thresholds require images. |
| F6 unlike oracle comparisons | RESOLVED; 626–661 | RESOLVED design; count/count and mean/mean oracle remains Stage 3. |
| F7 delivered bytes / lifetime / cost | PARTIAL; 59–107, 307–396, 837–856 | RESOLVED by D7: filter before freezing, one canonical conversion, separate delivery completion, reservation before retaining refs; writer total memory remains an m51 limitation. Cost remains Stage 3. |
| F8 hide-depth / pre-onset visibility | RESOLVED; 573–595, 827–828 | RESOLVED; accepted unavailable pre-onset visibility and depth-silencing semantics are unchanged. |
| F9 control refusal | RESOLVED for v1; 109–192 | RESOLVED; no control threshold/subtraction; D6 corrects spill wording to exclude mask pixels only. |

No unresolved design blocker remained after the release. No whole row of the previous §4 lifetime table lacked either a gate or an explicit UNEXERCISED line. The earlier compound-path gaps are mapped below: real skip plus stale predecessor; colour readback, colour writer and mask writer failure; forced coalescing; wrong index-0 family plus duplicate claim; held old colour and mask across reset; synthetic extent; actual delayed completion after closure. Projection-only change, natural letterboxing, same-size cut and deliberate hitch coverage remain explicitly limited. This report does not count Stage 2/3 declarations as executed gates.

**Artifact join:** `(session directory, session_index)` joins a Stage 1 pair to `labels.jsonl`; use the index, never JSONL row position (writers complete out of order). `frame_file` names the actual run-relative PNG; predecessor must be index N−1. The original v2 already specified that join. D5 adds current/predecessor receipt times and geometry. Missing labels cannot be treated as independently verified rows. Stage 1 diagnostics include unlabelled issued captures so lifetime faults remain auditable; the per-tag/phase measurement schema is deferred.

## 1. Implementation

- **Origin and serving identity:** immutable issue carries epoch, cut, token, capture index, owner viewport/world/level, GT issue time and camera. Colour and mask keep their independent request IDs. `ISceneViewFamilyExtentionData` carries the selected token and unique family ID through UE's family copy. Owner/single-view checks and separate colour/mask served flags reject unsupported and duplicate callbacks. Drain checks generation/late admission before assembling an immutable receipt. Writer success is a separate completion record.
- **Counts and mask ownership:** each coalesced mask result carries the same receipt as its counts plus all served IDs, actual serving token and payload-owner ID. Full uint64 mask IDs are decimal strings. The non-owner has no payload and is refused. Multiple tags in one arm are not counted as two capture requests.
- **Colour ownership:** existing `Frame.RawBytes` still moves into the writer job. The existing single `ConvertTightToBGRA` result moves into a shared const array before encoding; encoder and stage reference that same canonical array. No second conversion or pixel copy was added there.
- **Mask ownership:** `Result.MaskPixels` moves to mutable `Gray`; non-event filtering, old MASK-TIE checks and exposure-exclusion folding happen first. Only then does `Gray` move into a shared const array given to writer and stage. Gate 14 compares the frozen Gray8 CRC with the decoded delivered PNG.
- **Ordered worker:** a serial worker consumes by session index and retains predecessor pixels plus receipt after legacy snapshots disappear. A later colour completion exposes an ordering gap; four further captured frames bound that observed gap. This is not a four-frame bound on the entire GPU/writer pipeline. No writer waits for capacity or a predecessor.
- **Bounds:** reserve allocated bytes before taking a reference; failed admission takes no reference and emits `budget_exceeded`. At most three canonical colour arrays total, plus masks within 64 MiB. Stage 1 has no pre-onset reference. Writer-pool memory remains outside this bound. JSON rows have a separate 100,000-row cap and drop counter.
- **Closure:** GT records the last issued index; the worker resolves through it, with a two-second/eight-further-captured-frame terminal bound. Run-end admits no further captures, so its wall-time arm is active. Unresolved work becomes `closure_timeout`. Persist before clearing/resetting; reject late results without changing final rows. Deinitialization also attempts persistence and counts teardown. Phase/event closure awaits Stage 2.
- **Artifact:** sidecar has identity-validity rows, compact current/predecessor/mask receipts and GT-submit/RT-drain times. Camera caveats are integer cm and integer tenths of a degree for rotation/FOV; they never decide validity. Summary adds counters and sidecar path only. The closed 14-reason vocabulary and exact fields are documented.

The field reference also documents each bench device. G-VIEW constructs a real second `FSceneViewFamilyContext` and index-0 view with the same scene frame and foreign render target, then calls the actual GT owner check. It **does not GPU-render that family**. The duplicate device repeats the actual RT claim, not an entire RDG delegate. G-EXTENT perturbs receipt geometry; it is not a real viewport resize. Those are limits of the evidence, not omitted implementation details.

## 2. Build and artifact provenance

Candidate source commit: **`3dc1ddaa355093f05610c8eac94d7cf15c48c7f0`**; later documentation-only commits do not alter its source. Game, modular StackOBotEditor and LyraEditor passed with zero warnings. All three immediate repeat builds were up to date. Final logs: `build-game-16-pending`, `build-editor-14-pending`, `build-lyra-11-pending`; no-op logs: Game 17, Editor 15, Lyra 12, under [evidence](D:/IntrusiveAnomalies/_reviews/081-05-evidence).

| Artifact | SHA-256 / disposition |
|---|---|
| Archived predecessor exe | `0844220e16019485ee6f6b1068ba84fdcb9ade903686461aecdf17ea91fadbd4`; preserved and hash-verified before swaps |
| Final candidate Game exe | `aebd09ea195b16317ad2d4c09ad621facfa12f0588636d9f5400f2331bf44d58`; 241,655,296 bytes; built = staged = archived |
| Lyra candidate | Full module set plus manifest/PDBs archived at `_binary_baselines/m55-stage1-lyra-74598de2`; complete hashes in the manifest below |
| Containers | All five original hashes unchanged; **no cook** |

[Full final binary/module/container manifest and A44 scans](D:/IntrusiveAnomalies/_reviews/081-05-evidence/binary-candidate-v6.json). A44 scans ASCII and UTF-16: new strings and old MASK-TIE are present; invented control string absent in Game, Editor and Lyra. The predecessor was temporarily re-staged only for its fixed-fixture controls, then the then-current candidate was hash-verified back into place; subsequent candidates were archived/staged separately.

Source checks: no shaders, assets, AnomalyShaders, anomaly implementations or verifier tools changed; existing observable assignments and reduce-count statements are identical. Gotchas are append-only and the plan's historical fold is unchanged. [Source audit](D:/IntrusiveAnomalies/_reviews/081-05-evidence/source-audit.json). These constraints supplement runtime comparison; they do not substitute for it.

## 3. Runtime evidence

Exact commands, logs, validity attempts and per-leg JSON audits are retained under the evidence directory; sessions are banked under `_bench_sessions_bank`. External harness copies preserve original focus/pose rules, refuse overwrites and terminate only their own PID after an idle check. CaptureBench repository files were not edited.

StackOBot: CB_GateLevel, StaticMeshActor_49, corrupted_texture, seed 777, native 1280×720 PNG, AA off, config `2 4 16 4 0`, 90 frames. Natural and synthetic tick orders run separately. The closure fault deliberately uses 9 frames and is lifetime-only evidence, not a three-event campaign certification. Lyra uses its worktree, `/ShooterCore/Maps/L_ShooterGym`, Cube4, seed 4242, native host rendering defaults (`r.AntiAliasingMethod=4`, `r.EyeAdaptationQuality=2` in the recorded logs), 1280×720 and the same config; host-shaped readings, no StackOBot B1 pose claim. Lyra camera movement is reported by the receipt caveats; these are not static-camera arithmetic tests.

### Legacy reduce invariance — final V6

The banked MainWorld blinking recipe passes the requested reduce comparison: two predecessor legs and final-candidate off/on/on/off agree on all 90 count/observable rows, all 30 mask PNG bytes, MASK-TIE tuples, event frame ranges, onset indices, camera records and old schema key sets. The comparison joins by session index. The first helper incorrectly derived onset from asynchronous JSONL file order; it was corrected to sort before interpreting temporal adjacency.

The additional fixed CB_GateLevel recipe preserves all 90 count/observable rows, event frame ranges/onsets, camera and old key sets. **Its raw-mask byte invariant is not established in either tick order.** Matched-order predecessor comparisons are:

| Order | Candidate evidence OFF: matching masks | Candidate evidence ON: matching masks |
|---|---:|---:|
| Native | 16/71 | 0/71 |
| Synthetic | 0/71 | 0/71 |

Every one of the 71 decoded occupied-pixel maps agrees in every comparison, and every nonzero byte equals its own label's `mask_value`. The differences are event tag values. Census/tag allocation source is unchanged; logs show the existing shared tag pool in use. A timing explanation remains an inference, not proof that m55 has no effect. The original V5 synthetic readings happened to match 71/71; V6 does not. No favourable older reading substitutes for the final candidate.

[MainWorld ledger](D:/IntrusiveAnomalies/_reviews/081-05-evidence/legacy-comparison-v6.json), [fixed-fixture ledger](D:/IntrusiveAnomalies/_reviews/081-05-evidence/legacy-comparison-bench-v6.json), [decoded tag/occupancy audit](D:/IntrusiveAnomalies/_reviews/081-05-evidence/mask-tag-audit-v6.json). The two additional native predecessor-only diagnostics match **0/71 raw masks against each other**, while all 71 occupied-pixel maps agree. Against the original native predecessor, diagnostic 1 matches 0/71 and diagnostic 2 matches 71/71. First/last tags are 247/251 versus 227/250. Thus same-binary native tag variability exists without m55. This does not establish that m55 has no additional timing effect or waive a byte-equality requirement. [Full diagnostic comparison](D:/IntrusiveAnomalies/_reviews/081-05-evidence/legacy-comparison-bench-diagnostic-v6.json), [decoded audit](D:/IntrusiveAnomalies/_reviews/081-05-evidence/mask-tag-audit-diagnostic-v6.json), [hash-verified swap/restore](D:/IntrusiveAnomalies/_reviews/081-05-evidence/predecessor-diagnostic-swap.json). Colour PNGs match 0/90, including between the two predecessor repetitions; no Stage 3 pixel/cost pass is claimed.

The [full legacy artifact delta](D:/IntrusiveAnomalies/_reviews/081-05-evidence/artifact-delta-v6.json) also compares annotation, index-joined labels and every old summary field. Added summary keys stay within the documented Stage 1 set. Relative to the **predecessor pair alone**, MAIN1/2/3/4 have **8/2/1/4 extra changed paths** respectively: shader prewarm timing; some capture/key-ring timing counters; a ~3e-17 coverage-ratio difference; and, on evidence-OFF MAIN4, stamped/video FPS 21.535 versus 30 plus coverage_pct 9.254945755 versus 9.254786491. Relative to the union of predecessor and candidate same-binary spreads, zero additional changed paths remain. That union is a descriptive variability comparison using these same readings, **not an independent pass criterion**. The strict predecessor-only EXTRAS test is nonzero; the report does not claim whole-artifact byte invariance or performance acceptance.

### Final identity/lifetime matrix

N/S below means native/synthetic tick order. Numbers count **identity-valid pairs**, not measured pixel change. Final-source results alone are credited. [Full per-leg ledger](D:/IntrusiveAnomalies/_reviews/081-05-evidence/final-ledger-v6.json), [coverage totals](D:/IntrusiveAnomalies/_reviews/081-05-evidence/coverage-v6.json).

| Gate | Stack N/S valid pairs | Lyra N/S valid pairs |
|---|---:|---:|
| 1: Two-ready reverse drain | 71/71 | UNRUN/UNRUN |
| 2: Actual skipped arm | 66/66 | UNRUN/UNRUN |
| 3: Colour writer failure | 69/69 | UNRUN/UNRUN |
| 4: Readback failure | 69/69 | UNRUN/UNRUN |
| 5: Mask writer failure | 70/70 | UNRUN/UNRUN |
| 6: Forced mask coalescing | 69/69 | UNRUN/UNRUN |
| 7: In-flight epoch rejection | 68/68 | UNRUN/UNRUN |
| 8: GT foreign family / duplicate claim | 71/71 | UNRUN/UNRUN |
| 9: Synthetic geometry mismatch | 69/69 | UNRUN/UNRUN |
| 10: Forced capacity refusal | 69/69 | UNRUN/UNRUN |
| 11: Late completion / frozen sidecar | 5/5 | UNRUN/UNRUN |
| 12: Actual stale predecessor buffer | 70/70 | UNRUN/UNRUN |
| 13: Synthetic registration gap | 69/69 | 67/25 |
| 14: Healthy frozen-mask transport | 71/71 | 35/FAILED |
| 15: Pending-arm epoch cleanup | 68/68 | 66/60 |

StackOBot: 30 accepted legs, 2,536 sidecar rows, 1,948 identity-valid pairs; maximum retained-byte high-water 17,510,400 bytes, zero retained after closure, no persistence failures or handled ensures. The two healthy controls each have 71 valid pairs, one first-frame refusal, 18 unlabelled/missing-mask refusals, zero natural ordering timeouts and 71/71 frozen-mask CRC matches. All ordinary gate legs retained five events. G11's deliberately short legs each have five valid pairs and a closure timeout; they are not campaign certification. Actual byte-cap=1 controls in both orders refused all 90 admissions and held zero bytes.

Lyra: five accepted final legs, 448 rows, 253 identity-valid pairs; high-water 19,353,600 bytes, zero held after closure. Ordinary accepted legs retained at least three events. G15 cancels one old pending colour and mask arm in each order and recovers, while its two closure timeouts per leg are explicit. G13 refuses the unissued index; synthetic G13 also reports 40 empty-region refusals. Native G14 verifies all **35 delivered nonempty masks** against frozen bytes, with 34 empty-region refusals, one capacity refusal and one dependent predecessor refusal. These are material coverage limits, not every-pair success.

**Lyra G14 synthetic FAILED and stopped the suite:** 90 colour images, zero retained annotation events, zero nonempty mask PNGs, 69 `empty_region` refusals, one capacity refusal and one predecessor refusal (plus first-frame/unlabelled refusals). The three-event minimum failed and the frozen-mask assertion was vacuous. No retry was allowed: the predeclared retry rule covered handled-ensure fixture failures only, not unfavourable measurements. Gates 1–12 in both orders remain **UNRUN on the final candidate**. Older binaries' results are retained but cannot fill that gap.

The [visibility/pose stop evidence](D:/IntrusiveAnomalies/_reviews/081-05-evidence/lyra-visibility-stop.json) shows the failed leg's starting camera at approximately (-945,-1105,470), rotation (-44.94,24.08,0), versus the older visible reference around (-444,-70,212), near-zero rotation. Later final legs also move substantially. This establishes a changed view and absent target pixels; it does **not** establish why the view changed. The runner intentionally has no StackOBot B1 pose certification. I stopped after evidence review rather than change visibility/pose rules after seeing this result or continue a vacuous mask-transport matrix. Further Lyra qualification is part of the remaining Stage 1 work.

### Iterations and failed evidence retained

1. Initial Game build caught a shared-ref/nullptr conditional; modular Editor caught a missing direct JSON print-policy include hidden by Game unity compilation. Both were corrected and rebuilt. The 15 GB free-space check was missed before the first Game link, then checked (initially above 100 GB); the 15 GB floor was enforced before subsequent builds. This procedural miss is recorded.
2. The first predecessor run used a hidden interactive window and failed the independent A63 focus rule. Restoring the original visible fixture launch produced two valid controls. No result-based retry was used.
3. Candidate `519EF682` failed lifetime: the accepted healthy leg had 51 natural ordering timeouts. Starting the four-frame clock at issue bounded total pipeline latency. It now starts only when a later completion reveals a gap. The original audit omitted the declared healthy first-frame/zero-timeout checks; those checks were added. The failed candidate is retained, not credited.
4. Candidate `76297A7E` fixed ordering but G8's throwaway view used a zero rotation matrix, causing a UE handled ensure and hitch. Initial audit lacked an ensure check. G8 is not credited on that binary. Initialize its rotation/projection; require no handled ensure; rebuild and rerun the full matrix on `3D816A89`.
5. Bounded admission can refuse additional pairs under forced batching/gaps. On superseded V3, StackOBot G1 native had one budget refusal and one dependent predecessor refusal; G2 both orders had two budget refusals each; G6 synthetic had one. Lyra's valid healthy controls had six budget refusals and six dependent refusals in each order. These are coverage readings, not a claim that every pair measured. Final-candidate readings are reported separately above.
6. Source review found a closure hang if synchronous fallback advances the capture index without issuing to m55. The worker now counts/skips never-issued indices and preserves the next actual-predecessor refusal. The first additional gate forced real synchronous capture: queue behavior was correct, but D3D12 `InRHITexture` ensure made that leg invalid. The final gate uses an explicit synthetic registration gap while retaining ordinary legacy delivery. Clean natural synchronous-fallback coverage remains UNEXERCISED; no engine change was made.
7. Lyra V3 synthetic healthy attempt 1 hit a startup material/texture-sampler ensure before capture. It is invalid. Attempt 2 passed the same whole-log no-ensure rule. Remaining cases permit at most three attempts only for this independent fixture failure, never to improve m55 readings. V3 Lyra work was stopped after the current owned leg finished so the closure fix could be built; its partial results do not certify later binaries. V5 was also superseded by the pending-arm fix; 28 StackOBot and 17 Lyra accepted V5 legs do not certify V6.

8. Source review found a second reset edge: unserved colour/mask arms retained their old owner and could block a new owner after the stage reset. Final source cancels only unserved other-generation arms under queue locks, removes legacy pending bookkeeping, and records explicit mask unavailability. In-flight GPU work remains untouched for G7. G15 forces real arms to remain pending across an epoch change and checks cancellation/recovery. Prediction commit `02ded14` preceded source commit `3dc1dda`; all final acceptance comes from the resulting binary. The final suite then stopped on the Lyra visibility/coverage failure above; no claim of a complete two-fixture matrix is made.

## 4. Coverage, boundary and disposition

### Coverage limits carried forward

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

### Boundary

Feature local/origin: **`aa99a103010b48c4811c85f7068f9f5c4a5c7492`** (pushed). Build source is `3dc1ddaa355093f05610c8eac94d7cf15c48c7f0`; later commits are documentation only. Main live checkout is restored to **m51 `53bf725bd406f8ce3e38d6ed153b7ff77e3a3e67`**, tracked clean, with all 11 pre-existing untracked handoffs preserved. Lyra is restored to detached **`caa68c6799db023f7a24c0d6cfc56124ce27b542`** and its complete entry DLL/PDB/manifest set is hash-verified back in place. CaptureBench remains `b2ca7646e094b61d423f879dbad47af116f0ae10`, unchanged, including its eight pre-existing untracked files. Master local/origin remains `031a103b18b90b2ce9dd626b22e9498717c60815`. No merge, tag, cook or mailbox write occurred.

The staged StackOBot exe remains the archived final candidate **AEBD09EA**; restoring source to m51 does not make that exe an m51 build. The archived predecessor **0844220E** and all five containers remain unchanged. No owned build/capture/helper process remains in flight. [Restoration manifest](D:/IntrusiveAnomalies/_reviews/081-05-evidence/restoration.json), [postflight](D:/IntrusiveAnomalies/_reviews/081-05-evidence/postflight.json), [final binary/container verification](D:/IntrusiveAnomalies/_reviews/081-05-evidence/boundary-binaries.json).

Use the [frozen feature snapshot](D:/IntrusiveAnomalies/_reviews/081-05-frozen/manifest.json), especially the [plan v2.1](D:/IntrusiveAnomalies/_reviews/081-05-frozen/docs/predictions/2026-09-20-m55-change-evidence.md), [field reference](D:/IntrusiveAnomalies/_reviews/081-05-frozen/docs/change-evidence-fields.md) and [full journal](D:/IntrusiveAnomalies/_reviews/081-05-frozen/docs/sessions/2026-09-21-081-05-codex-m55-stage-1.md). Live source/docs are m51 again. CURRENT preserves its full prior contents beneath the new boundary entry.

**Disposition: NEEDS-DECISION.** Identity/lifetime code is built and tested to the scope above; final two-fixture acceptance is incomplete, and the artifact findings require disposition. This stopped on a failed non-vacuity/coverage gate, not the four-hour work limit. No measurement arithmetic or release readiness is claimed. **Chat review REQUIRED on this report before Stage 2 or the next Stage 1 qualification round. Chat review NOT REQUIRED** for the Stage 1 fixes/checks already released and performed. The next mandatory checkpoint is Chat's disposition of the failed Lyra control, remaining final matrix and artifact findings; Code's independent review remains after Stage 3. No fresh session is recommended for this continuing workstream.

## Questions for chat

1. **Stage 1 artifact acceptance:** the specified banked MainWorld reduce comparison passes exactly, but the additional fixed-fixture masks have different tag bytes and the full legacy artifact comparison has nonzero predecessor-only EXTRAS. Accept the narrowly demonstrated reduce invariant and carry these findings into the Stage 3 controls, or require a further Stage 1 invariance investigation before release? Occupancy equality and same-binary spread are diagnostic evidence, not substitutes for the strict tests.
2. **Lyra completion before Stage 2:** G14 synthetic failed with zero events/no nonempty masks; 24 remaining final-candidate legs are unrun. I recommend keeping Stage 2 held and defining a Lyra visibility/pose precondition before re-qualifying the failed control and completing those legs, while retaining the failed result. Which fixture qualification should apply without treating StackOBot's B1 calibration as portable? The three-buffer limit and host AA/exposure defaults are unchanged. The report's synthetic-test limits also remain explicit; Stage 2 still needs its own teardown/phase/event finalisation gates and Stage 3 needs arithmetic/cost acceptance.

**Chat review REQUIRED before any Stage 2 implementation.** Codex stops after this report; the owner carries Chat's ruling. No Code brief or independent Code review is requested before the agreed Stage 3 checkpoint.

VERDICT: NEEDS-DECISION
