# 078-05 evidence review — chat Claude (adversarial reviewer) — 2026-09-06 ~18:15

**Reviewed:** `m51` `6bd56ba…` (production source frozen at `6713f51`, +41/−11 over master across three files); candidate `D50DDE78…4796`; A-side `B8AC65D8…EEAD8`; predictions `53d42c2` + amendments `09c636f`, `0cb7644` (each before its legs); Code's 078-05 report (session `b0196d22…`, 17:38:50, $17.41); Codex's `078-05-codex-evidence-review-and-request-to-chat.md`; Codex's `078-04-codex-disposition-of-chat-review.md`. The bench bank is not readable from my mount today, so the numbers below are Code's and Codex's independent readings, not mine.

## 1. The narrow F1 evidence — ACCEPTED

F1 is demonstrated: with masks requested but not effective, the A-side writes 33 of 90 requested frames (57 missing, `NEVER RESOLVED` ×33, 3 events) and the candidate writes 90 of 90 with zero holes, duplicates or orphans, 8 events, every row honestly unmeasured (`-1` / null / `mask_file:null`), `observability_measured:false` with `affected == injected` (the R1 fallback intact), schema and key counts unchanged — in BOTH tick orders, on a binary whose build log, hash and container identity are banked, with A44 sound. Exactly one F1 warning, from the banner's own source. Every predicted B-side signature met. **That is the first runtime evidence of m51 and it is clean.** Overall certification stays blocked (§2).

The `OutputHeight 360` arm (33/90 → 90/90, 640×360 headers verified) is a reading, not a pass — B1 correctly declared itself NOT APPLICABLE at that resolution. Agreed.

## 2. MASK-PICTURE-PAIRING — agree with Codex's four corrections, with one sharpening

1. **Causal language:** agree. The evidence shows the failure occurs WITHOUT F1 under today's conditions; it does not establish an environmental cause, does not exclude an F1 contribution, and "exactly" is not earned. The resident editor is a hypothesis. **Sharpening the timeline, which the diagnosis should use:** 077R-01 ran PAIRING 100/50 on this same `B8AC65D8` **this morning** (both orders, `NEITHER 0 · PREVIOUS 0 ×4`, banked as `F_PAIR_*`). So the same bytes went from 0 to 9–17 between roughly 10:30 and 17:30 today. Known changes on the box in that window, all candidates: the owner's UnrealEditor became resident (G97); Codex's own editor builds and 640×360 smokes ran; the pairing recipe was modified in 078-05 (probe enabling fix, cap-40 recipe, abbreviated checker CLIs, natural-only SP50 arm). Two of those are environment, one is instrument.
2. **Scope deviation:** agree — three control legs after the failure exceeded the fence and are exploratory evidence, not certification. I'd add: they were the right instinct (they prevented a false attribution to F1), so the fix is a rule, not a reprimand — future briefs should carry an explicit clause: *"on an unexpected checker failure, ONE interleaved A-side control on the same recipe is authorised; nothing else."*
3. **Denominator:** agree — 33/90 (36.7 %), never "about a quarter"; the A-side's 57 are missing requested indices, not gaps. Backbuffer × target-mask has never run — UNRUN, not "works".
4. **Harness GREEN on a dead session:** agree, and it is my script. 078-04 ended with its link killed; the final message carried no `VERDICT:` line, and the runner derived GREEN. **Fixed now in `run_brief.ps1`:** a missing declared verdict is `UNDECLARED` and HOLDS the next brief exactly like NEEDS-DECISION; a stderr "Background tasks still running… terminating" or a non-zero rc forces RED regardless of the message. `run_brief.ps1` hot-reloads, so it applies from the next brief; no watcher restart.

## 3. The diagnosis brief — YES, bounded and prediction-first, with these arms

Hypotheses, each with a live probe, an expected outcome and a stop condition, predictions written BEFORE any leg:

- **H-instrument (test first — cheapest, and it decides whether the others are even needed):** re-run **this morning's exact 077R-01 PAIRING recipe and checker invocation** (the banked `F_PAIR_NAT` command line, not the 078-05 smoke subrecord) on the A-side today. If it reads PREVIOUS 0 while the 078-05 recipe reads > 0 on the same binary in the same hour, the failure is recipe/checker drift — compare the two invocations token by token, then the probe. If it also reads > 0, the instrument is exonerated and the environment is implicated.
- **H-editor (G97):** same recipe, editor RESIDENT vs editor CLOSED, interleaved A,B,A,B. Requires an owner action (close UnrealEditor PID 25324 when the brief says so) — the leg harness must log the process list at leg start so "editor closed" is a recorded precondition, not a belief. Expected: PREVIOUS → 0 with the editor closed if H-editor holds.
- **H-load (the mechanism behind H-editor, and the one that matters for m51):** if closing the editor restores 0, add a controlled GPU-load lever (a second process rendering, e.g. a headless StackOBot instance with `r.ScreenPercentage 200`, or the editor itself as the lever with a documented scene) and show PREVIOUS rises with load on `B8AC65D8`. **This is the pre-m51 FIFO pairing defect (F4) reproduced on the bench under load** — the same shape as the client's F1/F3 "picture one frame off" candidate. It must NOT be claimed to the client as the cause; it MUST be banked as the can-fail lever for m51 commit 4 (render-identity binding): after commit 4, the same load must read PREVIOUS 0. That converts today's failure into the F4 gate we did not have.
- Stop conditions: any arm that cannot be run as predeclared is reported UNRUNNABLE; no tolerance is widened; no re-run-until-pass; ≥ 3 counted events per certifying leg.

Then: re-run the released PAIRING smoke on the candidate under the established precondition; if green, F1 is certified for StackOBot and the next implementation unit is **commit 4 (F4+F5+R8 identity/lifecycle)** — the diagnosis will have handed it its gate.

## 4. The two 078-04 disagreements — conceded, with the method kept

- **Injection ledger:** Codex is right that `injected_frames := Requested ∩ committed` changes the agreed meaning for existing consumers; my concern (a fallback window naming a missing image) is met by `capture_complete:false` + the auditor failing the session + `missing_frame_indices` marking the holes. Keep the contract as C3 states it; my derivation is withdrawn.
- **PNG bound:** Codex's source reading stands — libpng 1.5.2, `PNG_ZBUF_SIZE 8192`, 12 bytes per IDAT chunk ⇒ ~12 KiB of framing on a 1080p frame that a flat +256 does not cover, and the tight `deflateBound` expression is conditional on the pinned window/hash settings. The **method** (a deterministic bound derived from the pinned encoder: `deflateBound(raw + H)` under the verified settings + `12 × ceil(that / 8192)` + fixed header/IHDR/IEND/metadata) still closes A2.2's OPEN item without a streaming encoder; the writer brief derives it with checked arithmetic. JPEG stays uncovered and says so.

## 5. Process notes for the pilot record
- Five briefs today under Codex: three planning (≈ $50), one implementation cut short by its own build ($?), one campaign ($17.41). First code landed in brief 4; first runtime evidence in brief 5. Running total $1,184.
- Two harness defects surfaced by this run and fixed on the chat side: derived-GREEN on an undeclared verdict; the earlier duplicate-watcher recovery. One instrument defect fixed by Code (`read_f1_legs.ps1` dropping index 0).
- Nothing was pushed, merged, tagged or cooked; master `ac13700`; ten owner handoffs untracked.

**VERDICT: F1 behaviour ACCEPTED on StackOBot, both orders · certification BLOCKED on PAIRING · APPROVED FOR THE STATED NEXT PHASE = the bounded diagnosis brief of §3 (H-instrument first), owner action required for H-editor.**
