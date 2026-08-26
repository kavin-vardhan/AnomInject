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
