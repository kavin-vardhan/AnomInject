# REPORT for 093-08-j2-admission-requal.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 093-08-j2-admission-requal.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-10-14 21:56:28 | finished: 2026-10-14 23:20:11 | duration: 84 min | exit code: 0
context at last turn: 560538 tokens (56% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\093-08-j2-admission-requal.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

**2. Plain summary**
- Codex built the "don't fire at an object that's off screen" check. Then it stopped on its own, before running any test, over a small labelling detail my review had flagged.
- So nothing new is proven: no in-engine tests ran, nothing was cooked or pushed, and everything is back as it was.
- Chat needs to decide two small questions (section 9) before Codex can resume the tests.

**3. Part R (review of the 093-07 projection fix): 8 OK · 1 GAP**
- OK:
  - No FOV/aspect maths is left in the targeting or label path.
  - Geometry is the engine's own (`GetProjectionData` refreshed at world-tick end, then the serving view's unjittered matrices carried with the request and mapped through the copied rect).
  - The near-clip projector change is sound.
  - `f9b9328` compiles to a fallback on UE 5.1, whose camera struct has no axis-override member (checked in the engine source), and uses nothing from 5.7.
  - 15 product files, no new dependency or private header.
  - The `projection` block is in `run_summary.json` only.
- GAP (minor): `fovDeg` now reads the camera-cache FOV instead of `GetFOVAngle()`, so a host-locked FOV (`LockedFOV`) is no longer reported. It affects metadata only; boxes and visibility are unaffected.
- Nothing invalidated Codex's legs, so I did not stop the job.

**4. The Codex job**
- Resumed the same thread, 1 try, 4,493 s (22:00–23:15), exit 0, `VERDICT: NEEDS-DECISION`.
- Codex stopped on my R8 by its own reading of the scope rule. Its report says Code did not order the stop.
- Heads pushed: none. The remote stays `1bf7054`; local is `1071ca1`.
- New local commits: `7f4329c` (design and predictions), `41c1a0c` (product), `1071ca1` (docs).
- Unqualified archive `…J2-admission-09308-41c1a0c-5F75ECA8-AD40DA99-UNQUALIFIED`. I rehashed its key binaries; they match.
- Harness fixes, each on the first attempt:
  - Lyra helper rebuilt with `Switches` verified (L4 item 1).
  - The camera reader now counts pending pre-switch drops (L4 item 2, synthetic proof only).
  - Two encoding fixes in tooling.
- Not done: the 0.813 replay (L4 item 3) and the healthy PASS line (L4 item 4).

**5. The admission in 3 lines**
- Paths: capture targeted fire, auto-pool, selector, console `IAI.Apply`, dashboard `inject` and public C++ all converge on `ApplyAnomaly`. I verified that with `git grep`; globals and internal sub-effects are excluded by design.
- The check: for object anomalies, before any change to the world, it builds the engine-projection frustum of the unique local player (frustum only). Any matched actor off screen refuses the whole call; no unique projection also refuses.
- Counters `targeted_refused_off_screen` / `targeted_refused_no_projection` sit at the `run_summary.json` root, reset at start. There are no per-leg values: no leg ran.

**6. Main review: 17 OK · 3 OPEN (unrun because of the stop) · 1 GAP**
- GAP: after the product change, Codex made `w=r` binding for resize and camera-switch positives. That goes beyond the admission re-freeze (PART C item 1). Combined with K6 (drops per switch are a reading), those legs would fail by construction.
- Verified OK:
  - Design committed before the code.
  - Scope is 3 files plus the public `ConvexVolume.h`.
  - All 8 builds pass at `41c1a0c`.
  - PF-1 can-fail: the checker rejects 093-07 (4 fires, recounted by me) and 093-06 (96 false rows).
  - Staging (5,764 files), both `Content` trees, host binaries, retained evidence and archives are unchanged against my own baselines.
  - No kill command was issued; G482 is unique.
- Disclosure (mine): the three commits are dated 2001-01-01. This mailbox session's environment sets `GIT_AUTHOR_DATE`/`GIT_COMMITTER_DATE`, and my launch passed them to Codex. The commits are unpushed; the next launch should clear both variables.

**7. Reader lines** (run by me, Python 3.10.11 and 3.13.1, identical)
- Healthy PASS: not run (no new session).
- Null-mask FAIL: `I1.11 FAIL r=360 w=360 v=0 e=-1 x=0 m=96 c=-1 b=-1 #9 I-fail=360,360,385,0,0,3,0,0,0,0,0,0,0,0,0,0`
- StackOBot `f608d56`: `B1 n=96 b=999 #0`
- Lyra `f608d56`: `B1 n=96 b=967 #4`

**8. Ready for office checkpoint I?** No. J2 is unqualified: 0 of 84 cases and no PF gate ran on any candidate, and there is no healthy PASS line. The owner needs nothing yet. When it is ready, the office needs the qualified build, cases 11–14 on the second office host, and Python 3.10+ with numpy and Pillow.

**9. NEEDS-DECISION**
1. **R8:** either authorise restoring `fovDeg` to `GetFOVAngle()` (a one-line `AnomalyViewport.cpp` change plus a locked-FOV check) inside the next continuation, or rule that the camera-cache FOV is acceptable. Then Codex restarts K7 from PF-1.
2. **M8:** say whether L5's `w=r` floor covers the resize case and the camera-switch legs, or whether their drops stay readings.
3. **Process:** mid-job Code findings in `_reviews` are read by Codex. Decide whether to mark them non-binding or write them elsewhere.

Full review: `D:\IntrusiveAnomalies\_reviews\093-08-code-review.md`; receipts in the run folder under `code-receipts-093-08\`.

Unrelated: several MCP connectors (Google Calendar, GitHub, Slack and others) need authorising in claude.ai settings or via `/mcp`; `meshy` and the PDF viewer failed to connect.
api-equivalent cost (not billed on Max): $13.85 | turns: 92 | running total across all briefs: $4,524.77
