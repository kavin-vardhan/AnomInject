# 090-10f2 — finish 090-10e; PIE labelled frames without a mask; `texture_uniform`; B's scope in engine; cooked bias staged

Date: 2026-10-02. Author: Claude Code (Opus 5.5), headless. Ruling: `_reviews\090-10c-chat-ruling-r1.md` (items 1–3) and the
090-10f2 brief (Part 0, Part M). Branches: `fix/m52-label-timing` (Part 0, Part M) merged into
`feat/m53-uv-normal-corruption` (items 1–3). Harness `_reviews\090-10f2-*`, work dir `D:\IA_BankOverflow\_r910f2` (E: is below
its 50 GB floor). Every launch waited for the idle gate (the owner was active several times; legs waited, nothing was touched).

## 0. Part 0 — 090-10e finished and pushed
090-10e lost its login after its legs and code commits. Finished here from its records: the fix-branch strict pass (0/0), lever
audit (34/34), string scan, archives on D:, the journal `2026-10-02-090-10e-pie-endsettle-census-slice.md`, status blocks and G450
(the "PIE end +1" was the edge reader using `affected_frames`; 0 of 147 events end late against the labelled set). The readme row
now says `pie_end_settle` is a precaution, not a correction. Pushed fix `d3c9423..ba1af33`, m53 `88801bd..48a63a2`.

## 1. Part M — a labelled PIE frame shipped without its mask (fix branch `6f9ada2`, include `7d4debc`)
**What the data showed.** On such a frame: `labelled: true`, `mask_state: "unmeasured"`, `mask_file: null`, no PNG,
`target_pixels -1`, `observable null`; the frame is in `injected_frames` and missing from `affected_frames`.

**Cause.** Every such frame logs `TARGET MASK UNAVAILABLE ... pixels=0` and a `MASK-SERVED ... captureArms=2` pair: one Tonemap mask
pass served every pending arm, and in PIE warm-up the game thread had already armed frame N+1 when frame N's pass ran
(`si=5 token=6 servingToken=5`). Only the first arm gets pixels, so N+1's labelled row had none (had it got them, they would have
been frame N's). PIE: 44 of 86 banked legs (183 frames), every one also showing a render serving two capture arms; staged: 0 of
31 banked legs — but this session's staged can-fail legs on the pre-fix binary `65607703` found **3 such frames in 320 labelled**
(2 of 7 legs), so in a packaged game it is rare, not absent. On the fixed builds: staged 0 of 936 labelled frames.

**Fix (G451).** Each arm carries a sequence number; `BeginRenderViewFamily` (game thread, after this frame's arms, before the next
tick's) attaches `FAnomalyMaskArmBoundData{Bound}` to the family (family extension data survives the renderer's copy); the pass serves
only arms with `seq <= Bound` and leaves later arms pending for their own frame. A family without the bound serves everything
(counted). The run log ends with `ARM HOLD SUMMARY`. Rule in `AnomalyMaskServe.h`; `mask_serve_selftest` 15/15, mutants serve-all /
off-by-one / unbounded-serves-none all fail. The strict include pass then caught `ISceneViewFamilyExtentionData` used without
`SceneView.h` (unity hid it): `7d4debc`.

**PIE proof (A recipe: fixture, AA-off arbiter, 150 frames, 7 fires; uv / normal / missing / corrupted × 3 runs):**

| build | legs | labelled frames | labelled without a measured mask | renders serving 2 capture arms | edges | pictures |
|---|---|---|---|---|---|---|
| `EBEED6F4` (090-10e, can-fail) | 12 | 672 | **55** (all 12 legs) | 110 lines | 84/84 vs labelled set | CORRUPTS 84/84 |
| `C0AFD287` (Part M only) | 11 | 616 | **0** | 0 | 77/77 | CORRUPTS 77/77 |
| `0A99BA3C` (final code) | 12 | 672 | **0** | 0 | 84/84 | CORRUPTS 84/84 |

Arm holds per leg 21–46 (the path is exercised, mostly in the first second). The flag route (`transition_reason:
mask_unmeasured`) was not needed: the cause is fixed, and the readme says a labelled frame with an unmeasured mask must be reported.
(The 12th `C0AFD287` leg did not run: its driver was stopped while waiting for the gate; the final build repeats all 12.)

