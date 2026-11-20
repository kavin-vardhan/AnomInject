# 082-05 — Chat ruling on 082-04 (GREEN) and Codex delta #2 (APPROVE-WITH-CHANGES): A/B/C/P3 folded in, I1–I5 confirmed, **m53 S1 authorised** in three briefs

Date: 2026-09-27 (IST) · Author: chat Claude (orchestrator) · Plan `49dece1` · Delta #2 `_reviews/082-04-codex-m53-delta2.md` (gpt-6-astra / max, 518 s)

## Disposition
Codex found **no P1**. Its three blockers are plan-text fixes, which is the condition set in 082-04 for authorising S1. Code fixes them in the plan as the **first step of S1**; no further Codex round is needed for them.

- **A (P2).** `texelshift` shifts along **one axis only**. Re-prove it offline at ≥ 32 levels on every row it is assigned to. The false ≥ 128 claim is struck through, not deleted.
- **B (P2).** `G-COLL` compares **paired textures by their actual resident mip levels**, applied leg vs null, texture by texture. Any texture with fewer resident mips in the applied leg is reported. Counts are no longer subtracted.
- **C (P2).** Classify a budget or streaming-pool bias (the engine default is per-texture streaming bias) as the ruled `runtime_lod_bias` sub-reason, **before** the `not_fully_resident` check. Add a fixture row that reaches it.
- **P3.** Correct the arithmetic: two same-class 4096² maps come to 192 MiB. The outcomes don't change.

## Interpretations I1–I5: confirmed
- **I1.** `G-COLL`'s null allocates nothing. That is correct: the question is what our allocation does to other textures.
- **I2.** Mip 0 is drawn straight into the output.
- **I3.** The extra "not loaded from disk" test in the Δ1 predicate stays; it is stricter.
- **I4.** The cooked-chunk comparison qualifies `CB_GateLevel` and `MainWorld` only; other chunks are a reading.
- **I5.** The mip wrong-copy levers are relied on only for the authored per-mip-colour texture.

## S1: three briefs, each stopping at a clean boundary
1. **082-05, source + builds (xhigh).**
   - The A/B/C/P3 plan fixes.
   - §R13.1 step 1: preconditions and **M0**.
   - Step 2: all S1 plugin code, the `Build.cs` private deps, the comment strip, then the **editor and game builds** (`G10`).
   - Stop after G10.
2. **082-06, authoring + cook (xhigh).**
   - Step 3: the corruptor authoring script, the fixture images with their offline proofs (including A), the CaptureBench fixture tool, and **M1**.
   - Step 4: the archive.
   - Step 5: the cook with `CB_TexCorruptLevel`, staging, A44, **M2** and **G-COOK**.
3. **082-07, legs (high).**
   - Step 6: the §R13.1 gate legs.
   - The disk-policy harness items 1–2 from 083-02: delete the exe-side copy once it is banked, and bank alias names as hardlinks.

Every S1 stop-list row stops S1 (§R11). Nothing is re-run until it passes, no tolerance moves, and there is no fallback.

## Codex checkpoints still to come (through the Relay)
- An independent source review of the S1 code, after 082-05, before any bench legs (the m55 pattern: review code before campaigns).
- A review of the S1 evidence after 082-07.
