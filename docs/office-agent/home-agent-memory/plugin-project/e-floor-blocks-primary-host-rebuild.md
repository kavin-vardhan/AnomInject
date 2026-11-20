---
name: e-floor-blocks-primary-host-rebuild
description: "Since 095-01b (2026-11-13) the primary checkout is on master (ebc34ed, release d7eee97 + docs), but its host was never rebuilt because E: (22.7 GiB) is below the 50 GiB floor; no bench leg may run until it is."
metadata:
  node_type: memory
  type: project
  originSessionId: <uuid>
  modified: 2026-11-13T06:52:10.627Z
---

On 2026-11-13, 095-01 stopped at pre-flight on the disk floor. 095-01b then did the git-only part: master fast-forwarded b5f15a3 -> d7eee97 and pushed, primary checkout moved m51 -> master (m51 ref unmoved at 53bf725), and docs commit ebc34ed pushed. The 15 former m51 untracked files remain in place, untracked, and are copied in code-receipts-095-01b.

The primary host's Intermediate/Binaries/Saved junction to E:\IA_BuildCache\StackOBot. Intermediate was EMPTY, so the first build is a full rebuild (~14.5 GB). E: had 22.7 GiB free against the standing 50 GiB floor (runbook 8.6 step 0b; chat 095-01 ruling D1 kept it). The 0944* Europa evidence/package dirs under E:\IA_BuildCache held 52.8 GiB; freeing them is the owner's call (D3).

**Why:** ruling D5 — the host's Binaries do not match master's source until that rebuild.

**How to apply:** before any primary-host build or bench leg, re-check E: free space and whether the owner freed E:; if E: is still under 50 GiB, stop at pre-flight. Units: A=m56, B=m57, C=m58; plan in _reviews\095-00-ruling.md. A pre-push scan template for master is code-receipts-095-01b\prepush-ebc34ed\prepush_scan095.py (edit BASE/TIP). Related: [[bench-bank-is-a-junction-to-e]].
