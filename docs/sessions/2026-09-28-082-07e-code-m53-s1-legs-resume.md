# 082-07e — m53 S1 part 3c: the legs resumed and stopped at G-ID MASKED (the alpha wrong copy reads 0)

Date: 2026-09-28 (IST; the window ran 2026-09-27 19:18–19:23 UTC) · Author: Claude Code (Opus 5.5) · Branch
`feat/m53-uv-normal-corruption` at `202ae72` when the window ran · Boundary `082-07-evidence/boundary.json` `4b73aa2e…` ·
Staged exe `2FCDF059`, utoc `30FE0FDE`, ucas `11231356`, pak `FD766B7B` (unchanged; nothing was built or staged).

## 1. Goal

Resume the S1 gate legs on the 082-07c build with the 082-07d harness: `G0_FSYN` first, the U-N1 re-run, then N-N1 onward,
stopping at the first qualification failure (plan §R11, §R13.1). Record the owed in-engine proofs.

## 2. What ran

`C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\082-07-window.py 082-07e 540`, with neither `IAI_R53_ROOT` nor
`IAI_R53_STUB_LAUNCH` set. Console: `_reviews/082-07-evidence/082-07e-window-1.out.txt`.

- **Preflight** `run/preflight-3.json`: problems `[]`, quiet, feature head `202ae72`, stub false. The only foreign process was
  `UnrealTraceServer.exe`, which is not ours and was not touched.
- **Quiet wait** 0 s; budget tag `082-07e` spent 87.8 s of 2,700 s.
- **Legs launched, in order, 12:** `G0_FSYN_A2`, `ID_UN1_A2`, `NULL_UN1_A2`, `WC_UN1_NORMAL_A2`, `WC_UN1_CHANSWAP_A2`,
  `ID_NN1_A2`, `NULL_NN1_A2`, `WC_NN1_NORMAL_A1`, `WC_NN1_CHANSWAP_A1`, `ID_MASKED_A1`, `NULL_MASKED_A1`,
  `WC_MASKED_ALPHA_A1`. All 12 ACCEPTED on attempt 1 of their numbering. **No already-decided leg was re-launched** other than
  the three the amendment re-runs by design (`G0_FSYN`, the U-N1 set, `NULL_NN1`/`ID_NN1`).
- **Stop:** `G-ID MASKED FAIL (stop row 3): wrong copy alpha: 57 measured frame(s) read max |d| < 16`. Window exit **1**.
- **Postflight PASS** (exit 0): 55 accepted legs, 7 superseded attempts, 9,020 alias hardlinks verified, no exe-side copy left,
  staged build, harness, `m51` and feature head unchanged.

## 3. Gate results (live rows)

| gate | class | rows | result | key reading |
|---|---|---|---|---|
| FIXTURE-NAMES | PRE | 1 | PASS | all 77 listed |
| G0 F-SYN (re-run) | D | 1 | READING | 77 targets; compared normal 77, uv 28; **0 findings** (A1.12 predicted this) |
| G0 F-MW (082-07b) | D | 1 | READING | 10 targets, 6 findings: floor `not_fully_resident`, rocks `texture_not_parameter` (A1.8) |
| ADMIT | ADMIT | 6 | ADMITTED | U-N1, N-N1, MASKED rows all admitted |
| LUMA / BAND F-SYN / G-BIND link | Q / PRE / Q | 3 | PASS / VALID / PASS | 082-07b, stand (A1.10) |
| G-ID | Q | 14 | **13 PASS, 1 FAIL** | identity max \|d\| **0 (EXACT) on all 14 rows**, 8 measured events each |
| G3 (A1.5) | Q | 14 | PASS | 0 at the same index on every judged event, incl. U-N1, N-N1, MASKED |
| TRIP | Q | 54 (+1 read-only) | PASS | `texcorrupt_rt_mip_mismatch` 0 |
| G11 | Q | 54 (+1 read-only) | PASS | all three plugin assets `resolved=1` |
| G4 LEVEL-CHANGE / RECREATED-COMPONENT | Q | 2 | UNEXERCISED | declared in advance (§6) |

