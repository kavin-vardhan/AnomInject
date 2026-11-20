---
name: powershell-git-commit-message-traps
description: "In PS 5.1, a git -m message containing double quotes splits into pathspecs (the commit fails, the staged files ride the next commit); Set-Content -Encoding utf8 message files carry a BOM into the commit."
metadata:
  node_type: memory
  type: feedback
  originSessionId: <uuid>
  modified: 2026-11-14T02:12:53.965Z
---

095-06b hit both on 2026-11-14. (1) `git commit -m $msg` where `$msg` contains `"..."`: PowerShell 5.1 re-quotes native
arguments badly, git reads the tail as pathspecs and fails; the files stay staged and the NEXT `git add x; git commit`
sweeps them in under the wrong subject. (2) `Set-Content -Encoding utf8` writes a BOM; `git commit -F file` then stores
EF BB BF at the start of the message (G141's shape).

**Why:** both produce a wrong commit silently-ish; fixing needs an amend.

**How to apply:** keep `-m` messages free of double quotes, or write the message with the Write tool (no BOM; but if
the file already had a BOM, Write keeps it - strip with python) and use `git commit -F`. After any failed commit,
check `git status` before the next commit. Verify a message's first bytes with `git cat-file -p HEAD`.
