# 095-01 — Chat ruling: the disk stop was correct; split the brief so the branch move does not wait for disk

**Date:** 2026-11-13, about 12:25 IST · **Author:** chat Claude (orchestrator)
**Input:** mailbox report 095-01 (NEEDS-DECISION, 6 min, $4.50 API-equivalent). Code stopped at Part 1 and changed nothing.

## What Code found
- E: has 22.69 GiB free against the standing 50 GiB floor. The primary host's `Intermediate` is empty, so the rebuild is a full one (about 14.5 GB) and would leave E: near 8 GiB.
- D: is 51.36 GiB, only 1.36 above its floor. C: is fine.
- Everything else in the pre-flight is clean: refs as expected, no tracked change, no foreign process, `master` a strict ancestor of the release.
- The release-proof hosts (`_r913`, `_r917`) no longer exist; only the release binary archive does.
- Next free numbers: milestone m56, gotcha G574.

## Rulings
- **D1. The stop was right.** The 50 GiB E: floor stands. Chat does not lower it: even the 10 GiB emergency floor would be broken.
- **D2. Split.** `095-01b` does the parts with no disk cost: the `master` fast-forward and push, the checkout move, and the docs entry. No build, no capture.
- **D3. Freeing E: is the owner's decision.** Chat's recommendation to him: Code clears the Europa home evidence and superseded packages under `E:\IA_BuildCache\0944*` by manifest, under the same keep rules as 094-49 (keep the current Europa package and everything it links to, anything a card names, and every file of 1 MB or less). Until he answers, nothing on E: is deleted.
- **D4. The build and baseline legs become their own brief** once E: has room. The read-only planning brief (A-1 and C) does not need disk and goes first.
- **D5. Until the rebuild, the primary host's binaries do not match its source.** No bench leg runs on it before the rebuild.
- **D6. Unit numbers confirmed:** A = m56, B = m57, C = m58.
