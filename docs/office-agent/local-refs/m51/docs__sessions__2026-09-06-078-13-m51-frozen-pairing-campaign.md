# 2026-09-06 — session 078-13 — `m51` frozen-pairing campaign EXECUTED, and STOPPED at leg 2

**The campaign ran and it stopped on its own stop rule. Leg 1 (the fresh A-side reference) is VALID
and PASSES. Leg 2 (the first candidate cell) HALTED after four consecutive `B1`-invalid attempts and
was given NO VERDICT — by the runner and by this record. That is the attempt-exhausted stop class:
it releases NO A control, and no remaining candidate cell was run.**

⛔ **No source, shader, fixture, harness, checker or schema change. No build, no cook, no third
binary, no push, merge or tag. Production frozen at `6713f51`; `master` == `origin/master` ==
`ac13700`; `m51` HEAD was `d727aec` at every leg and moves only by this session's docs commit.**

> 🔻 **CORRECTED 2026-09-06 BY BRIEF 078-14 — READ THIS BEFORE THE SECTIONS BELOW.**
> Codex's independent artifact review (`_reviews/078-13-codex-independent-artifact-review.md`) and
> correction findings (`_reviews/078-14-codex-report-correction-findings.md`) identified **five
> evidentiary overstatements in this journal as first written**. They are corrected **in place**
> below, each marked 🔻, and the authoritative dated correction is
> **`docs/OPERATING-CONTRACT.md` §9, entry `078-14`**. ⛔ **The campaign outcome is UNCHANGED** —
> the stop stands, no leg was re-run, no gate was loosened, and no negative result was erased. What
> changed is **what this record claims the evidence shows**:
> **(1)** `NO-DATA` is **missing probe evidence**, not "a legitimate mask outcome" ·
> **(2)** the reference's one unavailable effective-mask frame is **not** established as the class
> `F1` addresses · **(3)** view rows **do change within tries 1 and 3**, `B1` does **not** read
> camera data alone, the failing predicate there is the **64 % modal coverage** (`distinct=2`
> *passes* the `<= 3` test), and no cause — including "genuine A47 bifurcation" — is established ·
> **(4)** `m44_pairing_probe.py` uses **centroids**, not `TAU`, so `P8` is not why no pairing number
> exists · **(5)** there are **EIGHT** receipt files, not seven.

---

## 0. What this session was released to do, and what it did

Brief `_reviews/078-13-run-frozen-pairing-campaign.brief.md`, released by
`_reviews/078-13-codex-amendment-acceptance-and-execution-release.md` after Chat's conditional
approval (`078-11-chat-prediction-review.md`) and Codex's disposition (`078-12-…`). The released
recipe is `docs/predictions/2026-09-06-m51-frozen-pairing-recheck.md`, verified at execution time as
**SHA-256 `8D7951F542D61658738D8ED4716D98FCCF563C10ABDCCF2EFBDFF05197D2E9B5`, 691 lines**, governed by
`AMENDMENT 2` then `AMENDMENT 1` then §§0–10.

**Two legs of a possible six ran. Two of the five main cells are therefore resolved; three are
UNRUN, and all four conditional controls are UNRUN and UNRELEASED.**

---

## 1. THE RESULT ROWS (verdict, counts, throughput — one row each, `A2.3`)

| leg | binary | order | SP | phys. attempts | validity | PAIRING | decidable / CURRENT / PREVIOUS / NEITHER / same-pos / no-data | sustained fps | `speed_ratio` |
|---|---|---|---|---|---|---|---|---|---|
| **`F1R_A_REF_NAT`** | A `B8AC65D8` | natural | 100 % | **1** (accepted try1) | **VALID** | ✅ **PASS** | **33 / 33 / 0 / 0 / 0 / 7** | **29.9237** | **1.002550** |
| **`F1R_B_NAT`** | candidate `D50DDE78` | natural | 100 % | **4** (all discarded) | ⛔ **INVALID — attempt-exhausted (`B1`)** 🔻 | **NO VERDICT OFFERED** | **UNKNOWN — no accepted session, released checker recipe not runnable** | **UNKNOWN** (per-attempt below) | **UNKNOWN** (per-attempt below) |

