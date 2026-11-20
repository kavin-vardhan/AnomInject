---
name: europa-custom-depth-leak-094-41
description: "4.25 capture tags custom depth on saved frames (census, event targets, C5 street-bench plants), so a host outline effect leaks labels; the office's d1cdc7b frames are not clean"
metadata:
  node_type: memory
  type: project
  originSessionId: <uuid>
  modified: 2026-11-03T06:34:34.655Z
---

094-41 (2026-11-02) diagnosis, source-confirmed at `d1cdc7b`: the 4.25 C5 capture writes host-visible custom depth on the same frames whose colour it saves.
- **The writers:** `r.CustomDepth` is forced to 3; census batches of about 23 actors (two in flight); event targets for the whole live fire (all lit receivers for lighting); and the C5 street bench, which plants 203 on, 18 on (ERSM_1) and 19 off on the first three name-sorted candidates. `IAI.Capture.C5` sets `CaptureStreet425=true`.
- **The worst leak:** hidden-class targets keep custom depth, so a host outline draws the outline of the object labelled missing.
- **A home-record correction:** the EA_Core "host-on" components (DefaultPawn_0=203, EA_BlinkCube=18) are `bench_setup:true` plants, not host-owned.
- **A 4.25 fact:** `SetRenderCustomDepth` and the stencil value are direct proxy updates; only a write-mask change recreates the proxy.

**Why:** the office run on Europa showed outlines on every frame, with polluted=1. Its 2,352 frames are not training-clean.

**How to apply:**
- Codex's plan (`_reviews\094-41-codex-plan.md`) recommends a private ID/depth mesh pass.
- Its unsized risk: a new FMeshMaterialShader type changes every accepted material's shader-map ID in editor builds (MaterialShared.cpp:534-545), forcing a full recompile.
- Code's review proposes trying the engine's existing depth-only shader types, and lists a same-frame scrub as the runner-up.

**094-42r (2026-11-03), local f7b32f0 (unpushed):**
- FZ3/FZ4 are in. The guard scans before any tag (`Labels425.cpp:89`). Plants now land only on `EA_`-tagged actors of recognized fixtures. Routes default to blinking + lod_corruption. The EA_Core smoke passes, with the four routes passed through an `exec` macro.
- The outline fixture is at `E:\IA_Fixtures\Outline425` and is built. Its first authoring launch omitted `-nowrite`, wrote its own `Saved\Config\Windows\Engine.ini` and hit the FB2/FF2 stop. Repro, guard and FZ6(b) are unrun.
- In 4.25, `-nowrite` gates config writes only (`ConfigCacheIni.cpp:1478`), so package saves still work.
- FZ6(a): the depth-only route is buildable by source; reviewed in `_reviews\094-42r-code-review.md`.

**094-43a (2026-11-04), NEEDS-DECISION:** the by-name depth-only route (spike `c1193bd`, local) links (0 LNK), adds no material shader type (0 `Missing cached shader map`), writes no custom depth (P5 PASS), but fails P3:
- Masked (7 frames) and WPO (12 frames) read IoU 0: the private pass labels an object one frame before the picture shows it (scene depth empty there; the next frame matches).
- Offline, counting shown objects only where private depth == scene depth (the "drawn" bit) and keeping the visible test for hidden/missing targets passes P3 everywhere on that run (`p3sim_drawn43a_v2.py`). That is a chat decision.
- The fixture evidence planes `evidence-N.u32x4.zlib` are "ASP1" + zlib of 16 byte-planes (byte k of each pixel's 16 bytes), then uint4 = (tag, customZ, sceneZ, M|D<<8).
- Readers: `D:\IA_Europa\Temp\CodeReview09443a\` (p3review43a grouped by tag, p3diag43a, p5review43a, cost43a, gc6real43a, view708_43a).

Related: [[europa-425-editor-runtime-facts]], [[europa-port-pushed-d1cdc7b]].

**094-43r (2026-11-04), GREEN and pushed at `93613ae`:** FZ2 is in.
- **Stage A (spike `42ea37e`, GD3):** visible IDs are kept only at scene depth within 4/(2^24-1), and hidden selected targets in front of it.
  - Result: IoU 1.0 on every object, including masked and WPO; 0 new magenta; can-fail 474 of 588 frames on 094-42c's legacy repro.
- **Stage B (`ea08258`/`59c1282`):** `AnomalyPrivatePass425` keeps IDs in a plugin-owned depth-stencil attachment.
  - Its private depth starts as a copy of scene depth, so visible draws use hardware depth-equal.
  - Every custom-depth write is behind `Legacy()` (`-IAIBench` + the exact fixture map + `-IAICaptureLegacyReference`), and the bench plant is gone.
- **H1-H4 pass:** on the fixture, and on Paris EA_Core under D3D11 and D3D12, including 1279x719.
- **H5 (private pass):** GPU about 0.16 ms; render-thread submit 0.16 ms on D3D11 but 1.6-1.7 ms on D3D12.
- **Limit:** ISM, HISM and skeletal are tested only by the native policy test (no fixture has them).
- **Office:** card `_reviews\094-43-office-card-europa.md` (default routes blinking + lod_corruption; reads polluted, tagged, unavailable). Its §5 commands are tested on home output.
- **Readers:** `D:\IA_Europa\Temp\CodeReview09443r\`: p3review43r, gd3rule43r, exacteq43r, groupunion43r, p4review43r (magenta), h4check43r (the annotation uses type `blink`, not `blinking`), cost43r, cardtest43r.bat.
- **Pixel format:** in 4.25, format 18 = PF_A2B10G10R10 (10-bit), not B8G8R8A8.
