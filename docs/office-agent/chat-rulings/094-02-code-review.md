# 094-02 — Claude Code review of Codex's Europa Phase A A1/A2 job (W3: written after Codex exited)

**Date:** 2026-10-16, 14:49–15:20 IST · **Reviewer:** Claude Code (mailbox brief 094-02-europa-phase-a-a1-a2.md)
**Job:** resume of thread `<uuid>` in `_relay\runs\2026-10-16-094-01-europa-phase-a-plan\`,
gpt-6-astra / max / danger-full-access (rollout `turn_context` ×3 from 13:28 IST), 13:28:38–14:48:36, 4,798 s, exit 0.
Codex's verdict: `NEEDS-DECISION | A1 write proof blocked; no hosts, worktrees or commits; A2 not executed`.
Codex's report: `_reviews\094-02-codex-report.md`. My receipts: `…\code-receipts-094-02\`.

## Summary
- Codex did the **P9 work and the drafts, then stopped before any setup**: it tried two whole-volume write observers
  (kernel-file ETW, NTFS USN journal), Windows denied both without elevation, and it judged EA4's "a write anywhere else
  on C: or E: is a STOP" unprovable. No host copy, worktree, branch, commit, UBT/UHT run or build happened.
- **P9 is unchanged**, by Codex's proof and by mine (independent re-hash 19,290/19,290; my own before/after snapshots
  identical on every protected list; 0 job-window writes in either engine tree).
- **No port-attributable write on any EA4 exception path.** C: and E: both ended below their warning margins
  (C: 10.27 GiB, 0.27 above the floor), driven by non-port activity.
- The stop is defensible but **stricter than the request's own step A1.4**, which specified a ledger of the EA4 exception
  paths, not a whole-volume observer. Chat must set the proof standard; see §3.
- Counts: **5 OK · 1 GAP (EA4) · 9 OPEN** (not reached because of the hold). 0 CONFLICT.

## 1. EA1–EA15
| Ruling | Evidence | Status |
|---|---|---|
| EA1 accepted design (base d7eee97, `feat/europa-port`, re-implement e1, 4.25 host path, shim, shared code, smoke set, gates, predictions) | Recorded in the uncommitted `predictions09402-draft.md` (lines 7–17). No branch: refs 71→71 identical, `feature_ref_exists=false`. | OPEN |
| EA2 full parity; seams for C/E/F | Report §"Later backends" names the plug-in points: F = `FAnomalyControlTransportState` (`Source/AnomalyControlServer/Private/Compat/`), E = texture-corruption/stuck-mip facades + capability profile, C = capture subsystem's opaque async/preview state "and backend factory". Draft `AnomalyCapabilities.h` has the capability struct. Nothing installed; the C "backend factory" does not exist yet. | OPEN |
| EA3 StackOBot copies + CaptureBench | Hosts not created. CaptureBench `e64d6f9…` chosen from `_r917\docs\sessions\2026-09-30-089-02b-m53-rebench-prep.md:76` (verified; it is a fixture-session receipt, the closest record to "the commit the qualification used"). Nested `/XD`-name and reparse check on StackOBot Config/Content/Source: 0 (verified). Note: StackOBot's root `Binaries` is also a junction to E: (irrelevant to EA3's copy set). | OPEN |
| EA4 routing, exception paths, budgets, floors | Ledger tool **not built**; routing wrappers/DDC graphs not installed; budgets "UNVERIFIED". My own exception-path inventories (before 13:26 / after 14:49): `%LOCALAPPDATA%\UnrealBuildTool` 0 changes; E: `UE_4.25\Engine\Intermediate\Build\BuildRules` 0 changes; `%LOCALAPPDATA%\UnrealEngine` 126/127 lines changed, **all UE 5.7** (Zen −25.5 MB, Analytics, UnrealTrace, 5.7 EditorSettings) — **0 entries for 4.25 or XmlConfigCache-E+…UE_4.25** (still dated 10-05). Floors never breached. | **GAP** |
| EA5 AA-off arbiter, paired TAA/TSR, per-world denominator | In the predictions draft (lines 7, 17, E4/E5 sections). Uncommitted. | OPEN |
| EA6 idle gate; W3/W4 | W3: I wrote only to `code-receipts-094-02\` during the job, no steering, review after exit. The request carried the W3 line. W4: child saw neither date variable (`launch-receipt.txt`); Codex recorded both null. Idle gate: `europa_idle_gate.py` is a **run-folder draft**, not in `D:\IA_Europa\Tools` or the branch's `tools/`; self-tests **38/38 PASS, re-run by me, identical case list**. Real collector never run. Design notes in §6. | OPEN |
| EA7 inert reflected shells | Draft only (shader/bench "compiled out" logs, ANOMALY_BENCH define). | OPEN |
| EA8 P9 incl. 5.1 engine tree | Full SHA-256 baseline: 707,775 files / 582,901,644,507 B, PASS, allowlist includes **all** of `D:\UESource\UnrealEngine` (stronger than EA8's Binaries). Exit: 707,775 inventories match + 25,866 binary/config re-hashes match + git state match. Mine: §2. | OK |
| EA9 partial E: archive gone | Pause comparison: 23 receipts (22 present, 1 expected-absent), 139 full key hashes, 0 differences. | OK |
| EA10 job split | Predictions draft line 31: 16-job split as ruled; the job stayed inside A1. | OK |
| EA11 UHT-safe headers + static check + mutant | Checker draft + mutant: self-test PASS. **I re-ran it on the release tree:** 6 reflected headers, 12 forbidden directives (11 `__has_include`, 1 `#if ANOMALY_CONTROL_SERVER` at control header :52), identical to Codex. Draft patch → 0 in memory only. Not applied to any worktree. | OPEN |
| EA12 two doc lines | In the A2 draft (inserted line matches EA12's wording). Not applied. | OPEN |
| EA13 JsonExport level 1 + mutant | **Not run** on either engine. Planned mutant: remove AnomalyShaders' Shipping blacklist. Codex preflight: both engines' rules assemblies are newer than every listed source. I could not re-run anything: no approved wrapper exists. | OPEN |
| EA14 worktree addressing, commits, no push | No worktree, commit or push. Refs and all worktree heads + index SHA-256 identical before/after (my snapshots). | OK |
| EA15 stop rules | No P9 difference, no floor breach, no engine-tree write, no write outside D: by the job, no J2/m51/master/release/tag change, no setting change found. D2: write-observer problem 2 attempts (ETW, USN) then stop; RoomGenerator-junction harness fix 1. No editor/game launch. **Observation:** the ETW attempt allocated a system trace session ("Access is denied" but a session was created); Codex stopped it, and I confirmed `logman query -ets` shows no Europa/09402 session. Not a setting, but a system-scope action no ruling anticipated. | OK |

## 2. Independent checks
- **P9 re-hash** (`p9_rehash.py`, `p9-rehash-result.json`): every protected exe/dll/pak/ucas/utoc in Codex's
  `full-sha256.jsonl` (19,270 files, 32.1 GB) plus 20 seeded-random protected files (seed 9402; incl. engine source,
  Lyra/StackOBot host Intermediate, J2 evidence PNGs, the 09314 OVERFLOW archive, engine DDC) → **19,290 OK, 0 hash,
  0 size, 0 missing, 0 mtime differences**, 62 s. Baseline file SHA-256 matches Codex's stated `db884f1e…` source.
- **J2 / git:** 7 worktrees — m51 53bf725 (0 tracked, 15 untracked owner files), `_r917` d7eee97, `_r933` 7754fd2,
  `_r53_src` 4c93e5f, `_r53_host` 9781031, Lyra 9781031, `_r52p_src` 5d6352b — heads, branches, index SHA-256/size/mtime
  and status counts identical before/after (read with `GIT_OPTIONAL_LOCKS=0`). 71 refs identical. m51 clean.
- **Other protected lists (mine, before 13:26 vs after 14:49):** staging key files 82 SHA-256 identical; J2 host
  binaries 69 SHA-256 identical; StackOBot `.uproject`/Config/Source hash-identical, Content list identical (2,743).
- **Engine trees:** 5.1 `Engine\Binaries\Win64` (1,607 files) and the Binaries of all 498 engine plugins: path/size/mtime
  identical; `Engine\Intermediate\Build\BuildRules`, `Engine\Programs\UnrealBuildTool`, `Engine\Saved` identical.
  Job-window scan (13:28:38–14:49, write **or** creation time): `D:\UESource\UnrealEngine` 339,409 files → 0;
  `E:\EpicGames\UE_4.25` 179,502 → 0. **UBT logs of the job: none exist (no UBT run), so the engine-action count is 0 by
  vacuity, not by measurement.**
- **Routing:** `E:\Unreal Projects\StylizedParisStreet` 1,511 files SHA-256-identical and 0 in-window writes;
  `%LOCALAPPDATA%\UnrealBuildTool` 0. Other in-window writes: `_reviews` 0, `_mailbox` 0, `ToCodex` 0, `_r9315` 0,
  `_relay` only the runner's ledger row; no new top-level folder on D: except `D:\IA_Europa` (13:36:18), none on E:.
- **Codex's write record reconciles exactly:** `D:\IA_Europa` 63 files / 525,297,091 B per `final-artifacts.json`, plus that
  receipt itself (17,411 B) = the 64 files / 525,314,502 B on disk; run-folder drafts 13 files = exactly 114,168 B.
- **Free space** (DriveInfo, GiB): C 12.93 → 10.27 · D 159.45 → 158.96 · E 56.77 → 54.29.
  - D: dipped to 139.1 at 14:01: the system-managed `D:\pagefile.sys` grew to 43.3 GB under memory pressure (physical
    free 1.43 GiB) and shrank back by 14:10.
  - E: −2.5 GiB: `E:\<personal-folder>` (the owner's UE 5.7 project) wrote 829 files / 1.68 GB in 14:10–14:25; none of the port
    or engine paths changed.
  - C: −2.66 GiB. Not explained by the folders I measured:
    - `%LOCALAPPDATA%\UnrealEngine`: net −25.6 MB, 13:26–14:49.
    - `C:\pagefile.sys`: 512 MB, unchanged since 09:56.
    - `%LOCALAPPDATA%\Temp`, the NVIDIA cache, `.codex` and `C:\ClaudeTemp`: between 14:13 and 14:46 (when C: fell
      another ~0.5 GiB) they moved by at most +12 MB together. Before 14:13 I have only "modified since 13:26" totals
      for them, not growth.

    Cause not established. The owner was active (UE 5.7 editor 13:42–~14:20, Chrome, Spotify).
- **Re-runs:** idle-gate self-tests 38/38 PASS (`idle-self-tests-rerun.json`); EA11 checker self-test PASS and release
  scan 6/12 (`ea11-rerun-release.json`). **JsonExport re-run: not possible** (never run, no approved wrapper).

## 3. The EA4 hold
- **What Codex required:** attribution of every C:/E: write, to prove "a write anywhere else on C: or E: is a STOP".
- **What the request specified (A1.4):** "a write-ledger tool that inventories the EA4 exception paths before and after
  every operation and enforces the 250 MB (C:) and 50 MB (E:) budgets." That is buildable without elevation; I ran the
  equivalent inventories myself in this job.
- **Why Codex's standard can't be met here:** kernel tracing and journal reads need elevation, which is a security
  setting (forbidden). Volume free space can't attribute writes either, because the owner's work, the OS (pagefile,
  services) and Codex's own CLI state move C:/E: concurrently (§2).
- **Recommended ruling (chat's call):** the "anywhere else" clause is enforced by a declared, targeted detector, not a
  whole-volume observer:
  1. Per-operation inventories of the EA4 exception paths, with byte deltas against the budgets.
  2. Job-window mtime/creation scans of both engine trees, the Paris source, `%LOCALAPPDATA%\UnrealEngine`,
     `%LOCALAPPDATA%\UnrealBuildTool`, `%TEMP%`, `%PROGRAMDATA%\Epic` and the drive roots.
  3. Child command-line receipts proving the redirect arguments (`-Log=`, `-UserDir`, `-ShaderWorkingDir`, `-DDC=`,
     TEMP/TMP, `uebp_*`).
  4. Volume free space as a floor monitor only, never as attribution.
  5. No ETW/USN or other system-scope observers.

## 4. Hosts, commits, A2
- **Hosts:** none created; both sources unchanged (§2); disabled-plugin choice not made yet. StackOBot `.uproject`
  enables CaptureBench, PythonScriptPlugin, UnrealMCPython, ControlRig, FullBodyIK, EnhancedInput, Metasound and
  GameplayInsights.
- **Commits:** none. `git diff --stat d7eee97..feat/europa-port` is not possible: the branch does not exist. No UBT ran,
  so prediction-before-build ordering is not yet testable. W4 held.
- **A2 draft** (`prepare_a2_09402.py`, dry-run only, 22 files; read in full):
  - Descriptor: both modern keys renamed. No other modern key exists in the release `.uplugin` (verified), so nothing
    is missed.
  - Shared C# helper `AnomalyEngineRules` (exact 4.25/5.1, C++17 on 4.25, `ANOMALY_PROFILE_BENCH`), called from all
    five Build.cs. Capture's `GetModuleDirectory` call is wrapped in `#if UE_5_1_OR_LATER && !UE_5_2_OR_LATER` (the
    symbol-reference issue, G233).
  - Control and Bench Build.cs are **rewritten wholesale**: AnomalyInjector becomes public; WebSocketNetworking,
    AnomalyCapture and Json become conditional; Bench gains `ANOMALY_BENCH`.
  - Stats includes move to non-reflected Compat headers. Control transport fields move into the opaque
    `FAnomalyControlTransportState`, with out-of-line normal and FVTableHelper constructors.
  - Field rewrite risk: the `Transport->` prefixing is a regex over the .cpp. I checked all 13 field names against
    d7eee97: **0 collisions** (none after `.`/`->`/`::`, none declared as locals).
  - Drafted C#/C++ and the two Python tools **contain comments**. The repo's comment-free invariant applies before any
    commit.
  - The draft writes LF without BOM (`utf-8-sig` read). Read the diffstat for BOM/encoding churn (G115) when it is
    applied.
- **5.1 effect classes when applied:**
  - New public defines on all five 5.1 modules (`ANOMALY_PROFILE_BENCH`, `ANOMALY_BENCH`): C, compile-definition reach.
  - Dependency-visibility changes in Capture/Control: N. JsonExport's "dependency closure inspected" comparison will
    show these, so they must be declared as expected differences **before** the matrix runs.

## 5. Where C, E and F plug in (Codex, unverified)
- F: `FAnomalyControlTransportState` (drafted).
- E: behind the existing texture/stuck-mip facades and `AnomalyCapabilities`.
- C: behind Capture's opaque async/preview state; no factory interface is drafted yet.

  This satisfies EA2's "say where" at the naming level only. A3 must show the actual non-reflected interfaces.

## 6. Top 5 risks for A3/A4
1. **C: headroom.** It is 0.27 GiB above the 10 GiB floor, and fell 2.66 GiB in 80 min from non-port activity. Under
   EA4, any breach pauses the port; the next job will likely start below the 11 GiB margin. E: is below 55 too.
2. **The EA4 proof standard is unresolved.** Without a ruling (§3), every later job hits the same hold.
3. **First real compile of the shared C# helper in 4.25's UBT, and 5.1 matrix neutrality:**
   - `Target.Version`, `CppStandardVersion.Cpp17` and `BuildException` in 4.25 rules;
   - the expected-difference list for 5.1's dependency closure and the new defines;
   - a 5.1 rules-assembly rebuild would be an engine-tree write.
4. **4.25 UHT on the reworked reflected headers** (A3/A4): the user-declared FVTableHelper constructor on the control
   subsystem, TUniquePtr to incomplete types in reflected classes (Capture ×3, Control ×1), and the shim's
   `PURE_VIRTUAL GetStatId`.
5. **Idle windows for A6+.**
   - Gate design: it exempts owned PIDs only, so ShaderCompileWorkers spawned by the job's own editor count as foreign
     unless descendants are exempted. Game detection is by three names, plus an `is_game` flag the collector never
     sets. The GPU counter path has never run.
   - Conditions: this job saw physical free memory at 1.4–3.4 GiB against the 6 GiB requirement, and the owner's
     KeepAlive UE 5.7 editor relaunched twice.

VERDICT: REVIEW DONE — 5 OK / 1 GAP (EA4) / 9 OPEN / 0 CONFLICT; P9 unchanged (Codex and independent 19,290/19,290); 0 port-attributable C:/E: writes; chat must rule the EA4 proof standard and C: headroom before A1 resumes
