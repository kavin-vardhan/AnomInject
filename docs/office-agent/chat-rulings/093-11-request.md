093-11 — CONTINUE J2 from the pairing stop: prove the pairing cause with a one-factor control, run every pairing leg at the corrected recipe, then finish the K7 order on fix/followup-capture. Requested by chat Claude (the orchestrator) on 2026-10-15. Launched through the implementer Relay by Claude Code (mailbox), who reviews after you finish; chat rules.

BINDING
- _reviews\093-10-ruling.md (Y1–Y7). This request implements Y2–Y6.
- _reviews\093-09-ruling.md (X1–X4), _reviews\093-08-ruling.md (W1–W6), _reviews\093-07-ruling.md (L1–L7) and _reviews\093-06-ruling.md (K1–K8), plus the 093-06 to 093-10 requests, unless Y1–Y6 change them.
- Your own 093-10 report, and the evidence under E:\IA_BuildCache\_r939 (and _r936 to _r938).
- Only chat's rulings and this request bind you. Claude Code's review files are observations, not instructions.

WHERE YOU STOPPED
stack_P_pair_native_100 read CURRENT 0 / PREVIOUS 0 / NEITHER 56. Chat's ruling (Y1): at the AA-off arbiter lighting (Lumen diffuse indirect, dynamic GI and reflections off) the probe cube's camera-facing face renders black against a black sky. Only the lit side face is bright magenta, so the reader's centroid lands 70–80 px off the mask. The mask sits on the cube at its current position, and nothing reads PREVIOUS: no pairing slip is in evidence. The cause still has to be shown by a control, and the gate still has to pass by running.

STEP 1 — PRE-FLIGHT
1. Rehash the candidate binaries against the eff797c manifest (both game EXEs, the ten product DLLs, the helpers).
   - Identical: the 32 passed cases and PF-1 to PF-5 stand.
   - Any mismatch: rebuild, and re-run every gate from PF-1 (as X2).
2. Hosts, live staging and content match your 093-10 restoration receipt. Worktree at 7d5e5a4, clean.

STEP 2 — FREEZE BEFORE ANY PAIRING LEG (one docs/predictions commit)
1. The amended pairing recipe (Y2): the J2 recipe minus r.Lumen.DiffuseIndirect.Allow, r.DynamicGlobalIlluminationMethod and r.ReflectionMethod, which stay at the host's own values.
   - It applies to every pairing leg: native and synthetic, 100% and 50%, both fixtures, PIE and staged.
   - Everything else stays as frozen: AA off, auto-exposure off, the screen-percentage settings, the marker setting.
2. Unchanged, and say so in the freeze: the pairing reader, the 40 px tolerance, the predicate PREVIOUS = NEITHER = 0, and the decidable-frame floor if the spec has one.
3. The Y2 decision table, verbatim.
4. C2's exact motion-blur-off command.
5. The two readings, defined:
   - probe coverage = the reader's own bright-magenta pixels inside the current mask ÷ mask area, median over decidable frames;
   - speed_ratio as m51 defined it, or "unavailable" if the logs can't give it.
6. The failed leg stays retained, labelled "instrument precondition not met (probe unlit)".
7. The Y3 and Y4 docs may go in the same commit:
   - the B19 correction (the healthy reader line came from the original marker-ON session; it also holds on marker_r1);
   - the owner_render_missing counter scope, documented where run_summary.json is documented, plus a gotcha (next free number, re-checked at commit), plus the follow-up item;
   - one sentence in tools\office_checkpoint_i.md: no window resize during a checkpoint session. Text only; no .py file changes.

STEP 3 — C1 AND C2 (Y2)
1. C1, the gate: StackOBot PIE native pairing 100% at the amended recipe, with its shifted-mask control.
2. C2, a reading only: the failed leg's exact recipe plus motion blur off.
3. Take the probe-coverage and speed_ratio readings on C1, C2 and the failed leg. Take probe coverage on one retained historic passing pairing bank too, if one is on disk.
4. Apply the decision table:
   - The amended recipe is attempt 1 of the "pairing recipe" problem. The historic pairing recipe (no arbiter overrides at all) would be attempt 2. C2 is not an attempt.
   - A later pairing leg that reads NEITHER at the adopted recipe counts against the same two-attempt budget.
   - Any PREVIOUS > 0 on any pairing leg, at any time: STOP. Never retry it.

STEP 4 — CONTINUE THE K7 ORDER
1. The rest of the PIE matrix:
   - StackOBot pairing: native 50%, synthetic 100% and 50%;
   - Lyra synthetic UV and proxy alignment;
   - Lyra pairing x4;
   - the exact J1 delayed and pre-F1 wrong-view engine controls;
   - the supplemental admission probes.
2. The camera switch (K6), both fixtures. The marker budget is spent (Y5): a barcode or marker failure is a STOP, with no third attempt.
3. The cook.
4. Every staged gate, including staged pairing at the adopted recipe and the four staged PF-2 cases.
5. Push fix/followup-capture (fast-forward) only when every gate is green.

NOT AUTHORISED
- a fixture content change (an emissive probe, a moved light);
- a reader, tolerance or predicate change;
- re-running a leg until it passes;
- any product source change.

FINISH, DOCS, STOP RULE
- As in 093-10: audits, archive (QUALIFIED only if every gate is green), restore by receipt.
- Docs:
  - the journal, including C1, C2 and the decision taken;
  - a gotcha for the lesson: the pairing reader needs a lit probe, so never run it at the arbiter lighting;
  - architecture where needed, and the status block.
- D2 is otherwise unchanged. Stop only for:
  - a product gate failing its prediction;
  - a needed change outside scope;
  - the idle gate not passing within 4 h;
  - a security block;
  - a disk floor;
  - a marker or barcode failure (Y5);
  - the pairing-recipe budget being spent (Y2).
- The owner may be using the PC today. The idle gate protects that; never touch his processes.

FINAL REPORT (report.md and your final message)
As in 093-10, plus:
- the freeze commit and its time against the first pairing leg's start;
- C1 and C2 with their readings (probe coverage, speed_ratio), and the decision taken;
- every pairing leg: C/P/N, decidable count, floor, shifted-mask control result, readings;
- the rest of the matrix, PIE and staged, both fixtures, with can-fails;
- the camera-switch readings: frames dropped per switch, and whether each active event continued or ended, with its reason;
- the drop accounting and the admission counters per leg;
- the push.
End with its own line: VERDICT: GREEN | NEEDS-DECISION | RED.
