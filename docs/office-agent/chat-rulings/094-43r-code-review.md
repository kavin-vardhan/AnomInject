# 094-43r — Code's review: GE1 is in and the full P9 passes; GD3 matches the legacy labels exactly with zero outline leak; FZ2 is built, and H1–H5 pass on the fixture and on Paris EA_Core under D3D11 and D3D12

**Date:** 2026-11-04, about 05:00 IST · **Author:** Claude Code (mailbox launcher and reviewer) · **These are observations, not rulings.**

**How it ran:**
- **Resume:** thread `01a14334`, detached, with W4 (no `GIT_*_DATE` in the child), after a dry run of my launch gates.
- **Run:** 02:28:30 → 04:35:44 (7,634 s), exit 0, one try. Codex verdict: **GREEN**.
- **Report:** collected byte-identical as `_reviews\094-43r-codex-report.md`.
- **Receipts:** `code-receipts-094-43r\`. My readers: `D:\IA_Europa\Temp\CodeReview09443r\`. My P9 run: `D:\IA_Europa\Receipts\CodeReview09443r\p9main\`.

## 1. GE1 (`d2f481d`)
- **Diff:** `owner_p9.py:72`'s equality assertion becomes `validate_refs` (new `owner_ref_policy.py`).
  - All 71 baseline refs must be present and unmoved.
  - Only four named extras are admitted, each by its rule: the backup at exactly `08fe652`; 42c at exactly `740220c`; origin feat an ancestor of or equal to local feat; 43a at `c1193bd` or a descendant.
  - Any other ref raises. The baseline is not regenerated.
  - The names are exact refs, stricter than the ruling's `spike/…-*` families.
- **Self-test, my run on the current refs:** 8 of 8 pass. That covers current state, plus the four required mutants (unknown extra, moved backup, moved 42c, moved baseline ref), plus missing baseline, origin not an ancestor, and 43a not a descendant.
- **Full `main()`:**
  - **Codex, at entry:** PASS in 354.4 s.
  - **Mine, unmodified, at the final tip `93613ae` with spike 43a moved to `42ea37e`:** PASS in 559.7 s.
  - Both runs: 382,970 inventory files with 0 differences (1,023 admitted by EG1); 22,628 binary/config files (100,696,938,006 B) rehashed equal; protected git equal; both source manifests equal; toolchains, fsck and ledger all pass.
  - My `ref-admission.json` admits exactly the four refs, with 43a under the descendant rule.
- **Doc slip, already corrected:** the GE1 gotcha reused **G524**. Codex appended **G527** as the number correction (append-only). The next free number on feat is **G528**.

## 2. Stage A (spike `42ea37e`, fixture only; tolerance frozen in `09443-stagea-predictions.md` at 02:41:16, before any launch)
- **GD3 as built** (`AnomalyMask425.usf`):
  - Visible IDs are kept only where `|privZ−sceneZ| ≤ 4/(2^24−1)` = 2.384e-7 device-Z.
  - Logically hidden **and** selected event IDs are kept where `privZ ≥ sceneZ−tol` (reversed Z).
  - Rejected pixels never replace an admitted ID.
  - Scene depth comes through the exported scene-texture uniform buffer.
  - The tolerance is quantization-derived: four D24/float steps.
- **A-P3, my reader** (`p3review43r.py`; reference `09443a\reference\02` vs `09443r\private-off\01`):
  - 2,352 pairs, 0 signature mismatches.
  - **Every object has IoU 1.0, with reference px = private px on every object.**
  - Masked: 84 non-empty frames, 394,800 px. WPO: 1,637 frames, 8,122,794 px.
  - Hidden blink: 54 + 78 frames, all non-empty and equal.
  - Event masks (lighting 157, LOD 91 + 156, missing 65 + 130 frames): all 1.0.
- **78 lighting owner-group frames** (six runs of 13 frames). One side tags the lit receivers as one group and the other tags them per actor. This is reported under GD4; 094-43a had 26 such frames.
  - Their combined coverage is identical on all 78 (union IoU 1.0, minimum included).
- **GD3 rule check, mine, on every private pixel:**
  - 1,002,810,012 ID pixels at scene depth.
  - All 29,920,890 off-depth pixels belong to hidden targets (OF_LOD_A/B).
  - **0 violations.**
- **A-P4, my reader:**
  - **0 new magenta on all 2,352 frames**, and 0 magenta on either side.
  - Each frame differs only by tonemapper grain (≤2 levels).
  - The on-run carried `OutlineFixture.Blendable 1`, the off-run `0`.
  - **Can-fail:** the same classifier finds magenta on 474 of 588 sampled frames of 094-42c's legacy outline-on run (max 250,978 px).
- **A-P2:** 0 `Missing cached shader map` lines in both runtime logs.
- **A-P5, both legs:** 0 rows with components on or tagged, 0 unexpected; depth-before = after (14 candidates, all off); `r.CustomDepth = "1" LastSetBy: Constructor` at all 3 reads.

## 3. Stage B (`ea08258`, `59c1282`; recorded in `93613ae`)
- **What changed:**
  - New `AnomalyPrivatePass425.{h,cpp}`, `AnomalyPrivatePolicy425.h`, `AnomalyPrivate425Shader.{h,cpp}` and `AnomalyPrivate425.usf`.
  - Proxy straight from the component (`:367`); a plugin-owned `FMeshElementCollector` for ISM, HISM and skeletal; two batched phases (visible, hidden).
  - Eight fenced persistent slots and non-blocking timing queries.
  - IDs are kept in a plugin-owned depth-stencil attachment. Private depth starts as a copy of the frame's scene depth, so visible draws use hardware depth-equal (stricter than the frozen tolerance).
  - All new includes resolve to 4.25 `Public` folders. Both targets link (editor modular), and no non-exported symbol is named.
  - 0 comment lines added to source.
- **Custom-depth writes:**
  - Every remaining write sits behind `Legacy` (`AnomalyMasks425.cpp:57,80-84,91-105,135-141`): `RestoreAll`, `TagActor`, `SetCustomDepthStencilWriteMask` and `EnableCustomStencil`.
  - `Legacy()` (`AnomalyPrivatePass425.cpp:270-272`) needs `-IAIBench`, the exact fixture map and `-IAICaptureLegacyReference`.
  - The bench plant is removed. `r.CustomDepth` is only read (`AnomalyHostDepth425.cpp:87`).
- **Admission** (`AnomalyHostDepth425.cpp:83`): with the private pass available, capture is admitted with no host write; the scanner still runs on every C5 request.
  - Without the private pass, only the bench legacy path can be admitted, and reader/unknown hosts still refuse.
  - So outside the bench, a missing private pass refuses outright. That is stricter than FZ3.
- **GD5:** unsupported components are excluded from census and selection (`AnomalyMasks425.cpp:65-66`) and listed per run.
  - Counts: fixture 7 verified / 0 unavailable; EA_Core 5 / 0 (all `StaticMeshComponent`).
  - ⚠ **ISM, HISM and skeletal are covered only by the native policy test.** No fixture contains them, so they were never drawn live.
- **Self-tests (Codex receipts):**
  - native policy: four kinds; admission; padded rows at widths 1, 1279, 1280 and 1600; the tight-copy mutant fails;
  - contract mutants: host write, legacy guard, bench plant and cached scanner are all rejected;
  - FXC: all three new global-shader bodies compile.
- **Builds:** FM1, BelowNormal, 2 actions. Fixture A Editor 01 and Game 01; B Game 01; **B Editor 01 failed (C4458, `Views` shadowing a member)**; B Editor 02 passed; Paris Editor 01 and Game 01. 0 LNK errors.
- **Card draft:** `docs/europa/office-card-094-43-draft.md` carries all three fixes.

## 4. H1–H5, by my own readers
- **H1:** 0 new magenta on all 2,352 frames (Blendable 1 vs 0, default routes, seed 4242); grain ≤2 levels; 222 hidden-blink frames.
- **H2:** against a fresh Stage B legacy reference (which tags 200–206 on 1,896 rows and sets `r.CustomDepth` 3, proving it is the legacy path):
  - every object IoU 1.0 with identical pixel counts;
  - masked 168 frames, 789,600 px; WPO 1,658 frames, 8,226,996 px;
  - hidden blink 84 + 138 frames, all non-empty;
  - 0 owner-set mismatches (no lighting route).
  - Stage A had 119 pixels (1 per frame, OF_LOD_A) inside the tolerance but not bit-exact. Hardware equality would drop those; none was lost in H2.
- **H3, private legs:** 0 on/tagged rows; depth before = after; `r.CustomDepth` 1; `polluted` and lost 0 on every row; 0 host depth or stencil writes.
- **H4 (EA_Core, four explicit routes), all three runs** (D3D11 1280×720, D3D12 1280×720, D3D12 1279×719):
  - 2,352 frames of the right size, 0 dropped, 0 zero-byte;
  - 69 events (blink 22, lighting 13, LOD 19, missing 15);
  - 731 masks = 731 affected frames, PNG counts equal to the row counts;
  - every LOD event changes in-mask pixels (minimum 14,456 / 14,456 / 14,428);
  - census lost/no-pass/polluted/exhausted 0; player view = captured view;
  - `SELF_CHECK` shows the right size, RHI and `GameViewportClient`. Format and backbuffer format are 18 = `PF_A2B10G10R10` (the stock 10-bit backbuffer, 4.25 `PixelFormat.h`) on both RHIs, as Codex said;
  - 0 fatal lines; upload check PASS; unavailable 0 (5 static).
  - My checker first failed "fires ≥3": it looked for `blinking` where the annotation type is `blink`. The same naming slip was in my 094-42r checker. Fixed; nothing in the product changed.
- **H5 (private pass only), mean / p95:**

  | Run | GPU | Render-thread submit |
  |---|---|---|
  | D3D11-1280 | 0.159 / 0.189 ms | 0.162 / 0.226 ms |
  | D3D12-1280 | 0.157 / 0.189 ms | **1.688 / 2.076 ms** |
  | D3D12-1279 | 0.158 / 0.189 ms | **1.597 / 1.949 ms** |

  - 0 blocking waits.
  - Submit on D3D12 is about 10× D3D11. It is not a gate, but worth knowing.

## 5. Safety
- **Crashes and uploads:** 0 crashes and 0 upload signs across 8 runtime sessions (each with its normal crash monitor); 0 fatal lines in every log I read.
- **Admission waits:** at most 244 s (idle gate, Stage B private-off), far below GA5's 3 h.
- **Paris:** Content (1,456), plugin Content (5), Config (18), descriptor and `.uproject` are hash-equal; `Saved` is unchanged.
  - Binaries changed (5 host, 6 plugin), only while UBT ran (17 timeline entries, all `build_alive=True`).
  - Intermediate `AssetRegistryCache` and build caches changed, from the launches and builds.
- **Fixture:** the seven assets and the preserved `Engine.ini` (`64d45f80`) are hash-equal.
- **Refs and m51:**
  - `m51` passes at start, prelaunch, exit and after my P9; `master`, `release`, 30 tags and the tag refs are unchanged.
  - Only two refs moved: feat `75ca580` → `93613ae` and spike 43a `c1193bd` → `42ea37e`.
  - Both spikes are local, and neither is an ancestor of feat.
- **Deletions:** none by command. Switching the fixture worktree from the spike to a detached `59c1282` removed three spike-only tracked files. They equal their `c1193bd` blobs and survive in `c1193bd`/`42ea37e` (receipt: `fixture-checkout-receipt.json`).
- **Tree lists:** P9 files unchanged; 0 reparse points.
- **Process:** none left running.
- **My polls:** 40 lines, 2–5.5 min apart. ⚠ One gap was 6.5 min (03:58–04:04), while I ran a reader just as the Paris builds began. Every build and session was still sighted.

## 6. Phase B check (GE3, read-only, 4.25 source and assets)
- **SkeletalCube:** `/Engine/EngineMeshes/SkeletalCube` exists in 4.25 as a real `SkeletalMesh` with its `Skeleton`. It is an old package (UE4 version 288), which 4.25 loads.
  - The only other engine skeletal mesh is `EditorMeshes/SkeletalMesh/DefaultSkeletalMesh`, which is editor-only content.
- **py27 port of `create_anomaly_materials.py`:**
  - Every `MaterialEditingLibrary`, `EditorAssetLibrary` and `AssetToolsHelpers` call exists in 4.25.
  - Every expression property exists: `R`, `UTiling`/`VTiling`, the mask channels, `ConstB`, `Constant`.
  - Floor, Frac, Add and Lerp exist, and `EMaterialProperty` is BlueprintType. The script is ASCII-only.
  - ⚠ **`used_with_nanite` does not exist in 4.25.** Drop it; `bUsedWithClothing` and the other five flags exist.
  - ⚠ **`unreal.MaterialShadingModel` is probably not exposed.** `EMaterialShadingModel` is `UENUM()` without BlueprintType, and the only Blueprint-callable reference is in `BlueprintMaterialTextureNodes`, which the fixture does not enable. So `_verify`'s shading-model check must change.
  - This is source reading only; nothing was run.

## 7. Stage C (mine)
- **Scan:** clean (`_reviews\094-43r-prepush-scan.md`), over `d1cdc7b..93613ae` (20 commits, 8,008 added lines). The one carried non-secret item: 8 new UBT warning lines repeat the owner's Windows profile name, which is already public on origin.
- **Push:** `feat/europa-port` only, refspec explicit, no `--force` and no tags.
  - `d1cdc7b..93613ae`, a fast-forward. `ls-remote` returns `93613ae`.
  - 0 spike refs and 0 tags on the remote.
- **Card:** `_reviews\094-43-office-card-europa.md`.
  - Its §5 read commands were run against the home D3D12 output and print the expected values (`code-receipts-094-43r\card-command-test.txt`).

## 8. Decisions for chat
None needed for this job. Two notes:
- ISM, HISM and skeletal private drawing is unproven live until a fixture carries them. 094-44's SkeletalCube would be the first live skeletal check.
- D3D12 render-thread submit costs about 1.6–1.7 ms per captured frame.
