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

⛔ **Do not run Section B until m36's home gates pass.** A census leg on an unproven build produces
numbers that look like data.

### B-1. The leg

Same as A-5, with the census on:

```
IAI.Capture.Census 1
IAI.Capture.Config 2 4 8 4 0
IAI.Capture.Start "" png 4242 90 blinking
```

### B-2. StartRun echo — read it BEFORE anything else

```
=== Capture(census): EFFECTIVE FOR THIS RUN - census on ... floor=6.00% ... maxVerdictAgeTicks=12 ... excludeTranslucent=1 ... reservation=1 reserved=N [values]
```

**Read back:** `census on/off`, `floor`, `maxVerdictAgeTicks`, `excludeTranslucent`, and
**`reserved=N` plus the list of values**.
🚨 **If this line is absent, or says `census off`, the leg is VOID — not a pass.** A census that
silently did not run reads exactly like a clean result (`G139`).
📌 **`reserved=N [values]` IS the full custom-depth census** A-7 could only partly answer: every
component on that host already writing custom depth in 200–254 that the plugin did not tag.
**Read it for both hosts.**

### B-3. The typed counters

From `run_summary.json`:

| field | expected on a healthy Bates leg |
|---|---|
| `census_candidates` | **≥ 1** — below that the leg counts toward **no** prediction |
| `census_unmeasurable_nanite` | **0** — Bates has no Nanite at all. **Non-zero means the classifier is misfiring; report it, do not re-run to a green** |
| `census_zero` | a number |
| `census_below_floor` | a number |
| `census_excluded_translucent` | a number |
| `census_fires_fallback_all` | **0** on a healthy leg. Non-zero = the loud-inert path fired — **report it, do not re-run** |
| `vetoed_events` | banked Bates band is **12–15** per run. Census-ON expectation is a **marked drop**. ⚠ **This is REPORTED, NOT GATED** — the veto is the backstop and residual vetoes are it working |

**Read back: seven numbers.**

### B-4. The histogram read

The census logs a per-cycle summary. **Read back, for the run:**
1. **number of census cycles** completed;
2. **candidates per cycle** — the low and the high (two numbers, not every cycle);
3. **the verdict split** — how many `MEASURED_ZERO`, `MEASURED_NONZERO`, `NOT_MEASURABLE` in total.

That is five numbers and it is enough to see the shape: a census whose candidate count collapses
after cycle 1, or whose split is nearly all `NOT_MEASURABLE`, is measuring almost nothing even
though every counter is populated.

### B-5. The eye judgment — the product definition

**Type a list: each fired target's name, and `visible: yes/no`** — could a viewer see that object
change?

✅ **This one IS RDP-valid.** It judges *what was selected*, not how smoothly it rendered. A target
either is or is not a thing a viewer can see change, and RDP does not alter that.
⛔ Do not use this to judge stutter. That is the physical-only gate.

---

## C. IF SOMETHING FAILS

**Report and stop. Do not re-run to a green, and do not fix on the box.**

Anything on this card that fails is a result. The most useful thing you can send back is the
numbers you actually saw plus the line that surprised you — not a second attempt that looked
better. Several findings in this project came from a leg that failed once and was reported verbatim.
