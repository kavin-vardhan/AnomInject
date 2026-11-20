# CHAT HANDOFF — Europa (UE 4.25) port: Phase A jobs A1+A2 done · development PAUSED by the owner

**Date:** 2026-10-16, about 19:15 IST (chat session 093/094) · **Supersedes:** `CHAT-HANDOFF-093-J2-paused-europa-port-opened.md` on Europa matters. That doc stays the source for J2 (its §1–§3).

**Confidentiality:**
- **Names:** "Europa" is the 4.25 office title, and "Europa's fork" is its engine. Other office hosts appear in files as "first/second office host". StylizedParisStreet and StackOBot are the owner's own bench projects.
- **Office data never comes home.** Only numbers the owner types himself do.

---

## 0. READ THIS FIRST
1. ⏸️ **Development is PAUSED by the owner** (10-16, about 16:25 IST): *"After this Codex/Code round let's pause development until I ask you to resume."*
   - The round (094-03) is finished and ruled.
   - **Nothing is queued. The watcher is idle. No scheduled checks are pending.**
2. ▶️ **To resume, the owner says "resume".** Chat then writes **094-04: Europa Phase A job A3**, the shared C++ closure, per `094-03-ruling.md` "Next" and EC1–EC4.
   - Codex resumes thread `01a14334` through `relay_resume_codex_impl_v1.ps1 -OwnerApprovedFullAccess`, with run folder `_relay\runs\2026-10-16-094-01-europa-phase-a-plan`.
   - Code reviews after Codex exits (W3).
   - Effort xhigh.
