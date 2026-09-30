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

- The selftest takes about 1 minute. Its **last line must say `OK`**. If it says `FAILED`, stop and read
  that line back.
- The check takes **about 5 to 10 minutes** for this capture when Pillow is installed, and **about 30 to 45
  minutes** without it (measured on our PC: 56 anomalies in 3 minutes with Pillow, 14 minutes without). The
  `decoder:` line says which one it used. It prints nothing until it finishes; leave the window open.
- You can give it several session folders, or the whole captures folder, in one run.

## 3. Read back
Read the lines under **READ BACK**, one per anomaly, for example:

    blinking           judged 12 | start {+0:24} | end {+0:24} | wrong-object 0 (upper bound 2) | censored 1 | fail 0 | interrupted 0

For each anomaly read: **judged**, **start**, **end**, **wrong-object** (both numbers), **censored**, **fail**,
**interrupted**. Also read the **`sessions read`** line and the **`decoder:`** line. That is all; `numbers.txt`
holds the same text.

**`interrupted`** counts anomalies the game removed partway through (for example by swapping the material back);
their labels stop at that frame and are checked there, so it is not a failure by itself.

Then, in the plugin folder, run `python tools\m52_log_counts.py "<the game's log>"` and read back every line it
prints (numbers only).

## If something looks odd
- **`judged 0`** for an anomaly: it fired too rarely, or the camera moved. Capture once more (Step 1), then
  give the check both session folders together.
- **Many `confounded reference`** events: the `IAI.Capture.Config` line was not applied, so the anomalies came
  too close together to measure. Type it again (Step 1.2) and capture again.
- **`camera_clipping`** always says **not judgeable by this kit**. That is expected.
- **`refused`** on the `sessions read` line: the frames were JPEG or the labels were switched off. Capture
  again with PNG.
