# 088-01 — Delivery rehearsal: both paths built as the client receives them, every packaging check run; three packaging faults found and fixed in tools and docs; nothing delivered, nothing launched (2026-09-29)

**Brief:** 088-01 (Code, Opus 5.5, headless), from `_reviews\084-11-chat-ruling-yield-plan.md` ("088-01 released now")
and `_reviews\087-01-chat-ruling-office-kit.md` decision 2 (the merge order).
**Paths:** **A** = the m52-only fallback, `fix/m52-label-timing`; **B** = the full delivery, `feat/m53-uv-normal-corruption`.
**Outcome:** both packages build, cook and pass every check that can run without a launch, once three packaging faults
were fixed (§5). No product-source fault found. The runtime half of the checklist is owed to one launch per package (§7).
⛔ No game launch, nothing took focus, nothing staged into BenchGate, no merge to `master`, no tag. `m51` untouched.
Everything the rehearsal produced is under `E:\IA_BuildCache\_rehearsal\` (evidence in `…\_rehearsal\evidence\`).

## 0. Bootstrap, in three lines

1. The checklist (`PRE-DELIVERY-CHECKLIST.md`) and 084-10 turned the packaging items into scripts: `stage_plugin_delivery.py`
   (allowlist, AnomalyBench dropped), `check_package_descriptor.py` (G354, must PASS without `--bench-cook`),
   `lever_audit.py` (+ `--binary`), `check_delivery_bundle.py` (token file + README paths); the token file is `dashboard\config.json`.
2. 085-00 housekeeping and 087-01: `.env` → token file, readme host-tools path, exclusion lines, twins hidden; merge
   kit → fix → m53 (no-ff), fresh `npm run build`, the token check on a real build.
3. The kit (`label_sync_check.py`, `OFFICE-CHECK.md`) ships in `host-tools/` through AnomDash's manifest (`ab6e63f`), which
   already requires both.

## 1. Step 0 — the merges

| merge | commit | conflicts | after |
|---|---|---|---|
| `feat/office-check-kit` → `fix/m52-label-timing` | `df63a8a` | `CLAUDE.md` status (084-11 kept on top, 087-01 below), `gotchas.md` (G395, G396, G397 in order) — both append-only, both sides kept | kit selftest **32/32 OK** (54 s) |
| `fix/m52-label-timing` → `feat/m53-uv-normal-corruption` | `9bd7316` | the same two files (fix's 084-11/087-01 entries above m53's 085-0x; G378–G394 then G395–G397); `client-readme.md` auto-merged | every suite below |

The fix head was `ebe856d` as the brief expected. No product source moved in either merge (tools and docs only).

**Suites re-run on both merged trees, all green:** A — Python 11/11 (verify_capture bare / `--label-rule` 24 / `--label-pixel-gate`
98 / `--change-oracle` 35, both unittest files, lever audit selftest 11 + source PASS, G354 selftest, bundle selftest, kit
32/32) and C++ `m52_window_selftest` 282/0, `camera_clipping_slab_selftest` 154/0 (`/W4 /WX`). B — the same 11 plus
`exclusion_gate` / `stuck_mip_destructor_check` / `m53_glue_check` / `census_readonly_check` (selftests and source
verdicts, 18/18), C++ `texcorrupt_pure_test` 251/0, `texcorrupt_draw_test` 978/0, `exclusion_selftest` 243/0,
`active_source_selftest` 47/0, m52 282/0, camera 154/0, and **all 23 mutants fail as required** (tcmut, draw1–8, excl1–11,
src1–3). The C++ suites were built from copies of `_r53_selftest\Y\all.bat` in `_rehearsal\work\` so 085-02c's evidence
folder was not overwritten.

## 2. How each package was built (exactly the staged plugin, Development)

For each path: `stage_plugin_delivery.py --ref <head>` → `_rehearsal\<A|B>\plugin\AnomalyInjector` (A `df63a8a`: 115
files; B `9bd7316`: 129; both **STAGE VERDICT PASS - deliverable**, AnomalyBench / docs / tools / CLAUDE.md / WebClient
excluded). Then, on the warm scratch hosts (A on `_r84_host`, B on `_r53_host`), so no engine rebuild was needed:

1. the host's plugin worktree was moved aside with `git worktree move` (run from another worktree — from inside the folder
   Windows refuses the rename), and its `Intermediate\Build\Win64\StackOBot\Development\Anomaly*` parked;
2. the host's plugin slot became a junction to a build copy of the staged folder (the staged folder itself stayed pristine);
3. the host's `Config\DefaultGame.ini` was swapped for a **delivery-shaped** one: a fresh random 64-hex token per path
   (never printed), the m27 block with `bDeliveryModeDefault=True`, `[AnomalyInjector] +ExcludedTargetNamePatterns=Decal`
   / `_CR_`, and the never-cook lines — all three (§5.1);
4. `_r84_host` got its temporary `Content` / RoomGenerator `Content` junctions to the D: project (as in 084-07b);
   `_r53_host` has permanent ones. A path+size+mtime manifest of D: Content (4,966 entries) before and after each cook:
   **identical both times**;
5. `Build.bat StackOBotEditor` then `StackOBot` (Win64 Development), then `RunUAT BuildCookRun -cook -stage -pak -archive
   -build -nocompileeditor -map=MainMenu+MainWorld` into `_rehearsal\<A|B>\game`;
6. everything restored: ini (hash `3CF789B92FDE` again), slot junction removed, parked intermediates back, worktree moved
   back (A on the branch, B detached at `270ed32`, both clean), `_r84_host`'s content junctions deleted.

| | editor | game | cook + stage + pak | packages | exe (SHA-256 first 8) |
|---|---|---|---|---|---|
| A | 13 actions, 138 s, 0 warnings | 6 actions, 109 s | 68 s, **0 errors, 0 warnings** | 792 | `6D1B659F` |
| B | 13 actions, 114 s, 0 warnings | 6 actions, 113 s | 80 s, **0 errors, 0 warnings** | 795 | `C18D0F41` |

Cooked maps (container index): `MainWorld`, `MainMenu`, `/Engine/Maps/Entry` on both. ⚠ The host still carries its own
bench plugin CaptureBench (the host's, not the delivery's); a client host would not.

**The AnomDash bundle:** `npm run build` fresh (exit 0; `dist/` had been a month older than `src/`), then
`make_delivery.py --dest _rehearsal\<A|B>\bundle --plugin-repo <branch> --token-ini <the COOKED DefaultGame.ini extracted
from that package's pak>`: **BUNDLE COMPLETE, 14/14 entries, 30 files**; `dist\config.json` (the dev token) **removed**;
the delivered build's token written and read back.

