# The home mailbox (how briefs reach the home agent)

- `README.md` — the home mailbox readme (`D:\IntrusiveAnomalies\_mailbox`): drop a brief in `to_code`,
  the watcher runs Claude Code headless on it, the final message lands in `to_chat`, the brief moves to
  `processed`.
- `watcher.ps1`, `run_brief.ps1`, `render.ps1`, `parse_check.ps1`, `start_watcher.bat` — the watcher scripts.
- `ran.txt` — the list of briefs the watcher has run.
- `mailbox-app-README.md` — the readme of a separate desktop mailbox app on E: (tray icon, local web
  dashboard). Its token and key files were not copied.

Not copied: the mailbox config, costs, watcher log, live transcripts, and the app's token/key files.
The briefs themselves are in `../briefs/`, the reports in `../reports-to-chat/`.

The office has no mailbox: the owner carries a brief in and photographs the report out.

Copies carry the pack's mechanical redactions (profile path, machine name, owner's name, e-mail,
session ids); see `../START-HERE.md` section 7.

**Code files in this pack carry an added `.txt` suffix** (for example `run_leg.ps1.txt`): they are a text snapshot, not runnable copies, and the repo's no-comments rule for source does not apply to them.
