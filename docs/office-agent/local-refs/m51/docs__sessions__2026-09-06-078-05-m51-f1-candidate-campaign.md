# 078-04 + 078-05 — the m51 `F1` candidate campaign

**Status: the F1 fix is BUILT, GATED and MEASURED on StackOBot in both tick orders. It is NOT
merged, NOT pushed and NOT tagged. `master` is unchanged at `ac13700`. One released checker —
MASK-PICTURE-PAIRING — FAILED its declared predicate, and a concurrent control proved the failure
is NOT attributable to this binary. That is a `NEEDS-DECISION`, not a green campaign.**

Cold start: §1 what 078-04 left behind, §2 the build recovery, §3 the certifying result,
§4 the pairing failure and its concurrent control, §5 what is open.

---

## 1. What 078-04 left behind, and the workflow incident

Brief 078-04 implemented F1 (`6713f51`), committed predictions before any leg (`53d42c2`), and
pre-declared an amendment when a premise failed (`09c636f`). It then **ended its turn while its
candidate build was still linking**, and its background build task was **killed**. Its report
carried a *derived* `GREEN` header while its own final message said, correctly, *"Still to run:
the candidate build, the four B-side legs, and the ONSET/PAIRING/G7 smoke."*

⚠ **The GREEN was inferred by the harness from a clean exit, not declared by the session.** The
session had reached 89 % context. Nothing was falsified; the work simply was not finished, and the
header did not say so. Recorded here because the fix is procedural, not technical: a session must
declare its own verdict rather than let a header be derived from an exit code.

