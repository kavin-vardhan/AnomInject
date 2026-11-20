---
name: nonintrusive-platform-relay-room
description: "2026-10-15 Relay Room (Claude+Codex) designed a UE4SS-hosted non-intrusive inject+label platform; PROPOSAL only, 5 open questions await the owner, Chat review before P0"
metadata:
  node_type: memory
  type: project
  originSessionId: <uuid>
  modified: 2026-10-15T11:11:11.226Z
---

On 2026-10-15 the owner asked for a Relay Room discussion on "a platform like UE4SS" to non-intrusively inject AND accurately label anomalies in UE games. Room "UE4SS-style non-intrusive anomaly injection + labeling platform" (5 replies, both at max). Outputs in `D:\IntrusiveAnomalies\StackOBot\_relay_room\2026-10-15-ue4ss-platform\`: the brief, the transcript, and `NONINTRUSIVE-PLATFORM-DESIGN.md` (the final + Codex's 7 closing fixes marked "[Codex fix N]", a 23-citation check, builder's notes).

Conclusions:
- UE4SS = reach only. Host it pinned behind a replaceable adapter; never fork.
- Only delivered-image evidence, on every frame, certifies a label. Coverage masks and before/after scene-capture brackets (ABAB, recorded as calibration interventions) are supporting evidence.
- Footprint levels: L0 nothing added to the game install, L1 proxy plus `ue4ss\` folder, L2 version-matched pak. Anti-cheat titles are a hard refusal.
- P0 is a 2–4 engineer-week GO/PROXY-ONLY/NO-GO proof on a packaged StackOBot build. GO only means "invest in P1". P0–P2 total 14–26 engineer-weeks.
- The UE4SS engine-tick hook exists only after stable v3.0.1 (`main` and experimental builds).

Prior art: The owner's own May 2026 ReShade prototype at `D:\AnomalyInjection`.

**Why:** the owner wants injection plus labeling without host source or a rebuild. The room's output is a proposal, not authorization.

**How to apply:**
- Build nothing until the owner answers the 5 open questions: audience, footprint ceiling, custom-depth caveat, PROXY-ONLY, anomaly looks.
- Chat review is REQUIRED before P0 is commissioned.
- If office titles come first, test the `-PLUGIN=<path>` switch first (UE 5.1.1, PluginManager.cpp:47-83), which may mount M3 without project edits.
- Relay Room for this project:
  - pass `--workspace D:\IntrusiveAnomalies\StackOBot` and `--marker "FINAL PLATFORM DESIGN"` (or the deliverable's marker);
  - Codex's global effort became `ultra` on 2026-10-15, so start the server with `RELAY_CODEX_EFFORT=max`;
  - set `RELAY_REPLY_TIMEOUT_MIN=60` for this project's big docs.

See [[m3-shipped-deimos-dropped]].
