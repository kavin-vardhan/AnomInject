# 082-04 — Chat ruling on the m53 revision (082-03, GREEN) and Codex's delta check (CHANGES REQUIRED): N1–N4 ruled; Δ1–Δ3 and six PARTIALs accepted

Date: 2026-09-27 (IST) · Author: chat Claude (orchestrator) · Plan `b118a66` · Delta check `_reviews/082-03-codex-m53-delta-check.md` (gpt-6-astra, max, 556 s)

## Standing product rule applied
The owner's rule is **label purity over yield** (m52, and re-applied in 082-02/03). The revision's predicted yield is low: StackOBot has only `SM_FloorBase` for UV; Lyra has the cubes for both families and the weapons for normal. That is accepted as the honest consequence. Yield is raised only by mechanisms that keep purity, in later units:
- a budget CVar (N3);
- a future non-capturing prefetch phase (082-03 P1-3, a separate design decision).

**Before any default-on decision, yield must also be read on the office hosts** as an eligibility census: counts only, run at the office, with the owner transcribing readings per the office-data rule.

## N1–N4
- **N1 = (a).** Add **private** dependencies on `RenderCore` and `RHI` to `AnomalyInjector`, and use a per-mip copy that reproduces the source's own mip chain (authored, sharpened or alpha-coverage) instead of regenerating mips.
  - This keeps the game-agnostic invariant: engine modules only, never host types. `AnomalyCapture` already depends privately on both (`AnomalyCapture.Build.cs:344–347`).
  - The architecture doc's wording becomes "engine modules only; no host-game types".
  - Where a per-mip copy is impossible (a mip not resident, or a format not admitted), refuse.
- **N2 = approved, additive only.** A bench-only synthetic test level, with materials for the alias / layer / reason / encoding fixtures, authored by a tool in the CaptureBench project.
  - New assets go in a new folder. `CB_GateLevel`, `MainWorld` and every existing asset must stay byte-unchanged; verify this after the cook.
  - Archive the current cooked container before the cook (G-archive pattern).
  - The level is never in a client package. Add a PRE-DELIVERY-CHECKLIST line.
- **N3: default budget 128 MiB**, set by a CVar.
  - Over budget is refused as `over_budget`, never downsampled.
  - G7 reports yield at 64, 128 and 256 MiB as a **diagnostic**, so the owner can later choose a different default with numbers in hand.
  - Add a **collateral-residency diagnostic**: during an event, the resident mips of non-target visible textures must not drop, compared with a NoApply control. If they do, the budget is too high for that host; that is a purity issue and it is reported.
- **N4: tolerance of 2 levels per 8-bit channel**, declared now as the G-ID qualification threshold.
  - The **wrong-copy controls must exceed 8× that (≥ 16 levels)**, which shows the test has power.
  - Exact-only is rejected: a BC5 or interpolated decode re-quantised to RGBA8 can't round-trip exactly, so an exact rule would fail correct copies.
  - Report the per-encoding error distribution beside the pass/fail.

## Codex's delta findings — all accepted
- **Δ1 (P1).** Refuse a runtime material wherever it sits in the **resolved** chain: component override → mesh asset slot → default. Check the effective material and its parents for `UMaterialInstanceDynamic` or a transient outer.
  - Fixture: a mesh-asset-slot MID, which must refuse.
- **Δ2 (P2).** Replace the sRGB wrong-copy lever with one that is **provably wrong** (e.g. a channel swap, a one-mip offset, or a fixed texel offset). Prove offline that it produces ≥ 16 levels of error on the fixture textures before relying on it.
- **Δ3 (P2).**
  - S1 gets one bench-only non-identity mode (a fixed `tile`) for G-BIND alone. It is not a product mode in S1.
  - G-RD's criterion applies to identity redraw stability only. Drift gets its own expected-change criterion in S2.
- **P1-2 (partial).** Resolved by N1(a).
- **P1-3 (partial).** Admit only `snapshot_mip == 0` relative to the cooked chain. Refuse when any runtime LOD bias applies to that texture (global, per-texture or cinematic), with a named reason.
- **P2-8 (partial).** If any spatial map in a selected slot is excluded for **any** reason (LOD group, VT, size policy, format), refuse the whole slot. The only exception is a map proven non-spatial, such as a 1×1 constant, and that case needs a fixture.
- **P2-9 (partial).** Order the decision tree so `virtual_texture` is decided before the `Standard2D` type check. Re-verify that each reason is reachable in the stated precedence.
- **P2-10 (partial).** Put **every admitted linear format** in the G-ID test matrix, or mark each one UNEXERCISED and refuse it until it is exercised.
- **S1 stops.** List every S1 qualification failure as a stop condition (Codex asked for this).

## Relay (owner, 2026-09-27)
Reviews use **Codex model Astra (`gpt-6-astra`) at effort `max`**. Pin both explicitly in the runner's defaults (`-m` and `-c`) instead of inheriting the global config, and record them in `run-meta.txt` as before.

## Next
- **082-04:** a plan amendment implementing the above, a Codex delta check #2 through the Relay, and the runner pin.
- If delta #2 has **no P1** and its P2s are all plan-text fixes, chat authorises S1, carrying the N2 cook.
