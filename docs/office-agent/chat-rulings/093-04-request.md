093-04 — CONTINUE J1 on fix/followup-capture to qualification. From chat Claude (the orchestrator), 2026-10-13 night. Binding ruling: _reviews\093-03-ruling.md (also 093-02 P1/P2/P10). This resumes your own J1 session; your harness, predictions and evidence under E:\IA_BuildCache\_r933 stay valid.

YOUR STOP WAS CORRECT UNDER THE STOP RULE, BUT THE DIAGNOSIS WAS WRONG (Claude Code's review, _reviews\093-03-code-review.md, row 11)
- Both the old and the new checker run the per-row self-consistency test (event_id == id@start_frame|target_name) on EVERY row, before the transition filter. Your mutant changed only event_id, so both rejected it.
- The intended F6 control is a SELF-CONSISTENT wrong event:
  - change start_frame, target_name and event_id together (for example 999999 / WrongTarget / the matching event_id), on a row with no transition;
  - frozen expectation: the old checker misses it ([]); the new checker rejects it ("labels event missing from annotation").
  Code reproduced this on scratch copies (_relay\runs\2026-10-13-093-03-j1-natural-base\code-receipts\f6diag\). No transition-free capture is needed.
- Keep your event_id-only mutant as a second, documented control: the self-consistency test catches it in both versions.

DO, IN THIS ORDER
1. F6: run the corrected control on the existing F1_P_uv_aaoff PIE capture now. Run it later on a staged capture too.
2. Docs (before any push):
   - correct the F6 explanation in the gotcha, the CLAUDE.md status block and the journal;
   - renumber the gotcha from G465 to the next free number across ALL refs (G477 at review time; re-check at commit).
3. Wording: the existing verify_capture.py --label-pixel-gate exits 0 and reports UNASSESSABLE on the wrong-view release control. The rejection comes from your marker and mask checks. Say exactly that in the report and the docs. Do not change the reader in this job.
4. Bench cook: cook a bench container with the fixtures the remaining gates need (C1/C2 scope, the alpha pair, the hold maps). Use the existing staging-and-receipt routine, and restore staging by receipt afterwards.
5. Complete the J1 matrix from your predictions file (unchanged; no re-tuning). Each gate with its can-fail, in PIE AND staged:
   - F1 owner guard: the shared-proxy mode (the UV multi-viewport mode already passed in PIE), plus staged;
   - F2: C1/C2 scope and tag retirement;
   - F3: the opaque/masked alpha pair;
   - F6 (corrected);
   - dropped-mask accounting, with deliberate drops;
   - m52 blurry hold;
   - shared proxy blur: unchanged co-user, measured masks, recovery;
   - UV/normal regression (normal and co-user are still unrun);
   - the label-pixel gate: 0 frames off, with measured masks.
6. Capture size: explain in one line why the PNGs were 1123x870 while the viewport metadata read 1280x992 (from a 1280x1024 request). Give the measured cause, or say "unknown". This is not a gate.
7. Archive and source:
   - No source change is allowed in this continuation; E41ABF93 stays the candidate.
   - Record the cooked container's hash separately.
   - If ANY source change turns out to be needed, stop and report NEEDS-DECISION.
8. Only when every gate is green: push fix/followup-capture (fast-forward), with the corrected docs. Not before.

STOP CONDITIONS
- An unexpected gate failure: stop and report.
- The idle gate does not pass within 4 hours: report.
- Never weaken or re-tune a gate.

FINAL REPORT (report.md and your final message): the same structure as before, plus:
- the corrected F6 result in PIE and staged;
- the cook receipt;
- the capture-size note.
End with its own line: VERDICT: GREEN | NEEDS-DECISION | RED.
