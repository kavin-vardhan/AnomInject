# 094-43r — pre-push scan of `feat/europa-port` (Claude Code, 2026-11-04, about 05:05 IST)

**PUSH-READY: yes. Scan clean.** No secrets, no codenames or titles, no drive-root listing, no binaries or large files. One known non-secret identifier class is carried (§2): the owner's Windows profile name in a `C:/Users/<profile>/…` path, already public on origin. This push adds 8 more lines of it, inside quoted UBT build warnings.

## Scope (read-only git, `GIT_OPTIONAL_LOCKS=0`)
- **Tip and origin:**
  - Tip: `feat/europa-port` = `93613ae`, clean worktree.
  - `origin/feat/europa-port` = `d1cdc7b`, an ancestor of the tip, so this is a fast-forward.
  - `d1cdc7b..93613ae` is the scanned range; everything before it was scanned in 094-40 and is already public.
- **What the push sends:**
  - 20 commits not on any origin ref, 0 merges: 094-42r, 094-42c, 094-43a and this job's 6.
  - 20 commits, 121 trees, 144 blobs; 24.7 MB uncompressed; largest blob 822,859 B (`docs/gotchas.md`).
  - Neither spike commit is in it: `740220c` and `42ea37e` are not ancestors of the tip.
- **What was read:**
  - every added line in the range (8,008 lines, intermediate states included);
  - the tip content of all 53 changed files;
  - every commit message, author and committer.
- **Push mechanics:** no hooks, no `push.followTags`, no `push.default` or upstream set, no tag at the tip. Only `feat/europa-port` is named. The local-only `refs/backup/europa-pre-fv6-09440` does not go.
- **Receipts:** `code-receipts-094-43r\prepush\` contains:
  - `prepush_scan43r.py`, `scan_summary.json`, `scan_findings.txt`;
  - `extra43r.py` / `extra_summary.json`, `paths43r.py` / `path_roots.txt`, `terms_prose43r.py` / `terms_candidates_prose.txt`.
  - These are 094-40's scripts with only the base changed to `d1cdc7b`.

## 1. Secrets — 0
- **Name = value hits:** 14 at the tip, 4 added in this range. All are rule-citation `authority` / `author_receipt` strings, a code expression (`owner_p9.py:48`, `write_ledger.py:108`) or the long-public placeholder at `docs/gotchas.md:2469`.
  - The 4 added ones are `owner_ref_policy.py:57` (`d2f481d`), `09442-guard-fixture-receipts.json:333,341` (`f7b32f0`) and `capture_c5_kit.py:20` (`08215aa`).
- **Private keys, bearer/basic auth, URL user:password, cloud key shapes:** 0.
- **Known control-server token (checked by value, not printed):** 0. The Paris host's `DefaultGame.ini` has no `Token` line.

## 2. Account and machine identifiers
- **E-mail addresses in content:** 0.
- **GitHub account, machine name, Windows SIDs:** 0.
- **Author and committer:** one identity on all 20 commits, the same as on all 503 `origin/master` commits.
- **Carried, non-blocking, non-secret: the owner's Windows profile name.**
  - Already public on origin at `tools/europa/write_ledger.py:22`, `CLAUDE.md` and `docs/onboarding.md` (tip lines, not added here).
  - New in this range: 8 lines in `docs/europa/09443-private-capture-receipts.json:3445-3503` (commit `93613ae`). They are UBT's own warnings about `C:/Users/<profile>/AppData/Roaming/Unreal Engine/UnrealBuildTool/BuildConfiguration.xml`, quoted in the build receipts.
  - No new identifier appears; the same name is just repeated.
- **`docs/gotchas.md:2524` and `:4688`:** generic `/c/Users/.../` lines, already on origin.

## 3. Codenames — clean
- **Scrub table** (`m36_scrub_hostname.py`, selftest passed): 0 hits in the range and 0 at the tip outside the printed `AnomalyCapture.Build.cs` exclusion.
- **Commit messages:** 0 names.
- **Eyeball list:** title-case and CamelCase words not used in lower case. All are engine, config or code terms: DevOptions, LogShaderCompilers, NumUnusedShaderCompilingThreads, BelowNormal, ShaderProbe, PrimitiveVisibilityMap, CreateRenderQuery, DistccExecutablesPath, ProcessorCountMultiplier, the engine default-material names, BaseLogFileName, MinFilesUsingPrecompiledHeader, UnsafeTypeCastWarningLevel, WorldPositionOffset, and so on.
  - All-caps tokens: WPO, API, FXC, DLL, GATES, GT, LNK, SRV.
- ⚠ **Limit, as in 094-40:** the scrub table covers the Concorde/Bates terms. Europa's own title is covered only by the eyeball list.

## 4. Drive-root listings — clean
- **Density:** 0 lines with 3 or more root names; the most distinct root names in one commit's file is 4; 0 files hold 10 or more.
- **Drive paths in added lines:**
  - `E:/IA_Fixtures` (207), `D:/IA_Europa` (50), `E:/EpicGames` (13);
  - `C:/Users` (8, the UBT lines in §2);
  - `D:/IntrusiveAnomalies` (5) and one `E:/fixture`.

## 5. Size and type — clean
- Blobs over 1 MB: 0. Binary blobs: 0, at the tip and in history. `.uasset` / `.umap`: 0.
- Paths under `Receipts`, `Saved`, `Intermediate` or `Binaries`: 0.
