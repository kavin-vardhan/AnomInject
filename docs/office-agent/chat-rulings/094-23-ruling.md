# 094-23 — Chat ruling: the launcher fix works, but one bug blocks the real run (G1); 094-24 fixes it and re-arms this evening, and the real run (094-25) follows after midnight; the owner narrows the port to 4.25 only

**Date:** 2026-10-28, about 17:25 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Codex's report:** `_reviews\094-23-codex-report.md` (10:47–15:55 IST, 18,435 s over 2 tries, exit 0). Try 1 ended at 13:33 on an OpenAI "model at capacity" error; the relay resumed it at 13:40. Its verdict: `GREEN | EX1–EX5 kit re-armed and dry-run; live launcher gate and closeout pass; zero Unreal launches`.
- **Claude Code's review:** `_reviews\094-23-code-review.md`. 41 rows: 35 OK · 4 N/A · 1 OPEN (carried) · 1 GAP (G1, blocking) · 1 decision (D1) · 0 STOP. Also O2–O6.
- **Mailbox report 094-23:** NEEDS-DECISION, 395 min.
- **The owner, 10-28 about 12:30 IST:** "lets not do A8 to A12 now, 4.25 should be a separate branch in the plugin repo, 5.1 plugin remains as is."
- **The owner's standing OK:** unattended editor runs when the PC is free are the norm. The PC is free tonight from about 23:00 until morning.

## Verdict
**EX1–EX5 are ACCEPTED as built.**
- **EX1:** launcher ancestry is re-read at every poll, with a PID-reuse guard, and the image-name exemptions are gone.
  - **Live:** Code's independent walk equalled the kit's on three polls. Both excluded the same 9 PIDs (launcher 26116, 3 web helpers, 5 overlays) and found 0 foreign blockers. Codex's 12:48 reading agrees.
- **EX2:** the start gate is unchanged. In-session, only free commit below 6 GiB, a drive floor and the 1,800 s limit act. Each stopped the owned tree, including a detached grandchild, in 2–3 s, and a sibling survived.
- **EX3:** quarantine by receipt, declared partial outputs only. Pre-existing, hard-linked or changed files refuse, and a retry refuses until chat re-arms.
- **EX5:** the wording says "armed for EW2".