## 2. Item 1 — `texture_uniform` (m53 `3e47fdb`)
**Rule.** After A6 a slot is refused `texture_uniform:<rule>` when every required transformable binding is measured uniform for the
drawn mode: `tile` / `scramble` → `no_spatial_variation` (every channel's max − min ≤ 2/255 on the resident top mip); `invert` →
`flat_normal` (X and Y within 0.5 ± 2/255); `green_flip` → `flat_normal_y` (Y only — a map whose X varies and Y is flat cannot show a
green flip); census (no mode) → the family rule; bench identity / tile-probe modes are not checked. One measured-varying binding
admits; `pending` (measurement not back) and `unmeasurable` (failed) refuse. B applies (the component is skipped).
**Tolerance:** 2 of 255 encoded steps — a constant colour or a flat normal stays within one step after block compression (measured
constants read min = max; BC5 flat maps read 0.5020); authored detail is far above it (fixture checkers 0.22–0.69, real normals
0.35–0.67). A constant but tilted normal is not flat (invert changes it) — tested.
**Measurement and cost.** `FAnomalyTexStatsCS` (AnomalyShaders) over RHI mip 0 (the resident top), raw values (sRGB decode off), 8×8
groups, groupshared min/max, 9-uint result, async readback polled on the render thread, cache per (texture, resident first mip); ≤ 4
kicks per frame, ≤ 16 in flight, no game-thread stall. Measured: 53 fixture textures in 16 uncaptured warm frames before the first
captured frame; census `uniform kicked=53 ... waited_s=0.3` (fixture), `46 ... 0.2` (MainWorld); MainWorld census-window longest frame
**26.1 ms** (090-10e: 27.6).
**Tests both ways.** `texcorrupt_uniform_selftest` 30/30; six mutants fail (alpha ignored, pending treated as decided, bench modes
checked, tolerance 20 steps, green_flip judged on XY, texel count unchecked). Z0910c texcorrupt suites unchanged (7/7, 27/27).

**Which textures measured uniform** (fixture, office-like): `T_TC_ChainA/B/CAlphaCov/CBlur/CSharpen/D` (16×16 constants; B and D
flat normals), `T_TC_LayerGlobal` (32×32 constant), `T_TC_UI1x1`. Every other fixture texture varies. MainWorld: `127grey`,
`BaseFlattenNormalMap` (1×1), `WhiteSquareTexture`, `T_black` — engine/project defaults.

**Eligibility before / after** (`IAI.TexCorrupt.Census allreasons`, same session, `B3103E32` = 88801bd vs `0A99BA3C`):

| content | uv eligible | normal eligible | difference |
|---|---|---|---|
| office-like fixture (82 objects) | 72 → **32** | 8 → **4** | `texture_uniform` 40 uv / 4 normal, each the ONLY blocker; every other key identical |
| MainWorld plain (343) | 33 → **25** | 9 → 9 | `partial_footprint/texture_uniform` 8 (default textures on a component) |

**Auto-pool, office-like fixture, PIE (600 frames, coverage 0, poll 0):**

| build | events | visibly corrupt (pixel rule) | no visible change | edges | labelled frames without a mask |
|---|---|---|---|---|---|
| `B3103E32` (88801bd, can-fail) | 28 | 17 | **11** (the 090-10c class) | 17/17 judged | 0 |
| `0A99BA3C` | 28 | **28** | **0** | 27/27 judged | 0 of 219 |

No other labelled-but-invisible class surfaced.

## 3. Item 2 — B's component scope in engine (m53 `38fed49` fixture, `4dfce28` fix)
**Fixture.** `AAnomalyBenchTwoPartActor` (AnomalyBench, bench-only): a static, non-animated root with `PartEligible` (engine cube,
the uv target's `M_TC_UC1` or the normal target's `M_TC_NN1`) and `PartIneligible` (cube, `M_TC_Alias` = `texture_not_parameter`),
summoned in PIE 4.5 m in front of the camera with a gap; the step records each part's projected box (`_reviews\090-10f2-twopart.py`
reads mask, bbox, picture and m26 per box).

