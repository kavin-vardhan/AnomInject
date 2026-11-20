# How we work (as of 2026-11-20)

## The parties

| who | where | does | never does |
|---|---|---|---|
| **chat Claude** (the orchestrator) | a chat thread | designs, plans, rules on forks, writes every brief, reads every report | edits code, runs anything |
| **home agent** (Claude Code) | the owner's home PC | writes and commits all plugin code and docs, builds, runs bench legs on the home fixtures, pushes | decides design forks on its own; touches office data |
| **office agent** (you) | the office PC with the real hosts | investigates on the real hosts: groundwork, tests, debugging, experiments, plans | changes plugin code that is kept; pushes anything (see `OFFICE-AGENT-RULES.md`) |
| **the owner** | both places | carries briefs in, carries photos of report cards out, runs office procedures, makes product decisions | technical calls (those go to chat) |
| **Codex** (history) | home PC, via `_relay` | was implementer for tool-only units and an independent reviewer at checkpoints | — (only used when the owner says so) |

Traffic is **one-way**: home → office by `git push` / `git pull`; office → home only as the owner's
photos of short report cards. Office data never comes home.

## The loop

1. Chat writes a **brief** (examples: `briefs/processed/`). The owner drops it into the home mailbox
   (`mailbox/README.md`), or carries it to the office agent.
2. The agent works the brief **headless**: nobody answers questions mid-run, so a blocked fork is
   reported as `NEEDS-DECISION` rather than waited on.
3. The agent's **final message is the report** chat reads (examples: `reports-to-chat/`). At home it
   goes back through the mailbox; at the office it is a photographed card (`REPORT-FORMAT.md`).
4. Chat reads the report, rules (rulings: `chat-rulings/`), and writes the next brief.

## How a brief is structured

Briefs follow a stable shape (read three or four in `briefs/processed/` to see it):

- **Header:** session type, model, effort, name (`<NNN-NN> <short title>`), and an acknowledgement
  of the previous verdict.
- **Why:** the purpose in a few sentences.
- **Parts / tasks:** numbered, each with what to deliver.
- **Predictions:** for any run, what it must show, **frozen in writing before the run**
  (`docs/predictions/`, `runs/`). Changes after freezing are separate, dated **amendments**, never
  silent edits.
- **Gates / can-fails:** each gate states how it could fail, and a deliberate breakage (a "can-fail
  lever", e.g. an `IAI.Bench.*` switch) proves the gate is not blind before its pass is trusted.
- **Rules:** what may not be touched (refs, folders, processes), stop conditions ("stop at the first
  failure, report, do not fix in the same turn"), context and time limits.
- **Final message:** the first line is a bare verdict line, then a fixed list of sections.

## Verdict lines

The very first line of every final report is exactly one of:

- `VERDICT: GREEN` — everything asked was done and every gate passed.
- `VERDICT: NEEDS-DECISION` — work is done or safely parked, and chat must rule on something
  (a fork, a failed premise, an unexpected reading) before the next step.
- `VERDICT: RED` — something failed in a way that is a defect or a broken run.

## Method rules that travel (from `CLAUDE.md` and the gotchas)

- **Observation and explanation are separate claims.** Record what you saw as fact; record a mechanism
  as a hypothesis with its evidence, or "cause not established". Never rule out a route on an unverified
  mechanism (G120).
- **Prove an instrument can fail** before trusting its pass (G96): a known-answer control, or a lever
  that breaks the thing on purpose.
- **Read baked values back from the running system** (G119): the source ini is an input, not the artifact.
- **Validate packaged, not only in the editor** (G76).
- **Predictions before runs; amendments are dated and never rewrite history.**
- **Report numbers with their method and run count**; keep MEASURED and INFERRED apart.
- **Codenames only** for hosts; never a real title, studio, fork or person.
- **Stop rule:** when a gate fails, report it and stop; the fix is its own turn.

## Where the home side keeps things (for orientation)

- Plugin repo (this repo): code, `docs/` (architecture, gotchas, journals, predictions, runbook).
- `chat-rulings/` here = the home `_reviews` folder (rulings, plans, reviews, office cards, hand-offs).
- `runs/` here = text receipts of relay runs and later unit runs (`_relay\runs`).
- `bench/` here = a text snapshot of CaptureBench, the home bench plugin (local repo, no remote).
- `briefs/`, `reports-to-chat/`, `mailbox/` here = the home mailbox (`_mailbox`).
- `root-docs/` here = `AGENTS.md` (Codex's instructions), the comment stripper, the Codex relay README.
