# 094-42c — Code's review: P9 v3 is exact, the outline leak reproduces at home (including the outline of a hidden target), the guard refuses before any tag, and FZ6(b) is measured; the direct StaticType link fails

**Date:** 2026-11-03, about 23:45 IST · **Author:** Claude Code (mailbox launcher and reviewer) · **These are observations, not rulings.**

**How it ran:** resumed thread `01a14334` with `-OwnerApprovedFullAccess`, detached, W4 applied (no GIT_*_DATE in the child), at 17:14:37. Codex exited 0 at 23:18:22 (21,824 s, one try), verdict GREEN. Report: `_reviews\094-42c-codex-report.md` (byte-identical body). Receipts, scripts and polls: `code-receipts-094-42c\`. The resume prompt is the request verbatim, my context, and the implementer block with the two BankOverflow edits (`prompt-receipt.txt`; my first prompt build carried a wrong clock time and was replaced before launch, receipt kept).

## 1. P9 v3
- **My check** (`p9v3_check42c.py` → `p9v3-check.json`, PASS): v3 `full-sha256.jsonl` equals v2 minus exactly the **89,583** lines under `D:\IA_BankOverflow`, byte for byte and in order (382,969 kept). v3's removed manifest = v2's 235,223 + 89,583 = 324,806. v1 re-partitions byte-exactly into v3 kept + removed under v3's roots (= v2's roots + `D:\IA_BankOverflow`). All receipt hashes match; `parent_sha256` = the v2 receipt. v1 and v2 files are hash-equal before and after.
- **Receipt** `baseline-v3\v3-receipt.json`: same format (the legacy `v1`/`v2`/`removed` stream keys are kept); v3 SHA `0e7c3386…ab666`.
- **Kit lines** (`de29514`): `owner_p9.py` L17 `baseline-v2`→`baseline-v3`, L19 `v2-receipt.json`→`v3-receipt.json`, L64 `8e3ed095…`→`0e7c3386…`; `root_attribution.py:56` → `baseline-v3`. The same commit binds the 09442c scope in `job_scope`/`write_ledger` and caps `build_memory`'s wait at 3 h (GA5).
- **Tests** (`Receipts\09442c\tests\p9-v3`):
  - Current disk: 382,970 files against 382,969 expected, 0 differences.
  - Planted record: an expected entry for the absent `StackOBot/Content/09442c-missing-negative.uasset` → MISSING → STOP. No file was created or removed.
  - BankOverflow exclusion is lexical; the parent folder still exists, and the test also covers an absent path.
  - **Caveat:** the tests call `derive()` and run `main()`'s inventory and missing-file code by source extraction (`inspect`+`exec`). The rest of `main()` (the EA8 rehash, toolchain and fsck checks) was not run, under FJ1.

## 2. The fixture
- **`-nowrite`:** on all **12** session logs (by each log's own command line) and on all 11 editor launches I saw live.
- **Seven assets**, matching `assets.json` and my own hashes:
  - `EA_Outline.umap` `bc5482f8…`
  - `M_Masked` `bee36524…`, `M_Outline` `1b8ec079…`, `M_Surface` `ab457d9d…`, `M_WPO` `40395df2…`
  - `SM_OutlineCubeSphere` `c7482e0d…`, `SM_OutlineSphereCube` `0aa066da…`
- **Preserved `Saved\Config\Windows\Engine.ini`** (1,502 B, `64d45f80…`): unchanged.
- **Other fixture-only changes:**
  - `Config\DefaultEngine.ini` (the DDC graph), `Scripts\author_*.py`, and `Source\Outline425.cpp` (the WPO helper).
  - Two small editor writes that `-nowrite` does not gate: `Saved\Autosaves\PackageRestoreData.json` (96 B) and `Saved\Config\WorldState\2636331765.json` (50 B).
- **Installed 4.25 engine:** 0 of 179,534 files modified.

## 3. The repro gate, by my own reader (`outline_check42c.py`; label masks, not Codex's stencil evidence)
- **Pairing:** 2,352 index pairs. The control and repro labels are identical except for a constant −212 engine-frame offset on `start_frame` (same seed, 69 events, same camera, sweep, tags and targets).
- **Control and untagged frames:** the control has **0** magenta pixels on every frame. The 456 untagged frames are pixel-identical. Every changed pixel is new magenta.
- **Tagged frames without an anomaly:** all 1,270 carry 204,600–205,672 new magenta pixels.
- **Per anomaly**, new magenta on the target's 3 px silhouette band:
  - `lod_corruption`: **404/404** frames, 6,038–6,321 px.
  - `blinking`, all hidden frames: **222/222**, 6,071–6,321 px.
  - Codex's 351/186 is the scored-phase subset of these frames.
- **Example, frame 746** (`_reviews\094-42c-example-frame746-hidden-blink-outline.png`):
  - `blinking` has hidden `OF_LOD_A`, and the control shows the cube gone.
  - The repro draws that cube's full silhouette in magenta, exactly on the label mask's edge (green overlay), and fills its right face.
  - It also paints the tagged floor out to about 5/6 of the frame height, plus edges on the census-tagged sphere and back plane.
  - ⇒ 094-41's worst case is reproduced: **the outline of an object labelled missing**.
  - Most new magenta is floor fill (149k–230k px against about 6k on the band), so this fixture shader is not a constant-width outline.

## 4. The guard gate
- **Log** (`guard\01\session\Runtime.log`):
  - L1841 `Capture425 host custom-depth reader readers=1 writers=0 unknown=0 tagged=0 bench_bypass=0`
  - L1842 `Warning: C5 refused: host custom-depth reader`
  - `Exiting.` follows 0.75 s later.
- **Receipt:** `admitted=false`, `tagged_before=0`, `tagged_after_scan=0`, `custom_depth_mode=1`.
- **No capture:** `plugin-frames` holds only `routes.json` and `host-custom-depth.json` (no frames, labels or masks).
- **The bypass difference:** the repro's identical scan reads `bench_bypass=1` and admits.

## 5. FZ6(b)
- **The spike branch:** `spike/europa-09442c-mesh-shader` (`66dcf8d`, `26048a6`, `8447eba`, `740220c`), in the fixture's plugin worktree.
  - It is **local only**: `ls-remote` shows no spike ref, and origin feat = `d1cdc7b`.
  - It is not merged: none of the four is an ancestor of `feat/europa-port`, and `f7b32f0..d840cdb` has no merges and touches no `Source`/`Shaders`.
- **Cost**, by my recount of the raw logs:
  - Warm no-change `baseline\02`: 0 `Missing cached shader map` lines, 20.6 s log span (Codex: 22.612 s, launch to exit).
  - Fresh-key `spike\03`: **53 lines, 47 distinct materials**, 2 workers, synchronous compile, 2,904 s log span (Codex: 2,906.412 s), empty queue and normal exit.
  - ⇒ **about 2,884 s (48 min) extra on first launch.**
  - The 47 are engine and editor materials plus the fixture's `M_Surface`, `M_Masked` and `M_WPO`. The post-process `M_Outline` is not among them.
  - This is one pair, not a throughput benchmark.
- **Earlier tries**, preserved and marked incomplete:
  - `spike\01` (13 workers; the ini override was malformed): the 4 GiB commit floor after 661 s, with 5,239 jobs pending. Stop handling then hit WinError 5; the denied operation is unconfirmed.
  - `spike\02` (1 worker): the probe's own 1,800 s limit, with 985 of a 5,425-job peak pending.
  - The policy changes between tries are disclosed, and no threshold was lowered.
- **The link risk:** a real build referencing `TDepthOnlyVS<false>`/`<true>`, `FDepthOnlyPS<false>`, `FDepthOnlyHS` and `FDepthOnlyDS` `::StaticType` fails with **5× LNK2001 + LNK1120** (UBT exit 6; log `8a362ce5…`).
  - ⇒ GA3's route is viable only if it never names `StaticType` (for example, a lookup by name). **That form was not compiled.**
- **FM1:** fixture Editor 1, fixture Game 1, spike Editor 3 (including the intended link failure); all at BelowNormal with two actions. `spike\03` changed only the `.usf` and reused build 03 (Codex checked its 20 native output hashes).

## 6. Safety: none
- **Crash and upload:** no new crash folder; 0 fatal or assert lines in the 12 logs; 0 upload lines. The only hit in each log is the crash client printing its `DataRouterUrl` setting at start.
  - Two sessions ended by owned stop. The kit's first result for `author\01` reads "crash" (exit 125); the separate reread is right: the stop intent (12:26:50 UTC) precedes the exit (12:27:19), and the log ends mid shader compile.
  - The other was `spike\01`, at the memory floor.
  - FI4 count: 0.
- **Paris host:** Content (1,456), plugin Content (5), Config (18), Binaries (16 + 11), the descriptor and the `.uproject` are hash-equal before and after; `Saved` (36 files) is size- and mtime-equal.
- **`m51`:** PASS at start, both prelaunches and exit (`53bf725`, `526aba7f`, 15 untracked). `master` `b5f15a3`, `release` `d7eee97`, 30 tags with unchanged tag refs. StackOBot Content and Config and the 5.1 engine's `Binaries\Win64`: 0 files modified.
- **Binaries:** they changed only while UBT ran (all 10 change events), all under `E:\IA_Fixtures\Outline425`.
- **Deletions:** 0 files removed in `D:\IA_Europa`, the run folder and `E:\IA_Fixtures`.
- **Processes Codex stopped**, by recorded PID: its own editor trees, and two Epic Games Launcher processes that the fixture editors auto-started (about 18:01 and 18:08). Later authoring used `-BENCHMARK`.
- **No push:** local feat `d840cdb` holds 11 unpushed commits (8 from 094-42r plus `de29514`, `0b8baca`, `d840cdb`); no tags.
- **Foreign activity:**
  - The owner's DaVinci Resolve reached about 12.6 GB, until about 20:20.
  - D: dipped to 46.5 GiB (17:24) and 50.8 GiB (19:47); the writer is not established.
  - Admission holds (author 22 min, spike build about 19 min, spike\02 60 min, spike\03 34 min, baseline\02 about 9 min) all stayed under GA5's 3 h.
- **Cosmetic:** the ledger id `…-resume48` is used twice (a numbering lag left by the earlier crash).
