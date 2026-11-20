# REPORT for 082-07h-m53-s1-legs-final.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 082-07h-m53-s1-legs-final.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: high
started: 2026-09-28 02:56:43 | finished: 2026-09-28 03:39:30 | duration: 43 min | exit code: 0
context at last turn: 415174 tokens (42% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\082-07h-m53-s1-legs-final.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

S1 stopped partway, at `G-REASON T10` (stop row 9): the leg applied the corruption where it was expected to refuse. The logs point at the test setup rather than the feature, but that call is chat's. Every other leg that ran passed or gave its declared reading. 39 legs never ran.

**2. Plain summary for the owner**
- **Identical copy:** passed on every row that ran, including the fixed masked tile. Every identity check reads a difference of 0, or at most 1 on four mip rows (the limit is 2).
- **Left behind after revert:** nothing, on all 43 revert checks.
- **Refusals:** 14 of 15 refused correctly. The exception is T10: its test texture had finished loading by the time the feature looked, so the feature correctly went ahead. The test assumed the texture would still be partly loaded.
- The remaining 39 tests did not run, so the rollback, watch and collateral proofs are still outstanding.

**3. S1 gate table across 082-07b, 07e and 07h** (live rows only)

| gate | class | runs | result | key reading |
|---|---|---|---|---|
| G-ID, 14 encoding rows | Q | 07b 11, 07e 2, 07h 1 | 14 PASS | max \|d\| 0 on every row, 57 frames each (wrong-copy minimums below) |
| PRE-CLIP MASKED | PRE | 07h | PASS | mask 2,048 px on 57/57 frames; 0 px outside the sky range |
| G-ID-M (29 rows incl. SHARED) | Q | 07h | 29 PASS | max 0; M-D-MIN, -MIN_NM, -TRANS, -TRANS_NM max 1 (rule ≤ 2) |
| G-ID-M-D (6) | D | 07h | READING | 5 BELOW-16; M-CBLUR-TRANS READS-WRONG (16) |
| G3 (43 rows) | Q | all | 43 PASS | post-revert difference equals the null's own band; ledger balanced; exact material names restored |
| G-RD (OPAQUE, ALPHA) | Q | 07h | 2 PASS | identity 0 over 101 frames; phase-matched drift 0 of 92 frames; the no-clear lever reads wrong, as it should |
| G-BIND (1) link | Q | 07b | PASS | — |
| G-BIND (3) layer | Q | 07h | PASS | layer half changes 1,192 px; ring max 4 (< 32); far max 1 (≤ 2); global half max 1 |
| G-BIND (4) dead | D | 07h | READING | bindings `transformable` |
| G-REASON (15) | Q | 07h | 14 PASS, **T10 FAIL** | all expected finals produced; T10 applied |
| G6 S3A | Q | 07h | PASS | lever armed 1 slot, restored 1 |
| G4 | Q | 07b / — | 2 UNEXERCISED (declared); the rest UNRUN | — |
| G-COLL | D | — | UNRUN | — |
| G11 / TRIP | Q | all | 154 / 154 PASS | — |
| LUMA / BAND F-SYN | PRE | 07b | PASS / VALID | mean luma 66.8962; whole-frame max 0 |
| G0 F-SYN (A3) / F-MW | D | 07h / 07b | READING | 77 targets, 0 findings / 10 targets, 6 findings (ruled in 07c) |
| FIXTURE-NAMES | Q | 07h | PASS | — |

Wrong-copy minimums, which all passed (floor 32 for G-ID, 16 inside the mask for G-ID-M):

| rows | minimums |
|---|---|
| U-C1 | chanswap 150, sRGB-twice 115 |
| U-C2 | chanswap 146, sRGB-twice 108, alpha 194 |
| U-C3 | chanswap 150, sRGB-twice 101, alpha 197 |
| U-C4 | chanswap 151, sRGB-twice 118 |
| U-C5 | sRGB-twice 90 |
| U-D1 to U-D4 | chanswap 71–73, alpha 205 |
| U-D5, U-D6 | chanswap 206 |
| U-N1 | normal 172, chanswap 72 |
| N-N1 | normal 180, chanswap 65 |
| MASKED | alpha 154 (offline prediction 138/140) |
| SHARED | chanswap 154 |
| M-A-MIN | mipshift 200, mipgen 182 |
| M-A-GRAZE | mipshift 184 |
| M-A-TRANS | mipshift 199, mipgen 193 |
| M-B-MAG | texelshift 59 |
| M-D-MAG, M-D-MAG_NM | texelshift 34 (the thinnest margin) |

**4. The failure**
- **Gate:** G-REASON T10, stop row 9.
  - **Row:** `TC_Stream2k` (`StaticMeshActor_74`) without the streaming bias should refuse with `not_fully_resident` (V1).
  - **Evaluator output:** `['no REFUSED line', 'step(s) [] != V1', '8 APPLIED line(s) on a refusal row', 'texcorrupt_fires_applied != 0']`.
  - **Evidence:** `_reviews\082-07-evidence\run\M53S1_REASON_T10_A1\` (`game.log`, `command.json`), and in the bank `M53S1_REASON_T10`.
- **What the logs show:**
  - All three frame-1 censuses (G0_FSYN A1, A2, A3) read `T_TC_Stream2k resident=7 max=12` → `not_fully_resident`.
  - The T10 leg first decides at **frame 18**, after the 2-frame warm draw and the 16-frame lead-in. There it reads `resident=12 max=12` → `transformable` → `APPLY`.
  - The refusal fires only when `NumResidentLODs != MaxNumLODs` (`TexCorruptTree.cpp:466`).
- **Cause read:** I believe the **gate's premise is what failed, not the feature**.
  - The feature applied, correctly by its own rule, on the state it read.
  - The row assumed a streamed 2048² texture on a far plane stays partly loaded until apply. It is fully loaded within 17 frames.
  - The row was admitted on a frame-1 census. That is G320's shape on the time axis, now recorded as G328.
  - Why the streamer loads the full chain is **not established**.
- Nothing was fixed, re-run or edited.

**5. The owed in-engine proofs**
- **P2-1 and P2-3: CONFIRMED over the 153 accepted m53 capture legs.**
  - This is not "the full run": the window evaluates these at the end, and it never got there.
  - I computed them read-only (`082-07h-evidence\readings-082-07h.json`). The live run files and the locked harness hashed identical before and after.
- **P2-2, P2-5 (including the F+2 0/0 balance) and P2-6: NOT OBTAINED.** Their legs (G-COLL, the G4 FailStep rows and G4 DES2) are among the 39 unrun.

**6. Readings**
- **G0 against §R14:** F-SYN (A3, on the build under test) has 0 findings over 77 targets. F-MW is unchanged from 07b.
  - New: the frame-1 census residency for `TC_Stream2k` does not hold at the apply frame.
- **G-COLL:** unrun.
- **G-BIND (3) far-strip reading (ruling 1):** 784 px of the global half lie more than 16 px from the layer half. All 8 events read max \|d\| **1**, with 0 px above 2. That is not a finding.
- **The halo:** ring max 4 (42 px above 2, extent 2 px), far field max 1. It is consistent with G324 and well inside R = 42.

**7. Deviations**
- P2-1 and P2-3 were computed read-only over the accepted legs rather than by the window's final row, because the window stopped first.
- The noflip citation fix edits one line in the predictions file, as the 082-07g ruling asked. With the head also moved, the **boundary is now stale: any resume needs a re-issue.**
- Wait budget used: 333.6 s of 2,700 s. No `-2` or `-3` tag was needed.

**8. Commits pushed**
- `f9d6710` on `feat/m53-uv-normal-corruption` (local = origin). It carries:
  - the journal `2026-09-28-082-07h-code-m53-s1-legs-final.md`, with the full 166-attempt ledger: 11 superseded attempts marked, the 39 unrun listed;
  - the status block;
  - gotcha G328;
  - the noflip citation fix, in the predictions and the 07g journal.
- No tag. `m51` is still `53bf725` and `master` `b5f15a3`.
- Postflight PASSED. The only foreign process present was an `UnrealTraceServer.exe`, left untouched.

**9. NEEDS-DECISION**
1. **Rule T10:** a gate-premise defect (my read) or a feature FAIL.
2. **If it is a premise defect:** how T10 gets a producer that is still partly loaded at its decision frame (for example a lever that holds residency, or a decision before the lead-in), or whether to re-scope the row.
3. **Resume:** re-issue the boundary, then run the 39 unrun legs from `REASON_A5`.

**10. Wall-clock:** 02:57 to 03:39 IST, about 42 minutes, of which the legs took about 34.
api-equivalent cost (not billed on Max): $8.85 | turns: 66 | running total across all briefs: $2,764.51
