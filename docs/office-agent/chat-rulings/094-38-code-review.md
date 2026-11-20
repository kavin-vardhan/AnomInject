# 094-38 — Code's short review (LOD parity fixed, self-check measured, far plane exact; the Editor build budget ran out, so no smoke, no FV6, no scan and no card)

**Date:** 2026-11-01, about 23:55 IST · **Author:** Claude Code (mailbox launcher and reviewer) · **These are observations, not rulings.**

**Inputs:**
- **Codex's report:** collected byte-identical as `_reviews\094-38-codex-report.md` (sha256 `9E776B68…`). Exit 0 after 4,427 s (22:27:40 → 23:41:27), one resume try. Verdict **NEEDS-DECISION**.
- **Commits:** 8 on `feat/europa-port`, `07c853e..8bdcb4b`. The source changes are in `99e69a2`, `50ad76f` and `4dbcf9b`.
- **My receipts:** `code-receipts-094-38\` (gates, Paris lists, tree lists, polls) and `D:\IA_Europa\{Receipts,Temp}\CodeReview09438\` (`sweep-check.json`, `rootnames-check.json`, `safety-38.json`).
- **The 5.1 reference** is `release/2026-10-05` (`d7eee97`): the port's merge base, and +782/−81 lines in `Source/AnomalyInjector` at `07c853e`.

## 1. The diff against 5.1
- **LOD selection: fixed.** The 4.25-only argument block is gone, so `ApplyArgs` holds only `=<target>`. 5.1's default then applies: `WorstLodSentinel`, the coarsest LOD (`Anomaly_LodCorruption.cpp:29-32` and `AnomalyLod.cpp:143-151` at `d7eee97`).
  - ⚠ The same commit also removed the 4.25 multi-LOD candidate filter. 5.1 never auto-fires `lod_corruption`, and its shared `Apply` has no two-LOD guard (`lod_popping` has one). So an auto fire can now pick a single-LOD mesh and log "forced LOD 1 of 1": it is labelled but invisible (D3's class).
- **The sweep.** Codex lists 104 4.25-only lines and 54 compat calls (`docs/europa/09438-auto-parity.json`).
  - My own diff `d7eee97..8bdcb4b` finds no added line inside a 4.25-only guard missing from that list. The one hit my script raised is the shared "neither engine" `#error`.
  - The five removals are in the diff: the pool swap, the default-pool swap, the multi-LOD filter, the LOD argument, and missing_object's bench gate.
  - **Not in the list.** The inventory counts only lines that differ from the same branch's 5.1 path, in `AnomalyInjector`. Three things therefore fall outside it:
    1. **The shared extension (a decision item).** `GAutoPool` gains `lighting_mismatch` and `lod_corruption` on both engines, at positions 10–11, default off; `NumPoolKeys` goes from 9 to 11; and the lighting candidate block now runs on 5.1 too.
       - The first nine positions and the default set are unchanged, so seeded 5.1 draws stay the same unless someone enables the new ids.
       - But on 5.1, `IAI.Auto.Pool all` now enables 11 ids, `KeyPool[9]` and `KeyPool[10]` get no default key, and `IAI.Auto.Bind` still accepts only `pool1`–`pool9`.
       - Codex disclosed this as `shared_extension`. A pure removal would have broken the capture: `AnomalyCaptureLabels425.cpp:75` enables the four routes through `SetAnomalyEnabled`, which refuses an id that isn't in the pool.
    2. **API adapters that also apply to 5.1:**
       - the three subsystems now derive from `UAnomalyTickableWorldSubsystem`, and the Game/PIE rule moved from `DoesSupportWorldType` to `ShouldCreateSubsystem`;
       - `TObjectPtr` UPROPERTYs became raw pointers (6 members);
       - `TFunction::Reset` became `ResetFunction`.

       These are justified as 4.25 API differences. Equal behaviour on 5.1 is asserted, not tested here.
    3. **The 4.25 capture driver's own pool setup** (`AnomalyCaptureLabels425.cpp:47,74-76`): the four routes, the seed, a 0.8 s interval, a 0.4 s hold and at most 1 concurrent fire. It replaces 5.1's per-burst firing. It belongs to the accepted C-series design, not a leftover, but it is a 4.25-only firing choice.
- **The self-check line** (new `AnomalyCaptureDiagnostic425.cpp`, emitted at request 1 and request 300).
  - **Live reads:**
    - `viewport` = `FViewport::GetViewportType()`;
    - `client` = the game viewport client's runtime class name;
    - `viewport_size` = `GetSizeXY()`;
    - `size` and `format` = the captured frame;
    - `backbuffer_format` = the Slate backbuffer's RHI format, read in `OnBackBufferReadyToPresent` (it is 0 until the marker's first draw);
    - `RHI` = `GDynamicRHI->GetName()`;
    - `PostRenderViewFamily` = the backend's hook count;
    - the census line with lost / no-pass / polluted counts is unchanged;
    - `fixed_step` and `fixed_dt` come from `FApp`; `fixed_rate`, `fixed_fps` and `smoothing` from `GEngine`; `tick_calls` is a counter.
  - ⚠ **Not discriminating:**
    - `tick=` is the compile-checked name of the only caller (`OnWorldPostActorTick425`), so it is true but can never vary.
    - `tick_type` can only read `LEVELTICK_All`, because the caller returns on anything else.
  - **`pre_ui` is a pixel test:**
    - An 8-cell canvas marker is drawn from 2.5 s after start until 8 captures have been checked.
    - Up to 3 post-UI backbuffer reads must find it (7 or more of 8 cells), and none of the checked captures may contain it.
    - ⚠ The rows don't record which captures were rendered while the marker was drawn. The overlap rests on timing: polling starts at 3 s, so the 2.5–3 s frames sit in the backlog. A marker that was never drawn shows up as `ui_draws=0` and `pre_ui=0`.
  - **Runtime: NOT RUN.**
