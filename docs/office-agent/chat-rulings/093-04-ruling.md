# 093-04 — Chat ruling: the J1 continuation's fixes are ACCEPTED; the stop rule is clarified; finish J1 in 093-05

**Date:** 2026-10-14, about 00:20 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- Codex's resumed J1 session: run `2026-10-13-093-03-j1-natural-base`, attempt 2, 1,508 s, NEEDS-DECISION.
- Claude Code's review: `_reviews\093-04-code-review.md` (16 OK · 11 GAP).
- Mailbox report 093-04: NEEDS-DECISION, 37 min, $8.79 API-equivalent.

## Rulings

### D1. Accepted
Code verified all of these independently:
- the **corrected F6** control in PIE: the old checker misses, the new checker and the CLI reject, and the event-ID-only control is caught by both;
- the **docs fix**: the F6 cause is corrected everywhere, and the gotcha is renumbered G477;
- the **reader wording**;
- the capture-size note ("unknown");
- no source change;
- predictions unchanged;
- archives, staging, build host and refs intact.

### D2. The stop rule, clarified for every implementer job on this track
**Not a stop condition: a failure in the implementer's own harness.** That means its own scripts, cook/stage arguments, output paths and helper code.
- Fix it within the job and disclose it: what failed, the fix, and the evidence.
- At most **two** fix attempts per harness problem.
- Never change a gate, a threshold or a prediction to get past it.

**Stop conditions stay as they were:**
- a **product** gate failing its prediction;
- **any** needed change to product `Source`, `Shaders` or `tools`;
- the idle gate not passing within 4 h;
- a security block, for example `dbghelp.dll` (G418): report exactly what failed, stop that item, and never work around it;
- a disk floor.

**Why:** in 093-04 the cook failed on the harness's own `-CookOutputDir`, which did not end in `Windows`. Stopping then left every remaining gate unrun after 25 minutes of a 14-hour budget. Code's diagnosis matches the engine source (UAT staging appends the platform folder).

### D3. Order for 093-05
1. **Every remaining PIE gate first** (none needs a cook), each with its can-fail, from the unchanged predictions:
   - F1 shared-proxy mode;
   - F2 C1/C2 scope and tag retirement;
   - the F3 alpha pair;
   - dropped-mask accounting with deliberate drops;
   - the m52 hold;
   - shared-proxy blur;
   - normal and co-user;
   - the label-pixel gate's capture-specific can-fails (±1 shift, missing/blank/wrong-count mask, erased marker).
2. **The bench cook**, with the output path fixed: a `-CookOutputDir` ending in `\Windows`, or the flag omitted. Record the `.pak`, `.utoc` and `.ucas` hashes. Restore staging and the host by receipt.
3. **Every staged gate**, including staged F6, each with its can-fail.
4. **Push `fix/followup-capture`** (fast-forward) only when every gate is green.

### D4. Partial outcomes
- If some product gates fail, report each one precisely, keep going with the gates that don't depend on it, and don't push.
- If `dbghelp.dll` blocks the cook, the staged gates stay blocked. Report NEEDS-DECISION; the owner may need to look at Windows Security.

## Next
- 093-05 is released: Codex resumes J1 again, and Code reviews.
- Then J2 (the office-robust F1) on the qualified branch.
