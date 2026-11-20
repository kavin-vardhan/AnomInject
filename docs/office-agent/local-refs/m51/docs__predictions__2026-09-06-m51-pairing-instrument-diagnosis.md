# 078-06 — H-instrument: is the MASK-PICTURE-PAIRING failure recipe drift or runtime state?

**Written and committed BEFORE any leg of this brief ran.** This is the executable form of the
H-instrument arm approved in the 078-05 chat review §3. It is NOT a rewrite of any past prediction;
`53d42c2`, `09c636f` and `0cb7644` stand unchanged.

⛔ **Production source is FROZEN at `6713f51`** (`git diff 6713f51 -- Source Shaders` is EMPTY).
No build, cook, source/shader/fixture/gate-tool change, Lyra or office action, push, merge or tag.
⛔ **H-editor and H-load are HELD** — neither is released here, and no owner action is requested.

---

## 0. What §1 established BEFORE this record was written

**(a) The checker and runner are byte-unchanged and are EXCLUDED as drift candidates.**
`run_leg.ps1` (`9E8D669F…`, mtime 03-09), `m44_pairing_probe.py` (`918EB4B5…`, mtime 03-09) and
`m49b_campaign.ps1` (`0D258AB9…`, mtime 04-09) all predate the morning run; CaptureBench is at
`472a409` with tracked files clean. The CURRENT checker re-run offline on the immutable banks
reproduces **both** eras exactly — morning `PREVIOUS 0` ×4, afternoon `PREVIOUS 9/10/9`. The
required stop-gate ("if the morning output no longer reproduces, stop") therefore **did not fire**.

**(b) There IS one material executed recipe difference, and `_leg_geometry.json` is blind to it.**
The morning campaign passes no `-Marker`, so it ran at the runner's **default `Marker = 1`**; the
078-05 pairing legs passed **`-Marker 0`**. `run_leg.ps1:214` issues `CaptureBench.Marker $Marker`
into ExecCmds, and the geometry JSON records no `marker` field — which is why the 078-05 `G184`
payload diff read "identical on every axis". **Confirmed from the artifact, not the script:** an 8×8
grid of consecutive-frame luminance difference matches to ~3 decimals everywhere (146.617 vs 146.626,
55.227 vs 55.229) **except the top strip**, where the morning reads **45.797 / 39.998** and the
afternoon reads **0.286 / 0.668** — `G125`'s marker signature exactly.
Also differing but NOT material to the executed leg: `MaxAttempts` 4 (morning) vs 3 (078-05 default).
Both legs were accepted on attempt 1, so the retry budget was never consumed.

**(c) There is a second, larger difference which is RUNTIME STATE, not recipe.**
`speed_ratio` reads **1.0040 / 1.0004 / 1.0000 / 1.0003** on the four morning pairing legs and
**3.0001 / 2.9677 / 1.5933 / 1.6669** on the four 078-05 pairing legs, with the two concurrent
controls at **3.5351** and **3.1823**. Sustained wall fps falls from ~29.9 to 8.5–10.0. Across all
eleven pairing legs measured, every leg at ratio ≈ 1.00 passes and every leg at ratio ≥ 1.59 fails.
⚠ **That is an ASSOCIATION over legs that are confounded on time, marker and throughput together.
It is NOT a cause, and this brief does not attempt to establish one.**

**(d) Editor residency alone is already insufficient, measured.** Owner `UnrealEditor` PID 25324
started **11:33:30**; the morning pairing legs ran at **10:18**, before it existed. But the 078-05
`M51_F1A_*` legs ran at **17:00–17:04 with it resident and read ratio 1.0002**. So residency does not
by itself produce the loaded regime. ⛔ This does not refute `G97` as a contributor; it refutes only
the simplest form of it. H-editor stays HELD and unjudged.

## 1. What this experiment CAN and CANNOT separate — stated before it runs

✅ **It separates the MARKER/recipe axis**, because both legs run in the same session, minutes apart,
on the same archived A-side bytes, with the same container and the same owner state.
⛔ **It does NOT separate the THROUGHPUT axis.** No load lever is released, so whatever regime the box
is in is simply *observed* on each leg. `speed_ratio` is therefore a **recorded reading on every leg,
never a gate and never a knob.**
⛔ It cannot establish F4, the FIFO mechanism, render/request identity, or the client's cause.