**Wrong-copy minimum per row** (min over measured frames of the per-frame max \|d\|; the rule needs ≥ 16 on every frame):
U-C1 chanswap 150 / srgbtwice 115 · U-C2 146 / 108 / alpha 194 · U-C3 150 / 101 / alpha 197 · U-C4 151 / 118 · U-C5 srgbtwice
90 · U-D1 chanswap 73 · U-D2 71 / alpha 205 · U-D3 72 / alpha 205 · U-D4 73 · U-D5 206 · U-D6 206 · **U-N1 normal 172 /
chanswap 72** · **N-N1 normal 180 / chanswap 65** · **MASKED alpha 0**.

`WC_MASKED_ALPHA`'s own TRIP and G11 were not evaluated by the window, because the halt came first (G319's shape). They were read
read-only afterwards, without writing `gates.json`: TRIP PASS (0), G11 PASS.

## 4. The failure: G-ID MASKED

- **Rule (predictions §4.2):** every assigned wrong copy must read d ≥ 16 on every measured frame of ≥ 3 events; below 16 the
  instrument is invalid (G96): FAIL, stop row 3.
- **Evaluator output:** identity PASS, EXACT, max 0, 8 events / 57 frames measured; wrong copy `alpha`: `BELOW-16`,
  `min_frame_max` **0** on all 8 events, `why` "57 measured frame(s) read max |d| < 16".
- **Evidence:** `bank\M53S1_ID_MASKED_A1`, `bank\M53S1_NULL_MASKED_A1`, `bank\M53S1_WC_MASKED_ALPHA_A1`
  (`session_20260928-005321`); readings `_reviews/082-07e-evidence/readings.json`.

### 4.1 Cause read (measured; the mechanism is NOT established)

1. **The fault was applied.** Every `APPLIED` line of the wrong-copy leg reads `fault=alpha noapply=0`, and every
   `TEXCORRUPT-DECIDE` line reads `levers=[… wrongcopy=alpha identity=1 …]` (8 fires).
2. **The wrong copy and the null are byte-identical**, whole frame, on the frames checked (si 20, 21, 60): max 0, 0 pixels
   differ. The target mask covers 4,096 px, the full 64 × 64 on-screen tile.
3. **The host tile itself never clips.** `M_TC_Masked` (fixture tool `make_texcorrupt_fixture.py`: Masked, clip 0.5, Opacity
   Mask = `Tex`.A) reads `T_TC_UC2`, whose alpha is 72 / 104 / 152 / 184 in four equal quarters, one per colour. So half the
   texels (orange, a = 72; blue, a = 104) are below the clip. **In the null leg**, where the feature is not installed
   (`slot_not_installed`), **all four colours are drawn**: the orange cells read ≈ (162, 98, 42) and the blue cells ≈
   (31, 31, 161), not the background (16, 31, 50). The host material's opacity mask has **no visible effect** in this build.
4. **The alpha fault changes only alpha** (`DbgOpaque` → the copy writes Opacity 1, stored A = 0, per the 082-06 journal). If
   the host's clip has no effect, no alpha value can show, so the fault is invisible by construction.
5. **This row never had an offline proof.** The 082-06 manifest (`texcorrupt_fixture_manifest.json`) proves `alpha` only for
   U-C2 / U-C3 / U-D2 / U-D3 (min 72, the `alpha_split` readouts). There is **no** `TC_Masked` entry, and the predictions table
   §4.2 leaves MASKED's offline minimum blank. The fixture tool's own `all_relied_on` guard cannot catch this, because the
   MASKED assignment lives in the leg list (`082-07-legs.py`), not in the manifest.

**Read:** this is a fixture / gate-encoding defect of the same class as 082-07b's normal readouts (G320), not a feature
defect. The feature's identity copy is exact against a host that is itself not clipping. Why `M_TC_Masked` does not clip is
**not established** (candidates, none tested: the opacity-mask connection in the authored asset, the cooked material, a
render setting). It needs an editor read of the asset, which this run had no licence for.

