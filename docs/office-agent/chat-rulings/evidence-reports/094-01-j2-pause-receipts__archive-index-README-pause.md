# Binary baseline chain — AnomalyInjector / StackOBot bench

**Evacuated here 2026-08-19, immediately before the MainWorld + G118 re-cook.**

## Why this directory exists

All five binaries previously lived in
`D:\IntrusiveAnomalies\StackOBot\Builds\BenchGate\Windows\StackOBot\Binaries\Win64\` —
**the exact directory `BuildCookRun`'s stage/archive step writes into.** `G92` records that the
archive step wipes `<Package>\Windows\<Game>\Saved\`; **nothing in the record said what it does to
`Binaries\Win64\`.** Rather than find out with the only copies in the blast radius, the chain was
copied out and hash-verified here first.

*(Measured afterwards: that cook wiped **nothing**. But it was one flag set — no `-clean`, archiving
into an existing tree — and the 2026-08-16 wipe is not retracted. The precaution stays.)*

⛔ **This location is deliberately OUTSIDE `Builds\`.** Do not move it back under `Builds\`.

---

## 🚨 READ THIS BEFORE TRUSTING ANY ENTRY BELOW — G121

**AN EXE IS HALF AN ARTIFACT.** A content-only re-cook leaves the exe **byte-identical** while
changing the cooked maps, the enforced secrets and hundreds of MB of content. Measured 2026-08-19:
the exe was `101AFEA4` **before and after** the cook, same mtime, while the map set went 3 → 4, the
enforced control-server token went `TESTVALUE123` → a 64-char rotated value, and `.ucas` went
125 MB → 284 MB. The reverse case is equally live: a **code-only hot-swap (G103)** moves the exe and
leaves the pak. **The two halves move independently.**

⇒ **GOING FORWARD, A BASELINE IS A QUARTET:**

```
exe hash  +  StackOBot-Windows.utoc hash  +  .ucas hash  +  .pak hash
```

**Anything less is not a baseline.** A same-exe-hash comparison is **not** a same-build comparison,
in either direction.

---

## What is here, and what each entry does and does NOT reconstruct

### Exe-only artifacts — these DO NOT reconstruct a build

| file | SHA-256 (first 8) | what it is | reconstructs? |
|---|---|---|---|
| `StackOBot.exe.m25-baseline` | **`101AFEA4`** | **`m25`** — SVE default grab point. The binary S4-4's delivery pair and **both H4 legs** ran on | ⛔ **EXE ONLY. The m25-era pak (3 maps, `TESTVALUE123` token, `.utoc` 194,996 B) WAS OVERWRITTEN BY THE 2026-08-19 COOK AND IS GONE.** This file does **not** reconstruct the build H4 was measured on. |
| `StackOBot.exe.s4-2-baseline` | **`259BF64F`** | pre-flip binary for the **S4-2** gates (a)–(d) | ⛔ exe only; its pak is gone |
| `StackOBot.exe.s4-0-baseline` | **`834BB30A`** | **the binary all ten S4-1 matrix legs ran on** (42 counted events, 42 ALIGNED) | ⛔ exe only; its pak is gone |
| `StackOBot.exe.m24-baseline` | **`3BA854FB`** | **`m24`** — S3 certified, ratio-independence discharged | ⛔ exe only; its pak is gone |
| `StackOBot.exe.m23-baseline` | **`85A39CFB`** | **`m23`** — the P3 fix / DA60 floor era | ⛔ exe only; its pak is gone |

⚠ **THE LOSS IS BOUNDED, AND THE RECEIPT IS THE G-2 DEBT SWEEP.** Before the cook, every claim still
owed against `101AFEA4` was enumerated and **the list was EMPTY**: m25's gates are closed and its
matrix ran on `834BB30A`; H4's limits are limits, not pending work; path (a) *cannot* be owed against
a build that lacked MainWorld; and `B2` / `B1`-NDC / `P5` / `P7` are all gated against **banked data**,
which no cook touches. ⛔ **Do NOT attempt to reconstruct the m25-era pak.** Record the loss as
bounded, not as an open hole. *(Journal 045 §30.)*

### The complete quartet — this DOES reconstruct a build

**`pathA-measurement-build-paks/`** + `StackOBot.exe.m25-baseline`

| component | hash | detail |
|---|---|---|
| exe | **`101AFEA4`** | `StackOBot.exe.m25-baseline` in this directory |
| `StackOBot-Windows.utoc` | **`939B9C9B`** | 268,036 B · 2026-08-19 17:00:27 · **4 maps: `CB_GateLevel`, `Entry`, `MainMenu`, `MainWorld`** |
| `StackOBot-Windows.ucas` | **`8A602D4D`** | 284,469,920 B |
| `StackOBot-Windows.pak` | **`7CAE22DD`** | 10,115,703 B |
| *(also kept)* `global.utoc` / `global.ucas` | `C70ECDAA` / `A16A18A8` | |

**This is the PATH-(a) MEASUREMENT BUILD.** Path (a) is the most client-relevant result this project
will produce and it will be measured against **this exact container**. A future cook overwrites the
live pak silently — that is now a **known** failure mode, not a hypothetical, which is why the
container is preserved here rather than only referenced. All five files were **hash-verified at this
location** after copying (A62). 282.9 MB.

### 🆕 `m25-h4h5m1-measurement-build/` — THE SECOND COMPLETE QUARTET (2026-08-19, before the m26 cook)

**Preserved immediately before the `m26` slice-1 cook, on owner precondition. All six files
hash-verified AT THIS LOCATION after copying — 6/6 match (A62).** ~537 MB.

| component | hash | detail |
|---|---|---|
| `StackOBot.exe` | **`101AFEA4`** | same exe as `m25`; the quartet is what distinguishes the build |
| `StackOBot-Windows.utoc` | **`939B9C9B`** | 268,036 B · **4 maps: `CB_GateLevel`, `Entry`, `MainMenu`, `MainWorld`** |
| `StackOBot-Windows.ucas` | **`8A602D4D`** | 284,469,920 B |
| `StackOBot-Windows.pak` | **`7CAE22DD`** | 10,115,703 B |
| `global.utoc` / `global.ucas` | `C70ECDAA` / `A16A18A8` | |

🚨 **WHAT RESTS ON IT — this is the `H4` / `H5` / `M-1` MEASUREMENT BUILD.** **Every result in journal
045 PARTS TWO THROUGH FOURTEEN was taken on this container:**

- **PART TWO** — `H4 SUPPORTED` (path b), the C4 divergence 8/8, and its `StaticMeshActor_49` control.
- **PARTS SEVEN–EIGHT** — MainWorld first launch, the mover census, the geometry survey.
- **PARTS NINE–ELEVEN** — **`H5` class (ii) reproduced**, `G124`'s generalisation, and the **7-target
  cure measurement table** (`InstancedFoliageActor_0_0_0`, `BP_SplineSpawn_C`, `SM_Ramp2`,
  `StaticMeshActor_49`, `BP_SpawnPad_C`, `BP_MovingPlatform_C`, `RoomBuilderSquare_C`).
- **PART FOURTEEN** — `M-1`'s readback-latency legs *(on exe `1EBA8944`, a code-only hot-swap of this
  same pak — see the note below)*.

⚠ **It is COMPLETE — all four halves — unlike `StackOBot.exe.m25-baseline`, which is EXE-ONLY and
whose own pak was overwritten and is gone.** That loss is precisely why this entry exists.

⚠ **Note on `1EBA8944` (the `M-1` log-only instrument build): its EXE was NOT archived before being
overwritten.** The loss is bounded — it rebuilds from plugin commit `0185c10`, and `M-1`'s results are
banked (`P14_M1_DELIVOFF`, `P14_M1_DELIVON`) — **but it is an instance of the rule in this file not
being followed, recorded rather than quietly fixed.** Its pak half is this quartet.

---

## `m34-precook-m33exe-s051container\` — the pre-m34-cook staged build, COMPLETE (2026-08-23)

Preserved immediately before the m34 GPU-mask-reduce cook (branch `feature/mask-gpu-reduce`), per
A62 and G-R2's precondition. 6/6 hash-verified at this location:

| file | SHA-256 (first 8) |
|---|---|
| `StackOBot.exe` (the m33 exe) | `757A5DD4` |
| `StackOBot-Windows.utoc` | `E4FE9B35` |
| `StackOBot-Windows.ucas` | `D9929F6F` |
| `StackOBot-Windows.pak` | `BFB95333` |
| `global.utoc` | `C70ECDAA` |
| `global.ucas` | `A16A18A8` |

**What rests on it:** the container half is the session-051 quartet — the last cooked container
before m34's, carrying every leg from sessions 051 through 056 (m31 V-2 post-fix work, m33's
G-A/G-B/M33_CTRL legs, the 052-055 change set legs). The exe half is the m33 staged exe
(`757A5DD4`), the binary the m33 home gates ran on.

---

## 🆕 The m34 / m35 exe chain — EXE-ONLY, and ALL THREE ARE LOAD-BEARING (added 2026-08-26, s062)

These ride the **unchanged m34 container** (`utoc 2A66CA57` · `ucas A7EF9B12` · `pak D8009AD7`), so
each is a code-only hot-swap (`G103`) of the same cooked build — not a new quartet.

| file | SHA-256 (first 8) | what it is | ⛔ what depends on it |
|---|---|---|---|
| `StackOBot.exe.m34-candidate-17DEAA74` | **`17DEAA74`** | m34, the GPU mask reduction, as cooked | **THE EXE EVERY m34 HOME GATE LEG RAN ON** — `G-R1`..`G-R6` and `G-R7`'s StackOBot half, the 145/145 `COMPARE` IDENTICAL set (journal 058). The A-side of any future m34-attribution question. **Named in NO stop block** — which is exactly why it is written down here. |
| `StackOBot.exe.m34-fix-candidate-7F37A4AC` | **`7F37A4AC`** | m34 **+** `b05066f`, the stale-present display fix | **`G-M6`'s A-SIDE.** The hook-cost prior is taken by swapping this against `733FE83C` with pacing OFF, order `A,B,B,A`. **It cannot be rebuilt from the current tree** once the m35 WIP is amended in. ⛔ **DO NOT DELETE.** |
| `StackOBot.exe.m35-buildb-733FE83C` | **`733FE83C`** | m35 Build B — the plugin-owned sub-rect copy | The staged bench binary. `G-M1`–`G-M5` all ran on it; `G-M6`'s B-side; `G-M7`/`G-M8`/`G-M9` will. |

⚠ **Gap on the record:** the m35 **Build A** exe was **never archived** — it was overwritten by Build
B, and a rebuild is ruled out. That is why `G-M6`'s A-side is `7F37A4AC` (pre-m35 entirely) rather
than Build A, so the prior measures m35 in its entirety. Journal 059 records the same failure for
`64568A5D` (the `A-I1` instrument build). **Two instances now — archive before every swap.**

---

## 🆕 The m35-`G-M9` / m36 exe chain — EXE-ONLY (added 2026-09-02, session 067)

🚨 **THESE SEVEN BINARIES EXISTED HERE UNRECORDED.** They were archived correctly and every one
verifies by hash, but **not one had an entry in this file** — so by this file's own deletion rule
they were undeletable-but-undefended: nothing said what depended on them. Recorded now.
**All seven re-hashed and re-sized at this location 2026-09-02; 7/7 match their filename identity.**

They all ride the **unchanged m34 container** — `utoc 2A66CA57` · `ucas A7EF9B12` · `pak D8009AD7`
(re-verified at the staged bench path 2026-09-02) — so each is a **code-only hot-swap (`G103`)** of
the same cooked build, **not a new quartet**.

| file | SHA-256 (first 8) | size (bytes) | what it is | ⛔ what depends on it |
|---|---|---|---|---|
| `StackOBot.exe.m35-gm9-6B579F91` | **`6B579F91`** | 240,901,632 | m35 Build B **+** the `G-M9` dual-path readback comparator | **`G-M9`'s ENTIRE RESULT** — 270 frames, zero byte differences, at `rect=(0,0)`, `(0,92)` and `(280,0)`. The two non-zero-origin legs are what discharged the gate (`G192`); a re-read needs this exe |
| `StackOBot.exe.master-merge-1F6A2188` | **`1F6A2188`** | 240,901,632 | master's first post-merge build (m34 + m35) | the **inert-merge proof** for the m34/m35 merge — master's build vs the branch's |
| `StackOBot.exe.m36-s1-rebased-02C1DFA2` | **`02C1DFA2`** | 241,011,200 | m36 **S1**, census core, selection NOT wired | **`P-C7`'s first anchor** (census OFF ≡ parent tip) and the `P-C6` hygiene/leak-probe legs |
| `StackOBot.exe.m36-s2-E046D1CA` | **`E046D1CA`** | 241,023,488 | m36 **S2**, selection wired | superseded by `CBBF6644` within the same stage; **nothing is graded against it** — the one entry here that could be deleted, and it is written down rather than assumed |
| `StackOBot.exe.m36-s2-CBBF6644` | **`CBBF6644`** | 241,025,536 | m36 S2 **+ the gate instruments** | 🚨 **THE EXE EVERY S2 GATE LEG RAN ON** — `P-C1`·`P-C2`·`P-C3`·`P-C4`·`P-C5`·`P-C8`·`P-C10`·`P-C11`·`P-C12`·`P-C13` and the `run_summary` subset gate (journal 065 §4). The A-side of any m36-attribution question |
| `StackOBot.exe.m36-s3-70F6B72C` | **`70F6B72C`** | 241,026,048 | m36 **S3**, the per-cycle census cost line | **`P-C9`'s cost numbers** — the `A,B,B,A` legs and the paced pair, and the `P-C7` re-anchor at that boundary |
| `StackOBot.exe.m36-master-postmerge-D2BB25A5` | **`D2BB25A5`** | 241,026,048 | **master's own post-m36 build — CURRENTLY STAGED at the bench path** | the merge-inert proof for m36, and **the declared exe for the `P9` legs** (`docs/predictions/2026-09-02-p9-blinking-boundary-repro.md` §1) |

⚠ **`70F6B72C` AND `D2BB25A5` ARE THE SAME SIZE AND HAVE DIFFERENT HASHES ON BYTE-IDENTICAL
`Source/`.** That is `G201` in this file rather than in a journal: **an artifact hash is provenance,
not content identity** — MSVC link non-determinism moves the hash across two links of the same
source. ⛔ **Never conclude "the code changed" from a hash change alone**, and never conclude the
reverse either (`G121`: a content-only re-cook leaves the exe byte-identical).

📌 **Gap on the record, stated rather than left to be discovered:** the m36 **container** was never
re-cooked, so there is **no m36 quartet** — only this exe chain on m34's container. If m36 is ever
cooked, archive the quartet **before** the cook, per this file's own rule.

### 🆕 m37 — the census selection defaults (added 2026-09-02)

| file | SHA-256 (first 8) | size (bytes) | what it is | ⛔ what depends on it |
|---|---|---|---|---|
| `StackOBot.exe.m37-census-defaults-6C80E872` | **`6C80E872`** | 241,036,800 | **m37** — census floor default `6.0 → 0.5`, NEW `CensusMaxDrawnCoveragePct` ceiling default `25.0` (inclusive band), `census_above_ceiling` counter + `ABOVE-CEILING` token | **ALL FOUR m37 GATES RAN ON IT** — (a) the `P-C7` census-OFF re-anchor at this build boundary, (b) the ceiling mechanism control, (c) the floor-0.5 spot-check, (d) the disabled-ceiling echo check. **It is also the current STAGED bench binary.** |

⚠ **EXE-ONLY SWAP — the container is UNCHANGED and was re-verified at the staged path after the
swap:** `utoc 2A66CA57` · `ucas A7EF9B12` · `pak D8009AD7` · `global.utoc C70ECDAA` ·
`global.ucas A16A18A8`. **m37 adds no global shader and no content**, so there is no cook and no new
quartet — it rides the m34 container exactly as the whole m35/m36 chain does (`G103`).

📌 **`D2BB25A5` IS STILL LOAD-BEARING and must not be deleted:** it is the **A-side of gate (a)**
(via the banked `M36_M36_MERGE_INERT_MASTER` leg) and the exe every `P9` v1 and v2 leg ran on.
🔓 **The `D2BB25A5` freeze is RETIRED** — it existed for the `P9` bench measurement legs, which are
complete; remaining `P9` work runs on Bates' own build.

---

## ⛔ THE DELETION RULE (owner ruling, 2026-08-26)

**NO ARCHIVED BASELINE IS DELETED UNTIL THE DOCS SAY WHAT GATE DEPENDS ON IT.** "It looks old" is not
an answer. The answer is a **named gate**, or an explicit **"nothing depends on it"** written down.
Every table in this file now carries that column for the entries added since; fill it in for anything
you add, and if you cannot fill it in, the file is not ready to be deleted from.

📐 **The instrument, stated once:** every 8-hex identity in this file is the **first 8 hex of SHA-256** —
`(Get-FileHash <path> -Algorithm SHA256).Hash.Substring(0,8)`. Verified 2026-08-26 against
`StackOBot.exe.m34-fix-candidate-7F37A4AC` (MD5 and SHA-1 do not reproduce it). ⚠ This file was the
**only** place that instrument was stated, and this file is **outside version control** (`G112`); it
is now also in `Plugins/AnomalyInjector/docs/setup-runbook.md` §8.1.

---

## If the chain ever moves again

Verify by hash at the destination **before** deleting the source, and update these tables. The hashes
above are load-bearing: they are cited in the `CLAUDE.md` status block and in five session journals,
and a result whose build cannot be identified is not a result (A44 · G121).

---

## m40 — order-independent label sampling (2026-09-02)

| file | hash | what it is |
|---|---|---|
| `StackOBot.exe.m38-runlog-F2FA6BCD` | `F2FA6BCD` | LOAD-BEARING - this is m40's A-SIDE. It is the pre-fix binary leg L1 (the m40 control) ran on, and the only archived binary carrying the shipped sampler position. Do not delete: without it the m40 control cannot be re-taken. |
| (staged) `StackOBot.exe` | **`C0AD3F91`** | m40 - the fix (label active-state sampled at FWorldDelegates::OnWorldTickEnd) plus the bench-only IAI.Bench.SynthTickOrder lever. Legs L3 and L4. Container quartet UNCHANGED, code-only hot-swap, no cook (G103). |
| (not archived, deliberately) | `DC16710D` | The lever-only INTERMEDIATE that leg L2 ran on: the lever present with the sampler still at its shipped call site. An ephemeral gate build, never committed and never shipped; reconstructible from the m40 commit by moving the single SampleDeferredActiveState() call back to the top of UAnomalyCaptureSubsystem::Tick. Recorded here so the L2 result can be attributed to a build. |

F2FA6BCD's freeze is NOT retired by m40. D2BB25A5's was retired at m38; F2FA6BCD replaces it as the live A-side.

## m41 (census ON by default) - 2026-09-03, session 069

- **StackOBot.exe.m40-orderindep-C0AD3F91** (241,069,056 B) - the m40 exe. ARCHIVED THIS SESSION,
  hash-verified at the new location BEFORE the swap. It is **m41's A-SIDE**: every pre-m41 control
  leg runs on it. It is also still m40's own shipped/validated binary. DO NOT DELETE.
- **Staged bench exe is now `2BF9E1B9`** (241,115,136 B) = m41, branch
  `m41-CG1a-GATE-FAILED-do-not-merge` @ `52f6430`. Code-only hot-swap (G103); the container
  quartet is UNCHANGED (utoc `2A66CA57` / ucas `A7EF9B12` / pak `D8009AD7`); NO COOK.
- **2BF9E1B9 IS NOT A VALIDATED BINARY.** Pre-declared gate C-G1a FAILED on it (the host-PP
  preflight scanned 0 volumes / 0 camera blends / 0 materials on `CB_GateLevel`, which the
  predictions file names as BLINDNESS, not a clean read). The gate campaign was stopped there and
  the remaining legs were NOT run. Do not cite this exe for anything except the C-G1a reading and
  the M41_M41_ON_A leg banked beside it.

### m41 session 069 brief 3 - exe chain and what is NOT archived

- `C0AD3F91` (archived above) is **m41's A-SIDE** and was used as the pre-m41 control leg
  (`M41_M41_M40_CTRL`). Load-bearing. DO NOT DELETE.
- `2BF9E1B9` (m41 first build, branch @ `52f6430`) was **NOT archived as a file** - it was
  overwritten in staging by the next build. **Stated rather than hidden.** Loss is bounded: it is
  rebuildable from `52f6430` (pushed), and the only result taken on it is the `CB_GateLevel`
  `0/0/0` preflight reading, which is banked in `M41_M41_ON_A`'s own `anomaly_log.txt`.
- `7616F144` (probe + camera-POV source) - superseded within the same session, not archived.
- **Staged bench exe is now `5C073AC9`** (241,122,816 B) = branch `m41-CG1a-GATE-FAILED-do-not-merge`
  @ `6b150f0`. Container quartet UNCHANGED (`2A66CA57` / `A7EF9B12` / `D8009AD7`); no cook.
- ⚠ `5C073AC9` is **NOT a validated binary**: gate `A-G1` was not met as written (see journal /
  report for session 069 brief 3). master is still at `2543674` - predictions only.

### m41 CLOSED — 2026-09-03, session 069 brief 4 (supersedes the "NOT a validated binary" line above)

- ✅ **`5C073AC9`** (241,122,816 B) is the **VALIDATED m41 bench exe**, built from master's `m41`
  feat commit. Container quartet UNCHANGED (`2A66CA57` / `A7EF9B12` / `D8009AD7`); code-only
  hot-swap, no cook (G103). Every m41 gate that can run on this container passed; `B-G1` and
  `C-G1b` are UNRUNNABLE here and ride the client cook (see PRE-DELIVERY-CHECKLIST §1.1).
- ⛔ **`C0AD3F91` IS `m41`'s A-SIDE AND STAYS LOAD-BEARING — DO NOT DELETE.** It is the binary the
  pre-m41 P-C7 control legs ran on (`M41_M41_M40_CTRL`, `M41_M41_M40_CTRL_B`), and it is also m40's
  own validated exe.
- 📌 The earlier line calling `5C073AC9` "NOT a validated binary" was written while gate `A-G1` was
  still open. **A-G1 PASSED under the P-C7 v2 comparator** (journal 069 §2.3): against a pose-matched
  control on `C0AD3F91`, every `labels.jsonl` field is byte-identical except `t_wall`. The line is
  superseded, not deleted.
- ⚠ `2BF9E1B9` and `7616F144` remain UNARCHIVED (overwritten in staging). Bounded and rebuildable
  from master's history; the only result taken on `2BF9E1B9` is banked inside `M41_M41_ON_A`'s own
  `anomaly_log.txt`. Stated, not hidden.

### m43 attempt 1 — 2026-09-03, session 069 brief 6. GATE (i) FAILED.

- ⛔ **`87D1EA89`** (241,152,000 B) = branch `m43-GATE-i-FAILED-do-not-merge` @ `43b901b`.
  **NOT a validated binary** — gate (i) failed on it (coverage 87 of 90, and 71 frames written as BLANK
  that are not hidden). Staged on the bench; do not cite it for anything but that reading.
- ✅ **`5C073AC9` archived as `StackOBot.exe.m41-census-on-5C073AC9` — it is the VALIDATED m41 exe
  AND `m43`'s A-SIDE.** Hash-verified at the archive before the swap. **Load-bearing, do not delete.**
- ⛔ `C0AD3F91` remains `m41`'s A-side and stays load-bearing.
- Container quartet UNCHANGED throughout (`2A66CA57` / `A7EF9B12` / `D8009AD7`); no cook.

- m43 attempt 2 = `0D51A7E3` (241,156,608 B), branch tip. ALSO NOT VALIDATED - gate (i) fails on pass contention (see journal / report, session 069 brief 7). Attempt 1 `87D1EA89` archived as `StackOBot.exe.m43-attempt1-unvalidated-87D1EA89`. `5C073AC9` remains m43's A-side.

- m43 attempt 3 = `6299A172` (241,159,168 B), branch tip. Shared-pass change PROVEN (gate A pass; m26 arm starvation fixed: 24/24 served, lag 1/1) but gate (i) FAILS - see session 069 brief 8 report. `0D51A7E3` and `87D1EA89` superseded-unvalidated. `5C073AC9` remains m43's A-side.

- m43 attempt 4 (final of session 069) = `99CD7BA3` (241,169,408 B), branch tip. Gates A,B,C,E,F PASS; gate D fails on tagOvertaken 0->1. NOT validated. `6299A172` archived as `StackOBot.exe.m43-attempt3-unvalidated-6299A172`; `2800C7EC` (attempt 4a, pre-tie-instrument) not archived - reconstructible. `5C073AC9` remains m43's A-side.

### m43 SHIPPED - 2026-09-03, session 069 brief 10
- VALIDATED m43 bench exe = `AD543F42` (241,169,920 B), master's m43 feat commit. Container quartet UNCHANGED; code-only hot-swap, no cook.
- ⛔ `5C073AC9` (StackOBot.exe.m41-census-on-5C073AC9) is m43's A-SIDE and the validated m41 exe. LOAD-BEARING, do not delete.
- Superseded-unvalidated m43 attempts: `87D1EA89` (archived), `6299A172` (archived), `0D51A7E3` and `2800C7EC` and `99CD7BA3` NOT archived - overwritten in staging, reconstructible from master's history. Stated, not hidden; the only results taken on them are banked in their own legs' logs.

### LogAnomaly export fix - 2026-09-03, session 069 brief 11
- Staged bench exe = `0EF535DC` (241,169,920 B) = master's `fix(injector): export LogAnomaly across modules`. Same SIZE as its predecessor; the hash moved anyway (G201 - a hash is not content identity).
- `AD543F42` archived as `StackOBot.exe.m43-AD543F42` - the validated m43 exe and this fix's A-SIDE. Load-bearing.
- ⛔ `5C073AC9` remains m43's A-side for the m43 gates and the validated m41 exe.
- P-C7 v2 vs the m43 control: frame_index delta 0, t delta 0, view identical, every label field byte-identical EXCEPT mask_value - and a SAME-BINARY control pair shows the same 23-row mask_value difference, with the post-fix second run reproducing the PRE-fix tag set exactly. mask_value is run-unique (stencil tag allocation is timing-sensitive); the export change is inert.

## 2026-09-03, session 069 briefs 13/14

- `StackOBot.exe.m43-logexport-0EF535DC` - m43 plus the LogAnomaly export fix. **LOAD-BEARING:**
  it is the binary both m44/m45 reproduction legs of journal 069 section 7 ran on, and it is
  m44's A-SIDE for the M44-G6 comparison. Do not delete.
- `StackOBot.exe.m44-GATE-G1-FAILED-unvalidated-15A31043` - the m44 attempt. **M44-G1 FAILED**
  (first mask frame is still one frame after the first labelled frame). NOT shipped, NOT on
  master; source is on branch `m44-GATE-G1-FAILED-do-not-merge`. Archived so the measurement in
  journal 069 section 8 is reproducible.
## 2026-09-03, m44 (session 069 brief 18)

- `StackOBot.exe.m44-tagownership-635A615A` — **the m44 staged bench exe.** Every m44 gate ran on it:
  G1 4/4 delta 0 both tick orders, G2/G3/G7 pass, G6 all six veto counters identical to the m43
  control, MASK-TIE 27 lines 0 mismatch.
- `StackOBot.exe.m43-logexport-0EF535DC` — **LOAD-BEARING: m44's A-SIDE** and the binary both Bates
  reproduction legs ran on. Do not delete.
- master's own post-merge build is `06657B35` and is deliberately NOT staged — identical Source,
  different exe hash across two links (G201). The gated binary stays on the bench.
- `StackOBot.exe.m44-b16-probe-366C4B94` and `.m44-GATE-G1-FAILED-unvalidated-15A31043` are
  measurement binaries from the refuted-hypothesis briefs; keep them while journal 069 §9-§10 is live.
## 2026-09-03, m45 (session 069 brief 21)

- `StackOBot.exe.m45-canfail-8A895272` — **the m45 staged bench exe.** Every m45 gate ran on it:
  identity arbiter control 0 / test 0 / can-fail 20-of-60, IoU 0.9987 and 0.9969, ONSET 6/6, G7
  equality 35=35, MASK-TIE 0 mismatch.
- `StackOBot.exe.m44-tagownership-635A615A` — **now m45's A-SIDE** (it is the old-hide leg of every
  identity comparison as well as the shipped m44 binary). Do not delete.
- `StackOBot.exe.m45-GATE-G4-unobtained-2F5301BC` — the attempt whose can-fail lever (shadow omission)
  could not fire in this fixture. Kept while journal 069 §13 is live.
## 2026-09-03, m46 + the two cooks (session 069 brief 22)

- `m45-precook-container-d204598\` — the quartet master ran on BEFORE the first cook
  (exe 8A895272, utoc 2A66CA57, ucas A7EF9B12, pak D8009AD7). Every m41-m45 measurement was taken on
  this container.
- `m45-postcook-container-A4\` — the first cook's quartet (exe 38A55169, utoc 4621F571, ucas 513F4D35,
  pak 2163A13A). **These are master's A4 re-baseline controls**; the cook was proven to change no
  behaviour against the pre-cook container.
- LIVE (staged) — the m46 quartet: exe 60AE8C61, utoc EF8EB23C, ucas A8BFFF88, pak 3C026A8D. It is the
  binary every m46 gate ran on, including the 50%-screen-percentage leg.
- `StackOBot.exe.m45-canfail-8A895272` stays: it is the exe the m45 identity arbiter and its can-fail
  leg ran on, and the pre-cook staged exe.## 2026-09-03, m47 (session 069 brief 25)

- LIVE (staged) - `StackOBot.exe` **F309D836**, the m47 bench exe. Every m47 packaged gate ran on it:
  P1 prewarm OFF, P2 prewarm ON, P3 prewarm ON + SynthTickOrder; ONSET 8/8 delta 0, G7 stray 0,
  black-frame gate PASS on all three. Container quartet UNCHANGED (utoc EF8EB23C, ucas A8BFFF88,
  pak 3C026A8D) - code-only hot-swap, no cook: m47 adds no shader and no shader parameter.
- `StackOBot.exe.m46-resolution-60AE8C61` - **m47's A-SIDE and the shipped m46 binary.** It is the exe
  every m46 gate ran on, including the 50%-screen-percentage leg, and it is the P-C7 v2 control the
  m47 run_summary key-set delta was measured against. Do not delete.
- An intermediate staged exe `2C167ADE` was OVERWRITTEN without being archived, and that is stated
  rather than hidden. It was linked before the second round of m47 edits and A44 caught it: two new
  symbols read 0 in the staged binary while IAI.Capture.ShaderPrewarm read 7 and the pre-existing
  IAI.Bench.MaskPairingProbe read 5, so the scan was sound and named the missing half. No gate ever
  ran on it; it is reconstructible from the same tree minus one commit's worth of edits.


## 2026-09-03, m48 + brief 28 (session 069)

- LIVE (staged) - `StackOBot.exe` **DE65F84A**, the m48 bench exe. Every m48 gate ran on it.
  Container quartet UNCHANGED (utoc EF8EB23C, ucas A8BFFF88, pak 3C026A8D) - code-only hot-swap,
  no cook.
- `StackOBot.exe.m48-exposuredip-DE65F84A` - archived at brief 28. It had been STAGED but never
  ARCHIVED, and brief 28 needed to swap the staged exe to run a bisect, so it was archived FIRST
  and hash-verified at the archive before any swap (A62). Stated rather than left implicit.
- `StackOBot.exe.m47-shaderprewarm-F309D836` - m48's predecessor. It was m48's pairing "A-side",
  and brief 28 showed that A-side was NOT a control: it was itself off-baseline (see below).
  It remains the m47 gate binary. Do not delete.
- `StackOBot.exe.m46-resolution-60AE8C61` - **LOAD-BEARING AND NOW DOUBLY SO.** Besides being the
  shipped m46 binary and m47's A-side, brief 28 staged it to run `B28_B1` and `B28_L2`. It is the
  binary that PRODUCED the passing MASK-PICTURE-PAIRING baseline (33/33 NEITHER 0 PREVIOUS 0) and
  that ALSO reproduces NEITHER 54 exactly when given the newer leg recipe - which is what exonerated
  the binary and located the fault in the recipe. Do not delete.

BRIEF 28, FOR ANYONE READING A PAIRING NUMBER OFF THESE BINARIES: the MASK-PICTURE-PAIRING gate's
result depends on the LEG RECIPE, not only on the exe. The baseline recipe is
`-Anomaly blinking -Target StaticMeshActor_49 -Seed 4242 -MaxFrames 40`, both tick orders. A leg
firing `corrupted_texture` puts a second magenta object in frame (the probe wears the same material)
and reads NEITHER on every anomaly-present frame on EITHER binary. The analyser now refuses such a
leg outright rather than returning a number. See journal 069 s20 and G235.

BOTH SWAPS THIS BRIEF ARE STATED, IN BOTH DIRECTIONS: DE65F84A -> 60AE8C61 (for B28_B1),
60AE8C61 -> DE65F84A (for B28_B4/L1), DE65F84A -> 60AE8C61 (for B28_L2), 60AE8C61 -> DE65F84A
(restored). Every stage was hash-verified after the copy. The staged exe was left as found.
--------------------------------------------------------------------------------------
SESSION 073 (2026-09-04) - m49 PHASE A1 FIX
--------------------------------------------------------------------------------------
- `StackOBot.exe.m49-a1-EC60E9B9` - the m49 Phase A1 binary: the onset-join fix, per-frame
  `target_pixels`/`observable`, schema v2, LG-3 and LG-4. Every 073 gate leg ran on it
  (2 auto-pool, 4 pairing + 2 controls, 8 G-EDGE). Container quartet UNCHANGED
  (EF8EB23C/A8BFFF88/3C026A8D + A16A18A8/C70ECDAA) - code-only hot-swap, NO COOK (G103).
- `StackOBot.exe.m49-a1-GATEMISS-1C859DC9` - the 072-02 gate-miss build. **LOAD-BEARING:
  it carries the OBS-1 CAN-FAIL NEGATIVE** (banked leg `A1_A1_NAT`, 4 of 6 events with an
  EMPTY `affected_frames`). G96 for `observable` is satisfied by that artifact, not by a
  synthetic lever, so this binary is the proof the gate can fail. Do not delete.
- `StackOBot.exe.m48-exposuredip-DE65F84A` - m48, master's own binary. **STILL LOAD-BEARING:
  it is m49 A1's P-C7 v3 A-SIDE** (the 8 banked `M49_GEDGE_*` legs ran on it) AND it was
  staged again in 073 for `A1F_PAIR_SYN_ASIDE`, the A-side that reproduced `NEITHER 1` at
  si=8 on master's own binary. Do not delete.

SWAPS THIS SESSION, STATED IN BOTH DIRECTIONS: DE65F84A -> EC60E9B9 (stage the A1 build),
EC60E9B9 -> DE65F84A (for `A1F_PAIR_SYN_ASIDE`), DE65F84A -> EC60E9B9 (restored). The final
staged hash was verified EC60E9B9 after the restore.

si=8 SYNTH-ORDER PAIRING EXCURSION, MEASURED AGAIN IN 073 AND STILL NOT BINARY-ATTRIBUTABLE:
EC60E9B9 read picCx 306.1 (delta 49.5, NEITHER 1) and 322.0 (delta 33.6, NEITHER 0) on two
runs of the SAME binary, and DE65F84A read 314.4 (delta 41.2, NEITHER 1) on a fresh run.
That is branch (a) of `tools/b28_si8_predeclared.md` - run-to-run variance at a marginal
frame - and `PREVIOUS == 0`, the half that detects mis-pairing, held on every leg. The 49.5
is LARGER than any previously banked value (33.6/38.2/38.9/40.7) and that is stated, not
smoothed; the tolerance was NOT widened.

--------------------------------------------------------------------------------------
SESSION 077 (2026-09-04) - m49 PHASE B, AND THE FIRST COOK SINCE m46
--------------------------------------------------------------------------------------
- `m46-container-m50exe-A159EDFD\` - the PRE-COOK QUARTET, rescued and hash-verified AT
  THE NEW LOCATION (A62) BEFORE the cook. It had never been archived: the m46 container
  (EF8EB23C / A8BFFF88 / 3C026A8D + A16A18A8 / C70ECDAA) is what EVERY result from m46
  through m50 was measured on, and only the m45 container had a copy. **LOAD-BEARING** -
  it is the only way to reconstruct the binary+container pair behind m46, m47, m48, m49
  A1/A2 and m50. Contains all five container files plus the m50 exe A159EDFD.
  Do not delete.
- `StackOBot.exe.m50-A159EDFD` - still LOAD-BEARING: it is m49 phase B's P-C7 v3 A-SIDE
  (the 8 banked `M50G_*` legs and `PB5C_AEON_A` all ran on it) and it carries the
  exposure A-side reading that ruling 1 is measured against. Do not delete.

NEW QUARTET AFTER THE PHASE B COOK (G121 - a baseline is exe + utoc + ucas + pak):
  exe                    2AFD2DDB   241,399,296 B
  StackOBot-Windows.utoc 67EA1FE0       294,235 B
  StackOBot-Windows.ucas 2CEFB8F4   364,596,784 B
  StackOBot-Windows.pak  E03C6610    10,115,711 B
  global.ucas            A16A18A8  UNCHANGED
  global.utoc            C70ECDAA  UNCHANGED

**BOTH global.* FILES ARE BYTE-UNCHANGED ACROSS A GLOBAL-SHADER COOK, AND THAT IS NOT A
SIGN THE SHADER DID NOT LAND.** The plugin's global shaders live in the PROJECT container
(`StackOBot-Windows.ucas`, which moved), not in `global.ucas`. The evidence the shader
reached the artifact is the cook log naming `FAnomalyVisibleMaskPS` and
`FAnomalyMaskReduceCS` compiled 2 permutations each, and then THE BOOT (runbook 8.6 step
3.7): the staged build reaches `Game Engine Initialized` and `Bringing World ...
CB_GateLevel up for play` with ZERO `Missing global shader` lines. A string scan of the
container proves nothing either way (shader bytecode in IoStore is not text).

MAP SET read back from the artifact: CB_GateLevel + MainMenu + MainWorld + Entry, all
PRESENT, `verify_cooked_maps.ps1` exit 0. The utoc answered in ASCII this time (4 hits
ascii, 0 utf16) - the tool scans both encodings for exactly this reason.

ENFORCED TOKEN read back from the RUNNING build: 64 chars, non-placeholder. G118 stays closed.

GATE BINARY FOR THE m49 PHASE B CAMPAIGN (code-only hot-swap over the phase B container):
  StackOBot.exe.m49b-gate-B8AC65D8    exe B8AC65D8, 241,399,296 B
  paired with the PHASE B container   67EA1FE0 / 2CEFB8F4 / E03C6610 + A16A18A8 / C70ECDAA
It is 2AFD2DDB (the cooked exe) plus commit 1c3d62c, which touches ONE .cpp and ZERO shader files -
verified with `git diff --name-only 5e45339..1c3d62c`, so the .usf source hashes are unchanged and
the cooked global shader map still matches (G103; G129 does not bite). **Every m49 phase B gate leg
ran on this exe.**

ALSO ARCHIVED: m49b-postcook-container-2AFD2DDB\ - the cooked exe AND its container, taken BEFORE the
pre/post A/B swap. **That archive is what allowed 2026-09-06 to re-run the post-cook pose A/B with
the EXACT artifacts two days later and overturn a wrong conclusion (G257).** An archived artifact is
what turns "I was wrong" into a measurement instead of an argument. Do not delete either directory.

--------------------------------------------------------------------------------------
SESSION 077R-02 (2026-09-06) - m49 PHASE B MERGED TO master; THE PHASE B PAIR IS NOW LIVE
--------------------------------------------------------------------------------------
`m49-phase-b` was merged to master as **4a4bfcc** (--no-ff, pushed). The merge is INERT:
`master^{tree}` == `m49-phase-b^{tree}`, so nothing entered master that was not on the gated branch.

NO REBUILD WAS NEEDED, AND THAT WAS VERIFIED RATHER THAN ASSUMED:
  `git diff --name-only 1c3d62c HEAD -- Source/ Shaders/ Config/ *.uplugin` is EMPTY.
  The only files that moved between the gate binary's source and master are docs. Therefore the
  archived gate exe B8AC65D8 IS what master builds, and staging it is not a stale artifact.

STAGE SWAP THIS SESSION, STATED IN BOTH DIRECTIONS (G121 - a pair is exe AND container):
  FROM: exe A159EDFD + m46 container EF8EB23C / A8BFFF88 / 3C026A8D + A16A18A8 / C70ECDAA
  TO:   exe B8AC65D8 + PHASE B container 67EA1FE0 / 2CEFB8F4 / E03C6610 + A16A18A8 / C70ECDAA
  All six files hash-verified AT THE DESTINATION after the copy (A62).
  The A-side was archived COMPLETE before the swap and remains so - `m46-container-m50exe-A159EDFD\`
  holds all five container files PLUS the exe A159EDFD, so the swap is fully reversible.

ARCHIVED A-SIDES, BOTH STILL LOAD-BEARING - DO NOT DELETE:
  - `StackOBot.exe.m50-A159EDFD` + `m46-container-m50exe-A159EDFD\` - m49 phase B's P-C7 v3 A-SIDE
    (the 8 banked M50G_* legs and PB5C_AEON_A ran on it) and the exposure BEFORE picture that
    ruling 1 of brief 077R-02 is measured against. It is ALSO the only reconstruction of the
    binary+container pair behind m46, m47, m48, m49 A1/A2 and m50.
  - `m49b-postcook-container-2AFD2DDB\` - the cooked exe AND its container. It is what let 2026-09-06
    re-run the post-cook pose A/B with the EXACT artifacts and overturn a wrong conclusion (G257).

CORRECTION TO THE SESSION 077 BLOCK ABOVE, STATED RATHER THAN SILENTLY EDITED:
  it records `StackOBot.exe.m49b-gate-B8AC65D8    exe B8AC65D8, 241,399,296 B`.
  **The size is WRONG - it duplicated the COOKED exe's size.** Measured:
      B8AC65D8 (gate exe)   241,400,320 B
      2AFD2DDB (cooked exe) 241,399,296 B
  A 1,024-byte difference, which is what a code-only relink of one .cpp looks like. The HASHES in
  that block are correct and nothing rests on the size; it is corrected here because a baselines
  README whose numbers cannot be trusted is worse than one with a gap.

## m52 `stuck_low_mip` (branch `feat/m52-stuck-mip`, 2026-09-20) — sessions 080-02 and 080-03

  - `StackOBot.exe.m52-stuckmip-A8742A4A`          exe A8742A4A, 241,468,416 B  — **LOAD-BEARING**
    The 080-02 binary. It is the **A-SIDE of gate `G3b`**: the banked leg `M52_M52_G5_SETTLE8`
    ran on it and read ZERO held frames with 24 x "already at or below the requested resident mip
    count", which is the BEFORE picture the 080-03 fix is measured against. Do not delete it.

  - `StackOBot.exe.m52-verified-restore-DC8BA0C1`  exe DC8BA0C1, 241,508,352 B  — **STAGED**
    The 080-03 binary and the one currently in
    `Builds\BenchGate\Windows\StackOBot\Binaries\Win64\`. Every `M52C_*` banked leg and both
    `M52C_LYRA_*` legs ran on it.

  Container quintet for BOTH: 67EA1FE0 / 2CEFB8F4 / E03C6610 + A16A18A8 / C70ECDAA, hashed before
  AND after every swap and BYTE-UNCHANGED. Code-only hot-swap, no cook (G103).

  STATED RATHER THAN HIDDEN: an intermediate 080-03 binary `FA9C1737` (241,507,840 B) was staged,
  ran ONE leg, and was NOT archived — the m40 `DC16710D` precedent. It carried two defects that
  leg exposed (the `FName` JSON-key case collision, G267, and the two onset counters differing by
  one). The loss is bounded and it is rebuildable by reverting the fix hunks; the leg itself is
  banked as `M52B_PREFIX_FA9C1737_NAMEKEY_EVIDENCE` and is the evidence behind G267.

## m52 080-04 (branch `feat/m52-stuck-mip`, 2026-09-20) — the ratio gate, the destroyed target, the VT probe

  - `StackOBot.exe.m52-ratio8-destroy-F65A8C90`     exe F65A8C90, 241,546,240 B  — **LOAD-BEARING**
    The first 080-04 binary: perceptibility ratio 4.0 -> 8.0, `too_small_for_ratio`, the
    `OnEndPlay` target-destroyed revert, `IAI.Bench.DestroyTarget`, `IAI.Bench.StuckMipVirtualProbe`.
    ⛔ **It is the A-SIDE of the `G4e` A/B and the ONLY binary that exhibits the `ComputeFireActive`
    over-claim (G269)**: its banked leg `M52D_M52D_G4E_DESTROY` reads `injected=[35..41]` against
    `affected=[35,36]`, with frames 37-41 carrying no anomaly entry at all. The B-side reads
    `injected == affected`. **Do not delete it** — without it that defect has no before picture.
    Its `M52D_*` legs (`G3_HOLD_NAT`, `G2_CANFAIL`, `G4E_DESTROY`) all ran on it.

  - `StackOBot.exe.m52-firewindow-fix-5588F6FB`     exe 5588F6FB, 241,546,752 B  — **STAGED**
    The 080-04 gate binary: `F65A8C90` plus the one-branch `ComputeFireActive` fix discriminated on
    `F.bWholeFrameExtent`. **This is what currently sits in
    `Builds\BenchGate\Windows\StackOBot\Binaries\Win64\`.** Every `M52E_*` banked leg ran on it,
    and so did both `M52E_LYRA_*` legs. Container quintet BYTE-UNCHANGED across both swaps
    (`67EA1FE0` / `2CEFB8F4` / `E03C6610` + `A16A18A8` / `C70ECDAA`) — code-only hot-swap, no cook.

  ⚠ THE STAGED EXE IS AN m52 BINARY WHILE THE MAIN CHECKOUT IS BACK ON `m51`. That is deliberate
  (the m52 arc leaves its own binary staged between sessions, as 080-02 and 080-03 did), and it is
  exactly the condition shared-tree rule 4 says to check: **verify the staged hash before any leg
  and re-stage if you need a different one.** The m51 F1 candidate is preserved beside it as
  `StackOBot.exe.m51-f1-candidate-D50DDE78`.

## m53 082-05 (branch `feat/m53-uv-normal-corruption`, 2026-09-27) — S1 code, built, NOT STAGED

  - `StackOBot.exe.m53-s1-D7BA87AC`     exe D7BA87AC, 241,926,144 B  — **NOT STAGED**
    The game target of commit `bb048bb` (m53 S1 plugin code: the decision tree, the per-mip draw and
    copy, the transaction, the restore, the S1 bench levers, private RenderCore/RHI). Built in the
    scratch host `E:\IA_BuildCache\_r53_host\StackOBot` with the plugin as a detached worktree at
    `bb048bb` and the plugin's build products cleared first, so every plugin module was compiled from
    the committed bytes. Its embedded provenance strings name the E: host (the 081-44 note applies).
    ⛔ **Not staged by design (brief 082-05): the bench stays on `E0BE6F0A` until 082-06 cooks the
    corruptor assets and the F-SYN level and stages.** Without those assets every m53 fire on this exe
    refuses `assets_unavailable`, so it is only useful paired with the 082-06 container.
  - `m53-s1-editor-bb048bb\`  the five plugin editor DLLs + PDBs + `UnrealEditor.modules` of the same
    commit (`UnrealEditor-AnomalyInjector.dll` 5BC7DBBB, 1,107,456 B). 082-06's cook runs on EDITOR
    binaries (G47), so these are what its commandlet must load.

  ⚠ RECORD GAP FOUND, NOT FIXED HERE: this file had no entry for any m55 binary. The exe that
  currently sits in `Builds\BenchGate\Windows\StackOBot\Binaries\Win64\` is **E0BE6F0A**
  (241,728,512 B), byte-identical to `StackOBot.exe.m55-merge-strip-E0BE6F0A` (verified by SHA-256
  in 082-05), with the unchanged Phase B container quintet (`67EA1FE0` / `2CEFB8F4` / `E03C6610` +
  `A16A18A8` / `C70ECDAA`). The "STAGED" marker on the m52 `5588F6FB` entry above is stale.
## m53 082-06 (branch `feat/m53-uv-normal-corruption`, 2026-09-27) — pre-cook archive; the S1 cook ran, G-COOK FAILED, NOTHING STAGED

  - `m53-s1-precook-container-67EA1FE0\`  the bench as it stood before the m53 S1 cook, archived and
    **hash-verified at this location before the cook**: `StackOBot.exe` E0BE6F0A (241,728,512 B),
    `StackOBot-Windows.utoc` 67EA1FE0, `.ucas` 2CEFB8F4, `.pak` E03C6610, `global.utoc` C70ECDAA,
    `global.ucas` A16A18A8 (the Phase B container). The pre-cook IoStore listing is
    `_reviews\082-06-evidence\iostore-precook.csv`. This is G-COOK's A-side.
  - ⛔ **STILL STAGED: E0BE6F0A + the Phase B quintet above.** The S1 cook (exe 2FCDF059, container
    `651656EF` / `F01CAFC0` / `EC4EB9A7` + `462B8AC6` / `BB05CF99`) was NOT staged and is NOT archived
    here: G-COOK failed (MainWorld's cooked chunk is not reproducible across identical-input cooks, and
    five fixture packages were missing because the fixture tool's mesh-slot assignments did not persist).
    It stays at `E:\IA_BuildCache\_r53_cookout\` (a same-input control cook at `..._r53_cookout_ctl\`)
    as evidence only. Journal `docs/sessions/2026-09-27-082-06-code-m53-s1-authoring-cook.md`.

  RECORD GAP CLOSED (082-06b / 082-06c archived here without an entry; hashes re-read 082-06):
  - `StackOBot.exe.m53-s1fix-8BF054FA` exe 8BF054FA (241,942,528 B) and `m53-s1fix-editor-1d7cada\` —
    082-06b's rebuild after Codex's source review, NOT STAGED.
  - `StackOBot.exe.m53-s1rem-2FCDF059` exe 2FCDF059 (241,945,600 B) and `m53-s1rem-editor-1fc20ea\`
    (11 files) — 082-06c's rebuild after the two remainders, NOT STAGED. 2FCDF059 is also the exe the
    082-06 cook packaged (the host's game target was up to date).

## m53 082-06d (branch `feat/m53-uv-normal-corruption`, 2026-09-27) — fixture fixed, re-cooked, G-COOK PASSED, **STAGED**

  - `m53-s1-cook-9A26D497\`  the m53 S1 build now on the bench, archived and **hash-verified at this
    location before staging** (`_reviews\082-06d-evidence\archive-candidate.json`):
    `StackOBot.exe` **2FCDF059** (241,945,600 B; = `StackOBot.exe.m53-s1rem-2FCDF059`, the host's game
    target at plugin source `1fc20ea`/`c9f8089`), `StackOBot-Windows.utoc` **9A26D497** (313,123 B),
    `.ucas` **10D7F0C0** (365,221,520 B), `.pak` **ABD931A2** (10,119,807 B), `global.utoc` **462B8AC6**
    (539 B), `global.ucas` **BB05CF99** (1,833,072 B).
  - Provenance: cook **d1** of 13 identical-input cooks, all on the scratch host
    `E:\IA_BuildCache\_r53_host\StackOBot` (plugin worktree detached at `c9f8089`, `Content` junctioned
    to the real D: content), inputs = M1′ (`_reviews\082-06d-evidence\M1prime-manifest.json`: M0's 2,082
    files byte-identical, the fixture as re-authored by CaptureBench `47d6fff`'s slots mode). Cook command
    `_reviews\082-06d-evidence\cook-d1-command.txt`; 899 packages, 0 warnings, 0 errors. d1 was named the
    candidate before any G-COOK reading. Controls d2…d13 stay at `E:\IA_BuildCache\_r53_cookout_d2…d13\`
    as evidence only.
  - G-COOK (amended `MainWorld` row, ruling 082-06d): `CB_GateLevel` chunk identical to the A-side;
    `MainWorld` header identical and its difference from the A-side inside the 13-cook variance region
    (0 words at every alignment, 0 bytes); 101/101 fixture + 3/3 plugin packages present; map gate exit 0.
  - **✅ STAGED 2026-09-27 06:32 IST** into `Builds\BenchGate\Windows\StackOBot\` — exe + the container
    quintet only, each hash-verified at the destination (`staging-receipt.json`); the launcher stub,
    manifests and PDB were not copied. Replaced: E0BE6F0A + `67EA1FE0` / `2CEFB8F4` / `E03C6610` +
    `C70ECDAA` / `A16A18A8`, which remain archived at `m53-s1-precook-container-67EA1FE0\` (the rollback).
    A44 on the staged exe: every m53 token present in UTF-16, controls `IAI.Capture.ShaderPrewarm` 7 and
    `IAI.Bench.StuckMipNoHold` 5 (`a44-staged.txt`).
  - ⚠ **The main checkout is on `m51`, so a D:-side rebuild would NOT reproduce this exe.** Any leg must
    first verify the staged hash (shared-tree rule 4).
  - 🔻 **SUPERSEDED ON THE BENCH by 082-07c (below), 2026-09-27 10:21 IST.** This archive stays: it is the
    build every 082-07b leg ran on (the 12 accepted UV rows and the N-N1 stop), and the 082-07c rollback.

## m53 082-07c (branch `feat/m53-uv-normal-corruption`, 2026-09-27) — normal-family fixture fixed, re-cooked, G-COOK PASSED, **STAGED**

  - `m53-s1-normalfix-cook-30FE0FDE\`  the m53 S1 build now on the bench, archived and **hash-verified at
    this location before staging** (`_reviews\082-07c-evidence\archive-candidate.json`, full SHA-256 6/6):
    `StackOBot.exe` **2FCDF059** (241,945,600 B, UNCHANGED — plugin source unchanged, every cook's build
    step "Target is up to date"), `StackOBot-Windows.utoc` **30FE0FDE**, `.ucas` **11231356**, `.pak`
    **FD766B7B**, `global.utoc` **462B8AC6**, `global.ucas` **BB05CF99** (both global files unchanged).
  - Provenance: cook **e1** of 3 identical-input cooks on the scratch host `E:\IA_BuildCache\_r53_host\StackOBot`
    (plugin worktree detached at `c9f8089`), inputs = 082-07c M1′ (`_reviews\082-07c-evidence\M1prime-manifest.json`):
    082-06d's fixture with ONLY `M_TC_UN1` / `M_TC_NN1` / `M_TC_ChainD` / `M_TC_NormalEmis` re-authored by
    CaptureBench `41ef8b9`'s `normals` mode (Default Lit, the `Tex` normal map into Normal as well as the
    Emissive readout). Cook command `cook-e1-command.txt`; 899 packages, 0 warnings, 0 errors. e1 named the
    candidate in `gcook-predeclare.md` before any cook ran. Controls at `E:\IA_BuildCache\_r53_cookout_e2…e3\`.
  - G-COOK (amended `MainWorld` row; region = 082-06d's 13 cooks ∪ this session's 3): `CB_GateLevel` chunk
    identical to the A-side; `MainWorld` header identical in 17 containers and diff(A, e1) inside the region
    (0 words at every alignment, 0 bytes — and 0 bytes outside 082-06d's region alone); 101/101 fixture +
    3/3 plugin packages; map gate exit 0. d1 → e1 non-shader changes: the four materials, the two shader
    archives, `MainWorld`'s known-variant bytes and the container header (`gcook-082-07c.json`).
  - **✅ STAGED 2026-09-27 10:21 IST** into `Builds\BenchGate\Windows\StackOBot\` — exe + the container
    quintet only, each verified at the destination (`staging-receipt.json`). Replaced `2FCDF059` + `9A26D497`
    / `10D7F0C0` / `ABD931A2` + `462B8AC6` / `BB05CF99`, archived at `m53-s1-cook-9A26D497\` (the rollback).
    A44 on the staged exe: every m53 token in UTF-16 incl. `normal_unconnected` 1 / `no_normal_map` 1,
    controls `IAI.Capture.ShaderPrewarm` 7, `IAI.Bench.StuckMipNoHold` 5, `IAI.Bench.MaskPairingProbe` 5.

## m53 082-07f (branch `feat/m53-uv-normal-corruption`, 2026-09-28) — `TC_Masked` fixed (never clipped), re-cooked, G-COOK PASSED, **STAGED**

  - `m53-s1-maskedfix-cook-20DA6F98\`  the m53 S1 build now on the bench, archived and **hash-verified at
    this location before staging** (`_reviews\082-07f-evidence\archive-candidate.json`, full SHA-256 6/6):
    `StackOBot.exe` **2FCDF059** (UNCHANGED — no plugin source change), `StackOBot-Windows.utoc` **20DA6F98**,
    `.ucas` **534C5863**, `.pak` **FD766B7B**, `global.utoc` **462B8AC6**, `global.ucas` **BB05CF99**.
  - Provenance: cook **f1** of 3 identical-input cooks on the scratch host (plugin worktree detached at `c9f8089`),
    inputs = 082-07f M1‴ (`_reviews\082-07f-evidence\M1ppp-manifest.json`): 082-07c's fixture with ONLY
    `M_TC_Masked` re-saved by CaptureBench `2c5ec56`'s `masked` mode (RecompileMaterial with OpacityMask
    connected, so its saved `bCanMaskedBeAssumedOpaque` is false and `GetBlendMode()` returns Masked).
    899 packages, 0 warnings, 0 errors each. f1 named the candidate in `gcook-predeclare.md` (SHA-256
    `36F325E0…`) before any cook ran. Controls at `E:\IA_BuildCache\_r53_cookout_f2…f3\`.
  - G-COOK (region = 082-06d's 13 cooks ∪ 082-07c's 3 ∪ this session's 3): `CB_GateLevel` chunk identical to
    the A-side; `MainWorld` header identical in 20 containers and diff(A, f1) inside the region (0 words at
    every alignment, 0 bytes); 101/101 fixture + 3/3 plugin packages; map gate exit 0. e1 → f1 non-shader
    changes: `M_TC_Masked`, the two shader archives, `MainWorld`'s known-variant bytes and the container
    header; shader chunks 1,075 → 1,077 (`gcook-082-07f.json`).
  - **✅ STAGED 2026-09-28 01:27 IST** into `Builds\BenchGate\Windows\StackOBot\` — exe + the container
    quintet, each verified at the destination (`staging-receipt.json`). Replaced `30FE0FDE` / `11231356`
    (utoc / ucas; exe, pak and globals unchanged), archived at `m53-s1-normalfix-cook-30FE0FDE\` (the
    rollback, re-verified 6/6 against the staged files before staging). A44 on the staged exe: token table
    identical to 082-07c's (`a44-staged.txt`).

## m52 fix 084-07b (branch `fix/m52-label-timing`, 2026-09-29) — cooked set, archived, NOT staged

  - `m52fix-cook-87A46A27\`  exe **FF41BFF3** (= `m52fix-FF41BFF3\`, unchanged) + `StackOBot-Windows.utoc` **87A46A27**, `.ucas` **55A65BA4**,
    `.pak` **26FFC026**, `global.utoc` **462B8AC6**, `global.ucas` **BB05CF99**; re-hashed at this location 6/6 (`archive-hashes.json`).
    Cooked on `_r84_host` at `a6a4893` (source `8b8b4d0`), 60 s, 794 packages, 0 warnings. Its cooked `AnomalyInjector.uplugin` is
    byte-identical to the branch (5 modules incl. `AnomalyBench`, Shipping-denied) - the G354 descriptor proof. The m53 set
    (`m53-s1-maskedfix-cook-20DA6F98`) stays staged. Carries D1/G361 (texture-swap `labelled` false); see journal 084-07b.

## m52 fix 084-08b (branch `fix/m52-label-timing`, 2026-09-29) - the LOD fixture cook, archived, NOT staged

  - `m52fix-cook-BF06AE61\`  exe **8A6074AA** (= `m52fix-8A6074AA\`, unchanged) + `StackOBot-Windows.utoc` **BF06AE61**, `.ucas` **B294A1C8**,
    `.pak` **87FF0240**, `global.utoc` **462B8AC6**, `global.ucas` **BB05CF99**; re-hashed at this location 6/6 (`archive-hashes.json`).
    Cooked on `_r84_host` at `17b9d5d` (source `4bf0915`), 77 s, 801 packages (794 + the 7 fixture packages), 0 warnings. The only input
    change from `m52fix-cook-4BF63FB8` is the bench-only LOD fixture (`/Game/AnomalyFixtures/`, 7 files; D: content manifest diff = those
    7 files) and `CB_LodFixture` in the map list. Cooked `AnomalyInjector.uplugin` byte-identical to the branch (G354). Container diff and
    the cook-to-cook baseline in its README. The 084-09 harness pins this pairing as FIX. The m53 set stays staged. See journal 084-08b.
  - `m52fix-667FD4EF\`  exe **667FD4EF** + `editor\` (the five `UnrealEditor-Anomaly*.dll`), 084-10 release hygiene: the one bench gate, source
    `92c3500` on `fix/m52-label-timing`; 6/6 re-hashed at this location (`archive-hashes.json`). Code-only, NOT cooked, NOT staged, NOT
    swapped in (084-09 keeps its pins). Legs on it need `-IAIBench` / `-IAIBenchFixture` to reach any `IAI.Bench` lever (G377). See journal 084-10.

  - `m53dc-27055E55\`  exe **27055E55** + the five `UnrealEditor-Anomaly*.dll`, 085-02 m53 delivery cut (DC) source `b1cfc8a` on `feat/m53-uv-normal-corruption` (the merge of `fix/m52-label-timing` 02d15a4 at `7cda2f1` plus the DC); clean plugin rebuild on `_r53_host`, 0 warnings both targets; 6/6 re-hashed here (`archive-hashes.json`); `lever-audit-binary.txt` 46/46 names, echo present; `a44-08502-game.txt` the DC token scan. Code-only, NOT cooked, NOT staged, NOT swapped in. Legs on it need `-IAIBench` for any `IAI.Bench` lever (G377). See journal 085-02.

- `m53dc-cook-F76F1119\` - **085-03, 2026-09-29: the m53 DC bench pairing (NOT a delivery build).** Merged build `feat/m53-uv-normal-corruption` source `270ed32` (docs head `c6aa9c9`), exe **6B671987** (= 085-02c's build, byte-identical: built == cooked == archived) + utoc **F76F1119** / ucas B54C2235 / pak BBC9A58C / globals 462B8AC6 / BB05CF99. Cooked on `_r53_host` (cook dc1 of two identical-input cooks, 74 s; dc2 differs only in the shader archives, MainWorld.umap and AssetRegistry.bin, G371). Maps CB_GateLevel, MainMenu, MainWorld, Entry, CB_TexCorruptLevel (map gate PASS; inverted probe for CB_LodFixture exit 1). Container diff vs `m53-s1-maskedfix-cook-20DA6F98`: +13 DC packages (+2 .ubulk), CB_TexCorruptLevel.umap changed, plus the cook-variant entries. Descriptor: STOP EXTRA AnomalyBench, FIXTURE without `--bench-cook` (correct: never delivered), PASS with it. `lever_audit.py --binary` PASS (46 names). Staged by the 085-04 harness only, receipt per stage; restore target is `m53-s1-maskedfix-cook-20DA6F98`.

- `m53dc2-cook-45E2FA86\` - **089-02b, 2026-09-30: the m53 re-bench pairing (DC2; NOT a delivery build).** Exe **6B671987** (the archived DC exe, the build of `270ed32`; this cook's `-build` relinked the host exe to ECC3F085, NOT used, G201) + utoc **45E2FA86** / ucas **8CA6FF5C** / pak **B35DBE96** / `global.utoc` **462B8AC6** / `global.ucas` **BB05CF99**; 6/6 re-hashed at this location (`archive-hashes.json`). The only input change from `m53dc-cook-F76F1119` is the 089-02b `dcfix` of the m53 fixture (`CB_TexCorruptLevel.umap` + `M_TC_OracleN.uasset`; D: content manifest diff = those 2 files; the cook changed 0). Cooked on `_r53_host` (detached 270ed32), 912 packages, 0 errors; staging redirected to `E:\IA_BuildCache\_r53_stage_rb3` and the container taken from there because the environment refused the NonUFS copy of `dbghelp.dll` (any file of that name, since about 03:00; the pak / IoStore step had exited 0 before it). Map gate 5/5 (+ CB_LodFixture MISSING as the inverted probe); container diff vs F76F1119: the fixture map, M_TC_OracleN and G371's set (MainWorld + both shader archives), 3 of 1,084 unnamed chunks replaced; pak AssetRegistry only. Pinned by `_reviews\089-04-dc2pin.json`; the 089-04 harness stages it as DC2 and restores m53 S1. See journal 089-02b.

- `m52fix-EB01156E\`  exe **EB01156E** + `editor\` (the five `UnrealEditor-Anomaly*.dll`), 090-01 final source round, source `c12b7e9` on `fix/m52-label-timing` (TAA off-frames default 16, LOD log wording, readme); 6/6 re-hashed (`archive-hashes.json`); lever audit `--binary` PASS 33/33 vs the branch, and a delivery-shaped exe from the staged folder PASS 32/32 vs that folder. Code-only, NOT cooked, NOT staged, NOT swapped in. See journal 090-01.

- `m53-09001-CE55D997\`  exe **CE55D997** + `editor\` (the five `UnrealEditor-Anomaly*.dll`), 090-01, source `d57246c` on `feat/m53-uv-normal-corruption` (the merge `3ca5dcb` of `fix/m52-label-timing` `0347e8c` + the exclusion selftest fix + the readme §8.8); 6/6 re-hashed (`archive-hashes.json`); lever audit `--binary` PASS 46/46 vs the branch, and a delivery-shaped exe from the staged folder PASS 45/45 vs that folder. Code-only, NOT cooked, NOT staged, NOT swapped in. See journal 090-01 §5-§9.

- `m53-0910b-65607703\`  exe **65607703** + `editor\` (the five `UnrealEditor-Anomaly*.dll`), 090-10b, 2026-10-01: source `0a69ea3` on
  `feat/m53-uv-normal-corruption` - the build that passed the in-engine "nothing corrupts" proof (PIE and staged game: targeted uv 6/6,
  normal 6/6 visibly corrupt; edges 0 off). Host shader readiness per vertex factory, corruptor `EnsureIsComplete`, census time-sliced,
  eligibility-aware auto-pool, the fix branch's yield/startup lines. 6/6 re-hashed at this location (`archive-hashes.json`). Code-only,
  NOT cooked; for staged legs it was swapped into BenchGate over the DC2 container (`m53dc2-cook-45E2FA86\`) and BenchGate was put back
  to the m53 S1 set afterwards. Strict 0/0, lever 47/47, string scan, C++ suite summary beside it. See journal 090-10b.
- `m52fix-0910b-DA903919\`  exe **DA903919** + `editor\`, 090-10b: source `cf422d5` on `fix/m52-label-timing` (never-silent yield lines,
  `IAI-STARTUP`, `anomaly_refusal_counts.py`). 6/6 re-hashed (`archive-hashes.json`). Code-only, NOT cooked, NOT staged. See journal 090-10b.
- The 090-10b intermediate m53 exes `01DBA661` (`c0896df`, the refuted render-thread flag attempt), `2F924FB3` (`c7bbc51`) and
  `07A65316` (`9f3bae1`) are kept with their editor DLLs under `E:\IA_BuildCache\_r910b\builds\` (not archived here); every leg that
  read them is banked with the session.

- `m53-0910b2-E2BCE76B\`  exe **E2BCE76B** + `editor\` (the five `UnrealEditor-Anomaly*.dll`), 090-10b2, 2026-10-02: source `61d186a` on
  `feat/m53-uv-normal-corruption` (readiness: identity before draw readiness, base-pass VS+PS for the component's vertex factory,
  `draw_shaders_missing`; census `timing` line). The strict pass's normal rebuild; every 090-10b2 PIE and staged leg ran on these bytes.
  6/6 re-hashed (`archive-hashes.json`). Code-only, NOT cooked; staged legs swapped it into BenchGate over DC2 (`m53dc2-cook-45E2FA86\`).
  Strict, lever, string scan, C++ and Python summaries and the pre-declarations beside it. See journal 090-10b2.
- `m52fix-0910b2-5E93A74E\`  exe **5E93A74E** + `editor\`, 090-10b2: source `a5f0a06` on `fix/m52-label-timing` (Source as `c60de04`:
  the stuck_low_mip purity line names its world; HELD NONE wording). 6/6 re-hashed. Code-only, NOT cooked, NOT staged. See journal 090-10b2.
- The 090-10b2 intermediate m53 exe `109A8406` (`5fba7df`, pre-strict) is kept under `E:\IA_BuildCache\_r910b2\builds\5fba7df\`; no leg read it.

## 093-06 J2 candidate, 2026-10-14 - UNQUALIFIED, D2 STOP, NOT PUSHED

- `followup-capture-J2-09306-f608d56-4BB0F074-B0F2B30C-UNQUALIFIED`: product `f608d56`, local documentation head `bb9e741`; StackOBot EXE **4BB0F074**, Lyra EXE **B0F2B30C**. Current and retired candidates plus five unchanged StackOBot J1 containers, helper source and evidence; 470/470 files re-hashed. Manifest SHA-256 `507e2aef1c8f76b746280792365bbeeb26cf062a0633932a8925980d8a3d13e9`. Copied one-byte DLL mutant rejected.
- 22 planned engine cases pass; Lyra selected-viewport PIE fails: 96 positive label rows with zero target pixels/masks, while the target is outside the actual picture. 61 planned cases unrun after D2. Hosts restored, live staging unchanged, remote `fix/followup-capture` stays `1bf7054`. No delivery or office qualification. Full report: `_relay/runs/2026-10-14-093-06-j2-office-robust-f1/report.md`.


## 093-13 exposure-exclusion repair, 2026-10-16 - UNQUALIFIED, SCOPE STOP, NOT PUSHED

- `followup-capture-J2-exposure-09313-9781031-D8D30951-DFE6F164-UNQUALIFIED`: product `9781031`, closing head `0bc6e9f`; Stack EXE **D8D30951**, Lyra EXE **DFE6F164**. All eight normal/strict builds pass 0/0. Both new Stack camera runs survive cut/blend; active primary pixel/mask/moving-box floors pass, but the full gate stops because its frozen negative-control donor 299 is a named pre-blend drop. No reader edit, retry, identity inheritance, cook, staged proof or push.
- 4019/4019 archived payloads rehashed, byte-mutant rejected; manifest SHA-256 `b2fd25bb0921f98f86283191d08bc2fd1944fa34b2edab0a47b50eca0d872b55`. Hosts/content and 5,764 live-stage files restored. Remote stays `1bf7054`. Full report is retained inside this archive.


## 093-14 drop-aware donor/full rerun, 2026-10-16 - UNQUALIFIED

- `followup-capture-J2-donor-09314-9781031-D8D30951-DFE6F164-OVERFLOW-UNQUALIFIED`: product `9781031`, qualification head `0e50b56df4baf516ec815f433a93084388e2cf05`; Stack EXE **D8D30951**, Lyra EXE **DFE6F164**. The drop-aware camera controls and all 42 fresh PIE ledger cases passed on unchanged product 9781031. Qualification stopped at the dedicated TSR setup reader: it confuses target-mask serials with colour sample indices. Hosts and staging are restored; cook, staged gates and push remain unrun, and the candidate remains UNQUALIFIED.
- 88146 payload files rehashed; byte-mutant rejected. Manifest SHA-256 `347274f3a818ab578bd4e6020612dcc29daf255e6afafecb87b38b5df91a7b8f`. Hosts/content and all 5,764 live-stage files restored. Remote `1bf7054f5b8664f057282ed863a62ab5dbbd60e3`. Full report retained inside this archive.
