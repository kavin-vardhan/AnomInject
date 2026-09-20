# Session 080-02 — `m52` `stuck_low_mip` is BUILT and GATED. The lever works; two findings stop it short of GREEN.

**2026-09-20. Branch `feat/m52-stuck-mip`.** ⛔ No merge · no tag · no cook · no office data.
Binary **`A8742A4A`** built == staged == archived. Container quintet BYTE-UNCHANGED.
`master` `3ff88db` untouched; `m51` `53bf725` untouched; CaptureBench untouched.

**VERDICT: NEEDS-DECISION.** Nine gates pass, one is partial, two are unrun, and the
partial one has a named mechanism that is **not chased** (`G120`) and **not fixed in the
same turn as its diagnosis** (the standing stop rule).

---

## 1. What shipped

`stuck_low_mip`, the tenth anomaly. A target actor's streamable textures are held at a
LOW resident mip for the event span — the object goes blurry while everything else stays
sharp — then restored.

**Lever `L4'` as planned** (predictions §1.4): raise the texture's own public runtime
`NumCinematicMipLevels`, call `UpdateCachedLODBias()`, and let the resulting drop in
`MaxAllowedMips` make **the streamer perform the stream-out itself and hold it there**.
Nothing races us, no RHI resource is recreated, nothing blocks the game thread.
The achieved bias is **read back** (`GetCachedLODBias()`) and corrected in a bounded loop
rather than computed and trusted, so a group `MinLODSize` clamp cannot silently change the
depth. Measured on every hold: `predicted_max_allowed=7` == the target, every time.

**Sharing policy `S4` as planned:** gate PER TEXTURE on the VISIBLE set, not per target.
`UPrimitiveComponent::GetUsedTextures` -> eligibility filter -> count other visible
components using each texture -> hold only those at or below `MaxCoAffected` (0).

**Label wiring as planned:** `EAnomalyActiveSource::AnomalyState` with
`IsCurrentlyAnomalous()` = *the mip is measurably below baseline*, so **a frame where the
hold has not engaged is not labelled at all**.

Telemetry turned out to be worth generalising: `IAnomaly` gains one defaulted virtual
`GetTelemetry(FAnomalyTelemetry&)` and the label writer emits whatever key-value pairs an
anomaly puts in the bag. `m53`/`m54` reuse it; the writer needs no per-anomaly knowledge;
a run with no `stuck_low_mip` event gains no key. `AnomalyViewport` needed **no change at
all** — `GetVisibleRenderableActors` was already public.

## 2. Gate results

All legs packaged, MainWorld, 1280x720, binary `A8742A4A`, B1 correctly NOT APPLICABLE
(`G117`, off the calibration target) and every leg accepted on attempt 1.

| gate | reading | verdict |
|---|---|---|
| **G0 build** | `StackOBotEditor` (modular) and `StackOBot` (monolithic), both **exit 0, ZERO warnings** | ✅ |
| **G1** registry + preconditions | catalog **10** entries; `stuck_low_mip \| scope=object \| args: mip_levels:int[-1.0..-]=-1`; default pool still **4** and does NOT contain it; `r.TextureStreaming=1 r.Streaming.UseAllMips=0` **read back from the live console** and echoed at Apply | ✅ |
| **G2** 🚨 can-fail | `IAI.Bench.StuckMipNoHold 1`: `held` **false on 51/51** frames, `observable` **null on 51/51**, **all 7 events `injected=[] affected=[] manifested=false`**, `observable_frames 0`, `stuck_mip_frames_held 0`, `bench_no_hold` flag on every row | ✅ |
| **G3** hold through span | holds engage and reach the floor: resident **11/12 -> 7**, `top_resident_px` **2048/1024 -> 64**, `observable true`, `target_pixels` 32k-38k. **But only 4 of 7 fires held at all, and 22 of 51 window frames.** Zero frames went held->unheld mid-window | ⚠ **PARTIAL** |
| **G4** restore | 7 of 7 reverts **restored=N, left-to-game=0, unresolved=0**. Normal revert and `FinishRun` exercised | ⚠ 2 of 5 exit paths; three UNRUN |
| **G5** onset | **first labelled frame has `held:true` on EVERY manifested event, in BOTH tick orders.** m44 ONSET satisfied BY CONSTRUCTION, as designed. Measured onset latency **1, 2, 3, 4 frames** on the four that landed | ✅ |
| **G6** hitch | m52 leg `speed_ratio` **1.0000017 / 29.99995 fps**; blinking control on the same binary/map **1.0020464 / 29.93873 fps**. m52 is **0.068 ms/frame FASTER** than the control — far inside `+2 ms` | ✅ (see §4 limit) |
| **G7** yield | see §3 — **printed, no threshold** | 📊 |
| **G8** Lyra | **UNRUN** | ⛔ |
| **G9** schema | row keys **13 -> 13** (added 0, removed 0); anomaly keys **12 -> 23**, added exactly the **11** `stuck_mip.*` keys and ONLY inside a `stuck_low_mip` entry (the control leg has none); `run_summary` adds exactly the **8** `stuck_mip_*` keys against a pre-m52 session, removed 0; **`annotation.json` root 4 -> 4, `P6` DOES NOT MOVE**; `label_schema` still **2** | ✅ |
| **G10** both tick orders | native and `SynthTickOrder` are **identical**: fires 7, manifested 4, frames_held 22, window frames 51, onsets `[None,4,2,1,None,3,None]`, same `speed_ratio`. Expected — the hold is streamer-driven, not tick-order-driven | ✅ |
| **G11** verifier | `--label-pixel-gate`: **`NO FAILURE FOUND`, exit 0, NO-TRACE 0** (1 OFFSET-NOTE, 1 PARTIAL, 5 UNASSESSABLE of 7) | ✅ (see §4 limit) |

