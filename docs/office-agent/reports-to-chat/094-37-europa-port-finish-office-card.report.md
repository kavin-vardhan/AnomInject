# REPORT for 094-37-europa-port-finish-office-card.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 094-37-europa-port-finish-office-card.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-11-01 17:37:09 | finished: 2026-11-01 22:00:24 | duration: 263 min | exit code: 0
context at last turn: 729294 tokens (73% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\094-37-europa-port-finish-office-card.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

**For the owner:**
- The 4.25 port is not yet ready for the office capture test. All the finishing fixes are built and tested, but the street-map smoke never got past its first few seconds: both allowed sessions were stopped by the PC running out of memory.
- The memory went to Europa's own game session plus other programs running at the same time: another Claude session's UE 5.7 project, DaVinci Resolve, wlanext and WSL.
- The cleanup freed 8.4 GiB on D: (23.6 → 32.0 GiB).

**Cleanup (FU4): in scope.** I listed all of `D:\IA_Europa` before and after.
- **What was removed:** exactly the 3,008 files (9.01 GB) in `removed.tsv`, all inside the 74 scope folders, with matching sizes and times. Nothing outside the scope and no directories were removed.
- **What was kept, verified intact:** top-level P9, 09434–09436 and their CodeReview folders, FH2's probe files, 09427e's contact sheets, 09427b's crash analysis, and every small summary file.
- **Minor notes:**
  - 9 tiny `.bin` stand-ins (905 bytes) fall outside FU4's named categories.
  - 124 large files came out of P9 sub-folders inside the old job folders. I read "P9" as the top-level `Receipts\P9`, which is untouched.
  - Codex kept every referenced folder whole, so 32.9 of the 41.3 GiB in scope remains (for example `09403\ledger`, 6.5 GB).

**The diff against 5.1:**
- **Lost-batch timeout:** matches 5.1 (`AnomalyCensus.cpp:629-699`): lost after more than 8 ticks, plus the no-pass and polluted discards. Its test is a unit test of the decision function plus source-text checks; nothing drops a real readback in the engine.
- **Far plane:** an infinite far plane is now written as 1,000,000, as in 5.1. Unlike 5.1, a finite far plane keeps its real value.
- **Self-check line:** the tick-mode field and the `/FSceneViewport` part are fixed text, not measured, and `pre_ui=1` only means the capture hook fired.
- **Parity read:** blinking, lighting and missing_object match 5.1. Codex's read missed one 4.25-only line in `AnomalyAutoInjectorSubsystem.cpp:630-637`: for `lod_corruption` it forces the finest LOD (LOD0), while 5.1's default is the coarsest LOD. On nearby street props LOD0 is already showing, so the swap changes nothing. That fits 094-36's invisible-LOD result, so that dataset item (D3) is at least partly a 4.25 difference.
- **Builds:** both targets built first try (0 C++ errors and 0 warnings), the binary manifest matches, and the launch-timing fix to the kit is sound.

**The smoke (FU5), checked from the raw files: every gate unreached.**
- Slot 1 stopped after 11 s with 0 frames (free memory 9.6 → 1.0 GiB).
- Slot 2 (the retry) stopped after 54 s with 24 frames (8.7 → 3.7 GiB), still in the first hold. No anomaly fired, no mask exists, and the labels and `annotation.json` were never written.
- The only real reading is the start self-check line, which is sane: `format=18 RHI=D3D11 size=1280x720 PostRenderViewFamily=4 pre_ui=1 fixed=1 census=bench_off lost=0 no_pass=0 polluted=0`. The frame-300 line was never reached.

**Pre-push scan and office card:** not written, since both depend on a passing smoke. I trial-ran the scan on `cedb505` and found no secrets, no binaries and no new codename problems. One new item for the owner: `docs/europa/09434-live-capture-receipts.json` (commit `47a6972`) contains 244 folder names from the C:, D: and E: drive roots, including two folders named after the owner. It isn't secret, but it shouldn't be pushed without the owner's OK.

**Safety: none.**
- No crash: both stops were the kit's own deliberate memory stops, with no crash folders.
- No upload sign and no map or config save.
- The Paris Content, Config, plugin descriptor and `.uproject` are unchanged by my own hashes, and binaries changed only during the builds (19:03–19:05).
- `m51` equals P9 at start and exit. The feature branch is clean at `07c853e`, not pushed, no tags.

**Decisions needed:**
1. **Memory rules for the smoke.** The street-map session needs about 4.4 GiB of memory itself (094-36 went from 12.2 to 7.8 GiB free). Starting it at 8 GiB leaves almost nothing above the 4 GiB stop. Options: a higher start requirement (about 12 GiB), a lower stop, or running only when the other workloads are idle. Either way, new session slots, because both are spent. The window ends at 07:00 IST.
2. **Whether to change the 4.25 `lod_corruption` choice** to 5.1's coarsest LOD (or a LOD different from the one currently showing) before the next smoke.
3. **Whether the self-check line's fixed-text fields** (tick mode, viewport type, `pre_ui`) are acceptable for the office card's reads.
4. **Whether to push the drive-root listing,** or remove it from the receipts JSON first.

My review is in `_reviews\094-37-code-review.md`, and Codex's report is collected as `_reviews\094-37-codex-report.md`.
api-equivalent cost (not billed on Max): $28.08 | turns: 193 | running total across all briefs: $5,444.16
