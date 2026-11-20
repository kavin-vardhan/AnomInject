---
name: brief-gate-beats-standing-shared-tree-rule
description: "When a brief's opening repo-state gate is stricter than CLAUDE.md's shared-tree rule, the brief wins — stop, don't switch the checkout."
metadata: 
  node_type: memory
  type: feedback
  originSessionId: <uuid>
  modified: 2026-09-03T11:59:57.493Z
---

Multiple chat threads share one checkout and one physical bench in this repo. A side-session
brief often opens with a repo-state gate ("if not on master, or HEAD != origin/master, STOP").
CLAUDE.md's standing shared-tree rule is *looser* — it PERMITS switching the main checkout to
another branch when there are no tracked modifications and the branch being left == its origin.

**When the two disagree, the brief's gate wins. Stop and report; do not switch.**

**Why:** the standing rule protects against *losing commits*. The brief's gate additionally
protects a *shared physical resource* — the staged bench exe and cooked container. Another
thread may be mid-flight there, and that is invisible from `git status`. Satisfying the looser
rule (work is pushed, nothing at risk) does not clear the stricter concern, so "CLAUDE.md says
I may" is not a licence to proceed.

**How to apply:** report the gate facts (branch, HEAD vs origin/master, whether the parked
branch is pushed), then deliver every sub-task that does *not* depend on the checkout —
source reads, engine-source verification, static analysis — and name exactly what stayed
blocked. Reading files is not "touching another thread's work"; building, staging, or running
a leg is. See [[verify-before-reexecuting-a-brief]].
