# 082-07a — m53 S1 part 3a: the S1 leg harness, built and locked, with disk-policy items 1–2. BENCH-FREE

Session 082-07a, 2026-09-27, Claude Code (Opus 5.5), headless from the GDP mailbox. Brief: build and lock the harness for
§R13.1 step 6 (the S1 gate legs); 082-07b runs the legs. Inputs: plan revision 3.4 §R12, §R12.2, §R12.4, §R13.1 and its stop
list; rulings 082-05, 082-06b, 082-06c, 082-06d; 083-02 policy items 1–2. Evidence: `D:\IntrusiveAnomalies\_reviews\082-07-evidence\prep\`.

**Outcome: the predictions and criteria file is declared; the harness is built on the 081-23 infrastructure; every evaluator
passes a synthetic good case and fails a synthetic bad case; the dry run of all 194 leg invocations is clean. The boundary is
locked after this commit, then the post-lock dry run and the offline preflight replay run against it (§6).** No game launch,
no leg, no build, no cook, no tag.

## 0. Where the work ran

- Main checkout untouched on `m51` at `53bf725`. Docs committed from the branch worktree `E:\IA_BuildCache\_r53_src\AnomalyInjector`.
  The harness lives in `D:\IntrusiveAnomalies\_reviews\` (not a repo) and is hash-locked.
- Staged build unchanged and checked: exe `2FCDF059`, container `9A26D497` / `10D7F0C0` / `ABD931A2`, `462B8AC6` / `BB05CF99`.
- Source at the branch head equals the source the exe was built from (`git diff 1fc20ea HEAD -- Source` is empty).
- The only processes started were Python, PowerShell (the runner in `-Stub` mode) and git. Nothing was killed.

## 1. The predictions file

`docs/predictions/2026-09-27-m53-s1-legs.md`, declared before any leg. For each step-6 gate it gives the legs, maps, targets,
recipe, the exact pass/fail rule, the class (Q / D / PRE / PROOF), the minimum count and the stop row. It also gives the leg
validity rules and budgets, how pixels are paired and which region is read, the owed in-engine proofs (P2-1, P2-2, P2-3,
P2-5, P2-6 and the F+2 balance), predictions, named risks, bench time, and twelve named deviations from the plan's wording.

- **Tick orders:** no S1 gate asserts a per-frame alignment, so every S1 leg runs native order. The only alignment gate,
  `G1`, is S2.
- **Counts:** 194 legs (185 F-SYN, 8 F-MW, 1 F-GATE), 548 gate rows.

## 2. The harness (`_reviews/082-07-*`)

| file | role |
|---|---|
| `082-07-common.py` | paths, the staged-build hashes, process classification by path (ours vs foreign, `dotnet` = UBT only), the quiet test, the event log |
| `082-07-legs.py` | the leg catalogue: 194 legs, each with fixture, map, id, target, levers, config, frames, kind and role |
| `082-07-run.ps1` | one attempt: launch by path with `-abslog`, force and record focus, wait for the artifact (or the census / cancel / close condition), flush wait, end **our own PID** (kill, or `CloseMainWindow` for the world-end legs). Refuses if the bench is not idle; kills nothing else. `-Stub` records the invocation without launching |
| `082-07-lib.py` | session and log parsing (regexes checked against the plugin's own `UE_LOG` format strings), pixel comparison, leg validity, banking (policy 1–2), the person-present gate, the quiet wait and the wait budget (copied from the 081-23 lib), the resume-safe ledger, the attempt budgets, stop at the first failure, and one evaluator per gate |
| `082-07-preflight.py` / `082-07-postflight.py` | the boundary, the feature head (local == origin), the committed predictions, every harness hash, the staged build, `m51` untouched, no process of ours, disk floors (E: 50 GB, D: 15 GB); postflight also checks that every accepted alias is a hardlink set and that no exe-side leg copy is left |
| `082-07-window.py` | preflight → bounded quiet wait → person-present gate → the sequence → postflight |

**Reused 081-23 infrastructure:** the process classifier, the quiet wait, `PersonMonitor` and the person-evidence prongs, the
person-present gate with its 45-minute budget per tag, ENV-INTERRUPTED (never counted, cap 5), the resume-safe ledger, the
boundary lock and stop at the first failure (a recorded stop stays stopped until a ruling).

**New in this harness:** per-attempt banking with manifest verification; the exe-side copy is deleted after verification; the
alias is written as hardlinks; a separate pose budget; and gate evaluation as soon as a gate's legs are accepted.

## 3. Proofs (all offline, `prep/`)

| proof | checks | result |
|---|---|---|
| `proof-evaluators.py` | 207 | **0 failed.** Each of the 30 offline-proven wrong-copy pairs in the 082-06 manifest was rebuilt as a picture from the fixture's own images (chanswap, srgbtwice, alpha, normal, texelshift, mipshift and mipgen from the chain-(a) DDS mips, noclear). G-ID passes the identity picture (±1 requantisation noise) and fails the fault picture. The wrong-copy check reads the fault as wrong and reads **invalid** when the fault is invisible. The measured minima equal the manifest's (for example chanswap 120, srgbtwice 73, texelshift 128). Every other gate evaluator is run through the real gate registry on synthetic sessions and logs, and each has a good and a bad case. The synthetic log lines are generated from the `UE_LOG` format strings in the source (and the engine's `LogGarbage` line), so the regexes are checked against the real formats |
| `proof-sequencer.py` | 20 | **0 failed.** Resume skips accepted legs. Three invalid attempts give NOT-RUN, and a resume after it launches nothing. A Q fail stops before the next leg and stays stopped. D readings never stop. A failed precondition stops with code 3; an evaluator error with code 7. ENV-INTERRUPTED is not counted and caps at 5. The pose budget is 6. A foreign editor gives code 5; not quiet gives code 4; the person-present gate waits within its budget and stops when the budget is spent |
| `proof-disk.py` | 15 | **0 failed** (bank on E: through a D: junction, like the real bank). See §4 |
| `dryrun.py pre-lock` | 194 legs | **0 problems.** Each invocation went through the real runner in `-Stub` mode. The ExecCmds round-trip; all 36 distinct console tokens are present in the staged exe; every F-SYN target is a fixture actor; every leg feeds a gate and every gate leg exists; no game process started |
| runner parse | — | `082-07-run.ps1`: 0 parser errors |
| post-lock dry run and offline preflight replay | — | run **after** the lock, which needs this commit. Results are in `prep/dryrun-post-lock.json` and `prep/preflight-replay/`, and in the 082-07a report |

## 4. Disk policy (083-02 items 1–2)

- **Item 1.** `bank_attempt` copies the exe-side leg to `_bench_sessions_bank\M53S1_<leg>_A<n>\`. It compares the two manifests
  (path, size, SHA-256) and only then deletes the exe-side copy (exact path). A mismatch keeps the exe-side copy and stops the
  window. An existing bank folder without a verified record is never overwritten. Resume is idempotent.
  - Proven: verified and deleted; a sibling folder untouched; a one-byte flip caught with the exe-side copy kept; an existing
    folder refused.
- **Item 2.** `alias_hardlinks` writes `M53S1_<leg>\` as NTFS hardlinks of the accepted attempt. It verifies the file id, the
  link count (≥ 2) and the manifest. An existing alias that is a copy is refused. There is no copy fallback (a cross-volume
  link raises).
  - Proven through a D: → E: junction, as on the real bank.

## 5. What building it found (each is in the predictions file's §9)

1. **F-SYN has no A47 guard.** `IAI.Bench.InputLock` and `IAI.Bench.PlaceView` refuse every map except `CB_GateLevel` and
   `L_ShooterGym` (`AnomalyBenchModule.cpp`), and `IAI.Bench.ChangeTeardownAt` is `CB_GateLevel`-only. Legs whose pixels are
   compared therefore require the PlayerStart rotation `(0,0,0)` and the campaign reference pose, with a separate budget of 6
   pose-only attempts. G313.
2. **A packaged unattended leg has a console channel at startup only.** Forced GC uses the engine purge timer; level change
   and a re-created component are UNEXERCISED; world end is a graceful close of our own process.
3. **The G-ID region needs the m26 mask pass.** `IAI.Capture.Mask`'s compiled default is off (the ini turns it on), so both
   mask switches are now set in ExecCmds.
4. **G4 TWO** compares against the other event's live bytes. Its scratch is already pending when APPLIED prints, so its
   reservation is the wrong number.
5. **The Bot's name is spawned**, so it is resolved from G0's `IAI.ListActors`.

## 6. The boundary and 082-07b

- `_reviews/082-07-evidence/boundary.json` pins the pushed feature head that carries this journal, the predictions' SHA-256,
  every harness file, the staged build and the fixture inputs. The lock was taken after this commit; its own SHA-256 is in
  the 082-07a report. Any later commit on the branch needs a re-issue before 082-07b (G-rule from 081-33 / 081-39).
- **082-07b:** `C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\082-07-window.py 082-07b 540` (resume: the same command,
  with a new budget tag if the wait budget is spent). About 2 h of legs plus waits.

## 7. State at the end

- Branch: the predictions file, this journal, G313 and the status block; one `docs` commit, pushed. `m51`, `master`, tags,
  `ToCodex\` and `E:\AmmaYT` untouched; no tag.
- Bench: unchanged, staged `2FCDF059` + `9A26D497` / `10D7F0C0` / `ABD931A2` + `462B8AC6` / `BB05CF99`.
- Scratch from the proofs was removed (`prep/scratch-*` stays on D:, a few MB; the E: scratch bank was deleted).

## 8. NEEDS-DECISION

- The twelve named deviations in the predictions file's §9, in particular: forced GC via the purge timer; level change and
  re-created component UNEXERCISED; the union-mask region; the F-SYN pose rules and budget. Each was fixed before any leg.
