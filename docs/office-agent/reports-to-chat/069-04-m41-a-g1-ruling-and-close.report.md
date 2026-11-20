# REPORT for 069-04-m41-a-g1-ruling-and-close.md
brief: 069-04-m41-a-g1-ruling-and-close.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: high
started: 2026-09-03 00:17:59 | finished: 2026-09-03 00:31:02 | duration: 13 min | exit code: 0
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\069-04-m41-a-g1-ruling-and-close.live.txt

---- FINAL MESSAGE (Claude Code) ----
`m41` is closed and on master.

---

# SESSION 069, BRIEF 4 — `m41` SHIPPED

## 1. Ruling 1(a) — the `−4` offset: **run-to-run startup variance, measured, not argued**

The source diff between `0bd1a6d` (m40) and the m41 source contains **nothing that consumes ticks before the first arm**: the preflight is inside the `bCensusEffective` guard (confirmed by its absence from the census-OFF leg's log), the new per-fire line is inside `if (CensusQuery …)`, and console registration and log strings consume no ticks. So the ruling's fallback applied — **one extra launch of each binary, all values reported, no leg discarded, no matching pair picked:**

| leg | binary | `start_frame` | arm@si0 | arm@si89 | span |
|---|---|---|---|---|---|
| `M41_OFF` | m41 `5C073AC9` | 1 | **1** | 120 | **119** |
| `M41_OFF_B` | m41 `5C073AC9` | 1 | **1** | 120 | **119** |
| `M41_OFF_C` | m41 `5C073AC9` | 1 | **1** | 120 | **119** |
| `M41_M40_CTRL` | m40 `C0AD3F91` | 4 | **5** | 124 | **119** |
| `M41_M40_CTRL_B` | m40 `C0AD3F91` | 1 | **1** | 120 | **119** |

🔑 **The m40 binary produced BOTH 5 and 1.** The offset is startup variance present **within the old binary itself** — not a property of m41, not code. The arm **span is 119 on all five legs**: the run is rigidly translated, its internal structure identical.

## 2. P-C7 v2 verdict — **PASS**, both pairs reported

| pair | `frame_index` Δ | `t` Δ | `view.rot` Δ | other fields | `anomalies` differing | v2 |
|---|---|---|---|---|---|---|
| `M41_OFF` vs `M41_M40_CTRL` | **−4**, one constant ✅ | **−0.164644**, one constant ✅ | **5 distinct**, yaw drifting 0 → −0.175 → −0.35 → −0.525 ❌ | byte-identical ✅ | 48/90 | **FAIL, pose conjunct** |
| `M41_OFF` vs `M41_M40_CTRL_B` | **0** ✅ | **0.000000** ✅ | **0,0,0** ✅ | byte-identical ✅ | **0/90** | ✅ **PASS** |
| `M41_OFF` vs `M41_OFF_C` (same binary) | 0 ✅ | 0 ✅ | 0,0,0 ✅ | byte-identical ✅ | 0/90 | ✅ PASS |

The failing pair is **not pose-matched** — the `A47` yaw axis, still drifting through the settle tail — and by `A64` pose match is a **precondition of the pair comparison**, not part of its verdict. **Both legs banked, neither discarded.**

🎯 **The pose-matched cross-binary pair returns byte-identity of `labels.jsonl` except `t_wall`** — stronger than `A-G1` originally asked for. **Substance recorded separately: the event set is string-identical across all five census-OFF legs on both binaries, and differs on the census-ON leg** — census OFF ≡ the pre-census picker, census ON changes selection. That pairing is the census's own positive control.

⇒ **`A-G1` PASSES.** Remaining gates all as reported in 069-03; `B-G1` and `C-G1b` recorded **UNRUNNABLE HERE** and moved to the client cook as required pre-delivery gates.

## 3. SHAs, push, branch, exe

| | |
|---|---|
| `master` HEAD == `origin/master` | **`db2f49b`** ✅ verified equal |
| feat commit (one, squashed) | **`1c10a4f`** — `feat(census): m41 - census on by default (+ translucent rule, host-PP preflight, cycle-relative expiry, coverage assertion)` |
| history | `2543674` predictions → `8fa807d` addendum → `1c10a4f` feat → `db2f49b` docs |
| squash integrity | `git diff 6b150f0 HEAD` **empty** — content-identical to the pre-squash tip |
| branch deleted | ✅ **yes, origin and local.** Remaining origin refs: `master`, `feature/mask-gpu-reduce`, `feature/selection-census`. **No reachable ref to the dead name, either side.** |
| force-push | `--force-with-lease` against the verified tip `6b150f0` |
| exe | **`5C073AC9`** (241,122,816 B) validated; **`C0AD3F91` = m41's A-side, load-bearing**; container unchanged (`2A66CA57`/`A7EF9B12`/`D8009AD7`), **no cook** |
| tags | none |

**Commit-shape call, stated:** I kept the predictions addendum as its **own commit placed before the feat** — predictions must precede the implementation they gate, and folding it into the trailing docs commit would have inverted that ordering in the permanent history.

**Docs landed:** `CLAUDE.md` (m41 close-out as "you are here", both defaults ON, exe, P-C7 v2, 15 keys, tag sequence …→m40→m41, m42 measurement-first, m43 next) · `client-delivery.md` (census keys + the provenance-readout note) · `PRE-DELIVERY-CHECKLIST.md` §1.1 (G-R7(ii) hard precondition, C-G1b and B-G1 as cook-time gates, 15-key read-back, bench-lever grep) · ledger §9.1–9.3 (F correction, fog-card OPEN with no mechanism, the camera-POV defect) · card **Section E** (E-0…E-5, D-5 superseded, E-0(d) with no expected value) · gotchas **G211–G215** · journal 069 §2.

## 4. NEEDS-DECISION

**None.** Two things are queued rather than open: **`m43` (target-ID mask)** awaits its own brief, and **`m42` (persist-tags)** is measurement-first with its premise correction already in the ledger. The two unrunnable gate halves are booked as required client-cook gates, and the Bates read is card Section E.

**STOP.**
api-equivalent cost (not billed on Max): $16.67 | turns: 33 | running total across all briefs: $164.66
