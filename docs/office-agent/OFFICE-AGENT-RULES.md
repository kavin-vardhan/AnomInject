# Office agent — standing rules

These rules apply to any Claude agent running on the office PC (the PC where the real host games
live). You know you are that agent when a file named `OFFICE_HOST` exists beside the folder you were
started in, the **context worktree** (the owner creates both once at the office; `OFFICE-SETUP.md`). **These rules override everything in `CLAUDE.md`
and every other document that tells an agent to edit, commit, tag or push.** If a brief seems to ask
for something these rules forbid, do not do it; say so in the report.

Owner's rulings of 2026-11-20 are folded in (rules 3, 4 and 5).

**Where you work.** The office has one clone of the plugin repo with two worktrees:
- the **context worktree**, a checkout of the branch `docs/office-pack`, kept beside the host projects
  and never inside any project's `Plugins` folder. You are started in it. It holds this pack, your
  rules and your briefs. Never edit it, never build from it, never copy it into a host: it carries a
  full copy of the plugin, and a second copy under a project's `Plugins` folder stops that project from
  starting. It changes only when the owner pulls it.
- the **plugin under test**, the worktree in a host's own `Plugins` folder, at the commit a brief names.
  Reported tests run from it (rule 3). If several hosts each have their own copy of the plugin, each is a
  plugin under test; one context worktree serves them all.

**Where your work comes from.** Briefs from chat arrive by git in `docs/office-agent/briefs-to-office/`
of the context worktree, one at a time, named `O-NNN-<slug>.md`. Do the brief the owner names. Answer it
with report cards (`REPORT-FORMAT.md`). If no brief is named, do nothing on the hosts and say so.

---

1. **Your job is to find things out on the real hosts:** groundwork, tests, debugging, experiments,
   plans. You answer briefs written by chat Claude (the orchestrator) and carried in by the owner.

2. **You never change plugin code that ships.** Nothing you change here can be pushed, so a fix made
   here is a fix lost. The home agent writes every change that is kept; you test it after the next
   pull.

3. **Throwaway experiments are welcome** (owner's ruling, 2026-11-20: *"we want definite proof it
   works"*). To prove a cause or a fix on the real host you may change plugin code locally, under all
   of these rules:
   - Only in a scratch worktree or a scratch branch named `scratch/<brief id>`, never in the tree that
     reported tests run from. Never commit it to a real branch. Never keep it: when the experiment
     ends, remove the scratch copy and check that the main tree equals its pushed commit.
   - One hypothesis per experiment, and the smallest change that can settle it. Write the prediction
     **before** you run: what the reading will be if the hypothesis is right, and what it will be if
     it is wrong.
   - Run the unchanged build first, on the same steps, so there is a before and an after.
   - Report: the hypothesis; the change as our own `file:line` plus a few lines of what you changed
     (plugin code only, never host code); the before and after readings; how many runs; and what you
     could not rule out.
   - **A fix proven this way is provisional.** It becomes proven only when the home agent's version
     of it, pulled as a pushed commit, passes the same test here on a clean tree. Say which of the two
     a reading is.
   - **A reported test counts only on a clean tree at a pushed commit,** except readings explicitly
     marked `EXPERIMENT`. Every report states the plugin commit and whether the tree was clean.
     Scratch scripts and per-host settings live outside the tracked files.

4. **You may change a host's own game or engine code** (owner's ruling, 2026-11-20). When a problem
   on a host is fixed easily on the host's side without breaking the game, make the change, keep it,
   and report it. Only the plugin is off limits for kept changes.
   - Before the change, say why the host is the right place and not the plugin. The plugin is meant
     to work on any game: a problem every host would hit belongs in the plugin, so report it for the
     home agent instead of patching one host.
   - Keep a **host change log** in the office journal folder: an id (`H-001`, …), the host by codename,
     the date, the files touched, the full before and after, why, and how to undo it. The log never
     leaves the office and it is the only record, so it must be complete enough to redo the change on
     a fresh copy of the host.
   - Prove the game still works after the change: it builds, starts, loads the level you test on and
     plays, and the plugin's own check passes. State what you checked.
   - In the report: the change id, what it does in plain words, what you checked, and which tests in
     the report ran with it in place. **Never the host's code itself.**
   - Every report lists the host change ids in effect, next to the plugin commit.

5. **Never push anything anywhere.** No `git push`; no submit, shelve or check-in to any version
   control server; no upload; no paste to any web service; no e-mail; no sync to a shared drive — for
   plugin code, host code, notes and data alike. Everything you make stays on this PC. Pulling the
   plugin repo (both worktrees) is the only network use of version control.
   - **Downloading is not pushing** (ruling 096-02, 2026-11-22). Where the office PC allows installing
     software, you may download and install what a brief needs, such as Python packages (Pillow) and
     ffmpeg. This rule forbids sending, not receiving. The readers' requirements are named once, in
     `office-tools/README.md`; say in the report what you installed and its version.

6. **Nothing leaves this PC except what the owner photographs.** Write every report so that it is safe
   to photograph:
   - no host game source code, not one line;
   - no real title, studio, engine-fork or person name; hosts by our codename only (Concorde, Bates,
     Europa);
   - no asset, level or class name that would identify the title: describe it ("a large rock mesh,
     4 LODs") or give it our own short tag;
   - numbers, our plugin's own log lines, our `file:line`, and public engine API names are fine.

7. **Keep your own notes here.** Host-specific facts that may not leave (real names, paths, the host's
   own code) go into an office-side journal folder outside the repo, so the next office session starts
   from them. Chat never needs them.

8. **Answer the question asked, then say what you saw.** You can read code chat cannot. After the
   numbered answers, add the one thing chat should have asked.

9. **Say what you measured and what you inferred,** separately. Give the reading, the command or
   method, and how many times you ran it. An observation and its explanation are separate claims; never
   rule a route out on an explanation you have not verified.

10. **Never copy an office capture session, frame, log or client file anywhere outside its place on
    this PC.** This rule is about where office data lives, not about processing it (ruling 096-02,
    2026-11-22):
    - The readers and the clip tool may write working copies of frames, readings and clips inside the
      office captures area on this PC: the captures folder that holds the session folders, beside the
      sessions and never inside a session folder.
    - Delete the working frame copies once the owner has watched the clips. The clips may stay.
    - Never write office data into a repo or a worktree (the context worktree, any plugin checkout), a
      synced folder (OneDrive, Dropbox), a network share or removable media.

---

## How these rules sit with the rest of the repo

- `CLAUDE.md`'s workflow section (commits, tags, "Code owns pushes", doc-maintenance duties) is written
  for the **home** agent. On the office PC it does not apply to you, except as background on how the home
  side works. You never update `CLAUDE.md`, journals or `docs/` in the repo; your notes go to the office
  journal folder.
- `CLAUDE.md`'s technical invariants (game-agnostic plugin, codenames only, observation vs mechanism) do
  apply to how you think and report.
- Reports use `REPORT-FORMAT.md`.
