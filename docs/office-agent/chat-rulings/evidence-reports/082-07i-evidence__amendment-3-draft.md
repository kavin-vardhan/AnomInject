
## AMENDMENT 3 — 082-07i, 2026-09-28, declared before the resume (082-07j)

**Source:** `_reviews/082-07h-chat-ruling-t10-premise.md`, rulings 1–4.

**Bench-free.** No leg, build, cook, plugin source change or CaptureBench change was made. The build under test is unchanged
(A2.1: `2FCDF059` / utoc `20DA6F98` / ucas `534C5863`).

**What it carries:**

- T10's disposition (A3.1);
- the V1 predicate citation and declaration (A3.2);
- the time-axis review of every unrun row (A3.3);
- one encoding fix, the F-MW partial admission (A3.4);
- the MainWorld decision-frame reading (A3.5);
- supersede entry 4 (A3.6);
- one new finding from re-running the proofs (A3.7);
- the harness changes (A3.8);
- the proofs (A3.9);
- the resume plan and predictions for 082-07j (A3.10).

### A3.1 T10: FIXTURE-CANNOT-EXERCISE (premise), with the S2 debt

**Measured (082-07h):**

| read | frame | `T_TC_Stream2k` | tree result |
|---|---|---|---|
| G0_FSYN census, attempts 1–3 | 1 | `resident=7 max=12` | `not_fully_resident` |
| T10 decide | 18 | `resident=12 max=12` | `transformable` → `APPLY` |

- **The feature was correct.** It refuses only when `NumResidentLODs != MaxNumLODs` (`TexCorruptTree.cpp:466`). Its texture was
  fully resident by the decision frame, so it applied.
- **The row's premise was false on this bench:** "partly resident from the frame-1 census until apply" (G328).

**Disposition in S1:**

- The attempt `M53S1_REASON_T10_A1` and its halt row `G-REASON T10 FAIL` are superseded (A3.6), with their history kept.
- The row is re-declared **class D, FIXTURE-CANNOT-EXERCISE (premise), not PASS**:
  - it has **no legs** (`fn DECLARED-NOT-RUN`);
  - the first `evaluate_ready` of 082-07j records it before any launch.
- **No `REASON_T10` leg exists in S1** (`082-07-legs.py` `NOT_RUN`), so no TRIP, G11 or P2 row can wait on it.
- T10 stays in `REASONS`, so the G0 census expectation for `StaticMeshActor_74` uv stays `not_fully_resident`. That is the
  frame-1 truth.

**S2 debt: a full Q T10 row with a deterministic producer.** Per ruling 2:

- **No timing race.** Deciding before the lead-in is not acceptable.
- **No lever the tree files under another sub-reason.** `r.Streaming.MipBias` is T9S's `runtime_lod_bias`.
- **First establish why the streamer loads the full chain** for a far, off-screen plane (the cause read), then propose the
  producer with evidence.
- Candidates, none tested: an ample pool that never drops mips; the level-load streaming boost; the plane's screen size.

### A3.2 The V1 predicate: one function (ruling 2)

**Citation.** There is a single `EvaluateTree`, defined at `TexCorruptTree.cpp:736` and declared at `TexCorruptCore.h:290`.
It has two callers:

- **the census,** at `TexCorruptTree.cpp:1001-1007` (`In.bCensus = true`, then `EvaluateTree`, then `LogTree(…, "CENSUS")`);
- **the apply path,** at `Anomaly_TexCorrupt.cpp:657-668` (`EvaluateTree`, then `LogTree(…, "DECIDE" | "REFUSED")`; on refusal the
  `REFUSED … (step …)` line, and no fire).

`In.bCensus` is read **once**, at `TexCorruptTree.cpp:814`. It skips E6 (`mode_invalid`), which precedes every slot and binding
evaluation.

`not_fully_resident` is decided in `EvaluateBinding`, at `TexCorruptTree.cpp:460-470`:

