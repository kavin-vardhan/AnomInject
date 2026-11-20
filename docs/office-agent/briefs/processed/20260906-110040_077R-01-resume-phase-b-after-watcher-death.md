# NAME: 077 m49-phase-b
# EFFORT: xhigh
# MODEL: claude-opus-5

# 077R-01 — RESUME Phase B after the watcher died on 2026-09-04 at 21:14: establish the tree state, finish the post-cook pose finding, land or park Phase B honestly
This resumes the SAME Code session as 077-01 (the watcher lost it mid-turn at 21:14:25 on 2026-09-04, while you were reading the camera traces of `PBAB_AB_PRECOOK` vs `PB5C_AEON_B`). Nothing you did after that point reached anyone — there was no final message. It is now Sunday 2026-09-06; the bench PC was off in between.
If you do NOT remember the 077-01 work (the drawn-count shader, the cook, the pose investigation), your own transcript is at `D:\IntrusiveAnomalies\_mailbox\to_chat\077-01-m49-phase-b-drawn-count-and-cook.live.txt` (read-only; lines indented three spaces are your own reasoning; `->`/`<-` are tool calls). Read it before anything else in that case.
Headless; nobody answers; FINAL MESSAGE structure in §5; failed gate = branch-and-restore, report, STOP. Codename discipline (Concorde / Bates / Europa only). ⛔ Office data never leaves the office PC. ⛔ Never write temp files into `_mailbox`.
⛔ SCOPE FENCE: an external code review of the plugin landed today and becomes milestone **m51** under its own brief (078-01). Do NOT read it, act on it, or widen this brief toward it. This brief is Phase B only.

## 0. Step 0 — state of the world (journal it BEFORE touching anything, so a second crash loses nothing)
Write `docs/sessions/2026-09-04-077-m49-phase-b.md` §1 first, from these reads:
- `git status`, `git log --oneline -6`, current branch. Expected from chat's read-only look: branch `m49-phase-b` at `5e45339` (`feat(capture): m49 phase B …`) on top of `18343b1` (predictions); UNCOMMITTED: `Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp` (~18 lines), `docs/PRE-DELIVERY-CHECKLIST.md`, `docs/architecture.md`, `docs/client-delivery.md`, `docs/client-readme.md`; untracked `m50_pc7v3.py`. Say what each uncommitted hunk is FOR — the 18 code lines especially: post-`5e45339` gate work, or a fix in progress for the pose finding? Do not discard anything; if a hunk is unexplained, commit it on the branch as `wip(077): …` with the honest message and move on.
- Binaries: which exe is staged now (hash), which A-sides are archived, and was the cook COMPLETED and its container quartet recorded in `_binary_baselines/README`? If the quartet is not recorded, record it now from the cooked container.
- Gates: which of 077-01 §4 (PB-1..PB-5, the regression set, P-C7 v3, Lyra) had PASSED at the crash, which was running, which never ran. Use the banked legs (`PB5C_AEON_A`, `PBAB_AB_PRECOOK`, `PB5C_AEON_B_try1`, `PB5C_AEON_B_try2`, `PB0_SMOKE_try1..3`) and their logs — do NOT re-run legs to find this out.
- Lyra worktree: SHA it is at, whether LyraEditor was rebuilt from the Phase B SHA.

## 1. The post-cook pose finding — finish the read you were in the middle of
Your last recorded conclusion (21:13): "on the same box state the pre-cook build accepted on attempt 1 with a perfect pose; the box is exonerated; the post-cook build behaves differently"; pacing identical across the cook (`speed_ratio` 1.0000 both sides, tick counts unchanged); the camera trace itself differs. Finish it:
- State the difference in NUMBERS (pitch / yaw / origin per `si`, both legs, the first 14 rows you had already printed).
- Decide, with evidence, which it is: **(a)** a cook-time config/asset difference (cooked `DefaultGame.ini` vs loose ini — the G88 class; a settle/pose knob that only the cooked ini carries), **(b)** a Phase B code path that touches the view or the first frames (the exposure-exclusion mask, the drawn-count arm, anything that changed tick/render pacing at session start), **(c)** the settle/pose lever itself (attempt counting, `settle_start`, `dropped`), **(d)** a container quartet mismatch (a stale container beside the new exe, or the reverse).
- (b) ⇒ a Phase B defect: fix on the branch, re-stage, re-run the pose-dependent legs; if a shader is involved SAY SO (that is another cook — report, do not cook again unprompted).
- (a)/(c)/(d) ⇒ NOT a Phase B defect: fix the bench/config so the read is honest, gotcha it, re-run the pose-dependent legs.
- Undecidable after ~40 minutes of work ⇒ STOP with the two best candidates and the ONE measurement that splits them. Do not merge.

## 2. Finish the Phase B gates — only what the crash left undone
The remaining 077-01 §4 gates, BOTH tick orders, on the post-cook binary; the Lyra leg from the Phase B SHA (rebuild the worktree if it is not there). `docs/predictions/2026-09-04-m49-phase-b.md` is already written — compare against it; amend with AMENDMENT blocks, never rewrite a prediction after the fact. Pre-cook vs post-cook validity of each leg stated in a table.

## 3. Merge or park — no third option
ALL green ⇒ merge to master with the 077-01 §5 message, push, checkout stays on `master`. Anything red ⇒ leave `m49-phase-b` unmerged and pushed, checkout back on `master`, staged exe back to the last master A-side (`A159EDFD`), and report which gate and why.

## 4. Docs (both outcomes)
Journal 077 complete (§1 state · §2 pose finding · §3 gates · §4 merge/park). CLAUDE.md status block: if merged, `m49` is COMPLETE (A1 + A2 + B) with `m50`; in BOTH outcomes the NEXT line reads: **"m51 — release hardening from the 2026-09-06 external review; plan brief 078-01 follows. Delivery gates on Lyra, card Section G/F on Bates and the client cook come AFTER m51."** client-readme §8 (`target_drawn_pixels`, the hide-class sentence) and client-delivery per 077-01 §5 only if merged. Gotchas for the pose finding at least. Quartet in `_binary_baselines/README`.

## 5. Final message
1. Plain-language rundown (10 lines, no jargon). 2. Step-0 state table (branch, commits, uncommitted hunks and what they were for, staged/archived hashes, quartet recorded Y/N). 3. Pose finding: cause, evidence, fix, gotcha number. 4. Gate table (both orders) + Lyra, with pre-/post-cook validity. 5. Merge status, commits, pushes, both checkout states, Lyra worktree SHA. 6. NEEDS-DECISION if any — one line each.
