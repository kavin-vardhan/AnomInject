# 094-38 — Chat ruling: the LOD parity fix, the sweep, the measured self-check and 5.1's far plane are accepted. The Editor build ran out of attempts (memory, then one compile error, now fixed), so nothing launched. 094-39 rebuilds and runs the smoke, then the push prep

**Date:** 2026-11-02, about 01:15 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Mailbox report 094-38:** NEEDS-DECISION. It ran 22:21–23:52 IST.
- **Codex's report:** `_reviews\094-38-codex-report.md`.
- **Code's review:** `_reviews\094-38-code-review.md`.

## Verdict
**Accepted:**
- **LOD parity:** `lod_corruption` now uses 5.1's default, `WorstLodSentinel` (the coarsest LOD).
- **The sweep:** 104 4.25-only lines are listed and 5 differences are removed. Code's own diff finds nothing missing.
- **The self-check line:** its fields are live reads (viewport type, client class, sizes, frame and backbuffer formats, RHI, hook count, census counts, fixed step and rate, smoothing).
- **The far plane:** 1,000,000 in every case, exactly as in 5.1.
- **The Game build** passed. **The Editor compile error** (5 errors at `AnomalyCaptureDiagnostic425.cpp:117`) is fixed in `4dbcf9b`, which compiled in the Game target.

**Not reached:** the Editor build used FM1's three attempts. Two were memory stops (free commit fell to 2.1 GiB under other work on the PC), and the third was that compile error. So there was no launch, no smoke, no FV6, no scan and no card.

**Open items from Code:**
1. The auto pool on this branch gains `lighting_mismatch` and `lod_corruption` in shared code (11 slots, the two new ones without keys).
2. With the 4.25 multi-LOD filter removed, an auto fire can pick a single-LOD mesh: it is labelled, but nothing changes on screen. 5.1 never auto-fires `lod_corruption`.
3. `pre_ui`'s overlap with the marker rests on timing. `tick=` and `tick_type` can never vary.
4. The 4.25 capture driver's firing schedule (0.8 s interval, 0.4 s hold, at most 1 at a time) isn't in the sweep's list.

## Rulings

### FW1. Accepted: the LOD parity fix, the sweep, the measured fields, the far plane, the Game build and the Editor compile fix

### FW2. The pool extension stays on this branch
- **Why:**
  - `feat/europa-port` is 4.25-only and never merges into the 5.1 line (EY5);
  - the capture enables its four anomalies through the pool.
- **No new keys now.** Log it in gotchas, and list keys for the two slots as a FUTURE debug-UI item.

### FW3. Auto-fired `lod_corruption` picks only meshes with at least 2 LODs
- Use the same guard as `lod_popping`.
- The coarsest-LOD apply stays as 5.1 has it.
- **Why:** 5.1 never auto-fires `lod_corruption`, so there is no 5.1 behaviour to match, and a single-LOD target is guaranteed to show nothing.
- **Log it** as a deliberate addition, under D3's follow-up.

### FW4. The self-check proves `pre_ui` frame by frame, and says which fields are constants
- Record the frames on which the marker was drawn, and show that those exact captures don't contain it.
- Label `tick=` and `tick_type` as build constants in the line. The office card must not treat them as reads.
- `tick_calls` stays as the measured read.

### FW5. The capture driver's firing schedule is accepted as the 4.25 capture design
- The schedule is a 0.8 s interval, a 0.4 s hold and at most one concurrent fire, as accepted in C4.
- List it in the sweep document as a 4.25-only firing choice.

### FW6. 094-39
- **The build:** a fresh FM1 budget (3 attempts per target).
- **The smoke,** as FV5:
  - the street map at a 12 GiB free-commit start line, with the in-session stop at 4 GiB;
  - **the fallback:** if the street map isn't admitted within 45 minutes of the build finishing, or by 03:00 IST, use EA_Core at 8 GiB;
  - two slots.
- **The gates:** FV5's gates, plus FW3's guard (no single-LOD target) and FW4's per-frame `pre_ui` proof. Report each LOD event's visible change, as information.
- **If the smoke passes:** FV6's redaction (a rewrite of the 22 commits that carry `09434-live-capture-receipts.json`, with a backup ref and tree-diff proof). Then Code's pre-push scan and office card.
- **The window for real sessions:** until 07:00 IST on 11-02.

## Also done tonight (chat)
**The D-lite office card's push step is amended.** It now pushes only up to the tested commit:
`push origin 4f46a9c:refs/heads/feat/europa-port`

The old line pushed the whole branch tip, which includes the drive-root listing that FV6 is removing. The office build was already pinned to `4f46a9c`.

## For the owner
- **Nothing is required tonight.**
- **If you push before the capture card is ready,** use the D-lite card's updated push lines. They send only the tested version.
