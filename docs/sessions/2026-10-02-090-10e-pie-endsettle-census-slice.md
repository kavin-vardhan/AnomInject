# 090-10e — PIE end-settle flag, census listing in slices; the "PIE end+1" was a reader artefact

Date: 2026-10-02 (06:09–08:11 IST, then finished by 090-10f2 Part 0). Author: Claude Code (Opus 5.5).
Branches: `fix/m52-label-timing` (code `eacd308`, tools `257ff68`, readme `3b06311`) merged into
`feat/m53-uv-normal-corruption` (census `64c0817`, tools merge `9b911e7`).

> **Why this journal was written by another session.** 090-10e's watcher lost its login ("OAuth session expired") at
> 08:11, after every leg had run and every code commit had been made, but before the fix-branch strict pass, the archive,
> the docs and the push. 090-10f2 Part 0 finished those steps from 090-10e's own records (its live feed and
> `E:\IA_BuildCache\_r910e`). Nothing below was re-measured unless it says so.

## 1. What the brief asked
- **Part A:** the PIE "end +1" seen in 090-10b2/090-10c (the picture apparently still changed one frame after a texture
  anomaly's label ended, on about one event per PIE run): flag that frame in PIE so it is never a positive, a negative or
  a reference, and prove it in PIE.
- **Part B:** the m53 census (`IAI.TexCorrupt.Census`) put a 40k-actor listing in one frame; slice it.

## 2. Part A as built (`eacd308`, tools `257ff68`)
- Transition reasons widen from 8 to 16 bits; bit 256 is **`pie_end_settle`**: in PIE only, the first captured frame after
  each labelled run of a fire-window event (`missing_texture`, `corrupted_texture`, `uv_corruption`, `normal_corruption`)
  gets a transition-only entry (`labelled: false`, no target mask, never a member of the event, never an anti-aliasing
  excuse). `run_summary.json` gains `pie_end_settle_active`, `pie_end_settle_frames` and
  `label_transition_pie_end_settle_entries`. Never in a staged or packaged game.
- Pure decision functions in `AnomalyLabelSync.h`, `pie_end_settle_selftest.cpp` over them, six new `pie_*` mutants
  (fix-family suites 6/6 base, 26/26 mutants fail; m53 texcorrupt 7/7 base, 27/27 mutants fail).
- `verify_capture.py --label-rule` knows the reason (five `PIE-SETTLE-*` checks, 65/65 selftest), the label sync kit
  (`label_sync_check.py`, 84/84 both decoders) and the E1 successor `_reviews\090-10e-eval.py` (13/13; `084-09-eval.py`
  byte-unchanged).

## 3. The finding: the end +1 was the reader, not the labels
The first two legs flagged every end edge, but the flag was **not** on the frame the old edge reader called late. Reason:
the reader compared the picture against **`affected_frames`**, which since `m49` is the **observable** subset of the label.
A labelled frame whose PIE target mask was **unmeasured** (`mask_state: "unmeasured"`, `target_pixels -1`, `observable null`)
is left out of `affected_frames`; when that frame is the last labelled one, the reader thinks the label ended a frame early
and reads the next frame as "picture still changed after the label". Against **`injected_frames`** (the labelled set):

| sessions re-read | events | old reader "end +1" | of those, labelled set ends one later | end +1 against the labelled set |
|---|---|---|---|---|
| 090-10e (21 PIE sessions) | 147 | 12 | 12 | **0** |
| 090-10b / 10b2 / 10c (96 PIE sessions) | 552 | 21 | 20 | 1 (the `BP_EnergyOrb` leg already called inconclusive: rotating, pulsing) |

The 12 A-recipe legs on the new build (`EBEED6F4`; uv / normal / missing / corrupted × 3 runs, 7 fires each, AA off,
`CB_TexCorruptLevel`): **84 events, 84 in sync against the labelled set**, every end edge flagged at e+1, every picture
`CORRUPTS` (uv 40.95, normal 17.47, missing 47.97, corrupted 72.89), `pie_end_settle_frames` 7 per leg = rows. The
census-recipe legs (`r11`, `r12`) and the same recipe on the previous build `B3103E32` show **no** end +1 on either build.
Staged legs on the same build: all 4 types 7/7 in sync, `pie_end_settle_active false`, 0 flags. All three consumers pass
on all 12 PIE sessions (verify_capture `NO MISMATCH`, kit `RELEASE pass 5 / fail 0`, E1 090-10e PASS).

**Disposition:** `pie_end_settle` **stays**. It only marks a frame that is already unlabelled, so it costs one frame per
event in PIE and nothing else, and it keeps working if a real PIE end +1 ever appears. The readme no longer says the picture
"was measured to stay changed"; it says the flag is a precaution and why (`3b06311`). Gotcha **G450**.

**What this left open, and 090-10f2 Part M answers:** why do early PIE frames have **labelled** entries with an
**unmeasured** mask (15 of 224 labelled frames on the old build, 15–22 on the new)? Pre-existing, not caused by this change.

## 4. Part B — census listing in slices (`64c0817`, m53 only)
`TActorIterator` construction is cheap (it gathers by class); the cost was the per-actor renderable test run inside one
frame. The listing now snapshots actors, then tests them in the existing 4 ms slices before sorting and evaluating; a time
limit during listing finishes the listing first; no progress line while listing; "complete" only after the listing.
Census selftest 40/40. Longest frame inside the census window, PIE, counts unchanged:

| leg | before (`B3103E32`) | after (`EBEED6F4`) |
|---|---|---|
| MainWorld (343 actors) | — | 27.6 ms |
| 15,000 extra actors | 63.9 ms (090-10b2) | 37.5 ms |
| 40,000 extra actors | 86.6 ms | 50.8 ms |
| staged MainWorld census | — | 16.2 ms max frame, 0 frames over 100 ms (600-frame CSV) |

Every value is under the 100 ms bound.

## 5. Builds, archives, checks
- m53 `64c0817`: strict pass Game 93 / Editor 102 actions, 0 errors / 0 warnings, 64/64 header TUs, restore OK, normal
  rebuild 0/0; leg build exe **`EBEED6F4`** (Capture `6CDBC673`, Injector `D4B7281C`); lever audit PASS 52/52; UTF-16
  string scan vs `dcd8d29`'s exe: 11 added / 6 removed, all this round's strings or one-byte pool artefacts.
- fix `3b06311` (code = `eacd308`): strict pass (090-10f2) Game 70 / Editor 79 actions, 0/0, 58/58 header TUs, restore OK,
  normal rebuild 0/0; exe **`68BA4D9A`**; lever audit PASS 34/34 (exe + 5 editor DLLs); UTF-16 string scan vs `5E93A74E`:
  7 added (6 this round's, 1 short) / 2 removed (the reworded reasons line and a short artefact).
- Python suites: fix 19/19, m53 27/27.
- Archives (E: is below its 50 GB floor, so on D:): `D:\IA_BankOverflow\_binary_baselines\m53-0910e-EBEED6F4\` and
  `...\m52fix-0910e-68BA4D9A\`, each with `archive-hashes.json`.
- Run records: `E:\IA_BuildCache\_r910e` (legs `ev\`, sessions `out\`, logs `logs\`, recheck `recheck_*.txt`).

## 6. Not done here
No tag, no merge to `master`. The unmeasured-mask frames are handled in 090-10f2 Part M.
