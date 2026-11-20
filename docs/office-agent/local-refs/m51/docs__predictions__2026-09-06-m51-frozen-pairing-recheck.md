# m51 - FROZEN PAIRING RE-CHECK - campaign predictions

# DRAFT - NO EXECUTION.
# NOTHING IN THIS FILE MAY RUN UNTIL (a) Codex and Chat review the exact snapshot
# and (b) a follow-up brief explicitly RELEASES it. Brief 078-10 releases
# DOCUMENTATION AND READ-ONLY PREFLIGHT ONLY. No capture, staging, build, cook,
# lever, probe or implementation is released here.

Author: Code, brief 078-10, 2026-09-06. Append-only. This file **rewrites no earlier
prediction and erases no earlier failure**. The 078-05 / 078-06 failure banks stand
exactly as recorded and are the comparator, not something this campaign supersedes.

---

## 0. Purpose, and the ceiling on what a PASS would mean

Re-check the MASK-PICTURE-PAIRING predicate on **frozen bytes** - the archived A-side and
the archived F1 candidate over the byte-identical Phase B container - across both tick
orders at 100 % and 50 % screen percentage.

A PASS here is **scoped pairing smoke on these frozen bytes and these recipes.** It is
**NOT** release certification of `F1` or `m51`, not an all-gates claim, not a Lyra pass,
and it does not retire the banked failures. A candidate FAILURE plus its one A-control
**informs attribution and proves nothing about environment, FIFO ordering, or a client
cause.**

⚠ **Known ceiling of the instrument, stated before it is used:** the pairing probe has
**two positions** (`posA` / `posB`) alternating per captured tick. A slip of an **even**
number of frames maps position onto itself and is therefore **invisible to this gate**.
`CURRENT` on every decidable frame does not exclude an even-frame slip.

---

## 1. Preflight receipts (read-only, taken 2026-09-06 under brief 078-10)

| item | value |
|---|---|
| `AnomalyInjector` HEAD | `d572b3dbcace8202f18de33ce9c697ab89f96037` (branch `m51`) |
| `master` == `origin/master` | `ac13700d99fe116056f5aa11611f8931a563a3f2` |
| production freeze | `git diff 6713f51..HEAD -- Source Shaders` **EMPTY**; tracked tree clean |
| untracked owner handoffs | **10**, preserved |
| `CaptureBench` | `472a409dc70ae78d01a38d08fc3bddb15fe41c58`, tracked clean, 7 untracked tools preserved |
| Lyra worktree | detached `4a4bfcc2cdbade6bbeb99494e540952847ae9b63`, clean, untouched |

**Binaries (full SHA-256):**

- A archive `D:\IntrusiveAnomalies\_binary_baselines\StackOBot.exe.m49b-gate-B8AC65D8`
  241,400,320 B `B8AC65D87030D311632A4605C8EFF5C6D54A1ACCB23BE76682C6E643498EEAD8`
- Candidate archive `D:\IntrusiveAnomalies\_binary_baselines\StackOBot.exe.m51-f1-candidate-D50DDE78`
  241,401,856 B `D50DDE789A94933182DC6B3FA74811F8BB57DC1F8332B4AC88ACE47337BD4796`
- Staged runtime currently **== the candidate** (as left by 078-09).

**Frozen Phase B container quintet** (`Builds\BenchGate\Windows\StackOBot\Content\Paks`):

| file | bytes | SHA-256 |
|---|---|---|
| `StackOBot-Windows.utoc` | 294,235 | `67EA1FE0A15ACF2429516C461C5FD85989285B3C93B24C42855AE4959386257A` |
| `StackOBot-Windows.ucas` | 364,596,784 | `2CEFB8F43D048E2534471BC1D9532B77A870FAF46F14C9C2AFAE813B99B6C4D3` |
| `StackOBot-Windows.pak` | 10,115,711 | `E03C661084D8BC2BB6EF0F51F2DECB162853B2188B733B1DBD22B96CA974CDB7` |
| `global.ucas` | 1,833,008 | `A16A18A8FFB5151D2E850BB52841162F6844A0361818F8D1F158E8377DFFBA9E` |
| `global.utoc` | 539 | `C70ECDAA232DB8CF77BEEAB47B7226C48FCC91FF705526D507500CD14CF21C51` |

⛔ **No quarantined interrupted-link artifact is used.** A recursive search of
`D:\IntrusiveAnomalies` (depth 6) for `*QUARANTINE*` returned **no file**; whether it was
removed earlier is not established here and is not chased. The only executables this
campaign may stage are the two archives named above.

**Harness provenance (SHA-256, verified unchanged since the 078-08/09 legs):**

- `D:\IntrusiveAnomalies\StackOBot\Plugins\CaptureBench\tools\run_leg.ps1`
  26,334 B `9E8D669F1C194FE5998FC277D9285EC17602D4399CC6EA6B4D1137E16FADB945`
- `D:\IntrusiveAnomalies\StackOBot\Plugins\CaptureBench\tools\m44_pairing_probe.py`
  10,684 B `918EB4B5888A65D589C079D355AC09FBFC3D0987A69979C4DF3FC0F477A7AF1F`

⚠ These are **today's** hashes. They are **not** historical receipts: nothing here proves
what the bytes were during the 078-05 era. Tool-drift claims stay scoped to what was
actually hashed at the time.

---

## 2. Recipe provenance, and the Marker correction

The recipe is derived from the **banked Phase B pairing legs' own recorded configs**
(`G184`), not hand-transcribed. `_leg_geometry.json` of `PB1F_F_PAIR_{NAT,SYN,NAT_SP50,SYN_SP50}`
records, identically on all four: `map /Game/CaptureBenchGate/CB_GateLevel`, `res 1280x720`,
`windowed`, `sve 1`, `delivery 0`, `pace 1`, `stall_game_ms 0`, `video_fps 30`,
`anomaly blinking`, `target StaticMeshActor_49`, `seed 4242`, `max_frames 40`,
`b1_pose_gate_applies true`, `letterboxed_fixture false`, `require_modal_rot_zero false`,
and `extra_execcmds` differing exactly as the four rows below.

