# 094-32 — Code's short review (C1 closed with the 10-bit writer, C2 labels in sync)

**Date:** 2026-11-01, about 01:55 IST · **Author:** Claude Code (mailbox launcher and reviewer) · **These are observations, not rulings.**

**Inputs:** Codex's report (collected byte-identical as `_reviews\094-32-codex-report.md`, body 5,054 B, sha256 `56167CFE…`); Codex exit 0 after 3,350 s (00:42:17 → 01:38:06 IST), VERDICT GREEN; my receipts in `code-receipts-094-32\` and `D:\IA_Europa\Receipts\CodeReview09432\`; my scripts in `D:\IA_Europa\Temp\CodeReview09432\`.

## 1. FN2, the 10-bit writer
- **Source matches 4.25.** `AnomalyPixel425.h` decodes R = bits 0-9, G = 10-19, B = 20-29, alpha ignored, with `(v*255+511)/1023`. 4.25 maps `PF_A2B10G10R10` to `DXGI_FORMAT_R10G10B10A2_UNORM` (`D3D11Device.cpp:164`), and its bitfield struct is `R:10, G:10, B:10, A:2` (`D3D11RenderTarget.cpp:547-553`). Accepted formats are exactly B8G8R8A8, R8G8B8A8 and A2B10G10R10; anything else is counted in `Failures` and never written.
- **My conversion test (`fn2-conversion-test.json`): PASS.**
  - The formula equals exact round-half-up for all 1,024 inputs and reaches all 256 outputs.
  - The three synthetic vectors decode to (0,0,0), (255,128,0) and (0,127,255).
  - 200,000 random packed words decode with 0 failures.
  - The header hash equals Codex's receipt (`3765cbe9…`).
- **On real frames.** 4.25's own readback converts 10-bit by truncation (`RHISurfaceDataConversion.h:94`, `Quantize()` truncates). So the plugin's frame 0 minus the engine's diagnostic read of the same backbuffer must be 0 or +1 on every channel. It is, in all three C1 sessions, with no other value:

  | Session | Channels at 0 | Channels at +1 |
  |---|---:|---:|
  | PIE01 | 1,666,046 | 1,098,754 |
  | Game01/02 | 1,507,783 | 1,257,017 |

  A layout or channel-order error could not produce this.
- **No override anywhere.** No backbuffer override in any of the six sessions' `prepared.json` argv, their logged command lines, or the 6 editor/game launches my poll recorded. The kit refuses one (`require_frame_format`).

## 2. C1, from the raw files
| Session | What the files show |
|---|---|
| **PIE01, EA_Core** | **PASS.** 60 PNGs, indices 0-59, none missing or duplicated, list equal to `capture.json`. 1280×720, 0 black, format 18, D3D11, dropped 0, pitch 1280 / height 720, 64.2 s. Region against the independent Phase A EA_Core reference: mean 0.50, fraction 0. `LogExit: Exiting.`, exit 0. |
| Game01, EA_Travel | FAIL, kept. Same counts and format; region against the EA_Core reference: mean 10.63, fraction 0.198. |
| **Game02, EA_Travel** | **PASS** against Game01's engine readback. Same counts, format 18, D3D11, dropped 0, 9.14 s, normal exit. |

- **The frames I opened.** PIE frames 0, 30 and 59 show the white-cube fixture. Game02 frames 0, 30 and 59 show the same camera (`EA_Camera`, -550/0/500) and the same geometry, with grid-textured side cubes.
- **The fixture claim checks out.** `EA_Core.umap` holds 2 `BasicShapeMaterial` references and an `OverrideMaterials` entry; `EA_Travel.umap` holds none.
- **Not new evidence.** Game02's 60 frames are byte-identical to Game01's. Game02 is the same pixels re-judged against a new reference, not a second capture result. Codex discloses this.
- **The weaker reference.** The new reference is the engine's read of the backbuffer, taken by the plugin's own diagnostic in Game01. It is independent of the plugin's copy, staging and decoder, but not of the rendered scene. No Phase A picture of EA_Travel exists. So the Game half of C1 rests on that read plus the visual check, not on an independent fixture picture.
- **Fresh frames.** Each session has 7 distinct images repeating with period 7; no two consecutive frames are equal. That fits the tonemapper's per-frame grain on a static scene.
- **Finding:** the plugin's own heartbeat text, "[IAI] AnomalyInjector ticking (active: N/4, scoping: OFF)", is burned into every 4.25 frame (C1 and C2). The 4.25 path never calls `SetOverlaysSuppressed`, which the 5.1 run does. It is periodic and does not track per-frame state, so it isn't a per-frame label leak. Every gate ROI excludes it. But it would ship in a dataset.

