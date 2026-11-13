# 095-04 / 095-04e — m58 terrain blink subtype: written, Editor-compiled, selftest passed (2026-11-13)

Claude Code (Opus 5.5, xhigh), headless. Branch `feat/m58-terrain-blink-subtype` off `master` `ebc34ed`.
Binding inputs: `_reviews\095-02-plan-C-terrain-blink-subtype.md` (build spec), `_reviews\095-02-ruling.md`
(Unit C answers), `_reviews\095-03b-ruling.md` (light brief: Editor compile only, no engine legs),
`_reviews\095-00-ruling.md` §4 and §6. Receipts:
`D:\IntrusiveAnomalies\_relay\runs\2026-11-13-095-client-feedback\code-receipts-095-04\`.

**Status in one line: the code is written and committed, the plugin's Editor modules compile with 0 errors
and 0 warnings, the pure selftest C-G1 passed and its three mutants fail, NO engine leg has run, and the Game
target was NOT built.**

## 1. What was built

- **The client's ask (095-00 C1):** a `blinking` event whose target is terrain reads `anomaly_type: blink`,
  `anomaly_subtype: terrain_disappear_reappear`. Every other blink stays `disappear_reappear`.
- **Scope (b), ruled:** blinking on ENGINE LANDSCAPE is refused (`terrain_unlabelable`, counted), because such
  an event has no mask and a box that is not the terrain (plan §4). Mesh terrain matched by a pattern is not
  refused. The auto pool cannot pick engine landscape anyway, so this closes the targeted-fire path.
- **Terrain rule (C2, Q5):** class rule = the actor's class chain holds `LandscapeProxy`, or a primitive
  component's chain holds `LandscapeComponent`; pattern rule = a case-insensitive substring of the actor
  name, the actor's own class name, or a primitive component's name, own class name or mesh asset name. An
  actor is terrain if it or any component matches; the rule text says which. Class rule first; patterns in
  list order.
- **Patterns default empty (Q3):** `[AnomalyInjector] TerrainNamePatterns` + `IAI.SetTerrainPatterns`. The run
  banner's `Capture(m58): TERRAIN RULES` line echoes the class rule (ACTIVE/INERT), the patterns and their
  source, and the refusal state.
- **Unchanged (C3, Q4, Q6):** selection, eligibility, the hide, masks, boxes, frame lists, `missing_object`, the
  schema numbers.

## 2. Commits

| Hash | Subject |
|---|---|
| `a1fa6b1` | feat(injector): m58 game-agnostic terrain rule |
| `8dc8d58` | feat(capture): m58 terrain blink subtype |
| `db9b072` | fix(blinking): m58 refuse engine-landscape targets that cannot be labelled |
| (this) | docs: m58 terrain blink subtype |

Commit 2 was made from a snapshot of the capture files taken before the commit-3 edits, then the final files
were restored (hashes recorded in the receipts' `commit2-snapshot` and `final-snapshot`). Only the tip was
compiled; the intermediate commits were not compiled separately.

## 3. Where it lives

- `Public/AnomalyTerrainRule.h` — the pure rule (no engine types). `Classify`, `RefusesBlink`,
  `BlinkSubtypeFor`, the class names and the strings.
- `Public/AnomalyTerrain.h`, `Private/AnomalyTerrain.cpp` — builds the facts from the live actor; class lookup by
  path at use time (`FindObject<UClass>(nullptr, "/Script/Landscape.LandscapeProxy")` and `…LandscapeComponent`,
  never loads; verified against UE 5.1 `StaticFindObject`/`ResolveName` with a null outer); rule text; the refusal
  counter; the two bench levers.
- `AnomalyDefaults` — the ini key and console command, built the same way as the exclusion patterns.
- `AnomalyCaptureSubsystem.cpp` — `FreezeEventAnchor` classifies blinking targets once (anchor fields `bTerrain`,
  `TerrainRule`); `MapAnomalyToClient(Id, bTerrain, …)` at both annotation call sites, the transition-only path
  looking the anchor up by the same event id; `TERRAIN-SUBTYPE` log line per event; banner line; run-end
  `TERRAIN SUMMARY` line.
- `AnomalyLabelWriter` — `run_summary.blink_terrain_events` and `run_summary.terrain_unlabelable_refusals`
  (always written); `selection_provenance.json` `terrain_rule` (only on a terrain record).
- `Anomaly_Blinking` — drops refused targets, logs `REFUSED-TERRAIN`, returns `terrain_unlabelable` through
  `GetLastRefusalReason` when nothing remains (the auto-injector's yield tallies read it).

## 4. Gates run in this brief

- **C-G1 pure selftest** (`tools/terrain_rule_selftest.cpp`, MSVC 14.38 `cl /std:c++17 /EHsc /O2 /W4 /WX`, compiled
  against the header the plugin compiles): **42 checks, 0 failures.** Mutants from `tools/terrain_rule_mutants.py`,
  each compiled clean under the same flags: class rule removed → **8 failures**; pattern rule removed → **11**;
  case-sensitive match → **11**. ⚠ The first mutant set used constant conditions that `/WX` rejected at compile
  time; a compile error is not a caught mutant, so they were rewritten to compile and fail at run time (both
  sets kept in the receipts, `cg1\`).
- **Editor compile** (`StackOBotEditor Win64 Development`, incremental, `-MaxParallelActions=2`), started only
  with ≥ 4 GiB free: free memory 11.32 GiB at start, minimum 7.28 GiB during, never near the 1 GiB stop line.
  **Exit 0, 14 actions, 129.5 s, 0 errors, 0 warnings.** ⚠ A first launch attempt (`c1`) never started UBT
  (`cmd /c` quoting); fixed with a `.bat` wrapper, receipts kept.
- **Lever audit:** source PASS (55 lever names, the two new ones `scope=static shipping=excluded gate=swept`);
  `--binary` on the five Editor DLLs PASS (55 in binary, 55 registered, 0 unknown, 0 missing, echo present);
  `--selftest` 12 cases OK.

## 5. Gates that remain (all engine legs; brief 095-07, when the owner frees the PC)

C-G2 a/b/c (MainWorld, a `LandscapeStreamingProxy` tile: refused / refusal-off subtype / class-rule-INERT
can-fail; PIE and staged), C-G3 + can-fail (CB_GateLevel, `IAI.SetTerrainPatterns Cube`, staged both tick
orders + PIE), C-G4 (non-terrain unchanged against the release build), C-G5 (MainWorld auto-pool, no
eligibility change), C-G6 (echo and docs on a real run). Predictions frozen before any leg:
`code-receipts-095-04\predictions-095-04.txt`. The Game target build and the lever audit on the Development exe
also belong there.

## 6. Deviations from the plan, stated

1. **Two bench levers, not one.** `IAI.Bench.TerrainRefusalOff` (C-G2b) and `IAI.Bench.TerrainClassRuleInert`
   (the plan's own C-G2 can-fail, "class lookup forced to fail (bench lever)").
2. **Both levers and `bRefuseBlink` live in commit 1** (the helper), not commit 3; commit 3 is the anomaly's use.
3. **The banner echo's call site is in commit 2** (capture); the describe function is in commit 1.
4. **`run_summary.terrain_unlabelable_refusals` is added** beside the plan's `blink_terrain_events`: "counted" needs
   a place a delivered session can show.
5. **Pattern fields:** the actor's and each component's **own** class name, never parent classes (a pattern like
   `Mesh` or `Actor` would otherwise match everything). Primitive components only.
6. **Manual multi-target `IAI.Apply blinking <pattern>`:** engine-landscape matches are dropped and the rest still
   blink; the refusal reason is set only when nothing remains. Capture events always have one target.
7. **ASCII case folding** in the pure matcher (the exclusion patterns use `FString::Contains`); identical for
   ASCII names.

## 7. Interruption

The 095-04 run was cut off at 15:17:58 by `Failed to authenticate. API Error: 403`, one step after the lever
audit and before any commit; the 095-04b resume died the same way. 095-04e resumed in the same session, found the
working tree identical to the edits (every source file older than the compile's start, 15:14:35), committed,
and finished. No source changed after the compile, so no second compile was needed: HEAD's `Source/` equals the
compiled tree.

## 8. What the host binaries match now

The host's plugin **Editor** DLLs match this branch's source (`db9b072` = the compiled tree): AnomalyInjector
`A7EAAE2F`, AnomalyCapture `7ABA30EA`, AnomalyControlServer `8E3B504D`, AnomalyBench `DCF5EDD0`, AnomalyShaders
`79B7E535` (unchanged from 095-03b). The **Game** exe is still the 24 Sep `002805CF` and matches neither `master` nor
this branch (095-03b ruling: no bench leg on it until the deferred rebuild).
