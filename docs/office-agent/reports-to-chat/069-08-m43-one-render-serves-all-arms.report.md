# REPORT for 069-08-m43-one-render-serves-all-arms.md
brief: 069-08-m43-one-render-serves-all-arms.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-03 02:03:14 | finished: 2026-09-03 02:12:10 | duration: 9 min | exit code: 0
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\069-08-m43-one-render-serves-all-arms.live.txt

---- FINAL MESSAGE (Claude Code) ----
The shared-pass change is proven and fixes the latent defect; gate (i) still fails, and the gates found a second bug. Stopping.

---

# SESSION 069, BRIEF 8 — `m43` ATTEMPT 3: OPTION (a) PROVEN, GATE (i) STILL FAILS

## 1. The latent defect — **NAMED, measured both sides, and now fixed**

Per `m26` arm: the tick armed (its `RequestId` = `GFrameCounter`) vs the tick the pass served it (the `M23 PASS` line's own `[GFrameCounter % 1000]` prefix).

| leg | census arms | `m26` arms | served | **UNSERVED** | lag min/max/mean |
|---|---|---|---|---|---|
| banked `m41`, census **ON** (shipped default) | 78 | 24 | 22 | **2** | 1 / **3** / 1.86 |
| banked `m41`, census **OFF** | 0 | 24 | 24 | 0 | 1 / 1 / 1.00 |
| banked `m41`, census OFF (2nd) | 0 | 24 | 24 | 0 | 1 / 1 / 1.00 |
| **this build**, census **ON** | 97 | 24 | **24** | **0** | **1 / 1 / 1.00** |

🚨 **Confirmed:** on shipped `m41`, the census starves `m26`'s arms through the shared FIFO — **2 of 24 arms were never served** and the rest ran up to 3 frames late, so `framesContributed` (a veto input) was coupled to census cycle length. The census-OFF pair is the control that proves the cause. ⚠ **LATENT, not observed to have changed a verdict** — both `m41` legs had identical event sets and `vetoed_events` 0. ⛔ Not "the veto was wrong".

**Option (a) removes it entirely:** 24/24 served, lag exactly 1, zero unserved.

## 2. Gates — predicted vs measured

| gate | predicted | measured | verdict |
|---|---|---|---|
| **A** one render/frame | passes == frames with ≥1 pending arm, never >1 | **103 frames, exactly 1 pass on each of the 103**; `servedArms` = 1 on 73, **2 on 30** | ✅ **PASS** |
| **C** (lag half) | after ≤ 1 for every arm, unserved 0 | **1/1/1.00, 0 unserved** | ✅ **PASS** |
| **E/(i)** coverage | `measured + blank == 90`, `unavailable == 0`, 90 files | **28 + 59 = 87**, `unavailable = 3`, **87 files** | 🔴 **FAIL** |
| **B, D, C (verdict half), E(ii)–(x), F** | — | **NOT RUN** — stopped at the failure | ⛔ |

**Frame-by-frame classification** (90 captured: 43 fire-active, 47 no-fire):

| | with content | blank | no file |
|---|---|---|---|
| fire-active (43) | 26 | 14 | **3** |
| no-fire (47) | **2** ⚠ | 45 | 0 |

## 3. Why it fails — two residuals, and the second is the serious one

**(1) `unavailable = 3`** — `session_index` **29, 43, 89**. Only **89** is end-of-run residue (last captured index), which ruling 3 pre-authorises reporting. **29 and 43 are mid-run and are not yet explained** — so I cannot claim ruling 3's escape covers this.

**(2) 🚨 `session_index` 23 and 47 carry mask CONTENT while their own labels row says `anomalies = 0`.** Both sit immediately after a frame that had a live fire.

**Cause, established:** `m26`'s records **outlive their fire window** and it keeps arming to fill its 4-arm budget, so **"`m26` armed this frame" is not "a target is live this frame"** — and my write filter uses `BuildBaseTagSet()`, which is *every record tag ever*, so an ended event's lingering tag is never zeroed.

⚠ **That means the delivered mask would assert a target on a frame the labels call clean.** A mask that contradicts its own labels is worse than a missing mask, and it is the same failure family as attempt 1's wrong blanks — the artifact stating something untrue rather than stating nothing.

**The fix (next turn):** derive **both** the measure/blank decision **and** the filter set from the **live fires on that frame** — never from `m26`'s arm, never from all-records tags. That also tightens gate (iv) from "event tags" to "this frame's live tags", which is the stronger and correct predicate.

## 4. SHAs, exe, branch

| | |
|---|---|
| `origin/master` | **`2691f8c`** — unchanged, predictions only |
| branch `m43-GATE-i-FAILED-do-not-merge` | `75c8688` → `50463c2` (ADDENDUM 2, committed **before** the source change) → **`c564c34`**. Pushed. ⛔ **not merged, not deleted** |
| staged exe | **`6299A172`** (241,159,168 B) — ⛔ not validated |
| archived | `87D1EA89`, `0D51A7E3` superseded-unvalidated; **`5C073AC9` remains `m43`'s A-side** and the validated `m41` exe |
| container | UNCHANGED, no cook · tags none |

## 5. The three echo branches

**Still only ON has been exercised** (quoted verbatim in the 069-06 report, unchanged). **OFF** and **refused** require gates (v) and (viii), which were not reached. I am not quoting them as verified.

## 6. NEEDS-DECISION

**None on the design.** Option (a) is correct, proven, and worth keeping on its own merits — **it fixes a measured latent defect in shipped `m41` independently of whether `m43` ships**, and that is worth chat's attention as a separable result.

Two things for the next brief, both already diagnosed and neither needing a ruling from me: derive liveness and the filter set from the frame's live fires (defect 2), and explain the mid-run `unavailable` at `session_index` 29 and 43 (defect 1) rather than assuming they are end-of-run residue.

⚠ One flag worth raising now: **if `m43` is deferred, the shared-pass change should probably still land**, because the `m26` starvation it fixes is in the client's current build. That is chat's call, not mine.

**STOP.**
api-equivalent cost (not billed on Max): $14.09 | turns: 18 | running total across all briefs: $231.99
