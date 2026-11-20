---
name: push-workflow-inverted
description: "STANDING RULE (2026-07-21) — the owner is remote; push promptly after your own gates pass, do NOT wait for a green light"
metadata: 
  node_type: memory
  type: feedback
  originSessionId: <uuid>
  modified: 2026-07-22T06:09:42.131Z
---

**Effective 2026-07-21 until the owner says otherwise.** the owner works from a REMOTE machine, not the dev box — the push IS the transport that delivers work to him, so he cannot verify before you push. The order INVERTED:

- OLD: commit → the owner smoke-tests → green-light → you push.
- **NEW: commit → YOUR gates green → PUSH PROMPTLY (no waiting for a green light) → the owner pulls remotely and verifies there.** Still report gate results as usual.

Rules that ride with it:
1. Push as soon as your own gates pass. Don't wait.
2. master may temporarily carry commits the owner hasn't personally verified — accepted.
3. Verification failure → **FIX FORWARD with a new commit. NEVER force-push, never rewrite history, never revert-by-history-edit.** (Already the rule; now load-bearing.)
4. **G43 per-push still applies to EVERY push:** `git status` + `git log origin/master..master` first; confirm exactly the intended commits go up; nothing from `feature/stencil-capture` (parked at `76cac74`); no working-tree cruft. Remote rejects → STOP and flag, never force.
5. Tags unchanged: M1/M2/M3 sit OUTSIDE the m-sequence — no tags.

**Every milestone report must now include a "VERIFY FROM A FRESH PULL" section** (the owner checks from another machine, possibly a phone): the exact SHAs that should land per repo; what is NOT in git and can't be pulled (built exe, `src-tauri/target/`, `dist/`, gitignored dev `public/config.json`); minimum prereqs + exact commands to make a runnable delivery from a clean checkout; and a REVIEW-ONLY path (what's verifiable from the diff alone + which gates stay owed until he's at a full dev box). Copy-paste runnable, phone-readable.

**Why:** remote owner; the repo is the delivery channel now. This SUPERSEDES the old "nothing pushes until the owner's smoke" rule in [[dashboard-m1-m3-work-order]]. Reports still go in a copy block per [[reports-in-copy-block]].
