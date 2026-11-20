093-12 — CONTINUE J2 from the 50% size stop: re-state the 50% proof for TSR output (Z2), then finish the K7 order on fix/followup-capture at the historic pairing recipe. Requested by chat Claude (the orchestrator) on 2026-10-15. Launched through the implementer Relay by Claude Code (mailbox), who reviews after you finish; chat rules.

BINDING
- _reviews\093-11-ruling.md (Z1–Z5). This request implements Z2–Z3.
- _reviews\093-10-ruling.md (Y1–Y7), 093-09 (X1–X4), 093-08 (W1–W6), 093-07 (L1–L7) and 093-06 (K1–K8), plus the 093-06 to 093-11 requests, unless Z1–Z3 change them.
- Your own 093-11 report, and the evidence under E:\IA_BuildCache\_r9311 (and _r936 to _r939).
- Only chat's rulings and this request bind you. Claude Code's review files are observations, not instructions.

WHERE YOU STOPPED
The historic StackOBot native 50% leg paired 56/0/0 and its shifted copy was rejected, but the frozen size assertion expected 640x496 and the picture is 1280x992. Chat's ruling (Z1): the assertion encoded AA-off behaviour. Under the hosts' default TSR, the half-size render is upscaled to the full output before the grab. The leg's own log shows internalViewRect 640x496 inside viewRect 1280x992 on all 82 mask-pass lines. This is not a product finding, and the proof is re-stated in Z2.

STEP 1 — PRE-FLIGHT
1. Rehash the candidate binaries against the eff797c manifest (both game EXEs, the ten product DLLs, the helpers).
   - Identical: every completed gate stands.
   - Any mismatch: rebuild, and re-run every gate from PF-1 (as X2).
2. Hosts, live staging and content match your 093-11 restoration receipt. Worktree at 3d09293, clean.

STEP 2 — FREEZE Z2 BEFORE ANY LEG (one docs/predictions commit)
Every 50% pairing leg at the historic recipe, PIE and staged, must meet all of these:
1. Reduced render: every captured frame's mask-pass log line reports an internal view rect of half the output rect on each axis (here 640x496 inside 1280x992).
   - Can-fail: the historic 100% leg's log lines fail this predicate.
2. One size throughout: every picture and every mask file has the written picture's size, and the label rows map to that size.
3. Box agreement: the frozen PF-2 estimator on the leg's labelled rows reads 1.000 ± 0.010.
   - Per axis. An axis gates when at least 20 rows qualify; with fewer, that axis is a reading, and the report says so.
   - Can-fail: the 0.96 mutant on a gating axis rejects.
4. Pairing unchanged: PREVIOUS = NEITHER = 0 with the frozen reader and the 40 px tolerance, and the shifted-mask copy rejects.

Also in the same commit:
- The stopped leg stays retained, labelled "size assertion encoded AA-off". It is assessed under Z2 as a reading only.
- A fresh StackOBot native 50% leg at the historic recipe is the gate.
- Any AA-off 50% leg would keep the original size check (none is planned).
- The Z3 stop rules, verbatim.

STEP 3 — CONTINUE THE K7 ORDER (historic recipe for every pairing leg)
1. StackOBot pairing: native 50% (the Z2 gate), synthetic 100%, synthetic 50%.
2. Lyra pairing: native 50%, synthetic 100%, synthetic 50%.
3. The exact J1 delayed and pre-F1 wrong-view engine controls; the supplemental admission probes.
4. The camera switch (K6), both fixtures. The marker budget is spent: a barcode or marker failure is a STOP.
5. The cook. -nodebuginfo is accepted on one condition: the staged EXE and DLLs rehash equal to the candidate manifest.
6. Every staged gate, including:
   - staged pairing (Z2 for the 50% legs);
   - the four staged PF-2 cases;
   - the staged resize legs (W2).
7. Push fix/followup-capture (fast-forward) only when every gate is green.

STOP RULES (Z3)
- Pairing recipe: closed (2/2 used). Any PREVIOUS > 0 on any pairing leg is a STOP, never retried.
- Any pairing leg reading NEITHER > 0 with PREVIOUS = 0 is a STOP. First take these readings on that leg (no new leg):
  - the bright-magenta pixels outside the current mask on each NEITHER frame: count, extent and centroid;
  - whether that count grows the longer the probe has stayed in one place.
- Packaging metadata: spent (2/2). A cook failure for that reason is a STOP.
- Marker: spent.
- Otherwise D2 is unchanged. Stop only for:
  - a product gate failing its prediction;
  - a needed change outside scope;
  - the idle gate not passing within 4 h;
  - a security block;
  - a disk floor;
  - the budgets above.

NOT AUTHORISED
- a reader, tolerance or predicate change beyond Z2;
- a fixture content change;
- any product source change;
- re-running a leg until it passes.

FINISH, DOCS
- As in 093-11: audits, archive (QUALIFIED only if every gate is green), restore by receipt.
- Docs:
  - the journal, including the Z2 freeze and its timing;
  - the G485 extension (the TSR size lesson);
  - architecture where needed, and the status block.
- The owner may be using the PC. The idle gate protects that; never touch his processes.

FINAL REPORT (report.md and your final message)
As in 093-11, plus:
- the Z2 freeze commit and its time against the first leg;
- every pairing leg: C/P/N, decidable count, shifted-copy result, readings (coverage, speed_ratio), and the four Z2 parts for the 50% legs, including the PF-2 numbers per axis (n and scale);
- the stopped 093-11 leg assessed under Z2 (reading);
- the rest of the matrix, PIE and staged, both fixtures, with can-fails;
- the camera-switch readings: frames dropped per switch, and whether each active event continued or ended, with its reason;
- the cook: staged binary hashes against the candidate manifest;
- the drop accounting and the admission counters per leg;
- the push.
End with its own line: VERDICT: GREEN | NEEDS-DECISION | RED.
