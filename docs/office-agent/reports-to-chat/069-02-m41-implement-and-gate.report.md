# REPORT for 069-02-m41-implement-and-gate.md
brief: 069-02-m41-implement-and-gate.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: medium
started: 2026-09-02 22:56:58 | finished: 2026-09-02 23:17:39 | duration: 21 min | exit code: 0
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\069-02-m41-implement-and-gate.live.txt

---- FINAL MESSAGE (Claude Code) ----
Gate **C-G1a FAILED** its pre-declared condition. Campaign stopped, work secured on a branch.

---

# SESSION 069, BRIEF 2 — m41 IMPLEMENTED, GATE C-G1a FAILED, CAMPAIGN STOPPED

## 1. Status, SHAs, push, exe

| | |
|---|---|
| `origin/master` | **`2543674`** — `docs(m41): pre-declared gates and predictions, before any source change`. **Pushed.** Master carries the predictions file **only**. |
| source commit | **`52f6430`** on branch **`m41-CG1a-GATE-FAILED-do-not-merge`**, **pushed** (`origin` tracking set). ⛔ **DO NOT MERGE.** |
| diffstat | 8 files, **702 insertions / 49 deletions** — exactly the planned file set, no encoding smell (G115 check clean) |
| comment stripper | `changed 0 / nochange 93 / skip 0`, run before each commit |
| built = staged exe | **`2BF9E1B9`**, 241,115,136 B (size sane, not truncated — G164) |
| predecessor archived | **`C0AD3F91`** → `_binary_baselines\StackOBot.exe.m40-orderindep-C0AD3F91`, **hash-verified at the new location BEFORE the swap**. It is m41's A-side. |
| container | **UNCHANGED** — utoc `2A66CA57` · ucas `A7EF9B12` · pak `D8009AD7`. Code-only hot-swap, **no cook** (G103). |
| tags | none |

**A44, staged exe, both encodings** — all new strings present, and the scan is proven sound in both directions:
`SpawnTranslucentProbe` 0/3 · `CensusFixedExpiry` 0/6 · `CensusDropEntry` 0/7 · `CensusBatchCap` 0/6 · `HOST-PP CUSTOM-DEPTH READERS` 0/2 · `CensusTranslucentWriters` 0/6 · `census_fires_unseen_candidates` 0/3 · `census_fires_partial_fallback` 0/1 · `census_host_pp_customdepth_readers` 0/1 (ascii/utf16). Positive controls `VETOED-OBJECT` 1, `IAI.Capture.CensusFloor` 8; negative control `ZZ_NOT_IN_THIS_BUILD_ZZ` 0.

**Leg run:** exactly one — `M41_M41_ON_A`, auto-pool, seed 4242, 90 frames, `CB_GateLevel`, **accepted on attempt 1**, `modal_rot (0,0,0) AT REST`, B1 declared **NOT APPLICABLE** (auto-pool, G117). Banked `_bench_sessions_bank\M41_M41_ON_A`.

## 2. Gates — predicted vs measured

| gate | predicted | measured | verdict |
|---|---|---|---|
| **C-G1a** | `= 0` at Log **with non-zero scanned counts**; `scanned 0/0/0` is *"BLINDNESS, NOT A CLEAN READ and is a FAILURE"* | `HOST-PP CUSTOM-DEPTH READERS = 0 (scanned **0** volume(s), **0** camera blend(s), **0** material(s), 0 of which read some scene texture)` | 🔴 **FAIL** |
| **A-G3** (key set) | exactly the 15 named `census_*` keys | **15**, exactly the named set (list below) | ✅ PASS *(the subset diff vs a census-OFF leg was not run — see §5)* |
| **A-G4** (`P6`) | added 0, removed 0 | annotation keyset vs banked `M40_L1_PRE_OFF`: **added 0, removed 0** | ✅ PASS |
| **A-G2 (a)** census provenance | census ON from `COMPILED DEFAULT (on)` | exactly that, single source, no disjunction | ✅ PASS |
| **A-G2 (a)** mask provenance | mask ON from `COMPILED DEFAULT (on)` | mask ON **from the ini** — StackOBot's cooked `DefaultGame.ini` already carries `bMaskMeasureDefault` | ⚠ **NOT OBSERVABLE on this container** (not a failure — ini correctly beats compiled; the compiled-default branch for the mask cannot be exercised here) |
| **D-G2** | `window = 12`, `expired = 0` | `window` reads **12–14**; `expired = 1` on 4 of 7 fires | ⚠ **PREDICTION WRONG IN DETAIL** — bench cycles are 4–6 ticks, so `max(12, cycle+8)` = 12–14, not 12. Behaviour is correct by design; **my prediction was wrong, not the code** |
| **E-G1** | `census_fires_unseen_candidates = 0` | **0** | ✅ PASS (one leg; the second was not run) |
| per-fire line | present on **every** fire | present on **all 7** | ✅ |
| **A-G1**, **A-G2 (b)**, **B-G1**, **D-G1**, **E-G2**, m38 close marker, m40 L4-shape | — | **NOT RUN** — campaign stopped at the failure | ⛔ |

