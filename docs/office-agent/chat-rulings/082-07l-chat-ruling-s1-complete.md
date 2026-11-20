# 082-07l — Chat ruling: m53 S1 COMPLETE. P2-1 and P2-3 confirmed, scoped; the MainWorld yield answer; Codex evidence review next

Date: 2026-09-28 (IST) · Author: chat Claude · Branch `2ca9faf` · Staged `2FCDF059` / `20DA6F98` / `534C5863`

## S1 result (193/193 legs accepted, no halt)
- **Identity is exact:**
  - G-ID is 14/14 at 0, including a masked tile that really clips;
  - G-ID-M is 29/29, all ≤ 1;
  - G-RD and G-BIND pass.
- **Nothing is left behind:** G3 is 44/44, and G3-COUNT covers 267 uv and 35 normal events.
- **Refusals:** G-REASON Q is 19/19, with every expected final reason produced.
- **Lifecycle:** G4 is 26/26, including destroy, window-close end of play, frame cap and stop-before-fire.
  - **P2-5:** 12/12 FailStep rollbacks read `live=0 pending=0 balanced`.
  - **P2-6:** the watches and `EndPlay` are CONFIRMED.
  - **P2-2:** the collateral set is CONFIRMED.
- **Integrity:** TRIP and G11 pass 193/193.
- **Declared dispositions, owed to S2:**
  - T10 (a live `not_fully_resident` refusal);
  - A2 (`no_normal_map` as a stop row);
  - G4 LEVEL-CHANGE and RECREATED;
  - cancel-before-focus.

## Rulings
1. **P2-1 and P2-3: CONFIRMED, scoped to rows that carry the texcorrupt block.**
   - The proofs are about that block's contents: the NoApply predicate and the subtype per event. Outside the three destroy legs they are clean (10,808 readings and 1,183 events, with 0 problems). On those legs, the one row that carries the block is correct.
   - The rows after `TARGET LOST` carry no block by design (`GetTelemetry` returns false once the anomaly is inactive, `Anomaly_TexCorrupt.cpp:1248`).
   - Record the scoping as a dated note in the predictions at the S1 close-out commit.
2. **Finding for the Codex review and m51/M3: labels after `TARGET LOST`.** Rows si 16–22 keep `anomaly_present: true` after the target was destroyed, until the burst's own revert.
   - **If those rows carry `observable: false`** (m49), the label is honest: the fire is live but nothing can be seen.
   - **If they don't,** it is a **capture-layer** label item for m51/M3, not an m53 defect. The client's `annotation.json` is correct.
3. **The MainWorld yield answer.** "Zero yield" was wrong.
   - Once its textures load, the StackOBot floor passes every eligibility check. Its full map set, 30 slots at 209 MiB, exceeds the 128 MiB budget cap, so it is refused `over_budget`.
   - The first fire, at frame 42, applies to the 12 qualified slots only.
   - **The budget default is the owner's product call at S3,** with G7's cost numbers at 64/128/256 MiB.
   - **Partial-slot application** (12 of 30) is a question for the Codex review and S3 G-STR: is it visible enough, and does the label say which slots?
4. **BAND F-MW INVALID-ARBITER.** The MainWorld control pair isn't pixel-deterministic (whole-frame max 254) even with the pinned camera path.
   - Any MainWorld pixel gate needs a masked arbiter. That is an S3 design item.

## Next
- **082-08 (medium, continue the 082-07l session):** the Codex evidence review of all of S1 through the Relay (Astra/max). The request is `_reviews\082-08-codex-m53-s1-evidence-review-request.md`.
  - Code launches the review, collects it, and **does not act on it**. Chat rules.
- **Then:** the S1 close-out docs, then S2 planning, which folds in every S2 debt listed above plus the cause-read list (the halo, the start-up element, the TC_Layer gradient, P5a, the Stream2k full chain, the 51.7 s input).
