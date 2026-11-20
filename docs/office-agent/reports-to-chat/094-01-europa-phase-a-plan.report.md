# REPORT for 094-01-europa-phase-a-plan.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 094-01-europa-phase-a-plan.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-10-16 10:57:15 | finished: 2026-10-16 12:08:59 | duration: 72 min | exit code: 0
context at last turn: 591404 tokens (59% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\094-01-europa-phase-a-plan.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN

**1. Plain summary for the owner**
- Phase A makes the plugin's basic anomaly injection build and run on the 4.25 Paris test project, and proves the existing 5.1 version still behaves exactly the same.
- Codex proposes 12 work sessions, about 5–8 working days.
- You'd do three things: a short read-only check card at the office (about 45–75 minutes), launch the 4.25 editor once and open one mesh preview, and look at four anomaly effects to confirm them.

**2. The J2 pause**
- **Processes:** nothing from J2 is running. There's no PID 35788, no Codex job on thread 01a13778, no `run_staged.py`, and no Unreal editor, game, build or shader-compiler process. The Codex processes present are the desktop app, started at 10:00.
- **Stop timeline:** confirmed as chat read it.
  - Codex exited with `1073807364` at 09:55:11, after 2,203 s.
  - `cook_candidate.py` started at 09:35:35 and only ever polled the idle gate (`clear=false`); the last poll was at 09:54:59.
  - Every cook and stage folder under `_r9315` has 0 files, so the cook never started.
  - 093-15 has no `_final` report file.
- **Receipts:** written to `_reviews\094-01-j2-pause-receipts\` (notes in `compare-notes.md`). The two expected differences are there: `fix/followup-capture` went `0e50b56` → `8fe108d` → `7754fd2` (remote still `1bf7054`), and `_r9315` is new.
- **Three unexpected differences, reported and not repaired:**
  1. Both 5.1 host plugin worktrees moved to `9781031`.
  2. Both 5.1 hosts still have J2's candidate installed: StackOBot.exe `D8D30951`, LyraGame.exe `DFE6F164`, new DLLs, Review0936 and CaptureBench plugin folders, and 199 CaptureBench fixture files in Lyra's Content. J2's install step ran at 09:30, and it was stopped before restoring.
  3. The 093-14 partial archive on E: (26,577 files) is gone. Its parent folder changed at 10:58:59, about an hour after J2 stopped, so neither J2 nor this job removed it. It isn't in the Recycle Bin. Comparing my earlier listings: 26,571 of its files exist with the same size in the intact D: OVERFLOW copy. Two differ in size (revised scripts). Four are missing there: `INCOMPLETE-ATTEMPT.txt`, `final-verification09314.json` and two source zips.
- **Unchanged:** the live staging folder, StackOBot Content and Config, host config, every other archive and leg folder, and the main checkout (`m51` at `53bf725`, no changes).
- **Free space now:** C 11.74 GiB, D 159.45, E 57.26.

**3. The Codex job**
- Run folder `_relay\runs\2026-10-16-094-01-europa-phase-a-plan`. Astra (`gpt-6-astra`), effort max, read-only sandbox, exit 0, 3,069 s.
- Copied unedited to `_reviews\094-01-codex-europa-phase-a-plan.md`; ledger row added.
- Its verdict: `PLAN READY FOR RULING — P8 WRITE-ROUTING HOLD | base=d7eee97 | europa-e1=reimplement | 4.25 host=D:\IA_Europa\StylizedParisStreet | 5.1 host=D:\IA_Europa\ReGate51 | phase A jobs=12 | owner legs=3`

**4. The plan**
- **Base and europa-e1:** new branch `feat/europa-port` from `d7eee97`. It re-implements europa-e1's `1f5e305` rather than cherry-picking it, with the expected conflicts listed per file.
- **Hosts:** three copies of Paris on D: (2.757 GiB each before builds), each with its own plugin worktree:
  - `D:\IA_Europa\StylizedParisStreet` for 4.25;
  - `ReGate51`, the 5.1 candidate;
  - `Baseline51`, a fresh `d7eee97` build as the 5.1 control.
  - It reserves about 92 GiB on D:.
- **DDC and caches:** a project-scoped cache configuration in the D: copy's `DefaultEngine.ini`, selected with `-DDC=Europa425DDC`. A launch wrapper sets TEMP and TMP for that process only and passes `-ShaderWorkingDir`, `-UserDir`, `-abslog` and per-build `-Log=`.
- **Shim:** a literal `UCLASS(Abstract)` class deriving from `UWorldSubsystem` and `FTickableGameObject`.
  - On 4.25 the guard is `IsTickable` returning `!IsTemplate() && bInitialized`, and the class default object never ticks.
  - On 5.1 `IsAllowedToTick` is declared `final` without `override`, so 5.1 keeps exactly the engine's behaviour.
  - World-type limits move into `ShouldCreateSubsystem`.
- **Descriptor:** the legacy `WhitelistTargets` / `BlacklistTargetConfigurations` keys.
- **Phase A smoke on 4.25:** blinking, lighting_mismatch and lod_corruption, plus missing_object as a bench-only test, checked with engine screenshots and pixel thresholds. Capture, control server, shaders and the bench module compile to inert shells there.
- **Gates:** A0–A8 on 4.25 and R0–R9 on 5.1, each with a can-fail, and all twelve 5.1 routes exercised in PIE plus staged products.
- **Merge cost with J2:** 21 source files overlap.
- **Rulings it asks for:**
  - **R1:** strict all-on-D: routing is impossible with stock tools, so allow a bounded list of exceptions.
  - **R2:** zero frames off is measured with anti-aliasing off.
  - **R3:** supply the idle-gate recipe.
  - **R4:** accept inert shells and the cost.

**5. My review** (`_reviews\094-01-code-plan-review.md`): 11 OK, 3 GAP, 0 CONFLICT.
- **P12 GAP:** the plan treats W3/W4 as an idle-gate recipe. In `093-08-ruling.md:52–65`, W3 says Code's mid-job findings never steer Codex, and W4 says to clear the commit-date variables. The plan carries neither. The idle gate itself is J2's own 093-06 script, so the port needs its own idle-gate spec.
- **P9 GAP:** the shared 5.1 engine binaries (`D:\UESource\…\Engine\Binaries`) aren't protected. New 5.1 editor builds, and the Python/scripting plugins the plan enables, could relink files the J2 hosts use. Also, a full hash of every protected file at each job's start and end covers about 253 GiB in 326k files, and the plan doesn't size that.
- **P8 GAP:** the write list misses one measured E: write. The 10-05 proof build wrote 4.26 MB of UBT's rules files into `E:\EpicGames\UE_4.25\Engine\Intermediate\Build\BuildRules`. C: got only a 190-byte cache file and a 323 KB log. These sizes support R1's bounded-exception option.
- **Other findings:**
  - 4.25's header tool will reject all five plugin headers' `#if __has_include` lines (`HeaderParser.cpp:3517`). The plan's fix is right but says "if needed"; it is required.
  - The 5.1 Paris copies would build with `IncludeOrderVersion` defaulting to `Oldest`, not the release's `Unreal5_1`.
  - 4.25's build tool also has `-Mode=JsonExport`, which lists each target's modules without compiling; that is a cheap descriptor proof.
  - Two docs still name the modern descriptor key.
  - Job A4 carries about 3–4 h of build time, and A10/A11 are too big for one idle window.
- **Spot-checks:** I re-read 20 of the plan's load-bearing claims myself and all held. They include:
  - the 4.25 tick loop has no `IsAllowedToTick`;
  - 5.1's `WorldSubsystem.cpp:51–79`;
  - descriptor key parsing in both engines' runtime and build tool, and 5.1's fallback to the old keys is silent;
  - the 4.25 DDC config lines, override code and shader working folder;
  - `-UserDir`, the build-time XML cache and crash-reporter paths;
  - build-file version symbols;
  - the 4.25 API gaps;
  - the Mannequin and Paris sizes, the merge base and diff stats, the pointer count, and `Super::Tick`.

**6. New module dependencies proposed**
- None from the engine for the plugin.
- AnomalyCapture and AnomalyControlServer take AnomalyInjector as a public dependency, because the shim lives there.
- On 4.25 AnomalyShaders depends on Core only, and AnomalyBench on Core, CoreUObject and Engine.
- Host-only: a new `EuropaPhaseAProbe` module, and PythonScriptPlugin plus EditorScriptingUtilities enabled in the new hosts.

**7. Owner product questions**
- After the office card, choose the first Europa target: injection-only, the six-anomaly capture/mask set, or wider parity.
- Say whether that first delivery needs skeletal LOD and dashboard control.

**8. NEEDS-DECISION items** (chat rulings; nothing blocks this brief)
- Codex's R1–R4.
- A corrected W3/W4 carry, and a port idle-gate spec.
- Protecting the shared 5.1 engine tree, and whether port builds must cause zero engine rebuilds.
- How much hashing P9 needs per job.
- Whether the port may use the CaptureBench repo at a frozen commit, not J2's installed copies.
- Confirm with the owner that removing the 093-14 partial E: archive was intended, so the P9 baseline starts from the current state.

Separately: several connectors (Google Calendar and the plugin-hosted ones) need authorizing in an interactive session or claude.ai settings, and the pdf-viewer server failed to connect. None were needed here.
api-equivalent cost (not billed on Max): $15.96 | turns: 109 | running total across all briefs: $4,622.85
