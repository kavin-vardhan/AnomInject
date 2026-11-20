<!-- relay: run folder runs\2026-09-28-082-12-m53-s2-delta | model gpt-6-astra | effort max | duration 652 s (09:48:16-09:59:08) | collected by Claude Code 082-12, content unedited -->
**Delta disposition: CHANGES REQUIRED. S2-0 is not sound enough to run as specified.** Seven findings are resolved at the plan level; four are partial. No P1 found.

Reviewed git objects `f6b47d4..82c0ebd4d24970c5d30f0c58860c4064e6ebcad4`, both 082-11 reports, and the 082-12 journal. Source is unchanged. No repository or bench changes; no builds, launches or captures. Below, **S2** means `docs/predictions/2026-09-28-m53-s2.md` at that head; source basenames refer to the same tree. These are proposed findings for Chat's ruling.

| Original finding | Status | Evidence / remaining issue |
|---|---|---|
| P2-1 | **PARTIAL** | Mode-specific oracles and wrong-mode negatives added (`S2:115`, `S2:158`); drift's event minimum is impossible, D3 below. |
| P2-2 | **PARTIAL** | Independent snapshot identity, binding, residency, rollback and budget rows added (`S2:242`–`246`); the ownership negative has the wrong expected result, D4. |
| P2-3 | **PARTIAL** | Matrix, paired nulls, footprint limitations and state readings added (`S2:1324`, `S2:1340`, `S2:1361`); D1, D2 and D6 remain. |
| P2-4 | **RESOLVED** | Renamed cleanup/default fallback, same-name exact control and retained-MID negative (`S2:298`–`317`). |
| P2-5 | **RESOLVED** | Measured drop, actual insertion, decision-time T7, conditional plumbing fallback and pin cleanup (`S2:338`–`365`). |
| P2-6 | **RESOLVED** | Surviving ticket, ordered old/new-world readings, post-GC completion and retained-object negative (`S2:384`–`420`). |
| P2-7 | **PARTIAL** | Serialization, empty arrays, commit/NoApply separation and compatibility proofs are scoped (`S2:439`–`497`); flattened binding identity is incomplete, D5. |
| P2-8 | **RESOLVED** | Explicit unconditional m53 mode fraction, real routine/KAT, precision/state logs and integration coverage (`S2:514`–`583`). |
| P2-9 | **RESOLVED** | Concurrency determination, original-binding enumeration fix and can-fail proof (`S2:613`–`685`). Correctly distinguishes default concurrency from the default enabled set. |
| P2-10 | **RESOLVED** | E6 through Apply; cancellation predicates/TRIP exception; per-slot restore dispositions (`S2:696`–`736`). |
| P3-1 | **RESOLVED** | Explicit bench enablement, Shipping exclusion, gated DestroyTarget and separate M3 artifact proof (`S2:750`–`804`). |

**New and remaining findings:**

1. **P2-D1 — New: S2-0's swap arm has a different clock.** `S2:1307`, `S2:1326` and `S2:1357` assume family B shares F=62 and same-index comparisons. But `corrupted_texture` skips m53's two uncaptured warm frames (`AnomalyCaptureSubsystem.cpp:2442`, `:3633`); its m53 nulls take them. With the declared recipe, C applies at 60, and equal saved indices represent different engine frames. This can invalidate pose/dither comparisons or manufacture a trace. **Proposed fix:** give all B legs a common warm policy and redeclare their frame/index schedule—for example, warm OFF throughout B with an adjusted lead-in. Validate timing for every arm, not only A's N1.

2. **P2-D2 — New: valid N2/R legs fail the single-event predicate.** `S2:1378` requires one `APPLIED` or a corrupted-texture apply. NoApply-2 returns before the m53 `APPLIED` line (`Anomaly_TexCorrupt.cpp:769`–`782`, `:989`). Bank confirmation: `M53S1_COLL_N_FMW_A1/_leg/game.log:2543` and `:2545` show its NoApply-2 marker and successful `Auto.FireSpecific`, without `APPLIED`. **Proposed fix:** count the existing success marker for N2/R, require exactly one attempt, and check the NoApply-2 marker separately.

3. **P2-D3 — New: G-MODE-UV cannot meet its drift event floor.** `S2:179` / `S2:1633` require at least four counted events, but prescribe `2 16 100 4 0`, 120 frames. This is the same explicitly **one-event** recipe at `S2:1630`. Correct drift therefore fails on coverage. **Proposed fix:** declare a one-event drift exception with its required frame coverage, or lengthen/add legs to provide four complete events.

4. **P2-D4 — New: object retention cannot falsify the ledger as specified.** `S2:245` / `S2:1638` expect `TexCorruptRetainRt` to read unbalanced. Its definition retains an already released UObject (`S2:416`, `S2:1065`); accounting still moves bytes to pending and expires them (`Anomaly_TexCorrupt.cpp:163`; `TexCorruptPure.h:396`). The negative can retain an object while reading balanced. **Proposed fix:** judge retained snapshots with a post-GC object census; use a separate omitted-accounting-release fault for an unbalanced-ledger negative.

5. **P2-D5 — New: flat bindings discard part of their identity.** `{slot_key,param,texture}` (`S2:453`) cannot support the exact inventory check at `S2:475`. The existing fixture has both global `Tex` and layer-0 `Tex` (`M53S1_BIND_ID_A1/_leg/game.log:1997`–`1998`). Swapping their associations leaves the proposed rows unchanged. **Proposed fix:** include `association` and `layer_index` in each binding key, preserve the texture path, and add an association-swap negative.

6. **P2-D6 — Remaining P2-3: PLUGIN-WIDE still overclassifies.** `S2:1402` calls any persistent C-versus-null difference in K reproduction; `S2:1412`–`1413` then classifies PLUGIN-WIDE when refresh does not reproduce, requiring state evidence merely to be attached. A different swap trace with perfectly restored materials satisfies this, without establishing a shared restore defect. **Proposed fix:** compare C directly with ID-F to establish the same trace; retain the descriptive swap flag and an unresolved cause unless rendering-relevant defective state is established.

**S2-0 answer: NO.** Exact blockers are D1's unmatched warm phase, D2's impossible N2/R success predicate, and D6's unsupported causal classification. These are specification/harness corrections; they do not establish a need for a new S1 binary. Chat review REQUIRED for this delta's disposition before S2-0; the owner's PC-OK condition remains separate.

VERDICT: CHANGES REQUIRED
