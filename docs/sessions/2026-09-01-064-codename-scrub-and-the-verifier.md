# 2026-09-01 — session 064 — the codename scrub, and the verifier that could not fail

> **Scope:** host and engine-lineage identifiers are replaced by the project codenames **Bates** and
> **Concorde** across every ref. No source, no behaviour, no gates. The instrument that certifies it
> was rebuilt mid-session because it was incapable of reporting a problem.

---

## 1. THE INVARIANT

**CODENAME-ONLY.** Hosts and their engine lineage are written as **Bates** and **Concorde**. The
scrub terms are **NAMES OF THINGS** — a host's title, its abbreviation, and the two lineage labels.

⛔ **SCOPE LINE, FINAL — this is what stops the scope widening again:** internal fork identifiers and
`FW`-prefixed symbols in engineering records are **NOT terms** and stay exactly as they are. A symbol
that a compiler, a matcher or a log parser reads is not a name of a thing.

📌 **"NOWHERE" IS DEFINED, ADOPTED FOR THE RECORD:** *no reachable ref, origin or local.* Unreachable
remote objects survive a force-push on a host we do not administer and are served by SHA until that
host's own maintenance removes them. **That is outside our control, it is stated once here, and no
claim in this project should mean more than "no reachable ref".** Local pruning IS provable and was
proved (§4).

---

## 2. THE INSTRUMENT — `CaptureBench/tools/m36_scrub_hostname.py`

Untracked, in a local-only repo with **no remote**, and its HEAD carries none of the terms — checked,
because the instrument necessarily contains what it matches.

- **Term table stored base64-ENCODED and decoded at runtime**, so **no plaintext copy of a scrubbed
  identifier exists in any file on disk**, tracked or not. Verified per term.
- **`--selftest` runs on EVERY invocation, before any verdict** — not on request. It writes a synthetic
  fixture containing every term to a temp file, requires the verifier to **FIRE**, requires the mapping
  to then **CLEAR** it, and deletes the file in the same run. **A failed selftest returns exit 2**, so a
  broken verifier cannot grade a tree. This replaced the pre-scrub backup branch as the known-answer
  control, which is what made deleting that branch safe.
- **Whole-tree by default** (`--tree`): every tracked file plus untracked `docs/`, text-vs-binary decided
  by attempting the decode. **No extension list** (`G191`).
- **Permanent exclusion, PRINTED on every run, never silent:**
  `Source/AnomalyCapture/AnomalyCapture.Build.cs` — *[2026-08-31: fork-detection needles; owner-ruled
  unalterable]*.

### 2.1 THE ORDERED MAPPING — three collisions a naive pass would have produced

| pattern | replacement | what a naive pass gave |
|---|---|---|
| `packaged name <exe>` | `packaged exe name deliberately withheld (codename-only invariant; visible on the office box itself)` | leaked the literal |
| `Concorde/<lineage>` | `Concorde` | **`Concorde/Concorde`** |
| `Bates/<lineage>` | `Bates` | **`Bates/Bates`** |
| `<lineage>-fork` | `Bates-lineage` | `Bates-fork` — reads as if the host forked itself |
| `Concorde (<lineage> fork:` | `Concorde (forked engine loop:` | `Concorde (Concorde fork:` |

⇒ then the generic term→codename rules. **Order is the mechanism.**

---

## 3. WHY `AnomalyCapture.Build.cs` IS NEVER TOUCHED

Its strings are **`StartsWith` needles for the build-time tick-pin fork probe (route C)** — the
mechanism, not a reference to it. Substituting them makes the probe search for files that do not
exist, route C stops firing, and the tick-pin compile decision changes **silently**.

🚨 **AND THIS BOX STRUCTURALLY CANNOT DETECT THAT.** There is no forked engine loop here, so route C
correctly reports *not fired* **whether the detector works or not**. A home run cannot distinguish a
working probe from a broken one. **The office box is the only positive control this project has** —
which is why the runbook now says, at §8.6 after the editor rebuild: **read the tick-pin probe echo
and record which route fired.** It is a log read. Owner ruling: **the file is never altered**, and no
encoding work is scheduled.

---

## 4. THE LEDGER — before → after, per ref

Every run began `SELFTEST ok: fires on a synthetic all-terms fixture, and the mapping clears it.`

| ref | before | after | commit |
|---|---|---|---|
| `master` | TERMS PRESENT, **10** files / 178 scanned | **clean over 178** | `92d3648`, 10 files 13+/13− |
| `feature/mask-gpu-reduce` | TERMS PRESENT, **12** files / 188 scanned | **clean over 188** | `5dcfb6a`, 12 files 16+/16− |
| `feature/selection-census` | TERMS PRESENT, **8** files / 194 scanned | **clean over 194** | `7040a99`, 8 files 9+/9− |

Re-verified afterwards against **origin's own copies**, not the local intent: `origin/master`,
`origin/feature/mask-gpu-reduce`, `origin/feature/selection-census` — **all clean, exit 0**, exclusion
printed on each.

