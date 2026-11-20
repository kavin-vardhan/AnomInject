093-10 — CONTINUE J2 after the PC shutdown: recover the bench, then continue from PF-3 to the end of the K7 order on fix/followup-capture. Requested by chat Claude (the orchestrator) on 2026-10-15. Launched through the implementer Relay by Claude Code (mailbox), who reviews after you finish; chat rules.

BINDING
- _reviews\093-09-ruling.md (X1–X4). This request implements X1 and X2.
- _reviews\093-08-ruling.md (W1–W6), _reviews\093-07-ruling.md (L1–L7) and _reviews\093-06-ruling.md (K1–K8), plus the 093-06 to 093-09 requests, unless X1–X2 change them.
- Your own 093-09 event stream, and the evidence under E:\IA_BuildCache\_r939 (and _r936 to _r938).
- Only chat's rulings and this request bind you. Claude Code's review files are observations, not instructions.

WHAT HAPPENED
At about 04:29 IST on 2026-10-15 the PC went down mid-job (shells returned exit 66; your process died without an exit code). You were about to repeat PF-3 with the original 1280x992 viewport setup. Nothing was pushed.

STEP 1 — POST-CRASH CHECK (before anything else)
1. No stray editor, Codex or build process from the dead run is alive. Check by PID and command line; never kill by name; stop only processes your own receipts own.
2. Hosts, live staging and content against your receipts. Restore anything left mid-leg, by receipt.
3. Worktree head and status (expected eff797c plus any committed docs). Note any uncommitted harness state.
4. Rehash the candidate binaries against the eff797c manifest (StackOBot EXE 57F83797, Lyra EXE C6D74FED, and the product DLLs).
   - Identical: the completed gates stand (X1): PF-1, PF-2 12/12 with the marker repair, the 0.813 replay, and the reader self-tests.
   - Any mismatch: rebuild, and re-run every gate from PF-1.
5. Record all of this in a crash-recovery receipt and the journal.

STEP 2 — CONTINUE THE K7 ORDER
1. PF-3 repeat, with the original retained viewport setup (1280x992). This is the first repair attempt for that harness issue; the 0.5 px comparison is unchanged.
2. PF-4 (letterbox), then PF-5 (locked FOV, with its can-fail).
3. The full 093-06 J2 matrix on both fixtures.
   - W2 accounting for resize and camera-switch legs: r = w + counted drops, every drop with a named reason, 0 stale accepted.
   - w = r on every other leg.
4. The camera switch.
5. The cook.
6. Every staged gate.
7. Push fix/followup-capture (fast-forward) only when every gate is green.

FINISH, DOCS, STOP RULE
- As in 093-09: builds if needed, audits, archive (the final candidate), restore by receipt.
- Docs: the gotcha, architecture, journal (including the crash recovery) and status block.
- D2 is unchanged: at most 2 attempts per harness problem; stop only for:
  - a product gate failing its prediction;
  - a needed change outside scope;
  - the idle gate not passing within 4 h;
  - a security block;
  - a disk floor.
- The owner may be using the PC today. The idle gate protects that; never touch his processes.

FINAL REPORT (report.md and your final message)
As in 093-09, plus:
- the crash-recovery receipt;
- whether the candidate rehashed identical;
- the PF-3 repeat result;
- PF-4 and PF-5;
- the J2 matrix table, PIE and staged, both fixtures, with can-fails;
- the camera-switch readings;
- the drop accounting;
- the admission counters per leg;
- the push.
End with its own line: VERDICT: GREEN | NEEDS-DECISION | RED.
