# 081-44 — Chat review of 081-43 (GREEN): Codex's fixes accepted, replay clean → comment strip, build check, no-ff merge authorised

Date: 2026-09-26 (IST) · Author: chat Claude (orchestrator) · Feature head `fd175dc`

## Review of the 081-43 evidence (the checkpoint Codex required before merge)
- **All nine findings are fixed as ruled.**
  - Codex's F1, F2 and F3 reproductions now fail as intended: G270 mutation → FAIL (oracle exit 1); MAINON_S2 count mutation → FAIL (INV-2); malformed or ill-typed input → exit 3 with no traceback. Mismatch still outranks malformed input (1 > 3).
  - Unverifiable `empty_region` (no mask PNG) stays distinct.
- **Nine per-leg invariants (INV-1…9) now run before any noise excusal.** Each is proven to fire on its own mutation, with 0 violations over 382 sessions and 59,923 rows. The only exception is the G5 fault gate's deliberate missing mask, which is not a legacy leg.
- **Bank replay, old vs new: no real result changed.**
  - Oracle: 476/476 session folders, exit 0 → 0; 4,981 measured rows matched by both; `empty_region` 0 agree / 0 disagree / 83 unverifiable.
  - Legacy: all 9 groups unchanged. 081-22 C was already FAIL; it is not a legacy-compare group.
  - All 70 G270, stack, Lyra and NoHold evaluations unchanged.
- **The three AMENDMENT 6 proof expectations superseded by F2.2 are confirmed.** A synthetic candidate-only singleton now reads NEEDS-DECISION, as ruled. Ordering proven failures ahead of the new decision was correct.
- **Client docs:** F4–F6 and N1/N2 are corrected; the grep is clean. Accepted.
- **Process note.** The session reached 91 % context. Future fix-and-replay work is split into two briefs.

**Disposition:** the 081-42 findings are closed. Codex asked for this chat checkpoint before merge, and it is satisfied. The merge is authorised.

## Merge procedure (081-44 brief, per the 081-41 merge-prep inventory §2 standing rules)
1. **Strip** with `D:\IntrusiveAnomalies\_strip_comments.py` in a scratch worktree of the feature branch.
   - Expected: `git diff --stat` shows 14 files and 65 changed lines. `AnomalyCapture.Build.cs` must stay byte-identical.
   - Prove the change is comment-only with a token-level comparison: for every changed file, the token stream minus comments is identical before and after. Anything larger is a stop.
   - Commit the strip as its own commit.
2. **Build** both targets from the stripped source: packaged `StackOBot` and `StackOBotEditor`. Both must exit 0.
   - Follow the established build and staging procedure from 081-21/22.
   - A44-scan the staged exe and record its hash and provenance. The exe hash may move (G201).
   - Keep build 3 (`002805CF`) archived under its provenance name.
   - Do not disturb the live `m51` checkout.
3. **Merge no-ff** of `feat/m55-change-evidence` onto `master`.
   - Resolve nothing silently. A conflict is a stop and gets reported.
   - Push `master` to origin. **No tag.**
   - Keep the feature branch.
4. **Docs on master:** a merge journal and the CLAUDE.md status block ("m55 merged; next m53 = UV/normal texture corruption; the owner reordered texture corruption ahead of flicker on 2026-09-26"). Update the roadmap/queue text so the order reads m53 = UV/normal corruption, m54 = flicker.
