# 090-01 — The final source round before delivery: stuck_low_mip TAA off-frames default 16, the LOD log wording, the client readme finalised to the week's evidence; then the fix branch merged into m53 again (2026-09-30)

**Brief:** 090-01 (Code, Opus 5.5, headless), from `_reviews\089-04b-chat-ruling-m53-finish.md` ("090-01: the final source round"),
with `089-01-chat-ruling-adjudication.md` rulings 2, 5 and 6, `089-03-chat-ruling-rebench.md` (the per-anomaly status) and
`089-04-chat-ruling-m53-rebench.md` / `088-01-chat-ruling-rehearsal.md` for the readme.
**Branches:** `fix/m52-label-timing` (warm host `_r84_host`, branch worktree) from `d1ab068`; then `feat/m53-uv-normal-corruption`
(worktree `_r53_src`, warm host `_r53_host`) from `620e3cb` — the m53 half of this journal is §5 onwards (it exists only on m53).
⛔ No game launch, no cook, no UE staging, nothing took focus. `m51`, `master`, tags, `ToCodex\` and `E:\AmmaYT` untouched. No tag,
no merge to `master`.

## 0. Bootstrap, in three lines

1. 089-01 ruling 2: under temporal AA the stuck_low_mip flagged tail goes to **16** frames (release rule stays t50); ruling 6: the
   LOD log prints `forced_lod_model=N (LOD index N−1 of M)`; ruling 5: Nanite targets are labelled with boxes but get no mask.
2. 089-03 / 089-04 / 089-04b: every delivered anomaly's label sync is proven (lod_popping and m53 on fixtures; camera_clipping on
   its confirmed frames, unconfirmed frames dropped); the readme must state the evidence basis per anomaly.
3. 088-01: README Step 1 (launch) is the owner's, and §8.8's per-mode rate reads "not measured on realistic content".

## 1. The fix branch — source (`c12b7e9`)

| file | change |
| --- | --- |
| `AnomalyInjector/Public/AnomalyLabelSync.h` | `DefaultOffFramesTemporal` **8 → 16**. `DefaultOnFramesTemporal` 3 and `DefaultHideFramesTemporal` 1 unchanged; `ResolveTransitionFrames` unchanged (0 without temporal AA; an explicit value wins, clamped to 64). |
| `AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp` | `IAI.Label.TransitionOffFrames` help: "-1 (default) = 16 (ruled 089-01)"; the `Capture(labelsync)` echo: "-1 = default 3/16/1". Text only. |
| `AnomalyInjector/Private/Anomalies/Anomaly_LodPopping.cpp` | the CURRENT-LOD and REFUSED lines print `forced_lod_model=N (LOD index N-1 of M)` where they printed the 1-based `worst=N` (and "forcing LOD N") beside a 0-based `level=`. Behaviour unchanged (089-01 §5, G408). |
| `tools/m52_window_selftest.cpp` | the default check reads 16 (was 8); new: the default is 0 without temporal AA; an explicit 8 / 0 / 24 overrides it. |

**Where the off default applies, from source:** `LabelOffFrames` is consumed only by the render-truth transition tracks
(`AnomalyCaptureSubsystem.cpp` `ResolveDetachedTransitionCandidates`, the per-frame `Track.Observe`, the run-end carry), and a fire
is render-truth only if `IAnomaly::UsesRenderResidencyTruth()` — overridden `true` by `FAnomaly_StuckLowMip` alone. On m53 it also
sets how long `stuck_low_mip` stays "live" for the texture-corruption exclusion (`RunEmitsTransitionTail`), §6.

**Tests both ways:** `m52_window_selftest` **284 / 0** (282 + 2; `/W4 /WX`); a copy of the header with the old default 8
(`_r84_selftest\mutate_09001.py`) fails **exactly** the two default checks (284 checks, 2 failures); the override check passes in
both. `camera_clipping_slab_selftest` **154 / 0**.

**Builds (warm `_r84_host`):** editor **7 actions / 111 s**, game **4 actions / 98 s** (both plugin modules recompiled), **exit 0,
0 warnings**. Exe **`EB01156E`** (SHA-256 `EB01156E916E7EF8…`, 242,134,016 B).

## 2. The fix branch — checks

| check | result |
| --- | --- |
| `verify_capture.py` bare / `--label-rule` / `--label-pixel-gate` / `--change-oracle` `--selftest` | OK / **24** / 98 / 35 |
| both unittest files | OK / OK |
| kit `label_sync_check.py --selftest` | **32 OK** (67 s) |
| lever audit `--selftest` / source | 11 OK / PASS |
| G354 selftest / bundle selftest | 9 OK / 9 OK |
| lever audit `--binary` host exe `EB01156E` vs the branch | **PASS** `names=33 unknown=0 missing=0 echo=present` |
| lever audit `--binary` the five editor DLLs vs the branch | **PASS** (same counts) |
| `stage_plugin_delivery.py --ref c12b7e9` → `E:\IA_BuildCache\_r90_stage\A` | **STAGE VERDICT PASS - deliverable** (115 files; G354 PASS, lever audit PASS on the staged tree) |
| lever audit `--binary` host exe vs the staged folder | FAIL B1 on `IAI.Bench.CameraSchedule` only — **expected** (G398): the host exe is compiled with `Source/AnomalyBench` |
| lever audit `--binary` a **delivery-shaped exe** (`D6B094DD`, built from the staged folder) vs the staged folder | **PASS** `names=32 unknown=0 missing=0 echo=present`; `IAI-BENCH READY` 0, `IAI.Bench.CameraSchedule` 0 |
| string scan (UTF-16 + ASCII), host exe and the editor DLLs (union), and the delivery-shaped exe | **PASS**: every new string present once, `worst=%d`, `forcing LOD %d`, `= 8 ` and `3/8/1` absent, controls present |
| comment stripper | 0 changed / 127 |

**The delivery-shaped exe** was built by 088-01's recipe minus the ini swap and the cook: the host's plugin worktree moved aside
with `git worktree move` (run from `_r87_kit`), `Intermediate\Build\Win64\StackOBot\Development\Anomaly*` parked, the slot
junctioned to a copy of the staged folder, `Build.bat StackOBot` (6 actions, 143 s, exit 0, 0 warnings), then everything restored
(worktree back on the branch, clean; intermediates back). Only the exe was used. ⚠ **The first restore build said "Target is up
to date" (3.3 s) with the staged exe still on disk** — its timestamp beat every restored object and the makefile was the staged
build's. Moving the host exe and the game `Makefile.bin` aside forced the link (2 actions, 55 s): host exe **`4B804F41`**, same
size and objects as `EB01156E` (G201), lever audit 33/33 PASS against the branch. → G422.

**Archive:** `D:\IntrusiveAnomalies\_binary_baselines\m52fix-EB01156E\` — exe + the five editor DLLs (6/6 re-hashed at the
destination, `archive-hashes.json`), both build logs, the selftest outputs (including the mutant), the suite summary, the string
scans, the stage output and both lever-audit binary readings. Code-only: NOT cooked, NOT staged, NOT swapped in.

## 3. The client readme (fix branch)

- **New §8.7a — how far each label is proven, and which flags to drop:** one row per delivered anomaly with the content it was
  proven on, AA on/off, and the `transition_reason`s a strict set drops (`hide_return`; `partial` / `unresolved` / `temporal_aa`;
  `camera_clipping_unconfirmed`; nothing for the texture swaps and lod_popping). Below it: stuck_low_mip's partial onset (2 per
  event, 3 in a longer capture, up to 5 on the first event) and the fade-out (+4..+6 under TAA, inside the 16; exactly 0 with AA
  off); the yield; camera_clipping (unconfirmed = over-labels by design; MainWorld 200 → 2 flagged); lod_popping proven on the
  fixture only, with the content-independence argument 089-03 ruled; masks recycled at runtime (MASK55A); **Nanite targets
  labelled with boxes but no mask** (engine limit, 5.1), with the exact fields and `unmeasurable_targets_admitted`.
- **§8.6a / §8.7:** 3 / 8 / 1 → **3 / 16 / 1** (0 / 0 / 0 without temporal AA), the fade-out measurement (+4..+8 below half,
  5–45 % traces to +13..+29) and why 16; the override range.
- **camera_clipping note:** "not yet checked frame by frame against the pictures" replaced by the bench result.
- **The stuck_low_mip yield note** (Capture pool panel) said it holds a texture "no other *visible* object is using" — the legacy
  bench rule (`IAI.Bench.StuckMipLegacyPurity`). The shipping rule since 084-02 is **exactly one user component in the whole
  loaded world** (`Anomaly_StuckLowMip.cpp`, `shared_world`). Corrected; it now also says it fires rarely on shared-texture
  content and is off in Auto-pool by default (`GAutoPoolDefaultEnabled`, both branches). → G421.
- The two mask-scope lines say Nanite targets get **no mask** (they said "excluded" / "never appear", which read as "never
  targeted").
- **README Step 1 (launch) stays the marked placeholder** — the owner supplies it; the bundle checker keeps STOPping on it (G399).
- `architecture.md`: the defaults line says 3/16/1.

**Left as found, named:** the stuck_low_mip `HELD NONE` log line still says "shared with a visible component" while it counts
`shared_world` (084-11 §43 queued it for the yield round). `lod_corruption`'s apply line ("forced LOD 2 of 2") prints an ordinal,
not a 0-based index beside it, so it is outside G408's unit mix and was not touched.

## 4. State at the end of the fix-branch half

`fix/m52-label-timing` at `c12b7e9` + this docs commit (G421, G422, the status block), pushed. Host `_r84_host` warm (exe
`4B804F41`, editor DLLs = the archived ones), branch worktree clean, no junctions. Staged folder `E:\IA_BuildCache\_r90_stage\A`
and the delivery-shaped exe `…\A_exe\` (with the replaced host exe + makefile in `…\A_exe\host-replaced\`) kept as evidence.
