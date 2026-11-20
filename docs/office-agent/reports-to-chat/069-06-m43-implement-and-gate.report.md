# REPORT for 069-06-m43-implement-and-gate.md
brief: 069-06-m43-implement-and-gate.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: medium
started: 2026-09-03 01:15:44 | finished: 2026-09-03 01:32:45 | duration: 17 min | exit code: 0
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\069-06-m43-implement-and-gate.live.txt

---- FINAL MESSAGE (Claude Code) ----
Gate (i) failed. Stopped as instructed — no fix in this turn.

---

# SESSION 069, BRIEF 6 — `m43` BUILT, **GATE (i) FAILED**, CAMPAIGN STOPPED

## 1. SHAs, exe, push state

| | |
|---|---|
| `origin/master` | **`2691f8c`** — `docs(m43): pre-declared gates and predictions, before any source change`. **Predictions only**, pushed before any source, as required. |
| source commit | **`43b901b`** on branch **`m43-GATE-i-FAILED-do-not-merge`**, pushed. ⛔ **DO NOT MERGE.** |
| diffstat | 13 files, **540 insertions / 30 deletions** — no encoding smell |
| staged exe | **`87D1EA89`** (241,152,000 B) — ⛔ **not validated** |
| archived | **`5C073AC9`** → `StackOBot.exe.m41-census-on-5C073AC9`, hash-verified before the swap. It is the validated `m41` exe **and `m43`'s A-side**. `C0AD3F91` still load-bearing. |
| container | **UNCHANGED** (`2A66CA57`/`A7EF9B12`/`D8009AD7`), code-only hot-swap, **no cook** |
| tags | none |

## 2. Gates — predicted vs measured

| gate | predicted | measured | verdict |
|---|---|---|---|
| **(i) coverage** | `measured + hidden_blank == 90`, `unavailable == 0`, **90 files** | **`16 + 71 = 87`**, `unavailable = **3**`, **87 files** | 🔴 **FAIL** |
| **(x) A44** | all new strings, both encodings | `IAI.Capture.TargetMask` 0/6 · `target_mask/frame_` 0/4 · `mask_map.json` 0/3 · `bTargetMaskDefault` 0/3 · `mask_file` 0/6 · `mask_value` 0/4 · `TARGET MASK` 0/4; negative control 0/0 | ✅ |
| **(ii)–(ix)** | — | **NOT RUN** — campaign stopped at the failure | ⛔ |

**What does work** (built, exercised, not gate-certified): the RT pixels come out of the mask SVE behind a per-arm `bWantPixels` flag with the `m34` GPU-table path untouched · grayscale PNG encode and a `bGrayMask` writer job · **87 files written, correctly named by `session_index`** · `mask_map.json` written · the three `run_summary` counters present · the event-tag filter · the knob, ini key and three-branch echo · the `R3` output-height refusal.

## 3. The failure, with its measurement — and the cause is mine, not the design's

```
Capture(m43): TARGET MASK SUMMARY measured=16 hiddenBlank=71 unavailable=3
M23 ARM lines (m26 event arms) = 24
target_mask files = 87   (72 at 4,102 B = all-zero; 15 with content)
```

**Cause, measured not assumed: I implemented only the SHARED-ARM half of ruling `R1`.** `R1` required a **separate per-frame arm budget with its own `RequestId` space**. This commit arms the target mask **only when `m26` arms**, and `m26` is capped at `MaxArmsPerEvent = 4` per event — **24 arms on this leg**. Every other captured frame falls into the `else` branch and is written as a **blank** mask.

🚨 **That conflates "`m26` declined to arm" with "every target is hidden" — which is exactly the distinction `R2` and the predictions file exist to protect.** A blank mask asserts **MEASURED AND NOTHING VISIBLE**. 71 frames were written that way and most of them have a visible target, so the delivered artifact would state ground truth that is false. ⚠ **A missing file would have been less harmful than a wrong blank**, and the client is training on these.

The three `unavailable` are arms still pending at run end, and they are a second, smaller issue.

**The fix (for the xhigh turn, not this one):** implement the missing half of `R1` — arm the target mask on its **own** `RequestId` when `m26` declines — and reserve the blank path for frames where the live targets are **actually hidden**, read from the game-side state `m40` already samples.

## 4. The StartRun echo, verbatim (ON; the OFF and refused branches were not reached before the stop)

```
=== Capture(m43): TARGET MASK ON FOR THIS RUN - requested on, from COMPILED DEFAULT (on),
output dir '<RunDir>/target_mask' === READ THIS LINE, NOT THE INI. One 8-bit grayscale PNG per
captured frame, numbered by SESSION INDEX; non-zero pixel values are the stencil tags of the ANOMALY
TARGETS visible in that frame and 0 is background. mask_map.json maps value+event to target and
anomaly type. A BLANK png means MEASURED AND NOTHING VISIBLE (a hidden blinking target); mask_file:null
in labels.jsonl means NOT MEASURED - they are different facts. It reuses the m26 pass and does NOT
change the m26 measurement, the veto, or annotation.json. Delivery mode does NOT suppress it.
```

⚠ **On this build that line's own promise is the thing gate (i) falsified** — the blanks are not all "nothing visible". The OFF and output-height-refusal branches are implemented and their text is in the commit, but **neither was measured**, so I am not quoting them as verified.

## 5. NEEDS-DECISION

**None on the design** — `R1`–`R4` are sound and gate (i) did its job. One thing to confirm when the fix turn is briefed: with the separate per-frame budget, the mask pass will arm on **every fire-active captured frame** rather than ≤4 per event, so the full-surface readback lands per frame — the cost `R4` accepted, now paid at ~90 readbacks per 90-frame run instead of ~24. Gate (ix) will measure it; **it is the cost that most likely shows on the client box**, and `G-R7(ii)` is the gate that would catch it at the cook.

**Not done, named:** gates (ii)–(ix) · docs commit · journal 069 §4 · card `E-6` · `_binary_baselines` README updated (done) · no tag · master untouched at `2691f8c`.

**STOP.**
api-equivalent cost (not billed on Max): $30.72 | turns: 91 | running total across all briefs: $205.13
