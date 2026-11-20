---
name: verify-before-reexecuting-a-brief
description: "Briefs in this repo can be re-delivered verbatim; verify completion from CLAUDE.md + git + the staged exe hash before acting, because re-running a milestone is destructive here."
metadata: 
  node_type: memory
  type: feedback
  originSessionId: <uuid>
  modified: 2026-09-02T13:30:44.385Z
---

Briefs arrive as files through `D:\IntrusiveAnomalies\_mailbox` and **can be delivered more than
once, byte-identical**. Before executing one, check whether it is already done:

1. `CLAUDE.md`'s status block — milestones carry an explicit ✅ BUILT/GATED/SHIPPED marker.
2. `git log --oneline -5` and `git log --oneline origin/master..master` (empty = pushed).
3. The staged bench exe's `sha8` against the hash the status block names.
4. The session journal under `docs/sessions/` for the matching section.

**Why:** re-running a completed milestone here is **destructive, not merely wasteful**. A rebuild
mints a new exe hash, the stage step overwrites the staged binary, and gate legs are graded against
a *pose-matched pair* — so re-running silently destroys the `P-C7` A-side pairing that the previous
gate result rests on, while looking like ordinary work.

**How to apply:** verify first; if the work is already landed, do **not** re-execute — report the
verification and flag the repeat delivery as `NEEDS-DECISION`. ⛔ Never write into `_mailbox`.
Related: [[headless-final-message-is-the-report]].
