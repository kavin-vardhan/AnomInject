# 095-02 — Chat ruling: both plans ACCEPTED; answers to Code's questions; build order

**Date:** 2026-11-13, about 13:15 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- Mailbox report 095-02: GREEN, 26 min, $47.00 API-equivalent, context 71 % (roll the session).
- `_reviews\095-02-plan-A1-capture-continuity.md` (m56) and `_reviews\095-02-plan-C-terrain-blink-subtype.md` (m58), both read in full by chat.
- Read-only; nothing built or changed.

## Verdict
**Both plans are accepted as the build specifications,** with the changes below. Where this ruling and a plan differ, the ruling wins.

## The headline finding (A5)
- A banked home session shows the gap directly: `frame_index` jumps by 3 at every onset and offset, and `ticks_per_captured_frame` reads 1.344.
- At default settings, onset frames sit at session index ≡ 3 (mod 12), and **675 = 56 × 12 + 3**. So the client's "first corrupted frame at 675, jump between 675 and 676" is exactly where an onset frame is followed by the two skipped ticks, if that session ran defaults and counts from 0.
- **Still not claimed as the cause.** One office read decides it (§ Owner reads): a `frame_index` step of 3 at 675→676 confirms the gap; a step of 1 points to a wall-clock hitch instead.

## Unit A-1 (m56) — answers
- **Q1 (office read of rows 674–677): yes.** Added to the owner's read.
- **Q2: redefine `ticks_per_captured_frame`** as span ticks / frames written, and add `capture_span_ticks`. `capture_game_ticks` keeps its meaning, so the old ratio stays derivable. Changelog it.
- **Q3: yes,** every settle frame carries the row-level flag.
- **Q4: remove and count.** A frame that failed to write leaves `annotation.json`'s frame lists and is counted. Its file number stays missing; nothing is renumbered.
- **Q5: the writer cap goes in A-1.** One change: **bound it by pending pixel bytes, not only by job count** (proposed 1 GiB, with a small count floor), because a 6 MP host frame is twenty times a bench frame. Echo the bound in the run banner.
- **Q6: agreed, as a later dashboard brief.** The encode step and the dashboard report gaps and never repeat or truncate silently. **Consequence for A-1's gates:** drop "the encode step warns" from can-fail G4(b); record today's behaviour as a reading instead.
- **Q7: agreed.** The client is not advised to use settle 0 in the meantime.
- **Q8: agreed.** C does not bump the schema minor; A-1 bumps it to 2.

## Unit A-1 — further rulings
- **A-R1. The settle-exactness table (plan §8, G6) is a required reading of the gate campaign,** not optional. It is still not a pass/fail gate.
  - **Why it matters:** with the flag on, a cycle is 16 frames of which 4 are dropped by the client's rule, and a frame-limited capture holds about a quarter fewer events. If settle frames prove exact at AA-off, setting K to 0 gives 12 continuous frames per cycle with nothing skipped and nothing flagged: today's event density, without the gaps. That is the end state chat is aiming for.
  - Chat rules on the K default right after the table lands. The plan's caveat stands: K = 0 shrinks `missing_object`'s post-revert veto window to exactly its budget, so that ruling re-checks m26 first.
- **A-R2. The independent review runs after implementation and before the cook and gates,** as a fresh read-only side session. Fixing a finding is cheaper before the bench hours are spent.
- **A-R3. CaptureBench changes are allowed in the gate brief** (leg predictions, the oracle's flank rule, the two new check scripts and the comparator), committed in that repo.
- **A-R4. Before any client drop that contains A-1:** the dashboard brief of Q6 and a re-bundle, so the client's copy of the check kit knows `settle` and schema 2.2. Without it their kit refuses every new session.
- **A-R5.** The `t_wall` documentation fix, the two bench drop levers (bench-gated, compiled out of Shipping) and the commit breakdown are accepted as planned.

## Unit C (m58) — answers
- **Q1: option (b).** Ship the subtype **and** refuse `blinking` on engine-landscape targets (`terrain_unlabelable`, counted), because such an event has no mask and a wrong box today. The auto pool can never pick engine landscape anyway, so this only closes the targeted-fire path. Option (c), real landscape masks and boxes, becomes a later unit **only if** the office read shows the client's terrain is engine landscape.
- **Q2 (office yes/no read): yes.** Added to the owner's read.
- **Q3: default patterns stay empty.** A host's terrain pattern is set in that host's ini **at the office, at integration**, and is never written at home (codename and asset-name discipline). The office card for m58 says how to find it from a terrain event's own label fields.
- **Q4: `missing_object` on terrain is not in m58.** Noted for later.
- **Q5: an actor is terrain if the actor or any of its components matches a rule.** The rule text says which.
- **Q6: no schema bump for C.**
- **C-R1. Parked, not built:** an automatic "ground under the viewer" rule that needs no per-host pattern. Chat revisits it after the office read says what the client's terrain actually is.
- **C-R2. New gotcha, as the plan proposes:** non-Nanite runtime grass reports zero instances while it draws, so it fails the empty-instance check. This likely explains the old "zero-instance grass" note.

## Order of work (one brief in flight, one bench)
1. **095-03 (running):** free E:, rebuild, baseline legs.
2. **095-04 (xhigh):** implement m58 on its branch; both targets build clean; selftest; the PIE gates. Staged gates wait for the shared cook.
3. **095-05 (xhigh):** implement m56 commits 1 to 5 on its branch; both targets build clean; offline tests.
4. **095-06 (xhigh, side session):** independent read-only review of both branches.
5. **095-07 (high):** one cook, then the staged and remaining gates for both units, including the settle-exactness table.
6. Chat rules on the K default; docs; merge to `master`; dashboard brief and re-bundle; the 4.25 forward-port; one office pull.

Each implementation brief starts a fresh default session from the plan files. The planning session is at 71 % and is retired.

## Owner reads (office, numbers only; nothing leaves the host)
1. Which host and which build made the reviewed session.
2. In its `labels.jsonl`, the `frame_index` and `t_wall` of the four rows with `session_index` 674, 675, 676, 677.
3. Its `run_summary.json` `ticks_per_captured_frame`.
4. For one terrain blink event in it: is `nodes[0].component_class` a landscape or grass class, or a mesh class? Yes/no only, no asset names.
5. Whether the review document has more than the three-topic table.