**Failing pairing indices: NONE on leg 1** — `PREVIOUS` and `NEITHER` are both empty sets, so there
is no index list to print. **Leg 2 has no pairing table at all** (see §3.3 for why that is structural
and not a choice).

⚠ **Leg 2's throughput is reported per physical attempt rather than as a leg value, because there is
no accepted session to read it from.** All four attempts ran at essentially the target rate:

| attempt | sustained fps | `speed_ratio` | frames / rows / PNGs | events | schema | probe | synth | marker | internal/unscaled viewrect | zero-byte |
|---|---|---|---|---|---|---|---|---|---|---|
| try1 | 29.9819 | 1.000605 | 40 / 40 / 40 | 4 | 2 | spawned, 40 STEP | off | 1 | 1280×720 / 1280×720 | 0 |
| try2 | 29.9998 | 1.000006 | 40 / 40 / 40 | 4 | 2 | spawned, 40 STEP | off | 1 | 1280×720 / 1280×720 | 0 |
| try3 | 29.9855 | 1.000485 | 40 / 40 / 40 | 4 | 2 | spawned, 40 STEP | off | 1 | 1280×720 / 1280×720 | 0 |
| try4 | 29.9394 | 1.002026 | 40 / 40 / 40 | 4 | 2 | spawned, 40 STEP | off | 1 | 1280×720 / 1280×720 | 0 |

---

## 2. LEG 1 — `F1R_A_REF_NAT`, the fresh A-side reference: VALID and PASS

Accepted on **attempt 1**, `pose_match=True`, `modal_rot=(0.0, 0.0, 0.0)`,
`bbox=(0.0, 485.2, 306.1, 234.8)` — **exactly `CALIB_BBOX`** — `rows=25 distinct=1 modal=100.0%`,
`start_frame=1`, focus at 1.9 s (no timeout ride).

**Checker (unchanged `m44_pairing_probe.py`, exit 0, read not assumed):**
`TOTALS decidable=33 CURRENT=33 PREVIOUS=0 NEITHER=0 (same-pos=0 no-data=7)` ·
`VERDICT: MASK IS THE CURRENT RENDER on every decidable frame`.
No-data frames were `si` 15, 24, 25, 26, 32, 34, 35.
🔻 **CORRECTED (078-14, finding 1).** This originally read *"a legitimate mask outcome, not an
artifact-integrity violation"*. **`NO-DATA` IS MISSING PROBE EVIDENCE** — the frame carried no
picture-and-mask centroid pair, so the gate could not decide it. What the record supports is only
this: the prediction permits undecidable frames **provided** `decidable >= 20` and the separate
artifact-integrity conditions pass (`A1.3` scopes those conditions and says a `no-data` frame does
not violate them), and both held here. ⛔ **`NO-DATA` does not certify correct or legitimate mask
production on those frames**, and the fact that all **34** promised `mask_file` references exist on
disk **cannot certify the readings on the seven frames that produced none**.

**Every validity condition met, checked individually:**

- **coverage floor `decidable >= 20` (`A2.1`): 33 ✅**
- **counted events `>= 3`: 4 ✅** (`label_schema 2`)
- **artifact integrity ✅** — 40 `labels.jsonl` rows with `session_index` 0..39, 40 PNGs
  `frame_00000..frame_00039`, **no duplicate, no missing, no orphan RGB, no row without an RGB, no
  zero-byte file**, image dimensions **1280×720 on all 40**.
- **live probe ✅** — one `MASK-PAIRING PROBE SPAWNED tag=255` line and **40 `PROBE STEP` lines**,
  read from the leg's own `anomaly_log.txt`.
