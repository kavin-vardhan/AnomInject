# PART FOURTEEN — PRE-DECLARED BRANCHES

**Written and committed BEFORE any result was read.** Restated verbatim in the journal before each
result. A leg is discarded for HOW IT RAN, never for WHAT IT SHOWED (A63); every attempt is banked.

---

## M-1 — DOES THE DRAIN FORCE THE READBACK HOME?

**The question.** `DrainTail` is `FMath::Max(10, ViewLagFrames + 4)` = **10** frames;
`feature/stencil-capture` budgets **12** (`HeldAges[i] > 12`). **10 < 12.** Is that a real conflict?

⚠ **A REFRAME THAT MUST BE STATED BEFORE THE RESULT: the "12" is a BUDGET THE BRANCH AUTHOR CHOSE,
NOT A MEASURED LATENCY.** Nobody has measured what a GPU readback on this box actually costs. The
honest instrument therefore measures **the real readback latency**, and the 10-vs-12 question is
answered by comparing that latency to both numbers.

**The instrument.** On the EXISTING colour readback path (`FAnomalySveCapturer`), record
`GFrameNumberRenderThread` at `SubmitInFlight_RenderThread` and again when `IsReady()` first returns
true in `Drain_RenderThread`. The difference is the readback latency in render frames. Also count how
many `DrainAsyncToCompletion` iterations (of its 8, each with a blocking `FlushRenderingCommands()`)
are actually consumed at run end.

**Why the colour readback is the right proxy, stated in advance:** it is armed in the same frame and
the same graph as the mask would be, via the same `AddEnqueueCopyPass` →
`FRHIGPUTextureReadback` → `IsReady()` mechanism. ⚠ **Its payload is larger** (BGRA vs R8), so its
latency is an **upper bound** on a mask's, not an equal.

### Pre-declared branches

| # | branch | reading that selects it | what it means for the design |
|---|---|---|---|
| **B1** | **DRAIN FORCES IT HOME, COMFORTABLY** | max observed latency **≤ 8** render frames, **0** frames dropped, and `DrainAsyncToCompletion` consumes **few** of its 8 iterations | **10 < 12 NEVER BITES.** Shape (c) needs no lifecycle change |
| **B2** | **DRAIN FORCES IT HOME, BUT ONLY VIA THE FLUSH** | 0 dropped, **but** frames remain pending when `DrainTail` ends and are resolved only by `DrainAsyncToCompletion`'s blocking flushes | Works today, **but the safety margin is the flush, not the tail.** Must be stated in the design as a dependency |
| **B3** | **DRAIN DOES NOT FORCE IT HOME** | any frame dropped / unresolved at `WriteSessionAnnotationFile` | 🚨 **The design needs an EXPLICIT WAIT at `FinishRun` — a LIFECYCLE CHANGE, which is the OWNER'S RULING, not mine.** HALT and report |
| **B4** | **IT DEPENDS ON N, WITH A THRESHOLD** | latency varies systematically with how close to run end the frame was armed | Report the threshold as a number; the design must budget above it |
| **B5** | **INVALID / NOT MEASURED** | instrument did not report, leg failed for a HOW-IT-RAN reason | Discard the leg, bank it, re-run. **NOT a result.** |

⛔ **NO GRADING.** This is a measurement feeding a design, not a certification. No A54, no B1 pose
gate verdict is claimed.

---

## M-2 — IS A COUNT SUFFICIENT, AND WHAT DOES THE HARDWARE PATH COST?

**The question.** Per `T-4`'s reframe: a veto needs only a COUNT; only fixing the label RECT needs
the mask. If `RQT_Occlusion` supplies a usable count, the mask, the readback and the W×H scan
disappear from the veto path.

### Pre-declared branches

| # | branch | reading that selects it | consequence |
|---|---|---|---|
| **C1** | **COUNT SUFFICIENT, HARDWARE PATH USABLE** | `RQT_Occlusion` returns a mesh-accurate drawn-pixel count for a target at acceptable latency/cost | **The mask leaves the veto path entirely.** Cheapest possible cure |
| **C2** | **COUNT SUFFICIENT, HARDWARE PATH NOT USABLE** | a count answers the veto, **but** `RQT_Occlusion` cannot deliver a mesh-accurate one | **The mask stays**, justified by a correctness reason rather than by cost. Cost of the hardware path becomes **MOOT and must not be measured to look thorough** |
| **C3** | **COUNT INSUFFICIENT** | the veto provably needs spatial information, not a scalar | The mask stays and the rect question re-opens |
| **C4** | **NOT DETERMINABLE WITHOUT MORE WORK** | source does not settle it and measuring needs work beyond the permitted narrow scope | Say so plainly; do not estimate |

⚠ **STATED IN ADVANCE, because it decides whether cost is even worth measuring:** if the hardware
path is disqualified on **CORRECTNESS**, measuring its **COST** is theatre. A disqualified option is
not made more disqualified by a millisecond number. **Report the disqualification and stop.**

---

## Conditions common to both

- Shipping defaults. **Delivery OFF** for instrumentation visibility, **AND** a delivery **ON**
  confirmation, because delivery is where shape (c) is load-bearing.
- **Marker OFF** is not required — neither measurement is a per-frame pixel differencer (**G125**
  does not apply).
- **B1 pose gate: NOT APPLICABLE** unless a leg runs `CB_GateLevel` / `StaticMeshActor_49` /
  1280×720. **Declared, never silently skipped** (G117).
- Build identity after any code change is a **QUARTET** (**G121**): a code-only hot-swap moves the
  **exe** and leaves the **pak**. Record both halves.
- Every attempt banked, including discards (**A63**).

*(Written 2026-08-19, PART FOURTEEN, before any measurement.)*
