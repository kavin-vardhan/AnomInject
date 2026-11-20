# 090-04 — Chat ruling on the strict-include fix (NEEDS-DECISION): pull target `fee8c7a`; the 3-line vtable-helper constructor is APPROVED (folded into 090-05); a Shipping strict build is not required (a Shipping-only code read instead)

Date: 2026-09-30 ~13:15 IST · Author: chat Claude · Heads `fix/m52-label-timing` `37e01f0`, `feat/m53-uv-normal-corruption` `fee8c7a` · Report `_mailbox\to_chat\090-04-strict-include-fix.report.md` (143 min)

## Accepted
- **4 includes, and nothing else in `Source/`:**
  - `EngineBaseTypes.h`: the office root error;
  - `Materials/Material.h`: the office C2027;
  - `WeakObjectPtr.h` in `IAnomaly.h`;
  - `IWebSocketServer.h` in the control-server header.
- **The strict tool is a real gate.**
  - It is per-module: unity off, NoPCHs, include order Latest, plus one extra compile unit per header.
  - The unfixed control (run 1) reproduced every office error code at the same sites.
  - It proved each unit compiles alone from its response files, and restored `Build.cs` byte-for-byte on all 4 runs.
- **The normal build is unchanged on both branches.** 0 warnings, zero differing strings (the tool reads 15,269 on a real difference, so its zeros are meaningful), the lever audit passes, suites pass, and archives are re-hashed.
- **Pull target for both office hosts: `feat/m53-uv-normal-corruption` `fee8c7a`** (given to the owner at 13:10). A host on the fix branch takes `37e01f0`.

## Rulings
1. **The 3-line constructor: APPROVED. It goes in 090-05 as part D, on the fix branch first, then merged.**
   - The change:
     - declare `UAnomalyCaptureSubsystem(FVTableHelper& Helper);` in the header;
     - define it in the `.cpp` as `: Super(Helper) {}`, the same body UHT generates today.
   - This is UE's standard way to hold a `TUniquePtr` to a type that is only forward-declared in the header. UHT stops emitting its own copy, and the constructor is then compiled where the three types are complete.
   - **Why now, not "accept 3 errors":**
     - a gate that accepts a known failure is weaker than one that reads zero;
     - we don't know the second office host's exact build setting (Code's claim that it avoids the residual is an inference);
     - the change is behaviour-free.
   - **Evidence it must carry:**
     - the strict build before and after (3 → 0) on both targets, both branches;
     - the generated `.gen.cpp` no longer defining its own vtable-helper constructor;
     - the normal build 0 warnings, with the string scan's only differences explained;
     - suites pass;
     - the checklist box reads **0 errors**, and the "accept 3" wording is removed.
2. **Shipping strict build: NOT required** (a whole-engine build on the scratch host is out of proportion).
   - **Instead, a code read in 090-05:** list every Shipping-only branch in the plugin (`#if UE_BUILD_SHIPPING`, the `#else` of `!UE_BUILD_SHIPPING`, `WITH_EDITOR`-off paths, and the compiled-out bench gate), and show that each uses only types its file already includes.
   - The final delivery package (a normal Shipping build) remains the Shipping compile gate.
3. **Office follow-up:** if the second office host still fails on `fee8c7a`, the owner sends a photo, and chat rules before 090-05 merges. A clean compile there is recorded as the office confirmation of 090-04.

## Next
- **090-05:**
  - F1;
  - the Nanite skip;
  - the readme corrections;
  - **D: the constructor, plus the Shipping-only code read.**

  Fix branch first, then merge into m53; builds. It is released now with its ACK line.
- **Tonight:** the 090-06 bench smoke (idle-gated).
- **Thursday:** the owner's office test on the 090-05 head.
