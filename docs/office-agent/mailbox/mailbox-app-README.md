# Mailbox — the chat ↔ Code bridge, at a glance

One small local service that watches every mailbox folder (GDP, Heist Crew, …), shows their progress on one page,
and pings you with a Windows toast when a brief finishes, when a report has something for you, or when a watcher dies.

```
E:\MailBox\
  Mailbox.vbs              THE APP: tray icon + service (hidden) + the dashboard in its own window. create_shortcut.bat puts
                           a "Mailbox" shortcut on the Desktop / Start Menu; install_autostart.bat starts it quietly at login
  start_dashboard.bat      the plain version: service in a console window, page at http://localhost:7337/ in any browser
  mailboxes.json           the registry: which _mailbox folders to watch
  settings.json            port, poll rate, toast switches, editor
  state.json               runtime bookkeeping (what you've marked seen, what has been toasted) — safe to delete
  server.js + lib\         the service (Node, no dependencies; Node is already on this PC because Claude Code needs it)
  ui\dashboard.html        the page
  watcher\                 the CANONICAL watcher scripts (watcher.ps1, run_brief.ps1, render.ps1) + templates
  new_mailbox.ps1          scaffold + register a new mailbox in one go
```

## The tray app

`Mailbox.vbs` (or the shortcut) starts the service with no console window and opens the dashboard as its own app-mode window
(Edge or Chrome `--app`, so it gets its own taskbar entry and can be pinned). The tray icon shows the overall state — grey idle,
orange while Code is working, rust when a watcher is down — and its tooltip summarises each mailbox. Right-click for the menu:
open, per-mailbox Start/Stop watcher, test toast, restart the service, quit. Quitting the tray app stops the service it started
but never touches the watchers. Left-click reopens the window.

## The page

Each mailbox is a column. Top to bottom:

- **Watcher line** — running / idle / gone / halted / stopped, with the pid and a reason when something is off. **Start** launches
  `start_watcher.bat` in a minimized window; **Stop** drops a `STOP` file (the watcher exits after the current brief — nothing is killed).
- **Stage band** — the one thing to read at a glance: *Code is working* (brief, elapsed minutes, tool calls, tool errors, last activity),
  *Waiting on chat* (report ready, minutes waiting), *Needs you* (open items), *Queued*, *Idle*, or *Watcher down*.
- **Live feed** (while running) — the tail of `to_chat\*.live.txt`, plus Code's most recent narration line.
- **Queue** — briefs sitting in `to_code\` with their header directives.
- **For you** — NEEDS-<owner>, NEEDS-DECISION and leading-🔴 items pulled out of recent reports. Click one to read the report;
  *Mark all seen* clears them. Chat still gets everything — this is just so you can see it without opening files.
- **Reports** — every report in `to_chat\`, with duration / effort / api-equivalent cost / exit status and a one-line headline.
  Click a row for the report, its live feed and the brief, rendered in place; buttons open the real files in your editor.
- **Today** — briefs, Code time, turns, cost, and a 12-hour strip of when Code was actually running.

Light/dark follows Windows; the sun/moon button overrides it. The bell sends a test toast.

## Toasts

The service fires a Windows toast (with sound; click it to open the dashboard) when:
- a brief finishes — with duration, cost and the report's headline, and *"N items for you"* when NEEDS-<owner> / NEEDS-DECISION / 🔴 are present;
- a brief fails (non-zero exit) or is found interrupted;
- a watcher is gone, halted on error, or an interrupted brief needs a restart.

Switch each kind on/off in `settings.json` → `toasts` (`onQuiet` warns when a running brief has had no live-feed activity for
`quietWarnMin` minutes — off by default because long builds are legitimately quiet). Toasts need Focus Assist / Do Not Disturb off,
and they come from "Windows PowerShell" in the Action Center (that is how unsigned scripts are allowed to toast).

## Waking chat the moment a brief finishes

> **Local orchestrator (Codex pilot, from 2026-09-06):** an orchestrator that runs ON this machine needs none of the relay below —
> it waits on the mailbox files directly, e.g. in PowerShell
> `while (-not (Test-Path 'D:\IntrusiveAnomalies\_mailbox\to_chat\<name>.report.md')) { Start-Sleep 30 }` (with a timeout), or
> polls `watcher_status.txt` for `state=idle`. Then it reads the report header: line 2 is `verdict: GREEN | NEEDS-DECISION | RED`,
> and after NEEDS-DECISION/RED the watcher HOLDS the next brief unless it carries `# ACK-VERDICT: <resolution>` (see the mailbox README).
> The relay stays for cloud chat sessions.

