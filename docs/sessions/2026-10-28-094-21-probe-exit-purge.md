# 094-21 exit-purge safety and incremental rebuilds

The accepted 1722348 probe still called RemoveFromRoot on raw UObject pointers in Stop and restored GC fields through stale pointers at module shutdown. UE 4.25 Obj.cpp:4476 sets GExitPurge before fully purging objects; LaunchEngineLoop.cpp:4218 and its subsequent UnloadModulesAtShutdown call order explains why module cleanup cannot assume UObject lifetime. EV5 authorizes skipping these accesses.

The probe centralizes root changes behind engine-exit/purge/GC and object-validity guards. ReleaseGC skips restoration and records why after exit or during GC. Identity, command and screenshot callbacks guard their UObject access. Receipt world keys are integer addresses with cached identity; their weak references are read only behind normal frame guards. Cleanup continues to emit skipped rows and Stop reports whether root release was skipped. No new runtime leg is permitted.

The source gate now covers LoadClass, StaticLoadClass, TObjectIterator, FindFirstObject and root changes, as well as the prior actor/lookup families. Real source plus 24 mutants pass the gate's can-fail suite. EV3 and job-scoped EV4 are implemented and tested, without lowering full-build memory admission. The source commit precedes all six incremental builds; final outcomes and manifests are recorded in Receipts/09421 and the run report. A6 owner evidence keeps its old bindings. EV6's glass-normal baseline and EV7's carries are recorded in the dated predictions.

## 09421 final rebuild evidence

All six incremental builds passed on the first compile attempt. The static check passed the real source and rejected all 24 mutants. The same five probe files are present in all three hosts. Production source, descriptors, C# and content are unchanged.

| Leg | Plugin source | Executed actions | Engine actions |
|---|---|---:|---:|
| 425-editor | 40a8eaf983518fe1992e172434aa8caf347654f1 | 6 | 0 |
| 425-game | 40a8eaf983518fe1992e172434aa8caf347654f1 | 5 | 0 |
| 51base-editor | d7eee97731812384d4fd00af9032254923363ada | 6 | 0 |
| 51base-game | d7eee97731812384d4fd00af9032254923363ada | 5 | 0 |
| 51port-editor | 40a8eaf983518fe1992e172434aa8caf347654f1 | 6 | 0 |
| 51port-game | 40a8eaf983518fe1992e172434aa8caf347654f1 | 5 | 0 |
| clean-engine-query | d7eee97731812384d4fd00af9032254923363ada | 0 | 0 |

Probe and build-tooling source: 40a8eaf983518fe1992e172434aa8caf347654f1. Baseline51 remains at d7eee97. The clean-engine query followed the successful rebuild and found no actions. It used the conservative EI2 16 GiB commit start threshold; the six builds used EV4 10 GiB, physical 4 GiB, three polls and MaxParallelActions=4.

- 425: 27 files; D:\IA_Europa\Receipts\09421\qualification\final-425-binaries.json; SHA-256 c501895c9856638dd76e1c2091be8726f6797014bcb0714e306fd9d6ebd98df6.
- 51base: 39 files; D:\IA_Europa\Receipts\09421\qualification\final-baseline51-binaries.json; SHA-256 b05e6f8c9ce12fa1c5b4dbd142a59383f737bb8a3b7885b7219d1bcdb6c1fc37.
- 51port: 44 files; D:\IA_Europa\Receipts\09421\qualification\final-regate51-binaries.json; SHA-256 3865fe0c6ed039d5bad2bc54d1a6a89f11c403e98237b4c6caf74ca661ea12e2.

EA8 entry and exit passed against P9 v2; EA11 passed. Full EK1 and launch-second canary receipts are under Receipts/09421. The ET2 marker remains. No runtime launch, owner-kit execution, push, tag or machine setting change occurred. The owner exit is not runtime-proven by this compile-only job; EV7 owns the later owner-leg arming work.
