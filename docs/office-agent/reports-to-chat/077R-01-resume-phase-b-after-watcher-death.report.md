# REPORT for 077R-01-resume-phase-b-after-watcher-death.md
brief: 077R-01-resume-phase-b-after-watcher-death.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-06 10:04:54 | finished: 2026-09-06 11:00:40 | duration: 56 min | exit code: 0
context at last turn: 811755 tokens (81% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\077R-01-resume-phase-b-after-watcher-death.live.txt

---- FINAL MESSAGE (Claude Code) ----
Everything verified and restored. Final report.

---

## 1. Plain-language rundown

The session that died mid-turn on Thursday is finished. Phase B is **built, cooked, fully tested — and deliberately not merged**, waiting on one decision.

The scare that killed Thursday's session turned out to be **nothing to do with our code**. Seven test runs in a row failed a camera-position check right after the cook, and I'd concluded the new build was at fault. I re-ran the *exact same files* today: they passed 4 out of 4. The failures were clustered in one four-minute window on a loaded machine; my "control" test ran *after* that window closed, so it couldn't see the real pattern.

The genuinely important finding is that **the feature as specified was wrong, and our own test caught it**. The plan said: if the GPU still sees the object's depth, mark the frame as "not visible". But a frame did exactly that while the picture plainly did *not* contain the object — I checked the actual pixels. Hiding an object switches off two separate things, and one can lag the other. So the new number tells you reliably when something is *gone*, but not when it's *there*. Left as specified, it would have **deleted correct labels from customer data**. That's now fixed, and every customer-facing document says so.

Everything else passes: 21 test runs, both timing orders, plus both tests on the second game. Two of my own predictions were too strict and one of my own checking tools was wrong — each corrected with a measurement, not an argument.

**Why I stopped short of merging:** one of the nine failure conditions I wrote down *before* starting did trip, on the second game. The pixels show it's harmless and no label is affected — but I wrote that rule to stop exactly the kind of after-the-fact reasoning I'd have to do to dismiss it. That call belongs to you, not to me. Everything is committed and pushed; a one-line decision merges it.

## 2. Step-0 state at resume

| item | found |
|---|---|
| branch / tip | `m49-phase-b` @ `5e45339`, on `master` `21c1e0d` |
| pushed? | **No** — neither commit was on origin |
| **`AnomalyCaptureSubsystem.cpp` (18 lines)** | **Not WIP — the central finding of 077-01.** Removed the `observable` veto + rewrote the log line. Committed as `1c3d62c` with the measurement in the message |
| `docs/gotchas.md` (staged) | `G255`+`G256`, append-only |
| 4 other docs | Phase B text — **found stale against the fix, corrected** |
| `m50_pc7v3.py` (root) | stray from 076, flagged, **not deleted** (out of scope) |
| staged exe | **`A159EDFD` + m46 container** — the bench was frozen **mid-A/B-swap** |
| cook | ✅ **completed**; quartet **already recorded** in `_binary_baselines/README` |
| gates run | only PB-5c A-side + the halted smoke/AEON attempts. PB-1…PB-4, regression, P-C7 v3, Lyra: **never ran** |
| Lyra | `7ecdf5f` (m50), dll built **19:57 — before Phase B existed** |

## 3. Pose finding — **cause: the bench, not the build** (`G257`)

Re-ran the **exact archived artifacts**, interleaved `A,B,B,B,B,A`:

| binary | 09-04 21:07–21:12 | 09-06 |
|---|---|---|
| pre-cook `A159EDFD` | 2/2 | 2/2 |
| **Phase B `2AFD2DDB`** | **0/7** | **4/4** |

Same bytes, opposite outcome ⇒ **(a) cook, (b) Phase B code, (d) container all refuted**; (c) environmental supported. Campaign then ran **21 legs / 26 attempts / zero halts**. Cause of the cluster itself **not established** (`G120`). Fix: none needed — the bench was already correct. **Gotcha `G257`.**

## 4. Gate table — all on `B8AC65D8`, both orders

| gate | reading | verdict |
|---|---|---|
| PB-1 | 96 rows, 0 violating | ✅ |
| PB-2 | 128 rows, max diff **0** (exact) | ✅ |
| PB-3 | probe **174/174**; census tags all `count==drawn` | ✅ |
| PB-4 | lever → **16 both orders** vs 0/96 natural | ✅ |
| PB-5a | `1/0/0/0`, suppressed `3/3/5/5` | ⚠ **miss — mark proven correct on pixels** |
| PB-5b | `0/0` | ✅ |
| PB-5c | 9 → **6**, still firing | ✅ |
| PB-6 | undeclared diffs **NONE**; `frame_indices` identical 4/4 on all 8; `run_summary` 65→68 | ✅ |
| PB-7 | m44 ALL PASS ×10 · MASK-TIE **512/0** · TARGET-PIXELS TIE **294/0** · pairing `NEITHER 0·PREVIOUS 0` ×4 · m50 G1 0 / G2 22 / G6 0 · black-frame PASS · both targets exit 0 zero warnings | ✅ |
| **PB-8 Lyra LG-9** | **`-1` on 43/43, ZERO rows read `0`** | ✅ |
| **PB-8 Lyra Leg A** | **`frames_drawn_unexpected = 7`** (0.27–0.81%) | 🔴 **F2** |

**Pre/post-cook validity:** no B-side leg was valid pre-cook (`G129`); the `1c3d62c` fix rode a code-only hot-swap — verified `git diff --name-only`: one `.cpp`, **zero shaders**.

## 5. Merge status

**PARKED.** `master` `21c1e0d` untouched `== origin`, checkout on `master`, bench restored to `A159EDFD`+m46 (hash-verified). Branch **`m49-phase-b` `9b27b05`, pushed, `== origin`**. Lyra worktree **`1c3d62c`** (left deliberately — the banked legs correspond to it). CaptureBench `472a409`.

## 6. NEEDS-DECISION

1. **Merge `m49-phase-b` or not** — F2 fired on Lyra (7 rows, 0.27–0.81%); pixels show the hides took (hidden 87.4–91.0 vs visible 94.5–97.0); **no label is affected**. F2's wording is host-agnostic but PB-1's scope was StackOBot and PB-8 never mentioned the counter — genuinely ambiguous, and not mine to resolve in my own favour.
2. **Lyra's drawn residual** — chase the mechanism (temporal-upsampler edge is a candidate, not established) or document as a named host limitation?
3. **`m50_pc7v3.py`** in the repo root — delete, or move to `CaptureBench/tools/`?

---- STDERR ----
Background tasks still running after 600s; terminating. Set CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0 to wait indefinitely.

api-equivalent cost (not billed on Max): $53.78 | turns: 122 | running total across all briefs: $1,059.64