⛔ Nothing was fixed, re-run or edited. The legs from G-ID-M onward (139 of 194 remain) are UNRUN.

## 5. The owed in-engine proofs (predictions §4.9)

| proof | verdict | reading |
|---|---|---|
| P2-1 NoApply predicate | **CONFIRMED** (read over the 53 accepted capture legs so far) | 3,445 readings, 0 problems: `installed` / `slot_not_installed` as the lever says, and `condition_held` true exactly on `installed` |
| P2-2 collateral set | **NOT REACHED** | its G-COLL F-SYN sample is after the stop |
| P2-3 subtype | **CONFIRMED** (same legs) | 424 events, all `anomaly_subtype` = `identity`, labels' `texcorrupt.mode` agrees, truncated and last events included |
| P2-5 rollback via `FailStep`, F+2 0/0 | **NOT REACHED** | G4 FS rows are after the stop |
| P2-6 NoApply 2 watches, incl. `EndPlay` | **NOT REACHED** | G4 DES2 is after the stop |

P2-1 and P2-3 are final-pass rows in the harness. They were computed read-only with the harness's own evaluators (`ev_p21`,
`ev_p23`) and are **not** written to `gates.json`; they are partial (55 of 194 legs).

## 6. Leg ledger (every attempt; `bank\` = `D:\IntrusiveAnomalies\_bench_sessions_bank\`)

