
## AMENDMENT 4 — 082-07k, 2026-09-28, declared before the resume (082-07l)

**Source:** `_reviews/082-07j-chat-ruling-can-premise.md`, rulings 1–3.

**Bench-free.** No leg, build, cook, plugin source change or CaptureBench change was made. The build under test is unchanged
(A2.1: `2FCDF059` / utoc `20DA6F98` / ucas `534C5863`).

**What it carries:**

- G4 CAN re-encoded as STOP-BEFORE-FIRE, and its S2 debt (A4.1);
- TRIP read from `run_summary.json` (A4.2);
- the runtime-conditions review of the 35 unrun legs (A4.3), with its three encoding fixes:
  - the pose scope for F-MW and F-GATE (A4.3.1);
  - the G4 GC detector (A4.3.2);
  - the G4 TWO frame-1 refusal (A4.3.3);
- the person-evidence read (A4.4);
- supersede entry 5 (A4.5);
- the harness changes (A4.6);
- the proofs (A4.7);
- the resume plan and predictions for 082-07l (A4.8).

### A4.1 G4 CAN-UV / CAN-NM: STOP-BEFORE-FIRE (class Q, ruling 1)

**Measured (082-07j, 3 of 3 attempts):**

- The runner forces the game window into the foreground from 400 ms after launch (`082-07-run.ps1:55,67-76`, `Hold-Focus` →
  `[M53Fg]::Force`). `launch.json` records `focus_at_s` 0.9, 1.3 and 1.2 s.
- The ExecCmds batch runs at engine frame 1, about 2.3 s after launch.
- `StartRun` waits for focus only when the window is **not** focused (`AnomalyCaptureSubsystem.cpp:3362-3373`), so it began
  the run synchronously. The same-batch `IAI.Capture.Stop` then FINISHED it, 7 ms later, with `0 frame(s) (positive=0) |
  0 burst(s)`.
- `texcorrupt_fires_applied=0`, and there was no m53 line.
- The banked session holds `run_summary.json` and `annotation.json` (0 events), and no `labels.jsonl`.

**The premise "the window is not yet focused at frame 1" can never hold under this runner (G332).**

**Re-encoding (ruling 1, option a):** the rows keep their names `CAN-UV` / `CAN-NM`. The legs become kind `stop_before_fire`,
launched through the unchanged runner as kind `capture`. The batch is `IAI.Capture.Start …, IAI.Capture.Stop`.

**The row (`lib.sbf_check`, `SBF_RULE`) PASSES only if all of these hold:**

- exactly one FINISHED line, with 0 frames;
- `run_summary` `total_frames` 0 and `texcorrupt_fires_applied` 0;
- `annotation.json` carries no event, and no frame PNG exists;
- no CANCELLED line;
- **no m53 line of either id:** no APPLIED, REFUSED, ROLLED BACK, NoApply 2, LEDGER or COLL line, and no live-tree DECIDE / REFUSED
  block.

**Validity:**

- **Foreground and A63:** as for a capture leg.
- **Session:** a session with `run_summary.json` is required.
- **A cancelled-before-focus run is INVALID** (`focus:` error), never a FAIL: it did not meet the synchronous-start premise.
  It is retried within the 3-attempt budget.

**Proven both ways (A4.7):**

- the three real banked attempts PASS;
- each of these FAILS:
  - a synthetic APPLIED, REFUSED, LEDGER, DECIDE or ROLLED BACK line;
  - FINISHED with 5 frames;
  - `total_frames` 5;
  - a written PNG;
  - a CANCELLED line;
  - no session;
- the real accepted `G4_FIN_UV` leg FAILS (applied, frames > 0).

**Not passed from the bank:** the three attempts carry person evidence (A4.4). Both legs run fresh.

**The declared cost:** the cancel-before-focus branch itself (`AnomalyCaptureSubsystem.cpp:5874`) is not exercised in S1.

**S2 debt:** a bench-only "treat as unfocused" lever, so that `StartRun` takes `ArmedPending` deterministically, and a full
CAN row on it. S2 builds anyway for its GC, level-change and re-created-component levers.

