# REPORT for 094-32-europa-c1-close-c2-labels.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 094-32-europa-c1-close-c2-labels.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-11-01 00:37:08 | finished: 2026-11-01 01:52:27 | duration: 75 min | exit code: 0
context at last turn: 564498 tokens (56% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\094-32-europa-c1-close-c2-labels.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN

**For the owner:** yes, on both counts. The plugin now captures the stock 4.25 game's own 10-bit picture correctly, with no setting changed on the game. In the labels test the labels matched the picture on every one of 480 frames, at every point an anomaly appeared, disappeared or blinked. I recounted all of it myself and got the same answer.

**C1** (60 frames each, 1280×720, format 18, D3D11, 0 dropped, normal exit):
- **PIE on EA_Core: PASS.** Indices 0–59 with none missing, no black frame. It matches the independent Phase A reference (mean error 0.50 against a limit of 8). I opened frames 0, 30 and 59: they show the white-cube fixture.
- **`-game` on EA_Travel: Game01 FAIL (kept), Game02 PASS.** Game02's frames are byte-identical to Game01's, so it passed only because the reference changed.
  - Game01 failed because the checker compared EA_Travel against EA_Core's picture. EA_Core has a white-material override and EA_Travel doesn't; I confirmed that in the map files.
  - Game02 was judged against the engine's own read of the same backbuffer in Game01. That is independent of the plugin's copy and decoder, but not of the scene, because no Phase A picture of EA_Travel exists.
  - Frames 0, 30 and 59 show the same camera and cubes, with grid material.
- **The 10-bit conversion is exact.** I checked it four ways: against 4.25's format mapping, an exhaustive test of all 1,024 values, 200,000 random pixels, and real frames. The engine's own readback truncates, so the plugin's rounded frame should differ from it by 0 or +1 on every channel. It does, in all three sessions, with no other value. No launch carried a backbuffer override.

**C2:** zero mismatches, from the reader and from my own separate pixel classifier, which uses no shared references. Both found the same edges, and the cleanest separation between "anomaly" and "no anomaly" was 133×.

| Anomaly | Normal order: apply / revert / blink toggles | Synthetic order: apply / revert / blink toggles | Mismatches |
|---|---|---|---:|
| blinking | 3 / 3 / 6 | 3 / 3 / 6 | 0 |
| lighting_mismatch | 3 / 3 / 0 | 3 / 3 / 0 | 0 |
| lod_corruption | 3 / 3 / 0 | 3 / 3 / 0 | 0 |
| missing_object | 3 / 3 / 0 | 3 / 3 / 0 | 0 |

- **Pacing-off block:** it ran on frames 240–479.
  - On 4.25, `IAI.Capture.Pace` means fixed step on or off, because wall pacing doesn't exist there yet.
  - The Pace 1 block actually ran 5.1's old "pace-off" regime: fixed steps, free-running, about 5 ms per frame.
  - Both blocks read 0 mismatches.
- **The three negatives:** I ran each one-frame shift against the real files. Apply (24 shifted frames), revert (24) and blink interior (12) each fail as they should, in Codex's reader and in my classifier. My own shifts in both directions, late and early, also all fail.
- **The 12 frames after travelling to EA_Travel:** clean, under a new epoch and world.
- **Scope:** events were driven by a test driver, not the game's real anomaly-firing path, and the label exporter is shaped for this test.

**Safety:** none. No crash, no upload sign. The Paris host's Content, Config, descriptor and `.uproject` match my own before and after hashes. The binaries changed only while the four builds ran, one at a time at below-normal priority with 2 parallel actions, and the 27-file manifest matches disk. `m51` is unchanged (HEAD `53bf725`, index `526aba7f`, the 15 untracked entries). `feat/europa-port` is at `e19fefc`, clean, with nothing pushed and no tags.

**NEEDS-DECISION:** none. Three things for chat before C3:
1. The plugin's green heartbeat text ("[IAI] AnomalyInjector ticking…") is burned into every 4.25 frame, because the 4.25 capture path never suppresses overlays. It isn't a per-frame label leak and every gate's region excludes it, but it would ship in a dataset.
2. The EA_Travel half of C1 rests on the engine's read of the same scene, not on an independent picture.
3. Codex rewrote the 094-31b paragraph in `docs/gotchas.md` instead of appending a new one; that file is meant to be append-only.

The next blocker for C3 is that `AnomalyShaders` is still compiled out on 4.25.

My review is in `_reviews\094-32-code-review.md`, and Codex's report is collected at `_reviews\094-32-codex-report.md`.
api-equivalent cost (not billed on Max): $13.90 | turns: 115 | running total across all briefs: $5,334.98
