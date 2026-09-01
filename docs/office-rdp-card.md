# Office box — RDP card

> **One card. Everything on it is runnable over RDP except the two items marked
> PHYSICAL-ONLY.** Written for the owner at the keyboard, sealed box, **no clipboard between
> machines** — every read-back is a short list of numbers or a yes/no you can type by hand.
> Code never reaches that box; this card is the whole interface.

**Written 2026-09-01 against `master` = `635f389`**, which contains the m34 + m35 merge
(`3f835c3`). ⚠ **Do not trust that SHA if time has passed — `git log --oneline -1` on the box is the
authority.** What matters is that the tip contains the merge, and Section A step 2 checks it.

---

## 0. WHAT RDP CAN AND CANNOT JUDGE

| item | verdict | why |
|---|---|---|
| Pull, rebuild, cook, artifact hashes | ✅ **RDP-VALID** | file and console work; nothing depends on how pixels reach your eye |
| Tick-pin probe echo | ✅ **RDP-VALID** | a build-log read |
| m35 Bates hotfix validation (`READBACK-LAYOUT`, no-crash, frame counts) | ✅ **RDP-VALID** | log lines and files on disk |
| `m31` V-3 / V-4 | ✅ **RDP-VALID** | counter and artifact reads |
| Custom-depth question | ✅ **RDP-VALID** (partial in A, full in B) | see A-7 |
| m36 census legs, `[BATES-TYPABLE]` sheet | ✅ **RDP-VALID** | typed counters + one eye judgment on *content*, not on smoothness |
| **`G-R7(ii)` DISPLAY half — the hitch A/B eye gate** | ⛔ **PHYSICAL-ONLY** | RDP resamples, drops and re-times frames. A stutter you see over RDP may be the link; one you don't may be hidden by it. The gate judges `b05066f`, and RDP cannot carry that judgment either way. |
| **`G-R7(ii)` THROUGHPUT half** | ⛔ **PHYSICAL-ONLY** | the RDP server encodes every frame on the same GPU and CPU the capture is using. That load lands inside the numbers the gate reads (`t_wall` span vs frames/VideoFps). A throughput figure measured under RDP is a figure about RDP. |

⚠ **Both halves of `G-R7(ii)` are physical-only, so the DELIVERY gate still needs one trip to the
box.** Everything else on this card can be done from anywhere, today.

📌 **NAMED RDP ROUTE FOR THE EYE GATE, offered not required:** run **OBS on the office box itself**,
record locally at 60 fps, then review the file **frame-stepped**. The recording is made before the
RDP encoder touches anything, so the artifact is honest even though the live view is not. This turns
the eye gate into a file you can watch later — it does **not** make it RDP-valid in real time, and if
it disagrees with what you see live, **the local recording wins.**

---

# SECTION A — RUNNABLE NOW, on `master`

Nothing here waits for m36. This is the m35 hotfix validation plus the standing office reads.

### A-1. Working tree must be clean BEFORE anything

```
git -C <plugin-path> status --short
```
**Read back:** the number of lines printed.
**Expected: 0**, or only lines starting `??`.
⛔ **If any line starts with ` M` or `M `, STOP and report it.** A dirty tree on that box means
something was edited locally and a pull would either fail or bury it.

### A-2. Pull, and confirm you got the merge

```
git -C <plugin-path> pull --ff-only
git -C <plugin-path> log --oneline -3
```
**Read back:** the three subject lines.
**Expected:** one of them is `Merge branch 'feature/mask-gpu-reduce' - m34 + m35 - OFFICE GATE PENDING ON MASTER`.
⛔ If `pull --ff-only` refuses, STOP — do not merge or rebase on that box.

### A-3. Rebuild the **EDITOR** target FIRST — not optional

```
& "D:\UESource\UnrealEngine\Engine\Build\BatchFiles\Build.bat" StackOBotEditor Win64 Development -project="<path>\StackOBot.uproject" -waitmutex
```
**Read back:** `exit code` and the `Total execution time` line.
**Expected: exit 0.**
⚠ The cook runs on **editor** binaries. Skipping this once cost a 39-minute cook that produced an
unbootable build (`G47`/`G131`).
⚠ **Check free space first** on the volume holding `Binaries`: **≥15 GB GO, <10 GB NO-GO.** A link at
2 GB free died mid-write *and had already deleted the previous exe* — `G164`'s second form.

### A-4. Read the tick-pin probe echo — THE BASELINE READ

In the build output from A-3, find the line beginning:

```
AnomalyCapture: TICKPIN probe   route C fork-named files: ...
```