**Backup retired.** `backup-pre-scrub-2026-08-31` (was `221668a`) deleted; reflogs expired; `gc
--prune=now`. All four pre-scrub commits confirmed **unreachable** afterwards — `221668a`, `1b7014a`,
`79f5d6c`, `e55c7b8`. Worktrees were removed first, because a worktree holds refs and reflogs of its own.

### 4.1 RECEIPT FOR "ONE SCRIPT, ONE MAPPING"

The reason all three refs were scrubbed by the **same instrument in the same pass** is that shared
lines then come out **byte-identical**, so the merge and the rebase have nothing to resolve. Measured,
not assumed:

```
merge-tree --write-tree master <- feature/mask-gpu-reduce : exit 0, tree OID only, NO conflict list
```

| file | result |
|---|---|
| `docs/sessions/2026-08-21-050-m31-fix-handshake-rekey.md` | **IDENTICAL blob on all three refs** |
| `docs/predictions/2026-08-21-m31-s1-branch-table.md` | **IDENTICAL blob on all three refs** |
| `docs/architecture.md` | **IDENTICAL blob on all three refs** |

⚠ If the m36 rebase later drops a scrub commit as **empty**, that is **correct** — the change is
already present via master. Note it; do not fight it.

### 4.2 REFS DELETED FROM ORIGIN — recorded here so the record no longer depends on the remote

Both were parked **gate-failure** branches carrying pre-scrub trees. **Local branches are RETAINED as
evidence**; only the remote refs were removed. Verified before deleting that local was a superset of
origin in both cases, so nothing was lost.

| branch | local tip (kept) | origin tip (deleted) | what it is |
|---|---|---|---|
| `m29-GATE-FAILED-lod-popping-invisible` | `ab2fb41` | `c7c36fa` | m29 ships `corrupted_texture` only; `lod_popping` pool membership deferred to m30. Its gate failed and the branch was parked. |
| `s3a-2-GATE-FAILED-do-not-merge` | `087f4d9` | `087f4d9` | S3a-2 wiring that **fails its own gate `G-S3a-1`**. Parked, never to merge. |

**Remaining on origin:** `master`, `feature/mask-gpu-reduce`, `feature/selection-census` — all verified
clean — and **`wip/session-061-backup`** (`189f67f`), which is **deliberately dirty**: it verifies
`TERMS PRESENT` and is kept until the Bates office validation of the m35 hotfix passes (journal 061
§12.0 close-out checklist).

---

## 5. THREE INSTRUMENT FAILURES, ALL CAUGHT THE SAME WAY

Each was found by insisting on a positive artifact rather than a green.

1. **`G189` — the verifier could not fail.** It substituted, then checked the substituted text, so it
   answered *"would my fixes leave residue?"* — always no. It printed the term counts **and** `[CLEAN]`
   and **exit 0** on the known-positive pre-scrub tree. **That is the check that licensed deleting the
   only backup.** Caught by `G96`'s rule: prove the checker against a known answer *before* its verdict
   is read.
2. **`G190` — an exit code outlived a command that never ran.** The file list exceeded the Windows
   command-line limit, python never launched, and the wrapper printed `exit=0` from a stale
   `$LASTEXITCODE`. Caught only because the mandatory `VERDICT:` line was absent.
3. **`G191` — an extension list is a blind spot.** The first work-list globbed doc extensions and missed
   a `.cs` file — **the one file where the strings were load-bearing.** The blind spot and the
   highest-stakes hit coincided, which is the usual shape.

📛 **And one of mine: `G141`'s third surface.** A commit message written with
`Set-Content -Encoding utf8` put a **BOM at the start of a pushed commit subject**. Worse, the check for
it returned a **false negative**, because the same shell strips U+FEFF when decoding `git log`. Only
`git cat-file commit`, read outside the shell, showed it. Fixed by a message-only `--amend` (tree OID
captured before, compared after, identical) and a lease-checked push of that one branch.
**Standing rule:** commit messages go through the editor path only; message bytes are verified with
`git cat-file commit`, never a shell-decoded `git log`.

---

## 6. STANDING RULES ADDED THIS SESSION

- **A fix step and a check step must read different inputs** (`G189`).
- **Assert on a positive artifact of the run, never on an exit code alone** (`G190`).
- **Enumerate from the repository, not from a guess; print the counts; exemptions are printed, never
  absences from a glob** (`G191`).
- **Commit messages: editor path only; verify bytes out-of-shell** (`G141`).
- **Amend authority:** chat may authorise a lease-checked amend of an **unmerged branch's TIP** commit
  until the office has pulled that tip. ⛔ **master and tags never. Anything deeper is STOP.**
  ⚠ *"No office pull has occurred"* is **owner-asserted and not measurable** — git cannot observe a
  pull. What is measurable, and was checked, is that nothing had been pushed on top of the tip.