- `MaxNumLODs != M` gives `max_below_cooked` or `max_above_cooked`;
- `NumResidentLODs != MaxNumLODs` gives `not_fully_resident`.

Both callers reach it through `EvaluateSlot` → `EvaluateBinding`.

**⇒ The G0 census and the apply path call the same V1 predicate.**

**Declaration: S1's evidence for `not_fully_resident` is exactly this:**

1. **The predicate, proven in-engine.** Three frame-1 G0 F-SYN censuses, on three builds (A1 utoc `9A26D497`, A2 `30FE0FDE`, A3 `20DA6F98`), read
   `T_TC_Stream2k resident=7 max=12` → `not_fully_resident` (V1). The frame-1 G0 F-MW census reads it on the MainWorld floor
   (48 of 102 bindings at 7/11–7/13).
2. **The refusal plumbing, proven by the other 14 G-REASON rows plus G6.** Each refused event runs `RefuseEvent` → the REFUSED
   line with its final and step → `texcorrupt_refused_<reason>` → no fire and no label. T9S shows it on the same actor.

**Not proven in S1:** a live fire refused *for this reason* end to end (a `REFUSED not_fully_resident` line with its
`texcorrupt_refused_not_fully_resident` counter). That is carried by the S2 T10 row.

### A3.3 The time-axis review of every unrun row (ruling 3)

**The question for each row:** does any premise rest on a state read earlier than the leg's own decision frame (residency,
streaming, async loads, level streaming, a frame-1 census admission)?

**The facts used:**

- **F-SYN textures.** From the A3 census binding lines, only `T_TC_Cine` (7/9) and `T_TC_Stream2k` (7/12) stream
  (`streams=1`). Every other F-SYN fixture texture is `streams=0` and fully resident at frame 1, and a non-streaming texture
  is loaded whole with its package.
- **The final across slots.** If any slot qualifies, the final is `APPLY`; otherwise the lowest-rank failing slot wins
  (`TexCorruptTree.cpp:866-903`, `TexCorruptPure.h:777`).
- **Ranks:** S1–S8 = 1–8; binding step k = 10+k (so `not_fully_resident` = 20); A2 = 31, A3 = 32, A5 = 35, A6 = 36.

