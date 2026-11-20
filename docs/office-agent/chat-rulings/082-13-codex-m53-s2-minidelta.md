<!-- relay: run folder runs\2026-09-28-082-13-m53-s2-minidelta | model gpt-6-astra | effort max | duration 545 s (10:32:17-10:41:22) | collected by Claude Code 082-13, content unedited -->
**Mini-delta: CHANGES REQUIRED. Seven items are RESOLVED; three remain PARTIAL. No P1.**

Reviewed `82c0ebd..9d622e0250bae2f7b5ff41c74d8c4fab5c33b2f8` through git objects, the 082-12 delta/ruling, and Code's journal. Source is unchanged. Below, **S2** means `docs/predictions/2026-09-28-m53-s2.md` at that head; source basenames refer to the same tree. Findings and fixes are proposed for Chat's ruling.

| Item | Status | Evidence |
|---|---|---|
| D1 | **RESOLVED** | Common warm policy, corrected B schedule, per-arm timing validation: `S2:87`, `S2:98`, `S2:148`. Proof-input problem separately below. |
| D2 | **RESOLVED** | One successful fire; separate N1/N2/R markers; N2/R require zero APPLIED: `S2:193`, `S2:1834`. |
| D3 | **RESOLVED** | Four complete 100-frame events and a matching null: `S2:213`, `S2:220`, `S2:2121`. The event floor is achievable; offset prediction needs correction below. |
| D4 | **RESOLVED** | Post-GC census detects retention; separate accounting fault defeats `Close` and leaves live bytes: `S2:250`, `S2:258`, `S2:266`. |
| D5 | **RESOLVED** | Association/layer keyed bindings, texture-path equality, association-swap negative: `S2:295`, `S2:304`, `S2:890`. |
| D6 | **PARTIAL** | Direct trace comparison and unresolved-cause fallback added, but disposition counts still substitute for defective final state: `S2:319`, `S2:328`, `S2:1880`; M1 below. |
| P2-1 | **PARTIAL** | Coverage fixed (`S2:2121`); conflicting drift-clock expectations remain (`S2:218`, `S2:639`, `S2:2118`), M3. |
| P2-2 | **RESOLVED** | Independent snapshot qualification plus separate object/accounting negatives: `S2:652`, `S2:655`, `S2:262`. |
| P2-3 | **PARTIAL** | Matrix timing/protocol corrected (`S2:1743`, `S2:1834`); classification and pre-launch proof still need M1/M2. |
| P2-7 | **RESOLVED** | Complete binding identity and equality-by-key now specified: `S2:865`, `S2:890`, `S2:905`. |

**Remaining/new findings:**

1. **P2-M1 — Remaining D6: `left-to-game` / `unresolved` does not establish defective final state.** `S2:328`–`330` and `S2:1880`–`1883` accept either count instead of an Effective-material mismatch. Suppose the game restores the original material before revert in both arms: m53 reports `left-to-game` (`Anomaly_TexCorrupt.cpp:1118`–`1125`), as does C (`Anomaly_CorruptedTexture.cpp:193`–`199`), although both final materials equal their originals. With the specified pixel flags, this still classifies PLUGIN-WIDE-RESTORE-DEFECT and contradicts the equal-state negative at `S2:336`. **Proposed fix:** require demonstrated rendering-relevant defective final state on both arms; disposition counts alone leave cause unresolved. Add an equal-Effective/nonzero-disposition negative.

2. **P2-M2 — New: the required bank proof cannot pass all T-checks.** `S2:168`–`170` requires the T-checks to pass the named bank inputs before launch. Neither log contains T6's census commands or T8's cache query (`S2:158`, `S2:160`). Moreover, the warm-free bank's first fire is explicitly **not applied** (`_bench_sessions_bank/M55B3R23_NOHOLD_S_A1_try1/_leg_game.log:1895`), failing T3 even with a first-event slice. **Failure:** a literal proof suite stops before S2-0, or silently weakens the checks to proceed. **Proposed fix:** name the applicable bank-backed subchecks explicitly; use declared synthetic positives/negatives for the full new protocol, retaining first-live-leg validation for the unexercised mechanisms.

3. **P2-M3 — New offset correction still uses saved-frame counting.** `S2:217`–`218` predicts k=1…100 / 0.008–0.833, but `S2:639`–`642` requires engine-frame counting, including unsaved settle frames. Replaying the phase rules (`AnomalyCaptureSubsystem.cpp:737`–`779`) gives each event k=**1,4,5,…,102**, ending at **0.85 UV**. The four event ranges themselves are correct. **Failure:** a correct engine-clock implementation fails an oracle using the new sequence; G-RD-DRIFT's saved-frame formula (`S2:2118`) also rejects it. **Proposed fix:** derive every offset expectation from `frame_index - ApplyFrame + 1`, and correct both ranges/formulas before implementation.

4. **P3-M4 — Residual noise ceiling.** The ruling's measure/lock/FAIL sequence is applied at `S2:346`–`368` and `S2:2187`, but `S2:2122` still retains an unstruck `τ … ≤ 8` in its rule column. A sound locked τ=9 would receive contradictory treatment. **Proposed fix:** remove that residual ceiling and reference the locked τ consistently.

**S2-0 runnable as specified on the current staged S1 build: NO.** Exact blockers are **M2's unsatisfiable pre-launch proof requirement** and **M1's remaining unsupported causal-classification branch**. These require specification/evaluator corrections, not a new S1 binary. Independently rehashed the staged exe/utoc/ucas: `2FCDF059` / `20DA6F98` / `534C5863`; reproduced the saved token counts and all 24 batch descriptions (maximum command line 3,106 characters). Nested DEFER and warm-off m53 timing remain appropriately subject to first-leg validation. M3/M4 concern later S2 gates.

Read-only review; no builds, launches, captures or repository/bench changes. **Chat review REQUIRED** for disposition of these proposed corrections before S2-0; the owner's PC-OK condition remains separate.

VERDICT: CHANGES REQUIRED
