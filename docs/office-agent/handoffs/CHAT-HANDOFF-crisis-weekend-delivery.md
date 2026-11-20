# CHAT HANDOFF — Crisis Weekend → Monday Delivery (Fri 2026-08-21 → Sun 2026-08-23)

**Read me first.** This chat ran the entire delivery crisis: m31 fallout → tick pin → material fix → measurement instrument → five pre-delivery change sets → two explainer videos → and it ends MID-INVESTIGATION on a sped-up-video defect that is currently **blocking packaging**. A fresh chat picks up at §1.

**Delivery deadline: MONDAY MORNING. Client captures immediately on receipt.** Owner has office access all weekend; incremental cook on the client host ≈ 5 minutes, so iteration is cheap. Office box is SEALED (nothing leaves it; owner types commands manually, reads screens back).

---

## 1. THE OPEN INVESTIGATION — sped-up videos (packaging is paused on this)

### Symptom (owner-observed, client host, packaged, TickPin 1, clock=wall)
- Two targeted lod_popping captures → output MP4s play **very visibly sped up** (character movement / gun sway).
- Artifact numbers: `video.fps=30`, `target_fps=30`, `total_frames=120`, MP4 duration 4.0 s — internally consistent.
- BUT owner wall time was ~7–9 s for that capture; an ALL-anomalies run took **~23 s** for the same 120 frames (≈5.2 fps true rate, ≈5.75× compression).
- `speed_ratio` read **1.000x on every one of these runs, including the 23-second one.**