| recipe | banked `extra_execcmds` |
|---|---|
| natural 100 % | `IAI.Bench.MaskPairingProbe 1` |
| synth 100 % | `IAI.Bench.MaskPairingProbe 1, IAI.Bench.SynthTickOrder 1` |
| natural 50 % | `IAI.Bench.MaskPairingProbe 1, r.ScreenPercentage 50` |
| synth 50 % | `IAI.Bench.MaskPairingProbe 1, IAI.Bench.SynthTickOrder 1, r.ScreenPercentage 50` |

🔻 **CORRECTION CARRIED FORWARD — the 078-05 smoke subrecord's identical-recipe claim was
FALSE on the Marker axis.** That subrecord pinned its four pairing legs with **`-Marker 0`**
while asserting *"This reproduces `PB1F_F_PAIR_{NAT,SYN,NAT_SP50,SYN_SP50}` exactly (G184)"*.
The banked Phase B legs pass **no** `-Marker` at all and therefore ran at the runner's
**compiled default `Marker 1`** (`run_leg.ps1:48`). `-Marker 0` is consequently **not** the
banked recipe. **This campaign issues `-Marker 1` explicitly**, which matches the banked
legs' effective value while making it a visible argument rather than an inherited default.

⚠ **Two honest limits on that statement.** (1) `Write-LegGeometry` (`run_leg.ps1:167-188`)
records **neither `Marker` nor `AutoExposure` nor `MaxAttempts`**, so the Phase B legs'
Marker value rests on the runner default, **not on a recorded receipt** — this is 078-06's
blindness finding, confirmed precisely here. (2) 078-06 measured the marker axis to move the
verdict by **nothing** (`M51D_HI_OLD_NAT` at `Marker 1` and `M51D_HI_NEW_NAT` at `Marker 0`
both read `CURRENT 4 / PREVIOUS 9 / NEITHER 0`), so correcting the axis is about **honest
provenance, not about expecting a different number.**

---

## 3. Runner defaults deliberately retained (resolved, not inherited silently)

Not passed on any command line below, therefore in force at their compiled values
(`run_leg.ps1:42-66`):

`-Map /Game/CaptureBenchGate/CB_GateLevel` · `-StallGameMs 0` · `-WaitSec 150` ·
`-Fullscreen` **absent** (windowed) · `-RequireModalRotZero` **absent** ·
`-LetterboxedFixture` **absent** · `-VerboseAnomalyLog` **absent** · `-GeometryNote ""`.

Runner-fixed inside the script and **not overridable from the CLI**: `IAI.Capture.Fps 30`
and `IAI.Capture.Config 2 4 8 4 0` (`run_leg.ps1:219-220`). Both are preserved unchanged.

**cvars actually ISSUED** (assembled at `run_leg.ps1:203-231`, `-ExtraExecCmds` inserted at
`:224` **before** `IAI.Capture.Start` at `:231`, so all are in force at StartRun):
`r.DefaultFeature.AutoExposure 0`, `r.EyeAdaptationQuality 0` (from `-AutoExposure 0`), the
six bare AE names as A48 read-backs, `CaptureBench.Marker 1`, `CaptureBench.Stall.GameMs 0`,
`IAI.Capture.SVE 1`, `IAI.Capture.Delivery 0`, `IAI.Capture.Pace 1`, `IAI.Capture.Fps 30`,
`IAI.Capture.Config 2 4 8 4 0`, then the recipe's `-ExtraExecCmds`, then `IAI.Capture.Start`.

**Defaults READ BACK from the run log rather than issued** — the mask, target mask and
census run at their **compiled shipping defaults** and are asserted from each leg's own
`Capture(mask)` / `Capture(m43)` / `Capture(census)` / `Capture(m28)` banners (A48).
⛔ **No extra switch is invented.** In particular `IAI.Capture.TargetMask`,
`IAI.Capture.Mask`, `IAI.Capture.Census` and `IAI.Capture.OutputHeight` are **NOT** issued
on these legs — issuing them would change the recovered recipe away from the banked baseline.
`IAI.SetViewportScoping` is likewise deliberately not issued.

**Tick order is declared from CLI semantics, not assumed:** natural order = `-ExtraExecCmds`
**without** `IAI.Bench.SynthTickOrder`; synth order = with `IAI.Bench.SynthTickOrder 1`. Each
leg's log must be read back for `m40 SYNTH TICK ORDER = off` / `= on` accordingly.

---

## 4. The campaign - fixed order, nine predeclared commands, at most six legs run

Bank prefix **`M51R_`**. Verified free at preflight: all 9 final banks and all 36
`_tryN` bank paths, plus all 36 runner attempt directories under
`Builds\BenchGate\Windows\StackOBot\Binaries\Win64\`, are **unused** (45 + 36 paths checked,
zero clashes). ⛔ **Nothing existing may be overwritten, deleted, copied over or renamed.**
If any path is occupied at release time, **stop** and document a different prefix; do not
reuse.

Common argument block, identical on **all nine** commands:

```
-BankPrefix M51R_ -Anomaly blinking -Target StaticMeshActor_49 -Seed 4242 -MaxFrames 40
-ResX 1280 -ResY 720 -Sve 1 -Delivery 0 -Pace 1 -Marker 1 -AutoExposure 0 -MaxAttempts 4
```

### 4.1 Leg 1 - fresh A-side natural reference (staged binary: A `B8AC65D8`)

```
powershell -NoProfile -ExecutionPolicy Bypass -File D:\IntrusiveAnomalies\StackOBot\Plugins\CaptureBench\tools\run_leg.ps1 -Label F1R_A_REF_NAT -BankPrefix M51R_ -Anomaly blinking -Target StaticMeshActor_49 -Seed 4242 -MaxFrames 40 -ResX 1280 -ResY 720 -Sve 1 -Delivery 0 -Pace 1 -Marker 1 -AutoExposure 0 -MaxAttempts 4 -ExtraExecCmds "IAI.Bench.MaskPairingProbe 1"
```

### 4.2 Legs 2-5 - candidate, only if leg 1 is VALID and PASSES (staged binary: candidate `D50DDE78`)

```
powershell -NoProfile -ExecutionPolicy Bypass -File D:\IntrusiveAnomalies\StackOBot\Plugins\CaptureBench\tools\run_leg.ps1 -Label F1R_B_NAT -BankPrefix M51R_ -Anomaly blinking -Target StaticMeshActor_49 -Seed 4242 -MaxFrames 40 -ResX 1280 -ResY 720 -Sve 1 -Delivery 0 -Pace 1 -Marker 1 -AutoExposure 0 -MaxAttempts 4 -ExtraExecCmds "IAI.Bench.MaskPairingProbe 1"

