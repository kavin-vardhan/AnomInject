# 094-15 — Chat ruling: A5 is COMPLETE; the A6 owner leg is authorized; a small job first arms the kit so it is easy to retry and says what it is waiting for

**Date:** 2026-10-26, about 02:30 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Codex's report:** `_reviews\094-15-codex-report.md` (23:05–01:43 IST, 9,475 s, exit 0). Its verdict: `GREEN | A5 readiness complete; six rebuilds and protected-state proofs pass; no runtime launched`.
- **Claude Code's review:** `_reviews\094-15-code-review.md`. 52 OK · 0 GAP · 6 OPEN · 0 stop violations · 0 conflicts.

## Verdict
**A5 is ACCEPTED as complete,** at feature HEAD `3d20b5c`, with probe source `550e9b7`.

**The builds:**
- **Six final rebuilds:** 4.25 Editor and Game; Baseline51 Editor and Game, against `d7eee97`; ReGate51 Editor and Game.
- **Results:** all exit 0, with 0 C++ errors, 0 C++ warnings, 0 engine actions and 0 engine-plugin actions.
- **Warnings:** the only ones are the two accepted user-XML lines on 4.25.
- **The probe** compiled on 5.1 for the first time, from one source in all three hosts (EO4).
- **EI4:** 0 outdated actions.
- **Manifests:** re-bound at 27, 39 and 44 files, and re-hashed by Code. **A6–A8 use exactly these binaries.**

**The rulings held:**
- **EO1:** the reader passed Code's known answers and Code's 9 own can-fails.
- **EO2** was not needed.
- **EO3:** all 9 receipts are correct.
- **EN1** holds on all six makefiles, with both engines' MSVC and SDK pinned.
- **EO6:** the repaired self-test's can-fail works.
- **The three EN5 items are ratified,** each cited in engine source:
  - UBT's toolchain environment;
  - the stale-output cleanup;
  - the makefile refresh.

**Protected state:**
- P9 v2 18,264 of 18,264;
- the protected-state check PASS ×3;
- `m51` equal to P9 throughout;
- engine trees changed only in EG1;
- the canary found 0 times in 2,566 files;
- 0 launches.

## Carried OPENs
1. **The failed first Baseline51 run's makefile has no EN1 proof of its own. Closed:** its UBT boundary passed, and the same target's regenerated makefile passed.
2. **EO1's reader budget is spent. Standing:** any future reader failure uses EO2's fallback.
3. **The 5.1 probe reads the control subsystem by its own path,** and asserts if the class is missing. **For A9:** decide then whether to change it. Nothing exercises it before A9.
4. **EJ1, EJ3 and EJ5:** carried, unchanged.

## Rulings

### EP1. The A6 owner leg is authorized
**Exactly one owner-started 4.25 Editor session** on the Paris host, through the kit:
- the editor opens;
- one Paris mesh preview is opened for 60 seconds;
- the editor is closed with File > Exit, saving nothing.

**Not allowed:**
- Play-in-Editor, cook, commandlet or packaging;
- automated input;
- more than one editor process.

**The kit's authorization record** cites this ruling (EP1).

### EP2. First, arm the kit (job 094-16; no launch)
**Why:** the kit as built refuses until it is armed, and a refused attempt uses up its job folder. A single refusal would cost the owner a full round.

**What the owner sees:**
- **One double-click file, with the same name:** `D:\IA_Europa\Launch\Europa425-Editor-OwnerLeg.bat`.
- **Three armed attempt slots,** each a fresh job root. The kit uses the first unused one.
  - An attempt that stops before the editor starts leaves the next slot usable.
  - **Once any attempt has started the editor, the kit refuses every later attempt** until chat re-arms it.
- **Plain waiting messages.** While it waits, the console prints one plain line a minute naming what is not ready yet: for example, low memory, a busy GPU, or another Unreal program running.
- **A time limit.** It gives up after 45 minutes with a plain message, without starting the editor.

**The quiet-start gate:**
- **The owner leg drops the human-idle condition,** because the owner starts it on purpose.
- **These all stay:**
  - at least 6 GiB of free physical memory;
  - GPU use at most 15 %;
  - no foreign Unreal process;
  - three passing polls.
- **Measured legs keep the full gate,** including human idle.

**Self-tests, with can-fails:**
- all slots used → refuse;
- a launched marker → refuse;
- the slot order;
- the waiting messages;
- the 45-minute limit;
- an unarmed config → refuse;
- a measured-leg profile without human idle → refuse.

**A dry run of the armed kit, on throwaway slots.** The three real slots stay unused, with 0 launches.

**The bound binaries and manifests stay unchanged.** Any change to them is a STOP.

### EP3. After the owner leg (job 094-17, staged when the owner says "done")
**The checks:**
- P9 v2 and the protected state;
- the engine trees: EG1, plus the runtime attribution classes;
- the C: and E: charges (EA4).

**The content admission, against the prediction:**
- the five 5.1 plugin packages are rejected at the header and skipped by the registry;
- no load, resave, thumbnail or cook;
- no Paris content changed.

**The evidence:** the probe's EditorPreview observations, and the editor's own logs (crashes, prompts, warnings).

**Then A6's second half,** the scripted fixture authoring of `EA_Core` and `EA_Travel`, runs as its own job after chat rules on 094-17.

## Next
- **094-16 is queued now:** arm the kit (EP2), dry-run only. Code reviews after Codex exits.
- **In the morning:** chat sends the owner the final plain card, matching the armed kit's real messages.
