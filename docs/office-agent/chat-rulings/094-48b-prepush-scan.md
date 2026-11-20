# 094-48b plugin pre-push scan — PASS, PUSH HELD

**Range scanned:** `bd767ca8ac7cd1ca18abe34d9cc66764fee100a6..1667d0576f817bb7ad6e1d4568034cd331321559` on `feat/europa-port` (Paris worktree). 2 commits, no merges, 0 binary numstat entries, 18 changed files, 159 added history lines. Scan run by Claude Code on 2026-11-06, about 16:45 IST.

The two commits:
- `6b171b50f0c38638333ccf0e99495a62c2039560` fix(europa): start dashboard captures in free play
- `1667d0576f817bb7ad6e1d4568034cd331321559` fix(europa): write dashboard captures as flat library sessions

**Scripts:** 094-47's five scan scripts, regenerated for this range by `code-receipts-094-48b\make_prepush48b.py` into `code-receipts-094-48b\prepush-1667d05\` (outputs and stdout files are beside the scripts).

**Results:**
- Scrub selftest OK. No codename, scrub-term, email, SID, private-key, auth-string, cloud-key or known-control-token finding.
- Author and committer identity match all 503 `origin/master` commits.
- No uasset/umap, no blob over 1 MB, no binary blob, no tag at the tip, no hooks, no upstream config.
- Flagged lines are all **TIP-tree lines that exist byte-for-byte in published `bd767ca`**, checked line by line with `prepush_tipcheck48b.py`. They are `P2_user_profile_path` ×2, `P4_identity_windows_username` ×2 (the carried GF1 class), `S1_name_assign` ×1 (a documentation placeholder) and `X_long_hex` ×6.
- The only flag on an added line is one long hex in the new session journal: the game EXE's SHA-256.
- Path roots in the added lines are the usual local job folders: `D:/IA_Europa` ×10, `E:/IA_BuildCache` ×1. Prose terms: "Rust" only.
- No `Content/` change in the range. The pak is unchanged (SHA-256 `d926673c…`).

**Publication: HELD.** The scan is clean, but this brief's GREEN requires S1–S4, and S4 fails strict texture IoU (the same failure as F1/F3/F5). Under 094-48, a failing F1–F3 blocks the push, and under the 094-47 card a new correctness-gate failure is a mandatory chat checkpoint. Local `feat/europa-port` = `1667d05`; origin and `ls-remote` = `bd767ca`. Nothing was pushed, tagged or force-pushed, and the dashboard repo is unchanged at `b8ffaca`.

**When chat authorizes publication:** push `1667d0576f817bb7ad6e1d4568034cd331321559:refs/heads/feat/europa-port` as a fast-forward from `bd767ca`, then confirm with `ls-remote`. A new commit on top needs a new scan of `bd767ca..<new tip>`.