### A4.2 TRIP reads `run_summary.json`, not `Sess`

`Sess` requires `labels.jsonl`, and a 0-frame STOP-BEFORE-FIRE session has none. Under the pre-edit lib, TRIP on such a leg
raised `FileNotFoundError` → EVALUATOR-ERROR (stop 7).

TRIP now reads `texcorrupt_rt_mip_mismatch` from `find_session(...)/run_summary.json`. That is the same file and key, so the
readings are identical.

**Proven:**

- on the real CAN session: new PASS (0); the pre-edit lib raises;
- no-flip: all 158 recorded TRIP rows are unchanged (A4.7).

### A4.3 The runtime-conditions review of the 35 unrun legs (ruling 2)

The raw data is in `_reviews/082-07k-evidence/`:

- `review-static-082-07k.json`: every lever and console token in the staged exe, in both encodings, with 0-hit controls;
  the three maps in the staged utoc;
- `review-logs-082-07k.json`: the ledger lines of 158 accepted legs, F-SYN poses, the Bot name, durations;
- `review-mw-pose-082-07k.json`, `review-mw-traj-082-07k.json`, `review-mw-majority-082-07k.json`: 329 / 161 banked
  MainWorld sessions;
- `review-apply-frame-082-07k.json`: DECIDE → APPLIED frames over 1,069 real fires;
- `pp-timing-082-07k.json`: the person-evidence timing (A4.4).

**The real runner's runtime facts, `082-07-run.ps1`:**

- **Focus and input:**
  - `Hold-Focus` runs on every startup sample (every 400 ms, until 8 stable) and, in capture kinds only, whenever a capture
    sample finds another foreground window.
  - Every `Force` sends a synthetic Alt down/up (`:41`).
  - census and cancel kinds take no action after startup.
- **Process lifetime:**
  - capture and census kinds are killed 2.5 s after the artifact settles;
  - close kinds use `CloseMainWindow()` and wait 30 s.
- **The ExecCmds batch** runs at engine frame 1.

