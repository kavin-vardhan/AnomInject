# m35 BUILD B — pre-declared gates (plugin-owned sub-rect copy + whole-texture readback)

**WRITTEN AND COMMITTED BEFORE ANY BUILD B LEG RUNS.** Restate verbatim before reading any result.
Build A's two banked legs are the reference:
`session_20260826-152855_letterboxed` and `session_20260826-152922_noletterboxed`.

---

## 0. WHAT BUILD A ESTABLISHED (measured, not read) — the premise these gates rest on

| | sourceExtent | rect | picture | bufferHeight | rowPitch | fmt |
|---|---|---|---|---|---|---|
| letterboxed | 821x869 | (0,263)-(821,607) | 821x344 | **869** | 832 | 18 |
| un-letterboxed | 821x869 | (0,0)-(821,869) | 821x869 | **869** | 832 | 18 |

`bufferHeight 869 == sourceExtent.y 869` on BOTH legs ⇒ **stock UE 5.1 allocates FULL-SOURCE-SIZE
staging and copies the sub-rect to its own position — MEASURED on the engine we build against**, not
inferred from `RHIGPUReadback.cpp:156/:172`. The unconditional offset removal is therefore refuted by
measurement as well as by source. Build A's pre-`m35` indexing is CORRECT here, which is exactly what
makes its frames a valid reference.

Two facts the leg added:
- **`fmt=18` is `PF_A2B10G10R10` — byte-identical to the format Bates crashed on.** The home letterbox
  lever is a closer Bates proxy than assumed.
- **`rowPitch 832` vs `width 821` = 11 px of padding** (832*4 = 3328 = 13*256, D3D12 256-byte
  alignment). This is the EMPIRICAL basis for rejecting the BufferHeight/pitch layout sniff: at a
  pillarbox narrower than the padding the two engine layouts become numerically indistinguishable, so
  the sniff has a physical blind spot and fails silently inside it. The rejection is measured, not
  argued.

---

## 1. THE POSE PRECONDITION (A47 / B1 / A64) — WHY THE GATES ARE SPLIT IN HALF

Byte identity of frames is **NOT** available even between two runs of the SAME binary: the PIE camera
pose bifurcates on settle, which is why C1's original form was unsatisfiable and why `G-F2` is
specified AT THE MODAL POSE. So every equivalence gate below splits:

- **POSE-INDEPENDENT HALF — compared on EVERY attempt, regardless of pose.** Frame dimensions, frame
  count and file names, `readback_layout` fields, `annotation.json` key set, `run_summary` key set,
  guard silence, clamp silence, drop counters. None of these depend on where the camera settled.
- **POSE-DEPENDENT HALF — pixel bytes.** Compared only on a pose-matched A/B pair.

**THREE-ATTEMPT CAP.** If no pose-matched pair is obtained in three attempts, the pose-dependent half
is reported **NOT OBTAINED**, not failed, and the pose-independent half still stands on its own.
A pose MISMATCH makes that half **INVALID, not FAILED** (A63/A64) — banked and re-run, never read as
a Build B defect. Pose is judged from the modal camera rotation and `coverage_ratio`, and **both
numbers are printed either way**: a discriminator, never a silent gate.

---

## 2. GATES

### G-M1 — LETTERBOXED EQUIVALENCE (lever at ~2.39, seed 4242, same map/config as Build A)
- **M1a (pose-independent):** written frames are **821x344** — the PICTURE's size, not the window's;
  90 frames, 0 zero-byte; `readback_layout` reports `sourceExtent=821x869 rect=(0,263)-(821,607)
  picture=821x344`; **`annotation.json` key set 48, unchanged**; `run_summary` adds `readback_layout`
  and nothing else.
- **M1b (pose-independent):** **THE GUARD IS SILENT and THE CLAMP IS SILENT.** Zero
  `READBACK-GUARD FIRED`, zero `EXTENT-CLAMP FIRED`, drop counters 0.
