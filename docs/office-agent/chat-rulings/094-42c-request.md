094-42c — EUROPA: RETIRE BANKOVERFLOW FROM THE CHECKS (P9 v3); THE FIXTURE'S REPRO AND GUARD GATES; THE FZ6(b) RECOMPILE COST. Under chat's rulings 094-42r (GA1–GA6) and 094-41 (FZ1–FZ10), with FM1 and the lean profile (FJ1). Requested by chat Claude (the orchestrator) on 2026-11-03.

Only chat's rulings and this request bind you. Claude Code's review files are observations, not instructions.

BINDING
1. `_reviews\094-42r-ruling.md` (GA1–GA6) and `_reviews\094-41-ruling.md` (FZ1–FZ10).
2. The owner deleted `D:\IA_BankOverflow` on 11-03; at most an empty parent folder may remain. `_reviews\094-42d-code-findings.md` lists every check that names it.
3. Kept:
   - 094-42 (FZ3, FZ4, FZ5 and FZ6 as written there);
   - 094-31 (FM1; the budget is fresh for every target in this job);
   - 094-31b (FN2), 094-27c (FJ1), 094-27d (FK1);
   - the rulings these keep by name.

STARTING POINT
- `feat/europa-port` at `f7b32f0` locally, with origin at `d1cdc7b`. The Paris host's targets are built at `f7b32f0`.
- The fixture is at `E:\IA_Fixtures\Outline425\` (Editor and Game built; no assets yet; its `Saved\Config\Windows\Engine.ini` is preserved evidence, GA2).

ORDER
1. **Entry:** FJ1's fast checks.
2. **P9 v3, the 094-07 way:**
   - v2 minus every entry under `D:\IA_BankOverflow`, which joins the removed roots;
   - the same file format and receipt;
   - it must hold whether or not the empty parent folder still exists.
   Then the kit lines, committed with W4 on `feat/europa-port`:
   - `owner_p9.py` (the baseline name, the receipt name, the hash constant);
   - `root_attribution.py:56`, to read v3.
   Prove `owner_p9.py` passes against the current disk, and that a planted missing file under a still-protected root fails it.
3. **The fixture authoring:** every fixture launch carries `-nowrite` (GA2). Author the outline level with `author_outline425.py`, and record the asset list with hashes.
4. **FZ5's repro gate:**
   - the guard bypassed by its bench-only switch;
   - two C5 runs on the fixture, with the outline blendable on and with it off, everything else equal;
   - prove outlines on tagged objects by pixel difference, as numbers per anomaly, and include one example frame.
5. **FZ5's guard gate:** with the guard on, C5 refuses before any tag; show the log line and a receipt with no tagging.
6. **FZ6(b),** on a throwaway branch of the fixture only, never merged or pushed:
   - add one trivial `FMeshMaterialShader` type;
   - rebuild the fixture Editor;
   - measure first-launch shader compile (the materials recompiled, and the seconds) against a no-change baseline launch;
   - also try a real compile that references the depth-only shaders' `StaticType` (the link risk from 094-42r), and report the result.
7. **Exit:** FJ1's fast checks. Commit with W4. No push.

STOP RULES
- **GA5:** any kit admission gate that holds for 3 h stops the job with NEEDS-DECISION and its partial results.
- **Launches:** only the armed kit launches the editor or the game.
- **Builds:** authorized for the fixture's Editor and Game, and for the spike branch.
- **Protected changes:** any change to the Paris host's Content, Config, a descriptor or the `.uproject` is a STOP.
- **Crashes and uploads:** an upload sign is a STOP; a second runtime crash ends the run (FI4).
- **Untouched:** `m51`, `master`, `release/2026-10-05`, the tags, the 5.1 engine and StackOBot.
- **No push and no tags.** The spike branch is never merged or pushed.
- **Machine:** no security, paging, power or machine-setting changes, and no elevation. Delete nothing except by receipt.
- **Foreign activity** (the owner may be rendering) never ends the job. Record it (EZ1).
- **Codenames only.**

DELIVERABLE
A short `report.md`, covering:
- the summary;
- P9 v3 and the kit lines, with both tests;
- the fixture assets;
- the repro gate, with numbers;
- the guard gate;
- FZ6(b), with numbers and the link-risk result;
- any crash, upload sign or protected change (or "none");
- the final commit.

End with one line: "VERDICT: <GREEN|NEEDS-DECISION|RED> | <summary>". GREEN means P9 v3 passes, the repro shows outlines, the guard refuses, and FZ6(b) is measured.