powershell -NoProfile -ExecutionPolicy Bypass -File D:\IntrusiveAnomalies\StackOBot\Plugins\CaptureBench\tools\run_leg.ps1 -Label F1R_B_SYN -BankPrefix M51R_ -Anomaly blinking -Target StaticMeshActor_49 -Seed 4242 -MaxFrames 40 -ResX 1280 -ResY 720 -Sve 1 -Delivery 0 -Pace 1 -Marker 1 -AutoExposure 0 -MaxAttempts 4 -ExtraExecCmds "IAI.Bench.MaskPairingProbe 1, IAI.Bench.SynthTickOrder 1"

powershell -NoProfile -ExecutionPolicy Bypass -File D:\IntrusiveAnomalies\StackOBot\Plugins\CaptureBench\tools\run_leg.ps1 -Label F1R_B_NAT_SP50 -BankPrefix M51R_ -Anomaly blinking -Target StaticMeshActor_49 -Seed 4242 -MaxFrames 40 -ResX 1280 -ResY 720 -Sve 1 -Delivery 0 -Pace 1 -Marker 1 -AutoExposure 0 -MaxAttempts 4 -ExtraExecCmds "IAI.Bench.MaskPairingProbe 1, r.ScreenPercentage 50"

powershell -NoProfile -ExecutionPolicy Bypass -File D:\IntrusiveAnomalies\StackOBot\Plugins\CaptureBench\tools\run_leg.ps1 -Label F1R_B_SYN_SP50 -BankPrefix M51R_ -Anomaly blinking -Target StaticMeshActor_49 -Seed 4242 -MaxFrames 40 -ResX 1280 -ResY 720 -Sve 1 -Delivery 0 -Pace 1 -Marker 1 -AutoExposure 0 -MaxAttempts 4 -ExtraExecCmds "IAI.Bench.MaskPairingProbe 1, IAI.Bench.SynthTickOrder 1, r.ScreenPercentage 50"
```

### 4.3 The four CONDITIONAL A-side controls (staged binary: A `B8AC65D8`)

🚨 **AT MOST ONE of these may ever execute.** It is selected by the **FIRST valid candidate
pairing failure**, and it is the control matching **that** recipe. **After that control runs,
STOP regardless of its result** - no continuation to the remaining candidate cells.

```
powershell -NoProfile -ExecutionPolicy Bypass -File D:\IntrusiveAnomalies\StackOBot\Plugins\CaptureBench\tools\run_leg.ps1 -Label F1R_A_CTL_NAT -BankPrefix M51R_ -Anomaly blinking -Target StaticMeshActor_49 -Seed 4242 -MaxFrames 40 -ResX 1280 -ResY 720 -Sve 1 -Delivery 0 -Pace 1 -Marker 1 -AutoExposure 0 -MaxAttempts 4 -ExtraExecCmds "IAI.Bench.MaskPairingProbe 1"

powershell -NoProfile -ExecutionPolicy Bypass -File D:\IntrusiveAnomalies\StackOBot\Plugins\CaptureBench\tools\run_leg.ps1 -Label F1R_A_CTL_SYN -BankPrefix M51R_ -Anomaly blinking -Target StaticMeshActor_49 -Seed 4242 -MaxFrames 40 -ResX 1280 -ResY 720 -Sve 1 -Delivery 0 -Pace 1 -Marker 1 -AutoExposure 0 -MaxAttempts 4 -ExtraExecCmds "IAI.Bench.MaskPairingProbe 1, IAI.Bench.SynthTickOrder 1"

powershell -NoProfile -ExecutionPolicy Bypass -File D:\IntrusiveAnomalies\StackOBot\Plugins\CaptureBench\tools\run_leg.ps1 -Label F1R_A_CTL_NAT_SP50 -BankPrefix M51R_ -Anomaly blinking -Target StaticMeshActor_49 -Seed 4242 -MaxFrames 40 -ResX 1280 -ResY 720 -Sve 1 -Delivery 0 -Pace 1 -Marker 1 -AutoExposure 0 -MaxAttempts 4 -ExtraExecCmds "IAI.Bench.MaskPairingProbe 1, r.ScreenPercentage 50"

