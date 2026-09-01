# m35 BUILD A — pre-declared predictions (readback-layout telemetry + letterbox lever)

Written and committed **BEFORE** Build A is compiled and **BEFORE** any leg runs.
Restate this file verbatim before reading any result.

Build A is **current indexing + telemetry + the letterbox lever ONLY**. There is no
behaviour change: the drain still indexes `Base + ((Rect.Min.Y + y) * RowPitch + Rect.Min.X) * BPP`,
no guard, no plugin-owned copy. Build A exists to turn a source read into a measurement
and to produce the REFERENCE FRAMES that Build B must reproduce.

---

## 0. WHAT STEP 0 ALREADY ESTABLISHED (source read, read-only, no checkout)

Read from the Epic remote's release tags in `D:\UESource\UnrealEngine`:

| Engine | Staging texture size | Copy dest position |
|---|---|---|
| **5.1.1** | `SourceTexture->GetSizeXYZ()` — FULL SOURCE (`RHIGPUReadback.cpp:156,160`) | `CopyInfo.DestPosition = CopyInfo.SourcePosition` — **the rect's own position** (`:172`) |
| **5.2.1** | `Rect.X2-Rect.X1 x Rect.Y2-Rect.Y1` when the rect is valid (`:158-165`) | **never set** ⇒ default `FIntVector::ZeroValue` ⇒ **0,0** (`:180-188`) |
| **5.3.2** | `Size` when positive, else `GetSizeXYZ()` (`:152-159`) | **never set** ⇒ **0,0** (`:187-204`) |

⇒ **The layout changed at 5.2, not 5.3.** 5.3 keeps a `FResolveRect` compat overload that
forwards to the new signature, and `AddEnqueueCopyPass` still takes `FResolveRect` in 5.3, so
our call sites compile unchanged on 5.1/5.2/5.3.

⛔ **NOT ESTABLISHED AND NOT GUESSED:** why a 5.1-based fork (Bates) behaves like 5.2+.
Back-port, vendored RHI, something else — **cause not established**, and the design deliberately
does not depend on the answer.

---

## 1. PREDICTIONS — LETTERBOXED LEG (the lever ON, ~2.39 on a 16:9 viewport)

Source basis for the lever: `CameraStackTypes.cpp:140-144` → `UnrealClient.cpp:2041-2048`.
A requested aspect WIDER than the viewport's gives `Result.Min.Y = round(0.5*(SizeY-NewSizeY))`,
i.e. a centred letterbox — the Bates shape (`dH_slate = -2 * Min.Y`).

- **A-1 — the lever bites.** The `READBACK-LAYOUT` line reports `rect.min.y > 0` and
  `rect.height < sourceExtent.y`. `rect.min.x == 0` (bars top/bottom, not sides).
- **A-2 — THE DECIDING ONE. `bufferHeight == sourceExtent.y`**, i.e. the mapped staging surface
  is FULL-SOURCE height, not `rect.height`. This is the direct measurement of
  `RHIGPUReadback.cpp:156` on the engine we actually build against.
- **A-3 — `rowPitchInPixels >= sourceExtent.x`** (D3D12 pitch alignment can exceed it; it must
  never be less).
- **A-4 — THE PICTURE IS CORRECT.** Kavin's eyes on one saved frame: the full game picture,
  **no black band at top or bottom**, nothing shifted. The saved frame's dimensions equal
  `rect.width x rect.height` (the PICTURE's size), NOT the window's.
- **A-5 — no crash, run completes, `total_frames` matches the configured cap.**
- **A-6 — `run_summary.readback_layout` carries the same six fields as the log line**, and
  `annotation.json`'s key set is UNCHANGED (`P6` does not move).

## 2. PREDICTIONS — UN-LETTERBOXED CONTROL LEG (lever never applied / reverted)

- **A-7 — `rect.min.x == 0 && rect.min.y == 0`** and `bufferHeight == sourceExtent.y`.
- **A-8 — picture correct** (this is today's shipped behaviour; it must not move).

---

## 3. FAILURE BRANCHES — PRE-DECLARED, EACH WITH ITS RESPONSE

- **F-1 — `bufferHeight == rect.height` on the letterboxed leg (i.e. A-2 fails).**
  Then stock 5.1 does NOT allocate full-source staging and my step-1 source read of
  `RHIGPUReadback.cpp:156` is WRONG. ⇒ **STOP.** Do not build Build B. The whole
  three-layout picture is re-derived from measurement before anything else happens.
- **F-2 — the picture is WRONG (band, shift) or the run CRASHES on the letterboxed leg.**
  Then the current indexing is not correct on this engine either, and the brief's own step-2
  stop applies: **STOP — the mechanism is wrong.**
- **F-3 — the lever does not produce `rect.min.y > 0`** (A-1 fails).
  Then the lever is a NO-OP and its clean result is an ARTIFACT OF INSULATION, not evidence
  (`G114`'s shape — a lever that does nothing yields a null indistinguishable from a pass).
  ⇒ the leg is **INVALID, not passed**; fix the lever, re-run, do not read A-2/A-4 from it.
- **F-4 — `sourceExtent` reads 0x0 or the layout line never prints.**
  The telemetry is blind. ⇒ leg INVALID; no claim in either direction.

⚠ **NOT-CRASHING IS NOT A PASS CONDITION ANYWHERE IN THIS FILE.** A-4 is an eyeball gate on
the picture; A-2 is a numeric gate on the buffer. A leg that merely fails to crash has
established nothing.

---

## 4. WHAT BUILD A DELIBERATELY DOES NOT DO

- No bounds guard (that is Build B).
- No plugin-owned sub-rect copy (that is Build B).
- No loud extent clamp (that is Build B; the backbuffer path's existing SILENT clamp is
  left exactly as found).
- No change to the mask SVE.
- No indexing change anywhere.

## 5. WHY BUILD A'S FRAMES ARE THE REFERENCE

Today's code is **measured-correct on this engine** (that is what A-2 + A-4 establish). So
Build A's saved frames — letterboxed and un-letterboxed — are the byte-level reference that
Build B's frames must reproduce IDENTICALLY. If Build B's frames differ from Build A's on the
same gate scene and seed, Build B is wrong, regardless of how reasonable its code looks.
