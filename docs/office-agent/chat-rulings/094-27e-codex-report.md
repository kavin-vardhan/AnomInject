<!-- collected by Claude Code (mailbox brief 094-27e) from D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\report.md, 2491 B, sha256 3B0F2633CB2ABE2C355F0590179543E94171C1A3D7B819E21C0B3F607E879942, at 2026-10-31T07:55:28.953309+05:30 -->
Phase A is complete under FJ2. Smoke, blinking and LOD passed with exit 0; all four anomalies have at least three counted apply/revert events. No extra anomaly slot was needed.

| Anomaly | Attempted | Applied | Reverted | Counted | Reader / contact sheet |
|---|---:|---:|---:|---:|---|
| blinking | 8 | 6 | 6 | 6 | [PASS](D:/IA_Europa/Receipts/09427e/runtime-attempts/A7_-blinking-positive/01/blinking-contact-sheet.png) |
| lighting_mismatch | 8 | 6 | 6 | 5 | [PASS](D:/IA_Europa/Receipts/09427c/runtime-attempts/A7_-lighting_mismatch-positive/01/lighting_mismatch-contact-sheet.png) |
| lod_corruption | 8 | 6 | 6 | 6 | [PASS](D:/IA_Europa/Receipts/09427e/runtime-attempts/A7_-lod_corruption-positive/01/lod_corruption-contact-sheet.png) |
| missing_object | 8 | 6 | 6 | 6 | [PASS](D:/IA_Europa/Receipts/09427d/rereads/missing_object/missing_object-contact-sheet.png) |

Attempted includes two refusal checks per anomaly. Lighting retains its 094-27c result; missing_object retains the 094-27d re-read. Blinking and LOD are new 094-27e sessions.

[Contact-sheet index](D:/IA_Europa/Receipts/09427e/contact-sheet-index.json) lists all four PNG paths and 69 counted screenshots, each hash-checked at 1280×720 with its AA=0 echo. The new sheets visibly show the right cube disappearing/returning and the central cube changing to a sphere/back.

FK1/FK2 parameter tests passed. The first dry invocation refused before any slot because the shared validator lacked FK1’s exact memory tuple; that narrow amendment passed positive/negative checks, then the required three-slot dry run passed. No in-editor script or native binary changed. [Qualification](D:/IA_Europa/Receipts/09427e/qualification.json).

Crashes, upload signs and protected changes: **none**. All three sessions passed their upload and protection checks. Fast entry/exit checks passed; m51/protected refs and all 27 bound artifacts are unchanged, with no drive-root changes. [Session evidence](D:/IA_Europa/Receipts/09427e/session-summary.json); [exit receipt](D:/IA_Europa/Receipts/09427e/exit.json).

Local commits on `feat/europa-port`: `4276ca2d2a974e5cf6b191f4ea6e1e72a5490b89` (re-arm parameters), `f77d0fcc171425acb6c01b2c7dbc193fe44bcaed` (shared validator), and `abb240bc0431b53e5b21809986da59b534efe7b1` (Phase A closeout). W4 dates are actual. Worktree clean; no push or tags.

VERDICT: GREEN | Phase A done: blinking 6, lighting_mismatch 5, lod_corruption 6, missing_object 6 counted events.