## 3. The checks on the built packages

| check | A | B |
|---|---|---|
| G354 `--build` (no `--bench-cook`) | **PASS** — 4 modules identical, plugins identical, FIXTURE `hits=none`, descriptor SHA-256 `62E6B9AD…` (= 084-10's staged descriptor) | **PASS** — same descriptor (m53 does not change the `.uplugin`), FIXTURE `hits=none` |
| lever audit `--binary`, the checklist's branch root | **FAIL B2** (a false fail — §5.2) | **FAIL B2** (same) |
| lever audit `--binary --root <staged folder>` | **PASS** `names=32 unknown=0 missing=0 echo=present` | **PASS** `names=45 unknown=0 missing=0 echo=present` |
| exe strings (UTF-16/ASCII) | `IAI-BENCH READY` 0/0; both gate echoes present; `uv_/normal_corruption` 0 | `IAI-BENCH READY` 0/0; echoes present; `IAI.TexCorrupt.Census` present |
| cooked `DefaultGame.ini` (extracted with UnrealPak) | token 64 chars; `bDeliveryModeDefault=True`; no `ContentClockDefault`; Decal/_CR_; 3 never-cook lines | identical to A but for the token |
| cooked `DefaultEngine.ini` | `GameDefaultMap` = MainMenu (StackOBot's own map); no exposure / Nanite-proxy keys | identical to A |
| container paths: `AnomalyFixtures` / `CaptureBenchGate` / `CaptureBenchTexCorrupt` / `/CB_` / `AnomalyBench` | 0 / 0 / 0 / 0 / 0 | 0 / 0 / 0 / 0 / 0 |
| bundle token vs the cooked ini | **PASS** | **PASS** |
| can-fail: same bundle vs the dev ini | **STOP TOKEN** | **STOP TOKEN** |
| can-fail: B's bundle vs A's build | — | **STOP TOKEN** |
| bundle README placeholders (new check, §5.3) | **STOP** line 61 (LAUNCH stub) | **STOP** line 61 + line 1200 (`(rate: to be measured)`) |

**Strings attributed, not faults:** `CB_GateLevel` (4 UTF-16) is the named-map fixture gate in capture / control server / the
bench twin, behind `-IAIBenchFixture` (084-10 kept it deliberately); `AnomalyBench` is the control server's lazy
`LoadModulePtr` (null without the module); B's `/Game/CaptureBenchTexCorrupt/` (2) is the refusal prefix of the swept
lever `IAI.Bench.TexCorruptAssetSlotMid` (m53's checklist names it). `null_effect` / `solid_swap` are compiled into
Development and registered only under the gate (R4 source proof); their runtime absence is G-LEVER's. `Hidden_shrine`
(31 container entries) is MainWorld's own rock texture set, not bench content.

**The readme walked as a client** (on copies of each bundle, with the client's `py.exe`, Python 3.13.1):
§1 prerequisites met; **§2 `Setup.bat` PASS** — ffmpeg found on PATH (the download path was not exercised), Python found,
captures folder set, `controlToken = (present, unchanged)`, `config.json` fetchable over localhost HTTP, no BOM;
**Step 1 FAIL** — the LAUNCH stub is unfilled (§5.3); **Step 2 `Run.bat` not run** (it opens three windows and a browser):
every file it names exists, and `selfcheck.py` runs and reports all three services not running; **Step 5** `verify_capture.py
--selftest` OK; **Step 6** `--label-pixel-gate --selftest` 98 OK (with `measure_label_offset.py` beside it); **Step 7**
`label_sync_check.py --selftest` **32/32 OK** from the bundle (60 s A, 51 s B); every host-tool compiles.

## 4. A against B

- **Plugin:** 29 files differ, and git and a folder hash agree: 3 m53 assets (`M_CorruptTex_Normal`, `M_CorruptTex_UV`,
  `T_CorruptTex_NoiseN`), 11 new m53 sources (the TexCorrupt family, `AnomalyExclusion.h`, `AnomalyTexCorrupt.h`,
  `AnomalyActiveSource.h`) and 15 shared sources. **Every one of the 19 non-merge commits behind them is m53-round work**
  (TexCorrupt S1, its 082-06b/c review fixes, the 085-02/02b/02c delivery cut). Notable shared-code changes, all m53 by
  design: `Anomaly_StuckLowMip.*` (the m52 ⟂ m53 exclusion, DC item 6), `AnomalyInjector.Build.cs` (+`RenderCore`, `RHI`,
  private, 082-05 ruling), and `1067481` in capture (the subtype source), whose branch is gated on `IsTexCorruptId`. The
  descriptor is identical.
- **Bundle:** 28 of 30 files identical; `README.md` differs in 7 hunks, **all m53**: "Seven" → "Nine" types, the two pool
  rows, "five unticked" + the m53 note, `IAI.TexCorrupt.Census`, the two §8.2 cells, the §8.7 row, and the new §8.8.
  `dashboard\config.json` differs by the token (each build's own, by design).
- **Package:** B = A + the 3 m53 assets; the only changed common chunks are the two shader archives, `MainWorld.umap` and
  the container header (G371's per-cook variance, plus the new materials' shaders). Cooked configs equal but for the token.
- **Nothing unexpected.**

## 5. Packaging faults found and fixed (tools and docs, on `fix/m52-label-timing`; m53 inherits them by merging the fix branch again)

1. **`048d6c8` — the never-cook list and G354's needles missed m53's fixture folder.** The bench project's Content holds
   `AnomalyFixtures`, `CaptureBenchGate` and, since 082-05, **`CaptureBenchTexCorrupt`** (`CB_TexCorruptLevel` + ~110 `TC_*`
   assets). The printed exclusion lines and the checklist named only the first two; G354's needles caught the TC map but
   not a TC asset without it. Sized honestly: **no shipping map or asset references any of the three** (1,888 files
   scanned), so no package carried TC content — the guard was incomplete, not leaking. Now three lines; `CaptureBenchTexCorrupt`
   is a needle; selftest 9 cases; proven both ways (old needles PASS a planted TC asset, new ones STOP it). → G400.
2. **`acfc46d` — the checklist's binary lever audit FAILED a correct delivery exe.** It ran from the branch root, which
   still holds `Source/AnomalyBench`; the delivered exe is compiled from the staged folder, so B2 read AnomalyBench's own
   `IAI.Bench.CameraSchedule` as "stale build" on both paths. `--root <staged folder>` PASSes both. The checklist now gives
   `--root`, and `stage_plugin_delivery.py` prints the exact command for the folder it staged. → G398.
3. **`acfc46d` — the bundle README ships an unfilled Step 1 LAUNCH stub, and nothing stopped it.** The only box asking for
   it sat in the superseded Tauri §3; `check_delivery_bundle.py` checked presence and script paths only. It now STOPs on
   the stub and on any value "to be measured"; selftest 9 cases; both rehearsal bundles STOP; a copy with the stub filled
   PASSes. New checklist §2 box. → G399.

**Product faults: none found.** B's README line 1200 `(rate: to be measured)` is m53 documentation waiting on a measurement
(m53's own checklist already forbids shipping it); it is reported, not filled.

## 6. Checklist boxes

**PASS on both** (static or package evidence): G354 step 1 (+ SHA recorded) · F9 AnomalyBench excluded (staged + package) ·
G-LEVER-AUDIT selftest and binary (with `--root`) · source-ini token (the rehearsal cook's ini) · dashboard token = the build's
cooked token · `bDeliveryModeDefault=True` · no `ContentClockDefault` · Development, not Shipping · `GameDefaultMap` a real
game map (stand-in) · Decal / `_CR_` exclusions · the editor target builds exit 0 before the cook · README §8 v2 tables +
§8.5 changelog in the bundle (byte-identical to the branch) · `Setup.bat` (ffmpeg, Python, captures folder) · `Setup.bat`
keeps the token · the build forces no exposure. **B only:** no m53 bench fixture ships · the comment strip (0 changed on
`9bd7316`) · G-LEVER-AUDIT on THIS delivery ref (with `--root`).

**FAIL:** the new README-placeholder box — both (LAUNCH stub), B also (§8.8 rate). B's "no `(rate: to be measured)`"
box. Both are fill-ins, not builds.

**MANUAL (the owner, Thursday, or one launch per package):** everything read from a running build or a captured session —
`IAI.ListAnomalies` with no twins; `IAI bench levers: DISABLED … preset=0` and no AnomalyBench / bench-gate lines; the token
**read back from the build's log** (the brief's "real build's log": a launch, which this brief forbade — §7); the grab-point
line; `MaskProbe EFFECTIVE=0`; the delivered pool line (and, B, neither m53 id in it); B's census on the delivery build (cold
and warm) and its `IAI.TexCorrupt.Census` grammar; B's label rule and m52/m53 overlap counter on a delivered session;
every m41 / m43 / m45 / m47 / m48 / m49 / m50 / phase B session box; the §4 dry-run boxes that need the game (`Run.bat`,
a real capture end to end, G76); §5 (the delivered session) and §6 (the security note at handover). **Host boxes** (office
host, not StackOBot): Support Nanite off, no host custom depth, the H6 note, G-R7(ii) physical, C-G1b, B-G1, the client
decal note, the changelog paragraph in the delivery note. **N/A:** the superseded Tauri §2/§3, WebView2, SmartScreen.

## 7. Deviations and open items

- **Token check on a log:** a real Development build's log needs a launch (`IAI.Server.Start`); the brief forbade one. The
  strongest no-launch source was used — the **cooked** `DefaultGame.ini` extracted from each package's own pak (what the
  build enforces, G118) — and the can-fail directions were shown on the real bundles. The log read-back is owed.
- **The host carried CaptureBench** (the bench host's own plugin) in both packages; the client's host will not.
- **ffmpeg's download path** was not exercised (ffmpeg is on PATH here).
- **G-LEVER** (tonight) is the runtime half for the bench build; the delivered packages themselves have not run.

## 8. State and hand-off

- `fix/m52-label-timing`: `df63a8a` (kit merge) → `048d6c8` → `acfc46d` → this docs commit, pushed. Then merged into
  `feat/m53-uv-normal-corruption` again, pushed. No tag, no merge to `master`.
- Hosts restored exactly: `_r84_host` on the branch, `_r53_host` detached at `270ed32`, both inis `3CF789B92FDE`, no
  leftover junctions; D: Content unchanged. The next plugin build on either host recompiles the plugin once.
- Kept in `_rehearsal\`: both staged plugins, both packages (`A\game`, `B\game`, ~2.45 GB each), both bundles (they carry
  the rehearsal tokens — rehearsal-only values, never the dev token), the cooked configs and `evidence\`.
