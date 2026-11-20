# 094-40 — pre-push scan of `feat/europa-port` after FV6 (Claude Code, 2026-11-02, about 04:58 IST)

**PUSH-READY: yes. Scan clean.** No secrets, no new account identifiers, no codenames or titles, no drive-root listing, no binaries or large files. One known non-secret item from 094-29 is carried unchanged (§2).

## Scope (read-only git, `GIT_OPTIONAL_LOCKS=0`)
- **Tip:** `feat/europa-port` = `d1cdc7b` (after FV6), clean worktree. `origin/feat/europa-port` does not exist (`ls-remote` empty), so the push creates the branch.
- **Scanned range:** `abb240b..d1cdc7b`, 67 commits = the 66 commits since `4f46a9c` plus `4f46a9c` itself. `d7eee97..abb240b` was scanned in 094-29 (`_reviews\094-29-prepush-scan.md`).
- **What the push sends:** 159 commits not on any origin ref (`d7eee97..d1cdc7b`, 0 merges): 159 commits, 885 trees, 1,119 blobs, 104.2 MB uncompressed, largest blob 811,585 B (`docs/gotchas.md`).
- **What was read:**
  - every added line in the 67 commits (37,764 lines, intermediate states included);
  - the tip content of all 160 changed files;
  - every commit message, author and committer.
- **Push mechanics:** no hooks, no `push.followTags`, no `push.default` or upstream set, no tag at the tip. The local-only `refs/backup/europa-pre-fv6-09440` is not named, so it does not go.
- **Receipts:** `code-receipts-094-40\prepush\`:
  - `prepush_scan40.py`, `scan_summary.json`, `scan_findings.txt` (values masked to shape);
  - `extra40.py` / `extra_summary.json` (root-listing density, push composition);
  - `terms_candidates_prose.txt`, `path_roots.txt`.

  The FV6 proof is in `D:\IA_Europa\Receipts\CodeReview09440\fv6-check-b.json`.

## 1. Secrets — 0
- **Name = value hits (TOKEN / SECRET / PASSWORD / KEY / AUTH / COOKIE / CREDENTIAL):** 57, triaged by context. None is a credential:
  - rule-citation strings in `authority` / `AUTHORITY` fields (for example `capture_c5_kit.py:20`, `runtime_lean_gate.py:36`, the `09433*`–`09440` receipt JSONs);
  - the `keyword_rotators='N/A: …'` test field (`capture_fx_tests.py:46`, `09440-office-capture-receipts.json:91`);
  - a code expression (`write_ledger.py:105`, `key = str(directory).casefold()`);
  - the long-public placeholder at `docs/gotchas.md:2469`.
- **Private-key blocks, bearer/basic auth, URL user:password, cloud key shapes:** 0.
- **The real control-server token:** 0. The StackOBot token was checked by value, without printing it. The Paris host's `DefaultGame.ini` has no `Token` line.

## 2. Account and machine identifiers
- **E-mail addresses in content:** 0.
- **GitHub account, machine name, Windows SIDs:** 0.
- **Author and committer:** one identity on all 67 commits, the same identity as on all 503 `origin/master` commits.
- **Carried from 094-29 (non-blocking, non-secret):** the owner's Windows profile name in a `C:/Users/<profile>/…` path at `tools/europa/write_ledger.py:22`. It is in the tip only, not added in this range. The older commits 094-29 listed are still in the push.
  - The same first name is already public on origin (`CLAUDE.md:8389`, `docs/onboarding.md:13`, every commit's author).
  - `docs/gotchas.md:2524` and `:4688` are generic `/c/Users/.../` lines already on origin.

## 3. Codenames — clean
- **Scrub term table:** the project's verifier table (`CaptureBench/tools/m36_scrub_hostname.py`) was loaded in memory after a passing selftest.
  - Its only hits are `Source/AnomalyCapture/AnomalyCapture.Build.cs:112` and `:261`, the permanent printed exclusion (the fork-detection needles).
  - Neither line is added in this range (0 history hits).
- **Commit messages:** 0 names.
- **Owner eyeball list:** title-case words in added prose that are not used in lower case in the tree, plus CamelCase words. All are engine, code or plain terms: BelowNormal, ControlServer, CaptureSubsystem, FrameCapturer, CharType, LabelWriter, MaskSceneViewExtension, PostActorTick, PrintPolicy, ControlSubsystem, PreviewCapture, SkySphereBlueprint, SourceCopy, ActiveSource, AsyncWriter, CaptureModule, DeferredShadingCommon, LogOutputDevice, PostConfigInit, PostRenderBasePass, PrePostProcessPass, Principal, SimpleGrassWind, StencilTag, TexStats, VisibleMask.
  - All-caps tokens: API, RNG, IST, GT, GREEN, FUTURE, FT, MB, ABI, DPI, SDR, USF, VR.
- ⚠ **Limit:** the scrub table covers the Concorde/Bates terms. Europa's own title is covered only by the eyeball list, and a title that is also an ordinary English word would pass it.

## 4. Drive-root listings — clean after FV6
- **The rewritten file:** `docs/europa/09434-live-capture-receipts.json`.
  - Pre-rewrite (`c3741a56…`, control): **all 189** non-generic drive-root names.
  - After FV6 (`91427e49…`, the only version left in the range): **4** of them, and each is an ordinary path word in an ordinary path. They are `Game` (`/Game/EuropaPhaseA/EA_Core`), `Cpp` (a `.cpp` file name), `Builds` (a `Receipts\…\builds\` path) and `Config` (a JSON key).
- **Across all 445 changed blobs in the range and every added line:**
  - the most distinct root names in any one commit's file is **7**;
  - 0 commit-files hold 10 or more;
  - 5 lines hold 3 or more, all prose path words.
- **Root names new relative to `4f46a9c` and origin:** only ordinary words: Builds, Captures, Config, Cpp, Game, Logs, Misc, Models, Python313 (`C:\Python313\python.exe` in the kit launchers), renders.
- **Drive paths in added lines:**
  - `D:/IA_Europa/…`, `E:/EpicGames/UE_4.25`, `C:/Program Files…`, `C:/Windows/System32/conhost.exe`, `C:/Python313/python.exe`;
  - one `C:/D:/E:` drive list in prose;
  - the engine's own `D:/Build/++UE4/Sync` build string.

## 5. Size and type — clean
- Blobs over 1 MB: 0 (largest 811,585 B).
- Binary blobs: 0, at the tip and in history (numstat binary entries 0).
- `.uasset` / `.umap`: 0.
- Paths under `Receipts`, `Saved`, `Intermediate` or `Binaries`: 0.
