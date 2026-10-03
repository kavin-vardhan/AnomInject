# Office label-sync check (one page)

**What it does:** it reads your capture folders and prints **numbers only**: for each anomaly, how many
frames the labels are off from the picture where the anomaly starts and where it ends. **0 means in sync.**
It changes nothing, and it prints no folder, object or frame names, so the result can be read aloud.

## 1. Capture (about 5 minutes)
1. Start the game and the dashboard as usual (README, Steps 1 and 2).
2. In the game, open the console (the **`** key under Esc), type this line, press Enter, close the console:

       IAI.Capture.Config 2 40 8 30 0

   (It gives 40 quiet frames before the first anomaly and 30 quiet frames after each one.)
3. In the dashboard: **Auto-pool**. In **Capture pool**, tick every anomaly **except `camera_clipping`**.
   Leave format **PNG** and size **native**. Set **frames** to **3000**.
4. Press **Start capture**, click once into the game window, then **take your hands off the keyboard and
   mouse until the capture stops** (about 2 minutes). The camera must not move.

## 2. Run the check
Open a Command Prompt in the delivery folder and run these two lines, one after the other:

    python host-tools\label_sync_check.py --selftest
    python host-tools\label_sync_check.py "<your captures folder>\<the new session folder>" --out numbers.txt

- The selftest takes about 1 to 2 minutes. Its **last line must say `OK`**. If it says `FAILED`, stop and read
  that line back.
- The check takes **about 5 to 10 minutes** for this capture when Pillow is installed, and **about 30 to 45
  minutes** without it (measured on our PC: 56 anomalies in 3 minutes with Pillow, 14 minutes without). The
  `decoder:` line says which one it used. It prints nothing until it finishes; leave the window open.
- You can give it several session folders, or the whole captures folder, in one run.

## 3. Read back
Read the lines under **READ BACK**, one per anomaly, for example:

    blinking           judged 12 | start {+0:24} | end {+0:24} | wrong-object 0 (upper bound 2) | censored 1 | fail 0 | interrupted 0 | nanite 0 | unjudged 0 | unpaired 0

For each anomaly read: **judged**, **start**, **end**, **wrong-object** (both numbers), **censored**, **fail**,
**interrupted**, **nanite**, **unjudged**, **unpaired**. Also read the **`sessions read`** line, the
**`unpaired frames`** line and the **`decoder:`** line. That is all; `numbers.txt` holds the same text.

**`interrupted`** counts anomalies the game removed partway through (for example by swapping the material back);
their labels stop at that frame and are checked there, so it is not a failure by itself.

**`nanite`** counts anomalies whose object turned out to draw Nanite partway through, so the plugin stopped
labelling it and removed the effect; those frames are left out of the check, so it is not a failure by itself.

**`unjudged`** counts anomalies the kit could not fully judge (for example a frame image is missing, unreadable or
damaged, or a line of `labels.jsonl` is missing); one that also shows a failure counts under **fail** instead. It is
**not a pass**: copy the session folder again and re-run the check, and if it is still above 0, read it back.

**`unpaired`** counts anomalies beside a frame written on the sync capture path, whose picture is the previous frame
and not the one its label describes; the kit drops that frame and reads the label edge beside it as `censored`. It
is a dropped frame, not a failure of the label. It should be 0 with the shipped settings; if it is not, read it
back. A session made only of such frames is not judged at all: it is listed under `refused` as
`sync-path capture: unsupported for delivery`.

**`censored`** is not proof of correct timing: a one-frame interruption between two labelled runs (or another gap
too short to measure) reads `censored`, not failed.

Then, in the plugin folder, run `python tools\m52_log_counts.py "<the game's log>"` and read back every line it
prints (numbers only).

## If something looks odd
- **`judged 0`** for an anomaly: it fired too rarely, or the camera moved. Capture once more (Step 1), then
  give the check both session folders together.
- **Many `confounded reference`** events: the `IAI.Capture.Config` line was not applied, so the anomalies came
  too close together to measure. Type it again (Step 1.2) and capture again.
- **`camera_clipping`** always says **not judgeable by this kit**. That is expected.
- **`refused`** on the `sessions read` line: the frames were JPEG or the labels were switched off. Capture
  again with PNG. If it says **`sync-path capture`**, the capture ran on the unsupported sync path (for example
  `IAI.Capture.Async 0` was typed); restart the game so the default settings return, and capture again.
- **`annotation transitions mismatch`** means the settling-frame lists in `annotation.json` disagree with
  `labels.jsonl`. Keep both original files together and read back this refusal; the kit does not judge that session.
  Kit 1.5 checks schema 2.1 sessions automatically. Keep `verify_capture.py` beside `label_sync_check.py` in host-tools.
- In PIE, the kit drops the first settling frame after a fire-window event, including a recorded proxy-route blur.
  Later temporal-AA frames still need their own flags and measured decay; the PIE flag alone never excuses them.

## If an anomaly never shows: the one-command read-out (090-10b)

It prints **numbers only** (no object, material, map or path name), so the result can be read aloud.

1. Run the game (or Play in the editor) with the anomalies ticked in the dashboard for a minute or two, then stop.
2. Open a Command Prompt in the project folder and run:

       python Plugins\AnomalyInjector\tools\anomaly_refusal_counts.py --selftest
       python Plugins\AnomalyInjector\tools\anomaly_refusal_counts.py "Saved\Logs\<project name>.log"

   (The selftest's last line must say `OK`. For a packaged game the log is in the game's own `Saved\Logs` folder.)
3. Read out every line. The ones that matter most: `STARTUP` (editor or not, Nanite probe registered or MISSING,
   r.VirtualTextures), `ZERO-ELIGIBLE` (why a type found nothing it could change), `UV_CORRUPTION` / `NORMAL_CORRUPTION`
   (applied and refused per reason), `TEXCORRUPT-SHADERMAP` and the stuck_low_mip block.
   From 090-10b2 (m53 builds) the `TEXCORRUPT-SHADERMAP` count is split by verdict: `admitted`, `refused:compile_pending` (its shaders
   are still compiling; it is tried again), `refused:no_vertex_factory_shaders` / `no_base_pass_vs` / `no_base_pass_ps` (the census reason
   `draw_shaders_missing`); a census reason `default_material_path` means the game draws the default material on that mesh.