| window | attempt | status | evidence (bank session) | started (UTC) |
|---|---|---|---|---|
| 082-07b | `M53S1_G0_FSYN_A1` | ACCEPTED → SUPERSEDED (082-07d) | — (census leg) | 2026-09-27T04:01:32Z |
| 082-07b | `M53S1_CTL_A_A1` | ACCEPTED | `bank\M53S1_CTL_A_A1\session_20260927-093243` | 2026-09-27T04:02:40Z |
| 082-07b | `M53S1_CTL_B_A1` | ACCEPTED | `bank\M53S1_CTL_B_A1\session_20260927-093256` | 2026-09-27T04:02:54Z |
| 082-07b | `M53S1_G0_FMW_A1` | ACCEPTED | — (census leg) | 2026-09-27T04:03:17Z |
| 082-07b | `M53S1_ID_UC1_A1` | ACCEPTED | `bank\M53S1_ID_UC1_A1\session_20260927-093329` | 2026-09-27T04:03:26Z |
| 082-07b | `M53S1_WC_UC1_CHANSWAP_A1` | ACCEPTED | `bank\M53S1_WC_UC1_CHANSWAP_A1\session_20260927-093343` | 2026-09-27T04:03:41Z |
| 082-07b | `M53S1_WC_UC1_SRGBTWICE_A1` | ACCEPTED | `bank\M53S1_WC_UC1_SRGBTWICE_A1\session_20260927-093357` | 2026-09-27T04:03:55Z |
| 082-07b | `M53S1_ID_UC2_A1` | ACCEPTED | `bank\M53S1_ID_UC2_A1\session_20260927-093421` | 2026-09-27T04:04:19Z |
| 082-07b | `M53S1_NULL_UC2_A1` | ACCEPTED | `bank\M53S1_NULL_UC2_A1\session_20260927-093435` | 2026-09-27T04:04:32Z |
| 082-07b | `M53S1_WC_UC2_CHANSWAP_A1` | ACCEPTED | `bank\M53S1_WC_UC2_CHANSWAP_A1\session_20260927-093449` | 2026-09-27T04:04:47Z |
| 082-07b | `M53S1_WC_UC2_SRGBTWICE_A1` | ACCEPTED | `bank\M53S1_WC_UC2_SRGBTWICE_A1\session_20260927-093503` | 2026-09-27T04:05:01Z |
| 082-07b | `M53S1_WC_UC2_ALPHA_A1` | ACCEPTED | `bank\M53S1_WC_UC2_ALPHA_A1\session_20260927-093517` | 2026-09-27T04:05:14Z |
| 082-07b | `M53S1_ID_UC3_A1` | ACCEPTED | `bank\M53S1_ID_UC3_A1\session_20260927-093543` | 2026-09-27T04:05:41Z |
| 082-07b | `M53S1_NULL_UC3_A1` | ACCEPTED | `bank\M53S1_NULL_UC3_A1\session_20260927-093557` | 2026-09-27T04:05:55Z |
| 082-07b | `M53S1_WC_UC3_CHANSWAP_A1` | ACCEPTED | `bank\M53S1_WC_UC3_CHANSWAP_A1\session_20260927-093612` | 2026-09-27T04:06:09Z |
| 082-07b | `M53S1_WC_UC3_SRGBTWICE_A1` | ACCEPTED | `bank\M53S1_WC_UC3_SRGBTWICE_A1\session_20260927-093625` | 2026-09-27T04:06:23Z |
| 082-07b | `M53S1_WC_UC3_ALPHA_A1` | ACCEPTED | `bank\M53S1_WC_UC3_ALPHA_A1\session_20260927-093639` | 2026-09-27T04:06:36Z |
| 082-07b | `M53S1_ID_UC4_A1` | ACCEPTED | `bank\M53S1_ID_UC4_A1\session_20260927-093706` | 2026-09-27T04:07:03Z |
| 082-07b | `M53S1_NULL_UC4_A1` | ACCEPTED | `bank\M53S1_NULL_UC4_A1\session_20260927-093719` | 2026-09-27T04:07:17Z |
| 082-07b | `M53S1_WC_UC4_CHANSWAP_A1` | ACCEPTED | `bank\M53S1_WC_UC4_CHANSWAP_A1\session_20260927-093734` | 2026-09-27T04:07:31Z |
| 082-07b | `M53S1_WC_UC4_SRGBTWICE_A1` | ACCEPTED | `bank\M53S1_WC_UC4_SRGBTWICE_A1\session_20260927-093747` | 2026-09-27T04:07:45Z |
| 082-07b | `M53S1_ID_UC5_A1` | ACCEPTED | `bank\M53S1_ID_UC5_A1\session_20260927-093811` | 2026-09-27T04:08:09Z |
| 082-07b | `M53S1_NULL_UC5_A1` | ACCEPTED | `bank\M53S1_NULL_UC5_A1\session_20260927-093825` | 2026-09-27T04:08:22Z |
| 082-07b | `M53S1_WC_UC5_SRGBTWICE_A1` | ACCEPTED | `bank\M53S1_WC_UC5_SRGBTWICE_A1\session_20260927-093839` | 2026-09-27T04:08:37Z |
| 082-07b | `M53S1_ID_UD1_A1` | ACCEPTED | `bank\M53S1_ID_UD1_A1\session_20260927-093900` | 2026-09-27T04:08:58Z |
| 082-07b | `M53S1_NULL_UD1_A1` | ACCEPTED | `bank\M53S1_NULL_UD1_A1\session_20260927-093914` | 2026-09-27T04:09:11Z |
| 082-07b | `M53S1_WC_UD1_CHANSWAP_A1` | ACCEPTED | `bank\M53S1_WC_UD1_CHANSWAP_A1\session_20260927-093928` | 2026-09-27T04:09:26Z |
| 082-07b | `M53S1_ID_UD2_A1` | ACCEPTED | `bank\M53S1_ID_UD2_A1\session_20260927-093949` | 2026-09-27T04:09:47Z |
| 082-07b | `M53S1_NULL_UD2_A1` | ACCEPTED | `bank\M53S1_NULL_UD2_A1\session_20260927-094003` | 2026-09-27T04:10:00Z |
| 082-07b | `M53S1_WC_UD2_CHANSWAP_A1` | ACCEPTED | `bank\M53S1_WC_UD2_CHANSWAP_A1\session_20260927-094017` | 2026-09-27T04:10:15Z |
| 082-07b | `M53S1_WC_UD2_ALPHA_A1` | ACCEPTED | `bank\M53S1_WC_UD2_ALPHA_A1\session_20260927-094031` | 2026-09-27T04:10:29Z |
| 082-07b | `M53S1_ID_UD3_A1` | ACCEPTED | `bank\M53S1_ID_UD3_A1\session_20260927-094054` | 2026-09-27T04:10:52Z |
| 082-07b | `M53S1_NULL_UD3_A1` | ACCEPTED | `bank\M53S1_NULL_UD3_A1\session_20260927-094108` | 2026-09-27T04:11:05Z |
| 082-07b | `M53S1_WC_UD3_CHANSWAP_A1` | ACCEPTED | `bank\M53S1_WC_UD3_CHANSWAP_A1\session_20260927-094122` | 2026-09-27T04:11:20Z |
| 082-07b | `M53S1_WC_UD3_ALPHA_A1` | ACCEPTED | `bank\M53S1_WC_UD3_ALPHA_A1\session_20260927-094136` | 2026-09-27T04:11:33Z |
| 082-07b | `M53S1_ID_UD4_A1` | ACCEPTED | `bank\M53S1_ID_UD4_A1\session_20260927-094159` | 2026-09-27T04:11:57Z |
| 082-07b | `M53S1_NULL_UD4_A1` | ACCEPTED | `bank\M53S1_NULL_UD4_A1\session_20260927-094213` | 2026-09-27T04:12:10Z |
| 082-07b | `M53S1_WC_UD4_CHANSWAP_A1` | ACCEPTED | `bank\M53S1_WC_UD4_CHANSWAP_A1\session_20260927-094227` | 2026-09-27T04:12:25Z |
| 082-07b | `M53S1_ID_UD5_A1` | ACCEPTED | `bank\M53S1_ID_UD5_A1\session_20260927-094247` | 2026-09-27T04:12:45Z |
| 082-07b | `M53S1_NULL_UD5_A1` | ACCEPTED | `bank\M53S1_NULL_UD5_A1\session_20260927-094301` | 2026-09-27T04:12:58Z |
| 082-07b | `M53S1_WC_UD5_CHANSWAP_A1` | ACCEPTED | `bank\M53S1_WC_UD5_CHANSWAP_A1\session_20260927-094315` | 2026-09-27T04:13:13Z |
| 082-07b | `M53S1_ID_UD6_A1` | ACCEPTED | `bank\M53S1_ID_UD6_A1\session_20260927-094336` | 2026-09-27T04:13:33Z |
| 082-07b | `M53S1_NULL_UD6_A1` | ACCEPTED | `bank\M53S1_NULL_UD6_A1\session_20260927-094442` | 2026-09-27T04:14:40Z |
| 082-07b | `M53S1_WC_UD6_CHANSWAP_A1` | ACCEPTED | `bank\M53S1_WC_UD6_CHANSWAP_A1\session_20260927-094456` | 2026-09-27T04:14:54Z |
| 082-07b | `M53S1_ID_UN1_A1` | ACCEPTED → SUPERSEDED (082-07d) | `bank\M53S1_ID_UN1_A1\session_20260927-094517` | 2026-09-27T04:15:14Z |
| 082-07b | `M53S1_NULL_UN1_A1` | ACCEPTED → SUPERSEDED (082-07d) | `bank\M53S1_NULL_UN1_A1\session_20260927-094530` | 2026-09-27T04:15:28Z |
| 082-07b | `M53S1_WC_UN1_NORMAL_A1` | ACCEPTED → SUPERSEDED (082-07d) | `bank\M53S1_WC_UN1_NORMAL_A1\session_20260927-094545` | 2026-09-27T04:15:42Z |
| 082-07b | `M53S1_WC_UN1_CHANSWAP_A1` | ACCEPTED → SUPERSEDED (082-07d) | `bank\M53S1_WC_UN1_CHANSWAP_A1\session_20260927-094558` | 2026-09-27T04:15:56Z |
| 082-07b | `M53S1_ID_NN1_A1` | ACCEPTED → SUPERSEDED (082-07d) | `bank\M53S1_ID_NN1_A1\session_20260927-094621` | 2026-09-27T04:16:19Z |
| 082-07b | `M53S1_NULL_NN1_A1` | ACCEPTED → SUPERSEDED (082-07d) | `bank\M53S1_NULL_NN1_A1\session_20260927-094634` | 2026-09-27T04:16:32Z |
| — | `*RESUME*` | RULING | 082-07d supersede record `run/supersede.json` | — |
| 082-07e | `M53S1_G0_FSYN_A2` | ACCEPTED | — (census leg) | 2026-09-27T19:19:31Z |
| 082-07e | `M53S1_ID_UN1_A2` | ACCEPTED | `bank\M53S1_ID_UN1_A2\session_20260928-005042` | 2026-09-27T19:20:39Z |
| 082-07e | `M53S1_NULL_UN1_A2` | ACCEPTED | `bank\M53S1_NULL_UN1_A2\session_20260928-005055` | 2026-09-27T19:20:53Z |
| 082-07e | `M53S1_WC_UN1_NORMAL_A2` | ACCEPTED | `bank\M53S1_WC_UN1_NORMAL_A2\session_20260928-005110` | 2026-09-27T19:21:08Z |
| 082-07e | `M53S1_WC_UN1_CHANSWAP_A2` | ACCEPTED | `bank\M53S1_WC_UN1_CHANSWAP_A2\session_20260928-005123` | 2026-09-27T19:21:21Z |
| 082-07e | `M53S1_ID_NN1_A2` | ACCEPTED | `bank\M53S1_ID_NN1_A2\session_20260928-005147` | 2026-09-27T19:21:45Z |
| 082-07e | `M53S1_NULL_NN1_A2` | ACCEPTED | `bank\M53S1_NULL_NN1_A2\session_20260928-005200` | 2026-09-27T19:21:58Z |
| 082-07e | `M53S1_WC_NN1_NORMAL_A1` | ACCEPTED | `bank\M53S1_WC_NN1_NORMAL_A1\session_20260928-005216` | 2026-09-27T19:22:13Z |
| 082-07e | `M53S1_WC_NN1_CHANSWAP_A1` | ACCEPTED | `bank\M53S1_WC_NN1_CHANSWAP_A1\session_20260928-005229` | 2026-09-27T19:22:27Z |
| 082-07e | `M53S1_ID_MASKED_A1` | ACCEPTED | `bank\M53S1_ID_MASKED_A1\session_20260928-005252` | 2026-09-27T19:22:50Z |
| 082-07e | `M53S1_NULL_MASKED_A1` | ACCEPTED | `bank\M53S1_NULL_MASKED_A1\session_20260928-005306` | 2026-09-27T19:23:03Z |
| 082-07e | `M53S1_WC_MASKED_ALPHA_A1` | ACCEPTED | `bank\M53S1_WC_MASKED_ALPHA_A1\session_20260928-005321` | 2026-09-27T19:23:18Z |

Accepted and live: 55. Superseded by 082-07d (kept, aliases renamed `…__SUPERSEDED_082-07d`): 7. Not run: 139.

## 7. Deviations

- P2-1 / P2-3 / the halted leg's TRIP and G11 were read with the harness's own evaluators, read-only, outside the window. Nothing
  was written into the locked run dir.
- The pixel checks in §4.1 are ad-hoc reads of banked PNGs, not a gate.
- Committing this journal moves the feature head, so the 082-07d boundary no longer admits a window. A re-issue is needed
  anyway before any further leg, because S1 is stopped.

## 8. Hand-off

- **For chat:** rule the MASKED row. The options visible from here: fix `M_TC_Masked` (and prove the clip in a null leg and
  offline), re-assign or drop the MASKED alpha wrong copy, or make MASKED's alpha half read-only. Whatever the ruling, the
  predictions need an amendment and the boundary a re-issue before 082-07f.
- **Owed in S2** (from the 082-07d ruling): the synthetic `no_normal_map` stop row and the start-up element's cause read.