| unrun legs | rows | runtime conditions the premise depends on | checked against (real runner / banked real logs / source) | disposition |
|---|---|---|---|---|
| `G4_CAN_UV`, `G4_CAN_NM` | G4 CAN-UV/NM (Q); TRIP; G11 | focus at the frame-1 `Start`; `Start` + `Stop` in one frame; a 0-frame session's artifacts | `Hold-Focus` forces focus by 0.9–1.3 s; the 3 real attempts; the banked session has no `labels.jsonl` | **affected → A4.1 + A4.2** (proven) |
| `G4_FIN_NM` | G4 FIN-NM (Q) | the frame cap with a live event; fixed step | G4 FIN-UV PASSED on the real bench (082-07j), same shape; the N-N1 legs are real | **unaffected** |
| `G4_DES_UV`, `G4_DES_NM`, `G4_DES2_UV` | G4 DES, DES2 (Q; P2-6) | the DestroyTarget latch counting anomalous ticks; the NoApply 2 fire counting as active; the frame of the apply line; the post-destroy no-match lines | TexCorrupt does not override `IsCurrentlyAnomalous` (default `IsActive`, `IAnomaly.h:30`); NoApply 2 sets `bActive` (`Anomaly_TexCorrupt.cpp:769-782`); DECIDE → APPLIED is 0 frames on **1,069 of 1,069** real fires; validity cuts no-match lines after the destroy; lever in the exe (7 hits) | **unaffected** |
| `G4_END_UV`, `G4_END_NM` | G4 END (Q; P2-6 EndPlay) | the APPLIED line read live from the log; `CloseMainWindow` → a graceful exit within 30 s; EndPlay(Quit); live bytes at world end | the runner read `TEXCORRUPT-CENSUS END` live in 4 real census legs; `M40_TEARDOWN_GRACEFUL` (2026-09-02): `CloseMainWindow` → exit code 0, `Object subsystem successfully closed.`; `UGameEngine::PreExit` routes `EndPlay(Quit)` (`GameEngine.cpp:1249`) → `OnWatchedTargetEndPlay` treats Quit as world-ending (`AnomalyInjectorSubsystem.cpp:413-436`) → `Revert`; `ReleaseToPending` drops `Live` on the game thread (`TexCorruptPure.h:350-357`) | **unaffected** (the m53 world-end path itself is what the row exercises) |
| `G4_GC_UV`, `G4_GC_NM` | G4 GC (Q) | GC passes inside the event; the detector line reaching the log | `gc.TimeBetweenPurgingPendingKillObjects 1` gives about one pass per 30 fixed-step frames (`UnrealEngine.cpp:1805-1820`); **`LogGarbage` is declared `Warning` (`GarbageCollection.h:49`) and "Collecting garbage" is logged at `Log` (`GarbageCollection.cpp:2293`)**; 3 real 65 s logs with GC passes carry 0 `LogGarbage` lines | **affected → A4.3.2** (proven) |
| `G4_FR_UV`, `G4_FR_NM` | G4 FR (Q) | the ForeignReplace lever | lever in the exe (3 hits); deterministic, no runtime premise | **unaffected** |
| `G4_FS2/FS2O/FS3/FS4/FS5/FS6_UV`, `…_NM` (12) | G4 FS (Q; P2-5) | the FailStep lever; the rollback ledger read at F+2 (`off=2`, live 0, pending 0) | lever in the exe (10 hits); **all 1,069** real ledger lines read `off=2 on_frame balanced live=0 pending=0`; releases are due at F+2 (`TexCorruptPure.h:357`) | **unaffected** |
| `G4_TWO` | G4 TWO (Q) | `IAI.Apply` at engine frame 1 (live tree + transaction); a graceful close | the live frame-1 census reads `TC_NN1` normal **APPLY at engine frame 1** (`M53S1_G0_FSYN_A3`, `[  1]`); **the transaction at frame 1 has never run on the bench**; the close as for END | **affected (partly) → A4.3.3** (proven) |
| `COLL_A_FSYN`, `COLL_N_FSYN` | G-COLL F-SYN (D); P2-2 | the collateral set taken at apply from on-screen primitives → the F-SYN pose | **156 of 156** real F-SYN capture sessions at one pose, `(0,0,500)/(0,0,0)` | **unaffected** |
| `FMW_CTL_A`, `FMW_CTL_B`, `FMW_ID` | BAND F-MW (D), G-ID-FMW (Q), G3 FMW (Q), MW-FLOOR (D) | the MainWorld camera (`validity`'s modal clause); cross-launch determinism; residency (A3.4) | **329 banked MainWorld sessions:** rotation settles by si ~24, but the origin eases through si ~84–112, so modal coverage is ~0.19. **146 of 161** frame-1 launches follow one trajectory bit-exactly; held out 73 / 73 | **affected → A4.3.1** (proven) |
| `REASON_S3O`, `REASON_A2`, `COLL_A_FMW`, `COLL_N_FMW` | G-REASON S3O (Q), A2 (D); G-COLL F-MW (D); MW-FLOOR | the same camera (the modal clause applied to **every** capture kind); the Bot's name | these rows read logs only; the 082-07b G0_FMW listing gives `BP_Bot_C_2147482434`, equal to the banked constant from an earlier campaign and build | **affected → A4.3.1** (no pose clause) |
| `REASON_S8` | G-REASON S8 (Q) | CB_GateLevel cooked; the target name; the camera | the staged utoc lists `CB_GateLevel`; the targets carry `BasicShapeMaterial` (`make_gate_level.py:60,69`); `no_textures` is decided at `TexCorruptTree.cpp:540`; the gate-level camera has a recorded A47 drift | **affected → A4.3.1** (no pose clause: the row reads logs, and the clause could only add a stop) |
| all 35 | TRIP, G11, P2-1, P2-3, G3-COUNT | the TEXCORRUPT-ASSETS line at subsystem init; `run_summary` | G11 PASS on 158 of 158 including F-MW; TRIP via A4.2 | **unaffected** |
| all 35 | person-present gate, ENV-INTERRUPTED | the runner's own input | A4.4 | **unaffected** (cannot void a leg) |

**Rows that can only be exercised with a build or fixture change:** none among the 35. The cancel-before-focus branch
(A4.1) goes to S2.

#### A4.3.1 The pose scope (F-MW, F-GATE)

**The defect.** `validity()` applied the settled-camera clauses to every `capture` / `close_after` leg on every map:

- modal coverage ≥ 0.5;
- every labelled row on the modal view;
- for pixel roles, the campaign pose reference.

On MainWorld the camera never holds one pose (at the check's 0.1 cm resolution) inside a 100-frame leg. Under the pre-edit lib
**every** F-MW leg would have been INVALID-POSE on 6 of 6 attempts, then NOT-RUN, stopping at `FMW_CTL_A`.

**The rule (`lib.pose_scope`, `POSE_RULE`):**

- **F-SYN:** the modal clauses, unchanged (156 of 156 real sessions at one pose; C1 no-flip).
- **F-MW pixel roles** (`FMW_CTL_A/B`, `FMW_ID`): every captured row must equal the **pinned MainWorld trajectory** at its
  `frame_index`.
  - **The reference file:** `_reviews/082-07-mw-trajectory.json`, sha256 `1ce9d988…`, locked by the boundary.
  - **How it was built, read-only from the bank:**
    - the per-frame majority over the 161 banked MainWorld sessions that start at `frame_index` 1;
    - the 146 sessions that match that majority on every covered frame;
    - their union, with 0 disagreements.
  - **It covers** `frame_index` 1–798.
  - **Gap rule:** a gap frame is checked against its two neighbours when they are equal, and is otherwise an error. The only
    gap an F-MW leg can reach is fi 5, inside the initial pose, which holds on fi 1–23.
  - **A row off the trajectory is INVALID-POSE** (6 attempts). The per-launch majority share is 146/161 = 0.907, so six
    failures in a row is about 6 × 10⁻⁷.
- **Non-pixel roles on F-MW / F-GATE:** no pose clause. Their rows read logs only.

**Proven both ways** (A4.7 C1–C10, on real banked sessions):

- a majority MainWorld session is on the trajectory; the pre-edit modal rule reads its coverage < 0.5;
- `FMW_CTL_A` / `REASON_S3O` validity reads `[]` under the new lib, pose errors under the pre-edit lib;
- a real no-lever outlier launch (`M52_M52_G3_HOLD_NAT`) is OFF the trajectory;
- **held out:** a reference built from 73 real sessions predicts the other 73 exactly;
- the gap rule, both ways;
- validity reads the same errors on every live attempt under both libs, except the three CAN attempts, whose leg changed by
  design.

**Declared risk:** if the build under test drives the camera off the bank's trajectory, every F-MW pixel attempt is INVALID-POSE
and the leg stops NOT-RUN (code 2). That is a harness stop with the reason printed, not a verdict.

#### A4.3.2 The G4 GC detector

**The defect:** the GC row matched `LogGarbage: Collecting garbage`, which this build never logs at default verbosity. The row
could only read UNEXERCISED, and gave the wrong reason ("no pass fell inside the first event").

**The fix:**

- the GC legs carry `Log LogGarbage Log` ahead of the Start;
- the command echoes `LogGarbage <pad> Log` through `Exec` (`LogSuppressionInterface.cpp:600-617`), matched by `RX['gcverb']`;
- the evaluator reports `gc_verbosity_echo` and `gc_passes_in_leg`;
- with no `LogGarbage` line anywhere, the row reads **UNEXERCISED with the reason "detector blind"**, never a pass.

**Proven (D1–D7):**

- the three real 65 s CAN logs have GC passes (`RemoveUnreachableObjects`) and 0 `LogGarbage` lines;
- the echo regex matches the echo and rejects the GC line and a Warning line;
- a pass inside the event → PASS;
- none anywhere → UNEXERCISED "blind" (the pre-edit lib says "no pass inside");
- passes only after the revert → UNEXERCISED "no pass inside";
- both GC legs issue the lever before the Start.

#### A4.3.3 The G4 TWO frame-1 refusal

**The premise:** the second id, fired by `IAI.Apply` at engine frame 1, applies.

- **Measured:** the live frame-1 census reads `TC_NN1` normal APPLY.
- **Not measured:** the transaction at frame 1.

**Under the pre-edit lib** a refusal read FAIL ("the second id was not applied"), which halts with code 1.

**Now:** if the second id is not applied and its own REFUSED or ROLLED BACK line is logged, the row reads
**FIXTURE-CANNOT-EXERCISE**, with the refusal named. That is not a Q halt. With no line at all it is still FAIL.

**Proven (E1–E4).**

### A4.4 Person evidence (ruling 3)

**From the timestamps** (`pp-timing-082-07k.json`): each person-present gate's `idle_s` gives the last input's time. The raw
monitor samples are not persisted.

- **13 of 174 attempts** carry person evidence.
- **The 10 capture / census ones** place their last input at `sampling_started_utc` −0.010 … +0.033 s.
  - That instant is the runner's final startup `Hold-Focus` → `[M53Fg]::Force`, and its synthetic Alt.
  - Clock granularity puts it just inside the span, and no force window covers it, because the startup samples lie before
    the span.
  - ⇒ **The runner's own input is a source.**
- **The three CAN attempts:**
  - A1 and A2 each show an input at **+51.70 s / +51.67–51.70 s after the runner started**. That is 47 s after the last
    runner action and 13 s before the kill.
  - The cancel branch sends nothing then, and the game log is silent between 23:41:46 and GC at 23:42:47.
  - **This input is not the runner's. Its cause is not established.**
  - A1 and A3 report 2 events. Their second is untimed; the only mechanism measured elsewhere is the boundary Alt.

**Can it void an unrun leg? No.**

- Person evidence never adds a validity error.
- **Its only effects:**
  - a foreground-only failure becomes ENV-INTERRUPTED instead of INVALID, which is more lenient (cap 5);
  - the next leg's person gate waits up to 60 s from the 45-minute budget. At the measured rate (~6 %) that is about 2 waits over
    35 legs.
- The longest real attempt is 11.1 s, so the +51.7 s event cannot fall inside an unrun leg.

**⇒ No detector change** (ruling 3's condition is not met).

**For S2:**

- persist the monitor's input timestamps in the ledger row;
- excuse inputs within `PP_FORCE_S` of the final startup `Force`.

### A4.5 The supersede record, entry 5

- **Record:** `_reviews/082-07-evidence/run/supersede.json`, sha256 `e67f0ec9…`. The prior record was `ff7f884f` (the 082-07i lock).
- **Attempts:** `M53S1_G4_CAN_UV_A1/A2/A3` (all INVALID).
- **Record-level entry:** the label-less NOT-RUN row (`records`, keyed `(leg, status, recorded)`). The pre-edit `ledger_live`
  filtered by label only, so the NOT-RUN row would have kept every window stopped (code 2).
- **Gate rows superseded:** none were recorded.
- **Kept live:** the ADMIT row `G4 CAN-UV`, a frame-1 census reading.
- **Never evaluated:** G4 CAN-UV, TRIP and G11 `G4_CAN_UV`.
- **Bank:** the three attempt dirs are renamed `…__SUPERSEDED_082-07k`, 12 files each, manifest-verified; nothing was deleted.
- **After:**
  - no halt, no live NOT-RUN;
  - 158 accepted legs, 35 unaccepted, the first being `G4_CAN_UV`;
  - no ready row.
- **The fresh attempt is labelled `A4`:** attempt numbers now count labelled ledger rows only. No other leg's next number
  changes (F3).

### A4.6 Harness changes carried by the re-issued boundary

- **`082-07-legs.py`** (`c552c587`):
  - `G4_CAN_*` kind `stop_before_fire`;
  - the `G4_GC_*` levers gain `Log LogGarbage Log` (`GC_LOG`).
  - Still 193 legs and 548 gate rows.
- **`082-07-lib.py`** (`96831d8f`):
  - `RX` `finished` / `gcverb`;
  - `pose_scope`, `trajectory_errors` and `mw_trajectory`;
  - `validity(…, sdir=)`, with the stop-before-fire branch;
  - `RUNNER_KIND`;
  - `build_cmds` (the Stop);
  - `sup_records` / `ledger_live`;
  - `run_leg` (labelled attempt numbering; the modal reference is stored only for modal scope);
  - `ev_trip` (A4.2);
  - `sbf_check` / `SBF_RULE`;
  - `ev_g4` CAN / GC / TWO.
- **New locked input:** `082-07-mw-trajectory.json` (`1ce9d988`).
- **Unchanged:** the runner, `082-07-common.py`, preflight, postflight and the window.
- **Pre-edit copies:** `_reviews/082-07k-evidence/prep/pre-edit/`: lib `b1e0cdcd`, legs `84a423a6`, common `91af53d8`.

### A4.7 Proofs, no-flip and dry run (the counts are in the 082-07k journal and report)

- `proof-082-07k` (new, 46 checks, both ways against the pre-edit lib and real banked data);
- the six earlier suites, re-run on the final lib from copies (only output paths changed; `proof-082-07d`'s one synthetic F-MW
  sequencer leg is given a valid A4.3.1 camera);
- the no-flip over every recorded live gate row, before and after the supersede act;
- the dry run before and after the lock;
- after the lock: the stubbed end-to-end window and the preflight negatives.

### A4.8 Resume plan and predictions for 082-07l

**Command:** `C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\082-07-window.py 082-07l 540`, with neither `IAI_R53_ROOT`
nor `IAI_R53_STUB_LAUNCH` set.

**Order:** it resumes at `G4_CAN_UV` (a fresh `A4`) and runs the 35 unaccepted legs in order:

- the 25 remaining G4 legs, including the fresh `G4_CAN_NM`, `G4_DES2_UV` and `G4_TWO`;
- `COLL_A/N_FSYN`;
- `FMW_CTL_A/B`, `FMW_ID`, `REASON_S3O`, `REASON_A2`, `COLL_A/N_FMW`;
- `REASON_S8`.

No decided leg is re-launched. The dry-run estimator gives 24 minutes; waits come on top.

**Predictions:**

- **CAN-UV, CAN-NM:** PASS (STOP-BEFORE-FIRE).
- **G4 FIN-NM, DES ×2, FR ×2, FS ×12, END ×2:** PASS.
  - **P2-5:** CONFIRMED (every FS row reads `off=2 live=0 pending=0 balanced`; FS2O created the resource first).
  - **P2-6:** CONFIRMED (DES2's apply→LOST offset equals DES-UV's; END's world-end revert leaves live 0 and the process exits
    within 30 s).
- **G4 GC ×2:** the echo is present, about 6 passes fall inside the first event, and the row PASSES. A blind UNEXERCISED would
  be a finding (the lever did not take).
- **G4 TWO:** PASS. A named frame-1 refusal would read FIXTURE-CANNOT-EXERCISE.
- **G-COLL F-SYN:** a reading, with **P2-2 CONFIRMED**.
- **F-MW:**
  - The pixel legs are valid only on the pinned trajectory (about 91 % per attempt).
  - No prediction of judged / NOT-ADMITTED / NOT-ADMITTED-PARTIAL / INVALID-ARBITER.
  - **MW-FLOOR:** a reading.
- **S3O:** PASS (`host_mid:override`).
- **A2:** FIXTURE-CANNOT-EXERCISE (D), with its decision-frame reading.
- **S8:** PASS (`no_textures`).
- **At the end:** P2-1 and P2-3 CONFIRMED over every m53 leg, and G3-COUNT PASS.
