# Request to Codex (via the GDP Relay): design review of the m53 S2 plan (chat Claude, 2026-09-28)

**Role:** adversarial design reviewer.
- Read-only. Write the report to `review.md`; it will be copied to `_reviews/082-11-codex-m53-s2-design-review.md`.
- Findings are P1 / P2 / P3, each with a file:line and a concrete failure scenario.
- End with a `VERDICT:` line: APPROVE / APPROVE-WITH-CHANGES / CHANGES REQUIRED.

**Context**
- S1 (route B core, identity only) closed as a scoped stage after your 082-08 evidence review (APPROVE-WITH-FIXES, all findings dispositioned in `_reviews/082-08-chat-ruling-codex-evidence-review.md`).
- S2 adds:
  - the eight corruption modes on both ids;
  - the auto-pool mode draw;
  - the five S1 debts (T10 live, a Q `no_normal_map`, LEVEL-CHANGE, RECREATED, cancel-before-focus);
  - your P2-3 telemetry;
  - the S2 harness fixes.
- S1 stopped mid-campaign four times on premises that stub proofs couldn't see (fixture, streaming time axis, focus, runtime conditions). S2's plan therefore adds a per-row premise audit and a read-only premise window before the lock.

**Inputs** (read from git objects at `f6b47d4` on `feat/m53-uv-normal-corruption`)
- **The S2 plan:** `docs/predictions/2026-09-28-m53-s2.md`.
- **Chat's ruling on it:** `_reviews/082-10-chat-ruling-s2-plan.md`. It moves the MainWorld shadow trace up front as S2-0 and settles the plan's defaults.
- **The m53 plan:** `docs/predictions/2026-09-26-m53-uv-normal-corruption.md`, for the mode definitions, §R13 and §9.
- **The S1 predictions,** "S1 RESULT" section: `docs/predictions/2026-09-27-m53-s1-legs.md`.
- **Source at `f6b47d4`:**
  - `Source/AnomalyInjector/Private/Anomalies/Anomaly_TexCorrupt.cpp`;
  - `TexCorruptTree.cpp`;
  - the render-target module;
  - `AnomalyAutoInjectorSubsystem.cpp`;
  - `Anomaly_StuckLowMip.cpp`;
  - `AnomalyCaptureSubsystem.cpp`;
  - `AnomalyLabelWriter.cpp`;
  - the AnomalyBench module.
- **The S1 bank,** for the shadow-trace claim: `D:\IntrusiveAnomalies\_bench_sessions_bank\M53S1_FMW_*`, and the other MainWorld legs the plan's §3.2 cites.

**Questions**
1. **Modes.** For each of the eight modes, on both ids:
   - Is the implementation plan correct against the authored corruptor graphs? Is "0 plugin cooks" true?
   - Are the scramble CRC, the drift snapshot render target (and its unaccounted bytes) and the mode draw's byte-identity for non-m53 runs sound?
2. **Gates and can-fail.**
   - For each S2 Q row, can its falsifier actually fire?
   - Name the cheapest real defect in a mode that would still pass S2 as planned.
3. **The premise audit.** Pick the rows you think are riskiest and test their premises against source and banked real logs:
   - T10's `PauseTextureStreaming` producer;
   - G5's m52 interaction and the pin lever;
   - the level-change new-world reading;
   - the renamed-successor RECREATED lever;
   - TreatUnfocused;
   - the census-off seed test.

   Would any of them stop the gate window the way S1's four premises did?
4. **The MainWorld shadow trace (§3.2, plan line ~361).**
   - Is the evidence real in the bank? Installing vs non-installing legs, si 39 to 99, the ramp contact shadow.
   - Is the P-VSM discriminator well designed to separate a shadow-cache invalidation, a plugin-wide material-swap restore issue, and an m53-specific restore defect?
   - What would you add?
5. **Telemetry (your P2-3).** Do `slots_installed[]` and `textures[].slots[]` make a partial event localisable offline? Are they additive under schema 2?
6. **Scope.**
   - Is anything S2 must prove missing, or deferred without a sound reason?
   - Is the claim that m52's shared-texture check "cannot arise inside a capture run" true in source?
   - Is excluding `IAI.Bench.DestroyTarget` and every new lever from Shipping sufficient?
7. **Verdict.** Blockers before S2-1 source work, if any.

Depth where the risk is: questions 2, 3 and 4.
