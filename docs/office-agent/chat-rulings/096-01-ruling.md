# 096-01 — Chat ruling: the office context pack is ACCEPTED; six decisions ruled; the push is released

**Date:** 2026-11-20, about 11:45 IST · **Author:** chat Claude (orchestrator)
**Input:** mailbox report 096-01 (NEEDS-DECISION as expected; 26 min, $13.08 API-equivalent; session `8d69c0fb`, xhigh). Nothing was pushed.

## Background: the new arrangement (owner's rulings of 2026-11-20)
- The owner has a Claude agent on his **office PC**, where the real host games are. It does groundwork, tests, debugging and planning there.
- **The plugin is off limits for kept changes** at the office. Kept plugin changes are written only at home and travel by `git push`.
- **Throwaway experiments on the plugin are welcome** ("200% yes. We want definite proof it works"): scratch copy only, prediction first, before and after readings, then discarded. A fix proven that way is provisional until the home agent's pushed version passes the same test on a clean tree.
- **The office agent may change and keep a host's own game or engine code** when that is the easy, safe fix, and report it. It keeps a complete host change log at the office.
- **It never pushes anything anywhere.**
- Traffic: home → office by git; office → home only as the owner's photos of short report cards. The hard rule on office data is unchanged.

## What Code assembled
- Local branch `docs/office-pack` = `cde2943` (9 commits on `integ/m56-m58` `0c71f6f`), in a worktree at `_relay\runs\2026-11-20-096-office-agent\wt`.
- 1,768 files, 17.2 MiB under `docs/office-agent/`: START-HERE, the rules, the report card, how-we-work; 740 chat rulings and reviews; 324 briefs; 316 reports; run predictions; a text snapshot of the bench repo; documents from unpushed local refs; the home agent's memory; hand-offs.
- About 940 mechanical redactions (the owner's name, profile path, machine name, personal folders, e-mails, session ids). Scans clean.
- Chat's own check, read-only on the worktree: no file in the pack contains the employer's name or its products.

## Decisions
- **D1. Push scope: all nine commits.** The brief and report archive is the development path the owner asked for. The hand-off copies are copies; the owner's untracked originals stay untouched and unstaged.
- **D2. The held-back items.**
  - **The 12 files with a real-name term: released after a codename edit** of the affected lines, then a rescan. The file whose own name carries a term is renamed.
  - **Chat's rulings `095-00` and `095-11` and the hand-off `CHAT-HANDOFF-095`: released with the client's verbatim wording replaced by our own paraphrase,** marked as such. They are the basis of all current work, and the same findings in our own words are already throughout the pack and the pushed journals.
  - **Stay out** (the owner has not released them): `_bates_reads\`, the m36 census hand-off, `M2-client-feedback-jira-tickets.md`, the 071-076 client-feedback hand-off, note `078-15`, the client-sync and workshop folders. The office agent can read the originals of all of these at the office.
- **D3. The redaction policy is ratified.** The pack says in one file (`REDACTIONS.md`) that copies carry placeholders and are not byte-identical to their sources.
- **D4. Mentions of the owner's other home projects are redacted in the pack** to a placeholder. They are his private business and the pack goes to an office machine. The eight files already on origin are left alone.
- **D5. The pack lives on its own long-lived branch, `docs/office-pack`.** It is not merged into `integ/m56-m58`, `feat/m57` or `master`.
  - **Why:** the held heavy run (095-07b) checks that those tips equal `tips.txt`; moving them now would fail its pre-flight. And 17 MB of history does not belong on the line that ships.
  - **At the office:** one clone, two worktrees. The agent is started in a worktree of `docs/office-pack` (context, briefs, rules, the `CLAUDE.md` switch); the plugin under test stays in the host's own `Plugins` folder at a named commit. `OFFICE-SETUP.md` gives the owner the exact steps.
  - The branch is refreshed by merging the newest shipping line into it at each push.
- **D6. The `_reviews` harness scripts go in as text** (3.5 MB). Methods are knowledge.
- **Briefs to the office travel by git** in `docs/office-agent/briefs-to-office/`. The first one, O-001, goes with this push.
- **Chat's own memory is not exported separately.** Its content is in the rulings and hand-offs; the next session hand-off goes into the repo and so into the pack.

## Corrections for START-HERE
- m57: the review fixes are accepted (095-13) and the prep is applied (095-14, `feat/m57` = `d8d3a7e`); it is not merged into the integration branch. **None of m56, m57, m58, m59 has run in an engine.** The first engine run (095-07b) is written and held until the owner's home PC is free of his renders.
- Facts taken from memory notes or dated sections are marked with their date and source, or re-read from the journals.

## Next
1. **096-01b (light, xhigh, released now):** apply D1 to D6, add `OFFICE-SETUP.md` and brief O-001, rescan, **check the remote is private, then push `docs/office-pack`.**
2. The owner sets the office side up once (`OFFICE-SETUP.md`) and hands the agent O-001.
3. 095-07b stays held.
