---
name: cold-start-invisible-anomaly-arc
description: "Entry point — m34/m35/m36/m37 on master; P9 (A) smear CLOSED as temporal accumulation, (B) phase displacement OPEN and localized to the Bates host path; m38 approved-unimplemented; last tag still m30"
metadata: 
  node_type: memory
  type: project
  originSessionId: <uuid>
  modified: 2026-09-02T11:54:56.280Z
---

**START HERE, then trust the repo's CLAUDE.md status block over this file — it is refreshed
every milestone and is canonical.** Repo: `D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector`.

---

🆕 **2026-09-02, SESSION 067 — CURRENT "YOU ARE HERE". Everything below this block is HISTORY.**
Cold start: `docs/sessions/2026-09-02-067-p9-ledger-plan-and-card-fixes.md`.

- **`master` = `be4dd1b`, pushed, == origin.** ⚠ SHA moves with every docs commit — `git rev-parse origin/master` is the authority.
- 🔢 **NUMBERING TRAPS — TWO MINTING INSTRUCTIONS THIS SESSION NAMED A TAKEN NUMBER. ALWAYS SWEEP
  BEFORE MINTING.** `P8` was taken (TAU pose-invariance) ⇒ the blinking phenomenon is **`P9`**.
  `A59` was taken (MCP-bridge provenance) ⇒ the A54-on-constrained-fixtures rule is **`A65`**
  (`A44`–`A64` all in use). Gotchas now reach **`G208`**.
  `master` is the **m34 + m35 + m36 + m37** line. Origin holds exactly master +
  `feature/mask-gpu-reduce` (`7151875`) + `feature/selection-census` (`7f82d52`). **No `wip/*`
  anywhere.** `feature/stencil-capture` (`76cac74`) is **LOCAL-ONLY** — never pushed, never check it
  out (verifier: **clean over 98 files**).
