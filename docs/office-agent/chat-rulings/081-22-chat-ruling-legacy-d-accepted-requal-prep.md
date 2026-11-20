# 081-22 — chat ruling: build 3 runtime accepted; (d) native-order difference classified as timing-sampled; requalification prepared bench-free while the PC is busy (chat Claude, orchestrator, 2026-09-24 07:45 IST)

## Disposition
- **Part 1 accepted.** (a) 1080p smoke PASS (20/20, zero budget refusals, oracle exit 0, high-water identical to build 2); (b) F1 cancel-before-focus PASS (no folder, nothing written, `STAGE-DISCARDED … issued=0`); (c) F2 **exercised for real** with `DebugCreatePlayer 1` — legacy output equal to pre-m55 on every compared field (incl. `SVE-WANT-SUMMARY`), every m55 row `unsupported_delivery`, `change_multi_view_families` 120, pending arms flat; (d) synthetic order PASS, native order FAIL on the declared gate. The five-minute quiet window was used perfectly (14 launches in 4.5 min). AMENDMENT 1 (a comparator hole found and tightened **before** any leg) — accepted and credited.
- **(d) native order — ruling: accept as timing-sampled, not an m55 output defect.**
  - Everything deterministic is identical: every mask byte, labels, the whole 081-12 field set on all 6 pairs, key presence.
  - The differing values are the ones the code samples at drain time, not from the frame's own state: `coverage_pct` is read from the **live** view when the drain processes the anchor frame (Code's reading of the source), and the pre-m55 binary **already differs from itself** on events 3 and 5; `end_frame`, `capture_game_ticks`, `key_ring_*` and `ticks_per_captured_frame` vary **within one binary** in the 081-12 bank.
  - The event-2 shift (8.6927 pre-m55 vs 8.6518 build 3, ≤ 0.47 % relative) is systematic but tiny, both values are samples of the wrong view, and build 3 native equals synthetic order on **both** binaries. Neither number is "the" right one.
  - **Classification:** `coverage_pct` and those six counters join the declared **timing-sampled** set for legacy comparisons (documented in the field reference with this reason). **Queued (m51 pairing / M3, not m55):** compute `coverage_pct` from the frame's own mask instead of the live view — exact and frame-consistent, but a legacy-output change that belongs outside m55.
- **Foreign editors:** <other-home-project> ran until ~07:10 and a new one started 07:18:28. Part 2 (long-run ON/OFF) was correctly not started.

## Next
1. **No bench work until the owner decides how the PC is shared** — it is morning; the owner will be using the PC.
2. **081-23 (bench-free, Code, high): requalification campaign prep** — the full build-3 leg inventory scripted, chunked into bench windows, predictions committed, comparators and the oracle wired, dry-run on banked data. So that any agreed bench window is spent running, not preparing.
3. When the PC is scheduled: Part 2 long-run readings first (decides whether a build 4 scheduling change is needed), then the requalification chunks.
