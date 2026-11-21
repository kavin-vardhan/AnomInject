# Disk cleanup after M3 shipped (2026-10-10)

Done at the owner's request (the owner: delete the safe list and the Bates VT work too) by a Claude Code Desktop session. M3 shipped as `release/2026-10-05` @ `d7eee97` (exe `218BA4C7`). The owner says Bates M3 will not ship.

## Deleted permanently (not to the Recycle Bin)

| Path | Was |
|---|---|
| `D:\IA_BankOverflow\_r912` | job 090-12 (m52 proxy blur) workspace |
| `D:\IA_BankOverflow\_r910f2` | job 090-10f2 workspace |
| `D:\IA_BankOverflow\090-10b2-evidence` | job 090-10b2 evidence |
| `D:\IA_BankOverflow\M52S09_*`, `M52S10_*`, `M53DC_*`, `M53DC2_*` (340 folders) | M52/M53 bench sessions from 09-29 and 09-30 |
| `D:\IntrusiveAnomalies\_reviews\089-03-evidence`, `089-04-evidence`, `090-06-evidence` | evidence hard-linked to those sessions |
| `E:\IA_BuildCache\_r912b` | job 091-02 (include compatibility) |
| `E:\IA_BuildCache\_r913` | job 091-03 (annotation transitions); its result is in the shipped code |
| `E:\IA_BuildCache\_r915` | job 091-05 (review fixes, not shipped; code on `fix/m52-label-timing` `1eaf205`) |
| `E:\IA_BuildCache\_r916` | job 091-06 (stopped); its uncommitted work is saved here |
| `E:\IA_BuildCache\_dash` | Dashboard v2 test labs |
| `E:\IA_BuildCache\_r921` | **Bates** VT jobs 092-03, 092-03b, 092-03c and 092-05 (code on origin `feat/vt-proxy-blur` `af79520`, `feat/vt-blur-stacks` `5dc8717`) |

Their rulings, reports, run folders (`_relay\runs\...`) and all `_binary_baselines` are KEPT. Any path in older notes that points into the folders above no longer exists; read the matching `_reviews\*-ruling.md` and report instead.

## Result

352 folders deleted, 0 errors, 228 s. Free space went from 65.8 to 207.3 GiB on D: and from 49.1 to 282.4 GiB on E:. `git worktree prune` then removed the stale registrations AnomalyInjector3/5/6/8. The local branches `fix/m52-label-timing`, `feat/vt-proxy-blur`, `feat/vt-blur-stacks` and `release/2026-10-05` are untouched.

Kept: both `_binary_baselines` (including the shipped `release-2026-10-05-23098fa-218BA4C7`), `E:\IA_BuildCache\StackOBot` (the live project's junction targets), `_r917` (shipped release worktree), `_r53_host`, `_r52p_src`, `_r53_src`, `_r53_selftest`, the Lyra bench, and `D:\UESource`.

## Saved before deleting

- `r916-091-06-uncommitted-wip.patch` plus `r916-untracked\`: the 091-06 office-robust capture-ownership work (F1), never committed, base `1eaf2056`. To restore it, check out `1eaf205`, run `git apply r916-091-06-uncommitted-wip.patch`, then copy in `r916-untracked\Source\...`.
- `r913-staged.patch`: changes staged (index only) in the 091-03 worktree, base `1eaf2056`. It mostly reverts 091-05's fixes toward the shipped code. Kept only as a precaution.

Both patches were checked with `git apply --check -R` against their worktrees before deletion.
