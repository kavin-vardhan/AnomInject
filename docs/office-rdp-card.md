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
| 3 | `READBACK-GUARD FIRED` line count | **the one command below** | **0** |
| 4 | `EXTENT-CLAMP FIRED` line count | **the same command** | **0** |

**Read back: four numbers.** That is the entire m35 hotfix result.

🆕 **NUMBERS 3 AND 4 ARE ONE COMMAND, NOT A SEARCH.** They were left UNREAD on 2026-09-02 because
"search the Output Log" is a scrolling job that is easy to skip. Paste this into PowerShell **after
the run has finished**, with the project path filled in:

```
$log = "<TITLE-SAVED-LOGS>"; "LOG  $log"; "MTIME $((Get-Item $log).LastWriteTime)"; foreach ($p in 'READBACK-GUARD FIRED','EXTENT-CLAMP FIRED') { "{0,-22} = {1}" -f $p, @(Select-String -Path $log -Pattern $p -SimpleMatch).Count }
```

**Expected output — exactly four lines, and it prints them even when both counts are zero:**

```
LOG  <TITLE-SAVED-LOGS>
MTIME 09/02/2026 14:31:07
READBACK-GUARD FIRED   = 0
EXTENT-CLAMP FIRED     = 0
```

📸 **Photograph all four lines, not just the two counts.** The path and MTIME are there so the
counts can be tied to the run you just did — a zero read off yesterday's log is not a zero.
⚠ **`= 0` and "I did not look" are different results and must never be reported the same way.** If
the command errors, report the error; **a missing count is UNREAD, not zero** (`G197`).
📌 **`<TITLE-SAVED-LOGS>` = the host project's own `Saved\Logs\<its>.log`.** You know that path; **it
is deliberately not written on this card and never should be.** ⚠ It differs by how you launched: a
**PIE / editor** run writes under `<project>\Saved\Logs\`, a **packaged** run under
`<Build>\…\Saved\Logs\`. A-5 is a PIE run, so use the first.
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

What `master` **can** answer, from the same A-5 capture, is the part that actually bites: **did
anything on this host collide with the plugin's stencil range 200–254 during the run?**

🚨 **CORRECTED 2026-09-02 — THE OLD INSTRUCTION HERE WAS "search the Output Log for
`bRenderCustomDepth`", AND THAT SEARCH CANNOT ANSWER THE QUESTION.** On `master` the reservation
line itself contains the words *"Host-set `bRenderCustomDepth`"*
(`AnomalyCaptureSubsystem.cpp:1423`), so **the bare word matches on every healthy run** — a "yes"
means nothing and a "no" means the search never reached the log at all. ⚠ **On 2026-09-02 that read
came back "no line seen", which on this build is itself the tell that the search hit console
scrollback rather than the log file.**

**Search for the COLLISION LINES BY NAME instead.** One command, printing a labelled count per
signal even when every count is zero:

```
$log = "<TITLE-SAVED-LOGS>"; "LOG  $log"; foreach ($p in 'OBSERVED - the stencil tag did not read back','OBSERVED - the mask carried reserved-range tag','OBSERVED - batch id=','CENSUS-HYGIENE final DIFF','CENSUS-HYGIENE cycle DIFF','TAG-POOL EXHAUSTED') { "{0,-52} = {1}" -f $p, @(Select-String -Path $log -Pattern $p -SimpleMatch).Count }
```

**Read back: six labelled numbers. Expected: all zero.**
- **All zero** ⇒ nothing on that host collided with the plugin's range during the run. That is the
  answer that matters for tagging correctness.
- **Any non-zero** ⇒ **copy that whole line out of the log.** Each one names what it saw, and each
  says *CAUSE NOT ESTABLISHED* on purpose. Report it verbatim; **do not re-run to a green.**

⛔ Do **not** infer "the host writes no custom depth" from six zeros. This detects **collisions**,
not writers. The complete enumeration is the `reserved=` line in **B-2**, which lists every host
component already writing into 200–254.

### A-8. What Section A satisfies

Passing A-6 on Bates **is the m35 close-out condition**. On that result:

- m35's Bates hotfix validation is **done**;
- and the close-out checklist item unlocks — *delete `wip/session-061-backup` from origin and local*
  (it is the last deliberate pre-scrub copy on the remote, kept only until this validation passed).

⛔ **Still NO TAG.** Tagging is the end of the physical visit, after `G-R7(ii)`. The order is
`m31` → `m33` → `m34` → `m35` → `m36`, and nothing is tagged before that gate.

✅ **DONE 2026-09-02: A-5/A-6 PASSED ON BATES** — 90/90 frames, `rect` origins `(0,138)` and `(0,69)`,
no crash — **and the consequence was executed**: `wip/session-061-backup` is deleted from origin and
local. ⚠ **Counters 3 and 4 were NOT read on that run** — they are Section C item (a).

### A-9. Also RDP-valid while you are there

- **`m31` V-3 / V-4** — counter and artifact reads, no eye judgment.

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

🆕 **THE `reserved=` LINE, EXACTLY — and how to capture it whole (added 2026-09-02: on the first
Bates run this line was CUT OFF IN THE PHOTO, so the host's custom-depth answer was lost).**

**Exact text.** One line, `LogAnomalyCapture` at `Log` verbosity, emitted **at `StartRun`** — i.e.
the moment you press enter on `IAI.Capture.Start`, *before* any frame is captured. It reads:

```
Capture(mask): M36 STENCIL RESERVATION ON - reserved=3 [ 200 201 250 ]. Host-set bRenderCustomDepth with a stencil value in 200..254, snapshotted at StartRun, is never assigned by the census OR the event allocator this run: hygiene restores host values AFTER, this prevents host pixels being COUNTED under a plugin tag DURING. No per-cycle rescan in v1.
```

⚠ **The prose after the `]` is why the photo failed — it is long, and the numbers you need are at
the FRONT.** Do not photograph the console. Run this instead, which prints **only** the part that
matters, plus an independent second read of the same quantity:

```
$log = "<TITLE-SAVED-LOGS>"; Select-String -Path $log -Pattern 'M36 STENCIL RESERVATION' -SimpleMatch | ForEach-Object { ($_.Line -replace '^.*(M36 STENCIL RESERVATION.*?\]).*$','$1') }; Select-String -Path $log -Pattern 'M36 TAG POOL' -SimpleMatch | ForEach-Object { ($_.Line -replace '^.*(M36 TAG POOL.*?)\.\s*$','$1') }
```

**Expected: two short lines per run, both photographable in one shot:**

```
M36 STENCIL RESERVATION ON - reserved=3 [ 200 201 250 ]
M36 TAG POOL assignable 200..254 (255 is NEVER mintable by any allocator - it stays the residual StencilDummy detector), hostReserved=3, assignable=52
```

✅ **The two lines are an `A48` cross-check and that is the point of asking for both:** `reserved=N`
and `hostReserved=N` are written by **different** code paths and **must agree**. If they disagree,
report both numbers and stop — do not pick one.
🚨 **`reserved=0 [ ]` is a REAL AND EXPECTED ANSWER on a host that writes no custom depth.** It is
not a failed read. **A MISSING LINE is the failed read**, and it means the leg is void by `G139`.

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

# SECTION C — THE NEXT RDP VISIT

> **Added 2026-09-02, after the first Bates run.** Everything here is short, all of it is RDP-valid,
> and none of it needs a rebuild, a cook or a new build. **Sections A and B are DONE** — this is the
> list of what those runs left unread, plus one standing rule.
>
> 🚨 **THIS IS NOT A RE-RUN OF SECTION A OR B.** Items (a)–(c) read the logs **already on that box**
> from the runs already done. Only (d) is a new capture, and it is optional.

### C-(a). The two counters Section A left UNREAD — one command

Run the **A-6 command** (the four-line one) against the Section A run's log.
**Read back: all four lines.** Expected `READBACK-GUARD FIRED = 0` and `EXTENT-CLAMP FIRED = 0`.

⚠ **These are currently recorded as UNREAD, NOT ZERO.** m35's Bates pass rests on frames, origins
and no-crash; **these two counters are the part of that verdict nobody has seen.** A non-zero is a
finding, not a failure — the guard exists to drop a frame rather than mis-capture it.

### C-(b). The `reserved=` line, captured whole

Run the **B-2 command** (the two-short-lines one) against a Section B leg's log.
**Read back: both lines** — `reserved=N [ … ]` and `hostReserved=N`, which must agree.

📌 **This is the host's complete custom-depth answer** and it was lost to a cropped photo last time.
`reserved=0 [ ]` is a real answer; a **missing** line is not.

### C-(c). The custom-depth collision question, asked properly

Run the **A-7 command** (six labelled counts). **Read back: six numbers. Expected all zero.**

⚠ The previous instruction here could not have answered the question — see A-7's correction. This
one can.

### C-(d). ONE GLANCE at a banked leg-1 black frame — the cheapest item on this card

Open **any pitch-black PNG from Section B leg 1** (floor 6.0) in `Actual_Frames\` and look at it.

| what you see | what it means |
|---|---|
| **character and/or sky still visible**, a large scenery object missing | a **landscape hide** — the expected consequence of floor 6.0 leaving the landscape blueprint eligible. **Nothing is wrong.** |
| **the frame is FULLY black**, nothing at all | a **readback defect** — a different problem entirely, and it would mean m35's Bates pass needs re-reading |

**Read back: which of the two.** ⛔ **One word settles it, and the two causes are indistinguishable
from the counters alone** — which is why the frame itself has to be looked at. Not urgent; very
cheap.

### C-(e). ✅🔻 **SUPERSEDED 2026-09-02 — `P9` CLOSED, MITIGATION LIFTED. DO NOT APPLY THIS.**

> 🏁 **`blinking` IS BACK IN THE BATES POOL.** `m40` was validated on Bates by `SECTION D` on
> 2026-09-02 (`D-0` PASS, strong row): labels == code == eye. **The condition this item waited on —
> *"until `P9` closes"* — is met.** Closure: `docs/invisible-anomaly-mechanisms.md` §8.6a.
> 📌 **Kept, not deleted**: it was correct for the whole time it stood, and the text below is what it
> said.

**`P9` is open** (`docs/invisible-anomaly-mechanisms.md` §8): on Bates, `blinking` events have been
observed ×3 where the labelled hidden frames and the visible ones do not agree. **No mechanism is
claimed and none is implied by this instruction.**

⇒ **Leave `blinking` out of the auto pool on Bates runs until `P9` closes.** Every other anomaly
this plugin ships is single-state, so none of them has a hide boundary inside its window for this to
land on. **The cost is one anomaly type on one host; the benefit is a Bates dataset with no known
open labelling question in it.**

### C-(g) and C-(h) — 🔻 **REPLACED 2026-09-02 by the `P9` Bates protocol below.**

*(Both are superseded. `C-(g)`, the delivery read, was already demoted to tidy — delivery is
excluded as a `P9` factor by bench measurement. `C-(h)`, the one-event typed bundle, is **not
dropped**: it survives as **`C-3`**, and it is now taken on a `C-1` leg rather than on a banked one.)*

---

# 🔴 THE `P9` BATES PROTOCOL — `C-1`, `C-2`, `C-3`

**Why this exists:** you reproduced `P9` on Bates **with every plugin flag off** — no census, no
mask, plain `blinking` + capture — and measured a per-frame opacity ladder. That makes it **not a
census or mask phenomenon**, and it puts two separate things on the table:

- **(A) BOUNDARY SMEAR** — partial opacity at `n` (≈20 %), `n+1` (≈10 %) and `n+6` (≈20 %).
- **(B) PHASE DISPLACEMENT** — fully gone at `n+2`/`n+3` where the labels say visible, fully visible
  at `n+5` where the labels say hidden. **This is `P9` proper.**

⛔ **NO CAUSE IS CLAIMED FOR EITHER.** These legs are built to separate them, not to explain them.

📌 **Your `r.AntiAliasingMethod 2` test does not settle (A), and here is the honest reason:**
**method `2` is TAA, which RETAINS HISTORY** — `0` off · `1` FXAA · `2` TAA · `3` MSAA · `4` TSR.
You moved from one temporal method to another. **`C-1` is the leg that actually reaches a
non-temporal state.** Also: **the FSR3 upscaler and FSR3 frame interpolation are SEPARATE
switches** — turning off the upscaler does not turn off interpolation.

### C-1. THE AA-OFF LEG — ✅ **SUBSTANTIVELY DONE BY THE OWNER, 2026-09-02**

🔻 **RESULT, AND THE PRE-DECLARED DISCRIMINATOR FIRED.** With **all anti-aliasing and motion blur
disabled through the title's own settings menu**: **the partial-opacity frames are GONE**, and **the
displacement PERSISTS** as a clean binary read —
**observed hidden `{n, n+1, n+2, n+6}` vs claimed `{n, n+1, n+5, n+6}`.**

⇒ ✅ **(A) BOUNDARY SMEAR IS CLOSED — temporal accumulation**, exactly as the discriminator below
pre-declared. 🔴 **(B) PHASE DISPLACEMENT IS OPEN and is now the whole question.**

📌 **RESIDUE, and it is small: the three bare-cvar read-backs and the frame-generation switch lookup
were never captured** — the settings were changed by MENU, so the effective values are unrecorded.
**They live in `C-3(e)` now. Do them there; there is no need to re-run `C-1`.**

*(The full procedure is kept below as the record of what was pre-declared and why the result means
what it does. ⛔ Do not re-run it unless something below is contradicted.)*

#### C-1 as it was pre-declared

**Set all four and CONFIRM EACH BY READ-BACK *BEFORE* starting the run.** Type each bare name to
print its current value:

```
r.AntiAliasingMethod 0
r.MotionBlurQuality 0
```

then, for the two FSR3 switches — 🚨 **I CANNOT GIVE YOU THE EXACT CVAR NAMES AND I AM NOT GUESSING
THEM.** The FSR3 plugin is not in the bench engine, so any name I wrote would be invented. **Find
them on that box:** type `FidelityFX` then `FSR` in the console and read what completion offers, or
look in the FSR3 plugin's own `Config/`. You need **both** the **upscaler** switch and the **frame
interpolation** switch, each set OFF.
⛔ **A cvar that answers `Unrecognized command` is UNREAD, NOT OFF.** Report the name you used and
what it printed.

**Read back all four before the run and copy the four lines.** Then:

```
IAI.Capture.Config 2 4 8 4 0
IAI.Capture.Start "" png 4242 90 blinking <target>
```

⛔ **No census, no mask** — do not issue `IAI.Capture.Census` or `IAI.Capture.Mask`.

**Then, for ONE event, the opacity ladder `n` … `n+7`** — the same reading you already did: for each
frame, **fully visible / partial (≈ what %) / fully gone**.

**PRE-DECLARED, so the leg means something either way:**

| what you see | what it establishes |
|---|---|
| **the partial-opacity frames VANISH** (every frame now fully visible or fully gone) | **(A) is temporal accumulation on Bates' pipeline** |
| **the partial-opacity frames PERSIST** | **(A) is host-side fading on a DIFFERENT axis** — not temporal accumulation |

🎯 **AND (B) IS READ ON THE SAME LEG, INDEPENDENTLY:** with (A) removed, write down **hidden or
visible for each frame `n` … `n+7`**. **If fully-gone still lands at `n+2`/`n+3` and fully-visible
still lands at `n+5`, (B) stands on its own** — that is the whole point of doing both readings on
one leg.

### C-2. THE TARGET-CLASS LEG — does (B) depend on what the target is?

**Same config as `C-1`** (all four switches off, confirmed by read-back, no census, no mask).
Run it **twice**, targeted:

1. on a **plain `StaticMeshActor`** — from the census histogram, `StaticMeshActor_1246` or
   `StaticMeshActor_1158` are known to draw;
2. on a **Blueprint actor**.

```
IAI.Capture.Start "" png 4242 90 blinking StaticMeshActor_1246
```

**Read back: does (B) appear on BOTH?** One line each is enough — *"displaced yes/no"*.

### C-3. ✅ **DONE BY OWNER 2026-09-02 (run 13:57). IT ANSWERED. DO NOT RE-RUN.**

> ✅ **RESULT, one line: the toggle log sides with the EYE on both interior flips — HIDDEN `[38]` =
> frame 28 = `n` · VISIBLE `[41]` = `n+3` · HIDDEN `[44]` = `n+6`, with the eye gone at 28, 29, 30, 34
> — so PIXELS, AA, the capture path and the overlay are CLEARED, and the LABELS `{28,29,33,34}` are
> the outlier.** The apply tick is not counted by the injector there (`apply [35] → first toggle
> [38]` = **+3**, against **+2** on the bench). Full result, join table and arithmetic:
> `docs/invisible-anomaly-mechanisms.md` §8.6a. Fix plan (no code): journal 068 §9.
> ✅ **AMBER on `n−1` and NO box on `n+7` both confirmed on Bates**, as the code predicted.
> 📌 **Everything below is kept as the record of what was run and how it was read** — a post-fix
> re-read will use the same four commands.

🔻 *(Was: "THIS IS NOW THE MOST IMPORTANT THING ON THE CARD.")* `C-1` closed **(A)**. Every other axis
for **(B)** was excluded — AA in both directions, census, mask, delivery, pacing, tick ratio,
letterbox — **and `C-3` then located the remainder.**

**PREREQUISITES — all three, or the bundle cannot answer the question:**

1. 🚨 **A FRESH RUN. Every banked run lacks the toggle lines.** They are **Verbose-only**
   (`Anomaly_Blinking.cpp:95`), so no existing session has them, however carefully you read it.
2. 🚨 **TYPE `Log LogAnomaly Verbose` IN THE CONSOLE BEFORE STARTING THE RUN.**
   ⚠ **If the toggle lines still do not appear afterwards, THAT IS A READ, NOT A FAILURE** — record
   it exactly as *"`Log LogAnomaly Verbose` was set and no `blinking toggle ->` line appeared"*, and
   send the bundle anyway. An absent line that was never asked for and an absent line that was asked
   for are different results (`G197` family).
3. **AA + motion blur OFF**, as you already established in `C-1`.
4. 🆕 **ANCHOR ON AN EVENT INSIDE THE FIRST ~64 CAPTURED FRAMES — two independent anchors instead of
   one.** The `SVE-WANT-TRACE arm … gameFrame=` line stops after 64 arms (`AnomalySveCapturer.h:38`),
   and it is the backup anchor for the toggle line; past frame 63 the engine `[frame]` prefix is the
   *only* anchor left. With this card's run config (`IAI.Capture.Config 2 4 8 4 0`, 90 frames) the
   captured frames run `0..89`, the lead-in is `0-3`, and each burst's event window is 8 frames:
   **burst 1 = `4-11` · burst 2 = `16-23` · burst 3 = `28-35` · burst 4 = `40-47` · burst 5 =
   `52-59`** (burst 6 starts at `64` and is already outside). ⇒ **pick your event from any of the
   first five bursts.**

**THE PRE-DECLARED THREE-WAY COMPARISON.** For **each interior flip** — the mid-event show (labels
`n+2`, eye `n+3`) and the second hide (labels `n+5`, eye `n+6`) — ask **which frame the toggle log's
timing sides with**:

| what the log shows | what it says |
|---|---|
| **log + LABELS agree** | the **PIXELS** are the outlier on this host |
| **log + EYE agree** | the **SAMPLING / LABELLING** side is the outlier on this host |
| **mixed, or neither** | ⛔ **report raw. Classify nothing.** |

⛔ **NO BRANCH IS ASSERTED AND NONE IS PREFERRED.** ⛔ **These outcomes name WHERE the divergence
sits. They never name WHY.**

🚨 **RUN THE COMMANDS IMMEDIATELY AFTER THE CAPTURE, BEFORE RELAUNCHING ANYTHING.** The engine **rotates
its log on the next launch**, so the toggle lines in (a) exist only until the title starts again.
Everything else is on disk and safe, but (a) is not.

✅ **EACH COMMAND ALSO APPENDS ITS OUTPUT TO `$r\p9_bundle.txt` IN THE RUN FOLDER while still
printing to the screen**, so the bundle survives a relaunch and travels with the session. Run all
four and the file holds the whole thing — **nothing to photograph line by line.**
*(`Tee-Object -FilePath … -Append` — verified present and working on PowerShell 5.1. ⚠ It writes a
UTF-8 BOM at the head of the file; harmless to read, worth knowing if anything ever parses it.)*

**Set these two first**, then run the four commands in order:

```
$r = "<run>"
$log = "<TITLE-SAVED-LOGS>"
```

📌 **`<run>`** = the session directory (`…\Saved\…\session_<timestamp>`).
📌 **`<TITLE-SAVED-LOGS>`** = **the host project's own `Saved\Logs\<its>.log`.** You know that path;
**it is deliberately not written on this card** and never should be.

Four commands, each capped to be photo-friendly.

**(a) toggle lines** — ⚠ **`Verbose`, so ABSENT unless the run enabled it.** Add
`Log LogAnomaly Verbose` to the `C-1` run's `ExecCmds`. **Nothing printed is a READ, not a failure:**

```
"== (a) toggle lines ==" | Tee-Object -FilePath $r\p9_bundle.txt -Append; Select-String -Path $log -Pattern 'blinking toggle ->' -SimpleMatch | Select-Object -First 20 | ForEach-Object { $_.Line } | Tee-Object -FilePath $r\p9_bundle.txt -Append
```

**(b) `session_index` ↔ `frame_index` for the span** (replace `40` with `n-2`):

```
"== (b) index map ==" | Tee-Object -FilePath $r\p9_bundle.txt -Append; (Get-Content $r\labels.jsonl | Select-Object -Skip 40 -First 12) | ForEach-Object { $o=$_|ConvertFrom-Json; "{0,4}  {1,8}  {2}" -f $o.session_index,$o.frame_index,$o.image } | Tee-Object -FilePath $r\p9_bundle.txt -Append
```

**(c) the event's `frame_indices`:**

```
"== (c) frame_indices ==" | Tee-Object -FilePath $r\p9_bundle.txt -Append; (Get-Content $r\annotation.json -Raw|ConvertFrom-Json).anomalies | ForEach-Object { "{0,-12} {1,-28} {2}" -f $_.anomaly_type,$_.affected_objects.nodes[0].name,($_.affected_frames.frame_indices -join ',') } | Tee-Object -FilePath $r\p9_bundle.txt -Append
```

**(d) `labels.jsonl` rows `n-1` … `n+8`** (replace `41` with `n-1`):

```
"== (d) label rows ==" | Tee-Object -FilePath $r\p9_bundle.txt -Append; (Get-Content $r\labels.jsonl | Select-Object -Skip 41 -First 10) | ForEach-Object { $o=$_|ConvertFrom-Json; "{0,4} present={1,-5} {2}" -f $o.session_index,$o.anomaly_present,(($o.anomalies|ForEach-Object{ "$($_.target_name) bbox_valid=$($_.bbox_valid)" }) -join ' | ') } | Tee-Object -FilePath $r\p9_bundle.txt -Append
```

**(e) the effective render settings — `C-1`'s RESIDUE, because those were set by MENU and the actual
values were never captured.** Type each bare name in the game console and copy what it prints, then
record them into the bundle:

```
r.AntiAliasingMethod
r.MotionBlurQuality
r.ScreenPercentage
```

```
"== (e) render settings: r.AntiAliasingMethod / r.MotionBlurQuality / r.ScreenPercentage ==" | Tee-Object -FilePath $r\p9_bundle.txt -Append
```

**PLUS the frame-generation switch, which still has no confirmed name.** 🚨 **I cannot give it to you
and I will not guess it** — the FSR3 plugin is not in the bench engine, so any name I wrote would be
invented. **Find it on that box:** type

```
fidelityfx.
```

in the console and read what **completion** offers; the frame-generation / frame-interpolation entry
is the one wanted, separately from the upscaler. ⛔ **A cvar that answers `Unrecognized command` is
UNREAD, NOT OFF** (`G197`). Record the name you used and exactly what it printed.

📌 **For the record only.** `C-1`'s result already stands on the menu settings doing what they say;
this pins the numbers behind it.

**Then send `p9_bundle.txt`.** ✅ One file, whole thing, survives a relaunch.

🚨 **(c) IS THE ONE THAT SETTLES THE OVERLAY QUESTION.** You reported red boxes at `n+1, n+2, n+5,
n+6` while the recorded cadence is `n, n+1, n+5, n+6` — one frame later, **for the first pair only**.
**From the code, red and amber CANNOT disagree by one frame:** red means *"this frame is in
`annotation.json`"*, amber means *"this frame has a `labels.jsonl` row but is not in
`annotation.json`"*, and **both are stamped from the same counter one line apart**. So the red boxes
**are** `frame_indices`, and printing that array once tells us which reading was right. ⛔ **Nobody
needs to look at the overlay again to settle it.**

#### 🆕 C-3 READ GUIDE — how to READ the bundle. ⛔ No new steps, no new commands.

> 🏁 **`C-3` WAS RUN AND IT ANSWERED — owner, 2026-09-02, 13:57. NOTHING BELOW NEEDS RUNNING AGAIN.**
> **Result in one line: the toggle log sides with the EYE on both interior flips (HIDDEN `[38]` =
> frame 28 = `n` · VISIBLE `[41]` = `n+3` · HIDDEN `[44]` = `n+6`), so pixels, AA, capture path and
> overlay semantics are CLEARED and the LABELS `{28,29,33,34}` are the outlier.** Full result and join
> table: `docs/invisible-anomaly-mechanisms.md` §8.6a. **The guide is kept as the record of how it was
> read, and because a post-fix re-read will use it again.**

*(Added 2026-09-02 from the transition-driver source read, journal 068 §1. **Nothing here changes what
you run.** It changes what to look at afterwards, and it names the one thing that can quietly make the
bundle unanswerable.)*
*(⚠ **Amended 2026-09-02 after the adversarial re-read, journal 068 §6.6.** Items 1, 3, 5 and 6
carried errors that would have mis-read a correct bundle. The corrections are marked 🔻.)*

**1. 🚨 THE TOGGLE LINE CARRIES NO INDEX OF ITS OWN. ITS ONLY ANCHOR IS THE `[…]` PREFIX.** The line is
literally `blinking toggle -> HIDDEN (1 actors).` and nothing more — no frame number, no session
index. What identifies it is the **engine's own prefix**, which should look like:

```
[2026.09.02-10.07.33:125][  4]LogAnomaly: Verbose: blinking toggle -> HIDDEN (1 actors).
```

That `[  4]` is the engine frame counter mod 1000, **and it is the same counter `labels.jsonl` calls
`frame_index`.** ⇒ **join `(a)` to `(b)` on the bracketed number, not on `session_index`.**
🔻 **CORRECTION — do NOT match on the date.** The frame bracket is **the LAST bracket before the
category**, and the engine prints it in four of its five timestamp modes. The bracket in front of it
may be a date (`[2026.09.02-…]`), a **seconds float** (`[0012.34]`) or a timecode, depending on the
build. **All of those are fine and the anchor is present.**
⚠ **Report "no prefix" ONLY if there is no bracket at all** — that is the single mode
(`LogTimes=None` / `-NOLOGTIMES`) that removes the anchor, and then only line ORDER survives. Nothing
to fix on the box; we just need to know.
✅ *(On the actual `C-3` run the prefix was PRESENT and the exact-frame join worked.)*

**2. READ `(b)` FOR `frame_index`, NOT ONLY `session_index`.** The `(b)` command already prints both
(`session_index  frame_index  image`). `frame_index` is the join key to `(a)`; `session_index` is the
key to the PNG name and to `(c)`.

**3. `SVE-WANT-TRACE arm … gameFrame=…` STOPS AFTER 64 FRAMES.** It is a useful second anchor sitting
next to the toggle lines, but only for captured frames up to ~64. Beyond that its absence is normal.
The `keyed frame id=N submitted` lines continue for the whole run, but they come from the **render
thread** — use their `id=` field, **never** their position in the file or their `[…]` prefix.
🔻 **AND IT IS MORE THAN A SECOND ANCHOR — IT IS THE CHECK ON THE FIRST ONE. CONDITIONAL STEP:** *if*
`SVE-WANT-TRACE arm` lines exist in the log, **verify `[fff] == gameFrame % 1000` on the first one.**
That line reports its own frame as a field *and* receives the prefix, so the two must agree; if they
disagree, the prefix was not stamped when the line was logged and **every toggle-line join in the
bundle is void.** Measured on the bench: **704 of 704 across 11 packaged logs, 0 divergent.**
⚠ **Bates' installed build prints NO such line** (measured 2026-09-02: `gameFrame=30038` and
`gameFrame=30041` both returned **0 hits**). **Its absence there is EXPECTED and is not a fault** —
the prefix anchor alone sufficed, and it did.

**4. TWO FREE CROSS-CHECKS ON THE OVERLAY, both expected from the code:**
- **`n − 1` should carry an AMBER box** (it has a label row for the fire but is not in
  `frame_indices`).
- **`n + 7` should carry NO BOX AT ALL — neither red nor amber.** The last frame of each burst is
  dropped from the event by construction. **If you see a box at `n+7`, or no box at `n−1`, write it
  down and send it raw** — that is a finding, not a mistake.

**5. EXPECT EXACTLY THREE TOGGLE LINES PER EVENT, in the order `HIDDEN · VISIBLE · HIDDEN`,** followed
by `IAI.Revert 'blinking' -> reverted.` (which is the event's end). The line
`blinking: matched 1 actor(s) for '…' at half-period N frame(s).` in the same log reports the
half-period — **copy that line too.**
🔻 **CORRECTION — THE RULE IS BOUNDED, AND TWO SENTENCES OF IT WERE WRONG.**
- **It holds for every FULL burst only.** ⛔ It does **NOT** hold for the **LAST** event of a 90-frame
  run: the frame cap ends the run mid-burst, the revert comes from run shutdown instead, and that
  event reads `frame_indices [88,89]` with fewer than three toggle lines. **Measured on 235 banked
  90-frame sessions and live on the bench.** ⚠ The `-First 20` cap in `(a)` also truncates the last
  event it prints. ⇒ **count toggle lines only on an event from bursts 1–5**, as prerequisite 4
  already requires — and **a short count there is NOT evidence that the half-period differs.**
- 🚨 **THE FIRST HIDE'S POSITION IS A DISCRIMINATOR, NOT AN EXPECTATION.** Under the tick order this
  box has, flip 1 lands on a tick that is deliberately **not** captured, so its `[fff]` matches **no**
  `labels.jsonl` row and `frame_index(n) − frame_index(n−1)` reads `3`. **If flip 1's `[fff]` instead
  lands ON `frame_index(n)`, that is a READING, not a fault — write it down and send it raw. It is the
  single most informative line in the bundle.** *(On the actual `C-3` run it DID land on
  `frame_index(n)` — `[38]` = 30038 = session 28 — and that is what located the divergence.)*

**6. WHAT THE COMPARISON THEN ASKS, in one sentence each.** For the mid-event show (labels `n+2`, eye
`n+3`) and the second hide (labels `n+5`, eye `n+6`): find the frame the toggle line lands on.
**Lands on the LABELS' frame ⇒ the pixels are the outlier on this host. Lands on the EYE's frame ⇒ the
divergence lies in the INTERVAL BETWEEN THE TOGGLE CALL AND THE LABEL SAMPLE. Anything else, or no
usable anchor ⇒ report raw and classify nothing.** ⛔ These name WHERE, never WHY, and no branch is
expected in advance.
🔻 **CORRECTION — row 2 used to say "the SAMPLING / LABELLING side is the outlier". It now names the
INTERVAL, not a subsystem** (journal 068 §6.5): a one-tick change in *when the toggle runs relative to
the arm* produces the same reading with the labelling code behaving identically, so naming a subsystem
would point a reader at the wrong place. ⛔ **Do not write "the sampler is broken on this host".**

### Standing

> 🔻 **SUPERSEDED — THE FIX BUILD NOW EXISTS. GO TO `SECTION D`.** This block said *"nothing further
> is needed from Bates for `P9` until a fix build exists"*; `m40` landed on the bench 2026-09-02
> (`0864e7a`, exe `C0AD3F91`), so the errand it was waiting for is live. **`C-1` closed (A); `C-3`
> located (B); `SECTION D` validates the fix.** ⛔ **Still do not re-run `C-1` or `C-3`.**

✅🏁 **THE MITIGATION IS LIFTED (2026-09-02). `blinking` IS BACK IN THE BATES POOL.**
`SECTION D` read **YES**: labels == code == eye, on the strong row. **It lifted because `m40` is
VALIDATED on that host, not because it shipped.**
🔻 *(superseded, kept as history: "Blinking stays UNTICKED on any Bates run that is not `C-1`, `C-2`,
`C-3` or `SECTION D`" — that mitigation stood from the day `P9` was minted until `D-3` answered.)*
⛔ **Still never on a host: `IAI.Bench.SynthTickOrder`.**

### C-(f). OPTIONAL — a clean Bates dataset

If there is time, repeat Section B with `blinking` unticked. Same two legs, same payloads, same
reads. That yields a Bates census dataset carrying no `P9` exposure at all.
⛔ **Optional. Not a gate, and nothing waits on it.**
---

# SECTION D — `m40` VALIDATION ON BATES. ✅ **DONE BY OWNER 2026-09-02. IT PASSED.**

> 🏁 **RESULT, one line: `D-0` PASS on the STRONG ROW — `apply → first toggle` still reads `Δ = +3`
> (Bates' tick order UNCHANGED) and `frame_indices {28,29,30,34}` equals the eye exactly. `P9` (B)
> IS CLOSED.** Toggles `[863]`/`[866]`/`[869]` → frames `28`/`31`/`34`; eye gone at 28, 29, 30, 34.
> Full result: `docs/invisible-anomaly-mechanisms.md` §8.6a (closure block at the head).
> ✅ **CONSEQUENCE: the `blinking` mitigation on Bates is LIFTED — see Standing.**
> 📌 **Everything below is kept as the record of what was run**; a future host validates the same way.

> 🎯 *(as briefed)* **THIS CLOSES `P9` ON THE HOST IT WAS FOUND ON.** `m40` landed on the bench
> 2026-09-02 (`0864e7a`, exe `C0AD3F91`) and passed four legs there, including a **bench reproduction
> of `P9` (B)** and its removal. **The bench cannot validate it for Bates — only Bates can.**
> ⏱ **Budget: one pull, one editor rebuild, one capture, five reads.**

---

## D-0. THE PASS CONDITION — **pre-declared, and it is the whole point. Read it before you run anything.**

> ### On the `m40` build, for ONE `blinking` event in the first five bursts:
> # **`annotation.json`'s `frame_indices` MUST EQUAL the set your eye reads — whichever tick order this host has.**

**That is the entire test.** `m40`'s claim is that the label no longer depends on the tick order, so
the order is now a *report*, not a gate. Concretely, the two admissible outcomes:

| if the toggle lines read | i.e. | then EXPECT `frame_indices` | and the eye gone at |
|---|---|---|---|
| **Δ = +3** — `blinking: matched` at `[a]`, first `blinking toggle ->` at `[a+3]` | this host still ticks the injector first | **`{n, n+1, n+2, n+6}`** | **`n, n+1, n+2, n+6`** |
| **Δ = +2** — first toggle at `[a+2]` | this host's order also changed | **`{n, n+1, n+5, n+6}`** | **`n, n+1, n+5, n+6`** |

✅ **PASS = labels and eye agree, in EITHER row.** 🎯 **Δ = +3 with agreement is the STRONGEST result**
— the disorder is still there and the labels are right anyway.
⚠ **Δ = +2 with agreement is a PASS but a WEAKER one** — that build's order also moved, so it does not
exercise the fix. **Say so; do not report it as the strong result.**
🔴 **FAIL = labels ≠ eye, in any row. Report raw and classify nothing.** Do not re-run to a green and
do not change anything on the box.

**Two free cross-checks, unchanged from `C-3` and both expected to hold either way:**
**AMBER box on `n − 1`** · **NO box at all on `n + 7`**.

---

## D-1. UPDATE THE BOX

Exactly the `A-1` / `A-2` / `A-3` steps, with one new expectation.

**D-1a — clean tree first** (`A-1`): `git -C <plugin-path> status --short`
**Expected: 0 lines, or only lines starting `??`.** ⛔ Any ` M`/`M ` line ⇒ STOP and report.

**D-1b — pull** (`A-2`):

```
git -C <plugin-path> pull --ff-only
git -C <plugin-path> log --oneline -1
```

**Read back: the one subject line.**
**Expected: `0864e7a feat(capture): m40 - order-independent label sampling (+ bench-only synth tick-order lever)`, or later.**
⛔ If `pull --ff-only` refuses, STOP — do not merge or rebase on that box.

**D-1c — rebuild the EDITOR target** (`A-3`): check free space on the volume holding `Binaries`
first — **≥15 GB GO, <10 GB NO-GO** (`G164`) — then

```
& "D:\UESource\UnrealEngine\Engine\Build\BatchFiles\Build.bat" StackOBotEditor Win64 Development -project="<path>\StackOBot.uproject" -waitmutex
```

**Expected: exit 0.**

📌 **STATED PLAINLY, so nobody wonders what just changed on that box: this pull brings `m37`
(census selection defaults) + `m38` (the run-scoped session log) + `m40` (this fix) to Bates'
EDITOR build. The PACKAGED build on that box is UNTOUCHED — nothing is cooked and nothing the
client would receive is altered by Section D.**

---

## D-2. THE RUN

**Prerequisites — exactly `C-3`'s, no additions except one:**

1. **A FRESH editor session** (the run log and the toggle lines are per-run).
2. **AA + motion blur OFF via the title's own settings menu**, as established in `C-1`.
3. **`blinking` TICKED** — Section D is one of the four runs where it is allowed.
4. **Anchor on an event in the first five bursts.** At this config the windows are
   **burst 1 = `4-11` · 2 = `16-23` · 3 = `28-35` · 4 = `40-47` · 5 = `52-59`.**

**Type these in the editor console, in this order:**

```
Log LogAnomaly Verbose
IAI.Capture.RunLog 1
IAI.Capture.RunLogVerbose 1
IAI.Capture.Config 2 4 8 4 0
IAI.Capture.Start "" png 4242 90 blinking
```

🚨 **`IAI.Capture.RunLog 1` IS REQUIRED ON BATES AND WAS LEARNED THE HARD WAY (2026-09-02):
that project runs DELIVERY MODE, and the `m38` run log's default MIRRORS `run.json` — so on a
delivery-shaped build it is AUTO-OFF and `anomaly_log.txt` is never written.** The first attempt
produced no run log for exactly this reason. **The `Capture(runlog)` echo tells you which way it
went, in its own words, on every run — read it, and force the log when you want one** (`G210`).

🆕 **`IAI.Capture.RunLogVerbose 1` IS THE NEW LINE, AND IT IS WHY THIS IS EASIER THAN `C-3` WAS.** On
an `m38`+ build the plugin writes its own run-scoped log **into the session folder**, so after the
run:

```
$r   = "<the session directory just written>"
$log = "$r\anomaly_log.txt"
```

⇒ **no editor-log hunting and no rotation risk** — the file travels with the session and survives a
relaunch. **The `C-3` commands `(a)`–`(d)` then work UNCHANGED with those two variables set.**

⚠ **FALLBACK: if `$r\anomaly_log.txt` is ABSENT**, point `$log` at the host project's own
`Saved\Logs\<its>.log` exactly as `C-3` did, and **run the reads immediately, before relaunching
anything.** 📌 **Its absence is a READING, not a failure:** the run log is on by default when
delivery is off, and the build says why in its own words — grep `Capture(runlog)` in the log and copy
that line. **Send it either way.**

---

## D-3. THE READS — in this order

| # | what | command / action |
|---|---|---|
| 1 | **`(c)` the event list** — pick your event, note `n` | `C-3` command `(c)` |
| 2 | **`(b)` the index map** — run it with **skip = `n − 2`** | `C-3` command `(b)` |
| 3 | **`(a)` the toggle lines** | `C-3` command `(a)` |
| 4 | **the apply bracket** | `Select-String -Path $log -Pattern 'blinking: matched'` — note the `[fff]` |
| 5 | **the eye** | open `Actual_Frames\` and read frames **`n−1` … `n+8`**: gone or visible, one per frame |
| 6 | **compare against `D-0`** | — |

**Send back one line, plus the numbers behind it:**

```
labels == eye: YES / NO
delta (apply -> first toggle): +N
frame_indices: {...}
eye gone at:   {...}
amber on n-1: yes/no      box on n+7: yes/no
```

---

## D-4. 🚨 TWO THINGS THAT MUST NOT HAPPEN

⛔ **NEVER TYPE `IAI.Bench.SynthTickOrder` ON BATES — NOT EVEN `0`.** It is a **bench device**. On a
host it would synthesise the symptom *on top of whatever that host really does*, and the validation
would measure the lever instead of the fix. **It is default-OFF and console-only, so simply never
issue it.** *(If it were ever set, the `Capture(bench): m40 SYNTH TICK ORDER = ...` line at `StartRun`
would say `ON` — check that it says `off` and copy that line.)*

⛔ **`blinking` stays UNTICKED on every other Bates run until `D-3` reads `YES`.** The old mitigation
does not lift because the fix shipped; it lifts because the fix is *validated there*.

---

## D-5. OPTIONAL, SAME BUILD, IF THERE IS TIME — the `m37` ceiling read

⛔ **NOT a gate for tonight. Data for the census-ON decision, nothing waits on it.** A `B-1`
leg-2-style census run, **`blinking` UNTICKED**:

```
IAI.Capture.Mask 1
IAI.Capture.Census 1
IAI.Capture.Config 2 4 8 4 0
IAI.Capture.Start "" png 4242 90
```

**Read back two things:**
1. from the **`StartRun` echo**: the **ceiling value and its SOURCE** (the `Capture(census): m36/m37`
   line names both);
2. from the **`CYCLE` lines**: **`aboveCeiling=N`** and the **named candidate(s) over 25 %**.

**Expected:** the landscape-class actor **EXCLUDED**, with an **ABOVE-CEILING** line naming it.
⚠ **A different number is a result, not a fault — report it verbatim.**

---

## IF SOMETHING FAILS (any section)

**Report and stop. Do not re-run to a green, and do not fix on the box.**

Anything on this card that fails is a result. The most useful thing you can send back is the
numbers you actually saw plus the line that surprised you — not a second attempt that looked
better. Several findings in this project came from a leg that failed once and was reported verbatim.

---

# SECTION E — `m41` VALIDATION ON BATES (census ON by default)

⛔ **RDP-valid.** Editor/PIE only, same route as Section D. The packaged Bates build stays sealed.
🔻 **`D-5` (the `m37` ceiling read) is SUPERSEDED by `E-3` step 3 — do not run it separately.**

## E-0. THE PASS CONDITIONS — pre-declared. Read them before you run anything.

**(a)** the StartRun echo reads **census ON** and **mask ON**, and **names a source for each** — one
source, never a disjunction. ⚠ **Either `COMPILED DEFAULT (on)` or the ini key is a PASS.** Which one
appears tells you whether the Bates project's ini carries the keys, and that is worth knowing either
way — the bench container has them, so it reads `ini` there.
**(b)** the **`HOST-PP CUSTOM-DEPTH READERS =`** line is present **with NON-ZERO `scanned` counts**.
🔑 **Read the scanned counts, not just the number.** A `= 0` with `scanned 0/0/0` is **BLINDNESS, not a
clean read**, and on this host it would mean the enumeration found nothing to look at.
**(c)** `aboveCeiling >= 1` with an `ABOVE-CEILING` line naming the landscape-class actor.
**(d)** 🔴 **the fog-card actor: report its CLASSIFICATION, and if it is MEASURED, its DRAWN %.**
⛔ **NO EXPECTED VALUE IS STATED HERE, DELIBERATELY.** The owner has observed a surface-translucent
material **without** custom-depth writes being selected, which under `m41`'s rule should not happen —
so something on that actor draws into custom depth and we do not know what. **An expected value would
bias the one observation we have.** Whatever it says is the result.
**(e)** an `Auto.Fire: census consulted=…` line on **every** fire.
**(f)** eye list: anomaly visible at **≥ the m36 leg-2 rate (~90 %)**, **no repetition of the same 2–3
targets**, **no pitch-black frames**.

⚠ **(c) can come back different and still be a RESULT:** `aboveCeiling = 0` means the landscape actor's
drawn coverage moved (a different window/letterbox) — report the histogram verbatim, do not re-run.

## E-1. Update the box

Mirrors `D-1`: `git status` clean → pull → **confirm the SHA** → rebuild the **EDITOR** target.
⚠ **Runbook §8.6 STEP 3.5 is not optional** — the cook/editor binaries are what actually run (`G47`).

## E-2. The run

**`blinking` TICKED** — `P9` is closed and the standing mitigation is lifted.

```
IAI.Capture.RunLog 1
IAI.Capture.Config 2 4 8 4 0
IAI.Capture.Start "" png 4242 90
```

🚨 **`IAI.Capture.RunLog 1` FIRST, and it is not optional here.** The Bates project runs **delivery
mode**, so `m38`'s run log is **auto-OFF** there (`G210`); forcing it puts this entire read into
`anomaly_log.txt` beside `annotation.json` instead of console scrollback, which is where the last
Section-A read got lost.

## E-3. The reads, in this order

1. the two **`EFFECTIVE FOR THIS RUN`** lines (mask, then census) — **whole lines**;
2. the **`HOST-PP CUSTOM-DEPTH READERS =`** line — **whole line, including the `scanned` counts**;
3. from **one settled cycle**: `CYCLE n DONE` · `DRAWN-COVERAGE histogram` · any `ABOVE-CEILING` line ·
   the `NOT-MEASURED` listing *(this replaces `D-5`)*;
4. **every** `Auto.Fire: census consulted=` line;
5. the **15 `census_*` keys** from `run_summary.json`;
6. the **eye list — target name + visible yes/no, per event, written down DURING the run.**

## E-4. Two things that must not happen

⛔ **Do not type `IAI.Capture.Census` or `IAI.Capture.Mask`.** The entire point of this leg is that the
**compiled defaults** do it; typing either destroys `E-0(a)` and there is no way to recover it after
the fact.
⛔ **Do not re-run to a green.** Anything that fails here is a result; send the numbers you saw.

## E-5. What Section E is NOT

It is **not** `C-G1b` and **not** `B-G1` — those need an authored custom-depth-reading post-process
material and an authored translucent-with-custom-depth-writes material respectively, and both are
**cook-time gates on the client build** (`PRE-DELIVERY-CHECKLIST.md` §1.1). Section E reads what the
shipped defaults do on a real host; it does not manufacture a fixture.

## E-6. `m43` — THE TARGET MASK, TWO PAIRS AND ONE LINE

⛔ **RDP-valid.** Same leg as `E-2` — no extra run needed; these are reads off the session it produced.

**Read the run's summary line first:**

```
Capture(m43): TARGET MASK SUMMARY measured=? hiddenBlank=? unavailable=? tagFlips=?
```

**Report all four numbers.** `measured + hiddenBlank` should equal the captured frame count and
`unavailable` should be **0**. ⚠ **A non-zero `unavailable` is a result, not a fault** — it means those
frames' readbacks never arrived and their `labels.jsonl` rows say `mask_file: null`. `tagFlips` is the
target mask's own stencil churn; it read **0** on every bench leg and a non-zero value on a real host is
worth knowing.

**Then open TWO PAIRS, side by side, and say what you see. No expected value is stated for the pixels —
describe them.**

**Pair 1 — a MEASURED frame.** Pick any `session_index` whose `labels.jsonl` row has a non-empty
`anomalies` array and a non-null `mask_file`. Open `Actual_Frames/frame_NNNNN.png` and
`target_mask/frame_NNNNN.png` together.
→ **Read back: does the mask's non-zero region sit on the anomaly target in the colour frame?**
"silhouette matches" / "silhouette is offset" / "mask is blank but the target is visible" — whichever it
is.

**Pair 2 — a HIDDEN-BLANK frame.** Pick a `session_index` inside a `blinking` event's hidden set (the
`frame_indices` in `annotation.json`, joined by `session_index`). Open the same two files.
→ **Read back: is the mask all black, and is the target absent from the colour frame?**
"blank as expected" / "blank but the target is visible" / "not blank".

🔑 **Why both pairs:** the first checks the mask points at the right pixels; the second checks that a
**blank** mask means *"measured, nothing visible"* and not *"we failed to measure"*. Those two are
different facts and the whole artifact rests on keeping them apart.

⚠ **If `target_mask/` is absent entirely**, read the `Capture(m43): TARGET MASK` echo — it names the
reason on its own line (requested off / the mask pass is off / refused because the output height is
non-zero).

---

# SECTION F — `m44` RE-READ ON BATES (target mask onset)

⛔ **RDP-valid.** Editor/PIE only, same route as Sections D and E. The packaged Bates build stays
sealed. **Minimal by design: one run, six reads.**

## F-0. Update the box

Mirrors `E-1`: `git status` clean → pull → **confirm the SHA** → rebuild the **EDITOR** target.
⚠ Runbook §8.6 STEP 3.5 is not optional (`G47`).

## F-1. The run — `blinking` TICKED

```
IAI.Capture.RunLog 1
IAI.Capture.Config 2 4 8 4 0
IAI.Capture.Start "" png 4242 90
```

## F-2. The reads

**(a) NO all-zero PNG exists.** In the session folder:

```
python D:\IntrusiveAnomalies\StackOBot\Plugins\CaptureBench\tools\m44_gates.py <session-dir> BATES
```

**EXPECTED: `G2 blank PNGs: 0 PASS`.**

**(b) First mask frame == first labelled frame, for one non-hidden event.**
Same command as (a) — read the per-event table's `1stLbl` / `1stMsk` / `delta` columns.
**EXPECTED: `delta` is `0` on every `corrupted_texture` and `missing_texture` row, and
`G1 delta==0 ... PASS`.**

**(c) No `mask_file` on any frame not labelled for its event.**
Same command as (a).
**EXPECTED: `G7 mask frames subset of labelled frames: ... stray=0 PASS`.**

**(d) The three counters reconcile.** Same command as (a).
**EXPECTED: `G3 ... present==PNGs True | sum==rows True -> PASS`.** ⚠ `unavailable` is NOT expected to
be 0 — `blinking` frames are `unmeasured` by design until the follow-up build.

**(e) 🆕 A READING, NOT A PASS — internal vs output view rect.** In the run log:

```
Select-String -Path <session-dir>\anomaly_log.txt -Pattern 'M23 PASS' | Select-Object -First 1
```

**Report the two numbers `viewRect=WxH` and `internalViewRect=... WxH` and nothing else.**
⛔ **No expected value is stated, deliberately.** If they are EQUAL on that host, the mask is fine as
shipped. If they DIFFER, this host runs a screen percentage other than 100 (dynamic resolution or a
temporal upsampler) and **the `F1` fix must land before the client cook** — that is the whole purpose
of this read.

**(f) 🔴 A READING — the fog-card actor.** As in `E-0(d)`: report whether it is still selected, and its
census classification line verbatim. ⛔ **No expected value.**
**(g) 🆕 `m45` — a blinking event's hidden frames carry masks, its visible frames do not.**
Same command as (a) — read the per-event table and the `G7` line.
**EXPECTED: every `blink` row has a `1stMsk` equal to its `1stLbl` (not `NONE`), and
`G7 ... files=N labelled=N stray=0` with the two counts EQUAL.** ⚠ A `blink` row still reading `NONE`
means the hidden-class hide did not take on this host; report it, do not re-run.

**(h) 🆕 `m48` — the exposure dip, and it is a READING plus one EXPECTED value.**
In the session folder:

```
python -c "import json;d=json.load(open(r'<session-dir>\run_summary.json'));print('frames_exposure_dip =', d.get('frames_exposure_dip'))"
python D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector\tools\verify_capture.py --dir <session-dir> --black-frame-gate --quiet
```

**Report `frames_exposure_dip` verbatim — no expected value, this is the first reading of it on that
host.** 🔑 **EXPECTED, and this half IS a pass condition: `BLACK FRAMES 0` and
`DARK FIRST FRAMES 0`.**
⚠ **Report the gate's `whole-frame luminance min/max` line too** — it is what says whether that host
had auto-exposure live at all. A span of roughly 7 units means exposure was effectively pinned and a
`frames_exposure_dip` of 0 there is expected and carries no information; a span nearer 30 means
auto-exposure was adapting, and **a 0 in that case is a finding worth reporting, not a clean result.**
⛔ **Do not change any exposure setting on that box to make this read tidier** — the point of the
number is that it describes the host as the client runs it.

---

# SECTION G — `m49` LABEL-vs-PIXEL VERIFIER, RUN ON THE HOST BOX

✅ **RDP-valid.** Four steps, one of them optional-until-later. **Nothing is copied off that machine.**

🎯 **THE HOST IS BATES, RULED 2026-09-04 (session 076).** The flagged session — two texture-type
events labelled one frame early, masks correct — is on the **Bates** box, and the transcription goes
to `_bates_reads\`. Journal 075 §4 left this open because the two "−1 at onset" findings on record
were made against the other office host's M2 bundle; the owner has now stated which box holds the
session. ⛔ **The Concorde findings are the CLIENT's M2 review and are NOT read through this section**
— they are read only through the verifier the client runs, or through a run of it on that host.

## G-0. WHY THIS SECTION EXISTS, AND THE ONE RULE THAT SHAPES IT

The `m49` verifier answers the question the client actually asks: **does the first labelled frame
match the first frame whose pixels change, and is the frame after the last labelled one clean?**

🚨 **THE SESSION NEVER LEAVES THE HOST MACHINE. THE ONLY THING THAT COMES BACK IS NUMBERS THE OWNER
TYPES.** That is why the tool runs *there* rather than the session being copied *here*.

⚠ **THIS IS THE THIRD ATTEMPT AND THE FIRST TWO WERE NOT FAILURES — THEY WERE UNRUNNABLE.** The
`m49` A1 and A2 campaigns both declared this gate a required read and both had to report **UNRUNNABLE,
never passed**, because they were waiting for a session folder to arrive at
`D:\IntrusiveAnomalies\_bates_reads\` and none ever did (journals 072-02 §, 073 §, 074 §6). **Section G
removes that dependency: the reading is produced where the data already is.**

📌 **A reading is the deliverable. A PASS is not required for this section to have done its job** —
an `ONSET-SHIFT` here is exactly the finding the whole `m49` milestone was built to surface.

## G-1. Update the box

`git status` clean → pull → **confirm the SHA**. As in `E-1`/`F-0`.

```
git -C <plugin-repo> status --short
git -C <plugin-repo> pull
git -C <plugin-repo> log --oneline -1
```

**REQUIRED: the tip is `0d05c4b` or later.** *(That commit is `m49` phase A2's docs close-out; the
verifier itself has shipped since `b062832`, but the A2 tip is what this card was written against.)*
⚠ **No rebuild is needed for G-2 or G-3** — the verifier is a Python script and reads an
already-captured session. A rebuild is only needed for **G-4**.

## G-2. Run the verifier, READ-ONLY, on the session the owner flagged

**Which session.** The one the owner flagged as *"the anomaly appears one frame before the label
says"* — the session containing **two texture-type events early in the run**
(`missing_texture` / `corrupted_texture`).

⚠ **NAMING IT IS THE OWNER'S STEP AND THIS CARD CANNOT DO IT** — this box has never seen either
host's disk. If you cannot identify that session with confidence, **say so and stop**; running the
gate on the wrong session produces a confident number about the wrong thing, which is worse than no
number. If more than one candidate exists, run it on each and label the readings.

**The command.** Use whichever copy of the tool that box has:

```
:: from a delivered bundle
python <delivery-root>\host-tools\verify_capture.py --label-pixel-gate --report-only --dir <sessionDir>

