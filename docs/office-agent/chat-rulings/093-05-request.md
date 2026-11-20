093-05 — FINISH J1 on fix/followup-capture. From chat Claude (the orchestrator), 2026-10-14. Binding ruling: _reviews\093-04-ruling.md (also 093-03 and 093-02 P1/P2/P10). This resumes your own J1 session.

ACCEPTED SO FAR (ruling D1): the corrected F6 in PIE, the docs fix (G477), the reader wording and the capture-size note.

STOP RULE, CLARIFIED (ruling D2)
- NOT a stop condition: a failure in your own harness (your scripts, cook or stage arguments, output paths, helper code).
  - Fix it within the job and disclose what failed, the fix, and the evidence.
  - At most 2 fix attempts per harness problem.
  - Never change a gate, threshold or prediction to get past it.
- STOP only for:
  - a PRODUCT gate failing its prediction;
  - any needed change to product Source, Shaders or tools;
  - the idle gate not passing within 4 hours;
  - a security block (for example dbghelp.dll, G418): report exactly what failed, stop that item, never work around it;
  - a disk floor.
- Partial outcomes (ruling D4): if a product gate fails, report it precisely and continue with the gates that don't depend on it. Do not push.

DO, IN THIS ORDER
1. Every remaining PIE gate first (none of them needs a cook). Use your unchanged predictions; each gate has its can-fail:
   - F1 owner guard, shared-proxy mode;
   - F2: C1/C2 scope and tag retirement;
   - F3: the opaque/masked alpha pair;
   - dropped-mask accounting, with deliberate drops;
   - m52 blurry hold;
   - shared proxy blur: co-user unchanged, measured masks, recovery;
   - normal and co-user regression;
   - the label-pixel gate's capture-specific can-fails: ±1 shift, missing/blank/wrong-count mask, erased marker.
2. Bench cook.
   - Fix the output path: a -CookOutputDir ending in \Windows, or omit the flag and use the default layout.
   - Cook and package; record the container hashes (.pak, .utoc, .ucas).
   - Restore staging and the host by receipt afterwards.
3. Every staged gate, including staged F6, each with its can-fail.
4. Only when every gate is green: push fix/followup-capture (fast-forward) with the docs. Not before.

FINAL REPORT (report.md and your final message): the same structure as before, plus a short list of every harness problem you fixed and how. End with its own line: VERDICT: GREEN | NEEDS-DECISION | RED.
