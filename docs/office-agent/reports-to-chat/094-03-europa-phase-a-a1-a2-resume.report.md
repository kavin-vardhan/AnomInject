# REPORT for 094-03-europa-phase-a-a1-a2-resume.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 094-03-europa-phase-a-a1-a2-resume.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-10-16 15:26:27 | finished: 2026-10-16 18:18:42 | duration: 172 min | exit code: 0
context at last turn: 636318 tokens (64% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\094-03-europa-phase-a-a1-a2-resume.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN — A1 and A2 are complete, no GAP blocks them, and P9 is unchanged (Codex's exit proof and my own re-hash agree). There are three non-blocking GAPs; one of them needs chat's decision before the first 5.1 launch (A5/A6).

**For the owner, in plain words**
- The port setup and its build-rule checks are finished and passed, and nothing protected was touched.
- The port is not what fills C:. Three things did:
  - Visual Studio Installer is downloading an update in the background into the Windows temp folder: 3.7 GB since 15:35, and still going.
  - Windows and driver updates (CCleaner installed a graphics driver at 15:48) added about 2.6 GB.
  - Windows restore-point storage on C: is full and keeps cycling (a new restore point was taken at 15:59). Its space is invisible to normal folder sizes.
- Nothing is required from you. The read card can still be done whenever you like.

**Headroom**
- The job could start at the first poll: C: 26.71 GiB, E: 54.91 GiB at 15:28, and C: 26.11 / E: 54.85 just before launch at 15:37.
- During the job E: fell to 50.57 GiB, because the owner's own UE 5.7 project on E: was writing; it never breached the 50 GiB floor.
- You then freed space. At the end: C: 38.38, D: 144.05, E: 101.08 GiB.

**The C: audit (sizes only, nothing deleted)**
- **Biggest users at the start** (26.99 GiB free):
  - `<USERPROFILE>` 130.3 GiB, mostly AppData\Local 71.8 GiB. That includes the Claude desktop app's virtual-machine disk (8.76 GiB), uv 10.9 and Google 9.5.
  - `C:\Windows` 47.9 GiB, ProgramData 33.8 GiB (the Epic launcher's cache alone is 8.2), Program Files 31.5, Program Files (x86) 20.3, `C:\Dev` 10.1.
- **What you removed during the job:** about 23.5 GiB (`C:\Dev`, the Unity folders, GitHub Desktop, PIX).
- **What grew during the job:** the temp folder +3.76 GiB (the Visual Studio download), `C:\Windows` +1.85, CCleaner +0.76. The port's only C: write was rewriting one 190-byte UE 4.25 settings cache file at the same size, which the rulings allow.
- **094-02's unexplained 2.6 GiB drop:** partly explained but not established. The likeliest causes are the same three: the Visual Studio download, restore-point storage growing, and updates.
- **Correction to 094-02's numbers:** `<USERPROFILE>\.codex` has been a junction to `E:\CodexData\home` since 2026-09-18. Its 24.7 GB was never on C:.

**The Codex job**
- Same run folder and thread `01a14334`, `gpt-6-astra`, effort `max`, full access (confirmed from its session log).
- Ran 15:38:07–17:54:06 (8,159 s), exit 0, one try.
- Its verdict: `GREEN | A1/A2 complete; protected state unchanged; both UBT matrices and blacklist can-fails pass; A3 not started; no push`.

**A1**
- The entry check passed: 707,775 protected-file inventories, 25,866 binary/config hashes and git state all matched.
- Three hosts were created on D:: the 4.25 Paris copy and the Baseline51 / ReGate51 StackOBot copies. Each copy was compared against its source by hash, and both sources stayed unchanged.
- Worktrees were added from the shared repository as ruled: Paris on `feat/europa-port`, Baseline51 detached at `d7eee97`, ReGate51 detached at the tested `2ee5cac`.
- CaptureBench in both 5.1 hosts comes from `git archive e64d6f9`.
- Routing is in place: D:-only DDC graphs and redirected temp/log folders, recorded in every child process's receipt.
- The write ledger ran 18 operations, all passing. It charged 19,944 B on C: and 0 on E:. The charged files are crash-reporter logs of your UE 5.7 editors, so they are not really the port's, but Codex kept the charge anyway.
- The idle gate now lives in port tooling and passes its 45 synthetic tests.
- The predictions were committed as `ac81ecb` at 16:32:37, before the first UBT run at 16:47:19.

**A2**
- The 22-file change is applied in `fe74543`, plus a fix in `2ee5cac` and a results commit `4c6c1d2`. There are four local commits and no push.
- What changed:
  - **Build rules:** exact 4.25/5.1 rules, C++17 only on 4.25, legacy descriptor keys.
  - **Headers and stubs:** UHT-safe header includes, an opaque control-transport state, and do-nothing Bench/Shaders stubs on 4.25.
- The header check passes in the real worktree, and its deliberately broken test header fails as it should.
- Both UBTs compiled the shared rules. 5.1 uses MSVC 14.38.33130 (compiler 14.38.33144); 4.25 uses 14.16.27051.
- The module matrices:
  - 5.1 port equals release on 8 target/configuration pairs.
  - 4.25 passes its 5 expected pairs.
  - In both engines, removing the Shipping exclusion adds exactly `AnomalyShaders` to the Shipping build; restored, it goes back to none.

**My review** (`_reviews\094-03-code-review.md`): 16 OK · 3 GAP · 1 OPEN · 0 CONFLICT.
- **GAP EA3:**
  - The two 5.1 copies disable the EnhancedInput, ControlRig and Metasound engine plugins (also FullBodyIK and GameplayInsights).
  - StackOBot's content uses the first three. The default game mode `GM_InGame` loads `PC_InGame` and `BP_Bot`, the gate level has no override, and `DefaultInput.ini` names EnhancedInput classes.
  - So every map in those copies would load assets of disabled plugins. They are not the qualified configuration.
- **GAP EA12:** the doc lines got the new key names, but the explanation line was inserted mid-sentence in `docs/PRE-DELIVERY-CHECKLIST.md:291–292` and `docs/architecture.md:26–27`, garbling both sentences.
- **GAP EB1:** the ledger's C:/E: drive-root check records top-level files only, not folders, so a new or deleted top-level folder is invisible to it. My own listings show the port created none (the `C:\Dev` deletion was yours).
- **OPEN EA2:** the dashboard-transport seam is real. The capture and texture seams are only named so far; A3 must show them.
- **My independent checks:**
  - **P9:** re-hash 19,290/19,290 OK.
  - **Engine trees and Paris:** 0 writes during the job window in either engine tree or the Paris original, 0 build actions in all 7 UBT logs, and the full 4.25 install listing is unchanged.
  - **Protected git state:** all protected worktrees and refs unchanged; `m51` clean.
  - **JsonExport re-run** through the approved tooling (my receipts kept separate under `D:\IA_Europa\Receipts\CodeReview09403`): all 23 exports, both mutants included, are identical to Codex's module by module.
  - **Finer 5.1 check:** the only per-module differences from release are the new defines and `AnomalyInjector` becoming a public dependency of Capture and ControlServer. No game, CaptureBench or engine module differs.
- **Top risks for A3:**
  - the plugin set above;
  - the first real 4.25 UHT/C++ compile of the reworked headers;
  - include reach in 5.1 from the new public dependency;
  - the Visual Studio update may add MSVC 14.44, so 5.1's 14.38 selection must be re-checked at each A4b build;
  - the idle gate has never run for real and only exempts PIDs it is given.

**Anything the owner must do:** nothing for the port. If you want C: to stop shrinking, these are your settings to change, and I changed none: Visual Studio Installer's automatic update downloads, and the size limit for restore points on C:. Several connectors (GitHub, Slack, Google Calendar and others) are unauthorised in this headless session, and the PDF viewer failed to connect. None of them was needed here.

**NEEDS-DECISION items** (for chat; none blocks A3):
1. **EA3:** re-enable EnhancedInput, ControlRig and Metasound in both 5.1 copies (and FullBodyIK and GameplayInsights, which the original enables), keeping only the project-local RoomGenerator and UnrealMCPython disabled. The alternative is to prove the gate levels never load those assets. This must be settled before A5/A6.
2. **EA12:** move the explanation line out of the middle of the two sentences, as a small docs fix in A3.
3. **EB1:** make the drive-root check record folders before any job that launches the editor or game.
api-equivalent cost (not billed on Max): $18.00 | turns: 120 | running total across all briefs: $4,654.12
