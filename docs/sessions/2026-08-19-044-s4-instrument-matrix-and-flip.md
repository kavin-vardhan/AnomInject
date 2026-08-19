# 2026-08-19 — 044 — S4: the instrument, the resolution matrix, and the default flip

**Plugin:** `AnomalyInjector` — S4-0 instrument, S4-2 flip.
**Bench:** `CaptureBench` — harness geometry, the resolution reader, label honesty. Local-only.
**Base:** `a4b0755` (tag `m24` → `8373b76`).

**S4 demotes the backbuffer to the UI-on option. The default grab point becomes the SVE / scene-colour
path, which is UI-free by construction — so this changes DELIVERED IMAGE CONTENT and is planned as a
client-visible change, not a silent default flip.**

---

## 1. Stages and what each one was for

| stage | content | gate |
|---|---|---|
| **S4-0** | the 3-rect instrument + grab point in the startup banner. **Log and harness only** | the line prints all three rects on BOTH paths and reproduces S3a's `dW=0 dH=0` |
| **S4-1** | the resolution matrix — ten legs | the CASE A/B/C/D decision table, restated verbatim before any result was read |
| **S4-2** | **the flip** — one line | four sub-gates (a)–(d), see §6 |

**S4-3/S4-4/S4-5 are NOT in this journal.** They were not run.

---

## 2. S4-0 — the instrument

The `RESOLUTION DELTA` log already compared the grabbed rect against the Slate window rect. Two gaps
closed, both **log-only**:

- it did not print `GetViewportSize()` — **the third rect, and the one that feeds
  `annotation.video.resolution` and `run.json` viewport**, therefore the only one that can silently
  disagree with the delivered pixels. New quantity: `dW_vp` / `dH_vp`.
- it was gated on the SVE path, so it could not characterise the backbuffer. Un-gated; one instrument
  now covers both grab points. `bSveRectLogged` → `bRectDeltaLogged`.

The `Initialize` banner reported delivery mode, content clock and focus gate but **not the grab point**.
After the flip that is the line a client build's startup log must carry, so it now names the grab point
and its UI implication.

**`resolution_delta.py` is NEW and COMMITTED** (**G106** — an instrument that a certified result depends
on is a committed artifact). It reads four rects: the PNG from its **IHDR chunk** (ground truth — the
only source that cannot be wrong about itself), `labels.jsonl`, `annotation`, `run.json`. It **grades
nothing**, and says so in its own output, because *a reporting delta and an alignment failure are
different things and conflating them is how a Case B becomes a Case C*.

> **VALIDATED IN BOTH DIRECTIONS BEFORE IT GRADED ANYTHING** (G96/G105 — a metric that has only ever
> returned "pass" is not an instrument). Known-answer: three banked sessions (backbuffer / SVE /
> SVE+delivery) all read 1280×720 on every rect, exit 0. **Positive control:** a synthesised session with
> `annotation.video.resolution` forced to 1920×1080 over 1280×720 pixels reported `dW=+640 dH=+360`,
> flagged, exit 1, **and correctly left `labels.jsonl` and `run.json` reading OK — it ISOLATES the wrong
> source rather than merely failing.** That isolation is the property worth having, not the failure.

---

## 3. S4-1 — the matrix. CASE A throughout.

Ten legs. Full table in `architecture.md` beside its caveats; headline:

**All four rect sources agree on every leg** — 1280×720, 1280×1024, fullscreen 1920×1080, odd 1281×721,
non-multiple 1001×721, `r.ScreenPercentage` 50 and 170, and desktop scale 150 % in two DPI regimes.
**The SVE grab is at OUTPUT resolution in every case.** On the six judgeable legs: **42 counted events,
42 ALIGNED, 0 SHIFTED, 0 ABSENT, 42/42 decidable**, in-leg positive control decisive in both directions
on each. **CASE D never fired** — no empty rect, no dropped frame, no missing key, ring
`published == consumed`, `missed = 0` everywhere.

### 3.1 The DPI catch — the finding of this stage, above the rects

