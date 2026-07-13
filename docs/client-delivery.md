# Shipping a client build — delivery mode (m12)

This is the owner handoff for shipping AnomalyInjector to an external client who runs capture in their
own build. There is no post-processing step between the client's capture and the client — **whatever
capture writes to disk IS what the client receives.** Delivery mode limits a run's output to only the
client-facing files.

## What delivery mode does

`IAI.Capture.Delivery 1` (or the packaged default below) makes each capture run write **only**:

```
<session>/
  Actual_Frames/frame_%05d.png    the image sequence
  Video_Clip/<session>.mp4         the encoded video (produced host-side by encode_watcher.py)
  run_summary.json                 run counts + video-timing + delivery_mode flag (the encoder's done-signal)
  annotation.json                  the labeled ground truth (client's primary artifact)
```

and **suppresses** (never writes):

```
  labels.jsonl    per-frame label sidecar (internal / QA)
  run.json        run manifest — and this is where the SEED lives
```

The label ground truth is still fully **computed** in delivery mode (annotation.json is complete) — it
is simply not written to the per-frame sidecar. Default is OFF (full fidelity); OFF output is identical
to normal capture except that annotation.json no longer carries the internal `schema_version` /
`source_id` tags (removed in both modes as of m12).

## Content clock (m15) — default is wall (client titles are wall-clock, RESOLVED)

The shipped default is `IAI.Capture.ContentClock wall`. Owner-tested wall vs game on the actual office
machine (m15, 2026-07-13): the client titles (Until Dawn, Concorde) are **wall-clock** — wall produces
correct-SPEED videos for them. Their video LENGTH varies with the real capture duration, which is
correct for wall-clock content (natural playback speed is the criterion). This RESOLVES the m14
open question (it briefly shipped as game pending this test).

**Do NOT set `ContentClockDefault=game` in a CLIENT build** — for wall-clock titles that stamps a slow
run at the target and plays their videos ~2× FAST (the Issue-2 regression). Client build = wall default,
no action needed. `game` is only for game-clock content like StackOBot, set in that build's own ini.
See `capture-fps.md` for the full model.

## How to set it before packaging a client build

Add to the **project's** `Config/DefaultGame.ini`:

```
[AnomalyCapture]
bDeliveryModeDefault=True
```

The subsystem reads this at startup, so a packaged Development build ships in delivery mode with no
editor and no console command. Note: GConfig caches the ini at startup — if you edit it while the editor
is open, restart the editor for it to take effect.

At runtime, `IAI.Capture.Delivery <0|1>` overrides the default for the current session (mid-run changes
are refused; stop first). `IAI.Capture.Status` shows the current `delivery=` state.

## What the client receives — and does not

- Receives: the frames, the mp4, the run summary, and the full labeled annotation.json.
- Does NOT receive: the per-frame labels.jsonl, the run manifest, or the **seed**. Because the seed lives
  only in run.json (suppressed), **a delivered session is not client-reproducible** — this is intentional;
  reproduction metadata stays owner-side. If you need a reproducible archive of a delivered run, capture it
  once in delivery-OFF mode for yourself (that keeps run.json + the seed), or read the seed from the
  "Capture run STARTED" log line (logged in both modes).

## Our QA tooling and delivery sessions

`host-tools/overlay_watcher.py` and `tools/verify_capture.py` read labels.jsonl and therefore **do not
process delivery sessions** (by design). Verify capture correctness in the default (delivery-off) mode;
ship in delivery mode. `host-tools/encode_watcher.py` is unaffected — it keys off run_summary.json +
annotation.json only, so the client's mp4 still encodes from a delivery session.

## run_summary.json fields (shipped in delivery mode — reviewed client-safe)

`type`, `schema_version`, `total_frames`, `positive_frames`, `bursts_done`, `zero_match_bursts`,
`end_frame` (raw engine frame counter), `target_fps`, `sustained_wall_fps`, `speed_ratio`, `stamped_fps`,
`paced`, `delivery_mode`. No seed; nothing owner-sensitive.
