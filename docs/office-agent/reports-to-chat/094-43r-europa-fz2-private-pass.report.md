# REPORT for 094-43r-europa-fz2-private-pass.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 094-43r-europa-fz2-private-pass.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-11-04 02:23:49 | finished: 2026-11-04 04:54:48 | duration: 151 min | exit code: 0
context at last turn: 713923 tokens (71% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\094-43r-europa-fz2-private-pass.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN

**For the owner:**
- The outline leak is fixed on the test level: with the game's outline effect on, 0 new outline pixels in all 2,352 saved frames, and the labels are identical to the old method.
- The Paris host still passes on D3D11 and on D3D12, including the odd 1279×719 size.
- It is pushed (`93613ae`), and there's a new office card: `_reviews\094-43-office-card-europa.md`.

**GE1** (`d2f481d`)
- `owner_ref_policy.py` keeps all 71 baseline refs exact and admits only the four named extras (backup, origin feat, the two spikes); any other ref fails.
- Its self-test passes 8 of 8 on my run, including the four required failing mutants.
- Full P9 `main()` passes: Codex's at entry took 354 s, and mine, unmodified at the final tip, took 560 s. Both saw 382,970 files with 0 differences and 22,628 files (100.7 GB) rehashed equal.
- One doc slip: the gotcha number G524 was reused. Codex added G527 to correct it; the next free number is G528.

**Stage A** (spike `42ea37e`, my own readers)
- **GD3 rule:** visible IDs need private depth equal to scene depth within 4/(2^24−1); hidden selected targets need it at or in front. The tolerance was committed at 02:41:16, before any launch. Across all frames, 0 pixels break the rule.
- **A-P3:** IoU 1.0 with identical pixel counts on every object. Masked: 84 frames, 394,800 px. WPO: 1,637 frames. All 132 hidden-blink frames are non-empty. 78 lighting frames are tagged as a group on one side and per object on the other; this is reported under GD4, and their combined coverage is identical.
- **A-P4:** 0 new magenta. The same check finds magenta on 474 of 588 frames of 094-42c's legacy run, so it can fail.
- **A-P2:** 0 `Missing cached shader map` lines.
- **A-P5:** 0 tagged components, and `r.CustomDepth` stays 1.

**Stage B and H1–H5** (`ea08258`/`59c1282`, recorded in `93613ae`)
- **Custom-depth writes:** every one left sits behind `Legacy()`, which needs `-IAIBench`, the exact fixture map and `-IAICaptureLegacyReference`. The bench plant is removed, and only public 4.25 headers are used.
- **Without the private pass,** capture outside the bench refuses outright, which is stricter than FZ3.
- **Builds:** one editor build failed (a shadowed `Views` variable); the retry passed, with 0 link errors overall.

| Gate | Result (my readers) |
|---|---|
| H1 | 0 new magenta on 2,352 frames |
| H2 | IoU 1.0 everywhere against a fresh legacy reference; masked 168 frames, WPO 1,658; 222 hidden-blink frames non-empty |
| H3 | 0 polluted rows, 0 tagged, depth state equal before and after |
| H4 | D3D11 1280×720, D3D12 1280×720 and D3D12 1279×719 all pass: 2,352 frames, 0 dropped; 69 events (blink 22, lighting 13, LOD 19, missing 15); 731 masks; every LOD event changes pixels (min 14,428); format 18 (10-bit); 0 unavailable (5 static meshes) |
| H5 | GPU 0.157–0.159 ms per captured frame on all three runs; render-thread submit 0.16 ms on D3D11 but 1.60–1.69 ms on D3D12 |

**Push and card**
- **Scan:** clean (`_reviews\094-43r-prepush-scan.md`). One carried, non-secret item: 8 new build-warning lines repeat the owner's Windows profile name, which is already public on origin.
- **Push:** `feat/europa-port` only, `d1cdc7b..93613ae`, fast-forward. `ls-remote` confirms `93613ae`, with no spike branch or tag on the remote.
- **Card:** fetch and check out `93613ae`, default routes blinking + lod_corruption, and the three fixes (run the launch line exactly as written, read R1 from the source clone, keep the copied plugin out of the game's repo). It reads size, RHI, client, polluted, tagged and unavailable as numbers only. I ran its read commands on the home D3D12 output and they print the expected values.

**Phase B note** (from 4.25 source and assets; nothing was run)
- `/Engine/EngineMeshes/SkeletalCube` exists as a real skeletal mesh.
- The py27 material script mostly works on 4.25, with two exceptions:
  - `used_with_nanite` does not exist and must be dropped.
  - `unreal.MaterialShadingModel` is probably not exposed to Python, so the script's shading-model check must change.

**Safety**
- 0 crashes and 0 upload signs across 8 sessions; admission waits peaked at 244 s.
- Paris Content, Config, plugin Content, descriptor and `.uproject` are hash-equal; binaries changed only while builds ran.
- The fixture's assets and preserved `Engine.ini` are hash-equal. `m51` passes at every check.
- No deletions. Switching the fixture worktree off the spike removed three spike-only files; they are preserved in git.
- My status lines were 2–5.5 minutes apart, with one 6.5-minute gap (03:58–04:04) as the Paris builds began. Every build and session was still caught.

**Decisions for chat:** none. Two notes:
- Instanced, hierarchical-instanced and skeletal meshes are covered only by unit tests; no fixture contains them, so they were never drawn live. 094-44's SkeletalCube would be the first live skeletal check.
- The D3D12 render-thread cost is about 1.6 ms per captured frame.

Full review: `_reviews\094-43r-code-review.md`.
api-equivalent cost (not billed on Max): $29.15 | turns: 210 | running total across all briefs: $5,629.49