M1 was designed as: desktop to 150 %, run at the engine default, run again under a per-application
`~ DPIUNAWARE` override, compare. **Both returned `dW = dH = 0`.** Read naively: *"DPI scaling does not
move the capture rects."*

**That reading would have been wrong and it would have entered the record.**

`GetProcessDpiAwareness` against the **live PID**, run **with and without** the override, returned
**`PROCESS_DPI_UNAWARE` in BOTH cases.** The packaged game is *already* DPI-unaware, so Windows
virtualises it — the process is told 96 DPI whatever the desktop is set to. The override was a **no-op**,
the two legs were **one regime measured twice**, and **the DPI axis was not probed by either.**

The **opposite** override is the one that works: `~ HIGHDPIAWARE` flips it to
`PROCESS_PER_MONITOR_DPI_AWARE`, verified *before* the leg ran. M1b was redefined and re-run under it —
and also returned `dW = dH = 0`, but now as a **measurement** rather than an artifact of insulation.

> **This is G96's principle applied to a LEVER rather than to an oracle. Every previous instance was a
> blind INSTRUMENT; this was a blind MANIPULATION, and it is worse in one specific way: a blind oracle
> leaves unevaluable output to notice, while a lever that does nothing produces a clean null
> INDISTINGUISHABLE from a clean result. There is no residue to catch it by.** → **G114**.

Three things made it catchable, recorded as the pattern: **read the LIVE PROCESS state, not the cvar
default** (`EnableHighDPIAwareness` defaults to 1 and reads as "games are DPI-aware"; the packaged target
measures the opposite); **run the lever control in BOTH directions and require the readings to DIFFER**,
rather than confirming the setting was applied; **redefine and RE-RUN the leg** under the working
override instead of reporting the no-op leg's clean number.

The no-op leg is **retained in the bank with a `CORRECTION` field**, not deleted — *a bank that silently
loses a mislabelled leg loses the evidence that the mislabelling happened.*

**M1c is the sharpest single data point:** `1001/1.5 = 667.33` and `721/1.5 = 480.67`, both non-integer,
under a **verified** DPI-aware process, and all four rects exact. The rounding seam probed at its worst,
returning nothing.

### 3.2 B1 cannot run off-calibration — an inherited gap, not one S4 introduces

Four legs failed B1's pose precondition (M1c, M2 ×3, M3). **Three of the four had a provably motionless
camera** — `modal_rot` stable, `distinct = 1`, modal 100 %.

`CALIB_BBOX = (0.0, 485.2, 306.1, 234.8)`, `POSE_TOL_PX = 8.0`, **frozen in PIXELS at 1280×720**. M3 at
1920×1080 measured a per-component ratio of **uniform 1.5** — exactly the resolution ratio. M2 at
1280×1024 kept width at **1.0** (horizontal resolution unchanged) and moved y/h because a 5:4 viewport
under horizontal-FOV projection covers a different vertical extent. Both fully explicable, neither pose.

**G107's shape on a new axis:** a frozen ABSOLUTE threshold silently inheriting a dependency of the
quantity it thresholds — G107 was TAU inheriting camera pose; this is `CALIB_BBOX` inheriting resolution.
**It fails SAFE** (honest NOT-CERTIFIABLE, never a false verdict), which is exactly what B1 was adopted
for. **Only its LABEL was wrong** — see §4.

**RULED:** accept and scope. `CALIB_BBOX`, `POSE_TOL_PX` and B1's definition are **untouched**.

