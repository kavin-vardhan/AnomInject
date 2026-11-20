---
name: brief-instructions-carry-premises-verify-them
description: "A brief's instruction here often restates a fact from a journal; journals can be wrong in small ways, so verify the premise before executing — especially for move/overwrite/delete."
metadata: 
  node_type: memory
  type: feedback
  originSessionId: <uuid>
  modified: 2026-09-25T20:23:11.009Z
---

Briefs in this project are written from the session journals, so an instruction frequently carries an
embedded **factual claim** ("move X, it is a copy of Y", "the checkout is on master", "tick the two
placeholders"). **Those claims are inherited, not re-measured, and they can be wrong.**

**Measured instance (2026-09-06, brief 077R-02 ruling 3):** the ruling said *move `m50_pc7v3.py` to
`CaptureBench/tools/`*, on journal 077 §1.1's description of it as *"a copy of the CaptureBench
tool"*. In fact the plugin-root file was **0 bytes** and the real **4,672-byte tool was already
there and already tracked**. Executing literally would have **overwritten a working tool with an
empty file** and recorded the destruction as a tidy-up commit.

**Why this matters more than usual here:** this project's whole culture is measurement-over-argument
(`G253` — pricing a fix can refute it; `G120` — never scope-decide on an unverified mechanism). A
brief that turns out to rest on a refuted premise should be executed **by intent, with the deviation
stated**, not abandoned and not followed off a cliff.

**How to apply:**
- Before any move / overwrite / delete, **stat both source and destination** (size, tracked?,
  first lines). "Move A to B" is always also "overwrite whatever is at B".
- When the premise is false, do the thing the ruling *wanted* (here: keep the plugin repo root
  clean → delete the empty stray), and report it as a **named deviation with the measurement**.
- The same check applies to the checkout, the staged exe pair, and doc claims like "run_summary has
  N keys" — this session also found two stale gates (`frames_drawn_unexpected` "reads 0", including
  in client-facing text) that would have **failed a correct build on a real host**.

**Sub-shape added 2026-09-06 (brief 078-01), and it inverts the work item rather than merely
mis-stating it: a review OF A BRANCH describes defects, but some of them are defects the BRANCH
INTRODUCES, not defects the base has.** Three items handed down as "fix these stale/vestigial
things on master" were measured to be **branch-introduced regressions** — the counters are live on
master and dead only on the branch; the silent map reset exists only on the branch, and master
already solves the same problem correctly one file away. Attributing them to the base turns
"do not import this" into "go and change working code".

**How to apply:** when a finding arrives via a review of a branch/PR/fork, **check the claim against
BOTH refs before planning any change** (`git show <branch>:<file>` vs the working file). State which
ref each defect lives on. The same check catches the mirror-image trap: `git diff base..branch` on a
branch cut from an older base shows the base's newer work as **deletions**, so applying that diff or
merging as a unit silently reverts it.

**Sub-shape added 2026-09-26 (ruling 081-32): a ruling's required NEGATIVE proof can presuppose a check the rule set does not
make.** It demanded "colour differs → FAIL" on the legacy-identity gate, but no legacy rule reads colour frames (they are not even
byte-stable run to run), so the negative could only PASS. **How to apply:** before writing a negative proof, name the rule that reads the
mutated artifact; if none does, run it as written, report it not-as-expected, and stop for a ruling — never invent the missing check
(here it would have needed an uncalibrated tolerance). Repo record: G296.

Related: [[verify-before-reexecuting-a-brief]], [[brief-gate-beats-standing-shared-tree-rule]].
