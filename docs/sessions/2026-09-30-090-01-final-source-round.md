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

## 5. The merge into m53 (`3ca5dcb`, no-ff)

`fix/m52-label-timing` `0347e8c` → `feat/m53-uv-normal-corruption` `620e3cb` (merge-base `5c2bc83`; the fix side brought
`c12b7e9`, `0347e8c` and the 084-09 / 089-01 / 089-02a / 089-03 docs). **Conflicts: `CLAUDE.md` (status) and
`docs/gotchas.md`** — both append-only, both sides kept: the fix branch's status blocks above m53's 089-04 block (the 088-01
precedent), the gotchas in number order G401–G422 (`_r84_selftest\resolve_09001.py`: exactly one conflict per file, no
duplicate number). **Source and `client-readme.md` auto-merged**; the source delta `620e3cb..3ca5dcb` is exactly the fix
branch's four files (+20 / −12), and the readme's §8.7a landed before "Reading `labels.jsonl`", with m53's §8.8 intact.

| suites | before (`620e3cb`) | after the merge (`3ca5dcb`) | after the test fix (`d57246c`) |
| --- | --- | --- | --- |
| `texcorrupt_pure_test` / `texcorrupt_draw_test` | 251 / 0 · 978 / 0 | same | same |
| `exclusion_selftest` | 243 / 0 | **243 / 7 FAIL** (§6) | 243 / 0 |
| `active_source_selftest` | 47 / 0 | same | same |
| `m52_window_selftest` / `camera_clipping_slab_selftest` | 282 / 0 · 154 / 0 | **284 / 0** · 154 / 0 | 284 / 0 · 154 / 0 |
| the 23 mutants (tcmut, draw1–8, excl1–11, src1–3) | 23 / 23 fail as required | 23 / 23 | 23 / 23 |
| Python: verify_capture ×4, 2 unittests, lever ×2, G354, bundle, kit 32, exclusion gate, destructor ×2, glue ×2, census ×2 | 18 / 18 OK | — | 18 / 18 OK |