## 3. G7 — the yield number chat asked for

MainWorld, **600 frames**, auto-pool with **only `stuck_low_mip` enabled**, gates ENFORCED:

```
stuck_mip_fires_applied                 32        events                   32
stuck_mip_frames_held                   29        manifested                6
positive (window) frames               256        non-manifested           26
observable_frames                       17
stuck_mip_refused_shared               177   <-- the S4 gate, dominant
stuck_mip_refused_not_streamable       133
stuck_mip_refused_no_eligible_textures  17
stuck_mip_refused_virtual                0
stuck_mip_refused_imperceptible          0
```

**6 of 32 fires manifested = 18.75 %.** Targets that manifested: `SM_FloorBase` (3),
`SM_rock_02` (2), `SM_rock` (1). Co-affected refusals were `1 other visible component` 57
times, `6` 44 times, `2` and `3` 30 times each, `7` 12 times, `8` 4 times.

⇒ **Against chat's steer (default-on if >= 25 % of visible targets are eligible on
MainWorld), 18.75 % is BELOW the bar. The recommendation is NOT to flip default-on.**
⛔ No default was changed and no threshold was tuned to reach that number.

## 4. Two declared limits on gates that passed

- **G6 measures PACING, not max frame time.** `speed_ratio` and sustained fps are what the
  harness instruments; a per-frame maximum is not recorded. The m52 leg being *faster* than
  its control is **within run-to-run spread** — the honest statement is
  *below the resolution of this instrument* (`G169`), never *no cost*.
- **🚨 G11 CANNOT FAIL FOR THIS ANOMALY IN ITS CURRENT FORM.** The verifier printed
  *"NO-TRACE unavailable for class `stuck_low_mip`"* on every assessable run — m52 is not in
  the tool's NO-TRACE allowlist, so the ONLY failure verdict it can produce is unreachable
  here. **The zero is therefore weaker than it looks and is reported as such.** Adding the
  class to the allowlist is a verifier change with its own gates and was not made here.
  The runs also disclosed `masks: in-span actual, edges extrapolated` — the capture-side
  item the 079 handoff already queued (write masks a few frames either side of every event).

## 5. 🚨 The two findings — reported, NOT fixed in this turn

### F1 — a re-fire on an already-held target refuses: "already at or below the requested resident mip count"

The `G5`-settle leg (`IAI.Capture.Config 8 4 8 4 0`, i.e. K=8) read **ZERO held frames
across the whole session** — worse than K=2, not better, which **refutes** the obvious
"a longer settle fixes onset" hypothesis. The log names the mechanism precisely:

```
stuck_low_mip: SKIPPED '...' - it is already at or below the requested resident mip count
               (resident 7, target 7, floor 7). Holding it would change nothing.      x24
stuck_low_mip: matched 1 component(s) ... with 6 candidate texture(s) but HELD NONE
               - 0 virtual, 6 not streamable or already at the floor ...              x4
```

⇒ after the first hold-and-revert cycle the texture had **not returned** to its
pre-anomaly resident count, so every later fire on the same target correctly concluded
there was nothing to hold, refused, and produced no event.

