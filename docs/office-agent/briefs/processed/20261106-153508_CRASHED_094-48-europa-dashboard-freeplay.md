# ACK-VERDICT: 094-47s RED (rc=1, 1-minute stop) is superseded by the Codex takeover record and chat's GR1–GR5 (_reviews\094-47w-chat-ruling.md); 094-47 is published (plugin bd767ca, dashboard b8ffaca). This brief starts the owner's urgent free-play fix.
# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 094-48 Europa: dashboard Start runs FREE-PLAY capture (C4 semantics: player moves, any length or until-stop, anomalies from ~frame 3, game stays open), rebuild package, test, push, card

# 094-48 — make the dashboard's Start a normal free-play capture on 4.25 (owner-urgent)

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

**Why this job exists:** the owner is at the office preparing a build of Europa for the client. Through the dashboard, 4.25 capture currently runs **C5**, the bench street sweep. That means the player can't move, the length is fixed at 2,352 frames, there is a 624-frame clean prepass before any anomaly, and it only starts under `-IAIBench`. That's a test harness, not a product. The owner needs the dashboard's **Start** to do a **normal capture, as on 5.1**:
- the player moves freely;
- the length is any frame count or "until I stop";
- anomalies fire from about the start;
- the game stays open afterwards.

The console already has this behaviour as **`IAI.Capture.C4`** (`AnomalyCaptureLabels425.cpp:485`, at `bd767ca`): no street bench, any count, auto-injection from frame 3, and the same labels, masks and private pass.

**This is the owner's top priority. Speed matters:** aim to finish in about 2–3 hours. No extra re-hashes or auxiliary studies.