- **mask references ✅** — 34 rows carry a non-null `mask_file`, **every referenced file exists on
  disk**, zero zero-byte masks; `mask_state` = present 34 / empty 5 / unmeasured 1.
- **A63 / B1 ✅** — accepted on attempt 1, no focus-timeout ride.

**Effective configuration, read back from the leg's own banked `_leg_game.log` (A48), never from the
typed command:** grab point `SVE/scene-colour (UI-free)` (`sve=1 sveCapturer=1 sveExtension=1
backbufferCapturer=0 forceMiss=0`) · mask **ON** · target mask **ON** (compiled default) · census
**ON** floor 0.50 % ceiling 25.00 % · `m28` requested output height **0 (native)** · run log **ON
(auto, delivery off)** · **`m40 SYNTH TICK ORDER = off`** ⇒ natural order · `CaptureBench.Marker = "1"`
· `r.DefaultFeature.AutoExposure = "0"` and `r.EyeAdaptationQuality = "0"` (both `LastSetBy: Console`)
· StartRun banner `mode=targeted[blinking on StaticMeshActor_49] delivery=off clock=wall seed=4242
fmt=png capture=sve/scene-colour fps=30(fixed-step, paced) K=2 L=0 pre=4 positive=8 post=4
frameCap=40`.

🔑 **The 100 % arm is proven from RENDERED GEOMETRY, exactly as `A1.3` requires and not from a cvar
echo:** every `M23 PASS` line reads `internalViewRect=(0,0)-(1280,720) 1280x720
unscaledViewRect=1280x720`, one distinct pair across the whole leg, and **no `r.ScreenPercentage`
echo exists** on a 100 % leg — which `A1.3` had already established would be the case and forbade
requiring.

📌 **Two readings recorded, neither a gate.** `key_ring_missed 0` / `key_ring_corrupted 0` /
`vetoed_events 0` / `non_manifested_events 1` / `ticks_per_captured_frame 1.375` /
`frames_exposure_dip 0`. And on this **pre-`F1`** binary the leg carries
**`target_mask_frames_unavailable = 1`** with one `mask_state: unmeasured` row.
🔻 **CORRECTED (078-14, finding 2).** This originally called that frame *"the very class `F1`
addresses"*. **That equivalence is withdrawn.** `F1`'s tested change concerns **requested-but-
INEFFECTIVE** target masks: a snapshot that waits for an outcome that can never be produced, losing
RGB and label rows. **This leg ran with the target mask EFFECTIVE and ON** (`Capture(m43): TARGET
MASK ON FOR THIS RUN`), so it is not that condition. *Mask unavailable / unmeasured* is a **broader
outcome with several possible routes**, and ⛔ **this run establishes no cause for it.** The
observation is preserved exactly as measured — **one frame, one leg** — and no comparison is drawn:
the candidate cell that would have supplied the other side never produced a valid leg.

---

## 3. LEG 2 — `F1R_B_NAT`, the first candidate cell: HALTED, NO VERDICT

### 3.1 What the runner did

`run_leg.ps1` exited **2** with `HALT: 4 consecutive invalid attempts on F1R_B_NAT. No verdict is
offered.` **Every one of the four attempts failed the B1 pose gate**, and every one was banked.

| attempt | `start_frame` | focus at | `modal_rot` | measured bbox | `rows` | `distinct` | `modal` | `pose_match` | verdict |
|---|---|---|---|---|---|---|---|---|---|
| try1 | 1 | 1.3 s | (0.0, 0.0, 0.0) | (0.0, 485.2, 306.1, 234.8) | 25 | **2** | **64.0 %** | **True** | POSE GATE FAILED (B1) |
| try2 | 1 | 1.3 s | **(338.54, 358.63, 0.0)** | (9.9, 240.4, 342.9, 230.4) | 17 | 1 | 100.0 % | False | POSE GATE FAILED (B1) |
| try3 | 1 | 1.6 s | (0.0, 0.0, 0.0) | (0.0, 485.2, 306.1, 234.8) | 25 | **2** | **64.0 %** | **True** | POSE GATE FAILED (B1) |
| try4 | 1 | 1.4 s | **(358.05, 349.65, 0.0)** | (173.5, 449.0, 271.1, 238.9) | 24 | 2 | 62.5 % | False | POSE GATE FAILED (B1) |

