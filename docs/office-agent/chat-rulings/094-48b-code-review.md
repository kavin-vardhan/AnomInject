# 094-48b — Claude Code review of Codex's 094-48 completion and flat-session change

**Verdict: NEEDS-DECISION.** I agree with Codex's measured results. The push is **HELD**: S4 (and F1/F3/F5) fail the strict texture IoU gate, and only chat can rule on that gate.

## What ran

- **094-48 was interrupted.** The mailbox watcher died at about 15:34 IST and the Codex turn died with it: no exit code, no report, no commit, no push. It left 9 uncommitted source files in the Paris worktree (diff sha256 `877849F2…`, receipt `code-receipts-094-48b\entry-receipt-094-48b.txt`). No process of that attempt survived. The PC did not reboot.
- **The resume.** I resumed thread `01a14334` with `resume-prompt-094-48b.txt` (sha256 `89A50A46…`). It contains the brief's first line, Why/Starting point, the owner's idle waiver, The change and Gates verbatim, plus my context notes. The launch was detached, with the W4 environment removal, `-OwnerApprovedFullAccess` and `-Effort high`, through the new `relay_resume_codex_impl_fast_v1.ps1`.
- **Fast mode was accepted.** The rollout records `service_tier="priority"` and `reasoning_effort=high` (094-48 ran `default`/`max`). The model is unchanged (gpt-6-astra). Exit 0 after 3,385 s, ledger row `…-resume76`.
- **Commits** on local `feat/europa-port`:
  - `6b171b5` free play. Its source is byte-equal to the interrupted worktree diff.
  - `1667d05` flat sessions.
  - Origin stays at `bd767ca`. The dashboard is unchanged at `b8ffaca`.

## My independent checks

- **The 094-48 diff** (control server + 4.25 capture).
  - The dashboard `capture_start` now calls `StartFreePlayRun`. It accepts any `maxFrames >= 0`, where 0 or absent means until stop. JSON-number type checks were added. The seed comes from the request or the auto-injector, the routes are validated against the pool, and `CaptureExit425=false`, so the game never exits.
  - The street bench and camera slab bench are excluded on this path (`AllowBench=false`). C5, BenchGeometry and the `-IAIBench` gates are untouched.
  - The session timeout counts from last progress. Stop drains and ends as PASS with `stopped_by_user`. Input-ignored flags are recorded per row.
- **The 094-48b diff** (5 source files).
  - Every change is gated on `CaptureFreePlay425`/`FreePlay`. Dashboard captures write `Actual_Frames/frame_%05d.png` and `target_mask/frame_%05d.png` directly in `session_*`. Rows gain `session_index`, `mask_file`, and per-anomaly `bbox_px` ([x, y, w, h], as `overlay.rs:208-225` reads it), `bbox_norm`, `bbox_valid` and `mask_value`. The annotation's `frames_dir` becomes `Actual_Frames`.
  - `run_summary.json` is written as `.pending` and promoted only after `capture.json`, on a passing run, so it is the last file and marks completion.
  - Mask pixels are unchanged. The console C4/C5 paths and `epoch01` are unchanged, and S5 confirms it.
- **S2 re-run by me.** `cargo run --offline` of Codex's harness passes. Its `sessions.rs` is byte-identical to `b8ffaca`. Its `overlay.rs` is `b8ffaca` plus 43 appended lines (0 removed, by line diff). Output: complete=true, 600 frames, 1280×720, 23 events, 285 boxes all SHIPPED and matched to row `bbox_px`, 285 outline frames, and 13,231,036 mask pixels all inside their boxes.
- **S1 read directly** (`flat_session_check48b.py`). No `epoch01`. 600 rows with `session_index` 0..599 exact. Every `image` is `Actual_Frames/frame_<si>.png` and exists. There are 285 non-null `mask_file` (0 missing) and 315 null. All 600 mask PNGs exist; the 315 unreferenced ones are blank (RGB 0). `run_summary` reads total 600, measured 600, dropped 0, PASS.
- **S3.** `S3-03\library.png` shows the desktop Library listing the session (600 frames, 23 anomalies, thumbnail). The owner's `E:\AnomalyCaptures` is unchanged (newest entry 2026-10-04), and the settings file was restored byte-equal.
- **The strict texture failure, broken down** (`f1_texture_breakdown48b.py` over Codex's own reader JSON):

  | Run | missing_texture mask px unchanged | corrupted_texture mask px unchanged | spill outside mask | changed px on clean edge frames | labelled frames with no change |
  |---|---|---|---|---|---|
  | F1 | 64.2 % | 5.3 % | 0 | 0 | 0 |
  | F3 | 59.2 % | 1.2 % | 0 | 0 | 0 |
  | F5 | 63.8 % | 4.5 % | 0 | 0 | 0 |
  | S4 | 64.2 % | 5.3 % | 0 | 0 | 0 |

  IoU is constant within each event, which fits a static unattended view. ⇒ **The labels sit on the exact frames.** The miss is that target pixels don't visibly change under the swap in this scene. Some checker cells and shadowed magenta stay within ±2 levels of the original. The C5 fixture was built so that every pixel changes, and it passes (F4, S5: 2,352 frames).
- **Pre-push scan** of `bd767ca..1667d05`: clean (`_reviews\094-48b-prepush-scan.md`).
- **Protected state.** m51 HEAD `53bf725`, index `526ABA7F`, 0 tracked changes, 15 untracked. master `b5f15a3`, release `d7eee97`, 30 tags, 76 refs. No leftover game, dashboard, UBT or Codex process.

## Observations for chat (not blocking)

1. Flat mode writes a blank `target_mask` PNG for frames with no target pixels; the row's `mask_file` stays null. 5.1 (since m44) writes no blank PNG. Codex chose this so the Library's settle count (`target_mask_frames_measured` = written) is met immediately.
2. `mask_value` is 0 when a target has more than one stencil tag. The overlay then draws the box but no outline.
3. The session folder still carries the internal `evidence-*`, `reduce-*` and `ui-*` files at its top level, as `epoch01` did. The Library ignores them.
4. The disk reserve schedule from 094-48 (C/D/E floors 10/25+3/10 GiB) was carried. E: is at 14.96 GiB free.
5. The interrupted 094-48 attempt has no ledger row (resume75 is absent), which is cosmetic.
