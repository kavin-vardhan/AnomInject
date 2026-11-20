# 085-00 — Chat ruling: the 5 October 2026 delivery. Owner's scope, the m53 delivery cut (DC), the m52 fix, and the sequence

Date: 2026-09-28 (IST) · Author: chat Claude · Owner decisions confirmed by the owner: "Okay to all your points"

## Owner decisions (product; recorded verbatim in intent)
- **The delivery on Mon 5 Oct must include:** blurry textures (`stuck_low_mip`, with fixed labels) **and** UV and normal texture corruption (`uv_corruption`, `normal_corruption`).
- **D5, the modes shipped:** UV **tile + scramble**; normal **invert + green_flip**. drift, swap, flat and noise are deferred to the next delivery.
- **All-or-nothing per object:** if any slot of the target's components doesn't qualify, the whole event is refused. There is no partial application.
- **m52 and m53 never run at the same moment** in auto-injection.
- **Defaults:** both ship **available**. They are on by default in auto-injection **only if** the owner's Thursday office test shows they fire and look right; otherwise available but off.
- **PC use:** game tests at night and whenever the PC is idle. **Compiling is allowed during the day.**
- **Fallback, if Thursday's office test finds a real m53 problem:** deliver m52 (fixed) and move m53 to a follow-up drop.

## m53 delivery cut: IN
1. **The four modes,** implemented against the authored graphs (Codex confirmed 0 plugin cooks). Other mode names parse and refuse with a named reason; E6 is proven through an actual Apply (P2-10).
2. **The mode-correctness oracle (082-11 P2-1)** for each shipped mode, on fixture cells built to be asymmetric (a fixture cook), with **wrong-mode negatives**. invert checked as green_flip must FAIL, and scramble must be checked against its logged (a, b), excluding no-op pairs.
3. **Scramble:** a CRC over (seed, per-id fire counter), with (a, b) logged.
4. **The auto-pool mode draw under the 082-11 RNG contract:** exactly one draw per m53 attempt, `no_mode_enabled` on an empty set, and none for non-m53 ids. It has a known-answer test (G-SEED per 082-11 P2-8) and full-precision logging.
5. **All-or-nothing:** a new refusal reason for a partial footprint, placed in the ordered decision tree with its precedence stated. It has a Q producer: the MainWorld floor, which qualified 12 of 30 in S1, must now refuse.
6. **The m52 ⟂ m53 exclusion** in the auto-injector, both ways: while either is live, the other is not admitted.
   - A Q row proves both directions.
   - Raw `Apply` / `Auto.FireOnce` outside the FSM is recorded as a limitation (082-11 P2-9).
7. **Levers out of client builds:**
   - every `IAI.Bench.*` lever, `DestroyTarget` included, registers only under `-IAIBench` and is compiled out of Shipping (082-11 P3-1);
   - the bench twins `null_effect` and `solid_swap` are **not listed in the catalogue or dashboard** unless under `-IAIBench`.
8. **The label and restore predicates** from 082-11 P2-10 (G3's per-slot disposition, and so on), as they apply to the reduced gate set.
9. **The S2 harness items:** G-BIND per-frame, the global-half far-strip gate ≤ 2, the TRIP key required, and the active-lifetime block-presence check.
10. **A quick cost reading** on StackOBot (game thread and GPU per fire, render-target bytes).
11. **An office census command,** runnable in the delivery build, that prints the count of eligible targets per id and per refusal reason (counts only, no data). The owner runs it on Thursday.
12. **Docs:**
    - the client readme sections for m53 (modes, refusals, limitations: the 128 MiB cap, all-or-nothing, the MainWorld floor example, the stale-shadow note, no partial application) and for m52's corrected timing;
    - the catalogue;
    - the checklist;
    - a comment strip before merge.

## m53 delivery cut: OUT
Deferred to the next delivery, and listed in the docs:
- drift, swap, flat and noise, and the drift snapshot path;
- the T10 live producer, cancel-before-focus, and the LEVEL-CHANGE / RECREATED levers. For world teardown, S1's END legs cover the same teardown path;
- the P2-3 slot telemetry, which all-or-nothing makes unnecessary for this delivery;
- the m52 co-affected source fix, replaced by the exclusion;
- G-STR calibration, Lyra, the G7 budget sweep, the MainWorld arbiter, and the cause-read list.

## m52 label-timing fix: IN
- **The target:** the labelled window and the target masks equal the frames where the anomaly is **visible**, on both edges.
- **The owner's readings** (`_bates_reads\2026-09-28-m52-label-timing-owner-observation.md`):
  - onset error from 1 frame early to 2 frames late;
  - the offset closes about 5 frames early, with 3 frames still fully held and 2 intermediate.
- The design comes from the 084-01 diagnosis (Code and Codex), ruled by chat, with a can-fail bench gate per edge.

## Release housekeeping: IN
- the dashboard `.env` in the package;
- the readme's host-tools path mismatch;
- the AnomalyBench/fixture exclusion lines;
- hiding the bench twins;
- any uncommitted AnomDash edits on the home PC, committed if they belong in the delivery (to check).

## Sequence (one brief in flight; bench at night; compiling allowed by day)
1. **084-01** (running): the m52 diagnosis, part A (Code and Codex).
2. **084-02:** m52 fix source and build (compile by day). **Tonight:** a bench window with the baseline vs the fix on both edges.
3. **085-01:** a DC plan revision of `2026-09-28-m53-s2.md` (bench-free, short), then **085-02:** DC source and build, then the **Codex source review** (Relay) of the m52 fix and m53 DC.
4. **085-03:** the fixture cells cook, the harness, and the premise window (night), then the gate window (night).
5. **Codex evidence review,** then merges to master, then the **delivery build** and packaging, handed to the owner **Thursday 1 Oct**.
6. **Thu–Sun:** the owner's office test (the census, the labels, how it looks), fixes, and the final build. **Mon 5 Oct:** delivery.

## ADDENDUM (2026-09-28, after the m52 findings): label sync is the product
- **Owner, verbatim in intent:** "Label sync is the most important part of our plugin … if labels aren't synced the dataset, and the tool as a whole, is completely useless for the client."
- **HARD RELEASE RULE:** no anomaly ships unless its labels **and** target masks match the picture on the first and last visible frame, **0 frames off**.
  - It is measured from **pixels**, not from the plugin's own signal.
  - It is measured on realistic content.
  - Its gate is proven to FAIL on a broken build.
  - Circular gates (the label compared with its own driving signal) are banned.
- **The label-sync audit of every anomaly in the delivery,** not just m52. It uses m55's per-frame change evidence and bank or bench pixels, and runs straight after the m52 fix.
- **The office check kit covers every anomaly.** The owner runs it over RDP and reads back, per anomaly: the start offset, the end offset and the wrong-object count. Only numbers come home.
- **Priority order for the week:** label sync (the m52 fix and the audit) comes before m53. If time runs short, m53 slips; sync never does.
- **An anomaly that fails sync and can't be fixed by Thursday:** the owner decides on Thursday whether to pull it. Until then, the goal is to fix everything.
