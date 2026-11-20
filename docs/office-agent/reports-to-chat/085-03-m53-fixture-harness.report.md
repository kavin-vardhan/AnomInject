# REPORT for 085-03-m53-fixture-harness.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 085-03-m53-fixture-harness.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-29 11:37:25 | finished: 2026-09-29 13:00:19 | duration: 83 min | exit code: 0
context at last turn: 916581 tokens (92% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\085-03-m53-fixture-harness.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

**Summary for the owner:** The new test targets are built into the game level and the merged build is cooked and archived. The overnight test harness for m53 is written, all its offline self-tests pass, and a dry run exited 0 with every launch accepted. The game was not launched. One call is needed before tonight: the plan wants a single MainWorld object that works for both corruption families, and old data suggests no such object exists.

**1. Fixture and cook**
- **New cells (CaptureBench `8f8b7f3`, authored by commandlet in a new `dcadd` mode):**
  - `TC_OracleUV16`: 16 colours, used for `tile`.
  - `TC_OracleUV64`: 64 colours, used for `scramble`.
  - `TC_OracleN`: the closed normal set, shown through a readout with BaseColor 0 and Specular 0.
  - `TC_Mixed`: one component with two Default-Lit slots, one normal-mapped and one colour-only.
  - `TC_M52Hold` / `TC_M52Share`: a streamed 1024² texture, the m52 target for the exclusion gates.
- **Offline image checks:** 44, all pass. They include the closed-set separation (45) and every wrong-mode pair differing on at least 25 %. The Python recomputation of the scramble pair reproduces all 7 C++ reference vectors.
- **Authoring took five runs.** The mesh-merge route failed three times; each failed run was rolled back to the starting manifest by a script that re-hashes the tree (G388, G393). Run 4 then failed on a mesh-slot check before saving the level and was rolled back the same way. Run 5 built the two-slot mesh directly.
- **Content manifest diff:** 2,194 → 2,207 files. It adds exactly the 13 declared files and changes only `CB_TexCorruptLevel.umap`, which it has to, to place the actors. Nothing was removed. Verify (a fresh process reading the saved files) and both cooks changed nothing further.
- **Cook:** exe **`6B671987`** (085-02c's build, unchanged) + utoc **`F76F1119`**, ucas `B54C2235`, pak `BBC9A58C`. Archived with hashes at `_binary_baselines\m53dc-cook-F76F1119\`.
  - The map gate passes; the check that it can fail does fail.
  - The container diff against the S1 cook shows the new packages, the level, and the files that also differ between two identical cooks (G391).
- **Descriptor:** without `--bench-cook` it reports `STOP EXTRA AnomalyBench, FIXTURE`, which is correct because a bench cook must never ship. With the flag it passes. The binary lever audit passes (46 names). The check that the bench twins stay hidden without the flag at runtime is a G-LEVER night reading.

**2. Harness (`_reviews\085-04-*`, built on 084-09)**
86 legs: a 7-leg premise window, then 79 gate legs. Predictions for every leg are in the README.

| gate | legs | prediction |
|---|---|---|
| B-M53 fixture | 16 | PASS: onset and offset 0 at 50 %, at least 16 judged per mode per AA condition, 0 no-change events, 0 m53 transition flags, masks exact, `--label-rule` exit 0, the 086-01 checker as second verdict |
| B-M53 MainWorld | 16 | PASS on every judged edge; the no-change rate is reported with no number predicted |
| Arms: CommitDelay / RestoreDelay 3, NoApply | 5 | must FAIL (+3 at the delayed edge; holes) |
| G-MODE-UV / G-MODE-N | 6 | PASS; every wrong-mode reading below 50 % |
| EXCL-A / EXCL-B | 2 | PASS, with 0 frames carrying both an m52 and an m53 entry |
| G-REASON-DC | 3 | bogus, drift and wrong-family modes refused; `TC_Mixed` normal refused as `partial_footprint:1/2:no_normal_map`; uv applies |
| G-SEED | 3 | the draws replay exactly; empty mode sets read `!none` |
| G-LEVER, G-CENSUS, G-BIND, G-ID | 2 / 2 / 4 / 6 | as planned |
| G-SYNC-REG | 8 | 084-09's predictions unchanged |
| COST | 5 | a reading only |

- Identity-null legs must read `mode_source=bench_lever lever=identity`.
- Census legs run cold and must pass `--log --cold`.
- `m53_glue_check.py` runs in preflight (PASS).
- The G377 bench-flag line and the F5 one-RestoreDelay-per-process rule are checked on every relevant leg.
- The harness stages the new pairing itself with a hash receipt, and restores m53 S1 at the end from its own archive, not from whatever 084-09 left staged.

**Offline can-fail proofs:**
- Gate self-test **54/54**:
  - Exclusion: a planted overlap frame fails, and so does an m53 fire applied before the m52 trail closes.
  - Seed: one extra draw fails the replay.
  - Levers, census and mode source: an accepted lever fails, a name in a census line fails, and `round_robin` under the identity lever fails.
  - Known answer: the +3 case is re-read at +3 on 6 of 6 events. The E1 evaluator cannot judge those sessions (G392), so this rests on the 086-01 checker.
  - Restore: with a real 8 s file lock, the copy succeeds on retry.
- Mode oracle **39/39**: each mode passes as itself; every wrong mode, the ±1 scramble pair and a mismatched logged pair fail.
- Ported m53 checks **98/98** on real S1 data. The doctored-label checks (onset/offset ±1) run tonight on the real frames.

**3. ND-4 (MainWorld eligibility at the delivered 128 MiB)**
- The live pre-check is the first premise legs tonight. They record the 128 MiB yield as its own reading, then pick hosts. Any bench-only cap raise (next power of two ≥ the hosts' need, ≤ 1 GiB) is written into a lock file committed before any gate leg; the delivered default stays 128 MiB.
- An offline pre-read of old S1 data (frame 1, re-read under the current rules) found **0 of 10 MainWorld objects eligible for either family**:
  - The rock refuses `texture_not_parameter`, so the plan's "rock first" can never work (G394).
  - The uv candidates were only partly eligible, which the all-or-nothing rule now refuses.
  - The floor's normal map covers only 18 of 30 slots, so normal is refused at any cap.

**4. ND-5 definition**
An event counts as "no visible change" when the E1 evaluator finds no labelled frame whose difference inside the target mask reaches its threshold. It counts only events with a still camera, not a warm-up and not a confounded reference. The rate per mode is reported on MainWorld at t50 and strict. On the fixture it is a FAIL, except scramble pairs predicted to be invisible.

**5. Dry run and expected minutes**
- **Final dry run: exit 0, 86/86 launches accepted, the restore succeeded on attempt 3 against the injected lock, 398 s.**
- On the stand-in data, every reading was the expected one. G-BIND reproduced the old known answer and SYNC-REG B3/B9 failed as they did on the old build. G-MODE reported no map, the fixture cells read insufficient, and the exclusion gates failed because the old logs carry no exclusion lines.
- An earlier full dry run found three harness bugs, all fixed. A short premise-only dry run on the final files: exit 0, 7/7.
- **Expected night: about 145 min, under 170 with person waits. It runs after 084-09.**

**6. Commits and context**
- `feat/m53-uv-normal-corruption` `031434b` (journal, status block, gotchas G388–G394), pushed.
- CaptureBench `8f8b7f3` (local-only repo).
- `m51`, `master`, `fix/m52-label-timing` and `ToCodex\` untouched. The staged set is still m53 S1 (`2FCDF059` / `20DA6F98`). No game was launched.
- Context used: about 25 %.

**7. NEEDS-DECISION**
- **ND-4 host shape.** The plan's P-HOST wants one MainWorld object eligible for both families, with the rock first. Old data predicts no such object exists, and the rock can never qualify. The harness therefore picks hosts separately for each family (an object eligible for both is still preferred). A family with no host at any cap gets its MainWorld legs marked not-run, which on the old data looks likely for normal. That would leave normal-family MainWorld rows as a shortfall, with the fixture still covering normal. **Recommendation: accept per-family hosts, and accept a normal-family MainWorld shortfall as a reported gap rather than a stop.** The alternative is ND-4 option (b), fixture-only for normal.
api-equivalent cost (not billed on Max): $71.08 | turns: 236 | running total across all briefs: $3,612.42
