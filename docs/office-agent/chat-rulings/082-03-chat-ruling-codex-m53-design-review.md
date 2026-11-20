# 082-03 — Chat disposition of Codex's m53 design review (082-02, CHANGES REQUIRED): all 12 findings accepted; D1 (residency, fallback) and D7 revised

Date: 2026-09-27 (IST) · Author: chat Claude (orchestrator) · Review `_reviews/082-02-codex-m53-design-review.md` (owner's desktop run, against plan `946c1bf`) · Earlier ruling `082-02-chat-ruling-m53-plan-decisions.md`

## Summary
Codex found 5 P1, 6 P2 and 1 P3, all source-backed. **Chat accepts every finding.** Two of them overturn chat's own additions in 082-02:
- **D1's force-residency with a bounded wait** changes residency for textures shared by other objects, and the engine's wait is unbounded.
- **D7's default admission of cloned host MIDs** loses RVT overrides and freezes host parameter animation, and a disclosure key doesn't make that pure.

Chat also withdraws the **automatic route A fallback**. A known reconstruction error is not a recovery under the same label contract.

## Revised rulings
- **D1: route B stays provisional.** Changes from 082-02:
  - **Residency:** the source must be *already* resident at its required cooked top mip at Apply; otherwise refuse `not_fully_resident`. Wait **0 frames / 0 ms**. Never force residency or call `WaitForStreaming` from Apply.
    - Check that the resource is valid, the actual resident mip index and its dimensions, and that nothing is pending in streaming or init.
    - Record `snapshot_mip` relative to the cooked chain, with its dimensions.
    - A prefetch phase is a separate, later design decision, and only if yield demands it.
  - **No automatic route A fallback.** If only the normal corruptor fails, defer `normal_corruption` and qualify UV through B. If a shared path fails (normal decoding used by UV, colour/data/alpha), restrict to a qualified subset or hold. A takeover would be a separately approved change in product meaning.
- **D2: accepted, amended.** `uv_scramble` must be a **true deterministic permutation**: seeded, bijective, no cell collisions. Route A's `channel1` is removed along with route A.
- **D3: stands.** Two ids, with state and cleanup isolated per registration.
- **D4: stands.** Default OFF; all-refused data is never presented as support.
- **D5: stands provisionally.** `drift` is conditional on repeated-draw fidelity and cost (P2-6, P2-11).
- **D6: stands, amended.** Split the shared helper so prewarm-only inclusion doesn't widen the per-frame check. Shader completeness is not packaged PSO readiness (P2-11).
- **D7: reversed.** Cloning slots that hold host MIDs is **refused by default** until state-following support qualifies (all override types, including RVT, plus a defined policy for later host updates). `host_mid_cloned` stays a diagnostic key for the future.

## Required plan revision (082-03)
Resolve each finding as Codex specified, or bring a reasoned counter-proposal:
- **P1-1:** active parameter bindings from the material resource actually used (world, feature level, quality, static permutation). Keep the full `FMaterialParameterInfo` and use `…ByInfo`. Refuse what can't be proven within Core/CoreUObject/Engine. Add fixtures for the inactive-parameter / active-constant alias and for a material-layer parameter.
- **P1-2:** a pixel-format, sampling and mip contract in place of a compression denylist plus unconditional autogeneration.
  - Refuse float formats (HalfFloat/SingleFloat) and authored lower-mip or alpha-coverage chains that can't be preserved.
  - **No silent downsampling.** A source whose full chain exceeds the budget is refused `over_budget`. Keep both dimensions and the aspect ratio.
  - Qualify magnification, minification, grazing angles and mip transitions.
- **P1-3, P1-4, P1-5:** as the revised D1 and D7 above.
- **P2-6:** clear to (0,0,0,1) before **every** AlphaComposite draw, then regenerate mips. Add a repeated identity-redraw stability gate with opaque and fractional-alpha inputs.
- **P2-7:** hold the displaced original materials strongly. Capture the raw slot binding separately from the effective material, and inspect Nanite routing before any change. Restore only slots the event still owns. Extend G4.
- **P2-8:** atomic coherent-UV eligibility. Either the slot's required map set can all be transformed within the format, residency and memory limits, or the slot or event is refused. Rename the 64 px floor as a policy limit, and count exclusions deterministically.
- **P2-9:** one ordered decision tree with per-texture and per-slot diagnostic counts and exactly **one** final event reason. It must include asset/resource readiness, unsupported encoding, host-MID policy and residency. Check the active material resource before reading an empty `GetUsedTextures` as "no textures". Give a fixture for each reachable reason; mark the rest UNEXERCISED.
- **P2-10:** mark every row as a **qualification gate**, a **diagnostic reading** or an **owner decision**.
  - Give runnable matched controls per fixture; no widening of fixture-only cases.
  - **G-ID** runs through both corruptors and every supported encoding, and includes a deliberately wrong-copy control that must fail.
  - Functionality gates need counted successful applications, with a minimum count.
  - **G-LYRA** needs one successful counted event per id to claim support there.
  - **G-COST** gets a predeclared comparison and acceptance method, or stays visibly pending. m53 off/on is compared with m55 held constant.
- **P2-11:** separate the prewarm set from the per-frame check. Define copy readiness and transaction ordering before binding or labelling; a void draw call is not validation. Qualify the cold first fire for both corruptors and every target format, or measure it and keep it pending.
- **P3-12:** rename the key to `expected_strength_class`, documented as a mode prior, not a per-event measurement. m55 stays out of runtime eligibility, ranking, seeds, verdicts and `observable`.

Then re-state S1–S4, the cook and build count, and the expected yield impact of the stricter refusals (over-budget, not-resident, host-MID, atomic coherence).

## Review loop
After the revision, **Code runs a Codex delta check through the Relay** in the same brief. Request: `_reviews/082-03-codex-m53-delta-check-request.md`. Chat then reviews the revision and the delta check together before authorising S1.

## Also ruled (from 083-03)
- The existing 082-02 review is the owner's desktop run. No second review is needed.
- `run-meta.txt` is accepted as the record of Codex's model and effort.
