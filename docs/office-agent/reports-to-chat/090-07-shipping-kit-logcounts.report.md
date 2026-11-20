# REPORT for 090-07-shipping-kit-logcounts.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 090-07-shipping-kit-logcounts.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-30 16:48:07 | finished: 2026-09-30 18:52:30 | duration: 124 min | exit code: 0
context at last turn: 687089 tokens (69% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\090-07-shipping-kit-logcounts.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN

**Summary for the owner:** Both office hosts should pull **`feat/m53-uv-normal-corruption` at `2015cfa`** on Thursday; a host that tracks the fix branch pulls `fix/m52-label-timing` at `c29e223`. With this round the plugin simply drops out of Shipping builds, the office check now handles effects the game removes partway through, and a Python script replaces the PowerShell log counter. Nothing was launched or cooked.

**A. Shipping exclusion (descriptor only, no source change)**
- `"TargetConfigurationDenyList": [ "Shipping" ]` is now on `AnomalyShaders`, `AnomalyInjector`, `AnomalyCapture` and `AnomalyControlServer`; `AnomalyBench` already had it. G428 now records that the Shipping-only code is unreachable, not fixed.
- Module lists came from UBT `-Mode=JsonExport`, which lists modules without compiling (about 7 s each). Both hosts give the same result:

| Target / configuration | Modules before → after | Plugin modules before → after |
|---|---|---|
| StackOBot Shipping | 306 → 302 | 4 → **0** (the other 302 are the same) |
| StackOBot Development | 335 → 335 | 5 → 5, export byte-identical |
| StackOBot Test | 329 → 329 | 5 → 5, export byte-identical |
| StackOBotEditor Development | 742 → 742 | 5 → 5, export byte-identical |

- **No real Shipping compile was run.** It was optional and would have meant a full Shipping engine build. The readme and checklist therefore claim only what the module list proves: "The plugin is excluded from Shipping builds by design; capture with Development or Test."
- **One knock-on fix (G431):** the lever audit's selftest stopped catching its two Shipping-leak plants, because no plugin module is built for Shipping any more. The source audit still passed, so this would have looked healthy. The selftest now puts the mutated module back into Shipping in its test copy, and one new case expects a pass for an excluded module. That new case fails on the old descriptor, so it also detects a missing exclusion. 12/12.

**B. The office kit (`label_sync_check.py`, kit 1.1)**
- **Rule:**
  - `effect_interrupted` is a recognised reason and is never treated as an anti-aliasing flag.
  - A label end next to an interruption is judged against the picture during the interruption, using the kit's own 50 % rule, and must be 0 frames off to pass. For interruptions of 8 or more frames the reference is the last 6 frames, because the first ones can still show the effect when a label is mistimed (G430).
  - If the effect comes back after an interruption, each labelled run is judged at its own edges.
  - If the interruption's picture still shows the effect, the event fails with "interrupted frames show the effect".
- **READ BACK line:** `blinking judged 12 | start {+0:24} | end {+0:24} | wrong-object 0 (upper bound 2) | censored 1 | fail 0 | interrupted 0`. `OFFICE-CHECK.md` has one plain line explaining `interrupted`.
- **Selftest:** 39 checks (was 32), OK on both decoders.
- **Doctored copies of two real banked sessions** (TSR; missing_texture and corrupted_texture), 7 shapes each:
  - The raw-revert end (Codex's 10/20/40, scaled to the capture), both re-install shapes (host material or original in the gap), and host material until the next event all read **PASS 0/0**.
  - Label running 4 frames past the interruption: **FAIL, end −4**. Label stopping 2 frames early: **FAIL, end +2**. Gap still showing the effect: **FAIL**.
  - Result: 14 of 14 as designed.
- **Mutants:**
  - Treating `effect_interrupted` as an anti-aliasing flag excuses the 2-early copies (reads end 0) and fails the selftest.
  - Ignoring the reason (which behaves like the old kit) misreads the host-material copies (end +14; +2/−1) and lets a gap still showing the effect through as CENSORED; it fails the selftest.
- **Regression:** the old and new kit on 8 real sessions without interruptions (91 events, 60 judged) print identical reports apart from the new lines.
- **Known limits:**
  - A one-frame interruption between two labelled runs has no picture of its own, so it is read against the pre-event picture.
  - Under anti-aliasing, a smear above half strength on the first interrupted frame would read end +1, because interruptions carry no anti-aliasing flag.

**C. `tools/m52_log_counts.py`**
- Standard library only; prints numbers only, in the same lines as chat's PowerShell function: PURITY, HOLDING (the "HOLDING n of m" form only), HELD NONE with its six buckets, shared_world (2–8 / 9+, min/max), baseline_pending, HOLD CONTAMINATED, matched 0 mesh.
- **Selftest (9/9):**
  - It checks that its format strings still match the plugin source (10 of 10).
  - On a sample log it reads 2 / 1 / 2, buckets 1/1/0/3/1/0, shared_world 4 = 2 + 2, min/max 3/12, baseline_pending 1, CONTAMINATED 2, matched 0 mesh 1, in UTF-8 (with and without BOM) and UTF-16.
- Its output is identical to the PowerShell function on the sample and on two real logs.
- The "counts HOLDING RIGHT NOW" mutant fails, and so does a user-count boundary mutant.
- `OFFICE-CHECK.md` has one line telling the owner to run it.

**Builds**

| | Fix branch | m53 |
|---|---|---|
| Head built | `bcdd7e1` | `983215f` |
| Normal build (Editor + Game) | Up to date, **byte-identical to `129E8E53`** | Up to date, **byte-identical to `97D292F8`** |
| Strict-include | 0 errors / 0 warnings, PASS | 0 errors / 0 warnings, PASS |
| After the strict pass relinks | exe `8AA57A53`, 0 UTF-16 string differences | exe `A3EC29D8`, 0 UTF-16 string differences |
| Lever audit (source / exe / DLLs) | 34/34, PASS | 47/47, PASS |
| Python suites | 15/15 | 23/23 |
| C++ suites | 296 / 154 / 25 checks, 0 failures; 5 mutants fail | 6/6 + 25/0; 28 mutants fail |
| Archive (6/6 re-hashed) | `m52fix-09007-8AA57A53` | `m53-09007-A3EC29D8` |

**Yes, the 090-06 pinned exe `97D292F8` still represents the m53 head's Development code:** the m53 head's normal build is byte-identical to it.

**Commits pushed**
- `fix/m52-label-timing`: `bcdd7e1` (descriptor, kit, counter, lever-audit selftest, readme, checklist, office card) and `c29e223` (journal, status block, G428 addendum, G430–G431, architecture note).
- `feat/m53-uv-normal-corruption`: `983215f` (merge, no conflicts) and `2015cfa` (docs merge; I resolved the CLAUDE.md conflict by stacking the status blocks, the new m53 block first).
- `m51`, `master`, tags, `ToCodex\`, `E:\<personal-folder>` and `_reviews\090-06-*` were not touched. Commit messages were written without a BOM.

**NEEDS-DECISION:** none. For chat's acceptance:
- The lever-audit selftest change was not in the brief; without it that suite fails on the new descriptor.
- The gap check that fails an interruption still showing the effect is my addition to the ruling's "must not show the effect".
- m53 already had duplicate G149/G153 headings; they are older than this work and I left them.
- Several claude.ai connectors (Gmail, Calendar and some plugin servers) need authorising in their connector settings, and the pdf-viewer server failed to connect. None was needed here.
api-equivalent cost (not billed on Max): $27.65 | turns: 235 | running total across all briefs: $4,047.02
