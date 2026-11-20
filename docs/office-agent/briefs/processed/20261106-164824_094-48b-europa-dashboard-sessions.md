# ACK-VERDICT: pre-staged by chat on owner order (no time to wait): whatever 094-48 reported (GREEN, NEEDS-DECISION or RED, including an idle-gate timeout), this brief continues it. It re-runs any failing or unrun F-gate under the owner idle-wait waiver below, then does the flat-session change.
# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 094-48b Europa: dashboard captures written as normal flat session folders (no epoch01) that the AnomDash Library lists, previews and overlays like 5.1; rebuild package, test, push, card

# 094-48b — 4.25 dashboard captures must be normal sessions in the dashboard Library (owner-urgent)

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

**Why:**
- **The problem:** at the office, the 4.25 dashboard capture writes `<Captures>\session_<ts>\epoch01\…`.
- **What the Library expects** (`anomaly-dashboard` `src-tauri/src/sessions.rs:220-245`, `overlay.rs`): a session folder **directly** containing `annotation.json`, `labels.jsonl` and `run_summary.json` (which marks it complete), plus the frames and masks that `labels.jsonl` refers to.
- **The result:** the owner's captures don't show up.
- **The owner's words:** "normal sessions, not epoch01 stuff".
- **Speed matters.**

**Starting point:** `feat/europa-port` at 094-48's pushed tip, or its local tip if 094-48 didn't push (say which). **If 094-48 ended before F1–F6 all ran or passed (for example, the idle-gate timeout), Codex finishes 094-48's change and gates here first**, under the waiver below (`_reviews\094-48`'s brief is in `_mailbox\processed`). Then it does this change. Push once, at the end, covering both.

## Start (quick)
- **The watcher starts you inside the protected `m51` checkout.** Every write uses an absolute path. Receipts go in `D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\code-receipts-094-48b\`.
- **Check** `m51` against P9 (read-only git). Record the plugin and dashboard tips.
- **Read:**
  - `_reviews\094-48-code-review.md`;
  - 094-48's report;
  - the dashboard's `sessions.rs` and `overlay.rs` at `b8ffaca` (read-only git).
- **The resume:** write `resume-prompt-094-48b.txt` with "The change" and "Gates" verbatim, then resume thread `01a14334`, detached, with W4, plus `-OwnerApprovedFullAccess`.
- **Owner's speed instruction (2026-11-06): run Codex in fast mode.**
  - Pass `-Effort high` to the relay (the fastest it allows).
  - **Also enable Codex's fast mode.** The documented options are `-c service_tier="fast" -c features.fast_mode=true` (TOML string; `service_tier = "fast"` plus `[features] fast_mode = true` in config.toml).
  - Make a copy of the relay resume script (`relay_resume_codex_impl_v1.ps1` → `relay_resume_codex_impl_fast_v1.ps1`) that adds those two `-c` options to the `codex exec resume` line. Use it for this job, and record the exact flags in the receipts. Leave the copy in place, because today's later briefs will reuse it. Do **not** edit `config.toml` or any global Codex setting; fast mode is per-run only.
  - **Fast mode is reportedly limited to certain models and ChatGPT sign-in.** If Codex rejects the options or reports the model isn't eligible, record that and rerun with the original script at `-Effort high`. Don't spend more than one attempt on it.
  - Never change the model.

- **Owner waiver (2026-11-06): the 15-minute input-idle wait (GDP10 / "094-48 item9") is WAIVED for this job and every brief today.**
  - The owner may be using the PC. Launch the runtime gates without waiting for keyboard or mouse idle, and record `idle_wait_waived=true` in admission rows.
  - Keep the other admission checks: GPU, physical memory ≥ 6 GiB, and no foreign UE or build process.
  - Put this waiver verbatim in the resume prompt, so Codex's harness doesn't re-impose the wait.

## The change (Codex)
1. **Dashboard-started 4.25 captures are flat.**
   - Write directly into `<base>\session_<timestamp>\`. No `epoch01` subfolder.
   - Use the same file names and JSON shapes the 5.1 dashboard reads: `annotation.json`, `labels.jsonl`, `run_summary.json` (written at the end, so it marks the session complete), frames and masks at the paths `labels.jsonl` and `overlay.rs` resolve.
   - Where the 4.25 schemas differ from what `overlay.rs` and `sessions.rs` parse, **make the 4.25 writer emit the 5.1 shape**. Add fields; never change label or mask meaning.
2. **Console `IAI.Capture.C4` and `IAI.Capture.C5` keep `epoch01`** (the kit depends on it). Only dashboard-started captures change.
3. **No dashboard code change unless it's unavoidable.** If one is needed, keep it minimal, on `feat/europa-425`, and rebuild the desktop exe at home.
4. **Rebuild** the 4.25 Game (and the Editor if needed). Restage the package exe without a re-cook if it's code only.

## Gates (packaged Development game, D3D11)
- **S1:** a dashboard-protocol free-play capture of 600 frames writes a flat `session_*` folder containing `annotation.json`, `labels.jsonl`, `run_summary.json`, 600 frames and the masks. There is no `epoch01`.
- **S2, the Library reads it,** proven with the dashboard's **own** Rust code at `b8ffaca` (or the new dashboard tip). Run a small test harness or `cargo test` that calls:
  - `sessions::is_session_dir`;
  - `describe` (complete = true, correct frame count);
  - `overlay::plan_session` and `plan_outlines` on that folder.

  All succeed with no "not in the expected format" errors, and the overlay boxes land on the labelled events.
- **S3:** the real desktop dashboard exe, pointed at that captures folder, lists the session. Log or screenshot evidence from home only.
- **S4:** the 094-48 gates still hold for this capture: player input not ignored, the first fire before frame 30, strict texture checks, the game still running, and an until-stop capture also produces a flat, complete session.
- **S5:** the console C5 still writes `epoch01` and passes, in one D3D11 run.

## Push and card (yours, after your own review)
1. **The pre-push scan,** then push `feat/europa-port` (fast-forward, no tags or force, `ls-remote`). Push the dashboard `feat/europa-425` only if it changed.
2. **Write `_reviews\094-48b-office-card-europa-dashboard.md`.** It's short:
   - the commit to pull;
   - repackage with `-skipcook` if only code changed;
   - launch without `-IAIBench` (`-game -ExecCmds="IAI.Server.Start 8077"`);
   - dashboard Start (free-play, any length or until-stop);
   - captures appear in the Library as normal sessions;
   - numbers-only reads.

## Rules
- **Poll** every 2–3 minutes during builds and sessions, with UTC epochs.
- **Never steer Codex.** No work by you beyond the review, scan, push and card.
- **Never touch:** `m51`, `master`, `release/2026-10-05`, tags, the dashboard's `master`, `E:\<personal-folder>`, `E:\UEProjects\LyraStarterGame`, `E:\Unreal Projects\StylizedParisStreet`.
- **Never kill by name.** No machine settings. Delete nothing except by receipt. Never print a token.
- **Codenames only.**

## Final message
1. **`VERDICT: GREEN | NEEDS-DECISION | RED`.** GREEN means S1–S4 pass and the push is confirmed.
2. **A plain summary for the owner** (3 lines): do dashboard captures now show up as normal sessions, what to pull, and which card.
3. **S1–S5 with numbers; the push and the card.**
4. **Safety,** or "none".
5. **Decisions chat must make,** or "none".