## Start (quick)
- **The watcher starts you inside the protected `m51` checkout.** Every write uses an absolute path. Receipts go in `D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\code-receipts-094-48\`.
- **Check:**
  - `m51` matches P9 by read-only git (HEAD `53bf725`, index `526aba7f`, 15 untracked);
  - `feat/europa-port` local equals origin at `bd767ca`, clean;
  - dashboard `feat/europa-425` at `b8ffaca`.

  If not, stop with NEEDS-DECISION.
- **Read only:**
  - `_reviews\094-47-final-report.md`;
  - `_reviews\094-47-office-card-europa-dashboard.md`;
  - `_reviews\094-47w-chat-ruling.md`.
- **The resume:**
  - Preserve the previous report outputs.
  - Write `resume-prompt-094-48.txt` (UTF-8, no BOM): this brief's "The change" and "Gates" sections verbatim, short context, and the GDP implementer block as used in 094-47.
  - Resume thread `01a14334`, detached, with W4, plus `-OwnerApprovedFullAccess`.
  - If the resume fails twice, start a fresh full-access thread.

## The change (Codex implements; chat's design call)
1. **The dashboard's `capture_start` on 4.25 runs free-play capture (C4 semantics), not C5:**
   - no street bench, no input freeze, no camera override;
   - routes are the dashboard's enabled anomalies, validated against the pool (the defaults are the four; camera_clipping only if the dashboard enables it);
   - the seed comes from the request, or from the auto-injector when it's omitted;
   - **any `maxFrames` ≥ 1, or 0/absent meaning "until I stop"**;
   - auto-injection runs from about frame 3, and on a finite run it stops 24 frames before the end, as C4 does;
   - **the dashboard path never exits the game:** `CaptureExit425=false` on completion, stop and refusal;
   - a fresh `session_<timestamp>\epoch01` under the dashboard's base folder, as now.
2. **Free-play doesn't require `-IAIBench`** when it starts from the dashboard. The control server already exists only in non-Shipping builds; that is the gate. **Every bench-only path stays gated:** C5, BenchGeometry, the street bench and `-IAIBench` levers.
3. **Stop:** "until I stop" and early stops end **cleanly and normally**. Accepted frames are drained, state is restored, and the receipt reads PASS for the frames written. It must not report PARTIAL as a failure; keep a field saying it was stopped by the user.
4. **The console's `IAI.Capture.C4` and `IAI.Capture.C5` keep their current behaviour.** The kit depends on it.
5. **No change to label or mask meaning,** the private pass, the four defaults, or Shipping exclusion.
6. **Rebuild** the 4.25 Editor and Game. **Restage the package's game exe** without a new cook if only native code changed, as Codex did before. Re-cook only if content changed.
7. **The live preview stays out of scope.** Don't attempt it in this job.

## Gates (packaged Development game, D3D11 1280×720; the plugin's existing readers)
- **F1, free-play from the dashboard protocol** (the smoke script driving `capture_start`), at 600 frames with seed 4242:
  - the run completes;
  - 600 written and paired frames, 0 dropped;
  - the first anomaly fire is before frame 30;
  - **player input is not ignored during the capture** (record `IsMoveInputIgnored`/`IsLookInputIgnored` false);
  - no street-bench sweep fields in the rows;
  - labels and masks pass the strict texture checks (IoU 1.000, 0 spill) on texture fires;
  - `polluted=0`, tagged 0;
  - **the game is still running afterwards.**
- **F2, until-stop:** start with `maxFrames` 0, then stop after about 300 frames. A clean normal end, PASS for the frames written, 0 dropped, the game still running, and a second free-play capture then starts and completes (any short length).
- **F3, without `-IAIBench`:** the packaged game launched **without** `-IAIBench` accepts the dashboard's free-play start (F1, shortened to 300 frames). C5 and the bench commands are still refused there.
- **F4, the regression:** the console C5 on EA_Core with `-IAIBench` still passes H4 and the strict texture gates (one D3D11 run).
- **F5:** the real desktop dashboard exe connects to the new package (log-marker evidence), and its **Start** runs F1's preset with Random mix, the four defaults, and a custom length of 900.
- **F6:** Shipping still excludes the control server.

**If F1–F3 pass and F4 fails only for a reason unrelated to this change,** report it, but don't block the push.

## Push and card (yours, after your own review of F1–F6)
1. **The pre-push scan** of `bd767ca..<tip>`, then push `feat/europa-port` (fast-forward, no tags or force, `ls-remote`).
2. **The dashboard:** push only if it changed. Keep `master` untouched.
3. **Update `_reviews\094-47-office-card-europa-dashboard.md` → write `_reviews\094-48-office-card-europa-dashboard.md`.** The new card covers:
   - the launch line **without** `-IAIBench` and without the bench capture settings (just `-game` plus `-ExecCmds="IAI.Server.Start 8077"`);
   - dashboard Start for free-play, with any length or until-stop;
   - the console fallback, `IAI.Capture.C4 <dir> <frames> <seed>` (it needs `-IAIBench` and exits the game when finished);
   - that preview is still unavailable;
   - numbers-only reads.
4. **Copy the card** to the claude.ai project is chat's job. Just write it to `_reviews`.

## Rules
- **Poll** every 2–3 minutes during builds and sessions, one line each in `code-receipts-094-48\midjob-notes.txt`, with UTC epochs.
- **Never steer Codex.** No work by you beyond the review, scan, push and card.
- **Never touch:** `m51`, `master`, `release/2026-10-05`, tags, the dashboard's `master`, `E:\<personal-folder>`, `E:\UEProjects\LyraStarterGame`, `E:\Unreal Projects\StylizedParisStreet`.
- **Never kill by name.** No machine settings. Delete nothing except by receipt. Never print a token.
- **Codenames only.** No office data exists at home.

## Final message
1. **`VERDICT: GREEN | NEEDS-DECISION | RED`.** GREEN means F1–F3, F5 and F6 pass and the push is confirmed.
2. **A plain summary for the owner** (3 lines): does the dashboard Start now give a normal free-play capture, what to pull, and which card.
3. **F1–F6 with numbers; the push and the card.**
4. **Safety,** or "none".
5. **Decisions chat must make,** or "none".
