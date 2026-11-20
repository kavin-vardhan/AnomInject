# 094-11 — Chat ruling: A5 is two-thirds done; child environments switch from an allowlist to a denylist (a new design, fresh D2), a canary proves the filter, then the rebuilds; the A6 owner card becomes a double-click script plus a plain card

**Date:** 2026-10-25, about 16:55 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Codex's report:** `_reviews\094-11-codex-report.md` (thread `01a14334`, 13:07–14:38 IST, 5,482 s, exit 0). Its verdict: `NEEDS-DECISION | A5 incomplete: D2 SDK environment propagation stop; P9 v2 and all 110 prior artifacts preserved; rebuilds and post-rebuild environment proof pending`.
- **Claude Code's review:** `_reviews\094-11-code-review.md`. 27 OK · 5 GAP (all from the stop) · 5 OPEN (not exercised) · 0 stop violations.
- **Mailbox report 094-11:** NEEDS-DECISION, 147 min.

## Verdict
**Done and verified:**
- **EK2's clean-up, steps 1–2:** 3 receipts redacted and 8 makefiles deleted. Code's post-scan of 34,702 files found no secret value left.
- **The corrected makefile reader** passed on its first attempt (17/17 synthetic cases, all 8 real makefiles), and agrees with Code's own reader.
- **The probe and screenshot-driver sources** are written and identical in all three hosts. They are **not yet compiled.**
- **69 reader can-fails** pass.
- **Fixture recipes:** recipes only, and they refuse to overwrite.
- **Runtime readiness,** including a real 15-minute observe-only idle collection.
- **The content-admission analysis:** stock 4.25 rejects the five 5.1 packages at the header (legacy version `-8`) and its registry skips them. Code's own header scan and the cited source agree. **No isolation design is needed.** The owner card still warns him away from those assets.

**Protected state:** P9 v2 18,264 of 18,264; the 110 qualified binaries equal; 0 engine writes; 0 launches.

**The stop was correct.** The allowlisted child environments lacked `VULKAN_SDK`, `VK_SDK_PATH` and `LINUX_MULTIARCH_ROOT`. UBT's platform and SDK detection reads them, so the module comparison changed (Linux target-platform and Vulkan objects), and the can-fail did its job. The committed fix (`89b81a4`) never reached the retry, because a long-running outer process kept the old filter.

## Rulings

### EL1. Child environment: a denylist, not an allowlist (replaces EJ4's allowlist)
**Why:** UBT and MSVC read many environment variables, including platform SDK roots, VS/MSBuild variables and Windows Kits paths. An allowlist will keep missing one, and every miss quietly changes the build.

**The new design:** children inherit the full user environment **minus** two classes:
1. every `CLAUDE_*`, `CODEX_*`, `ANTHROPIC_*` and `OPENAI_*` name;
2. every EK1 secret-named variable (`TOKEN`, `SECRET`, `PASSWORD`, `PASSWD`, `KEY`, `AUTH`, `COOKIE`, `CREDENTIAL`).

The job's own routing variables are added on top.

**Assert it at every process boundary** (wrapper, UBT, compile children, via the makefile reader):
- (a) no blocked name is present;
- (b) every non-blocked variable from the base environment is present, unchanged.

**Each attempt starts from a fresh process.** No long-running outer process may carry an old filter across attempts.

**This is a new, chat-specified design. It gets its own two D2 attempts.**

### EL2. A canary that can really fail
- The next job's launch adds one harmless, randomly generated, secret-named variable: `IA_CANARY_TOKEN`, with a value that is not a real secret.
- The post-rebuild makefile proof and the receipt scan must find it **absent** from every child and every new file. Its presence is a STOP.
- Code generates it and records its name. Its value may appear only in Code's own launch receipt.

### EL3. Finish A5
1. EL1 and EL2, with self-tests.
2. **The 6 rebuilds:** Editor and Game in each of the three hosts, under EI2, with EG2 receipts. These compile the probe for the first time.
3. **EK2 step 3:** the post-rebuild makefile proof (no blocked name, canary absent).
4. **The final module comparison:** the JsonExport set equals the accepted exports, except for the intended probe changes.
5. **The EI4 clean-engine query.**
6. **Re-bind the manifests.** ReGate51 moved from `1c4cce7` to `89b81a4` with no production change; re-label the source and re-hash the binaries.
7. **Remove the docstrings** from the six new tools, per the repo's no-comments convention.

### EL4. The A6 owner kit (Codex builds it; chat polishes the card)
**The script:** `D:\IA_Europa\Launch\Europa425-Editor-OwnerLeg.bat`. It starts the 4.25 editor on the Paris host **through the approved wrapper**:
- with `-UserDir`, `-DDC`, `-ShaderWorkingDir`, `-abslog` and the TEMP routing;
- refusing to start unless the run's routing and P9 checks pass;
- logging everything to D:.

**The card,** in plain language, to `_reviews\094-12-owner-card-draft.md`:
- when to run it: only when chat says;
- what to expect: a first-run shader compile of tens of minutes;
- **never accept any convert, update or resave prompt;**
- **never open anything inside the plugin's own Content folder;**
- the exact Paris static mesh to open, by its Content Browser path;
- how long to leave the preview open;
- how to close the editor (File > Exit);
- what to tell chat afterwards;
- what to do if a dialog or error appears: screenshot it, choose No or Cancel, then close.

**Nothing is launched in A5.** The script is built and self-tested in dry-run mode only.

### EL5. Code's deviation is accepted
Code removing its own secret-named variable from Codex's launch environment was right. EL2's canary now gives the proof a real failure path.

## Next
- **094-12 is queued:** A5, finishing the work under EL1–EL5. Code reviews after Codex exits.
- **Then A6:** the first 4.25 editor launch. Chat sends the owner a short, plain card and the one double-click.
