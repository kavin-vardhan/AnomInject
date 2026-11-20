VERDICT: GREEN

# 081-17R — m55 build 2 (Claude Code, Opus 5.5): 256 MiB payload cap + ownership census; Stack 1080p twins 4/4 PASS; Lyra G270 PASS

## 1. Verdict
**GREEN.** Steps A–F all completed. Step D: 4/4 Stack 1080p twins PASS on attempt 1. Step E: Lyra
G270 at 300 frames reached the **PASS** branch. Stage 3 is not started (the 70-leg requalification
brief comes first, per 081-16 Ruling 3).

## 2. Plain summary for the owner
- The memory cap is now 256 MiB and the plugin records exactly what is held whenever memory peaks
  or runs out.
- One build, first try, no warnings. Every 1080p test on StackOBot passed with every required
  measurement present and no memory refusals.
- The Lyra 1080p test also passed: five events, every required measurement present, nothing refused.
- Worth knowing: Lyra used 197 MB, about three quarters of the new cap. It passed, but the headroom
  at 1080p on a real game is modest, and 1440p would probably not fit.
- A different project's Unreal editor started on this machine mid-run. I left it alone and waited
  39 minutes for it to finish before building.

## 3. What changed
Branch `feat/m55-change-evidence` only; pushed; no merge, tag or cook.

| Commit | Content |
|---|---|
| `db9e74d` | Dated 081-17 predictions amendment (top of `docs/predictions/2026-09-20-m55-change-evidence.md`), committed and pushed before the build and before any result. |
| `9c75abe` | Built source: `Source/AnomalyCapture/Private/AnomalyChangeStage.{h,cpp}` (+146/−5) and `docs/change-evidence-fields.md`. |
| `fcfde07` | Journal `docs/sessions/2026-09-24-081-17-code-m55-build2.md`, measured readings in the field reference, G282 addendum, `CLAUDE.md` status block. |

Exact source scope:
- `IAI.Capture.ChangeMaxBytes` compiled default 64 MiB → 256 MiB (268,435,456). `ReserveLocked`,
  the completion clock and every refusal semantic are unchanged from 5649a6d.
- (i) At every colour or mask reservation failure (including bench gate 10): one
  `Capture(m55): BUDGET-EXCEEDED` line and a `budget_census` array on the refused index's pair
  row(s).
- (ii) At each new high-water: the census is taken after the buffer's ownership is recorded and kept
  as the peak. `HIGH-WATER` lines are throttled to one colour allocation of rise, one
  `HIGH-WATER-PEAK` line is logged at closure, and `run_summary.change_bytes_peak_census` is added.
- Census fields: bytes held; distinct colours split pending / previous / phase_ref / in_hand /
  unowned (a shared owner counted once); masks and their bytes; `unaccounted_bytes`; cursor; latest
  index; open phases (first/last/closing); head state and `waits_on`.
- To make in-hand buffers visible, `Work()` records a pointer to the item being finalized
  (`InHand`). Nothing else depends on it.
- No closure-bound field, no formula-based warning. No comments added.

## 4. Build identity
- Game exe: `8e64fa3954fea8d57a31a6ac01a1e69ceb907ae3a0d1801662b163f57fac0650` (**8E64FA39**),
  241,763,328 B. Built = staged = `_binary_baselines\StackOBot.exe.m55-stage2-8E64FA39`, verified
  before every leg.
- Editor modules: archived to `_binary_baselines\m55-stage2-r17-main-editor-9c75abe` (27 files,
  hashes in `main-editor-binary.json`).
- Lyra: 11-module archive `_binary_baselines\m55-stage2-r17-lyra-48b9e048`.
- Cooked containers: five files, byte-unchanged.
- A44: all four new UTF-16 tokens are present in the staged exe and in the Lyra capture DLL; the
  invented control token is absent in both encodings.
- Build: StackOBot 113 s, StackOBotEditor 85 s, LyraEditor 61 s. All exit 0, no warnings.

## 5. Leg table
Budget = `change_reason_budget_exceeded` / `BUDGET-EXCEEDED` lines / rows carrying `budget_census`.

