# 094-44 — pre-push scan of `feat/europa-port` (Claude Code, 2026-11-04, about 05:35 IST)

**Scan clean. NOT PUSHED.** The range carries an unbuilt native change (the 4.25 material finders), so it is not a passing step and was held; see `_reviews\094-44-code-review.md` §6. This scan is recorded so a later push only needs the range re-checked from `9c87836`.

## Scope (read-only git, `GIT_OPTIONAL_LOCKS=0`)
- **Tip and origin:** tip `feat/europa-port` = `9c87836`, worktree clean; `origin/feat/europa-port` = `93613ae`, its parent, so a push would be a fast-forward.
- **The range `93613ae..9c87836`:** 1 commit, 0 merges, 13 files, 372 added lines; 1 commit, 12 trees and 13 blobs not on any origin ref (2,196,847 B uncompressed; largest blob `docs/gotchas.md`, 824,130 B).
- **Push mechanics:** no hooks, no `push.followTags`, no `push.default` or upstream set, no tag at the tip.
- **Receipts:** `code-receipts-094-44\prepush\`: `prepush_scan44.py`, `extra44.py`, `paths44.py`, `terms_prose44.py` (094-43r's scripts with only the base changed to `93613ae`), their outputs, and `assetstrings44.py` / `asset_strings.json`.

## 1. Secrets — 0
- **Name = value hits:** 2 in the range, both code: `create_anomaly_materials425.py:136` (`key`, a loop variable) and `:183` (`authored = json.load(...)`).
- **Private keys, bearer or basic auth, URL user:password, cloud key shapes, the known control-server token:** 0.

## 2. Account and machine identifiers
- **E-mail addresses in content:** 0. One author and committer identity, the same as on all 503 `origin/master` commits.
- **Profile-name hits:** 4, all tip lines already on origin and not touched by this range (`docs/gotchas.md:2524,4688`, `CLAUDE.md:8409`, `docs/onboarding.md:31`). 0 in added lines.

## 3. Codenames — clean
- **Scrub table:** selftest passed; 0 hits outside the printed `AnomalyCapture.Build.cs` exclusion.
- **Eyeball list:** no title-case or CamelCase candidates; one all-caps token, `API`.

## 4. Drive-root listings — clean
- 0 lines with 3 or more root names; at most 1 distinct root name in one file.
- Drive paths in added lines: `D:/IA_Europa` (6) and `E:/IA_Fixtures` (5).

## 5. Size and type
- Blobs over 1 MB: 0. Paths under `Receipts`, `Saved`, `Intermediate` or `Binaries`: 0.
- **Binary blobs: 2, both intended (D2):** `Content/Engine425/Materials/M_CorruptedTexture_Pink.uasset` (99,183 B) and `M_MissingTexture_Checker.uasset` (81,634 B).
  - **Their strings, ASCII and UTF-16, checked for drive paths, profile paths, project or fixture names, e-mail and secret words:** 0 real hits. The one flagged string, `:]\\"`, is binary noise.