- **M1c (pose-dependent):** frames byte-identical to Build A's letterboxed leg.
- **M1d (eyes):** Kavin confirms one frame — full picture, no band, no shift.
- ⚠ **`bufferHeight` WILL NOW READ 344, NOT 869, AND THAT IS THE FIX WORKING.** We own the texture,
  so the buffer is picture-sized BY CONSTRUCTION. Reading 344 as "this became a 5.2+ engine" is the
  misreading this line exists to prevent.

### G-M2 — UN-LETTERBOXED EQUIVALENCE (lever off / reverted)
Same four sub-gates against Build A's un-letterboxed leg. Frames **821x869**, `rect=(0,0)-(821,869)`,
guard and clamp silent. **This is the leg that proves the shipped path did not move.**

### G-M3 — GUARD PROOF-BY-BREAKING (`IAI.Bench.ReadbackGuardInflate <rows>`)
- The guard **FIRES**: `READBACK-GUARD FIRED` at Error, carrying rect / W / H / checkedH /
  rowPitchInPixels / bufferHeight / inflateRows, with `inflateRows` NON-ZERO so the line says of
  itself that it was provoked.
- Frames are **DROPPED AND COUNTED** — `total_frames` short by the dropped count, dropped frames
  absent from disk.
- **NO CRASH, and the run COMPLETES and writes its artifacts.**
- Knob back to 0 ⇒ guard silent again. **Proven BOTH WAYS (`G96`)** — a guard that has only ever been
  silent is not a guard.

### G-M4 — THE DISPLAY FIX MUST REPRODUCE (`b05066f`, NOT m34 — see AMENDMENT 2 §A2.1)
m35 rewrites `AfterPass_RenderThread`, the function the display fix routes through, so its own gates
are re-run: **`A-I1`** (the `M23 PASS` line reads back `overrideOutput=1`) and the **`G-F2` compare**.
**`b05066f`'s numbers must reproduce.** `FinalizeSveAfterPassOutput` and its `OverrideOutput`
handling are on EVERY return path of the rewritten function, including both new early-outs.
⛔ If they do not reproduce, m35 has undone the display fix — **STOP and report**, do not tune.

### G-M5 — m34's BENCH GATES ON THE SVE PATH
Re-run exactly the bench gates m34 passed (`G-R1..G-R6` as applicable, SVE path). **Must stay green.**
The mask drain gained a bounds guard and nothing else; a guard failure there lands in the existing
`NOT_MEASURED` ⇒ **ADMIT** direction, so the m26 safety property is unmoved by construction.

### G-M6 — HOOK-COST PRIOR (an ATTRIBUTION INSTRUMENT, not a number)
Measure at home on this branch, **Build A vs Build B, BOTH capture paths**, and record it as a stated
PRIOR before the branch goes to Concorde. Reason: `G-R7(ii)`'s throughput half was pre-declared to
isolate m34, and m35's extra per-armed-frame copy puts a second variable in it. **Without a home prior
a throughput reading on Concorde cannot be attributed to either milestone.**
⛔ **NUMBERS ONLY. NO THRESHOLD IS PROPOSED OR IMPLIED**, and none must be.
⚠ Build A's own two legs already show `speed_ratio` 1.1677 (letterboxed, ran FIRST) vs 1.0123
(un-letterboxed, ran second) — a 15 % spread with the SMALLER copy on the slower leg. That ordering is
consistent with warm-up (`G66`) and is recorded as an **association, not a mechanism**. The prior must
therefore control for run order, not just for build.

---

## 3. FAILURE BRANCHES

- **F-B1 — pixels differ at a MATCHED pose.** The plugin-owned copy is not reproducing the engine's
  sub-rect. ⇒ **STOP**, do not tune; report the diff location (which rows/columns).
- **F-B2 — the clamp fires on a normal home leg.** Then the view rect is outside the scene-colour
  extent ON THIS HOST, which would be a finding about StackOBot, not about m35. ⇒ report before
  reading anything else.
- **F-B3 — `A-I1` / `G-F2` do not reproduce.** m35 has disturbed `b05066f`. ⇒ **STOP and report** —
  that is a design question (can the two coexist), not an implementation one.
