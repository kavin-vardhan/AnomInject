# REPORT for 094-39-europa-port-finish-take3.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 094-39-europa-port-finish-take3.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-11-02 01:05:04 | finished: 2026-11-02 02:44:38 | duration: 100 min | exit code: 0
context at last turn: 572199 tokens (57% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\094-39-europa-port-finish-take3.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

**For the owner:** The 4.25 port is not ready for the office capture test yet. Both targets build, and almost every smoke check passes. But one anomaly, missing_object, never fired in either of the two allowed sessions, and only the street map's admission cutoff is past; the session window is still open until 07:00. The cause is a test-driver issue, not a crash or a data problem. Nothing was pushed, and the push prep, scan and office card are waiting on a passing smoke.

**FW3, FW4, FW5 and the build**
- **FW3 (2-LOD guard):** correct, and it ran both ways. Slot 01 rejected the only candidates (`0 of 2 candidates eligible - single_lod 2`); slot 02 accepted the two-LOD mesh 12 times, each `forced LOD 2 of 2`. The coarsest-LOD rule from 5.1 is unchanged.
- **FW4 (per-frame pre-UI proof):** correct.
  - The marker is drawn only on the first 8 capture requests (frames 1712–1719 in slot 02).
  - Each post-UI control reads 8 of 8 marker cells. The capture of the same request and frame reads 1 of 8, which is only the black cell on a black backdrop.
  - None of the 2,352 captures contains the marker.
  - `tick` and `tick_type` now carry `build_constant`; `tick_calls` is a measured read.
- **FW5:** the schedule is listed in the sweep document. It omits the per-hold restart, which is what broke this smoke.
- **Build:** Editor attempts 1 and 2 stopped on memory while a foreign UE 5.7 cook was running. Editor attempt 3 and Game attempt 1 passed with 0 errors. The rebuilt Editor DLLs contain the new code, which includes `4dbcf9b`. The Editor budget is now spent.

**Self-check line (slot 02, frame 300), verbatim:** `stage=300 frames=300 size=1280x720 format=18 RHI=D3D11 PostRenderViewFamily=315 viewport=SceneViewport client=GameViewportClient viewport_size=1280x720 backbuffer_format=18 slate=1 pie=0 pre_ui=1 ui_draws=8 ui_controls=8 ui_positive=8 ui_checked=300 ui_hits=0 ui_paired=8 tick=OnWorldPostActorTick425 tick_source=build_constant tick_type=2 tick_type_source=build_constant tick_calls=1664 fixed_step=1 fixed_dt=0.033333333 fixed_rate=0 fixed_fps=30.000000 smoothing=0 census=on batches=0 lost=0 no_pass=0 polluted=0`

**The map:** EA_Core. The street map never reached 12 GiB free commit before its 02:10 IST cutoff (45 minutes after the builds finished).

**The smoke (slot 02), every gate by my own recount of the raw files; my numbers equal Codex's**

| Gate | Result |
|---|---|
| Each anomaly fires 3+ times | **FAIL:** blinking 30, lighting 6, LOD 12, missing_object 0. Slot 01 fired lighting only (12). |
| Masks | PASS: all 414 affected frames have the target in the mask; the 1,938 clean frames have no mask file and 0 mask pixels. |
| `annotation.json` | PASS: schema 2.1, frame sets agree on all 48 events, far plane 1,000,000; coverage is 42 measured plus 6 lighting events marked unmeasurable (−1). |
| Census | PASS: peak 3 drawn, nothing exhausted, lost, no-pass or polluted. The street-only checks (over 55 drawn, wall strip) are waived for EA_Core. |
| Custom depth | PASS: all 14 components restored; the two actors with their own custom depth were never touched. |
| View | PASS: 2,352 of 2,352 frames use the player's view. |
| Self-check and FW4 | PASS |
| FW3 | PASS |
| Crash, upload | none |

- **Masks I opened:** blinking covers exactly the hidden cube. LOD covers the centre cube. Lighting covers the floor and two cubes but misses a third cube that also darkens; that cube has its own custom depth, so it is never tagged by design.
- **LOD visible change:** 0 target pixels changed in all 12 events. At this distance the engine already draws the coarsest LOD, so forcing it changes nothing.

**FV6, scan and card:** none of them ran, because each needs a passing smoke.
- No commit was rewritten and there is no backup ref.
- The drive-root listing is still in the history, unchanged; 25 commits now carry it.
- The three new commits add no new listing.

**Safety:** none.
- Both sessions exited normally.
- The only crash-client logs from tonight belong to the foreign UE 5.7 build.
- The Paris Content, Config, descriptor and `.uproject` are hash-equal by my own hashes.
- Binaries changed only while our builds ran.
- `m51` equals P9 at start and exit. The feature branch is clean at `0621027`, with no push and no tags.
- Only two things are gone: a build temp file and an empty engine temp folder.

**Decisions for chat**
1. **The missing_object failure.** 5.1's injector reseeds every time it is started, and the 4.25 test driver restarts it for every hold. On EA_Core's fixed view, the six holds replay the same eight picks, and missing_object is never one of them. Retrying needs one of these, plus fresh slots, a fresh Editor budget and the remaining window:
   - a continuous scored hold;
   - no reseed between holds;
   - the street map at its 12 GiB line.
2. **The 0.5 % selection setting.** The retry used it on EA_Core only. The shipped value, 6 %, is the same as 5.1, and at 6 % EA_Core's targets are never picked. Chat should decide which value the office card uses.
3. **The LOD guard isn't enough.** Requiring 2 or more LODs doesn't guarantee a visible change when the coarsest LOD is already on screen. Possible fix: also require that the target LOD differs from the one currently drawn.
4. **Lighting masks** leave out receivers that have their own custom depth (G498).

Reports are in `D:\IntrusiveAnomalies\_reviews\`:
- 094-39-code-review.md
- 094-39-codex-report.md
api-equivalent cost (not billed on Max): $12.54 | turns: 87 | running total across all briefs: $5,470.70
