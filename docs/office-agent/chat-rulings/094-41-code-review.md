# 094-41 — Code's review of Codex's plan (diagnosis confirmed, including label leakage; the recommended option is buildable, but it carries an unsized shader-recompile risk on Europa; one runner-up was missed)

**Date:** 2026-11-02, about 23:45 IST · **Author:** Claude Code (mailbox launcher and reviewer) · **These are observations, not rulings.**

**Inputs:**
- **The plan:** `_reviews\094-41-codex-plan.md`, collected byte-identical from `code-receipts-094-41\fresh-run-094-41\report.md` (sha256 `C3869321…`), with one header line added.
- **How it ran:** the brief's resume of thread `01a14334` without `-OwnerApprovedFullAccess` was refused twice by `relay_resume_codex_impl_v1.ps1`. That script refuses any resume without the flag (exit 7) and wrote nothing; the run folder was unchanged after both tries. As the brief allows, I started a **fresh thread**, `01a19d4a`, through the review runner (`-s workspace-write` confined to its own folder, no network, gpt-6-astra at max). It exited 0 after 2,030 s (22:54–23:28) with verdict GREEN.
- **My receipts:** `code-receipts-094-41\`, holding the gates, the Paris lists and diff, the citation check (`citecheck-a.txt`), the command scan, the polls and the launch receipts.

## 1. Diagnosis: confirmed, label leakage included
- **Every citation checks out:** I tested all 90 cited ranges at `d1cdc7b`, in `E:\EpicGames\UE_4.25` and at `d7eee97`. Each holds the claimed content (`TOTAL=90 FAIL=0`).
- **The writers, all confirmed:**
  - **The global stencil mode:** `r.CustomDepth` is forced to 3 for the run (`AnomalyMasks425.cpp:54`, `AnomalyStencilTag.cpp:653-674`).
  - **Census batches:** about `min(free/2, free-8)` ≈ 23 actors per batch, two batches in flight, re-tagged every request (`AnomalyMasks425.cpp:182-207`, `AnomalyCensusPolicy425.h:8`). This matches "outlines changing every frame".
  - **Event tags:** they are held for the whole live fire (`:153-180`). For lighting, every actor the light reaches is tagged (`:104-110`).
  - 🆕 **A third writer, which I missed and Codex found:** `IAI.Capture.C5` sets `CaptureStreet425=true` (`AnomalyCaptureLabels425.cpp:359`), so the street bench runs on every C5. It plants custom depth on the first three name-sorted candidates (`AnomalyStreetBench425.cpp:42-56`: values 203 on, 18 on with `ERSM_1`, 19 off) for the whole capture. **At the office these were real game objects.**
- **Label leakage: YES, by source.**
  - `Selection` runs `Advance` (the tagging) inside `RequestFrame` for the same `GFrameCounter` (`AnomalyCaptureBackend425.cpp:38`, `AnomalyMasks425.cpp:327`).
  - The engine then flushes end-of-frame updates (`SceneRendering.cpp:3736`) and draws custom depth before post-process (`DeferredShadingRenderer.cpp:1826/2149` vs `:2802`).
  - The colour of that same family is copied at `PostRenderViewFamily` (`AnomalyViewMetadata425.cpp:92-97`).
  - **So the labelled target carries custom depth on every saved frame of its live fire.** That includes blink's visible frames that are labelled clean.
  - **Worst case, hidden objects:** for missing_object and blinking-hidden frames, custom depth is the only pass left drawing the object (`AnomalyHiddenClass.cpp:104-114`, `SceneVisibility.cpp:2248`). A host outline effect would draw **the outline of the object the label says is missing.**
  - ⇒ **The office's 2,352 frames are not training-clean.**
- 🔻 **Two corrections to my own records:**
  - **(a) The value 203 was planted, not host-owned.** My Code-added note 3 and my 094-40 review called "EA_BlinkCube / DefaultPawn_0 (203)" host-owned. They were planted by C5: `Receipts\09440\…\epoch01\depth-before.json` marks DefaultPawn_0=203, EA_BlinkCube=18 and EA_Floor=19 as `bench_setup:true`.
  - **(b) Setting the flag doesn't recreate the proxy.** In 4.25, `SetRenderCustomDepth` and `SetCustomDepthStencilValue` are direct proxy updates (`PrimitiveComponent.cpp:3665/3680`). Only a write-mask change marks render state dirty (`:3690`).
- **`polluted=1`:** I agree with Codex's narrower reading (one batch saw an unrecorded value in 200–254). The planted 203 is not the cause: the street bench starts before the mask owner (`AnomalyCaptureLabels425.cpp:68-74`), so 203 is snapshotted into `HostReserved`.

## 2. The options
- **The recommended option (a private ID/depth mesh pass) is buildable on stock 4.25 without engine edits.** The pieces it needs are exported:
  - `FMeshPassProcessor`'s constructor (`MeshPassProcessor.h:1550`);
  - `DrawDynamicMeshPassPrivate` (`:1686`);
  - `FMeshMaterialShader` (`MeshMaterialShader.h:57`);
  - `AnomalyCapture.Build.cs:361-364` already adds Renderer plus its private includes on 4.25.
- **It is RHI-agnostic,** so the same code serves D3D11 and D3D12. The two still need separate qualification, as the plan says.
- ⚠ **The one risk that matters, which the plan names but doesn't size:** in an editor build, a material's shader-map ID includes every shader type that compiles for it (`MaterialShared.cpp:534-545`). A new plugin `FMeshMaterialShader` type therefore changes the ID of every material it accepts, and that **whole** shader map recompiles on the office editor's first launch. On a real game that could be long, and C5 waits only 120 s for warmup.
- **A refinement worth checking first:** drive the private pass with the engine's own depth-only shader types, which are already compiled for every material for the prepass. Find them by type name and write IDs as the stencil reference into a **private** depth-stencil target. That mirrors the engine's custom-depth pass without touching the host's attachments or adding shader types. *Not verified buildable;* the type lookup and bindings need a 4.25 source check.
- **A runner-up Codex missed: a same-frame scrub.**
  - **How it works:**
    - force `r.CustomDepth.Order 0`, so custom depth draws before the base pass (`:1826`);
    - measure at `PostRenderBasePass_RenderThread` (`:2039`, `SceneViewExtension.h:126`);
    - then clear our own stencil values from custom depth with a global-shader pass, before translucency (`:2414`) and post-process (`:2802`).
  - **For it:** cheap, and no material recompiles.
  - **Against it:** it is leakier. Our geometry still occludes the host's custom-depth objects, which leaves outline-shaped holes; DBuffer readers see it (the cvar's own help says Order 0 enables custom depth for DBuffer); any fork pass in that window sees it; and it forces two cvars.
  - It is a fallback, not a replacement for the recommendation.
- **Option 4, detect and refuse:** I agree. The 5.1 scanner is compiled out on 4.25 (`AnomalyCaptureFeatures.h:3`); `UsesSceneTexture` exists there (`MaterialShared.h:1228`); a negative scan can't rule out a native reader.

## 3. The home repro
- **It avoids touching the Paris host's Content and Config: yes.** It uses a separate project folder and the installed, untouched engine.
- **Three practical notes:**
  - it must be a **C++ project**, or it can't build a Game target with the plugin;
  - its **first build is a full plugin build,** and FM1 is written for incremental builds;
  - put it on **E:**, because D: swung 22.0 → 2.5 → 12.8 GiB today as the system-managed page file grew under foreign load.
- **Why the home fixture can't show outlines today:**
  - `EA_Core` and `EA_Travel` each hold a PostProcessVolume with **0 blendables** and no CustomDepth strings;
  - `author_425_fixture.py:95-102` sets exposure and blur only;
  - the Paris Config has no custom-depth keys.

## 4. The smaller findings: I agree with all four
- **Resolution:**
  - Labels latch `Size` at start (`AnomalyCaptureLabels425.cpp:64/122/310`).
  - 4.25 maps are accepted only if pitch equals width (`AnomalyReadback425.h:9-11`), and masks copy `W*H*16` in one block (`AnomalyFrameCapturer.cpp:514`). The two must change together.
  - At 1920, colour rows (7,680 B = 30×256) and mask rows (30,720 B = 120×256) are both aligned, which fits the office's 0 dropped.
- **D3D12:** the pitch comes from the copy footprint (`D3D12RenderTarget.cpp:1462`).
- **-ExecCmds:**
  - A quoted value runs every command (`Parse.cpp:243-256`, `UnrealEngine.cpp:1586-1604`).
  - **I add one step to Codex's chain:** `LaunchWindows.cpp:161-197` re-inserts the quotes after `=` once `CommandLineToArgvW` strips them, so a typed `-ExecCmds="a b,c"` survives to the parser.
- **client:** a reading.

## 5. Safety: none
- **Codex wrote only inside its own folder:** 45 commands, 0 flagged; 1 file change (`report.md` in its own folder).
- **No editor, game, build or shader process ran:** none sighted in 4 polls (20-second sampling).
- **The Paris host is unchanged:** Content 1,456, plugin Content 5, Config 18, Binaries 16+11, the descriptor and the `.uproject` are hash-, size- and mtime-equal; both Intermediate listings are equal.
- **Git is unchanged:**
  - `m51` equals P9 at start, at all three prelaunches, at exit and at the final check;
  - all 74 refs are hash-equal (`8a6d9e1c…`), with `feat/europa-port` = origin = `d1cdc7b`, clean.
- **Nothing was deleted or killed.**
- **Foreign load, not ours:**
  - ComfyUI held 11.3 GB (pid 46144);
  - another session's four `r9.py` renders held about 2 GB each;
  - free commit fell to 1.2 GiB at 23:06 and recovered.