| leg(s) | rows | premise read before the decision frame? | disposition |
|---|---|---|---|
| `REASON_A5` | G-REASON A5 (Q) | `below_size_policy` is decided after the binding checks (`TexCorruptTree.cpp:610`), so it needs full residency. `T_TC_Small32A/B` are 32×32, 6/6, `streams=0`, single slot. | **unaffected** |
| `REASON_A6` | G-REASON A6 (Q) | `map_set_over_cap` (`:615`), same position. `T_TC_Nine1–9` are 64×64, 7/7, `streams=0`, single slot. | **unaffected** |
| `REASON_V2` | G-REASON V2 (Q) | `T_TC_Big4kA/B` are 4096², 13/13, `streams=0`. The need of 201,326,588 B exceeds the cap of 134,217,728 B whatever is live (`Fits`). | **unaffected** |
| `G4_FIN/CAN/DES/END/GC/FR/FS2/FS2O/FS3/FS4/FS5/FS6_UV` (12) | G4 rows (Q) | See the ADMIT row below. Every G4 clause reads only the leg's own APPLIED / REVERT / LEDGER / LOST lines. The DestroyTarget latch counts the leg's own anomalous ticks (`AnomalyInjectorSubsystem.cpp:337-354`). END closes 1 s after the leg's own APPLIED line. CAN never decides. | **unaffected** |
| the same, `_NM` (12) | G4 rows (Q) | As UV, on `TC_NN1`. | **unaffected** |
| `G4_DES2_UV` | G4 DES2-UV (Q; P2-6) | As UV. NoApply 2 still runs the tree. | **unaffected** |
| `G4_TWO` | G4 TWO (Q) | The second fire, `IAI.Apply normal_corruption =StaticMeshActor_12`, decides at start-up: the census moment itself. | **unaffected** |
| `COLL_A_FSYN`, `COLL_N_FSYN` | G-COLL F-SYN (D) + P2-2 | The collateral set is taken **at apply**, from primitives rendered on screen within the window (`Anomaly_TexCorrupt.cpp:397-446`), so P2-2 reads the leg's own apply sample. **But** the §6 prediction "no deficit (every fixture texture is never-streamed)" rests on a false premise: two fixture textures stream. | **affected (prediction only).** A deficit is possible on a streamed texture from the two legs' streaming timing. Every deficit is reported by name (unchanged), and 082-07j reports whether each is `streams=1`. A deficit on a streamed texture is not attributed to the feature; on a never-streamed texture it keeps its meaning. D row, no encoding change. |
| `FMW_CTL_A`, `FMW_CTL_B`, `FMW_ID` | BAND F-MW (D), **G-ID-FMW (Q), G3 FMW (Q)** | Not pre-checked (A1.4). The floor reads uv `not_fully_resident` on 30/30 slots at frame 1 (48 of 102 bindings at 7/11–7/13), the same load-time state as `T_TC_Stream2k`. A1.8 covered all refused (`NOT-ADMITTED`) and all applied (judged). It **did not cover a partial yield**: three separately launched legs deciding at about frame 42 can differ in which of their 5 events applied. Under the pre-edit lib that gives G-ID-FMW `INVALID-PAIR` (halt 3), G3 FMW `FAIL` (halt 1) and, for a uniform partial yield under 3 events, `FAIL` on the event minimum (halt 3). All three were shown on synthetic legs. | **affected → encoding fix A3.4, proven both ways** |
| `REASON_S3O` | G-REASON S3O (Q) | The Bot's 3 of 3 slots read `host_mid:override:mid` at frame 1. `host_mid` (rank 3) is decided in `EvaluateSlot` (`TexCorruptTree.cpp:497-501`) before any binding or residency is read. No slot reaches the binding checks, so none can qualify as residency changes. | **unaffected.** The residual premise, that the host keeps its MID, is host-owned and not streamed. |
| `REASON_A2` | G-REASON A2 (D, DECLARED-CANNOT) | A1.8's argument "no_normal_map cannot be produced on it at any residency" was derived from the frame-1 reading. The structural part holds: 18 of 30 slots carry a normal-map binding (`:553`). The conclusion does not: once resident, those 18 either qualify (`APPLY`) or fail at A3/A5/A6 (32/35/36), and then the 3 `no_normal_map` slots (31) win. | **affected (argument only).** The declaration text is corrected in `082-07-legs.py`. The row stays class D (it cannot stop). Its decision-frame final is read by MW-FLOOR (A3.5). A `no_normal_map` there is a **finding**: a Q-producer candidate for S2. |
| `COLL_A_FMW`, `COLL_N_FMW` | G-COLL F-MW (D) | A reading. `NOT-ADMITTED` if the fire is refused. A partial yield leaves unpaired samples and `clean=false` (already a reading). | **unaffected (D).** Their decisions feed MW-FLOOR. |
| `REASON_S8` | G-REASON S8 (Q) | `no_textures` is decided when a slot has no bindings (`TexCorruptTree.cpp:538-541`), a material property. Cooked shader maps are complete at load (S5/S6 precede it). | **unaffected** |
| **The ADMIT pre-check** (A1.4) | every Q row above that admits (the 26 G4 rows, including DES2 and TWO) | It reads the frame-1 census A3. The only needs are `TC_UC1` uv and `TC_NN1` normal, both `APPLY`. Both are single-slot with `streams=0`, fully resident (8/8) textures. They applied at their decision frames on every event of `CTL_A`/`CTL_B`/`ID_UC1`/`WC_UC1_*` and of the N-N1 legs. **No admission can flip by the decision frame,** and no unrun admission reads a refusal that could turn into an `APPLY`. F-MW rows are not pre-checked, which is correct under G328. | **unaffected** |
| every leg | TRIP, G11, P2-1, P2-3, G3-COUNT | Each reads the leg's own run_summary, log or labels. | **unaffected** |

