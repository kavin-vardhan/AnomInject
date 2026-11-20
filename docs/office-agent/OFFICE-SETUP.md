# Office setup — once, at the office PC

For the owner. About 10 minutes. You type commands into a terminal (PowerShell or Command Prompt); you
do not need to understand them. Replace the words in angle brackets with your real paths, keep the
quotes. Nothing here changes the plugin your host games use, and nothing here sends anything anywhere.

**Two paths you need first:**
- `<PLUGIN>` — the plugin folder inside one host game project, the one you already pull, for example
  `X:\Games\HostA\Plugins\AnomalyInjector`.
- `<BASE>` — an ordinary folder next to your host projects, for example `X:\Games\OfficeAgent`. It must
  **not** be inside any game project, inside any `Plugins` folder, inside a Perforce, Plastic or git
  workspace, or inside a synced folder (OneDrive, Dropbox, a network share). If you are not sure, use
  `C:\OfficeAgent`.

## Steps

1. **Note what the host's plugin is on now**, so you can check it did not move:
   `git -C "<PLUGIN>" status`
   Write down the first line (for example `On branch master`).

2. **Fetch from home:**
   `git -C "<PLUGIN>" fetch origin`

3. **Make the context folder** (a second, separate checkout that holds the agent's instructions):
   `git -C "<PLUGIN>" worktree add --track -b docs/office-pack "<BASE>\anomaly-office-pack" origin/docs/office-pack`
   - If it says a branch named `docs/office-pack` already exists, run instead:
     `git -C "<PLUGIN>" worktree add "<BASE>\anomaly-office-pack" docs/office-pack`
   - Check: `git -C "<BASE>\anomaly-office-pack" status` says `On branch docs/office-pack` and
     `nothing to commit`.
   - Check: `git -C "<PLUGIN>" status` shows the same first line you wrote down in step 1.

4. **Create the marker file** that tells the agent it is on the office PC. In PowerShell:
   `New-Item -ItemType File "<BASE>\OFFICE_HOST"`
   (In Command Prompt: `type nul > "<BASE>\OFFICE_HOST"`.) The file is empty and has no extension. Do not
   make it with Notepad, which adds `.txt`.

5. **Create the office journal folder** (the agent's own notes and the host change log; they never
   leave the office):
   `mkdir "<BASE>\office-journal"`
   You now have, side by side in `<BASE>`: `anomaly-office-pack`, `OFFICE_HOST`, `office-journal`.

6. **Start the agent in the context folder:**
   `cd "<BASE>\anomaly-office-pack"` then `claude`
   Paste this as the first message, exactly:

   > You are the office agent. Read docs/office-agent/START-HERE.md and
   > docs/office-agent/OFFICE-AGENT-RULES.md. Confirm the rules back to me in five lines. Then do brief
   > O-001 in docs/office-agent/briefs-to-office/. Your office journal folder is office-journal, beside
   > this folder: your host notes and the host change log go there. Never push anything anywhere.

   If its five lines leave out "never push" or "nothing leaves this PC except your photos", stop it and
   say so.

## Bringing an answer home

- The agent answers in **report cards**. Each card starts with `REPORT O-001 · 1/3 · …` and the last one
  ends with `END O-001 · <number>`.
- **Photograph each card straight on, one card per photo**, the whole card in the frame, sharp, no glare.
  Check that every card number is there (1/3, 2/3, 3/3).
- Photograph nothing else: no code, no game screens, no file lists. If a card shows a real game name or a
  line of the game's code, do not photograph it; ask the agent to rewrite the card.

## Getting the next brief

1. `git -C "<BASE>\anomaly-office-pack" pull --ff-only`
2. Tell the agent: "Do brief O-002 in docs/office-agent/briefs-to-office/." (use the id chat gives you)

If the pull refuses, something was changed inside the context folder. Do not force it; tell chat. The
plugin your host games use changes only when a brief says so, and that brief gives the steps.

## Never

- Never push, commit or upload from either folder.
- Never copy `anomaly-office-pack` into a game's `Plugins` folder (a second copy of the plugin there stops
  the game project from starting).
- Never move `office-journal` into a repo or a synced folder.
