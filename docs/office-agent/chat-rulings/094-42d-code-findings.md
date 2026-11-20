# 094-42d — Code findings: D:\IA_BankOverflow delete check (read-only)

Claude Code, 2026-11-03, about 12:25–13:05 IST. Owner list: `_reviews\094-42d-OWNER-DELETE-LIST.md`. Receipts: `D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\code-receipts-094-42d\`.

**VERDICT: NEEDS-DECISION.**
- **No data dependency:** nothing reads, runs or links to the files.
- **One fail-closed check would fail:** the committed kit's P9 preflight (`owner_p9.py`) still expects the three BankOverflow roots, and two owner launchers run it.
- **The decision needed:** approve P9 v3 and three kit lines (§5 A).

The delete itself is data-safe and can happen while no job runs; v3 can be written after it, as v2 was in 094-07.

## 1. m51 against P9 (read-only git, GIT_OPTIONAL_LOCKS=0)
- **Start and exit both PASS:** HEAD `53bf725`, branch m51, index `526aba7f`, 0 tracked changes, the same 15 untracked entries in order. Extra and missing are both empty.
- master `b5f15a3`, release/2026-10-05 `d7eee97`, 30 tags (tag-ref SHA `4ee29fb7e50d…`, the same as 094-42r's exit), 74 refs.
- Receipts: `m51-gate-start.json`, `m51-gate-exit.json`.

## 2. Sizes
Measured with `os.stat`, deduplicating by file id (volume + index). Receipts: `sizes-a-summary.json` and `sizes-a.json.gz` (every file).

| Entry | Files | Apparent B | Unique B (hard links once) | On-disk B (4 KiB clusters) |
|---|---|---|---|---|
| `followup-capture-J2-donor-09314-9781031-D8D30951-DFE6F164-OVERFLOW-UNQUALIFIED` | 88,147 | 63,251,421,988 (58.907 GiB) | 16,910,284,655 (15.749 GiB) | 16,955,228,160 (15.791 GiB) |
| `_r9315` | 1,356 | 4,899,658,417 (4.563 GiB) | same | 4,903,383,040 (4.567 GiB) |
| `_binary_baselines` | 80 | 2,713,090,244 (2.527 GiB) | same | 2,713,264,128 (2.527 GiB) |
| `09102-final-binaries.zip` | 1 | 739,783,904 (0.689 GiB) | same | 739,786,752 (0.689 GiB) |
| **Total** | **89,584** | **71,603,954,553 (66.686 GiB)** | **25,262,817,220 (23.528 GiB)** | **25,311,662,080 (23.573 GiB)** |

- **Hard links:** 22,626 unique files. 77,031 entries (all in the OVERFLOW archive) have more than one link; the most links on one file is 648.
- **Every link is inside `D:\IA_BankOverflow`:** for 0 files is the in-folder link count below the file's link count. No hard links are shared across the four top entries.
- **No reparse points inside; 0 errors.**
- **D: free rises by about 23.57 GiB, from 37.56 to about 61.1 GiB.**
- **Cross-check against P9:** v1 and v2 each hold exactly 89,583 BankOverflow lines (70,864,170,649 B; per entry 88,147 / 1,356 / 80). The zip is the one extra file now on disk. It predates P9 v1, but the allowlist roots cover only the three subfolders.
- **The zip** holds 3 × 8 files, exactly the three `09102-*` folders in `_binary_baselines`.

## 3. The E: twin
- `D:\IntrusiveAnomalies\_binary_baselines\followup-capture-J2-donor-09314-9781031-D8D30951-DFE6F164-UNQUALIFIED` **does not exist.** `_binary_baselines` (→ `E:\IA_BuildCache\_binary_baselines`) holds only `release-2026-10-05-23098fa-218BA4C7`.
- **History:**
  - 093-14 recorded it on 10-16 at 26,577 files (about 21.4 GB), with the `INCOMPLETE-ATTEMPT.txt` marker.
  - Ruling 094-01 EA9: the owner deleted it at about 10:59 IST on 10-16; only the marker had no counterpart in the D: OVERFLOW copy.
  - It has 0 lines in P9 v1, v2 and the removed manifest.
  - The OVERFLOW junction that once sat on E: (`E:\IA_BuildCache\_binary_baselines\…-OVERFLOW-UNQUALIFIED` → D:) was removed in 094-09 (step 1 of the 094-05 list).
- **Nothing joins the list.**

## 4. Dependencies (every hit classified)
**Search coverage:**
- the plugin repo, all 74 refs (56 unique trees, `git grep -i`), plus the three untracked scripts at the m51 root;
- `_relay` (top-level scripts, drafts, dashboard, every run folder; text up to 20 MB);
- `_mailbox` (scripts, briefs, processed, to_chat) and `E:\MailBox` (the canonical watcher);
- `_reviews\094-*` and the older `_reviews` top-level scripts;
- `D:\IA_Europa` (Tools, Launch, Receipts, Temp scripts) and the P9 baseline files;
- CaptureBench and `E:\IA_Fixtures`;
- links: a reparse scan of `D:\IntrusiveAnomalies`, `D:\IA_Europa`, `E:\IA_BuildCache`, `E:\IA_Fixtures` and `E:\UEProjects` (66,997 folders), plus the C:/D:/E: top levels;
- the environment (User, Machine, Process), scheduled tasks and running processes.

Hit receipts: `search2-hits.jsonl`, `gitgrep-hits.json`, `links-into-delete-set.json`.

**No data or function dependency was found:**
- no junction or symlink points into the set (56 links found, 0 into it);
- no hard link reaches outside it;
- `IAI_BANK_OVERRIDE` is unset in all three scopes, and no environment value names BankOverflow;
- no scheduled task names it;
- no process names it (only this job);
- no source or tool code on any branch names it.

**Checks and settings that would fail or misroute once the folder is gone:**

| # | Where | What happens without the folder | Live? |
|---|---|---|---|
| L1 | Kit `tools/europa/owner_p9.py` (`feat/europa-port` `f7b32f0`). Called by `owner_leg.py:95` (`entry-owner`) and `fixture_leg.py:107` (`entry-fixture`); launched by `D:\IA_Europa\Launch\Europa425-Editor-OwnerLeg.bat` and `Europa425-FixtureAuthoring.bat`. | **Fails:** `Path(...).resolve(strict=True)` at L127 raises on the three allowlist roots under BankOverflow. Even past that, L162–167 would list 89,583 MISSING and STOP; the leg then raises "P9 … preflight failed". Fail-closed. Last run: 094-27c exit (31-10). Not used by the 094-31…094-42r job line. | **Yes (fail-closed check)** |
| L2 | Kit `tools/europa/root_attribution.py:56` (`protected_roots` from P9 v2), used by `write_ledger.py:259-262` for `d_root` removals. | A removed `D:\IA_BankOverflow` is classed **RED** ("P9-protected root entry removed or changed"). This bites only inside a job whose operation ledger already holds it. No live ledger exists now: the newest `current.sqlite` is 094-27c, 31-10, and `Receipts\09442` has no ledger. | Latent |
| L3 | `_relay\gdp-impl-block-v1.txt` L11 (item 4: allowed write "D:\IA_BankOverflow (evidence, archives)") and L22 (item 11: "when E: is below its floor, put new work folders and evidence on D:\IA_BankOverflow"). This text is in every Codex resume prompt (e.g. `resume-prompt-094-42r.txt` L105/L116). | Nothing fails; a Codex job with E: under 50 GB would recreate the folder. E: is at 130.9 GiB now. | Routing text (chat's) |
| L4 | `_relay\relay_run_codex_gdp_impl_v1.ps1` L53 (a default `--add-dir`). | Harmless: L70 drops paths that fail `Test-Path`. | No |
| L5 | `_relay\dashboard\config.json` L10 (`artifacts`). | Harmless: `server.py:360` resolves non-strictly; the dashboard is not running. | No |

**Historic mentions only (nothing runs from them):**
- **Plugin repo docs:**
  - `CLAUDE.md` on 17 refs;
  - journals 084-09, 085-04, 089-03, 089-04, 090-10e, 090-10f2, 090-12, 091-02, 091-03 and 093-14/15;
  - `docs/predictions/093-15-…`;
  - `docs/europa/09434-live-capture-receipts.json` (`refs/backup/europa-pre-fv6-09440`).

  The same docs appear in the `E:\IA_Fixtures\Outline425` plugin worktree.
- **`_reviews`:**
  - 094-00 to 094-28 docs, including rulings 094-04 L105, 094-05 L54, 094-06 L89 and 094-08 L105 (see §5 D);
  - `094-01-j2-pause-receipts\*` (the J2 pause baseline).
- **Older bench harnesses** default their bank root to `D:\IA_BankOverflow`: `089-01-tables.py`, `089-03/089-04/090-06-common.py`, `089-04-gateselftest.py`, `089-04-window.py`, `090-06-gateselftest.py`, and `090-10f2-common.py`/`-cook.ps1`/`-author-bias.py`.
  - The banks they read (`M52S09_*`, `M53DC*`, `_r910f2`, `_r912`) are already gone from the folder.
  - Re-running any of them would recreate the folder. Nothing runs them now.
- **Relay run folders:** prompts, events, reports and J2's 093-06 receipts (`baseline.ps1`, `rehash.ps1`), the 090-12 draft, and the 094-0x J2 pause inventories.
- **Mailbox:** processed briefs and to_chat reports.
- **P9 receipt files:** v1/v2 `full-sha256.jsonl`, `allowlist.json`, `protected-links.json`, the `*-inventory-pause.json` files, `v2-receipt.json` kept_paths.
- **Every Europa job's `entry.json`/`exit.json`:** `roots/D:` lists the name; `exit0944x.py` records `root_changes` with no assert.
- **Root-name privacy scans:** `rootnames38/39.py`, `fv6check40/40b.py`, `roots37.py` (BankOverflow sits in their generic list).

**A J2 note (an accepted loss, now measured):** `_r9315` holds 97 `.py`/`.ps1` scripts.
- 0 are in any git ref.
- 81 are byte-identical in `code-receipts-094-05\cleanup\courtesy-r-folder-scripts-docs.zip`, and 11 exist elsewhere.
- **16 exist only in `_r9315` (75,255 B):** `archive09315.py`, `assess_j1.py`, `audit09315.py`, `bench.py`, `complete_pie09315.py`, `cook_candidate.py`, `final_gate_audit09315.py`, `install_candidate09314.py`, `pairing_gate.py`, `preflight.py`, `prepare_packaging_storage09314.py`, `reassess_stack09315.py`, `state.py`, `strings09315.py`, `tsr_box_assess.py`, `tsr_served_receipts.py`.
- "Rebuild from git" would mean rewriting these. Nothing was copied. Receipt: `r9315-script-copies.json`.

## 5. Snapshot checks: where, and the proposed change (nothing edited)

**A. Kit (`feat/europa-port`; live; chat rules)**
- **A1.** First write P9 v3 by the 094-07 method: v2 minus every line under `D:\IA_BankOverflow`, i.e. 472,552 − 89,583 = 382,969 lines. In v3's receipt, `removed_roots` = v2's roots plus `D:\IA_BankOverflow`. Then `owner_p9.py` needs **three** lines, not one:
  - L17: `baseline-v2` → `baseline-v3`;
  - L19: `v2-receipt.json` → the v3 receipt name;
  - L64: the v2 SHA constant → v3's.

  `allowlist.json` needs no edit: L125 `excluded(raw)` then skips its three BankOverflow roots. No safe one-line change avoids v3. Adding the root to `removed` alone breaks `derive()`'s byte-for-byte re-derivation.
- **A2.** `root_attribution.py:56`: `baseline-v2/full-sha256.jsonl` → `baseline-v3/full-sha256.jsonl` (one line).

**B. P9 receipts (immutable; superseded by v3, not edited):** `baseline\allowlist.json` (3 roots), `baseline\protected-links.json` (the old link entry), the v1/v2 `full-sha256.jsonl` files, and `baseline-v2\v2-receipt.json` kept_paths.

**C. Code's own start/exit checks.** None is in current use; the last BankOverflow snapshot was 094-28.
- **C1.** `verify_keep*.py` (`code-receipts-094-05…094-28\protect\`; the last one is `verify_keep28.py`), block `bankoverflow_vs_v1` (L31–48). This is the check ruling 094-06 L89 created. **Proposal:** don't revive it; any future keep check drops that block.
- **C2.** `snap*.ps1` (last `snap25.ps1` L34): the free-space path list. Drop `'D:\IA_BankOverflow'` if it is ever reused.
- **C3.** The current `entry09427d.snapshot()` `roots` list and `exit0944x.py` `root_changes` are informational: no change. If the folder disappears mid-job, `root_changes.D:.removed` = `["IA_BankOverflow"]`. Expected, not a finding.

**D. Chat's text (listed only)**
- **D1.** Ruling 094-06 L89 — "Code checks `D:\IntrusiveAnomalies\StackOBot\Content` and `D:\IA_BankOverflow` against their snapshots at its start and again after Codex exits…" → supersede with a line that drops BankOverflow (owner deletion of 11-03).
- **D2.** Rulings 094-08 L105 ("`D:\IA_BankOverflow` against v1;"), 094-05 L54 ("`D:\IA_BankOverflow`, unchanged.") and 094-04 L105, plus 094-00 L116 (J2 evidence includes `_r9315`) → superseded by the same deletion.
- **D3.** The brief "Don't touch" line, in every brief from 094-29 to 094-42r (094-42r L75) → drop "`D:\IA_BankOverflow`,".
- **D4.** The brief "protected-state check" (094-27 L45, 094-28 L42: "StackOBot Content, BankOverflow, …; any loss is RED") is already absent from 094-29 on; keep it out.
- **D5.** The implementer block, `_relay\gdp-impl-block-v1.txt`:
  - L11: drop "D:\IA_BankOverflow (evidence, archives);".
  - L22: replace "put new work folders and evidence on D:\IA_BankOverflow" with chat's choice (for example "stop and report").

**E. Relay scripts (live; harmless; optional cleanup, not edited)**
- **E1.** `relay_run_codex_gdp_impl_v1.ps1` L53: delete `'D:\IA_BankOverflow',`.
- **E2.** `dashboard\config.json` L10: drop `"D:/IA_BankOverflow"` from `artifacts`.

## 6. Safety
- **Writes:** only `code-receipts-094-42d\` and `_reviews\094-42d-*`.
- **Nothing else:** nothing deleted, moved or killed; no builds, launches, Codex, commits or pushes.
- **Priority and git:** my own scanning processes ran at idle priority; read-only git with `GIT_OPTIONAL_LOCKS=0`; the owner's render processes were not touched.
- **No secrets** were printed or copied.
- **One stop:** I stopped one of my own background searches after it lost CPU to the render, and relaunched a narrower one.
