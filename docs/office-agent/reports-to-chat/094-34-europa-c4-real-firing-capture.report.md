# REPORT for 094-34-europa-c4-real-firing-capture.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 094-34-europa-c4-real-firing-capture.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-11-01 07:03:07 | finished: 2026-11-01 08:22:29 | duration: 79 min | exit code: 0
context at last turn: 556580 tokens (56% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\094-34-europa-c4-real-firing-capture.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN

**For the owner:** Capture now runs through the real automatic firing path on 4.25. Frames, labels and masks lined up on every frame at both 1280×720 and 1600×900 for all four anomalies, with no crash, no upload and no project change. It is not yet ready for whatever is on screen at the office, though. It labels only objects visible when capture starts, it stops if more than 55 objects are visible, and its bench command forces a fixed scene camera.

**FR2, from the kept 094-33c files:** the real lighting mask covers 1.000 of the lit objects on all 42 frames, with 0 pixels outside them. The best single object covers 0.4401 and everything but the smallest 0.8406, so both fail the 0.95 line on 42 of 42 frames. These match Codex's numbers.

**Static check:** my own scan of the final commit `47a6972` (168 Source and Shader files) finds no fixture names, no fixed mask tags and no hard capture resolution. What remains is unrelated: a 10-bit midpoint check, a "128 MiB" help string, a reason code, and 5.1's 1920×1080 fallbacks in anomalies outside the smoke set. Codex's three 5.1 citations match the source (live fires, the stencil ledger, the census provider).

**C4:** two sessions, 1,440 frames each, 0 dropped, auto-exposure locked from frame 1. I recomputed everything from the raw files.

| Anomaly | Events (720 / 900) | C2 mismatches | IoU | Precision 720 | Precision 900 | Spill | Completeness |
|---|---|---|---|---|---|---|---|
| blinking | 19 / 19 | 0 | 1.000 | 0.9388–1.000 | 0.9374–1.000 | 0 | — |
| lighting_mismatch | 11 / 11 | 0 | 1.000 | 1.000 | 1.000 | 0 | 1.000 |
| lod_corruption | 16 / 16 | 0 | 1.000 | 0.9786–0.9794 | 0.9798–0.9803 | 0 | — |
| missing_object | 12 / 12 | 0 | 1.000 | 0.9388–0.9789 | 0.9370–0.9857 | 0 | — |

- **C2 (my own label-free check):**
  - All 819 clean frames show 0 changed pixels against frame 0, so no revert leaves anything behind; changed frames show at least 123,118 pixels.
  - The 154 pixel edges match the label edges exactly, including 38 blink toggles inside events.
  - All 116 edge-shift tests fail, as they should.
  - My first attempt used the wrong cut-off and reported 478 mismatches. That was my classifier's error, not the capture's.
- **Masks:** each mask image equals the GPU evidence on 621 of 621 frames. All 819 clean frames have no mask, and every mask carries only its own event's tags. One mask per anomaly, opened and looked at: each covers exactly the right object (`c4-mask-sheet-720.png`).
- **Negatives:** a mask shifted by one frame is wrong on 154 frames each way. Another object's silhouette never passes, and neither does a single lit object or all-but-the-smallest for lighting.
- **Same seed (4242):** both accepted sessions fired the identical 58 (anomaly, target) events. The stopped 1600×900 attempt matches the first 46.
  - The pair differs in resolution and in one writer-only binary change, so it is not a byte-identical re-run.
- **Retries:** both retry slots were used.
  - The first 1280×720 run chose an invisible pawn for blinking; the fix only admits targets with drawn pixels.
  - The first 1600×900 run was stopped by the kit at its memory floor; the fix caps pending writes at four.
- **FR3:** the reference rule was committed at 07:20, before the first session at 07:24, and never changed. The one later reader change makes the wrong-target test stricter, not looser.

**Safety:** none.
- No crash in any of the four sessions; the one stop was the kit's controlled stop. No upload sign.
- The Paris Content, Config, descriptor and `.uproject` are unchanged by my own hashes, and so are the drive roots.
- Binaries changed only while a build was running, and the 27-file manifest matches disk.
- `m51` equals P9 at start and at exit; master, release and the 30 tags are unchanged. Nothing pushed, no tags.
- A Codex Windows sandbox service is still running. It is not the relay process, and I left it alone.

**NEEDS-DECISION:** none. These are the next steps before an office smoke on Europa's fork:
1. **Scene scale:** to handle an office scene, the 4.25 path needs 5.1's rotating census (or per-event tagging) with its coverage floor and ceiling. Right now it:
   - tags every visible object once, at the start, for the whole session;
   - fails outright beyond the 55 free stencil values;
   - can never target an object that comes into view later;
   - leaves custom depth on for all tagged objects, a risk on games that read custom depth.
2. **Capture command and labels:**
   - `IAI.Capture.C4` sets the view to the scene's first camera actor rather than the player's own view;
   - the 4.25 `annotation.json` lacks 5.1's affected and injected frame sets.

Files are in `D:\IntrusiveAnomalies\_reviews` (`094-34-codex-report.md` is Codex's report):
- 094-34-code-review.md

My computed results are in `D:\IA_Europa\Receipts\CodeReview09434\`; my scripts are in `D:\IA_Europa\Temp\CodeReview09434\`; the launch and poll receipts are in `code-receipts-094-34\`.
api-equivalent cost (not billed on Max): $12.77 | turns: 103 | running total across all briefs: $5,383.39