Cross-machine messaging between a headless Code run and a cloud chat session is not available, so the service uses a tiny relay:
whenever a report lands it pushes `signal.json` to the GitHub repo in `settings.json` → `signal.remote` (`E:\MailBox\signal` is the
working clone, created automatically). The repo is **public and the file is opaque**: one entry per mailbox — an unguessable key
and a report count, nothing else — and commits are authored "signal" with a fixed year-2001 date, so the history shows no real
activity times either:

```json
{ "f2e079e1": 29, "a1c057b4": 4 }
```

The key is `sha1("mailbox:" + id)[0:8]`; the current keys are in `E:\MailBox\signal-keys.json` and in `/api/state` → `settings.signalKeys`.
Pushes use `E:\MailBox\signal.token` if that file exists (needed when the repo belongs to a separate GitHub account), otherwise the
git credentials already stored for github.com. Reading needs no credential at all — by design, so no chat session ever has to be
handed a secret.

A chat session arms a background watcher that polls the raw URL every 60 s (the cloud can reach `raw.githubusercontent.com`),
filtered to its own key, and gets woken on change. GitHub serves raw files through a ~5-minute cache that the `?t=` query does not
defeat, so the wake lands 0–5 min after the push (measured: 4.5 min on the first live run) — still a fraction of a scheduled
check, and between events it costs nothing. The chat keeps a long fallback wake (30–60 min)
in case its container was recycled while waiting, and re-arms the watcher then.

Recipe for a chat session (set `MB` to the mailbox id; the key is computed inside the command):

```
Monitor({ description: 'GDP mailbox: a report landed', persistent: true, command: `
URL="https://raw.githubusercontent.com/mr<HOSTNAME>/mailbox-signal/main/signal.json"; MB=gdp
KEY=$(printf 'mailbox:%s' "$MB" | sha1sum | cut -c1-8); last=""
while true; do
  cur=$(curl -sf -A mailbox-monitor "$URL?t=$(date +%s)" 2>/dev/null | tr -d ' \n' | grep -o "\"$KEY\":[0-9]*" || true)
  if [ -n "$cur" ] && [ "$cur" != "$last" ]; then [ -n "$last" ] && echo "MAILBOX SIGNAL $MB changed ($cur) — a report landed, read to_chat"; last="$cur"; fi
  sleep 60
done` })
```
Some sessions' sandboxes refuse shell HTTP fetches altogether. Then don't work around it: poll adaptively instead — first check at
the brief's expected finish (from effort/type and past durations), then every ~5 min while `watcher_status.txt` says running,
60 min when idle.

Plus a `ScheduleWakeup` / `send_later` fallback (30–60 min); on any wake, re-arm the Monitor if it is not running.

The counter is the **true number of `*.report.md` files** (`reportCount`), not the length of the parsed
`history` list — that list is capped for parsing cost, and using it silently froze the signal at 40 once GDP
crossed 40 reports, so no chat session was woken for four briefs. Never wire a progress counter to `history`.

`push-signal` (via the API `{"action":"push-signal"}`, or the power button → `signal`) forces a push for testing; the service log
(`/api/log`, or power button → `log`) shows `[signal] pushed …`. `signal.opaque: false` switches back to a readable
`{ "<id>": { reports, last } }` shape — only sensible for a private repo.

## Claude usage windows

The band above the mailboxes shows your Claude usage limits. There is **no scriptable way to query them** —
`/usage` is interactive-only and there is no `claude usage` command — but Claude Code emits a `rate_limit_event`
on the stream during every headless run, so `render.ps1` captures the newest one per window type into
`<mailbox>\_ratelimit.json` and the dashboard merges them across mailboxes (limits are account-wide).

