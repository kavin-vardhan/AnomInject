# REPORT for 072-02-m49-phase-a1-observable-labels.md
brief: 072-02-m49-phase-a1-observable-labels.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-04 15:39:13 | finished: 2026-09-04 16:08:49 | duration: 30 min | exit code: 0
context at last turn: 663046 tokens (66% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\072-02-m49-phase-a1-observable-labels.live.txt

---- FINAL MESSAGE (Claude Code) ----
Stopped at the first failed gate, as instructed.

---

## 1. Plain-language rundown

1. **The thing this brief was really about — the onset gap — is solved, and the cause turned out to be an engine behaviour, not a logic slip.**
2. UE broadcasts multicast delegates **in reverse registration order**, so our "sample the labels" handler ran *before* the "create the mask records" handler it depends on.
3. On every anomaly's **first captured frame** the record therefore didn't exist yet, and the lookup silently returned `0`.
4. I confirmed that **before changing any code**: the zero lands on each fire's first active frame and nowhere else — 6 of 6 on Lyra, 6 of 6 on StackOBot, zero false positives.
5. That also corrects 072-01: the "4/4 texture vs 0/2 blink" split is a **consequence** of which frame gets labelled first, not the mechanism. It's systematic and has been in every session since `m43`.
6. Fixed **structurally** — one handler calling the two blocks in an order written in our own code — rather than by swapping two registrations and depending on engine behaviour staying put.
7. **Post-fix reading: 0 of 6.** Both build targets exit 0 with zero warnings, first try. MASK-TIE 35/35, schema v2 emitted, `target_pixels` correct and large.
8. **Then a gate failed, and it was my defect**: `affected_frames` came out empty on all four texture events.
9. Cause is exact and not a threshold to tune — I used the wrong "is this frame active" term, one that is meaningless for texture-type anomalies. The right predicate already exists in the file.
10. **I stopped there and did not fix it.** Code is on a branch named for the defect; master carries only docs; the bench binary is restored.

## 2. The onset-gap mechanism — MEASURED

`FMulticastDelegateBase::Broadcast` (`MulticastDelegateBase.h:163`) — the engine's own comment:

```
// call bound functions in reverse order, so we ignore any instances that may be added by callees
for (int32 InvocationListIndex = LocalInvocationList.Num() - 1; InvocationListIndex >= 0; --InvocationListIndex)
```

`OnWorldTickEndMask` was bound first, `OnWorldTickEndSample` second ⇒ **Sample ran first.** `SampleDeferredActiveState` fills `MaskValues` from `MaskMeasure.GetRecords()` by `(Id, Target, StartFrame)` and **defaults to `0`**; the records are created by `EnsureMaskRecordsForCapturedFrame()` in the *Mask* handler.

**Confirming reading, taken before any code changed** (predicate: *the zero is each fire's first fire-active captured frame and nothing else*):

| session | first-active frames with `mask_value==0` | any other row with 0 |
|---|---|---|
| Lyra `LYRA_SMOKE_01` | **6 of 6** | **0** |
| StackOBot `B20_M45_FULL_NAT` | **6 of 6** | **0** |

**After the fix (`A1_NAT`, same banked recipe): 0 of 6** — tags `227/228/229/252/253/254`. ⛔ Swapping the two registrations was rejected: journal 068 §8 already ruled that an ordering guarantee **cannot be requested, only constructed**.

## 3. `MPP_Outline` — **ANY NON-ZERO**

`bEnableStencilTest` is **present** (non-default ⇒ true); `StencilCompare` and `StencilRefValue` are **absent** ⇒ defaults. `StencilRefValue = 0` inline (`Material.h:940`); `StencilCompare` has no initialiser so it takes the enum's zero value, **`MSC_Less`** (`EngineTypes.h:774`). Engine's own mirror: `case MSC_Less: bStencilTestPassed = (StencilRef < StencilValue)` (`PostProcessMaterial.cpp:159`) ⇒ **runs wherever custom stencil > 0.**

**`G96` both ways**: the same scan *shows* `bEnableStencilTest`, `BlendableLocation`, `MD_PostProcess` — so its silence on the two compare properties is a reading, not blindness.

⇒ **every census tag 200..254 is inside that test in frames the labels call clean.** ⛔ `G120`: the material *runs* there; **no artifact observed**. Caveat: that `MaterialInstanceDynamic_365` is an instance of `MPP_Outline` is strong but circumstantial. **Refuse-or-warn rule PROPOSED, not implemented** — and it matters for Bates too.

## 4. Gate tables

**StackOBot** (`A1_NAT`, native order, accepted attempt 1):

| gate | result |
|---|---|
| **A1-ONSET-JOIN** | ✅ **PASS — 0 of 6** (was 6 of 6) |
| both build targets | ✅ exit 0, 0 errors, 0 warnings |
| A44 (both encodings) | ✅ 16/16 new symbols, controls sound |
| MASK-TIE | ✅ 35 lines, 0 MISMATCH |
| **OBS-1** | 🛑 **FAIL** — `affected_frames` empty on 4 events |
| ONSET · PAIRING (100/50 %) · G7 · G-EDGE · OBS-5 · synth order | **NOT RUN** — stopped |

**Lyra:** **NOTHING RUN** — stopped before it. Worktree not refreshed, still `9083d95`.
**Bates known-answer:** **UNRUNNABLE** — `_bates_reads\` holds only the 2026-09-02 transcription markdown, no session folder.
**`LG-4`:** implemented, **UNEXERCISED** — StackOBot writes no host custom depth (`identical=1`), so the new `HOST-ATTRIBUTED` branch needs Lyra to fire.

**The failure, diagnosed:** `target_pixels` = 48,591 and `frames_condition_lost` = 0, so the false term is `active`. I used `FireActive`, which is meaningless for `FireWindow` ids — `ComputeFireActive`'s fallthrough returns `IsLogicallyHidden`, false for a texture swap. Journal 071 §3.1 says *"the value is never consulted for them"*; I consulted it. **`IsFireLabelledThisFrame` (`default: return true`) is the correct predicate and already exists.** Second consequence: `FramesConditionLost` is gated on the same term, so its `0` is blindness, not a clean read.

## 5. Schema v2 sample

```json
// annotation.json — blink event (correct)
{"anomaly_type":"blink","affected_frames":{"start_frame":16,"end_frame":22,"frame_count":4,
 "span_frame_count":7,"frame_indices":[16,17,21,22]},
 "injected_frames":{"start_frame":16,"end_frame":22,"frame_count":4,"span_frame_count":7,
 "frame_indices":[16,17,21,22]},
 "observable_frame_count":4,"unmeasured_frame_count":0,"observability_measured":true,"manifested":true}
// root: "label_schema": 2

// labels.jsonl — one anomaly entry
{"id":"blinking","target_name":"StaticMeshActor_49","mask_value":227,
 "target_pixels":19551,"observable":true,"bbox_valid":true, ...}
// and on an unmeasured frame: "target_pixels":-1,"observable":null
```

## 6. Commits, hashes, checkouts

- **`master` = `bcdf9f9`**, pushed, `origin/master == master`, **0 tracked modifications**. Contains `5077268` (predictions, committed before any code) + `bcdf9f9` (journal). **No feat commit on master.**
- **`m49-a1-GATE-MISS-firewindow-active` = `8842251`**, pushed, **DO NOT MERGE**.
- **Staged exe restored to `DE65F84A`** = master's binary. Gate-miss build archived as `StackOBot.exe.m49-a1-GATEMISS-1C859DC9`; A-side `DE65F84A` archived and hash-verified at the archive.
- **Container quartet byte-unchanged**: `EF8EB23C`/`A8BFFF88`/`3C026A8D` + `A16A18A8`/`C70ECDAA`. No cook.
- **Lyra worktree** `9083d95`, detached, clean — not refreshed, not rebuilt.
- Banked: `_bench_sessions_bank\A1_A1_NAT`.

## 7. NEEDS-DECISION

1. **The A1 fix turn is one term**: `observable`'s `active` ⇒ `IsFireLabelledThisFrame(F)`, and `FramesConditionLost` must count independently of it. Everything else in A1 is green and can stand.
2. **`LG-4` cannot be gated on StackOBot** — it needs a Lyra leg to fire. Should the A1 re-run make a Lyra leg a *blocking* gate rather than a follow-on?
3. **The `MPP_Outline` refuse-or-warn rule**: refuse the census outright, or warn and disable? It affects Bates as well as Lyra, and it changes selection (a `G140` boundary on both hosts).
4. I should flag one judgement I made unasked: on the gate failure I put the code on a named branch and restored the staged binary rather than leaving master dirty — the journal-040 precedent. If you'd rather the feat sat on master unpushed, say so and I'll move it.
api-equivalent cost (not billed on Max): $44.91 | turns: 130 | running total across all briefs: $799.60
