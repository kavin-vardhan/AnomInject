# REPORT for 069-09-m43-liveness-from-fires-and-unavailable.md
brief: 069-09-m43-liveness-from-fires-and-unavailable.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-03 02:31:56 | finished: 2026-09-03 02:45:34 | duration: 14 min | exit code: 0
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\069-09-m43-liveness-from-fires-and-unavailable.live.txt

---- FINAL MESSAGE (Claude Code) ----
Both defects fixed; nine of ten gate groups pass. **Gate D fails on one counter.** Stopping.

---

# SESSION 069, BRIEF 9 — `m43` ATTEMPT 4: DEFECTS 1 AND 2 FIXED, **GATE D FAILS**

## 1. Defect 1 — cause per index: **one cause for all three, and it is a peek/take race**

`session_index` **29, 43, 89** (`frame_index` 40, 58, 120). All three have `anomalies = 1` and **`M23 ARM` on their own tick** — so `m26` armed, the target mask took the *shared* path, and the render happened. Not "never armed", not a lost readback, not a writer drop.

**The shared path itself was the cause.** Each tick ran `EnqueueDrain()` → `ServiceTargetMask()` (peek) → `CollectResults()` (take, removes). `EnqueueDrain` only *enqueues*; the render thread publishes at an arbitrary moment. **If a result landed after the peek but before the take in the same tick, the target mask never saw it** and the entry stayed pending → counted unavailable. Sporadic by nature — which is why it hit 3 scattered frames, 29 and 43 mid-run.

**Fixed by removing the race, not timing around it:** the target mask now **always arms its own `RequestId`**, which costs nothing since one render already serves every pending arm. `unavailable` **3 → 0**.

## 2. Gates A–F — predicted vs measured