- **F-B4 — the guard cannot be made to fire by the knob.** Then the guard is unreachable and its
  silence on M1b/M2b means nothing. ⇒ the guard is unproven; treat M1b/M2b as VOID.

⚠ **NOT-CRASHING IS NOT A PASS CONDITION ANYWHERE IN THIS FILE.**

---

## 4. WHAT BUILD B CHANGES ABOUT THE BATES / DEIMOS PHOTO — STATED, NOT DISCOVERED LATER

The section-6 branch table was written for the PRE-m35 drain, where `bufferHeight` vs `sourceExtent.y`
discriminated the engine's staging layout. **Build B removes that discriminator by design** — we own
the texture, so `bufferHeight == picture height` on every engine.

**What a Bates/Deimos photo of `READBACK-LAYOUT` decides UNDER BUILD B:**

| reading | conclusion |
|---|---|
| `rect` inside `sourceExtent`, no `EXTENT-CLAMP` line, picture correct | the host's view rect and scene-colour texture agree; **D-2 REFUTED**; the fix is working |
| an `EXTENT-CLAMP FIRED` line instead | the view rect is OUTSIDE the source texture ⇒ **D-2 CONFIRMED** — the coordinate spaces disagree on that host, and the frame was dropped rather than silently mis-captured |
| `rect.min.y > 0` with a correct picture | the host letterboxes AND the sub-rect origin is being applied correctly — the Bates crash condition, now handled |

⇒ **D-1 (fork allocates rect-sized) vs stock is NO LONGER DISCRIMINABLE — and no longer matters**,
because the engine's staging layout is irrelevant by construction. That is the design working, but it
is a REAL CHANGE to what the photo can answer versus what the ruling's §3 assigned it. Recorded here
rather than left for whoever takes the photo to discover.
📌 **Deimos (5.3) is still worth the photo**: under Build B it should report a correct picture with
`bufferHeight == picture height`, and — being 5.2+ — it is the host where the PRE-m35 code would have
been wrong at a non-zero origin. It confirms the fix, not the layout.

---

## 5. STANDING CONSTRAINTS

- `P6` DOES NOT MOVE. `annotation.json` key set 48, measured against a banked baseline, not asserted.
- `run_summary` gains `readback_layout` and nothing else (measured against a post-m33 baseline).
- No ratio, no threshold, anywhere.
- `feature/stencil-capture` untouched. Master untouched — **and master STILL CARRIES THE CRASH.**
- ONE ROUTE ONLY: m35 reaches master by the branch MERGE, never also by cherry-pick.

---

## 6. CORRECTION (2026-08-26, owner-raised, CONFIRMED FROM SOURCE) — THE COPY IS **PER CAPTURED FRAME**

Every earlier description of m35's added copy as a **per-armed-frame** cost is WRONG, and is corrected
here rather than edited above. Confirmed from the call site, not taken on trust:

- `UAnomalyCaptureSubsystem::CaptureCurrentFrame()` — `AnomalyCaptureSubsystem.cpp:1736`, called from
  the capture FSM at **`:569`, `:579`, `:589`**.
- Inside it: **one** RequestId minted (`:1752`) → **one** readback armed (`ArmWanted` `:1769` SVE /
  `ArmForCapture` `:1773` backbuffer) → `++SessionFrameIndex` (`:1777`).
- `SessionFrameIndex` **is** the captured-frame index: it names the output file at `:1790`
  (`Actual_Frames/frame_%05d`) and is what `FrameCap` is tested against at `:551`.

⇒ **ONE ARM == ONE CAPTURED FRAME == ONE OUTPUT PNG, strictly 1:1.** The SVE copy pass and the
backbuffer copy therefore execute **once per CAPTURED frame** — 30× per second at 30 fps, for the
whole capture.

