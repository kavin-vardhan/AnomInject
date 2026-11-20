# REPORT for 094-35-europa-c5-real-scene-scale.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 094-35-europa-c5-real-scene-scale.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-11-01 09:12:42 | finished: 2026-11-01 10:21:56 | duration: 69 min | exit code: 0
context at last turn: 543109 tokens (54% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\094-35-europa-c5-real-scene-scale.live.txt

---- FINAL MESSAGE (Claude Code) ----
**VERDICT: NEEDS-DECISION.** C5 produced no scored events, so none of the anomaly gates (C2, C3, the negatives, a late-entering target) could be measured. The census, custom-depth, view and safety checks all pass on my own numbers.

**For the owner:** not yet. The change builds, and the plugin ran a full real scene with a turning camera on 4.25 without crashing, dropping frames or disturbing the game. But the clean warm-up pass rejected every camera pose, so no anomalies were fired or checked. It also showed that our own object tagging probably makes part of the picture flicker; that has to be fixed before the office smoke.

**C5** (one street-map session, 09:37–09:45 IST; 3,996 frames, 0 dropped, exit 0)
- **Per anomaly:** 0 events for blinking, lighting, LOD and missing_object. Nothing fired; no row carries an anomaly or a mask, so there was no mask to open.
- **Why every pose was dropped:** the drops follow the frozen 5% rule. My recomputed unstable shares equal Codex's exactly (26.2–41.5%). My own analysis splits the cause three ways:
  1. **Tonemapper grain:** it repeats every 7 frames and spans 2 levels, so the frozen "more than 1 level" tolerance counts the grain itself as instability.
  2. **Real scene motion on 5 of the 6 poses:** wind on foliage, a moving fog layer and the sky. Even with the grain removed, 8–13% of pixels change by more than 8 levels.
  3. **A flicker at the wall pose that tracks our census:** 3.84% of the frame (the bottom 34 rows) alternates every frame. It switches in lockstep with which of two census-tagged actors (the street or the wall) wins custom depth there.
     - With the grain and this flicker removed, that pose has 0.0% of pixels moving more than 8 levels, so it would have been kept.
     - The likely cause: on 4.25 the census un-tags and re-tags every tagged actor on every captured frame, event targets included. 5.1 holds each batch until its readback and keeps event targets tagged. This is not proven; the same pose with the census off is the control.
- **FR3:** holds. The references and unstable maps were frozen at 09:39:48.12, before the first scored frame was written (09:39:48.52).
- **Census:**
  - 168, 125 and 116 distinct actors were drawn at three held poses, which clears the "more than 55 visible" bar.
  - The 0.5% floor and 25% ceiling are applied; the ceiling correctly excluded a wall covering 93% of the frame.
  - The ledger was never exhausted (0 on all 3,996 rows).
  - No late-entering target could be shown, because no event fired.
- **Custom depth:**
  - All 1,980 components are identical before and after.
  - All 3,996 mid-session samples show custom depth on for exactly the tagged targets plus the ones already on.
  - The reserved value 203 was never handed out.
- **View:** on all 3,996 frames the captured view equals the player's own view (position, rotation and FOV differences all 0).
- **Annotations:** the schema matches 5.1's key for key and in order. Two caveats:
  - `coverage_ratio` is written as a frame ratio, where 5.1 writes a screen-area share.
  - With no events, agreement between the affected/injected sets and the per-frame rows is still unproven on real data.

**Safety:** none.
- No crash and no upload sign.
- The Paris host's Content (including the street map), Config, descriptor and `.uproject` are hash-equal by my own hashes.
- `m51` equals P9 at every check.
- The feature branch is clean at `2143215`, not pushed, no tags.
- Binaries changed only while builds were running.

**NEEDS-DECISION items:**
1. **Pre-pass protocol on a real map.** As frozen, the rule cannot keep any pose here. Options: compare each frame with a reference at the same point in the grain's 7-frame cycle, or loosen the tolerance, and pick poses or regions without wind and fog motion (or rule on how animated pixels are treated). Codex rightly did not use the retry, because the fix means changing the frozen rule.
2. **4.25 census parity.** Make it hold tags the way 5.1 does (each batch until its readback; event targets for the whole event). Then measure the wall pose with the census on and off before rerunning C5.
3. **Minor:** `coverage_ratio` should carry 5.1's meaning.

Files are in `D:\IntrusiveAnomalies\_reviews`:
- `094-35-code-review.md`
- `094-35-codex-report.md`

Also: several MCP connectors (Google Calendar and the plugin connectors) need authorization in claude.ai or `/mcp`. They weren't needed here.
api-equivalent cost (not billed on Max): $13.05 | turns: 86 | running total across all briefs: $5,396.44