| gate | predicted | measured | verdict |
|---|---|---|---|
| **A** one render/frame | passes == frames with ≥1 arm, never >1 | 103 frames, **max 1 pass on any frame**; `servedArms` 1/2/**3** | ✅ |
| **B** `m26` census OFF | labels byte-identical, `m26` counters identical | `frame_index` Δ **0**, `t` Δ **0**, `view` identical, **all other fields byte-identical**; event set identical | ✅ |
| **C** `m26` census ON | verdict set identical, `framesContributed` ≥ before, lag ≤ 1 | verdict set **identical**; `fc` 4,4,4,4,4,**4** vs m41's 4,4,4,4,4,**2**; lag **24/24 served, 1/1/1.00** (was 22/24, max 3) | ✅ |
| **D** census | `framesPolluted` 0, `batchesLost` 0, **`tagOvertaken` unchanged or lower**, histogram equal | polluted **0**, lost **0**, histogram **identical** (`zero=13 nonzero=64 belowFloor=49`) — but **`tagOvertaken` 0 → 1** | 🔴 **FAIL** |
| **E(i)** coverage | `m+b == 90`, `unavail == 0`, 90 files | **29 + 61 = 90**, **0**, **90 files** | ✅ |
| **E(ii)** bit-exact tie | 0 mismatches | **29 `MASK-TIE` lines, 0 MISMATCH** | ✅ |
| **E(iii)** hidden blanks | identity holds, no wrong blanks | 29 content + 14 blank on fire-active; identity `29+61+0 == 90` | ✅ |
| **E(iv-b)** | 0 violations | **0** (was 2) | ✅ |
| **E(v)** OFF inertness | no dir, no keys, `m26` identical | no dir, 0 `target_mask*` keys; `m26`/veto counters identical | ✅ |
| **E(vi)** key sets | exactly the 2 + 3 | `mask_file`, `mask_value`, the 3 counters; **annotation unmoved** | ✅ |
| **E(viii)** height refusal | no masks, loud echo, unavail == frames | **no dir, 0/0/90** | ✅ |
| **E(ix)** cost | reported | see §5 | ✅ |
| **E(x)** A44 | all strings both encodings | all present utf16/0 ascii; negative control 0 | ✅ |
| **F** `m40` L4-shape | labels byte-identical | byte-identical (same comparison as B) | ✅ |
| **E(vii)** letterbox, **E(x)** shutdown leg | — | **NOT RUN** — stopped at the failure | ⛔ |

🎯 **The latent `m41` defect's effect is now visible in a veto input and repaired:** `StaticMeshActor_49@116` went **`framesContributed` 2 → 4** — the two arms shipped `m41` never served.

## 3. Gate D — the failure, attributed not guessed

```
Census: TAG-OVERTAKEN 'StaticMeshActor_49' tag=242 (component reads
bRenderCustomDepth=1 value=224, expected 1/242)
```

| leg | binary | target mask | `tagOvertaken` |
|---|---|---|---|
| `M41_M41_ON_A` | m41 | n/a | **0** |
| `M43F_M43F_ON` | m43 | **ON** | **1** |
| `M43F_M43F_TMOFF_CENON` | **m43** | **OFF** | **0** |

⇒ **Same binary, census ON, target mask the only variable ⇒ it is the target mask, not the shared pass.** Mechanism: the target mask's tag/restore cycle opens extra ticks in which a live target is *untagged* when `Census::Tick` runs, so the census can claim an actor that `m26` then re-tags with an event value.

⚠ **Nothing downstream moved** — `framesPolluted` 0, `batchesLost` 0, cycle histogram identical, verdict set identical, and the census's own log calls this class "the expected case". **But gate D said "unchanged or lower" and 1 > 0**, so it is reported as a FAIL rather than waved through.

**Candidate fix (next brief's call, not taken here):** hold the target mask's self-tag for the whole fire window instead of one frame, so a live target is never momentarily untagged. ⚠ That touches tag lifetime — `m36`'s closed ruling territory — which is exactly why I am not taking it unprompted.

## 4. SHAs, exe, branch

| | |
|---|---|
| `origin/master` | **`2691f8c`** — unchanged, predictions only |
| branch | `c564c34` → `e036680` (ADDENDUM 3, **before** source) → **`f931d1b`**. Pushed. ⛔ **not merged, not deleted** |
| staged exe | **`99CD7BA3`** (241,169,408 B) — ⛔ not validated |
| archived | `6299A172` → `…m43-attempt3-unvalidated-6299A172`; ⚠ `2800C7EC` (attempt 4a) **not archived**, reconstructible; **`5C073AC9` remains `m43`'s A-side** |
| container | UNCHANGED, no cook · tags none |

## 5. Cost + readback size

`speed_ratio`: `m41` control **1.0000009** · `m43` TM-ON **1.0000001** · `m43` TM-OFF **1.0000017**; sustained ≈ **30.000 fps** on all three. **No per-frame delta is measurable at the shipped paced 30 fps — the pacer absorbs it.** ⚠ That is **headroom, not free**; `speed_ratio` is the instrument on a client box. Readback: **921,600 B** (`1280×720×1`, `PF_R8_UINT`) per armed frame, now once per fire-active frame.

## 6. The three echo branches

**Still only ON is exercised.** OFF and the refusal are now *behaviourally* confirmed (no directory / `0/0/90`), but I did not capture their echo text verbatim from the logs, so I am not quoting them as verified — that lands with the completion turn.

## 7. NEEDS-DECISION

1. **Gate D** — accept `tagOvertaken` 0 → 1 as a benign, fully-attributed perturbation (nothing downstream moved), or require the tag-lifetime fix. ⚠ The latter reopens `m36`'s tag-lifetime ruling and should be briefed as such.
2. ⚠ **Standing recommendation, unchanged and now stronger:** the shared-pass change is **independently valuable** — it repairs a measured `framesContributed` loss in the client's current build (`fc2 → fc4`). If `m43` needs another turn, **split option (a) out and land it on its own.**

**STOP.**
api-equivalent cost (not billed on Max): $20.29 | turns: 31 | running total across all briefs: $252.28
