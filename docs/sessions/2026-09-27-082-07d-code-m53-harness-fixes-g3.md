# 082-07d — m53 S1: window fix, Q-row admission pre-check, G0 normal coverage, G3 amendment, resume record, boundary re-issue

**Session 082-07d, 2026-09-27. Claude Code, headless. BENCH-FREE: no build, no cook, no leg.** Branch
`feat/m53-uv-normal-corruption`, parent `9bdc6f6`. Rulings: `_reviews/082-07c-chat-ruling-s1-legs-red.md` items 2–5, and chat's
ruling on the two 082-07c notes (U-N1 re-runs on the new build; 082-07b frames of the four normal tiles are not compared across
builds). Evidence: `_reviews/082-07d-evidence/`. The contract for 082-07e is **AMENDMENT 1** of
`docs/predictions/2026-09-27-m53-s1-legs.md`.

## 1. What was done

| item | where | proof |
|---|---|---|
| Window fix | `082-07-window.py` now calls `L.budget_state()['left_s']` | the file is byte-identical to the ratified 082-07b driver (`0C18E9A3…`); the driver is retired as `082-07b-window-driver.py.RETIRED-082-07d`; the real window is run end to end with the launch stubbed after the lock (the final report carries the result) |
| Q-row admission pre-check | lib `admission_needs` / `admission_check` / `precheck_before`, called by `run_sequence` before the first leg of every Q row | `prep/proof-082-07d.py`: both ways, on the real 082-07b census and on synthetic census output |
| G0 normal family on F-SYN | lib `g0_fsyn_expected`, legs `NORMAL_APPLY` / `NORMAL_SHARED` / `normal_expected` | same proof file: 77 targets compared per family |
| G3 amendment | lib `g3_pixel_clause` (same-index applied vs null, union mask + whole frame, ≤ 2); `g3_temporal` kept as a diagnostic | proof file (synthetic both ways and a real-data negative) and the updated `proof-evaluators.py` |
| Resume record | `082-07-evidence/run/supersede.json` (2 entries), a `RULING` ledger row, 7 aliases renamed `…__SUPERSEDED_082-07d` | `supersede-act.json`; proof file (resume re-runs as `_A2`, history kept, superseded halt no longer stops) |
| REASON A2 | declared FIXTURE-CANNOT-EXERCISE, class D (`legs.DECLARED_CANNOT`) | source + G0 F-MW reading (AMENDMENT 1, A1.8) |
| Latent defect fixed | an alias `BankError` in `run_leg` escaped as an uncaught exception | now `INVALID-BANK` + stop code 6; proof file shows the code-6 path |
| Stub mode | `IAI_R53_STUB_LAUNCH=1` with a scratch `IAI_R53_ROOT` | refused at import on the live evidence dir |
| New build constants | `common.STAGED` = 082-07c's staged set; `ARCHIVE` = `m53-s1-normalfix-cook-30FE0FDE` | the preflight checks them against the staged files |

The 082-07a harness files were backed up to `082-07d-evidence/harness-before/` first. Their hashes matched the 082-07a boundary,
so nothing had drifted.

## 2. Readings

### 2.1 The cause of 363,001 (named)

The pre-onset/post-revert difference seen in both applied and null runs is **UE 5.1's tonemapper grain quantization**
(`r.Tonemapper.GrainQuantization`, default on). It is a ±½-LSB dither seeded from `Halton(ViewState frame index mod 8)`
(`PostProcessTonemap.cpp:637-643`, `PostProcessTonemap.h:23`).

Measured on the 082-07b bank (`082-07d-g3-dither-probe.json`):

- max |d| is 1 on every changed pixel;
- `start−1` and `end+1` are 11 engine frames apart on every event (11 mod 8 = 3);
- pairs of frames whose engine frames differ by a non-multiple of 8 differ over 362k–364k pixels;
- from `session_index` 23 on, every pair at a multiple of 8 is bit-identical;
- applied against null is 0 over the whole frame on all 100 frames.

The frame-index step is 1 on 85 rows and 3 on 14: the settle ticks are uncaptured.

A second, small start-up element is **not established**. It sits on the reason-producer tile row (y ≈ 255–316, up to ~316 pixels,
up to 3–4 levels) and is present only in the first ~22 frames.

### 2.2 G3 under the amended rule, from the bank (read-only, `g3-reeval-readonly.json`)

| row | verdict | judged events | region max | whole-frame max | diagnostic (px, max) applied / null |
|---|---|---|---|---|---|
| U-C1 … U-C5, U-D1 … U-D6 (11 rows) | PASS | 7 each | 0 | 0 | (363001, 1) / (363001, 1) |
| U-N1 (history only, superseded) | PASS | 7 | 0 | 0 | (363001, 1) / (363001, 1) |

The 8th event of every leg ends at the frame cap, so it has no post-revert frame and is not judged, as before. The amended rows
are written to `gates.json` by the **locked** lib after the boundary, and must reproduce this table.

