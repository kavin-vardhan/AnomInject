# 094-27d — Claude Code short review (FJ1 lean profile), written after Codex exited

**Date:** 2026-10-31, 07:03–07:25 IST · **Author:** Claude Code (mailbox brief 094-27d) · **Inputs:** Codex's report `_reviews\094-27d-codex-report.md` (body 4,316 B, `CF450954…`), thread `01a14334`, resumed 02:21:42 → 07:02:57 IST (16,875 s, 1 try, exit 0, ledger row `…-resume32`). **Codex's verdict: `NEEDS-DECISION | lighting 5 and missing_object 6 qualified; blinking and LOD remain unproven after the launch window`.** Commits on `feat/europa-port`: `9360c18` (fixes, 02:33) and `cda5cdd` (docs closeout, 07:02); author date = committer date = real time; no `origin/feat/europa-port`, tag count unchanged (30), none at HEAD.

**My receipts:** `…\runs\2026-10-16-094-01-europa-phase-a-plan\code-receipts-094-27d\` (gates, `paris-lists-{before,after}.json`, `paris-diff.json`, `preserved\` = 094-27c's 45 report outputs hash-equal, `midjob-notes.txt`, `memory-sightings.txt`). Owned scratch: `D:\IA_Europa\Receipts\CodeReview09427d\` (`count-*.json`, `sheets.json`, `rest-cases.json`, `copy-tools-receipt.txt`) and `D:\IA_Europa\Temp\CodeReview09427d\` (`scripts\`, `tools-copy\`, `py27-run1\`, `unit-run1\`).

## 1. Per anomaly (my count from the raw probe rows vs the reader)
| Anomaly | Source | Attempted / applied / reverted / refused (mine) | Complete events, mine | Counted (reader) | Agree |
|---|---|---|---|---|---|
| lighting_mismatch | 094-27c kept PASS (no re-read, no new session) | 8 / 6 / 6 / 2 | 6 (ROI change ~81.6 levels during, ~0.25 after, 0 px ≥8) | 5 (one event was invalidated on coverage under the pre-FJ1 rule) | Yes |
| missing_object | **FH4 re-read** of 094-27c's kept leg with the corrected reader (no new session) | 8 / 6 / 6 / 2 | 6 (~33.7 during, ~0.26 after, 0 px ≥8) | 6 | Yes |
| lod_corruption | re-read of kept leg, still FAIL; **no new session** (gate never cleared) | 8 / 6 / 6 / 2 | 6 complete, but change only ~0.31 levels during (= dither) | 0 | Yes |
| blinking | kept 094-27c stopped leg; **no new session** | 0 / 0 / 0 / 0 | 0 | 0 | Yes |

No new runtime session ran: the Smoke gate took 450 readings from 02:36 to 07:00 IST and every one was blocked on physical memory (3.15–5.32 GiB free against the 6 GiB start line; commit blocked 4 times). The Smoke slot was prepared but nothing launched; there are 0 session logs under `Receipts\09427d\runtime-attempts`.

## 2. The pictures
- **lighting_mismatch** (`09427c\…\A7_-lighting_mismatch-positive\01\lighting_mismatch-contact-sheet.png`, counted): visible during (whole scene dimmed, shading appears), gone after (matches before) on all 5 rows.
- **missing_object** (`09427d\rereads\missing_object\missing_object-contact-sheet.png`, counted): the left cube and its shadow are gone during each of 6 events and back after; the sheet carries `AA=0` on every label.
- **lod_corruption** (`09427c\diagnostic-contact-sheets\…`, NOT COUNTED): no visible change in any column; the centre target is already a cube before, during and after.
- **blinking** (`09427c\diagnostic-contact-sheets\…`, NOT COUNTED, SkipRestore control): the right cube vanishes during and stays gone after (restore skipped by design), so it shows the hide, not a revert.
- **Counted pictures:** 15 (lighting, events 0–4) + 18 (missing_object, events 0–5); every one exists, matches its indexed SHA-256, is a 1280×720 PNG, and has AA echo 0 (every screenshot in both legs is preceded by a `prepare` row with `r.PostProcessAAQuality` 0).

## 3. Codex's three diagnoses
- **LOD — agree.** The kept during-frame shows the cube; 4.25 `StaticMeshComponent.h:174` forces LOD N−1 for `ForcedLodModel` N, and the fixture's LOD1 is the cube (`author_425_fixture.py:73-75`), so `2` forced what auto-LOD already showed. The fix (`1`, the sphere) is unproven in a real session.
- **missing_object noise — agree.** The kept clean pairs differ by at most ±1 level with a near-zero signed mean; 4.25 `PostProcessTonemap.usf:594-600` adds a per-frame grain of ±½ level at 1/256 for 8-bit output (seeded by frame index, `PostProcessTonemap.cpp:494-499`). It is dither, not exposure drift, so no exposure override was needed.
- **blinking — agree.** The kit stopped on `PermissionError` reading the probe's exclusive append file. Codex's fix removes every live probe read during A7 sessions, so neither side of 094-27c's G1 can collide. The probe's silent self-disable on a failed write (C++ `:107-109`) is still in place, untouched (it needs a build). Another reader such as an AV scan could still trigger it.

## 4. The fixes (diff `2e2766f..cda5cdd`, 13 tool files + docs) and their tests
- **Diff:** `gate_readers.clean_noise` accepts clean noise above 0.25 only when every channel differs by ≤1 level and |signed mean| ≤ 0.01. The active (`≥ max(4, 4·noise)`, ≥1 % px ≥8) and restore (`≤ max(0.5, 2·noise)`, ≤0.1 % px ≥8) checks are unchanged. The readers make observation coverage record-only (FJ1). The monitor never reads the live probe for A7; it closes 2 s after the last `-5-after-1.png` and reads the probe once after exit with bounded sharing retries. Other changes: the lean gate (6/8 GiB start, 4 GiB commit stop, C13/D25/E15 start, C10/D15/E10 floors, idle ≥300 s), the scope narrowed to Smoke + blinking + LOD positive, LOD arg `1`, and `begin_step` guards in the editor driver. The post-result change to the noise check is disclosed, bounded and backed by 4.25 source.
- **My test runs** (from a copy in my scratch):
  - The **2.7.14 harness passes 4/4**: Smoke, a 5-tick multi-tick Load, single-shot failure, and the unguarded mutant failing as expected.
  - **Python 3 `runtime_fj_tests.py`: the first 6 groups pass**, including the two-level drift mutant and active-as-restore both failing, sharing retry/defer, no live probe open, gate boundaries, and the LOD arg.
  - The **"spent arming" group then fails**, only because it reads the live `Launch\Europa425-Runtime.json`, which became the 09427d arming at 02:34 after Codex's 02:32 run. Re-run against the kept `superseded-09427c-runtime.json` it is refused; missing_object, lighting and NoApply slugs are refused; the keyword-Rotator check passes on the 5 executed scripts; `tools\europa` has 0 comment lines.
  - **Observation:** that test is not re-runnable after arming. It is not a code defect.

## 5. Safety, from the raw files
- **Crash:** none (no session ran). **Upload sign:** none (no session logs exist).
- **Paris Content (1,456), plugin Content (5), Config (18), descriptor (1) and `.uproject`:** my before (02:20) and after (07:03) lists are hash- and mtime-equal, with 0 reparse points.
- **`m51`:** HEAD `53bf725`, index `526aba7f`, 0 tracked, 15 untracked at start, before launch and after.
- **Processes:** drive roots were unchanged at every poll; no UE-class process was seen at any time; no Codex process remains.

## 6. Next blocker for Phase A
- **Memory, not code.** The kit needs 6 GiB of free physical memory to start a session, and the machine sat at 3–5 GiB all night.
- **What holds it** (owner-side, recorded only): `vmmem` 4.0 GB, a foreign `node.exe server.js` growing from 2.0 to 3.8 GB private between 03:00 and 05:25, `wlanext` 3.0 GB and the Epic launcher 2.3 GB.
- **What remains:** blinking and LOD each need one real session (Smoke first), about 15 minutes in total once the gate clears.
