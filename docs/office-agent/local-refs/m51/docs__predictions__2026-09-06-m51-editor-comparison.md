# 078-08 - H-editor comparison: RESIDENT vs CLOSED

**Written and committed BEFORE the RESIDENT leg runs.** Implements the reviewed plan
`_reviews/078-08-editor-comparison-plan.md` (outside the repo). Chat approved the owner-coordinated
diagnostic pair in the 078-06 review; **implementation of any `m51` finding remains blocked.**

## 0. What this pair is, and what it can never be

The 078-05 MASK-PICTURE-PAIRING failure (`PREVIOUS` 9/12/10/17 against a banked baseline of 0) was
shown by 078-06 to occur on the **pre-fix A-side binary too**, so it is not attributable to `F1`.
078-06 also excluded tool drift (runner + checker byte-unchanged, offline re-run reproduces both
eras) and recipe drift (`Marker 0` vs `Marker 1` moved the verdict by nothing: `M51D_HI_OLD_NAT`
and `M51D_HI_NEW_NAT` both read CURRENT 4 / PREVIOUS 9 / NEITHER 0). **Root cause remains OPEN.**

This pair asks exactly one question: **does the reading change when the owner's resident Unreal
Editor is closed?**

- It **cannot** certify `m51`, and does not.
- It **cannot** establish a FIFO, F4 or client-side cause.
- It **cannot** license a permanent ban on editors, or any new FPS / `speed_ratio` validity
  threshold. `G120` applies: an association is not a mechanism.
- Two legs is **diagnostic**, never a certification set, whatever the event count.

## 1. The confound this pair inherits, stated up front

078-06 already measured that **editor residency alone is insufficient**: the 17:00-17:04 `F1A` legs
ran **with** PID 25324 resident and read `speed_ratio 1.0002`. The association 078-06 actually
found was with **render cost** - every pairing leg at `speed_ratio ~1.00` passed (4, all morning)
and every leg at `>= 1.59` failed (5) - and it is **confounded on time, marker and throughput**.

Further, **PID 25324 exited; the resident workload is a RESTARTED editor (PID 20300)**, opened at
18:59:43 today. This is **not continuity of the original workload**: its project, scene, viewport
state and per-interval activity are different-or-unknown. **A negative result therefore does not
refute the original association**, and a positive one is association, not mechanism. Recorded as a
limitation, not discovered later.

## 2. Identity, verified before this record was committed

| Item | Value |
|---|---|
| Repo / branch / HEAD | `AnomalyInjector` / `m51` / `8dc708c` |
| `master` == `origin/master` | `ac13700` (unchanged) |
| Production source freeze | `git diff 6713f51..HEAD -- Source Shaders` **EMPTY**; worktree clean |
| Untracked owner handoffs | 10, preserved |
| CaptureBench | `472a409`, no tracked modifications |
| `run_leg.ps1` | 26,334 B, mtime 2026-09-03 18:24, sha256 `9E8D669F1C19...` |
| `m44_pairing_probe.py` | 10,684 B, mtime 2026-09-03 21:46, sha256 `918EB4B5888A...` |

**Binaries (full SHA-256, read from the files):**

- A-side archive `_binary_baselines\StackOBot.exe.m49b-gate-B8AC65D8`, 241,400,320 B
  `B8AC65D87030D311632A4605C8EFF5C6D54A1ACCB23BE76682C6E643498EEAD8`
- Candidate archive `_binary_baselines\StackOBot.exe.m51-f1-candidate-D50DDE78`, 241,401,856 B
  `D50DDE789A94933182DC6B3FA74811F8BB57DC1F8332B4AC88ACE47337BD4796`
- Staged runtime **at predict time == candidate `D50DDE78`**. A is staged for the leg and the
  candidate is **restored and re-hashed before the final report, including on failure.**

**Frozen Phase B container (full SHA-256, `Builds\BenchGate\Windows\StackOBot\Content\Paks`):**

| File | Bytes | SHA-256 |
|---|---|---|
| `StackOBot-Windows.utoc` | 294,235 | `67EA1FE0A15ACF2429516C461C5FD85989285B3C93B24C42855AE4959386257A` |
| `StackOBot-Windows.ucas` | 364,596,784 | `2CEFB8F43D048E2534471BC1D9532B77A870FAF46F14C9C2AFAE813B99B6C4D3` |
| `StackOBot-Windows.pak` | 10,115,711 | `E03C661084D8BC2BB6EF0F51F2DECB162853B2188B733B1DBD22B96CA974CDB7` |
| `global.ucas` | 1,833,008 | `A16A18A8FFB5151D2E850BB52841162F6844A0361818F8D1F158E8377DFFBA9E` |
| `global.utoc` | 539 | `C70ECDAA232DB8CF77BEEAB47B7226C48FCC91FF705526D507500CD14CF21C51` |

All five prefixes match the frozen quintet. **No cook, no build, no rebuild.**

