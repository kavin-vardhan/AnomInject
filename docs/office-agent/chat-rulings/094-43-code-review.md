# 094-43 — Code's review: Codex stopped at the entry gate (the first full P9 `main()` fails on four known refs); Stages A and B did not start; no push

**Date:** 2026-11-04, about 02:30 IST · **Author:** Claude Code (mailbox launcher and reviewer) · **These are observations, not rulings.**

**How it ran:**
- Resumed thread `01a14334` with `-OwnerApprovedFullAccess`, detached, W4 applied (no `GIT_*_DATE` in the child), after a dry run of my launch gates.
- Ran 01:57:03 → 02:04:04 (421 s), exit 0. Codex verdict: **NEEDS-DECISION**. The resume itself worked, so no second attempt and no fresh thread.
- Report collected byte-identical as `_reviews\094-43-codex-report.md`. Receipts: `code-receipts-094-43\`. My readers and probe: `D:\IA_Europa\Temp\CodeReview09443\`, `D:\IA_Europa\Receipts\CodeReview09443\p9probe\`.

## 1. The stop, verified
- **What ran:** FJ1's fast checks (pass), then one unmodified `owner_p9.py main()` through Codex's wrapper `p9_main09443.py` (imports the module, `sys.argv=['entry']`, job config `Receipts\09443\entry-job-config.json`). `derive()` passed (707,775 → 382,969 + 324,806 rows). `git_proof` then failed at `owner_p9.py:72` after 7.8 s.
- **Why, by my own recompute** (`p9refs43.py`): the P9 baseline (`Receipts\P9\baseline\git-state.json`, sha256 `e9a6e184…`) lists 71 refs. Today there are 76; with local feat excluded, 75. All 71 baseline refs are present, unchanged and in baseline order; nothing is missing. The **4 extras** each trace to a recorded job:

  | Ref | Commit | From |
  |---|---|---|
  | `refs/backup/europa-pre-fv6-09440` | `08fe652` | 094-40, FV6 backup (local only) |
  | `refs/remotes/origin/feat/europa-port` | `d1cdc7b` | 094-40 push |
  | `refs/heads/spike/europa-09442c-mesh-shader` | `740220c` | 094-42c |
  | `refs/heads/spike/europa-09443a-depthonly-byname` | `c1193bd` | 094-43a |

- **The checker is unchanged:** `git hash-object` of `tools/europa/owner_p9.py` equals its `75ca580` blob `ffd9311` (raw sha256 `d878704a…`, as Codex recorded); the feature worktree is clean.
- **Consequence:** this was the first end-to-end run of `main()` (GC1 owed it). Its ref line excludes only local feat, so it would have failed at any time since 094-40 created the backup ref and the remote ref. P9 v3 (`de29514`, 11-03 17:27) kept that line. **The stop is a stale gate baseline, not a change made by this job.** Codex stopped as the request requires and did not touch the checker or the baseline.

## 2. My probe of the rest of P9 (review evidence, not the GC1 gate)
To learn whether anything after line 72 would also stop the job, I imported `owner_p9` unchanged and replaced only `git_proof`'s ref line with: "every baseline ref present and in order, and the extras are exactly these four refs at these SHAs" (`p9probe43.py`). Outputs went only to `Receipts\CodeReview09443\p9probe\`. **371 s, PASS on every step:**
- `git_proof`: the 2 protected worktrees are equal, 5 are EG5-exempt, and the three owned checkouts are clean.
- Inventory: 382,970 files, **0 differences** (EG1 admitted 1,023).
- Binary/config rehash: 22,628 files (100.7 GB), all equal.
- Source manifests equal: 4.25 original (1,511 files), 5.1 base (2,756).
- Toolchains: 4.25 MSVC 14.16.27051 / SDK 10.0.17763, 5.1 port MSVC 14.38.33144 / SDK 10.0.22621.
- `git fsck --connectivity-only` exit 0; the 09405 ledger SQLite `integrity_check` ok (opened read-only).

After the probe the m51 and refs gates still pass (same refs sha `99bf1c84a2b0`), and the Paris worktree index is untouched since 01:02:07. **So, as of 02:19 IST, admitting the four refs is the only change P9 needs to pass.**

## 3. Stages A and B: not run
- GD3 was not implemented. A-P2..A-P5 and H1..H5 do not exist, so there is nothing to recompute.
- No build, launch, commit, merge, push or tag. `feat/europa-port` is still `75ca580`; both spikes are local and unchanged.

## 4. Safety: nothing
- **Crashes or upload signs:** none. No UE4Editor, game, CrashReportClient, ShaderCompileWorker or UBT process appeared in my polls. ⚠ My polls were 01:57:38 and 02:04:19, 6.7 min apart (I was reading for the plan; the brief asks 2–3 min during likely builds or sessions). The evidence that nothing launched does not rest on them: 0 binary changes, no new `Saved` files on the Paris host or the fixture, and Codex's own receipts.
- **Paris:** Content (1,456), plugin Content (5), Config (18), host and plugin Binaries (16 + 11), descriptor and `.uproject` hash-equal before and after; `Saved` and Intermediate listings unchanged.
- **Fixture:** the seven assets and the preserved `Engine.ini` (`64d45f80`) hash-equal.
- **`m51`** passed at start, prelaunch, exit and after the probe (HEAD `53bf725`, index `526aba7f`, 15 untracked); `master` `b5f15a3`, release `d7eee97`, 30 tags and tag refs unchanged.
- **Tree lists** (D:\IA_Europa, the run folder, E:\IA_Fixtures): 0 removed, 0 binaries changed. The only additions are Codex's 8 receipt files under `Receipts\09443`, my scratch, and the run folder's resume files.
- **Push:** none (`ls-remote` not needed: no push was attempted; the local `origin/feat/europa-port` is still `d1cdc7b`).
- **Foreign activity:** none seen.

## 5. Stage C: not done
The gates did not run, so nothing was scanned or pushed and no office card was written (Codex's card draft does not exist yet).

## 6. Phase B plan
Written: `_reviews\094-43-phaseB-plan.md`. Headline: the three anomalies' release code is already on the port branch, compiled out on 4.25; the 5.1 material assets cannot load on 4.25 and must be re-authored; camera_clipping needs a session-global hold and a whole-frame label path in the 4.25 capture; and the fixture outline colour equals the pink, so the leak gate must be paired.

## 7. Decisions for chat
1. **How P9 treats the four refs.** Options: admit them by name and SHA in the ref line (each traces to a recorded job), or regenerate the baseline ref list once. My probe says nothing else in `main()` blocks today.
2. **Re-issue 094-43** (the same thread resumes cleanly; this attempt used 7 minutes).
3. **Phase B decisions D1–D5** (in the plan).
