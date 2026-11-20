094-26d receipts (Claude Code, mailbox brief 094-26d), 2026-10-30.

Start (05:52-05:57 IST): step0\ (headroom, memory, toolchain, watcher, roots-*-start.tsv), m51-gate-start.json, protect\protect-start.txt,
state-start.txt, kit-hashes-start.json, review\kit-config-check-start.json, feature-gate-start.json, paris-lists-before.json, p9v2-file-start.txt.
Launch: preserve-receipt.txt + preserved\ (094-26c's report outputs, 37 files, hash-equal), code-added-094-26d.txt, implementer-block.txt,
resume-prompt-receipt.txt (prompt in the run folder), patch-launch-receipt.txt, prelaunch-headroom.txt, launch-start.txt (05:57:19),
launch-receipt.txt, canary.txt (name and hashes only), m51-gate-prelaunch.json, feature-gate-prelaunch.json, codex-pids.txt.
During: midjob-notes.txt (6 polls). No ue-sightings.txt or root-change-sightings.txt was written (nothing to record).
After (06:49 IST and later): m51-gate-after.json, m51-gate-final.json, protect\protect-after.txt, state-after.txt, step0\roots-*-after.tsv,
kit-hashes-after.json, review\kit-config-check-after.json, paris-lists-after.json, p9v2-file-after.txt, collect-receipt.txt, ledger-row.txt.

Review outputs (review\): paris-diff.json, diff-detail.json, session-raw-EA_Core.json, session-raw-EA_Travel.json, details.json, gates-check.json,
console-check.json, launches.json, commits.json, ledger-check-26d.json, canary-scan-code.json, canary-control.json (+ canary-control-note.txt),
audit-spot.json, first-cmds.json, live-compare.json, package-names.json, ff2-canfail.json.

Notes on my own files:
- copy_tools26d.py copied 094-26c's helpers with job names replaced (copy-tools-receipt.txt); copy_review26d.py copied 094-26c's review
  scripts into review\from26c\ (copy-review-receipt.txt); adapt26d.py made the recipe-pair versions in review\ (adapt-receipt.txt).
  paris_diff26d.py, diff_detail26d.py, ledger26d.py, ff2_canfail26d.py, canary_control26d.py and first_cmds26d.py in review\ are mine for 26d.
- console_check26d.py reports 4 lines not matched by its fragment matcher; each was traced by hand to the catalog: fixture_wait.py:66 (via :45),
  fixture_leg.py:142 (twice), fixture_entry.py:86-87.
- canary_control26d.py crashed on its own summary line (wrong key name); the scanner had already written canary-control.json (see the note).
  The real scan ran after the control folder existed, so its 4 candidates are the control's 4 strings.
- audit_spot26d.py (review\from26c\) counts the 2 direct Get-Content reads as "wrapped" because their text names filtered09426c.ps1;
  first-cmds.json shows them: 79 filtered + 2 direct read-only.
- My launch shell had GIT_OPTIONAL_LOCKS=0 set; it reached the launch environment (launch-receipt.txt). The filtered wrapper sets the same value.
- code-added-094-26d.txt estimated the launch "a little after 06:00 IST"; the real launch was 05:57:20.
Scratch: D:\IA_Europa\Temp\CodeReview09426d\ (ff2\ hosts 00-16, canary-control\); plain folders, 0 reparse points. Nothing deleted.