(C++ built from a copy of `_r53_selftest\Y\all.bat` in `_r53_selftest\Z09001\`, so 085-02c's evidence folder is untouched.)

## 6. The exclusion selftest hard-coded an 8-frame tail (`d57246c`)

The F1 scenarios in `tools/exclusion_selftest.cpp` read `AnomalyLabelSync::DefaultOffFramesTemporal` but asserted the frame
numbers an **8**-frame tail produces: transition entries on 102..109, m53 admitted at 111, a carried tail on the next run's
0..3 with admission at 5, and the out-of-order window done by 109. With the merged default 16 the product does exactly what
089-01 ruled — `stuck_low_mip` stays live through its 16 flagged frames and m53 is admitted after them (entries 102..117,
admission 119; carry 0..11, admission 13) — and 7 checks failed on the old literals. **Not a merge conflict and not a product
change:** the expectations are now derived from `Off` (102..101+Off, admission 103+Off, carry 0..Off−5 / admission Off−3,
the loops sized to reach them). **Both ways:** 243 / 0 at 16; the same file compiled against a header copy with `Off = 8`
(`Z09001\off8_check.py`) is 243 / 0, i.e. it reproduces the old numbers; all 11 exclusion mutants still fail. → G423.

The product side needed nothing: `RunEmitsTransitionTail(bRenderTruthRun, LabelOffFrames)` and the exclusion's `label_tail`
state read the run's resolved value, so the texture corruptions now wait up to 16 frames (was 8) behind a `stuck_low_mip`
tail under temporal AA. §8.8's "up to 8 more frames" says 16.

## 7. The client readme on m53 (§8.8, §8.7a)

- **§8.8 "How far these labels are proven":** label sync proven on the fixture, all four modes, two independent captures
  (085-04: 4 modes × NAT / NAT_AAOFF / SYN, 17 / 17 at +0 / +0; 089-04: the regression), arms caught (CD3 / CD5 / RD3 / RD5);
  mode correctness proven (089-04b G-MODE 4 / 4, every wrong mode < 0.5); 0 co-entry frames with `stuck_low_mip`;
  **ordinary scenery not checked from pixels this week** — the only framable eligible MainWorld hosts are Nanite (12 / 12
  `reason=nanite`), labelled per the unmeasured contract, no wrong-object change; the mechanism is `corrupted_texture`'s swap.
- **The "barely changes" rate per mode: "not measured on realistic content"**, with the fixture reading (0 of 17 per mode,
  085-04 ND-5) and why the large test level could not give one; the `(rate: to be measured)` placeholder is gone (the bundle
  checker's placeholder scan now STOPs on the Step 1 stub only: 1 line, was 2).
- **The 128 MiB default and the MainWorld example:** the floor object's 219,541,488 B need (kept) plus 089-02b's settled-view
  reading — none eligible at 128 MiB (uv: over_budget 2, partial_footprint 1, texture_not_parameter 2; normal: partial_footprint
  3, texture_not_parameter 2), two uv hosts at 256 MiB (~214–220 MB each), normal none at any cap — and what a raised cap costs.
- **§8.7a** gains the `uv_corruption` / `normal_corruption` rows.
- **Step 1 stays the owner's placeholder.**

## 8. m53 builds and checks

**`_r53_host`** moved with `git checkout --detach d57246c`, plugin products cleared (both plugin folders and
`Intermediate\Build\Win64\StackOBot\Development\Anomaly*`): editor **16 actions / 157 s**, game **7 actions / 92 s**, **exit 0,
0 warnings**. Exe **`CE55D997`** (SHA-256 `CE55D99703C5FFA0…`, 242,454,528 B).

| check | result |
| --- | --- |
| lever audit `--binary` exe vs the branch / the five editor DLLs | **PASS** `names=46 unknown=0 missing=0 echo=present` / PASS |
| string scan (exe; editor DLLs as a union) | **PASS** (new strings present, old absent); `IAI.TexCorrupt.Census` 4, `excluded_partner_live` 2 |
| `stage_plugin_delivery.py --ref d57246c` → `_r90_stage\B` | **STAGE VERDICT PASS - deliverable** (129 files; G354 PASS, lever audit PASS on the staged tree) |
| lever audit `--binary` a **delivery-shaped exe** (`5E44E39F`, built from the staged folder) vs that folder | **PASS** `names=45 unknown=0 missing=0 echo=present`; `IAI-BENCH READY` 0, `IAI.Bench.CameraSchedule` 0, `IAI.TexCorrupt.Census` 4; string scan PASS |
| lever audit `--binary` the host exe `CE55D997` vs the staged folder | FAIL B1 on `IAI.Bench.CameraSchedule` only — expected (G398; compiled with AnomalyBench) |
| comment stripper | 0 changed / 149 |

**Archive:** `D:\IntrusiveAnomalies\_binary_baselines\m53-09001-CE55D997\` — exe + the five editor DLLs (6/6 re-hashed,
`archive-hashes.json`), both build logs, the C++ and Python suite summaries before / after the merge / after the test fix, the
7-failure exclusion output, the Off = 8 check, both lever-audit readings and the string scans. Code-only: NOT cooked, NOT staged,
NOT swapped in.


**The delivery-shaped exe** was built the same way as §2's (088-01's recipe minus the ini swap and the cook): the host's
detached worktree moved aside from `_r87_kit`, `Anomaly*` intermediates parked, the slot junctioned to a copy of `_r90_stage\B`,
`Build.bat StackOBot` (6 actions, 104 s, exit 0, 0 warnings). Restore applied G422 from the start: after the worktree and the
intermediates were back, the staged-linked exe and the game `Makefile.bin` were moved aside before the relink (game 2 actions,
30 s; editor up to date): host exe **`1A99DDDA`**, the same size and objects as `CE55D997` (G201), lever audit 46 / 46 PASS
against the branch. The makefiles stay in `_r90_stage\{A,B}_exe\host-replaced\`; the moved-aside exes were byte-identical to
`_r90_stage\{A,B}_exe\StackOBot.exe` and were deleted as duplicates (E: stood at 47.4 GB free; 48.0 after), which corrects §4's
"replaced host exe" wording.

## 9. State at the end

- `fix/m52-label-timing` **`0347e8c`**, pushed (§4). `feat/m53-uv-normal-corruption` **`3ca5dcb`** (merge) → **`d57246c`** (test
  fix + readme) → this docs commit, pushed. No tag, no merge to `master`; `m51`, `master`, `ToCodex\`, `E:\AmmaYT` untouched.
- `_r53_host` warm, plugin worktree detached at `d57246c`, clean, its permanent Content junctions unchanged; `_r53_src` on the
  branch, clean. `_r84_host` as §4.
- **For Thursday: pull `feat/m53-uv-normal-corruption` at this commit** (the full delivery: every m52 fix + m53); the m52-only
  fallback is `fix/m52-label-timing` `0347e8c`. Both still need the owner's README Step 1 and the final delivery build (cook,
  package, launch check), which this brief did not do.
