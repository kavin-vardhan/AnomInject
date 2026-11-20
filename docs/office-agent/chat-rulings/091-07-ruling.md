# 091-07 — Ruling (ACCEPTED): the Monday release is `release/2026-10-05` `23098fa`

Date: 2026-10-04 ~11:45 IST · Author: Claude Code Desktop (orchestrator/reviewer) · Implementer: Codex `gpt-6-astra` / `max`, full access · Run `_relay\runs\2026-10-04-091-07-release-transitions-off\` (10:54–11:28, VERDICT GREEN)

## What it is
- `d660b5d` plus one behaviour change: the temporal transition defaults On/Off/Hide are **0/0/0** (owner's choice, release freeze).
- Explicit CVar values ≥ 0 still apply, and a new test restores 3/40/1 that way.
- Nothing from 091-05/091-06 is included.

## My independent checks
| check | result |
|---|---|
| origin heads | `release/2026-10-05` = `23098fa` (new ref, one commit). proxy `5d6352b`, m53 `4c93e5f`, fix `1eaf205`, master `b5f15a3`: all unchanged |
| diff `d660b5d..23098fa` | 7 files: 3 constants (`AnomalyLabelSync.h`), 3 help strings (`AnomalyCaptureSubsystem.cpp`), 2 pure tests, readme, journal, CLAUDE.md status. Read in full; correct and in scope |
| my own recount, PIE leg (`session_20261004-111827`) | 8 events, 384 labelled frames, **0 label mismatches, 0 transition mismatches, 0 orphans**; reasons `pie_end_settle` ×8 only; run_summary on/off/hide **0/0/0** |
| my own recount, staged leg (`session_20261004-112212`) | 7 events, 336 labelled frames, **0 mismatches**, **no transition reasons**; on/off/hide **0/0/0** |
| labelled frame sets vs the retained `C7FB5212` baselines | identical injected and affected sets on every event. PIE event ids differ by a constant +6 engine frames (startup), and the transition count per event falls from 43 (3+40) to 1 (`pie_end_settle`) |
| archive `release-2026-10-05-23098fa-218BA4C7` | all six binaries re-hashed; they match the report |

## Ruling
- **ACCEPTED.** The office pull for Monday is **`release/2026-10-05` at `23098fa`**.
- Follow-up 091-08 is docs only and owner-approved: the readme gets the one-live-game-view note, the two rare edge cases and the anti-aliasing tip. It adds one docs commit on top of `23098fa`; no source change.

## 091-08 addendum (ACCEPTED, 11:50 IST)
- `release/2026-10-05` is now **`d7eee97`**, a fast-forward from `23098fa`.
- 3 docs files only: the readme §7 "Known limitations of this build", the §8.7 link, a CLAUDE.md pointer and the journal.
- I verified that `git diff 23098fa d7eee97 -- Source Shaders tools Config *.uplugin` is empty, and that master and proxy are unchanged.
- **The final office pull is `release/2026-10-05` @ `d7eee97`. Its code is identical to `23098fa`, so exe `218BA4C7` stands.**
