# REPORT for 082-07e-m53-s1-legs-resume.md
verdict: RED (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 082-07e-m53-s1-legs-resume.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: high
started: 2026-09-28 00:48:02 | finished: 2026-09-28 01:01:11 | duration: 13 min | exit code: 0
context at last turn: 394250 tokens (39% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\082-07e-m53-s1-legs-resume.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: RED

**Plain summary for the owner**
- The run stopped early on its own stop rule, after 12 of the remaining legs; 139 of 194 legs are still unrun. The stop is caused by a faulty test tile, not by the feature.
- The "identical copy" test passed exactly (zero difference) on all 14 texture-encoding rows. That includes the two re-built normal-map tiles.
- Nothing visible was left behind after revert: G3 read 0 difference on all 14 rows.
- The stop came from the masked test tile. The test deliberately corrupts that tile's transparency and expects a visible change. But even with no corruption installed, the tile's transparency cutout has no visible effect, so a transparency-only corruption cannot change any pixel. The deliberately wrong copy therefore looked the same as the untouched tile.

**3. Gate table** (live rows)

| gate | class | legs / rows | result | key reading |
|---|---|---|---|---|
| FIXTURE-NAMES | PRE | G0_FSYN | PASS | 77/77 |
| G0 F-SYN (re-run) | D | G0_FSYN_A2 | READING | 77 targets; normal 77 / uv 28 compared; **0 findings** (as predicted) |
| ADMIT | ADMIT | 6 | ADMITTED | U-N1, N-N1, MASKED |
| G-ID | Q | 14 rows | **13 PASS, 1 FAIL** | identity max \|d\| **0 (EXACT) on all 14**, 8 events each |
| G-ID wrong-copy minimums | Q | 24 WC legs | ≥16 except MASKED | U-C1 150/115 · U-C2 146/108/α194 · U-C3 150/101/α197 · U-C4 151/118 · U-C5 90 · U-D1 73 · U-D2 71/α205 · U-D3 72/α205 · U-D4 73 · U-D5 206 · U-D6 206 · **U-N1 normal 172 / chanswap 72** · **N-N1 normal 180 / chanswap 65** · **MASKED α 0** |
| G3 (A1.5) | Q | 14 rows | PASS | 0 at the same frame index, incl. U-N1, N-N1, MASKED |
| TRIP | Q | 54 + 1 read-only | PASS | mip mismatch 0 |
| G11 | Q | 54 + 1 read-only | PASS | all 3 assets `resolved=1` |
| LUMA / BAND / G-BIND link | Q/PRE/Q | from 082-07b | PASS / VALID / PASS | stand (A1.10) |
| G4 LEVEL-CHANGE / RECREATED-COMPONENT | Q | — | UNEXERCISED | declared in advance |
| G-RD, G-REASON, G6, G-COLL, G-ID-M, other G4 rows | — | — | **NOT REACHED** | after the stop |

**4. Failure**
- **Gate:** G-ID MASKED, class Q, stop row 3. The rule is that every assigned wrong copy must read at least 16 on every measured frame; below that, the instrument is invalid.
- **Evaluator output:**
  - identity PASS, EXACT, max 0, 8 events / 57 frames;
  - `alpha`: BELOW-16, `min_frame_max 0` on all 8 events, "57 measured frame(s) read max |d| < 16".
- **Evidence:**
  - bank `M53S1_ID_MASKED_A1`, `M53S1_NULL_MASKED_A1`, `M53S1_WC_MASKED_ALPHA_A1\session_20260928-005321`;
  - `_reviews/082-07e-evidence/readings.json`;
  - console `_reviews/082-07-evidence/082-07e-window-1.out.txt`.
- **Cause read. I believe the gate's encoding and the fixture are at fault, not the feature:**
  1. The fault was applied. Every APPLIED line reads `fault=alpha noapply=0`, and the DECIDE lines read `wrongcopy=alpha`.
  2. The wrong copy and the null are byte-identical over the whole frame (0 pixels differ on the frames checked).
  3. `M_TC_Masked` is Masked, clips at 0.5, and takes its opacity mask from `T_TC_UC2`'s alpha. That alpha is 72, 104, 152 or 184 (quarters, one per colour), so half the texels are below the clip.
  4. **In the null leg, where the feature is not installed, those texels are drawn anyway.** The orange (a=72) cells read ≈(162,98,42) and the blue (a=104) cells ≈(31,31,161), not the background (16,31,50). The host tile never clips.
  5. The alpha fault changes only alpha, so it cannot show on this tile.
  6. The row never had an offline proof: the 082-06 manifest has no `TC_Masked` entry, and the predictions table leaves its minimum blank.
- **Not established:** why the tile does not clip. Candidates, none tested: the authored opacity-mask connection, the cooked material, a render setting.

**5. Owed in-engine proofs**
- **P2-1:** CONFIRMED. 3,445 readings over 53 capture legs, 0 problems.
- **P2-3:** CONFIRMED. 424 events, subtype `identity` everywhere, and the labels agree.
- Both were read with the harness's own evaluators, read-only. They are partial and not written to `gates.json`.
- **P2-2, P2-5 (with F+2 0/0) and P2-6 (with EndPlay):** NOT REACHED; their legs come after the stop.

**6. Readings**
- **G0 F-SYN against §R14 / A1.12:** 0 findings in both families, and all 7 normal-map targets now read APPLY.
- **G0 F-MW (082-07b, unchanged):** 6 findings, the floor `not_fully_resident` and the rocks `texture_not_parameter`, as recorded in A1.8.
- **G-COLL:** not reached.

**7. Deviations**
- P2-1, P2-3, and the halted leg's TRIP and G11 were computed read-only outside the window.
- The pixel checks in the cause read are ad-hoc reads of banked PNGs, not a gate.
- **The docs commit moves the feature head, so the 082-07d boundary is now stale and must be re-issued before any further leg.**
- Postflight PASS. Nothing was built, edited, killed or staged; `m51` is still at `53bf725` and `master` at `b5f15a3`.

**8. Commits pushed**
- `570e6f4` on `feat/m53-uv-normal-corruption`, local = origin. It contains:
  - the journal `docs/sessions/2026-09-28-082-07e-code-m53-s1-legs-resume.md`, with the full leg ledger of 63 rows;
  - the status block;
  - G321, the PowerShell `>` UTF-16 and `-Encoding ascii` → `?` gotcha (the ruling asked for the first; the second happened in this run);
  - G322, wrong-copy rows need an offline proof and a null that shows the channel.
- No tag.

**9. NEEDS-DECISION**
1. **The MASKED row.** The options are:
   - fix `M_TC_Masked` and prove its clip both offline and in a null leg;
   - re-assign or drop MASKED's alpha wrong copy;
   - make its alpha half a reading only.

   Whichever you choose, it then needs a predictions amendment and a boundary re-issue before 082-07f.

**10. Wall-clock**
- The window ran about 5 min (19:18–19:23 UTC); the wait budget spent was 88 s of 2,700 s.
- The whole session took about 35 min.
api-equivalent cost (not billed on Max): $7.02 | turns: 52 | running total across all briefs: $2,716.91
