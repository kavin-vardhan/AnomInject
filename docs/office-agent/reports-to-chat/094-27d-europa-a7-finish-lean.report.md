# REPORT for 094-27d-europa-a7-finish-lean.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 094-27d-europa-a7-finish-lean.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-10-31 02:17:51 | finished: 2026-10-31 07:05:41 | duration: 288 min | exit code: 0
context at last turn: 489600 tokens (49% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\094-27d-europa-a7-finish-lean.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

**Plain summary for the owner**
1. Two of the four test anomalies now work on 4.25 with pictures: lighting_mismatch (5 events) and missing_object (6 events). Codex fixed missing_object by correcting a picture check against last night's kept pictures, so no new Unreal session was needed for it.
2. Codex also fixed lod_corruption and blinking, but no Unreal session ran overnight, so neither fix has been tried. The PC never had the 6 GB of free memory the test kit waits for (it stayed between 3 and 5 GB until 07:00).
3. Nothing is running now, and nothing you own was touched.

**Per anomaly** (my own count from the probe rows agrees with the reader every time)
- **lighting_mismatch:** 5 counted events, kept from 094-27c (no re-read, no new session). Applied 6, reverted 6, plus 2 refusal checks. The pictures of all 6 events are complete; the reader counted 5 because 094-27c's older coverage rule dropped one.
- **missing_object:** 6 counted events, from a re-read of 094-27c's kept evidence with the corrected checker (no new session). Applied 6, reverted 6.
- **lod_corruption:** 0 counted. The re-read still fails, and its new session never started. Applied 6, reverted 6, but nothing visibly changes in the pictures.
- **blinking:** 0 counted. The kept leg had stopped before its first event, and its new session never started.

**The owner's eye check**
- **Index:** `D:\IA_Europa\Receipts\09427d\contact-sheet-index.json`
- **lighting_mismatch (counted):** `D:\IA_Europa\Receipts\09427c\runtime-attempts\A7_-lighting_mismatch-positive\01\lighting_mismatch-contact-sheet.png`. The middle column should be darker with visible shading, and the right column should match the left.
- **missing_object (counted):** `D:\IA_Europa\Receipts\09427d\rereads\missing_object\missing_object-contact-sheet.png`. The left cube and its shadow should be gone in the middle column and back in the right.
- **lod_corruption (diagnostic, not counted):** `D:\IA_Europa\Receipts\09427c\diagnostic-contact-sheets\lod_corruption-diagnostic-contact-sheet.png`. You should see no difference between the columns; that is the defect Codex diagnosed.
- **blinking (diagnostic, not counted):** `D:\IA_Europa\Receipts\09427c\diagnostic-contact-sheets\blinking-diagnostic-contact-sheet.png`. The right cube should be gone in both the middle and the right column, because this control skips the restore.

**Fixes and diagnoses** (commits `9360c18` fixes and `cda5cdd` docs on `feat/europa-port`; no push, no tags; I agree with all three diagnoses)
- **LOD:** the driver forced LOD model `2`, which is the cube the camera already shows (4.25 `StaticMeshComponent.h:174`: model N forces LOD N−1). It now forces model `1`, the sphere, which should be a visible change. Not yet tried in a real session.
- **missing_object:** the "noise" between clean frames is 4.25's tonemapper grain, ±1 level per frame (`PostProcessTonemap.usf:594-600`), not exposure drift. The check now accepts noise only if it stays within 1 level with a near-zero average. The thresholds for a visible change and a clean restore are unchanged, and a 2-level brightness mutant still fails. This changed a check after seeing its result; Codex disclosed it and the 4.25 source backs it.
- **blinking:** the kit's monitor no longer reads the probe file while the game runs. It closes the window 2 s after the last picture and reads the file once after exit, with retries. The probe's silent self-disable on a failed write is still in the C++ and needs a build.
- **Tests:**
  - The Python 2.7 harness passed 4/4 when I ran it.
  - Codex's Python 3 suite passed its first 6 groups, then failed one case only because it reads the live arming file, which Codex re-armed after running the tests.
  - I re-ran that case and the remaining one against the kept 094-27c arming, and they pass. The keyword-Rotator check passes, and `tools\europa` has 0 comment lines.

**Safety:** crash: none. Upload: none (no session logs exist). Protected change: none (my before and after hashes of the Paris Content, Config, descriptor and `.uproject` are equal; `m51` matches P9 at start, before launch and at the end).

**NEEDS-DECISION**
1. blinking and lod_corruption each still need one real session, with Smoke first; about 15 minutes in total once the gate clears. The blocker is the kit's 6 GiB free physical-memory start line. What held the memory (owner-side, recorded only): `vmmem` 4.0 GB, a foreign `node.exe server.js` that grew from 2.0 to 3.8 GB overnight, `wlanext` 3.0 GB, and the Epic launcher 2.3 GB. Either the owner frees memory before the next window, or chat lowers the line.
2. The LOD fix (model `1`) is unproven until that session runs.

Full review: `_reviews\094-27d-code-review.md`; Codex's report: `_reviews\094-27d-codex-report.md`.

Separately: Google Calendar and the plugin connectors need authorizing in the claude.ai connector settings or via `/mcp`, and the PDF-viewer server failed to connect. This job didn't use any of them.
api-equivalent cost (not billed on Max): $12.33 | turns: 108 | running total across all briefs: $5,278.53
