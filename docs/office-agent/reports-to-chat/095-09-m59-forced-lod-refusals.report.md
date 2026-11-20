# REPORT for 095-09-m59-forced-lod-refusals.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 095-09-m59-forced-lod-refusals.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-11-14 08:46:05 | finished: 2026-11-14 09:18:18 | duration: 32 min | exit code: 0
context at last turn: 646395 tokens (65% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\095-09-m59-forced-lod-refusals.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN

**1. For the owner**
- Yes. The delivered build could label frames where nothing changed on screen. It happened on two kinds of object: "hierarchical instanced" meshes (the kind used for grass, foliage and mass-placed props), which ignore a forced detail level; and, for `lod_corruption` only, meshes that have just one detail level.
- The fix makes both LOD anomalies skip those meshes and log the reason by name. If every mesh on the chosen object is skipped, nothing fires and no label is written.
- Characters (skeletal meshes) are unchanged. From the engine source, they show a forced detail level one frame late, so their edge frames may be labelled one frame off. This is read from source, not measured.

**2. Part 1, claim by claim** (full `file:line` list: `code-receipts-095-09\part1-claims-095-09.md`)
1. **HISM ignores a forced LOD: confirmed.**
   - The HISM proxy draws only through its dynamic path (`HierarchicalInstancedStaticMesh.cpp:936-938`).
   - It picks LODs per cluster from `ClampedMinLOD` and the `foliage.*` cvars (`:1748-1762`). `ForcedLod` appears nowhere in that file.
   - A plain ISM does honour a forced LOD, for the whole component (`StaticMeshRender.cpp:1165-1167`, `:2320-2323`; `InstancedStaticMesh.cpp:1327`).
2. **`lod_popping` selects a HISM outside foliage and labels it: confirmed.**
   - The selection filter drops only foliage-actor owners and zero-instance ISMs (`AnomalyViewport.cpp:794-823`). Runtime landscape grass is not selectable because it reports zero instances (G574).
   - Apply has no instancing check (`Anomaly_LodPopping.cpp:114-197`).
   - Every popped frame is labelled: the anomaly uses the `AnomalyState` policy, where the active bit alone decides (`AnomalyLabelSync.h:72-74`).
   - The m26 veto removes only measured-zero events (`AnomalyCaptureSubsystem.cpp:8749`), and these targets still draw pixels, so it keeps them.
3. **`lod_corruption`: confirmed, but reachable only by a targeted capture.** On `master` it is not in the auto pool (`AnomalyAutoInjectorSubsystem.cpp:35-46`) or the selector.
   - Apply had no guard at all (`Anomaly_LodCorruption.cpp:49-68`).
   - Single-LOD and HISM components were forced, and every on-screen frame was labelled (FireWindow policy).
   - A multi-LOD plain ISM changes visibly, so it is not a defect.
   - The 4.25 branch skips single-LOD meshes and runs its LOD predictor in the auto pool only, and has no HISM check either.
4. **Skeletal: confirmed one frame late, from source only.**
   - `SetForcedLOD` marks nothing dirty (`SkinnedMeshComponent.cpp:3393-3403`). The component reads it in its own `TG_PrePhysics` tick (`:1186`, `:3543-3546`).
   - Writes from our tickables or the capture tick therefore show on the next frame's render.
   - Under `SynthTickOrder` the injector's dispatch runs before that component tick (`LevelTick.cpp:1468` vs `:1537`), so the pops line up.
   - `lod_corruption` in both tick orders: the onset frame is labelled while still showing the old LOD; the revert frame is unlabelled while still showing the forced LOD.
   - `lod_popping`: every pop edge is one frame off in the native order; the revert edge is off in both orders.

**3. The fix**
- **Commits on `fix/m59-forced-lod-refusals` (off `master`, pushed):** code `19c0717`, docs `4f40f69`.
  - New pure header `AnomalyLodPolicy.h` and `AnomalyLod::GetForcedLodRefusal`.
  - Refusal reasons: `hism_ignores_forced_lod` and `single_lod`, logged per component in `lod_popping`'s style and counted in each anomaly's summary line.
  - They apply to every fire: auto pool or targeted, with or without a `lod-index`. This contradicts no documented rule; `lod_popping`'s log already says the single-LOD guard covers both paths.
  - Both anomalies now override `GetLastRefusalReason`, so the `Auto.Yield` tally shows the reason instead of `refused`.
- **Size:** 58 changed lines in existing source files, plus the 27-line pure header for the selftest. That totals 85, above the brief's "about 60" guide; I went ahead because the extra is the header the brief's selftest asked for.
- **Tests:**
  - `tools/lod_refusal_selftest.cpp`: 29 checks, 0 failures. Three mutants each fail (9, 1 and 2 failures).
  - Editor compile on the fix branch: 0 errors, 0 warnings; a string scan finds the new log text in the built DLL.
  - All 16 pure C++ selftests on that branch pass. Four needed include paths that my first run lacked; that was my harness, not the code.
  - Python suites pass; lever audit passes on source (53 levers) and on the built DLLs.
- **Skeletal recommendation: (b), refuse skeletal targets in both anomalies until a leg measures them.**
  - The predicted error hits both edges of every `lod_corruption` event in both tick orders.
  - No transition flag excuses those frames on the release defaults.
  - The bench has no known multi-LOD skeletal mesh (the Bot is single-LOD; the backpack's LOD count is unknown), so a leg needs a new cooked asset first.
  - StackOBot loses nothing: its skeletal targets are already refused as single-LOD.

**4. The integration tip**
- **Merge:** a true merge, `81c956a`. Source merged cleanly; only `CLAUDE.md` and `gotchas.md` conflicted, as expected for append-only docs.
- **Status note:** a status-only commit `075221f` followed; Source is unchanged since `81c956a`.
- **Compile:** Editor 0 errors, 0 warnings (60 s, 2 actions, launched with 10.4 GiB free).
- **Tests:** 18/18 pure C++ selftests, Python suites, all 5 m58 terrain mutants fail as they should, lever audit passes on source and binary (57 levers), pre-push scans pass.
- **Push:** both branches pushed and equal to origin.
- **Checkout:** on `integ/m56-m58` = `075221f`, no tracked changes. The host's plugin Editor DLLs match `81c956a`'s Source; the Game exe is not built.

**5. Legs for the heavy gate brief** (frozen in `predictions-095-09.txt`, SHA-256 `DDFA0C43…DDE6DD97`)
- **L59-1:** `lod_corruption` on single-LOD `StaticMeshActor_49` in CB_GateLevel. The release exe applies it and the pixel gate should read NO-TRACE (this is the can-fail). The new exe refuses `single_lod` and writes no event.
- **L59-2 / L59-3:** `lod_corruption` and `lod_popping` targeted at `InstancedFoliageActor_0_0_0` in MainWorld (SM_Bush, 2 LODs). The release exe applies and labels; the new exe refuses `hism_ignores_forced_lod`. The release pixel reading is reported but not decisive, because foliage motion may blur it.
- **L59-4 (control):** `CB_LodFix_N` still applies on the new exe, with edges as in 089-03.
- **L59-5 (optional, decisive):** an HISM and an ISM copy of CB_LodFix_N's mesh. This needs either a fixture-script option riding on 095-07's cook or a new bench spawn lever; neither is built.
- **L59-6:** only if chat picks skeletal option (a).

**6. Safety:** none. `master`, `m51`, the europa and followup branches, tags and the dashboard were not touched. No process was killed; no Game build, launch or cook.

**Decisions chat must make:**
- **Skeletal:** refuse now (my recommendation) or measure first, which needs a new cooked multi-LOD skeletal asset.
- **L59-5:** whether 095-07 gets the HISM/ISM fixture.
- **Telling the client:** whether to say that earlier datasets may contain these no-change LOD labels. This is the owner's call; the readme does not say it.
api-equivalent cost (not billed on Max): $20.46 | turns: 145 | running total across all briefs: $6,223.19