### 2.3 The admission pre-check against the real 082-07b census

- **Refused (FIXTURE-CANNOT-EXERCISE):** all **23 normal-family rows** read `normal_unconnected`.
  - G-ID and G3 N-N1;
  - G-ID-M and G3 for the four ChainD normal rows;
  - the twelve G4 normal rows;
  - G4 TWO.
- **Admitted:** all **93 uv-only rows**.

In the sequencer proof, the harness stops with code 12 **before `ID_NN1` launches**. The U-C2 and F-MW legs before it run
normally, and a resume stays stopped without launching anything. With the synthetic fixed census, all 116 rows are admitted and
the campaign completes.

### 2.4 G0 normal coverage against the real 082-07b census

Both families are compared on all 77 targets. The normal-family findings are exactly the 7 normal-map targets (predicted
`APPLY`, read `normal_unconnected`); there are 0 uv findings. The synthetic fixed census gives 0 findings, and single synthetic
flips are caught.

### 2.5 REASON A2

- **Source:** in `TexCorruptTree.cpp:566-574`, A2 (`no_normal_map`, rank 31) fires only for a slot with no normal-map binding. A
  binding failure (rank 20) wins `PickEarliestSlot` (`:880`).
- **Reading:** the MainWorld floor's normal family reads `not_fully_resident`. Another MainWorld actor reads uv
  `not_fully_resident` but normal `no_normal_map`.
- **So:** the floor has a connected normal map, and A2 cannot be produced on it at any residency.
- **Declared:** FIXTURE-CANNOT-EXERCISE, class D. The leg still runs.
- **Consequence:** `no_normal_map` has no Q producer in S1. This is flagged for chat.

## 3. Proofs (all in `_reviews/082-07d-evidence/prep/`, run against the final lib)

| proof | checks | failed |
|---|---|---|
| `proof-082-07d.py` (admission, G0 normal, G3 amended, sequencer resume/pre-check, A2) | 37 | 0 |
| `proof-evaluators.py` (082-07a suite; G3 cases rewritten to the amended rule, incl. "+1 now passes", "+3 fails", "time-varying scene in both legs passes") | 209 | 0 |
| `proof-sequencer.py` (082-07a suite, unchanged) | 20 | 0 |
| `proof-disk.py` (082-07a suite, unchanged) | 15 | 0 |

The boundary-dependent proofs are the end-to-end window runs, the dry run and the preflight replay and negatives. They run after
the lock and are reported in the final message and `082-07d-evidence/`.

## 4. Problems met

- **The supersede act's first attempt stopped on its own assert, with nothing changed.** It expected 19 superseded rows and found
  17. TRIP and G11 of `NULL_NN1` were never evaluated in 082-07b: `evaluate_ready` stopped at G3 N-N1, which precedes the per-leg
  read-backs in declaration order. The act now names the two rows and records them as never evaluated (G319).
- **An alias clash would have crashed the window.** `alias_hardlinks` raised a `BankError` that `run_leg` did not catch, so the
  window would have ended with a traceback and no postflight. The proof's resume case found it; it is fixed and proven.

## 5. Deviations and decisions taken here, flagged for chat

1. **REASON A2 changed class**, Q → D, as FIXTURE-CANNOT-EXERCISE. This is the only stop-row change, and it rests on the source
   and G0 argument in §2.5.
2. **The admission pre-check does not cover F-MW rows**, because G0 F-MW is a first-tick reading. Their declared non-stop
   readings (`NOT-ADMITTED`, `INVALID-ARBITER`) remain.
3. **The F-MW G3 arbiter condition** is now whole-frame ≤ 2 (was = 0), consistent with the same-index rule.
4. **The alias `BankError` fix** was not asked for. It is a latent-defect fix found by the proof.
5. **The amended G3 rows are written after the lock**, so the locked lib produces them. The read-only table above must be
   reproduced exactly.

## 6. State at the end of the docs commit

- **Branch:** `feat/m53-uv-normal-corruption` carries this journal, AMENDMENT 1, the status block and G318–G320. It is pushed.
- **Next:** the boundary is re-issued against this head and the 082-07c staged build.
- **Untouched:** `m51` (`53bf725`), `master`, tags, `ToCodex\`, and every other repo.
- **Bench:** idle, the staged build unchanged. No build, no cook, no leg.

## 7. Hand-off to 082-07e

Run the window with the new tag given in the 082-07d final report. It resumes in this order:

1. `G0_FSYN`, the census of the new build;
2. the U-N1 re-run (its attempts start at `_A2`);
3. N-N1 and everything after it.

**If G0 does not admit a normal target**, the harness stops with FIXTURE-CANNOT-EXERCISE (code 12) before that row's first leg.
That stop is a finding about the fixture or the feature, to be ruled; it is not a FAIL.