### Candidate diagnosis (Code's, PROVISIONALLY ACCEPTED — candidate, NOT established)
- `speed_ratio` = WallSpan/GameSpan (GameSpan from `World->GetTimeSeconds()`). Capture sets `FApp::SetUseFixedTimeStep(true)` + `SetFixedDeltaTime(1/30)` at BeginActualRun (~:1158-1160).
- **On the pinned fork, the engine evidently does NOT honor bUseFixedTimeStep** → game dt = wall dt → the ratio compares a clock to itself → **1.000 by construction**. The m11 honest-fps stamp (which would have shipped an honest ~5 fps video) triggers off this same ratio → **structurally blind on this host**. Same invariant-violation shape as m31: a guarantee resting on engine-global behavior a host can redefine.
- The stall itself = the **m26/m27 mask measure pass** (owner's bisect already proved: empty pool no hitch; full pool hitches during windows; Mask 0 → hitch gone on SOB). At client's 3200×2000 it costs ×6.94 the certified 720p cost. **Mask stretches wall time; blinded stamp compresses the video back. Two documented limitations composing into one very visible defect with a clean health report on top.**

### Verification owed — THE OWNER STILL HAS NOT DELIVERED THESE NUMBERS (first ask of the new chat)
From the sped-up session:
1. `labels.jsonl` — lowest and highest `session_index` rows (**rows are NOT in file order — G162 — sort/key by session_index**): `t` and `t_wall` from each.
2. `run_summary.json` — `ticks_per_captured_frame`, `capture_game_ticks`, exact `speed_ratio`.
3. Photo of the StartRun run-config echo line for that run.
4. **Yes/no: was any clock override active on those runs?** (Owner had just been experimenting with `-ini:` overrides / asking about content-clock commands. If YES → diagnosis re-rules from scratch.)

Pre-registered readings (fixed before numbers land):
- `t` deltas ≈ `t_wall` deltas (~0.19 s avg, spiky inside windows) → **confirms** blinded-ratio mechanism.
- `t` deltas exactly 0.0333 while `t_wall` spiky → fixed step WAS honored → mechanism **refuted**, re-read run_summary before concluding anything.
- `ticks_per_captured_frame` ≈ 1.2 expected (ticks didn't multiply; each took longer). A value near 5.7 → different mechanism entirely.
- Clock override was active → stop; chat re-rules.

### In flight at Code (report pending) — addendum Q4–Q7
- Q4: confirm encoder passes flat framerate, ignores timestamps (name the line in encode_watcher).
- Q5: fix options, LISTED not designed, each with cost/cook/one-leg validation: (i) re-key honest stamp's GameSpan off **plugin-owned armed-frame count × (1/VideoFps)** — m31's cure shape; must be no-op on stock hosts; validation leg = starved run ships honest ~5 fps video playing at correct speed. (ii) encoder-side honest remux from `labels.jsonl` t_wall span (no-cook belt-and-braces; `annotation.video.fps` will disagree — named caveat). (iii) 720p resolution lever — **on mask-cost grounds ONLY** (see correction below), client-visible, owner DECIDE required.
- Q6: is this Issue-2 family via starvation instead of clock choice; did the OLD unpinned path's ratio see what the pinned path cannot.
- Q7: **what did the removed Native/30/60 mode actually do** (git archaeology, citations). Gates BOTH this diagnosis AND the owner's new feature request (§5). Old Native produced ~0.5× slow-motion; parked note mandates the revisit question "did Native turn pacing off or is something forcing 0.5× game clock", NOT machine perf.
- Plus: honest paragraph on whether the client's EXISTING deliveries look like this (owner's 5 fps is **RDP-confounded**; her local sustained rate is UNKNOWN).
- Journal 054 §8 correction drafted (wording in report), committed WITH the eventual fix, not before.

### Likely ruling shape once numbers + report land (chat's prior stated intent)
Re-key fix (one cook, one validation leg) + encoder remux as backup. **Monday stays reachable.** The DATASET is not implicated — label↔pixel alignment measured at +0 on this host at 3200×2000; the defect is the review video only.

### Corrections already on the record from this thread
- **"The pin is a performance win (1.00 vs 1.36)" is DOWNGRADED.** The pinned 1.000x readings are clock-agreement readings, not capture-health readings. The strongest proof: 23 s starved run, pinned, ratio 1.000. The unpinned rows (1.3627/1.6568) remain meaningful (real clock divergence) — so the **P1 candidate note survives**: her original ~1.2-ratio one-frame offset vs her host running 1.36–1.66 unpinned; candidate, not claim, not to be investigated.
- **Operating rule on Concorde from now on: capture health = stopwatch vs frames÷fps. NEVER speed_ratio.**
- Code's "720p is the only certified resolution" is **STALE** — this weekend's PINNED2 leg measured +0 at 3200×2000, HIGH confidence, ±7 ceiling. Resolution lever survives on mask-cost grounds only.

---

## 2. STATE OF THE DELIVERY (everything except §1 is DONE and validated)

### The original crisis, resolved
- **Blocker (variable 1–6 frame label offsets on Concorde): FIXED and validated.** Root cause: the fork's fixed-sim/variable-render decoupling. Fix = **tick pin** (forces mode off during capture, restores after). Validated packaged, in-round, `TICKPIN active saved=1`: 300 frames, 7 events, 5 measurable, **every offset +0 start and end**, 4/5 HIGH confidence, ceiling ±7 (complaint was 1–6 → instrument could see it and found nothing).
- Unpinned control leg was **inconclusive** (ran short 290/300; only 2/4 measurable; did NOT reproduce 1–6). The A/B is weak; the pin's validation rests on the pinned leg + owner's direct Thu-vs-Fri observation. Recorded so nobody later claims a clean A/B.
- **Magenta/checker dual appearance: FIXED.** Root cause: missing `bUsedWithStaticLighting` (+`bUsedWithClothing`) usage flags → engine substituted WorldGridMaterial in cooked builds on statically-lit meshes. Fixed broadly (7 flags, scripted, both materials). Concorde grep post-fix: **zero** `missing bUsedWith` hits; classifier reads magenta 8/8. **Prior Concorde-captured deliveries contain WRONG-APPEARANCE samples** (grey grid where magenta intended, static-mesh targets only; skeletal was fine) — client-notes item, remediation not chased.
- Rogue editor-only camera frames: parked (absent in packaged).

### Shipped this weekend (all pushed; plugin repo, latest first — verify with git log, CLAUDE.md status block is STALE by design)
- Session 054: `aeb1930` docs · `befca64` **auto-pool lod_popping requires highest-LOD candidates** (restrict-not-prefer; per-candidate CURRENT-LOD log; `IAI.Anomaly.LodRequireHighestLod 0` = off-switch, no re-cook; targeted mode warns-never-blocks) · `4ec07ed` **tunable LOD coverage** (`IAI.Anomaly.LodMinCoverage`; compiled default stays calibrated 7.0; ⚠ **do not tune below 6.0** — general selection gate at 6% binds first and the refusal is silent/unlogged).
- Session 053: toggling anomalies label the **active subset** (gapped frame_indices — lod_popping went 8 labeled → 3 actually-popping; 62.5% of positives were false before; generalized table `ResolveAnomalyActiveSource`, flickering = one new row) · targeted fire bypasses auto-pool distance+coverage gates · node.bounds from rendering components only (bbox_px/coverage did NOT move — my "calibration voided" claim was WRONG, Code measured) · asset names in dashboard (primary label, UAID dimmed) · proximity-triggered targeted camera_clipping (walk close → clips; F-LABEL guard covers never-approached).
- Earlier: tick pin + console override + probe hardened (probes App.h for the fork symbol itself; `ANOMINJECT_TICKPIN_FORCE_ON/OFF` marker files as build-time override) · material usage flags · measurement instrument in plugin `tools/` · exclusion patterns (`+ExcludedTargetNamePatterns`, matches actor/component/ASSET name) · lod distance gate · labels.jsonl now written in delivery mode · overlay tool ships in delivery (red=shipped, amber=candidate w/ category; auto-launched by Run.bat; Pillow preflight) · measurement-ceiling banner + `--require-gap`.
- **Pending at Code (same brief as §1 addendum? NO — separate earlier brief, may already be done):** camera_clipping OUT of default-checked pool (stays available; 4 default-checked: blinking, missing_texture, corrupted_texture, lod_popping) · client-readme pool defaults · **overlay writes ONLY frames that carry boxes** (preserve original frame numbers — NEVER renumber; amber-only frames still written; summary states N/M/T; check (a) nothing assumes 1:1 with frames dir, (b) whether any video is built from overlay dir — if yes STOP, chat rules). If that report hasn't landed, chase it.

### Owner test results on the client host (all six session-053 tests PASSED)
Gapped LOD labels confirmed on his content · targeted bypass works both directions · bounds unchanged where expected (BP_SpawnPad was the real fix; dashboard viewport boxes are a DIFFERENT source — see §4 known issues) · asset names work (after two-clone mixup) · targeted camera clipping fires + labels accurate (⚠ due to a name mismatch he tested WITHOUT `=` prefix, substring match — exact-match `=` path with the ACTOR name technically unverified) · skeletal-lag question resolved: his late-overhang object labels correctly; the ~3-frame overhang is TSR/TAA temporal smear, nothing to fix.

### Hitching: resolved to a DOCUMENTED LIMITATION (owner ruling)
Mask pass stalls armed frames (readback is non-blocking render-thread; cost = full-screen pass + view-rect copy + W×H render-thread scan, ×6.94 at her res). **Owner ruled mask STAYS ON** (a Mask-0 run readmitted many invisible objects) over chat's recommendation. Client-notes paragraph drafted in-thread (search "During capture, frames containing an anomaly may show a brief stutter"). Post-delivery cure filed: **GPU-side per-tag reduction** (~1 KB readback; latency free since veto only needs FinishRun).

---

## 3. CONCORDE CONFIG + REMAINING PRE-PACKAGE CHECKLIST

`Config/DefaultGame.ini` before the final cook (owner-set; NOT compiled defaults — 7.0/200 stay calibrated in code):
```
[AnomalyInjector]
LodPoppingMinCoveragePct=7.5
LodPoppingMaxDistanceCm=300
CameraClippingTriggerRadiusCm=300
+ExcludedTargetNamePatterns=lightblockerplane
+ExcludedTargetNamePatterns=LocalVolumetricFog
[AnomalyCapture]
bMaskMeasureDefault=True
bDeliveryModeDefault=True
```
⚠ The ini route for the two LOD keys has NEVER been proven (G88; only console path proven at home). **After the cook, read the StartRun echo: `lodMinCov=7.5000%(ini)` and `lodMaxDist=300cm(ini)`. `(compiled)` = key didn't land.**

Checklist still owed before packaging: §1 resolved → final cook → the two `(ini)` echoes → one pinned validation leg (stopwatch-checked!) → **README-only dry-run** (owed since m27, NEVER done: unzip bundle to clean folder, follow README verbatim, launch, connect dashboard, ~30 frames — likeliest Monday-loser) → **two-clone cleanup** (stale trees bit TWICE: make_delivery, dashboard asset names; `D:\AnomDashboardV1\...` vs `D:\AnomDashboard\...` — confirm which is live, kill the rest) → `make_delivery.bat` 11/11 (note: **packaging script rework was briefed** — bundle must build from dashboard repo alone, `--plugin-repo` opt-in, plain notice of the 2 files owner supplies; if Code's report on that landed, use the new form) → client notes.

Client-notes items accumulated: hitching paragraph (drafted) · prior deliveries contain wrong-appearance corrupted_texture samples (static meshes) · delivery-mode captures cannot self-verify offsets (measured: FULLFRAME degrades to wrong answers; verification is always delivery-OFF) · camera_clipping = whole-session global; held weapon always clipped (expected) · labels.jsonl rows unordered — sort by session_index; frame_index is GFrameCounter, never join on it · overlay amber semantics (90.1% span-vs-subset BY DESIGN, 9.9% vetoed) · sped-up-video resolution per §1 outcome · tick pin behavior note (changes engine mode during capture, restores after).

---

## 4. KNOWN ISSUES / OPEN THREADS (not Monday-blocking)

- **Dashboard viewport boxes still oversized for spline/instanced meshes** — G124: their OWN component bounds are huge; the P6 fix didn't touch this source (boxes come from first renderable-visible component's bounds, not node.bounds). Real fix = per-instance bounds. Post-delivery.
- **Targeting-string mismatch**: dashboard now displays ASSET names but `=exact` targeting matches ACTOR names → `'=WLD_LrgCargo_B_SM' -> 0 matched`. Dashboard click-targeting sidesteps it (passes actor internally). Fix direction: accept asset names in targeting. Post-delivery.
- **Dashboard/annotation asset-name rules differ** for multi-mesh actors (snapshot = first renderable-VISIBLE; annotation = first visible with non-empty asset). Recorded, deliberately not unified.
- **lod_popping has NO automated offset oracle** — pop changes ~3% of bbox, under the measurement script's floor; it read UNMEASURABLE before AND after the fix (why Friday's clean validation missed the bug — G135 shape). Verification = direct in-bbox strong-pixel series (journal 053 §1). Extending the script = separate gated change.
- **Highest-LOD prediction fidelity**: ignores LODDistanceFactor / r.StaticMeshLODDistanceScale / MinLOD; can disagree with renderer; mitigated by per-candidate logging. Filter's real cost on moving-player content UNMEASURED (bench can't walk).
- Positive branch of GLOBAL camera_clipping determinism rests on structural argument (bench has no near-wall pose) — declared, G146 shape.
- `IAI.Capture.Start` arg positions: fps believed at position 5 (`... png 777 120 15`) — **verify via StartRun echo before trusting**; args 6+ go to the anomaly parser.
- LOD viz in PIE: `viewmode lodcoloration` / back with `viewmode lit` (coloration = renderer truth; our filter = prediction; disagreement is information).

