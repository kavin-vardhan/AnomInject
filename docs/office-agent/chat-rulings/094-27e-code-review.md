# 094-27e — Claude Code short review (FJ1 lean profile), written after Codex exited

**Date:** 2026-10-31, 07:55–08:10 IST · **Author:** Claude Code (mailbox brief 094-27e) · **Input:** Codex's report `_reviews\094-27e-codex-report.md` (body 2,491 B, `3B0F2633…`, byte-identical to `report.md`). Thread `01a14334` resumed 07:37:15 → 07:55:06 IST (1,071 s, 1 try, exit 0, ledger row `…-resume33`). **Codex's verdict: `GREEN | Phase A done: blinking 6, lighting_mismatch 5, lod_corruption 6, missing_object 6 counted events`.** Commits on `feat/europa-port`: `4276ca2` (re-arm), `f77d0fc` (shared validator admits FK1's exact tuple, job 09427e only), `abb240b` (docs closeout). Author date = committer date = real time; no `origin/feat/europa-port`; 30 tags, none at HEAD; worktree clean.

**My receipts:** `…\runs\2026-10-16-094-01-europa-phase-a-plan\code-receipts-094-27e\`. **My scratch:** `D:\IA_Europa\Receipts\CodeReview09427e\` (`review27e.py` → `review27e.json`; `thumbs\`).

## 1. Per anomaly (my count from the raw probe rows, compared with the reader)
| Anomaly | Session | Attempted / applied / reverted / refused (mine) | Complete events, visible and gone after (mine) | Counted (reader) | Agree |
|---|---|---|---|---|---|
| blinking | 094-27e, new | 8 / 6 / 6 / 2 | 6 (15 of 30 during frames changed, max 14.1 % px ≥8; after 0 px ≥8) | 6 | Yes |
| lod_corruption | 094-27e, new | 8 / 6 / 6 / 2 | 6 (38.9 % px ≥8 during; after 0 px ≥8) | 6 | Yes |
| missing_object | 094-27c kept, 094-27d re-read | 8 / 6 / 6 / 2 | 6 (14.1 % px ≥8 during; after 0) | 6 | Yes |
| lighting_mismatch | 094-27c kept | 8 / 6 / 6 / 2 | 6 (89.6 % px ≥8 during; after 0) | 5 (one event invalidated on coverage under the pre-FJ1 rule) | Yes |

Clean after-frames differ from before only by the tonemapper's grain (mean ≈0.27 level, 0 px ≥8). The gate cleared Smoke on one reading (phys 4.77 GiB, commit 10.4 GiB), blinking on the second (one reading below 4 GiB), and LOD after three idle-blocked readings.

## 2. The pictures
- **blinking** (`09427e\…\A7_-blinking-positive\01\blinking-contact-sheet.png`): the right cube is gone in the middle column on all 6 rows and back on the right.
- **lod_corruption** (`09427e\…\A7_-lod_corruption-positive\01\lod_corruption-contact-sheet.png`): the centre cube is a sphere in the middle column on all 6 rows and a cube again on the right. LOD model `1` worked.
- **lighting_mismatch** (`09427c\…\lighting_mismatch-contact-sheet.png`): the scene goes dark and the cubes turn grey with hard shadows in the middle; the right column matches the left on all 5 rows. Its labels carry no "AA=0" text, but the index and the probe rows do.
- **missing_object** (`09427d\rereads\missing_object\missing_object-contact-sheet.png`): the left cube and its shadow are gone in the middle on all 6 rows, and back on the right.
- **All 69 counted screenshots** (18 + 18 + 15 + 18) exist, match their indexed SHA-256, and are 1280×720 PNGs. Every one has AA 0, both in the index and in my own read of the probe (an `r.PostProcessAAQuality 0` prepare row comes before every screenshot in all four legs). The four sheet hashes match the index.

## 3. Safety
- **Crash:** none. All three new sessions (Smoke, blinking, LOD) exited 0 (`PROCESS_PASS`) with no new crash-report folders. There are 0 fatal or assertion lines in the six session logs. Nothing new is in the host's `Saved\Crashes` (latest entry 10-27).
- **Upload sign:** none. The only matches in the logs are the crash client printing its configured data-router URL at startup (it then reports `MTBF/NoTempCrash` and exits without sending) and the command line itself, which carries `bSendUnattendedBugReports=False` and `bSendUsageData=False`. The kit's own upload check is `PASS` on every log.
- **Paris host:** Content (1,456 files), plugin Content (5), Config (18), the descriptor and the `.uproject` are hash- and mtime-equal between my lists from 07:34 and 07:56 (`paris-diff.json`, `PARIS_UNCHANGED=True`), with 0 reparse points.
- **`m51`** at the end: HEAD `53bf725`, index `526aba7f`, 0 tracked changes, the same 15 untracked entries. `master` is `b5f15a3`, `release/2026-10-05` is `d7eee97`, and there are 30 tags.
- **Tools diff** (`cda5cdd..abb240b`): only the job ids, the window end (07:00 IST 11-01), FK1's 4 GiB physical start line and FK2's 120 s idle moved. The commit lines (8 GiB start, 4 GiB stop) and the drive lines are unchanged. The validator amendment admits only job `09427e` with FK1's exact tuple. Codex disclosed that its first dry invocation refused.
- **Leftovers:** no UE-class or relay process remains. No foreign UE editor was seen during the job.

## 4. Phase A
**Phase A is done (FJ2):** A3 Load passed earlier, and all four A7 anomalies have at least 3 counted events (6 / 6 / 6 / 5), each visibly applying and reverting on its sheet.
