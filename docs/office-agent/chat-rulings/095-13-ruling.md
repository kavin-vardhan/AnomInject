# 095-13 — Chat ruling: the m57 review fixes are ACCEPTED; the two decisions are ruled; the heavy run is released behind one short prep brief

**Date:** 2026-11-14, about 13:55 IST · **Author:** chat Claude (orchestrator)
**Input:** mailbox report 095-13 (NEEDS-DECISION on two findings, everything else done; 93 min, $64.51 API-equivalent; fresh session `e3188d21`, xhigh). Light brief: code, Editor-only compiles, offline tests. Nothing ran in the engine.

## What landed
- `feat/m57-lod-spatial-trigger` = `3296576` (pushed). It contains `integ/m56-m58` `e15d06d` (merge `16ed5dd`).
- Every ruled item of 095-11 is committed one by one: B1 (view entry uses a time-based margin), B2 (the distance arm looks ahead to the later of the contrast release and the 7 % coverage size; the arm timeout restarts at the first wrong frame), B3, S1 to S8, the applied notes, the readers, the kit (1.7 prints "not judgeable (spatial lod)"), the docs, gotcha **G582**.
- **Part 1b (owner input):** `IAI.Anomaly.LodEntryFrames` default **30**, range 1 to 90.
- Bench paths rebuilt; CaptureBench `m57-gates` = `ac719f6` (local; it contains `m56-gates`). Null-pair check, clip script, readout and fixture script fixed, each with mutants that fail. The fixture script is syntax-checked only.
- Tests at the final tip: 19/19 pure selftests; LOD trigger selftest 166 checks, 15 mutants fail; m59 selftest 141 checks, 13 mutants fail; label-rule selftest 119; kit selftest 104; pixel gate 101; oracle 35; lever audit PASS (62 names). Eight Editor compiles, all 0 errors and 0 warnings.
- Dashboard (read-only look): it has no inject sender and cannot send a numeric second argument to `lod_popping`. That check is closed.
- Worked numbers (model, not measured): turn-in arms at frame 51, labelled 69 to 98, swap at 99 (84 with 15 frames, 129 with 60); approach on the second pair labelled 128 to 132, swap at 133.

## Accepted without change
- **The deviation:** distance "frames ahead" uses the closing speed in distance. The linear screen-size rate under-counted by up to three times.
- **Decision 2, the bench copies.** They sit on the path's end as B2 asked and can never trigger: their LOD switch is at screen size 0.05, far beyond the path, and their 7 % coverage size is about 3 m away. They stay as an `at_finest_lod` refusal reading. Every distance clip and gate uses the second mesh pair, which rides on the heavy run's cook.

## Decisions
- **D1. The down-pop on retreat defaults to OFF** (`IAI.Anomaly.LodDownPop` / `LodPoppingDownPop`: 1 → 0). The code stays; the readme calls it experimental and not proven.
  - **Why:** it cannot occur on the bench as built. A re-hold needs the object below 0.85 × the swap size while the engine still draws finer detail, and the swap follows the engine's own switch by about five frames, so that band is empty. A label path that has never been seen to work does not ship switched on.
  - The client has not asked for it. The owner's question to the client stays open; if they want it, it becomes its own unit (a new re-hold rule, a fixture that can show it, its own clip and gate).
  - **Clip C4** becomes: one pop on the approach, nothing on the retreat, and no label on the retreat.
  - **Overrules** 095-08 Q6 and 095-13 Part 1b's last line ("on by default for the clips"). The owner can reverse this.
- **D2. The rule's property is documented, not changed.** Under B2 a mesh whose LOD switch happens while it is still small on screen (under the 7 % coverage minimum) gives no `distance_threshold` event at walking speed: the hold would have to last far past its 150-frame cap before anyone could see it. That is the right behaviour (no label for a change nobody can see), and it means the yield of distance events on a real host depends on that host's LOD settings. One readme line; the per-mode event counts in `run_summary` report it. An office reading of the yield comes with the next office pull.
- **D3 (closes 095-12 D1). The bench-only `periodic` mode needs no extra guard.** Outside the bench it is refused `periodic_bench_only`, so it cannot reach a client.

## Process
- **095-14 (light, xhigh, released now with the ACK line):** the D1 default and its docs and predictions; the readme line of D2; ruling 095-12 D2 (the changelog row and the predictions amendment for the texture-event node name); ruling 095-12 D3 (fixture options `pair_cube` and `minlod1`); merge the integration tip into `feat/m57`; compile, test, push; write `code-receipts-095-14\tips.txt`.
- **095-07 (heavy, xhigh, staged behind it with no ACK line, so it starts only if 095-14 ends GREEN):** fixtures in the editor, one cook, the Game exes, **the owner's LOD clips first**, then the gates for m56, m58 and m59 with the settle-exactness table. No m57 gate campaign before the owner's verdict on the clips.
- The owner confirmed at about 12:45 IST that E: has 115 GB free; his heavy apps are closed.