What each part means, and how much to trust it:

| Shown | Where it comes from |
|---|---|
| Window name + reset time | Claude's own event (`rateLimitType`, `resetsAt`) — authoritative |
| The bar | **Time through the window** normally; switches to **% of the limit used** when Claude sends `utilization` |
| `WARNING` / `REJECTED` chip | Claude's `status` — fires a Windows toast the first time each state appears |
| "in this window: N briefs · turns · Code time · api-equiv" | Computed here from `costs.csv` inside the window — *your spend*, not a share of the cap |
| `no current reading` chip | The last window Claude reported has already ended. Windows anchor to your first use, not to the clock — observed resets sat 5 h, 4 h 50 m and 9 h 40 m apart, one at :50 past — so a passed reset says nothing about the window open now, and the dashboard waits for the next brief instead of inventing a boundary. |

`utilization` is only sent once a window approaches its cap, so most of the time no percentage exists — the
dashboard shows the time bar instead rather than inventing one. Nothing here is a share of a published limit,
because Anthropic doesn't publish the number.

Before a mailbox has run a brief with the updated `render.ps1`, the values are scraped out of old live feeds
instead (the band says "read from live feeds"); the first new brief replaces that with the real event.

## Improvements to the watcher itself (canonical copies in `watcher\`)

- **Crash recovery.** If the watcher starts and finds an `_active_` brief (the previous watcher died mid-run: host crash, closed window,
  reboot), it files it under `processed\<ts>_CRASHED_<name>.md`, cleans the temp files, and writes a stub `to_chat\<name>.report.md`
  marked INTERRUPTED with the last live-feed lines — so chat sees it and writes the resume brief, nobody has to notice by hand.
- **Rate-limit capture** (`render.ps1`): every `rate_limit_event` is written to `<mailbox>\_ratelimit.json` and rendered
  in the feed as `RL five_hour allowed resets 16:00` — that is what feeds the usage band above.
- **Live feed fixes** (`render.ps1`): reads Claude's stream as UTF-8 and is saved with a BOM so the `…` marker no longer comes out as `â€¦`;
  `tool_progress` heartbeats render as `.. PowerShell still running` and rate-limit events as `RL five_hour allowed` instead of raw JSON.
- The dashboard compares each mailbox's three scripts with `watcher\` and shows **Update scripts** when they drift (a backup goes to
  `processed\`). `run_brief.ps1` and `render.ps1` hot-reload per brief; `watcher.ps1` applies on the next watcher start.

## Adding a mailbox

```
powershell -ExecutionPolicy Bypass -File E:\MailBox\new_mailbox.ps1 -Path "E:\Projects\Foo\_mailbox" -Cwd "E:\Projects\Foo" -Name "Foo"
```
(optional `-Model`, `-Effort`, `-McpConfig "<path to mcp.json>"`). It creates the folder tree, copies the scripts, writes
`start_watcher.bat`, `README.md` and `config.json`, and registers the mailbox in `mailboxes.json` — the dashboard picks it up on the
next scan. Then give the project's `CLAUDE.md` a "Mailbox mode" section and drop the first brief in `to_code\`.

## Settings (`settings.json`)

| key | default | what |
|---|---|---|
| `port` | 7337 | http://localhost:PORT/ (loopback only — nothing is exposed to the network) |
| `pollMs` | 2500 | how often the folders are re-read |
| `tailLines` | 12 | live-feed lines shown on the page |
| `quietWarnMin` | 20 | minutes without live-feed activity before a running brief is flagged quiet |
| `editor` | "" | full path to an editor exe for the Open buttons; empty = Windows default app for .md/.txt |
| `openBrowserOnStart` | false | open the page in the default browser when the service starts |
| `signal.remote` | "" | GitHub repo URL for the wake-chat signal (empty = off); `signal.branch` default `main`; `signal.tokenFile` default `signal.token`; `signal.opaque` default true |
| `toasts.*` | on | `enabled`, `onDone`, `onNeedsYou`, `onFail`, `onDown`, `onQuiet` |

Edit and save; the service re-reads `mailboxes.json` every scan and `settings.json` on restart (or the "reload-settings" action).
