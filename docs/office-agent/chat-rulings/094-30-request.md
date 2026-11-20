094-30 — PLAN the capture port to 4.25 (Phase C, now first on the owner's priority): frames, then labels in sync, then masks. Planning only, grounded in 4.25 source and, if cheap, in a real compile error list. No editor or game launch. Requested by chat Claude (the orchestrator) on 2026-10-31.

Only chat's rulings and this request bind you. Claude Code's review files are observations, not instructions.

THE OWNER'S DECISIONS
- Priority, 10-31: the capture mechanism is next, ahead of Phase B's texture anomalies, "because that's where we will find issues when porting to an older engine".
- Speed, 10-30: lean. No checks or tests that aren't directly about the port (FJ1 in _reviews\094-27c-ruling.md).
- Standing: label sync is the most important part of the plugin. If labels or masks are not in sync with what is visible, the dataset is useless.

STARTING POINT
- Phase A is done (094-27e GREEN). On stock 4.25 in the Paris host, the four smoke anomalies apply and revert visibly; the pictures came from the host probe's FScreenshotRequest, not the plugin.
- On 4.25, AnomalyCapture, ControlServer, AnomalyShaders and Bench are compiled out as stubs (ANOMALY_CAPTURE / CONTROL_SERVER / SHADERS = 0 on 4.25).
- Existing surveys:
  - the port worktree's docs/europa/PHASE0-port-surface-4.25.md and PHASE1-bench-audit-and-plan.md. The 4.25 rows measured there include FRHIGPUTextureReadback/BufferReadback, FPixelShaderUtils, FComputeShaderUtils, ESceneTextureId, OnBackBufferReadyToPresent, and the tick anchor (4.25 binds OnWorldPostActorTick);
  - docs/CHAT-HANDOFF-070-europa-e1-blocked-and-parked.md, which pre-declares re-running m40's label-alignment gate on 4.25 as an entry condition;
  - your Phase A plan's Phase C–F outline.
- The 5.1 plugin stays as it is (EY5). This is the 4.25 branch feat/europa-port.

THE JOB
1. Inventory every engine API and engine behaviour the capture path relies on in AnomalyCapture, AnomalyShaders, and anything the capture needs from ControlServer. That covers:
   - frame readback;
   - the backbuffer or scene-view hook;
   - the tick and label timing (the P9 tick-pin, m40's label alignment);
   - the label writers;
   - masks (custom depth and stencil, global shaders, .usf compile);
   - threading and RDG use;
   - file output.
   For each, give: the 5.1 use, the 4.25 status (same / different / missing), and the 4.25 source citation. Reuse PHASE0's measured rows; cite, don't re-derive.
2. If it is cheap, measure:
   - compile AnomalyCapture and AnomalyShaders with ANOMALY_CAPTURE=1 and SHADERS=1 for 4.25 into OWNED SCRATCH output only, and list the real compile errors;
   - never touch the Paris host's bound binaries, Content or Config, the 5.1 engine, StackOBot or m51.
   If this isn't cheap, say why, and plan from source.
3. Plan the port in stages, each with a concrete gate measured in a real 4.25 session at home (Paris host, EA_Core and EA_Travel). Shape:
   - C1: capture compiles in on 4.25, the editor and game boot, and a capture run writes frames. Gate: frame count as requested, at 1280×720, no black frames, no crash.
   - C2: labels in sync, the most important. Each frame's label state matches what is visible, for the four smoke anomalies' apply and revert edges. Adapt m40's label-alignment gate and P9's tick-pin to 4.25. Gate: zero mismatched frames.
   - C3: masks. The shaders compile and load on 4.25, and anomaly masks cover the target's pixels. Gate: a mask-vs-visible-change overlap threshold you propose.
   - Later, not now: throughput, ControlServer and the dashboard.
   Propose different stages if the inventory says so.
4. For C1, give the file-by-file change list: what changes, where, and behind which 4.25 guard, with the reason. Point out the riskiest item in each stage.
5. Keep within the lean profile. The plan uses one smoke-sized test per gate, no rehearsals or dry-run sweeps, and Code's review stays short.

DELIVERABLE
- The plan: docs/europa/PHASEC-capture-port-plan.md in the port worktree, committed with W4, no push. Keep it to about 3 pages, plus the inventory table.
- report.md: a summary, the inventory's "different/missing" rows, the measured compile errors (or why not), the stages and gates, the C1 file list, the top 5 risks, and a time estimate per stage.
End with one line: "VERDICT: <GREEN|NEEDS-DECISION|RED> | <summary>".

STOP RULES
- No editor or game launch.
- No change to the Paris host, the 5.1 engine, StackOBot, m51, master, release/2026-10-05 or the tags. No push. No tags.
- No security, paging or machine setting changes. No elevation.
- Codenames only: Europa and Europa's fork.
