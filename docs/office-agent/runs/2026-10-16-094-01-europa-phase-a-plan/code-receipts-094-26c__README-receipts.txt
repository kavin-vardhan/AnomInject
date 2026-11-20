094-26c receipts (Claude Code, mailbox brief 094-26c), 2026-10-30.

Notes on my own files:
- review\kit-config-check-x.json (04:36 IST) is a duplicate of kit-config-check-start.json, written by a second, mistaken run of
  kit_config26c.py with the tag "x" in the same command. Same content class; it is not used and changes nothing outside this folder.
- code-added-094-26c.txt says the launch is "about 04:50 IST" and the feature check "04:40 IST". The real times are: feature gate
  start 04:38, prelaunch hold READY 04:40:16, launch 04:40:25 (launch-start.txt). The prompt was built before the hold finished and
  build_prompt26c.py refuses to overwrite, so the prompt keeps the estimate. No decision depends on it.
- review\from26b\ holds copies of 094-26b's review scripts with the job names replaced (copy-review-receipt.txt). Each is read
  before use.

Used in the review (outputs in review\):
- from26b\console_check26c.py -> console-check.json (4 composed lines traced by hand: fixture_wait.py:45, fixture_leg.py:143,
  fixture_leg.py:189 -> :248 / fixture_entry.py:90)
- from26b\audit_spot26c.py -> audit-spot.json; first_cmds26c.py (mine, 26c offset) for the first command
- from26b\ledger26c.py -> ledger-check-26c.json (reads the job ledger summary and the Smoke runtime classified changes)
- from26b\canary26c.py -> canary-scan-code.json
- from26b\live_compare26c.py -> live-compare.json
- paris_diff26c.py, session_raw26c.py (session-raw-Smoke.json, session-raw-EA_Core.json), details26c.py, gates26c.py,
  fc2_setup26c.py (CodeReview09426c\fc2), package_names26c.py, launches26c.py, fe2_canfail26c.py, commits26c.py: mine, written for 26c.
Not used: from26b\paris_diff26c.py, smoke_raw26c.py, gates_check26c.py (replaced by my 26c versions above), fd2_canfail26c.py,
  fd3_launches26c.py (FD2/FD3 files unchanged since 09e776b, so 094-26b's can-fails stand), session_check26c.py, fc5_classes26c.py,
  cwriter26c.py, dwriter26c.py, proofs_ledger26c.py, first_cmds26c.py in from26b (hard-coded 094-26b offset).