:: from the plugin checkout you just pulled
python <plugin-repo>\tools\verify_capture.py --label-pixel-gate --report-only --dir <sessionDir>
```

🚨 **`--report-only` IS NOT OPTIONAL HERE.** It prints every reading and **always exits 0**, so the
step cannot end early on a non-zero exit before the per-event lines have been read. The exit code is
not what this section is collecting; **the lines are.**

⚠ **PROVE IT CAN FAIL FIRST — one extra command, and it takes seconds:**

```
python <same path>\verify_capture.py --label-pixel-gate --selftest
```

**It must print `SELFTEST: OK`.** *A gate that has never been shown to fire cannot make a clean read
mean anything (`G96`). If the selftest does not print OK, everything below is void — report that
instead.*

⚠ **If it refuses with a message about `measure_label_offset.py`, that file is missing beside it.**
The gate REFUSES rather than running degraded, on purpose. Copy `measure_label_offset.py` from the
same `tools/` or `host-tools/` folder and re-run. **Do not work around it.**

## G-3. Transcribe the reading — numbers only

Write into a NEW file on **this box's** shared folder:

```
D:\IntrusiveAnomalies\_bates_reads\2026-09-0X-bates-verifier-read.md
```

*(Replace `X` with the day. If the session came from the other office host, name the file for that
host instead — the folder is the drop point, not a claim about which host produced the read.)*

**Transcribe, verbatim and complete:**

1. the **header block** — the `session`, `frames / labels / events`, `masks present`,
   `diff threshold`, `edge search window` and **`MEASURABLE RANGE`** lines;
2. the **`baseline frames`** line;
3. **EVERY per-event line, in full.** Each is one line and looks like:
   `idx=<n> <anomaly_type> <target_name>  <VERDICT>  [HIGH|LOW] tau=<n> base=<n> [CONTAMINATED=<n>] [appearance=<n>]`
   — where `<VERDICT>` is one of `PASS`, `ONSET-SHIFT(±n)`, `END-SHIFT(±n)`, `NOT-VISIBLE`,
   `NOT-MEASURABLE(<why>)`. **Include the bracketed reason on any non-PASS line.**
4. the **summary** line — `PASS n   SHIFT n   NOT-VISIBLE n   NOT-MEASURABLE n   (of n event(s))`;
5. the **`VERDICT`** line.

⛔ **Do NOT copy the session folder, any frame, `annotation.json`, `labels.jsonl`, a log, or a
screenshot of the game.** The transcription is the entire channel.
⛔ **Do NOT re-run to get a tidier reading.** Whatever it says the first time is the result; if you
run it twice, report both.

### 🔴 THE PRE-DECLARED READING — read this BEFORE running G-2, and do not adjust it afterwards

**PRE-DECLARED, 2026-09-04, before any run of this card:**

| line | expected |
| --- | --- |
| the two flagged texture events | **`ONSET-SHIFT(-1)`** — the pixels changed ONE FRAME BEFORE the label said so |
| every other event | **`PASS`** |
| a truncated FINAL event, if the run has one | **`NOT-MEASURABLE`**, with its reason printed |
| summary | `SHIFT 2`, and `NOT-VISIBLE 0` |

🔑 **The sign matters and is easy to get backwards: `n` is SIGNED and NEGATIVE means the PIXELS moved
FIRST.** `ONSET-SHIFT(-1)` = the anomaly is visible on frame `start_frame − 1`.

⛔ **NO MECHANISM IS ASSERTED HERE.** A first-use shader-compile stall on that build (it predates
`m47`'s shader-readiness work) is a **CANDIDATE explanation and nothing more** — it has not been
measured on that host, and an observation and its explanation are separate claims (`G120`).
**Whatever the tool prints is the result; the candidate is not a reason to prefer one reading.**

⚠ **ANY OTHER READING IS A RESULT, NOT A FAILURE OF THE STEP, AND IS NOT RE-RUN:**

- **All `PASS`** ⇒ the flagged events do not reproduce under a pixel-ground-truth reading. That is a
  finding: it would mean the original observation was a numbering or playback difference rather than a
  label offset, and it must be reported as loudly as a shift.
- **`NOT-MEASURABLE` on the flagged events** ⇒ the gate could not judge them, usually because the
  clean gap between windows is too small to measure the offset (`MEASURABLE RANGE` says how small) or
  the camera was moving. **That is an unread surface, not a pass** — transcribe the reason.
- **`CONTAMINATED=n` / `[LOW]` on a line** ⇒ that row's baseline had frames above threshold, so its
  confidence is low. **Transcribe it; do not drop the row and do not promote it.**
- **A shift on an event nobody flagged** ⇒ report it. It is new information.

## G-4. AFTER an `m47`-or-later build exists on that box — the same recipe again

⛔ **DO NOT DO THIS IN THE SAME SITTING AS G-2.** G-2 reads the build the owner already has; G-4 reads
a NEW build. Running them together makes the comparison meaningless — **one variable at a time.**

1. Rebuild the **EDITOR** target on that box (runbook §8.6 STEP 3.5 is not optional — `G47`).
2. Capture again with **the same recipe as the flagged session** — same map, same anomaly, same
   `IAI.Capture.Config`, same frame count, same seed if it is known. ⚠ **If the original recipe is
   not known, say so** — an unmatched re-capture is a different experiment and cannot answer G-4.
3. Run the same G-2 command on the new session and transcribe it the same way, into a file named for
   the new date, **beside the G-3 file and never overwriting it.**

**PRE-DECLARED for G-4: `PASS` on every event, `SHIFT 0`, `NOT-VISIBLE 0`.**
⚠ **A remaining shift there is the more valuable outcome of the two** — it would say the timing class
is not what the newer build fixed, and that is a finding this project has no other way to obtain.
🚨 **Keep the G-3 reading. It is the BEFORE picture and there is no way to reconstruct it once the box
is rebuilt.**
