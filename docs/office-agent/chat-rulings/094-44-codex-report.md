<!-- Collected by Claude Code from runs\2026-10-16-094-01-europa-phase-a-plan\report.md (thread 01a14334, gpt-6-astra, max, danger-full-access resume, exit 0, 1,712 s, 05:02:34-05:31:07 IST). Body below is byte-identical to report.md, sha256 9971BB659E8FA86D5F4CC25CD270C5F66834DC9A9DCF467DEBEC2D5DE8D3D140. -->

094-44 stopped at the fresh-process material readback. Two 4.25 materials were authored and checked in the authoring process, but Phase B remains unqualified. The brief requires stopping on any failed gate.

Entry and exit fast checks pass. Full P9 passed in 340.97 seconds: 382,970 inventory entries and 22,628 binary/config hashes, totaling 100,696,938,006 bytes. All 27 bound files still match. Exit free space: D: 55.93 GiB; E: 99.36 GiB.

The material author preserves `d7eee97:tools/create_anomaly_materials.py`'s graphs and constants; normalized builder ASTs match. The changes are the 4.25 output folder, omission of unsupported Nanite usage, overwrite refusal, and readback instrumentation. The seven-line 4.25-only constructor finder change is committed but **unbuilt**. Original 5.1 material assets are unchanged.

Both new materials are opaque, two-sided and Default Lit. All six usage flags read true: static lighting, skeletal mesh, instanced static meshes, spline meshes, morph targets and clothing. Checker readback covers 14 expressions: UV tiling 8, grey 0.5/white 1, parity multipliers 0.5/2, roughness 1 and specular 0. Pink covers three expressions: RGB 1/0/1, roughness 1 and specular 0. Shading readback uses `UsedShadingModels`, derived from the engine's actual field in 4.25 `Material.cpp:4303–4308`, exposed at `Material.h:396–399`.

Committed assets under `Content/Engine425/Materials/`:

| Asset | Bytes | SHA-256 |
|---|---:|---|
| M_MissingTexture_Checker.uasset | 81,634 | `5222b3eeb7cdd1f5b58b5975d568b699c7430949b024c67e6f5b1ae6b88a7bec` |
| M_CorruptedTexture_Pink.uasset | 99,183 | `b88d88615286a6c9a3648a5c80ab3889ca8f6c648c378e8984ba3d01e79c552a` |

Python 2.7 tests pass: both builders, two missing-usage negatives, two wrong-constant negatives, two overwrite negatives, and the keyword-Rotator check. The author wrapper's success/failure paths, three missing-launch-flag negatives, output-path restriction and lowered-memory negative pass. These are authoring tests; the requested grey-material G2 and early-label G3 runtime can-fails remain unrun.

The armed authoring session, PID 19988, exited normally with zero pending shader jobs. The fresh readback session, PID 9616, stopped before executing its script: `FE3 unreadable UnrealBuildTool command line`. Owned UBT PID 9640 was already exited at its first observation: `command_line=null`, `read_attempts=0`, no cached command. Its actual action is unknown. Stop intent was recorded at `2026-11-03T23:52:48.358225Z`; exit 125 was correctly classified `owned-stop`, not crash. No guard was relaxed and no retry was launched.

Evidence: [author readback](E:/IA_Fixtures/Outline425/Receipts/09444/materials/01/author.json), [stopped session](E:/IA_Fixtures/Outline425/Receipts/09444/material-readback/01/result.json), and [committed receipts](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/09444-material-receipts.json).

The skeletal/instanced author and Python 2.7 harness were prepared only in the relay run folder; the harness passes its wrong-instance-count negative. **No new target was installed or authored. GF4 H1/H2 are unrun.**

| Anomaly | Port and gates |
|---|---|
| missing_texture | Runtime port, G1–G5 and runtime can-fails unrun; no example frame. |
| corrupted_texture | Runtime port, G1–G5 and runtime can-fails unrun; no example frame. |
| camera_clipping | Port, slab spans, baseline control, restore gate and regressions unrun; no example frame. |