- **The far plane: exact.** `AnnotationFar` returns 1,000,000 unconditionally, and so does 5.1 (`Out.CamFar = GAnomalyDefaultFarPlane`, `d7eee97` `AnomalyCaptureSubsystem.cpp:573,10184`; Codex's `:594,10291` are the same lines on the port branch).
- **Comments:** none in the 203 added source lines or in the tools changes.

**The builds.** The PIDs match my sightings. All ran at BelowNormal, with `-MaxParallelActions=2`, one after another.

| Attempt | PID | IST | Start / min commit (GiB) | Result |
|---|---|---|---|---|
| Editor 1 | 44692 | 22:53 | 10.02 / 2.08 | FM1 stop (UHT) |
| Editor 2 | 40680 | 23:25 | 10.44 / 2.06 | FM1 stop |
| Editor 3 | 50244 | 23:27 | 10.58 / 7.93 | exit 6, at `5971792` |
| Game 1 | 34800 | 23:29 | 10.21 / 8.01 | exit 0, at `4dbcf9b` |

- **Editor 3's five errors** are all at `AnomalyCaptureDiagnostic425.cpp:117`: `ToSharedRef` called on a `TSharedRef`, then four `Serialize` overload errors.
- **The fix `4dbcf9b` compiled in Game:** the unity file `Module.AnomalyCapture.2_of_3.cpp` includes it, and its `.obj` was rebuilt at 23:30.
- **The Editor binaries are mixed:**
  - `UE4Editor-AnomalyInjector.dll` is new (from Editor 3);
  - `UE4Editor-AnomalyCapture.dll` is still 094-37's (19:04 IST), and UBT deleted its PDB.

  So nothing can launch until the Editor target rebuilds. **FM1's three Editor attempts are spent.**

## 2. The smoke — not run
No kit was armed and no session ran, so there is no map, no event, no mask to open and no LOD picture. Every FU5 and FV5 gate is unreached.

## 3. FV6 — not run (it needs a passing smoke)
- **No rewrite happened:**
  - the only local branches are `europa-e1` and `feat/europa-port`; there are no backup or `refs/original` refs;
  - `4f46a9c`, `47a6972` and `07c853e` are unchanged ancestors of `8bdcb4b`.
- **Scope for when it runs:**
  - By my count, only `docs/europa/09434-live-capture-receipts.json` holds a listing: its `/exit/roots` field, with 189 non-generic root names across C:, D: and E:.
  - That blob is unchanged since `47a6972`, so **22 commits** carry it (`47a6972..8bdcb4b`).
  - The other receipts JSONs have `root_changes` fields. Their only root-name matches are common words (Game, Logs, Builds…), not listings.
  - This job's new `09438-*` files add no listing.

## 4. Safety
- **Crash:** none.
  - No 4.25 editor, game or crash monitor was seen at any poll.
  - No new `Saved\Crashes` entry (the latest is from 10-27).
  - The one LocalAppData CrashReportClient log write, at 22:50, matches the foreign UE 5.7 `CrashReportClientEditor`.
- **Upload:** none (no session ran).
- **The Paris host, by my own hashes, before against after:** Content (1,456 files, `SampleMap.umap` `9D35E89E…`), plugin Content, Config, the descriptor and the `.uproject` are hash-, size- and mtime-equal.
- **Binaries:** they changed only while our UBT, cl or link was alive (6 change events, 0 without a build). Codex's hashes for the host exe and both editor DLLs match mine.
- **Git:**
  - `m51` equals P9 at start, prelaunch and exit.
  - The feature branch is clean at `8bdcb4b`: 8 commits with real dates (W4), no origin branch, no push and no tags.
- **Deletions:**
  - My tree lists went from 138,589 to 138,800 files. 4 files are gone, all UBT outputs during authorized builds (2 PDBs, one Intermediate `.lib` and one unity `.txt`).
  - One partial `.obj` was moved into Codex's quarantine, with a receipt.
  - No directory was removed, and there are 0 reparse points.
- **Foreign load:** UE 5.7 DCDemo games (2–3.9 GB), a UE 5.7 cl/link build at Normal priority, blender, wlanext (6.0 GB) and vmmem (4.0 GB). Free commit fell to 0.5 GiB.

## 5. Not written
The pre-push scan and the office card: both depend on a passing smoke.
- **The card will need two changes** from the D-lite card:
  - missing_object no longer needs `-IAIBench`;
  - the capture enables its own four routes.
