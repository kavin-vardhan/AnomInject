# 094-37 — Chat ruling: the finishing fixes and the cleanup are accepted. The smoke ran out of memory before any gate. Code found a real 4.25 parity bug (LOD forces the finest LOD instead of 5.1's coarsest). 094-38 fixes it, makes the self-check measure what it claims, re-runs the smoke with a memory line that fits, and prepares the push

**Date:** 2026-11-01, about 22:35 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Mailbox report 094-37:** NEEDS-DECISION. It ran 17:37–22:00 IST.
- **Codex's report:** `_reviews\094-37-codex-report.md`.
- **Code's review:** `_reviews\094-37-code-review.md`.

## Verdict
**Accepted:**
- **The cleanup (FU4):**
  - in scope: 3,008 files and 9.01 GB, matched one-to-one against Code's before-and-after listing, with nothing removed outside the scope;
  - the keep-list is verified intact.
  - The 124 large files from the old jobs' own `P9\entry|exit` folders fit the intent (old records of finished jobs). The top-level `P9` is untouched.
- **The 5.1 lost-batch timeout, no-pass and polluted discard:** they match `AnomalyCensus.cpp:629-699`, unit-tested.
- **The far plane** for the infinite case.
- **The builds:** both first try, with 0 errors and 0 warnings.

**Not reached:** every FU5 gate. Both smoke slots stopped on the 4 GiB commit floor during the first hold, after 0 and 24 frames.
- The street-map session needs about 4.4 GiB on its own, so admitting it at 8 GiB lands it at the floor.
- Tonight there was foreign load as well: another session's UE 5.7 cook, DaVinci Resolve, wlanext at 5.9 GB and vmmem at 4.1 GB.
- In slot 1, D: also fell by 16 GB in the same 2.4 s as commit collapsed. That fits the page file growing under memory pressure.

**Found:**
1. **A 4.25-only parity bug.** `AnomalyAutoInjectorSubsystem.cpp:630-637` forces LOD0, the finest LOD, where 5.1 defaults to the coarsest (`WorstLodSentinel`, `Anomaly_LodCorruption.cpp:29-32`).
   - On nearby props LOD0 is already showing, so the "anomaly" changes nothing.
   - **This explains 094-36's invisible LOD swaps:** D3 is at least partly a port bug, not only a design issue.
   - It looks like a leftover from the Phase A bench driver.
2. **The self-check line prints fixed text for the tick mode and the viewport type,** and `pre_ui=1` only means the hook fired. Fields like these would tell the office card nothing about Europa's fork.
3. **`docs/europa/09434-live-capture-receipts.json` (commit `47a6972`) lists 244 folder names from the C:, D: and E: drive roots,** including two named after the owner. It isn't secret, but it is personal, and the branch has never been pushed.
4. **5.1 writes 1,000,000 for the far plane unconditionally;** the port keeps a finite far plane as it is.

## Rulings

### FV1. Accepted: the cleanup, the timeout and discard, the infinite far plane, and the builds

### FV2. Fix the LOD parity bug, and sweep the auto-injector for any others
- `lod_corruption` uses 5.1's default selection exactly (cite it).
- **The sweep:** diff the 4.25 auto-injection path against 5.1, and list every 4.25-only line. Each line must either be justified as a 4.25 API difference or removed.
- *Why:* one bench leftover already reached the real firing path.
- **In the smoke,** report each LOD event's visible change. This is information only, not a gate.

### FV3. The self-check line measures every field it prints
- **The viewport:** the runtime class or type of the game viewport and its client.
- **The tick mode:** read from engine state. This covers the fixed time step, the fixed frame rate, smoothing and the actual hook used.
- **`pre_ui`:** measured with a pixel test. Draw a small marker on the UI or canvas layer for a few frames, and report `pre_ui=1` only if no captured frame contains it.
- **Keep:** the format, RHI, size, hook count, census state, and the lost, no-pass and polluted counts.
- *Why:* the office card reads this line to learn how Europa's fork renders.

### FV4. Match 5.1's far plane exactly, finite case included

### FV5. The smoke's memory line (my call)
**Street-map sessions:**
- start at 12 GiB of free commit and 6 GiB of physical memory;
- the in-session stop stays at 4 GiB. *Why:* it protects the owner's other programs.

**Fallback:** if the street map can't be admitted by 03:00 IST, run the smoke on EA_Core instead, at the 8 GiB start line.
- That drops only the census-over-55 and wall-strip gates, which 094-36 already proved.

**The rest:**
- two new slots: one, plus a retry;
- **the window for real sessions:** until 07:00 IST on 11-02.

**The gates are FU5's, plus:**
- FV3's measured self-check fields;
- 5.1's far plane;
- a valid `coverage_pct`.

### FV6. Before the push (only if the smoke passes)
**Redact the drive-root listing from the unpushed history:**
- In every commit since `4f46a9c` that carries a drive-root listing, replace the names with counts and a hash.
- Keep a local-only backup ref.
- Verify that each rewritten commit's tree differs only in the redacted files.
- Add a check for drive-root listings to the pre-push scan.
- *Why:* personal folder names don't belong on GitHub. The branch has never been pushed, so rewriting it is safe. The owner can say "leave it", and the step is skipped.

**Then Code:**
- the pre-push scan of the final branch since `4f46a9c`;
- the office capture-smoke card, pinned to the final commit, with FV3's measured self-check fields among its reads.

## For the owner
- **Nothing is required.**
- **Optional, if you're done for the night:**
  - Closing DaVinci Resolve gives the test enough memory.
  - So does letting the other Claude session's Unreal job finish.
  - Please don't restart while the job runs, because a restart would kill it. If memory stays short, the job falls back to the lighter test map after 03:00.
- **The good news:** the invisible LOD bug on the street map was partly our port's mistake, and it's now being fixed.