> **FILED ALONGSIDE `B2` — normalising `CALIB_BBOX` to NDC.** Payoff, recorded: *"unblocks A54 alignment
> certification at any resolution other than 1280×720. Four legs are now blocked by this (M1c, M2, M3,
> and M2's discards), and THREE of the four were blocked while the camera was provably motionless. This
> is currently unverifiable on BOTH grab points — an inherited gap, not one S4 introduces."*
> A definition change requiring its own eight-control gate. **Not scheduled.**

⛔ **REJECTED: per-resolution calibration constants.** It multiplies the frozen-constant surface, which
is the same failure family as G107 and P8.

### 3.3 Screen percentage — reproduced and extended

**M4b reproduces journal 028's figure on the production path.** **M4a extends it into the upsample
direction 028 never tested.** A48 was satisfied **two ways, not one**: the artifact echoed
`r.ScreenPercentage = "50"`, *and* an independent read-back of the **effect** — gradient energy at
identical dimensions fell 68.045 → 62.914 (−7.5 %), so the image really is upscaled. Reported at that
strength: 7.5 % is modest because the scene is flat white shapes on black and TSR reconstructs.

### 3.4 Odd dimensions — the mp4 is 1 px larger, measured

Run through the **production** encoder, not a hand-rolled ffmpeg line. PNGs 1281×721,
`annotation.video.resolution` 1281×721, **mp4 1282×722**, 90 frames at 30 fps. h.264 requires even
dimensions and the pad is why the encode succeeds at all. **A constraint of odd launch dimensions, not a
defect, not a halt.** Recorded in `client-delivery.md` with a recommendation for even launch dimensions.
`encode_watcher.py` is deliberately **unchanged** — crop-vs-pad is its own decision.

---

## 4. A gate that fails safe still misleads if its LABEL names a cause it has not established

The harness reported the four B1 failures as **"INVALID (A47 bifurcated pose)"** and then declared
**"That is an ENVIRONMENTAL problem, not a leg problem."** Neither was true. Nothing bifurcated and
nothing was environmental.

- the causal attribution is **DELETED OUTRIGHT**. Nothing replaces it. It invited a reader to blame the
  box, and **G108** already records an "environmental" halt that was the harness itself.
- **the header's identical claim about FOCUS TIMEOUTS was softened under the SAME PRINCIPLE**, not
  under a separate ruling. G108 established that a stalled process fails foreground activation, so a
  focus-timeout header asserting an environmental cause carries exactly the defect this section exists
  to remove. Same principle, second instance, same turn — recorded this way because the audit trail
  matters more than the boundary between the two.
- the label is now **"POSE GATE FAILED (B1) — CAUSE NOT ESTABLISHED"**, with the reason stated inline.
- `check_pose.py` gained a **reporting-only** detail block — measured bbox, `CALIB_BBOX`, the
  per-component ratio, `modal_rot`/`distinct`/modal % — and the discriminator in words. No verdict, no
  exit code and no definition depends on it.

**Proven on both causes before being trusted:**

| case | ratio m/CALIB | `modal_rot` | reading |
|---|---|---|---|
| M3, 1920×1080 | **(—, 1.5, 1.4998, 1.5)** uniform | (0,0,0) stable | **RESOLUTION SCOPE** |
| M2, 1280×1024 | (—, 1.3133, **1.0**, 1.1503) | (0,0,0) stable | **RESOLUTION SCOPE** |
| M4b attempt 1 | (—, 0.9301, **0.0552**, 1.1444) | **(354.16, 18.18, 0)** | **genuine A47** |
| M0 (passing) | detail block does not print | — | certifiable |

> **Failing safe is not the same as reporting honestly.**

---

## 5. `TESTVALUE123` — a revert that left the convention intact is not a revert

`m16` recorded the `[AnomalyControlServer] Token=TESTVALUE123` gate artifact as **reverted**. It was
back. **Nothing writes that file** — the generator question was asked and answered. The dev pair had
**three legs and only one was tracked**: the ini (StackOBot is not a repo), the dashboard
`public/config.json` (gitignored), and CaptureBench's `verify_lastrundir.ps1`, which hardcoded the value
as a **parameter default, in version control**. Reverting one leg left the other two asserting it.

⚠ **A KNOWN DEFAULT IS WORSE THAN NO DEFAULT** — it is silent, it is committed, and it survives every
rotation of the actual secret. That tool now reads the token from the ini, throws on a placeholder, and
warns below 32 chars. Rotated to a 64-char random value on **2026-08-19**; the value is deliberately
absent from git, from this journal and from every report, and a post-commit guard confirmed **0
occurrences across both tracked trees**.

`PRE-DELIVERY-CHECKLIST.md` §1 now carries a **runnable** check, not a checkbox — proven against four
cases: live config PASS, historical `TESTVALUE123` FAIL, 6-char FAIL, absent key FAIL. **There is no
pre-cook or pre-stage script in this project**, so it has nowhere automatic to live and one was
deliberately **not invented**. → **G112**, generalised to `GameDefaultMap` (G88), which sits in the same
untracked host config.

---

## 6. S4-2 — the flip. ALL FOUR GATES PASS.

**One line:** `AnomalyCaptureSubsystem.h` — `bool bSveCapture = false` → `true`.

**The flip alone adds no string, so A44 had nothing to bind to.** Rather than weaken the gate, the
`Initialize` banner now also states **where the default came from** — compiled-in vs
`DefaultGame.ini` — which a client build's log should carry anyway, and which supplies the probe.

| gate | result |
|---|---|
| **(a)** A44 on the STAGED exe, both encodings | **PASS** — added `S4 COMPILED-IN DEFAULT (SVE, UI-free)` utf16=1, `default from %s` utf16=1; controls `bSveCaptureDefault` utf16=3, `IAI.Capture.SVE` utf16=10, `RESOLUTION DELTA (3-rect)` utf16=1 |
| **(b)** default leg, **`IAI.Capture.SVE` ABSENT from ExecCmds** | **PASS** — `capture_path: "sve"`, ring `published 121 = consumed 121`, `missed 0`, `corrupted 0`, 90 frames **on disk** (A62) |
| **(c)** backbuffer leg contains UI, positive pixel check | **PASS** — see below |
| **(d)** `content_clock == "wall"` on both legs | **PASS** |

Banner, verbatim, from the default leg:

> `Grab point: sve/scene-colour (scene colour, pre-Slate — UI EXCLUDED), default from S4 COMPILED-IN
> DEFAULT (SVE, UI-free); no ini key present; IAI.Capture.SVE 0 selects the backbuffer/UI-on path.`

### 6.1 Gate (c) — the pair is maximally controlled

Both legs launched **identically** except for the single cvar under test. **A63:** `start_frame = 1` on
both ⇒ comparable. **A64:** both pass B1 *and* their bboxes are **identical**
`(0.0, 485.2, 306.1, 234.8)` — the pair is pose-matched directly, which is stronger than the
`coverage_ratio` indicator.

| region | max(R−B) backbuffer / default | reading |
|---|---|---|
| `UI_orbs` | **1.00000 / 0.00392** | UI present / absent |
| `UI_prompt` | **1.00000 / 0.03529** | UI present / absent |
| `CTRL_cylinder` | 0.03137 / 0.03137 | Δ max(R−B) 0.0039, Δ mean L 0.0011 |
| `CTRL_sky` | 0.01176 / 0.01569 | Δ max(R−B) 0.0039, Δ mean L 0.00003 |

Backbuffer ≥ 0.90 on every sampled frame (min 1.00000); default ≤ 0.05 (max 0.00392); both controls
inside 0.01 on both statistics. **Separation 0.996 against a control band of 0.004.**

### 6.2 A54 on both legs, and one finding the pair produced for free

Both **ALL-ALIGNED**, 7 counted events, in-leg positive control decisive in both directions
(`+1` → 0 ALIGNED/7 SHIFTED; `−1` → 0 ALIGNED/8 SHIFTED). Default leg **7/7 decidable**
(median |margin| 0.1065); backbuffer leg **6/7** (0.1098).

⚠ **The backbuffer leg FAILS A45/A10; the default leg does not.**

```
DEFAULT (sve)   90 rows, 0 undecoded,  non-zero (frame_index - marker) rows = 0
BACKBUFFER      90 rows, 0 undecoded,  non-zero rows = 5
                (session_index, diff) : [(0,+1), (1,+2), (2,+3), (3,+4), (4,+7)]
```

That is **the exact signature journal 042 §10.1 recorded for I10's L1** — indices 0–4, diffs
`+1,+2,+3,+4,+7`. Journal 042 flagged it as *"a LEAD, NOT AN ATTRIBUTION"* and refused to claim it for
B′ **because the two corpora differed in BINARY as well as capture path.**

**RULED — the record entry, in these exact terms, so a cold reader cannot over-read it:**

> **The `+1,+2,+3,+4,+7` startup marker signature is now observed under a SAME-BINARY, SAME-SEED,
> SAME-LAUNCH control with the grab point as the only variable (S4-2 gate (c) pair). Journal 042
> §10.1's binary confound is REMOVED. This is a STRENGTHENED ASSOCIATION at n = 1 on the same-binary
> axis. It is NOT a mechanism claim about B′, and no gate depends on it. Both legs returned
> ALL-ALIGNED.**

Recorded alongside it **as consistency and NOT as explanation:** index 4 carries diff `+7`, the first
event's claimed set is `[4,5,9,10]`, so one noisy frame falls inside it — which sits beside the
backbuffer leg's 6/7 decidability against the default's 7/7.

⛔ **DO NOT CHASE THIS.** No phenomenon number. No lever. It is adjacent to `P1`, and **P1's lever
design is chat-side first — that ruling is intact and this does not touch it.** If a future
same-binary pair happens for another reason it costs nothing to check, and n rises for free. **That is
the whole plan for it.**

### 6.3 What did NOT change

`capture_path` is still emitted **only** when SVE is on, so the backbuffer leg's `run_summary` has **no
`capture_path` at all** — indistinguishable from a pre-S3 build. **That is S4-3, approved and not yet
built.** `annotation.json`'s field set is **unchanged**; **P6 is not moved**; the content clock is
untouched and its key stays unset.

---

## 6.4 S4-3 — `capture_path` on both paths. GATE PASS.

`WriteRunSummary` takes an explicit grab-point argument; the call site **stops inferring the grab point
from the `Ring` pointer**. `key_ring_*` stay inside `if (Ring)`.

⚠ **A44 CANNOT DISCRIMINATE THIS CHANGE, and I did not invent a marker string to pretend otherwise.**
S4-3 adds no new literal — `"backbuffer"` already occurs inside `"async/backbuffer"` and the banner
text. **The provenance proof here is the ARTIFACT (A62): the presence of `capture_path` on a backbuffer
leg IS the change**, which is a *stronger* discriminator than a scan, not a weaker one. Control strings
were still scanned to prove the scanner works and the binary is ours.

**Gate, by CONTROL PAIR — the run-unique set fixed empirically BEFORE any test diff was read:**

| | result |
|---|---|
| control pair (two S4-3 default legs, same binary/config) | key sets **identical**; run-unique values = `end_frame`, `key_ring_{published,consumed,wrapped}`, `speed_ratio`, `sustained_wall_fps` |
| **SVE path**, S4-2 leg → S4-3 leg field set | added `[]`, removed `[]` |
| **BACKBUFFER path**, S4-2 leg → S4-3 leg field set | added **`['capture_path']`**, removed `[]` |
| values | default `"sve"` · backbuffer `"backbuffer"` |
| `key_ring_*` | present on SVE only — correct |
| **`annotation.json` field set, BOTH paths** | added `[]`, removed `[]` — **P6 NOT MOVED** |

⛔ **RULING, recorded here and in the tag, not only in chat: C1's leak-check invariant — "a switch-OFF
`run_summary` is byte-identical to the pre-S3 shape" — is FORMALLY RETIRED as of S4-3.** It existed to
prove S3a was inert when off; once SVE is the default it has no remaining job. **A ruling, not a side
effect of the change.**

## 6.5 S4-4 — delivery orthogonality at the new default. GATE PASS, on the second pair.

**The first pair was INVALID, not FAILED, and the distinction is the whole point.**

The first delivery-ON leg landed **non-pose-matched**: OFF `coverage_ratio` uniform `0.077977`, ON
`0.060633, 0.060134, 0.062112×6`. The extras set was **entirely** `camera/rotation`, `coverage_pct`,
`coverage_ratio` — the camera-dependent fields, and nothing else. **A64's exact predicted shape**, and
the identical shape journal 042 §4 hit on D2/D3 (35 and 27 differing fields).

> **The pose indicator was read BEFORE the comparison, by a pre-fixed rule, on a condition independent
> of the outcome. So the comparison DID NOT RUN.** Re-running for a pose-matched partner is the method
> journal 042 established, not a laundering route — and every attempt was banked, including the
> non-matching ones.

Attempt 3 landed pose-matched (`0.077977`, one distinct value). Re-gated:

- **EXTRAS = 0** against the empirically-fixed run-unique set + `delivery_mode`.
- **127 invariants asserted POSITIVELY, 127 identical, 0 differing** — event count, `frame_indices`,
  `affected_frames` start/end/count, `manifested`, type/subtype, node name/path/asset/class/bounds,
  `coverage_ratio`, `coverage_pct`, the camera block per event, and the run_summary invariants.
- **`capture_path` is present in BOTH legs and INVARIANT across delivery mode** (`"sve"` / `"sve"`).

⚠ **A CORRECTION TO m24, found by the control pair.** Journal 042 states *"all five `key_ring_*`
counters and `capture_path` are identical across every pair."* **`published`/`consumed`/`wrapped` are
RUN-UNIQUE** — two runs of the same binary at the same config differ in them, because they count view
families rendered before capture starts. Only `missed`/`corrupted` are invariant. **m24's verdicts are
undisturbed** — they rest on `missed == corrupted == 0` and on the file set, not on the publish count —
but the claim as written is too strong and does not generalise. *My own first invariant list repeated
that error and asserted all three as invariants; the control pair had already shown otherwise, and the
control pair is authoritative.*

**The correction, in the form that travels:**

> **CORRECTION TO m24 (surfaced by S4-4's control pair): of the five `key_ring_*` counters, only
> `missed` and `corrupted` are invariant across runs. `published` / `consumed` / `wrapped` are
> RUN-UNIQUE — they count view families rendered before capture begins. m24's verdicts are UNDISTURBED;
> they rest on `missed == corrupted == 0` and on the file set, not on the publish count. The claim as
> written did not generalise.**

**Annotated in place** at journal 042 §1, where a cold reader hits the claim. ⚠ **The `m24` tag object
CANNOT be amended** — no force-push, fix-forward — so **`m25` carries the correction forward** and a
reader arriving at m24 via its tag is routed here.

**Why it matters beyond the numbers:** the claim was true of *those* pairs and became a *general
invariant only in the prose*. **That is the second time this project has found a certified claim resting
on prose rather than on a measurement** — the first was **G106**, where the grading instrument itself
existed only in prose. S4-4's control pair had already shown the three counters varying *before* the
test diff was read, and I wrote an invariant list that repeated m24's wording anyway. **The control pair
is authoritative over the prose:** I had the measurement in hand and deferred to the sentence.

## 7. `m25` TAG SCOPE — this text travels with the tag

> **Label alignment on the default (SVE) path is certified at 1280×720 and 1281×721 only. Rect
> equivalence is certified across ten legs including 1280×1024, fullscreen 1920×1080, SP50, SP170, and a
> VERIFIED DPI-AWARE process at 150 % including a non-multiple 1001×721 rect. Alignment at any other
> resolution is UNVERIFIED ON BOTH GRAB POINTS and is blocked by B1's pixel calibration, not by the grab
> point.**

Carried forward from `m24`, unchanged: **modal camera pose only**; **`VideoFps` 30 pinned (A52)**;
**the A54 oracle is certified at 30 fps and its margins are not reproduced above it (P7)**; and
**S3/S4 going green does not close `P1`.**

---

## 8. Corrections on the record

- **A cross-binary control-region delta was nearly reported as a real difference between grab points.**
  The I10-backbuffer vs S3B-SVE comparison showed `CTRL_cylinder` mean luminance differing by
  **0.002157**. The **same-binary** pair (one exe, one level, one seed, grab point the only variable)
  puts it at **0.00010**. The 0.002 was the cross-binary/pose confound (**A47**), not the grab point.
  Recorded because it was one step from entering the record as a finding.
- **The `~ DPIUNAWARE` leg was reported as a DPI regime before it was verified to be one.** It was not.
  See §3.1.
- `_leg_geometry.json` was initially written **only for the accepted leg**, so the two legs whose
  geometry mattered most — the discards — were the only ones without it. Hoisted into
  `Write-LegGeometry`, now written for every banked attempt; the four already-banked dirs were
  retro-filled and marked as retro-filled.
- **I re-encoded two docs while editing them.** A `Get-Content -Raw` / `Out-File -Encoding utf8`
  round-trip on `architecture.md` and `capture-fps.md` rewrote **every non-ASCII line and added a BOM** —
  `git diff --stat` read **377 insertions / 377 deletions** on files meant to change in a handful of
  places. Caught by reading the diffstat rather than trusting the edit, reverted with
  `git checkout --`, and redone through the editor tool: **35 insertions / 10 deletions, no BOM.**
  *A diffstat wildly larger than the intended change is an encoding smell; check it before committing,
  because the content can look right while every line is rewritten.*

---

## 9. New gotchas

**G111** a working-agreement written for ONE agent and stored in a SHARED doc will be executed by ANY
agent that reads it — *the failure mode is not disobedience, it is obedience to an instruction that was
never addressed to you*. **G112** a gate artifact in a file outside version control will silently return;
untracked config needs a **detector**, not a memory. **G113** the Bash tool exits 1 on every call in this
environment while producing correct output — this project gates on exit codes, so verify the **channel**
before trusting what it reports (mirror of G92). **G114** a packaged UE game is **DPI-unaware**, so a
display-scale change never reaches it and the null that produces is an **artifact**. **G115** a shell
round-trip re-encodes the whole file and **the tell is the DIFFSTAT, not the text** — a tool-reflex, not
an attention lapse.

---

## 10. Closing — S4 as shipped

**S4 demoted the backbuffer to the UI-on option.** The default grab point is the SVE / scene-colour
path, so delivered frames no longer contain game UI; `IAI.Capture.SVE 0` is the named bisect switch and
the shipped default needs no ini key. Rect equivalence is certified across **ten legs** with all four
rect sources agreeing on every one, **42/42 events ALIGNED and decidable** on the six judgeable legs, UI
exclusion and presence established by **positive pixel check** on an identical-bbox pair, delivery
orthogonality re-asserted, and `capture_path` now stating on **both** paths what produced a session.
Tagged **`m25`**, annotated, carrying its own scope. **S4 does not close `P1` and never could.**

### 10.1 Two gate/change mismatches, two OPPOSITE correct resolutions

Both stages hit the same shape — *the gate does not fit the change* — and the right answer was different
each time, which is the point worth keeping:

| | **S4-2** | **S4-3** |
|---|---|---|
| the mismatch | a one-token default flip adds **no string** for A44 to bind to | the change adds **no new literal** (`"backbuffer"` already occurs inside `"async/backbuffer"`) |
| resolution | **give the change something worth adding** — the banner now states the grab point *and where its default came from*, which a client build's log should carry anyway | **say the scan cannot discriminate it**, and name the stronger discriminator: the **artifact** (A62) — the presence of `capture_path` on a backbuffer leg *is* the change |
| what was NOT done | weaken or waive the gate | **invent a marker string so the scan appears to pass** |

> **Both were reached by asking what actually PROVES the thing, rather than what SATISFIES the
> checklist.** A gate that no longer fits its change is a signal, not an obstacle — and the two live
> answers are *make the change provable* or *name a better proof*, never *make the check pass*.

### 10.2 What this session got wrong

- **The m24 `key_ring` claim** — §6.5. Second instance of a certified claim resting on prose rather
  than measurement (G106 was the first), and I had the contradicting measurement in hand before I
  repeated the prose.
- **Two encoding incidents, the second after the lesson was written** — §8 and **G115**. Reported as
  two, not folded into one, because *the repeat is the more useful datum*: it shows the fault is a
  tool-reflex that a written note does not defend against. Nothing corrupted reached a commit; both
  were caught by reading `git diff --stat` before committing, which is now a standing pre-commit habit
  in `CLAUDE.md`.
- **A cross-binary control-region delta nearly reported as a real grab-point difference** — §8.
- **The `~ DPIUNAWARE` leg reported as a DPI regime before it was verified to be one** — §3.1.