🔻 **§3.1 IS CORRECTED IN FULL BY 078-14 (finding 3).** As first written this paragraph claimed
attempts 2/4 were *"genuine A47 rotation bifurcation"*, that attempts 1/3 were *"a different cause"*
with *"`modal_rot` exactly `(0,0,0)`"* implying a camera at rest, and that 1/3 *"fail only the `a56`
self-consistency conjuncts (`distinct=2`, `modal 64.0 %`)"*. **All three are corrected below against
the gate's actual source and the banked rows.** The table above reports the **MODAL** rotation, which
is not the same statement as "the camera was at rest".

**What `B1` actually reads (`check_pose.py` → `a54_oracle.a56_check`, read this session):** the
settle window comes from the **`view.rot` history**; the decision then comes from the **rounded
anomaly `bbox_px` history** after settle, and `a56_check` returns `ok` only when **ALL THREE** hold —
`coverage >= 0.90` **AND** `distinct <= 3` **AND** `pose_match(modal)`. ⛔ **So `B1` does not read
camera data alone**, and `pose_match=True` by itself never accepts a leg.

**Observed inputs, recomputed from the banked `labels.jsonl` this session:**

| attempt | `view.rot` rows across the whole leg | settle start / dropped | post-settle boxes | modal coverage | distinct | `pose_match` | **failing `B1` predicate(s)** |
|---|---|---|---|---|---|---|---|
| try1 | **24 × (0,0,0) + 16 × (0, 359.83, 0)** | 0 / 0 | 16 × `(0.0,485.2,306.1,234.8)` + 9 × `(0.0,484.9,308.6,235.1)` | **64.00 %** | 2 | True | **coverage only** |
| try2 | 25 × (338.54, 358.63, 0) after a 10-value ramp | 14 / 14 | 17 × `(9.9,240.4,342.9,230.4)` | 100.00 % | 1 | False | **`pose_match` only** |
| try3 | **26 × (0,0,0) + 14 × (0, 0.17, 0)** | 0 / 0 | 16 × `(0.0,485.2,306.1,234.8)` + 9 × `(0.0,485.4,303.6,234.6)` | **64.00 %** | 2 | True | **coverage only** |
| try4 | 23 × (358.05, 349.65, 0) + 13 × (358.05, 349.82, 0) + a 4-value lead-in | 4 / 4 | 15 × `(173.5,449.0,271.1,238.9)` + 9 × `(170.5,449.2,272.0,239.2)` | **62.50 %** | 2 | False | **coverage AND `pose_match`** |
| *(reference try1, for contrast)* | 40 × (0,0,0) | 1 / 0 | 25 × `(0.0,485.2,306.1,234.8)` | 100.00 % | 1 | True | *(none — accepted)* |

🔑 **Three specific corrections follow from that table.**
**(a) The view rows are NOT constant in tries 1 and 3** — 16 of 40 and 14 of 40 rows respectively sit
at a different rounded rotation. `(0,0,0)` is the **modal** value, not the whole series.
**(b) `distinct = 2` PASSES.** The criterion is `distinct <= 3`, so listing it beside the failure was
wrong: on tries 1 and 3 the **single failing predicate is the 64 % modal bbox coverage** against the
`>= 90 %` requirement. On try 4 both coverage (62.5 %) and `pose_match` fail; on try 2 only
`pose_match` fails, with coverage at 100 %.
**(c) No cause is established for any of the four.** ⛔ *"Genuine A47 rotation bifurcation"* and
*"a different cause"* both **exceed the evidence** and are withdrawn — the first is the wording of
`check_pose.py`'s own **REPORTING-ONLY** discriminator block, which the file itself says establishes
nothing. What is recorded is the **observed input** and **which predicate failed**; the mechanism
behind either shape is **UNRESOLVED and was not chased** (`G206`, `G120`).
⛔ **These four remain predeclared-INVALID and cannot be retried in this campaign.**

