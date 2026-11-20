# REPORT for 082-07i-m53-time-axis-amendment-3.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 082-07i-m53-time-axis-amendment-3.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-28 03:44:51 | finished: 2026-09-28 05:03:21 | duration: 78 min | exit code: 0
context at last turn: 713939 tokens (71% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\082-07i-m53-time-axis-amendment-3.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN

AMENDMENT 3 is committed and pushed, the boundary is re-issued, and 082-07j is ready to run the last 39 legs from `REASON_A5`. None of the stop conditions fired. One earlier real-data check now fails for a reason unrelated to this work; it is a finding for your evidence review (sections 6 and 11) and blocks nothing.

**2. Plain summary for the owner**
- I went through every test still to run and asked whether it relies on something read too early. One family would have falsely stopped the next run: the MainWorld floor tests, where separate runs can load textures at different moments. They are fixed and proven.
- The T10 test is set aside as "the test couldn't exercise this" and moves to S2, with history kept.
- The harness is re-locked; the next run takes about 20–30 minutes plus waits.

**3. Time-axis review** (the full table is §A3.3 of the amendment)

| legs | rows | earlier-read premise? | disposition |
|---|---|---|---|
| `REASON_A5`, `A6`, `V2` | G-REASON (Q) | Single-slot, non-streaming, fully resident at frame 1 (Small 6/6, Nine 7/7, Big 13/13). V2's need of 201 MB exceeds the 134 MB cap whatever is live. | unaffected |
| 12 G4 `_UV` + 12 `_NM`, `DES2_UV`, `TWO` | G4 (Q), P2-5, P2-6 | Every clause reads the leg's own lines. DestroyTarget is event-relative (`AnomalyInjectorSubsystem.cpp:337-354`). TWO's second fire decides at start-up, the census moment itself. | unaffected |
| (the ADMIT pre-check) | the 26 G4 admissions | It reads the frame-1 census, and only TC_UC1 uv and TC_NN1 normal: `APPLY`, single slot, `streams=0`, 8/8, and they applied at their decision frames in earlier legs. No admission can flip. F-MW is not pre-checked, which is correct. | unaffected |
| `COLL_A/N_FSYN` | G-COLL F-SYN (D), P2-2 | The set is taken at apply time from on-screen primitives (`Anomaly_TexCorrupt.cpp:397-446`). But the prediction "every fixture texture is never-streamed" is false: `T_TC_Cine` and `T_TC_Stream2k` stream. | affected (prediction text only): a deficit on a streamed texture is not attributed to the feature |
| `FMW_CTL_A/B`, `FMW_ID` | BAND F-MW (D), **G-ID-FMW (Q), G3 FMW (Q)** | The floor is at 7/11–7/13 at frame 1 (48 of 102 bindings), and the legs decide at about frame 42 in separate processes. A partial yield would halt: `INVALID-PAIR` (code 3) or `FAIL` (code 1). Shown on synthetic legs with the pre-edit lib. | **affected → encoding fix (section 5)** |
| `REASON_S3O` | G-REASON (Q) | All 3 of the Bot's slots are `host_mid`, decided at slot level (`TexCorruptTree.cpp:497-501`) before any residency read. | unaffected |
| `REASON_A2` | G-REASON A2 (D) | A1.8's "cannot at any residency" came from the frame-1 state. Once resident, the 18 normal-map slots either qualify or fail at rank 32/35/36, and then `no_normal_map` (31) wins. | affected (argument only): text corrected, stays class D, read by the new floor reading |
| `COLL_A/N_FMW` | G-COLL F-MW (D) | A reading. | unaffected |
| `REASON_S8` | G-REASON (Q) | No bindings, a material property (`:538-541`). | unaffected |
| all | TRIP, G11, P2-1, P2-3, G3-COUNT | Each reads the leg's own output. | unaffected |

No unrun row needs a fixture or build change.

**4. V1 citation and declaration**
- **Same predicate.** There is a single `EvaluateTree` (`TexCorruptTree.cpp:736`). The census calls it at `:1001-1007` and the apply path at `Anomaly_TexCorrupt.cpp:657-668`.
- **The only census-specific branch** is `:814`, which skips the mode check. It runs before any slot or binding is evaluated.
- **`not_fully_resident`** is decided in `EvaluateBinding` (`:460-470`), reached the same way from both callers.
- **Declared evidence:**
  - the predicate, proven by three frame-1 censuses on three builds (`resident=7 max=12` → `not_fully_resident`);
  - the refusal plumbing, proven by the other 14 G-REASON rows plus G6.
- **Not proven in S1:** a live fire refused for this reason end to end. That is owed with the S2 T10 row.

**5. AMENDMENT 3** (in `570352e`, §A3.1–A3.10)
- **T10:** declared class D FIXTURE-CANNOT-EXERCISE (premise), with no leg (`NOT_RUN`).
  - T10 stays in `REASONS`, so the census expectation is unchanged.
  - **S2 debt:** a deterministic producer, after a cause read of why the full mip chain loads.
- **The F-MW encoding fix:**
  - When any compared leg refused an event, only the events every leg applied are compared.
  - Each leg must account for its missing events with its own REFUSED lines; otherwise the row stays `INVALID-PAIR` and halts.
  - Fewer than 3 common events reads `NOT-ADMITTED-PARTIAL`, a reading.
  - With no refusals the output is byte-identical to before.
  - **Declared cost:** an event that any leg refused is not judged.
- **MainWorld reading:** a new row, `MW-FLOOR` (class D, read-only, never a gate, cannot raise an evaluator error).
  - All six floor legs provide it, so nothing goes to S3.
  - The "zero yield" finding is now UNVERIFIED.
  - The rocks' `texture_not_parameter` stands.
- **Predictions for 082-07j:** A5, A6, V2, S8, S3O and G4 PASS; P2-2, P2-5 and P2-6 CONFIRMED; no prediction for the F-MW outcome.
- **Totals:** 194 → 193 legs; 549 → 548 rows.

**6. Proofs, no-flip, dry run** (final lib `b1e0cdcd`, legs `84a423a6`)

| suite | result |
|---|---|
| `proof-082-07i` (new, both ways against the pre-edit lib) | 35/35 |
| evaluators | 210/210 |
| admission (`proof-082-07d`) | 37/37 |
| sequencer | 20/20 |
| disk | 15/15 |
| AMENDMENT 2 (`proof-082-07g`) | **32/33** |
| no-flip before the supersede act | 497/497, 0 flips |
| no-flip after the supersede act | 496/496, 0 flips |
| dry run, pre- and post-lock | 193 legs, 548 rows, 0 problems |
| end-to-end window, stubbed launch | 8/8 |
| preflight negatives | 13 cases refused, control passes, both stub refusals |

- **`proof-082-07d`:** its first run was 36/37. The corrected A2 text had dropped the `:566-574` citation the check requires; it was added back.
- **The one AMENDMENT 2 BAD is check P5a (§A3.7).** Its scan of the whole bank now includes 082-07h's legs. It finds:
  - 10 of 991 same-phase frame comparisons differing, at max |d| 1 on 1–2 px;
  - all on the ChainD grazing/transition tiles, in the NoApply nulls too;
  - the same 10 under the pre-edit lib.

  It is new evidence against the "phase-matched frames are identical" premise, not a regression. The recorded G-RD rows read 0 and stand.
- **End-to-end window:** it first records only the T10 row, then launches `REASON_A5`, and no decided leg. Without supersede entry 4 it stays stopped with code 1.
- The suites ran from copies in `082-07i-evidence\prep`, so 082-07g's evidence is intact.

**7. Supersede record, entry 4**
- **Record:** `ff7f884f`.
- **Superseded:** the attempt `M53S1_REASON_T10_A1` and the halt row `G-REASON T10 FAIL`.
- **Never evaluated:** TRIP and G11 `REASON_T10`.
- **Alias:** renamed to `…__SUPERSEDED_082-07i`, 170 hardlinks verified; nothing deleted.
- **After:** no halt; 154 legs accepted, 39 unaccepted.

**8. Boundary and preflight replay**
- **Boundary `456f4146`** replaces 082-07g's `6520d84d`, which is kept aside.
  - It locks head `570352e`, predictions `e0cddf9a`, lib `b1e0cdcd`, legs `84a423a6` and supersede `ff7f884f`.
  - Staged build `2FCDF059` / `20DA6F98` / `534C5863`, unchanged.
- **Preflight** right after the lock: 0 problems, bench quiet. It is also clean after all the post-lock suites.

**9. The 082-07j invocation**
```
C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\082-07-window.py 082-07j 540
```
- Don't set `IAI_R53_ROOT` or `IAI_R53_STUB_LAUNCH`. If the 45-minute wait budget runs out, use tag `082-07j-2`.
- It resumes at `REASON_A5`, runs only the 39 unrun legs, and re-launches no decided leg; the dry-run plan and the end-to-end window both show this.
- **Expected time:** the estimator says 27 min; 082-07h measured about 20 s per leg. F-MW legs take longer, and waits come on top.

**10. Commits pushed**
- `570352e` on `feat/m53-uv-normal-corruption` (local = origin). It carries AMENDMENT 3, the journal, the status block, and gotchas G329–G331.
- **Also in that commit:** the 082-07h G328 gotcha and journal tail (both from `f9d6710`) are repaired in place. A PowerShell here-string had turned their code spans into a newline, two CRs and a NUL (G331).
- No tag. `m51` is still `53bf725`, `master` `b5f15a3`.

**11. NEEDS-DECISION**
- None blocking 082-07j.
- For your evidence review:
  - P5a / §A3.7, the exception to the frame-dither premise;
  - the `MW-FLOOR` reading, once 082-07j produces it, for the owner-facing yield statement.
api-equivalent cost (not billed on Max): $26.28 | turns: 103 | running total across all briefs: $2,790.79