**Dataset consequence, stated plainly: the first event on a target works and later events
on the same target can silently produce nothing.** No false positive is created — those
fires record no event at all — but yield collapses.

⛔ **MECHANISM NOT ESTABLISHED AND NOT CHASED (`G120`).** One candidate is NAMED, not
claimed: the revert's belt `Asset->StreamIn(baseline, true)` is guarded by
`if (!Asset->HasPendingInitOrStreaming())` and is therefore **silently skipped exactly when
a stream operation is already in flight — which is the likely state right after a bias
change — and nothing logs that it was skipped.** That is a testable hypothesis and a
logging gap; it is not a finding.

⚠ It did not reproduce identically at K=2, where baselines moved (12, 11, 12, 11) and four
of seven fires did hold. So the failure is **conditional**, and what it is conditional on
is unknown.

### F2 — onset latency is 1-4 captured frames and sometimes exceeds the window

At K=2, three of seven fires never held inside their 8-frame positive window. Those events
are marked `manifested:false`, `injected_frames []`, `affected_frames []`,
`observability_measured false`, and counted in `non_manifested_events` — **the m23 F-LABEL
guard doing exactly its job**. The anomaly cannot claim a frame it did not change.

The cause is structural and was predicted in 080-01 §2: `IStreamingManager::Tick()` runs
**after** `RedrawViewports()` (`GameEngine.cpp:1891, 1899-1901`), so a bias set during
`UWorld::Tick` cannot take effect in that frame, and the streamer then works to its own
amortised schedule.

### F3 — a reporting defect of mine, found by reading the evidence

`stuck_mip.resident_mips` and `stuck_mip.baseline_mips` describe the **PRIMARY** texture
while `stuck_mip.held` is **ANY of the armed set**. At si 31-34 that produced
`armed=6 heldN=1 resident=11 baseline=11 held=true` — correct (one of six *is* blurrier)
but it **reads as self-contradictory**. Evidence that looks like it is lying is a defect
even when the value is right. Not fixed this turn; the fix is either per-texture rows or a
`held` that names which texture it refers to.

## 6. Environment

- Binary **`A8742A4A`** (241,468,416 B), built == staged == archived
  (`_binary_baselines\StackOBot.exe.m52-stuckmip-A8742A4A`). Predecessor **`D50DDE78`**
  (the m51 F1 candidate) hash-verified AT ITS ARCHIVE **before** the staged copy was
  overwritten (`A62`).
- Container quintet `67EA1FE0` / `2CEFB8F4` / `E03C6610` + `A16A18A8` / `C70ECDAA`,
  hashed before AND after — **BYTE-UNCHANGED**. Code-only hot-swap, **no cook** (`G103`).
- `A44` on the staged artifact, both encodings: 11 new symbols present in UTF-16, absent in
  ASCII; 3 pre-existing controls present (scan SOUND); 2 invented symbols absent (scan
  DISCRIMINATES).
- Banked: `M52_M52_G1_DISCOVERY_try1`, `M52_M52_G3_HOLD_NAT`, `M52_M52_G2_CANFAIL`,
  `M52_M52_G5_SETTLE8`, `M52_M52_G6G9_BASELINE_BLINK`, `M52_M52_G7_YIELD_AUTOPOOL`,
  `M52_M52_G10_HOLD_SYN`.
- New gotchas **`G265`** (`UTexture::LODBias` is a cooked-platform no-op) and **`G266`**
  (the streamer cancels a foreign stream request, and why unlinking cannot be ordered).

## 7. What is NOT done

- ⛔ **G8 Lyra UNRUN.** The second fixture has not seen this anomaly at all, so the
  virtual-texture `NOT_APPLICABLE` path (`F-e`) has **never fired** — `stuck_mip_refused_virtual`
  read 0 on every StackOBot leg, and a counter that has never fired is not a proven counter.
- ⛔ **G4 covers 2 of 5 exit paths.** Cancel-before-focus, target-destroyed-mid-span and
  level-change-mid-span are UNRUN.
- ⛔ **F1, F2 and F3 are NOT fixed.**
- ⛔ No merge, no tag, no cook, no default flipped, no threshold tuned.

## 8. Next

Chat rules on F1 (is the re-fire refusal a bug to fix, or is per-target-per-session the
accepted behaviour?), on F3's reporting shape, and on whether the 18.75 % yield is worth
raising `MaxCoAffected` above 0 — which would admit unlabelled blurry objects and is a
dataset decision, not a tuning one. Then G8 (Lyra), the three remaining G4 exit paths, and
the fix for whichever of F1/F3 chat wants.
