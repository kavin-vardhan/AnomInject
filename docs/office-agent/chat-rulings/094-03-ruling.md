# 094-03 — Chat ruling: A1 and A2 are complete and accepted; three small fixes and one open seam carry into A3; development is now PAUSED by the owner

**Date:** 2026-10-16, about 19:10 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Codex's report:** `_reviews\094-03-codex-report.md` (thread `01a14334`, Astra max, full access, 15:38–17:54 IST, 8,159 s, exit 0). Its verdict: `GREEN | A1/A2 complete; protected state unchanged; both UBT matrices and blacklist can-fails pass; A3 not started; no push`.
- **Claude Code's review:** `_reviews\094-03-code-review.md`. 16 OK · 3 GAP (EB1, EA3, EA12) · 1 OPEN (EA2) · 0 CONFLICT.
- **Code's independent checks:**
  - P9 re-hash 19,290/19,290;
  - 0 engine-tree or Paris writes;
  - all 23 JsonExports reproduced exactly, including both mutants.
- **Mailbox report 094-03:** GREEN, 172 min, $18.00 API-equivalent.
- **The owner (about 16:25 IST):** "After this Codex/Code round let's pause development until I ask you to resume."

## Verdict
**A1 and A2 are ACCEPTED as complete.**

**The hosts and branch:**
- Three hosts on physical D:, each hash-verified against an unchanged source:
  - `D:\IA_Europa\StylizedParisStreet` (4.25);
  - `Baseline51\StackOBot` (`d7eee97`);
  - `ReGate51\StackOBot` (`2ee5cac`).
- Branch `feat/europa-port` has 4 local commits, no push:
  - `ac81ecb` predictions, 16:32:37;
  - `fe74543` A2 source;
  - `2ee5cac` reader fix and toolchain log;
  - `4c6c1d2` results.
- **The predictions came before the first UBT run** (16:47:19).

**Protection and writes:**
- **P9 unchanged:** entry and exit proofs, plus Code's independent re-hash.
- **EB1 ledger:** 18 operations. Charged 19,944 B on C: (crash-reporter logs from the owner's UE 5.7 editors, conservatively charged) and 0 on E:. No STOP.

**The A2 proofs:**
- Both UBTs compiled the shared C# rules. The toolsets were 14.38.33130 for 5.1 and 14.16.27051 for 4.25.
- **5.1 port = release** on 8 target/configuration pairs. The only module-level differences are the new defines, and AnomalyInjector becoming a public dependency of Capture and ControlServer.
- **4.25** matches its 5 expected pairs.
- **Both Shipping-blacklist mutants fail exactly as predicted:** AnomalyShaders appears in Shipping. Both restore to none.
- **EA11:** the header check passes in the real worktree, and its mutant fails.
- **The idle gate** is now in port tooling, with 45 synthetic tests passing.

## Rulings (all carried into A3, applied when the owner resumes)

### EC1. EA3 gap: the 5.1 copies must keep the original's engine plugins
- **The problem:** the copies disabled EnhancedInput, ControlRig, Metasound, FullBodyIK and GameplayInsights. StackOBot content uses them:
  - `BP_Bot`, `PC_InGame`, `ABP_Bot`, `CR_MovingPlatform` and 6 SFX;
  - `GM_InGame` is the global default game mode;
  - `DefaultInput.ini` names EnhancedInput classes.
  As copied, they are not the qualified configuration.
- **The fix:** each copy's effective plugin set equals the original StackOBot's.
  - **The only exceptions** are the project-local `RoomGenerator` and `UnrealMCPython`. Their folders are not copied, and they stay disabled unless a gate level is shown to need them.
  - All of these engine plugins' binaries already exist in the 5.1 engine tree for StackOBot, so EA8's rule (zero engine-tree builds) still holds. Prove it from the UBT logs.
- **Proof:** re-run the 5.1 JsonExport matrix (port against release, plus the mutant) after the change, before any 5.1 build in A4b.

### EC2. EA12 gap: the doc lines
The explanation line was spliced mid-sentence in `docs/PRE-DELIVERY-CHECKLIST.md:291–292` and `docs/architecture.md:26–27`. Move it after each sentence, as a docs fix in A3.

### EC3. EB1 gap: drive-root folders
The C: and E: drive-root check must record top-level **folders** as well as files. This is required before any job that launches the editor or game (A5/A6).
- Code's own listings show the port created none this round.
- The `C:\Dev` removal was the owner's.

### EC4. EA2 open: the capture and texture seams
- **Already in place:** the F seam (`FAnomalyControlTransportState`, opaque).
- **A3 must add:**
  - the C (capture backend) interface or factory, behind the existing opaque members;
  - the E (texture adapters) interfaces behind the existing facades.
  Both as non-reflected declarations, with a one-line note on where each 4.25 backend will plug in.

### EC5. A3 risks to watch (Code's top five, adopted)
1. The plugin set (EC1).
2. The first real 4.25 UHT and C++ compile of the reworked headers.
3. 5.1 include reach from the new public dependency.
4. **The 5.1 toolset.** Visual Studio Installer is downloading an update that includes MSVC 14.44. Every A4b build must re-verify the 14.38.33130 selection, and a missing 14.38 is a STOP for chat, not a silent switch.
5. **The idle gate** has never run for real, and it exempts only the PIDs it is given.

### EC6. The C: question (for the record)
- **The port is not what fills C:.** The port's total C: charge across two jobs is 19,944 B.
- **Today's real C: consumers:**
  - Visual Studio Installer's background update download: 3.68 GiB in `%LOCALAPPDATA%\Temp`;
  - Windows and driver updates: about 2.6 GiB;
  - restore-point (shadow copy) storage on C:, which is full and cycling.
- **Also on C::** the Claude desktop app's VM disk (`rootfs.vhdx`, 8.76 GiB) and the Epic launcher's cache in ProgramData (8.2 GiB).
- **Correction:** `<USERPROFILE>\.codex` is a junction to `E:\CodexData\home` (since 2026-09-18). Its 24.7 GB was never on C:.
- **Owner-only settings, unchanged by us:** Visual Studio's automatic update downloads, and the restore-point size limit on C:.

## Next (when the owner says "resume"; nothing is queued now)
1. **094-04: A3, the shared C++ closure.** Codex resumes thread `01a14334`; Code reviews after.
   - The shim and the five re-parented subsystems;
   - `ShouldCreateSubsystem` and the ancestry audit;
   - raw `UPROPERTY` pointers;
   - Compat and includes;
   - the inert-closure and capability admission;
   - the host-probe sources;
   - plus EC1–EC4.
2. **Then:** A4a/A4b builds and strict passes, A5–A12, Phase B, D-lite (the first Europa office smoke), then C → D → E → F, per 094-01 EA2/EA10.
3. **Owner errands, any time:** the Europa read card (`claude/094-01-europa-read-card.md`). Later: the first 4.25 editor launch (A6) and the eye check (A7).