**Resident workload (the owner's own unrelated project - never touched, inspected or copied):**

- PID **20300**, `UnrealEditor.exe`, `E:\EpicGames\UE_5.7\Engine\Binaries\Win64\UnrealEditor.exe`
- Start **2026-09-06 18:59:43**; at predict time ~315 s CPU, ~3,979 MB working set.
- It is the **only** editor process on the box.

## 3. Blast radius, checked structurally rather than assumed

`run_leg.ps1:195,319` kills `Get-Process -Name "StackOBot*"`. The owner's process is
`UnrealEditor.exe` and **cannot match that pattern**, so runner cleanup cannot reach it. `:201`
and `:492` delete only `<outRoot>\<Label>_try<N>` and `<bank>\<BankPrefix><Label>`. Bank names
`M51D_HE_RESIDENT_NAT`, `M51D_HE_CLOSED_NAT` and every `_tryN` sibling were verified **free**, so
nothing existing is overwritten. At predict time there were **zero** `StackOBot*` processes.

## 4. The commands, exact

Both halves are the recovered morning recipe and differ **only** in label and editor presence.

```
# RESIDENT - THIS BRIEF
powershell -File D:\IntrusiveAnomalies\StackOBot\Plugins\CaptureBench\tools\run_leg.ps1 `
  -Label HE_RESIDENT_NAT -BankPrefix M51D_ -Anomaly blinking -Target StaticMeshActor_49 `
  -Seed 4242 -MaxFrames 40 -Sve 1 -Delivery 0 -Pace 1 -Marker 1 -AutoExposure 0 `
  -MaxAttempts 4 -ExtraExecCmds "IAI.Bench.MaskPairingProbe 1"

# CLOSED - NOT RUN IN THIS BRIEF. Awaits Codex reading the RESIDENT report and the owner
# saving/closing/confirming. Not run autonomously, on any timeout or any signal.
  ... -Label HE_CLOSED_NAT ...   (every other argument byte-identical)
```

**Unchanged runner defaults, resolved and pinned by not passing them:** `-Map
/Game/CaptureBenchGate/CB_GateLevel`, `-ResX 1280 -ResY 720`, windowed (native, **not**
`OutputHeight`-filtered), `-StallGameMs 0` (**natural tick order** - `SynthTickOrder` is never
issued), `-WaitSec 150`, no `-RequireModalRotZero`, no `-LetterboxedFixture`, no
`-VerboseAnomalyLog`. Mask, target mask and census are **not** issued as cvars and therefore run at
their **compiled shipping defaults**; the effective values are read from the run banner (`A48`),
never from the typed command. Runner-fixed: `IAI.Capture.Fps 30`, `IAI.Capture.Config 2 4 8 4 0`.

## 5. What will be recorded

Run banner / effective config echo; probe spawn and step lines; image dimensions; event count; row
and image index ranges; **CURRENT / PREVIOUS / NEITHER / same-pos / no-data** from the unchanged
`m44_pairing_probe.py <session-path> <label>`; sustained FPS; `speed_ratio`;
`ticks_per_captured_frame`; `target_mask_frames_measured / _hidden_blank / _unavailable`; and the
process/binary/container receipts above, re-verified immediately before the accepted attempt.

**Exit 0 from the checker means the leg was READ, not that it PASSED.** The declared predicate is
`NEITHER == 0 AND PREVIOUS == 0` over decidable frames. Exit 2/3/4 is a **refusal**, and a refusal
is recorded as a refusal - never as a pass, a fail or a `NEITHER` count.

If the namespace-aware batching counts are wanted, a **COPY** of `read_batched_arms.py` is
parameterised inside the new evidence directory. The 078-07 campaign is not re-run and no shipping
tool is edited. Target IDs are bit61, census IDs bit62; **raw `servedArms >= 2` is not by itself a
batching defect.**

## 6. Predictions

**Primary expectation (RESIDENT):** the leg **fails** the pairing predicate with `PREVIOUS > 0`,
reproducing the afternoon era, **and** reads `speed_ratio >= ~1.5` with sustained FPS well under
30. This follows 078-06's association, which tracked render cost rather than the calendar.

**Explicitly held open, and it is not a hedge:** a RESIDENT **pass** at `speed_ratio ~1.00` is a
live outcome, because 078-06 already measured `F1A` passing at ratio 1.0002 with an editor
resident. A pass would say the restarted editor does not load this box the way the original did,
and would make the CLOSED half's discriminating power near zero - which is itself worth reporting.

**Recorded but never converted into a rule:** whatever `speed_ratio` this leg reads, it is a
reading. **No threshold is minted, no tolerance is widened, no existing gate is re-calibrated.**

## 7. Predeclared outcome categories (fixed before the leg)

| RESIDENT | CLOSED (later) | Reading |
|---|---|---|
| fail | fail | Closing the editor did **not** restore this pair. |
| fail | pass | State-associated recovery **deserving replication**. Not proof of mechanism. |
| pass | pass | The current failure is **not reproduced**; the pair discriminates nothing. |
| pass | fail | **Inversion - report it**, do not explain it away. |

## 8. Stop and retry policy

`MaxAttempts 4` is the **pose-retry budget only** (B1 / `A47`). **A valid pairing failure is NEVER
retried** - re-running a leg until it passes is the laundering shape this project forbids. Any
identity, config or precondition mismatch **stops the phase** without substituting a workload. No
extra control, synthetic extension, load injection or candidate smoke is released by this record.

## 9. Verdict rule, fixed in advance

Because CLOSED is **deliberately unrun** pending owner action, this brief's verdict is
**NEEDS-DECISION** - **even if RESIDENT passes.** A half-pair is not a result.