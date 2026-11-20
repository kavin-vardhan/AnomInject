# 085-04 — B-M53 harness: the m53 DC label-sync release gate, the mode oracle and every DC night gate (written by 085-03; the bench runs as 085-04)

Governing: `docs/predictions/2026-09-29-m53-dc.md` (§5 B-M53, §6 the oracle, §3.7 EXCL, §7–§11) on `feat/m53-uv-normal-corruption`;
`_reviews\085-01-chat-ruling-dc-plan.md` (ND-4 with a yield reading, ND-5); `085-02*-chat-ruling-*.md`; 084-10's G377.
Built on **084-09** (runner, banking, person / quiet gates, G355 restore, E1 with amendment R1, `--label-rule`, `labelled_membership`,
`presence_contract_v2`), with the m53 rows ported beside it (`085-04-m53rows.py`, copied from `082-07-lib.py` / `082-14-s20-lib.py`,
never imported: G327).

## The pairing under test (085-03)
**DC = exe `6B671987` (085-02c's build of `270ed32`, byte-identical) + utoc `F76F1119` / ucas `B54C2235` / pak `BBC9A58C` / globals
`462B8AC6` / `BB05CF99`**, archived `_binary_baselines\m53dc-cook-F76F1119\` (`archive-hashes.json`, 6/6 re-hashed). Maps CB_GateLevel,
MainMenu, MainWorld, Entry, CB_TexCorruptLevel. The fixture level carries the 085-03 DC cells (below). **Restore target: m53 S1**
(`m53-s1-maskedfix-cook-20DA6F98`, exe `2FCDF059` + utoc `20DA6F98`), read from that immutable archive, never from what 084-09 left.

## Files
`085-04-common.py` (paths, the three pinned pairings M53 / FIX / DC, pinned tools), `085-04-legs.py` (the leg table), `085-04-lib.py`
(runner, banking incl. two-capture legs, staging with receipts, G355 restore, the premise-lock substitution, G377 checks),
`085-04-eval.py` (the gate logic), `085-04-m53rows.py` (the ported m53 rows), `085-04-oracle.py` (the mode oracle),
`085-04-window.py` (the driver), `085-04-run.ps1` (084-09's launcher + a census wait pattern), `085-04-gateselftest.py`.
Tools are pinned copies from `c6aa9c9` in `085-04-evidence\tools\` (blobs in `common.TOOLS_PINNED`; `verify_capture.py` is 084-09's
blob `a12ad792aa`).

## Invocation (night window, AFTER 084-09, PC idle)
```
C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\085-04-window.py night --commit   # premise -> lock (committed) -> gates -> restore
C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\085-04-window.py premise           # the premise window alone (DC stays staged)
C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\085-04-window.py lock --commit     # commit the lock doc on the feat branch
C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\085-04-window.py live              # gates (refuses without a committed lock)
C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\085-04-window.py gates             # re-evaluate accepted legs
C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\085-04-window.py restore           # manual G355 restore of m53 S1
C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\085-04-window.py dry               # stubbed night on E:, banked stand-ins
C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\085-04-gateselftest.py
```
`--only GROUP[,GROUP]` restricts. A second run resumes (accepted legs skipped). Evidence `_reviews\085-04-evidence\run\`; bank
`_bench_sessions_bank\M53DC_<leg>_A<n>` + hardlink alias `M53DC_<leg>`. Dry runs use an E: root.

## Sequence and safety
1. **Preflight:** the DC and M53 archives hold exactly their pinned six hashes; the DC container lists the five maps (read from the
   artifact); `feat/m53-uv-normal-corruption` == origin (or origin + the lock commit only) and contains `270ed32`; the main checkout
   is `m51` `53bf725`; pinned tool blobs; **`m53_glue_check.py` PASS on the feat worktree**; the oracle, m53-rows and gate selftests
   all-pass; the staged set is one of M53 / FIX / DC (anything else refuses: the restore target must be known); E: >= 50 GB, D: >= 25 GB;
   no engine/game/UBT of ours.
2. **Stage DC** (only differing files copied, hash receipt `receipt-01-dc.json`, the before-set recorded — it never assumes 084-09 restored).
3. **Premise window** (group PREMISE, ~12 min, no verdicts): builds `premise-lock.json` and `premise-amendment-draft.md`.
4. **Lock:** the amendment is written to `docs/predictions/2026-09-29-m53-dc-lock.md` and, with `--commit`, committed (path-scoped)
   on the feat branch **before any gate leg**; `live` refuses unless the lock's sha appears in that committed file at HEAD.
5. **Gate legs** in the order LEVER, CENSUS, BM53F, GMODE, BM53ARM, REASON, EXCL, BIND, GID, SEED, AON, BM53MW, COST, SYNCREG.
6. **G355 restore of m53 S1** in a `finally` (every copy retried with backoff; `receipt-99-restore-m53.json`; exit 12 if it fails).
7. Evaluate (`report.json`, `report.md`), postflight (m53 S1 staged, aliases are hardlinks, m51 untouched, no leg copies left).

Person gate (60 s idle) and quiet check before each attempt. **Nothing is killed except the runner's own launched PID.** Every leg
passes `-IAIBench` except the no-flag legs (`LEVER_OFF`, `P_HOST_CEN`, `CENSUS_FSYN`); **G377: every -IAIBench leg must log
`IAI bench levers: ENABLED`, every no-flag leg `IAI bench levers: DISABLED (<n> masked)`, else the attempt is INVALID.**

## The premise window (DC plan §11; dispositions go into the committed lock)
| leg | reads | disposition |
|---|---|---|
| `P_HOST_CEN` (MainWorld, **no flag, cold process**) | `IAI.TexCorrupt.Census` + `… all` at the delivered 128 MiB | **ND-4 yield at 128 MiB, always reported**; the G-CENSUS cold read (`census_readonly_check.py --log --cold` must PASS; UNDECIDABLE = relaunch) |
| `P_HOST_BENCH` (MainWorld, -IAIBench) | the product census and `IAI.Bench.TexCorruptCensus` in ONE process (same view) | **per family** (uv / normal) H1 / H2 = the actors with `final=APPLY` at 128 MiB, an actor eligible for **both** families first, then the rock, then name order (G394: the S1 bank predicts no both-eligible actor and the rock refuses `texture_not_parameter`). **ND-4: a family with none at 128 MiB but hosts refused only `over_budget` gets a bench-only cap raise DECLARED in the lock before any leg** = the next power of two >= those hosts' `required_bytes` (<= 1 GiB, the knob's range), applied as `IAI.Anomaly.TexCorruptMaxRtBytes <n>` on the MainWorld and COST legs only; the delivered default stays 134,217,728. A family with no host at any cap -> its MainWorld legs NOT-RUN-PREMISE (chat). The count join (product counts == aggregated bench lines) |
| `P_ORC_UV`, `P_ORC_N` (fixture, identity null, AA-off arbiter) | the oracle tiles on a null | P-ORC: cells >= 32 apart, interiors <= 2, fit residual <= 0.5 px, >= 1:1; a FAIL leaves the G-MODE rows unrunnable (reported, never PASS) |
| `P_M52F` (fixture, two captures: m52 on `TC_M52Hold` capped at 40 frames, then a harmless capture DEFERred 600 ticks) | `RESTORE CARRIED`, the trail close after run 1, the DEFER probes | P-M52F (else EXCL uses the rock the next night); P-EXCL: `excl_a_defer` = run 1's last GFrameCounter + 16 − S, and pre = 0 if the trail closes within 24 frames of run 1's end |
| `P_FLOOR` (MainWorld floor, identity) | the first decision k / 30 | G-AON-MW exercised only if 0 < k < 30 (else `TC_Mixed` carries the Q) |
| `P_SYN` (fixture, synth order, identity, DEFER probes) | the synth pose | P-SYN (synth fixture legs INVALID-POSE if it differs) |
| (`P_ORC_UV` also carries DEFER probes) | the first labelled GFrameCounter and the first event's end, native order, EXCL-B's schedule | EXCL-B's two FireOnce DEFER counts (inside the first positives; PostGap after R + 2) |

**DEFER calibration.** `DEFER n <cmd>` is the engine's own one-tick deferral, nested n times. Five probe cvars queried at 60 / 120 / 240
/ 360 / 480 DEFERs print at GFrameCounter S + n; the log prefix gives GFrameCounter % 1000, so S = frame − n (consistent to 2 frames
or it is unreadable).

## ND-5 — the "no visible change" definition (declared here, before any leg runs)
**An m53 event is NOT-MEASURABLE (no visible change) when the E1 evaluator (`084-09-eval.diff_event`, amendment R1) finds no frame of
its labelled set whose ON-vs-reference difference inside the event's target mask reaches the detection threshold** (its `measurable`
flag is false), for an event whose status is OK (camera still, not a warm-up, reference not confounded). The rate per mode = NOT-
MEASURABLE events / OK events over the MainWorld ON legs (H1 and H2, AA on and off, reported apart), at t50 and strict. **On the
fixture a NOT-MEASURABLE event is a FAIL**, except a `scramble` event whose recomputed pair is predicted invisible on `TC_UC1`
(no source cell of a different colour; `eval.scramble_invisible`), excluded by name.

## Legs and predictions (declared before any run; DC build; `-IAIBench` unless noted; exposure pinned, marker off, delivery off, SVE, pace 30; `2 4 8 14 0`)
| group | legs | frames | prediction |
|---|---|---|---|
| LEVER | `LEVER_OFF` (no flag), `LEVER_ON` | 60 | OFF: `DISABLED (46 masked)`, `SynthTickOrder` / `TexCorruptIdentity` / `DestroyTarget` refused, no twins in `IAI.ListAnomalies`; ON: `ENABLED`, twins listed |
| CENSUS | `CENSUS_FSYN` (no flag, cold), `CENSUS_FSYN_BENCH` | census | the four count lines parse, `stats_unchanged=1`, cold read PASS, count join exact |
| BM53F | `F_{TILE,SCRAMBLE,INVERT,GREENFLIP}_{NAT,NAT_AAOFF,SYN}` (12, `TC_UC1` / `TC_NN1`) + `F_NULL_{UV,N}_{AAON,AAOFF}` (4) | 400 | **PASS on every mode and condition:** E1 t50 onset 0 and offset 0, 0 off frames outside transition frames, >= 16 judged onsets and offsets per mode per condition (≈ 17 events per leg, the first a warm-up), 0 NOT-MEASURABLE (bar predicted-invisible scramble pairs), **0 m53 transition flags** (material swap, 086-01), masks: `mask_missing` 0, `mask_extra` 0, owners == committed components, `--label-rule` exit 0, membership / presence PASS, EXCL-DATA 0, mode_source=argument; nulls: `mode_source=bench_lever lever=identity`; AA-off legs: ON vs null reads >= 16 inside the mask on every labelled frame (G1-NAT arbiter). An E2 disagreement is a finding, never averaged |
| GMODE | `MODE_TILE` (`TC_OracleUV16`), `MODE_SCRAMBLE` (`TC_OracleUV64`), `MODE_UVNULL`, `MODE_INVERT` / `MODE_GREENFLIP` (`TC_OracleN`), `MODE_NNULL` | 200, AA-off arbiter | G-MODE-UV / -N PASS: every kept pixel / every cell of every labelled frame of >= 4 counted events matches; the logged scramble pair equals the recomputation; every wrong-mode negative < 50 %; the nulls as each mode < 50 % |
| BM53ARM | `ARM_CD3_{AAOFF,AAON}`, `ARM_RD3_{AAOFF,AAON}`, `ARM_NOAPPLY1` | 200 | **must FAIL:** CommitDelay 3 -> start +3, end 0; RestoreDelay 3 -> end +3, start 0 (one RestoreDelay run per process, F5); NoApply-1 -> FAIL (labelled, never visible) |
| REASON | `REASON_E6` (`bogus` + DEFERred `IAI.Apply` `drift` / `invert`), `REASON_MIXED_N`, `REASON_MIXED_UV` | 120 | `mode_invalid:unknown:bogus`, `mode_invalid:not_in_delivery:drift`, `mode_invalid:family:invert`, no APPLIED; `partial_footprint:1/2:no_normal_map`; uv on `TC_Mixed` APPLIES |
| EXCL | `EXCL_A` (two captures), `EXCL_B` | 120 | A: `RESTORE CARRIED` in run 1, `Auto.Exclude … state=trail_open` in run 2, no m53 APPLIED before the trail's close line, m53 applies after, 0 co-entry frames; B: the first FireOnce `result=all_excluded` with `seed_after == seed_before`, the second applies m52, 0 co-entry frames |
| BIND | `BIND_TILE`, `_TILENULL`, `_ID`, `_IDNULL` (`TC_Layer`, S1 recipe `2 16 8 4 0`) | 100 | G-BIND per-frame minimum PASS, far strip <= 2 |
| GID | `GID_{UC1,NN1}_{ID,NULL,WRONG}` | 100, AA-off arbiter | identity vs null <= 2 on every labelled frame; wrong copy >= 16 |
| SEED | `SEED_A1`, `SEED_A2` (repeat), `SEED_EMPTY` (mode sets `none`) | 300, auto-pool, seed 4242 | A1 == A2 draw for draw; every `Auto.Draw` replays from the seed; every shipped mode drawn at least once; empty sets: every m53 attempt draws once and reads `!none` |
| AON | `AON_MW` (floor, tile) | 120 | if P-FLOOR read 0 < k < 30: first decision `partial_footprint:k/30:…`; else NOT-EXERCISED (reported) |
| BM53MW | H1: 4 modes AA-on + 2 nulls, `tile` / `invert` AA-off + 2 nulls; H2: 4 modes + 2 nulls (16) | 300 | PASS on every judged edge; >= 8 judged per mode on H1 AA-on or the shortfall goes to chat (never PASS); NOT-MEASURABLE counted as the ND-5 rate (no number predicted; `green_flip` expected weakest); a blob on ON absent from the refresh-matched null at the same si FAILS |
| COST | `COST_DISCARD`, then `COST_A1`, `B1`, `B2`, `A2` (A = identity null, B = tile; pacing off) | 300 | a reading: per fire `apply_ms` / `revert_ms`, per frame GPU / GT from `TEXCORRUPT-COST`; "below the resolution of this instrument" wherever the A spread exceeds B − A; no threshold |
| SYNCREG | `SR_B0`, `SR_B5` (its matched null), `SR_B3`, `SR_B9`, `SR_REAL_{BL,MO,MT,CT}_NAT` on the merged build | 1200 / 300 | **084-09's predictions unchanged** (B0 / B9 / B3 E1 t50 PASS with record-backed `partial` / `temporal_aa` excuses, B5 null_check_v2 PASS, REAL_* PASS at t50 with onset / offset 0) |

## Proofs before any leg (offline, each both ways)
| suite | result | what it proves (kind: a = banked real data, b = synthetic, c = the night's own frames) |
|---|---|---|
| `085-04-gateselftest.py` | **54 / 54** (`gateselftest.json`) | G-SEED replay (a KAT-generated `Auto.Draw` sequence replays; one extra draw, a wrong seed, a repeat one attempt short all FAIL; coverage reports missing modes) · EXCL-DATA (a planted m53 entry on an m52 TAA-tail frame FAILs; a miscounted in-run counter FAILs) · EXCL-A (an m53 APPLIED before the trail close FAILs; no `trail_open` exclusion — the "live until BeginRevert" shape — FAILs) · EXCL-B (an excluded attempt that consumed a draw FAILs) · G-REASON-DC (an earlier-step refusal and `over_budget` in place of `partial_footprint` FAIL) · G-LEVER (an accepted spot lever, twins without the flag, no ENABLED echo FAIL) · G377 both directions · G-CENSUS (a name token FAILs the grammar; a doctored count FAILs the join; `census_readonly_check.py --selftest` 37 cases incl. the cold-process log cases) · mode_source (round_robin under the identity lever FAILs — 085-02c N1; no line FAILs; a wrong mode argument FAILs) · F5 · the arm classifier (a lever that did not bite is NOT proven) · ND-4's host / cap rule on four shapes incl. the S1-bank shape · **can-fail (a): 086-01's `solid_swap` DELAY known answer re-read by this copy's E2 = start +3 on every non-warm-up event (6 of 6)** (E1 reads those sessions CONFOUNDED-REFERENCE under R1, G392) · the label-doctoring machinery on a real S1 m53 capture · staging from a FIX-staged start · G355 with a real 8 s file lock |
| `085-04-oracle.py selftest` | **39 / 39** | (b) the affine map in 4 orientations incl. a bilinear sub-pixel case; P-ORC FAILs on two identical cells, on 20-apart cells, at 0.5 scale; tile / scramble / invert / green_flip each PASS as itself and every listed wrong-mode negative reads < 50 %; tile ×4, the ordinal + 1 pair, the forward map, invert rendered as green_flip FAIL; a logged pair that differs from the recomputation FAILs; < 4 events or a missing APPLIED is INSUFFICIENT; the null as each mode < 50 %; the 7 scramble KAT vectors |
| `085-04-m53rows.py selftest` | **98 / 98** | (a) + (b): G-BIND per-frame 57 × 1,192 px on the bank, a doctored copy FAILs (49 frames named) where S1's max rule passed it; far strip 784 px (max 1), a +20 tint FAILs; TRIP 191 PASS + 2 census N/A, key removed FAILs; presence names the 21 post-destroy rows, a stripped block FAILs; G-ID wrong copies 146 / 108 / 194 (S1's values); masks: removed mask / painted value / extra holder / stripped TAG-OWNERS each caught; g3_disp; transition flags; every regex on a banked line and on a line built from the DC format string |
| fixture offline proofs (`texcorrupt_fixture_images.py`) | 44, all relied on | cell separation, the closed normal set, every negative pair >= 25 %, the scramble KAT |
| night (kind c) | in `report.json` | the arms (c)/(d) must FAIL; the doctored-label variants (onset ± 1, offset ± 1) on each mode's AA-off fixture leg must FAIL while the exact labels PASS; P-ORC on the premise nulls |

## Dry run (085-03, stubbed launches on E:, banked stand-ins; `085-04-evidence\dry-final\`)
- **`085-04-window.py dry`, root `E:\IA_BuildCache\_r854_dry_0853_final`, `IAI_R854_INJECT_LOCK_S=8`: exit 0 (COMPLETE), preflight
  0 problems** (both archives' pins, the five cooked maps, feat == origin and contains `270ed32`, m51 untouched, the pinned tool blobs,
  **`m53_glue_check.py` VERDICT PASS**, the three selftests all-pass), **86 of 86 stubbed launches ACCEPTED**, DC staged from a
  FIX-staged stub start with a verified receipt, **the G355 restore copied the locked exe on attempt 3** and verified all six,
  `label_rule_all_pass` true, 398 s. No leg or row evaluator error.
- **Readings on the stand-ins (S1 / 084-06 banks and synthetic census logs; NOT verdicts about the DC build):** G-BIND PASS (the S1
  BIND known answer), G-SYNC-REG B0 PASS / B5 NULL-PASS / REAL_* PASS / **B3 and B9 FAIL = 084-06's known answers on the old build**,
  G-CENSUS cold reads PASS on the synthetic logs, EXCL-A / EXCL-B FAIL (S1 logs carry no DC exclusion lines), G-MODE NO-MAP (no oracle
  tiles in S1 frames — reported, never PASS), every B-M53 cell INSUFFICIENT (identity stand-ins change nothing), the doctored-label
  and arm can-fails NOT proven (nothing visible to move) — each the expected stand-in answer.
- A focused `dry --only PREMISE` on the final bytes (after the lock draft moved under the run root): exit 0, 7 / 7.
- An earlier full dry (`E:\IA_BuildCache\_r854_dry_0853`, 821 s) found three harness defects, all fixed before the final one: G-MODE's
  null path and aggregator, the m53 rows on auto-pool legs, and the oracle adapter (now the oracle's own `judge_session`).

## Expected bench time
Premise 7 legs ≈ 8 min of launches (+ reading and the lock commit ≈ 4 min ⇒ **≈ 12 min**). Gates (084-09's cadence: 200–300 frames
≈ 1.35 min with launch, banking and the person gate; 400 ≈ 1.45; 1200 ≈ 1.8; census ≈ 0.8): BM53F 16 × 1.45 ≈ 23 · BM53MW 16 × 1.35 ≈
22 · SYNCREG 4 × 1.8 + 4 × 1.35 ≈ 13 · GMODE 6 × 1.25 ≈ 7.5 · GID 6 × 1.05 ≈ 6 · COST 5 × 1.35 ≈ 7 · ARM 5 × 1.25 ≈ 6 · BIND 4 ×
1.05 ≈ 4 · SEED 3 × 1.35 ≈ 4 · EXCL 2 × 1.4 ≈ 3 · REASON 3 × 1.1 ≈ 3 · LEVER / CENSUS / AON ≈ 5 ⇒ **≈ 103 min of gate legs**; staging +
restore ≈ 4 min; evaluation ≈ 25 min (084-09's dry evaluated 47 legs in ≈ 24 min on fresh caches; this dry's 7–11 min on 100-frame
stand-ins is a floor); postflight 1 min ⇒ **≈ 145 min, ≤ 170 with person waits.** Runs after 084-09 (87–100 min) in the same night.

## Harness bytes as written by 085-03 (SHA-256 prefix; the README itself excluded)
| file | sha256[:16] | bytes |
|---|---|---|
| `085-04-common.py` | `3E7ABEB8B0B7A2F7` | 8858 |
| `085-04-eval.py` | `3C5B7DED8E455A22` | 31203 |
| `085-04-gateselftest.py` | `8CD8DDB76132B817` | 19580 |
| `085-04-legs.py` | `B87C81A9A8F076DB` | 13162 |
| `085-04-lib.py` | `FB82F34996DB92FC` | 26414 |
| `085-04-m53rows.py` | `99B157A83A58EDA6` | 97593 |
| `085-04-oracle.py` | `56A6C4665AABDB0E` | 97319 |
| `085-04-window.py` | `759B2829B06B3212` | 32735 |
| `085-04-run.ps1` | `34F7EA9133B12943` | 8487 |
| `085-03-content-manifest.py` | `085A394C4E0D7AB3` | 7214 |
| `085-03-container-diff.py` | `B5DBAFC25740530F` | 2710 |
| `085-03-nd4-preread.py` | `E01940A30B67C243` | 2193 |
| `085-03-cmdlet.ps1` / `085-03-cook.ps1` | `7965B11695FFD76F` / `EF5058E32940DDAF` | 1175 / 1783 |
| CaptureBench `tools\make_texcorrupt_fixture.py` / `texcorrupt_fixture_images.py` | commit `8f8b7f3` | — |