**The 15 `census_*` keys, measured:** `census_frames`=78 · `census_cycles`=25 · `census_candidates`=77 · `census_zero`=13 · `census_below_floor`=49 · `census_above_ceiling`=0 · `census_excluded_translucent`=0 · `census_fires_fallback_all`=3 · **`census_fires_partial_fallback`=6** · **`census_fires_unseen_candidates`=0** · **`census_host_pp_customdepth_readers`=0** · `census_unmeasurable_nanite`=0 · `census_unmeasurable_tag_failed`=0 · `census_unmeasurable_hidden`=2 · `census_unmeasurable_not_yet_measured`=0.

## 3. The provenance echoes, verbatim

**Default run (no ini/console for the census, no `IAI.Capture.Mask`):**

```
=== Capture(mask): EFFECTIVE FOR THIS RUN - mask ON (measure, report and veto), default from
DefaultGame.ini [AnomalyCapture] bMaskMeasureDefault, maskReduce=gpu(from COMPILED DEFAULT (gpu)) ===
READ THIS LINE, NOT THE INI. ...
```
```
=== Capture(census): EFFECTIVE FOR THIS RUN - census ON (requested on, from COMPILED DEFAULT (on)),
floor=0.50%(from COMPILED DEFAULT (0.5)), ceiling=25.00%(from COMPILED DEFAULT (25.0)) [band is
INCLUSIVE: eligible iff floor <= coverage <= ceiling], maxVerdictAgeTicks=12(compiled, m41: the FLOOR
of the freshness window, not a fixed age), excludeTranslucent=1(compiled),
includeTranslucentWriters=0(COMPILED DEFAULT (off)), reservation=1 === READ THIS LINE, NOT THE INI. ...
```

**The `IAI.Capture.Census 0` run: NOT RUN** — the campaign stopped before it. The `off` branch now returns the single string `IAI.Capture.Census (console)` in code, but **that is unmeasured** and A-G2(b) is outstanding.

## 4. B fixture — not exercised

`IAI.Bench.SpawnTranslucentProbe` is **built and present in the exe** (A44 confirms), but **B-G1 was not run** and **no material was chosen**, because the campaign stopped at C-G1a. The candidate table it would print is therefore unread, and the pre-declared question — whether this container holds a translucent material that opts into custom-depth writes — is **still open**.

## 5. The failure, with its measurement

```
Capture(census): HOST-PP CUSTOM-DEPTH READERS = 0 (scanned 0 volume(s), 0 camera blend(s),
0 material(s), 0 of which read some scene texture).
```

The predictions file (`2543674` §5) pre-declared: *"a `= 0` with `scanned 0/0/0` is **BLINDNESS, NOT A CLEAN READ** and is a FAILURE"*, and *"`scanned 0/0/0` … ⇒ the query path is dead; a shipped `N = 0` would be blindness. Report and STOP."* The condition fired exactly as written, so the verdict stands as declared and **the campaign stopped there. No leg was re-run, no code was changed, nothing was relabelled.**

⚠ **What this does and does not establish.** It establishes that **the detector's zero is unproven on this fixture** — which is the whole reason the scanned counts are in the line. It does **not** establish that the query path is dead: `ConsiderBlendable` was never reached because the *enumeration* found nothing, so `FMaterialShaderMap::UsesSceneTexture` was never called. A candidate explanation, **stated as a candidate and not a cause (G120)**: `CB_GateLevel` is a synthetic level authored by `make_gate_level.py` with no `APostProcessVolume`, and the `-unattended` pawn is a `SpectatorPawn` that pushes no camera blend — so `0/0/0` may be the *true* answer here rather than a broken read. **That was not measured and must not be assumed.** The distinction matters for the fix turn: if it is the fixture, C-G1a needs a different instrument, not a code change.

## 6. NEEDS-DECISION

1. **C-G1a's instrument.** The gate as written cannot pass on `CB_GateLevel` if that level genuinely has no post-process. Options: (a) re-aim C-G1a at a level that has one (MainWorld is cooked into this container per m36's map gate) — but that is a *different* fixture, and re-aiming a failed gate needs your ruling, not mine; (b) prove the query path directly with a unit-style console probe that resolves a known material and prints its `UsedSceneTextures` bits, so soundness is provable without any post-process at all; (c) accept `0/0/0` as the honest answer on this bench and move the whole of C's proof to the cook alongside C-G1b. **I have not chosen; the gate failed as written.**
2. **A-G2(a)'s mask half is unobservable here** — StackOBot's cooked ini carries `bMaskMeasureDefault`, so the mask's compiled-default branch cannot be exercised on this container. Confirm that the ini-provenance reading satisfies A-G2, or the mask flip's compiled branch rides the next cook.
3. **D-G2's prediction was wrong in detail** (window is 12–14 on the bench, not 12; `expired>0` occurs even here). Behaviour matches the design; confirm you want that recorded as a corrected prediction rather than a gate failure.
4. **Docs commit and journal 069 §2 are NOT written** — they would have asserted a shipped m41. Confirm they land with the fix turn.

**Not done, named:** A-G1 · A-G2(b) · B-G1 · D-G1 · E-G2 · m38 close-marker check · m40 L4-shape · docs commit · journal §2 · no tag. The staged bench exe is `2BF9E1B9` (unvalidated); `C0AD3F91` is archived and restorable.

**STOP.**
api-equivalent cost (not billed on Max): $25.86 | turns: 93 | running total across all briefs: $128.97
