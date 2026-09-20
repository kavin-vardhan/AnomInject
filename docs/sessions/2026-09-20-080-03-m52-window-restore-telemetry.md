# Session 080-03 — `m52`: the window starts at the first held frame, the restore is verified, and the verifier finally has a route that can fail

**2026-09-20. Branch `feat/m52-stuck-mip`.** ⛔ No merge · no tag · no cook · no office data ·
no default flipped · no threshold tuned.
Binary **`DC8BA0C1`** built == staged == archived. Container quintet BYTE-UNCHANGED.
`master` `3ff88db` untouched; `m51` `53bf725` untouched; CaptureBench untouched.

**VERDICT: NEEDS-DECISION.** Every ruling is built and measured, and the one gate that could not
fail before now can — and it **fires on the auto-pool leg**. `G11` reads `NO-TRACE 2`, which is a
gate FAILURE and a finding about m52's labels, not a tool problem. Reported, **not tuned**.

---

## 1. What the four rulings became

| ruling | built as | measured |
|---|---|---|
| **R1** window at first held frame | `IAnomaly::HasDeferredOnset()` (defaulted **false**) + a pre-roll in the capture FSM's `Positives` phase that captures the frame and does **not** decrement the window until the fire is measurably held. `IAI.Capture.DeferredOnsetTimeout`, compiled **30** | ✅ every manifested event is a **contiguous run of held frames starting at the first held frame**, on 4 legs, both tick orders, two fixtures |
| **R2** verified restore | revert clears the bias, then each texture below its recorded baseline is tracked and re-asserted every frame from the new `IAnomaly::TickAlways` until the engine's own `GetNumResidentMips()` reaches baseline. `IAI.Anomaly.StuckMipRestoreTimeout`, compiled **120** | ✅ `RESTORE VERIFIED` on every texture; **7 frames on StackOBot, up to 75 on Lyra**; `restore_timeout` **0** on every leg |
| **R3** eligibility | `not_restored` if any candidate texture is still tracked; `already_held` if the requested target is the one currently held | ✅ `not_restored` fired **2** on the bench and **4** on Lyra; `already_held` 0 (the condition did not arise) |
| **R4** telemetry shape | `stuck_mip.textures[]` per-texture rows; scalars renamed `primary_*`; `held_all` and `onset_latency_frames` added | ✅ and it immediately paid for itself — see §4 |

**Every other anomaly is untouched BY CONSTRUCTION, not by measurement.** `HasDeferredOnset()`
returns false for the other nine, so `BurstAwaitsDeferredOnset` returns false and the `Positives`
statement is the pre-m52 one. The blinking control leg reads it back: **row keys 13 → 13, anomaly
keys 12 → 12, added 0 removed 0** against a pre-m52 banked session.

---

## 2. Gate results — binary `DC8BA0C1`, MainWorld, 1280×720, every leg accepted on attempt 1

