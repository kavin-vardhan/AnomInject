# REPORT for 094-41-europa-office-findings-plan.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 094-41-europa-office-findings-plan.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-11-02 22:45:47 | finished: 2026-11-02 23:36:16 | duration: 50 min | exit code: 0
context at last turn: 600012 tokens (60% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\094-41-europa-office-findings-plan.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN. Codex's plan is complete, names a recommended option, and I agree with both its diagnosis and its recommendation. It has one unsized risk, which I flag below.

**For the owner, in brief:**
1. **What causes the outlines:** the 4.25 capture switches on the game's own "custom depth" highlighting for the objects it measures. That covers the census batches, the anomaly targets, and three objects the capture sets up itself. Europa's outline effect draws around all of them, on the very frames we save.
2. **Why it matters:** the outline sits exactly on the labelled object, so the office dataset leaks its labels and isn't clean training data. A missing object even gets drawn as an outline.
3. **The fix:** draw our measurements into our own private image and never touch the game's custom depth. It also refuses to make labelled captures in the old unsafe way.

**How the run went:** the brief asked me to resume Codex thread `01a14334` without `-OwnerApprovedFullAccess`. The relay's resume script refuses every resume without that flag (exit 7, nothing written), so I tried twice, as the brief describes, and then used its fallback. A fresh thread (`01a19d4a`) ran on the review runner, which can write only inside its own folder. It exited 0 after 2,030 s. The plan is in `_reviews\094-41-codex-plan.md` (byte-identical) and my review is in `_reviews\094-41-code-review.md`.

**Diagnosis: confirmed, and label leakage confirmed.**
- **Every citation holds:** all 90 cited source ranges contain what Codex says they do.
- **Four things set custom depth during the capture:**
  - the forced `r.CustomDepth 3`;
  - census batches of about 23 actors, two at a time, changing every frame;
  - event targets, tagged for the whole live fire (for lighting, every object the light reaches);
  - one I had missed: `IAI.Capture.C5` always runs the street bench, which plants custom depth on the first three objects by name (203, 18, 19). At the office those were real game objects.
- **Label leakage is real:** the tagging runs inside the request for the same frame whose colour is saved. Custom depth is drawn before post-process, and that frame's colour is then copied.
- **Hidden objects are the worst case:** missing_object and blinking keep custom depth while hidden, so the outline shows exactly where the "missing" object was.
- **Two of my own records were wrong:**
  - The "host-owned 203 / EA_BlinkCube" items in my notes are actually planted by C5 (the receipt marks them `bench_setup:true`).
  - Setting the custom-depth flag on 4.25 updates the existing proxy directly. Only a write-mask change rebuilds it.

**The options:**
- **Recommended, a private ID/depth pass:** buildable on stock 4.25 without engine edits, since the needed engine functions are exported, and the same code serves D3D11 and D3D12.
  - ⚠ **The unsized risk:** in an editor build, adding a new material shader type changes each affected material's shader-map ID. Europa's editor would then recompile those materials' full shader maps on first launch, and C5 waits only 120 s for warmup.
  - I suggest first checking whether the engine's existing depth-only shaders, which are already compiled for every material, can drive the private pass instead.
- **A runner-up Codex missed:** draw custom depth early, measure right after the base pass, then erase our values before translucency and post-process. It's cheap and needs no recompiles, but it leaks at overlaps and decals, and forces two cvars.
- **Detect-and-refuse:** I agree. The 5.1 host-reader scanner is compiled out on 4.25 and should be ported. A "no readers found" result can't rule out an outline pass built into Europa's fork.

**The home repro:** a separate fixture project with its own outline post-process material leaves the Paris host's Content and Config alone. It needs to be a C++ project, its first build is a full build rather than an incremental one, and it belongs on E:. Today's home fixture can't show outlines at all: its post-process volume has 0 blendables.

**The smaller findings, all agreed:**
- **Resolution:** the size is latched at start. Arbitrary sizes need pitch-aware copies for both colour and masks; the mask copy currently assumes tight rows. 1920 happens to be aligned, which fits the office's 0 dropped.
- **D3D12:** run the home check on `-d3d12`, including an odd width.
- **-ExecCmds:** on stock 4.25 a quoted list runs every command. I add one step Codex didn't cite: the Windows launcher puts the quotes back after parsing.
- **client:** a reading, not pass/fail.

**Safety:** none.
- Codex ran 45 commands, none flagged, and wrote only `report.md` in its own folder.
- No editor, game or build process ran.
- The Paris host's Content, Config, Binaries, descriptor, `.uproject` and Intermediate listings are all unchanged.
- `m51` equals P9 at every check, and all 74 git refs are unchanged.
- Nothing was deleted or killed.
- A foreign load (ComfyUI plus another session's renders) briefly pushed free commit to 1.2 GiB and D: to 2.5 GiB, because the page file on D: grew. Neither was ours.

**Decisions for chat:**
1. **The relay can't resume read-only.** Either accept fresh threads for read-only jobs, or authorize a resume variant without full access.
2. **The shader recompile cost:** size it, or test the depth-only-shader route, before the private pass is implemented.
3. **An interim guard before any further office capture:** remove the street bench's planted tags and refuse labelled capture on hosts that read custom depth. In the current plan this comes third.
4. **The office dataset:** tell the owner it isn't clean training data and must not be used.
5. **5.1:** the same tagging exists there. Decide whether to check the 5.1 hosts' custom-depth reader readings.
api-equivalent cost (not billed on Max): $15.56 | turns: 113 | running total across all briefs: $5,500.90
