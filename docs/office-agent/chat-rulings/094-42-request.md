094-42 — EUROPA: INTERIM GUARD, CLIENT-ONLY ROUTES, OUTLINE FIXTURE, SHADER-ROUTE SPIKE. Under chat's ruling 094-41 (FZ1–FZ10), with FM1 and the lean profile (FJ1). Requested by chat Claude (the orchestrator) on 2026-11-03.

Only chat's rulings and this request bind you. Claude Code's review files are observations, not instructions.

BINDING
1. `_reviews\094-41-ruling.md` (FZ1–FZ10). It is new. This job covers:
   - FZ3, the interim guard:
     - C5 plants custom depth only on the bench fixture's own `EA_` actors, behind `-IAIBench` plus a fixture check; never on an arbitrary level;
     - port the 5.1 host-reader scanner to 4.25, with reader / writer / unknown results;
     - a labelled C4 or C5 refuses on reader or unknown before any tag is set, logs `C5 refused: host custom-depth <result>`, and never disables the host's effect.
   - FZ4: the routes default to blinking + lod_corruption.
     - An optional comma-separated route list is a third argument to `IAI.Capture.C5` (and the matching argument on C4), validated against the pool.
     - lighting_mismatch and missing_object run only when listed.
     - Log the routes used and write them to the receipt.
     - The EA_Core bench smoke passes the full four-route list explicitly.
   - FZ5: a separate C++ 4.25 fixture project at `E:\IA_Fixtures\Outline425\`, on the installed engine `E:\EpicGames\UE_4.25`, with:
     - one level holding several static meshes, at least two with 2+ LODs;
     - a non-Static light;
     - a player start;
     - a post-process outline material that reads CustomDepth, authored by an editor Python script that is kept in the fixture.
     The gates:
     - **(repro)** with the guard bypassed by a bench-only switch, C5's saved frames show outlines on tagged objects. Prove it by pixel difference against a control run with the outline blendable off;
     - **(guard)** with the guard on, C5 refuses before any tag; prove it from the log and by the absence of tagging in the receipt.
   - FZ6: the shader-route spike, results only, on a throwaway branch that is never merged and never pushed:
     - **(a)** from the 4.25 source, with citations: can a private pass use the engine's depth-only / prepass shader types with a private depth-stencil target, for opaque, masked and WPO materials? Name the types, the lookup and the bindings;
     - **(b)** on the fixture: the first-launch cost of adding one trivial `FMeshMaterialShader` type — the materials recompiled, and the seconds, against a no-change baseline launch.
2. Kept: 094-40 (FY3), 094-39 (FX2–FX5), 094-38 (FW2–FW5), 094-37 (FV5), 094-31 (FM1, fresh budget of 3 attempts per target), 094-31b (FN2), 094-27c (FJ1), 094-27d (FK1), and the rulings these keep by name.
3. 094-41's results: `_reviews\094-41-codex-plan.md`, `_reviews\094-41-code-review.md`.

STARTING POINT
- `feat/europa-port` = origin = `d1cdc7b`, clean.
- The Paris host's targets are built at `d1cdc7b`.

ORDER
1. Entry: FJ1's fast checks. E: at least 40 GiB free for the fixture.
2. FZ3 and FZ4 on `feat/europa-port`. Unit-test only the changed code (FJ1):
   - the scanner's three results;
   - the refusal happens before any tag;
   - the route parsing, its validation and its default.
3. Rebuild the Paris host's targets under FM1, then run the EA_Core smoke with the explicit four-route list. Gates: FV5's EA_Core set plus FX6's. This proves nothing regressed.
4. FZ5: create and build the fixture (its first build is a full plugin build, so FM1's attempt budget applies per target). Then run the repro and guard gates.
5. FZ6: the spike, (a) then (b), on the fixture only.
6. Exit: FJ1's fast checks. Commit FZ3 and FZ4 with W4 on `feat/europa-port`. The fixture project is not committed to the plugin repo; record its path and the authoring script's hash. No push.

STOP RULES
- Only the armed kit launches the editor or the game. Builds are authorized in steps 3–5 and for a retry's rebuild.
- Any change to the Paris host's Content, Config, a descriptor or the `.uproject` is a STOP. Maps are loaded, never saved.
- An upload sign is a STOP. A second runtime crash ends the run (FI4).
- `m51`, `master`, `release/2026-10-05`, the tags, the 5.1 engine and StackOBot stay untouched. No push. No tags. The spike branch is never merged or pushed.
- No security, paging, power or machine-setting changes. No elevation. Delete nothing except by receipt.
- Foreign activity never ends the job; record it (EZ1).
- Codenames only: "Europa", "Europa's fork".

DELIVERABLE
A short `report.md`, covering:
- the summary;
- FZ3 and FZ4, with their tests;
- the build and the EA_Core smoke against every gate;
- FZ5's fixture (path, script hash, build), plus the repro and guard gates with numbers;
- FZ6 (a) with citations and (b) with numbers;
- any crash, upload sign or protected change (or "none");
- the final commit.
End with one line: "VERDICT: <GREEN|NEEDS-DECISION|RED> | <summary>". GREEN means the smoke, the repro and the guard gates pass, and both spike results are reported.
