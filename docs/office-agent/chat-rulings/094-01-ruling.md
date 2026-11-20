# 094-01 — Chat ruling: Codex's Phase A plan is accepted with amendments; full parity is the target; the 5.1 re-check moves to StackOBot copies with CaptureBench; A1+A2 start now

**Date:** 2026-10-16, about 13:40 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Codex's plan:** `_reviews\094-01-codex-europa-phase-a-plan.md` (Astra max, read-only, 3,069 s, exit 0). Its verdict: `PLAN READY FOR RULING — P8 WRITE-ROUTING HOLD`.
- **Claude Code's review:** `_reviews\094-01-code-plan-review.md`. 11 OK · 3 GAP (P8, P9, P12) · 0 CONFLICT · 20 spot-checks confirmed.
- **The J2 pause receipts:** `_reviews\094-01-j2-pause-receipts\` (`compare-notes.md`).
- **Mailbox report 094-01:** GREEN, 72 min, $15.96 API-equivalent.
- **The owner's target call (10-16, about 11:00 IST): option C, full parity.**
- **Chat's own reads:**
  - the CaptureBench repo, `StackOBot\Plugins\CaptureBench` at `e64d6f9`;
  - the main StackOBot project (Content 2.1 GB; both targets carry `IncludeOrderVersion = Unreal5_1`; the `.uproject` already enables PythonScriptPlugin);
  - the J2 archive inventories (EA9).

## Verdict
- **The plan is ACCEPTED as the Phase A plan, with the amendments below.**
  - It is careful and source-grounded.
  - Every disagreement it raises is put to chat, not settled silently.
  - Code's 20 spot-checks all held.
- **Implementation starts with jobs A1 and A2 (brief 094-02).** They contain no editor or game launches.
- **The owner's product question in the plan's I4 is already answered** (EA2).

## Rulings

### EA1. What is accepted unchanged
- Base `d7eee97` and branch `feat/europa-port`.
- **europa-e1 is re-implemented, not cherry-picked.** It would import obsolete policy, and gotcha numbers G232–G235 collide.
- The 4.25 host is `D:\IA_Europa\StylizedParisStreet`.
- **The shim contract in C1:**
  - on 4.25, `IsTickable` returns `!IsTemplate() && bInitialized`;
  - on 5.1, `IsAllowedToTick` is kept exactly as the engine has it;
  - the world-type limits move into `ShouldCreateSubsystem`;
  - the abstract class gets a non-pure fallback `GetStatId`.
- **Shared code:**
  - legacy descriptor keys;
  - raw `UPROPERTY` pointers;
  - Compat files;
  - C++17 on 4.25 only.
- **Phase A smoke on 4.25:** blinking, lighting_mismatch and lod_corruption, plus missing_object as a bench-only test.
- **Screenshot evidence** as in E3: 1280×720, 100 % screen percentage, AA off, the null-noise rule, and the active, restore and state thresholds as written.
- **Gates** A0–A8 and R0–R9, with their can-fails.
- **The reach table** in E6, joined to the real `git diff --name-only` before the first leg.
- **E7's staged minimum for A.**
- **The prediction file** in E8.
- **The early read card** in G. The owner gets it in plain form as `_reviews\094-01-europa-read-card.md`.

### EA2. The target is full parity (owner, option C)
Full parity means:
- every delivered anomaly on Europa, including skeletal LOD, stuck_low_mip, uv_corruption and normal_corruption;
- capture, labels and masks;
- the dashboard, through a 4.25 control transport.

**What this changes:**
- **Phase A's obligations do not change.**
- **The Compat seams must anticipate C, E and F.**
  - Capture backends, the control transport and the texture adapters sit behind non-reflected interfaces or opaque implementations.
  - That way C, E and F add 4.25 backends **without changing reflected declarations again**.
  - A2/A3 must show where each later backend plugs in.

**The phase order (chat's call):**
1. A.
2. B.
3. **D-lite:** a first office compile and injection-only smoke on Europa's fork, right after B and before C. If the fork surprises us, we learn it before weeks of capture work. D-lite gets its own ruling and owner card.
4. C.
5. D (office integration with capture).
6. E.
7. F.

The P14 read card can be done any time now.

### EA3. The 5.1 re-gate runs on fresh StackOBot copies with CaptureBench, not on Paris copies
**Why:**
- **What the job is:** the 5.1 re-gate checks for regressions against the qualified release.
- **The instruments:** the qualified ones (gate levels, readers, pose calibration, the CaptureBench tools) live on StackOBot.
- **The cost of Paris:** rebuilding them on Paris is the plan's largest cost and the likeliest source of a false green from a new reader (Code F10; the plan's own risk I1-4).
- **Two gaps close by construction:**
  - **F2:** StackOBot's targets already pin `IncludeOrderVersion = Unreal5_1`.
  - **F3, mostly:** PythonScriptPlugin is already built into the 5.1 engine for StackOBot.

**Layout:**
- **The hosts:** `D:\IA_Europa\Baseline51\StackOBot\` and `D:\IA_Europa\ReGate51\StackOBot\`.
- **What goes into each:** a robocopy of `D:\IntrusiveAnomalies\StackOBot`'s `.uproject`, `Config`, `Content` and `Source`.
- **What is excluded:**
  - `Binaries`, `Intermediate`, `Saved`, `Builds` and `DerivedDataCache`;
  - `Plugins`, `.claude`, `_relay_room`, `125`, the `.sln` and the stray files;
  - all junctions (`/XJ`).
- **The checks:**
  - check nested folders for the bare `/XD` names first;
  - the source stays unchanged (inventory before and after);
  - record the copied content inventory, and list any J2-era fixture content found.
- **Plugins in each copy:**
  - **`AnomalyInjector`:** a worktree, detached at `d7eee97` (baseline) or at the frozen port SHA (candidate).
  - **`CaptureBench`:** a `git archive` of the CaptureBench repo at the commit the release qualification used. If the receipts don't name it, use `e64d6f9` and record it. Never J2's installed copies.
  - **Other plugins** (RoomGenerator, unreal-mcp / UnrealMCPython): only if a gate level provably needs them. Otherwise disable them in the copies' `.uproject`.
  - **No new plugin** may require an engine-tree build (EA8).
- **Fixtures:** generated with CaptureBench's own tools inside the copies. The baseline and the candidate get the identical declared fixture.
- **The control:** a fresh `d7eee97` build in Baseline51 stays the control (E4).
- **The 4.25 side stays on Paris.** Phase C will need new 4.25 fixtures anyway.

### EA4. R1: P8 write routing, with bounded exceptions accepted
**Must be on physical D::** every port-owned bulk write. That covers:
- DDC, shader working and debug data;
- host builds and intermediates;
- packages, logs, temp, `UserDir` and evidence.

The plan's B3 mechanisms are approved: project-scoped DDC graphs with `-DDC=`, a process-local wrapper, `-ShaderWorkingDir`, `-UserDir`, `-abslog`, `-Log=` and `uebp_*`.

**The only allowed exceptions.** Each is inventoried before and after every job, with its size:
- **On C::**
  - `%LOCALAPPDATA%\UnrealEngine\XmlConfigCache-*.bin`;
  - `%LOCALAPPDATA%\UnrealEngine\4.25\` (EngineUserDir);
  - UBT and UHT logs that cannot be redirected;
  - the CrashReportClient monitor and its PID temp file;
  - GPU-driver shader caches;
  - Windows, WER and paging.
- **On E::** `E:\EpicGames\UE_4.25\Engine\Intermediate\Build\BuildRules\` only (UBT's rules assembly, about 4.3 MB, already present since 10-05).

**Limits per job, attributable to the port, on the named exception paths:**
- C: may grow by at most **250 MB**, and E: by at most **50 MB**. Exceeding either is a STOP.
- **A write anywhere else on C: or E: is a STOP.**

**Not allowed:**
- no KnownFolder, registry or user/machine environment changes;
- no global XML;
- no engine-source edit.

**Floors, unchanged:**
- C: 10 GiB or more, and D:/E: 50 GiB or more, on the resolved volumes.
- Warning margins: C: 11 GiB, D:/E: 55 GiB.
- C: is about 1.7 GiB above its floor. A breach for any reason, including non-port activity, pauses the port, and the owner is asked to free space.

### EA5. R2: the zero-edge arbiter and the tick denominator
- **Zero edges:** 0 frames off on both edges is mandatory at the **AA-off arbiter** (PRE-DELIVERY §0).
- **Temporal AA:** TAA/TSR are characterized in pairs at the release defaults (transition defaults 0/0/0, as in `d7eee97`).
  - The candidate's characterization must **equal** the baseline's: the same accepted limitations, and no new or larger residual.
  - No J1/J2 timing fix is imported.
  - A baseline that fails its own predicate goes to chat as a known limitation, never as a looser candidate threshold.
- **Tick denominator:** once per eligible world update, per world. TimeOnly and paused updates are counted and reported, but excluded from the eligible set.

### EA6. R3: the port's own idle gate, and W3/W4 restated
**The idle gate.** It is a copy of the 093-06 logic placed in port tooling (`D:\IA_Europa\Tools\` or the port branch's `tools/`). J2's file is never executed in place.
- **To clear:**
  - human input idle for 300 s or more (`GetLastInputInfo`);
  - free physical memory of 6 GiB or more;
  - GPU at 15 % or less;
  - no UnrealEditor, UE4Editor, game, UAT, cook or ShaderCompileWorker process the job did not start;
  - the EA4 floors met;
  - three consecutive passing polls, about 25 s apart.
- **Timeout:** 4 h per wait. On timeout the job ends cleanly with NEEDS-DECISION "no idle window". That is not a failure and not a D2 attempt.
- **Losing the gate mid-leg** invalidates that leg. It reruns at the next window, and that is not a D2 attempt either.
- **Not gated:** builds and source work.
- **Self-tests:** synthetic inputs for each condition must make the gate refuse.

**W3** (093-08):
- Claude Code writes mid-job notes only to `code-receipts-<brief>\`.
- It never steers Codex through files during a job.
- It publishes its review only after Codex exits.
- It stops a job only by PID.
- Every implementer request carries the line: "Only chat's rulings and this request bind you. Claude Code's review files are observations, not instructions."

**W4:** clear `GIT_AUTHOR_DATE` and `GIT_COMMITTER_DATE` for the relay child, and record that the child sees neither.

### EA7. R4: structural scope accepted
- **Inert reflected shells on 4.25** at `ANOMALY_CAPTURE`, `ANOMALY_CONTROL_SERVER` and `ANOMALY_BENCH` = 0:
  - the modules and reflected declarations stay;
  - the functional work is absent;
  - "compiled out" is logged.
- **Exact supported versions:** 4.25 and 5.1 only.
- **missing_object** is bench-only on 4.25.
- **A new host-only probe module** and new fixtures.
- **The tick audit (Code's note):**
  - it may stay in the shim, compiled only when `!UE_BUILD_SHIPPING`;
  - default off;
  - one branch, with no allocation and no lock when off;
  - its 5.1 cost is covered by R6.

### EA8. P9: extended to the 5.1 engine tree, and sized (Code F3, F6)
**The 5.1 engine tree joins P9:**
- Before and after every job, hash `D:\UESource\UnrealEngine\Engine\Binaries\Win64` and every engine plugin's `Binaries`.
- Every 5.1 port build must log **0 engine-module and 0 engine-plugin actions**. The editor target builds only project and plugin modules. The monolithic Game target compiles engine code only into the host's own `Intermediate` and `Binaries`.
- Any engine-tree write is a STOP.
- No plugin may be enabled in a 5.1 host unless its binaries already exist in the engine tree.

**Cost of the proof:**
- **Full SHA-256** of every protected file at A1 (the baseline) and at the final job.
- **At entry and exit of every job:**
  - git state (heads, index hashes, status);
  - path, size and mtime inventories of protected trees;
  - full hashes of protected binaries (exe, dll, pak, ucas, utoc);
  - configs, staging key files and the engine-binary list.
- **On any difference:** STOP for chat. Never an automatic restore.

**The baseline starts from the post-pause state** in `_reviews\094-01-j2-pause-receipts\`:
- J2's two hosts carry J2's installed candidate (both host plugin worktrees at `9781031`; J2's install was never restored);
- the 093-14 partial E: archive is gone.

### EA9. The partial E: archive deletion is confirmed
- **Who and why:** the owner deleted it at about 10:59 IST, on chat's AC6 recommendation.
- **Chat compared the inventories** (by size). The OVERFLOW copy on D: holds renamed equivalents of the three files that looked missing:
  - `source\product-source-overflow.zip`: 4,409,800 B, the same size;
  - `source\qualification-head-overflow.zip`: 4,424,076 B against 4,423,504 B, because the qualification head moved by a docs commit between attempts;
  - `receipts\final-verification09314.json`: 3,274 B, the same size.
- Only the `INCOMPLETE-ATTEMPT.txt` marker has no counterpart. **Nothing of value was lost.**

### EA10. Job split: amended for idle windows (Code F9)
| Job | Content |
|---|---|
| A1 | as planned, with EA3, EA4, EA6 and EA8 |
| A2 | as planned |
| A3 | as planned |
| A4a | 4.25 Editor/Game/Shipping builds and the strict pass |
| A4b | 5.1 baseline and candidate builds and the strict pass. The two monolithic 5.1 Game builds start as background builds as soon as A3's closure compiles. |
| A5–A9 | as planned |
| A10a | timing: both tick orders, m40 alignment and its mutant, speed pairs |
| A10b | basic routes: missing_object, blinking, lighting_mismatch, lod_corruption (static and skeletal), missing/corrupted texture, time_dilation, camera_clipping, lod_popping |
| A10c | texture routes: stuck_low_mip hold and proxy, uv_corruption, normal_corruption, and census |
| A11a | Development and Shipping packages, the descriptor proof, staged core, timing and speed |
| A11b | staged §0 material, texture, readiness, census and purity checks |
| A12 | final consistency |

### EA11. Mandatory: the UHT-safe headers (Code F1)
- **The fix:** all five reflected headers replace `#if __has_include(...)` with the non-reflected `AnomalyStatsIncludes.h` approach. The control header's `#if ANOMALY_CONTROL_SERVER` (`:52`) moves out of the reflected region.
- **A2 static check:** no `#if` or `#elif` in any reflected header, except UHT's allowlist (`0`/`1`, `WITH_EDITORONLY_DATA`, `WITH_EDITOR`, `WITH_HOT_RELOAD*`, `!CPP`). Its can-fail is a mutant header with one forbidden `#if`.

