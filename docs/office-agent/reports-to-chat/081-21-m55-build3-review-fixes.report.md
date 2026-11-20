# REPORT for 081-21-m55-build3-review-fixes.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 081-21-m55-build3-review-fixes.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-24 03:41:12 | finished: 2026-09-24 04:27:18 | duration: 46 min | exit code: 0
context at last turn: 748294 tokens (75% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\081-21-m55-build3-review-fixes.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN — every fix is in, the one build set is clean, and every step that doesn't need a quiet PC passed. **The four bench legs (a)–(d) were skipped** because the PC was never quiet: four UE 5.7 HeistCrewUE editors ran for the whole round. Per the brief that doesn't block GREEN, but nothing about build 3 has been checked at runtime yet.

## Plain summary for the owner
- The fixes from the review are in. The capture's normal pictures and masks no longer depend on the new change-evidence feature. If the feature can't handle a frame, it now marks that frame "unsupported" instead of dropping the picture.
- Cancelling a capture before the game window has focus no longer leaves an empty session folder behind.
- It built on the first try with no warnings. All tool tests pass, and the checker agrees with every one of the 228 saved sessions.
- The in-game test runs did not happen, because another project's Unreal editors were running on this PC all night. They are still owed.

## What changed
Branch `feat/m55-change-evidence`, pushed. No merge, no tag, no cook.
- **`4e54d57`**: the predictions, pushed before any source edit, build or result.
- **`85c99b3`**, source (6 files, +138/−43):
  - **F1:** the stage now knows whether its run actually started a session. A run that never started is discarded: nothing is saved and no folder is created. This applies in the cancel branch, at the next `StartRun` or `Deinitialize`, and as a backstop inside `CloseAndPersist`.
  - **F2/F3:** every eligible view family consumes the colour arm and publishes its key exactly as before m55, and `AfterPass` always submits a wanted legacy readback. The mask pass serves pending arms on any eligible view. m55 attaches its data only to the capture viewport's own single-view family; otherwise it records `unsupported_delivery`. New counter `change_multi_view_families`. The old pre-m55 multi-view unsoundness is not fixed, as ruled.
  - **F6:** the event-level `late_results` field is removed.
  - **F7:** new counter `change_unsupported_completion`, so `duplicate_completion` and `late_results` now count only real duplicates and real late results.
  - **F9:** a non-zero `IAI.Bench.ChangeGate` without `-IAIBenchFixture` is refused: the run uses gate 0 and logs `BENCH-GATE-REFUSED`.
- **`c27024d`**, oracle (2 files): `--change-oracle` exits 1 on any row or reference mismatch, 0 when everything compared matches, 3 when it can't run. The help text now names the limit that it trusts the row's own pair ids.
- **`94a56a9`**, docs:
  - Field reference: F4, F5, F10, F11, F12, F13, plus a new "legacy delivery never depends on m55" section.
  - The packaging-checklist line "exclude `AnomalyBench` from client packages" is in the branch's `docs/PRE-DELIVERY-CHECKLIST.md`.
  - New gotchas G284 and G285, the session journal, and the status block.
- No comments were added. F8 (stripping the existing comments) is still left for the merge checklist.

## Build identity
- **Game exe `002805CF`** (`002805cf7060…f4e29d`, 241,768,448 B). Built = staged = `_binary_baselines\StackOBot.exe.m55-build3-002805CF`.
- **Editor:** `_binary_baselines\m55-build3-main-editor-c27024d`.
- **Lyra:** `_binary_baselines\m55-build3-lyra-cf0b38cb`.
- **Build times:** Game 221 s, Editor 150 s, LyraEditor 111 s. All exit 0, zero warnings.
- **Unchanged:** the five cooked container files.
- **A44:** the five new tokens are present (UTF-16) in the staged exe and the Lyra capture DLL; the invented token is absent.

## Verification
| Check | Result |
|---|---|
| m47 selftest | 2 → 2, output byte-identical |
| Label-pixel selftest | 98 → 98, output byte-identical |
| Consistency contracts | 25 → 25 |
| Oracle selftest | 22 → 24 cases |
| Oracle contracts | 9 → 9 (exit-code test now expects 0 for good, 1 for bad) |
| Selftest can fail | sabotaging the new exit code breaks 6 of the 24 cases |
| Six existing CLI modes | exit codes identical before/after, on a healthy fixture and on one built to fail |
| Oracle rerun, 228 banked sessions | all exit 0; 1,234/1,234 rows and 1,232/1,232 references match |
| Reviewer's replay, 365 sidecars | 0 violations, output identical before/after |
| F2/F3 model | build 3's legacy behaviour equals pre-m55 in all 24 scenarios and 4,000 fuzz runs; build 2 differs in 3,546. Pending arms never grow |
| F1/F7 checks | pass: 32 start/cancel sequences never write into a deleted folder; unsupported frames count only as `unsupported_completion` |

- **Bench legs:** all four skipped because the PC was busy: (a) the 1080p `solid1080` smoke, (b) the F1 cancel-before-focus leg, (c) a real two-view family, (d) evidence-OFF byte-identity against master.
- For (d), the pre-m55 master binary archive exists: `StackOBot.exe.m52-veto-arm-window-0844220E`, built from the same source as `031a103`.

## Deviations and harness changes
1. The counter is named `change_multi_view_families`, because all stage counters carry the `change_` prefix. With evidence off there is no key at all.
2. F7 also keeps unsupported frames out of `late_results`, not just `duplicate_completion`.
3. The oracle's exit 1 also covers a mismatch in a row's reference comparison, not only in the row itself.
4. Bench gates 8, 2 and 15 read differently under the new rule; this was predicted in writing before the build. Gate 8 now loses SI 8's normal picture on purpose, since it's a fixture-only fault.
5. Chat's scorecard file changed at 03:07 IST, after the last baseline. I re-baselined it as an external update.
6. The idle check now sorts processes by engine and folder. I added `dotnet.exe` mid-round after noticing our own UnrealBuildTool runs under that name; this had no effect on the build.

## Boundary proof
`_reviews/081-21-evidence/postflight.json`: **"PASS: boundary restored (main m51 53bf725, Lyra caa68c6 with original 9 modules; main Game/Editor and staged exe are build 3 by design; every other binary, artifact and bank file byte-identical to preflight)"**, `problems: []`.
- Remote feature is at `94a56a9`, master at `031a103`.
- CaptureBench and `ToCodex\` are untouched.
- The foreign processes were recorded and never touched.

## NEEDS-DECISION items
None blocking. One point for chat:
- **Rebuilding exactly the pre-m55 behaviour also brings back an old pre-m55 bug that build 2 had been hiding.** If a second view that isn't the capture viewport draws after it in the same frame, it overwrites that frame's key. The model shows that frame's normal picture is then lost, which build 2 prevented.
- It can't happen on the single-viewport bench. In PIE it needs a non-realtime editor viewport to redraw.
- I implemented it as ruled and recorded it for the m51 pairing work, next to the multi-view issue.

## Wall-clock (IST)
| Step | Time |
|---|---|
| Read ruling, review and code; design; preflight | 03:42–03:58 |
| Predictions pushed | 03:58 |
| Implementation, oracle, Python suites, model proofs, banked rerun | 03:58–04:14 |
| One build set (3 targets) | 04:14:49–04:22:52, 8 min |
| Stage, A44, Lyra restore, replay | 04:23–04:24 |
| Docs, journal, push (partly overlapping the build) | ~04:05–04:24 |
| Postflight | 04:25–04:26 |

About 45 minutes in total. There was no waiting on foreign processes, because the legs were skipped.
api-equivalent cost (not billed on Max): $25.25 | turns: 176 | running total across all briefs: $2,096.46