| Leg | Att. | Fixture | Measured / req. | Budget | High-water B | Peak census | Latency ms p50/p95/max | Disposition |
|---|---|---|---|---|---|---|---|---|
| B_NULL1080_N | 1 | valid, fg 86/86 | 20/20, 5 ev | 0/0/0 | 51,867,648 | 4C (pend1 prev1 ref1 hand1) + 9M | 296/334/350 | PASS |
| B_NULL1080_S | 1 | valid, fg 86/86 | 20/20, 5 ev | 0/0/0 | 64,315,392 | 5C (2/1/1/1) + 11M | 334/434/455 | PASS |
| B_SOLID1080_N | 1 | valid, fg 83/83 | 20/20, 5 ev | 0/0/0 | 66,392,064 | 5C (2/1/1/1) + 12M | 387/446/474 | PASS |
| B_SOLID1080_S | 1 | valid, fg 87/87 | 20/20, 5 ev | 0/0/0 | 70,545,408 | 5C (2/1/1/1) + 14M | 390/476/512 | PASS |
| LYRA_G3_N_A1 | 1 | INVALID, pre-capture L2 (host respawn after placement) | — | — | — | — | — | fixture attempt 1/3 |
| LYRA_G3_N_A2 | 2 | valid, fg 218/218, L1 0.115 cm/0°, L2 pre+post PASS | 20/20, 5 ev | 0/0/0 | 196,992,000 | 22C (pend21 prev1) + 7M | 1191/1551/1629 | **PASS** |

On every leg:
- Invariants: the EFFECTIVE line reads `maxBytes=268435456(from compiled)`; retained bytes 0;
  denominator, histogram and ref-onset mismatch 0; persist_failed, late_record_mutations and
  rows_dropped 0.
- Timing: closure_timeout, out_of_order_timeout and late results all 0. `speed_ratio` was
  1.001–1.005 on the Stack legs and 1.017 on Lyra.
- Census self-check closes: `unaccounted_bytes` 0, no unowned colour, peak = summary high-water,
  exactly one PEAK line.

## 6. Readings
**Refusal reasons over all issued indices:**
- Stack: `first_frame` 1 and `mask_payload_missing` 18–19. These are unlabelled frames, which carry
  no mask; none of them is a required pair.
- Lyra: `first_frame` 1 and `mask_payload_missing` 264.

**Onset vs control (unsubtracted; target gt8/n and mean vs whole-frame control):**

| Leg | Target gt8/n | Target mean | Control gt8/n | Control mean |
|---|---|---|---|---|
| Null N | 0.00004–0.00023 | 0.0021–0.0022 | 0.0045–0.0048 | 0.0051–0.0060 |
| Null S | 0.00005–0.00011 | — | 0.0044–0.0048 | — |
| Solid N | 1.0 (all five) | 0.206–0.216 | 0.0099–0.0131 | — |
| Solid S | 1.0 (all five) | 0.2045–0.2155 | 0.0092–0.0104 | — |

Lyra `stuck_low_mip`, five events (Cube4, Cube4, Cube2_8, Cube3, Cube4):

| Event | Target gt8/n | Control gt8/n | Target mean | Control mean |
|---|---|---|---|---|
| 1 | 0.584 | 0.530 | 0.120 | 0.0865 |
| 2 | 0.0266 | 0.069 | 0.0061 | 0.0147 |
| 3 | 0.00009 | 0.0013 | 0.0033 | 0.0030 |
| 4 | 0.0011 | 0.0042 | 0.0032 | 0.0036 |
| 5 | 0.1015 | 0.0976 | 0.0208 | 0.0185 |

The control is large on events 1 and 5; in-leg host motion is allowed. No threshold, no subtraction,
no verdict.

**G270 branch reached: PASS.** The first fixture-valid capture had 5 events (≥ 3). Every final
phase has `pairs_measured == pairs_required` (4/4 × 5), every window row is measured, zero
`budget_exceeded` anywhere, zero `closure_timeout`, and invariants are 0.

**Lyra high-water (facts only, no mechanism claimed):**
- Across 20 HIGH-WATER lines the peak rises from 8 to 197 MB.
- The gap between the latest issued index and the cursor grows from 12 to 53. Pending colours grow
  from 1 to 21. Masks stay at 0–7.
- At 18 of the 20 lines the head is `pending` with `waits_on` empty, meaning all of its inputs had
  arrived but the worker had not yet taken it. At the other 2 it is `in_hand`.
- One phase is open at the peak. This is not the closing-phase shape from 081-16.
- Source fact: the change worker and the PNG writer jobs both use
  `Async(EAsyncExecution::ThreadPool)`. Whether the worker was queued behind writer jobs is **not
  established (G120)**, and I did not investigate it.
- Margin: 71.4 MB, about 8.6 full 1080p colours.

**Predictions vs observed (all readings, none were gates):**
- Missed:
  - Stack high-water was predicted at 67–120 MB; observed 51.9 / 64.3 / 66.4 / 70.5 (3 of 4
    below the range).
  - Stack latency p50 was predicted at 400–550 ms; observed 296–390.
  - Stack peak masks were predicted at ≥ 10; null N had 9.
  - Lyra high-water was predicted at 60–140 MB; observed **197.0 MB**.
