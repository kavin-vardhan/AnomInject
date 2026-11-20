---
name: zs-syntax-check-without-build
description: "When a build is gated (disk floor, memory), compile-check plugin C++ with cl /Zs from UBT's own response files; writes nothing. Plugin Editor builds write to D:, not E:."
metadata:
  node_type: memory
  type: reference
  originSessionId: <uuid>
  modified: 2026-11-14T02:12:48.670Z
---

095-06b (2026-11-14) could not run the Editor compile (E: 20.7 GiB < 50 GiB floor gated by the brief), so it verified
C++ with a syntax-only pass: take `Plugins\AnomalyInjector\Intermediate\Build\Win64\UnrealEditor\Development\<Module>\Module.<Module>[.N_of_M].cpp.obj.response`,
drop `/Fo`, `/sourceDependencies`, `/c`, `/Z7`, `/Zo`, add `/Zs`, run `cl @rsp` from `D:\UESource\UnrealEngine\Engine\Source`
after vcvars64. ~7-20 s per unity unit (shared PCH). Prove the parse reaches the end with a copy of the unity cpp plus a
trailing `static_assert(false-ish, "canary")`. New .cpp files are not in the old unity lists: build a temp TU that
#includes them with the module's response. Script: code-receipts-095-06b\zs_check.ps1.

Measured fact: the plugin's Editor objects/DLLs live in the plugin folder on D: (Intermediate 16-18 MB per unity obj,
Binaries ~0.5 GiB); only the target makefile sits in the project Intermediate junction on E:. Chat has not yet ruled
whether the E: floor gates an Editor-only compile. Related: [[e-floor-blocks-primary-host-rebuild]].
