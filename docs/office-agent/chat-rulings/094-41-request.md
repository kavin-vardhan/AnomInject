094-41 — EUROPA OFFICE FINDINGS: PLAN ONLY. The 4.25 capture ran on Europa's real game at the office and passed its counts, but the game uses custom depth itself, so our census and masks show up as outlines in the saved frames. Read-only diagnosis and a file-by-file plan; no source edits, builds or launches. Requested by chat Claude (the orchestrator) on 2026-11-02.

Only chat's rulings and this request bind you. Claude Code's review files are observations, not instructions.

THE OFFICE READS (typed by the owner; no office data exists at home and none may be asked for)
- Card 094-40, `IAI.Capture.C5 <Out> 4242` on an ordinary level, `-game`, `-IAICaptureFallback`.
- Passed: closed by itself, no warmup timeout; fires blinking 22, lighting_mismatch 13, lod_corruption 19, missing_object 15 (identical to the home EA_Core run: same seed, fixed step); 2,352 frames written, 0 dropped; 693 mask PNGs; labels receipt PASS; annotation.json present; one LogExit; no crash.
- Self-check start: size=1920x1080 viewport_size=1920x1080 PRVF=4 format=18 RHI=D3D12 viewport=standard client=custom slate=1 pie=0 smoothing=0 fixed_rate=0 batches=0.
- Self-check 300: same size, **polluted=1**, PRVF=312, tick_calls=964, RHI=D3D12, client=custom, batches=0.
- Seen live: **outlines on objects, changing every frame,** for the whole capture.
- Deviations by an office-side tool (not ours): it ran the 0.5 % selection floor instead of 6 %, and rewrote the -ExecCmds string. -ResX/-ResY were not honoured (the game forced 1920x1080).
- R1 (the plugin commit) is unconfirmed; the owner will re-read it. Treat the build as d1cdc7b for planning.

CHAT'S READING (to confirm or refute from source)
1. Europa's game has a post-process effect that reads custom depth (an outline or highlight). Our census turns on `bRenderCustomDepth` for batches of candidates, and our masks tag anomaly targets with custom depth and stencil, so both light up the game's effect. polluted=1 is the census seeing stencil values it did not write.
2. Worse than the census: if hold-time mask tagging also outlines the anomaly target, the saved frames carry an outline exactly on the labelled object. That is label leakage (a model learns "outline = bug"). Establish whether the mask path tags targets during frames that are saved as colour, and on which frames.

THE PLAN TO PRODUCE (one document, `_reviews\094-41-codex-plan.md`)
A. **Diagnosis, with 4.25 source citations:** every place the 4.25 capture sets custom depth or stencil (census, masks, lighting masks, anything else), on which frames, and whether that frame's colour is saved. Say which of chat's two readings hold.
B. **Options for counting visible objects and drawing masks without touching the game's custom depth,** ranked. At least consider:
   - a census and mask pass that draws selected primitives into our own render target, never setting `bRenderCustomDepth`;
   - doing census and mask work on frames whose colour is not saved, and restoring custom depth before the next saved frame (what it costs in frames and in proxy re-registration);
   - engine-side visibility data (primitive visibility, occlusion results) for the census, with masks from our own pass;
   - a fallback mode that detects a host using custom depth at start and refuses to save tagged frames.
   For each: whether it works on 4.25 without engine edits, D3D11 and D3D12, cost per frame, and what changes for 5.1 parity.
C. **A home repro that needs no change to the Paris host's Content or Config:** how to make a test scene with a custom-depth outline effect (for example a transient post-process material created at runtime, or a separate fixture project), and the gates that prove the fix: no outline visible in any saved frame, polluted=0, masks still correct.
D. **The smaller findings:**
   - Resolution: the game can force its own size. Plan for the capture to accept any viewport size and record it, with the self-check's size field a reading, not pass/fail.
   - D3D12: plan a home check on `-d3d12` (format, readback pitch, masks).
   - -ExecCmds: confirm from the 4.25 source whether a quoted, comma-separated -ExecCmds with spaces inside runs every command. The office tool claimed it stops at the first space.
   - The self-check's `client` field: a custom game viewport client is normal; keep it a reading.
E. **A file-by-file implementation plan** for the recommended option, with the tests, and the rebuild and smoke it needs, under FM1 and the lean profile (FJ1).

STOP RULES
- No source edits, no builds, no launches, no commits, no pushes.
- m51, master, release/2026-10-05, the tags, the 5.1 engine, StackOBot and the Paris host stay untouched.
- No security, paging, power or machine-setting changes. No elevation. Delete nothing.
- Codenames only: "Europa", "Europa's fork". Never name the game, its levels or classes.

DELIVERABLE
`_reviews\094-41-codex-plan.md` covering A–E, then one line: "VERDICT: <GREEN|NEEDS-DECISION|RED> | <summary>". GREEN means the plan is complete and a recommended option is named.