| gate | reading | verdict |
|---|---|---|
| **G0** build | `StackOBotEditor` (modular) **and** `StackOBot` (monolithic), both **exit 0, ZERO warnings** | ✅ |
| **G2** 🚨 can-fail | `IAI.Bench.StuckMipNoHold 1`: `held` false on every frame, **3 events all `manifested:false` with empty `injected_frames`**, `frames_held` **0**, `onset_preroll_max` **-1**. 🚨 **AND the hold timeout fired — `stuck_mip_hold_timeouts` 2 of 3 fires** (the third was cut by the frame cap). AMENDMENT 2 pre-declared that a can-fail leg which does NOT time out means R1's wait is not wired; it timed out | ✅ |
| **G3** hold | **2 fires, 2 windows of 7 contiguous held frames, `unheld-in-window` NONE, `non_manifested_events` 0.** 080-02 read 4 of 7 fires manifesting and 3 non-manifested; this reads **0 non-manifested on every stuck-mip leg** | ✅ *(with a corrected reading — §5)* |
| **G3b** 🚨 F1 A/B | the 080-02 `K=8` recipe re-run. **A-side (`A8742A4A`, banked): ZERO held frames, 24 × "already at or below the requested resident mip count".** **B-side (`DC8BA0C1`): 4 fires, 4 manifested events, 28 held frames, `refused_not_restored` 0, `already_held` 0, and NOT ONE "already at or below" line.** AMENDMENT 2's branch **(a)** — the defect is fixed, not merely made visible | ✅ |
| **G4** restore, five exits | **4 of 5 measured, 1 UNRUN with its reason** — §6 | ⚠ |
| **G5** onset | first labelled frame **is** the first held frame, window contiguous, on all four legs and **both tick orders** | ✅ |
| **G6/R7** hitch | real instrument, pacing **OFF**: m52 vs blinking control, max **64.089 vs 68.711 ms**, p99 **52.656 vs 59.183**. Excluding the startup transient (si ≥ 30): max **49.671 vs 53.771**. **m52 is faster on every statistic** | ✅ *(with its limit — §7)* |
| **G7** yield | **16 manifested events per 600 frames, against 6 before** — §3 | 📊 |
| **G8** Lyra | hold and verified restore **both work on a second host, including a SKELETAL target**; the shipped `MaxCoAffected 0` refuses **everything** there — §8 | ⚠ |
| **G9** schema | blinking leg **13 → 13** row keys and **12 → 12** anomaly keys, added 0 removed 0 vs pre-m52. Stuck-mip entry adds exactly the **14** `stuck_mip.*` keys. `run_summary` **68 → 83**, added exactly the **15** `stuck_mip_*` keys, removed 0. **`annotation.json` root 4 → 4 AND per-event keys 16 → 16, added 0 removed 0 — `P6` DOES NOT MOVE.** `label_schema` **2** | ✅ *(one count in my own pre-declaration was wrong — §5)* |
| **G10** both orders | native and `SynthTickOrder` **identical**: first held si **35** in both, `n=7` in both, same fires / frames_held / preroll_max | ✅ |
| **G11** 🚨 verifier | the class is now **AVAILABLE** (§9 proves the route both ways). Targeted legs: **NO-TRACE 0, exit 0**. 🚨 **Auto-pool leg: NO-TRACE on 2 events, exit 2 — FAIL** | ⛔ **FAIL** |

---

## 3. G7 — the yield, re-measured

MainWorld, 600 frames, auto-pool with only `stuck_low_mip` enabled, gates ENFORCED.

| | 080-02 (`A8742A4A`) | 080-03 (`DC8BA0C1`) |
|---|---|---|
| bursts | 49 | 23 |
| fires applied | 32 | 18 |
| events written | 32 | 17 (1 vetoed by `m26`) |
| **manifested** | **6** | **16** |
| non-manifested | 26 | 1 |
| manifested / applied fire | **18.75 %** | **88.9 %** |

```
stuck_mip_fires_applied                 18      stuck_mip_refused_shared              76
stuck_mip_frames_held                  119      stuck_mip_refused_not_streamable      52
stuck_mip_onset_preroll_max             19      stuck_mip_refused_no_eligible_textures 3
stuck_mip_restore_frames_max             7      stuck_mip_refused_not_restored         2
stuck_mip_restore_timeout                0      stuck_mip_refused_already_held         0
stuck_mip_hold_timeouts                  0      stuck_mip_refused_imperceptible        0
stuck_mip_textures_awaiting_restore      0      stuck_mip_refused_virtual              0
```

Targets that manifested: `SM_FloorBase` ×6, `SM_rock_02` ×5, `SM_rock` ×3, `SM_SpawnPad_Base` ×2.

⚠ **R1 trades bursts for correct windows, and the trade is visible in the first row:** the
pre-roll makes each burst ~20 frames longer, so 600 frames now hold 23 bursts instead of 49.
**The number that matters for a dataset is manifested events per session, and it went 6 → 16.**

🎯 **Against chat's steer (default-on if ≥ 25 % on MainWorld), 88.9 % clears it by a wide margin.**
⛔ **No default was flipped.** Per R6 and AMENDMENT 2 §A2.5 this is REPORTED to chat and the
default-on decision is the owner's. ⚠ And it should be read beside `G11` and `G8` before it is
taken: two of these very events are the ones the verifier calls `NO-TRACE`.

---

## 4. 🚨 The F1 mechanism, with the log evidence — and 080-02's diagnosis is CORRECTED

080-02 §5 named one candidate: *the revert's `StreamIn` is guarded by
`!HasPendingInitOrStreaming()` and is therefore silently skipped exactly when a stream operation
is already in flight.* **That candidate is NOT what the measurement shows, and it is withdrawn as
the explanation.**

