# Office setup — for the owner, at the office PC

You type commands into a terminal (PowerShell or Command Prompt); you do not need to understand them.
Replace the words in angle brackets with your real values and keep the quotes. The `git` and `robocopy`
lines are the same in both terminals; where the two differ, both are given. Nothing here sends anything
anywhere.

**How it fits together.** You keep one clone of the plugin repo on this PC and you are the only one who
uses git. The agent never uses git. You copy two things out of your clone:
- the **pack** (the agent's instructions and briefs) into `C:\OfficeAgent\pack`;
- the **plugin version a brief names** into a game's `Plugins\AnomalyInjector`.

The agent runs inside a game project folder. It keeps its notes in `C:\OfficeAgent\office-journal`.

**Values you need:**
- `<CLONE>` — your clone of the plugin repo, for example `D:\src\AnomInject`. It must not be inside any
  game project or any `Plugins` folder, nor in a synced folder (OneDrive, Dropbox, a network share).
  Never open it in an editor or build it: you only pull and copy from it.
- `<GAME>` — a game project folder, the one that holds the game's `.uproject` file.
- `<CODENAME>` — Concorde, Bates or Europa: the game you are working on.
- `<COMMIT>` — the commit a brief names, for example `d8d3a7e`.

Write every path without a backslash at the end (`"D:\src\AnomInject"`, not `"D:\src\AnomInject\"`):
robocopy misreads a backslash just before a closing quote.

## Once

1. **Make the two folders.**
   - PowerShell: `New-Item -ItemType Directory -Force "C:\OfficeAgent\pack", "C:\OfficeAgent\office-journal"`
   - Command Prompt: `mkdir "C:\OfficeAgent\pack"` and then `mkdir "C:\OfficeAgent\office-journal"`
   Never put your own files in `C:\OfficeAgent\pack`: refreshing the pack replaces that whole folder.
2. **Sign in to GitHub on this PC.** The repository is private, so `git fetch` needs a GitHub sign-in
   (or a read-only deploy key). If the first fetch asks you to log in, that is why.
3. **Get the pack** (A below).

## A. Get or refresh the pack

Do this once now, and again whenever chat says the pack changed (for example, a new brief).

1. `git -C "<CLONE>" fetch origin`
2. `git -C "<CLONE>" checkout --detach origin/docs/office-pack`
3. `robocopy "<CLONE>\docs\office-agent" "C:\OfficeAgent\pack" /MIR /R:2 /W:5 /NFL /NDL /NP`

robocopy ends with a small table. The `FAILED` column must read 0 on every line. If step 2 refuses
because files in your clone were changed, do not force it: tell chat.

## B. Put a plugin version into a game (only when a brief says so)

1. **Close** the game's editor and any running copy of the game.
2. **Check out the version:**
   `git -C "<CLONE>" fetch origin`
   `git -C "<CLONE>" checkout --detach <COMMIT>`
   Check: `git -C "<CLONE>" log -1 --oneline` starts with `<COMMIT>`.
3. **Move the old copy out of the game** into a backup folder, so nothing is lost. Use a folder name you
   have not used before, for example `Concorde-2026-11-24`:
   `robocopy "<GAME>\Plugins\AnomalyInjector" "C:\OfficeAgent\plugin-backups\<CODENAME>-<DATE>" /E /MOVE /R:2 /W:5 /NFL /NDL /NP`
   Afterwards `<GAME>\Plugins\AnomalyInjector` is gone or empty. If the game has no copy yet, skip this
   step.
4. **Copy the new version in,** without `.git` and without `docs\office-agent`:
   `robocopy "<CLONE>" "<GAME>\Plugins\AnomalyInjector" /MIR /XD "<CLONE>\.git" "<CLONE>\docs\office-agent" /XF .git /R:2 /W:5 /NFL /NDL /NP`
   The `FAILED` column must read 0 on every line.
5. **Write the commit into the copy:**
   - PowerShell: `git -C "<CLONE>" rev-parse --short HEAD | Set-Content -Encoding ascii "<GAME>\Plugins\AnomalyInjector\COPIED_COMMIT.txt"`
   - Command Prompt: `git -C "<CLONE>" rev-parse --short HEAD > "<GAME>\Plugins\AnomalyInjector\COPIED_COMMIT.txt"`
   Check: `type "<GAME>\Plugins\AnomalyInjector\COPIED_COMMIT.txt"` shows `<COMMIT>`.
6. **Check there is exactly one copy of the plugin** under the game's `Plugins` folder (two copies stop
   the game project from starting):
   - PowerShell: `Get-ChildItem -Recurse -Filter AnomalyInjector.uplugin "<GAME>\Plugins" | Select-Object FullName`
   - Command Prompt: `dir /s /b "<GAME>\Plugins\AnomalyInjector.uplugin"`
   It must list one line only: `<GAME>\Plugins\AnomalyInjector\AnomalyInjector.uplugin`. If it lists
   more, do not start the game; tell chat.

The new copy has no build files, so the game builds the plugin again; the agent does that build when the
brief says so. The copy you moved out in step 3 is your safety copy: keep it until chat says it is not
needed.

## C. Start the agent

1. Go to the game project folder and start Claude Code:
   - PowerShell: `Set-Location "<GAME>"` then `claude`
   - Command Prompt: `cd /d "<GAME>"` then `claude`
2. Paste the first message from `C:\OfficeAgent\pack\FIRST-MESSAGE.md`, with its two values filled in.
3. The agent answers with its rules in five lines and what it found in the game, then waits. If the five
   lines leave out "never push", "never use git" or "nothing leaves this PC except your photos", stop it
   and say so.
4. Then tell it which brief to do, for example: `Go: do brief O-001 in C:\OfficeAgent\pack\briefs-to-office.`

One session works in one game. For another game, start a separate session in that game's folder.

## Getting the next brief

1. Refresh the pack (A).
2. If the brief needs a different plugin version, put it in first (B).
3. Tell the agent: `Do brief O-002 in C:\OfficeAgent\pack\briefs-to-office.` (use the id chat gives you)

## Bringing an answer home

- The agent answers in **report cards**. Each card starts with `REPORT O-001 · 1/3 · …` and the last one
  ends with `END O-001 · <number>`.
- **Photograph each card straight on, one card per photo**, the whole card in the frame, sharp, no glare.
  Check that every card number is there (1/3, 2/3, 3/3).
- Photograph nothing else: no code, no game screens, no file lists. If a card shows a real game name or a
  line of the game's code, do not photograph it; ask the agent to rewrite the card.

## If you followed the earlier setup

The earlier steps (a second checkout called `anomaly-office-pack` made with `git worktree`, and a file
called `OFFICE_HOST`) are no longer used. Leave them alone; they do no harm as long as they are not inside
a game's `Plugins` folder. A game's `Plugins\AnomalyInjector` must now be a copy made as in B, with no
`.git` inside.

## Never

- Never push, commit or upload anything from this PC.
- Never put your own files in `C:\OfficeAgent\pack`.
- Never copy `C:\OfficeAgent\pack` or your clone itself into a game's `Plugins` folder; only a copy made
  as in B goes there.
- Never move `C:\OfficeAgent\office-journal` or `C:\OfficeAgent\plugin-backups` into a repo or a synced
  folder.
