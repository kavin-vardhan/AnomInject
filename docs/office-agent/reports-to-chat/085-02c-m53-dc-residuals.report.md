# REPORT for 085-02c-m53-dc-residuals.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 085-02c-m53-dc-residuals.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-29 11:05:19 | finished: 2026-09-29 11:36:19 | duration: 31 min | exit code: 0
context at last turn: 633020 tokens (63% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\085-02c-m53-dc-residuals.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN

**Summary for the owner.** I fixed all three issues from chat's ruling. Test identity runs now stay real identity runs; after an early manual revert of a texture corruption, `stuck_low_mip` can no longer start on the same frames; and the census prints only its four lines, even as the first command after launch. Every fix is tested both ways, both targets rebuild with zero warnings, and the build is archived. Nothing was launched, cooked or staged, so there is no runtime evidence yet; that comes in 085-03.

**The three items** (source commit `270ed32`)

- **N1: bench levers beat the targeted round-robin.**
  - Where: a new shared rule `TexCorruptPure::NoModeLever` is now used by both `Apply` and `AnomalyTexCorrupt::TargetedNoModeBenchLever`. `TargetedAttempt` gains a lever flag, and `TryFireSpecific` passes it and logs `mode_source=bench_lever lever=<name>`.
  - Behaviour: while a lever is set, no round-robin mode is inserted and the round-robin counter doesn't advance. The hold is still the only random draw. Client builds have no bench gate, so they are unchanged.
  - Test: a check that feeds the targeted decision through `Apply`'s mode selection over 48 combinations, plus 14 new known-answer rows.
  - Can-fail: mutant 7 (round-robin inserted under a lever) fails 80 checks, including `apply_selects_identity_got_round_robin`. Mutant 8 (counter advanced under a lever) fails 31. The existing known-answer rows are unchanged.
- **F1 reverse: the exclusion reads m53's retained fires.**
  - Where: three new rules in `AnomalyExclusion.h`, used in `QueryExclusionTrail`. For an m53 partner it now reads the auto-injector's retained live fires and pending FireWindow snapshots, both reported as `label_tail`. The m52-partner behaviour is unchanged.
  - Test (Codex's case): raw revert at frame 20, scheduled revert at 40, readback latency 1. A forced `stuck_low_mip` fire is first admitted at 40, with no frame carrying both entries.
  - Can-fail: mutant 10 (retained fires ignored) admits it at 22, and frames 22–39 carry both entries. Mutant 11 (pending snapshots ignored) fails on the latency-3 and latency-4 admission frames.
- **F4: the census is quiet on a cold start.**
  - Where: the four settings lookups the census reaches (`KnobGet`, `ModeKnobGet`, `GetExcludedTargetPatterns`, `GetAllowTranslucentOnlyTargets`) now return the value silently inside the census scope and don't cache it. The first real use still prints the settings message and any ini refusal warning.
  - Test: `census_readonly_check.py` gains a clause (g) with 8 new planted failures, and it fails 4 ways on the pre-fix source. It also gains a `--log <log> --cold` reader with 12 synthetic logs. Settings messages inside the census block FAIL, a warmed process reads UNDECIDABLE, and the fixed shape PASSes.

**Suites and mutants**

| suite | clean | mutants |
|---|---|---|
| exclusion | 243/0 | 1–11 all fail |
| draw | 978/0 | 1–8 all fail (pool mutants 1–3 unchanged at 107/13/131) |
| pure | 251/0 | 56 |
| active-source | 47/0 | 7/14/1 |
| m52 window / camera-clipping slab | 282/0 · 154/0 | — |
| census check | PASS | selftest 37 OK |
| new `m53_glue_check.py` | PASS | selftest 10 OK; pre-fix source fails 10 ways |

The exclusion-gate, destructor, verify_capture, consistency and change-oracle checks all pass, the comment strip changed nothing, and the lever audit passes on source (46 names, 0 failures, selftest 11 OK) and on the built binaries.

**Build**
- Editor: 16 actions / 151 s. Game: 7 actions / 114 s. Both exit 0 with 0 warnings.
- Game exe `6B671987…` (242,452,992 B). Archived with the five Editor DLLs in `_binary_baselines\m53dcres-6B671987\`; all 6 files re-hashed at the archive and match.
- The new log strings are in the exe and the Injector DLL. The F1 and F4 changes add no strings, so their presence rests on the clean rebuild plus the two structural checks.

**Commits**
- `270ed32` (source) and `c6aa9c9` (docs) are pushed to `feat/m53-uv-normal-corruption` (225854e..c6aa9c9). No tag, no merge.
- `m51`, `master`, `fix/m52-label-timing`, `ToCodex\` and `E:\<personal-folder>` were not touched. The host is left warm at `270ed32`.
- Docs: journal `2026-09-29-085-02c-m53-dc-residuals.md`, status block, readme §8.8, architecture, checklist, gotchas G384–G387.
- Context used: about 0.63M of the 15M-token session budget.

**NEEDS-DECISION:** none. Four interpretations, stated in the journal §4:
- **F1 covers m53 partners only.** After an ordinary revert, `stuck_low_mip` is released at the later of revert+2 frames and the last m53 frame being written. That is still revert+2 at readback latency 1, but under heavy load it can come later, which can shift a random draw.
- **Normal-corruption with only the tile probe set still takes the round-robin**, because the tile probe applies to `uv_corruption` only.
- **The cold-process log reader finds the census by its engine frame.** A UE log records no command echo, and I kept the pinned four-line census format rather than add a start marker.
- **`m53_glue_check.py` was not requested.** I added it because the unit tests can't reach the engine code Codex pointed to.

For 085-03, the harness should confirm `mode_source=bench_lever lever=identity` on every identity-null run and get a PASS from `--log --cold` on a cold-start census.

Several MCP connectors (Gmail, Calendar, the engineering, design and marketing plugins) need authorising in claude.ai or `/mcp`, and the pdf-viewer server failed to connect. None of them was needed for this brief.
api-equivalent cost (not billed on Max): $19.39 | turns: 171 | running total across all briefs: $3,541.34
