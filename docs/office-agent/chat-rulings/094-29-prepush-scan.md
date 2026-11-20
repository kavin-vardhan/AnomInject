# 094-29 — pre-push scan of `feat/europa-port` (Claude Code, 2026-10-31)

**PUSH-READY: yes.** No secrets, no binaries, no Unreal assets, no build/receipt folders, and no office title or name. There is one non-blocking personal identifier (§2): the owner decides whether it ships as is.

**Scope (read-only git, `GIT_OPTIONAL_LOCKS=0`).**
- `feat/europa-port` = `abb240b`. There is no `origin/feat/europa-port` in the local remote-tracking refs (no fetch done).
- Base = `d7eee97` (`origin/release/2026-10-05`). The only push boundary is `d7eee97`; `merge-base` with `origin/master` (`b5f15a3`) is `origin/master` itself.
- A push sends **92 commits** (`d7eee97..abb240b`, 0 merges): **1,220 objects, 670 blobs, 55.2 MB uncompressed, largest blob 790 KB**. It changes 255 files (+55,200 / −309).
- Scanned: every line added in all 92 commits (57,261 lines, intermediate states included), the full tip content of every changed file, and every commit message, author and committer.
- No custom hooks, no `push.followTags`, and no tag at the tip, so `git push origin feat/europa-port` sends the branch only.
- Receipts: `…\code-receipts-094-29\` (`prepush_scan.py`, `scan_summary.json`, `scan_findings.txt` (values masked), `terms_candidates_prose.txt`, `path_roots.txt`, `commits.txt`, `namestatus.txt`).
- `m51` P9: HEAD `53bf725`, index `526aba7f`, 0 tracked changes, 15 untracked. It was unchanged before and after.

## 1. Secrets — 0 hits
- **Name = value (TOKEN / SECRET / PASSWORD / KEY / AUTH / COOKIE / CREDENTIAL):** 116 pattern hits. Every one was triaged by masked shape, and none is a credential:
  - rule-citation strings in `authority` fields (prose, e.g. `runtime_policy.py:12`, 86 chars, 8 spaces);
  - synthetic canaries (`environment.py:217/223`, `fixture_self_tests.py:193`, `toolchain_environment.py:90`, value `synthetic…`);
  - code expressions (`key = str(…)`);
  - the documented public placeholder in `docs/gotchas.md:2469`, a pre-existing line.
- **Private-key blocks, bearer/basic auth strings, URL user:password, and cloud key shapes** (AWS, Google, GitHub, Slack, Stripe, OpenAI/Anthropic, Azure, GitLab, JWT, npm): **0**.
- **The real control-server token** from `StackOBot\Config\DefaultGame.ini` does not appear anywhere in the push. This was checked by value, without printing it.
- **Account/login fields** (LoginId, AccountId, MachineId, epicuser*, access/refresh tokens, client_secret): **0**.

## 2. Personal and machine identifiers
- **E-mail addresses:** 0 in pushed content. Author and committer are one identity on all 92 commits, identical to the identity on all 503 `origin/master` commits, so no new identity is introduced.
- **Machine name, GitHub account, Windows SIDs:** 0.
- **⚠ Non-blocking — Windows profile path.** The owner's Windows profile name appears in **19 hard-coded `C:/Users/<profile>/…` lines** in 9 new `tools/europa/` scripts. They are added by commits `ac81ecb`, `eb7b8e0`, `4c6c1d2`, `90377bf`, `1722348`, `0886769` and `7073697`. Tip lines:
  - `disk_pause.py:111`
  - `fixture_policy.py:213`
  - `ledger_budget_self_tests.py:34`
  - `ledger_delta_self_tests.py:20/21/24`
  - `resume_self_tests.py:62/67`
  - `runtime_monitor.py:124`
  - `tooling_self_tests.py:24–30`
  - `ubt_admission.py:30/33`
  - `write_ledger.py:22`

  The path form is new to origin (0 such lines on `origin/master`). The same first name is already public on origin, in `CLAUDE.md:8361`, in `docs/onboarding.md:11` and in every commit's author field. It is not a secret. **Owner's call:** push as is, or replace it with an environment lookup in a later commit.
- Two `/c/Users/.../` lines in `docs/gotchas.md` (2524, 4688) are generic and already on origin.

## 3. Codename discipline — clean
- **Scrub term table:** the project's verifier term table (`CaptureBench/tools/m36_scrub_hostname.py`) was used in memory, after a passing in-memory selftest, with nothing written to disk.
  - Its only hits are `Source/AnomalyCapture/AnomalyCapture.Build.cs:112` and `:261`. That file is the permanent printed exclusion (the build-time fork-detection needles).
  - Neither line is added by this branch: there were 0 hits across all 57,261 added lines. On 4.25 that probe does not run (`AnomalyCapture.Build.cs:309`, 5.1 only).
- **Commit messages:** 0 names.
- **Owner eyeball list:** capitalised words in added prose (docs, md/txt/json, commit messages) that are not used in lower case anywhere in the tree and are not code identifiers. All are engine, tool or plain terms; none is a title, studio or fork name:
  - Blender, Reparent, ControlServer, EditorRuns, ShouldCreate, TimeOnly, AreEditorAnalyticsEnabled, CanContainContent, HighResShot, LinuxTargetPlatform, OnWorldSaved, ProjectDescriptor, VersionTooOld, Adapt, Alternatively, AnomalyUnsupportedTexture, BuiltData, CaptureSubsystem, ControlRig, CookAll, CurrentLegacyFileVersion, CustomVersionMissing, DoesSupportWorldType, EngineAnalytics, EnhancedInput, ExtraModuleNames, FileReference, GameplayInsights, GetLodCount, InitializeSubsystems, JsonExports, LoadMap, LocalAppData, MapBuildData, Metasound, MidReproActor, NewBlankMap, NewLevel, NoAnalytics, NoHold, NoSessionFound, NoTempCrash, OnScreenshotCaptured, PostSaveWorld, ProbeDriver, ReGate, ReadAssetRegistryData, RequestEndPlayMap, RequestPlaySession, SaveCurrentLevel, SaveDirtyPackages, SetMasterPoseComponent, ShortName, Thumbnails, UnloadModulesAtShutdown, UploadType, Vulkan, WorldProbe, YesNoYesAllNoAllCancel.
  - All-caps: IST, API, GREEN, DLL, FH, MB, ETW, PY, USN, AST, SIE, BAT, CL, EA, EJ, LOAD, QPC, RULES, UFS, UNION.
- **Drive paths in added lines:** only `D:/IA_Europa`, `D:/IntrusiveAnomalies`, `D:/UESource`, `E:/EpicGames/UE_4.25|UE_5.7`, `E:/IA_BuildCache`, `E:/Unreal Projects/StylizedParisStreet`, Windows/system folders, and `Synthetic*` test paths.
- ⚠ **Limit:** a title that is also an ordinary English word would pass the lower-case filter. The scrub table covers the Concorde/Bates terms regardless.

## 4. Size and type — clean
- Over 1 MB: **0** (largest 790 KB, `CLAUDE.md`).
- Binary blobs: **0**, at the tip and in history (numstat `-\t-` count 0).
- `.uasset`/`.umap`: **0**.
- Paths under `Receipts`, `Saved`, `Intermediate` or `Binaries`: **0**.
- The 4.25 fixture packages stay in the Paris host and are not in the repo.

## 5. Plugin C++ since the last 4.25 build — EMPTY
- The last 4.25 build was job 094-21: Editor and Game Development at `40a8eaf` (`docs/sessions/2026-10-28-094-21-probe-exit-purge.md:15-16`).
- `git diff 40a8eaf..feat/europa-port -- Source AnomalyInjector.uplugin` is **empty**.
- The `Source` tree hash is `5b4cbbf1` at `489c828` (A4a clean build), `550e9b7` (A5), `40a8eaf` and the tip. The last `Source` commit is `d9c91c1` (10-24).
- ⇒ **The office compile is the first compile on Europa's fork, not the first compile of new changes.** On stock 4.25 this exact `Source` built with 0 C++ errors and 0 warnings (`docs/europa/09408-a4a-result.md`).
