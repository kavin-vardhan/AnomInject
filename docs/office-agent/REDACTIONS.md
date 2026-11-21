# Redactions in this pack

**Copies in `docs/office-agent/` are not byte-identical to their sources on the home PC.** Every file
was copied through the same redaction pass, and some files had further edits ruled by chat (ruling
096-01, D2 to D4). This page lists every kind of replacement and everything left out, so a reader
knows what a placeholder stands for and does not mistake it for a real path or name.

Never diff a pack copy against a home original and expect identity. A placeholder is never a real path.
If you need the home original of a file, ask in a report; the home agent maps it by file name.

## Placeholders

| what was in the source | what the copy says | where |
|---|---|---|
| the Windows profile path of the home user | `<USERPROFILE>` (and `C--Users-<user>` inside Claude project folder names) | every file |
| the home machine's name | `<HOSTNAME>` | every file |
| the owner's first name | "the owner" / "the owner's" | every file |
| any other spelling of the owner's name | `<owner>` | every file |
| the owner's account name | `<owner-account>` | every file |
| personal drive folders (video projects, phone backups, photos) | `<personal-folder>` | every file |
| e-mail addresses (except `noreply@anthropic.com` in commit trailers) | `<email>` | every file |
| lowercase UUIDs: Claude and Codex session ids, thread ids | `<uuid>` | every file |
| a real game title, studio or engine-fork name | the host's codename (Concorde, Bates), or a neutral sentence where no codename exists (the packaged executable's name) | 12 files, ruling 096-01 D2a |
| a home folder name that carried such a term (`_reviews\<term>-2026-10-01-diagnosis\`) | the same path with the codename (`_reviews\bates-2026-10-01-diagnosis\`); the folder on the home PC has a different name | 6 files, D2a |
| a file name that carried such a term | `chat-rulings/bates-mip-2026-10-02-triage.md` | 1 file, D2a |
| the client's own wording, quoted | our own plain paraphrase, marked `[client wording paraphrased]` | `chat-rulings/095-00-ruling.md` section 0 and its input list, D2b; one quoted sentence in brief `095-08` (`briefs/processed/`), found by the 096-01b rescan; a six-word fragment in brief `095-10`, 096-01c |
| names of the owner's other home projects, and of their tools and jobs | `<other-home-project>` (`<other-home-project> job`, `<other-home-project> tool`) | throughout the pack, D4 |
| a memory file name that carried such a project name | `home-agent-memory/plugin-project/foreign-editors-block-bench.md` | 1 file, D4 |
| another name an older document used for an office host | the host's codename (Concorde or Bates) | throughout the pack, 096-01c; four file names changed with it |

The codename edits use the home codename checker's own term-to-codename mapping, applied only to the
lines that carried a term. Hosts are written only by codename: Concorde, Bates and Europa. Where an older
document used another name for one of these hosts, the copy carries the codename instead (096-01c).

## Left out of the pack

- **Office readings and client material the owner has not released:** the owner's transcriptions of
  office readings, his write-up of office results, the client-feedback hand-off of sessions 071 to 076,
  the note about the client's reviewed dataset (078-15), the client ticket list, and the client-sync and
  workshop folders. The office agent can read the originals of all of these at the office.
- **Another home project's documents:** five briefs, five reports and six briefs and checkpoints for
  one of the owner's video projects that once ran through the same mailbox (D4).
- **Never copied:** binaries, captured frames, logs, live transcripts, bench evidence data, the
  control-server token, the mailbox app's key and token files, and the home codename checker itself
  (it carries the term list).
- **Owner's untracked originals** in the repo's `docs/` folder stay untracked and unstaged; the pack
  holds copies.

## Other changes to copies

- Code files (bench snapshot, `_reviews` harness scripts) carry an added `.txt` suffix, so they read as
  a text snapshot and are never built or run from here.
- Line endings are LF.
- Nested source paths became file names with `__` as the separator (`evidence-reports/`, `runs/`).
- Dates in the `INDEX.md` files are the last-modified dates of the source files on the home PC.