**Level streaming and async loads.** MainWorld is World Partition. The floor actor and the Bot both exist in the frame-1 census.
The census targets are resolved by name at the leg's own start. No other premise found.

**Rows that can only be exercised with a fixture or build change:** none among the unrun. T10 (A3.1) goes to S2.

### A3.4 The encoding fix: F-MW partial admission (G328)

**Scope:** F-MW only, in three evaluators: `ev_band` (BAND F-MW), `ev_gid_mw` (G-ID-FMW) and `ev_g3` (G3 FMW). BAND F-SYN
(PRE) and every F-SYN evaluator are untouched.

**Rule:**

- **When no compared leg logged a REFUSED line, the evaluation is byte-identical to before.** The new helper returns `None`.
- **When any compared leg refused,** events are keyed by their first `session_index`, and:
  - an event labelled differently in the legs that applied it stays `INVALID-PAIR` (halt 3);
  - a leg that lacks more events than it logged REFUSED lines stays `INVALID-PAIR`;
  - otherwise **only the events applied in every compared leg** are compared.
- **G-ID-FMW** needs ≥ 3 common events. Fewer reads **`NOT-ADMITTED-PARTIAL`**, a reading with no halt. The existing
  `NOT-ADMITTED` (the identity leg applied nothing) is unchanged.
- **G3 FMW** judges the pixel clause on the common events. With none, the pixel clause reads `NOT-ADMITTED-PARTIAL` and the log
  clause decides, as with `INVALID-ARBITER`.
- **BAND F-MW** is computed on the common events, so its whole-frame reading (which G3 needs) exists whenever the legs agree
  by refusal.
- **Every result carries a `partial` block:**
  - the refusal counts and finals per leg;
  - the union of events, the missing events per leg, and the common events;
  - any unexplained difference.

**Declared cost.** An event that any compared leg refused is excluded, so it is not judged. An error confined to such an event
is not caught; it is visible in `partial.missing`.

**Proof** (`082-07i-evidence/proof-082-07i.json`, 35/35). Each fix is shown against the pre-edit lib on the same synthetic
F-MW legs:

- **No refusal anywhere:** G-ID-FMW, G3 FMW and BAND F-MW are identical to the pre-edit output (the byte-inert case).
- **Identity leg refused event 1:**
  - pre-edit: `INVALID-PAIR` (halt);
  - now: judged on the 4 common events, PASS.
- **Refused 3 of 5:**
  - pre-edit: `INVALID-PAIR`;
  - now: `NOT-ADMITTED-PARTIAL`.
- **All three legs refused the same 3 events:**
  - pre-edit: `FAIL` on the minimum;
  - now: `NOT-ADMITTED-PARTIAL`.
- **A null leg refused one event:**
  - pre-edit: `INVALID-ARBITER`, unjudged;
  - now: judged, PASS.
- **Still caught:**
  - a missing event with no REFUSED line is `INVALID-PAIR`;
  - an event labelled differently is `INVALID-PAIR` even with a refusal present;
  - a 3-level error on a common event `FAIL`s;
  - a post-revert residue on a common event `FAIL`s G3.
- **The declared cost is shown:** an error only in a refused event is excluded and reported.

### A3.5 The MainWorld decision-frame reading (ruling 4)

- **Status of the finding.** A1.8's "route B's strict admission yields zero on StackOBot" is **downgraded to UNVERIFIED**. It
  rested on the frame-1 G0 F-MW reading.
- **What stands.** The rocks' `texture_not_parameter` is structural (binding step 4, `TexCorruptTree.cpp:390`, which precedes
  every residency check).

