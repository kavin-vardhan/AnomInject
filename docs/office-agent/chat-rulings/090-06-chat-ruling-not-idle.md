# 090-06 — Chat ruling on the smoke (NEEDS-DECISION: not idle, nothing ran): re-run as 090-06-r2 by day after the Codex re-check; poll-then-launch is the method; window 1's evidence is moved aside, not deleted; no harness change

Date: 2026-10-01 ~06:40 IST · Author: chat Claude · Report `_mailbox\to_chat\090-06-smoke-run.report.md` (688 min)

## Accepted
- **Nothing ran.** The owner told chat at ~23:35 that the home PC would render in UE 5.7 all night. Staging stayed at the M53 pairing and was hash-verified; postflight PASS; no bank, no git writes.
- **Code handled the unexpected harness behaviour correctly.**
  - In live mode a non-idle preflight exits 10 at once; the 90-minute wait applies only per leg and at staging.
  - Code polled the harness's own `idle_gate()` read-only instead of hammering `-Live`, and backed out cleanly at 22:11 when an editor reopened 30 s after two idle reads.
- **Pre-run checks all pass** (freeze, selftest 83/83, NEW/OLD pins, A44 string scan, container, maps). The harness is ready as is.

## Rulings
1. **Reschedule as 090-06-r2,** released after 090-08 (the Codex re-check, which needs no idle PC).
   - **By-day window:** until **17:00 IST Thursday**, so the slot is free for any fix the owner's office test calls for.
   - If it hasn't run by 17:00, it stands down and chat replans for Thursday night.
2. **Method: poll first, then launch. No harness change** (the harness stays frozen).
   - Poll `idle_gate()` read-only every 60 s, and invoke `-Live` only after **3 consecutive idle reads**. This becomes the declared method in r2.
   - The README §6 "waits up to 90 min" wording is noted as describing the per-leg and staging waits only.
3. **Evidence:** move `090-06-evidence\run\` (window 1) to `090-06-evidence\run-w1-notidle\` with `mv`, never deleting, before r2's first launch. r2 then starts clean, and window 1's `-FAILED` restore receipt (which restored nothing) can't confuse a resume.
4. **Disk** dropped overnight from activity that wasn't ours (D: 137 → 98 GB, E: 106 → 76 GB), so both are above the 50 GB floor. Not a blocker; reported to the owner for awareness.

## Next
1. **090-08, the Codex re-check:** released now.
2. **090-06-r2:** held. Released after 090-08 with its ACK line.
3. **Thursday:** the owner's office test on `2015cfa`.
