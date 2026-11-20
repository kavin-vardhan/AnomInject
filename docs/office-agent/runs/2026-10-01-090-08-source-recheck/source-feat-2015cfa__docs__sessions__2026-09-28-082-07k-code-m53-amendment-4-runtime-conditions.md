# 082-07k — m53 S1: the runtime-conditions review of the 35 unrun legs, the person-evidence read, AMENDMENT 4 (STOP-BEFORE-FIRE, the F-MW pose scope, the GC detector, the TWO refusal), supersede entry 5, proofs, and the boundary re-issue

Session 082-07k, 2026-09-28 (IST), Claude Code (Opus 5.5), headless from the GDP mailbox, continuing the 082-07j session.

- **Ruling:** `_reviews/082-07j-chat-ruling-can-premise.md` (rulings 1–3).
- **Branch:** `feat/m53-uv-normal-corruption`, parent `848651e`.
- **Evidence:** `D:\IntrusiveAnomalies\_reviews\082-07k-evidence\` (scripts in `prep\`).
- **Bench-free:** no leg, build, cook, plugin source change or CaptureBench change, and no tag.

## 1. Outcome

- **AMENDMENT 4** is appended to `docs/predictions/2026-09-27-m53-s1-legs.md` (§A4.1–A4.8) and is the spec for 082-07l.
- **G4 CAN-UV / CAN-NM are STOP-BEFORE-FIRE (class Q).** The three 082-07j attempts and their NOT-RUN record are superseded
  (entry 5), and both legs run fresh. The cancel-before-focus branch goes to S2, with a bench lever.
- **The review found three more premises that would have gone wrong on the real bench:**
  - **(a) F-MW would have stopped outright.**
    - `validity()` applied the settled-camera clauses to every capture leg.
    - The MainWorld camera origin eases for ~85+ captured frames (modal coverage ~0.19 on 329 banked sessions).
    - So all seven F-MW legs would have been INVALID-POSE × 6 → NOT-RUN at `FMW_CTL_A`.
    - The pixel legs now match a pinned trajectory; non-pixel legs carry no pose clause.
  - **(b) The G4 GC detector was blind.**
    - `LogGarbage` is declared `Warning` and "Collecting garbage" is logged at `Log`, so it never reaches the log.
    - The GC legs now raise it, and the evaluator separates "blind" from "no pass inside".
  - **(c) G4 TWO's frame-1 second fire.**
    - The frame-1 transaction has never run on the bench, and a refusal would have halted as FAIL.
    - A named refusal now reads FIXTURE-CANNOT-EXERCISE.
- **Also fixed on the way:**
  - **TRIP** raised on a 0-frame session: `Sess` needs `labels.jsonl`, so TRIP would have hit EVALUATOR-ERROR, stop 7. It now
    reads `run_summary.json`.
  - **`trajectory_errors`** crashed on a row without `frame_index`; `proof-082-07d` exposed it.
- **Person evidence:** the runner's own final startup Alt is a real source, but it cannot void a leg, so the detector is
  unchanged. The CAN attempts' +51.7 s input is not the runner's; its cause is not established.

## 2. The runtime-conditions review

- **The table** is AMENDMENT 4 §A4.3: one row per unrun leg family, with the runtime conditions, the evidence (the real runner, banked real logs or
  source) and the disposition.
- **Raw data:** `review-static-082-07k.json`, `review-logs-082-07k.json`, `review-mw-pose-082-07k.json`,
  `review-mw-traj-082-07k.json`, `review-mw-majority-082-07k.json`, `review-apply-frame-082-07k.json` and
  `pp-timing-082-07k.json`, all computed read-only.

| disposition | legs / rows |
|---|---|
| **affected → encoding fix (proven)** | CAN-UV/NM (A4.1, A4.2); FMW_CTL_A/B, FMW_ID (A4.3.1: trajectory); REASON_S3O, REASON_A2, COLL_A/N_FMW, REASON_S8 (A4.3.1: no pose clause); GC-UV/NM (A4.3.2); TWO (A4.3.3) |
| **unaffected (with evidence)** | FIN-NM; DES ×2 and DES2; END ×2; FR ×2; FS ×12; COLL_A/N_FSYN; TRIP / G11 / P2-1 / P2-3 / G3-COUNT; the person gate |
| **needs a build or fixture change** | none among the 35 (the cancel-before-focus branch → S2) |

**Evidence carrying the "unaffected" rows:**

- DECIDE → APPLIED is 0 frames on 1,069 of 1,069 real fires.
- All 1,069 real ledger lines read `off=2 on_frame balanced live=0 pending=0`.
- 156 of 156 F-SYN sessions sit at one pose.
- The graceful close is a real, banked result (`M40_TEARDOWN_GRACEFUL`, exit 0).
- The `EndPlay(Quit)` → `OnTargetLost` → `Revert` chain is read in source.
- The frame-1 census reads `TC_NN1` normal APPLY.
- Every lever and console token is in the staged exe (both encodings, with 0-hit controls), and the three maps are in the staged utoc.

## 3. Person evidence (ruling 3)

- **13 of 174 attempts carry person evidence.**
- **10 of them** place the last input at `sampling_started` −0.010 … +0.033 s: the runner's last startup
  `Hold-Focus` → `Force` synthetic Alt, which is self-generated.
- **The CAN attempts' last input** is at +51.7 s after the runner started, repeating to 30 ms across A1 and A2. The runner acts
  nothing then, and the game log is silent. It is **not** self-generated; its cause is not established.
- **It cannot void a leg:**
  - person evidence adds no validity error;
  - its only effects are ENV-INTERRUPTED instead of INVALID for foreground-only failures, and a ≤ 60 s person-gate wait;
  - the longest real attempt is 11.1 s.
- **So no detector change.** For S2: persist the input timestamps, and excuse the final startup Force.

## 4. The harness changes

- **`082-07-legs.py`** `84a423a6` → `c552c587`:
  - `G4_CAN_*` kind `stop_before_fire`;
  - `G4_GC_*` gain `Log LogGarbage Log`.
- **`082-07-lib.py`** `b1e0cdcd` → `96831d8f` (A4.6 lists every function).
- **New locked input:** `_reviews/082-07-mw-trajectory.json` `1ce9d988`.
  - Builder: `prep/build-mw-trajectory-082-07k.py`.
  - 161 banked frame-1 MainWorld sessions, 146 majority-consistent, 0 disagreements, fi 1–798.
- **Unchanged:** the runner, `082-07-common.py`, the preflight, the postflight and the window.
- **Pre-edit copies:** `prep/pre-edit/`.

## 5. The supersede act (entry 5)

- **Script:** `prep/supersede-082-07k.py`; results in `supersede-act.json`; the pre-act state in `run-before-supersede/`.
- **Preconditions asserted:**
  - the record matched the 082-07i lock (`ff7f884f`);
  - there was no recorded halt;
  - the three attempts are INVALID;
  - exactly one label-less NOT-RUN record exists;
  - no gate row was fed by the attempts.
- **Superseded:**
  - the attempts `M53S1_G4_CAN_UV_A1/A2/A3`;
  - the NOT-RUN record (the new record-level `records` key).
- **Kept live:** the ADMIT row.
- **Bank:** the three attempt dirs are renamed `…__SUPERSEDED_082-07k`, 12 files each, manifest-verified.
- **After:**
  - record `e67f0ec9`;
  - no halt, no live NOT-RUN;
  - 158 accepted legs, 35 unaccepted, the first being `G4_CAN_UV` (next attempt `A4`).

## 6. Proofs, no-flip, dry run (pre-lock, final lib `96831d8f` / legs `c552c587`)

| suite | result | note |
|---|---|---|
| `proof-082-07k` (AMENDMENT 4) | **46/46**, before and after supersede | both ways against the pre-edit lib and real banked data (A4.7) |
| `proof-evaluators` | **207/210** | the 3 BAD are the old-premise checks AMENDMENT 4 supersedes by design: "G4 CAN PASSES" (a synthetic cancel-before-focus run, which now FAILS STOP-BEFORE-FIRE), and two F-MW validity checks built on fabricated rows at `frame_index` 5000+ under the modal rule, now INVALID-POSE under A4.3.1. Their A4 counterparts are proven in `proof-082-07k` A13, C4 and C5 |
| `proof-082-07d` | **37/37** | first run crashed (a real lib defect, fixed); its one synthetic F-MW sequencer leg now carries a valid A4.3.1 camera (fi 7–18 on the pinned trajectory) |
| `proof-sequencer` | **20/20** | |
| `proof-disk` | **15/15** | |
| `proof-082-07g` | **32/33** | the BAD is P5a, the known A3.7 finding: the same 10 of 991 |
| `proof-082-07i` | **35/35** | |
| no-flip, pre-supersede | **511/511, 0 flips** | live files unchanged; the 082-07i script was taught the recorded `DECLARED-NOT-RUN` T10 row |
| no-flip, post-supersede | **511/511, 0 flips** | no halt |
| dry run, pre-lock | **193 legs, 548 rows, 0 problems** | the 082-07l plan is 35 legs from `G4_CAN_UV`, none re-launched; estimator 24 min |

The suites ran from copies in `082-07k-evidence/prep/` with only their output paths changed.

## 7. Boundary and the 082-07l invocation

- **The boundary is re-issued after this commit is pushed.** It locks:
  - the head;
  - the predictions with AMENDMENTS 1–4;
  - the harness (legs, lib, supersede and the new trajectory file);
  - the staged build.
- **Then:** the preflight replay, the post-lock dry run, the stubbed end-to-end window and the preflight negatives. Their
  results are in the 082-07k report.
- **Invocation:** `C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\082-07-window.py 082-07l 540`.

## 8. Gotchas

- **G333:** a validity clause calibrated on one fixture was applied to every fixture. MainWorld's camera eases for ~85 frames,
  so the settled-camera rule would have voided every F-MW leg.
- **G334:** a detector keyed on a log line whose category's default verbosity suppresses it is blind, and reads as a clean
  "unexercised".
- **G335:** a harness's own synthetic input at the edge of its measurement span registers as a person.

## 9. Hand-off

- **Next:** 082-07l, the 35 legs, with P2-2, P2-5, P2-6 and `MW-FLOOR`.
- **S2 carries:**
  - the cancel-before-focus row with a bench "treat as unfocused" lever;
  - the person-evidence timestamps and the startup-Force excuse;
  - the T10 row;
  - the cause-read list (P5a, the halo, the start-up element, TC_Layer's gradient, `T_TC_Stream2k`'s full chain);
  - the +51.7 s input in the CAN attempts (cause not established).