**Which unrun legs provide the reading:** all six decide on the floor, so the reading exists and nothing goes to S3 for lack
of it.

- `FMW_CTL_A`, `FMW_CTL_B`, `FMW_ID`, `COLL_A_FMW`, `COLL_N_FMW`: the uv family;
- `REASON_A2`: the normal family;
- plus `G0_FMW`, the frame-1 census, for comparison.

**How it is read:** a new row, **`MW-FLOOR decision-frame`, class D, verdict READING. It is read-only and never a gate.**

- **What it records, per leg:** every tree block the live tree logged for the floor, with its tag (DECIDE / REFUSED / CENSUS),
  family, engine frame mod 1000, final, slot and qualified counts, and binding residency (`resident/max`); plus the leg's
  APPLIED and REFUSED counts.
- **When it is evaluated:** after `COLL_N_FMW` and before `REASON_S8`.
- **Failure mode:** it catches every exception and returns READING with an error, so it can never raise `EVALUATOR-ERROR`.
- **Proven** in `proof-082-07i`:
  - a synthetic REFUSED → DECIDE sequence reads back exactly;
  - a broken input reads READING, not an error;
  - known answer: on the real frame-1 G0 F-MW census it reads uv `not_fully_resident`, 30 slots, 48 of 102 bindings not
    resident.
- **Next:** chat corrects the owner-facing yield statement once this reading exists.

### A3.6 The supersede record, entry 4

- **Record:** `_reviews/082-07-evidence/run/supersede.json`, sha256 `ff7f884f…`.
- **Attempt:** `M53S1_REASON_T10_A1`.
- **Row:** `G-REASON T10 FAIL` (2026-09-27T22:01:01Z), which is also the superseded halt.
- **Never evaluated:** TRIP and G11 `REASON_T10`. `evaluate_ready` stopped at the halt first (G319), and they no longer exist.
- **Alias:** `M53S1_REASON_T10` → `M53S1_REASON_T10__SUPERSEDED_082-07i` (170 files). Hardlinks were verified before and after,
  and nothing was deleted.
- **Ledger:** a `*RESUME*` RULING line.
- **After:**
  - no recorded halt;
  - 154 live accepted legs and 39 unaccepted, the first being `REASON_A5`;
  - the only ready row is `G-REASON T10` (class D).
- **The pre-act state is kept** in `082-07i-evidence/run-before-supersede/`.

### A3.7 Finding: the tonemapper-dither premise (G318) has an exception on grazing / transition geometry

- **What P5a found.** Re-running the 082-07g proofs on the final lib, check P5a now reads **10 phase-matched differences in 991
  comparisons**. It scans every banked F-SYN identity, null and control event, and now includes 082-07h's legs. Before, it read 0.
- **Where they are.** All 10 are on the ChainD grazing and transition tiles, uv and normal:
  - `ID_MD_GRAZE`, `NULL_MD_GRAZE`, `ID_MD_GRAZE_NM`, `NULL_MD_GRAZE_NM`;
  - `ID_MD_TRANS` (×2), `NULL_MD_TRANS`, `ID_MD_TRANS_NM` (×2), `NULL_MD_TRANS_NM`.
- **Size:** 1–2 px at max |d| **1**, in the NoApply nulls as well. So it is a property of the scene or renderer on that geometry,
  not of the feature.
- **Independent of this amendment:** the pre-edit lib reads the same 10.
- **What it does not change:**
  - The recorded G-RD rows (`RD-OPAQUE`, `RD-ALPHA`, head-on tiles) read 0 on their own legs and stand; no-flip is 0.
  - G-ID-M on those rows passed within its ≤ 2 tolerance (M-D-TRANS max 1).
  - No unrun row uses the G-RD drift rule.
