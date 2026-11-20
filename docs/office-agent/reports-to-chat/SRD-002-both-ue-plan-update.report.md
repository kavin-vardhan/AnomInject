# REPORT for SRD-002-both-ue-plan-update.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: SRD-002-both-ue-plan-update.md | cwd: E:/Unreal Projects/SRDVehicleDemo/Code | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-07 15:47:13 | finished: 2026-09-07 15:59:30 | duration: 12 min | exit code: 0
context at last turn: 103993 tokens (10% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\SRD-002-both-ue-plan-update.live.txt

---- FINAL MESSAGE (Claude Code) ----
Plan written to `E:\Unreal Projects\SRDVehicleDemo\Code\PLAN-UE.md` (120 lines). Nothing was constructed, compiled, rendered or launched; no Blender probes re-run; no changes outside the Code workspace. (One stray directory listing that hung on the E: drive was stopped — no in-flight work remains.)

**Summary**

The plan covers the Code project only and is driven by a single `Tools/srd.ps1 -Stage <s>` dispatcher over editor Python — the project is Blueprint-only with no Modules, so no C++ or custom plugin source is involved, only the built-in `PythonScriptPlugin`/`SequencerScripting`/`MovieRenderPipeline` set. It separates a far-placed `STUDIO` root (panel, wedge, camera bar, character, eye locator, `CAM_MAIN`, HERO proxy) from the game side, with one `SceneCapture2D` → linear RGBA16f render target → unlit panel material as the FAITHFUL path, and one game→presentation similarity governing panel corners, eye and HERO proxy alike, asserted per frame rather than claimed. The off-axis projection is rebuilt in Unreal's actual convention, checked against the installed 5.7.4 source: view space is X=right/Y=up/Z=forward (`SceneCaptureRendering.cpp:1136-1141`) and reversed-Z infinite-far, and because the custom matrix is taken *before* the `CustomNearClippingPlane` branch (`:1145-1153`) the near plane exists only in row 3, so N is asserted against the minimum per-frame eye-to-car distance instead of being assumed safe. The header's "does not currently affect culling" warning, together with the raw `FOVAngle` still handed to the renderer at `:1371`, is treated as a live hazard with a rendered corner-marker sweep (KAT-3), and shader/capture gamma, view-dependent Lumen, temporal accumulation, hidden-actor coverage and world-partition landscape streaming are each written as measurements to run, not guarantees. HERO is a constant-scale miniature carrying a permanent world-space panel-plane clip (so there is no mask-release event), staged near the tracked eye then cut low once clear; animation is baked from a shared 780-row route CSV because `bSubsteppingAsync=True` makes Chaos unusable as a reference; and output is MRQ PNG → ffmpeg with a per-frame emissive beacon decoded back out of the PNGs to prove capture/frame synchronisation rather than assert it.

Retained from the first plan: plane-point invariance alone does not prove full-image seamlessness (KAT-2 reports residuals from a spectator sweep), and KAT-0 matrix checks are explicitly marked as *not* a rendered PASS. Three items are left open for you in §12: who authors the canonical route CSV and how it reaches Codex, the studio offset/backdrop form pending the world-partition test, and the character asset — the template ships no character.

Chat review not required for this video. Next checkpoint is your plan approval, then still review before full sequences.

VERDICT: NEEDS-DECISION
api-equivalent cost (not billed on Max): $2.84 | turns: 32 | running total across all briefs: $1,390.27