📌 **WHY THE WORDING SLIPPED, so it does not recur:** the **MASK** arms a few times per burst, and
"per armed frame" was true of the mask. Receipts from this session's own legs — guard leg
`READBACK-GUARD FIRED` = **90** for a 90-frame cap (90 drained colour readbacks) against `M23 PASS`
= **29** mask arms in the CTRL49 leg. Two different arm rates, one phrase.
⇒ It changes the WEIGHT of the cost question (`G-M6`), not the design.

---

## 7. THE CRASH BRANCH (appended after it fired, 2026-08-26)

Build B's first leg **crashed at capture start**: `State != D3D12_RESOURCE_STATE_COMMON`.
**ROOT CAUSE:** the RDG texture was created with `TexCreate_ShaderResource` only, and the D3D12
**transient** allocator derives an initial state solely from RenderTarget / DepthStencil / UAV flags.
**FIX:** add `TexCreate_RenderTargetable` to `OwnDesc`.

⚠ **ASYMMETRY, DELIBERATE AND RECORDED:** the flag is added to the **RDG (SVE)** texture only. The
backbuffer path's texture is a persistent `FTextureRHIRef` created with
`.SetFlags(ETextureCreateFlags::ShaderResource).SetInitialState(ERHIAccess::CopyDest)` — not
transient, so it declares its initial state directly and needs no render-target flag. The asymmetry is
named in that path's own log line so a reader does not "tidy" the two into agreement.

⛔ **A CRASH IS NOT A GATE FAILURE OF THE DESIGN** — it was an allocator-contract error in the
implementation. But it IS why `G-M1`/`G-M2` are re-run after any change to `OwnDesc`.

---

## 8. GATES ADDED AFTER THE FIRST BUILD B LEGS (pre-declared here BEFORE they run)

### G-M7 — THE BACKBUFFER LEG (owner-approved 2026-08-26)
The backbuffer path got the same owned-copy treatment and **has never been exercised on Build B**.
One leg with `IAI.Capture.SVE 0`, letterboxed and un-letterboxed:
- frames are the PICTURE's size; guard and clamp **SILENT**; drop counters 0;
- the loud clamp (drop if the granted rect ≠ the requested rect) **does not fire** on a normal leg;
- `capture_path` reads `backbuffer` and `readback_layout` reports `bufferHeight == picture height`.
⛔ **A backbuffer format mismatch has NO assert and NO graceful failure by default** (§11) — so a
wrong-looking picture is the only signal, and the leg must be LOOKED AT, not only parsed.

### G-M8 — PILLARBOX / NON-ZERO `Rect.Min.X` (the coverage gap the letterbox lever cannot reach)
`Rect.Min.X` has been **ZERO in every leg ever run**, so the X half of the sub-rect origin is
UNTESTED. A pillarboxed leg (aspect NARROWER than the window) must give `rect.min.x > 0` with a
correct picture, guard and clamp silent.
**A COLUMN CHECKER IS REQUIRED, AND IT IS VALIDATED AGAINST A KNOWN ANSWER FIRST** — the
owner-supplied datum from the un-letterboxed frame: **0 near-black columns, left-edge column mean
well above zero (~50, min 5.0), right-edge ~87, no collapsed band at either edge.** A checker that
cannot reproduce that on the known-good frame does not get its verdict read on the pillarboxed one.
⛔ A row-only checker is BLIND to a horizontal defect — that is why this gate exists.

### G-M9 — WITHIN-FRAME DUAL-PATH COMPARATOR (owner-designed; the strongest available equivalence test)
Bench cvar, **default OFF**. When on, the SVE path enqueues a **second, independent** readback in the
**OLD form** (whole source texture, engine-chosen staging, sub-rect indexed with the `Rect.Min.Y`
offset) alongside the new owned-copy readback, **on the same frame**, and byte-compares the two
drained pictures.
- **PREMISE (a), VERIFIED BEFORE BUILDING:** RDG executes passes in handle order and contains no
  sort/reorder, and both passes are added consecutively and are read-only w.r.t. the source ⇒ they
  are guaranteed to see the same contents. That is what makes a within-frame compare sound where a
  cross-run frame compare is not (§9).
