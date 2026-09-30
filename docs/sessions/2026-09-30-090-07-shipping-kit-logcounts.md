# 090-07 — Shipping excluded by descriptor; the office kit reads `effect_interrupted`; `m52_log_counts.py`

**Date:** 2026-09-30 (by day, compile only). **Spec:** `_reviews\090-05-chat-ruling-f1-nanite.md` rulings 1–3.
**Heads in:** fix `25f9450`, m53 `5cda4e9`. **Heads out:** fix `bcdd7e1` (source) + the docs commit carrying this
journal; m53 `983215f` (merge) + the docs merge. **No game launch, no cook, no staging, no focus taken.** A UE 5.7
editor from another workload was open the whole time (G97); only compiles ran. `_reviews\090-06-*` was not touched;
the 090-06 smoke reads its exe from `_binary_baselines\m53-09005-97D292F8\`, so rebuilding the scratch hosts could not
disturb it. Work files: `_reviews\090-07-work\`, `E:\IA_BuildCache\_r90_07\`.

## 1. A — every plugin module is denied to Shipping (descriptor only)

`AnomalyInjector.uplugin`: `"TargetConfigurationDenyList": [ "Shipping" ]` added to `AnomalyShaders`,
`AnomalyInjector`, `AnomalyCapture`, `AnomalyControlServer` (`AnomalyBench` already had it). **No source change**;
the `ANOMALY_CAPTURE=0` / `UE_BUILD_SHIPPING` branches stay and are now unreachable (G428 addendum).

**Evidence, no compile — `UnrealBuildTool <Target> Win64 <Config> -Project=<host> -Mode=JsonExport` (~7 s each),**
run on both scratch hosts before and after (`_reviews\090-07-work\ubt\`, helper `ubt_modules.py`):

| target / configuration | modules before → after | plugin modules before → after | the rest |
|---|---|---|---|
| StackOBot **Shipping** | 306 → **302** | AnomalyCapture, AnomalyControlServer, AnomalyInjector, AnomalyShaders → **none** | identical set |
| StackOBot Development | 335 → 335 | all 5 → all 5 | export **byte-identical** |
| StackOBot Test | 329 → 329 | all 5 → all 5 | export **byte-identical** |
| StackOBotEditor Development | 742 → 742 | all 5 → all 5 | export **byte-identical** |

Same on both branches (fix host `_r84_host`, m53 host `_r53_host`). The capture module's TICKPIN probe line, printed
whenever UBT instantiates that module, appears in the Shipping export before and not after — the module is no
longer created for Shipping at all. `WebSocketNetworking` (the plugin's plugin dependency) is still enabled in every
configuration, as before.

**Development code unchanged:** the normal Development builds (Editor + Game) on both hosts read **"Target is up to
date"** and every binary is **byte-identical** to its 090-05 archive: fix exe `129E8E53`, m53 exe **`97D292F8`**, all
five editor DLLs equal on each branch. After the strict pass's relink (G201) the binaries' bytes differ but their
UTF-16 string sets are identical (§5).

**The lever audit read the change (G431).** `lever_audit.py` classifies modules from the descriptor; R1 is "no
`IAI.Bench.` string live in a Shipping compile of a module Shipping builds". With no module built for Shipping, R1
has nothing to inspect: the source audit still PASSES (34/34 fix, 47/47 m53) but its selftest read both R1 plants
**NOT CAUGHT**. The selftest now re-admits the mutated module to Shipping in its copy (R1 fires again) and adds a case
requiring the same plant in an excluded module to PASS — that case goes WRONG on the pre-090-07 descriptor, so it
detects a missing exclusion. 12/12 on both branches. Run against the unchanged source with the **old** descriptor,
R1 also PASSES: the source is Shipping-clean at preprocessor level as well as excluded.

**A real Shipping compile was not run.** It is optional per the ruling, and it would have cost a full Shipping engine
build (the Development prewarm on these hosts is 65–73 min) on a day with two strict passes to run. The claim made
in the docs is therefore the module-list one: a Shipping target contains none of the plugin.

**Docs:** `client-readme.md` §2 "Build configuration" and `PRE-DELIVERY-CHECKLIST.md`'s build box: "The plugin is
excluded from Shipping builds by design; capture with Development or Test." `architecture.md` notes the deny list.

## 2. B — the office kit (`label_sync_check.py`, kit 1.1, evaluator 090-07)

**What was wrong (reproduced before fixing):** the kit had no reason list. `effect_interrupted` frames are flagged
entries, so they were never *clean*, and the 084-06 edge-local off reference (the first clean frames after a run)
skipped them: an interrupted end was read against the picture after the **scheduled** end, and a run followed by a
re-install had no off reference and fell back to the pre-event picture. Where the game puts **its own material** in
the interruption, that material then read as the anomaly (below: +14, +2/−1, FAIL or CENSORED on correct labels),
and a gap that **still showed the effect** read CENSORED, not FAIL.

**The rule as built:**
- `effect_interrupted` is a known reason (`KNOWN_REASONS`; unknown reasons are counted and printed). It is **not** in
  `AA_ONLY_REASONS`, so it never excuses a frame and never triggers "anti-aliasing flag without temporal AA"; only
  entries without any reason list are inferred, and F1 entries always carry one.
- A run edge next to an interruption is read against **the interruption's own picture**: the off reference of a run
  whose end is followed by interrupted frames is drawn from them — from their **last 6** when the stretch is 8+ frames
  (the head can still show the effect when the label is mistimed; G430), otherwise from the stretch plus any clean
  frames before the next run, with the existing settle step. The on reference of a run preceded by 2+ interrupted
  frames is drawn from them too (nearest the onset; the first frame after a previous run is dropped when 3+).
- The label end is then judged by the kit's own offset rule (t50, transition-aware), **0 frames off to pass**. A
  labelled–interrupted–labelled event is judged **per labelled run** at each run's own edges.
- **The interruption must not show the effect (from pixels):** if its picture is within the noise band of the
  labelled picture, or within half the effect depth of it (the kit's own 50 % rule against the pre-event picture), the
  event FAILS with "interrupted frames show the effect". The noise-band test alone missed a Lumen-converging
  corrupted-texture gap; the 50 % rule catches it.
- **Counts, never a verdict by themselves:** the READ BACK line gains `| interrupted N` (judged events with an
  interruption); the per-type block prints events, judged, run ends at an interruption, interrupted frames, frames
  between two labelled runs, interrupted pictures still showing the effect, and unknown reasons.
- **Limits, stated:** an interruption of **one frame** between two labelled runs has no picture of its own and is read
  against the pre-event picture (a raw revert reads correctly; a host material there would read as a failure). Under
  TAA an interruption carries no temporal flag, so a smear above half strength on the first interrupted frame would read
  `end +1` — on the M52SYNC TSR legs a scheduled end reads 0 raw at t50, the same picture change.

**Tests, both ways.**
- **Selftest 32 → 39 checks, OK on both decoders** — six synthetic F1 cases plus "READ BACK carries the interrupted
  count": interruption after the label with AA off → PASS 0/0, counted · host material in a 3-frame interruption,
  re-installed → both runs 0/0, gap 3 · host material to the capture end → PASS 0/0 · label 4 frames past the
  interruption → FAIL end −4 · effect visible 2 frames into the interruption with TAA on → FAIL end +2 (not excused) ·
  gap still showing the effect → FAIL with the reason.
- **Doctored copies of real banked TSR sessions** (`M52SYNC_REAL_MT_R1` missing_texture and `M52SYNC_REAL_CT_R1`
  corrupted_texture, event 91–98 of each; driver `_reviews\090-07-work\f1_real_doctor.py`; copies hard-linked on E:,
  changed frames written fresh, masks dropped on interrupted frames, `labelled` keys and the v2 rule name added). Each
  undoctored event reads PASS 0/0. The shapes (Codex's 10/20/40 scaled to the capture's spacing; host replacement at
  15 with re-install at 18 scaled to an 8-frame event):

| shape | kit 1.1 | ignore-mutant (≈ the old kit) | TAA-flag mutant |
|---|---|---|---|
| raw revert: labelled 91–98, interrupted 99–108 | PASS 0/0, interrupted 1 | 0/0 but not counted | PASS 0/0 |
| host material 99–112 until the next event | PASS 0/0 | **end +14**, FAIL / CENSORED | PASS 0/0 |
| host re-install: 91–92, gap 93–95, 96–98 | PASS 0/0 and 0/0, gap 3 | **+2 / −1**, FAIL / CENSORED | PASS |
| raw re-install (gap = pre-event pixels) | PASS 0/0 and 0/0 | 0/0 (not counted) | PASS |
| label 4 past the interruption (91–102) | FAIL end −4 | FAIL −4 | FAIL −4 |
| label stops 2 early, effect to 98 | FAIL end **+2** | FAIL +2 | FAIL, **end 0** (excused) |
| gap 93–95 still showing the effect | FAIL, "interrupted frames show the effect" | **CENSORED** | FAIL |

  **Kit 1.1: 14 of 14 as designed. The TAA-flag mutant misses 2 (it excuses the late frames); the ignore mutant
  misses 10 (misreads the host shapes, counts nothing, and lets a gap showing the effect through as CENSORED).**
- **Mutants against the selftest:** `AA_ONLY_REASONS` += `effect_interrupted` → FAILED (4 checks WRONG);
  `INTERRUPT_REASON` renamed so the reason is ignored → FAILED (5 WRONG). Driver `kit_mutants.py`.
- **Regression on real sessions without interruptions:** old kit (`25f9450`) vs kit 1.1 on eight banked sessions
  (texture ×2 TSR, blinking and missing_object AA off, lod_popping, camera_clipping, stuck_low_mip, m53 fixture):
  91 events, 60 judged — **reports identical line for line** once the new lines are removed, 0 per-event differences,
  every interrupted count 0.
- The report stays numbers-only (selftest check) and `OFFICE-CHECK.md` gains one line on `interrupted`.

## 3. C — `tools/m52_log_counts.py`

Standard library only, numbers only. Selects the lines naming `stuck_low_mip` or `Capture(m52)` (case-insensitive, as
PowerShell) and prints exactly the PowerShell counter's lines: PURITY ENUMERATION · HOLDING (the `- HOLDING n of m`
form only) · HELD NONE and its six buckets · `shared_world`, users 2–8 / 9+, min / max · `baseline_pending` · HOLD
CONTAMINATED (both the injector's and the capture's line) · `stuck_low_mip: matched 0 mesh`; plus a lines-read line.
Reads UTF-8 with or without BOM and UTF-16.

**Selftest (9 checks):** the embedded format strings equal the plugin source's own, extracted from `../Source` when the
tool sits in the plugin (10 of 10); a sample log built from them (with decoys: an already-held refusal, which says
"HOLDING RIGHT NOW", lod_corruption's matched-0 line, a foreign log category) reads PURITY 2, HOLDING 1, HELD NONE 2,
buckets 1/1/0/3/1/0, `shared_world` 4 = 2 + 2 (users 3, 8, 9, 12 → min/max 3/12), `baseline_pending` 1, CONTAMINATED 2,
matched 0 mesh 1, in all three encodings; the printed lines equal the PowerShell counter's.
**Against chat's PowerShell function itself:** identical output on the sample and on two real logs (a Lyra
stuck_low_mip leg: PURITY 24, HOLDING 23, HELD NONE 1, `shared_world` 72; a texture leg: all zero).
**Mutants:** counting "HOLDING RIGHT NOW" → FAILED; splitting users at 8 instead of 9 → FAILED (the sample carries a
user count of exactly 8 for that reason).
`OFFICE-CHECK.md`: "in the plugin folder, run `python tools\m52_log_counts.py "<the game's log>"`".

## 4. Suites

| | fix | m53 |
|---|---|---|
| Python | **15/15** (the 11 of 090-05 + measure_label_offset, the counter, kit mutants, counter vs PowerShell + mutants) | **23/23** (the same 15 + census, exclusion gate, glue, destructor, draw KAT) |
| label-rule / kit / lever selftests | 27 / 39 / 12 | 27 / 39 / 12 |
| C++ | m52 window 296/0, camera slab 154/0, target policy 25/0; 5 header mutants fail | base 6/6, target policy 25/0; 23 + 5 mutants fail |

## 5. Builds

| | fix (`bcdd7e1`) | m53 (`983215f`) |
|---|---|---|
| normal Development, Editor + Game | exit 0, **up to date, byte-identical to `129E8E53`** | exit 0, **up to date, byte-identical to `97D292F8`** |
| strict-include (both targets) | **0 errors / 0 warnings**, restore OK, VERDICT PASS | **0 errors / 0 warnings**, restore OK, VERDICT PASS |
| after the strict pass's relink | exe `8AA57A53`; UTF-16 strings vs `129E8E53`: **0 differences** (exe + 5 DLLs) | exe `A3EC29D8`; UTF-16 strings vs `97D292F8`: **0 differences** (exe + 5 DLLs) |
| lever audit (source / exe / DLLs) | 34/34 PASS · 34 in binary, 0 unknown, 0 missing · Editor 34 | 47/47 PASS · 47 in binary, 0 unknown, 0 missing · Editor 47 |
| archive (re-hashed at the destination) | `_binary_baselines\m52fix-09007-8AA57A53\` 6/6 | `_binary_baselines\m53-09007-A3EC29D8\` 6/6 |

**The 090-06 pinned exe `97D292F8` is the m53 head's Development build:** the m53 head's normal build is byte-identical
to it, so tonight's smoke speaks for this head's Development code.

## 6. Commits
- fix `bcdd7e1` — descriptor, kit, counter, lever-audit selftest, readme, checklist, office card.
- m53 `983215f` — merge of the above, auto-merged, no conflicts, no m53 source file touched.
- The docs commit on fix (this journal, status block, G428 addendum, G430–G431, architecture note) and its merge into
  m53 with the m53 status block.
- `m51`, `master`, tags, `ToCodex\` and `E:\AmmaYT` untouched. Commit messages written by the file tool (no BOM).

## 7. Gotchas
G428 addendum (now unreachable by descriptor, not repaired) · **G430** an interrupted end is read against the
interruption's own settled picture · **G431** a deny list makes every Shipping source gate vacuous; re-plant the
can-fail case where the thing still exists; JsonExport is the no-compile module list. (Max over all refs was G429.)