**But the 09423 arming cannot be used (Code's G1).**
- The variable `EUROPA_AUTHORING_MARKER` contains "AUTH", so the kit's own secret-name filter drops it before the helper reads it.
- On the real branch the helper then fails (`KeyError`, `fixture_leg.py:129`) just before the editor would start. That spends a prelaunch slot, and EA_Travel never runs.
- The failure is fail-safe: no launch, no reservation, no quarantine.
- **No dry run could catch it:** dry runs return earlier (`fixture_leg.py:122–125`). Code found it by driving the real entry function with the session step stubbed.

**Protected state, all unchanged:**
- P9 v2 18,264 of 18,264, and `m51` equal to P9;
- 110 of 110 bound artifacts, the three manifests, Paris Content and the probe sources;
- the canary found 0 times, and EK1 found 0 secret values in 45,724 files;
- the command audit: 118 wrapped commands, 1 direct read-only, 0 direct write-capable;
- 0 launches.

**The capacity interruption was handled right:** the interrupted scan was kept as evidence, its processes were proven gone, and the scan was re-run. Nothing was invented.

## Rulings

### EY1. G1: remove the variable, close the class, and rehearse the real branch (Code's D2, option a)
**The fix:** drop `EUROPA_AUTHORING_MARKER`.
- `fixture_leg` derives the marker path itself (`<state>\authoring-started.json`).
- `disk_pause` keeps validating the same path from its own argument.

**Close the class, not just this instance:**
- a static check lists every environment variable the kit's Python helpers read;
- each must be set in the same process, or survive `filtered()`;
- a mutant that reads a filtered name must fail the check.

**The real-branch rehearsal (it closes the blind spot that hid G1):**
- **What it drives:** the same path a real run takes, for both recipes:
  - the `.bat`;
  - `fixture_leg.main()`'s non-dry branch;
  - the marker hand-off and the reservation;
  - the session gate and the deadline;
  - the post-session qualification;
  - the spent lock and the quarantine.
- **What it uses instead of the editor:** a harmless stand-in child.
- **Where it runs:** every root (host, Content, receipts, TEMP, state) is under owned scratch. Never the Paris host, and never a real slot.
- **The variants:**
  - **passing:** the stand-in writes synthetic declared outputs and probe rows (the skipped cleanup row and one `probe_stop`). The kit must reach `clean exit and output checks passed` and spend the recipe;
  - **failing:** the stand-in leaves a partial output and exits like a crash. The kit must quarantine by receipt, spend the recipe and not start EA_Travel.
- **No Unreal process starts.** The armed kit refuses any non-armed root outside its test mode.

### EY2. EX1 is narrowed to the launcher's own programs (Code's D1, option c)
**The rule:** inside the launcher's process tree, a process is exempt only if its image is under the Epic Games Launcher's install folder or the Epic Online Services install folder, read from the live image paths.
- Anything else in the tree, such as an editor or game started from the launcher, is judged like any outside process.
- An unreadable image path means **not** exempt.

**Why:** EU3 exempted "the launcher's own processes", not what the launcher starts. An idle editor opened from the launcher would still compete with our session for memory.

**The live reading must show:** the five overlays and the web helpers exempt by folder, with 0 blockers.

**Code's O4** (a missing creation time skips the PID-reuse guard) becomes harmless: a misjudged link can only exempt a launcher or EOS image.

### EY3. The smaller items
- **O2:** an acting reading (free commit, or a drive's free space) that stays unreadable for 60 s stops our tree, like a breach. Shorter gaps are recorded only.
- **O3:**
  - **They block:** `UnrealPak` and `UnrealLightmass` outside the exemption. They mean a build or a cook is running.
  - **Recorded only:** `CrashReportClient(Editor)`, `UnrealInsights`, `UnrealTraceServer`, `UnrealVersionSelector`, `UnrealCEFSubProcess` and `EpicWebHelper`. `UnrealTraceServer` can stay resident after a 5.x editor closes.
- **Code's risk 3:** the start gate also needs **free commit of 12 GiB or more** on its three polls. A session must not start straight into EX2's 6 GiB commit stop.
- **O5 and O6:** accepted (the harness timeout and the interrupted scan).

### EY4. E:'s lines are re-based (amends ED1)
- **E::** start 15, warn 12, floor 10 GiB. It was 25 / 20 / 15.
- **Why:**
  - the port's charged E: growth has been 0 in every job;
  - E: swings by tens of GiB with the owner's own work (33.4 → 19.1 GiB on 10-28);
  - the lines protect Codex's thread store (`E:\CodexData`) and the frozen J2 cache, which need under 1 GiB per job.
- **Where it applies:** in the kit (re-armed in 094-24), and at every job's entry.
- **Unchanged:** C: (13 / 11 / 10) and D: (40 / 30 / 25).

### EY5. The owner's scope decision: the port is 4.25 only from now on
**The decision (owner, product scope):**
- A8 to A12 are not done now.
- The 4.25 port is a separate branch in the plugin repo.
- The 5.1 plugin stays as it is.

**What it means:**
1. **`feat/europa-port` is the 4.25 line.**
   - It stays a separate branch and is never merged into `master` or `release/2026-10-05`.
   - Its name stays: every receipt, kit and ruling cites it.
   - It is pushed only by the owner, when chat asks (before D-lite).
2. **The 5.1 plugin is untouched.** `m51`, `master`, `release/2026-10-05` and the tags stay protected by P9 v2, exactly as now.
3. **094-00 P3 ("one source tree for both engines, 5.1 neutrality proven by gates") is superseded.**
   - The port no longer has to build or behave identically on 5.1.
   - **No clean-up churn:** the existing engine-version guards and 5.1 paths stay as they are. Removing them would change bound binaries for no gain.
   - **New code must build for 4.25 only:** stock 4.25 at home, and Europa's fork at the office.
   - **EA11 stays:** no `#if` in reflected headers (UHT safety on 4.25).
   - **P2's one-way rule stays:** anything from the 5.1 line comes into the port only as a deliberate, separate step.
4. **Dropped for now:** A8 (the 4.25 package), A9–A11 (the 5.1 re-gate), A12 (the final consistency check) and gates R0–R9.
   - They come back only by the owner's call. A packaging step may return later if the office needs a packaged build.
   - **Items carried for them lapse:**
     - O3, A8's package-output absence check;
     - EJ1 and EJ3, 5.1 ISPC actions and CaptureBench in the 5.1 Shipping exe;
     - EA13's level-2 package proof.
5. **The two 5.1 test hosts are retired.**
   - `D:\IA_Europa\Baseline51` and `D:\IA_Europa\ReGate51` get no more builds, no EO3 makefile refresh and no EI4 clean-engine query. EI4 applies only to a job that runs a 5.1 build, and none will.
   - Their EW1 manifests (`b05e6f8c…`, `3865fe0c…`) are frozen as a record.
   - They stay on disk, untouched. Removing them (to free D:) needs its own ruling and a receipt.
   - **The 09424 kit may keep checking them unchanged**, as the 09423 kit does. That is harmless. Kits from A7 on bind only the 4.25 manifest (`c501895c…`).
   - Their makefiles keep the old metadata names (EW3 G1). They are never refreshed again, so that never reaches a child.
6. **EO4 now means the 4.25 host only.** The static check runs on the 4.25 probe source and the tracked template.
7. **The full-parity target (EA2) is unchanged.** It is about what works on Europa, not about 5.1.
8. **Faster jobs:** a probe or plugin rebuild is now two builds (4.25 Editor and Game), not six.

**The new order:**
1. **A6, second half:** the kit fix (094-24, this evening), then the editor authors `EA_Core` and `EA_Travel` by itself (094-25, after midnight).
2. **A7:** the first 4.25 pictures, and the owner's eye check. **A7 closes Phase A.**
3. **Phase B:** the six basic anomalies, with 4.25-authored materials.
4. **D-lite:** the office compile and injection smoke on Europa's fork. The owner pushes the branch when chat asks.
5. **Then C → D → E → F,** each with its own ruling.

### EY6. 094-24, now: fix, rehearse, re-arm for 09424, dry-run. No launch
**In order:**
1. EY1–EY4;
2. the rehearsal;
3. re-arm for 09424, with fresh slots, EW2's scope, and the 09423 arming refused;
4. dry runs of both recipes;
5. a live, observe-only gate reading (EY2).

D2 applies: two attempts per harness problem.

### EY7. 094-25, tonight: the real authoring run (pre-ruled, if 094-24 is GREEN)
**What it runs:** the 09424 kit's real entry point, once: `EA_Core`, then `EA_Travel`. At most two editor sessions, one per recipe, each under its 1,800 s limit. Nothing else launches.

**When:**
- **The window:** not before 23:00 IST. No new start of the entry point after 04:30 IST.
- **Who decides when:** the kit's measured gate decides when each session starts:
  - human idle for 300 s;
  - 6 GiB free physical and 12 GiB free commit;
  - GPU at most 15 %;
  - no blocker outside EY2's exemption;
  - three passing polls.
- **A gate timeout is not an attempt.** If the kit ends with exactly `Stopped: no measured idle window within four hours. No editor was started.`, Codex may start it again on the next prelaunch slot, inside the window, and never beyond the 3 armed slots per recipe.
- **Any other `Stopped:` line ends the run.** No retry, and no fix in that job.

**What the run must show, per recipe:**
- **A clean exit:**
  - the probe's skipped cleanup row and exactly one `probe_stop`;
  - no PIE;
  - the editor's exit code, and how it ended;
  - every error and ensure in `Runtime.log`, classified as engine or host content, or our plugin or probe.
- **The outputs:**
  - exactly the declared packages are new, with sizes and hashes;
  - nothing else under Content is new or changed (no BuiltData, no extra package);
  - the 1,458 existing Content files, the config files, the descriptors and the `.uproject` are unchanged.
- **The authored content:** the authoring receipt matches the committed A5 recipe and manifest, item by item.
- **No uploads:**
  - the EU4 switch is in every recorded argv;
  - the crash-client and session logs show no upload;
  - any report folder is listed.
- **The EditorRuns markers** follow ET2's class, with 31124 unchanged.
- **The ledger:** C:, E: and 5.1-engine writes inside the EA4, EG1 and ET3 classes.
- **A failed session:** EX3's quarantine receipt.

**Stops:**
- **A crash is a STOP:** no further session, quarantine per EX3, then report.
- **Any change outside the three declared outputs** is a STOP.
- **Only the kit launches the editor.** The editor's own ShaderCompileWorker and crash-client children belong to its owned tree. Any other launch is a STOP.
- **No source change, build, re-arm or kit edit** in that job.

**Verdict meanings:**
- **GREEN:** both recipes completed, every check passes, the canary is absent, P9 v2 is unchanged, and there is no blocking GAP.
- **NEEDS-DECISION:** no session started (no idle window), or only `EA_Core` completed.
- **RED:** a crash, a protected-state change, or an upload.

### EY8. Carried
- **For A7:**
  - EO1's reader budget;
  - EV6's glass baseline (A7 masks glass);
  - EJ5's 6 GiB physical rule at the start gate;
  - EX2's note that measured legs get their own in-session rules;
  - Code's O1, the clean exit at runtime, which 094-25 proves.
- **Carried:** O2b.
- **Lapsed with EY5:** A9, O3 (for A8), EJ1 and EJ3.

## For the owner
**Nothing to run now.** The fix job runs this evening and needs nothing from you.

**Tonight, before you leave the PC:**
1. **Close Blender completely.** With it open today, free memory fell close to the line where the run stops itself.
2. **Close anything Unreal or DCDemo,** including anything opened from the Epic Games Launcher. Pause the DCDemo watcher. Leave the GDP watcher running.
3. **Keep the Epic Games Launcher open.**
4. **Check E: isn't still filling up.** It needs 15 GiB free or more.
5. **Leave the PC and the Claude app on.** No restart is needed.

**What happens:** after midnight, once the PC has been quiet for 5 minutes, the old editor opens by itself, builds the first test map, closes, then does the second. It takes about 1½–2½ hours.

**In the morning:**
- If an Unreal editor window is still open, don't touch it. It closes itself within 30 minutes.
- If a crash box is showing, click **Close Without Sending** and tell chat.
