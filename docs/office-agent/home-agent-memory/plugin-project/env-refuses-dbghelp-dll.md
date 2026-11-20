---
name: env-refuses-dbghelp-dll
description: "Since ~03:00 on 2026-09-30 this box refuses to create any file named dbghelp.dll (access denied, E: and D:), so BuildCookRun staging fails after its pak step."
metadata:
  node_type: memory
  type: project
  originSessionId: <uuid>
  modified: 2026-10-13T21:55:58.461Z
---

Observed 2026-09-30 (089-02b): creating a file named exactly `dbghelp.dll` fails with "access denied" in any directory on E: or D:,
including fresh ones; any other name (`dbghelp2.dll`, `x.dll`) copies fine and no visible process holds the file. The Defender log
shows a configuration change at 02:59 and real-time protection turned off at 03:03; 089-02a's cook at ~00:30 staged the file fine.
Cause not established (repo G418).
**Not seen since:** 090-10f2 (2026-10-02) and 093-05 (2026-10-14) cooks staged `dbghelp.dll` normally. Treat it as an intermittent risk; still probe before a cook.

**Why:** every BuildCookRun copies the engine's `ThirdParty\DbgHelp\dbghelp.dll` into staging, so cooks now fail at staging after
the pak / IoStore step, and the delivery build will stop there too.

**How to apply:** before a cook, probe `Copy-Item <engine dbghelp.dll> <fresh dir>\dbghelp.dll`. If refused, a container can still be
taken from the staging `Content\Paks` only when the pak / IoStore step exited 0, verified by the map gate and a container diff.
Never change security settings to get past it; report it to the owner. `-stagingdirectory` to a new folder does not help.
Related: [[scratch-host-build-when-checkout-is-protected]].