**Found and fixed (G454):** on `D2B57E14` the mask, `bbox_drawn_px` and m26 were scoped but `bbox_px` / `bbox_norm` covered the whole
actor (x 272.95 → 551.05 against the eligible part's 272.95 → 383.16): since 090-10 the label box comes from bounds frozen per picture
via `GetActorRenderableBounds`, and 090-10c's scope lived only in `ProjectActorBoundsToScreenRect`. `GetActorLabelBounds` now feeds both.

| leg (7 fires each) | mask px/frame in eligible box | in ineligible box | elsewhere | bbox_px inside eligible box | picture change eligible / ineligible (null 0) | m26 maxCount | edges | components corrupted / skipped |
|---|---|---|---|---|---|---|---|---|
| uv, `D2B57E14` (before the bbox fix) | 11,260 | 0 | 0 | **0/8** per event | 0.92 / 0.00 | 11,260 | 7/7 | 1 / 1 |
| uv, `0A99BA3C` | 11,260 | **0** | 0 | **8/8** ×7 | 0.92 / **0.00** | 11,260 | 7/7 | 1 / 1 |
| normal, `0A99BA3C` | 11,260 | **0** | 0 | **8/8** ×7 | 0.94 / **0.00** | 11,260 | 7/7 | 1 / 1 |
| uv, `NoPartScope` can-fail | 11,260 | **11,260** | 0 | 0/8 | 0.92 / 0.00 | **22,520** | 7/7 | 1 / 1 |
| normal, `NoPartScope` can-fail | 11,260 | **11,260** | 0 | 0/8 | 0.94 / 0.00 | **22,520** | 7/7 | 1 / 1 |

(Change = share of the part's box whose pixels differ from the clean frame before the event by > 12 levels, median over the labelled
frames.) Edges 0 off outside flagged frames on every leg. The smoke leg (`B_uv_r1`) is kept: its first event read two late frames
while the freshly summoned actor settled (0.1 s after the step); the step now waits 10 s.

## 4. Item 3 — cooked per-texture, group and cinematic bias, staged (m53 `a6fa73e` lever, cook `f2a`)
**Fixtures** (`_reviews\090-10f2-author-bias.py`, copies under `/Game/CaptureBenchTexCorrupt/Bias0910f2/`, one level per variant
with the uv/normal target cube): **V1** the texture's own `LODBias 1`; **V2** `TEXTUREGROUP_Project01` plus a cooked
`Config/DefaultDeviceProfiles.ini` giving that group `LODBias=1` (a dedicated group, so nothing else is biased); **V3** cinematic
mips (`NumCinematicMipLevels 1`). Second and third authoring passes added **V1S/V2S** (4096², non-streaming) and **V1T/V2T**
(2048², streaming) because the 128² copies are never streamed. Cooks `f2a`/`f2b`/`f2c` (BuildCookRun, container
`45E2FA86`), staged on BenchGate by the 090-10c runner with every swap receipted; dbghelp did not block.

**What the cook does (measured from the staged game's own decide lines):** V1 and V2 arrive as `cooked=64x64/M7` from a 128²
source, V1S/V2S as `2048x2048/M12` from 4096², V1T/V2T as `1024x1024/M11` from 2048², each with `asset_lod_bias=0
cached_lod_bias=0` — **the cook removed the biased top mip; there is no runtime bias left to refuse** (G455). V3 keeps its full
chain (`128x128/M8`) with `cinematic=1 cached_lod_bias=1` — the one runtime case.

| staged leg | build | events | pictures | identity change (null ≈ 0.34) | edges | labelled frames without a mask |
|---|---|---|---|---|---|---|
| V1 uv / normal | `36C8E30A` | 7 / 7 | CORRUPTS 7/7 (46.6) / 7/7 (22.3) | — | 7/7 · 7/7 | 0 / 0 |
| V1 identity | `36C8E30A` | 7 | no visible change 7/7 | 0.333–0.337 | (not judged: no change) | 0 |
| V2 uv / normal / identity | `36C8E30A` | 7 / 7 / 7 | 7/7 / 7/7 / no change 7/7 | 0.333–0.337 | 7/7 · 7/7 | 0 |
| V3 uv / normal / identity | `36C8E30A` | 7 / 7 / 7 | 7/7 / 7/7 / no change 7/7 | 0.333–0.337 | 7/7 · 7/7 | 0 |
| V1S, V2S uv / identity | `36C8E30A` | 7 / 7 each | 7/7 (33.3) / no change 7/7 | 0.335–0.338 | 7/7 | 0 |
| V1T, V2T uv | `36C8E30A` | **6** each | CORRUPTS 6/6 | — | 6/6 | 0 |
| V1T, V2T identity | `36C8E30A` | 7 each | first event **12.21**, then no change 6/6 | 0.333–0.342 after the first | — | 0 |

**Can-fail on the 090-10c binary `65607703` (no cinematic handling):** V3 **refused** — `runtime_lod_bias:cinematic`, 0 events.
V1, V2, V1S, V2S (7/7) and V1T, V2T (6/6) **corrupt and label exactly like the new build**: the old binary cannot misbehave on them
in a cooked build because the cook already removed the bias. ⇒ **the staged can-fail for per-texture and group bias is
structurally unobtainable**; it exists only in PIE, where the bias is runtime and 090-10c measured it. (The old binary also shows
Part M on staged: 3 labelled frames without a mask in 320.)

**Two findings on streaming textures (V1T/V2T), reported, not fixed:**
1. The uniformity measurement is keyed by the resident first mip. The pre-run measurement saw mip 4 (64²) while the texture was
   streaming in; the first fire (frame 33) found mip 0 resident, the new key unmeasured, and refused `texture_uniform:pending`. The
   old binary loses the same fire earlier (frame 6) to `not_fully_resident`. One fire of seven, refused safely, no mislabel.
2. Identity mode on the first event reads a change of 12.21 against ≈0.34 on the other six: the bench identity copy is made from
   what is resident when the event applies, so a texture still streaming in yields a lower-resolution copy. Bench-mode only; the
   uv/normal transforms corrupt 6/6 regardless.

**Lever (G449):** `IAI.Bench.TexCorruptHostTexBias` now refuses in any cooked build (`FPlatformProperties::RequiresCookedData()`),
help text says "EDITOR AND PIE ONLY"; verified on staged `36C8E30A` (leg `S_V1_leverref`: the lever issued before the capture logs
`REFUSED in a cooked build ... Nothing was changed`, and the leg still reads CORRUPTS 7/7, edges 7/7, 0 frames without a mask). BenchGate restored to
S1 afterwards (exe `2FCDF059`, container `20DA6F98`; the first restore attempt hit a still-exiting game and was retried).

## 5. Builds, checks, archives
| branch | code head | packaged exe | strict include pass | other |
|---|---|---|---|---|
| `fix/m52-label-timing` | `7d4debc` (docs `71a3be1`) | `822FA156` | **PASS** — strict 0 errors, normal StackOBot + Editor 0/0 (`strict_fix3`) | `mask_serve_selftest` 15/15 + 3 mutants fail; string scan vs `68BA4D9A`: +4 / −0 UTF-16 strings, none in the scanner's unexplained ("other") class |
| `feat/m53-uv-normal-corruption` | `2dbf6f2` (merge of fix `7d4debc` at `9e081fe`) | `36C8E30A` | **PASS** — strict 0, normal 0/0 (`strict_m53b`; `strict_m53` on `d7127e5` also PASS) | lever audit union of exe + 5 DLLs **PASS** 52/52 registered, 0 unknown, 0 missing; `texcorrupt_uniform_selftest` 30/30 + 6 mutants fail; `census_readonly_check` PASS, selftest 40/40, Python suite 27/27; string scan vs 090-10e `64c0817`: +37 / −5 UTF-16, the 11 "other" additions (bench actor, arm-bound identifier, lever and uniformity text) listed in `D:\IA_BankOverflow\_r910f2\strings_m53_final.json` |

PIE legs ran on `0A99BA3C` (code `4dfce28`, before the census-line fix and the fix-branch merge) and were **repeated on the pushed
head `36C8E30A`**: A recipe uv/normal (7/7 each, 0 of 112 labelled frames without a mask), item 2 uv/normal (mask 0 in the
ineligible box, bbox 8/8 ×7, change 0.92 / 0.94 vs 0.00, m26 11,260), auto-pool office-like (28 events, 28 CORRUPTS, 0 of 219),
MainWorld census (uniform kicked=46, `dt_max` 25.0 ms); every staged Item 3 leg ran on `36C8E30A`. Archives (each with
`archive-hashes.json`): `D:\IA_BankOverflow\_binary_baselines\m52fix-0910e-68BA4D9A`, `m53-0910e-EBEED6F4`,
`m52fix-0910f2-822FA156`, `m53-0910f2-0A99BA3C`, `m53-0910f2-36C8E30A`; cooks `D:\IA_BankOverflow\_r910f2\cookout_f2a|b|c`.
Hosts left warm: `_r53_host` detached at `2dbf6f2`, `_r84_host` on the fix branch at `7d4debc`.

## 6. Not done / open
1. **Staged can-fail for per-texture and group bias cannot exist** (the cook strips the biased mips). Item 3's can-fail clause is
   met for cinematic only; per-texture/group bias handling rests on the 090-10c PIE can-fail. CHAT-DECISION: accept that scope.
2. **`texture_uniform:pending` costs a fire when a texture's residency changes after the pre-run measurement** (streaming V1T/V2T:
   1 of 7). Safe direction (refuse, no mislabel); a re-kick on a key miss before refusing would recover it. Not built.
3. **Identity copy on a still-streaming texture is made at the resident resolution** (first event 12.21). Bench mode only.
4. The `C0AFD287` Part M build ran 11 of 12 legs (its driver was stopped at the gate); the final build repeats all 12.
5. No other labelled-but-invisible class surfaced in the auto-pool legs.
