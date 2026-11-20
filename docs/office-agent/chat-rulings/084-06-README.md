# 084-06 — label-sync bench harness (m52 confirmation, MASK55, B-CC, B-REAL, B-LOD, B-CC-MW; B-LYRA optional)

Written by 084-05b (2026-09-29) before any leg ran. Governing rule: `_reviews\085-00-chat-ruling-delivery-cut.md` ADDENDUM
(label sync is the product: 0 frames off on both edges, from pixels; every gate must fail on a broken build), with
`_reviews\084-03-chat-ruling-bench-results.md` decisions 2 (the transition flag) and 4 (edge-local thresholds, G349) and
`_reviews\086-01-chat-ruling-audit.md` decision 1 (the release threshold is 50 % of the effect; strict and 10 % are reported).

Files: `084-06-common.py` (paths, pinned sets, process classes), `084-06-legs.py` (the leg table), `084-06-lib.py` (runner,
banking, person/quiet gates, staging), `084-06-eval.py` (the evaluator), `084-06-window.py` (the driver),
`084-06-run.ps1` (the StackOBot launcher, a byte copy of `084-03-run.ps1`), `084-06-lyra.ps1` (the Lyra launcher: it
REFUSES when any bench-class process runs and kills only its own PID; the old `lyra_leg.ps1` killed `UnrealEditor` by
name and is not used), `084-06-gateselftest.py` (26 offline known-answer checks), `084-06-proofs.py` (real-pixel proofs).
The `084-03-*` files are read-only inputs (the evaluator imports `084-03-lib.py`'s pure helpers; nothing is edited).

## Invocation (night window, PC idle)
```
C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\084-06-window.py live                     # required legs
C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\084-06-window.py live --with-optional     # + B-LYRA and the Lyra CC pair
C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\084-06-window.py gates                    # re-evaluate accepted legs only
C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\084-06-window.py dry                      # stubbed launch, banked stand-ins
C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\084-06-window.py postflight
C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\084-06-window.py rebank                   # the 084-03 bank, re-evaluated
C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\084-06-gateselftest.py
C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\084-06-proofs.py <K_on> <K_off>
```
`--only M52,BCC` restricts to groups. A second `live` resumes: accepted legs are skipped, NOT-RUN legs stay NOT-RUN.
Evidence: `_reviews\084-06-evidence\run\` (ledger, receipts, `report.json`, `report.md`, `postflight.json`). Bank:
`_bench_sessions_bank\M52SYNC_<leg>_A<n>` plus a hardlink alias `M52SYNC_<leg>`.

## Sequence and safety
1. Preflight: the pinned build-under-test archive `_binary_baselines\m52fix-E9FF019A` exists and its exe hashes to
   `E9FF019A…`; `fix/m52-label-timing` == origin and contains `f51f124`; the main checkout is `m51` `53bf725`; the staged set is
   M53 or FIX; E: >= 50 GB, D: >= 25 GB; no engine/game/UBT process of ours. The Lyra precondition is REPORTED (see B-LYRA).
2. Back up the m53 set to `E:\IA_BuildCache\_r84_m53_staged_backup` (resumes if it verifies).
3. Stage FIX (exe `E9FF019A` + container `67EA1FE0`, all six files) with a hash receipt.
4. Run the legs in order: M52, BCC, MASK55, BREAL, BLOD, BCCMW (then BLYRA, BCCLY with `--with-optional`).
5. **Always** restore M53 in a `finally` block and verify all six hashes (`2FCDF059` / `20DA6F98` / `534C5863` / `FD766B7B` /
   `462B8AC6` / `BB05CF99`). The restore refuses only on processes of OURS (foreign editors are recorded, not blocking).
6. Evaluate, write the report, run postflight.

Before each attempt: the person gate (60 s idle; waits up to 45 min) and the quiet check (any editor, game, or our UBT/SCW
blocks). A foreign editor appearing mid-leg stops the window (INVALID-FOREIGN). An attempt is INVALID on protocol, focus,
frame count or PNG problems; at most 3 invalid attempts per leg, then NOT-RUN. Nothing is ever killed except the runner's
own PID.

## Legs (all on the build under test `E9FF019A`, `-IAIBench`; MainWorld unless noted; exposure pinned, marker off, delivery off, SVE, pace 30)
| group | legs | frames | what |
|---|---|---|---|
| M52 | B0, B5 (null), B3 (auto, seed 4242), B9 (AA off), B0L (timing lever), P1 (purity lever) | 1200 (P1 300) | the m52 confirmation. **B5 is added**: the edge-local reference needs the matched null |
| BCC | CC_ON_NAT, CC_NULL_A_NAT, CC_NULL_B_NAT, CC_ON_AAOFF, CC_NULL_AAOFF, CC_ON_SYN, CC_NULL_SYN | 200 | CB_GateLevel, `-IAIBench -IAIBenchFixture`, `IAI.Bench.InputLock 1`, `IAI.Bench.CameraSchedule cc_v1`, trigger radius 100000, `camera_clipping StaticMeshActor_49 <near>` (100 ON, 10 null). AA-off = the G184 arbiter recipe |
| MASK55 | MASK55 | 1200 | CB_GateLevel auto-pool (blinking, missing_texture, corrupted_texture, missing_object), `IAI.Capture.Config 2 4 6 4 0`, seed 4242: >= 90 events |
| BREAL | REAL_{BL,MO,MT,CT}_{R1,R2}, REAL_BL_R1_AAOFF, REAL_MO_R1_AAOFF | 300 | targeted on the rock `…_2044254803` (R1) and the second rock `…_2048592804` (R2), `2 4 8 14 0` |
| BLOD | LOD_NAT, LOD_SYN, LOD_NAT_AAOFF, LOD_SYN_AAOFF | 300 | lod_popping on the rock, `IAI.Anomaly.LodMaxDistance 0` (distance gate off), `LodHalfPeriod 8`, `2 4 16 14 0` |
| BCCMW | CCMW_ON, CCMW_NULL_A, CCMW_NULL_B | 200 | camera_clipping on MainWorld at the bench camera, no schedule; the hollow-mesh reading. **NULL_B is added** for a null-vs-null floor (G352) |
| BLYRA (optional) | LYRA_{BL,MO,MT,CT,LOD,M52} | 300 (M52 600) | L_ShooterGym, `IAI.Bench.InputLock 1`, single-type auto-pool, capture started over the control server |
| BCCLY (optional) | CCLY_ON, CCLY_NULL | 200 | ON = WS `capture_start camera_clipping Cube7` (default near 100); null = auto-pool with an empty pool (WS carries no target args) |

**B-LYRA precondition (NOT met at writing):** the Lyra plugin worktree is at `caa68c6` with 2026-09-20 binaries. The legs are
recorded `NOT-RUN-PRECONDITION` until `git -C E:\UEProjects\LyraStarterGame\Plugins\AnomalyInjector checkout --detach f51f124`
and a `LyraEditor` build have been done (a build; not done here).

## The evaluator (`084-06-eval.py`, id `084-06`)
- **Labels** come from `annotation.json` `injected_frames.frame_indices` (the client's file); for m52 also checked against
  `stuck_mip.held`. **Transition set T_e** = the frames where THIS event's entry carries `transition: 1` (only that event is
  exempt there, per 084-05a).
- **m52 (E1, sharpness signal).** 084-04a's ROI (null bbox, shrunk 15 %) and gradient sharpness `S`, plus:
  - **Edge-local thresholds (G349).** The leg's `S` is divided by the matched null's `S` at the same session index, shifted
    by a lag estimated by cross-correlation over clean frames (accepted when r >= 0.6, |lag| <= 30): the scene's own cycle
    divides out. Reference = the median of the 8 frames before the label, two-point detrended to the median of the last 6
    frames of the span. Threshold per edge = max(5 sigma of the local high-frequency residual, 2 %, the null's own dip
    below its 31-frame trend in the matched window). If the picture does not return to the pre-level (post/pre beyond
    max(thr, 25 % of depth)) the offset is CENSORED. Without a usable null the basis is the leg's own two-point detrend plus
    its local linear-trend residual (declared per event as `basis`).
  - The 084-04a rules stay: two-frame onset confirmation, 3-frame contiguous clean suffix, gap censoring (a), visible
    frames outside the window FAIL (b), the window gate (ii) with a MEASURED mask on every labelled frame (c), purity (iii)
    and contamination (d), edge paths and monitor cost (e).
  - The 084-04a whole-leg threshold is reported beside it (`legacy_whole_leg_threshold`), to show what it made measurable.
- **Other object types (E1, D-signal).** 086-01's per-frame D/P metric on the mask silhouette ROI, with an **onset
  reference** from the clean frames before each label run and a separate **offset reference** from the first clean frames
  after the run's transition frames (each with its own leave-one-out noise; an offset reference that is still moving is
  CENSORED). Gap handling follows 086-01 (engine-frame gaps are judged and flagged, never censored — deviation 1 there).
- **The transition-aware gate** (every type): with V = pixel-visible frames at the threshold, L = labelled, T = T_e:
  **FAIL** if any frame of V − L − T (an unlabelled visible frame without the flag), any frame of L − V − T (a labelled
  frame not visible outside the flag), a transition flag on a leg without temporal AA, or — when frames after a label end
  are excused only by T — the effect did not start to decay at the first unlabelled frame (by 3 sigma): **a flag cannot
  excuse a label that ended early**. Outside transition frames the result is therefore 0/0. The 084-04a two-sided verdict
  (no transition knowledge) and the strict 0/0 test are reported beside it.
  - **Known blind spot, by design:** under temporal AA a label starting up to K_on frames before the first 50 %-visible frame
    is indistinguishable, from pixels, from the TAA onset lag the flag exists for. The AA-off legs (K = 0) carry the strict
    test.
- **Second verdict (E2):** the unmodified 086-01 checker on every leg, with `transition_present` rows excluded from its
  reference selection (and, for pre-G350 builds only, its declared annotation-membership reference variant). Its raw status
  and a transition-aware reading of its own edges are both reported.
- **MASK55 gate:** >= 90 events; every labelled frame has a mask_value, a measured `target_pixels` and the PNG carrying the
  value (G295-signature frames with a `TARGET MASK UNAVAILABLE` log line are counted apart); no frame where two events carry
  one value; recycled values held by events whose frame ranges do not overlap; every mask's pixels inside its own target
  box (+16 px); `mask_tag_recycles` > 0 and equal to the `TAG RECYCLED` log lines; `mask_tag_exhausted` 0; no
  `TAG-OWNER VIOLATION`.
- **B-CC oracle:** exactly the 086-02 journal §8 spec (D_k = pixels whose max channel differs by > 8, ON vs null; F, P,
  theta = sqrt(F·P), UNJUDGEABLE if P <= 2F; the 50 % rule at designed transitions; gates 1–5) plus the validity checks.
- **B-CC-MW reading:** per-frame D_k against the null, with 2 × (null-vs-null max) as the floor; labelled frames at the floor
  with `eye_inside_box` > 0 read `OVER-LABEL-SUSPECT (hollow mesh around the eye)`; unlabelled frames above the floor are
  G352 candidates (shadow-cascade shifts), not failures.
- **Presence contract (reported, not gated):** rows with `anomaly_present: true` that no event labels (Codex 084-05b F1).

## Expected bench time
Required: 31 legs (6 of 1200 frames at ~1.25 min, 25 of 200–300 frames at ~0.6–0.7 min incl. launch and banking, from 084-03's measured cadence) ≈ 25 min, plus
staging/restore ~2 min and evaluation ~20 min: **≈ 50 min; ≈ 80 min with retries and person waits.** Optional Lyra legs:
8 × ~3–4 min ≈ 30 min more, once the Lyra precondition is met.

## Harness bytes as written by 084-05b (SHA-256 prefix; the README itself excluded)
| file | sha256[:16] | bytes |
|---|---|---|
| `084-06-common.py` | `6D917A1A38973338` | 6951 |
| `084-06-eval.py` | `4EEF56681ACA77CA` | 48448 |
| `084-06-gateselftest.py` | `577B196D23591390` | 9915 |
| `084-06-legs.py` | `A2E80856BE570EF5` | 6973 |
| `084-06-lib.py` | `7DA71319A6844EA9` | 18389 |
| `084-06-lyra.ps1` | `08309E8335C9F403` | 8269 |
| `084-06-proofs.py` | `F0813B0C37733DAD` | 15210 |
| `084-06-run.ps1` | `E175E6DD275B6DA6` | 8430 |
| `084-06-window.py` | `2CF8544723C84E62` | 22415 |
`084-06-run.ps1` is byte-identical to `084-03-run.ps1` (same hash).
