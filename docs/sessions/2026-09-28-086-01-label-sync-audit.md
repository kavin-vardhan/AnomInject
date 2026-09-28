# 086-01: Label-sync audit, part A (bench-free)

Date: 2026-09-28 (IST) · Author: Claude Code (Opus 5.5, headless) · Brief: `086-01-label-sync-audit-bank.md`
Governing rule: `085-00` ADDENDUM and `084-01` ruling. **Labels and target masks match the picture on the first and last
visible frame, 0 frames off, measured from pixels; circular gates are banned.**

**What ran:** Python over the existing bench bank only. There was no game launch, build, cook, staging, source change or
focus-taking process. It touched `_bench_sessions_bank` read-only; the working files are in `_reviews\086-01-work\`
(evaluator `ls086.py`, driver `batch.py`, aggregation `aggregate2.py` / `totals.py`, proof `canfail.py`, raw
`results.jsonl` with 4,708 event rows, `totals.json`). `m51`, `master`, the 084-03 harness and archives, the m53 S2 files,
`ToCodex\` and `E:\AmmaYT` were not touched.

## 0. Verdict in one table

| anomaly | class | evidence (bank, judged) | start offsets | end offsets |
|---|---|---|---|---|
| `blinking` | **SYNC-PROVEN at the 50 % threshold**; +1 on 4 of 592 offsets at 10 % (reappear residual, delivered AA) | 307 events / 106 sessions; CB_GateLevel, **MainWorld**, Lyra; AA delivered and off; both tick orders | 680 × **0** | 597 × **0** at 50 %; 588 × 0 + 4 × **+1** at 10 % |
| `missing_object` | **SYNC-PROVEN** (synthetic content only) | 36 events / 15 sessions; CB_GateLevel only; AA delivered and off; both orders | 36 × **0** | 30 × **0** (6 censored) |
| `missing_texture` | **SYNC-PROVEN, thin** | 25 events / 14 sessions; CB_GateLevel + Lyra; AA delivered only (the AA-off checker signal is too faint to measure) | 25 × **0** | 14 × **0** (11 censored) |
| `corrupted_texture` | **SYNC-PROVEN** | 130 events / 49 sessions; CB_GateLevel + Lyra; AA delivered and off; both orders | 130 × **0** | 103 × **0** (27 censored) |
| `stuck_low_mip` | **DESYNC** (the `master` build; the 084 fix is not yet bench-proven) | 34 judged events / 13 sessions; MainWorld, Lyra | at 50 %: −1 × 10, −2 × 2, −3 × 4, −5 × 2, −20 × 1, 0 × 2 | +4 × 2 measured; 20 censored, 10 unresolved (blur persists: post-label frame at 95 % of the effect, median) |
| `lod_popping` | **UNMEASURED** | 36 events, **0 judgeable** (21 not measurable, 6 confounded by scene motion, 3 camera moved, 3 warm-up, 3 no labelled frame) | — | — |
| `camera_clipping` | **UNMEASURED** | **0** schema-2 sessions in the bank; its per-frame label is a collision-sphere proxy (§5.3) | — | — |
| `uv_corruption` | **UNMEASURED for the shipped modes** (tile, scramble: not built) | pipeline evidence only: 188 events, identity / wrong-copy fixtures, AA off | 188 × 0 (fixture modes) | 155 × 0 |
| `normal_corruption` | **UNMEASURED for the shipped modes** (invert, green_flip: not built) | pipeline evidence only: 18 events, identity / wrong-copy fixtures | 18 × 0 (fixture modes) | 15 × 0 |

**Sign convention:** offset = first (or last) visible frame − first (or last) labelled frame. **Positive means the pixels
lag the label.** `stuck_low_mip`'s −1 means the blur is visible one frame before the label starts.

**Controls, read on the same instrument:**
- `solid_swap` (the bench twin sharing the texture-swap path) reads 289 onsets × 0 and 275 offsets × 0;
- its **DELAY** case (magenta applied 3 labelled frames late by design) reads **+3 on 12 of 12**;
- `null_effect` produces **no judgeable signal on any of its 186 events**.

## 1. The delivered anomalies (from `master` `b5f15a3`)

Registered: 10 shipping ids plus the bench twins `null_effect` and `solid_swap` (excluded from delivery, used here as
controls). **Delivered on 5 Oct:**
- the 7 `GAutoPool` ids: `missing_object`, `blinking`, `missing_texture`, `corrupted_texture`, `lod_popping`,
  `camera_clipping`, `stuck_low_mip`;
- plus `uv_corruption` and `normal_corruption` (m53, unmerged, `feat/m53-uv-normal-corruption` `d117a9d`).

`lod_corruption`, `time_dilation` and `lighting_mismatch` are registered but hidden from the client dashboard
(`HIDDEN_ANOMALY_IDS`) and are not audited here.

**The common labelling path:**
- Every label is sampled at `FWorldDelegates::OnWorldTickEnd`: `OnWorldTickEndSample` → `SampleDeferredActiveState`,
  `AnomalyCaptureSubsystem.cpp:885-892, 4846`. That is after every tickable and before the frame's draw.
- The source per id comes from `ResolveAnomalyActiveSource` (`:287-306`), and the per-frame bit from
  `IsFireLabelledThisFrame` (`:4955-4975`).
- **Masks are armed by the same predicate in the same tick** (`OnWorldTickEndMask` `:896-927` → `ArmTargetMaskOwn`
  `:1159`, gated on `IsFireLabelledThisFrame` `:1187`). Mask timing therefore equals label timing by construction, and the
  mask half of the rule reduces to "a mask exists on every labelled frame".

| id | label source | what drives the window (anchor) | same frame as the picture? | gate history |
|---|---|---|---|---|
| `blinking` | ActorHidden | `IsLogicallyHidden(actor)` (`AnomalyHiddenClass.cpp:174`); toggled in the injector's tick (`Anomaly_Blinking.cpp:74-92`) via `AnomalyHiddenClass::Hide/Show` (per-primitive main/depth/shadow/Lumen/DF flags + `MarkRenderStateDirty`, `AnomalyHiddenClass.cpp:95-130, ~155-165`) | yes: the dirty proxy is flushed inside `BeginRenderingViewFamilies` in the same frame (m26 `F-1`); m40 made the sample order-independent. Temporal AA / Lumen history can leave a partial reappear frame (§5.2). | **pixel, not circular:** m40 L1–L4 (pixel reader), m44 ONSET (first differing picture), m49 G-EDGE both edges (AA-off arbiter; delivered report-only) |
| `missing_object` | ActorHidden | same flag; `Hide` in `Apply` (`Anomaly_MissingObject.cpp:36`), `Show` in `Revert` (`:54`) | yes, as above | pixel: m49 G-EDGE both edges; m55 C2 HIDE legs. Not in any m44 onset leg. |
| `missing_texture` | FireWindow | the label is true on every frame the fire is live; per-component `SetMaterial` in `Apply` (`Anomaly_MissingTexture.cpp:123`), restore in `Revert` (`:205/:216`) | yes: a material swap applies at the next draw. Risk: an uncompiled shader draws the fallback (m47 measured none). | pixel: m44 ONSET, m49 G-EDGE (AA-off margin 5.7τ, the thinnest) |
| `corrupted_texture` | FireWindow | same code shape (`Anomaly_CorruptedTexture.cpp:123/:205/:216`) | yes | pixel: m44 ONSET, m49 G-EDGE; m55 `solid_swap` shares the path |
| `lod_popping` | AnomalyState | `bActive && bPoppedPhase` (`Anomaly_LodPopping.h:20`); starts BASELINE (`.cpp:214`) and flips every `HalfPeriodFrames` injector ticks with `AnomalyLod::SetForcedLod` (`.cpp:235-253`) | the proxy is dirtied in the same tick. **Candidate lag, not established: a dithered LOD fade on meshes with dithered LOD transitions.** | **circular:** m30 `G-P2′` counted toggles from the anomaly's own counter; `G-P1` was an eyeball of a pop, not per frame. **Never pixel-gated per frame.** |
| `camera_clipping` | AnomalyState | `IsCurrentlyAnomalous` (`Anomaly_CameraClipping.cpp:135-146`) → `AnomalyViewport::IsGeometryWithinNearClipRadius` (`AnomalyViewport.cpp:857-885`): `OverlapAnyTestByChannel` of a **sphere of radius `GNearClippingPlane` at the view origin**, `ECC_Visibility`, **pawn ignored**; session-global, near clip set in `BeginActualRun` | **no, by construction; it is a proxy** (§5.3) | **circular:** m30 `G-C2` ("close pose 60 positive / 0 negative") tested the label against its own overlap predicate; `G-C1` was a one-frame eyeball |
| `stuck_low_mip` | AnomalyState | `IsCurrentlyAnomalous` = resident mips below baseline via the game-thread mirror `GetNumResidentMips()` (`Anomaly_StuckLowMip.cpp:892, :719`) | **no** (084-01: the mirror updates after the render-thread swap; `BeginRevert` drops the fire while stream-in is in flight) | **circular onset, pixel-blind offset** (ruled in 084-01) |
| `uv_corruption` / `normal_corruption` | FireWindow (`d117a9d` `AnomalyCaptureSubsystem.cpp:304-305`) | canvas draw to a render target (`TexCorruptDraw.cpp:89-146`, `ENQUEUE_RENDER_COMMAND`) + host MID + `SetMaterial` in the fire tick (`Anomaly_TexCorrupt.cpp:340, 974`); a foreign-replace path re-applies 2 ticks later (`:1055-1065`) | expected yes (all of it is enqueued before the frame's scene render); **unmeasured on shipped modes** | S1 gates read logs and labels ("no pixel clause"); S2 plans a pixel ONSET gate (G1) and only a report-only offset reading (G9). **The offset has never been pixel-gated.** |

## 1a. Bank evidence used

- **Index:** 2,525 session folders, 1,549 unique by session id. Label-schema-2 sessions (m49 and later) with every frame
  on disk and `labels.jsonl` present: **770 sessions evaluated**.
- **Events:**
  - 4,708 event rows;
  - 1,198 excluded as lever legs (ChangeGate 600, lever families 451, ForceTagCollision 32, StuckMipNoHold 33,
    TagPoolLimit 24, VetoArmUngated 16, HideOmitDepthPassSilencing 15, SpawnTranslucentProbe 12, RetakeMaterialAfter 8,
    TeleportTargetOffscreenAt 4, DestroyTarget 2, StuckMipVirtualProbe 1);
  - **3,510 kept**.
- **Families that carry the verdicts:**
  - m55 requalification (`M55B3R23_*`: `C2_SOLID/DELAY/NULL/HIDE/BLINK`, the `MAINON` and `CB` blink legs, `L1`–`L4`
    Lyra stuck-mip);
  - `M55S1*` / `M55S2*`;
  - the m49–m52 banks (`A1*`, `A2*`, `PB*`, `M50*`, `M51*`, `M52*`, `R02S`);
  - `M53S1_*` (m53 fixtures).
- **Content:**
  - CB_GateLevel (synthetic primitives);
  - **MainWorld** (StackOBot's real level: blinking at delivered AA, 14 events, and at AA off, 72; stuck_low_mip;
    lod_popping);
  - **Lyra** (TSR, real-game materials, but moving cameras: 2–5 judged events per type).
- **AA state:** from each leg's `_leg_geometry.json` `extra_execcmds`. The m55 C-cases run through the 081-23 harness,
  whose recipe adds no AA override, so they are "delivered (no override recorded)".

## 2. The rule and the instrument

**Rule (085-00 ADDENDUM):** labels and target masks match the picture on the first and last visible frame, 0 frames off,
measured from pixels, with a gate that fails on a broken build; circular gates are banned.

**Instrument: `086-01` evaluator** (`_reviews\086-01-work\ls086.py`, driver `batch.py`, aggregation `aggregate2.py`;
outside the repo, like the 084-03 harness). It generalises the 084-04a two-sided evaluator from m52's blur signal to every
anomaly type:

- **Labelled window:** `annotation.json` `injected_frames.frame_indices` (the labelled anomalous frames; `affected_frames`
  is its observable subset). Each maximal run of labelled frames is one phase (blinking has one per hidden run).
- **ROI:** the event's own target-mask silhouette (union over its labelled frames, dilated 4 px), else its median projected
  bbox. Hide types use the m45 would-be silhouette, so the ROI exists on hidden frames.
- **Reference, per phase (run-local):** the median of the clean frames just before the phase (for blinking, the visible gap
  frames minus the first, which carries the temporal-AA ghost). Run-local references were needed because MainWorld is a
  dynamic scene: a far reference drifts.
- **Signal:** per frame, `D` = mean over the ROI of max-channel |frame − reference| (0–255), and `P` = fraction of ROI
  pixels with that difference > 8.
- **Strict visibility:** `D > μ + max(6σ, 1.0)` or `P > max(2 × noise P, 1 %)`, where μ, σ come from leave-one-out
  differences among the reference frames.
- **Effect fraction:** `(D − μ) / (phase depth − μ)`. A frame is **visible at t10** if strict-visible and fraction ≥ 0.10,
  **at t50** if ≥ 0.50 (majority). Strict-only frames are reported, never hidden.
- **Edges:** per phase, start offset = first visible frame − first labelled frame; end offset = last visible frame − last
  labelled frame (convention of `measure_label_offset.py`: **positive = pixels lag the label**). Blinking phases are judged
  inside windows split at the midpoint between hidden runs. Holes (labelled, not visible) and stray visible frames fail.
- **Censoring (edge not judged):** a missing row or PNG across the edge, no 3-frame suffix inside the span, or scene drift
  (the late clean frames differ from the phase reference by ≥ 10 % of the effect).
- **Gap-bounded (judged, flagged):** an engine-frame jump across the edge. The capture FSM deliberately does not capture its
  settle frames, so many edges sit next to one. For the delivered dataset the comparison is per captured frame and stays
  exact; contiguous edges are reported separately as mechanism evidence.
- **UNRESOLVED:** a residual still visible at the last frame of the phase window (no decay observed), or already visible at
  its first frame. Neither passed nor failed.
- **Not judged:** WARMUP (the fire starts before si 30, or the reference lies there: session-start lighting/streaming
  settle, as in 084-03's `SETTLE_SKIP`), CAMERA-MOVED (> 0.5 cm or 0.05° anywhere in reference + span), NOT-MEASURABLE
  (depth P < 0.20 and D < 3 × threshold), CONFOUNDED (the ROI's largest change lies > 3 frames outside the labelled window,
  so the dominant change is not the anomaly).
- **Masks:** every labelled frame must carry a mask with the event's value (`mask_missing`); no mask outside the label
  (`mask_extra`).
- **Wrong-object candidate:** a connected blob ≥ 400 px of > 48-level change outside the ROI (dilated 24 px), off the
  CaptureBench frame-ID marker strip and off regions already changing between reference frames, on a labelled frame, above
  twice the reference-frame noise. This is an upper bound: shadows, GI spill and reflections trigger it too.
- **Second witness (m55 `change_evidence.jsonl`):** per phase, window 0's pre-onset-reference change (`ref_gt8 / chg_n`)
  against windows 1–3: ≥ 0.5 × later ⇒ "onset on the first labelled frame"; < 0.1 × later ⇒ "label early"; all small ⇒
  "no change in window (late or invisible)". Onset only: the evidence covers the first four labelled frames of a phase.
- **Lever legs excluded:** any leg whose recipe carries an `IAI.Bench.*` lever other than `SynthTickOrder`,
  `MaskPairingProbe`, `ChangeEvidenceCases` or `CensusMaskDump`, plus the named m55 lever cases (ChangeGate G-cases,
  HIDE_DEPTH, DROP, TEARDOWN, COUNT/WALL_FAIL, EMPTY, SHORT, RUNEND) and the OBS2/OBS3/OBS4/PB4/G2/G6/LG9 lever families.
  Levers break sync by design; counting them would indict the product for the lever.

### Deviations from 084-04a, stated
1. **An engine-frame gap across an edge does not censor** (084-04a censors it). The capture FSM's settle gap sits beside
   most onsets and offsets by design. The can-fail proof below found that censoring on it **excused a real one-frame
   desync** (dropping the first labelled frame of a solid swap read "censored" instead of FAIL). Gap-bounded edges are
   judged per captured frame and counted apart from contiguous edges.
2. **No 2-frame onset confirmation:** a single visible frame counts (stricter).
3. **The fraction is mean-change based.** A pixel-count fraction was tried first and overstated faint lighting shifts
   (a reappearing object 16 % different in mean but with 64 % of its pixels > 8 levels off). `P` stays in the strict gate
   and its post-edge value is reported (`post1P`).
4. **Thresholds are reported at three levels** (strict / 10 % / 50 %), because hide types on temporal-AA content carry a
   partial first reappear frame, and which level counts as "visible" decides the class. See NEEDS-DECISION 1.

## 3. The gate fails on a broken label (can-fail proofs, all on real banked pixels)

1. **Known answers in the bank, judged blind:**
   - `solid_swap` DELAY (magenta 3 labelled frames late by design) reads **start +3 on 12 of 12** events, and the m55
     witness independently reads "label early" on every one;
   - `solid_swap` SOLID reads 0/0;
   - `null_effect` reads nothing judgeable on 186 of 186 events: no false visibility.
2. **Doctored labels** (`canfail.py`): each event's `injected_frames` are rewritten and re-judged against the unedited
   pixels. The variants are onset late by 1, onset early by 1, offset early by 1, offset late by 1, whole window +1 and
   whole window −1. **All of them FAIL at both 10 % and 50 %, and every exact label PASSes: 98 of 98 checks.**
   - 7 events: 3 `solid_swap`, 2 `missing_object` (CB, delivered AA) and 2 `blinking` (CB delivered AA with 17 hidden
     runs, and MainWorld AA off);
   - one further MainWorld event was not judgeable (camera) and is listed as skipped.
3. **The proof caught a defect in my own first censoring rule.** Censoring an edge because an engine-frame gap sat next to
   it (084-04a's rule) **excused a real one-frame desync**: dropping the first labelled frame of a solid swap read
   "censored". The capture FSM leaves its settle gap right beside most edges. The rule was changed before any verdict was
   read (§2, deviation 1).

## 4. Results in detail

Per anomaly, over judged events only. `n` counts phases (blinking has several per event). All three thresholds are shown.

| anomaly | strict (noise) end offsets | 10 % end offsets | 50 % end offsets | post-edge residual, mean-change fraction p50 / p90 / max | same, pixel-count fraction max | m55 onset witness |
|---|---|---|---|---|---|---|
| blinking | 0 × 223, +1 × 70, +2 × 10, +3 × 12 (283 unresolved) | 0 × 588, **+1 × 4** (6 unresolved) | 0 × 597 (1 unresolved) | 0.024 / 0.042 / 0.159 | 0.766 | onset on first labelled frame: **230 of 230** measured |
| missing_object | 0 × 29, +1 × 1 | 0 × 30 | 0 × 30 | 0.000 / 0.044 / 0.057 | 0.526 | 18 of 18 |
| missing_texture | 0 × 12, +1 × 1 | 0 × 14 | 0 × 14 | 0.109 / 0.144 / 0.271 (below the strict noise gate: the checker is a faint change) | 0.925 | — (no evidence rows) |
| corrupted_texture | 0 × 56, +1 × 17, +2 × 1, +3 × 2 | 0 × 103 | 0 × 103 | 0.010 / 0.023 / 0.049 | 0.115 | — |
| stuck_low_mip | +4 × 2 | +4 × 2 | +4 × 2 | **0.948** / 0.981 / 1.000 | 1.000 | across all 101 evidence phases: onset-on-first 22, partial 21, **no change (late or invisible) 45**, label early 11 |

**Onsets: every judged onset of the four same-frame types is 0 at every threshold, strict included (871 onsets).** The
m55 witness agrees on every phase it measured (248 of 248). Of blinking's 680 onsets, 307 sit next to the FSM's settle gap
(gap-bounded); the other 373 are contiguous (inner hidden runs) and are the mechanism evidence.

**Offsets carry the only residual.** The first captured frame after a hidden object reappears, or after a texture is
restored, keeps a small part of the effect: median 1–2 %, and below 6 % for missing_object and corrupted_texture. At
delivered AA on CB_GateLevel, 4 blinking offsets reach 12–16 % of the hide (A1F/A1/A2F/R02S auto-pool legs, the event at
ordinal 2). **AA off reads 0 at 10 % and 50 % on all 292 blinking offsets (max 7.3 %),** which places most of the residual
in temporal AA; see §5.2.

**Masks:**
- **0 labelled frames without a mask** on master-era legs of the delivered types. **0 masks outside the label.**
  - The 151 blinking mask-missing frames are all on `M51_F1*` legs, which ran `TargetMask 1 + Mask 0` on the unmerged
    `m51` branch: the honest-unmeasured design of that branch, not a delivered default.
- **Delivery-relevant exception, the tag-pool ceiling:**
  - in the 90-event m55 `S1_STACK_ON` leg, `TAG-POOL EXHAUSTED` fired 35 times and `unmeasurable_targets_admitted = 35`.
    **The last 35 events carry labels but no target mask.** This is m50's documented one-tag-per-event ceiling (55
    values, never released mid-run). **Any delivered capture session with more than about 55 events ships its later
    events without masks.** See NEEDS-DECISION 2.
  - separately, a 30-event 720p stress leg (`S3_720`) shows `mask_state: unmeasured` on 68 of 480 labelled anomaly rows
    with no pool exhaustion. **Cause not established here.**

**Wrong-object:**
- **Among delivered types, 2 events show an outside-the-target change tied to the label** (blink, AA off, CB). Checked by
  eye: it is **the hidden cube's own shadow and contact region**, which m45 silences by design. Not a wrong object.
- `stuck_low_mip`'s 22 outside-change events also change in clean frames (MainWorld scene motion), so none is tied to the
  label.
- **So the bank shows no wrong-object case.** It also cannot show one for m52: the owner's glove/wall case is office-only,
  and the purity fix has its own gate in 084-03 (P0/P1).
- The metric is an upper bound (shadow, GI and reflection spill trip it).

**Why so much is "not judged":**
- WARMUP: the first ~30 frames of every leg are session-start lighting/streaming settle;
- CAMERA-MOVED: all Lyra and several MainWorld legs;
- NOT-MEASURABLE: faint effects. Every `null_effect`; AA-off `missing_texture`; most m53 identity fixtures;
  `stuck_low_mip` on low-frequency content (the G270 class);
- CONFOUNDED: MainWorld scene motion dominating a distant `lod_popping` target.

These are reported, never counted as passes.

## 5. DESYNC and residual mechanisms, with fix sketches (none implemented)

### 5.1 `stuck_low_mip`: DESYNC on `master` (mechanism established by 084-01; this audit reproduces the shape independently)
- **Measured here:**
  - the blur is visible **before** the label: 10 onsets at −1, 2 at −2, 4 at −3 and 2 at −5, against 2 at 0 (50 % threshold);
  - the blur **persists after** the label: the first frame after the label still carries 95 % of the effect (median);
    2 offsets measured at +4, the rest run past the window (unresolved or censored);
  - the m55 witness reads "no change in window" on 45 of 101 phases, which is what a label starting after the blur
    produces.
- **Agreement:** with 084-01's bank reading (−1 on 43 of 43 onsets, +5 to +14 on 42 of 42 offsets) and with the owner's
  office reading (onset −1 to +2, a 5-frame offset tail).
- **Mechanism (084-01, ruled):**
  - the onset reads the game-thread mirror `GetNumResidentMips()` (`Anomaly_StuckLowMip.cpp:892`), which updates only
    after the render-thread swap;
  - at the offset, `BeginRevert` drops the fire and `Revert()` goes inactive in the same tick while stream-in is still in
    flight.
- **Fix:** already built on `fix/m52-label-timing` (`c71f826`, exe `B725678B`): a render-thread residency record per
  captured frame, the event outliving `Revert`, and a streamer fence. **Not yet bench-proven: 084-03 is the proof**, and
  this evaluator should run on its banked legs as a second, independent method.

### 5.2 Hide types under temporal AA: a partial first reappear frame (a residual, not a timing desync)
- **Measured:**
  - on delivered-AA CB_GateLevel legs, the first captured frame after `Show()` carries 12–16 % of the hide (mean change)
    on **4 of 592** blinking offsets, all in auto-pool legs at event ordinal 2;
  - median across all offsets 2.4 %;
  - **AA off: max 7.3 %, 0 offsets over 10 %.**
  - measured by pixel count instead, the same frames can have 50–77 % of silhouette pixels off by more than 8/255, i.e. a
    faint, full-object change (the object is present, but its lighting and edges are not yet settled).
- **Mechanism: CANDIDATE, not established (G120).** Temporal history: TAA/TSR blends previous frames, and a reappearing
  object has no history. A slower part appears after the final `Show()` (0.77 → 0.42 → 0.37 → 0.30 pixel-count fraction
  over 4 frames in `A1_A1_NAT` ordinal 2). That looks like Lumen / distance-field lighting reconverging once `Show`
  restores `bAffectDynamicIndirectLighting` and `bAffectDistanceFieldLighting` (`AnomalyHiddenClass.cpp:~155-165`). Not
  isolated.
- **This is not a label timing error.** The label tracks the scene state on the exact frame. The question is whether a
  90 %-present object counts as "visible anomaly". That is NEEDS-DECISION 1.
- **Fix sketches, if a 10 % rule is chosen:**
  - (a) an **additive, non-label** per-frame key (e.g. `transition_frame: true` on the first captured frame after a
    hide-type phase ends) so a client can drop or weight it. It changes no existing field, so `P6` is unaffected;
  - (b) for the Lumen part, restore lighting participation one frame **before** main-pass visibility on `Show()`, so the
    object reappears already lit. That changes the picture and needs its own identity arbiter (m45's AA-off lever pair);
  - (c) extending the label one frame is **rejected**: it would label a frame where the object is plainly present as
    "missing".

### 5.3 `camera_clipping`: the per-frame label is a proxy that can disagree with the picture (UNMEASURED, a structural risk)
The label is `OverlapAnyTestByChannel(ViewOrigin, sphere(GNearClippingPlane), ECC_Visibility, ignore pawn)`
(`AnomalyViewport.cpp:857-885`). Against what is actually clipped from the picture, it disagrees in four ways:
1. **Omnidirectional:** geometry behind or beside the camera inside 100 cm labels the frame positive while nothing
   visible is clipped.
2. **Collision is not render geometry:** a mesh without Visibility collision (decals, foliage, many props) is clipped in
   the picture but never labelled; oversized collision labels frames where nothing visible is clipped.
3. **The pawn is ignored:** in third person the player's own mesh inside the near plane is visibly clipped with no label.
4. The session-global mode has no target, so no mask and no m26 veto. (`P6`: `bbox_norm` 0,0,1,1.)

- **Fix sketch:**
  - replace the sphere with a **frustum-slice test**: the convex slab between the baseline near plane and the anomalous
    one, intersected with each primitive's render bounds, pawn included;
  - better, a render-side measurement: a depth pre-pass count of pixels whose scene depth falls inside the slab at the
    baseline near plane. That needs a matched-null bench oracle (B-CC) before anything ships as sync-proven.

### 5.4 `lod_popping`: UNMEASURED; candidate lag named
- **In the bank:**
  - no judgeable pop. The 33 MainWorld auto-pool events are distant: 21 are not measurable and 6 are confounded by scene
    motion (the ROI changes smoothly over 25 frames regardless of the label);
  - the 3 Lyra events are camera-moved or have no labelled frames.
- **Candidate lag (not established):** dithered LOD transitions. On such materials a forced-LOD change can fade over
  several frames under temporal AA, so a 50 % crossing could lag the label.
- **Fix sketch, only if B-LOD measures a lag:** refuse targets whose materials use dithered LOD transitions, or read the
  proxy's current LOD on the render side (the m52 fix shape).

### 5.5 m53 (`uv_corruption`, `normal_corruption`): no evidence for the shipped modes
- The bank holds only `identity` / `identity_redraw` / `tile_probe` fixtures and wrong-copy levers on a 64 × 64 tile.
- Where those produce a visible change, the draw → MID → picture pipeline reads **0/0 on 188 + 18 onsets and 155 + 15
  offsets** at AA off. That is pipeline timing, not proof for tile / scramble / invert / green_flip.
- The m53 plan has no offset pixel gate; part B adds one.

## 6. Limits of this audit, stated

- **The judged content is mostly synthetic.**
  - `missing_object` and `corrupted_texture` delivered-AA edges are CB_GateLevel cubes;
  - MainWorld (real StackOBot art) carries only blinking;
  - Lyra contributes 2–5 events per type because its legs move the camera.
  - "Realistic content" is part B's job (B-REAL, B-LYRA).
- **Office hosts are not measured.** Their AA (TAA / TSR / other) and lighting (Lumen or not) decide the §5.2 residual.
  That is what the kit (§8) is for.
- **Thresholds, the fraction metric, WARMUP=30, the 6σ gate and the 10-frame span pad (widened to 16) were fixed before
  the final run but chosen during this session**, after inspecting early outputs. Each change is stated in §2 with the
  finding that forced it. None was tuned to move a verdict; the doctored-label proof re-ran after every change.
- **A 1-frame edge offset inside the FSM's uncaptured settle frames is invisible to any captured-frame audit.** It does
  not reach the dataset, but a host with a different settle count would expose it. That is why contiguous edges (inner
  blink runs) are reported separately: 373 contiguous blinking onsets and every offset, all at 0.
- `gap_bounded` / censoring use `frame_index` deltas and on-disk PNG presence, not the capture log.

## 7. Part B: the bench plan (not run; night window, PC idle)

Every leg is judged by the `086-01` evaluator. Each is paired with a matched null or has a clean pre-onset reference, runs
at a settled camera (hands off, pose logged), and starts its first judged event after si 30. Legs are banked like
084-03's, with the same receipts and the same person-present and quiet gates. **Nothing here needs a cook. B-CC needs one
small bench-only source addition (day-time build); everything else runs on existing binaries.**

| id | closes | legs | notes |
|---|---|---|---|
| **B-M52** | stuck_low_mip DESYNC → fix | **already scheduled as 084-03** (B0/B3/B5/B9 on baseline `E0BE6F0A` and fix `B725678B`, B0L/P0/P1 can-fail) | Add one step: run `086-01` on the banked legs beside 084-04a's gate (an independent second method). |
| **B-LOD** | lod_popping UNMEASURED | 4 legs, MainWorld rock `StaticMeshActor_UAID_A036BC6AB247EBF902_2044254803` at close range: `IAI.Anomaly.LodMaxDistance 0` + targeted fire + `IAI.Anomaly.LodHalfPeriod 8`, 300 frames; native and synth order; AA delivered and AA off | The bank has no visible pop (every M52F `lod_popping` event is NOT-MEASURABLE or CONFOUNDED by scene motion). Needs a target whose LOD pop is large in the frame (m29 measured 66,615 px at 33 % coverage). The AA-off twin separates a dithered LOD fade (a candidate lag) from temporal AA. |
| **B-CC** | camera_clipping UNMEASURED | 4 legs: session-global `camera_clipping` on CB_GateLevel with a scheduled camera path (150 → 50 → 150 cm from a wall and a behind-the-camera pass), plus the same path with the near clip at baseline as the matched null | **Needs a bench lever** (AnomalyBench, `-IAIBench` only, compiled out of Shipping): a camera-pose schedule, since today's bench places the camera once. The pixel oracle is ON vs null at the same frame; it tests the three proxy risks in §5.3 directly (behind-camera overlap, non-colliding geometry, the ignored pawn). |
| **B-M53** | uv/normal corruption UNMEASURED | ride the **085-03 gate window**: run `086-01` on its mode legs (tile, scramble, invert, green_flip, both orders), plus **4 extra legs** on a realistic MainWorld target (floor `SM_FloorBase` and the rock) for the two default modes at AA delivered, each with a NoApply null | Measures both edges, the offset included (the m53 plan only has a pixel ONSET gate, G1). |
| **B-REAL** | realistic content at delivered AA for the types proven on CB_GateLevel | 10 legs on MainWorld: blinking, missing_object, missing_texture, corrupted_texture on the rock and on one more static target, native order; plus AA-off twins of blinking and missing_object on the rock (the TAA/Lumen residual attribution) | The bank's MainWorld coverage is blinking only at delivered AA (M52 legs, 2 events each). |
| **B-LYRA** | a second host with TSR and real-game materials | 6 legs, `L_ShooterGym` under the AnomalyBench camera lock: blinking, missing_object, missing_texture, corrupted_texture, lod_popping (a hero weapon or a multi-LOD prop at close range), stuck_low_mip | Every Lyra leg in the bank has a moving camera (CAMERA-MOVED) or a warm-up-only event. TSR's history differs from TAA, so the reappear residual must be measured there before the office host. |

**Time:** StackOBot legs about 2–2.5 min each including launch and banking (084-03's estimate scaled to 300 frames), Lyra
about 5 min each (shader warm-up).

| part | minutes |
|---|---|
| B-M52 | 0 extra (084-03) |
| B-LOD | ~10 |
| B-CC | ~10 bench, plus about half a day-time session to build and gate the lever |
| B-M53 | ~10 extra on top of 085-03 |
| B-REAL | ~25 |
| B-LYRA | ~35 |
| analysis | ~20 |
| **total** | **about 1 h 50 min of bench**, plus retries (≤ 3 invalid attempts per leg, as in 084-03) |

**What the owner's environment-pack project would add, once it exists:** realistic art on the StackOBot engine. That means
dense foliage (the `InstancedFoliageActor` exclusion class and wind), high-frequency albedo and normal maps (stuck_low_mip
and m53 visibility depend on texture content, the G270 class), textures shared across many meshes (a real test for
m52's and m53's wrong-object rule), authored LOD chains at close range (lod_popping), dense near geometry and
non-colliding props (camera_clipping's proxy risks), skeletal characters and translucency. One static-camera leg per
anomaly and per content class would move every "on which content" column from synthetic cubes to production-like art.

## 8. The office check kit (specification only; not built)

**Name:** `label_sync_check.py`, one file, beside `verify_capture.py` in the delivered host-tools.

**Runs on:** a stock office Python 3.8+ with the standard library only (`zlib`, `json`, `struct`). If Pillow is
importable it uses it for speed. The two decoders must give identical numbers, which `--selftest` proves. Read-only: it
never writes into a session folder.

**Inputs:** one or more session folders (or a parent folder, scanned for `annotation.json`). It reads:
- `annotation.json`: events, `injected_frames` and `affected_frames`;
- `labels.jsonl`: written in delivery mode by default since 2026-08-22; per-frame `frame_index`, `view` pose and anomaly
  entries with `mask_value`;
- `mask_map.json`;
- `Actual_Frames/frame_NNNNN.png` and `target_mask/frame_NNNNN.png`;
- optionally `change_evidence.jsonl` (m55) and `run_summary.json`.

**Capture requirements (the owner's recipe for check captures):**
- PNG frames (the default), target mask on (the compiled default) and `IAI.Capture.OutputHeight 0`; the mask is refused
  when downscaling, and the kit then falls back to the bbox at reduced confidence;
- `labels.jsonl` present (`IAI.Capture.DeliveryLabels` not set to 0);
- m55 change evidence on (`IAI.Capture.ChangeEvidence 1`, the default) for the independent onset witness;
- **hands off for the whole capture, so the camera stays still.** Moving-camera events are counted, not judged;
- at least 30 frames before the first event (the warm-up rule), with the capture config at the default spacing or wider.

**Method:** the `086-01` evaluator above, with the same constants, run on decoded pixels:
- it decodes only the frames in each event's span (±16);
- for the ROI metrics it decodes only the rows down to the bottom of the ROI's box, and for the wrong-object metric a
  4×-strided copy of the frame.

**Output: numbers only, to stdout and optionally `--out numbers.txt`.** No paths, names, frames or log lines.
- Header: kit version, evaluator id `086-01`, sessions read, the resolutions seen, and how many sessions were refused and
  why (JPEG, no labels, no masks).
- One block per anomaly type:
  - events: total / judged / camera-moved / warm-up / not measurable / confounded;
  - **start offset** histogram, e.g. `{-2:0, -1:0, 0:31, +1:2, +2:0}`, at t10 and at t50;
  - **end offset** histogram, same form;
  - **censored** edges, unresolved edges, gap-bounded edges;
  - **wrong-object** event count, marked as an upper bound;
  - mask-missing frames and mask-extra frames;
  - post-edge residual median and maximum;
  - m55 onset witness counts: `onset-on-first` / `label-early` / `no-change`.
- The owner reads back the offset histograms, the wrong-object count and the censored count per anomaly.

**Selftest (`--selftest`, run first; it must pass before any real number is read):** it builds synthetic sessions in a
temp folder with a stdlib PNG writer, with known answers:
- exact labels;
- onset ±1 and offset ±1;
- a delayed swap (+3);
- a one-frame 8 % reappear ghost;
- a slow drift;
- a missing mask;
- a moving camera;
- a gap next to an edge.

It asserts each reading, and it fails loudly if the kit passes a doctored label.

**Runtime:** the stdlib decoder is about 1–3 s per frame. About 50 events × 35 frames is 30–90 min, so leave it running
over RDP. With Pillow it takes 2–3 min.

**What it cannot see:**
- moving-camera events (most gameplay; hence the hands-off recipe);
- `camera_clipping` (no target mask; needs a matched null);
- targets without a measured mask (Nanite on the office host, masks refused when downscaling): bbox only, low confidence;
- effects too weak to measure (NOT-MEASURABLE; the G270 class for stuck_low_mip);
- any residual below 10 % of the effect (strict only);
- frames the FSM never captures (settle frames);
- lighting or shadow spill versus a real wrong-object change: the count is an upper bound, and an eye check of the flagged
  events is needed before calling it a bug;
- anything off-screen, such as shared-texture users outside the view.

## 9. NEEDS-DECISION (for chat)

1. **What counts as a "visible" frame (the release rule's threshold)?**
   - At the majority threshold (≥ 50 % of the effect), blinking, missing_object, missing_texture and corrupted_texture are
     **0 frames off on every measured edge**.
   - At ≥ 10 %, **4 of 592** delivered-AA blinking offsets are +1: the temporal-AA reappear frame.
   - At the noise floor, most hide-type and texture offsets show a 1–3 frame faint tail (< 10 %).
   - **Recommendation:** rule the release gate at **50 % (majority) with the 10 % and strict counts reported**, and ship
     fix sketch 5.2(a) (an additive `transition_frame` key) so the client can filter. The alternative (10 %) makes
     blinking a DESYNC on temporal-AA hosts that no label change can fix without mislabelling the other way.
   - The fraction is mean-change based; a pixel-count fraction reads the same frames at up to 77 %.
2. **The tag-pool ceiling.**
   - A capture session with more than about 55 events ships its later events with labels but **no target mask**
     (measured: 35 of 90).
   - This is m50's documented limitation. Under the new rule (labels **and** masks), is it a release blocker, a documented
     limit with a recommended session length, or a fix (release event tags after the event plus a quarantine; `mask_map`
     is already keyed by value and frame range)?
3. **`lod_popping` and `camera_clipping` have no pixel evidence at all, and a circular gate history.**
   - camera_clipping's per-frame label is a proxy with three named disagreement modes (§5.3).
   - Under the release rule neither can be called in sync by Thursday unless B-LOD / B-CC run, and B-CC needs a small
     bench lever built first.
   - The options are to schedule B-LOD and B-CC, to ship both "available, off by default, documented unproven", or to pull
     them. **The owner's call.**
4. **m53:** the shipped modes cannot be measured until 085-02 builds them. The m53 plan gates only the onset from pixels,
   so the offset needs adding (B-M53). This is the priority-order question already ruled in 085-00 ("m53 slips; sync never
   does").
5. **Minor, informational:** 68 of 480 labelled rows lacked a measured mask in one 720p stress leg with no pool
   exhaustion. The cause is not established; it will be checked if the kit or part B shows it on delivered types.

## 10. Lessons (candidate gotchas, not minted here: G-numbers diverge across branches; next free is G349 on `fix/m52-label-timing`)

- **A censoring rule can launder a desync.** Censoring an edge because an engine-frame gap sits beside it is right for
  measuring engine-frame latency, but wrong for judging a captured-frame dataset. The capture FSM puts its settle gap
  beside most edges, so the rule excused a one-frame desync that the doctored-label proof then caught. **Judge sync per
  captured frame; report gap-bounded edges separately.**
- **A far reference in a dynamic scene manufactures residuals.** On MainWorld (moving platforms, fans, orbs) a reference
  12–30 frames away drifted by more than 10 % of an anomaly's effect. Use run-local references, exclude the session-start
  settle (si < 30), and censor (never fail) an edge whose late clean frames no longer match the reference.
- **A visibility threshold must name its metric.** On the same reappear frame, a pixel-count fraction read 77 % while the
  mean change read 16 %. Faint full-object lighting shifts inflate pixel counts.
- **Lever legs are not product evidence.** 1,198 of 4,708 banked events came from legs whose levers break sync or
  measurement on purpose (ChangeGate, teleport, material retake, tag collision). Counting them would indict the product for
  the lever.
- **The bench bank cannot answer "on realistic content" by itself.** Its judged edges are mostly CB_GateLevel primitives;
  realistic content needs static-camera legs on real art (part B, then the environment pack).

## 11. Hand-off

- **Next:** chat rules on NEEDS-DECISION 1–4.
- **Then:**
  - 084-03 runs (m52 fix proof); add this evaluator to its evidence;
  - part B's B-REAL / B-LYRA / B-LOD run in a night window;
  - B-CC after its lever is built;
  - B-M53 rides 085-03.
- The office kit is specified in §8; building it is a separate brief. It reuses `ls086.py`'s constants and needs a
  stdlib PNG decoder with a selftest.
- **Nothing in this audit changes a label, a default or a binary.**