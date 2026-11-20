---
name: bash-tool-stalls-use-powershell
description: On this box the Bash tool can hang past its timeout on trivial commands; PowerShell is the reliable shell.
metadata: 
  node_type: memory
  type: project
  originSessionId: <uuid>
  modified: 2026-10-25T23:16:03.372Z
---

Observed 2026-09-20 (session 080-01): the Bash tool exceeded its 120 s timeout on a
plain `git rev-parse`/`git branch` batch, and again on a `cat` of the resulting task
output file — two trivial commands in a row, both backgrounded without producing output.
Every subsequent command in that session ran fine through the PowerShell tool.

**Why:** the environment declares PowerShell as the primary shell here; Bash appears to
be the less reliable path on this machine, and a stall costs two minutes per command
before it is even visible.

**How to apply:** reach for the PowerShell tool first for git, python and file probes in
this repo. If a Bash call is backgrounded on a command that should be instant, do not
retry it or poll the task file — switch shells. Remember PowerShell 5.1 syntax rules
(no `&&`/`||`, no ternary, use a message FILE for `git commit -F` — piping `$msg | git commit -F -`
put a U+FEFF on the commit subject in 082-04; write the file BOM-free first — and
`[System.IO.File]::WriteAllText` with `UTF8Encoding($false)` to avoid the BOM that
`-Encoding utf8` writes — see [[the G141 BOM gotcha in docs/gotchas.md]]).

Added 2026-09-30 (089-04b): a `git commit -m @'...'@` here-string whose text holds `"end -4"` was split into native
args and git read `-4` as a switch (nothing committed). And `$env:TEMP` resolves to `C:\ClaudeTemp` in this shell, not
AppData\Local\Temp — write the message file with the Write tool to an explicit path, then `git commit -F <that path>`.

Added 2026-10-25 (094-12): **a failed Bash call can WRITE INTO THE m51 MAIN CHECKOUT.** A background
`cd <dir> && python ... > f; echo exit=$? >> f` could not create its temp cwd, skipped `cd`/python, and the trailing
`echo >> ek1-prescan-console.txt` landed in the session cwd (the m51 checkout): one 7-byte untracked file. The Europa
EA8 entry proof covers that checkout's git status (15 untracked recorded), so Codex STOPPED at entry after 11 min and
the whole job was lost; I could not delete the file (brief: "you delete nothing"). **Never use Bash for anything that
writes; always give redirects an absolute path; before launching a job whose entry proof covers m51, run
`git status --porcelain` there and compare with the recorded untracked count.**

Added 2026-10-26 (094-15): passing multi-line Python to `python -c $code` from a PowerShell here-string strips the
code's double quotes (Windows PS 5.1 native-arg quoting), so the script fails with SyntaxError. Write the script to a file
with the Write tool and run `python -B <file>`.

Added 2026-10-26 (094-16): `Start-Process cmd.exe -ArgumentList '/d','/c',"`"$bat`" --x `"$y`""` fails with "The filename,
directory name, or volume label syntax is incorrect." because cmd strips the leading quote when the line holds more than two
quotes. Use `'/d','/s','/c'` and wrap the whole command in one more pair of quotes.

Added 2026-09-25 (081-24): the Monitor tool runs Git Bash, which here has no `grep`
and failed `tail -f | while read` with "fork: Bad file descriptor". To wait on a long
background job, run a small PowerShell loop in the foreground (up to the 600 s tool
timeout) that exits when the job's log gains a line; a bare long `Start-Sleep` is
blocked by the harness.

Added 2026-11-04 (094-43a): in PS 5.1, `Get-Date -UFormat %s` returns LOCAL-time seconds (here +19800 s), not a Unix epoch; a reader filtering files by mtime >= that value saw nothing (my gate monitor read "gates: none" for 30 min). Use `[DateTimeOffset]::UtcNow.ToUnixTimeSeconds()`.

Added 2026-11-06 (094-48b): PowerShell variable names are case-insensitive, so `$s = ...` overwrites `param $S`; a read block using both failed with a generic catch. Use distinct names (`$sum`).
