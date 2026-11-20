# REPORT for 084-06-sync-bench.md
verdict: RED (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 084-06-sync-bench.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: high
started: 2026-09-29 01:54:17 | finished: 2026-09-29 02:57:30 | duration: 63 min | exit code: 0
context at last turn: 448077 tokens (45% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\084-06-sync-bench.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: RED

**Plain summary for the owner**
- **In sync with the picture on both edges:** blinking, missing_object and corrupted_texture, under anti-aliasing and with it off. missing_texture is also in sync, apart from one borderline event in the long auto-pool run.
- **Out of sync:** stuck_low_mip, the m52 fix. With anti-aliasing off, the label starts 2 frames before the blur is even half visible, on all 16 events. The end of each event is exact.
- **Out of sync:** camera_clipping on MainWorld labels all 200 frames positive, but nothing in the picture is clipped.
- **Could not judge:** lod_popping (its effect is too faint to measure under anti-aliasing), and the scripted camera-clipping test. That test never started because the container we ran on is older than the bench module it needs.
- All 31 legs ran on the first try, and the m53 build is back in place and verified.

**Per-anomaly verdicts** (release rule: 0 frames off at the 50 % threshold, outside transition frames)

| anomaly | class | basis |
|---|---|---|
| blinking | SYNC-PROVEN | B-REAL: 20 events with anti-aliasing plus 10 without, every edge 0/0. MASK55: 32/32 |
| missing_object | SYNC-PROVEN | B-REAL: 20 with anti-aliasing plus 10 without. MASK55: 20/20 |
| corrupted_texture | SYNC-PROVEN | B-REAL 20/20, needing no transition flag. MASK55: 28 pass, 1 censored |
| missing_texture | SYNC-PROVEN on the targeted legs, **one borderline FAIL** | B-REAL 20/20. MASK55 35/36; the failing event is +1 at onset, with its threshold at 97 % of its depth |
| stuck_low_mip (m52) | **DESYNC** | Anti-aliasing off: onset +2 on 16/16, offset 0 |
| lod_popping | UNJUDGEABLE | 0 measurable events under anti-aliasing. Anti-aliasing off: 4 events pass 0/0, too few to prove |
| camera_clipping | **DESYNC (over-label)** on MainWorld; sync UNJUDGEABLE | MainWorld 200/200 labelled, no pixel evidence. B-CC could not run |

**Detail per leg set**

*Step 0 (the one harness edit).*
- **The edit:** `084-06-legs.py` gains 2 lines: `TRANS_FIX=['IAI.Label.TransitionOnFrames 3']`, appended to every non-null, non-Lyra leg. That's 24 legs carrying it and 7 nulls without it. The file before the edit is saved at `084-06-evidence\084-06-legs.py.pre-084-06-edit`.
- **Dry-run preflight re-run on the edited file:** exit 0, 0 problems, 31/31 launches accepted. The first dry attempt failed only because its temp folder was on C: while the bank is on E:; I re-ran it with the folder on E:.
- **Read-back from the build's own log:** `temporal=1; transition frames on=3 off=8 hide=1`. On the anti-aliasing-off legs the build zeroes all three values, so the cvar has no effect there.

*m52 (stuck_low_mip).*
- **B9 (anti-aliasing off): FAIL 16/16.**
  - On every event, the first two labelled frames show only 9–22 % of the blur. The full blur arrives 2 frames later, and it clears exactly on the first unlabelled frame.
  - The 086-01 checker agrees: FAIL 16.
  - I re-ran the evaluator on the 084-03 bank's B9_FIX. It has the same two-step shape on the same frames, but there the two early frames showed ~70–77 % of the blur, which passes the 50 % rule. So the shape is unchanged and only the level of those two frames dropped.
  - The cause is not established. Nothing in 084-05a's changes touches mip timing.
- **B0 (anti-aliasing on):** 4 pass, 12 censored. Onset +1/+2 and offset +4/+5, all inside the 3/8 flags.
- **B3:** 3 pass, 5 censored.
- **Can-fail levers:** B0L (timing) FAILs and P1 fails purity, as required. Purity passes on B0, B3, B9 and B0L.
- **The null never did its job.** B5 wrote 0 events, because with no frame held there is nothing to label. So every m52 leg fell back to its own detrend, and the null-divided threshold the harness was built around never ran (G356). The censored offsets come from that fallback.

*MASK55 (1200-frame auto-pool, four anomaly types).*
- **Mask gate PASS:** 119 events; 586/586 labelled frames masked; 64 ID recycles, matching 64 log lines; 55 values reused; 0 exhausted; 0 aliasing frames; 0 overlapping holders.
- **Peak live IDs was 1.** No two events ever held IDs at the same time, so Codex's F4 aliasing path was not exercised, only not seen.
- **Sync:** 115 pass, 1 fail, 1 censored. The failure is the missing_texture event on StaticMeshActor_85 (labelled frames 183–188, first visible 184). Its reference frames sit in a 4-frame gap after the previous event.

*B-CC (the scripted camera-schedule legs): INVALID on all 7 legs.*
- **Why:** the FIX container `67EA1FE0` was cooked on 2026-09-04. I extracted its cooked plugin descriptor with UnrealPak: it lists no `AnomalyBench`, which was only added on 2026-09-21. The m53 container's descriptor does list it.
- **Effect:** in a packaged build a module the descriptor doesn't list never starts, so `IAI.Bench.InputLock` and `IAI.Bench.CameraSchedule` never existed. The logs contain 0 of the module's lines, not even a refusal, because unknown commands passed at launch are dropped silently (G354).
- **What the legs did show:** at the static starting pose, both the new rule and the old proxy label 0 of 200 frames. Cost is 78 µs mean.

*B-CC-MW (camera_clipping on MainWorld).*
- 200/200 frames are labelled, and all 200 have the eye inside a primitive's box.
- The ON-vs-null pixel difference is a median of 10,763 px, below the null-vs-null floor of 24,178, so nothing visible was clipped. No unlabelled frame is above the floor.
- The old sphere proxy labels 0 of the 200.
- First hits: a StaticMeshActor (…1853259872, 180 frames) and a landscape proxy (20). Which mesh is hollow isn't established.
- Cost: 202 µs mean, 753 µs max, 830 primitives walked.

*B-REAL (two MainWorld rocks, four types).*
- All 10 legs PASS at 0/0, 10 judged events each (the other 4 are warm-up or camera-moved).
- Transition flags on the hide types: blinking 2 per event (28 per leg), missing_object 1 per event (14). The texture swaps and all anti-aliasing-off legs carry 0.
- The 086-01 checker shows 0 failures on any B-REAL leg.

*B-LOD.* Under anti-aliasing, both tick orders: 0 of 7 events measurable. With it off: 2 pass at 0/0 and 5 not measurable, in each order.

*Rows marked `anomaly_present` with no event labelling them* (reported only; this is Codex's F1 shape): MASK55 128 (blinking), REAL_BL legs 56 each, LOD legs 80 each (lod_popping), B0L 345 (the lever leg), every other leg 0.

**Pacing, waits, invalid attempts**
- 31 legs, 31 accepted on attempt 1, 0 invalid, 0 not run. Legs ran 02:02:35–02:37:28.
- The speed ratio was 1.0000–1.0004 on the short legs and 1.025–1.048 on the 1200-frame m52 legs (28.6–29.3 fps).
- Before most legs there was a ~45–50 s idle wait. It started at about 10 s idle each time, so it was the launcher's own focus reset, not a person.
- The only other process running was `UnrealTraceServer`; I left it alone.

**Staging and restore**
- **Staging:** m53 set `2FCDF059/20DA6F98/534C5863/FD766B7B/462B8AC6/BB05CF99` swapped to FIX (`E9FF019A` plus `67EA1FE0/2CEFB8F4/E03C6610/C70ECDAA/A16A18A8`), hash-verified by receipt.
- **Restore failure:** the automatic restore at the end hit a locked file (WinError 32) on its first copy, 3 s after the last game process exited. The harness only catches its own error type, so the run exited 1 with nothing replaced and no evaluation done (G355).
- **Recovery:**
  - I re-ran the harness's own restore function, and it verified on the first retry.
  - `window.py gates` then did the evaluation.
  - `window.py postflight` returned PASS: 0 problems, 31/31 legs, 16,540 hardlinks verified, m53 staged.
  - I checked the six m53 hashes again afterwards, and they match.
- **Untouched:** `m51` `53bf725`, `master` `b5f15a3`, `ToCodex`, `E:\<personal-folder>`, the m53 S2 files, Lyra. No builds, no source edits. Nothing was killed. My dry-run folder on E: is removed.

**Commits pushed**
- `b40a7f4` on `fix/m52-label-timing` (`b3ba294..b40a7f4`, local equals origin): the journal `docs/sessions/2026-09-29-084-06-sync-bench.md`, the status block, and gotchas G354–G356. No tag, no merge.

**NEEDS-DECISION**
1. **m52 onset at anti-aliasing off.** The two-frame partial onset (9–22 % of the blur) fails the 50 % rule; on the 084-03 build those frames were ~70 %. Two things to rule:
   - is this a desync to fix, or a partial-visibility transition that should be flagged with anti-aliasing off too;
   - should finding out why those frames dropped from ~70 % to ~10–20 % be part of 084-07?
2. **camera_clipping over-labels MainWorld 200/200.** This is the hollow-mesh case 086-02 predicted. It needs an enclosing-box rule, or a fallback to the old proxy, before delivery.
3. **B-CC cannot run on `E9FF019A`+`67EA1FE0`.** It needs a cook of the fix branch (so its descriptor includes `AnomalyBench`) or a ruling on another container.
4. **Harness fixes for the next bench:** the restore must catch all errors and retry (G355), and the null must calibrate from its sharpness series rather than from events this build no longer writes (G356).
5. **The borderline missing_texture event in MASK55:** does an event whose threshold is 97 % of its depth count against SYNC-PROVEN?

Several connectors (GitHub, Slack and others) need authorising in their connector settings, and the PDF viewer failed to connect. None were needed here.
api-equivalent cost (not billed on Max): $10.87 | turns: 85 | running total across all briefs: $3,239.81