### 3.2 The stop, and why it is not a laundered failure

**`-MaxAttempts 4` is a pose-invalidity budget only, and it was exhausted.** By `§6` and by
`AMENDMENT 1 §A1.4`, an **attempt-exhausted** leg **STOPS the campaign, does NOT count as a valid
pairing failure, and releases NO A control.** ⛔ **It was not re-run**, no budget was extended, no
tolerance touched, and no calibration changed.

🔑 **The discard is sound because it is `A63`'s shape exactly: a leg is discarded for HOW IT RAN,
never for WHAT IT SHOWED.** 🔻 **CORRECTED (078-14, finding 3):** this originally read *"the condition
is the camera pose, fixed at run start"*. **The condition is the leg's `view.rot` settle history plus
its rounded anomaly-`bbox_px` history**, read by `check_pose.py` / `a56_check` from `labels.jsonl`
**before any pairing result exists**, under a rule written down days ago and unchanged. ⛔ **It is not
camera data alone, and the measured rows show the rotation series is not constant within a run** (see
§3.1). What makes the discard sound is unchanged and is the part that matters: the predicate is
**fixed in advance**, it is **independent of the pairing outcome**, and **the pairing outcome was
never computed** — so the decision could not have been influenced by what the leg showed.

### 3.3 Why no pairing number is reported for this leg, and why that is structural

The runner exits **before** its final bank copy (`run_leg.ps1:481-489` precedes `:491-494`), so
**`M51R_F1R_B_NAT` does not exist** — measured, not assumed. The predeclared checker invocation
(`AMENDMENT 1 §A1.3`) resolves *exactly one* session directory inside that bank and **throws when it
cannot**, so **the released checker command is not runnable on this leg at all.** ⛔ **No pairing
verdict was computed from the discarded attempts and none is offered.**

🔻 **CORRECTED (078-14, finding 4).** The original text justified that with `P8` — *"`TAU` is an
absolute luminance difference and is not pose-invariant"*. **That explanation is WRONG and is
withdrawn.** `m44_pairing_probe.py` **does not use `TAU` and reads no luminance threshold**: it
compares **magenta picture centroids against tag-255 mask centroids**, with a **40 px** current /
previous threshold and a **50 px** same-position test (`m44_pairing_probe.py:215-230`, read this
session). `TAU` and `P8` govern the **A54 luminance oracle**, which is a different instrument and was
not involved here. ✅ **The governing reasons are two, and both are structural:** **(i)** the
campaign's **`A63` / `B1` validity precondition** — a leg that fails `B1` is never accepted, and an
attempt-exhausted leg stops the campaign; and **(ii)** the released checker recipe (`AMENDMENT 1
§A1.3`) **targets the accepted bank**, which for this leg **does not exist**. ⚠ **The tool is not
inherently incapable of reading a `_tryN` bank** — that is simply not what the released recipe
points at, and pointing it elsewhere was neither released nor done. ⛔ **Any later offline
diagnostic proposal must preserve these attempts' INVALID status: it could not release a control and
could not become a valid campaign result.**

### 3.4 The leg was correctly CONFIGURED — no configuration defect explains the stop