**Read back:** **which route fired, or "none".** One word.
🚨 **This reading is only meaningful on that box.** The home box has no forked engine loop, so its
probe correctly reports *not fired* **whether the detector works or not** — the office box is the
only positive control this project has. Take it now, before anything depends on it.
⛔ `AnomalyCapture.Build.cs` is **never edited** (owner ruling). This is a log read, nothing else.

### A-5. PIE capture on Bates

Open the project, open `MainWorld`, press Play, then in the console (`` ` ``):

```
IAI.Capture.Config 2 4 8 4 0
IAI.Capture.Start "" png 4242 90 blinking
```

Let it run to completion. **90 frames is the floor and it is not negotiable** — a one-frame look
cannot tell a working drain from one that survives its first frame.

### A-6. THE FOUR NUMBERS — the whole m35 verdict

From the run's `run_summary.json` and the Output Log:

| # | what | where | expected |
|---|---|---|---|
| 1 | `total_frames` | `run_summary.json` | **90** |
| 2 | files in `Actual_Frames\` | file explorer, count them | **90** |
| 3 | `READBACK-GUARD FIRED` line count | Output Log search | **0** |
| 4 | `EXTENT-CLAMP FIRED` line count | Output Log search | **0** |

**Read back: four numbers.** That is the entire m35 hotfix result.
⚠ **1 and 2 are both asked for on purpose.** A log line saying the run finished is not evidence a
file exists (`A62`), and the crash this milestone exists for wrote a clean-looking session with
**zero** frames on disk.

Then find the **one** layout line:

```
Capture(sve): READBACK-LAYOUT sourceExtent=____x____ rect=(__,__)-(____,____) picture=____x____ bufferHeight=____ rowPitchInPixels=____ fmt=__
```

**Read back: that line, copied by hand.** It is self-describing and it is the whole photo. What it
decides:

| what you see | what it means |
|---|---|
| `rect` inside `sourceExtent`, **no** `EXTENT-CLAMP` line, picture looks right | the host's view rect and scene-colour texture agree — **the fix is working** |
| an `EXTENT-CLAMP FIRED` line instead | the coordinate spaces disagree on that host — and the frame was **dropped rather than silently mis-captured**. Report it; this is a finding, not a failure of the run |
| **`rect.min.y > 0`** with a correct picture | the host letterboxes **and the sub-rect origin is being applied correctly** — this is the exact crash condition, now handled |

**And the no-crash evidence, stated as a yes/no:** *did the process reach the end of the run without
an assert dialog?* ✅ **Failure here is fast** — the one crash of this class fired **22 ms** after
capture start, on the **first** armed frame. If it survives the first second, it has cleared that
failure mode. Surviving is not a pass on its own; the four numbers above are the pass.

### A-7. Custom-depth question — both hosts

⚠ **Honest limit: there is no one-liner for this on `master`.** The full enumeration arrives with
m36 (Section B, B-2), where the StartRun echo lists it for free.

What `master` **can** answer, from the same A-5 capture, is the part that actually bites:

Search the Output Log for `bRenderCustomDepth`.
**Read back: yes/no — did any line appear?**
- **No line** ⇒ nothing on that host collided with the plugin's stencil range (200–254) during the
  run. That is the answer that matters for tagging correctness.
- **A line appeared** ⇒ copy it. It names a component the plugin did **not** tag that is writing
  custom depth into our range — a host writer, and exactly what the census is for.

⛔ Do **not** infer "the host writes no custom depth" from a silent log. This detects **collisions**,
not all writers. The complete census is B-2.

### A-8. What Section A satisfies

Passing A-6 on Bates **is the m35 close-out condition**. On that result:

- m35's Bates hotfix validation is **done**;
- and the close-out checklist item unlocks — *delete `wip/session-061-backup` from origin and local*
  (it is the last deliberate pre-scrub copy on the remote, kept only until this validation passed).

⛔ **Still NO TAG.** Tagging is the end of the physical visit, after `G-R7(ii)`. The order is
`m31` → `m33` → `m34` → `m35`, and nothing is tagged before that gate.

### A-9. Also RDP-valid while you are there

- **`m31` V-3 / V-4** — counter and artifact reads, no eye judgment.
- **Deimos**, if reachable: same A-5/A-6 sequence. Expected `bufferHeight == picture height` and a
  correct picture. Being 5.2+, it is the host where the **pre-m35** code would have been wrong at a
  non-zero origin, so it confirms the fix rather than the layout.

---

# SECTION B — ONCE m36 IS BENCH-GREEN

✅ **UNLOCKED 2026-09-01 — m36's home gates have passed and the branch is merged to `master`
(`0f35d7a`).** The rule that gated this section stands and is why it was gated: a census leg on an
unproven build produces numbers that look like data. That condition is now met, so Section B is
runnable — **pull `master` first** (A-2's step, which now brings m36 with it).

📌 **`master` IS INERT FOR THE CLIENT.** The census's compiled default is **OFF**, and with no
provider registered the selection path is byte-identical to the pre-census picker — measured, not
asserted. So merging m36 changed nothing about a delivered build unless someone turns the census on,
which is what Section B does deliberately.

🚨 **SECTION B IS A PAIR OF LEGS, NOT ONE.** Leg 1 runs the **shipped default floor (6.0)**; leg 2
runs a **pool-bearing floor (0.5)**. **Both are needed and neither is the pass.** On the bench,
floor 6.0 left exactly ONE eligible candidate out of 77 while floor 0.5 left about fourteen — so a
single leg at either floor tells you almost nothing about the other, and **the two legs' histograms
together are the INPUT TO THE FLOOR DECIDE.**
⛔ **Neither leg decides the floor and no number on this card recommends one.** You are collecting
the two distributions; the value is a later, separate decision.

### B-1. The two legs

Run leg 1, read everything in B-2…B-6, **then** run leg 2 and read the same things again.
⚠ **Restart the game between them.** `IAI.Capture.CensusFloor` is read at `StartRun`, and running
them back-to-back in one process is fine — but a restart is what makes each leg's **provenance**
echo independent, which is half of what B-2 is for.

**LEG 1 — shipped default floor. Do NOT issue `CensusFloor`; the point is that the compiled
default is what runs.**
```
IAI.Capture.Mask 1
IAI.Capture.Census 1
IAI.Capture.Config 2 4 8 4 0
IAI.Capture.Start "" png 4242 90
```

**LEG 2 — pool-bearing floor.**
```
IAI.Capture.Mask 1
IAI.Capture.Census 1
IAI.Capture.CensusFloor 0.5
IAI.Capture.Config 2 4 8 4 0
IAI.Capture.Start "" png 4242 90
```

🚨 **`IAI.Capture.Mask 1` IS NOT OPTIONAL AND IS THE EASIEST THING ON THIS CARD TO GET WRONG.** The
census is only active when the mask AND async capture are both on (`bCensusEffective = census &&
mask && async`). Without it the run completes, writes every artifact, and **emits no census at
all** — which reads exactly like a clean census result. The build says so out loud if it happens
(`census was REQUESTED but is INACTIVE for this run`); B-2 is where you catch it.

📌 **`IAI.Capture.Start "" png 4242 90` has FOUR arguments on purpose.** Naming an anomaly without
also naming a target actor is refused as an incomplete targeted run; and a **targeted** run would
bypass selection entirely, which is the one thing Section B exists to exercise. Four arguments =
auto-pool = the selector choosing its own targets, which is the thing under test.

### B-2. StartRun echo — read it BEFORE anything else, on BOTH legs

```
=== Capture(census): EFFECTIVE FOR THIS RUN - census ON (requested on, from ...), floor=6.00%(from ...), maxVerdictAgeTicks=12(...), excludeTranslucent=1(...), reservation=1 ===
Capture(mask): M36 STENCIL RESERVATION ON - reserved=N [ ... ]
```

**Read back per leg:** `census ON/off`, **`floor` AND the bracketed source next to it**,
`maxVerdictAgeTicks`, `excludeTranslucent`, `reservation`, and **`reserved=N` plus the list of
values**.

🚨 **If the line is absent, or says `census off`, THAT LEG IS VOID — not a pass** (`G139`).
✅ **The floor's PROVENANCE is a free correctness check across the pair:** leg 1 must read
`floor=6.00%` from a **compiled/ini** source and leg 2 must read `floor=0.50%` from **console**. If
leg 2 still says 6.00, the command did not take and **the leg is void**, not a low-pool result.
📌 **`reserved=N [values]` IS the full custom-depth census** A-7 could only partly answer: every
component on that host already writing custom depth in 200–254 that the plugin did not tag.
**Read it on both hosts.** It should be **identical across the two legs** — it is a property of the
host, not of the floor.

### B-3. The eleven counters, per leg

From `run_summary.json`. **All eleven exist only when the census actually ran** — if any are
missing, re-read B-2 rather than reporting zeros.

| field | what to expect on a healthy Bates leg |
|---|---|
| `census_frames` | a number |
| `census_cycles` | a number — **compare it against `census_candidates`; see B-4** |
| `census_candidates` | **≥ 1** — below that the leg counts toward **no** prediction |
| `census_zero` | a number |
| `census_below_floor` | a number — **expected MUCH larger on leg 1 than leg 2** |
| `census_excluded_translucent` | a number |
| `census_fires_fallback_all` | **0** on a healthy leg. Non-zero = the loud-inert path fired — **report it, do not re-run** |
| `census_unmeasurable_nanite` | **0** — Bates has no Nanite at all. **Non-zero means the classifier is misfiring; report it, do not re-run to a green** |
| `census_unmeasurable_tag_failed` | **0** expected. Non-zero is a tagging problem — report the number |
| `census_unmeasurable_hidden` | a small number — a candidate is unmeasurable while an anomaly is hiding it, which is normal |
| `census_unmeasurable_not_yet_measured` | a number |

**Plus, from the same file:**

| field | reading |
|---|---|
| `vetoed_events` | banked Bates band is **12–15** per run. Census-ON expectation is a **marked drop**. ⚠ **REPORTED, NOT GATED** — the veto is the backstop and residual vetoes are it working |

**Read back: twelve numbers per leg, twenty-four in total.**

### B-4. The five-number histogram — this is the floor-decide input

The census logs a per-cycle line:

```
Census: CYCLE n DRAWN-COVERAGE histogram zero=A (0,1]=B (1,3]=C (3,6]=D (6,12]=E (12,25]=F >25=G | <names and counts>
```

**Take ONE settled cycle (any cycle after the first two) and read back these FIVE numbers — the
count of candidates that would SURVIVE each candidate floor.** Each is a running total from the
right-hand end of that line:

| floor | how many survive | arithmetic from the printed buckets |
|---|---|---|
| **≥ 0.5 %** | *number* | B + C + D + E + F + G *(the `(0,1]` bucket straddles 0.5, so this slightly over-counts — say so, do not correct it)* |
| **≥ 1 %** | *number* | C + D + E + F + G |
| **≥ 3 %** | *number* | D + E + F + G |
| **≥ 6 %** | *number* | E + F + G |
| **≥ 12 %** | *number* | F + G |

**Five numbers per leg. Ten in total. These ten ARE the floor decision's evidence** — the whole
point is to see where the pool falls off a cliff on the host that matters, rather than on the bench.

⚠ **Also read `zero=A`** and say it separately. A large `zero` is not a problem — it is the census
finding objects that draw nothing, which is the entire reason it exists.
🚨 **A census whose candidate count collapses after cycle 1, or whose split is nearly all
`NOT_MEASURABLE`, is measuring almost nothing even though every counter is populated.** That is why
B-3 asks for `census_cycles` and `census_candidates` together.

### B-5. The `READBACK-LAYOUT` line — one line, four numbers

Search the log for `READBACK-LAYOUT` and copy the line. What matters is:

```
rect=(X0,Y0)-(X1,Y1) picture=WxH
```

**Read back: the rect and the picture size.** ✅ **`picture` must equal `X1-X0` by `Y1-Y0`.**
📌 **A non-zero `X0` or `Y0` is the whole reason m35 exists** — it is the condition that crashed
this host — so if either is non-zero, say so explicitly; it means the host is letterboxing or
pillarboxing and the fix is being exercised for real rather than in a bench simulation.
⛔ If the line is absent, do not infer anything from its absence — report that it did not appear.

### B-6. The eye judgment — the product definition

**Type a list per leg: each fired target's name, and `visible: yes/no`** — could a viewer see that
object change?

✅ **This one IS RDP-valid.** It judges *what was selected*, not how smoothly it rendered. A target
either is or is not a thing a viewer can see change, and RDP does not alter that.
⛔ Do not use this to judge stutter. That is the physical-only gate.
📌 **The comparison between the two legs is the interesting part:** leg 1 (floor 6.0) should fire on
a small number of large, obvious objects; leg 2 (floor 0.5) on a wider and smaller set. **If leg 2's
extra targets are things a viewer cannot see change, that is the floor being too low — and it is
exactly the judgement the bench cannot make for you.**

---

## C. IF SOMETHING FAILS

**Report and stop. Do not re-run to a green, and do not fix on the box.**

Anything on this card that fails is a result. The most useful thing you can send back is the
numbers you actually saw plus the line that surprised you — not a second attempt that looked
better. Several findings in this project came from a leg that failed once and was reported verbatim.