## 5. OWNER FEATURE REQUESTS — LOGGED, SEQUENCED POST-DELIVERY (do not build before their gates)

1. **Native-FPS capture toggle, default ON** (30 fps when off). **GATED ON Q7** — this is the removed Native mode rebuilt; old Native shipped ~0.5× slow-motion, never root-caused; locked revisit question in §1/Q7. Also carries a client-facing DECIDE: anomaly timing is frame-denominated, so native capture makes temporal signatures host-load-dependent; and owner's 4–5 fps is RDP-confounded, her real rate unknown. Chat brings a closed DECIDE when design lands.
2. **Per-anomaly probability sliders** (owner defaults: blink 30 / corrupted 30 / missing_texture 25 / LOD 15; camera_clipping outside scheme; missing_object excluded). Touches the DRAW → R-SEED determinism risk; reject-and-re-pick permanently rejected (journal 045 §103). Needs: renormalization rule when one is unticked; missing_object's status decided properly. Design pass required.
3. **Dashboard controls for anomaly parameters** (clip radius, blink/LOD half-periods) via the **typed arg schema / generic ArgControls route** — full brief was written in-thread (search "PREFERRED ROUTE — TYPED ARG SCHEMA"); includes cleaning up camera_clipping's float-vs-`=name` arg overload (m30 misparse still live). Not sent; Monday needs only the ini default.
4. Earlier parked list stands: skybox exclusion (NOW SOLVED via patterns — close it), lod dashboard slider (subsumed by #3), camera-clipping readme line (DONE).

## 6. VIDEOS (assets done or in flight; owner assembles in Shotcut)

- **LOD explainer: DONE** (~1:59 — only 1 s headroom; reclaim from shot 14→8 s or clip B). 10a/b/c "what the tool does" beat added; 10c headline softened to "Every anomaly is checked against what was actually rendered" (overclaim fix — NOT_MEASURED admits by design); 10b foot carries the authoring callback. **`_optional/10d_highest_lod.png` may now be INCLUDED** — the filter shipped enabled and survived owner testing (confirm owner didn't disable it). Owner supplies clips A (two objects, dramatic vs subtle), B (dashboard targeted), C (overlay on subtle capture).
- **Camera-clipping explainer: REBUILD BRIEFED** (v1 rejected — opened on frustum schematic, showed only whole-object vanishing). v2 spec: 6 cards ~55–65 s, player's-view-first, **card 2 shows BOTH looks** (small things vanish; larger things SLICED OPEN — owner confirmed on Concorde: "I see inside the crate, and through it at certain angles" — use his wording if Code's is vaguer), keeps the frame-strip per-frame-labelling card. Report pending.
- Future (discussed, parked): "Reading your captures" video (highest value — prevents silent data misparses: gapped lists, red/amber, session_index sorting) · one-page reference card (may beat a third video) · "start here" only if the README dry-run proves awkward.

## 7. PROTOCOL NOTES FOR THE NEXT CHAT (drift-prevention)

- One live brief per message, labeled "THE ONLY LIVE BRIEF"; addenda folded, never stacked — brief-versioning confusion bit twice this weekend.
- Cold Code sessions: bootstrap header (CLAUDE.md stale-by-design warning, git log over status block, read-don't-rebuild commit list, 10-line state summary FIRST).
- Effort: xhigh design/diagnosis · high mechanical/content. Validation split stands: Code implements + short home checks; owner's 5-min cook loop does the rest; hand owner-faster checks over explicitly.
- Cursor on the office box = transcription clerk ONLY (its FWNetSubsystem.cpp guess cost 2 h); its one good hack (the pin) went through canon re-expression.
- Chat-side errors this weekend, for calibration: asserted mask readback blocking on game thread (wrong); asserted bounds change voided the m30 calibration (wrong — measured unmoved); wrote a pre-registration encoding a false dichotomy (instrument-blind ≠ labels-wrong); called the pin a perf win off a blinded counter. Pattern: mechanism-first claims without measurement. Code caught all four. Keep letting it.
- m31 REMAINS UNTAGGED until post-cook validation passes. No tags cut all weekend. feature/stencil-capture untouched at 76cac74.
- Memory-edit list is FULL (30/30) — consolidation owed (ask owner before deleting; pre-m24 status blocks are the stale candidates). The parked-bug list from Friday lives in memory + this doc.

## 8. FIRST MESSAGE OF THE NEW CHAT SHOULD ASK

1. The §1 verification numbers (four items, incl. the clock-override yes/no).
2. Whether Code's Q4–Q7 addendum report has landed (paste it).
3. Whether the camera_clipping-default + sparse-overlay brief report landed (paste it).
4. Then: rule on the fix, run the remaining §3 checklist, package, ship.