3. **J2 is still paused too** (since 09:55). Its resume recipe is in the 093 handoff §3. It resumes only on the owner's word.
4. 🎯 **The Europa target is full parity** (the owner's option C): every delivered anomaly, capture/labels/masks and the dashboard. Rough size 6–10+ engineer-weeks, plus office latency.

## 1. Current state (verified at about 18:58 IST)

**Europa port, Phase A (12+ jobs; see EA10):**
- **Branch `feat/europa-port`** = `4c6c1d2`: local, **not pushed**, cut from `d7eee97` (release/2026-10-05).
  - Commits: `ac81ecb` predictions (16:32:37, before the first UBT run at 16:47:19) · `fe74543` A2 source · `2ee5cac` reader fix and toolchain log (the tested source) · `4c6c1d2` results.
- **Hosts on physical D:** (`D:\IA_Europa\`):

| Host | Plugin folder (a git worktree) |
|---|---|
| `StylizedParisStreet` (UE 4.25 copy; original at `E:\Unreal Projects\StylizedParisStreet`, untouched) | `feat/europa-port` |
| `Baseline51\StackOBot` (5.1 control) | detached `d7eee97` |
| `ReGate51\StackOBot` (5.1 candidate) | detached `2ee5cac` |

- **CaptureBench** is in both 5.1 hosts, from `git archive e64d6f9`.
  - ⚠ Both 5.1 copies currently disable 5 engine plugins StackOBot needs. A3 must fix this first (EC1).
- **Done and proven:**
  - **A1:** P9 baseline (`D:\IA_Europa\Receipts\P9\baseline\full-sha256.jsonl` `db884f1e…`, 707,775 files); hosts; worktrees; routing (D:-only DDC/temp/logs); the EB1 write ledger; the idle gate (45 tests); predictions.
  - **A2:**
    - exact-version build rules for 4.25 and 5.1, C++17 on 4.25 only;
    - legacy descriptor keys;
    - UHT-safe headers;
    - opaque control-transport state;
    - inert Bench/Shaders stubs on 4.25.
    - **The module matrices:** 5.1 port = release on 8 pairs; 4.25 = 5 expected pairs; both Shipping-blacklist mutants fail as designed.
- **Not started:**
  - A3 (shim + five parents, pointers, Compat, closure, probe);
  - every build of plugin code (A4a/A4b);
  - every editor or game launch;
  - Phases B–F.
- **No C++ has been compiled yet.** Only the C# build rules have been compiled, by both UBTs.

**J2 (`fix/followup-capture`), unchanged since the pause:**
- local `7754fd2`, remote `1bf7054`; candidate `9781031`;
- J2's two hosts still carry J2's installed candidate (not restored).
- Full detail is in the 093 handoff.

**Disk (end of 094-03), GiB free:**
- C: 38.38 (floor 10)
- D: 144.05 (floor 50)
- E: 101.08 (floor 50)

E: swings with the owner's own UE 5.7 project in `E:\<personal-folder>`.

## 2. Decisions this session, with the why (rulings in `_reviews\` and the project)
- **094-00 (P1–P14), the port charter:**
  - **Base `d7eee97`:** the client's qualified M3, and what 092-02 audited.
  - **One source tree:** 5.1 neutrality is proven by gates, never by `#if` arguments.
  - **The shim** follows 092-02's design, because stock 4.25 never calls `IsAllowedToTick`.
  - **Legacy descriptor keys:** 4.25 ignores the modern keys, so the Shipping exclusion would silently fail.
  - **Hosts on D:**, because E: and C: are tight.
  - **J2 frozen (P9).**
  - **The Target.cs compiler route.**
  - **Codex implements, Code reviews.**
  - **Codenames.**
  - **An early Europa read card.**
- **The owner's target, option C (full parity).**
  - The Compat seams must anticipate C, E and F, so later backends never change reflected declarations again (EA2).
- **094-01 (EA1–EA15), the plan accepted with amendments:**
  - **EA3:** the **5.1 re-gate runs on StackOBot copies with CaptureBench**, not on new Paris fixtures. The qualified instruments live there, and new readers risk false greens.
  - **EA4:** bounded C:/E: exceptions. A stock tool's tiny KnownFolder writes can't be redirected without global changes.
  - **EA5:** 0 frames off is required at the AA-off arbiter, with paired TAA/TSR characterization.
  - **EA6:** the port's own idle gate.
  - **EA8:** P9 extended to the 5.1 engine tree, with 0 engine-module actions required.
  - **EA10:** a job split sized for idle windows.
  - **EA11:** UHT-safe headers are mandatory.
  - **D-lite** (a first Europa office compile and injection smoke) moves **before** Phase C, so fork surprises surface early.
- **094-02 (EB1–EB5), the write proof:**
  - It is now a **targeted, declared check that needs no admin.** Windows denies ETW and USN without elevation, and we never escalate.
  - A C: headroom gate (13 GiB) before a job starts.
  - A resume runs only the per-job entry proof; the full hash runs only at the final job.
  - A C: size audit.
- **094-03 (EC1–EC6):** A1/A2 accepted. Gaps EC1 (plugin set), EC2 (doc splice) and EC3 (drive-root folders), plus the open EC4 (C/E seams), all carry into A3.

## 3. Forward plan on resume, in order
1. **094-04, A3:** shared C++ closure, plus EC1–EC4. EC1 includes re-running the 5.1 JsonExport matrix before any 5.1 build.
2. **A4a:** 4.25 builds + strict. **A4b:** 5.1 builds + strict.
   - Re-verify **MSVC 14.38.33130** at every 5.1 build. A Visual Studio update with 14.44 is downloading on the PC (EC5).
3. **A5:** probe and fixtures.
4. **A6:** the first 4.25 editor launch. **Owner leg:** open the editor once and open one mesh preview.
5. **A7:** 4.25 pictures. **Owner eye check.**
6. **A8:** 4.25 package.
7. **A9–A11:** the 5.1 re-gate.
8. **A12:** the final consistency check.
9. **Phase B:** the six basic anomalies, with 4.25-authored materials.
10. **D-lite:** the owner builds and smoke-tests at the office. That needs a push of `feat/europa-port`, which the owner does when chat asks.
11. **Then C → D → E → F.**

The Europa read card (`claude/094-01-europa-read-card.md`, numbers only) can be done any time.

## 4. Open vs locked
**🔴 Open, for the owner:**
- the Europa read card;
- when to resume Europa;
- whether and when J2 resumes;
- **the Visual Studio update.** Our 5.1 builds are pinned to MSVC 14.38. If the update is installed, keep the 14.38 build tools ticked in Visual Studio Installer, or hold the update;
- the Event Viewer restart check (from 10-15);
- K8.

**🔴 Open, technical:**
- EC1–EC5;
- from J2: AC3-C (the TSR 50 % mask box offset of (−1,−1) output px), AB3-D, AA7 (mailbox NAME hardening), and the `owner_render_missing` counter.

**🔒 Locked:**
- 094-00 P1–P14;
- 094-01 EA1–EA15;
- 094-02 EB1–EB5;
- 094-03 EC1–EC6;
- the full-parity target;
- J2's 093-xx rulings and P9 freeze.

## 5. Corrections from this session
- ~~"The port writes to C:"~~ → It doesn't: 19,944 B in total across two jobs, all crash-reporter logs of the owner's UE 5.7 editors. What fills C: is:
  - the Visual Studio Installer background download (3.7 GB of temp);
  - Windows and driver updates;
  - restore-point storage;
  - the Claude desktop VM disk (`rootfs.vhdx`, 8.76 GiB);
  - the Epic launcher cache (8.2 GiB).
- ~~"`.codex` (24.7 GB) is on C:"~~ → `<USERPROFILE>\.codex` is a **junction to `E:\CodexData\home`** (since 09-18).
- ~~"Hugging Face cache = 23.5 GB"~~ → about **7.9 GB** real. TreeSize counts linked files three times. It holds the owner's **<other-home-project>** model (keep it) plus Whisper models (deletable).
- ~~"J2 stopped with nothing half-done"~~ → J2's install step had run at 09:30, so both J2 hosts carry its candidate, unrestored. That is J2's paused state; the P9 baseline includes it.
- ~~"Paris 5.1 copies for the re-gate"~~ (Codex's plan) → StackOBot copies (EA3).

## 6. Pointers
- **Europa:**
  - `_reviews\094-00-europa-port-ruling.md`, `094-01-ruling.md`, `094-02-ruling.md`, `094-03-ruling.md`
  - `094-01-codex-europa-phase-a-plan.md` (the plan)
  - `094-01-europa-read-card.md`
  - `094-02/03-codex-report.md` and `094-0x-code-*review.md`
  - the J2 pause receipts in `094-01-j2-pause-receipts\`
  - receipts under `D:\IA_Europa\Receipts\`
  - predictions `docs/predictions/094-europa-phase-a.md` on `feat/europa-port`
  - Code's mid-job notes in `_relay\runs\2026-10-16-094-01-europa-phase-a-plan\code-receipts-094-0x\`
- **Background:** `_reviews\092-02-codex-europa-port-study.md` and its ruling; `_reviews\091-00-code-desktop-orchestrator-STATE.md` (lines 76–91); the 070 handoff.
- **J2:** `CHAT-HANDOFF-093-J2-paused-europa-port-opened.md`, rulings 093-10 … 093-14, `D:\IA_BankOverflow\_r9315`.
- **Process:** `_relay\README.md` (relay scripts, guards), `_mailbox\README.md`, `claude/mailbox-protocol.md`. Brief NAME headers must avoid `% ^ & | < > "`. Stage briefs as `.part`, then rename, and confirm INIT within about 2 minutes.
