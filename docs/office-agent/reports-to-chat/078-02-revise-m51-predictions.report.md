# REPORT for 078-02-revise-m51-predictions.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 078-02-revise-m51-predictions.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-06 15:27:01 | finished: 2026-09-06 15:41:47 | duration: 15 min | exit code: 0
context at last turn: 744143 tokens (74% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\078-02-revise-m51-predictions.live.txt

---- FINAL MESSAGE (Claude Code) ----
All verified. Final report:

## 1. Rundown

Both reviews are accepted in full — all eleven Codex objections and all four Chat blockers — and the corrected specification is **AMENDMENT 1**, appended to the predictions file as the active spec with §§0–8 marked superseded. I re-read every source line the rulings depend on rather than adopting either reviewer's assertion, and **two reviewer claims turned out to be wrong at source**: Chat's A-side prediction for the R8 leg (master has no `PendingSnapshots` cap — it is the branch's addition, so `B8AC65D8` cannot fail by filling it), and Chat's demand that `exposure_dip_scope` appear on every labelled row (it is emitted only inside `if (bExposureDip)`, so the demand is unsatisfiable without the schema change R1 forbids). Fork F-b is resolved **DELETE** — my re-pointed veto was rejected by both reviewers on the same correct ground, and my counter-argument described the m26-era baseline that m49 deliberately changed. Four of my own 078-01 claims are corrected, including the F1 severity claim: the delivery recipe never sets `OutputHeight`, so F1 is configuration-dependent, not reachable by shipped defaults.

## 2. Disposition of every review item

| item | disposition |
|---|---|
| Codex 1 / Chat 1 — observable predicate | **ACCEPTED.** Verified `AnomalyCaptureSubsystem.cpp:3872-3880`: `bLabelled && bHeld && Px >= ObservableMinPixels` after the unmeasured early-out; DrawnCount at `:3887` only increments a counter. Rows F–J are now one predicate; K is orthogonal. |
| Codex 2 / Chat 1 — reject F-b | **ACCEPTED, veto DELETED.** `vetoed_events` = 0 compatibility key + changelog line; new `zero_observable_events` excluding fully-unmeasured events; per-leg P-C7 delta declared. |
| Codex 3 / Chat 3 — stale arm ≠ ForceMiss | **ACCEPTED.** `WithholdWorldRender` specified against verified `GameViewportClient.cpp:1696-1707`; ForceMiss kept as `key_misses`; **A-side prediction corrected**; expiry sweep specified because `ConsumeWantedForPublish` is FIFO with no expiry; `unrendered_ticks == 40` explicitly **not** predicted as a bare constant. |
| Codex 4 / Chat 4 — wrong F9 lever | **ACCEPTED.** Verified omit-depth omits the *depth-pass half*; F9 uses an `r.CustomDepth 0` window with read-back verification and a VOID rule. |
| Codex 5 / Chat 5 — F2 | **ACCEPTED.** `ReadbackDelayTicks` (not `ViewLag`), `BENCH_GEOM` sidecar, independent oracle forbidden from calling the implementation helper, declared rounding/tolerance, conditional cross-run equality, both fixtures via runtime-spawned actors. |
| Codex 6 / Chat 6 — writer + load | **ACCEPTED.** 4 B/px vs 8 B/px corrected; 1.1× cap rejected as a policy-not-bound; reservation accounting with a *measured* factor and honest overrun reporting; 600/60 leg; **numeric 1.05 band** with the m33 prose conflict resolved; "3× in flight" withdrawn. |
| Codex 7 / Chat 7 — completeness | **ACCEPTED.** F-a removed as a fork; reconciliation replaces the four-counter iff; M-F16 now expects FAIL; publication order removes the circularity. |
| Codex 8 / Chat 8 — write boundaries | **ACCEPTED.** M-W1..W5 + encode-vs-OS-write split; orphan policy declared (retain + auditor must detect). |
| Codex 9 / Chat 9 — lifecycle | **ACCEPTED.** GT/RT table corrected; per-request terminal flag; lease released on fire-end **and** arms-resolved; `NoTagRetire`; eviction cancels explicitly; stereo declined. |
| Codex 10 / Chat 10 — matrix/provenance | **ACCEPTED.** 24-row matrix, both fixtures per row, script paths, per-leg P-C7 allowlists, full hashes. |
| Codex 11 / Chat 11 — provenance | **ACCEPTED.** Merge-vs-endpoint-diff corrected; cook fence narrowed to the port. |
| Chat blocker 1 — M-F4 editor route | **ACCEPTED with correction** — a packaged exe cannot host PIE, so editor A-side provenance is separate or an in-candidate falsifier is used. F-c is not a waiver. |
| Chat blocker 2 — exposure | **ACCEPTED with correction** (see §3). |
| Chat blocker 3 — row B `bSveCapture`, row N2 | **ACCEPTED.** Verified branch `:2901` adds it. |
| Chat blocker 4 — auditor exit 3 | **ACCEPTED.** Four exits, 2 and 3 distinct; `verify_capture.py` retained. |

## 3. Where a reviewer was corrected, at source

- **R8 A-side.** `ac13700` has **no `PendingSnapshots` admission cap** — only the drain bound `Iter < 8` (`:4040`) and the finalize sweep (`:4082-4086`); `if (PendingSnapshots.Num() >= 8)` is the **branch's** line. `B8AC65D8`'s real failure is FIFO-with-no-expiry: on resumption each family consumes the **oldest** arm ⇒ mass mis-pairing then bulk drop — journal 049 §8's banked hazard. The cap/cancellation regression therefore gets an **in-candidate** falsifier, since a console lever absent from an archived exe is not a device.
- **Exposure.** `AnomalyLabelWriter.cpp:156-162` emits both fields only inside `if (bExposureDip)`. Conditional emission preserved; the intent is met by suppression diagnostics — `frames_exposure_dip_suppressed` must be **> 0** on a leg with a known hide-induced whole-frame drop, which proves the excluded operand actively suppressed a false dip (a stronger claim than the field's presence).

## 4. Commit and state

**`adb0d4d12f3b751bece396499bedd50d00fdeb76`** on `m51` — `docs(m51): 078-02 AMENDMENT 1 …`, **1,053 insertions / 30 deletions, documentation only** (verified: nothing staged outside `docs/` and `CLAUDE.md`).

- `docs/predictions/2026-09-06-m51-release-hardening.md` — **SHA-256 `725CD30FF5A6F55BB6B96990BF194319DA4D665471F8DBBC8921ED119D55B50B`** (was `4A676AD2…`)
- `docs/sessions/2026-09-06-078-02-m51-predictions-revision.md` (new) — `8B1B686F800E5084B8750EE3AC6A80E7A4E75AF0FE1206D83D1B75586E620061`
- `docs/sessions/2026-09-06-078-m51-preflight.md` — §9 CORRECTIONS appended, body unchanged
- `docs/OPERATING-CONTRACT.md` — dated §9 rulings appended, body unchanged
- `CLAUDE.md` — status corrected in place

`m51` **not pushed**. `master` and `fix/capture-label-integrity` unmoved and equal to origin. `git diff ac13700..HEAD -- Source Shaders tools` **empty**. Staged exe `B8AC65D8` and the five container files re-hashed identical; Lyra worktree `4a4bfcc`, clean; ten owner handoffs untracked. **No implementation, lever, test, build, cook, leg, staging, worktree/branch change, merge, tag, push, mailbox write or office action.**

## 5. NEEDS-DECISION

**Chat's written review of the corrected exact snapshot** (`m51` = `adb0d4d`, predictions SHA-256 `725CD30F…`).

Remaining forks and prerequisites, with preferred resolutions:

- **`EncoderReserveFactor` has no default yet** — deliberately. Preferred: set it from the measured max `encoded/raw` ratio on the throughput calibration leg and amend the predictions with that number **before** the gate legs. Recording a guessed default now would be the thing the writer section exists to avoid.
- **M-F4 editor A-side provenance is a genuine prerequisite** — a packaged exe cannot host PIE, and no editor-target baseline binary is archived. Preferred: run M-F4 as an in-candidate pair (healthy vs `NoViewBinding`) on one editor build per fixture and record the module set at freeze; do not manufacture an editor A-side.
- **`WithholdWorldRender` and the other six devices do not exist yet** — they are implementation-commit work, and the R8 leg cannot be scheduled until commit 4 lands.

VERDICT: NEEDS-DECISION
api-equivalent cost (not billed on Max): $15.12 | turns: 37 | running total across all briefs: $1,109.47
