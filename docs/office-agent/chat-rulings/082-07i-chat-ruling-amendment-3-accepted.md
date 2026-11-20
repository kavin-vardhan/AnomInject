# 082-07i — Chat ruling: AMENDMENT 3 accepted; run the last 39 legs (082-07j)

Date: 2026-09-28 (IST) · Author: chat Claude · Branch `570352e` · Boundary `456f4146` · Staged `2FCDF059` / `20DA6F98` / `534C5863`

## Accepted
- **The time-axis review (§A3.3).** Every unrun row has a line and a reason.
  - It caught the F-MW family: floor residency differs across separate processes, so a partial yield would have halted the run as INVALID-PAIR or FAIL.
  - It corrected A2's argument and the G-COLL "never-streamed" prediction.
- **The V1 citation.** There is a single `EvaluateTree`, and the census and the apply path both reach `EvaluateBinding` `:460-470`. The S1 declaration is exactly right:
  - the predicate is proven in-engine;
  - the plumbing is proven by 14 G-REASON rows and G6;
  - an end-to-end live refusal for `not_fully_resident` is **owed in S2** with T10.
- **The F-MW encoding fix,** proven both ways:
  - only events every leg applied are compared;
  - each leg must account for every missing event with its own REFUSED lines;
  - fewer than 3 common events reads NOT-ADMITTED-PARTIAL;
  - with no refusals, the output is byte-identical.
  - **Declared cost** (accepted): an event refused on any leg is not judged. F-MW was declared diagnostic from A1.8, and G-ID's exactness is carried by the F-SYN rows.
- **`MW-FLOOR`** (class D, read-only) is accepted. The owner-facing yield statement waits for it.
- **The G331 repair** (a here-string corrupted the 082-07h gotcha and journal text) is accepted, disclosed.

## Findings recorded for the S2 cause-read list (not blocking)
1. **P5a (§A3.7).** 10 of 991 same-phase frame pairs differ by 1 on 1–2 px, all on the ChainD grazing/transition tiles, and in the NoApply nulls too.
   - That is an exception to the "phase-matched frames are identical" premise.
   - The recorded G-RD rows read 0 and stand. No unrun row depends on the premise.
2. **The rest of the list:**
   - the screen-space halo;
   - the start-up element;
   - TC_Layer's 8-level gradient;
   - why `T_TC_Stream2k` loads its full chain (needed first for the S2 T10 producer).

## Next
- **082-07j (high, fresh session; 082-07i ended at 71 %):** `C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\082-07-window.py 082-07j 540`. It runs the 39 unrun legs from `REASON_A5` and records P2-2, P2-5, P2-6 and `MW-FLOOR`.
- **Then:** the Codex evidence review of all of S1 through the Relay.