All four discarded attempts are **provenance-clean**, read from their own banked logs and artifacts:
**40 frames / 40 rows / 40 PNGs, 0 zero-byte, 4 counted events, `label_schema 2`, probe spawned with
40 `PROBE STEP` lines, `m40 SYNTH TICK ORDER = off`, `CaptureBench.Marker = "1"`,
`internalViewRect / unscaledViewRect = 1280×720 / 1280×720`, `speed_ratio` 1.000006–1.002026,
sustained 29.94–30.00 fps.** ⇒ **the recipe reached the engine correctly on every attempt, and no
configuration or artifact-completeness defect was found that would explain the stop.**
🔻 **CORRECTED (078-14, finding 3):** the heading originally read *"the failure is pose alone"*. **The
artifact-completeness and effective-configuration observations above are real and stand exactly as
measured** — they are what they say and nothing more. ⛔ **They do not identify a cause**, and
"pose alone" asserted one; the failing `B1` predicates are named per attempt in §3.1 and the
mechanism behind them is **not established**.

### 3.5 An observation about the box, with NO cause attributed

The A-side reference accepted on **attempt 1** at 20:49 with `distinct=1 modal=100 %`; the candidate
leg, launched **~100 seconds later on the same box, same container, same fixture, same recipe**,
produced **four consecutive `B1`-invalid attempts** — two of them with the **modal** rotation at
`(0,0,0)` and the modal bbox exactly on calibration, but with only 64 % modal coverage and a second
rotation value present in the rows (§3.1). **`A47`'s recorded discard band is roughly 2-in-5; this is
4-of-4 on one leg.** ⛔ **`n = 1` leg, and NO MECHANISM IS ASSERTED.**
🔻 **CORRECTED (078-14, finding 3).** The original text excluded the candidate binary on the grounds
that *"the pose gate reads the camera only"*. **Both halves of that are withdrawn:** `B1` reads the
`view.rot` settle history **and** the anomaly-`bbox_px` history, and — more importantly — **which
fields a validity gate reads neither isolates NOR excludes an `F1` contribution.** ⛔ **The honest
statement is that NO CAUSE IS ESTABLISHED IN EITHER DIRECTION:** this record does not attribute the
four invalid attempts to the candidate binary, and it does not clear it either. It is recorded
because it is the reason the campaign stopped, not because it is understood.

---

## 4. Binary, container and process receipts

