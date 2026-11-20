# _mailbox — chat Claude <-> Claude Code bridge (no copy-paste)

**Start:** double-click `start_watcher.bat` once (per boot). Minimize the window.
**Stop:** close that window, or create an empty file named `STOP` in this folder.

How it works
- Chat Claude writes a brief to `to_code\NNN-name.md`.
- The watcher runs Claude Code headless (`claude -p`) on it, resuming one long-lived session
  (a brief starting with `# SESSION: new` starts a fresh session — used at milestone boundaries).
- Code's final message is written to `to_chat\NNN-name.report.md`; the brief moves to `processed\`.
- Chat Claude reads the report, writes the verdict + next brief. The owner is pinged only when a human is needed.
- `watcher_status.txt` = alive/idle/running heartbeat. `watcher.log` = history. `config.json` = flags.
- Headless Code runs with permissions pre-approved (builds, legs, commits, pushes to origin — a remote EXISTS since m4x; tags are NOT
  Code's to create, they batch at the office). The review gate is the orchestrator reading every report before the next brief goes out.
- **Verdict gate (2026-09-06):** every report header carries `verdict: GREEN | NEEDS-DECISION | RED` — declared by Code as a
  `VERDICT: ...` line in its final message, else derived FAIL-CLOSED (rc≠0 or a killed session → RED; a non-empty NEEDS-DECISION
  section → NEEDS-DECISION; no VERDICT line at all → UNDECLARED). The watcher writes `last_verdict.txt`, and after NEEDS-DECISION/RED/UNDECLARED it HOLDS the next brief (filed as `processed\HELD_<name>`, never run,
  not in ran.txt) unless that brief carries a header line `# ACK-VERDICT: <how the decision was resolved>`. Exit code 0 is not acceptance.
- **Orchestrator:** from 2026-09-15 the GDP mailbox is orchestrated by **chat Claude**; the Codex pilot (078-01..078-17) is OVER.
  Codex is now used as an **independent reviewer at crucial checkpoints only**, named by chat. One orchestrator per mailbox, ever —
  the contended resource is the bench checkout. *(2026-09-06 → 2026-09-15 the orchestrator was a Codex session, with chat Claude
  as the adversarial reviewer; that arrangement is superseded, not deleted.)*

**Dashboard:** `E:\MailBox\start_dashboard.bat` shows this mailbox at a glance at http://localhost:7337/ and pings you with a Windows toast when a brief finishes, needs you, or the watcher dies. Canonical watcher scripts live in `E:\MailBox\watcher` (the dashboard flags drift). Since 2026-09-03 the watcher recovers a brief that was interrupted by a crash: it lands in `processed\` as CRASHED and a stub INTERRUPTED report goes to `to_chat\`.
