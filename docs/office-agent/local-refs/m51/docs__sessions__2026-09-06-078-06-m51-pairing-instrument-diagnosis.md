# 078-06 — is the MASK-PICTURE-PAIRING failure recipe drift or runtime state?

**Answer: it is NOT recipe drift, and that is tested rather than argued. The root cause is still
OPEN.** Production stayed frozen at `6713f51`; `master` unchanged at `ac13700`; nothing built,
cooked, pushed, merged or tagged; H-editor and H-load remain HELD and no owner action was requested.

Cold start: §1 what was established before any leg · §2 the one real recipe difference · §3 the
experiment and its result · §4 what is and is not established · §5 the next discriminator.

Predictions: `docs/predictions/2026-09-06-m51-pairing-instrument-diagnosis.md` (`265403a`, committed
**before** any leg). Evidence: `_bench_sessions_bank\M51_PAIR_DIAG_EVIDENCE\` (14 files).

---

## 1. Established BEFORE any leg — the cheap checks came first

**Tool drift is excluded.** `run_leg.ps1` (`9E8D669F…`) and `m44_pairing_probe.py` (`918EB4B5…`) both
carry mtime **2026-09-03**, `m49b_campaign.ps1` **2026-09-04**, and CaptureBench sits at `472a409`
with tracked files clean. All predate the morning run, so neither the runner nor the checker changed
between the two eras.

**The mandatory offline gate passed.** The current, unchanged checker re-run on the immutable banks
reproduces **both** eras exactly:

| leg | decidable | CURRENT | PREVIOUS | `speed_ratio` |
|---|---|---|---|---|
| `PB1F_F_PAIR_NAT` (10:18) | 37 | 37 | **0** | 1.0040 |
| `PB1F_F_PAIR_SYN` / `_NAT_SP50` / `_SYN_SP50` | 38 / 34 / 34 | all | **0** | 1.0000–1.0004 |
| `M51_F1B_PAIR_NAT` (17:26) | 13 | 4 | **9** | 3.0001 |
| `M51_F1CTRL_A_PAIR_NAT` (17:29) | 13 | 3 | **10** | 3.5351 |
| `M51_F1CTRL_B_PAIR_NAT` (17:30) | 13 | 4 | **9** | 3.1823 |

The morning output still reproduces, so the predeclared stop-gate did not fire and a live leg was
authorised.

## 2. The one material recipe difference — and `_leg_geometry.json` cannot see it

The morning campaign (`m49b_campaign.ps1:33`) passes **no `-Marker`**, so it ran at the runner's
default **`Marker 1`**; the 078-05 pairing legs passed **`-Marker 0`**. `run_leg.ps1:214` issues
`, CaptureBench.Marker $Marker` into ExecCmds, and **`_leg_geometry.json` records no `marker`
field** — which is exactly why 078-05's `G184` payload diff reported "identical on every axis". It
was identical on every **recorded** axis.

**Confirmed from the artifact, not the script.** An 8×8 grid of consecutive-frame mean luminance
difference agrees to ~3 decimals in 62 of 64 cells (146.617 vs 146.626; 55.227 vs 55.229) and
separates only in the top strip:

```
morning  row0 = 0.648  45.797  39.998  0.694 ...
078-05   row0 = 0.038   0.286   0.668  0.710 ...
```

That is `G125`'s frame-identity-marker signature. Also differing but **not material to the executed
leg**: `MaxAttempts` 4 vs 3 — a retry budget that neither era consumed, both having been accepted on
attempt 1. Everything else recorded matches. **UNKNOWN and recorded as UNKNOWN:** the machine's
concurrent process set and GPU state at 10:18 were never recorded, so the morning throughput regime
cannot be reconstructed — only its effect (`speed_ratio` 1.0040) is banked.

## 3. The experiment — two A-side legs, OLD then NEW, minutes apart

Both on archived A-side `B8AC65D8…EEAD8` + the frozen Phase B container, hashed before and after,
probe spawn verified live in each leg.

| leg | recipe | decidable | CURRENT | PREVIOUS | NEITHER | no-data | ratio | masks |
|---|---|---|---|---|---|---|---|---|
| `M51D_HI_OLD_NAT` | **`Marker 1`** (morning) | 13 | 4 | **9** | 0 | 27 | 3.6412 | 19 |
| `M51D_HI_NEW_NAT` | **`Marker 0`** (078-05) | 13 | 4 | **9** | 0 | 27 | 3.4256 | 19 |
| *(ref)* `PB1F_F_PAIR_NAT` | `Marker 1`, 10:18 | 37 | 37 | **0** | 0 | 3 | 1.0040 | 39 |

🚨 **The recovered morning recipe, replayed faithfully on the same A-side bytes today, reads
`PREVIOUS 9` — identical to the `Marker 0` recipe, counter for counter.** ⇒ **outcome D1: the
recipe-drift explanation is UNSUPPORTED.** The marker axis changes the verdict by nothing.

**A63 record:** `OLD` attempt 1 was a **genuine A47 pose bifurcation** (`modal_rot 359.3/8.57`, bbox
width 175.7 against `CALIB` 306.1, distinct 4, modal 64 %) — discarded and re-run **by the runner's
existing policy**, banked as `_try1`; attempt 2 accepted at the exact modal pose. `NEW` accepted on
attempt 1. ⛔ **No valid leg was ever retried for its pairing result**, no tolerance was changed and
no runner edit was made.

## 4. What this does and does not establish

✅ **Established:** tool drift excluded; the marker/recipe-drift explanation tested and unsupported;
the failure reproduces on a binary that **does not contain `F1`**.
⛔ **NOT established, per the predeclared D1 and the brief's own ruling:** this does **not** exonerate
the instrument in general, does **not** prove an environmental cause, does **not** exclude an `F1`
contribution, and says nothing about F4, the FIFO mechanism, render/request identity or the client's
cause. **Root cause remains OPEN.**

📊 **The association, stated as an association.** Across 9 pairing legs on both binaries and both
recipes, every leg at `speed_ratio` ≈ 1.00 passes (4, all morning) and every leg at ≥ 1.59 fails (5).
Sustained fps falls 29.9 → 8.2–10.0 and mask yield 39 → 19 in lockstep; SP50 reads 1.59–1.67 against
3.00 at full, a render-cost signature. ⚠ **Confounded on time, marker and throughput together.**

⛔ **Editor residency alone is already insufficient, and that is measured.** Owner `UnrealEditor`
PID 25324 started **11:33:30** — after the 10:18 morning legs — yet the 078-05 `M51_F1A_*` legs ran
at **17:00–17:04 with it resident and read `speed_ratio` 1.0002**. Its accumulated CPU is
**65,263.7 s over ~6.9 wall-hours (~2.6 cores sustained)**, so it is a heavy and plausible load
source, but its per-interval activity at 10:18 / 17:00 / 17:26 was never recorded and cannot be
reconstructed. **A named candidate, not a finding** (`G120`).

## 5. Next discriminator — proposed, not started

The throughput axis is the only one this experiment could not separate, because no load lever was
released. The cheapest honest next step is the **H-editor** arm exactly as the review framed it —
same recipe, editor RESIDENT vs CLOSED, interleaved, with the process list logged at leg start so
"closed" is a recorded precondition rather than a belief. It needs an owner action and is HELD.
⚠ **And its ruling is fixed in advance: even if closure modulates the failure, that alone establishes
neither the FIFO mechanism nor F4 nor the client's cause** — that needs a measurement tied to
request/render identity. A load lever may become a can-fail candidate for the m51 identity-binding
commit; it is not a proved F4 gate today.

**Restoration:** the staged candidate `D50DDE78` (241,401,856 B) was restored and hash-verified at
completion, with the container byte-unchanged and the owner's editor untouched throughout.