**Every swap and every leg is bracketed by a banked receipt** covering the staged executable **and
all five container files** (`A2.5`, `A2.6`). Receipts:
`_bench_sessions_bank\M51_FROZEN_PAIRING_RECHECK_EVIDENCE\receipts\`.

🔻 **CORRECTED (078-14, finding 5): there are EIGHT receipt FILES, not seven.** The original table
collapsed each swap's BEFORE and AFTER receipt onto one row and then counted rows, so both this
section and §5 said "seven". **The files are listed one per row below.**

| # | receipt file | staged exe | all five container files |
|---|---|---|---|
| 1 | `00_PRECAMPAIGN` | candidate `D50DDE78` | FROZEN |
| 2 | `01_LEG1_A_BEFORE_SWAP` | candidate `D50DDE78` (pre-swap state) | FROZEN |
| 3 | `01_LEG1_A_AFTER_SWAP` | **A `B8AC65D8`**, verified against the archive | FROZEN |
| 4 | `02_LEG1_POST` | A `B8AC65D8` | FROZEN |
| 5 | `03_LEG2_CAND_BEFORE_SWAP` | A `B8AC65D8` (pre-swap state) | FROZEN |
| 6 | `03_LEG2_CAND_AFTER_SWAP` | **candidate `D50DDE78`**, verified against the archive | FROZEN |
| 7 | `04_LEG2_POST` | candidate `D50DDE78` | FROZEN |
| 8 | `99_FINAL_RESTORE` | **candidate `D50DDE78`** | FROZEN |

**Container quintet, identical in all eight receipts:** `StackOBot-Windows.utoc` `67EA1FE0…257A` ·
`.ucas` `2CEFB8F4…C4D3` · `.pak` `E03C6610…CDB7` · `global.ucas` `A16A18A8…BA9E` · `global.utoc`
`C70ECDAA…F21C51`. **Only the two pinned archives were ever copied**; both were hash-verified before
each copy. ⛔ **No rebuild, no cook, no quarantined artifact** (a depth-6 search at 078-10 found no
`*QUARANTINE*` file and none was sought or used).

📌 **Restoration.** The final leg ran on the candidate, so the staged executable was **already**
`D50DDE78` at the stop; restoration is satisfied by identity and was **verified by hash rather than
assumed**, with `99_FINAL_RESTORE` banked. **Zero `StackOBot*` processes at every receipt**, including
the final one.

**Process scope.** Zero `StackOBot*` processes existed at preflight and none survives. One
`UnrealEditor` (PID 7924, started 20:42:41) and one `UnrealTraceServer` were resident throughout;
**neither matches the `StackOBot*` pattern the runner kills**, and ⛔ **no owner application was
closed, started, killed, reconfigured or inspected.** Editor absence is not a precondition (§8), and
078-08/09 already measured editor residency to be insufficient to produce the pairing failure.

---

## 5. Evidence, and every physical attempt named

**Campaign evidence:** `D:\IntrusiveAnomalies\_bench_sessions_bank\M51_FROZEN_PAIRING_RECHECK_EVIDENCE\`
(≈180 MB) — `receipts\` (**8** 🔻 *corrected from 7, 078-14 finding 5*) · `runner\` (2 command files
+ 2 full runner transcripts with exit codes)
· `checker\` (1 full checker transcript with exit code) · `artifacts\` (1 facts readout) ·
`attempts\` (5 raw attempt directories + the pre-campaign rotating log) · `tools\` (the three
orchestration/readout snippets, which **read** the unchanged tools and add no gate).

**Banks written by the runner** (`M51R_` prefix, all 81 candidate paths verified free at preflight,
zero collisions, nothing overwritten):

| physical attempt | `_tryN` bank | `_leg_game.log` | accepted alias |
|---|---|---|---|
| `F1R_A_REF_NAT` try1 | `M51R_F1R_A_REF_NAT_try1` ✅ | ✅ | ✅ `M51R_F1R_A_REF_NAT` → `session_20260906-204916` |
| `F1R_B_NAT` try1 | `M51R_F1R_B_NAT_try1` ✅ | ✅ | ⛔ none (runner halted) |
| `F1R_B_NAT` try2 | `M51R_F1R_B_NAT_try2` ✅ | ✅ | ⛔ none |
| `F1R_B_NAT` try3 | `M51R_F1R_B_NAT_try3` ✅ | ✅ | ⛔ none |
| `F1R_B_NAT` try4 | `M51R_F1R_B_NAT_try4` ✅ | ✅ | ⛔ none |

**Five physical attempts, five banks, five `_leg_game.log` files — no missing receipt.** `A1.4`'s two
receipt-less early-exit paths (`no run_summary`, `rode focus timeout`) **did not fire**: every attempt
reached `:408`. The accepted alias is a **copy**, not a sixth attempt. Each raw attempt directory was
copied to `evidence\attempts\` under its own name; **no destination was overwritten**, and the
pre-campaign rotating log was preserved before the first launch (it proved byte-identical to the copy
078-09 had already banked).

---

## 6. UNRUN — named, not implied

**Main cells released but NOT run** (the stop rule forbids continuing):

- ⛔ **`F1R_B_SYN`** — candidate, synth order, 100 %.
- ⛔ **`F1R_B_NAT_SP50`** — candidate, natural, 50 %.
- ⛔ **`F1R_B_SYN_SP50`** — candidate, synth, 50 %.

**Main cell attempted without a verdict:** `F1R_B_NAT` (four physical attempts, all `B1`-invalid;
the failing predicate per attempt is named in §3.1, with no cause established).

**Conditional A controls — all four UNRUN and UNRELEASED**, because the stop class that fired
(attempt-exhausted) releases none: `F1R_A_CTL_NAT` · `F1R_A_CTL_SYN` · `F1R_A_CTL_NAT_SP50` ·
`F1R_A_CTL_SYN_SP50`.

⇒ **`decidable >= 20`, the new coverage floor, was exercised once (33 on leg 1) and never bound.**
The `INVALID (coverage)` class did not fire; neither did `REFUSED`, `INVALID (UNDECIDABLE)` or
`INVALID (event count)`. **The class that fired is attempt-exhausted pose invalidity**, which is the
`§6` / `A1.4` rule, not a new one.

---

## 7. What this campaign does and does not establish

✅ **On these frozen bytes, at 20:49 on 2026-09-06, the archived A-side `B8AC65D8` over the frozen
Phase B container passes MASK-PICTURE-PAIRING at 100 % in natural order with `decidable 33`.** That
is the third consecutive A-side pass on this binary and container (078-08 read 35, 078-09 read 37).

⛔ **THERE IS NO VALID CANDIDATE PAIRING RESULT FROM THIS CAMPAIGN.** Its first cell produced no
valid leg, so there is **no candidate pairing reading of any kind** — not a pass, not a failure, not
a `NEITHER` count — and the remaining candidate cells are UNRUN.
🔻 **CORRECTED (078-14):** this originally read *"NOTHING is established about the `F1` candidate"*,
which is broader than the evidence in both directions. **Scoped correctly:** the campaign yields **no
valid candidate pairing verdict**, and it neither isolates nor excludes an `F1` contribution to the
four invalid attempts. ⚠ **What the four discarded attempts DO carry, scoped:** complete
artifact-integrity and effective-configuration readings (§3.4) — real observations that certify
neither pairing behaviour nor a cause.

⛔ **The banked 078-05 / 078-06 failures are untouched.** They are not reproduced, not refuted, not
reclassified, and **root cause remains OPEN and was not chased** (`G120`).

⛔ **`A1.1` / `A2.4` attribution, restated because it is the rule that governs whatever comes next:**
if a candidate leg fails and its A control **also** fails, **the failure occurs WITHOUT `F1`** — that
**neither isolates nor excludes an `F1` contribution**. If the control **passes**, a candidate
contribution is **consistent with the observation**, with **time and machine state confounded**.
🚨 **Neither branch is causal proof**, and neither establishes environment, FIFO ordering,
render-thread-ahead arming, or a client cause. Chat's paraphrase *"control fails ⇒ not attributable to
the candidate"* stays **rejected**.

⚠ **The instrument's declared ceiling is unchanged:** the probe has two positions, so **a slip of an
EVEN number of frames maps position onto itself and is invisible to this gate.** `CURRENT` on every
decidable frame does not exclude an even-frame slip. **Leg 1's PASS is scoped pairing smoke on these
bytes and this recipe — not `F1` or `m51` release certification, not an all-gates claim, not a Lyra
pass.**

**Everything in `§10` remains HELD:** production `Source/` and `Shaders/`, fixtures, harness and
checker source, schema, observability meanings, identity / lifecycle / geometry / tag / writer /
control-flow changes, all levers and new probes. **`M-F1b` (backbuffer × target-mask) remains UNRUN**;
the Lyra arm is **not waived**; **no push, merge or tag.**

---

## 8. Open, and unchanged by this session

Root cause of the 078-05 / 078-06 pairing failure · why the box ran at ~1.00 in the evening and
3.4–3.6 an hour earlier · **why this box produced four consecutive `B1`-invalid attempts on one leg
minutes after a clean attempt-1 acceptance** (new, `n = 1`, cause not established in either
direction) · `M-F1b` UNRUN ·
the identity / lifecycle unit, all of it unproved design work · the unchecked `SaveStringToFile`
label-append result named in `_reviews/078-12-codex-f1-source-review.md` · Lyra arm not waived.
**Pairing release certification and every `m51` implementation item remain BLOCKED.**

**VERDICT: NEEDS-DECISION** — one of five released main cells passed, one halted without a verdict,
three are UNRUN and all four controls are UNRUN. Codex and Chat evidence review is next; ⛔ **no
implementation unit is started and no leg is re-run.**
