# 094-44r — pre-push scan of `feat/europa-port`, range `93613ae..8df75de` (Claude Code, 2026-11-04, about 07:40 IST)

**Scan clean. PUSHED `93613ae..8df75de` (fast-forward, 4 commits).** The local tip `2c99a6f` was NOT pushed: `8e59a56` (the texture port) adds the textures to the default routes before their gates passed, and the two commits after it are docs on top of it. Why the range stops there: `_reviews\094-44r-code-review.md` §8.

## Scope (read-only git, `GIT_OPTIONAL_LOCKS=0`)
- **Range:** `9c87836` (4.25 materials + finders), `ae82fe7` (FE3b), `6ee4a0e` (loader/target docs), `8df75de` (GF4 docs). 0 merges, 22 files, 1,292 added lines; 4 commits, 26 trees and 39 blobs not on any origin ref (7,263,039 B uncompressed; largest blob `docs/gotchas.md`, 825,073 B).
- **Source in the range:** only `AnomalyInjectorSubsystem.cpp` +7 (the 4.25 finders). Source, Content and descriptor at `8df75de` equal `ae82fe7`'s, which built exit 0 on both Paris targets and both fixture targets.
- **Push mechanics:** no hooks, no `push.followTags`, no `push.default`, no upstream, no tag at the tip.
- **Receipts:** `code-receipts-094-44r\prepush-8df75de\` (094-44's five scripts with the tip set to the full `8df75de` sha) and their outputs. ⚠ `code-receipts-094-44r\prepush\` was generated with a mistyped tip and never run (`NOT-USED.txt`).

## 1. Secrets — 0
- **Name = value hits:** 13, all code or paths: `authority=` strings in `runtime_creation.py:48`, `runtime_children.py:75`, `runtime_owned.py:96,139`; `key` loop variables (`create_anomaly_materials425.py:136`, `write_ledger.py:108`); `authored =` (`:183`); an `"author"` receipt path (`09444-material-receipts.json:17`); the old `gotchas.md:2469` placeholder.
- **Private keys, bearer/basic auth, URL user:password, cloud key shapes, the known control-server token:** 0.

## 2. Account and machine identifiers
- **E-mail in content:** 0. One author/committer identity on all 4 commits, the same as all 503 `origin/master` commits.
- **Profile name:** 8 new lines, all in `docs/europa/09444r-loader-target-receipts.json` (77, 78, 87, 88, 113, 114, 123, 124): UBT's `BuildConfiguration.xml` warnings quoting the owner's Windows profile path. This is the same carried class GF1 accepted ("already public on origin, accepted as before"). The other hits (`write_ledger.py:22`, `CLAUDE.md:8415`, `onboarding.md:31`, `gotchas.md:2524,4688`) are tip lines already on origin and not touched by this range.

## 3. Codenames — clean
- Scrub table selftest passed; 0 hits outside the printed `AnomalyCapture.Build.cs` exclusion.
- Eyeball list: `SkeletalCube` (the engine asset) and five UBT config names from the same warning lines; all-caps `API`, `WPO`, `MSC`.

## 4. Drive-root listings — clean
- 0 lines with 3+ root names; at most 5 distinct root names in one file.
- Drive paths in added lines: `E:/IA_Fixtures` (55), `D:/IA_Europa` (17), the profile `AppData` path (8, §2).

## 5. Size and type
- Blobs over 1 MB: 0. Paths under `Receipts`, `Saved`, `Intermediate` or `Binaries`: 0.
- **Binary blobs: 2, intended (D2/GG1):** `Content/Engine425/Materials/M_CorruptedTexture_Pink.uasset` (99,183 B) and `M_MissingTexture_Checker.uasset` (81,634 B). Their ASCII/UTF-16 strings: 0 real hits; the one flagged string, `:]\\"`, is binary noise (as in 094-44).

## 6. The push (`code-receipts-094-44r\push-receipt.txt`)
- Before: `ls-remote` = `93613ae`; `93613ae` is an ancestor of `8df75de`, which is an ancestor of local feat `2c99a6f`.
- `git push --porcelain origin 8df75de4c2258cb412b27c5d374d56207e40bb68:refs/heads/feat/europa-port`, no `--force`, no `--tags`: exit 0, `93613ae..8df75de`.
- After: `ls-remote` = `8df75de`; local `origin/feat/europa-port` = `8df75de`; 0 spike heads on the remote; only the branch ref was in the push output.
