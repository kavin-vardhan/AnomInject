# 091-02 — Ruling (ACCEPTED, pushed on the owner's decision; the §0 home game check is deferred): the first office host's 8 compile errors are fixed with include-only changes across the plugin

Date: 2026-10-03 ~13:20 IST · Author: Claude Code Desktop (orchestrator/reviewer) · Implementer: Codex `gpt-6-astra` / `max`, full access · Run `_relay\runs\2026-10-03-091-02-include-compat\` (67 min; declared NEEDS-DECISION because §0 could not run)

## The problem (owner's photo, the first office host, branch `feat/m52-proxy-blur` `51e2fdb`)
- C2065 `MD_Surface` ×3 (`Anomaly_TexCorrupt.cpp:525, :1330`, `TexCorruptTree.cpp:278`).
- C2027 `FSkeletalMaterial` (`TexCorruptTree.cpp:1244`) and `FSkeletalMeshLODInfo` ×3 (`TexCorruptTree.cpp:221, :223`).
- MSB3073 follows from those.

The office engine lays out its headers differently from our 5.1.1: `TexCorruptTree.cpp` already included `MaterialShared.h` and `Engine/SkeletalMesh.h` and still failed. In the local UE 5.7 reference, `EMaterialDomain` lives in `MaterialDomain.h` (absent in 5.1) and `MaterialShared.h` only forward-declares it. The engine version of the first office host is still unknown (asked).

## What was done
- The 8 errors were fixed at their cause (direct defining includes; `__has_include` guards where 5.1 lacks the header).
- **A whole-plugin audit of the same class**, with 5.1.1 and 5.7.4 headers as ground truth: 45 files / 317 inserted lines / 183 include directives from 47 headers on m53 and proxy; 38 files / 242 lines on the fix branch.
- Builds: editor and game 0/0 on all three sources; strict include build 0/0 (one first-candidate strict failure, `RHIResources.h` needing `RHI.h` first, was fixed and the run repeated). Lever audit 53/53. String scan: zero new semantic strings (two PDB-GUID byte runs only). Archives `D:\IA_BankOverflow\_binary_baselines\09102-*`, re-hashed 6/6 each.

## My review
- **Diff check, re-run independently on all three branches:** every added `Source/` line is an `#include` or a `#if __has_include(` / `#else` / `#endif` guard; **0 removed lines**; the only other files are 3 docs files; each branch is a fast-forward from origin.
- Codex correctly **withheld the push**, because §0 (the in-game home check) could not run: the owner's UE 5.7 editor was open, the GPU was at 34–36 % and the person was active. That was the right call under the brief.

## Owner decision (~13:20 IST)
**"Push now, check later."** The orchestrator pushed (fast-forward):
- `fix/m52-label-timing` `12a6db8..8b86f43`
- `feat/m53-uv-normal-corruption` `b8cd3cd..cf16766`
- `feat/m52-proxy-blur` `51e2fdb..e09fa9c`

master `b5f15a3` is untouched.

**The office pull is now `feat/m52-proxy-blur` `e09fa9c`.**

## Deferred §0
- `_relay\wait_idle_then_resume.ps1` (detached; PID in the run folder's `idle-waiter-pid.txt`; log `idle-waiter.log`) waits for the idle gate, then resumes the same Codex session with `resume-prompt-1.txt` to run the predeclared ten legs and push the docs only.
- **Before launching any other implementer job, create `CANCEL-WAITER` in that run folder** (one implementer at a time).
- If any §0 leg fails, the owner is told at once and the office readings from `e09fa9c` are treated as provisional.

## Process note
`tools/strict_include_build.py` is blind to engine-version header layouts (Codex's G460). The audit used the local 5.7 headers as a second layout. A compile against the office engine is the only full proof, and that is the owner's compile today.
