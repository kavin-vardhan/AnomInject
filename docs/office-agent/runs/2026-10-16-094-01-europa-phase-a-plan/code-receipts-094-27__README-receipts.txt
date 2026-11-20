094-27 receipts (Claude Code, mailbox brief 094-27), 2026-10-30.

Start (07:02-07:07 IST): step0\ (headroom, memory, toolchain, watcher, roots-*-start.tsv), m51-gate-start.json, protect\protect-start.txt,
state-start.txt, kit-hashes-start.json, feature-gate-start.json, paris-lists-before.json, p9v2-file-start.txt.
Launch: preserve-receipt.txt + preserved\ (094-26d's report outputs, 35 files, hash-equal), code-added-094-27.txt, implementer-block.txt,
resume-prompt-receipt.txt (prompt in the run folder), patch-launch-receipt.txt, the *-dryrun files (wrapper dry run, no launch),
prelaunch-headroom.txt, launch-start.txt (07:07:09), launch-receipt.txt, canary.txt (name and hashes only), m51-gate-prelaunch.json,
feature-gate-prelaunch.json, codex-pids.txt.
During: midjob-notes.txt (10 polls). No ue-sightings.txt or root-change-sightings.txt was written (nothing to record; the one
editor session lasted 15 s, between polls).
After (08:39 IST and later): m51-gate-after.json, m51-gate-final.json, protect\protect-after.txt, state-after.txt, step0\roots-*-after.tsv,
kit-hashes-after.json, paris-lists-after.json, p9v2-file-after.txt, collect-receipt.txt, ledger-row.txt.

Review outputs (review\): paris-diff.json (paris_diff27b.py), cites.txt (cites27.py: 16 cited 4.25 ranges), receipts.txt (receipts27.py),
live-compare.json (from26d\live_compare27.py, pointed at 09427's live-gate.json), canary-scan-code.json and canary-control.json
(from26d\canary27.py, canary_control27.py), audit-spot.json (audit27.py), proofs.txt (proofs27.py), ledger-runtime-run.json (ledger27.py).
copy-review-receipt.txt lists the scripts copied from 094-26d's review into review\from26d\ with path renames.

Owned scratch (plain folders, 0 reparse points):
- D:\IA_Europa\Temp\CodeReview09427\py27\ : run_harness27.py (runs Codex's runtime_python27_tests.py under the engine's 2.7.14 with two
  edits: output root -> my scratch, and the tools path), patch-receipt.json, out\ (summary.json PASS).
- D:\IA_Europa\Temp\CodeReview09427\scripts\ : canfail27.py (56/56 PASS), canfail27-console.txt, standin27.py (runtime_standin.py with
  its owned-root check and sys.path changed).
- D:\IA_Europa\Receipts\CodeReview09427\canfail\ and D:\IA_Europa\Temp\CodeReview09427\canfail\ : the can-fail job config, monitor and
  execute roots, a copy of the real Smoke slot (smoke-copy\), summary.json.
- D:\IA_Europa\Temp\CodeReview09427\canary-control\ : 4 planted control files (synthetic values, not the canary).

Notes on my own files:
- canary_control27.py crashed on its own summary key (inherited from 26d) after the scanner wrote canary-control.json; read directly: 3/3.
- The real canary scan ran before the control folder existed (0 candidates).
- copy_tools27.py widened two regexes (old kit console names; kit process names) - recorded in copy-tools-receipt.txt.
Nothing deleted.
