# REPORT for 094-42d-bankoverflow-delete-check.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 094-42d-bankoverflow-delete-check.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-11-03 12:24:14 | finished: 2026-11-03 12:59:41 | duration: 35 min | exit code: 0
context at last turn: 568587 tokens (57% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\094-42d-bankoverflow-delete-check.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

1. **For the owner:** deleting `D:\IA_BankOverflow` gives back about **23.6 GiB** on D: (37.6 → about 61 GiB). Explorer will say 66.7 GB, because hard links get counted more than once.
2. Nothing uses these files, so the delete is safe. Do it only when no mailbox job is running, with Shift+Delete; the steps are in `_reviews\094-42d-OWNER-DELETE-LIST.md`.
3. One catch: the Europa owner-leg and fixture-authoring launchers will refuse to start until chat updates their safety check. Tell chat once it's done.

**Why NEEDS-DECISION:** nothing needs the data. But the committed kit's protected-files check (P9) still expects three of these folders and would refuse once they're gone. Chat needs to approve a new P9 baseline (v3) plus three kit lines. The delete can come first; v2 was also written after the 094-05 deletion.

## Checks
- **m51 against P9:** passes at start and at exit. HEAD `53bf725`, index `526aba7f`, 0 tracked changes, the same 15 untracked entries. master `b5f15a3`, release `d7eee97`, 30 tags, 74 refs.
- **No link reaches in or out:** no junction or symlink points into the folder (about 67,000 folders scanned), and every hard link stays inside it. Nothing in the environment, scheduled tasks or running processes names it, apart from this job.

## Sizes (hard links counted once)

| Item | Files | Explorer shows | Space freed |
|---|---|---|---|
| J2 `…-OVERFLOW-UNQUALIFIED` | 88,147 | 63,251,421,988 B (58.907 GiB) | 16,955,228,160 B (15.791 GiB) |
| `_r9315` | 1,356 | 4,899,658,417 B (4.563 GiB) | 4,903,383,040 B (4.567 GiB) |
| `_binary_baselines` | 80 | 2,713,090,244 B (2.527 GiB) | 2,713,264,128 B (2.527 GiB) |
| `09102-final-binaries.zip` | 1 | 739,783,904 B (0.689 GiB) | 739,786,752 B (0.689 GiB) |
| **Total** | **89,584** | **71,603,954,553 B (66.686 GiB)** | **25,311,662,080 B (23.573 GiB)** |

- 77,031 files in the J2 archive are hard links, up to 648 links per file.
- The zip holds the same three `09102-*` builds that `_binary_baselines` already holds.
- The P9 v1 and v2 baselines each list exactly these 89,583 files (all but the zip).
- Explorer can delete all of it: the longest path is 206 characters, and nothing is read-only or hidden.

## The E: twin
It no longer exists. The owner deleted the incomplete J2 archive on 10-16 at about 10:59 IST (ruling 094-01 EA9). It was never in P9, and the old E: junction to the OVERFLOW copy was removed in 094-09. Nothing joins the list.

## Dependencies
**Data or function dependencies: none.** No source or tool code on any of the 74 refs names the folder.

**Checks and settings that name it:**

| # | Where | Effect once the folder is gone | Live? |
|---|---|---|---|
| L1 | Kit `tools/europa/owner_p9.py` (`feat/europa-port` `f7b32f0`). Called by `owner_leg.py:95` and `fixture_leg.py:107`, i.e. `Launch\Europa425-Editor-OwnerLeg.bat` and `Europa425-FixtureAuthoring.bat`. | Fails: L127 `resolve(strict=True)` raises on the three missing roots, and L162–167 would report 89,583 files missing. Fail-closed. Last run 31-10 (094-27c); the 094-31 to 094-42r jobs don't use it. | **Yes** |
| L2 | Kit `root_attribution.py:56` | Treats the folder as P9-protected, so its removal reads RED. Only bites inside a job whose ledger already holds it; none exists now (newest is 31-10). | Latent |
| L3 | `_relay\gdp-impl-block-v1.txt` L11 and L22, copied into every Codex prompt | Names the folder as the archive location and the overflow target when E: is under its floor. Nothing fails; a job with E: under 50 GB would recreate it. | Routing text (chat's) |
| L4 | `relay_run_codex_gdp_impl_v1.ps1` L53 | Harmless: L70 drops missing folders. | No |
| L5 | `_relay\dashboard\config.json` L10 | Harmless. | No |

**Historic mentions only:**
- plugin docs on 17 refs;
- `_reviews` 094-00 to 094-28 docs, plus older m52/m53 harnesses that default their bank folder here (those banks are already gone);
- relay prompts, reports and run receipts;
- the P9 receipt files;
- `entry.json`/`exit.json` root-name lists (recorded without any assert).

**J2 note:** of the 97 scripts in `_r9315`, none are in git. 81 are in the 094-05 courtesy zip and 11 exist elsewhere. **16 (75 KB) exist only there**; they're named in the findings file, and a J2 restart would have to rewrite them.

## Snapshot checks and proposed changes (nothing edited)
- **A1, kit:** first write P9 v3 the 094-07 way (v2 minus BankOverflow, 382,969 lines; its removed roots gain `D:\IA_BankOverflow`). Then `owner_p9.py` needs three lines, not one: L17 `baseline-v2`→`baseline-v3`, L19 the receipt name, L64 the hash constant. `allowlist.json` then needs no edit. No safe one-line change works without v3.
- **A2, kit:** `root_attribution.py:56` → read the v3 baseline (one line).
- **B, P9 receipts:** immutable; superseded by v3, not edited.
- **C, Code's own checks:**
  - C1: the `verify_keep*.py` BankOverflow block (L31–48, last used in 094-28); don't revive it.
  - C2: `snap25.ps1` L34; drop the path if the script is reused.
  - C3: the current entry/exit root lists need no change. If the folder vanishes mid-job, `exit.json` will record it as removed; that's expected, not a finding.
- **D, chat's text:**
  - D1: ruling 094-06 L89 ("Code checks … `D:\IA_BankOverflow` … at its start and again after Codex exits") → supersede.
  - D2: rulings 094-08 L105, 094-05 L54, 094-04 L105 and 094-00 L116 → superseded by the owner's deletion.
  - D3: the brief "Don't touch" line (094-42r L75) → drop `D:\IA_BankOverflow`.
  - D4: keep the old "BankOverflow loss is RED" line out of briefs; it's been absent since 094-29.
  - D5: implementer block L11 → drop the folder from the allowed writes; L22 → a new overflow rule (for example "stop and report").
- **E, relay scripts (optional cleanup):** E1 delete launcher L53; E2 drop the folder from the dashboard's `artifacts`.

Full detail is in `_reviews\094-42d-code-findings.md`; raw evidence is in `code-receipts-094-42d\`.

## Safety
- I wrote only to `code-receipts-094-42d\` and `_reviews\094-42d-*`. Nothing was deleted, moved or killed, and nothing was built, launched, committed or pushed; no Codex.
- Scans ran at idle priority, git was read-only, and the owner's render processes were not touched.
- No secrets were printed or copied.
- I stopped one of my own background searches after the render starved it of CPU and ran a narrower one instead.
api-equivalent cost (not billed on Max): $15.47 | turns: 107 | running total across all briefs: $5,531.53