powershell -NoProfile -ExecutionPolicy Bypass -File D:\IntrusiveAnomalies\StackOBot\Plugins\CaptureBench\tools\run_leg.ps1 -Label F1R_A_CTL_SYN_SP50 -BankPrefix M51R_ -Anomaly blinking -Target StaticMeshActor_49 -Seed 4242 -MaxFrames 40 -ResX 1280 -ResY 720 -Sve 1 -Delivery 0 -Pace 1 -Marker 1 -AutoExposure 0 -MaxAttempts 4 -ExtraExecCmds "IAI.Bench.MaskPairingProbe 1, IAI.Bench.SynthTickOrder 1, r.ScreenPercentage 50"
```

Each control differs from its candidate leg **only in the staged executable and the label**,
which is what makes it a control.

### 4.4 Checker - one invocation per accepted leg, unchanged tool

```
C:\Python313\python.exe D:\IntrusiveAnomalies\StackOBot\Plugins\CaptureBench\tools\m44_pairing_probe.py D:\IntrusiveAnomalies\_bench_sessions_bank\M51R_<LABEL>\<session_dir> <LABEL>
```

`<session_dir>` is the single `session_YYYYMMDD-HHMMSS` folder inside the bank; it is read
from disk after the runner prints `BANKED`, never guessed. Usage is fixed by
`m44_pairing_probe.py:249` (`main(sys.argv[1], sys.argv[2])`).

---

## 5. Predicate, exit codes, and how a non-verdict is classified

**Primary predicate, UNCHANGED:** `PREVIOUS == 0` **AND** `NEITHER == 0` over decidable
frames. `decidable = CURRENT + PREVIOUS + NEITHER` (`:232`); `same-pos` and `no-data` are
**excluded** from decidable by construction.

- **Exit 0 means the leg was READ, never automatically PASS** (`:47`). The verdict is decided
  by the predicate above, applied to the printed TOTALS line.
- **Exit 2 / 3 / 4 are REFUSALS TO JUDGE** and are recorded as refusals - never as a pass,
  a fail, or a `NEITHER` count. Exit 2 = no `run.json` (fixture unestablishable; a delivery
  leg can never be read). Exit 3 = colliding fixture. Exit 4 = no probe in the leg.
- ⛔ **Exit 3 cannot fire on this campaign** and that is structural, not lucky:
  `COLLIDING_ANOMALIES = {"corrupted_texture"}` (`:~60`) and every leg fires `blinking`.
- 🚨 **`decidable == 0` exits 0 and prints `VERDICT: UNDECIDABLE`.** It is classified as
  **INVALID (vacuous), NOT a pass.** A leg whose probe produced no decidable frame carries
  no pairing evidence at all.

**Non-vacuity requirement, declared in advance:** an accepted leg must show a live probe -
a `MASK-PAIRING PROBE SPAWNED tag=255` line **and** `PROBE STEP` lines - read **from the
leg's own run log**, plus `decidable >= 1`. The checker enforces the spawn line itself
(`:101-115`, exit 4), which is why probe enablement is a **read-back and not an assumption**.

---

## 6. Retry, event-count and stop logic

- **`-MaxAttempts 4` is a POSE-INVALIDITY retry budget ONLY** (A63 / B1, both unchanged). It
  is **not** four chances at a checker pass.
- 🚨 **An accepted, pose-valid leg whose checker verdict fails is NEVER retried.** No
  hidden candidate retry, no re-run until it happens to pass, no tolerance widened, no
  calibration change, **no `si=8` waiver**.
- **Physical attempts are counted separately from accepted bank copies.** Every physical
  attempt is retained (`_tryN`), including discards; the accepted alias is a copy, not a
  replacement. A leg accepted on attempt 3 is reported as "3 physical attempts, 1 accepted".
- **`>= 3` counted events required.** An otherwise-accepted leg with **fewer than 3** counted
  events is **INVALID and STOPS the campaign for review** - it is **not** re-run ad hoc and
  **not** graded on the reduced set.
- **Reference gate:** if leg 1 is invalid, refused, or fails the predicate, **STOP with no
  control and no candidate leg.**
- **A refused / unrunnable / attempt-exhausted leg STOPS** and does **not** count as a valid
  pairing failure; it releases no control.
- **Any broader gate failure stops for review.** The one-control allowance applies
  *specifically* to a valid candidate pairing-predicate failure on an intact, correctly
  configured fixture.
- ⛔ No extra smoke, ONSET/G7, load, editor or Lyra leg is released by this record.

---

## 7. What every leg records

Per leg: staged executable SHA-256 and all five container SHA-256 **before and after**;
process-scope check (see §8); accepted session path and physical attempt count; the pose
line (`modal_rot`, `bbox`, `rows`, `distinct`, `modal`, `pose_match`); the effective-config
read-backs (grab point, mask, target mask, census, output height/geometry, screen
percentage, `m40 SYNTH TICK ORDER`); probe spawn line and `PROBE STEP` count.

From the checker: **`CURRENT` / `PREVIOUS` / `NEITHER` / `same-pos` / `no-data`**, the printed
`VERDICT` line, the exit code, and **every failing frame index listed explicitly** (not just
totals).

From the artifacts: `target_mask_frames_measured / _hidden_blank / _unavailable`;
`label_schema` (expected 2); `labels.jsonl` row count and `session_index` completeness;
`Actual_Frames` count, index range and zero-byte count; image dimensions; event count;
`vetoed_events`; `key_ring_missed` / `key_ring_corrupted`; `speed_ratio`;
`sustained_wall_fps`; `ticks_per_captured_frame`.

⚠ **FPS, `speed_ratio` and pacing are READINGS ONLY.** No validity band is introduced, none
is enforced, and **no historical leg is retroactively invalidated.** A valid pairing failure
remains a failure at any speed. (Chat's proposed pacing-validity band is **NOT ADOPTED**.)

---

## 8. Execution policy - FUTURE ONLY, staging remains prohibited under 078-10

For each leg, at release time: stage **only** from the two pinned archives (`Copy-Item`
archive -> staged path), verify the staged SHA-256 equals the intended archive **before**
launching, and **restore the candidate `D50DDE78` and re-verify its SHA-256 on every exit
path, including failure, refusal and abort.** Re-hash all five container files after each leg
and require byte-equality. ⛔ **No rebuild and no cook, ever, in this campaign.**

**Process scope:** `run_leg.ps1:195,319` kills `Get-Process -Name "StackOBot*"` only. Before
each attempt, assert zero `StackOBot*` processes and confirm that nothing else on the box
matches that pattern. ⛔ **No owner application is closed, started, killed, focused,
reconfigured, coordinated with, or inspected for project content.** ⛔ **Editor absence is
NOT a prerequisite for any leg** - a blanket `UnrealEditor.exe` rule would also kill the Lyra
editor fixture. The original ~2.6-core editor workload is **not reproducible from the
available receipts and is not chased here.** No office material or action is involved.

---

## 9. Predictions, and the genuine falsifier

**Leg 1 (A reference):** predicted **PASS** - `PREVIOUS 0`, `NEITHER 0`, `decidable >= 1`,
`>= 3` counted events. Basis: two A-side legs passed this exact predicate on this exact
binary and container three hours earlier (078-08/09).

**Legs 2-5 (candidate):** predicted **PASS** on all four recipes.

⚠ **Uncertainty preserved, and it is the point.** The recent A-side references passed, but
**the earlier failing state remains UNEXPLAINED**, so a reference pass does not guarantee the
box is in the same state an hour later. The 078-06 association is with throughput and is
**confounded on time**; it is **not a mechanism** and cannot be used to predict tonight.

⛔ **No exact `CURRENT` / `no-data` counts are predicted.** Those depend on how many frames
happen to carry both a picture and a mask centroid, which is arbitrary timing. Predicting
them would manufacture a false precision. (For orientation only, not as a prediction: the
078-08/09 passes read `decidable` 35 and 37 with `no-data` 5 and 3.)

**Genuine falsifiers - each would refute the predicted outcome:**

1. Leg 1 returns `PREVIOUS > 0` or `NEITHER > 0` ⇒ the reference itself is failing ⇒ **STOP**,
   no candidate leg. This is a live possibility: the failing state's cause is open.
2. Any candidate leg returns `PREVIOUS > 0` or `NEITHER > 0` on an intact fixture ⇒ a valid
   candidate failure ⇒ run **exactly one** matching A control, then **STOP**.
3. Any leg returns `decidable == 0`, or a refusal, or `< 3` counted events ⇒ **INVALID** ⇒
   **STOP** for review; it is not a pass and not a valid failure.

**Attribution rule fixed in advance:** if a candidate leg fails and its A control **also**
fails, the failure is **not attributable to the candidate binary**; if the control **passes**,
the reading is **consistent with** a candidate contribution and is **not proof of one**.
🚨 **Neither branch establishes environment, FIFO ordering, render-thread-ahead arming, or a
client cause.** ⛔ And **failure without `F1` does not exclude an `F1` contribution.**

---

## 10. Held - not released, not waived, not implemented by this record

Production `Source/` and `Shaders/`; fixtures; harness and checker source; schema;
observability meanings; identity / lifecycle / geometry / tag / writer / control-flow
changes - **all HELD.** No lever and no new probe is implemented.

Specifically held as **unproved design work for a later reviewed unit**, recorded so it is
not mistaken for settled: Chat's *"second target arm is stale"* is **narrowed - cancellation
must follow proven render/request identity, never queue position alone**; raw
`ArmGameFrame` / `FamilyFrame` equality is **not proved across counter domains** and an
observed offset is not itself proof of mismatch; the render-thread-ahead-arm account is a
**hypothesis**; the colour key path **does not certify mask pairing**; the 8-bit + validity
probe and `WithholdWorldRender` arming behaviour are **unproved**. The 078-07 payload-loss
finding stands independently and is neither repeated nor extended here.

⛔ The full campaign and the Lyra arm are **not waived**. `M-F1b` (backbuffer x target-mask)
remains **UNRUN**. No push, merge or tag. Pairing release certification and every `m51`
implementation item remain **BLOCKED**.

**VERDICT: NEEDS-DECISION** - this draft requires Codex and Chat review before any release.

---

# AMENDMENT 1 — 2026-09-06, brief 078-11 — FIVE BOUNDED CORRECTIONS

Written after Codex's review of the `e0d63f1` draft (`_reviews/078-10-codex-prediction-review.md`)
and **before any leg**. Sections 0–10 above are the **historical draft and are left intact**.
Where this amendment and the draft disagree, **THIS AMENDMENT GOVERNS.** No campaign command,
leg order, retry budget, one-control rule, predicate, tolerance or hold is changed by it.

## A1.1 — SUPERSEDES §9's attribution rule (it overclaimed and self-contradicted)

⛔ **Superseded text (§9, "Attribution rule fixed in advance"):** *"if a candidate leg fails and
its A control **also** fails, the failure is **not attributable to the candidate binary**; if the
control **passes**, the reading is **consistent with** a candidate contribution…"*

That first clause overclaims, and it contradicts the very next sentence of the same section
(*"failure without `F1` does not exclude an `F1` contribution"*).

✅ **Governing rule:** if a candidate leg fails and its A control **also** fails, then **the
failure occurs WITHOUT `F1`.** These observations **do not isolate an `F1` contribution and do
not exclude one.** If the control **passes**, a candidate contribution is **consistent with the
observation**, but **time and machine state remain confounded** and the reading is not isolated.
🚨 **Neither branch is causal proof**, and neither establishes environment, FIFO ordering,
render-thread-ahead arming, or a client cause.

## A1.2 — SUPERSEDES §2's "no recorded receipt" claim: a banked runtime receipt EXISTS

⛔ **Superseded text (§2, limit (1)):** *"the Phase B legs' Marker value rests on the runner
default, **not on a recorded receipt**."* **That is wrong.** `_leg_geometry.json` is blind to the
Marker axis — that half stands — but **blindness of the geometry file is not absence of a
receipt.**

✅ **The receipt exists and is banked.** Each `_tryN` attempt bank carries `_leg_game.log`, the
whole packaged game log. In all four:

| banked file | bytes | SHA-256 |
|---|---|---|
| `PB1F_F_PAIR_NAT_try1\_leg_game.log` | 564,449 | `A5960C07C2D1C190D2FFDE82F429B2597D8F098557472068F9FD900DCA2F79B3` |
| `PB1F_F_PAIR_SYN_try1\_leg_game.log` | 554,349 | `DF9143427DFEF5329C4B6F0CD1731584643BCDDBD35B1BC3FAE49ABD620AFE78` |
| `PB1F_F_PAIR_NAT_SP50_try1\_leg_game.log` | 585,903 | `87D80176100ED179C3FF7F932F5A803B41C728F3BD7838276D49E23D1899C411` |
| `PB1F_F_PAIR_SYN_SP50_try1\_leg_game.log` | 564,499 | `7015B0B9DC3AEC251D89E0E8F41F007860D58567A1C876CB5404819506ECF72F` |

- **Line 1533 of every one reads `CaptureBench.Marker = "1"`** — the engine's own effective
  read-back, adjacent to the AE echoes (`r.DefaultFeature.AutoExposure.Method`, `.Bias`,
  `.ExtendDefaultLuminanceRange`, `r.EyeAdaptation.MethodOverride` at 1529–1532, and
  `CaptureBench.Stall.GameMs = "0"` at 1534).
- **Lines 17 and 207 carry the FULL launch command**, which contains
  `... r.DefaultFeature.AutoExposure 0, r.EyeAdaptationQuality 0, ... CaptureBench.Marker 1,
  CaptureBench.Stall.GameMs 0, IAI.Capture.SVE 1, ... IAI.Bench.MaskPairingProbe 1,
  IAI.Capture.Start F_PAIR_NAT_try1 png 4242 40 blinking StaticMeshActor_49 ...`.

⇒ **`Marker 1` on the banked Phase B legs is PROVEN BY RECEIPT, not inferred from a default.**
The §2 correction of 078-05's `-Marker 0` claim is unchanged and still correct; only its
*evidentiary basis* is upgraded.

⚠ **Three limits kept explicit.** (1) **`MaxAttempts` remains invocation-script evidence, not a
runtime field** — it never reaches the engine and appears in no log. (2) **These logs do NOT
establish complete historical tool hashes**; §1's "today's hashes are not historical receipts"
stands unchanged. (3) 🚨 **The accepted alias bank carries NO `_leg_game.log`** — measured:
`PB1F_F_PAIR_NAT` holds only `session_.../` + `_leg_geometry.json`, because the runner's final
copy (`run_leg.ps1:491-494`) copies the **session**, not the `_tryN` bank. **Cite the matching
`_tryN` attempt bank for any launch-command receipt.**

## A1.3 — ADDS to §7 the specific read-back that identifies screen percentage

§7 named a "screen percentage" read-back without saying which line carries it. It is the
**`Capture(mask): M23 PASS`** line, which prints `internalViewRect` and `unscaledViewRect`:

- **SP50, banked `PB1F_F_PAIR_NAT_SP50_try1` line 1585:**
  `customStencilExtent=640x360 viewRect=1280x720 internalViewRect=(0,0)-(640,360) 640x360
  unscaledViewRect=1280x720`, with `r.ScreenPercentage = "50"` echoed at **line 1542**.
- **100 %, banked `PB1F_F_PAIR_NAT_try1`:** the SAME line reads
  `internalViewRect=(0,0)-(1280,720) 1280x720 unscaledViewRect=1280x720`.

🔑 **The 100 % arm is therefore read from RENDERED GEOMETRY, not from a cvar echo.** Measured:
**no explicit `r.ScreenPercentage = "100"` line exists** on the 100 % leg (only unrelated
`r.TSR.History.ScreenPercentage` / `r.Bloom.ScreenPercentage` config lines). ⛔ **Requiring an
explicit 100 echo would be an impossible new requirement and is NOT imposed.**

**Artifact preconditions, native output, distinct from allowed mask no-data:** each leg must
write **40 PNGs `frame_00000..frame_00039`** and **40 `labels.jsonl` rows with `session_index`
0..39**, at **1280×720**, with **no duplicate index, no row lacking its RGB file, no zero-byte
file, and no orphan RGB without a row.** ⚠ These are **artifact-integrity** conditions; a
`no-data` frame in the pairing table is a legitimate mask outcome and is **not** a violation of
any of them.

**Runnable checker invocation** (resolves exactly one real session directory; no timestamp is
invented, and the exactly-one assertion is the guard):

```
$LABEL = 'F1R_A_REF_NAT'
$bank  = "D:\IntrusiveAnomalies\_bench_sessions_bank\M51R_$LABEL"
$s = @(Get-ChildItem -LiteralPath $bank -Directory -Filter 'session_*')
if ($s.Count -ne 1) { throw "expected exactly one session dir in $bank, found $($s.Count)" }
& C:\Python313\python.exe D:\IntrusiveAnomalies\StackOBot\Plugins\CaptureBench\tools\m44_pairing_probe.py $s[0].FullName $LABEL
"EXIT=$LASTEXITCODE"
```

Substitute `$LABEL` with each of the nine labels in §4. The pattern was trialled **read-only**
against the existing `M51D_HE_RESIDENT_NAT` bank and reproduced its banked reading exactly
(`decidable 35 · CURRENT 35 · PREVIOUS 0 · NEITHER 0`, exit 0).

## A1.4 — CORRECTS §3's override claim and §6's retention claim; RESTORES §9's three-way split

⛔ **Superseded (§3):** *"Runner-fixed inside the script and **not overridable from the CLI**."*
**Measured:** `IAI.Capture.Fps 30` and `IAI.Capture.Config 2 4 8 4 0` are issued at
`run_leg.ps1:219-220`, and `-ExtraExecCmds` is appended **AFTER** them at `:224`, so a later
duplicate in `-ExtraExecCmds` **WOULD override**. ✅ **Governing statement: these nine commands
do NOT override them** — no `-ExtraExecCmds` string in §4 contains `IAI.Capture.Fps` or
`IAI.Capture.Config`. Override is *possible*; it is simply not *done*.

⛔ **Superseded (§6):** *"Every physical attempt is retained (`_tryN`)."* **Too strong.** The
runner has **early `continue` paths that fire BEFORE** the `_tryN` bank and log copy at
`:408-418`:

| path | line | banked `_tryN`? | `_leg_game.log`? |
|---|---|---|---|
| `INVALID (no run_summary)` | `:322-324` | **NO** | **NO** |
| `INVALID (rode focus timeout)` | `:371-373` | **NO** | **NO** |
| `POSE GATE FAILED (B1)` | `:460-463` | yes | yes |
| `A47 ROTATION GATE FAILED` | `:466-469` | yes | yes |
| accepted | — | yes | yes |

*(The two `-LetterboxedFixture` early exits at `:342`/`:349` cannot fire here — that switch is
not passed.)*

✅ **Governing retention statement.** The runner's **raw attempt directory**
`Builds\BenchGate\Windows\StackOBot\Binaries\Win64\<Label>_try<N>` persists on a **unique path**
for every physical attempt, and `:201` only clears a directory of that same name at the *start*
of its own attempt. But a **bank copy and `_leg_game.log` exist only for attempts that reach
`:408`.** ⚠ Additionally the packaged log is **rotated on the next launch** (`:414-418`), so an
early-exit attempt's game log is **gone once the next attempt starts.**
⇒ **After the runner exits, the raw `<Label>_try<N>` directories are copied into the campaign's
evidence directory as-is**, and **any attempt lacking a bank or a `_leg_game.log` is enumerated
by name with its verdict.** 🚨 **Never report a bank or log that does not exist.**
⛔ **No harness change is released to fix this** — it is described, not repaired.

⛔ **Superseded (§9, falsifier 3):** it folded refusal, `decidable == 0` and `< 3` events into a
single "INVALID". §5 already distinguishes them and §9 must not collapse them. ✅ **Governing
three-way split, all of which STOP the campaign:**

- **REFUSED** — checker exit 2/3/4. Recorded as a **refusal to judge**; never a pass, never a
  fail, never a `NEITHER` count. Releases **no** control.
- **INVALID (UNDECIDABLE)** — exit 0 with `decidable == 0`. Vacuous; carries no pairing evidence.
  Releases **no** control.
- **INVALID (event count)** — otherwise accepted but `< 3` counted events. Not graded on the
  reduced set, not re-run ad hoc. Releases **no** control.

⛔ **Superseded (§5):** *"Exit 3 cannot fire on this campaign and that is structural."* ✅ Exit 3
is **UNEXPECTED under the intended fixture** (`COLLIDING_ANOMALIES = {"corrupted_texture"}`;
every leg fires `blinking`) but **NOT impossible under artifact or configuration drift.**
🚨 **An actual refusal is always honoured as a refusal**, whatever was expected.

## A1.5 — CORRECTS §9's false relative time

⛔ **Superseded (§9):** *"two A-side legs passed this exact predicate on this exact binary and
container **three hours earlier**."* **False.** ✅ The two A-side passes are
`M51D_HE_RESIDENT_NAT` accepted at **19:07:43** and `M51D_HE_CLOSED_NAT` accepted at
**19:19:36** on 2026-09-06. **No relative duration is asserted**, because the elapsed gap to a
future campaign is not known when this is written.

---

**Everything else in this file stands unchanged**: the nine commands and their fixed order, the
`MaxAttempts 4` pose-invalidity budget, the never-retry-a-valid-failure rule, the at-most-ONE
conditional A control followed by STOP, the unchanged predicate and tolerances, the even-frame
blind spot, FPS/`speed_ratio` as telemetry only, and every hold in §10.

**VERDICT: NEEDS-DECISION** — corrected draft, awaiting Codex and Chat review of the exact
snapshot. No leg is released.

---

# AMENDMENT 2 — 2026-09-06, brief 078-12 — THE OBSERVATION-COVERAGE FLOOR

Written after Chat's conditional approval (`_reviews/078-11-chat-prediction-review.md`) and Codex's
disposition of it (`_reviews/078-12-codex-disposition-of-chat-approval.md`), and **before any leg**.
§§0–10 and `AMENDMENT 1` remain as written; where this amendment and anything earlier disagree,
**THIS AMENDMENT GOVERNS.**

⛔ **NO LEG IS RELEASED BY 078-12.** This is documentation only. Execution requires a separate
Codex release brief issued after inspection of this committed text. Nothing was run, staged, built
or cooked to produce it.

## A2.1 — SUPERSEDES the `decidable >= 1` non-vacuity test: the floor is **`decidable >= 20` of 40**

⛔ **Superseded text:** §5's *"plus `decidable >= 1`"* and §9's *"predicted **PASS** — `PREVIOUS 0`,
`NEITHER 0`, `decidable >= 1`"*.

✅ **Governing rule:** a leg is valid only if the checker reports **`decidable >= 20`**, where
`decidable = CURRENT + PREVIOUS + NEITHER` (`same-pos` and `no-data` are excluded by construction,
`m44_pairing_probe.py:232`). Below 20 the leg is **INVALID (insufficient probe coverage)**.

**Why 20, stated as what it is.** Each leg requests **40** captured frames, and the probe steps
once per captured frame, so 40 is the number of *frame opportunities* on which the gate could
distinguish the current render from the previous one. **20 is HALF of that request.** The number is
therefore derived from the **request size**, prospectively — it is a declared minimum on how much of
the leg was actually observed, and it is fixed before any leg runs.

🚨 **What this floor is NOT, said explicitly so it is not read as more than it is:**

- ⛔ **It is NOT a claim of statistical sufficiency.** 20 decidable frames do not make a `PREVIOUS 0`
  reading statistically adequate, and nothing here asserts a confidence level or an error bound.
  It separates *"the probe was actually observed on most of the leg"* from *"the probe was barely
  seen"*. That is all it does.
- ⛔ **It is NOT a tolerance, a band, or any loosening of the predicate.** The pairing predicate is
  **unchanged**: `PREVIOUS == 0 AND NEITHER == 0` over decidable frames.
- ⛔ **It is NOT an FPS or pacing band.** No throughput precondition is adopted (see A2.3).
- ⛔ **IT IS NOT DERIVED FROM THE FAILING BANK, AND MUST NEVER BE DEFENDED THAT WAY.** Choosing a
  floor that happens to sit above the failing legs' coverage, and then citing those legs as its
  justification, would be fitting the rule to the results it is meant to judge. The basis is the
  half-of-request definition above and nothing else.

**Attainability, from receipts actually read.** Two legs on the archived A-side over this frozen
container, at this cap and recipe, were measured in-session with the unchanged checker:
`M51D_HE_RESIDENT_NAT` read **decidable 35** (no-data 5) and `M51D_HE_CLOSED_NAT` read **decidable
37** (no-data 3). ⚠ **No other decidable count is cited here**, because no other was re-verified
against its receipt while writing this amendment; a figure recalled but unchecked is not evidence
(and the earlier campaign's aggregate counts are deliberately not repeated).

## A2.2 — What an under-coverage leg does, and what it still reports

An **INVALID (insufficient probe coverage)** leg:

- **STOPS the campaign for review.** It releases **no** A control, and it is **not** re-run ad hoc.
- 🚨 **STILL HAS ITS OBSERVED COUNTS AND INDICES REPORTED IN FULL** — `CURRENT` / `PREVIOUS` /
  `NEITHER` / `same-pos` / `no-data` plus **every failing frame index listed explicitly**.
  **Insufficient coverage prevents CERTIFICATION; it does not make an observed mismatch disappear**,
  and an under-covered leg that nonetheless shows `PREVIOUS > 0` is reported as exactly that.

⛔ **No retrospective reclassification.** Every historical leg keeps the classification it was given
at the time, including the earlier failing legs recorded at 13 decidable frames. This floor is
**prospective only** and reclassifies nothing in the bank.

**The floor applies to every cell**: the fresh A-side reference (leg 1), all four candidate legs
(2–5), and the single conditional A control.

### The stop taxonomy is now FOUR classes, not three

`AMENDMENT 1 §A1.4` restored three distinct stop classes; this adds a fourth. **None of the four
releases a control, and none is retried:**

| class | condition | note |
|---|---|---|
| **REFUSED** | checker exit 2, 3 or 4 | a refusal to judge; never a pass, fail or `NEITHER` count |
| **INVALID (UNDECIDABLE)** | exit 0 with `decidable == 0` | vacuous; no pairing evidence at all |
| **INVALID (coverage)** | exit 0 with `decidable` from 1 to 19 | 🆕 this amendment; counts still reported |
| **INVALID (event count)** | otherwise accepted, fewer than 3 counted events | not graded on the reduced set |

All other existing validity requirements are **unchanged and still required**: a live probe
(`MASK-PAIRING PROBE SPAWNED tag=255` plus `PROBE STEP` lines, read from the leg's own run log),
intact artifact geometry (40 PNGs and 40 rows, `session_index` 0..39, no duplicate, missing,
zero-byte or orphan), **at least 3** counted events, the A63 / B1 pose policy with `MaxAttempts 4`
as a **pose-invalidity budget only**, and **exactly ONE** conditional A control released only by the
FIRST otherwise-valid candidate pairing failure, **then STOP regardless of its result**.
⛔ **A valid pairing failure is never retried.**

## A2.3 — Same-row reporting of verdict and throughput

Each leg's row — in the report **and** in the journal — carries, **together on one line**: the
validity classification and PAIRING verdict · `decidable` / `CURRENT` / `PREVIOUS` / `NEITHER` /
`same-pos` / `no-data` · **sustained FPS** · **`speed_ratio`**. A reading that is genuinely absent
is printed **`UNKNOWN`** and is **never invented**.

⚠ **FPS and `speed_ratio` remain telemetry.** No pacing band is adopted, none is enforced, and no
historical leg is retroactively invalidated by throughput. The same-row placement exists so that a
pass at low throughput is **visible at a glance** rather than reachable only through a receipt — it
is a legibility requirement, not a gate. (Both quantities were already required by §7; this fixes
*where* they appear.)

## A2.4 — A1.1's attribution rule stands; the reviewer's paraphrase of it is REJECTED

The 078-11 review restates the attribution rule as *"control fails ⇒ not attributable to the
candidate"* and asks that it be kept verbatim. ⛔ **That sentence is NOT adopted.** It is the exact
overclaim `AMENDMENT 1 §A1.1` was written to correct, and adopting it would silently undo that
correction. The review accepted `A1.1`; it misquoted it.

✅ **Governing, unchanged from `A1.1`:** if a candidate leg fails and its A control **also** fails,
then **the failure occurs WITHOUT `F1`** — these observations **do not isolate an `F1` contribution
and do not exclude one**. If the control **passes**, a candidate contribution is **consistent with
the observation**, but **time and machine state remain confounded** and the reading is not isolated.
🚨 **Neither branch is causal proof**, and neither establishes environment, FIFO ordering,
render-thread-ahead arming, or a client cause.

## A2.5 — The container is a FIVE-FILE QUINTET

The 078-11 review refers to a *"container quartet"*. The correct count is **five**, and **§1 of this
document was already correct** — the slip is in the paraphrase, not in the record. The frozen Phase B
container is:

`StackOBot-Windows.utoc` · `StackOBot-Windows.ucas` · `StackOBot-Windows.pak` · `global.ucas` ·
`global.utoc` — full SHA-256 for all five in §1.

## A2.6 — Binary-swap and restoration receipts, carried as explicit policy

Leg 1 and the conditional control run on the archived A-side `B8AC65D8…EAD8`; legs 2–5 run on the
archived candidate `D50DDE78…4796`. That means swaps between legs, so the following is **policy for
the execution brief and is stated here rather than left implicit** (§7 already requires the hashes;
§8 already requires restoration — this fixes them as obligations rather than expectations):

- **Before every leg:** hash the staged executable and require it to equal **the archive that leg
  declares**. A mismatch stops the leg.
- **All five container files hashed BEFORE and AFTER every swap and every leg**, and both receipts
  **banked**.
- **On every exit path — success, failure, refusal or abort — restore the staged executable to the
  candidate `D50DDE78…4796` and re-verify its hash.**
- ⛔ **No rebuild, no cook, and no binary other than the two named archives.** Staging remains
  prohibited under 078-12 and is released only by the execution brief.

## A2.7 — What a PASS would mean, stated no more broadly than it is

A PASS across these legs is **scoped pairing smoke on these frozen bytes and these recipes**, read
through a probe with a **declared even-frame blind spot** (§0: a slip of an even number of frames
maps position onto itself and is invisible to this gate). ⛔ **It is NOT full `F1` or `m51` release
certification**, not an all-gates claim, not a Lyra pass, and it **does not erase the failure bank**.
Broader phrasing along the lines of *"a PASS certifies F1"* is **not adopted**.

⛔ **Everything in §10 remains HELD** — production source and shaders, fixtures, harness and checker
source, schema, observability meanings, identity / lifecycle / geometry / tag / writer / control-flow
changes, all levers and new probes. `M-F1b` remains **UNRUN**; the Lyra arm is **not waived**; no
push, merge or tag.

---

**Unchanged by this amendment:** the nine capture commands and their fixed order · the
`MaxAttempts 4` pose-invalidity budget · never retrying a valid failure · the at-most-ONE conditional
A control followed by STOP · the **unchanged** primary predicate and tolerances · the at-least-3
counted events requirement · the even-frame blind spot · every hold in §10.

**VERDICT: NEEDS-DECISION** — this amendment implements Chat's condition and Codex's disposition,
and **releases nothing**. Execution awaits the separate Codex release brief.