- **SELF-PROOF REQUIRED, BOTH DIRECTIONS (`G96`):** (b) it must be shown it CAN report a non-zero
  diff (provoke one), and (c) with the cvar OFF the build must reproduce Build B exactly.
- 🚨 **ITS OUTPUT LINE MUST NAME BOTH CAUSES OF A NON-ZERO DIFF AND SAY WHICH TO CHECK FIRST:**
  (i) the owned copy is not reproducing the sub-rect, or (ii) the added pass has broken the adjacency
  the premise rests on. **Check adjacency FIRST** — a broken premise makes the comparison
  meaningless rather than failing. **The comparator is its own guard.**

---

## 9. WHY THERE IS NO CROSS-RUN FRAME-BYTE GATE (measured, not conceded)

Strict frame byte-identity across two runs was **unobtainable in every environment measured**:
- MainWorld: mean |Δ| **4.42**, max **225**, **78 %** of pixels differ.
- CB_GateLevel control pair (`M33_CTRL_A` vs `M33_CTRL_B`, same binary): mean |Δ| **0.00117**,
  max **3**, **0.116 %** of pixels — concentrated in the lower half, top four grid rows exactly
  `0.000`, and **no corner box**, so it is **not** `G125`'s frame marker.
⇒ `G-M1c` / `G-M2c` are reported **NOT OBTAINED**, never FAILED, and `G-M9` is the replacement
instrument. **A pose-matched pair is necessary and not sufficient for byte identity.**

---

## 10. `G-M6`'s ANSWER, AND WHY A REBUILD CANNOT SUPPLY IT

There is **no hook-cost field in any artifact**. The available proxy is the median consecutive
`t_wall` delta from `labels.jsonl`, and it read **0.03334 on all five legs with `paced=True`** — the
**m11 pacer pins the tick to `1/VideoFps`**, so it absorbs any sub-budget hook cost and a paced leg
cannot measure one. ⇒ the prior must be taken with **pacing OFF**, by **swapping the archived
predecessor exe** (`StackOBot.exe.m34-fix-candidate-7F37A4AC`) against the staged Build B
(`733FE83C`), order-matched, hashes re-verified at every swap. **No rebuild, no owner leg.**
⛔ **NUMBERS ONLY. Report per-captured-frame ms AND per-megapixel; any Concorde (3200×2000) figure is
an EXTRAPOLATION and must be labelled one. NO THRESHOLD.**
📎 Coarse reading already in hand: both Build B un-letterboxed legs read `speed_ratio` exactly
**1.0000 / 30.000** vs Build A's **1.0123 / 29.635** — consistent with the pacer absorbing the cost,
and **not** a cost measurement.

---

## 11. THE BACKBUFFER FORMAT ANSWER

`FValidationRHIUtils::ValidateCopyTexture` (`RHIValidationUtils.h:10-45`) DOES carry
`checkf(bValidCopyFormats, ...)` plus source/dest bounds checks — **but the whole file is behind
`#if ENABLE_RHI_VALIDATION` (line 5), which is OFF in a default Development build.** The D3D12
backend's `RHICopyTexture` (`D3D12Texture.cpp:2868+`) has an `ensure()` for block alignment and **no
format check**. ⇒ on the shipped path a format mismatch is **undefined behaviour** — no assert, no
loud drop.
🚨 **The structural guarantee is therefore the ONLY protection on the backbuffer path**, which raises
rather than lowers the value of reading the format from `BackBuffer->GetFormat()` every frame.
✅ Recreate logic verified safe: `SrcFormat` is read from THIS frame's backbuffer, compared against
the cached `OwnFormat`, and the texture recreated **before** the copy in the same straight-line
block; `Item.Format = SrcFormat` is what the drain uses for BPP. No path copies into a stale-format
texture, not even for one frame. A creation failure DROPS the frame with a reason; there is no
fallback. Contrast the SVE path, where `AddCopyTexturePass`'s `checkf` is unconditional.
📌 This matters for the office-pass HDR preview-format item — exactly the scenario where the format
changes under us.

---

## 12. RESULTS LEDGER — every leg run, with its verdict

