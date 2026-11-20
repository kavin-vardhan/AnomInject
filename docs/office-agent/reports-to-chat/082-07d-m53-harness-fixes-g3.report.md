# REPORT for 082-07d-m53-harness-fixes-g3.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 082-07d-m53-harness-fixes-g3.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-27 10:26:19 | finished: 2026-09-27 11:15:15 | duration: 49 min | exit code: 0
context at last turn: 675028 tokens (68% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\082-07d-m53-harness-fixes-g3.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN

The two monitor-expiry notices are stale watchers left over from the end-to-end proof. They stayed silent because PowerShell's `>` redirect writes UTF-16, which their `grep` could not match. The proof itself was read directly from the console file and passed 10 of 10.

**Plain summary for the owner**
- The test harness is fixed and the next leg window (082-07e) is ready. The launch script that crashed in 082-07b now runs start to finish, proved with the game launch stubbed out.
- The big "before vs after" picture change that confused G3 is the engine's own anti-banding noise: the image changes by one brightness level from frame to frame on about 40 % of pixels. G3 now compares corrupted and uncorrupted runs at the same frame instead, and all 11 already-banked UV rows pass with zero difference.
- Nothing ran on the bench and nothing was built or cooked. The staged build is unchanged.

**3. Window fix and the end-to-end proof**
- **Fix:** `082-07-window.py` now calls `L.budget_state()['left_s']`. The file is byte-identical to the one-line driver chat ratified (both `0C18E9A3…`). The driver copy is retired as `082-07b-window-driver.py.RETIRED-082-07d`, bytes kept.
- **Proof** (`082-07d-evidence/prep/e2e-window.py`): the real `082-07-window.py` was run as a subprocess with the launch stubbed. The stub mode is refused if pointed at the live run dir. **10 of 10 checks pass:**
  - **V1, resume state as locked:** preflight clean against the real boundary, then quiet, person gate, run, postflight PASS, exit 2. It ran `G0_FSYN_A2` (accepted), admitted U-N1, then stub-launched `ID_UN1` as attempts A2–A4, which went NOT-RUN because a stub leg has no session.
  - **V2, the 082-07b census as the "cannot" case:** exit **12**, FIXTURE-CANNOT-EXERCISE before `ID_NN1` launches ("StaticMeshActor_12 normal read normal_unconnected"). Only `G0_FSYN` was launched.
  - **V3, spent wait budget:** exit 4, NOT-QUIET, before any wait or leg.
  - The live ledger, `gates.json`, supersede record, bank listing and exe-side dir are byte-for-byte unchanged.
- **Negatives:** stub mode aimed at the live dir is refused by both the preflight and the window, and nothing is written.

**4. Admission pre-check and G0 normal coverage**
- **The rule:** before the first leg of any F-SYN Q row launches, every (target, family) it needs must read `APPLY` in the current build's G0 census. That covers 116 rows: G-ID, G-ID-M, G3, G-RD, G-BIND (3), G4 for both ids, and G-REASON A1.
  - If a target reads anything else, the harness writes an `ADMIT` row and stops with **FIXTURE-CANNOT-EXERCISE, code 12**. It prints the census reason and does not call it a feature FAIL.
  - With no census it stops as NO-CENSUS, code 3.
  - Refusal rows and F-MW rows are not pre-checked: G0 F-MW is read on the first frame, before MainWorld has finished loading.
- **Proof both ways** (`prep/proof-082-07d.py`, 37 of 37 pass):
  - On the real 082-07b census: all 23 normal-family rows are refused and all 93 uv-only rows are admitted.
  - On synthetic output (the 7 normal targets set to `APPLY`): all 116 are admitted.
  - A uv target flipped to refused is caught.
  - In the sequencer, the stop comes before the leg launches and a resume stays stopped.
- **G0 normal coverage:** both families are now compared on all 77 targets.
  - On the 082-07b census, the normal-family findings are exactly the 7 normal-map targets (predicted `APPLY`, read `normal_unconnected`), with 0 uv findings.
  - The synthetic fixed census gives 0 findings, and single flips are caught.
- **The other proof suites, re-run on the final lib:** evaluators 209 of 209 (with G3 cases rewritten to the new rule), sequencer 20 of 20, disk 15 of 15, post-lock dry run 194 legs / 548 rows / 0 problems, and preflight negatives 7 of 7.

**5. G3: the new rule, the 12 UV rows, and the cause**
- **Rule:** take each event's first post-revert frame in the applied leg and the null's frame at the same `session_index`. Max |d| must be ≤ 2 inside the union of both legs' event masks and over the whole frame. The old pre-onset vs post-revert count is kept as a diagnostic only.
- **The 11 UV rows (U-C1…U-D6):** re-evaluated from the bank. **All PASS**, with region max **0** and whole-frame max **0** on 7 judged events each. The 8th event in each leg is cut by the frame cap and has no post-revert frame. The locked lib wrote them to `gates.json` after the lock, and they match the read-only run exactly. No failure, so no stop.
- **U-N1:** superseded, and it re-runs in 082-07e. Its banked result also passes the new rule, but it is kept as history only.
- **Cause read for 363,001, named and measured:** UE 5.1's tonemapper grain quantization (`r.Tonemapper.GrainQuantization`, on by default). It adds a ±½-level dither seeded by the view's frame index mod 8 (`PostProcessTonemap.cpp:637-643`).
  - max |d| is 1 on every changed pixel;
  - every event's before and after frames are 11 engine frames apart (11 mod 8 = 3), which is why the count is the same everywhere;
  - from frame 23 on, frames whose engine frame numbers differ by a multiple of 8 are bit-identical;
  - applied vs null is 0 over the whole frame on all 100 frames.
- **A second, small start-up element, cause not established:** up to ~316 px, up to 3–4 levels, on the reason-producer tile row in the first ~22 frames only.

**6. Predictions amendment** (AMENDMENT 1, committed before the lock)
- **Build under test:** 082-07c's (utoc `30FE0FDE`).
- **Normal rows' contradiction:** resolved by the fixture fix. `normal_unconnected` stays unexercised in S1, and its 082-07b readings are history of the superseded build.
- **Rules:** the G0 normal table, the admission pre-check and the new G3 rule, each with its cause read.
- **MainWorld yield reading:** recorded as a finding (floor `not_fully_resident`, rocks `texture_not_parameter`); the F-MW rows stay diagnostic.
- **Resume plan:** 082-07e runs G0_FSYN first, then the U-N1 re-run, then N-N1 onward.
  - The recorded stop is kept as history in `run/supersede.json`, which the boundary locks.
  - 7 attempts and 28 gate rows are superseded, and 7 bank aliases are renamed `…__SUPERSEDED_082-07d`. Nothing is deleted.
- **Across builds:** the four normal tiles are now lit, so 082-07b frames of those tiles are not compared with the new build. G-ID and G3 compare within a build and are unaffected.
- **Commit:** `202ae72` on `feat/m53-uv-normal-corruption`, pushed (local = origin), with the journal, status block and gotchas G318–G320. `m51` is still at `53bf725`, `master` at `b5f15a3`. No tag.

**7. Boundary and the 082-07e invocation**
- **Boundary:** `082-07-evidence/boundary.json` `4b73aa2e…`. It locks:
  - head `202ae72`;
  - predictions `ae1e6023…`;
  - lib `65bfd5b3`;
  - `supersede.json` `1a6abc46`;
  - the staged build: exe `2FCDF059`, utoc `30FE0FDE`, ucas `11231356`, pak `FD766B7B`.
- **Kept:** the 082-07a boundary, as `boundary-082-07a.json`.
- **Checks:** the preflight replay passed right after the lock and again at the end, with no problems and a quiet bench.
- **082-07e invocation:**
  ```
  C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\082-07-window.py 082-07e 540
  ```
  - Do not set `IAI_R53_ROOT` or `IAI_R53_STUB_LAUNCH`.
  - The run can be resumed: re-run the same command, with a new tag (for example `082-07e-2`) once the tag's 45-minute wait budget is spent.

**Two other things found and fixed**
- **Alias error:** a bank-alias error in `run_leg` escaped as an uncaught exception. It would have crashed the window before the postflight. It now stops with code 6 (`INVALID-BANK`), and that path is proven.
- **Unevaluated rows:** TRIP and G11 for `NULL_NN1` were never evaluated in 082-07b, because the halt stopped evaluation first. They re-run with that leg in 082-07e.

**8. NEEDS-DECISION** (not blocking; my defaults are adopted and locked, and changing one means re-issuing the boundary)
1. **REASON A2 is now diagnostic instead of a stop row.** Its premise, that the MainWorld floor has no normal map, is disproved by the source (`TexCorruptTree.cpp:566-574`) and by the G0 reading: the floor carries a connected normal map. So `no_normal_map` can't be produced on it, and the leg now only records a reading. This is the one stop-row change I made. The consequence is that `no_normal_map` has no stopping test in S1; only the G0 census reads it. Should a synthetic-scene A2 test be added later, for example normal corruption on `TC_UC1`?
2. **F-MW rows are left out of the admission pre-check** (G0 F-MW is a first-frame reading), and the F-MW G3 row is now judged when its control pair differs by ≤ 2 over the whole frame, not only at exactly 0. Please confirm both.
api-equivalent cost (not billed on Max): $20.83 | turns: 1 | running total across all briefs: $2,709.89
