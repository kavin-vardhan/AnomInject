# REPORT for 080-04-m52-perceptibility-retest-destroy-revert-and-coaffected-design.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 080-04-m52-perceptibility-retest-destroy-revert-and-coaffected-design.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-20 21:11:09 | finished: 2026-09-20 22:09:34 | duration: 58 min | exit code: 0
context at last turn: 664082 tokens (66% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\080-04-m52-perceptibility-retest-destroy-revert-and-coaffected-design.live.txt

---- FINAL MESSAGE (Claude Code) ----
All work is committed, pushed, and the checkout is restored. Final state verified.

---

# 080-04 — m52: ratio gate, destroyed-target revert, VT probe

**Binary `5588F6FB`** built == staged == archived. Container quintet BYTE-UNCHANGED. `master` `3ff88db`, `m51` `53bf725`, CaptureBench `472a409` all untouched. Branch `feat/m52-stuck-mip` **`791f8c8`** == origin (2 commits pushed). Checkout restored to `m51`.

## Gates — every leg on one binary, every leg accepted on attempt 1

| gate | exact reading | |
|---|---|---|
| **G0** build | `StackOBotEditor` (modular) **and** `StackOBot`, **exit 0, ZERO warnings**, twice | ✅ |
| **A44** | 13 new symbols utf16-present/ascii-absent · **4 controls present** (SOUND) · **3 invented absent** (DISCRIMINATES) | ✅ |
| **G2** can-fail | held false everywhere, `observable` **null**, 3 events `manifested:false` empty windows, `frames_held` **0**, `onset_preroll_max` **−1**, **`hold_timeouts` = 2 of 3** — AMENDMENT 2's test that R1's wait is wired | ✅ |
| **G3** hold | 2 fires, 2 × **7 contiguous held frames**, `unheld-in-window` none, `restore_frames_max` 7, `restore_timeout` 0, `awaiting_restore` 0 | ✅ |
| **G4e** 🆕 | DESTROYING → TARGET LOST → `restored=6 left-to-game=0 unresolved=0` → `RESTORE VERIFIED` ×6 after 4 frames → `revert_on_destroy` **1**, `awaiting_restore` **0**, **no labelled frame after the destroy tick**, exit 0 | ✅ |
| **G5/G10** | native and `SynthTickOrder` **identical** — first held si **35** in both, n=7 in both, preroll 19 in both | ✅ |
| **G7** yield | 16 fires → 15 events → **15 manifested, 0 non-manifested**; **`too_small_for_ratio` = 9** (read 0 at ratio 4.0) | 📊 |
| **G9** schema | no-stuck-mip run: rows **13→13**, anomaly **12→12**, added 0 removed 0. `run_summary` **68→86** = exactly 18 `stuck_mip_*`. With an event: anomaly **12→28** = exactly 16 `stuck_mip.*`. **`annotation.json` root 4→4, per-event 16→16 — `P6` DOES NOT MOVE.** `label_schema` **2** | ✅ |
| **G11** 🚨 R2 retest | selftest **98 cases OK**. Auto-pool: **`NO-TRACE 0`, exit 0** (080-03: `NO-TRACE 2`, exit 2). Targeted legs: `NO-TRACE 0`, exit 0 | ✅ |
| **G8** Lyra | shipped default: 1 event, **`NO-TRACE 0`**, `refused_shared` 21 all at `co=3`. Override 8: 4 events, `NO-TRACE 3`. `restore_frames_max` **79** vs the compiled 120 | ⚠ |

**R2's route to zero is a refusal, read from the log:** the nine `too_small_for_ratio` refusals are 3 fires × 3 textures on `StaticMeshActor_…_2048592804`/`SM_rock` — the exact actor 080-03 §9 named — at bbox **401.4 px, ratio 6.27 against 8.00**. That band contains both 080-03 failures *and* one non-failure, so it is a conservative filter, not a discriminator calibrated at 6.27.

## Three things you should read before the verdict

**1. R1's premise was wrong and is corrected in writing (AMENDMENT 3 §A3.2).** "Floored at 4 mips" is unreachable: `MaxAllowedMips` is clamped to `NumNonStreamingLODs` and then asserted (`StreamingTexture.cpp:229/233`, `check` at `:236`). The floor is **7** on every candidate ⇒ deepest top mip **64 px** ⇒ 401 px tops out at 6.27 however deep the request. The depth half cannot bite; the **filter** is what works. `-1` now means the deepest achievable, and the 4-mip guard is **inert on both fixtures**.

**2. G4e found a pre-existing defect on its first run (G269).** `ComputeFireActive` returned ACTIVE for any `AnomalyState` fire with **no target actor**, without consulting the anomaly — correct for global `camera_clipping`, backwards for an object-scoped fire whose target just died. A-side `F65A8C90`: `injected=[35..41]` vs `affected=[35,36]`, frames 37–41 carrying no anomaly entry at all. Fixed on `F.bWholeFrameExtent`; only two anomalies are `AnomalyState` and one is the global, so the blast radius is structural. `F65A8C90` is archived and **load-bearing as that A-side**.

**3. 🚨 The ratio is a SIZE test and cannot see texture CONTENT (G270, journal §6).** On Lyra at bench override 8, three events read `NO-TRACE` at ratios **26.76 / 26.76 / 31.59** — 3–4× the gate — on objects 968–1537 px wide holding a low-frequency paint/normal map. **Not a regression** (the 080-03 banked leg reads `NO-TRACE 2` on the old binary; it was never verified). **Not reachable at the shipped configuration** (all three carry `co_affected=4`; `MaxCoAffected 0` refuses them, and Lyra's shipped-default leg reads `NO-TRACE 0`). ⇒ R2's escalation target **m55** now has direct measurement behind it, and `MaxCoAffected 0` is excluding that class by luck rather than design.

## S2′ — designed and costed, not built (AMENDMENT 3 §A3.6)

Stencil pool is the stop: 55 assignable values, m50 reserves 8, `EventClaimed` never released mid-run. Measured co-affected distribution — StackOBot 80 shared refusals with **52 at `co ≤ 4`**; Lyra **21, all at `co = 3`**. A comparable 600-frame session spends **~50–80 tags against a ceiling of 47** ⇒ cap 4 exhausts the pool in one session. Against that: **every one of Lyra's 21 refusals sits inside cap 4**, so S2′ takes `L_ShooterGym` from 0 events to a real yield. ~320 lines plus its own gate campaign.

## Not done, named

No pool default flipped, `MaxCoAffected` stays 0 · **R5's teardown counter is implemented and UNEXERCISED** (no leg ended with a live hold at teardown) · `StuckMipMinResidentMips = 4` is inert and has no knob (deliberate smaller surface, flagged) · **the m26/m49 disagreement is untouched, still 080-05** · Lyra leg 1 gave 1 event where 080-03 gave 0 — reported as variance, not an improvement · `lyra_leg.ps1`'s stale map default (G268) passed explicitly per leg, CaptureBench not edited · no merge, no tag, no cook.

VERDICT: GREEN
api-equivalent cost (not billed on Max): $57.13 | turns: 187 | running total across all briefs: $1,901.62
