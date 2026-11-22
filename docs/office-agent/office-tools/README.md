# Office tools: readers for the first engine test (brief O-002)

Six read-only checks that read a capture session folder and print a verdict. They were written at home for the m56
and m57 bench gates and copied here (from the bench repository, branch `m57-gates`, commit `4ca8524`) so you can run
them at the office. On the office PC this folder is `C:\OfficeAgent\pack\office-tools\`. Each runs from this folder
with no home paths and needs no other file from this folder.

`<game>` below is the game project folder; the plugin under test is its copy, `<game>\Plugins\AnomalyInjector`, at
the commit in its `COPIED_COMMIT.txt`. Keep Python from writing `__pycache__` into this folder or into the plugin copy:
set `PYTHONDONTWRITEBYTECODE=1` in the terminal first (PowerShell `$env:PYTHONDONTWRITEBYTECODE = "1"`, Command
Prompt `set PYTHONDONTWRITEBYTECODE=1`), or run each command as `python -B ...`. The variable is the safer of the two:
`m57_make_clip.py` starts `verify_capture.py` itself, and `-B` does not pass on to it.

## What you need
This list is the one place the readers' requirements are named. Rule 5 allows downloading and installing them where
the office PC allows installing software (it forbids sending, not receiving); say in the report what you installed and
its version.
- **Python 3.7 or newer** (tested at home with Python 3.13.1). The four checks marked "standard library" need nothing
  else.
- **Pillow** for `m57_null_pair.py` and `m57_make_clip.py` (tested with Pillow 12.2.0): `python -m pip install Pillow`.
  The plugin's `verify_capture.py` also needs Pillow.
- **ffmpeg** for `m57_make_clip.py`, on the PATH or named with `--ffmpeg <path to ffmpeg.exe>`.
- **The plugin's `verify_capture.py`**: use the one in the game's plugin copy,
  `<game>\Plugins\AnomalyInjector\tools\verify_capture.py`. It is not copied here; it needs its
  `measure_label_offset.py` beside it, as it is in the copy.

## Before you capture
Take every session these checks read with **delivery mode off**: type `IAI.Capture.Delivery 0` in the console before
`IAI.Capture.Start`. With delivery mode on, the plugin writes no `run.json` and no run log (`anomaly_log.txt`), and
then `m56_continuity_check.py` cannot judge settle placement, `m56_settle_content_check.py` cannot run and
`m57_lod_readout.py` cannot run.

## Where the tools write (rule 10)
Everything these tools write about an office session (a `--json` reading, the clip tool's working frame copies, a
clip) goes in the office captures area on this PC: the captures folder that holds the session folders, beside the
sessions and never inside a session folder. Delete the working frame copies once the owner has watched the clips; the
clips may stay. Never write any of it into the pack (`C:\OfficeAgent\pack`, this folder included), the game's plugin
copy, a repo, a synced folder, a network share or removable media.

## First, prove each check can fail
Run each one with `--selftest` from this folder. It builds small fake sessions in a temporary folder, deletes them
afterwards, and must end with `OK`:

| Command | Must end with |
| --- | --- |
| `python m56_continuity_check.py --selftest` | `M56 CONTINUITY SELFTEST: OK - 10 case(s)` |
| `python m56_settle_content_check.py --selftest` | `M56 SETTLE CONTENT SELFTEST: OK - 8 case(s)` |
| `python m56_nonsettle_compare.py --selftest` | `M56 NON-SETTLE SELFTEST: OK - 26 case(s)` |
| `python m57_lod_readout.py --selftest` | `M57-LOD-READOUT SELFTEST: OK - 22 cases` |
| `python m57_null_pair.py --selftest` | `M57-NULL-PAIR SELFTEST: OK - 14 cases` |
| `python m57_make_clip.py --selftest --verify <game>\Plugins\AnomalyInjector\tools\verify_capture.py` | `M57-CLIP SELFTEST: OK - 15 cases` (13 if ffmpeg is not found; it says so) |

## The checks
`<session>` is one `session_<date-time>` folder. None of these writes inside a session folder. `--json <file>` (the
m56 checks) writes the full reading to a file you name.

**`m56_continuity_check.py`** (standard library) - was every engine tick of the capture written as a frame?
- Reads `labels.jsonl`, `run_summary.json`, `run.json` and the `Actual_Frames` file list.
- Prints one line per clause (C1 rows, C2 `tick_index`, C3 tick step, C4 `frame_index` step, C5 `t_wall_run`,
  C6 `run_summary` counts, C7 settle rows where the schedule puts them, C8 frame files), then a readings line
  (`ticks_per_captured_frame`, `frames_dropped`, `capture_span_ticks`, settle rows found and expected).
- PASS: `M56 CONTINUITY: PASS (exit 0)`. FAIL: `M56 CONTINUITY: FAIL (<clauses>) (exit 1)`. A session from the
  release build always fails (it has no `tick_index` and skipped two ticks after every anomaly edge). Exit 3 = no
  failure but a part was not judged (for example no `run.json`); exit 2 = cannot run.
- `python m56_continuity_check.py <session> [--json <file>]`

**`m56_settle_content_check.py`** (standard library) - what the settle frames say.
- Reads `labels.jsonl`, `run.json` (`settle_frames`) and `target_mask`.
- Per event prints how many frames are both labelled and flagged `settle`. Pink and checkered texture corruption,
  `uv_corruption`, `normal_corruption` and `missing_object` must have exactly K (2 at the default settings); blinks
  and the rest are only reported. Every such frame must have `mask_state` `present` or `empty`, and a present mask
  file must exist and not be empty.
- PASS: `M56 SETTLE CONTENT: PASS (exit 0)`. FAIL: `M56 SETTLE CONTENT: FAIL: <event>: <n> labelled settle frame(s),
  want 2 ...` (exit 1). Exit 3 = nothing judged; exit 2 = cannot run (no `labels.jsonl` or no `run.json`).
- `python m56_settle_content_check.py <session> [--json <file>]`

**`m56_nonsettle_compare.py`** (standard library) - is every frame the release build wrote unchanged in the new build?
- Reads `labels.jsonl` and `annotation.json` of a release session and a new-build session. Joins rows by tick offset
  (`frame_index` minus the session's first `frame_index`), not by frame number.
- Only meaningful when both sessions follow **the same camera path and pose** (a scripted or replayed path). On a
  played path the two sessions differ everywhere and this check says so; do not use it there.
- PASS: `M56 NON-SETTLE: PASS (exit 0)`. FAIL: `M56 NON-SETTLE: FAIL: fields differ: ...` (exit 1). `INVALID`
  (exit 3) = the two sessions rest in different camera poses. Exit 2 = cannot run (no `annotation.json`, or an
  annotation event without `event_id`).
- `python m56_nonsettle_compare.py <release session> <new session> [--control <second release session>] [--json <file>]`

**`m57_lod_readout.py`** (standard library) - the `lod_popping` counts in one session.
- Reads `annotation.json`, `run_summary.json` and the run log `anomaly_log.txt` (or `--log <file>`).
- It refuses to run unless you say what you expect:
  - `--expect no_events`: no `lod_popping` event, and the run either armed fires that were all listed in
    `lod_untriggered_event_ids` or logged `NONE CAN BE ARMED NOW [lod_not_armable:<reason>]` refusals. Use it for a
    capture where the player stood still. `--refusal <reason>` also requires every refusal to name that reason.
  - `--expect events`: at least one `lod_popping` event.
  - `--expect-mismatch 0`: `lod_render_mismatch_frames` and `label_transition_lod_unconfirmed_entries` are present
    and 0.
  - `--expect-restored yes`: every `lod_popping: ARMED` log line has its `REVERT` line, and every revert reads
    `RESTORED` (or `NOTHING TO RESTORE` for a destroyed mesh). Needs at least one armed fire.
- Prints a counts line (events, labelled frames, armed, untriggered, mismatch, checked, unconfirmed), one line per
  expectation (`L3`, `L5`, `L6` PASS or FAIL with the reason), then `M57-LOD-READOUT VERDICT PASS|FAIL|CANNOT RUN`
  (exit 0, 1, 3). A release session can never pass it (it has none of these keys).
- Turning or walking capture: `python m57_lod_readout.py --dir <session> --expect events --expect-mismatch 0 --expect-restored yes`
- Standing-still capture: `python m57_lod_readout.py --dir <session> --expect no_events --expect-mismatch 0`

**`m57_null_pair.py`** (Pillow) - does each `lod_popping` label match the pixels?
- Reads two sessions on the **same scripted camera path**: the `lod_popping` session and a twin with no anomaly. Both
  must hold the same frames. Compares their frames inside each event's box and over the whole frame.
- A played path has no twin, so this check does not apply to the owner's played captures; it refuses (exit 3) when
  the frame sets differ, the twin carries anomaly entries, or the session holds another anomaly type.
- Prints per event `L1` (labelled frames equal the differing frames), `L2` (each labelled run ends at a recorded
  swap), `L4b`, `L6`, then `M57-NULL-PAIR VERDICT PASS|FAIL|CANNOT RUN` (exit 0, 1, 3). Run it at the identity
  settings (anti-aliasing off).
- `python m57_null_pair.py --anomaly <lod session> --null <twin session> [--event <event_id>] [--tol 8] [--min-px 0]`

**`m57_make_clip.py`** (Pillow, ffmpeg) - a video of a `lod_popping` capture with its label boxes drawn on.
- Runs the plugin's `verify_capture.py` to draw the boxes (red = labelled, amber = flagged only) on copies of the
  frames, fills the gaps with the raw frames, and encodes at 30 fps with ffmpeg. With `--null <twin>` it puts the
  twin beside it; without `--null` it encodes the capture alone (use this for a played path).
- Writes only under `--out`: `<name>_overlay\`, `<name>_frames\`, and `<name>.mp4`. It refuses (exit 3) if `--out`
  is inside a session folder, a session has no frames or gaps in its frame numbers, the two sessions' frames differ,
  or `--verify` does not name a file. Prints `M57-CLIP WROTE <path> (<n> frames ...)` (exit 0). `--dry-run` prints
  the steps only.
- `--out` is a folder in the office captures area, beside the session folders (rule 10). `<name>_overlay\` and
  `<name>_frames\` are working frame copies: delete them once the owner has watched the clip. `<name>.mp4` may stay.
- `python m57_make_clip.py --verify <game>\Plugins\AnomalyInjector\tools\verify_capture.py --anomaly <session> --out <folder in the captures area, beside the sessions> --name <name> [--null <twin session>] [--ffmpeg <path>]`

## The plugin's own readers, from the game's plugin copy
- `python -B <game>\Plugins\AnomalyInjector\tools\verify_capture.py --dir <session> --label-rule` - are the labels
  consistent with the rule they were written under, settle frames included. Exit 0 no mismatch, 1 mismatch, 3 cannot
  run.
- `python -B <game>\Plugins\AnomalyInjector\tools\verify_capture.py --dir <session> --label-pixel-gate` - does every complete event
  change the pixels where it says. Exit 2 = at least one `NO-TRACE` (a label with no visible change); add
  `--report-only` to get exit 0 with the same report.
