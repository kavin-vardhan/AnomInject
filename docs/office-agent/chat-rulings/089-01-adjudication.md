# 089-01 — Offline adjudication of the 29→30 Sep night bench (084-09 + 085-04)

Date: 2026-09-30 · Author: Claude Code (Opus 5.5) · Read-only on every bank: no source edit, no build, no cook, no launch, no staging.
Spec: `_reviews\night-0930-chat-ruling.md` items 3, 4 and 5 (+ 8, folded into 4). Rule applied: `_reviews\087-01-chat-ruling-office-kit.md`
decision 1 (a FAIL is an artefact only when shown on that event).

Evidence produced here:
- `_reviews\089-01-tables.py` (SHA-256 `E8B7E5E1FA5FAB9F…`): reads the banks and the two night reports, writes
- `_reviews\089-01-adjudication-tables.md` (SHA-256 `235C832FE3DA4B7A…`, 288 lines): every per-event and per-frame table cited below
  (§A B3 si 181 frame by frame, §B leg-vs-null game-time offsets, §C every stuck_low_mip event, §D blinking SYN, §E MASK55, §F arms).
- `_reviews\night-0930\089-01-gt-align.json`: the game-time-aligned null reading per stuck_low_mip event.

Inputs: `084-09-evidence\run\report.json` (E1 = `084-09-eval.py`, E2 = 086-01 rows), `085-04-evidence\run\report.json`,
`night-0930\kit\p1|p2` (KIT, per-leg totals only), banks `D:\IA_BankOverflow\M52S09_*`, `M53DC_*`, and 084-06's
`_bench_sessions_bank\M52SYNC_MASK55`.

## 0. Method, and the one new measurement

Every disputed event was read from the bank directly: labels (`labelled`, `transition`, `transition_reason`, `partial`), the
stuck_low_mip render record (`stuck_mip.render_textures`: resident vs baseline mips per texture), `mask_state`, the anomaly log,
and the pixels (the event's own target-mask union, or E1's own ROI and cached sharpness series).

**New measurement: the null aligned by game time.** E1's stuck_low_mip basis divides the leg's ROI sharpness by the null leg B5
at ONE session-wide lag in `session_index`, chosen by correlation over the whole session (`084-09-eval.py:199-241`). The
game-time offset between each leg and B5 is not constant (tables §B):

| leg | first captured engine frame | uncaptured jumps (count, frames) | t(leg) − t(B5), frames, at si 0 / 181 / 300 / 500 / 800 / 1100 | E1 basis as run |
|---|---|---|---|---|
| B3 | 1 | 41, 82 | +0 / +0 / +6 / +16 / +32 / +38 | null-ratio, lag 16 (corr 0.93) |
| B9 | 1 | 20, 40 | +0 / +0 / +0 / −2 / +0 / −2 | null-ratio, lag 6 (corr 0.96) |
| B0 (attempt 5) | 591 | 18, 36 | +254 / +250 / +252 / +250 / +250 / +250 | lag REJECTED (best −14, corr −0.07; `LAG_MAX` 30) ⇒ own-detrend |

B3 fires more often than the null (every auto-pool attempt, held or not, costs a 3-frame capture hitch), so its phase against the
null drifts through the session; B0's accepted attempt started 8.5 s of game time late (the firewall-prompt focus waits, G401).
The re-reading divides each leg frame by the null interpolated at the SAME game time `t` (gaps > 4 frames refused), per event,
with the same ROI `(17,108,100,612)` and E1's cached sharpness tables. Before B3 si 181 the two agree at lag 0 to 0.2 %
(ratio 0.997–1.000 over si 177–188) and disagree at lag 16 (0.967–1.018).

## 1. The adjudication table

