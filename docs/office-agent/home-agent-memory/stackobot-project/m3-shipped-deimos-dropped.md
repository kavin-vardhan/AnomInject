---
name: m3-shipped-deimos-dropped
description: "M3 client delivery shipped as release/2026-10-05 (d7eee97, exe 218BA4C7); owner says Deimos M3 will NOT ship (2026-10-10)"
metadata:
  node_type: memory
  type: project
  originSessionId: <uuid>
  modified: 2026-10-10T15:42:09.205Z
---

M3 (the client re-delivery for both office games) shipped from plugin branch `release/2026-10-05` @ `d7eee97` (code == `23098fa`, exe `218BA4C7`, archive `E:\IA_BuildCache\_binary_baselines\release-2026-10-05-23098fa-218BA4C7`). On 2026-10-10 the owner said M3 for Deimos (the first office host, virtual-texture blocker, 092-xx VT jobs on `feat/vt-proxy-blur` / `feat/vt-blur-stacks`) "won't be shipped".

**Why:** Deimos's VT problem needed new subtypes and 5-10 engineer-weeks; the owner dropped it for M3.

**How to apply:** Don't keep artifacts or plan work just to serve a Deimos M3 drop. The 092 VT branches are on origin and could be revived later. On 2026-10-10, at the owner's go, the post-M3 job/evidence folders and the Deimos VT workspace `_r921` were deleted (D: 66→207 GiB free, E: 49→282 GiB free). The 091-06 uncommitted F1 office-robust WIP survives only as a patch in `D:\IntrusiveAnomalies\_reviews\2026-10-10-disk-cleanup\`. Read that folder's README before looking for 090-12, 091-03/05/06 or 092-03..05 evidence. See [[cold-start-invisible-anomaly-arc]].