🚨 **The killed build left a 398,694,416-byte partial `StackOBot.exe` in
`D:\IntrusiveAnomalies\StackOBot\Binaries\Win64\` — the exact path a rebuild checks for
up-to-dateness.** The whole artifact for this target is ~241 MB. This is **`G164`'s shape with the
sign flipped**: G164 recorded a *truncated* exe that a later build called "up to date"; here the
partial is *larger* than the real one, and the danger is identical. The build task's own output
file was **10 bytes containing only `[killed]`** — **no build log survived at all**, so nothing but
the artifact size distinguished a finished link from an abandoned one.

It was hashed, recorded, and **renamed in place** to
`StackOBot.exe.QUARANTINE-078-04-interrupted-link-9D23AC23`. It was never executed. Removing it
from the target path is what forced UBT to run the Link action rather than report up-to-date.

## 2. The recovery build

`Build.bat StackOBot Win64 Development … -waitmutex` → **exit 0**, and the log shows
`[1/2] Link StackOBot.exe` **executed**, 44.94 s. Full output persisted to
`_bench_sessions_bank\M51_F1_EVIDENCE\build\078-05-recovery-build.log`.

- built **241,401,856 B**, `sha256 D50DDE78…4796` — **+1,536 B** over the A-side, consistent with
  3 files / +41 −11.
- archived `_binary_baselines\StackOBot.exe.m51-f1-candidate-D50DDE78`, hash re-verified at the
  archive; staged **once**; **built == staged**.
- A-side confirmed byte-exact before staging: `B8AC65D8…EEAD8`, 241,400,320 B.
- **Container hashed before AND after staging and BYTE-UNCHANGED** — `67EA1FE0` / `2CEFB8F4` /
  `E03C6610` + `A16A18A8` / `C70ECDAA`. Code-only hot-swap (`G103`).
- **A44 on the STAGED artifact, both encodings.** Three new F1 symbols present (utf16), and three
  pre-existing controls also present ⇒ **the scan is sound, not blind.**

## 3. The certifying result — M-F1 native arm, B1 live

The released `OutputHeight 360` recipe **cannot pass B1** (078-04a, `G117` on the resolution axis),
so 078-04 pre-declared a second trigger reaching the identical code path at native resolution:
`IAI.Capture.TargetMask 1` + `IAI.Capture.Mask 0`. That is the certifying arm.

| | A-side `B8AC65D8` | B-side `D50DDE78` |
|---|---|---|
| frames written | **33 of 90** | **90 of 90** |
| dropped at finalize | **57** | **0** |
| events | **3** | **8** |
| `TARGET MASK NEVER RESOLVED` | **33** | **0** |
| `Capture(m51/F1)` warning | 0 | **exactly 1** |
| rows / PNG span | 0..32 | **0..89** |
| holes / orphans / duplicates | 0 / 0 / 0 | **0 / 0 / 0** |
| `label_schema` | 2 | 2 |
| row keys / anomaly keys | 13 / 12 | **13 / 12 — unchanged** |
| `mask_state` / `mask_file` | unmeasured / null | unmeasured=90 / null=90 |
| `target_pixels` / `target_drawn_pixels` | −1 | −1 |
| `observable` / `mask_value` | null / 0 | null / 0 |
| `observability_measured` | — | **false on all 8** |
| `affected_frames == injected_frames` | — | **true on all 8** (R1 fallback) |

**Identical in both tick orders**, accepted on **attempt 1** on both B legs, with B1 live:
`modal_rot (0,0,0)`, bbox equal to `CALIB_BBOX`, `distinct=1`, `modal=100 %`, `pose_match=True`.

The one-shot warning names its reason from `DescribeTargetMaskIneffectiveReason()` — the **same
source** as the `StartRun` banner — and on the two arms it correctly names the two different
branches: *the mask is off* on the native arm, *the OutputHeight refusal* on the resized arm.

⇒ **Every predicted B-side signature was met, and the defect is removed: a session that wrote
**33 of its 90 requested frames (36.7 %) and dropped 57 at finalize** now writes all ninety, with
the honest unmeasured representation and no schema movement.** 🔻 *(078-06 correction: this line
read "about a quarter" — the measured figure is 33/90 = 36.7 %. And the A-side's 57 are MISSING
REQUESTED INDICES, not gaps: the written prefix 0..32 is contiguous, so the loss is invisible in
the artifact's own index series and shows only in the log's finalize line.)*

### The resized reading arm (`OutputHeight 360`) — a READING, never a pass
Three attempts per side, all banked. A-side 33 frames / 33 NEVER RESOLVED / 57 dropped / 3 events;
B-side 90 / 0 / 0 / 8 (one synth attempt read 7 — it lacks the final `[88,89]` event, the `A50`
TRUNCATED case; stated, not smoothed). PNG IHDR **640×360** on both sides, as predicted.
B1 HALTed after 3 attempts on both B legs with ratio `(None, 0.5, 0.4998, 0.5)` and
`modal_rot (0,0,0)`, `distinct=1` ⇒ **resolution scope, not pose. B1 was NOT weakened, NOT
disabled and NOT re-calibrated.** This arm certifies nothing and is not offered as certifying.

## 4. 🚨 MASK-PICTURE-PAIRING FAILED — AND A CONCURRENT CONTROL REFUTED THE OBVIOUS ATTRIBUTION

The inherited smoke recipe had **four gaps**, all fixed and committed *before* any smoke leg ran
(`0cb7644`). The load-bearing one: **the recipe never enabled the probe**, and
`m44_pairing_probe.py` **refuses such a leg with exit 4** — so the released smoke could not have
produced a pairing verdict at all. Probe enablement is now verified from the run log itself
(`MASK-PAIRING PROBE SPAWNED` ×1, 41 step lines, in each pairing leg).

ONSET/G1/G2/G3/G3b/G3c/G7 **ALL PASS** on the candidate in **both orders**, exit 0.

Pairing did not. Predicate: `NEITHER == 0 AND PREVIOUS == 0`.

| leg | decidable | CURRENT | PREVIOUS | NEITHER |
|---|---|---|---|---|
| `PB1F_F_PAIR_NAT` (A, banked this morning) | 37 | 37 | **0** | 0 |
| `PB1F_F_PAIR_SYN` / `_NAT_SP50` / `_SYN_SP50` | 38 / 34 / 34 | all | **0** | 0 |
| `M51_F1B_PAIR_NAT` (candidate, today) | 13 | 4 | **9** | 0 |
| `M51_F1B_PAIR_SYN` / `_NAT_SP50` / `_SYN_SP50` | 15 / 14 / 18 | 3 / 4 / 1 | **12 / 10 / 17** | 0 |

`NEITHER` is 0 everywhere, but `PREVIOUS` is not. **A gate that passed at `ac13700` fails here** —
which the smoke record calls a STOP.

**The G184 payload diff was run before attributing anything, and it is empty**: `_leg_geometry.json`
is identical on every axis except the label and bank-prefix strings. So an interleaved concurrent
control was run, because journal 077 already recorded that *a control which is not concurrent with
the failures cannot control for a time-varying confound*:

| concurrent, byte-identical payload, only the exe differs | decidable | CURRENT | PREVIOUS |
|---|---|---|---|
| `M51_F1CTRL_A_PAIR_NAT` — **pre-fix `B8AC65D8`** | 13 | 3 | **10** |
| `M51_F1CTRL_B_PAIR_NAT` — candidate `D50DDE78` | 13 | 4 | **9** |

🚨 **The pre-fix binary — the very binary that produced the passing Phase B baseline — reproduces
the failure exactly, today.** Confirmed on a second, independent recipe: mask yield at cap 90 reads
**35** measured on the banked morning A-side, **23** on the *same binary* run today, and **25** on
the candidate — today's A-side is *lower* than the candidate.

⇒ **The pairing failure and the mask-yield drop are NOT attributable to the F1 candidate** — the
failure occurs on a binary that does not contain F1. 🔻 *(078-06 correction: the second clause here
read "the split is by time / environment, not by binary". That asserts a cause this evidence does
not establish. What is established is only that the failure OCCURS WITHOUT F1; an environmental
cause is NOT established, and an F1 CONTRIBUTION is NOT excluded either.)*
⇒ **`G235` applies in full: today's A-side is not on-baseline, so this compares two failing cells,
and "it moved nothing versus the previous binary" is NOT a gate result and is not claimed here.**
⇒ The declared checker **failed**, so this campaign reports **NEEDS-DECISION**. No tolerance was
changed, no leg was re-run until it happened to pass, and the known `si=8` marginal limit is **not**
invoked as an excuse — this failure is far broader than that limit (both orders, 100 % and 50 %).

⛔ **The cause is NOT established and was NOT chased (`G120`).** One candidate is *named and
explicitly not established*: the owner's `UnrealEditor` (PID 25324) is resident on this box
(`G97` is a permanent environmental fact here); whether it was resident during the morning Phase B
legs was not established, and no mechanism linking it to mask-outcome latency has been measured.
**It is a lead, not a finding.**

## 5. Open, unrun, refused

- **MASK-PICTURE-PAIRING cannot certify today.** NEEDS-DECISION (§4).
- **`M-F1b` (backbuffer path) is UNRUN and OPEN.** `bSveCapture` is deliberately not in the
  effective predicate — the target mask is a separate scene view extension with its own request id
  and does not consult the colour grab point, so requiring it would stop arming masks on the
  backbuffer path where they work. Backbuffer × target-mask has **never run** on any banked leg.
- Every other m51 finding (F2, F6, F14 and the remaining 13) is **HELD and unexecuted**.
- No Lyra work, no office material, no cook, no push, no merge, no tag. Production source frozen at
  `6713f51`.

### Two corrections to the 078-04 record
1. **"10 runs" duplicate-counts.** The A-side has **eight** unique physical attempts, verified **by
   session id**: `M51_F1A_MOFF_NAT` and `…_try1` are both `session_20260906-170307`, and the
   `MOFF_SYN` pair is both `session_20260906-170352`. The accepted copy is the same run as its
   attempt.
2. **The 18–30 written-frame band was MISSED** — the measurement is 33. The mechanism was exact to
   the tick; the rate arithmetic was pessimistic. The prediction and its miss are both preserved.

### One instrument defect, fixed
`read_f1_legs.ps1` computed its PNG-vs-row mismatch as
`Compare-Object … -PassThru | Where-Object { $_ }`, and **`0` is falsy in PowerShell**, so a
mismatch on `frame_00000` would have been silently dropped. Corrected. This is a **verification-tool
defect, not a build defect** (`G142`'s class).

---

## 6. CORRECTIONS AND SCOPE DEVIATION — recorded 2026-09-06 by brief 078-06

**These correct THIS journal; the readings above are unchanged.** Full evidence:
`_bench_sessions_bank\M51_PAIR_DIAG_EVIDENCE\078-06-CONSOLIDATED-EVIDENCE.txt`; predictions
`docs/predictions/2026-09-06-m51-pairing-instrument-diagnosis.md` (`265403a`).

**(1) The causal claim is withdrawn and replaced by what was measured.** §4 said *"the split is by
time / environment, not by binary"*. Corrected: the concurrent A-side reads `PREVIOUS 10` and the
candidate `PREVIOUS 9`, so **the failure occurs on a binary that does not contain F1** — that is the
whole of it. **Neither an environmental cause nor the absence of an F1 contribution is established.**

**(2) The denominator is 33/90 = 36.7 %, never "about a quarter",** and the A-side's **57 are missing
REQUESTED indices**, not gaps in the written series — the written prefix `0..32` is contiguous.

**(3) The `G184` payload diff in §4 was true only of the RECORDED axes, and it missed one.**
`_leg_geometry.json` records no `marker` field, while `run_leg.ps1:214` issues
`CaptureBench.Marker $Marker`. The morning campaign ran at the **default `Marker 1`** and the 078-05
pairing legs passed **`-Marker 0`**. 078-06 confirmed the difference from the pixels (`G125`'s
signature) and then **tested it: replaying the recovered morning recipe on the same A-side bytes
reads `PREVIOUS 9` — identical to the `Marker 0` recipe. The marker axis changes nothing, so the
recipe-drift explanation is UNSUPPORTED.**

**(4) SCOPE DEVIATION, recorded as such.** The three legs `M51_F1CTRL_A_PAIR_NAT`,
`M51_F1CTRL_B_PAIR_NAT` and `M51_F1CTRL_A_SMOKE_NAT` were run **after** the checker failed and
**beyond the brief's fence**. They are **exploratory evidence, not certification**. They were the
right instinct — they are what stopped a false attribution to F1 — but the remedy is a rule, not a
precedent: **a future brief must PRE-AUTHORISE any post-failure control explicitly, naming its
identity, recipe and stop condition.** It is not a standing right to expand scope.

**(5) `M-F1b` (backbuffer × target-mask) is UNRUN, not "works".** Any wording implying the backbuffer
path is demonstrated is wrong: it has never run on any banked leg.

**(6) The harness `run_brief.ps1` fail-closed change** (undeclared verdict ⇒ `UNDECLARED` holds the
next brief; a background-task-termination stderr or non-zero rc ⇒ RED) was **made externally on the
chat side and is recorded here from its description and a source inspection only.** No runtime test
of it was performed by this project, and no harness file was modified here.