Classes: ARTEFACT (shown on the event), CONFOUND (another event, the scene or the camera, shown), REAL (label/picture mismatch),
UNJUDGEABLE (no aligned reference exists in tonight's banks; neither REAL nor cleared).

### 1.1 blinking, synthetic tick order (REAL_BL_SYN): E1 FAIL 10/10; E2 and KIT 0

| event (sf) | label runs | class | evidence (tables §D) |
|---|---|---|---|
| 108 | [92,94] [98] | ARTEFACT (G395 family, G405) | run [98]: E1 depth_off 2.1 vs depth_on 47.9 ⇒ level(99) = 1.0; pixels: labelled 36.2–37.0, frame 99 2.5 |
| 134 | [114,116] [120] | ARTEFACT (G405) | depth_off 2.5 vs 44.8; level(121) 0.83; pixels 35.0–36.1 labelled, 121 2.7 |
| 160 | [136,138] [142] | ARTEFACT (G405) | depth_off 2.6 vs 47.0; level 1.0; pixels 36.7–37.7 vs 3.2 |
| 186 | [158,160] [164] | ARTEFACT (G405) | depth_off 2.5 vs 46.7; level 0.73; pixels 38.9–39.8 vs 2.2 |
| 212 | [180,182] [186] | ARTEFACT (G405) | depth_off 2.0 vs 45.4; level 1.0; pixels 37.0–38.0 vs 3.3 |
| 238 | [202,204] [208] | ARTEFACT (G405) | depth_off 2.0 vs 44.7; level 1.0; pixels 34.6–36.1 vs 2.6 |
| 264 | [224,226] [230] | ARTEFACT (G405) | depth_off 1.9 vs 45.6; level 1.0; pixels 35.2–35.7 vs 2.1 |
| 290 | [246,248] [252] | ARTEFACT (G405) | depth_off 2.1 vs 45.1; level 1.0; pixels 34.9–35.8 vs 2.2 |
| 316 | [268,270] [274] | ARTEFACT (G405) | depth_off 1.8 vs 45.7; level 1.0; pixels 35.4–36.3 vs 1.4 |
| 342 | [290,292] [296] | ARTEFACT (G405) | depth_off 2.0 vs 45.9; level 1.0; pixels 35.1–36.0 vs 1.8 |

Mechanism (`084-09-eval.py:508-519`): each label run is split at its midpoint; frames after it are scaled by `dD_off = max D over
the run's off half`. Synthetic order hides `{n, n+1, n+2, n+6}`, so the last run is ONE frame; its midpoint is that frame, its
off half contains no hidden frame, and `dD_off` is the largest post-label noise value. The noisiest post frame (b+1, 1.4–3.3 on
a 35–40 effect) therefore scores 0.73–1.0 by construction (E1's "+1" ends), and a slow whole-frame drift 13–14 frames later
crosses 50 % of that noise scale (the "+14" ends). Native order hides `{n, n+1, n+5, n+6}` (two-frame runs: the off half holds
a hidden frame), which is why NAT reads 10/10 PASS on the same instrument. It is G395's shape (the judged frame sets its own
scale), not its letter (the frame is not in the reference set); E2 and KIT read 0 on every event and the pixels agree with them.
**Blinking SYN has no label error.**

### 1.2 stuck_low_mip B9 (AA off): E1 PASS 16/16; E2 FAIL 5; KIT 0

| event (label) | class | evidence (tables §C B9) |
|---|---|---|
| 120–130 | CONFOUND (scene, in 086-01's own-run reference) | E2 onset partial frames 120–122 inside its transition set (`linv_outside_transition = []`); its FAIL is `unl_without_transition` +8..+16 with its own note "scene drift 0.549 of the effect"; aligned null: 0.07 of depth at +1, none ≥ 50 % after the label |
| 188–198 | CONFOUND (same) | E2 drift note 0.624; aligned null ≤ 0.05 at +1..+12 |
| 460–470 | CONFOUND (same) | E2 drift 0.457; aligned ≤ 0.07, strict residue to +8 |
| 528–538 | CONFOUND (same) | E2 drift 0.555; aligned ≤ 0.04 |
| 800–810 | CONFOUND (same) | E2 drift 0.40; aligned ≤ 0.06 |

**086-01 is NOT partial-blind here**: on all five its labelled-not-visible onset frames are the partial frames and sit inside
its transition set; every FAIL is on the offset, where 086-01's own reference moved (MainWorld's rock sharpness cycle, G349/G396)
and it says so. With the null aligned, AA-off offsets are exact on all 16 events (post-label residue ≤ 0.18 of depth on the
warm-up event, ≤ 0.08 elsewhere).

### 1.3 stuck_low_mip B0 (TSR): E1 PASS 6 / CENSORED 12; E2 FAIL 3; KIT 0 fail, 12 censored

| event (label) | E1 | E2 | class | aligned null (TSR window W = 8): < 50 % at / ≥ 50 % past W / strict last |
|---|---|---|---|---|
| 19–29 | CENSORED (drift +0.177) | WARMUP | CONFOUND (scene, own-detrend) → PASS aligned | +5 / none / +24 |
| 155–165 | CENSORED (−0.184) | PASS(te) | CONFOUND → PASS | +6 / none / +28 |
| 223–233 | CENSORED (−0.133) | CONFOUNDED | CONFOUND → PASS | +5 / none / +21 |
| 291–301 | CENSORED (+0.143) | CONFOUNDED | CONFOUND → PASS | +7 / none / +17 |
| 359–369 | CENSORED (+0.212) | PASS(te) | CONFOUND → PASS | +6 / none / +26 |
| 495–505 | CENSORED (−0.149) | PASS(te) | CONFOUND → PASS | +7 / none / +19 |
| 563–573 | CENSORED (−0.167) | **FAIL** | CONFOUND (E2 drift note; E1 drift) → PASS | +5 / none / +25 |
| 631–641 | CENSORED (+0.081) | CONFOUNDED | CONFOUND → PASS | +5 / none / +13 |
| 699–709 | CENSORED (+0.248) | PASS(te) | CONFOUND → PASS | +7 / none / +26 |
| 835–845 | CENSORED (−0.133) | PASS(te) | CONFOUND → PASS | +8 / none / +20 |
| 903–913 | CENSORED (−0.196) | **FAIL** | CONFOUND → PASS | +6 / none / +27 |
| 1039–1049 | CENSORED (+0.303) | **FAIL** | UNJUDGEABLE | t = 44.2 s; the null ends at 41.3 s |

The other six (87–97, 427–437, 767–777, 971–982, 1107–1118, 1175–1185) are E1 PASS; the last three are also beyond the null.

### 1.4 stuck_low_mip B3 (auto-pool, TSR): E1 FAIL 1 / CENSORED 4 / PASS 3; E2 FAIL 2; KIT 0 fail, 5 censored

| event (sf, label) | E1 | E2 (transition-aware) | class | evidence |
|---|---|---|---|---|
| 181, 189–198 | **FAIL** (one t50 frame, si 214) | CONFOUNDED (drift 0.80) | ARTEFACT (E1 null misalignment, G406) over a real TSR decay | §2 below; aligned: < 50 % at +6, none past +8 |
| 503, 485–494 | PASS | **FAIL** (drift 0.687) | CONFOUND (scene, in E2's reference) | aligned: < 50 % at +6, none past +8, strict last +23 |
| 1119, 1061–1070 | CENSORED | PASS(te) (status FAIL) | CONFOUND (E1 lag-16 drift censor) → PASS | aligned: < 50 % at +6, none past +8 |
| 1232, 1167–1176 | CENSORED | **FAIL** (+9, +10; drift 0.379) | UNJUDGEABLE | t = 41.7 s, beyond the null; last event, 23 frames of session after it |

The other E1-CENSORED events (55–62, 121–130, 849–858) read < 50 % by +5..+7 on the aligned null.

### 1.5 The RestoreDelay / CommitDelay arms (m53, 085-04)

| arm | E1 | E2 | KIT | truth (direct pixels, tables §F) | class |
|---|---|---|---|---|---|
| RD3 AA off (7 events) | start −14 / end −4 | end +1 (FAIL 3, CONFOUNDED 4) | end +1 on 5 | the revert-tick frame (si 33 of event 2) still corrupted (46.53), si 34 clean ⇒ **end +1** | E1 = ARTEFACT: end −4 is G395 exactly (its offset reference `[b+1..b+6]` starts with the delayed frame); start −14 is the PREVIOUS event's delayed frame inside this event's span |
| RD3 AA on (7) | 1 FAIL + 6 CONFOUNDED | end +1 (FAIL 5, CONFOUNDED 2; start −14 same contamination) | 1 judged, censored; post-edge residual 1.000 | end +1; the m53 events carry no transition flag, so TSR does not excuse it | can-fail PROVEN on E2; KIT saw the delayed frame at full strength and censored rather than judged |
| CD3 AA off / on (7 + 7) | onset +1 | onset +1 | onset +1 | apply-tick frame si 25 labelled and clean (0.33); si 26 (engine 35 = commit) corrupted ⇒ **onset +1** | all three correct; the +3 prediction was in the wrong unit (§6) |

### 1.6 The other disagreement row: SR_B0 / SR_B3 / SR_B9 onset

E1 prints the RAW onset (+2/+3, the partial frames, every one flagged `partial` and backed by the render record); KIT prints the
transition-aware onset (+0). No instrument FAILs these legs; nothing to adjudicate.

**Across every adjudicated event: 0 REAL.** 10 + 1 artefacts shown (blinking SYN, B3 si 181), 7 RD3 E1 artefacts shown,
17 confounds shown with the aligned null, 2 UNJUDGEABLE (B0 1039–1049, B3 1167–1176: beyond the null's game-time range).

## 2. stuck_low_mip B3 si 181: the 41-frame tail

Per-frame table: tables §A (si 185–245: render record per texture, label, transition, mask_state, E1-basis and aligned-null drop,
null and leg sharpness).

| question | answer | evidence |
|---|---|---|
| Is the blur still resident after the label ended? | **No.** | Render record: si 189–190 N 11/11 baseline with D and AORM held 7/11 (the partial onset, flagged); si 191–198 all three 7/11 held; **si 199–206 all three 11/11 baseline** (trailing, unlabelled, `transition_aware` flagged). Log: revert at engine 210 (si 199) with the streamer fence flushed; `RESTORE VERIFIED` (game-thread read-back) D and AORM at engine 214 (si 203), N at 216 (si 205); `RESTORE TRAIL CLOSED` at si 208 (baseline on 2 consecutive frames, no stream operation pending); the watch stayed on to si 239 with 0 reopens; detached at si 241 when the next fire began. Same rock, AA off (B9, 16 events): the offset is exact. |
| Is another anomaly on or near the target? | **No.** | B3's pool is stuck_low_mip only. Between si 189 and 240 no other fire applied anywhere: the attempts at engine 251 (BP_SpawnPad_C) and 293 (RoomBuilderSquare_C) were `HELD NONE … not applied`. |
| The target mask | present 189–198 (34,599–34,915 px); `empty` 199–239 (measured; unlabelled frames carry no mask by the m44 rule); `unmeasured` from 240 (after the detach) | labels rows |
| Pixel change in the target vs the scene | the rock's sharpness against the null at the SAME game time decays smoothly: **0.81 of depth at +1, 0.36 at +8, 0.14 at +16, 0.02 at +32** (strict noise at +20). Before the event the two legs agree to 0.2 %. Both legs' rock sharpness rises ~11 % over si 199–239 (null 3.96 → 4.41): the scene cycle (G349). | tables §A, §C |
| Why E1 read a 41-frame tail and a t50 FAIL | E1 divided by the null **16 frames later in game time** (its session-wide lag); over si 207–228 that null value is climbing, which manufactures the plateau and re-rise (0.40 → 0.56 of depth) and the single t50 frame (si 214, 0.507 of depth vs the 0.50 threshold). | §0; tables §A |

**Finding: ARTEFACT (E1's null alignment), not REAL and not CONFOUND.** The texture was back at baseline from si 199 (render
record) and verified by si 205 (read-back); nothing else was active. What remains in the picture after the label is temporal-AA
(TSR) history: below 50 % of the effect from +6, i.e. inside the product's 8-frame off-transition window, and above the noise
floor until about +20. Every TSR stuck_low_mip event on the aligned null has the same shape (0.75–0.86 at +1, < 50 % at
+5..+8, strict last +13..+29); that residue is sub-threshold by the release rule and is a policy question (§9), not a desync.

## 3. B0: why 12 offsets were censored, and the leg change

**Why.** B0's accepted attempt (A5) began capturing at engine frame 591 (8.5 s of game time late), after the firewall prompts
held focus through attempts 1–4 (G401). The null B5 starts at frame 1, so the true offset is ~250 frames; E1 searches ±30
(`LAG_MAX`) and rejected every lag (best −14, corr −0.07). E1 fell back to `own-detrend` (two-point interpolation between the
pre and post levels), which cannot remove MainWorld's rock-sharpness cycle (G349, G396): the post/pre level moved by
−0.20 … +0.30 on 12 events, beyond `max(thr_off, 25 % of depth)`, so each offset was censored ("the picture does not return to
its pre-event level"). KIT (no null by design, G396) censors the same 12 for the same reason. Nothing about TAA itself prevents
the judgement.

**What makes them judgeable — measured on tonight's bank, no re-run needed:** with the null aligned by game time, 11 of the 12
censored offsets are judged, and all 11 (plus the 3 in-range E1 PASS events) fall below 50 % of depth by +5..+8 with no frame
≥ 50 % after the 8-frame window (the release rule's PASS). The 12th and three E1-PASS events lie at t 41.9–48.9 s, beyond the
null's 41.3 s.

**Concrete leg change for the re-bench:**
1. **Evaluator:** replace the session-wide `session_index` lag with per-frame game-time alignment of the null (interpolate the
   null's series at the leg frame's `t`; refuse gaps > 4 frames; no correlation search). Apply to E1's stuck_low_mip basis and
   offer the same to the kit when a null is present. Re-evaluate tonight's B0/B3/B9 banks with it before re-running anything.
2. **Null coverage:** the null must span the leg's whole game-time range. Capture B5 with `MaxFrames` 1500 (leg 1200 + 300),
   run it directly after the stuck_low_mip legs, and gate each leg on "null covers t(last label) + 2 s".
3. **Start alignment:** Part 0 runs after the bench legs (already ruled), so every leg starts at engine frame 1 as the null
   does; list and close `Shell_SystemDialog` windows before the window (G401).
4. **Post-window:** not needed for the release rule (every judged TSR event is below 50 % by +8); `SPAN_AFTER` 60 already
   covers the strict residue (+13..+29) if chat wants that read too.

## 4. MASK55 v2: 270 of 442 labelled frames without a measured mask

| target | events | labelled frames | mask_state | target_pixels | product line on every event |
|---|---|---|---|---|---|
| rock `StaticMeshActor…2044254803` | 37 (blinking 13, corrupted 9, missing_object 4, missing_texture 9, stuck_low_mip 2) | **172** | present | > 0 | measured |
| `BP_SpawnPad_C` | 21 | 98 | unmeasured | −1 | `Capture(m50): UNMEASURABLE TARGET … reason=nanite … no arm is issued` |
| `RoomBuilderSquare_C` | 19 | 98 | unmeasured | −1 | same, reason=nanite |
| `SM_Ramp2` (the known-Nanite control, G134) | 17 | 74 | unmeasured | −1 | same, reason=nanite |

By type: corrupted 102, missing_texture 96, blinking 36, missing_object 36 unmeasured; stuck_low_mip 0 (it can only hold the
rock). `run_summary`: `unmeasurable_targets_admitted 57` (= 21 + 19 + 17 events), `census_unmeasurable_nanite 29`,
`target_mask_frames_measured 172`, `mask_tag_recycles 25`, `exhausted 0`, `prior_collision 0`, `nopass_discards 0`,
`vetoed_events 0`.

**Cause: the designed m50 ruling-3 path, not G295, not the bench lever, not a regression.**
- G295 (a coalesced mask arm) would log `TARGET MASK UNAVAILABLE`: **0** such lines in this leg.
- `TagPoolLimit 12` did bite (25 recycles), but the rock, the only taggable target, is masked on **172 of 172** labelled frames
  across those recycles, with 0 exhaustion and 0 prior collisions.
- **Not a regression:** 084-06's MASK55 (586/586 masked) ran on `/Game/CaptureBenchGate/CB_GateLevel`, whose targets are
  non-Nanite engine basic shapes (`unmeasurable_targets_admitted 0`). v2 moved the leg to MainWorld, where three of the four
  targets the auto-pool picks are Nanite and are admitted unmeasured by design (m50, since `7ecdf5f`). The gate
  "every labelled frame masked" was therefore unattainable as written on this map; restated: every labelled frame on a
  MEASURABLE target is masked (172/172 PASS) and every unmeasured frame carries an m50 reason (270/270 `reason=nanite`).

**Peak live 1, and a route to ≥ 2 the product allows (G366).** A capture fires one anomaly per burst; only mask RECORDS
overlap, and only when a stuck_low_mip restore trail is still unreleased when the next event's tag is allocated (the tag is
allocated at the fire, `M36 event tag allocator tick=594 assigned=208`). Measured across every banked leg (tables §E):

| leg shape | legs | peak live 2 | recycles |
|---|---|---|---|
| targeted stuck_low_mip on the rock (B0, B5, B9 and their 084-06 / 085-04 twins) | 15 | **11** (4 read 1) | 0 on all |
| auto-pool (B3 ×3, MASK55, MASK55 v2) | 5 | 0 | 64 and 25 on the MASK55 legs |

No leg has yet had both conditions at once. v2's two stuck events were each followed by a detach in the same tick as the next
fire, with nothing left in flight (`closedAt 612, lastNoted 617, detached 619`), so the overlap never formed.

- **Route A (no source change, recommended for the re-bench):** MASK55 v3 = B0's recipe (targeted `stuck_low_mip` on the
  rock, MainWorld, 18 events) + `IAI.Bench.TagPoolLimit 12`. Validity (pre-declared, independent of the verdict):
  `mask_tag_peak_live >= 2` AND `mask_tag_recycles >= 1`; below either the leg is INVALID and re-run (measured hit rate for the
  concurrency half: 11 of 15). Gate: 0 shared TAG-OWNERS frames, exhausted 0, prior collisions 0, every labelled frame masked
  (the rock is measurable). ⚠ Declared unknown: recycling beside two live records has never run; if peak live falls to 1
  whenever recycles > 0, that is a finding, not a pass.
- **Route B (deterministic, needs a build):** a bench-only lever that keeps a record unreleasable for N frames after its fire
  ends, so any leg (including mixed auto-pool) overlaps records on demand. Chat's call.
- **Route C (auto-pool realism, no concurrency claim):** v2 as run plus `IAI.SetExcludedTargets BP_SpawnPad,RoomBuilderSquare,SM_Ramp`
  (the product's exclusion list) makes "every labelled frame masked" attainable for recycling/aliasing; by G366 it will keep
  reading peak live 1 (0 of 5 auto-pool legs reached 2).

## 5. lod_popping `worst=2` on a two-LOD mesh

| question | answer | source |
|---|---|---|
| What LOD is forced? | `AnomalyLod::GetWorstLod` returns the mesh's LOD **count** (2) and `SetForcedLod` writes it to `UStaticMeshComponent::SetForcedLodModel`. `ForcedLodModel` is **1-based** ("If 0, auto-select LOD level. if >0, force to (ForcedLodModel-1)", `StaticMeshComponent.h:187`; `SkinnedMeshComponent.h:528` the same; `SetForcedLOD` "Range from [1, Max Number of LOD]"). So `worst=2` forces **LOD index 1, the last LOD**. `AnomalyLod::GetCurrentLod` already decodes it as `ForcedLodModel - 1`. | `AnomalyLod.cpp:32-74,86-88`; `Anomaly_LodPopping.cpp:176-181, 207-209, 253` |
| Does the engine clamp it? | It would (`Clamp(ForcedLodModel, ClampedMinLOD + 1, NumLODs) − 1`, `StaticMeshRender.cpp:1167, 2322, 2361`), but 2 is in range, so nothing is clamped. | engine 5.1 source |
| Does the label depend on it? | **No.** capture reads lod_popping from `AnomalyState` (`AnomalyCaptureSubsystem.cpp:553`) = `IsCurrentlyAnomalous() = bActive && bPoppedPhase` (`Anomaly_LodPopping.h`). | source |
| Do the pixels? | Yes: the popped phase renders LOD 1 against the baseline `ForcedLodModel 0` (auto, which the log predicts as LOD 0 at screen size 0.4466). On the fixture that is the full LOD0↔LOD1 contrast once the scene is lit (G402). | log |

**Finding: not a bug in the forced LOD — a unit mix in the log line**, which prints the 1-based `ForcedLodModel` as `worst=`
beside a 0-based `level=`. The `REFUSED … forcing LOD %d onto it` warning has the same mix. **Fix (wording only, no behaviour
change, 089-02):** print `forced_lod_model=2 (LOD index 1 of 2)` in both lines. Side note, unchanged by this: the label is the
popped PHASE, not a pixel test; a pop that renders identically (e.g. a platform `MinLOD` equal to the last LOD) would still be
labelled. Auto-pool guards it (single-LOD, coverage and highest-LOD gates); a targeted fire only warns.

## 6. The arm sizes

| lever | unit | what the capture does around it | captured-frame effect | measured |
|---|---|---|---|---|
| `IAI.Bench.TexCorruptCommitDelay D` | engine frames (`CommitDueFrame = GFrameCounter + D`, `Anomaly_TexCorrupt.cpp:1113, 1224`) | the apply tick is captured and labelled; the next **K = 2 settle ticks are not captured** (frame_index jumps by 3 after every apply) | onset = **1 + max(0, D − 1 − K)** ⇒ D = 3 → **+1**; +3 needs D = 5 | CD3: apply engine 32 = si 25 (labelled, clean 0.33); commit engine 35 = si 26 (46.53) ⇒ +1 on 7/7, both AA modes, all three instruments |
| `IAI.Bench.TexCorruptRestoreDelay D` | engine frames (`RestoreDueFrame = GFrameCounter + D`, `:1279`) | the revert tick is captured and unlabelled and normally already clean (TILE control si 33 = 0.27); then K = 2 uncaptured settle ticks | offset = **1 + max(0, D − 1 − K)** ⇒ D = 3 → **+1**; +3 needs D = 5 | RD3: revert engine 42 = si 33 still corrupted (46.53); restore due engine 45 = si 34 clean ⇒ +1 |

**Why RD3 reads three ways:** E2 and KIT (AA off) read the truth, end +1. E1 reads end −4 because its offset reference
`[b+1..b+6]` begins with the delayed frame (G395, the documented shape) and start −14 because the previous event's delayed frame
(b_prev + 1) lies unlabelled inside this event's span; E2's start −14 is the same span contamination. With AA on, the m53 events
carry no transition flag, so the +1 is not excused; KIT judged one edge, censored it and printed post-edge residual 1.000 — it
saw the delayed frame at full strength.

**Do the arms still prove can-fail? Yes, at +1.** CD3 FAILs on all three instruments in both AA modes (7/7); RD3 FAILs on E2
(both modes) and KIT (AA off) with end +1 on every event; NOAPPLY1 FAILs. The miss was the prediction's unit (engine frames
read as captured frames), not an instrument. For the re-bench: declare arm predictions in captured frames with the formula
above (or set D = 5 for a +3 arm), and patch E1 for G395 before reading RD magnitudes.

## 7. Proposed fixes for 089-02 (none made here)

| # | where | fix |
|---|---|---|
| 1 | `084-09-eval.py` (E1) | G405: for a run whose off half holds no labelled frame, scale by the run's `depth_on` (or the event depth), never by its own off-half noise |
| 2 | E1 | G395 (already owed): build each offset reference from frames that exclude b+1 when b+1 is under test; bound each event's span at the previous event's last labelled or transition frame + its own settle, so a neighbour's delayed frame cannot enter |
| 3 | E1 (+ kit when a null exists) | G406: game-time null alignment per frame (§3.1) |
| 4 | leg list | B5 null 1500 frames, run after the stuck legs; null-coverage gate; Part 0 last (§3) |
| 5 | leg list | MASK55 v3 per Route A (§4); v2's "every labelled frame masked" restated per measurable target |
| 6 | 085-04 lock / README | arm predictions in captured frames: `1 + max(0, D − 1 − K)`; CD5 / RD5 for a +3 arm (G407) |
| 7 | `Anomaly_LodPopping.cpp` (wording) | `forced_lod_model=N (LOD index N−1 of N)` in the CURRENT-LOD and REFUSED lines (G408) |

## 8. What this does NOT establish

- The two UNJUDGEABLE events (B0 1039–1049, B3 1167–1176) are not cleared; they need the longer null.
- The aligned-null reading is a NEW method applied after the results were seen. It is offered for chat's acceptance; nothing in
  this document changes a recorded verdict, and tonight's reports stand as recorded.
- No mechanism is claimed for why TSR keeps 5–45 % of a stuck_low_mip blur for 13–29 frames after restore; that it does is
  measured on 22 aligned TSR events and absent on 16 AA-off events.
- Route A's recycling-under-concurrency interplay is unmeasured.

## 9. NEEDS-DECISION

1. Accept the game-time-aligned null as the stuck_low_mip basis (E1, and the kit when a null exists), and the per-event
   classifications that rest on it (17 confounds, B3 si 181).
2. The TSR residue after a stuck_low_mip label: below 50 % from +5..+8 (inside the 8-frame off window), above noise to +13..+29.
   Keep the t50 release rule as the acceptance line, or extend `IAI.Label.TransitionOffFrames` for stuck_low_mip under TSR?
3. MASK55 concurrency: Route A (no build, ~11/15 per attempt) or Route B (bench hold lever, a build).
4. Arm predictions: re-declare in captured frames (and/or CD5/RD5), or change the levers to count captured frames.
