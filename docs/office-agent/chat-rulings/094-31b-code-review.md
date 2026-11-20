# 094-31b — Code's short review (C1 build + sessions)

**Date:** 2026-11-01, about 00:40 IST · **Author:** Claude Code (mailbox launcher and reviewer) · **These are observations, not rulings.**

**Inputs:** Codex's report (collected byte-identical as `_reviews\094-31b-codex-report.md`, body 7,269 B, sha256 `C7163DC7…`); Codex exit 0 after 12,118 s (21:09:45 → 00:31:43 IST), VERDICT NEEDS-DECISION; my receipts in `code-receipts-094-31b\` and `D:\IA_Europa\Receipts\CodeReview09431b\`.

**Entry note:** 094-31's processes were still alive at 21:04: its Codex tree and a kit build wrapper (`build09431.py`, python 3928) still waiting at the 16 GiB line. I stopped 14 PIDs, each matched by name and start time (`stop-09431-leftovers.txt`). No UnrealBuildTool had ever started.

## 1. Build
- **First build** (Editor 01, 21:19:54 IST, start 8.26 GiB free commit, minimum 6.05): 14 actions on 2 processes. **2 errors, both in AnomalyCapture**: `AnomalyFrameCapturer.cpp(406)` C2065 `GPixelFormats` and the cascade C2789 `BPP`. 0 warnings. AnomalyInjector and EuropaPhaseAProbe had 0 errors. My own scan of `UBT.log` gives the same counts.
- **Fix:** a 4.25-only `#include "RenderUtils.h"` (`024e5fc`). Editor 02 (5 actions) and Game 01 (9 actions): 0 errors, 0 warnings, exit 0.
- **FM1:** every UnrealBuildTool I saw (PIDs 48212, 6248, 43800) ran BelowNormal with `-MaxParallelActions=2`, and its cl/link children were BelowNormal too. Attempts: Editor 2 of 3, Game 1 of 3. Start readings 8.26, 8.27 and 8.17 GiB. The lowest reading during any build was 5.85 GiB, so the 3 GiB stop never fired.

## 2. Diff (`4f46a9c..32c5bf5`)
- **5.1 paths:** 95 `#if ANOMALY_CAPTURE` guards became `ANOMALY_CAPTURE_ADVANCED`, which is `ANOMALY_CAPTURE && ANOMALY_ENGINE_51`, so they are unchanged on 5.1. The new code sits behind `ANOMALY_CAPTURE_FRAMES425` or `ANOMALY_ENGINE_425`. `AnomalyCapture.Build.cs` still adds Renderer, AnomalyShaders and UnrealEd on 5.1. The 5.1 paths are intact by construction, but nothing was compiled on 5.1 this job (lean profile).
- **SHADERS and CONTROL_SERVER:** neither Build.cs changed; both still require `Is51 && !Shipping`, so both are 0 on 4.25.
- **FL3:** `AnomalyReadback425::Accept` requires data, a non-zero size, pitch equal to width, and mapped height equal to height. The 4.25 drain calls it, and a refused map is dropped, counted (`GuardDrops`) and never written.
  - The negative test is six `static_assert`s: the exact map passes; padded (1344), short pitch, short height, 0×0 and null fail. They compiled in both targets.
  - It tests the predicate at compile time. No malformed GPU map was injected (Codex says the same).
  - Side effect: pitch must equal width, so any row-padded map is dropped. 1280 is safe (5,120-byte rows); other widths may drop every frame.
- **G488:** at `docs/gotchas.md:10286`, added in the first C1 commit `a2cc30d`.
- **New this job, needs a ruling:** every C1 launch now carries a process-only `-ini:Engine:[/Script/Engine.RendererSettings]:r.DefaultBackBufferPixelFormat=0`, and the kit refuses a launch without it. No Config file changed (hash-equal). But with stock 4.25 defaults (a 10-bit `PF_A2B10G10R10` backbuffer), the 4.25 session writes nothing: it accepts only B8G8R8A8/R8G8B8A8 and stops at the first refused frame. The 5.1 path already converts format 18 (`AnomalyLabelWriter.cpp:444`). Europa's fork backbuffer format is unknown.

