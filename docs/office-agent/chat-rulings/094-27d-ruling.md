# 094-27d — Chat ruling: 2 of 4 smoke anomalies proven on 4.25 (lighting_mismatch, missing_object); blinking and LOD are fixed, but no session could start overnight for memory. 094-27e runs those two now

**Date:** 2026-10-31, about 07:35 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Mailbox report 094-27d:** NEEDS-DECISION. It ran 02:17–07:05 IST.
- **Code's short review:** `_reviews\094-27d-code-review.md`.
- **Codex's report:** `_reviews\094-27d-codex-report.md`.

## Verdict
**Counted on 4.25, with pictures:**
- **lighting_mismatch:** 5 events (094-27c);
- **missing_object:** 6 events, from a re-read of 094-27c's kept evidence (FH4).
  - **The cause:** the "noise" was 4.25's tonemapper grain, ±1 level per frame (`PostProcessTonemap.usf:594-600`).
  - **The check now** accepts noise only within 1 level with a near-zero average. The change and restore thresholds are unchanged, and a 2-level mutant still fails.

**Fixed, not yet tried in a session:**
- **lod_corruption:** the driver forced model `2`, which is the cube the camera already shows (model N forces LOD N−1, `StaticMeshComponent.h:174`). It now forces model `1`, the sphere.
- **blinking:** the monitor no longer reads the probe file during the session. It reads it once after exit, with retries.

**Commits:** `9360c18` (fixes) and `cda5cdd` (docs).

**Safety:** no crash, no upload, no protected change; `m51` matches P9.

**Why nothing ran:**
- free physical memory stayed at 3–5 GiB all night, against FD3's 6 GiB start line, and commit fell below 8 GiB by 04:27;
- **the holders,** recorded only:
  - `vmmem` 4.0 GB;
  - the Mailbox dashboard's `node.exe server.js`, which grew from 2.0 to 3.8 GB overnight (the known leak);
  - `wlanext` 3.0 GB;
  - the Epic launcher 2.3 GB;
- none of these is ours to touch.

## Rulings

### FK1. Physical memory at the start: 4 GiB (amends FD3)
- **Why:** free physical memory only affects paging speed. The safety comes from the commit lines, which stay as they are: 8 GiB to start, and a stop below 4 GiB in session.

### FK2. Idle at the start: 2 minutes for 094-27e (amends FJ1's 5)
- **Why:** this job is three sessions of a few minutes each, and the owner is told to leave the PC alone for them.

### FK3. 094-27e: smoke, blinking and lod_corruption, now, under the lean profile
- **Re-arm** for job 09427e with three fresh single-use slots: smoke, blinking positive, lod_corruption positive. 09427d and older armings are refused.
- **No new code** unless a session shows a new defect. Test only what changes.
- **Contact sheets for all four anomalies,** with one index: lighting's and missing_object's as counted, plus the new ones.
- **The window:** until 07:00 IST on 11-01.
- **GREEN means Phase A is done (FJ2):** all four anomalies have at least 3 counted events.

### FK4. Deferred (FUTURE)
- **The probe's silent self-disable on a failed write.** That needs a C++ build. It belongs with the probe's GC fix, after D-lite.

## For the owner
- **Quit the Mailbox dashboard from its tray icon.** It has grown to about 3.8 GB.
- **Then leave the PC alone for about 30 minutes.**
- **The eye check:** two contact sheets, listed in chat's message.