- **Where it goes.** The general premise "phase-matched frames are identical" does **not** hold on grazing or transition
  geometry. It joins the S2 cause-read list, with the halo and the 8-level gradient. No mechanism is asserted and no period is
  established; a mod-16 partner does not exist inside these 8-frame events.

### A3.8 Harness changes carried by the re-issued boundary

**`082-07-legs.py`:**

- `NOT_RUN = {'T10': …}`, and T10's leg is not generated;
- A2's declaration text is corrected (A3.3);
- 194 → **193 legs**.

**`082-07-lib.py`:**

- `EvView` and `fmw_partial` (A3.4);
- the F-MW branch in `ev_band`, `ev_gid_mw` and `ev_g3`;
- `floor_decisions` and `ev_mw_floor`, with `EVAL['MW-FLOOR']` (A3.5);
- gate rows: G-REASON T10 as `DECLARED-NOT-RUN` (D, no legs), and MW-FLOOR (D);
- `run_gate` handles `DECLARED-NOT-RUN`;
- 549 → **548 gate rows**: −TRIP and −G11 `REASON_T10`, +MW-FLOOR.

**Other:**

- `run/supersede.json`: entry 4.
- **Unchanged:** `082-07-common.py`, the runner, the preflight, the postflight, the window, and the build.

### A3.9 Proofs, no-flip and dry run (counts in the 082-07i journal and report)

- the new suite `proof-082-07i`;
- every earlier suite re-run on the final lib (from copies in `082-07i-evidence/prep/`, so 082-07g's evidence is not
  overwritten);
- no-flip before and after the supersede act;
- the dry run before and after the lock;
- the stubbed end-to-end window and the preflight negatives after the lock.

### A3.10 Resume plan and predictions for 082-07j

**Order:**

1. The first `evaluate_ready` records `G-REASON T10` FIXTURE-CANNOT-EXERCISE (class D, no stop).
2. Then the 39 legs, in the declared order:
   - `REASON_A5`, `REASON_A6`, `REASON_V2`;
   - the 12 G4 `_UV` legs, then the 12 `_NM` legs, then `G4_DES2_UV`, `G4_TWO`;
   - `COLL_A_FSYN`, `COLL_N_FSYN`;
   - `FMW_CTL_A`, `FMW_CTL_B`, `FMW_ID`, `REASON_S3O`, `REASON_A2`, `COLL_A_FMW`, `COLL_N_FMW`;
   - `REASON_S8`.

No decided leg is re-launched, and `REASON_T10` is not a leg.

**Predictions:**

- **A5, A6, V2:** PASS with their census finals (`below_size_policy:min_64`, `map_set_over_cap:9_of_8`, `over_budget`).
- **G4:** every row PASS.
  - **P2-5:** CONFIRMED on the FS rows, including `FS2O` (created, then rejected), with the F+2 reading `live=0 pending=0
    balanced`.
  - **P2-6:** CONFIRMED on DES2 (`TARGET LOST` ends the NoApply 2 event; the apply→lost offsets are equal).
  - **GC:** may read `UNEXERCISED` if no GC pass falls inside the first event. That verdict is declared in the evaluator.
- **G-COLL F-SYN:** a reading, with **P2-2 CONFIRMED** (`T_TC_UC2` in the set, `T_TC_UC1` not, `null_slots ≥ 1`). Any deficit is
  reported with its streamed flag (A3.3).
- **F-MW:** **no prediction** of NOT-ADMITTED, NOT-ADMITTED-PARTIAL or judged. If judged, G-ID-FMW identity ≤ 2, unless the band
  reads `INVALID-ARBITER` (moving platforms).
- **MW-FLOOR:** a reading.
- **S3O:** PASS with `host_mid:override`.
- **A2:** FIXTURE-CANNOT-EXERCISE (D), with its decision-frame reading.
- **S8:** PASS with `no_textures`.
- **At the end:** P2-1 and P2-3 CONFIRMED over every m53 leg, and G3-COUNT PASS.
