# Briefs to the office

Chat Claude (the orchestrator, at home) writes the office agent's work here. The home agent commits
each brief to the branch `docs/office-pack`; the owner pulls the context worktree at the office and
tells the agent which brief to do.

- **Naming:** `O-NNN-<slug>.md`, numbered in order (`O-001`, `O-002`, …). A number is never reused. A
  correction to a brief is a new brief that says what it replaces.
- **One brief at a time.** Do the brief the owner names, and only that one. Briefs already answered stay
  here as the record; do not redo one unless a newer brief asks.
- **Answers are report cards** (`../REPORT-FORMAT.md`): one card per photo, line 1 names the brief, the
  host, the plugin commit, whether the tree was clean and the host changes in effect; the last card ends
  with `ALSO:` and `END <brief id> · <total lines>`. The owner photographs the cards; nothing else
  travels home.
- **The rules always win** (`../OFFICE-AGENT-RULES.md`). If a brief asks for something they forbid, do
  not do it, and say so on a card.
- Your longer notes, the host change log and anything that may not leave the office go into the office
  journal folder, never into this folder.

## The briefs, in order

| Brief | What it asks | Runs |
| --- | --- | --- |
| `O-001-first-groundwork.md` | Groundwork on the delivered release build: what the ground is made of, how much scenery can pop a LOD, the texture-corruption stall, the reviewed session. | First. Its release-build readings are the baseline O-002 compares against. |
| `O-002-first-engine-test.md` | First engine test of m56, m57, m58 and m59 on the real hosts, at the pushed commit `d8d3a7e`. Uses the readers in `../office-tools/`. | After O-001. |
