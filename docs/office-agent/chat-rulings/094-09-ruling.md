# 094-09 — Chat ruling: A4b is COMPLETE; the build cache is cleared; A5 (probe, readers and fixtures, no launches) is next

**Date:** 2026-10-25, about 08:15 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Codex's report:** `_reviews\094-09-codex-report.md` (thread `01a14334`, 10-24 22:24 to 10-25 04:16, 21,123 s, exit 0). Its verdict: `GREEN | A4b baseline/candidate builds, R1 and 5.1 strict complete on 1c4cce7; P9 v2 and ledgers pass; A4a artifacts unchanged; no push`.
- **Claude Code's review:** `_reviews\094-09-code-review.md`. 36 OK · 0 GAP · 7 OPEN · 0 STOP.
- **Mailbox report 094-09:** GREEN, 396 min.

## Verdict
**EI7, the cleanup, is DONE:**
- 432,433 files deleted with 0 errors, after the 3 junctions were removed as links with their targets intact;
- E: free went from 48.0 to 271.9 GiB;
- the protected state passed at all three checks.

**A4b is ACCEPTED as complete, at `1c4cce7`** (source equal to `489c828`).

| Build | Actions | Time |
|---|---|---|
| Baseline51 Editor | 29 | 3.4 min |
| Baseline51 Game | 765 | 77 min |
| ReGate51 Editor (clean) | 29 | 3.5 min |
| ReGate51 Game | 765 | 78 min |
| ReGate51 Shipping | 706 | 60 min |

**What the builds showed:**
- MSVC 14.38.33130 and SDK 22621 in all 2,913 responses;
- 0 actions writing under either engine tree;
- EI2: 25 admissions and 0 stops, with free commit never below 14.9 GiB.

**R1 holds:**
- the only warnings are the same 2 × C4996 in host code, on both sides;
- the generated defines are 1/1/1, and the tick pin is 0 on both sides;
- Shipping has no IA or probe module.

**5.1 strict is complete:** 64 `.cpp`, 77 headers and 11 generated files, with PCH and unity off. The C2027 mutant fails as predicted, and the rules were restored exactly.

**The clean-engine query is 0.** The A4a 27-file manifest is unchanged.

**Final manifests:** 39 files (Baseline51) and 44 (ReGate51), re-hashed by Code. **A9–A11 must bind to these, and A6–A8 to A4a's 27.**

## Rulings on the open items

### EJ1. ISPC actions (Code's OPEN 2)
- **Report** engine ISPC header-generation actions as "engine-source in host" (45 Development, 31 Shipping).
- **The binding rule is unchanged:** 0 actions may write under an engine tree.

### EJ2. The five stale worktree registrations (OPEN 4)
- **Don't prune now.** They are harmless, and the registrations keep every commit reachable.
- **Revisit** when a branch they hold, such as `release/2026-10-05`, has to be checked out elsewhere.
- **If pruning is ever done,** P9 v3 removes exactly those registrations, by receipt.

### EJ3. CaptureBench in the 5.1 Shipping exe (OPEN 1)
- CaptureBench is a **host-only qualification instrument**, as it was in the release's own qualification.
- It stays in both 5.1 hosts, identical, and is **never part of any delivery**.
- A11a's package checker classifies it as host-only, and requires it to be identical between baseline and candidate.

### EJ4. Environment hygiene (OPEN 6)
**Why:**
- Cached UBT makefiles re-export the environment they were created under, and that put a local Claude Code messaging token into port receipts (`actions-1.json`).
- Receipts that describe the wrapper's environment are not proof of the compiler's environment.

**The rulings:**
- **Allowlist the environment.** Build and launch children receive only the variables they need: system, PATH, the D: TEMP/TMP, VS/MSVC, UE and the job's own. Never any credential-like variable (`*TOKEN*`, `*KEY*`, `*SECRET*`, `*PASSWORD*`, `*AUTH*`, `CLAUDE_*`, `ANTHROPIC_*`, `OPENAI_*`, `CODEX_*`).
- **Record the effective environment** from UBT's own exported actions or makefile, by variable name and with values masked, not just the wrapper's.
- **Clean up what exists:**
  - Codex finds every port-owned file (under `D:\IA_Europa` and the run folder) that holds a credential-like value;
  - it redacts the value in receipts (to `<redacted>`), with a redaction log of paths and before/after hashes;
  - it deletes regenerable makefiles holding one, by receipt, so the next build regenerates them clean;
  - it never prints a value, not in reports, notes or chat.
- **Code verifies by variable name and file count only.**

### EJ5. Minor items
- The two empty `D:\IA_Europa\Temp\CodeReview09409` folders may stay. They are Code's own scratch.
- **Physical memory reached 0.11 GiB during the compiles:** paging, which is allowed. A6's launches start from that margin, so the runtime idle gate (EA6) keeps its 6 GiB physical rule for editor and game legs.

## Next: job A5 (094-10), with no editor or game launch
Following plan §H's A5 row and EA10:
1. **The probe's real behaviour** (plan C4):
   - lifecycle observer;
   - independent per-world update denominator;
   - positive editor-preview sentinel;
   - the controlled screenshot, apply, revert, pause, travel and GC driver;
   - the screenshot driver per E3 (`FScreenshotRequest`, absolute D: paths, one outstanding, completion acknowledged).
2. **All the reader can-fails** for gates A3–A8 and R3–R9, on synthetic inputs.
3. **Fixture recipes and manifests:**
   - the 4.25 Paris `EA_Core` and `EA_Travel` maps, built by script;
   - the 5.1 CaptureBench fixtures, built with CaptureBench's own tools.
   - Recipes only. Authoring needs the editor and happens in A6/A9.
4. **Runtime readiness of the tooling:**
   - the ledger's runtime attribution classes (editor, ShaderCompileWorker, CrashReportClient, DDC, shader working dir);
   - the idle gate exempting the descendants of the job's own processes;
   - the real idle collector, run in observe-only mode for at least 15 minutes.
5. **EJ4.**
6. **The A6 content-admission analysis, with no launch.**
   - From 4.25 engine source, plus a static scan of the plugin `Content` package headers, predict exactly what a 4.25 editor does with mounted 5.1-saved packages: the registry scan, warnings, any startup load, thumbnails, cook.
   - If the prediction includes a crash, a blocking dialog or a startup load, **propose a content-isolation design for chat before A6.**
   - Also draft the A6 owner card: the exact wrapper command, and what to click.
7. **Normal rebuilds** of the changed probe and host sources in all three hosts (Editor and Game), under EI2. Bind the new hashes.
8. **The dated prediction revision,** committed before any authoring leg.

**Owner:** nothing now. Your hands-on step is in A6, the first 4.25 editor launch. I'll send you a short card for it.
