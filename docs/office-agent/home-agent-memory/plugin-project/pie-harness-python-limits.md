---
name: pie-harness-python-limits
description: "PIE legs driven by editor Python - what cannot be called (deferred spawn, AddComponentByClass), the summon route, -IAIBench for bench levers, editor-world lookup"
metadata:
  node_type: memory
  type: reference
  originSessionId: <uuid>
  modified: 2026-10-01T20:07:56.397Z
---

Learned in 090-10b2 (2026-10-01/02) with the harness `_reviews\090-10b2-*` (driver `090-10b2-pie.py`: slate post-tick recorder of
frame times + `PYSTEP <file>` hooks; launched as `UnrealEditor <uproject> <map> -pie -ExecCmds="py <driver>"`, never
`-ExecutePythonScript` with `-pie`, repo G440).

- **Spawning into the PIE world from Python:** `GameplayStatics.begin_deferred_actor_spawn_from_class` and
  `AActor.add_component_by_class` are NOT script-exported (AttributeError / absent). Working route: console `EnableCheats` then
  `summon /Script/Engine.<Class>` via `unreal.SystemLibrary.execute_console_command(pie_world, ...)`, then find the actor with
  `GameplayStatics.get_all_actors_of_class` and configure it (set mobility MOVABLE before `set_static_mesh`). 15,000 summons + setup
  took ~3.3 s. No InstancedStaticMesh can be built this way; `SplineMeshActor` (spline VF, MATUSAGE_SplineMesh) can.
- **Bench levers (`IAI.Bench.*`) need `-IAIBench` on the command line**, editor or game; without it the log says
  `Command not recognized` and a "null" leg silently becomes a normal leg (084-09 legs pass it; a 090-10b2 B5 pair was invalid).
- **`UnrealEditorSubsystem.get_editor_world()` returns None during PIE** (logs "The Editor is currently in a play mode"); load the map
  asset path (the PIE world path without `UEDPIE_0_`) to get the editor world object.
- In-memory material flag edits with `set_editor_property(..., PropertyAccessChangeNotifyMode.NEVER)` do not recompile; DEFAULT
  notify recompiles. Neither saved anything (content manifest unchanged); still verify with `085-03-content-manifest.py`.
- In the editor a missing shader for a drawn vertex factory is compiled on demand when the map reads incomplete
  (MaterialShared.cpp:3348-3388), so "missing" states self-heal in seconds; a material compiled once lands in the job-cache DDC, so
  use a fresh material per "missing" leg.

Related: [[scratch-host-build-when-checkout-is-protected]], [[foreign-heistcrew-editors-block-bench]].