## 3. Sessions (from the raw files)
| Session | What the files show |
|---|---|
| PIE 01, EA_Core, no override | RHI D3D11. One frame armed: 1280×720, pitch 1280, **format 18**. `capture.json`: written 0, requested 60, dropped 1. 0 PNGs. `LogExit: Exiting.`, exit 0. No frames to open. |
| PIE 02 | Kit's FE3 child-command guard stopped it before capture (exit 125, deliberate owned stop). 0 PNGs. |
| Game 01 | Same guard stop. 0 PNGs. |
| **Game 02**, EA_Travel, with the override | **60 PNGs, indices 0–59**, none missing or duplicated, file list equal to `capture.json`. 1280×720, format 2, AA 0, **D3D11, dropped 0**, pitch 1280 / height 720, 9.083 s. Normal exit (`LogExit: Exiting.`, 0). **Pixels FAIL:** all 60 files are byte-identical (one hash), every RGB value is 0. The check region is entirely zero (Codex: mean error 216.997, fraction 1.0, against limits 8 and 0.10). |
| Smoke 01, 02 | Six bindings PASS, exit 0. |

- **Frames 0, 30 and 59 of Game 02, opened:** each is a solid black 1280×720 image with nothing visible.
- **Observation, not a cause:** 4.25's tonemapper adds per-frame grain (094-27d), so 60 byte-identical frames point to the copy or readback returning zeros, not to a dark rendered scene. Candidates are the backbuffer → owned-texture `CopyTexture` and the owned → staging resolve on D3D11. Neither is established.
- **PIE never ran with the override** (slot 02 died in the guard), so it is unknown whether PIE gets real pixels.

## 4. Safety
- **Crashes:** none. Every slot shows crash_count 0, no new crash-report folder, and nothing new in `Saved\Crashes`. The two exit-125 results are the kit's own stops.
- **Upload signs:** none. Every upload-check passed and the CrashClient logs have no send or upload line. My log scan's only hit is the argv echo of the opt-outs.
- **Paris host:** Content (1,456 files), plugin Content (5), Config (18), the descriptor and the `.uproject` are equal in hash, size and mtime between my 21:06 and 00:33 lists.
- **Binaries:**
  - Host: 4 changed (`StylizedParisStreet.exe/.pdb`, `UE4Editor-EuropaPhaseAProbe.dll/.pdb`) plus 3 mtime-only (`.exp`, two `.target`). Plugin: 4 changed (AnomalyCapture and AnomalyInjector dll+pdb).
  - All 7 binary-change events in my timeline fell between 21:20 and 21:26 with UnrealBuildTool alive; there were none after.
  - **Manifest re-bound:** `Europa425-Runtime.json` points at `final-425-binaries.json` (sha256 `98d186d1…`, equal), and all 27 files are hash-equal on disk.
- **m51:** HEAD `53bf725`, index `526aba7f`, 0 tracked changes and the same 15 untracked entries, at entry, launch and exit. master, release and the 30 tags are unchanged.
- **feat/europa-port:** at `32c5bf5`, clean, no origin branch, nothing pushed, no tag.
- **Foreign load:** a ComfyUI python (`E:\AI\…`, ~23.5 GB from 23:19) and Resolve (6.8 GB) drove free commit down to 0.3 GiB. The kit held without launching; Smoke 02 waited 137 min.

## 5. Next blocker for C2
C1 has not passed: the 4.25 readback wrote 60 all-zero frames in `-game`, and PIE produced no frame. It needs a newly authorized session that checksums the source, copy and staging pixels, plus a ruling on supporting 10-bit backbuffers versus the 8-bit override.