- Met: every other declared prediction (builds, A44, the EFFECTIVE line, D/E dispositions, zero
  refusals, census self-check).

## 7. Deviations and harness faults
1. **My preflight script faulted before any bench action.** It read 081-16's bank manifest paths as
   relative to the wrong directory. I fixed it once and re-ran it: zero mismatches across 141
   binaries, 1,684 prior artifacts and 13,015 bank files.
2. **Environment, not ours.** A UE 5.7 `UnrealEditor` for another project (HeistCrewUE,
   `-unattended`, PID 17236) started at 00:02:25, after my start check had passed.
   - The build script's idle check refused at its first line: nothing was checked out and nothing
     was built.
   - I waited up to a declared bound and never touched the process. It exited on its own at ~00:42.
   - No build or leg ran while it was present. Record: `081-17-evidence/environment-event-1.json`.
3. **Predictions header date.** It reads 2026-09-24, but the commit is at 23:59:53 on 09-23. The
   content is unaffected.
4. **Harness copies.** Codex's 081-15 runners were copied under 081-17 names. Changes:
   - evidence root and bank prefix `M55S2R17_`;
   - binary manifest v2;
   - the added predicates: 20/20, exactly 5 events, budget counter / lines / rows all 0, and the
     256 MiB EFFECTIVE line, plus the census self-check;
   - a refusal to reuse an existing label;
   - the suite restricted to the four twins, and the Lyra driver restricted to G270 with its
     branches written out.

   No harness fix iterations were needed during the legs.
5. **Lyra A1 was fixture-invalid** (host `GA_AutoRespawn` after the L2 placement). The 081-14 rules
   count this as fixture attempt 1 of 3; A2 is the reading. No retry of a valid reading happened.
6. **Pre-existing comments.** The feature branch's Stage 1–2 m55 source carries comments, against
   the repo's no-comments convention. I did not strip them because that is out of this build's
   scope. The stripper must run before merge.
7. **Scratch files.** A scratch folder outside the repos (`_scratch_081_17`) was used and then
   removed. The drafts and the pre-amendment prediction bytes are kept in `081-17-evidence/drafts/`.

## 8. Boundary proof
`_reviews/081-17-evidence/postflight.json`: **"PASS: boundary restored (main m51 53bf725, Lyra
caa68c6 with original 9 modules)"**, with `problems: []`. It covers:
- 1,684 prior artifacts and 13,015 prior bank files re-verified;
- 1,016 new bank files hashed;
- CURRENT.md unchanged;
- remote: feature at `fcfde07`, master at `031a103`;
- no bench processes.

The main checkout's Game and Editor binaries are build 2 (8E64FA39) despite the `m51` source, as
disclosed. `master`, `m51`, tags, CaptureBench, `ToCodex\` and the owner's untracked files are
untouched.

## 9. NEEDS-DECISION items
None required by the brief's rules. For chat's attention, as observations only:
- (a) Lyra 1080p used 73 % of the payload cap, with a growing backlog behind a ready head. The
  mechanism is not established. 1440p is likely over the cap.
- (b) Diagnostic (i) is runtime-UNEXERCISED. Its proof rides the next brief's G10 and
  `ChangeMaxBytes 1` legs in both orders, which must show the line and the row field.
- (c) The legacy comparisons in the next brief will see exactly one added `run_summary` key,
  `change_bytes_peak_census`.
- (d) 70 legs remain on build 2 (66 inventory plus 4 can-fails). Stage 3 is held until they run.

## 10. Wall-clock per step (IST)

| Step | Window | Duration |
|---|---|---|
| A | 23:51:20 → 23:55:15 | ≈ 4 min (includes the 46 s hash sweep) |
| B | 23:55:15 → 23:59:53 | ≈ 5 min (source read + amendment + push) |
| C | 23:59:53 → 00:47:15 | ≈ 47 min: ≈ 8 active (implementation ≈ 3, build 4.4, staging ≈ 0.5) + **39 waiting on the foreign editor** |
| D | 00:47:21 → ≈ 00:50 | ≈ 3 min (4 legs in 90 s) |
| E | 00:49:28 → ≈ 00:54 | ≈ 5 min (2 launches, 3 min) |
| F | ≈ 00:54 → ≈ 01:00 | ≈ 6 min |

Total ≈ 68 min wall, ≈ 29 min excluding the foreign-process wait. 6 physical launches, 5 capture
starts, 1 build set.
