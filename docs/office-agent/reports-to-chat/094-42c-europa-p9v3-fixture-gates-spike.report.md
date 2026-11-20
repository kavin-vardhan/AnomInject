# REPORT for 094-42c-europa-p9v3-fixture-gates-spike.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 094-42c-europa-p9v3-fixture-gates-spike.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-11-03 17:04:57 | finished: 2026-11-03 23:25:08 | duration: 380 min | exit code: 0
context at last turn: 633570 tokens (63% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\094-42c-europa-p9v3-fixture-gates-spike.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN

All four gates pass, checked against the raw files by my own readers. Codex exited 0 after 21,824 s (17:14 to 23:18 IST), with verdict GREEN. My review is `_reviews\094-42c-code-review.md`, and Codex's report is `_reviews\094-42c-codex-report.md`.

**For the owner:**
- **Did we reproduce the leak at home?** Yes. With the outline effect switched on, the saved frames gain magenta outlines on every tagged object. This includes the outline of a cube that the labels say is hidden.
- **Does the lock stop it?** Yes. The guard refuses the capture before any object is tagged.
- **Is the cheap fix route confirmed?** Not yet. Its direct form does not link against the installed engine, and the alternative form was not tried. The expensive route works but costs about 48 minutes of shader recompiling on first launch.

**P9 v3**
- **The baseline:** my check confirms v3 is v2 with exactly the 89,583 `D:\IA_BankOverflow` lines removed, byte for byte, leaving 382,969 entries. Splitting the v1 baseline by v3's removed folders reproduces v3 byte for byte, and the v1 and v2 files are unchanged.
- **The kit lines (commit `de29514`):**
  - `owner_p9.py`: the baseline folder, the receipt name and the hash constant now point at v3.
  - `root_attribution.py:56`: now reads v3.
- **The tests:**
  - On the current disk: 382,970 files against 382,969 expected, with 0 unexplained differences.
  - The planted test: an expected entry for an absent file under the StackOBot Content folder is reported missing, and the check stops. No file was created or deleted.
  - Limit: the tests call `derive()` directly and copy the inventory and missing-file code out of `main()`. The rest of `main()` (the full rehash, toolchain and disk checks) did not run, because FJ1 defers it.

**The fixture**
- **`-nowrite`:** every one of the 12 sessions carried it.
- **Assets:** seven, with hashes recorded and matching my own (`EA_Outline.umap`, two two-level meshes, and four materials).
- **Preserved Engine.ini (GA2):** the 1,502-byte `Saved\Config\Windows\Engine.ini` is unchanged.
- **Fixture-only writes:** the fixture's own config, scripts and source changed. The editor also wrote two tiny files under the fixture's `Saved` folder that `-nowrite` does not block.
- **Installed 4.25 engine:** 0 of 179,534 files modified.

**The repro gate**
- **Pairing:** the outline-off and outline-on runs match index by index; their labels differ only by a constant frame offset.
- **Control and untagged frames:** the control run has 0 magenta pixels on any frame, and the 456 untagged frames are pixel-identical between the two runs.
- **Per anomaly**, new magenta on a 3-pixel band around the target's own outline:

| Route | Frames with outline | Pixels per frame on the band |
|---|---|---|
| lod_corruption | 404 of 404 | 6,038–6,321 |
| blinking (target hidden) | 222 of 222 | 6,071–6,321 |

  Codex's 351 and 186 are a subset of these frames.
- **Example, frame 746:** the blinking target cube is hidden, yet its full silhouette is drawn in magenta exactly on the label mask's edge. A copy is at `_reviews\094-42c-example-frame746-hidden-blink-outline.png`.
- **Caveat:** most new magenta is a fill on the tagged floor, so this fixture's shader is not a constant-width outline.

**The guard gate**
- **Log:** `Capture425 host custom-depth reader readers=1 writers=0 unknown=0 tagged=0 bench_bypass=0`, then `C5 refused: host custom-depth reader`. The session exits normally.
- **Receipt:** 0 objects tagged before and after the scan, custom depth left at mode 1, and no frames or labels written.

**FZ6(b)**
- **Cost of a new shader type:**
  - The fresh-shader launch re-requested shaders for 47 materials (53 log lines) and took 2,906 s with 2 workers.
  - The matching warm launch had none and took 22.6 s, so the extra is about 48 minutes.
  - This is one pair of runs, not a benchmark.
- **The two earlier tries** are kept and marked incomplete. One hit the 4 GiB memory floor (with an unexplained "access denied" error while stopping). The other hit the probe's own 1,800 s limit.
- **The link-risk result:** a real build that names the five depth-only shaders' `StaticType` fails to link (5× LNK2001 plus LNK1120).
- **The spike branch:** `spike/europa-09442c-mesh-shader` (4 commits) is local only: not merged into `feat/europa-port`, and not on the remote.

**Safety: none**
- **Crashes and uploads:** no new crash folders, no fatal lines and no upload lines. Two sessions ended because Codex stopped its own processes, and the crash count is 0. The kit first recorded one of those stops as a crash; that was corrected in a separate re-read, and the original record is kept.
- **Paris host:** Content, Config, Binaries, the descriptor and the `.uproject` are hash-equal before and after.
- **Protected refs:** `m51` matches P9 at start, at launch and at exit (`53bf725`, index `526aba7f`, 15 untracked). `master`, `release/2026-10-05` and the 30 tags are unchanged.
- **Binaries:** they changed only during the fixture builds.
- **Deletions and pushes:** nothing was deleted, and nothing was pushed. Origin is still at `d1cdc7b`; the local feature branch is at `d840cdb` with 11 unpushed commits.
- **Processes:** the fixture editors started the Epic Games Launcher twice. Codex stopped both launches by their recorded process IDs, and later sessions suppressed it.
- **Admission waits:** every gate cleared under GA5's 3 h bound; the longest wait was 60 minutes, behind the owner's activity.

**Decisions for chat**
1. **The depth-only route:** a version that looks the shaders up by name, instead of naming `StaticType`, still needs a compile test before this route can be chosen.
2. **The fallback route:** a new shader type forces a full material recompile on first editor launch (about 48 minutes for 47 materials here). On Europa's material count it will be larger; that is an extrapolation, not measured.
3. **The outline fixture:** its shader fills the tagged floor. If the fixture will serve later gates, chat may want a cleaner, constant-width outline material.
4. **Minor:** whether the P9 test method (copying code out of `main()`) is enough under FJ1, and whether the kit should record a stop's intent before classifying the exit, so a deliberate stop is never first logged as a crash.
api-equivalent cost (not billed on Max): $22.21 | turns: 183 | running total across all briefs: $5,553.74
