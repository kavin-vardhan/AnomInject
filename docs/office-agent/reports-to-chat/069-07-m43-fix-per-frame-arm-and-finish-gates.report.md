# REPORT for 069-07-m43-fix-per-frame-arm-and-finish-gates.md
brief: 069-07-m43-fix-per-frame-arm-and-finish-gates.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-03 01:44:11 | finished: 2026-09-03 01:52:14 | duration: 8 min | exit code: 0
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\069-07-m43-fix-per-frame-arm-and-finish-gates.live.txt

---- FINAL MESSAGE (Claude Code) ----
Gate (i) fails again — for a deeper reason. Stopping as instructed.

---

# SESSION 069, BRIEF 7 — `m43` ATTEMPT 2: **GATE (i) FAILS ON A BOTTLENECK, NOT A SHORTFALL**

## 1. What changed vs `43b901b`

R1's missing half, implemented as ruled:

| change | where |
|---|---|
| **`ArmTargetMaskOwn(int32 SessionIndex)`** — on a captured frame where `m26` declined, arms the mask pass on its **own** `RequestId` space (`GTargetMaskRequestBit \| serial`, bit 61), tagging the live event targets that are **not hidden**; returns false only when every live target is hidden or none is live | `AnomalyCaptureSubsystem.cpp:874-940` (new) |
| the blank path is now reached **only** via that `false` — never from "`m26` declined" | `AnomalyCaptureSubsystem.cpp:790` |
| **`ReleaseTargetMaskSelfTags()`** — tags **only** actors not already tagged, records exactly what it tagged, restores them at the **top of the next mask block**, before `VerifyPendingTags` and before `m26` can arm | `AnomalyCaptureSubsystem.cpp:858-873` (new), called at `:747` |
| own results removed from the SVE map, shared (`m26`) results **peeked without removal** so `CollectResults` is unaffected | `AnomalyCaptureSubsystem.cpp:947-951` |
| bounded 8-attempt final drain, then residual counted **unavailable** and named — never blank | `AnomalyCaptureSubsystem.cpp:3300-3345` |

🔑 **The `m26` safety argument is structural, not hopeful:** an actor `m26` has in flight is *already tagged*, so the "only tag untagged actors" rule skips it; `VerifyPendingTags` (`AnomalyMaskMeasure.cpp:304-344`) therefore cannot observe a change. That was the hazard I checked for first.

## 2. Gates — predicted vs measured

| gate | predicted | measured | verdict |
|---|---|---|---|
| **(i)** | `measured + hidden_blank == 90`, `unavailable == 0`, 90 files | **`26 + 59 = 85`**, `unavailable = 5`, **85 files** | 🔴 **FAIL** |
| **(ii)–(ix)** | — | **NOT RUN** — stopped at the failure | ⛔ |
| **(x) A44** | all new strings both encodings | verified on attempt 1's build; unchanged string set | ✅ (carried) |

Improvement is real but insufficient: measured **16 → 26**.

## 3. The cause — arithmetic, not inference

```
captured frames 90 · mask passes run 106 · m26 arms 24 · census arms 72
```

The mask pass renders **once per frame** and consumes **exactly one** pending arm (`RequestId = PendingArms[0]; PendingArms.RemoveAt(0)`). `m26` + the census issued **96** arms against **106** passes, leaving the target mask **`106 − 96 = 10`** slots for the **~45 fire-active frames** it tried to arm. **~35 of my arms were starved.**

🔑 **Confirmed by attempt 1's own number:** shared-arm-only measured **16**; attempt 2 measured **26** = the same **16** shared **+ exactly the 10** own arms that found a slot. The two runs agree to the unit.

⇒ **R1's per-frame arm is not reachable by adding an arm.** With the census ON it alone wants ~0.8 arms/frame, and the *single pass per frame* is the contended resource. ⛔ The ruling's fallback ("otherwise arm twice and say so") does not help either — a second arm still waits for a pass that does not exist.

## 4. Cost delta / readback size

**Not measured** — gate (ix) was not reached. Readback size at the bench rect is **1280 × 720 × 1 B = 921,600 B** per armed frame (`PF_R8_UINT`, tight rows after the pitch copy).

## 5. SHAs, branch, exe

| | |
|---|---|
| `origin/master` | **`2691f8c`** — unchanged, predictions only |
| branch `m43-GATE-i-FAILED-do-not-merge` | `43b901b` → **`75c8688`**, pushed. ⛔ **not merged, not deleted** |
| staged exe | **`0D51A7E3`** (241,156,608 B) — ⛔ **not validated** |
| archived | attempt 1 **`87D1EA89`** → `StackOBot.exe.m43-attempt1-unvalidated-87D1EA89`; **`5C073AC9`** remains `m43`'s A-side and the validated `m41` exe |
| container | UNCHANGED, no cook · tags none |

## 6. The echo branches

**ON** was captured verbatim in the 069-06 report and is unchanged. **OFF** and **refused** are implemented but **were not exercised** (gates (v) and (viii) not reached), so I am not quoting them as verified.

## 7. NEEDS-DECISION — one, and it is architectural

**The mask pass is a one-arm-per-frame resource, and three consumers now want it.** Options, with what each costs:

- **(a) Serve every pending arm from ONE render.** The mask RT's content depends only on *which actors are tagged at render time* — not on who asked — and each consumer already filters by its own tag set (`m26` by its record's tag, the census by its batch's, the target mask by the event set). So one render could satisfy all pending requests, removing the contention **and reducing total passes**. ⚠ It changes the census's arm→frame correspondence and `m26`'s result provenance, so it needs its own gate set and a fresh `P-C7`/veto proof. **My recommendation, but it is not "the missing half of R1" — it is a change to the shared mask pass and should be briefed as such.**
- **(b) Priority-queue the target mask.** Simple, but it delays `m26`'s arms, which risks their windows expiring and **changing `framesContributed` — a veto input.** ⛔ I would not take this without a gate that proves otherwise.
- **(c) Accept census-dependent coverage** and document it. Cheapest, but it ships an artifact whose completeness silently depends on an unrelated feature's cycle length.

⚠ Whichever is chosen, the honest current state is worth keeping in view: **59 frames are still written blank on this leg, and blank asserts "measured, nothing visible".** Under (c) that assertion would need re-examining too — a frame the target mask could not arm is **unavailable**, not blank, and only option (a) makes "blank" reliably true.

**STOP.**
api-equivalent cost (not billed on Max): $12.77 | turns: 18 | running total across all briefs: $217.90