## 2. Recipes — the recovered morning payload, and the 078-05 payload

Both legs: archived A-side **`B8AC65D87030D311632A4605C8EFF5C6D54A1ACCB23BE76682C6E643498EEAD8`**
(241,400,320 B) staged against the frozen Phase B container
`67EA1FE0` / `2CEFB8F4` / `E03C6610` + `A16A18A8` / `C70ECDAA`, hashed before and after.
Native 1280×720, `blinking` on `StaticMeshActor_49`, seed 4242, cap 40, `Sve 1`, `Delivery 0`,
`Pace 1`, `AutoExposure 0` (the default both eras issued), natural tick order, probe ON.

- **`M51D_HI_OLD_NAT`** — the recovered morning `F_PAIR_NAT` payload:
  `-Marker 1 -MaxAttempts 4`, `-ExtraExecCmds "IAI.Bench.MaskPairingProbe 1"`.
- **`M51D_HI_NEW_NAT`** — the 078-05 payload: `-Marker 0` (MaxAttempts at its default 3),
  `-ExtraExecCmds "IAI.Bench.MaskPairingProbe 1"`.

New unique labels and a new bank prefix (`M51D_`); **no historical bank is overwritten.**
Order: **OLD first, then NEW.**

**UNKNOWN, and recorded as UNKNOWN rather than assumed:** the machine's full concurrent process set
and GPU state at 10:18 were never recorded, so the morning throughput regime cannot be reconstructed
— only its *effect* (`speed_ratio` 1.0040) is banked. `WaitSec` and `VerboseAnomalyLog` are not
recorded in any artifact; both were at defaults in both eras by inspection of the invoking scripts.

## 3. Pose-validity policy, unchanged

The runner's existing A63/B1 policy governs. B1 is LIVE on both legs (native resolution, target is
`StaticMeshActor_49`). **An invalid-pose attempt is discarded and re-run by the runner and is NOT a
checker result**; a leg that is accepted and then fails the pairing predicate is a **valid failure and
is NEVER retried**. Attempt caps: OLD 4, NEW 3, exactly as each era ran. ⛔ No runner edit, no lever,
no tolerance change, no re-run-until-pass.

## 4. Discriminating outcomes — declared, with no invented numbers

The predicate is unchanged: **`NEITHER == 0` AND `PREVIOUS == 0`** over decidable frames.

- **D1 — both legs FAIL.** The failure reproduces on the A-side today under BOTH recipes ⇒ the
  specific marker/recipe-drift explanation is **unsupported**; the difference lies elsewhere.
  ⛔ This does **not** exonerate the instrument in general and does **not** establish an
  environmental cause. Root cause remains OPEN.
- **D2 — both legs PASS.** The failure is not reproduced in either cell today. ⛔ An isolated
  recovery certifies nothing; it makes the afternoon regime the thing to characterise, and the
  contrast with 078-05's readings is reported with both `speed_ratio` values.
- **D3 — OLD passes, NEW fails.** An association with the marker axis **in this bounded pair only**.
  Stop, report the differing tokens, propose the next discriminator. ⛔ Not causal isolation.
- **D4 — OLD fails, NEW passes.** Reported verbatim as an unexpected inversion; stop.
- **Any leg that cannot be run as predeclared** ⇒ **UNRUNNABLE**, reported as such, never as a pass.

⛔ **No outcome here licenses "environment proved", "instrument exonerated", "F4 established", or any
statement about the client's cause.**

## 5. Stop condition — hard

After `M51D_HI_OLD_NAT` and `M51D_HI_NEW_NAT`, **STOP**. No candidate smoke, no editor-close arm, no
controlled load, no synthetic extension, no additional control legs, no further campaign. The 078-05
allowance for one interleaved A-side control does **not** apply: this entire experiment is already
A-side. The staged candidate `D50DDE78` is restored and hash-verified at completion **even if a
diagnostic leg fails**.