| leg | session / bank dir | verdict |
|---|---|---|
| Build A letterboxed | `session_20260826-152855_letterboxed` | reference |
| Build A un-letterboxed | `session_20260826-152922_noletterboxed` | reference |
| Build B letterboxed | `session_20260826-160750` | `G-M1a`/`G-M1b` **PASS**; `bufferHeight` 869 → **344** = the fix working |
| Build B un-letterboxed | `session_20260826-160834` | `G-M2` **PASS** — NOTHING moved |
| Build B guard (inflate 1) | `session_20260826-160905` | `G-M3` **PASS** — 90 guard drops, 0 frames on disk, run completed and wrote artifacts |
| Build B restore (inflate 0) | `session_20260826-160926` | guard silent again — proven BOTH ways |
| first Build B attempt | `session_20260826-155749_DEAD_PARTIAL_BUILDB_CRASH_0_FRAMES` | the crash (§7) |
| `M35_M35_GM4_CTRL49` | banked | `G-M4` **PASS** — A-I1 29/29 `overrideOutput=1`, zero at 0; `G-F2` 29 IDENTICAL / 0 FIRST-DIFF; all 8 events in the known band with both endpoints (66,843 / 66,878) exact |
| `M35_M35_GR3_RAMP2` | banked | **INVALID** (bare token matched nothing: `zero_match_bursts=8`, 0 events) — kept per `A63`; my leg-design error, not a build result |
| `M35_M35_GR3_RAMP2B` | banked | `G-M5` **PASS** — NOT_MEASURED ×8, MEASURED_ZERO **0**, `nopass=29` = resolved arms, `vetoed=0`, COMPARE 29 IDENTICAL |
| `M35_M35_GR3_SPLINE` | banked | `G-M5` **PASS** — MEASURED_ZERO ×8, 8 `VETOED-OBJECT`, `vetoed_events=8`, annotation anomalies **0**, COMPARE 29 IDENTICAL |
| `M35_M35_PKG_LB_PROBE` | banked | **VOID** — lever refused (§13) |

`READBACK-GUARD FIRED = 0` and `EXTENT-CLAMP FIRED = 0` on every non-guard leg.

⚠ **REPLAYABLE TARGET TOKENS — recorded because they exist NOWHERE ELSE in replayable form** (they
had to be recovered from banked `run.json` files; a bare class name matches nothing):
- `SM_Ramp2_UAID_B42E9936F5429ADA00_2086822137`
- `BP_SplineSpawn_C_UAID_A85E45CFE40412DE00_1511100424`

---

## 13. `G-M7` / `G-M8` CANNOT RUN PACKAGED — MEASURED, NOT ASSUMED

A packaged leg with `IAI.Bench.Letterbox 2.39` ordered before `Capture.Start` returned:
`Capture(bench): LETTERBOX REFUSED - view target 'SpectatorPawn_2147482483' has no UCameraComponent.`
The bench pawn under `-unattended` is a **SpectatorPawn** with no camera component, and that leg's
`READBACK-LAYOUT` read `rect=(0,0)-(1280,720)` with a bbox identical to the un-letterboxed leg.
🎯 **THE REFUSAL PATH EARNED ITS KEEP:** a silent no-op there would have read as a letterboxed PASS.
The leg is **VOID by the pre-declared rule**, not a pass.
⇒ **`G-M7`, `G-M8`, and `G-M9`'s both-origins half must run in PIE on MainWorld** (view target
`BP_Bot_C_0 / FollowCamera`). `G-M9`'s zero-origin half CAN be self-proven packaged.
📌 UNTESTED AND CHEAP: a packaged **MainWorld** leg might supply a camera-bearing pawn and move
`G-M7`/`G-M8` back to unattended. Not attempted.
⛔ Cvars ARE reachable packaged — the Letterbox command EXECUTED and failed on a semantic
precondition, not on being unknown. `IAI.Capture.SVE`, `IAI.Bench.ReadbackGuardInflate`,
`IAI.Capture.MaskReduce` and the future `G-M9` cvar all reach a packaged leg.