## 3. C2, independently
- **Structure.** 480 frames: 24 events of 20 frames, three per anomaly per order. Order 0 is frames 0-239 (Pace 1); order 1 is frames 240-479 (synthetic order, Pace 0). Then 12 EA_Travel frames under epoch 2, a new world owner, and no labels.
- **The labels are not checked against themselves.** Labels come from the PostActorTick sampler. The reader classifies pixels against Phase A probe pictures, and the reference plan and prediction were frozen at 01:23, before the 01:28 launch.
- **My own classifier agrees exactly.** It uses its own clean baseline from each event's first two frames, a per-event change mask, and no Phase A references (`c2-independent.json`).
  - **0 mismatches** over 480 frames, 0 foreign labels.
  - Edges equal the reader's at every event.
  - Clean versus active separation is at least 133× (lod_corruption 133×, blinking 211×, missing_object 211×, lighting 285×).
  - The reader, re-run by me: PASS, 0 mismatches; its worst ambiguity is 0.022 of its limit.

| Anomaly | Order 0: apply / revert / interior | Order 1: apply / revert / interior | Mismatches |
|---|---|---|---:|
| blinking | 3 / 3 / 6 | 3 / 3 / 6 | 0 |
| lighting_mismatch | 3 / 3 / 0 | 3 / 3 / 0 | 0 |
| lod_corruption | 3 / 3 / 0 | 3 / 3 / 0 | 0 |
| missing_object | 3 / 3 / 0 | 3 / 3 / 0 | 0 |

- **The pacing-off block ran.** Labels carry `pace 0`, `fixed_step false` for 240-479; the log has `Capture425 C2: tick_order=1 pace=0 fixed_step=0`.
  - ⚠ On 4.25, `IAI.Capture.Pace` is a new variable meaning fixed step on or off. It is not 5.1's wall pacing, which has no 4.25 implementation yet.
  - **Measured:** the Pace 1 block ran fixed 1/30 s steps at about 5 ms of wall time per frame (7.97 s of game time in 1.51 s of wall time). That is 5.1's old "pace-off" regime from m21, where the −1 once appeared. The Pace 0 block ran variable steps held to about 30 fps by `t.MaxFPS 30`.
  - Both regimes read 0 mismatches.
- **The negatives, run by me against the real files:**
  - **Codex's three kept mutated label files:** the reader refuses each with "Pixel/label mismatch" (apply 24 shifted rows, revert 24, blink interior 12). My classifier also flags exactly those rows.
  - **My own shifts,** one frame late and one frame early, at apply only, revert only and blink interior only: **all six fail**, and each flags exactly the shifted indices.
- **Scope, for chat:**
  - Events were driven by a bench driver at PreActorTick. The 4.25 production fire path is not exercised, and the exporter is bench-shaped (fixed 20-frame events).
  - The two orders give identical edge phases (blink +6/+10/+14/+17 in both). That is expected with a PostActorTick sample after both dispatch points, so the tick-order lever doesn't act as a can-fail here; the shift negatives do.

## 4. Safety
- **Crashes and uploads:** none. All six sessions exited 0 with `LogExit: Exiting.`, `crashed false`, no new crash-report folder, and nothing new in host `Saved\Crashes`. Every upload-check passed, and neither CrashClient nor Runtime logs have a send or upload line.
- **Paris host:** Content (1,456 files), plugin Content (5), Config (18), the descriptor and the `.uproject` are hash-, size- and mtime-equal between my 00:41 and 01:39 lists (`paris-diff.json`).
- **m51:** HEAD `53bf725`, index `526aba7f`, 0 tracked changes and the same 15 untracked entries, at entry, launch and exit. master, release and the 30 tags are unchanged.
- **Binaries:**
  - Changed only in two windows with UnrealBuildTool alive: 00:50:20-00:51:21 and 01:24:17-01:25:39. Net change is the host exe/pdb and `UE4Editor-AnomalyCapture.dll/pdb`.
  - The four builds ran one at a time, BelowNormal, `-MaxParallelActions=2`, with 0 errors and no memory stop.
  - The bound 27-file manifest (`c2-425-binaries.json`) is hash- and size-equal on disk.
- **Feature branch:** `feat/europa-port` at `e19fefc`, clean, no origin branch, no tags; nothing pushed.
- **Docs-discipline nit:** `docs/gotchas.md`'s 094-31b paragraph was rewritten in place rather than superseded by appending (the file is append-only).

## 5. Next blocker for C3 (masks)
`AnomalyShaders` is still compiled out on 4.25 (its rules require 5.1). C3 has to port the visible-mask and reduce shaders and a 4.25 post-render mask hook, and freeze FL4's per-anomaly IoU, recall and spill thresholds, before any session. The heartbeat-overlay suppression should land on the 4.25 capture path first.