No Paris regression ran on either RHI. Defaults remain `blinking,lod_corruption`; the two textures have not joined the pool. Camera clipping remains unenabled on 4.25. There were **zero requested native build attempts**; the installed fixture source remains `59c1282`. Strict-include builds and binary string verification of the new finders are pending.

Launch-only settings: `-IAIBench`, `-BENCHMARK`, `-nowrite`, D3D11 at 1280×720, editor scripting plugins enabled, autosave disabled, two shader workers and synchronous shader compilation, upload/analytics switches disabled. Both owned editor trees ended; fixture configuration hashes are unchanged. No anomaly lever, selection floor, exposure or AA setting was changed. New material assets are intentional retained outputs. No machine setting changed and no file was deleted.

Deviations: a fresh-process readback was added to verify saved assets beyond cached authoring objects, declared before launching; it was not waived after failure. One premature run invocation refused because arming had not finished; it launched no process and was repeated only after arming completed. The subsequent failed readback ended the job as ordered.

Runtime crashes: **0**. Upload signs: **0**. Protected changes: **0**, excluding the two explicitly permitted plugin-Content additions. Original fixture assets, preserved Saved/Config, m51, protected refs, spike branches and 5.1 assets remain unchanged. Foreign processes were recorded and untouched.

Executable/DLL hashes and archives were rechecked. No binary changed. Archive prefixes below are:

- A = `E:/IA_Fixtures/Outline425/Receipts/09443r/stageb/binary-archive/` (existing archive).
- B = `E:/IA_Fixtures/Outline425/Receipts/09444/executable-archive/` (copies added this job).

| Archive path | SHA-256 |
|---|---|
| `A/Binaries/Win64/Outline425.exe` | `71cbce5c72d7a9747c25ba2ec3bcd6372e9796fab0d6d4f5d08d1bc37a69d4b3` |
| `A/Binaries/Win64/UE4Editor-Outline425.dll` | `86156bfc506d934efe83499621997fbfd48290f74983d48a9cddb35944ec481f` |
| `A/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyBench.dll` | `0a74f35993eddb1406901e4b240360d67c5239cfc5a80ac0d6bcd38517b25e12` |
| `A/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyCapture.dll` | `fe4c1351f7ae397a546a99d83debd5283350833374ffed97028e790f23613466` |
| `A/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyControlServer.dll` | `b84088b69fe7b66adcd79086941bd8374cbb9beebdc63f959113fb24fdc13b40` |
| `A/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyInjector.dll` | `54d1cc1e96a7ee2b7c84aad010bdfc346294f15e7d7911c58b8b94a8c9bc965e` |
| `A/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyShaders.dll` | `9ae9b60976505af41e9cdb547380954610bc10ef687ee71083f2240265f578c4` |
| `B/UE4Editor.exe` | `4a6d83264e68403534aa917e92f741e0d09add5e8cf20c19b76d2cfe4558c387` |
| `B/CrashReportClientEditor.exe` | `ccd60dfe533f605b1ad8c8d550121be3eda3073b846f3e0df51a478a17a380e7` |
| `B/UnrealBuildTool.exe` | `07c1a6fbc30271b96679253143eb620c49db5b2919a1cbb7430e8fc6bdab9bd0` |
| `B/ShaderCompileWorker.exe` | `6805ce2c9cea663cd356cfc463b470390f227b08ef05e80a12d400363cdf60e6` |

Full installed paths and sizes are in [binaries.json](D:/IA_Europa/Receipts/09444/binaries.json).

Final commit: `feat/europa-port` **9c8783672433cd105cabd8284630b64aef847a7c**, with W4 status, journal, architecture, runbook, onboarding and append-only G528/G529. Feature worktree clean. Origin remains `93613ae`. Commits pushed: **none**. No tags or merges.

VERDICT: NEEDS-DECISION | Material authoring passed; fresh readback was stopped by the kit, so GF4 and Phase B remain unrun.