What the instrumented revert actually reports:

```
stuck_low_mip: revert of 6 held texture(s) - restored=6 left-to-game=0 unresolved=0 relinked=0
               already-back=0 awaiting-restore=6 (total tracked 6).
stuck_low_mip: RESTORE VERIFIED 'T_rock_02_N' resident 11 >= baseline 11 after 7 frame(s),
               1 re-asserted stream-in request(s) and 7 frame(s) where the engine was already busy.
```

- The revert-time `StreamIn` **was issued** (the guard did not skip it — the "already in flight"
  line never printed).
- `already-back=0` — **not one of the six textures was back at baseline at the moment the revert
  returned.**
- The restore then took **7 more frames**.

⇒ **The mechanism is LATENCY, not a dropped request.** The old code issued one stream-in and
returned; any fire inside the next ~7 frames read the still-depressed count as its own baseline,
concluded there was nothing to hold, and produced no event. That is the whole of `F1`.

🔻 **Also corrected: 080-02 attributed the `already at or below` skip to a failed restore.** On this
session's first leg that message appeared on the **first two fires of the session, before any hold
had ever happened** — the rock's textures simply had not been streamed in yet. **The message has at
least two causes and the one 080-02 named is not the one that fires first.** The observation in
080-02 stands; the attribution does not.

⚠ **`7 frame(s) where the engine was already busy` is not evidence for the old hypothesis** — those
are the frames during which OUR OWN stream-in was completing. Stated so the number is not read
backwards later.

---

## 5. Two defects in my own PRE-DECLARATION, not in the build

