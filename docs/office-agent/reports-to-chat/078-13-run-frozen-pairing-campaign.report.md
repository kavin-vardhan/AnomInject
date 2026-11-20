# REPORT for 078-13-run-frozen-pairing-campaign.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 078-13-run-frozen-pairing-campaign.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-06 20:40:13 | finished: 2026-09-06 21:00:57 | duration: 21 min | exit code: 0
context at last turn: 448413 tokens (45% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\078-13-run-frozen-pairing-campaign.live.txt

---- FINAL MESSAGE (Claude Code) ----
The campaign is complete, banked, and stopped. Final state verified: bench restored to the candidate, container frozen, zero surviving processes.

# REPORT — 078-13, `m51` frozen pairing campaign

**The campaign ran and stopped on its own stop rule at leg 2.** Leg 1 (the fresh A-side reference) is VALID and PASSES. Leg 2 (the first candidate cell) HALTED after four consecutive pose-invalid attempts and was given **no verdict**. That is the attempt-exhausted stop class: it releases **no** A control, and no remaining candidate cell was run.

## Preflight (all verified before anything was staged)

`m51` HEAD `d727aec8…6df829` · `master` == `origin/master` == `ac13700d…63a3f2` · `git diff 6713f51..HEAD -- Source Shaders` **EMPTY** · 10 untracked handoffs preserved · CaptureBench `472a409d…41c58` tracked-clean with its 7 untracked tools · Lyra detached `4a4bfcc2…ae9b63`, clean. Prediction doc **`8D7951F542D61658738D8ED4716D98FCCF563C10ABDCCF2EFBDFF05197D2E9B5`, 691 lines** — exact match. `run_leg.ps1` `9E8D669F…B945` and `m44_pairing_probe.py` `918EB4B5…7AF1F` — unchanged. Both archives hash-verified. All five container files frozen. **81 paths checked (9 banks + 36 `_tryN` + 36 raw attempt dirs), 0 clashes.** Evidence dir absent. **Zero `StackOBot*` processes.**

## Per-cell results — one row each

| leg | binary | order | SP | attempts | validity | PAIRING | decidable / CURRENT / PREVIOUS / NEITHER / same-pos / no-data | fps | `speed_ratio` |
|---|---|---|---|---|---|---|---|---|---|
| `F1R_A_REF_NAT` | A `B8AC65D8` | natural | 100 % | **1** | **VALID** | ✅ **PASS** | **33 / 33 / 0 / 0 / 0 / 7** | **29.9237** | **1.002550** |
| `F1R_B_NAT` | cand `D50DDE78` | natural | 100 % | **4** (all discarded) | ⛔ **INVALID — attempt-exhausted (pose)** | **NO VERDICT** | **UNKNOWN** — no accepted session; checker not runnable | UNKNOWN | UNKNOWN |

**Failing pairing indices: none on leg 1** — `PREVIOUS` and `NEITHER` are both empty, so there is no index list. Leg 2 has no pairing table at all (§ below).

**Leg 1 session:** `D:\IntrusiveAnomalies\_bench_sessions_bank\M51R_F1R_A_REF_NAT\session_20260906-204916`. Checker exit **0** (read, not a pass): `TOTALS decidable=33 CURRENT=33 PREVIOUS=0 NEITHER=0 (same-pos=0 no-data=7)` · `VERDICT: MASK IS THE CURRENT RENDER on every decidable frame`. No-data at `si` 15, 24, 25, 26, 32, 34, 35 — a legitimate mask outcome per `A1.3`.

Every validity condition met individually: **coverage 33 ≥ 20** (the new floor's first exercise — it did not bind) · **4 counted events ≥ 3**, `label_schema 2` · 40 rows `session_index` 0..39 and 40 PNGs `frame_00000..039` at **1280×720**, no duplicate / missing / orphan / row-without-RGB / zero-byte · live probe `tag=255` with **40 `PROBE STEP`** lines · all 34 `mask_file` references present on disk. Effective config read from the leg's own log: SVE grab point, mask/target-mask/census ON at compiled defaults, output height 0 (native), `Marker "1"`, AE `"0"`, **`m40 SYNTH TICK ORDER = off`**. **The 100 % arm is read from rendered geometry** — `internalViewRect`/`unscaledViewRect` both 1280×720, one distinct pair, **no `r.ScreenPercentage` echo**, exactly as `A1.3` predicted and forbade requiring.

## The halt

Runner exit **2**: `HALT: 4 consecutive invalid attempts on F1R_B_NAT. No verdict is offered.`

🚨 **The four failures are two different shapes under one label.** try2/try4 are genuine **A47 rotation bifurcation** (`modal_rot` (338.54, 358.63, 0.0) and (358.05, 349.65, 0.0), bbox off calibration, non-uniform ratios). **try1/try3 are not pose displacement at all** — `modal_rot` exactly `(0,0,0)`, bbox **byte-for-byte `CALIB_BBOX`**, `pose_match=True` — failing only the `a56` self-consistency conjuncts (`distinct=2`, `modal 64.0 %`). That is the **third cause** behind `POSE GATE FAILED (B1)`, the `m26` P24 shape. ⛔ **Cause not established for either and none attributed.**

🔑 The discard is `A63`'s shape, not a laundered failure: the condition is camera pose, fixed at run start, read under a rule written days earlier — **and the pairing outcome was never computed.**

⛔ **No candidate pairing number is reported, and that is structural:** the runner halts before its final bank copy (`:481-489` precedes `:491-494`), so `M51R_F1R_B_NAT` **does not exist** (measured), and the predeclared checker resolves *exactly one* session there and throws when it cannot. Reading a verdict off a pose-invalid leg is what **`P8`** forbids.

✅ **The leg was correctly configured** — all four attempts: 40 frames / 40 rows / 40 PNGs, 0 zero-byte, 4 events, schema 2, probe live with 40 `PROBE STEP`, `SYNTH TICK ORDER = off`, `Marker "1"`, viewrect 1280×720/1280×720, `speed_ratio` 1.000006–1.002026 at 29.94–30.00 fps.

⚠ **Observation, n = 1, no mechanism:** the reference accepted on attempt 1 with `distinct=1 modal=100 %`; the candidate leg ~100 s later on the same box, container, fixture and recipe gave **4-of-4** pose-invalid attempts against A47's recorded ~2-in-5 band. ⛔ **Not attributed to the candidate binary** — the pose gate reads the camera only.

⚠ **One reading visible in the banked evidence that is NOT a result:** leg 1 (pre-`F1`) carries `target_mask_frames_unavailable = 1`; candidate try1 carries `0`. ⛔ **No comparison is drawn** — the candidate attempts are pose-invalid and certify nothing, and n = 1 either way.

## Stops and UNRUN

- **STOP fired:** attempt-exhausted pose invalidity on `F1R_B_NAT` (§6 / `A1.4`) — releases no control, not re-run, budget not extended.
- **UNRUN main cells:** `F1R_B_SYN` · `F1R_B_NAT_SP50` · `F1R_B_SYN_SP50`.
- **UNRUN and unreleased controls (all four):** `F1R_A_CTL_NAT` · `F1R_A_CTL_SYN` · `F1R_A_CTL_NAT_SP50` · `F1R_A_CTL_SYN_SP50`.
- Classes that did **not** fire: REFUSED · INVALID (UNDECIDABLE) · INVALID (coverage) · INVALID (event count).

## Retention — every physical attempt

Five physical attempts, five `_tryN` banks, five `_leg_game.log` files — **no missing receipt**; `A1.4`'s two receipt-less early-exit paths did not fire. Banks: `M51R_F1R_A_REF_NAT` (accepted alias → `session_20260906-204916`), `M51R_F1R_A_REF_NAT_try1`, `M51R_F1R_B_NAT_try1..try4` (sessions `…-205107/205114/205122/205130`). No accepted alias exists for `F1R_B_NAT`. Raw attempt directories copied as-is into `…\M51_FROZEN_PAIRING_RECHECK_EVIDENCE\attempts\`; nothing overwritten; the pre-campaign rotating log was preserved before the first launch (byte-identical to 078-09's banked copy).

## Receipts (sha256 of the receipt files)

| receipt | sha256 |
|---|---|
| `00_PRECAMPAIGN` | `96325260B11B1A8060C197630D0C13CB8DB2A0D02DEA07E9751CA3E675341A62` |
| `01_LEG1_A_BEFORE_SWAP` | `360A8409FE007F74EB256ECC55673E04C87BE5E9A42558A3A208CDEC327E481B` |
| `01_LEG1_A_AFTER_SWAP` | `2941583A176276D31C7FA1F26E085A3D5D85BD9CF68A319FB5164001E144C1C7` |
| `02_LEG1_POST` | `83267DDDDA33F7C3684FBA1FEC7D7337BC51AEAAFCD5603AC83A0748D0F86477` |
| `03_LEG2_CAND_BEFORE_SWAP` | `36F368DB50EB1C157EE89906B71093FE563AB213BE92306BF708E846BE70072C` |
| `03_LEG2_CAND_AFTER_SWAP` | `CD42BC868E6CBB2F92FC529A1A013AA47EEF9074AEA753490882DD2052559D41` |
| `04_LEG2_POST` | `EAD6BCABAA70221591AA72A5E8701FD687602C95F0DD330C876ABA2C403CCA30` |
| **`99_FINAL_RESTORE`** | `AD60C5E179EAD4C2429139C012D97BCC59950B1A8E50936C2269FC835CB90978` |

Transcripts: `runner\F1R_A_REF_NAT.runner.out.txt` `CDE08439…0B5B` · `runner\F1R_B_NAT.runner.out.txt` `0093F912…154A` · `checker\F1R_A_REF_NAT.checker.out.txt` `4167F5A3…6798` · `artifacts\F1R_A_REF_NAT.facts.txt` `21C0C52A…091BC`, plus both `.command.txt` files and `artifacts\F1R_B_NAT.attempts.facts.txt`.

**Final restoration:** staged exe `D50DDE789A94933182DC6B3FA74811F8BB57DC1F8332B4AC88ACE47337BD4796` (candidate, re-verified by hash — the last leg ran on it, so restoration is satisfied by identity, checked not assumed). **Container quintet byte-identical in all seven receipts:** `.utoc` `67EA1FE0…257A` · `.ucas` `2CEFB8F4…C4D3` · `.pak` `E03C6610…CDB7` · `global.ucas` `A16A18A8…BA9E` · `global.utoc` `C70ECDAA…21C51`. Both archives intact. Only the two pinned archives were ever copied. **Zero `StackOBot*` processes at every receipt and at exit.** The resident `UnrealEditor` (PID 7924) does not match the killed pattern; no owner application was touched.

## Docs commit

**`c73fc2d9274e0c247b25d61dbf108ceb7b478a2b`** — `docs(m51): 078-13 the frozen pairing campaign runs and stops at leg 2` — **3 files, 446 insertions / 0 deletions**, path-scoped adds, `commit --file` with an absolute message file. New journal `docs/sessions/2026-09-06-078-13-m51-frozen-pairing-campaign.md` (LF, no BOM) · `docs/OPERATING-CONTRACT.md` §9 `078-13` appended (+74, prefix byte-for-byte preserved, LF) · `CLAUDE.md` spliced (+72, CRLF 5965 → 6037, **zero bare LF**, no BOM, historical sections untouched). `git diff --check` clean. Source freeze re-verified EMPTY; tracked modifications 0; 10 handoffs still untracked; nothing pushed (`origin/m51` absent); no tag; no amend/reset/clean/stash.

## Attribution, stated as `A1.1` / `A2.4` govern

Both binaries failing would mean the failure occurs **without `F1`** — that **neither isolates nor excludes an `F1` contribution**. A control passing would be **consistent with** a candidate contribution with **time and machine state confounded**. Neither branch is causal proof, and neither establishes environment, FIFO ordering or a client cause. **Neither branch was reached here** — no control ran. Leg 1's PASS is **scoped pairing smoke** on these bytes and this recipe, through a probe with a declared **even-frame blind spot**; it is not `F1`/`m51` release certification and does not erase the failure bank. ⛔ **Nothing is established about the `F1` candidate.** The 078-05/078-06 failures are not reproduced, not refuted, not reclassified; **root cause remains OPEN and was not chased.** Everything in §10 stays HELD; `M-F1b` UNRUN; Lyra not waived.

**VERDICT: NEEDS-DECISION** — one of five released main cells passed, one halted without a verdict, three UNRUN, four controls UNRUN. Stopping here for Codex then Chat evidence review; no leg re-run, no attempt budget extended, no implementation unit started.
api-equivalent cost (not billed on Max): $19.57 | turns: 82 | running total across all briefs: $1,264.03
