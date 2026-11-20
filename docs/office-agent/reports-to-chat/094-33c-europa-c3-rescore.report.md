# REPORT for 094-33c-europa-c3-rescore.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 094-33c-europa-c3-rescore.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-11-01 05:33:26 | finished: 2026-11-01 06:08:03 | duration: 35 min | exit code: 0
context at last turn: 485467 tokens (49% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\094-33c-europa-c3-rescore.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

**Summary for the owner:**
- On 4.25, frames, labels and masks now line up with what's on screen for all four anomalies. Blinking no longer puts a mask on frames labelled clean.
- One rule question remains: the lighting check would also accept a mask covering only one of the lit objects.
- Capture isn't fully done until chat decides that, plus one question about how the label check was re-run.

**C3 (one session, retry unused).** Codex's numbers match my recompute from the raw frames and masks:

| Anomaly | Labelled frames | IoU | Precision (gate) | Spill | Result |
|---|---:|---|---|---|---|
| blinking | 21 | 1.000 | 0.9391–0.9400 (≥0.90) | 0 | PASS |
| missing_object | 42 | 1.000 | 0.9396–0.9408 (≥0.90) | 0 | PASS |
| lod_corruption | 42 | 1.000 | 0.9786–0.9793 (≥0.80) | 0 | PASS |
| lighting_mismatch | 42 | 0 outside the lit objects | 1.000 (≥0.80) | 0 | PASS |

- **Clean frames:** all 93 frames labelled clean have 0 mask pixels and no mask PNG. This matches the 5.1 rule (`m44`); I checked its citations in the 5.1 source.
- **Auto-exposure lock:** `r.EyeAdaptationQuality 0` on the command line. The 4.25 source citations check out, and every one of the 240 rows records 0.
- **One-frame shift negative:** fails for all four anomalies, as it should.
- **Wrong-target negative:**
  - It fails as it should for blinking (21 of 21 frames), missing_object (42 of 42) and LOD (42 of 42).
  - **For lighting it passes on all 42 frames.** Any single lit object, used alone as the mask, gets precision 1 and spill 0, because every tagged object in this test scene is lit by the one changed light.
  - A synthetic wrong target placed on unchanged pixels is still rejected for all four.
- **C2 (same session):**
  - With the old reference images, the label checker refuses (the exposure lock changed the picture).
  - Codex then picked new reference images from this same session, after it ran. With those, the checker passes: 240 of 240 frames, 0 mismatches, and its shift negatives fail.
  - My own check doesn't use reference images and agrees: 0 mismatches. Clean frames show 0 changed pixels and active frames at least 123,009; all 24 edge-shift negatives fail.

**Safety:** none.
- No crash: exit 0, crash count 0, no new crash folders, and the log ends normally.
- No upload sign: the only upload-like line is the crash client's address printed at startup.
- The Paris host's Content, Config, plugin descriptor and `.uproject` are hash-equal before and after, from my own lists.
- Binaries changed only during the two builds (05:45–05:46).
- `m51` still equals P9. master, release and the 30 tags are unchanged. The feature branch is clean at `03bdef0`, with no push or tags.
- **One gap on my side:** my poll missed the 31-second session (05:48:25–05:48:56). It ran between two of my polls, so its process IDs (editor 35140, crash monitor 37380) come from the kit's own records, not from my observation.

**Decisions needed (chat):**
1. **The lighting gate has no completeness term.** Either add one or redefine lighting's wrong-target test.
   - Without it, any subset of the lit objects passes.
   - For reference, "overlap with all lit objects ≥ 0.90" would separate them on this scene: the real mask scores 1.000, while the best single object scores 0.44 and leaving out the smallest one gives 0.84.
2. **Accept or refuse the post-session re-binding of the C2 reference images.** My independent check passes either way.

Before an office capture smoke on Europa's fork:
- The 4.25 mask path is still tied to the test scene: fixed tags 128–131 per anomaly, the blinking target found by the actor tag `EA_BlinkCube`, and a hard 1280×720 view.
- It needs to take the target from the live anomaly, as 5.1 does, and run through the real firing path (FO1).
- Then comes the read of Europa's fork.

My review is `_reviews\094-33c-code-review.md`; Codex's report is collected as `_reviews\094-33c-codex-report.md`.
api-equivalent cost (not billed on Max): $9.43 | turns: 70 | running total across all briefs: $5,370.62
