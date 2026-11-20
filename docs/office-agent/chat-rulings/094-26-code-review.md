# 094-26 (retry 094-26r) — Claude Code review of the real A6 fixture-authoring run (W3: written after Codex exited)

**Date:** 2026-10-29, 20:25–21:15 IST · **Author:** Claude Code (mailbox brief 094-26r) · **Inputs:**
- Codex's report `_reviews\094-26-codex-report.md` (collected byte-identical with a header line): thread `01a14334`, resumed 13:58:54 – 20:25:00 IST, 23,166 s, 1 try, no capacity error, exit 0. Ledger row `094-01-europa-phase-a-plan-resume25`, written once by the relay. **Codex's verdict: `RED | EA_Core authoring failed; a bound config disappeared and shutdown analytics sending was recorded; EA_Travel was not started.`**
- Chat's `094-25-ruling.md` (FA1–FA8), `094-26-request.md`, `094-24-ruling.md` (EZ1–EZ6), `094-23-ruling.md` (EY7) and the rulings they bind.

**My receipts:** `code-receipts-094-26r\` (with `step0\`, `protect\`, `review\`, `preserved\`). Nothing was written under `CodeReview09426`. No account identifier is reproduced; the analytics UserID in the engine logs is redacted in every receipt of mine.

## Summary
- **The run happened, once, exactly as armed, and it failed safe.** After the restart (boot 13:16:14) Codex re-proved entry (EA8 PASS), waited under FA5 until the PC had been idle 10 minutes (pre-check PASS 18:30:21), and started the real `.bat` once. The kit's own gate passed at 18:56:34; `EA_Core` attempt 01's editor (PID 30564) ran 18:58:33–18:59:17 (43.4 s) and exited 0 by `QUIT_EDITOR`. The authoring receipt was never written, the kit quarantined by receipt (0 files, nothing moved), spent the slot, and stopped. `EA_Travel` never started. No retry, fix or re-arm.
- **I agree with RED, on independent evidence, for three reasons:**
  1. **G1 — our authoring launcher is broken under the editor's Python 2.7.** The generated `script\author.py` (`fixture_leg.py:79`) calls `module['author']` from the dict that `runpy.run_path` returned. The engine runs Python 2.7.14 (Runtime.log); `run_path` executes the file in a `_TempModule` and removes it from `sys.modules` on exit (`E:\EpicGames\UE_4.25\…\Lib\runpy.py:272–274`, `:44`); a freed Python 2 module's globals are set to `None`, so `os` was `None` at `author_425_fixture.py:9` → `AttributeError: 'NoneType' object has no attribute 'path'` (Runtime.log:666–671). Source-supported, not reproduced (no launch is allowed to me). **No rehearsal could catch it:** the real-branch rehearsal replaces the editor with a stand-in, so the in-editor Python path never ran before this session (the EY1/G1 blind spot again, one layer deeper).
  2. **G2 — an upload happened: usage analytics, not a crash report.** The EU4 switch is in the argv (child-001.json, Runtime.log:145), but it covers crash reports only. **Two** POSTs to `datarouter.ol.epicgames.com` were outstanding at shutdown: the editor's own (`AppID=UEEditor.Rocket.Release`, Runtime.log:1064–1065, AnalyticsET) and the crash reporter's (`AppID=CrashReporter.Release`, CrashClient.log:86 "EditorSessionSummary sent report. Type=Shutdown", :92–93). Codex's report text cites only the crash reporter's; its `result-evidence.json` does include the editor's.
  3. **G3 — a kit-bound config file disappeared:** `Saved\Config\CrashReportClient\UE4CC-Windows-57F07A00…\CrashReportClient.ini` (112 B, `ffd2f515…`). My before/after lists agree: Saved\Config 23 → 22, nothing else changed. Codex's attribution (stock two-day `PurgeOldCrashConfig`, `GenericPlatformCrashContext.cpp:363–397, 909–932`) is source-supported; it is engine-managed state that the kit binds as "existing config". Not restored, correctly.
- **Everything protected is unchanged:** P9 v2 (Codex's EA8 entry and exit PASS; file `8e3ed095…` unchanged), `m51` = P9 at start, prelaunch and after, release baseline 8/8, BankOverflow no loss, StackOBot Content and the cache's Binaries equal, 110/110 bound files and the three manifests, the 1,458 existing Content files, the 18 Config files, descriptor and `.uproject`. The canary is absent (Codex 0 matches; mine 0 candidates in 359 files written since the launch second). No crash, no PIE.

## 1. Rulings
| # | Ruling / item | Evidence (mine unless stated) | Status |
|---|---|---|---|
| 1 | FA1 junction removal (done by 094-26) | `fa1-verify.json` 13:52: link absent; target a plain folder holding exactly `safe.txt` 9 B `B3CC0475…`, mtime equal to the receipt; 0 reparse points in 35 CodeReview folders (6,612 files) | OK |
| 2 | FA1 no reparse points created | None by me; the feature gate's CodeReview reparse scan is clean at start, prelaunch and after | OK |
| 3 | FA2 O1/O2 carried | O2 fired as predicted: Codex's job-entry ledger flagged the owner's `E:\codex-home` → `E:\_DELETE-ME-codex-copy-2026-10-29` rename (generic port-pattern names); recorded foreign, report-only (`foreign-root-closeout.json`); it did not stop the kit, whose own ledger started after the rename | OK · O2 fired |
| 4 | FA3/EY7 one real run, at most one session per recipe | One `.bat` start 18:30:21 → 19:00:54 (exit 2); one editor session (EA_Core 01); EA_Travel none | OK |
| 5 | FA5 window and idle pre-check | Raw `idle-precheck-01.jsonl` (209 readings): WAIT with `FA5: idle for 600 seconds` until PASS 13:00:21 UTC = 18:30:21 IST (657.6 s idle, 7.77 GiB physical, 13.17 GiB commit, GPU 1 %); inside the window (to 06:00 on 10-30); the 10:00–10:30 quiet slot was already past | OK |
| 6 | FA5 kit gate decides | `idle.jsonl` 8 polls 18:52:58 → 18:56:34; console "Waiting: mouse and keyboard idle for 300 seconds." ×3, "Waiting: three passing polls, 25 seconds apart.", "Measured gate passed: three quiet polls; the launcher is already running." | OK |
| 7 | Re-run rule | No re-run; the stop line was not the no-idle-window line, so none was allowed | OK |
| 8 | FA6 lean checks | Start/after: m51 by git, release 8 hashes, protected-state check; no EK1 scan, no P9 re-hash by me | OK |
| 9 | FA6 foreign activity never ends a job | Owner roots (C:\_DELETE-ME removed, codex-home rename, two DaVinci folders), the C: dip to 6.9 GiB (EU2 pause 14:30:43–14:52:43, 1,320 s) and low commit were recorded and waited out | OK |
| 10 | FA7 chaining | 094-27 must stay held: this job is RED | Note |
| 11 | FA8 retry as written | Fresh entry proof (Codex `09426r\P9\entry` PASS); FA1 verified not repeated; no Code dry run; prior turn's receipts preserved (`preserved\`, 17 files hash-equal; Codex's `Receipts\09426` untouched) | OK |
| 12 | EZ1 attributed roots | My evidence for every new/removed root (`root-evidence-*.json`, 0 files opened) matches Codex's foreign record | OK |
| 13 | EZ2 admission wait | C: below its start line 14:30–14:52 waited (no stop); D: dips 77 → 41 GiB recovered | OK |
| 14 | EZ3–EZ6 | Kit `4c5ad14e…`/`01b2ed6a…`, tools = `a8d3425` (diff empty) at start and after; no source, kit or arming change | OK |
| 15 | EY1 real-branch rehearsal | Held in 094-25, but the stand-in cannot exercise the editor's Python 2.7; G1 lived exactly there | **GAP (G1)** |
| 16 | EY2 launcher exemption | Launcher absent at entry (owner restart); started by the owner 18:08:07 (PID 25528 + 11 helpers, `state-1813.txt`); the gate then printed "the launcher is already running" | OK |
| 17 | EY3 blockers | UBT under the editor is outside the start-gate scope (in session); see O3 | OPEN (O3) |
| 18 | EY4 E: lines | E: 304–318 GiB throughout | OK |
| 19 | EY5 4.25 only | No 5.1 build or write; Baseline51/ReGate51 clean | OK |
| 20 | EY7 clean exit (a) | probe.jsonl 25 rows: one `world_cleanup` with `actor_snapshot="skipped"`, `skip_reason="engine_exit"`; exactly one `probe_stop`; 0 PIE rows; no PIE line in Runtime.log; exit 0, `QUIT_EDITOR` (Runtime.log:673), "Log file closed" | OK |
| 21 | EY7 errors (a) | My classification of every error-like line: OVRPlugin.dll not loaded (:574, engine/host, benign); "LoadErrors: New page" and MapCheck 0 errors (:625, :639, engine notices); the six-line traceback (:666–671) = **our authoring tooling**; 2 warnings; no ensure, fatal or plugin/probe C++ error | **GAP (G1)** |
| 22 | EY7 outputs (b) | My after-lists: Content 1,453 → 1,453, plugin Content 5, Config 18, descriptor, `.uproject` all equal; 0 new packages, no BuiltData; Saved\Config 23 → 22 (the crash-client ini); Saved adds `Autosaves\PackageRestoreData.json` (96 B); `Intermediate` gains 31 asset-registry cache files (editor, owned host) | **STOP (G3)** |
| 23 | EY7 authored content (c) | `authoring.json` never written; nothing to compare with the A5 recipe; recipe, author script and A5 manifest unchanged | N/A (G1) |
| 24 | EY7 / EU4 no uploads (d) | EU4 switch in every recorded argv; no `-unattended`; **two analytics POSTs** (editor and crash reporter, above); no crash-payload upload; no new folder under Paris `Saved\Crashes`, 4.25 `Saved\Crashes` or `%LOCALAPPDATA%\UnrealEngine\4.25\Saved\Crashes`; `%LOCALAPPDATA%\CrashReportClient\Saved\Config\WindowsNoEditor` rewrote 11 small inis (ledger port_C_exception) | **FAIL (G2)** |
| 25 | ET2 EditorRuns (e) | Only `31124` before and after; 0 B, times and empty hash unchanged | OK |
| 26 | Ledger classes (f) | Codex: charged C: 1,114,112 B (11 crash-client inis, 2 driver caches, UBT `XmlConfigCache-E+EpicGames+UE_4.25.bin`, 3 event logs), E: 0, 5.1 engine 0; limits 250 / 50 / 100 MiB | OK |
| 27 | EX3 quarantine (g) | `quarantine-receipt.json`: QUARANTINED, files [] moved [], original paths absent, launch lock unchanged, `retry_authorized=false`; slot EA_Core 01 spent, 02–03 and all Travel slots unused | OK |
| 28 | EX1, EX2, EX4–EX6 | No in-session stop was needed (exit before any limit); argv mutants N/A | OK / N/A |
| 29 | EW1 manifests | `kit-hashes-after.json`: 27/39/44 = 110/110 | OK |
| 30 | EW2–EW4 | Scope in `attempt-result.json` = EW2 (max 2 sessions, 1 per recipe, 1,800 s, no PIE/cook/commandlet/packaging/config edit) | OK |
| 31 | EV1–EV7 | EV1: one closed pause, no edit inside it (Codex's audit); EV3 D: lines; nothing else applies | OK |
| 32 | EU1 command audit | Spot check from `events.jsonl` after my offset (`audit-spot.json`): 116 commands — #1 the declared direct read-only `Get-Content` (ruling, old helpers, my launch-start and canary hash files), #2–#116 via `filtered09426r.ps1`; 0 direct write-capable. Equal to Codex's 115 + 1 | OK |
| 33 | EU2 foreign dips | C: 16.7 → 6.9 GiB 14:18–14:41 (Claude desktop VM disk, `c-writer-143254.json`), recovered 14:52; pause handled by the kit's rule | OK |
| 34 | EU3 launcher | Never started by us; the kit and Codex's pre-check waited for it | OK |
| 35 | EU5, EU6 | Not reached | N/A |
| 36 | ET1, ET3–ET7 | ET3 `ProgramData\Epic` rows foreign; ET7 owner-slot files unchanged in size and time (now readable after the restart) | OK |
| 37 | ER1–ER4 | Every refusal named its reason (the two `Stopped:` lines) | OK |
| 38 | EQ1–EQ5, EP1–EP3 | Not exercised | N/A |
| 39 | EO1–EO6 | EO5 Codex's entry/exit PASS; EO1 carried | OK |
| 40 | EN1–EN5 | No EN5 exception applied; the purge and analytics are not waived | OK |
| 41 | EM2 | `CLAUDECODE` in the launch base (positive control); absent from the editor's 92-name environment | OK |
| 42 | EL1–EL5 / EL2 canary | Launch control (`launch-receipt.txt`): a child cmd sees the name and a child python's SHA-256 of the value equals the generated `bbf67f1c…` in `canary.txt` (fresh; not 094-26's, and not my wrapper dry run's `84dbec87…`). Codex: launch-second scan PASS (203 files), final scan 334 files 0 matches, **GAP** on 3 access-denied `.evtx`. Mine (`canary-scan-code.json`): 359 files, 969 MB since the launch second over `D:\IA_Europa`, the run folder (events from the pre-launch offset), `%LOCALAPPDATA%\CrashReportClient`, the 4.25 `Intermediate`/`Saved`: **0 canary-shaped strings, 0 matches, 0 errors**. By name in the editor's environment: 0 | OK · GAP (Codex, carried) |
| 43 | EK1–EK3 | No full scan (FA6); editor environment has 0 secret-named names | OK |
| 44 | EI2 / EG1 / EG2 | No build; 4.25 install and 5.1 trees unchanged in charge | OK |
| 45 | P9 v2 / protected / m51 | m51 PASS ×3 (start, prelaunch, after); protected PASS start and after; P9 v2 file unchanged; Codex's EA8 exit PASS | OK |
| 46 | Launches | Only the kit's one editor session; its tree: UE4Editor 30564 → CrashReportClientEditor 14248 → 36112, and **UnrealBuildTool 22148** (+ conhost 9892), all gone at closeout. My polls never caught the 43 s session | OK · O3 |
| 47 | Codex's commit | `97aec94` (parent `83d614f`): CLAUDE.md + `docs/europa/09426-authoring-receipts.json` + session note; tools diff vs `a8d3425` empty; no UserID or datarouter text in it; no push | OK |

**Count (47 rows):** 37 OK (some with observations) · 6 N/A · 1 OPEN (O3) · 2 GAP (G1 in two rows; Codex's canary GAP carried) · 1 FAIL (G2) · 1 STOP (G3). P9-protected state: 0 changes.

## 2. What blocks A6 (for chat)
- **G1, the launcher script (blocking).** Any in-editor Python step generated the same way fails under 4.25's Python 2.7. Options, all needing a tooling change and a re-arm: keep the module alive (`imp.load_source`, or `exec` into a dict held by `author.py`), or put the call inside the run file itself. **A rehearsal that never runs the editor's Python cannot prove the fix**; chat may want one harmless real-editor Python smoke on owned scratch before a real slot.
- **G2, analytics (blocking).** EU4 needs a sibling: a proven opt-out that stops both the editor's AnalyticsET POST and the crash reporter's EditorSessionSummary send, proven from 4.25 source as EU4 was (candidate only: `[/Script/UnrealEd.AnalyticsPrivacySettings] bSendUsageData=False`, not verified by me).
- **G3, engine-managed crash configs (blocking for every later session).** `Saved\Config\CrashReportClient\UE4CC-*` is purged by the engine after two days, so binding those files as "existing config" stops any session two days after they were written. Chat decides whether to re-bind them as engine-managed (recorded, not protected).
- **Slots:** EA_Core 01 is spent; a retry needs chat's re-arm. **094-27 stays held.**

## 3. Observations
- **O1:** The interrupted 094-26 turn left no `exit-code.txt` and no ledger row; its receipts (`Receipts\09426`, run-folder `*09426*`) are preserved and Codex used a fresh `Receipts\09426r`.
- **O2:** FA2 O2 fired (generic names in the owner's renamed Codex home) in Codex's job ledger only; report-only per FA6.
- **O3:** The editor started UnrealBuildTool (22148) despite `-SkipCompile`; its argv was not retained. Codex's inference (stock `-Mode=QueryTargets`, `DesktopPlatformBase.cpp:835`) fits the one C: write (UBT XML config cache). Nothing compiled; bound binaries unchanged. EY7 names only SCW and crash-client children as the editor's own; chat may want UBT added, and its argv recorded.
- **O4:** Codex's `Receipts\09426r\result-evidence.json` quotes the analytics UserID from the engine log (an identifier, not a secret). It is not in the commit.
- **O5:** After the restart the commit limit fell (66 → 50–55 GiB) and the launcher did not auto-start; with the owner's DaVinci Resolve, Blender and the Claude desktop VM, the 12 GiB commit line passed only in the evening.
- **O6:** My own slips, both fixed within minutes: my first `paris-diff.json` held the commit's author line and my first `session-check.json` held a truncated analytics UserID; both were rewritten without them (0 identifier hits in my receipts afterwards).

## 4. Top 5 risks for A7 (the first 4.25 pictures in these two maps)
1. **The editor's Python path has never succeeded once on this host.** A7's capture and measurement steps are in-editor Python too; any `runpy`-style or Python-3-only construct fails only inside a real session, after a slot is spent.
2. **Analytics uploads from every editor session** until an opt-out is proven; each A7 session would otherwise be an EY7 RED.
3. **Engine-managed state in bound sets:** the two-day crash-config purge today; DDC, asset-registry caches and `Saved\Autosaves` are the same class and can trip an exact-equality binding.
4. **Rare admission on a busy PC:** after reboots the launcher may be absent and the commit limit lower; real sessions effectively happen only when the owner is away, and a 43 s session can fall entirely between 10-minute polls (rely on the kit's own receipts, as here).
5. **EA_Core still has never been authored:** A7 depends on both maps and the two-LOD mesh; every A5 recipe item (lights, camera, post-process, tags, LODs) is still unproved on 4.25.
