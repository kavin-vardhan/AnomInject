# REPORT for 078-06-pairing-instrument-diagnosis.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 078-06-pairing-instrument-diagnosis.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-06 18:18:12 | finished: 2026-09-06 18:34:18 | duration: 16 min | exit code: 0
context at last turn: 766027 tokens (77% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\078-06-pairing-instrument-diagnosis.live.txt

---- FINAL MESSAGE (Claude Code) ----
All verified: source frozen (diff vs `6713f51` empty), `master` untouched, staged candidate restored, container byte-unchanged, no background tasks, owner editor untouched.

---

# 078-06 — H-instrument diagnosis complete

## Commits (branch `m51`, nothing pushed)
| commit | what |
|---|---|
| `265403a` | **prediction record, committed before any leg** |
| `8dc708c` | journal + CLAUDE corrections + contract §9 |

Production frozen: `git diff 6713f51 -- Source Shaders` **empty**; `master` = `ac13700`.

## §1 — what the cheap checks settled before any leg
**Tool drift excluded.** `run_leg.ps1` (`9E8D669F…`) and `m44_pairing_probe.py` (`918EB4B5…`) both mtime **09-03**, CaptureBench `472a409` tracked-clean — all predate the morning run. The current checker re-run offline on the immutable banks **reproduces both eras exactly** (morning `PREVIOUS 0`×4, afternoon `9/10/9`), so the stop-gate did not fire.

**One material recipe difference found — and `_leg_geometry.json` is blind to it.** The morning campaign passes no `-Marker`, so it ran at the default **`Marker 1`**; 078-05 passed **`Marker 0`**. `run_leg.ps1:214` issues `CaptureBench.Marker`, and the geometry JSON has no `marker` field — that is why 078-05's `G184` diff read "identical on every axis". It was identical on every *recorded* axis. Confirmed **from the pixels**: an 8×8 consecutive-frame grid agrees to ~3 decimals in 62 of 64 cells and separates only in the top strip, **45.797/39.998 vs 0.286/0.668** (`G125`). Non-material: `MaxAttempts` 4 vs 3, never consumed. **UNKNOWN:** the 10:18 process/GPU state was never recorded.

## §2 — the experiment (outcome D1)
| leg | recipe | decidable | CURRENT | PREVIOUS | ratio | masks |
|---|---|---|---|---|---|---|
| `M51D_HI_OLD_NAT` | `Marker 1` (morning) | 13 | 4 | **9** | 3.6412 | 19 |
| `M51D_HI_NEW_NAT` | `Marker 0` (078-05) | 13 | 4 | **9** | 3.4256 | 19 |
| *ref* `PB1F_F_PAIR_NAT` | `Marker 1`, 10:18 | 37 | 37 | **0** | 1.0040 | 39 |

**The morning recipe replayed on the same A-side bytes reads `PREVIOUS 9` — identical counter for counter. ⇒ recipe drift is UNSUPPORTED.** `OLD` attempt 1 was a genuine A47 bifurcation, discarded by existing policy and banked; no valid leg was retried for its pairing result.

## §3 — what remains unknown
Not established: an environmental cause; absence of an `F1` contribution; F4, FIFO, render/request identity, the client's cause. The throughput link is an **association only** — 9 legs, ratio ≈1.00 pass / ≥1.59 fail, confounded on time, marker and throughput. **Editor residency alone is already insufficient, measured:** PID 25324 started 11:33:30 (after the 10:18 legs) yet the 17:00 `F1A` legs read 1.0002 with it resident; its 65,263.7 s CPU over ~6.9 h (~2.6 cores) makes it a heavy **named candidate, not a finding**. `M-F1b` backbuffer UNRUN; H-editor/H-load HELD; no owner action requested.

## §4 — documentation corrected
078-05's *"the split is by time / environment"* **withdrawn** → the failure occurs on a binary without `F1`, nothing more. Denominators **33/90 = 36.7 %** and **57 missing requested indices** (written prefix `0..32` contiguous). Backbuffer **UNRUN**. The three post-failure control legs recorded as an explicit **scope deviation / exploratory evidence**, with the remedy stated as a rule requiring pre-authorisation. Chat's `run_brief.ps1` change recorded as **externally made and source-inspected only**.

**One process note:** a first CLAUDE.md edit tripped `G115` — 3181/3178 on a six-line change with visible mojibake, from `Get-Content` decoding UTF-8 as ANSI. Caught by the mandatory pre-commit diffstat check, reverted, redone via `ReadAllText`; the committed diff is **39 lines**.

**Restoration:** staged `D50DDE78` restored and hash-verified, container byte-unchanged, no live processes, owner editor untouched.

**VERDICT: NEEDS-DECISION** — the released H-instrument arm completed as predeclared and returned outcome D1, which removes recipe drift as the explanation but leaves the root cause OPEN. Pairing certification remains blocked, so m51 is not certified. Stopping here per the hard stop; next proposed discriminator is the HELD H-editor arm with the process list logged as a recorded precondition.
api-equivalent cost (not billed on Max): $19.67 | turns: 43 | running total across all briefs: $1,203.67
