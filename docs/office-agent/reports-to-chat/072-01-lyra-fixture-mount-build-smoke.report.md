# REPORT for 072-01-lyra-fixture-mount-build-smoke.md
brief: 072-01-lyra-fixture-mount-build-smoke.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-04 14:10:56 | finished: 2026-09-04 15:30:47 | duration: 80 min | exit code: 0
context at last turn: 470014 tokens (47% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\072-01-lyra-fixture-mount-build-smoke.live.txt

---- FINAL MESSAGE (Claude Code) ----
Everything verified. Final report.

---

## 1. Plain-language rundown

1. **The plugin now runs in a second game.** Lyra is mounted, builds clean, and produced a full labelled capture session — the whole point being that Lyra is host-shaped in ways StackOBot cannot be.
2. **The build is itself a result**: exit 0, zero errors, zero warnings, **no host edit and no plugin edit** — the game-agnostic invariant held against TSR + Lumen + hardware ray tracing + GameFeature plugins.
3. **The verifier ran unmodified on a second game** and returned a clean verdict set: 5 PASS, 0 SHIFT, 0 NOT-VISIBLE, 1 NOT-MEASURABLE (the truncated last event — the same reason as every StackOBot run).
4. **The client's timing shapes did not reproduce here** — but that is a 6-event smoke, not a survey, so "not seen" is not "absent".
5. **Four things did show that the bench structurally cannot show**, and those are the deliverable.
6. **The host-PP custom-depth detector fired for the first time ever** — Lyra's own outline material reads custom depth *and* stencil, through the exact code path we added blind in session 069.
7. **One of our own checks accuses us wrongly on this host**: census hygiene calls the leg a `P-C6` failure for a write that is almost certainly Lyra's, and the evidence proving it isn't ours is already printed and simply unused.
8. **The Phase A precondition reproduced, and is sharper than we had it**: `mask_value: 0` isn't sporadic — it hits the *first labelled frame* of every texture-class event and neither hide-class event, i.e. exactly the frame the client's onset findings are about.
9. **Lyra sits naturally at `speed_ratio` 1.114** — the client band that the bench has to be stalled into.
10. **Nothing found was fixed**, by instruction; it is all written down with nine proposed gates.

---

## 2. Audit (§1)

| axis | finding |
|---|---|
| EngineAssociation | **`{B34F356C-…}` — the SAME GUID as StackOBot** ⇒ same source-built UE 5.1. **This is what makes a junction unsafe.** |
| Modules | `LyraGame` (Runtime) + `LyraEditor` (Editor) — C++ project confirmed |
| Plugins / disk | 67 `.uproject` entries, 13 project plugin folders, 5 GameFeature plugins · `E:` 358.2 GB free |
| `r.CustomDepth` | **`3` set BY THE HOST** (`DefaultEngine.ini:77`) |
| custom-depth/stencil readers | **13 assets.** Load-bearing: **`MPP_Outline`**, a *post-process* material reading **CustomDepth + CustomStencil**; plus `B_Hero_Default`, `B_Weapon`, `B_Hero_ShooterMannequin` (stencil), `B_Manny`/`B_Quinn`/`B_TinplateUE4` + 6 Niagara (depth) |
| AA | **`r.AntiAliasingMethod=4` = TSR** |
| GI/refl/RT/shadows | Lumen GI + Lumen reflections, `r.RayTracing=True`, `r.Lumen.HardwareRayTracing=True`, VSM |
| static lighting | `r.AllowStaticLighting=False` — **same as StackOBot ⇒ `G157` carries over** |
| auto exposure | no key ⇒ **engine default ON**; `ExtendDefaultLuminanceRange=True` ⇒ `m48` live |
| **Nanite (G134)** | **459/4,568 (10.0 %)** ShooterMaps static meshes, incl. all of `Convolution_Blockout`; **8/59 (13.6 %) in `/Game` — including ALL FOUR weapon meshes** `SM_Rifle`/`SM_Pistol`/`SM_Shotgun`/`SM_grenade`, the held-weapon class the Concorde symptom sat on |
| bots | **not in any map — they come from the Experience.** `B_ShooterGame_Elimination` + `B_LyraShooterGame_ControlPoints` carry `B_ShooterBotSpawner` |
| foliage / fog | foliage in `L_ShooterGym`, `L_Convolution_Blockout`, `L_FiringRange_WP`, both TopDown, both ShooterTest · **fog ONLY in `L_ShooterFrontendBackground`, which has no experience and no bots** |
| World Partition | `L_Expanse*`, `L_Convolution_Blockout`, `L_FiringRange_WP`, `L_ShooterFrontendBackground` |
| `GameDefaultMap` | `L_LyraFrontEnd` — the `G87` trap |

🚨 **A stale June-2026 clone was already at the mount point** (real `.git` *directory*, no remote, 14 dirty paths, 277.8 MB). **Moved, not deleted**, only after proving `e7fcabe` is an ancestor of `master` and every dirty path exists in `master` today — one had merely been **renamed** (`create_missing_texture_materials.py` → `create_anomaly_materials.py` at `1ccfca1`).

**Map chosen: `L_ShooterGym`** — Elimination experience (⇒ bots), has foliage, **not** World Partition (deterministic load, no streaming confound on a first run). ⚠ **"foliage/fog" is only half-satisfiable**: the sole fog map has no experience and no bots.

---

## 3. Mount + build

```
git worktree add --detach E:\UEProjects\LyraStarterGame\Plugins\AnomalyInjector 9083d95
```
**DETACHED deliberately** — a second worktree can't hold `master` while the main checkout does, and a detached HEAD can't move a branch. ✅ **`.gitignore` STOP CONDITION NOT TRIGGERED** (checked *before* mounting: `Binaries/`, `Intermediate/`, `Saved/`, `DerivedDataCache/`; four `check-ignore` probes). `.uproject` backed up to `.bak`, one entry added, re-parsed as valid JSON (68 plugins).

**Build: `LyraEditor Win64 Development` — 13 actions, 160.4 s, `EXITCODE=0`, zero errors, zero warnings.** All four modules: `AnomalyShaders` 62 KB · `AnomalyInjector` 680 KB · `AnomalyCapture` 1,093 KB · `AnomalyControlServer` 246 KB. TICKPIN probed NOT FOUND with its evidence (stock engine — correct).

**Untouched proof (hashed before *and* after):** main checkout `master`, **0 tracked modifications**; staged exe **`DE65F84A`**; quartet **`EF8EB23C` / `A8BFFF88` / `3C026A8D`** + `A16A18A8`/`C70ECDAA`. Worktree `git status` **empty**; **no file under Lyra's `Source/` has today's date**. The only host change in the session is the one `.uproject` line.

---

## 4. Session + readouts

**Recipe:** `/ShooterCore/Maps/L_ShooterGym` · `-game -windowed 1920×1080` · **auto-pool** · seed 4242 · 90 frames · SVE · delivery OFF · paced 30 · **`Config 2 4 8 4 0`** · run log ON · census/mask/target-mask at compiled defaults · **AA and exposure NOT pinned** (host-shaped leg, not an arbiter). Banked `_bench_sessions_bank\LYRA_SMOKE_01`, **131/131 files verified by path+size**.

**Two failed attempts, both recorded.** (1) exit 2 — `WaitSec` expired during a **cold global-shader compile** (`FTSRRejectShadingCS` alone 77.8 s). (2) exit 2 — **my error**: I launched at `/ShooterMaps/…`; `L_ShooterGym` lives in **`ShooterCore`**, and Lyra's own ini said so and I overrode it. That log also showed the plugin **initialising correctly before the map failed**, and a crash in **`ULyraPerformanceStatSubsystem::Deinitialize`** — whole callstack Engine + LyraGame, **no plugin frame; not attributed to us, not investigated**. (3) exit 0.

```
Capture run FINISHED | 90 frame(s) (positive=43) | 7 burst(s), 2 zero-match
```

**Camera (measured):** origin moved **1,213.9 cm total path**, spread (386.4, 965.5, 6.4) cm, one **977.4 cm** single-frame teleport, 16 frames > 1 cm; **`view.rot` has exactly ONE distinct value** — position moved, direction never did. **The first bench leg in this project where the camera moved.**

### §5.1 Verifier — **PASS 5 · SHIFT 0 · NOT-VISIBLE 0 · NOT-MEASURABLE 1**, exit 0

```
MEASURABLE RANGE +/-2 frames (min clean gap 4)   baseline frames 24
idx=0 blink             Cube7        PASS  [LOW]  tau=0.1321 CONTAMINATED=1
      run[16..17] d(onset=16)=0.7575 d(clear=18)=0.5512  region=mask(v219)/247837px
      run[21..22] d(onset=21)=0.5451 d(clear=23)=0.5414  region=mask(v219)/248341px
idx=1 corrupted_texture Cube4        PASS  [HIGH] tau=0.1279  appearance=MAGENTA
      run[27..34] d(onset=27)=0.9915 d(clear=35)=0.9946  region=mask(sole v220)/141575px
idx=2 blink             Cube4        PASS  [HIGH] tau=0.1329
      run[40..41] d(onset=40)=0.2517 d(clear=42)=0.1459  region=mask(v221)/152132px
      run[45..46] d(onset=45)=0.1609 d(clear=47)=0.1482  region=mask(v221)/152693px
idx=3 missing_texture   B_Manny_C_14 PASS  [LOW]  tau=0.3957 CONTAMINATED=1 appearance=CHECKER
      run[51..58] d(onset=51)=0.9891 d(clear=59)=0.9816  region=mask(sole v222)/27348px
idx=4 corrupted_texture Cube3        PASS  [LOW]  tau=0.0248 CONTAMINATED=3 appearance=MAGENTA
      run[63..70] d(onset=63)=0.9979 d(clear=71)=0.9759  region=mask(sole v223)/125497px
idx=5 missing_texture   Cube3        NOT-MEASURABLE [LOW] tau=0.0373 CONTAMINATED=3 appearance=OTHER
      run[87..89] d(onset=87)=1.0000 d(clear=-)=n/a — end truncated by the session's last frame
```
🚨 **The host-shaped reading is the CONFIDENCE column: 4 of 6 events are `[LOW] CONTAMINATED`** because the camera moves, so "clean" frames carry real change and τ rises. **The instrument degraded loudly, in the safe direction.** ⚠ `idx=3` is a **`SkeletalMeshComponent`** reading `appearance=CHECKER`. ⚠ `idx=5` reads `appearance=OTHER` — reported, not chased.

### §5.2 m26 / mask
`vetoed_events 0` · **`VETOED-OBJECT` 0** · `translucent_vetoes 0` · `non_manifested_events 0` · `mask_nopass_discards 0` · `mask_residual_discards 0` · `pattern_excluded_targets 0`.
`target_mask_frames` **measured 34 / hidden_blank 0 / unavailable 56** (=90 ✔). `mask_state`: **present 34, unmeasured 56** — ⚠ **by design since `m44`** (masks only on frames labelled for that event; 47 frames are negatives). **MASK-TIE 34 lines, 34 MATCH, 0 MISMATCH.** All six `M26S1 EVENT` = `MEASURED_NONZERO`, every bucket clean, `pctOfFrame` 11.98/6.72/7.37/**1.31**/6.14/4.29.

### §5.3 Census
```
SUMMARY frames=48 cycles=48 candidates=14 zero=2 nonzero=10 belowFloor=4 aboveCeiling=2
  excludedTranslucent=0 nanite=0 tagFailed=0 hidden=2 firesPartialFallback=6
  hostPpCustomDepthReaders=1 framesNoPass=0 framesPolluted=0 batchesLost=0
  tagOvertaken=4 proxyRecreatesQueued=534 tagBlockMsTotal=138.807
M36 STENCIL RESERVATION ON - reserved=0 [ ]
```
🚨 **`HOST-PP CUSTOM-DEPTH READERS = 1 [ MaterialInstanceDynamic_365(depth+stencil) ]` — `(scanned 0 volume(s), 1 camera blend(s), 1 blendable entr(ies), 1 material(s), 1 of which read some scene texture)`.** **First non-zero on any host, and it fired through the CAMERA-BLEND source** — the third post-process source journal 069 `C-G1a` found missing and added, which until now could only be shown *not to crash*. ⛔ **`G120`: no artifact observed.**

🚨 **`CENSUS-HYGIENE final DIFF n=3 first=B_Hero_ShooterMannequin_C_12/CharacterMesh0 gained bRenderCustomDepth (value 0) … the leg FAILS P-C6.`** Leak probe was OFF, so by its own wording we failed. **We almost certainly leaked nothing — the value is `0` and we only write `200..254`.** Lyra's heroes enable custom depth themselves for the outline system.

⚠ `firesPartialFallback` **6 of 7** (StackOBot: 0) · `tagOvertaken 4` with `framesPolluted 0` / `batchesLost 0` · `census_unmeasurable_nanite 0` — **this map's candidate set had no Nanite**, so §2's exposure is real but did not bite.

### §5.4 Per event

| idx | type | target | asset | component_class | cov% | frames | mask_state | bbox_valid FALSE |
|---|---|---|---|---|---|---|---|---|
| 0 | blinking | Cube7 | Cube | StaticMeshComponent | 46.84 | `[16,17,21,22]` | present 4 | none |
| 1 | corrupted_texture | Cube4 | Cube | StaticMeshComponent | 10.24 | `[27..34]` | present 8 | none |
| 2 | blinking | Cube4 | Cube | StaticMeshComponent | 10.24 | `[40,41,45,46]` | present 4 | none |
| 3 | missing_texture | **B_Manny_C_14** | **SKM_Manny** | **SkeletalMeshComponent** | 5.55 | `[51..58]` | **present 7, unmeasured 1** | none |
| 4 | corrupted_texture | Cube3 | Cube | StaticMeshComponent | 19.88 | `[63..70]` | present 8 | none |
| 5 | missing_texture | Cube3 | Cube | StaticMeshComponent | 16.49 | `[87,88,89]` | present 3 | none |

**`bbox_valid` TRUE on all 43 entries.** `missing_object`/`lod_popping` never drawn (`Auto.Fire: 'lod_popping' on 'Cube4' -> 0 matched`). All 4 reverts: `restored=N … **left-to-game=0** … swept=0`.

🚨 **F4's shape occurred naturally: event 3's `si 53` is inside `51..58` with NO mask** (PNGs exist for 51,52,54–58) — and **`bbox_valid` stayed TRUE**, i.e. the projector never signalled it, which is exactly why F4 needs `target_pixels`.

🚨 **Phase A precondition — reproduced and sharper than 071 §11.2:**

| si | row `mask_value` | PNG values | first labelled frame |
|---|---|---|---|
| 16 | **219** | `[219]` | yes |
| 27 | **0** | `[220]` | yes |
| 40 | **221** | `[221]` | yes |
| 51 | **0** | `[222]` | yes |
| 63 | **0** | `[223]` | yes |
| 87 | **0** | `[224]` | yes |

⇒ **first labelled frame of ALL FOUR `FireWindow` events, NEITHER `ActorHidden` event (4/4 vs 0/2)**. Not sporadic — an **onset-frame join gap with a clean class split**, on the frame F1/F3/F6 are about. ⛔ Class split MEASURED, mechanism NOT asserted.

**Provenance — every event `valid: true`, and the occlusion numbers are the story:** samples passed **3/9, 3/9, 3/9, 4/9, 2/9, 1/9**; `poll_distance` **negative on 3 of 6** (incl. `B_Manny_C_14` at −74.06). ⇒ **`P-a1` finally has a fixture at its boundary** (one target admitted on the bare minimum clear ray), and **`G124` reproduced on a character**.

⚠ Minor artifact inconsistency, filed: `mask_map.json` says `last_frame 88` for value 224 while `frame_00089.png` exists and MASK-TIE logged si=89.

### §5.5 Throughput
**`speed_ratio` 1.11437** (= `game_clock_speed_ratio` to 6 d.p.) · `sustained_wall_fps` 26.921 = **`stamped_fps` 26.921** (m11 honest stamp fired) · **`frames_exposure_dip` 1** — `si 35`, luma 145.139, **4.25 % below** rolling mean 151.581 (threshold 4.0 %), **m48's first fire on a real game with the host's own AE on** · **`frames_shaders_pending` 0 with `SHADERS pending=7301 incomplete=0` on 90/90** · `shader_prewarm_ms` **33,195** / incomplete 2 · `key_ring_missed/corrupted` 0/0 · `ticks_per_captured_frame` **1.35556**, identical to StackOBot's banked value · **black-frame gate PASS** (0 black, 0 dark-first, luma 145.0–154.1 mean 148.7) · readback 1920×1080 zero-origin.

---

## 5. F1–F8 on Lyra

| # | finding | Lyra | evidence |
|---|---|---|---|
| **F1** | `missing_texture` onset −1 on foliage | **NOT SEEN** (timing) + **CANNOT SEE** (target class) | 0 SHIFT; and the auto-pool **structurally cannot pick an `AInstancedFoliageActor`** (m27 exclusion) — needs a targeted fire |
| **F2** | "two runs, one record" | **SEEN — reproduced exactly** | `[16,17,21,22]` and `[40,41,45,46]`, `frame_count 4`, span 7; `TOGGLING-SUBSET … positives=4 of 8` |
| **F3** | `missing_object` onset −1 | **CANNOT SEE this leg** | never drawn in 7 bursts |
| **F4** | target leaves frame mid-event | **PARTIALLY SEEN — naturally** | event 3 (**skeletal**) `si 53` unmeasured inside its window; `bbox_valid` never signalled it |
| **F5** | labelled, nothing visible | **NOT SEEN; CONDITIONS reproduced** | NOT-VISIBLE 0, all `MEASURED_NONZERO`; but 1–4 of 9 occlusion samples, smallest drew 1.31 % |
| **F6** | blink interior mismatch | **NOT SEEN** | both blinks PASS at both edges of both runs |
| **F7** | `missing_texture` invisible | **NOT SEEN** | one read `appearance=CHECKER` |
| **F8** | game retakes the material | **NOT SEEN; mechanism exists here** | all reverts `left-to-game=0`; pool picked cubes + one `B_Manny`, never a weapon/hero |

⚠ **Six events on one leg is a SMOKE, not a survey — NOT SEEN ≠ ABSENT.**

---

## 6. What Phase A must prove on Lyra

- **`LG-1`** onset join — first labelled frame carries its own tag and `target_pixels == TagResults[tag].Count`. **Fails 4/4 today: the negative already exists.**
- **`LG-2`** event 3's `si 53` reads `observable: null` + `unmeasured_frame_count 1`; `affected_frames` drops it, `injected_frames` keeps it. **No lever needed.**
- **`LG-3`** `census_host_pp_customdepth_readers` reaches a **delivered artifact**, not only the log.
- **`LG-4`** hygiene DIFF separates host writes from plugin leaks **by stencil value** before failing `P-C6`.
- **`LG-5`** `observable` measured against selection admitting targets at **1–4 of 9** samples.
- **`LG-6`** per-frame alignment gates run here, where `speed_ratio` reaches 1.114 with no lever.
- **`LG-7`** `frames_shaders_pending` stays keyed on our own materials (at `pending=7301`, a regression marks 90/90).
- **`LG-8`** the delivery gate quotes the verifier's `[LOW]/CONTAMINATED` annotation, not a bare PASS.
- **`LG-9`** a leg on `L_Convolution_Blockout` or a targeted fire at a weapon — this map read `nanite=0`, so the audited Nanite exposure is **unmeasured**.

---

## 7. Commits and checkout states

**Plugin repo** (`master`, pushed, `origin/master == master`):
- `33c8b6d` `docs(072): second bench fixture — AnomalyInjector mounted, built and capturing in Lyra` (journal 439 lines + `G236`–`G239`)
- `08c6522` `docs(072): shared-tree rule 5 — second-host fixtures mount as a worktree, not a junction`
- `9083d95 → 08c6522`. Path-scoped adds only; **the 8 untracked owner handoffs untouched.**

**CaptureBench** (local-only, no remote): `1c09275` `tools(072): Lyra fixture leg runner and readout`. Pre-existing untracked files from other sessions were **not swept**.

**Checkouts, both directions:** main checkout was on `master` at start, is on `master` at end — **never switched**. New worktree `E:\UEProjects\LyraStarterGame\Plugins\AnomalyInjector` at **`9083d95` detached**, `git status` clean. **Staged exe `DE65F84A` and quartet `EF8EB23C`/`A8BFFF88`/`3C026A8D` re-hashed unchanged.** Banked `_bench_sessions_bank\LYRA_SMOKE_01` (131 files verified). Left on disk deliberately: `_anomalyinjector-stale-clone-2026-06-BACKUP` and `LyraStarterGame.uproject.bak`.

---

## 8. NEEDS-DECISION

1. **Do `LG-3` and `LG-4` belong inside `m49` Phase A, or their own item?** Both are host-only defects in *our* observability/guards, not in the labels; folding them in grows Phase A, deferring them leaves a check that fails legitimately-behaving hosts.
2. **Does Lyra become a standing gate host for `m49`** (i.e. do `LG-1`…`LG-9` join the pre-declared gate set), or stay a diagnostic fixture? It is now cheap to run — the DDC cost is paid.
3. **`LG-9` is the named gap**: Nanite is audited at 10 % of ShooterMaps and **100 % of weapon meshes**, but this map's candidate set read `nanite=0`, so `G134`'s reach on Lyra is **unmeasured**. Worth a second leg?
4. **Minor, filed not fixed:** `tools/__pycache__/` is untracked and not ignored (the verifier's import creates it); `mask_map.json last_frame` vs the PNG at `si 89`; and `lyra_leg.ps1` deletes rather than archives the previous log (`G238`).
api-equivalent cost (not billed on Max): $28.21 | turns: 126 | running total across all briefs: $754.69