- 🔢 **MILESTONE NUMBERING — RENUMBERED TWICE, BOTH DELIBERATE. `CLAUDE.md` HAS THE LIVE MAP:**
  **`m37` = census selection defaults ✅ SHIPPED** · **`m38` = the run-scoped session log** (plan
  **APPROVED at session close**, journal 067 §16 — **next session's FIRST implementation task**) · **`m39` = honest bbox**. `P-C13` conjunct 2 rides
  honest bbox regardless of number. Journals 065/066 say "m37 = bbox" and journal 067 §13.2 says
  "m38 = bbox" — **all are records, NOT retro-edited.**
- ✅ **`m37` IS IMPLEMENTED AND PUSHED** (`d257f7b`, docs `906d35a`): floor default **6.0 → 0.5**, NEW
  `CensusMaxDrawnCoveragePct` default **25.0**, **INCLUSIVE band**, above-ceiling **EXCLUDED
  categorically**, `census_above_ceiling` counter + `ABOVE-CEILING` token, **ceiling ≤ 0 = DISABLED
  and the StartRun echo says so.** All four gates green. **Census compiled default stays OFF ⇒
  client-inert, measured by `P-C7` re-anchor.** ⛔ **NO TAG** (office batch).
- 📦 **NEW BENCH EXE `6C80E872`** (from `D2BB25A5`), container UNCHANGED — exe-only swap, no cook.
  🔓 **The `D2BB25A5` freeze is RETIRED.** ⛔ `D2BB25A5` is still load-bearing as gate (a)'s A-side.
  ⚠ **`6C80E872` is a NEW `G140`-family boundary for census-ON legs**; census-OFF stays comparable.
- 📌 **Bench render settings, measured not assumed:** `r.AntiAliasingMethod = 4` (**TSR**, LastSetBy
  **Constructor**) · `r.MotionBlurQuality = 4` · `r.ScreenPercentage = 100`.
- ✅ **THE P9 READER IS BUILT AND GATED.** `CaptureBench/tools/p9_hidden_set.py` (CaptureBench
  `4319a9e`). `SEP_RATIO` **5.0**, owner-ruled, **calibrated from two known-answer populations**
  (known-ABSENT tops at 2.54, known-ALIGNED bottoms at 8.96, 5.0 ≈ the geometric middle) and
  **FROZEN — never retune it.** All four controls PASS plus `--d-unit` 3/3.
- 🎯 **THE FIXTURE PROBLEM IS SOLVED, AND THE SOLUTION IS THE MOST REUSABLE THING HERE:
  `CB_GateLevel` CAN BE LETTERBOXED WITH TWO CONSOLE COMMANDS, NO SOURCE CHANGE AND NO COOK —**
  `Set PlayerCameraManager bDefaultConstrainAspectRatio true` +
  `Set PlayerCameraManager DefaultAspectRatio 2.39` in the leg's `ExecCmds` ⇒
  `rect=(0,92)-(1280,628)`. Works because `UpdateViewTarget` applies those defaults
  **view-target-agnostically** (`PlayerCameraManager.cpp:352-355`), a camera-less `CalcCamera` never
  overwrites them (`Actor.cpp:3085`), and `Set` (`Obj.cpp:3937`) is **not shipping-gated** and hits
  **live instances**. ⚠ **BENCH DEVICE ONLY — never in a client payload.** ⛔ The letterbox **lever**
  is untouched. Harness switch `-LetterboxedFixture` declares `B1` NOT
  APPLICABLE and **self-polices** by invalidating any leg whose `readback_layout` origin is zero.
- 🚨 **P9 IS REPRODUCED ON BATES WITH NO FLAGS (owner, 2026-09-02) — census OFF, mask OFF, plain
  blinking + capture. NOT A CENSUS OR MASK PHENOMENON**, and **DETERMINISTIC** (every reviewed event).
  ✅ **(A) BOUNDARY SMEAR IS CLOSED — TEMPORAL ACCUMULATION.** With AA + motion blur off via the
  title's own MENU the partial-opacity frames **vanished**, which is exactly what card `C-1`
  pre-declared. ⚠ Set by menu, so the **effective cvar values were never captured** — `C-3(e)`
  collects them for the record; not a reason to reopen (A).
  🔴 **(B) PHASE DISPLACEMENT IS OPEN and is now the whole question**, a clean binary read:
  **observed `{n,n+1,n+2,n+6}` vs claimed `{n,n+1,n+5,n+6}`** — same structure as the original
  `{42,43,44,48}` vs `{42,43,47,48}`.
  🎯 **TRANSITION ARITHMETIC (sets only, NO mechanism):** four flips; the **OUTER pair matches the
  labels exactly** (first hide n, final show n+7) on BOTH observations; **both INTERIOR flips are +1
  frame late in pixels** (show: labels n+2 / eye n+3 · hide: labels n+5 / eye n+6); **hidden-frame
  count conserved 4v4 ⇒ no dropped frames; outer-pinned ⇒ no whole-event offset** (also why it is not
  P1). ⛔ **NO MECHANISM CLAIMED.**
  🔴 **(B) IS LOCALIZED: every other axis is EXCLUDED** — AA/temporal **in both directions** (bench
  TSR-on reads 16/16 ALIGNED; Bates displaced AA-on *and* AA-off), census, mask, delivery, pacing,
  tick ratio, letterbox. **REMAINING: the Bates host build + content.**
- 📌 **LOCALIZATION (arithmetic only, names no step):** labels are **host-independent** (same
  (n, n+1, n+5, n+6) cadence on both hosts, and it is **OBSERVED not scheduled** — the anomaly
  toggles on TICKS, `frame_indices` samples CAPTURED frames); pixels are **host-dependent** (bench
  16/16 ALIGNED, Bates 100 % displaced). ⇒ the discriminator is on the host path from *visibility set
  on tick t* to *pixels in captured frame f*. **The grab sits at the LAST post-processing pass**
  (`EPostProcessingPass::VisualizeDepthOfField`), so **whatever AA/upscale produced IS the captured
  pixel** — that is the host-dependent row.
- ⚠ **The owner's `r.AntiAliasingMethod 2` test did NOT clear temporal accumulation: method 2 is TAA,
  which retains history** (0 off · 1 FXAA · 2 TAA · 3 MSAA · 4 TSR default). Non-temporal is 0 or 1.
  FSR3 **frame interpolation is a separate switch** from the FSR3 upscaler. ⛔ **FSR3 cvar names
  CANNOT be confirmed from this box** — no FSR3 plugin in the bench engine; never guess them.
- ✅ **OVERLAY SEMANTICS RESOLVED FROM CODE:** `overlay_watcher.py:18-19` — **RED = frame is in
  `annotation.json`; AMBER = frame has a `labels.jsonl` row but is NOT in `annotation.json`.** They
  **cannot disagree by one frame by construction** — both stamped from the same `Snap->SessionIndex`
  one line apart. So the owner's red-set **is** `frame_indices`; the one-frame report means two
  readings of different things. Settle it by printing the array (card `C-3`), never by asking again.
- 🔴 **NEXT SESSION, TASK 1 — A SOURCE READ THAT WAS NEVER EXECUTED** (its brief was never delivered):
  **which code path drives each of the four flips, and what the toggle log line is anchored to** —
  tick, frame, or the sampled bit. Journal 067 §15.1's enumeration is the map; this is the layer
  below it, and it is what turns `C-3` into an attribution.
  🔵 **TASK 2 — implement `m38`** (run-scoped session log), plan approved at journal 067 §16.
- 🔴 **NEXT EVIDENCE FROM THE OWNER IS CARD `C-3`, now THE decisive item.** Prerequisites: a **FRESH**
  run (every banked run lacks the toggle lines — Verbose-only), `Log LogAnomaly Verbose` typed
  **before** the run (absent lines then are a **READ**, not a failure), AA + motion blur off.
  Pre-declared three-way, no branch preferred: log+labels agree ⇒ pixels are the outlier;
  log+eye agree ⇒ the sampling/labelling side is; mixed ⇒ report raw. **Names WHERE, never why.**
  ⛔ **Do not run bench legs for P9 unprompted.** `C-1` is DONE; `C-2` remains available.
- 🔴 **P9 RESULT SO FAR — NEITHER REPRODUCED NOR REFUTED, TWICE.** v1 on `MainWorld`: `UNDECIDABLE`
  on all five legs, a **fixture** failure (`G206`), not a P9 result. **v2 on the letterboxed
  `CB_GateLevel`: 16 counted events across four legs, ALL ALIGNED, `k=0`, both differences empty,
  ZERO `P9`-SHAPE**; 8 further events UNDECIDABLE on separation. ⛔ **That is still NOT the
  pre-declared "NOT REPRODUCED"** — it required *every* event graded. **P9 stays OPEN.**
  ⚠ `A54` **cannot answer the P1 question on a letterboxed fixture** (its `B1` conjunct fails since
  `CALIB_BBOX` is frozen unconstrained, and its printed message misattributes that to pose); the P1
  exclusion rests on the reader's own best-`k` search. ⛔ `a54_oracle.py` NOT edited — any edit
  re-triggers `A53`.
- ✅ **CLOSED AS NON-FINDINGS:** `ticks_per_captured_frame` — Bates' **1.3556 reproduces exactly**
  on three paced legs here (122/90; the counter jitters 122↔124), so **NOT a discriminator**; and
  **delivery mode is excluded as a P9 factor** (leg C ran delivery ON and matched leg A).
- ✅ **The two dirty local-only branches were DELETED** (owner-ruled). `"no reachable ref, origin OR
  local"` is **RESTORED**. Surviving local refs: `master`, `feature/mask-gpu-reduce`,
  `feature/selection-census`, `feature/stencil-capture` (local-only, verifier-clean, never push).
- ⛔ **STILL NO TAG.** Highest is `m30`; order at the physical office visit is
  `m31 → m33 → m34 → m35 → m36 → m37`, after `G-R7(ii)` on master's own cook.
- 🔴 **`P9` IS THE OPEN PHENOMENON — and the brief for it said "P8", which is WRONG. `P8` has been
  taken since 2026-08-18 (TAU is not pose-invariant); numbers are never reused.** Both chat handoffs
  call it P8. The ledger entry is `docs/invisible-anomaly-mechanisms.md` **§8**. Owner-observed ×3 on
  Bates, `blinking` only: claimed hidden `{42,43,47,48}` vs observed `{42,43,44,48}` — wrong in one
  position **in each direction**, so it is **NOT** `P1`'s constant shift. **NO MECHANISM CLAIMED.**
- 🚨 **THE FINDING WORTH CARRYING: the A54 oracle CANNOT SEE `P9`, and it fails toward "clean".**
  `a54_oracle.py:135` — `SHIFTS = (-1,0,1)`, so its whole hypothesis space is *uniform displacement*.
  `P9` is a two-directional set difference, which no shift expresses ⇒ **A54 returns `ALIGNED` on a
  `P9` event.** ⛔ **Never report "A54 says ALIGNED" as "P9 not reproduced."** The plan specifies a
  new instrument, `CaptureBench/tools/p9_hidden_set.py` (**SPECIFIED, NOT WRITTEN**), which reports
  the observed hidden set itself and must pass three controls including a **synthetic P9 fixture
  built from banked data** before it grades anything.
- ⏸ **AWAITING CHAT'S OK, ALL UNCOMMITTED:** the `P9` ledger entry, the predictions file
  `docs/predictions/2026-09-02-p9-blinking-boundary-repro.md`, the RDP card fixes, and the 067
  journal. **No leg has run and none may run before approval.**
- 🔴 **FLOOR/CEILING DECIDE still sits with the OWNER** (chat rec 0.5 + a ceiling of 25; **the
  ceiling is UNBUILT**). ⛔ Do not build it, do not change the 6.0 default.
- 📌 Small facts worth not re-deriving: `ticks_per_captured_frame` **is just**
  `capture_game_ticks / total_frames` (`AnomalyLabelWriter.cpp:546-548`) — 122/90 = 1.3556, so the
  open question is the 32 surplus ticks, not the ratio · delivery mode does **NOT** suppress
  `labels.jsonl` (`bWriteLabelsInDeliveryDefault` compiles ON); it suppresses `run.json` ·
  `_TRASH_pending_owner_delete` **no longer exists** (owner deleted it) · the scrub verifier's
  self-test **prints the decoded terms to stdout**, so never paste its raw output anywhere.

---

🚧 **2026-08-26 — SESSION 061. `m35` IS THE ACTIVE WORK. GO STRAIGHT TO
`docs/sessions/2026-08-26-061-m35-readback-sub-rect-copy-handoff.md` — it is SELF-CONTAINED and
supersedes everything below for m35 purposes** (pushed at `5f9cbbc` on `feature/mask-gpu-reduce`,
along with a refreshed CLAUDE.md "you are here" and the full results ledger in
`docs/predictions/2026-08-26-m35-build-b-gates.md` §12).
🚨 **THE FIX IS BUILT, STAGED, GREEN ON EVERY GATE THAT RAN, AND IS NOW A LOCAL `WIP` COMMIT THAT
MUST NOT BE PUSHED — `41d2708`.** Branch is **AHEAD 2**; `origin/feature/mask-gpu-reduce` is at
`5f9cbbc` (docs only). When `G-M7`/`G-M8`/`G-M9` pass, **AMEND the WIP into the real fix commit and
push once** — no force-push needed. ⛔ **Do not stash/clean/checkout without reading it first.**
No leg failed. Insurance diff kept at
`_binary_baselines\m35-buildb-uncommitted-2026-08-26.diff` (18,756 B, `sha256 7A0CC269…`).
⛔ **Do not delete `_binary_baselines\StackOBot.exe.m34-fix-candidate-7F37A4AC` — it is `G-M6`'s
A-side** and cannot be rebuilt once the WIP is amended.
**Master untouched and STILL CARRIES THE CRASH.** No cook, no tag; staged exe `733FE83C` on the
unchanged m34 container.
⛔ **DEAD ENDS — do not re-propose either:** the unconditional `Rect.Min.Y` offset removal (its stop
condition FIRED — stock 5.1 allocates FULL-SOURCE-SIZE staging; the layout changed at 5.2), and the
`bufferHeight`/pitch sniff (**11 px of row padding** makes the two layouts numerically
indistinguishable at a narrow pillarbox — a measured blind spot). The shipped design copies the
sub-rect into a **plugin-owned W×H texture at (0,0)** and reads back the **whole texture, no rect**.
📌 Owner rulings that bind: **do NOT checkout master** (the lift was withdrawn) · **ONE ROUTE ONLY —
m35 reaches master by the branch MERGE, never also by cherry-pick** · **any leg fails ⇒ report and
stop, do not fix in the same turn** · **every checker is proven against a known answer before its
verdict is read**.
🔧 Traps that cost time here: `git` via the **Bash tool hung** (use PowerShell `git -C <plugin>
--no-pager`) · never round-trip a tracked doc through PowerShell redirection (G115/G141) · the
`IAI.Bench.Letterbox` lever **cannot bite packaged** (the `-unattended` pawn is a SpectatorPawn with
no camera component), so `G-M7`/`G-M8` need PIE on MainWorld · **the m11 pacer masks hook cost**, so
`G-M6`'s prior must be taken with pacing OFF via the archived-exe swap, never a rebuild.

🚨 **2026-08-24 — SESSION 060, TWO FIELD BUGS, MASTER HAS MOVED PAST THE CLAUDE.md STATUS BLOCK
(that block is milestone-scoped and 060 closed no milestone — read journal
`docs/sessions/2026-08-24-060-field-bugs-overlay-labels-and-depth-blind-picker.md`).**
Plugin master `e9bf96d`; AnomDash master `be5b151` (`68bf3c0` dropdown names · `c3ce8ce`
Active/now-firing names · `5d35cbe` picker **→ reverted at `be5b151`**).
**No engine code, no cook, no tag.** ⚠ **`feature/mask-gpu-reduce` DID move at owner request** —
tip `3363d5f`; the three docs/tool commits were CHERRY-PICKED onto it (`f5e3f0f`, `3be67fc`,
`3363d5f`), so they exist on both lines under different hashes and will show twice in the eventual
merge log (identical bytes, no conflict). **No m34 code touched; G-R7 merge gate unchanged.**
`2b6c93f` (m33 docs) is on master and deliberately NOT on the branch.
🎯 **BUG 1 CHAIN, ESTABLISHED: depth-blind pick → decal target → opaque swap renders visibly →
veto correctly zeroes → empty annotation.** The dashboard's `pickActorAt` kept the SMALLEST
projected rect and never read `dist`, so a click aimed at a wall took the decal lying on its face
(`Wall_Grime_F_CR_Decal_INST776`, TRANSLUCENT) — the swap was visible ON THE WALL while the mask
measured the decal at zero and all 10 events were vetoed. **The veto is EXONERATED and stays
ZERO-ONLY.**
🛑 **THE SELECTION-SIDE FIX WAS SHIPPED AND THEN REVERTED THE SAME DAY (AnomDash `5d35cbe` →
`be5b151`, owner report): nearest-first ranking made objects BEHIND a large bounding box
UNREACHABLE.** `pickActorAt` is back to smallest-area-wins, byte-identical to pre-session.
⇒ **BUG 1's SELECTION HALF IS OPEN; the ini pattern exclusion (`Decal`, `_CR_`) is the ONLY defence
and a decal matching neither pattern reproduces it exactly.** ⚖ **DO NOT RE-PROPOSE EITHER RANKING
— area reaches occluded objects but mis-picks overlays; distance picks right but cannot reach
behind anything; both are guesses from projected bounds, which IS the defect.** The
**engine-side line trace from the clicked pixel** is the only trade-off-free formulation and is now
the recommended next move (needs a WS command + a cook; **not designed, needs approval**).
📌 **FIRST FIELD RECEIPT OF ROUTE (e)** — the checklist's "translucent population is UNMEASURED on
any delivered title" is no longer true: 10/10, `translucent_vetoes=10`. m27's hedge (the counter is
a PROPERTY OF THE MEASURED TARGET) is what let it identify WHICH object was measured.
⛔ **PARKED, do not start unprompted:** route (e) blast-radius read (post-delivery) · engine-side
line-trace picker · a general translucent-target exclusion (**owner sign-off required**) · PMCore
rewindable-tick ensure (office pass) · **`asset_name` in `labels.jsonl`** — vetoed/unmatched overlay
boxes have no asset name to show because `labels.jsonl` carries only `target_name`; that is a `P6`
contract change and **needs a ruling**.
📌 **AT THE NEXT COOK (shipped bundle NOT re-cooked):** `+ExcludedTargetNamePatterns=Decal` and
`=_CR_`. Both are boxes in `PRE-DELIVERY-CHECKLIST.md` §1; the canonical pre-cook ini block lives in
`docs/CHAT-HANDOFF-crisis-weekend-delivery.md` §3, **which is UNTRACKED in git**.

