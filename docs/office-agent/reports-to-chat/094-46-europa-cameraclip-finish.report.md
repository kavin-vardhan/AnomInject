# REPORT for 094-46-europa-cameraclip-finish.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 094-46-europa-cameraclip-finish.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-11-05 07:39:55 | finished: 2026-11-05 09:27:23 | duration: 107 min | exit code: 0
context at last turn: 613928 tokens (61% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\094-46-europa-cameraclip-finish.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

**For the owner:** Camera clipping marks exactly the right frames, but it fails the new "nothing else changed" test by one brightness level on a few hundred pixels. It is not pushed and the other test runs did not happen. Keep using office card `_reviews\094-45r-office-card-europa.md` (it covers the four default anomalies, not camera clipping).

**How it ran:** I resumed thread `01a14334` at 08:01 IST. It exited 0 at 09:19 (4,706 s) and Codex stopped at the first failed gate. Local `feat/europa-port` is now `eea2acd`: `1e65b6f` (counter and combined bench), `afa3ea0` (kit and readers; all three sessions ran on it) and `eea2acd` (docs only). Origin is still `c5d0919`.

**GN4 — the counter: done.**
- `label_transition_camera_clipping_unconfirmed_entries` is now written. It counts the same thing as 5.1 (`d7eee97` `AnomalyLabelWriter.cpp:1350`): one per frame entry with that reason.
- It reads 0 in all three sessions and matches the annotation sums. A nonzero value has not occurred at runtime; Codex's compiled unit test covers it.
- All 56 of 5.1's camera_clipping keys are now emitted (094-45r had 55).

**GN2 — the floor, then D4 on D3D11 1280×720: fails by one level.**
- Codex's combined run keeps the original 2,352-frame default section at the held view, then adds six 48-frame near/far holds (2,640 frames).
- **Floor:** control A vs control B (both at near 10) differ by at most 1 level, with at most 560,155 px at that level on any frame. My reader got the same thresholds. Codex sealed them at 03:22:02.80 UTC, before the positive launched at 03:23:29.80.
- **What passes:**
  - exactly 3 spans (2352–2399, 2448–2495, 2544–2591), and the labels agree in every place they are recorded;
  - every labelled frame changes all 921,600 px;
  - both baseline controls label 0 frames;
  - the near plane is restored to 10 at Stop.
- **What fails:** only 56 of 2,496 unlabelled frames stay within the floor.
  - The rest have up to 254 px differing by 2 levels, in 4,704 scattered positions in the lower 60 % of the frame, with no position failing in even half the frames. The average shift is about 0.
  - The positive shows the same pattern against control B. Two near-10 sessions never differ by 2 levels anywhere.
  - So the extra differences appear only when the near plane changes. The cause is not established.
  - Under 094-45r's old fixed ">2 levels" rule this leg would pass: no unlabelled pixel differs by more than 2.
- D3D12 1280×720 did not run.

**GN3 — the Paris legs: D3D11 passes; the rest did not run.**
- All three D3D11 sessions pass H4, strict texture G1–G3 and G5 by my recompute:
  - 780 masks, texture IoU 1.000, 0 pixels changed outside the masks;
  - 0 of 140 edge frames changed, 0 of 35 reverts with residue;
  - smallest LOD change 68,034 px, `unavailable` 0.
- **Not run:**
  - the D3D12 1280×720 and 1279×719 combined legs;
  - the default-only run compared against `c5d0919`'s outputs;
  - the card's read commands.
- An info-only key diff (a baseline control against Stage 1's output) shows no key removed and no type changed.

**D5 holds:** the defaults are unchanged (blinking, lod_corruption, missing_texture, corrupted_texture), and camera_clipping never fires from the auto pool.

**Push and card:** not pushed, because a gate failed and GN3 is incomplete. I wrote no pre-push scan and left the card as 094-45r. My review is `_reviews\094-46-code-review.md`; Codex's report is `_reviews\094-46-codex-report.md`.

**Safety:** none of the stop conditions fired.
- No crash and no upload sign (three upload checks pass).
- The Paris host's own Content, Config, descriptor and `.uproject` are hash-equal before and after, and so is plugin Content.
- Both generations of the material `.uasset`s are unchanged, and so is the fixture.
- m51 is unchanged (HEAD `53bf725`, index `526aba7f`, 15 untracked). The only ref change is local feat; the spikes stay local.
- Binaries changed only during the two builds. Nothing was deleted and the drive roots are unchanged.
- C: changed by under 0.004 GiB per session (23.8 GiB free). Admission gates cleared in seconds, nowhere near the 3 h bound.

**Decisions chat must make:**
1. **How D4's "unlabelled frames match" should treat a 2-level difference that appears only when the near plane changes.**
   - (a) Keep GN2 as written. Camera clipping cannot then pass on this bench.
   - (b) Also seal a near-plane floor first: a held near-100 session with no near holds (so nothing can be clipped), paired against a near-10 control. Judge against the larger of the two floors. This needs one more guarded bench switch and two more sessions per RHI.
   - (c) A fixed tolerance such as "no unlabelled pixel above 2 levels". This leg meets it with 0 failing frames.
2. **Once that's ruled:** the remaining work is D4 on D3D12, the two D3D12 combined legs, the default-only run and the card check. Codex planned only one default-only run (D3D12 1279×719), reading GN3's "a default run" as singular. Does one run satisfy GN3, or should there be one per leg?
api-equivalent cost (not billed on Max): $17.92 | turns: 123 | running total across all briefs: $5,785.90
