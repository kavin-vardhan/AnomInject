# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 094-47 Europa control-server port (4.25 WebSocket transport + shared C5 entry), packaged Development build, dashboard LOD corruption, console-vs-dashboard equivalence, scan and push

# 094-47 — Codex ports the control server to 4.25 and packages the build; Code reviews, scans and pushes the plugin and the dashboard

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

**Context:**
- **Where we are:** `feat/europa-port` has camera_clipping pushed (094-46s; tip `<46s TIP>`, origin equal). The owner's top priority now is **the dashboard working with a packaged Development build of the 4.25 port**.
- **The design:** `_reviews\094-47-codex-controlserver-plan.md` (Codex, read-only, from the owner's own thread), approved with bounds in `_reviews\094-47-plan-ruling.md` (GQ1–GQ5).
- **The PC belongs to the owner.** Respect the kit's admission gates and GA5's 3 h bound. Docs: minimal, as the owner asked. Code receipts and evidence still apply.

## Start (quick)
- **The watcher starts you inside the protected `m51` checkout.** Every write uses an absolute path. Receipts go in `D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\code-receipts-094-47\`.
- **Check `m51` against P9** with read-only git (`GIT_OPTIONAL_LOCKS=0`): HEAD `53bf725`, index `526aba7f`, 0 tracked changes, the 15 untracked entries.
- **Check the branches:**
  - `feat/europa-port` local equals origin at `<46s TIP>`, clean;
  - the dashboard repo `D:\IntrusiveAnomalies\anomaly-dashboard` is at `7ea2e51` on `master`, clean.

  Any difference: stop with NEEDS-DECISION.
- **Read only:**
  - `_reviews\094-47-plan-ruling.md`;
  - `_reviews\094-47-codex-controlserver-plan.md`;
  - `_reviews\094-46s-code-review.md`;
  - `_reviews\094-46s-office-card-europa.md`.
- **Take your own hashes** of:
  - the Paris host's own `Content`, `Config`, descriptor and `.uproject`;
  - the 4.25 and 5.1 material `.uasset`s.

  Record C:, D: and E: free space.
- **The resume:**
  - Preserve the previous report outputs.
  - Write `resume-prompt-094-47.txt` (UTF-8, no BOM): the plan verbatim, GQ1–GQ5 and the stages verbatim, short Code-added context, and the GDP implementer block as used in 094-43r.
  - **Add the dashboard repo** to the allowed writes, on branch `feat/europa-425` only.
  - **Add the packaging output folder** (a new folder on E:, named in the prompt) to the allowed writes.
  - Resume thread `01a14334`, detached, with W4, plus `-OwnerApprovedFullAccess`.
  - If the resume fails twice, start a fresh full-access thread.

## The stages (Codex does the work; each stage gates the next)
- **Stage A, the code:**
  - GQ1's transport, C5 bridge, include fix and transport tests, committed with W4;
  - 4.25 Editor and Game compile;
  - the **5.1 compile** of the changed modules on the existing 5.1 tree (GQ2), naming which tree;
  - the protocol smoke on the **editor `-game`** build with `tools/europa/control_server_425_smoke.ps1`: connect, hello, catalog (the four defaults), a bad-token refusal, then start, stop and a second start of C5.
- **Stage B, the package:**
  - GQ5: `BuildCookRun` Win64 **Development** of the Paris host with the plugin, into the new E: folder;
  - a console-started C5 **from the packaged exe** on EA_Core (bench flags) passes H4 and the strict texture gates on D3D11 1280×720;
  - the dashboard's `feat/europa-425` change (LOD corruption shown), its existing tests, and the desktop build (GQ3). If the toolchain is missing, stop with NEEDS-DECISION.
- **Stage C, equivalence on the packaged exe,** per RHI (D3D11 and D3D12 at 1280×720):
  - a console-started C5 against a dashboard-started C5 under the plan's gate 4 (seal the predictions first);
  - one early-stop test;
  - reconnect, and survival for a second capture.
  - Use the dashboard's real protocol, driven by the smoke script. A desktop-UI click session is not required; one connection by the built desktop exe to the packaged game **is** required, as evidence that discovery and auth work, shown by the log markers.

## Poll
- **Cadence:** every 2–3 minutes while a build, cook or session is likely; otherwise about 5 minutes.
- **One line per poll** in `code-receipts-094-47\midjob-notes.txt`:
  - free space (C:, D:, E:) and commit;
  - build and cook processes;
  - our editor, game, crash-monitor and ShaderCompileWorker PIDs;
  - how long any admission gate has held;
  - the current stage.
- **Use UTC epoch seconds.**
- **Never end the run while Codex runs.** The cap is 24 h. **Never steer Codex.**

## After Codex exits: the review, `_reviews\094-47-code-review.md`
1. **Stage A:**
   - the transport diff against the plan: loopback bind, raw binary with no prefix, reassembly, bounds, game-thread dispatch, single teardown;
   - the C5 bridge: one shared path, cancellable ticker, refused starts return an error, keep-alive;
   - the include fix;
   - both compiles;
   - the smoke results.
2. **Stage B:**
   - the package (size, location, and that the 4.25 materials and private pass were cooked);
   - the packaged console C5 gates, recomputed by you;
   - the dashboard diff, tests and build.
3. **Stage C:** the equivalence comparison recomputed by you, the early stop, reconnect, the second capture, and the desktop-exe connection evidence.
4. **Safety, as always:**
   - the Paris host's own Content and Config hash-equal;
   - packaging writes only to its output folder plus recorded `Saved`/`Intermediate`;
   - `m51` checked by git;
   - nothing deleted except by receipt.

## Push (yours), only if every stage passes by your own review
1. **Pre-push scans:**
   - `_reviews\094-47-prepush-scan.md` for `feat/europa-port`;
   - `_reviews\094-47-dashboard-prepush-scan.md` for `anomaly-dashboard` `feat/europa-425`.
2. **Push both branches** under the 094-39 rules: those branches only, no tags, backup refs or force, fast-forward or new branch, confirmed with `ls-remote`.
3. **Write a short card,** `_reviews\094-47-office-card-europa-dashboard.md`:
   - package the game as Development;
   - set or discover the token;
   - start the listener (`IAI.Server.Start`);
   - open the dashboard and connect;
   - run the preset (Random mix, the four defaults, PNG, native size, seed 4242, 2352 frames);
   - numbers-only reads.

**If a stage fails,** push only what stands alone and compiles, with a clean scan, and say exactly what was pushed.

**Where you may write:**
- `code-receipts-094-47\`;
- the resume prompt;
- the preserved report outputs;
- the `_reviews\094-47-*` files;
- `D:\IA_Europa\Receipts\CodeReview09447\`;
- `D:\IA_Europa\Temp\CodeReview09447\`;
- and the two pushes.

## Hard rules
- **No work by you** beyond the review, the scans, the pushes and the card: no source edits, builds, launches, commits or history rewrites. Codex does the work.
- **Never touch the editor, the game, their windows or their dialogs.** Never touch the owner's processes.
- **Delete nothing.** No reparse points.
- **Kill nothing,** except, if necessary, the relay's Codex process, by PID.
- **Never print or copy a secret value,** including the control-server token. Mask it in every receipt.
- **No security, paging, power or machine-setting changes. No elevation.** No firewall changes; the listener is loopback-only.
- **Don't touch:** `m51`, `master`, `release/2026-10-05`, tags, the dashboard's `master`, `ToCodex\`, `E:\<personal-folder>`, `E:\UEProjects\LyraStarterGame`, `E:\Unreal Projects\StylizedParisStreet`.
- **Codenames only.**

## Final message
1. **`VERDICT: GREEN | NEEDS-DECISION | RED`.**
   - **GREEN:** all three stages pass by your own checks, both pushes are confirmed, and the card is written.
   - **NEEDS-DECISION:** a stage stopped, a gate is short, or a toolchain is missing. Say what was pushed.
   - **RED:** a second crash, an upload sign, a change to the Paris host's own Content or Config, a changed material `.uasset`, a non-fast-forward push, or a leaked token.
2. **A plain summary for the owner** (3 lines): does the dashboard connect to and run captures on a packaged 4.25 build, is it pushed, and which card to follow.
3. **Stages A, B and C with numbers; the pushes and the card.**
4. **Safety,** or "none".
5. **Decisions chat must make,** or "none".
