# Documents from the home workspace root

- `AGENTS.md` — the standing instructions given to Codex when it worked in this workspace.
- `_strip_comments.py` — the deterministic comment stripper. The repo's source carries no comments by
  convention (`CLAUDE.md` Invariants); the home agent runs this before every commit. You do not commit,
  so you only need to know why the source has no comments.
- `relay/README.md`, `relay/gdp-impl-block-v1.txt`, `relay/gdp-relay-block-v1.txt` — how the home side
  drove Codex as implementer and reviewer ("the relay"). History and context only; the office never runs it.

Copies carry the pack's mechanical redactions; see `../START-HERE.md` section 7.

**Code files in this pack carry an added `.txt` suffix** (for example `run_leg.ps1.txt`): they are a text snapshot, not runnable copies, and the repo's no-comments rule for source does not apply to them.
