# REPORT for 082-07a-m53-s1-leg-harness.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 082-07a-m53-s1-leg-harness.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-27 06:39:28 | finished: 2026-09-27 07:44:35 | duration: 65 min | exit code: 0
context at last turn: 892221 tokens (89% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\082-07a-m53-s1-leg-harness.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN

**Summary for the owner.** The harness for the m53 S1 gate legs is built and locked, and every gate's rules are written down before any leg runs. Every checker was tested offline: each one passes a correct synthetic case and fails a deliberately broken one, including the real fault patterns. The next brief (082-07b) runs about 2 hours of legs. Nothing was launched here: no game, no leg, no build, no tag.

**Gates** (194 legs, 548 gate rows; predictions in `docs/predictions/2026-09-27-m53-s1-legs.md`, committed as `7dc666e`). No S1 gate asserts per-frame alignment (G1 is S2), so every leg runs native tick order.

| gate | class | legs | rule | minimum | stop row |
|---|---|---|---|---|---|
| FIXTURE-NAMES | PRE | G0_FSYN (`IAI.ListActors`) | all 77 fixture actor names exist in the cooked level | 77 | code 3 |
| LUMA | Q | CTL_A, first frame | mean luma > 0 and non-zero pixels > 0 | 1 frame | 13 |
| G11 | Q | every leg's log | `TEXCORRUPT-ASSETS` shows all 3 assets resolved=1 | every leg | 10 |
| TRIP | Q | every leg | `texcorrupt_rt_mip_mismatch` = 0 | every leg | 5 |
| BAND | PRE (F-SYN) / D (F-MW) | two NoApply-1 legs per fixture | max difference ≤ 2 inside the target mask | 100 frames | code 3 |
| G-BIND (1) | Q | none (static check) | Build.cs dependency set as declared; G10 exited 0 with no warnings; staged exe = G10 archive `2FCDF059` | once | 2 |
| G-BIND (2), (3) | Q | alias leg; layer tile-probe/identity pairs | alias refused `texture_not_parameter`; the global half changes by ≤ 2, the layer half by ≥ 16, nothing outside the target changes; identity passes G-ID | 3 events | 2 |
| G-ID | Q | 14 rows, 53 legs | identity vs its matched null: max ≤ 2 on every measured frame; each assigned wrong copy ≥ 16 on every measured frame | 3 events per row | 3 |
| G-ID-M | Q (+6 D) | 29 rows, 73 legs | same rule; texelshift, mipshift, mipgen and chanswap where assigned | 3 events per row | 4 |
| G-RD | Q | 6 legs | redraw frames within ±2 of the null; frame N = frame 1 exactly; noclear ≥ 16 on the last frame | one event of ≥ 90 frames | 6 |
| G3 | Q | every identity/null pair, plus a count check | post-revert frame matches the pre-onset frame (whole frame, within the null's band); exact restore; ledger balanced at revert+2 | 3 events per id | 7 |
| G4 | Q | 26 legs | FinishRun, cancel, destroy, world-end close, GC, foreign material, FailStep 2/2+1/3/4/5/6 (read at F+2), NoApply-2 watches, both ids live. Level change and re-created component are UNEXERCISED | each exit once per id | 8 |
| G-REASON / G6 | Q | 22 legs | each producer is refused with its §R6.4 reason and step, and no other refusal counter moves | 1 each | 9 |
| G-COLL | D | 2 pairs | per-texture residency deficit (null − applied), never subtracted counts; any incomplete sample is flagged | every event | — |
| G0 | D | 2 census legs | census reasons vs §R6.4 / §R14.1; disagreements reported as findings | — | — |
| P2-1, P2-2, P2-3 | PROOF | all m53 legs / G-COLL F-SYN | condition read-back, collateral set, subtype | — | not a stop; reported |

**Evaluator proofs** (207 checks, 0 failed; plus the sequencer at 20/0).

| evaluator | good case → PASS | bad case → FAIL |
|---|---|---|
| G-ID / wrong copy | identity ±1 on all 30 offline-proven pairs, built from the fixture's own images | each fault picture fails G-ID and reads wrong (≥ 16 per frame); an invisible fault reads instrument-invalid |
| (fault values) | chanswap 120, srgbtwice 73, texelshift 128/89 match the manifest exactly | noclear-alpha reads 46 against the manifest's 51, because my synthetic readout models only the alpha term; still ≥ 16 |
| G-ID also | — | a +3 on one frame fails; 2 events fails; a shifted label schedule is INVALID-PAIR; no masks fails |
| G-ID row U-C2 | the row passes with its 3 wrong copies | an invisible alpha fault fails the row |
| BAND | an identical null pair is VALID | a 3-level difference is INVALID-ARBITER |
| G3 | pixels and log clean | left-to-game, pending, restored name, override length and a changed post-revert frame each fail |
| G-RD | passes | a 1-level drift, a noclear that doesn't show, and an event under 90 frames each fail |
| G-BIND (1), (3) | pass | an extra module, a warning line, the global half changing and the layer half unchanged each fail |
| G-REASON / G6 | T4, E4, S1, A1, S3A pass | an earlier reason, a stray counter, the wrong step, a missing disposition, and the MID left in the slot each fail |
| G4 (every kind) | FIN, CAN, DES, DES2, END, GC, FR, FS2O, TWO pass | each has at least one failing variant; GC with no garbage-collection pass reads UNEXERCISED |
| G-COLL / P2-2 | deficit 1 correctly paired; CONFIRMED | target texture in its own set, or no null slot → NOT-CONFIRMED |
| LUMA, G11, TRIP, FIXTURE-NAMES, P2-1, P2-3, validity | pass | black frame, unresolved asset, mismatch = 1, missing actor, wrong condition, wrong subtype, focus/pose/zero-byte/no-match each fail |

The synthetic log lines are generated from the plugin's own log format strings, so the parsers are checked against the real line formats.

**Disk-policy proofs** (15/15, with the bank on E: reached through a D: junction, as the real bank is):
- **Item 1:**
  - The exe-side copy is deleted only after a file-by-file check (path, size, SHA-256); sibling folders are untouched.
  - A one-byte corruption in the bank copy is caught and the exe-side copy is kept.
  - An existing bank folder without a verified record is never overwritten.
  - Resuming is safe to repeat.
- **Item 2:**
  - Aliases are NTFS hardlinks (same file ID, link count ≥ 2, same manifest).
  - An alias that is a copy is refused.
  - A cross-volume link raises an error; there is no copy fallback.

**Boundary and next step**
- `_reviews/082-07-evidence/boundary.json` (`d936c032…`) pins:
  - feature head `7dc666e` (equal to origin);
  - the predictions `87930fdd…`;
  - 8 harness files;
  - the staged `2FCDF059` + `9A26D497`/`10D7F0C0`/`ABD931A2` + `462B8AC6`/`BB05CF99`;
  - the fixture inputs.
- Checks against the lock:
  - The dry run after locking is clean: 194 legs, 0 problems, and all 36 console commands are present in the staged exe.
  - The offline preflight replay exits 0.
  - Three tampered copies of the boundary are each refused, and the unchanged control passes.
- Repo state: `m51` is untouched at `53bf725`; `master` equals origin at `b5f15a3`; no tag.
- **082-07b:** `C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\082-07-window.py 082-07b 540`
  - About 2 hours of legs (123 minutes estimated) plus waits for a quiet PC.
  - It can resume: run the same command again, using a new budget tag once the wait budget is spent.
- Any commit on the branch before 082-07b needs the boundary re-issued first.

**NEEDS-DECISION** — for chat's review. None blocks the lock; each was fixed in writing before any leg (predictions §9):
1. **Forced GC** uses the engine's own purge timer, because a packaged unattended leg only accepts console commands at startup. The row reads UNEXERCISED if no GC pass lands inside the event.
2. **Level change** and **re-created component → swept** are UNEXERCISED. The level-change lever only works on `CB_GateLevel`, and S1 has no lever that re-creates a component. A graceful window close exercises the same world-teardown callback.
3. **The new fixture level has no camera-rotation guard (A47).** The input-lock lever is restricted to `CB_GateLevel`/`L_ShooterGym` (recorded as G313). Legs whose pixels are compared must therefore settle at rotation (0,0,0), with a separate budget of 6 attempts that fail on pose alone. The discard rate is unknown and will be reported.
4. **The G-ID region** is the union of both legs' target masks, and a wrong copy must read ≥ 16 on every measured frame, which is stricter than the plan's wording.
5. **Chain (d) rows** run through both ids.
6. **The MainWorld G-ID row** is judged only if its own null pair agrees within 2 levels. Moving platforms may make it INVALID-ARBITER, which is reported but does not stop the run.

Separately, several MCP connectors (Figma, Slack, GitHub and others) need authorizing via claude.ai settings or `/mcp`; this brief used none of them.
api-equivalent cost (not billed on Max): $30.01 | turns: 180 | running total across all briefs: $2,670.15
