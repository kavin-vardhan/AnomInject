# REPORT for 068-07-bates-m40-validation-card.md
brief ran 20:43-20:48 | model claude-opus-5 | effort high | turns 15 | api-equiv $10.96 (watcher post-processing crashed on a locked costs.csv; report assembled by chat)

---- FINAL MESSAGE (Claude Code) ----
**SHA `2f16bf7`**, pushed. Card `SECTION D` is live; journal §11.1 records the ratification; CLAUDE.md and ledger §8.6a both point at §D as the pending Bates errand. No source, no build, no leg, no tag.

## D-0 — the pass condition, verbatim

> **On the `m40` build, for ONE `blinking` event in the first five bursts:**
> **`annotation.json`'s `frame_indices` MUST EQUAL the set your eye reads — whichever tick order this host has.**

| if the toggle lines read | i.e. | then EXPECT `frame_indices` | and the eye gone at |
|---|---|---|---|
| **Δ = +3** — `blinking: matched` at `[a]`, first `blinking toggle ->` at `[a+3]` | this host still ticks the injector first | **`{n, n+1, n+2, n+6}`** | **`n, n+1, n+2, n+6`** |
| **Δ = +2** — first toggle at `[a+2]` | this host's order also changed | **`{n, n+1, n+5, n+6}`** | **`n, n+1, n+5, n+6`** |

✅ **PASS = labels and eye agree, in EITHER row.** 🎯 **Δ = +3 with agreement is the STRONGEST result** — the disorder is still there and the labels are right anyway. ⚠ **Δ = +2 with agreement is a PASS but a WEAKER one** — that build's order also moved, so it does not exercise the fix; say so, don't report it as the strong result. 🔴 **FAIL = labels ≠ eye, in any row. Report raw and classify nothing.** Two free cross-checks, expected either way: **AMBER on `n − 1`** · **NO box on `n + 7`**.

## D-2 — the exact lines to hand the owner

Prerequisites: fresh editor session · AA + motion blur OFF via the title's menu · `blinking` **ticked** · anchor on bursts 1–5 (windows `4-11` · `16-23` · `28-35` · `40-47` · `52-59`).

Editor console, in this order:

```
Log LogAnomaly Verbose
IAI.Capture.RunLogVerbose 1
IAI.Capture.Config 2 4 8 4 0
IAI.Capture.Start "" png 4242 90 blinking
```

After the run:

```
$r   = "<the session directory just written>"
$log = "$r\anomaly_log.txt"
```

The `C-3` commands **(a)–(d) then work unchanged** with those two variables set — the `m38` run log lives inside the session folder, so there's no editor-log hunting and no rotation risk. Fallback if `anomaly_log.txt` is absent: point `$log` at the host project's own `Saved\Logs\<its>.log` as `C-3` did and run the reads before relaunching; its absence is a **reading**, not a failure — grep `Capture(runlog)` and copy that line.

Box update first (D-1): `git -C <plugin-path> status --short` (expect 0 lines or only `??`) → `git -C <plugin-path> pull --ff-only` then `git -C <plugin-path> log --oneline -1`, **expected `0864e7a feat(capture): m40 …` or later** → disk check (≥15 GB GO) → `Build.bat StackOBotEditor Win64 Development … -waitmutex`, **exit 0**. That pull brings **m37 + m38 + m40 to the editor build only; the packaged build is untouched.**

🚨 **D-4: `IAI.Bench.SynthTickOrder` is NEVER typed on Bates, not even `0`** — it would synthesise the symptom on top of whatever the host really does. The `StartRun` line should read `SYNTH TICK ORDER = off`; copy it. **`blinking` stays unticked on every other Bates run until D-3 reads YES.**

D-5 (optional, not a gate) is the m37 ceiling read with `blinking` unticked.