1. **AMENDMENT 2 predicted "exactly `PositiveFrames` = 8 labelled frames". The measured maximum is
   7, and that is a pre-existing FSM property, not a regression.** On the 8th `Positives` tick the
   phase machine captures, decrements to zero and calls `BeginRevert()` **in the same tick**, so
   `FinalizeArmedLabel()` samples an empty live-fire list and that frame carries no anomaly entry.
   Every other anomaly obeys the same arithmetic and gets its 8th frame back at the other end — the
   tick `BeginFire()` ran on is captured and IS labelled for them (m18's `[3..10]`). For a
   deferred-onset anomaly that frame is correctly NOT held, so m52 cannot claim it. ⛔ **Not fixed:
   the alternative is moving `BeginRevert` a tick later, which changes every anomaly's window on a
   shipped and gated system for one frame of a new one.**
2. **AMENDMENT 2 said `run_summary` goes "8 → 16" `stuck_mip_*` keys; measured 15.** The LIST in
   A2.3 named seven additions and 8 + 7 = 15. The list was right and the total was arithmetic.

Both are recorded rather than quietly reconciled, because a pre-declaration that is corrected after
the fact without saying so is not a pre-declaration.

---

## 6. G4 — the five exit paths

| exit | how it was produced | reading |
|---|---|---|
| **normal revert** | every leg | `restored=N left-to-game=0 unresolved=0` + a `RESTORE VERIFIED` line per texture |
| **`FinishRun`** (frame cap with a live hold) | a 600-frame run cut at the cap | `revert of 6 - restored=6 left-to-game=0 unresolved=0 already-back=6 awaiting-restore=0` |
| **cancel before focus** | `IAI.Capture.Start` + `IAI.Capture.Stop` in the same startup batch | `0 frame(s), 0 burst(s)`, **zero fires applied, nothing held** — structurally there is nothing to restore, because fires begin at `BeginActualRun`, which IS the focus resolution |
| **world teardown / level change** | 1800-frame run, `CloseMainWindow()` at ~26 s with a hold live | 🎯 **`Subsystem deinitializing; reverted 1 active anomaly(ies).`** then `revert of 6 held texture(s) - restored=6 left-to-game=0 unresolved=0`, **exit code 0**, `Object subsystem successfully closed.`, zero Fatal/Assertion lines |
| **target destroyed mid-span** | ⛔ **UNRUN — no lever exists and none was built** | structural reason: m52 holds **texture ASSETS**, not components, so destroying the target actor does not destroy the texture and `Revert()` still resolves and restores it. The garbage-collected branch exists, logs `unresolved`, and `unresolved` reads **0 on every leg** — which is a reading about this fixture, not a proof of that branch |

⚠ **Named limit, visible in the teardown line itself:** it ends `awaiting-restore=6 (total tracked
6)`. At teardown the bias IS cleared on every texture, but the restore is **not verified**, because
nothing ticks after the world is gone. That is safe — nothing is holding the mip down any more, and
the runtime state dies with the world — but it is not the same evidence the other exits produce, and
it is stated rather than folded into the pass.

---

## 7. G6/R7 — a real hitch instrument, and what it still cannot see

`t_wall` deltas from `labels.jsonl`, **pacing OFF**, because the pacer sleeps up to `1/VideoFps`
each tick and absorbs exactly the variance a hitch test is looking for (the `m35` `G-M6` lesson —
080-02's `speed_ratio` reading could not have failed).

| window | leg | median | mean | p95 | p99 | max |
|---|---|---|---|---|---|---|
| all 89 frames | blinking control | 17.338 | 23.375 | 51.891 | 59.183 | **68.711** |
| all 89 frames | m52 | 16.815 | 20.239 | 49.521 | 52.656 | **64.089** |
| si ≥ 30 | blinking control | 17.150 | 22.422 | 50.711 | 51.891 | **53.771** |
| si ≥ 30 | m52 | 16.362 | 18.643 | 48.588 | 49.521 | **49.671** |

m52 is **faster on every statistic in both windows**, and both legs' maxima land at the same
session position (si=16), which says the peak is the fixture's startup, not the anomaly.

⚠ **Stated as `G169` requires: the within-build spread is 17 → 69 ms, so this instrument could not
resolve a +2 ms effect either.** What it establishes is that **m52 adds no LARGE hitch**. It does
not establish "no cost", and a negative delta is run-to-run variance, not a speed-up.

---

## 8. G8 — Lyra, and the number that validates the restore timeout

**Leg 1, shipped defaults (`/ShooterCore/Maps/L_ShooterGym`, 120 frames):** 9 fires, **0 held**.
`refused_shared` **27**, `refused_not_streamable` 9, `refused_no_eligible_textures` 9.
⇒ **under the shipped `MaxCoAffected 0`, m52 yields NOTHING on Lyra's ShooterGym.** Every
candidate texture there has a visible co-user. That is the `S4` policy working as designed and it
is a real limit on this anomaly's reach on authored content.

**Leg 2, bench override `IAI.Anomaly.StuckMipMaxCoAffected 8` (150 frames)** — a bench device, no
default changed: **3 fires, 3 manifested events, 7 contiguous held frames each,
`observability_measured true`, 0 non-manifested, 0 vetoed.** Targets `Cube` ×2 and **`SKM_Manny`**
— a **skeletal** target, a class no StackOBot leg has exercised for this anomaly.

🚨 **And the restore number is the finding: `RESTORE VERIFIED ... after 34 frame(s)`, with
`stuck_mip_restore_frames_max` **75**.** StackOBot restores in 7. **The compiled 120-frame timeout
was set before either measurement and survived a host where the restore is ten times slower — with
1.6× of margin, not a comfortable one.** `refused_not_restored` fired **4** times there, which is
`R3` doing on a second host exactly what it was built for.

⛔ **`stuck_mip_refused_virtual` reads 0 on both Lyra legs** across 27+ candidate textures, so the
`NOT_APPLICABLE` path (`F-e`) is **UNEXERCISED and its zero is not a proven counter** — the honest
branch of AMENDMENT 2's pre-declaration. An offline scan finds **332** Lyra assets serialising
`VirtualTextureStreaming`, **four of which (`T_Manny_01_*`) this very leg HELD**, i.e. they resolve
as NOT virtual at runtime. ⚠ The scan's weakness travels with it: the token's presence means the
property was serialised, not that it is true, and not that the runtime backs it with a VT resource.
**The per-texture `IsCurrentlyVirtualTextured()` is what decides and it said no, every time.**

📌 `lyra_leg.ps1`'s default map path is **stale** — see `G268`. CaptureBench was not edited.

---

## 9. 🚨 G11 — the verifier route now exists, and it FAILS on the auto-pool leg

`verify_capture.py`'s NO-TRACE class contract gains a second tier: `stuck_low_mip` is
**HELD-GATED** — eligible only on runs where `stuck_mip.held` is true on **every** labelled frame.
An unheld or missing flag leaves the run `UNASSESSABLE`, which is the verdict the class already
had, so the tool is narrowed rather than trusted. Proven both ways in `--selftest` (**98 cases**):
held rows + no pixel change ⇒ **NO-TRACE, exit 2**; one unheld frame ⇒ UNASSESSABLE; no flag at
all ⇒ UNASSESSABLE.

**Targeted legs: NO-TRACE 0, exit 0.** `G3B_SETTLE8` 4 OFFSET-NOTE; `G10_HOLD_SYN` 2 OFFSET-NOTE;
`G3_HOLD_NAT` 1 UNASSESSABLE (`threshold unsatisfiable: tau=1.5286 >= 1.0` under `m_edge=0.73` —
MainWorld motion, not a class problem).

🚨 **`G7_YIELD`: `VERDICT FAIL: NO-TRACE on 2 event(s)`, exit 2.**

```
run[252..258] NO-TRACE - no change above the noise floor (tau=0.0530) on this target
              across frames 252..258; the label claims a visible change
run[581..587] NO-TRACE - no change above the noise floor (tau=0.0867) ...
```

Both on `StaticMeshActor_UAID_..._2048592804`, asset **`SM_rock`**:
`bbox_px 401×269` (11.7 % of frame), `target_pixels` ~25,000, **3 textures held from baseline 10
to resident 7**, `top_resident_px` **64**, `held: true`, `held_all: true`, `observable: true`.
401 px on screen against a 64 px top mip is a ratio of **6.3**, comfortably above the `4.0`
perceptibility gate — and `stuck_mip_refused_imperceptible` read **0** on that leg, so **the gate
never fired at all.**

⛔ **CAUSE NOT ESTABLISHED AND NOT CHASED (`G120`).** Candidates NAMED, none claimed: the
perceptibility rule's own declared weakness (it assumes the texture maps roughly once across the
object, and a tiling texture repeats); the held set may not include the texture that dominates that
surface's appearance; or a mip drop of 10→7 on low-frequency rock maps is genuinely near the
instrument's floor. ⛔ **The ratio was NOT tuned and no default was moved.**

✅ **The instrument is not blind on this fixture:** the same leg's `SM_rock_02` events — the
targeted rock — return OFFSET-NOTE, i.e. a transition WAS found. It discriminates between the two
rocks in the same session.

🔑 **This is precisely the value R5 bought.** 080-02's `G11` printed a zero and said in writing
that the zero was weaker than it looked, because the only failing verdict the tool could produce
was unreachable for this class. It is reachable now, and the first thing it did was find two
labelled events whose pixels do not support them.

---

## 10. Two defects of MINE, found by reading the artifact and fixed mid-session

1. 🚨 **The per-texture record's `name` field shipped as `"Name"`.** `FAnomalyTelemetry` keyed on
   `FName`, and `FName::ToString()` returns the case of the **first registration anywhere in the
   process** — the engine registers `"Name"` long before any plugin runs. The telemetry bag now
   keys on `FString`. → **`G267`**, and it is the most transferable thing in this session.
2. **`stuck_mip.onset_latency_frames` and the run counter disagreed by one** — 18 against 17 — and
   they were not the same quantity. The row key counts every captured frame from the one the
   anomaly was applied on; the run counter counts only the frames the **window** skipped, which
   begins one frame later. Shipping two near-identical numbers under names that do not distinguish
   them is `F3` again, so the run counter is now `stuck_mip_onset_preroll_max` and
   `client-readme.md` states both intervals.

⚠ **An intermediate binary `FA9C1737` carried both defects, ran one leg and was NOT archived** —
the `m40` `DC16710D` precedent. The loss is bounded and it is rebuildable by reverting two commits;
the leg itself is banked as **`M52B_PREFIX_FA9C1737_NAMEKEY_EVIDENCE`** and is the evidence for
`G267`. **`A8742A4A` (the 080-02 binary and `G3b`'s A-side) is untouched at its archive.**

---

## 11. Observations recorded, NOT chased

🚨 **The `m26` veto and the `m49` observability measurement disagreed about one event.** On
`G3_HOLD_NAT` an event was deleted as `MEASURED_ZERO maxCount=0` while its own `labels.jsonl` rows
carry `target_pixels` **16,020–16,184** and `observable: true` on every frame of its window. Two
measurements of drawn pixels, from the same mask pass, one reading zero and one reading 16k.

**Incidence, stated as an association only:** 2 vetoes across 25 applied fires on `DC8BA0C1`
(`G3_HOLD_NAT` 1 of 2, `G7_YIELD` 1 of 18) against **0 across 46** on the banked `A8742A4A` legs.
⛔ **NO CAUSE ESTABLISHED. It is NOT attributed to R1 and R1 is NOT excluded** — the sample is
small and the pre-roll does lengthen the interval between a fire starting and its first labelled
frame, which is the sort of thing a fixed budget elsewhere could notice. **Not investigated.**
⚠ The direction matters: it DELETES an event the labels call visible, which is the dataset-loss
direction `m26`'s admit bias exists to avoid.

📌 The two extra `labels.jsonl` row keys on stuck-mip legs (`exposure_dip`, `exposure_dip_scope`)
are **m48's conditional keys**, emitted only when a frame's luminance dips. They appear on the m52
leg and not on the blinking control purely because of which frames dipped on MainWorld. Nothing to
do with m52; recorded so the `13 → 15` count is not read as schema movement.

---

## 12. Environment

- Binary **`DC8BA0C1`** (241,508,352 B), built == staged == archived
  (`_binary_baselines\StackOBot.exe.m52-verified-restore-DC8BA0C1`). Predecessor **`A8742A4A`**
  hash-verified AT ITS ARCHIVE **before** the staged copy was overwritten (`A62`), and it is
  **LOAD-BEARING as `G3b`'s A-side**.
- Container quintet `67EA1FE0` / `2CEFB8F4` / `E03C6610` + `A16A18A8` / `C70ECDAA`, hashed before
  AND after — **BYTE-UNCHANGED**. Code-only hot-swap, **no cook** (`G103`).
- **`A44` on the staged artifact, both encodings:** 19 new symbols present in UTF-16 and absent in
  ASCII; **3 pre-existing controls present** (the scan is SOUND); **3 retired symbols ABSENT**
  (`stuck_mip.resident_mips`, `stuck_mip.baseline_mips`, `stuck_mip_onset_latency_max` — so the
  renames actually took, read out of the artifact rather than the source, `G119`); **2 invented
  symbols absent** (it DISCRIMINATES).
- Lyra worktree refreshed to the m52 tip (`git -C … checkout`, detached, never edited);
  **`LyraEditor Win64 Development` exit 0, zero warnings.**
- Banked: `M52C_M52C_G3_HOLD_NAT`, `_G2_CANFAIL`, `_G3B_SETTLE8`, `_G10_HOLD_SYN`, `_G9_BLINK`,
  `_G6_M52_NOPACE`, `_G6_BLINK_NOPACE`, `_G7_YIELD`, plus `M52C_LYRA_STUCKMIP`,
  `M52C_LYRA_STUCKMIP_CO8` and `M52B_PREFIX_FA9C1737_NAMEKEY_EVIDENCE`.
- New gotchas **`G267`** (FName as a JSON key) and **`G268`** (the stale Lyra map path).

---

## 13. What is NOT done

- ⛔ **`G11` FAILS** — 2 NO-TRACE events on the auto-pool leg. Not fixed, not tuned, cause not
  established.
- ⛔ **`G4`'s target-destroyed path is UNRUN** and no lever was built for it.
- ⛔ **`F-e` (virtual textures) is UNEXERCISED on both fixtures.**
- ⛔ **No default flipped** — `MaxCoAffected` stays 0 and `stuck_low_mip` stays out of
  `GAutoPoolDefaultEnabled`, despite 88.9 %.
- ⛔ No merge, no tag, no cook, no office data. `m51` and `master` untouched.

## 14. Next

Chat rules on the `G11` NO-TRACE pair — whether it is the perceptibility ratio's known weakness
(and therefore a calibration campaign with its own anchors, `m30`'s shape) or something about which
textures get held — and on whether 88.9 % takes the default-on question to the owner. The `m26`
veto disagreement in §11 needs a decision about whether it becomes its own unit. Then the
target-destroyed lever and a Lyra map with a genuinely virtual-textured candidate.

⛔ Do not merge, do not tag, do not flip the pool default, and do not tune the perceptibility ratio
to make `G11` green.
