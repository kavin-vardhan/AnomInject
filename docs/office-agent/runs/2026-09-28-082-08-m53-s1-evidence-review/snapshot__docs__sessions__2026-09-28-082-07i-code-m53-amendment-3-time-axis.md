# 082-07i — m53 S1: the time-axis review of the 39 unrun legs, AMENDMENT 3 (T10, the V1 predicate, the F-MW partial rule, the MainWorld reading), supersede entry 4, proofs, and the boundary re-issue

Session 082-07i, 2026-09-28 (IST), Claude Code (Opus 5.5), headless from the GDP mailbox, continuing the 082-07h session.

- **Ruling:** `_reviews/082-07h-chat-ruling-t10-premise.md` (rulings 1–4).
- **Branch:** `feat/m53-uv-normal-corruption`, parent `f9d6710`.
- **Evidence:** `D:\IntrusiveAnomalies\_reviews\082-07i-evidence\` (scripts in `prep\`).
- **Bench-free:** no leg, build, cook, plugin source change or CaptureBench change, and no tag.

## 1. Outcome

- **AMENDMENT 3** is appended to `docs/predictions/2026-09-27-m53-s1-legs.md` (§A3.1–A3.10) and is the spec for this session.
- **T10** is superseded (entry 4) and re-declared class D FIXTURE-CANNOT-EXERCISE (premise). It is not a leg in S1, and a
  full row is owed in S2.
- **V1:** the census and the apply path call **the same** `EvaluateTree` / `EvaluateBinding`, so S1's `not_fully_resident`
  evidence is declared as the predicate proven in-engine plus the refusal plumbing (A3.2).
- **The time-axis review found one row family affected in a way that could falsely stop 082-07j:** the F-MW partial yield.
  - Its encoding fix is proven both ways (A3.4).
  - Two further items are affected only in their text: the G-COLL F-SYN prediction premise, and A2's declaration.
  - Everything else is unaffected, with reasons (A3.3).
- **The MainWorld "zero yield" finding is UNVERIFIED.** All six floor legs now feed a read-only `MW-FLOOR` reading (A3.5).
- **One new finding from re-running the proofs:** P5a sees 10 phase-matched differences, max |d| 1, on the ChainD
  grazing/transition tiles (A3.7). It is not caused by this amendment and blocks nothing in 082-07j.

## 2. The time-axis review

The full table is AMENDMENT 3 §A3.3. The raw data, per leg (rows fed, admission needs and the census finals they read), is
`082-07i-evidence/review-raw-082-07i.json`, computed read-only from the harness.

| disposition | rows |
|---|---|
| **affected → encoding fix (A3.4)** | G-ID-FMW, G3 FMW, BAND F-MW (a partial yield across separately launched legs) |
| **affected → text only** | G-COLL F-SYN "no deficit" prediction (two fixture textures stream); G-REASON A2's DECLARED-CANNOT argument (derived from the frame-1 state) |
| **unaffected** | A5, A6, V2 (non-streaming, single slot); all 26 G4 rows and their ADMIT pre-checks (TC_UC1 / TC_NN1, `streams=0`, 8/8, applied at decision frames); P2-2; S3O (slot-level host_mid, rank 3); S8 (no bindings); G-COLL F-MW (D); TRIP / G11 / P2-1 / P2-3 / G3-COUNT |
| **needs a fixture or build change** | none among the unrun (T10 → S2) |

**The source facts the review rests on:**

- one `EvaluateTree` (`TexCorruptTree.cpp:736`);
- `not_fully_resident` at `:460-470`;
- the final across slots at `:866-903` (`APPLY` if any slot qualifies, else the lowest rank wins);
- the ranks at `TexCorruptPure.h:5-24` (binding step k = 10+k; A2 31, A3 32, A5 35, A6 36);
- host_mid at `:497-501`, before any binding;
- the collateral set taken at apply from on-screen primitives (`Anomaly_TexCorrupt.cpp:397-446`);
- the DestroyTarget latch counting the leg's own anomalous ticks (`AnomalyInjectorSubsystem.cpp:337-354`).

## 3. The harness changes

- **`082-07-legs.py`:** `NOT_RUN` (T10), and A2's declaration corrected. 193 legs.
- **`082-07-lib.py`:**
  - `EvView`, `fmw_partial` and the F-MW branch of `ev_band` / `ev_gid_mw` / `ev_g3`;
  - `floor_decisions` and `ev_mw_floor` (MW-FLOOR, D, never raises);
  - G-REASON T10 as `DECLARED-NOT-RUN` (D, no legs).
  - 548 gate rows.
- **Pre-edit copies** are kept in `082-07i-evidence/prep/pre-edit/`: lib `3fd05513`, legs `ac9f11f2`, common `91af53d8`.
- **Final hashes:** lib `b1e0cdcd`, legs `84a423a6`. `082-07-common.py`, the runner, the preflight, the postflight and the window are
  unchanged.

## 4. The supersede act (entry 4)

- **Script:** `prep/supersede-082-07i.py`; results in `supersede-act.json`; the pre-act state in `run-before-supersede/`.
- **Preconditions it asserted:** the record matched the 082-07g lock (`85fb48dd`), and the recorded halt was `G-REASON T10 FAIL`.
- **Superseded:**
  - the attempt `M53S1_REASON_T10_A1`;
  - its one recorded row (the halt).
- **Recorded as never evaluated:** TRIP and G11 `REASON_T10` (G319).
- **Alias** renamed to `M53S1_REASON_T10__SUPERSEDED_082-07i`, 170 hardlinks verified before and after; nothing was deleted.
- **After:**
  - record `ff7f884f`;
  - no halt;
  - 154 live accepted legs, 39 unaccepted (first `REASON_A5`);
  - the only ready row is the new T10 row.

## 5. Proofs, no-flip, dry run (pre-lock, on the final lib `b1e0cdcd` / legs `84a423a6`)

| suite | result | note |
|---|---|---|
| `proof-082-07i` (AMENDMENT 3) | **35/35** | F-MW partial rule both ways against the pre-edit lib; MW-FLOOR incl. a real known answer; T10 row plumbing |
| `proof-evaluators` | **210/210** | |
| `proof-082-07d` (admission and the declared-cannot row) | **37/37** | first run 36/37: A2's corrected text had dropped the `:566-574` citation the check requires; the citation was added (it is where `no_normal_map` is decided) |
| `proof-sequencer` | **20/20** | |
| `proof-disk` | **15/15** | |
| `proof-082-07g` (AMENDMENT 2) | **32/33** | the one BAD is P5a, the real-data drift scan over the grown bank (A3.7): 10 of 991 phase-matched comparisons differ, max |d| 1; identical on the pre-edit lib |
| no-flip, pre-supersede | **497/497, 0 flips** | the T10 row re-read through its recorded leg (still FAIL); live files unchanged |
| no-flip, post-supersede | **496/496, 0 flips** | live files unchanged, no halt |
| dry run, pre-lock | **193 legs, 548 rows, 0 problems** | no console token missing; the 082-07j plan is 39 legs from `REASON_A5`, none re-launched; estimator 27 min |

The suites were re-run from copies in `082-07i-evidence/prep/` with only their paths changed, so 082-07g's evidence files are
not overwritten.

## 6. Boundary and the 082-07j invocation

- **The boundary is re-issued after this commit is pushed** (`prep/boundary-issue-082-07i.py`). It:
  - moves the 082-07g lock aside as `boundary-082-07g.json`;
  - locks the head of this commit, the predictions with AMENDMENTS 1–3, the harness (legs, lib and supersede changed) and the
    staged build.
- **Then:** the preflight replay, the post-lock dry run, the stubbed end-to-end window and the preflight negatives run.
- **The results of those are in the 082-07i report and in `082-07i-evidence/`, not in this journal.** A docs commit after the
  lock would move the head and stale it.
- **Invocation:** `C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\082-07-window.py 082-07j 540`, with no `IAI_R53_ROOT` or
  `IAI_R53_STUB_LAUNCH`. If the 45-minute wait budget is spent, re-run with `082-07j-2`.

## 7. Gotchas

- **G329:** a two-branch rule for a race (all refused / all applied) omitted the third branch the time axis creates.
- **G330:** a real-data proof is a claim about the bank it ran on; when the bank grows, a new failure is new evidence.
- **G331:** a PowerShell double-quoted here-string eats markdown backticks.
  - 082-07h's G328 and its journal hand-off were committed with a newline, two CRs and a NUL where code spans should be.
  - Both are repaired in place, marked 🔻.
  - Found by counting CR characters in files that should have none.

## 8. Hand-off

- **Next:** 082-07j, the 39 legs, with P2-2 / P2-5 / P2-6 and the MW-FLOOR reading.
- **For chat's evidence review:**
  - A3.7 (the dither exception);
  - the MW-FLOOR reading once it exists (the owner-facing yield statement);
  - any `no_normal_map` on the floor from `REASON_A2`.
- **S2 carries:**
  - the full T10 row (with its cause read);
  - A3.7's cause read, alongside the halo and the 8-level gradient;
  - an A2 Q-producer, if MW-FLOOR shows one.
