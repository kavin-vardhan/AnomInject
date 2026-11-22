# Report card format (what travels home as a photo)

The only path from the office to home is the owner photographing a report on the office screen.
So the report is built for a photo: one card per photo, nothing wraps, nothing can go missing unnoticed.

## Rules

- Plain monospace text, **at most 90 characters per line**, **at most 40 lines per card**. One photo
  holds one card.
- **Line 1 of every card:**
  `REPORT <id> · card <i> of <n> · host <codename> · plugin <commit> · copy untouched yes/no · host changes <ids or none> · <date>`
  — if that does not fit in 90 characters, use the short form
  `REPORT <id> · <i>/<n> · <host> · <commit> · untouched yes/no · H <ids|none> · <date>`.
  - `plugin <commit>` is the commit in the game's `Plugins\AnomalyInjector\COPIED_COMMIT.txt`. If that file
    is missing (the delivered build, O-001), write the version you identified and `(by files)`, and say on
    the card how you identified it.
  - `copy untouched yes` means every reading on the card that is not marked `EXPERIMENT` was taken with the
    plugin copy exactly as the owner copied it, no experiment applied (or one restored and proven,
    `OFFICE-AGENT-RULES.md` rule 3).
- Then numbered answers `A1, A2, …` matching the brief's questions `Q1, Q2, …`. One fact per line.
  Numbers carry units (frames, px, ms, %, MiB).
- `MEASURED:` and `INFERRED:` lines are kept apart. A reading from a throwaway experiment is marked
  `EXPERIMENT:` and carries its before and its after.
- The last card ends with `ALSO:` (what chat should have asked) and then
  `END <brief id> · <total line count across all cards>`, so chat can tell no card and no line is missing.
- Nothing that identifies the host title: codenames, our own tags for assets, our `file:line`, our log
  lines, public engine API names. No host code. No person, studio or fork names.
- A longer version may stay at the office in the journal folder. The card is what travels.

## Worked example (invented data — no real host, run or reading)

```
REPORT 097-03 · 1/2 · Bates · 0c71f6f · untouched yes · H none · 2026-11-24
A1 Q1 build: plugin copy at 0c71f6f (COPIED_COMMIT.txt), Editor built, exit 0, 0 warnings.
A1 MEASURED: LogAnomaly startup banner present; 12 anomalies registered.
A2 Q2 m58 terrain patterns: ini [AnomalyInjector] TerrainNamePatterns set to 2 tags.
A2 MEASURED: 3 blinking events on our tag T1 (ground mesh, 1 LOD) -> subtype
A2   terrain_disappear_reappear on 3 of 3; 5 events on other targets -> plain subtype.
A2 MEASURED: run log line "terrain match pattern=1 field=asset" on all 3.
A3 Q3 settle flags: 2 captures x 300 frames, delivery OFF.
A3 MEASURED: transition_reason settle on 2 frames after each onset and offset,
A3   16 of 16 edges; frames_dropped 0 both runs.
A3 INFERRED: settle K=2 is enough here; not tested at other frame rates.
A4 Q4 forced LOD on our tag T2 (rock mesh, 4 LODs, instanced proxy):
A4 MEASURED: lod_corruption refused hism_ignores_forced_lod on 4 of 4 fires.
A4 EXPERIMENT: before = refusal 4/4; after (in place, backup 097-03-backup, one-line
A4   change in AnomalyLodPolicy.h:41 to treat T2 as plain ISM) = fired 4/4, picture
A4   unchanged on 4/4 (max pixel diff in box 2 levels). Supports the refusal; 1 run
A4   each. Restore proven: 0 files differ from the backup, plugin rebuilt.
```

```
REPORT 097-03 · 2/2 · Bates · 0c71f6f · untouched yes · H none · 2026-11-24
A5 Q5 check kit on capture 1: label_sync_check.py 1.6, Pillow present.
A5 MEASURED: 8 judged, 0 FAIL, 2 unjudged (spatial lod), runtime 41 s.
ALSO: Q3 should have asked about letterboxed views; this host letterboxes
ALSO:   in one menu level, and the settle flags were not checked there.
END 097-03 · 23
```