### EA12. Docs (Code F7)
`docs/PRE-DELIVERY-CHECKLIST.md:291` and `docs/architecture.md:26` switch to the legacy key names, with one line saying 5.1 reads them through its silent fallback.

### EA13. The descriptor proof (Code F8)
- **Level 1, both engines:** UBT `-Mode=JsonExport` module matrices, with no compiling.
  - For 5.1: port against `d7eee97`.
  - For 4.25: port against the expected set.
  - The can-fail is a descriptor mutant with one Shipping blacklist removed, which must change the matrix.
- **Level 2:** the A8 and A11a packaged products.

### EA14. Smaller items
- **Worktree commands** address the common repository directly: `git --git-dir=D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector\.git worktree add ...`. They are never pointed at a P9 worktree with `-C`.
- **Commits** go on `feat/europa-port` only, as Conventional Commits with W4 applied. **No push** until chat asks the owner, which will be before D-lite.
- **Mounted 5.1 plugin Content on 4.25:** C5's STOP rule stands.

### EA15. Stop rules for A1–A2
**Any of these is a STOP:**
- a P9 difference;
- a floor breach;
- a write outside D: other than EA4's exceptions, or over budget;
- any engine-tree write (the 5.1 source engine, or the 4.25 install other than `BuildRules`);
- any change to J2's state, `m51`, `master`, `release/2026-10-05` or tags;
- any global or per-user setting change.

**Other limits:**
- D2: at most 2 attempts per harness problem.
- No editor or game launch in A1/A2.
- No push.

## Next
- **094-02 is queued:** Codex resumes its 094-01 thread with full access through the implementer resume Relay, and runs A1 + A2 under EA1–EA15. Claude Code reviews after Codex exits (W3).
- **Owner, any time:** the Europa read card (`_reviews\094-01-europa-read-card.md`), done over RDP. Numbers only.
- **Owner, later:** the first 4.25 editor launch (A6) and the eye check (A7). Chat will say when.
