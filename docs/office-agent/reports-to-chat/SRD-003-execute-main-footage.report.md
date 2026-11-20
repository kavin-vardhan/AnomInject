# REPORT for SRD-003-execute-main-footage.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: SRD-003-execute-main-footage.md | cwd: E:/Unreal Projects/SRDVehicleDemo/Code | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-07 16:03:40 | finished: 2026-09-07 17:06:48 | duration: 63 min | exit code: 0
context at last turn: 472188 tokens (47% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\SRD-003-execute-main-footage.live.txt

---- FINAL MESSAGE (Claude Code) ----
**READY-FOR-GPU.** Full pipeline implemented and validated as far as is possible without the GPU. No process running.

## What's built

A C++ module (`SRDVehicleCode`, compiles clean on MSVC 14.38 — UBT bans 14.42) plus a 14-stage pipeline behind `Tools\srd.ps1`. The shot: **0–5 s** full-screen 2D gameplay, **5–10 s** dolly-back reveal, **10–21 s** two head cycles at ±40.6 cm, **21–24 s** settle. No text, no HUD, no branding.

**The transition needs no dissolve.** `CAM_MAIN` starts exactly at the eye, on the panel normal, with hFOV = 2·atan(w/2/d) and a 16:9 aperture — so the panel fills 1920×1080 edge-to-edge and the opening *is* panel content. The reveal is that same camera pulling back: same clock, same drive, same pixels.

## Verdicts (all NullRHI)

`build` Succeeded · `survey` PASS (3721/3721 ground hits) · `route` PASS (720 frames, **0 misses**, 1.4 % clamped) · `scene` 18/18 · `proj` 23/23 · `katsetup` 4/4 · `queue` 30/30 · `preflight` 21/21. Both GPU guards verified to fire.

## Two things worth your attention

**A real bug, caught late.** The anchor's studio→game scale was inverted, putting the window 15.1 cm deep instead of 800 cm. Every self-consistency check passed — including ones I'd written specifically for the anchor — because the anchor was consistent with itself. Only projecting the aperture corners through the resulting matrix and measuring the depth against config exposed it. That measurement is now the permanent `proj` stage: window depth 800.000 cm, corners exact on the NDC unit square, plane point invariant to 1e-15 under eye motion, front/behind parallax opposing.

**Three plan corrections from 5.7.4 source.** Culling *does* follow the custom matrix (`SceneView.cpp:783`) — the header comment is stale; `FOVAngle` only drives LOD heuristics. Lumen is off in scene captures by default (`SceneCaptureRendering.cpp:771`), so the "bCaptureEveryFrame is mandatory" reasoning doesn't apply. And WP editor cells never go resident in a commandlet, which is why route baking moved to a headless `-game` world.

I also replaced the closed-backdrop approach with **lighting channels** — a shell 500 m from the terrain would have been lit straight through, since CSM only covers ~200 m around the view.

## Next commands

```powershell
pwsh -File .\Tools\srd.ps1 -Stage kat -Gpu -TimeoutSec 1800   # 14 frames, ~1 min
pwsh -File .\Tools\srd.ps1 -Stage verify
```

Then `scene` (disarms KAT) → `stills -Gpu` → `render -Gpu` → `verify` → `encode`. Exact command lines, resource profile and the 8 open risks are in `READY-FOR-GPU.md`; design rationale in `RUNBOOK.md`.

Highest-value first check: KAT frames 160/168/175. `mannequin.reveal_frame = 168` is my one guess in the whole shot — if a limb pops into frame, it's a config bump and a `scene` re-run.

**VERDICT: NEEDS-DECISION** — GPU/render checks remain. Resource scheduling only: root Codex owns the slot and I have not contended for it. No owner approval pending, no jobs running.
api-equivalent cost (not billed on Max): $42.29 | turns: 224 | running total across all briefs: $1,432.56