**As of 2026-08-21 (later): 🎯 the m31 FIX IS SHIPPED AND PUSHED** (`b071469` fix + `ebaa078` docs;
supersedes the S1-only state). The defect: the SVE path (shipping default since m25) wrote ZERO
frames on Concorde — a fixed-sim/variable-render Concorde fork on which two independent
reads of `GFrameCounter` (arm vs publish) disagree; first defect ever found by a second host.
**The fix (Option B, mechanism-independent by design):** plugin-owned monotonic serial minted once
at the arm site → keys `PendingSnapshots` + both capturers; SVE pairing = pending-wanted FIFO
consumed by the next ELIGIBLE view family (scene/reflection captures guarded out and counted);
ring key stays `FSceneViewFamily::FrameNumber`. Permanent gate: unconditional run-end handshake
summary + bounded trace + pair-drop promoted to Warning (tokens named in journal 050 §6 only).
`run_summary.wanted_published` → `wanted_matches`. G153 closed (outDir dequote + loud StartRun
refusal). **V-2 passed on StackOBot, both paths, true A/B (pre-fix m30 exe `99AE7526` vs post-fix
`DC55CB9B`): events identical, annotation keyset identical, gate line 90=90=90=90.**
⛔ **Awaiting Concorde V-3 (in-editor) and V-4 (packaged) office-side — m31 does NOT tag until V-4
passes.** Pre-registration: `docs/predictions/2026-08-21-m31-fix-validation.md` (restate verbatim
before reading any leg; V-3's failure branch must localise arm/publish/eligibility/pairing).
Owner procedure: journal 050 §10.

**As of 2026-08-21 (latest, session 051): SIX COMMITS + ONE COOK on top, all pushed, still NO TAG**
— `ea99d6b` tools move · `d3f3152` material usage flags · `98d04d4` measurement-ceiling banner ·
`f999d7a` tick-mode pin · `b4e07e0` pin console override · `a3aa1e6` auto-pool anomaly defaults.
AnomDash at `7922457`. **Docs ARE current: journal 051, CLAUDE.md status block, architecture.md
control surface, gotchas G155–G160.** 📦 New quartet: exe `DD76385F` · utoc `E4FE9B35` · ucas
`D9929F6F` · pak `BFB95333`; map gate exit 0. Prior quartet preserved at
`_binary_baselines\m31-v2-container\`.
- ⏱ **Tick pin** `IAI.Capture.TickPin <0|1>` = NAMED BISECT; build-time probe
  `ANOMINJECT_FW_TICKPIN` (marker `FWNetSubsystem.cpp`), compiles out on stock. **Unpinned home
  baseline `ticks_per_captured_frame` = 1.3556.** run_summary +6 fields; P6 unmoved.
- 🔩 **Auto-pool anomaly defaults**: `[AnomalyInjector]` ini keys + `IAI.Anomaly.BlinkHalfPeriod` /
  `IAI.Anomaly.LodHalfPeriod`. Precedence targeted-arg > console > ini > compiled. **NO VALUE
  CHANGED** — lever only.
- ⚠ **G88 GENERALISES: any ini-only lever is a no-op beside a package.** That is why BOTH new levers
  got console overrides. Remember this before adding another ini key.
- 🔧 **`tools/measure_label_offset.py`** measures pixel-manifestation vs annotation per event.
  ⚠ **STANDING RULE: internal diagnostics ride the PLUGIN repo's `tools/`; client-shipped host
  tooling stays in AnomDash `host-tools/`** (office pulls the plugin repo only).
  ⚠ **Its measurable range is `min_clean_gap // 2` — about ±2 on the standard `Config 2 4 8 4 0`.**
  Beyond that it warns BASELINE CONTAMINATED / UNMEASURABLE rather than under-reading silently.
  Blink half-period is settable ONLY as a targeted-fire arg (no cvar, no ini); default 3.
- 🎨 **Material usage flags: both shipped materials were missing `bUsedWithStaticLighting`** (and
  clothing); now 7 flags each, set by headless `tools/set_material_usage_flags.py` (idempotent).
  🚨 **StackOBot CANNOT reproduce or verify this — `r.AllowStaticLighting=False` in its
  DefaultEngine.ini means `MATUSAGE_StaticLighting` is never queried here.** Concorde's re-cook is
  the ONLY verification. **Prior Concorde-captured deliveries contain wrong-APPEARANCE samples**
  (statically-lit static meshes drew the engine default instead of our material; skeletal was fine).
- ⏱ **Tick-mode pin** behind build-time probe `ANOMINJECT_FW_TICKPIN` (marker `FWNetSubsystem.cpp`);
  compiles out on stock, guard proven by forcing the define → build fails naming
  `FApp::sUseFixedGameTickWithVariableRenderTick_Net`. run_summary +6 fields.
  **Unpinned home baseline: `ticks_per_captured_frame` = 1.3556.**
  ⚠ **A47 measured again: 90/90 frames differ between two runs of the SAME binary** — never expect
  frame byte-identity; use the control-pair + invariant-core comparison (`subset_gate.py`), and note
  it EXITS NONZERO by construction when a change adds artifact fields (G158).
- 🚨 **G160: `preFrames` is a ONE-TIME lead-in; `postFrames` governs the clean gap.** Diagnostic
  capture = `IAI.Capture.Delivery 0` + `IAI.Capture.Config 2 4 8 14 0` (gap 14) + `--require-gap 12`.
  `2 14 8 4 0` leaves the gap at 4. Config governs spacing on the AUTO-POOL path too.
- 🚨 **G156: DELIVERY MODE CANNOT SELF-VERIFY** — no labels.jsonl ⇒ no bbox ⇒ fullframe, no ambient
  ring. A known-+0 missing_texture session reads median startΔ **+6 (wrong)**; a known-+1 one goes
  entirely unmeasurable. **Verification is ALWAYS a delivery-OFF exercise.**

**As of 2026-08-22 (session 053, the day before the client build): FIVE plugin commits + one journal
+ TWO AnomDash commits, all pushed, STILL NO TAG.** Plugin `c15e961..881497c`, AnomDash
`21d9fae..2b4264c`. Staged exe `F9E0941C`; **code-only hot-swap, container unchanged**. Journal
`docs/sessions/2026-08-22-053-...md` is self-contained.
- 🎯 **TOGGLING ANOMALIES now label the ACTIVE SUBSET.** `IsHideTypeAnomaly` is WIDENED into ONE
  table `ResolveAnomalyActiveSource` with three sources: `actor-hidden` (blinking, missing_object —
  behaviour byte-identical), `anomaly-state` (lod_popping, camera_clipping, via a new
  `IAnomaly::IsCurrentlyAnomalous()` defaulting to `IsActive()`), `fire-window` (the rest). Sampled
  in m20's DEFERRED slot. A global fire (no target actor) records active=1 by construction.
  **Measured: lod_popping went 267-274 (8) → {272,273,274}; 5 of 8 old positives carried NO pop.**
- 🚨 **`lod_popping` HAS NO AUTOMATED OFFSET ORACLE.** `measure_label_offset.py` reads it
  UNMEASURABLE before AND after — its detectors want whole-region magenta/checker swaps and a pop
  changes only ~2.4-3.5 % of the bbox. **Verify it with a direct in-bbox strong-px series
  (method in journal 053 §1); do NOT read its silence as a defect.**
- 🎯 **PAST-THE-WINDOW question ANSWERED: (b) render-side ramp, NOT a late revert.** A late revert
  would STEP; measured 272→274 ramps UP (2024/2449/2942) and 275→277 DECAYS (2041/1345/1313). TSR
  temporal history is the NAMED CANDIDATE (read from the process log), not an established mechanism.
  ⚠ **SOURCE-READ, UNMEASURED: `USkinnedMeshComponent::SetForcedLOD` lands ONE TICK LATE** (writes
  `ForcedLodModel` only; `UpdateLODStatus()` runs in a tick group BEFORE tickable subsystems) —
  static meshes revert same-frame via `MarkRenderStateDirty`. Owner capture settles it.
- ⚖ **Targeted fire BYPASSES the auto-pool proximity gates** (lod_popping distance + m30 coverage);
  the single-LOD guard applies to both. Proven both ways on one binary.
- 📐 **`node.bounds` is now the SM/SK union** (`AnomalyViewport::GetActorRenderableBounds`).
  🚨 **bbox_px / bbox_norm / coverage did NOT move and the m30 7.0 % CALIBRATION IS NOT VOIDED** —
  the projector and the gate always used mesh-only unions; only `node.bounds` was wrong. **No
  re-calibration was performed and none should be invented.** `BP_Bot` unchanged in a packaged build
  CONFIRMS journal 035: the 1010 cube is editor-only.
- 🖥 **Snapshot `visible[]` gains `asset`/`compClass`; catalog gains `targetable`.** Dashboard shows
  the asset name primary, actor name dimmed after it. **Not visually verified — needs live PIE.**
- 🎥 **Targeted `camera_clipping`**: `Args[0]` starting `=` selects it; radius
  `[AnomalyInjector] CameraClippingTriggerRadiusCm` (200, deliberately the SAME number as
  lod_popping's) + `IAI.Anomaly.CameraClipTriggerRadius`. Whole-frame extent is a property of the
  ANOMALY (`FAutoLiveFire::bWholeFrameExtent`), not of how it fired. F-LABEL guard confirmed to
  cover it. ⚠ **Global determinism pair is VACUOUS on the positive branch (0 positives both sides).**

**As of 2026-08-23 (session 054): TWO plugin commits + journal, pushed, STILL NO TAG.** Plugin
`881497c..aeb1930`; staged exe `060F7B07`, code-only hot-swap. **All six of session 053's owner tests
PASSED on the client host.** Journal `docs/sessions/2026-08-23-054-...md` is self-contained.
- 🎯 **ROOT CAUSE of "visible on some objects, not on others": `lod_popping`'s magnitude is the
  CONTRAST between the LOD an object is CURRENTLY at and the one forced on it.** Auto-pool now
  RESTRICTS to candidates at LOD 0 (`LodPoppingRequireHighestLod`, default ON,
  `IAI.Anomaly.LodRequireHighestLod`). RESTRICT not PREFER because a re-pick would break the seeded
  draw protocol (R-SEED / journal 045 §103). It is the GRADED form of the single-LOD guard.
  **AUTO-POOL ONLY — targeted fire WARNS naming the level and fires anyway.**
- 🔬 **THE LOD READ, and it is NOT a "was it rendered" read:** skinned =
  `USkinnedMeshComponent::GetPredictedLODLevel()` (real game-thread state); static = PREDICTED with
  the engine's own `ComputeBoundsScreenSize` vs `RenderData->ScreenSize[]` (deterministic screen-size
  computation, game thread). ⚠ Does NOT model `LODDistanceFactor`, `r.StaticMeshLODDistanceScale`
  or per-platform `MinLOD` — every candidate logs `CURRENT-LOD` so a disagreement is visible.
  **No new module dep — SceneManagement.h / StaticMeshResources.h are Engine public headers.**
- ⚠ **FIRE RATE ON THIS BENCH IS UNCHANGED (10 events / 1 lod_popping, filter off AND on,
  byte-identical).** The only multi-LOD candidate an unattended run ever draws is already at LOD 0.
  **The filter's real cost can only be measured where a player moves.**
- 📏 **`LodPoppingMinCoveragePct` + `IAI.Anomaly.LodMinCoverage` — tunable. COMPILED DEFAULT STAYS
  7.0 (m30-MEASURED: visible 9.3453 / invisible 3.9045). Tuning is NOT a re-calibration.**
- 🚨 **THE FULL GATE STACK, and the trap: the GENERAL selection coverage gate (6.0 %,
  `IAI.SetMinScreenCoverage`) ALSO applies on top of the lod-specific 7.0 %.** Below 6 % the general
  gate silently becomes binding and the lod lever stops having any effect. Reported; a lod-specific
  override was NOT built.
- 🩹 **HITCHING = THE m26/m27 MASK PASS. Owner bisect settled it; OWNER DECISION: THE MASK STAYS ON**
  (Mask 0 admitted many invisible objects as labels = the client's complaint #1 returning). Ships as
  a documented limitation. **The GPU readback is ALREADY NON-BLOCKING and not on the game thread**;
  the cost is a full-screen pass + a full view-rect GPU→CPU copy (×6.94 at 3200×2000) + a **W×H CPU
  reduction ON THE RENDER THREAD**, once per armed FRAME, on ~13 % of frames that fall inside anomaly
  windows by construction. Post-delivery candidate = **GPU-side reduction** (the veto only needs its
  answer at FinishRun, so latency is free). NOT designed.
- 📊 **TICK PIN IS ALSO A BIG PERF WIN on the decoupled host: speed_ratio 1.0006/1.0012 pinned vs
  1.3627/1.6568 unpinned.** P1 adjacency (her defect reported at ~1.2) is a CANDIDATE, not a claim,
  and is NOT to be investigated.

**As of 2026-08-23 (session 055, the FINAL pre-delivery change set): THREE plugin commits + ONE
AnomDash commit, pushed, STILL NO TAG.** Plugin `aeb1930..4bc9739` (`a75d601` catalog, `6e143f2`
overlay, `4bc9739` client docs); AnomDash `2b4264c..dcf2004`. Staged exe `DCF9C192`; predecessor
`060F7B07` archived at `_binary_baselines\StackOBot.exe.session054-060F7B07`.
- **`camera_clipping` is now UNTICKED by default in the auto pool.** It is a whole-session global,
  so it lays one continuous condition under every other anomaly. It STAYS in `GAutoPool` and is
  still selectable. Read back from the running build BOTH ways (G96/G119): default echo reads
  `Default pool: blinking, missing_texture, corrupted_texture, lod_popping`, and
  `IAI.Auto.Pool camera_clipping 1` still takes it to `enabled pool (5)`.
- **WHERE THAT DEFAULT LIVES: `GAutoPoolDefaultEnabled[]` in `AnomalyAutoInjectorSubsystem.cpp`,
  consumed ONCE in `Initialize()`. ENGINE-SIDE ONLY.** The dashboard holds no default of its own
  (`AutoPanel` renders `Object.keys(auto.pool)`, ticked state comes from `ControlSnapshot`,
  `client.autoConfig` is sent only on a user click). **Compiled state, NO ini route => it needs a
  rebuild AND a cook to reach a client build.**
- **THE CAPTURE-POOL PANEL SHOWS SIX BOXES, NOT FIVE.** The snapshot emits every Object- or
  Global-scoped catalog entry; AnomDash hides `{lod_corruption, time_dilation, lighting_mismatch}`;
  `lighting_mismatch` is Component-scoped anyway. What remains is blinking, missing_texture,
  corrupted_texture, lod_popping, **missing_object** and camera_clipping. `missing_object` has been
  available-but-unticked since m19 and was never written down. **"The delivered pool is FIVE" in the
  m30 status block means the five DEFAULT-ON at m30 -- it is not the number of boxes on screen.**
  client-readme.md now documents six.
- **The overlay tool writes ONLY frames carrying boxes** (`tools/verify_capture.py`; the watcher in
  AnomDash relays its new summary). Output is SPARSE and NON-CONTIGUOUS; **filenames keep the
  original 0-based frame index and are NEVER renumbered**; AMBER-only frames are still written;
  `--red-only` (default OFF) narrows to shipped labels. Measured on a real 300-frame session:
  300 files/315.0 MB -> 96 files/100.4 MB (**68.0 % saved**), identical box and category counts,
  all 96 retained images byte-identical. Nothing enforces 1:1 with `Actual_Frames`, and
  `encode_watcher.py` builds video from `Actual_Frames` ONLY (`FRAMES_SUBDIR`), so a sparse
  `annotated/` cannot break any video.
- **NEW TRAP, cost me a build: a KILLED build task left a TRUNCATED `StackOBot.exe` (2 MiB against
  the real 240 MB), and the next `Build.bat` reported "up to date" at EXIT 0 in 2.8 s.** UBT trusted
  the output timestamp. **Always check the exe SIZE, not just the exit code** -- G119's shape
  applied to the build system itself. Fix: delete exe+pdb and rebuild (objs survive; ~90 s relink).
- **OWED, NOT DONE (not in the brief's commit list): the session-055 journal and the CLAUDE.md
  status-block refresh.** Standing doc discipline wants both as a standalone `docs:` commit.

**As of 2026-08-23 (owner report from Concorde, ANALYSIS ONLY — no code, no commit): the SPED-UP
VIDEO defect is DIAGNOSED.** Owner leg: 120 frames, annotation fps=target=30 (video 4 s), capture
took ~23 s wall, tickpin 1, content clock wall, speed_ratio 1.000xxx on EVERY leg. Arithmetic:
ratio≈1 with wall≈23 s ⇒ GameSpan≈WallSpan ⇒ `World->GetTimeSeconds()` tracked WALL across the
armed span ⇒ **the fork does not honor `FApp::SetUseFixedTimeStep` when TICKPIN pins its mode off
⇒ the m11 honest-fps stamp (`speed_ratio = WallSpan/GameSpan`, cpp ~:2080-2157) is STRUCTURALLY
BLIND on the pinned decoupled host** — it reads ~1.000 whatever the health, so a mask-hitch-starved
run (true sustained ≈5.2 fps) shipped stamped 30 ⇒ 5.75× sped-up video. The stalls themselves = the
m26/m27 mask pass (054 §7, owner-bisected; MASK STAYS ON by ruling — do not relitigate).
🚨 **Consequence: 054 §8's pinned ratios (1.0006/1.0012) are CLOCK-AGREEMENT readings, not health
— on Concorde measure capture health by wall math (frames/VideoFps vs labels.jsonl `t_wall` span),
never by speed_ratio.** Discriminator pre-registered: labels.jsonl per-row `t` deltas ≈ `t_wall`
deltas (not 1/30) + `ticks_per_captured_frame` ≈1.2 CONFIRM; `t` deltas exactly 1/30 REFUTE (then
re-read run_summary). No-code mitigations named to owner: capture at the certified 1280×720 view
(mask cost ÷6.94; m25's only alignment-certified res) · `Mask 0` is the bisect ONLY (ruled
not-ship) · existing sessions rescued host-side by remuxing at (N−1)/(t_wall span) ≈5.2 fps. Code
candidates (chat-side, NOT started): GameSpan re-keyed to plugin-owned armed-frame count ×
1/VideoFps (the widened m31 invariant applied to the ratio instrument — a host redefined an engine
global's semantics again) · the already-filed GPU-side mask reduction (054 §7.2).
**CONFIRMED FROM SOURCE (chat live-brief, same day):** ratio = WallSpan/GameSpan
(AnomalyCaptureSubsystem.cpp:2114-2123), game side `World->GetTimeSeconds()` at arm (:1680),
wall side `FPlatformTime::Seconds()` (:1688, true QPC); fixed step set UNCONDITIONALLY at
BeginActualRun :1157-1161, restored :2533-2538; pacer overrun branch :2059-2062 resets silently
— NOTHING records a blown budget; stamp→annotation at :2692. Encoder is flat CFR: encode_watcher.py
:92-96 + :115-116 (`-framerate` from video.fps, image2 glob, timestamps ignored) ⇒ any speed fix is
HOST-SIDE, NO COOK. 🚨 **The m21 ship rule (`speed_ratio ≤ ~1.05` ⇒ deliver, capture-fps.md:170-179)
is VACUOUS on the pinned fork — always passes.** Owner also saw ~2x fast on a SINGLE targeted
lod_popping run (7-9 s wall for 4 s video), so it is not only the full pool. **Native-mode history
(Q7): pre-`c5d58b0` capture armed one frame per ENGINE TICK, free-run, stamped flat 30 ⇒ 3x fast on
10 fps PIE / 0.5x slow on a 60 fps box — pure stamped/actual arithmetic, NO pacer existed (m11
later), NO forced game clock. `c5d58b0` stamped the MEASURED rate (correct); `500eac7` (18 min
later) replaced it with fixed timestep — which is the assumption the fork now breaks. Any rebuilt
Native must stamp/space from measured WALL rate.**

**CHAT VERDICTS LANDED (2026-08-23, late):** diagnosis ACCEPTED (Reading 1 fired: t span 34.219865
vs t_wall 34.220319 vs fixed-step prediction 3.967 s — game clock ran on wall; ratio blind
CONFIRMED at observation level). Session 055 ACCEPTED; **the authorized docs commit is DONE and
PUSHED: `1f98523`** (journal 055 + CLAUDE.md 053-055 you-are-here + G164). **NUMBERING RULED:
m32 BURNED (bench legs) · m33 = GameSpan re-key + encode_watcher cross-check (THE ONLY LIVE BRIEF,
rides Monday's delivery cook, master, plan-before-code, gates G-A/G-B home + G-C Concorde) ·
m34 = GPU mask reduction (plan ACCEPTED with A1 FinishRun-loss-is-loud, A2 gates→commit→push,
A3 m33-first; branch `feature/mask-gpu-reduce` cut from master HEAD, commits/pushes to branch ONLY,
own POST-delivery cook, merge gated on G-R7 Concorde leg). VFR REJECTED — no-VFR doctrine stands.**
🚨 **m33 PLAN FLAG (mine, delivered to chat, awaiting ruling): chat's literal formula
GameSpan=(ArmedFrames−1)/VideoFps is WRONG — settle/gap ticks between bursts sit INSIDE the
first→last-arm span (owner's own 162 ticks/120 frames), so it would misread a HEALTHY run at
ratio ≈1.2-1.3 and falsely fire the honest stamp (slow videos — the inverse defect). Correct
nominal span = (CaptureGameTicks at last arm − at first arm) × 1/VideoFps, sampled inside
StampArmWallClock so the operands stay paired; on stock fixed-step hosts this is IDENTICAL to the
old world-clock span by construction. CaptureGameTicks increments unconditionally at cpp:450
(bRunning+bRunBegun only, NOT tickpin-gated — verified).**
✅ **m33 IS BUILT, HOME-GATED, PUSHED (2026-08-23, session 056): plugin `0298143`(predictions) +
`491eca5`(AMENDMENT 1) + `03b0b7a`(feat) + `4046aff`(docs); AnomDash `132d27d`. Chat WITHDREW its
literal formula and ratified mine. All home gates green: G-A identity 1e-7, armTicks 119 (withdrawn
form would read 1.337 healthy), G-B stall-40ms ratio 1.2374 (model 1.239) honest stamp fired
end-to-end (24.244), subset gate +1 field exactly, G-W 4/4 fixtures. ⛔ m33 closes only after G-C
(owner starved Concorde capture post-cook); NO TAG.** 🔴 **PENDING CHAT RATIFY-OR-REVERT: the
watcher estimator — ruled (N−1)/span under-reads healthy gapped sessions 23% (measured 22.99 vs 30);
shipped 1/median(consecutive t_wall delta), pre-registered AMENDMENT 1.** Staged bench exe
`757A5DD4`; predecessor archived `_binary_baselines\StackOBot.exe.session055-DCF9C192`. Client-
readme gained the owner's capture-resolution paragraph (1080p/720p rec + 4-second stopwatch check).
Runbook §8.2's A44 example control `IsHideTypeAnomaly` is stale (renamed 053) — flagged unfixed.
🎬 **m34 IS CLEARED TO START and its COLD-START CONTRACT IS IN THE REPO:
`docs/sessions/2026-08-23-057-m34-gpu-mask-reduce-approved-plan-handoff.md` (pushed, `1a3b1eb`) —
approved design, file list, G-R1..G-R7 with the known-answer set, amendments A1–A3, branch rule
(ALL work on `feature/mask-gpu-reduce`, cut from master HEAD at cut time, branch not yet cut;
merge gated on G-R7's Concorde leg), first-actions checklist. An m34 session reads THAT file, not
this memory. m34 needs a FULL COOK (G129) and stays OUT of the client delivery build.**
✅ **m34 IS BUILT AND HOME-GATED (2026-08-23, session 058) on `feature/mask-gpu-reduce`:
predictions `3e20032` → feat `0fc00ef` → journal `73ebffc`, ALL BRANCH-ONLY, pushed; master
untouched at `1a3b1eb`+.** G-R1..G-R6 + G-R7's StackOBot half GREEN: 145/145 per-frame
MASK-REDUCE COMPARE IDENTICAL across five equivalence legs (`_49` band-exact 66,843–66,878 px;
spline MEASURED_ZERO ×8; SM_Ramp2 NOT_MEASURED ×8; probe fired on the new path); guard proven to
FAIL (perturbed local build → FIRST-DIFF tag=200 field=count, reverted, staged exe hash-restored);
same-seed cpu/gpu pairs identical incl. the veto pair and delivery-ON; annotation 48/48,
run_summary +0 vs the m33 binary (subset-gate's one extra vs the PRE-m33 pair = m33's own
game_clock_speed_ratio, reported not relabelled). **G-R7(i) fired its pre-declared honest-limit
branch: at 1920×1080 (2.1 MP) the cpu scan doesn't rise above frame-time noise ⇒ NO-REGRESSION
reading (gpu ≤ cpu); the COLLAPSE demonstration is the Concorde half, which gates the MERGE.**
📦 New quartet: exe `17DEAA74` · utoc `2A66CA57` · ucas `A7EF9B12` · pak `D8009AD7`; predecessor
(m33 exe + s051 container) preserved 6/6 at `_binary_baselines\m34-precook-m33exe-s051container\`.
⚠ The BENCH COOKED INI has `bMaskMeasureDefault=True` — a leg with no mask command runs mask ON;
true inert needs explicit `IAI.Capture.Mask 0`. ⚠ Pre-cook disk was NO-GO (6.12 GB): 163 verified-
banked session payloads freed (16.82 GB, prune_verify method), 16 unbanked dirs + 3 same-day
Saved sessions RESCUED into the bank (`RESCUE_M34PRECOOK_*`). Lever: `IAI.Capture.MaskReduce
<gpu|cpu|both>`, default gpu, ini `MaskReduceDefault`, console beats ini. ⛔ **NO TAG, NO MERGE
until G-R7(ii) passes on the owner's post-delivery Concorde pass; cpu/both deletion is the NEXT
cook's decision item. Journal 058 is self-contained.**
🚨 **SESSION 059 (2026-08-24): THE VISIBLE MASK "HITCH" WAS NEVER A PERF STALL — it is a
DISPLAY-ONLY STALE-PRESENT defect (mask SVE = designated final writer via AcceptOverrideIfLastPass,
`Inputs.OverrideOutput` ignored ⇒ screen target never written on armed frames ⇒ swapchain
re-presents stale frames), present since m26, DATASETS NEVER AFFECTED (owner Blender-reassembly
control; per-engine-frame t_wall excess ≤ +0.4 ms). Owner-isolated by eye/OBS on the bench;
mechanism MEASURED before the fix (A-I1: overrideOutput=1 on 29/29 armed frames). FIXED at
`b05066f` on the branch (OCIO pattern, both SVE callbacks, every return path; helpers per-file
NAMED differently — unity build makes an identical static in two cpps a C2084). Chain on branch:
`ead7764` predictions AMENDMENT 1 → `310a87f` A-I1 instrument → `b05066f` fix → `d3b9f08` journal
059. ALL fix gates green incl. the OWNER EYE GATE (rubberband gone, smooth). Staged exe now
`7F37A4AC` (archived; container = m34 quartet unchanged).**
⚖ **THE 054 CONCORDE BISECT IS RE-READ AS TWO STACKED PHENOMENA (chat-accepted): (1) real
throughput starvation at 6.4 MP (m34's GPU reduction addresses it; bench never had it) + (2) this
display defect. G-R7(ii) IS SPLIT: eye/OBS judges ONLY the display fix; THROUGHPUT is read
EXCLUSIVELY from the m33 wall instruments (t_wall span vs frames/VideoFps, speed_ratio with
game_clock_speed_ratio). The eye is never again the throughput instrument.** Monday's delivery
build ships WITHOUT the fix (chat venue ruling — branch only; client note amended chat-side).
⚠ Instrument lesson: consecutive labels.jsonl t_wall deltas measure CAPTURE CADENCE (3-tick gaps
read as exact-100 ms "stalls" at 30 fps) — always normalise by frame_index delta. ⚠ PS 5.1: a
`git commit -m` message containing embedded double quotes silently becomes pathspecs — NO commit.

🚨 **G-C FIELD RESULT (2026-08-23): PLUGIN HALF PASSED (ratio 1.105 → stamped 27.149 == annotation
fps, honest chain end-to-end); THE WATCHER OVERRODE THE HONEST STAMP with 29.993 ⇒ MP4 played
~1.10× fast.** Fixed as **AMENDMENT 2** (plugin `2b6c93f` predictions+readme on master, AnomDash
`ea19e16` watcher). Chat's spike-blindness diagnosis ACCEPTED; **chat's ruled discriminator
(session_index contiguity ⇒ span estimator) REFUTED BY MEASUREMENT and NOT shipped — every banked
session is INDEX-contiguous and NONE is TIME-contiguous (game-delta histogram 74×1-tick /
15×3-tick), so span under-reads 23–25 % everywhere and would have pushed G-C's honest 27.149 to
~20.3.** Shipped: **(2a) estimator = MEAN over intervals whose GAME-time delta is ONE TICK** (tick
period derived from data; gap-immune from artifact evidence, spike-SENSITIVE, measures the same
quantity the plugin does; median fallback if `t` unusable) · **(2b) a session carrying
`game_clock_speed_ratio` (m33+, gated stamp) is NEVER overridden — disagreement is a NOTE and the
stamp wins; pre-m33 keeps the override.** Both flagged ratify-or-revert. G-W 8 fixtures green incl.
guard-proven-by-breaking on (e) and the 2b branch proven BOTH ways by (b)/(h). ⚠ Fixture (g) as
designed was VACUOUS (estimator agreed) — recorded, replaced by (h). Owner still owes the field
re-encode (delete `.mp4_done` on the G-C session ⇒ expect SILENT, 27.149, true by stopwatch).
🖥 **SHARED-WORKING-TREE HAZARD, LIVE: the m34 session runs in the SAME repo/working tree on this
box.** Git branches are per-repo, so its `checkout` moved my tree onto `feature/mask-gpu-reduce`
and my master-side edit got staged there. **Do master-side work from an isolated `git worktree`
(scratchpad), never by switching branches in the main tree**; path-scope every `git add`; never
`add -A` (the owner's untracked `docs/CHAT-HANDOFF-crisis-weekend-delivery.md` would be swept).
m34 is committing correctly on its own branch (`0fc00ef`, ahead 1).

⚖ **ROLE RULING, PERMANENT: two Code instances exist. THIS box is the ONLY canonical author; the
office instance is eyes/builder/runner only — it commits NOTHING; everything reaches Concorde by
git pull; NOTHING leaves the office machine.**

Tags: **highest is m30** (m28 output-height, m29 corrupted_texture, m30 lod_popping +
camera_clipping — delivered pool is FIVE). Cold-start tag check: `git rev-parse --short
<tag>^{commit}` (G143 — annotated tags).

Standing (unchanged): veto is ZERO-ONLY — **never propose a ratio/threshold (journal §209)** ·
m26 is a PARTIAL cure (over-claim ships; does not close P1 or cure H4) · H6 documented-not-fixed ·
G134 permanent (mask can't see Nanite on 5.1) · L3 live, deliberately unfixed · don't touch
`feature/stencil-capture` (`76cac74`) · **P6 does not move** · don't start a new milestone
unprompted · G76 (PIE licenses mechanism only) · source carries zero comments (`_strip_comments.py`
before commit) · [[push-workflow-inverted]] · [[reports-in-copy-block]].
