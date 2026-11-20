VERDICT: GREEN

Pushed `release/2026-10-05` commit `d7eee97731812384d4fd00af9032254923363ada` with the three approved client notes and cross-reference.
Code is identical to `23098fa7415f148e935acbd4a97e18da04e6c46c`.

Only commit pushed: `release/2026-10-05` / `d7eee97731812384d4fd00af9032254923363ada`.
Commit message: `docs(readme): release known limitations and anti-aliasing tip`.
Repository co-author trailer: `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`.

Start: correct branch, clean worktree, local and remote at `23098fa7415f148e935acbd4a97e18da04e6c46c`. New commit's parent is exactly that base. Push was fast-forward, explicitly scoped to the release branch with tag following disabled. Afterwards `git ls-remote --heads origin refs/heads/release/2026-10-05` returned:

```text
d7eee97731812384d4fd00af9032254923363ada	refs/heads/release/2026-10-05
```

Full diff:

```diff
diff --git a/CLAUDE.md b/CLAUDE.md
index 75cf7ee..6f22c02 100644
--- a/CLAUDE.md
+++ b/CLAUDE.md
@@ -17,6 +17,7 @@ and is the single source of truth for the project.
 > overrides retain their existing behavior. All other production source is unchanged, including labels,
 > capture, anomalies, non-temporal transition reasons and schema; no 091-05 or 091-06 changes.
 > Journal: `docs/sessions/2026-10-04-091-07-release-transitions-off.md`.
+> 091-08: client-readme limitations and FXAA tip; code unchanged from `23098fa`. Journal: `docs/sessions/2026-10-04-091-08-release-readme-limits.md`.
 
 > **091-03, completed 2026-10-04 — REQUIRED FINAL PROOF PASSES; RELEASE READY (GREEN).**
 > Schema 2.1 exports each event's exact transition indices/reasons and the session union. Drop these frames
diff --git a/docs/client-readme.md b/docs/client-readme.md
index eb5ac14..f70b426 100644
--- a/docs/client-readme.md
+++ b/docs/client-readme.md
@@ -454,6 +454,12 @@ Your machine renders the game at some frame rate ("native fps") — check it in
 * **An anomaly seems stuck on screen** — press **Revert all** in the dashboard (or run `IAI.RevertAll` in the console).
 * **Nothing captures / commands not recognized** — confirm you're running the provided build (capture features are included in this build).
 
+### Known limitations of this build
+
+* **Capture with one live game view.** Run the game in a new window (**Play → New Editor Window**) or as the packaged game, and close or pause other realtime editor viewports while capturing. With several realtime views open in the editor, a capture can occasionally record a frame from an editor view instead of the game view and label it as the game. Packaged and standalone games have one view and are not affected.
+* **Two rare edge cases for UV/normal corruption.** When the plugin cannot tell which parts of an object it changed, the label box and target mask cover the whole object even if only some parts look different. A texture that varies only in a channel the material does not use can also be swapped with no visible change; the event is still labelled. Both are rare. The labelled preview images (or the overlay inspector) make them easy to spot.
+* **Anti-aliasing tip.** With temporal anti-aliasing (TAA/TSR), the picture can trail the label by a frame or two at an event's start and end (see §8.7). To avoid it, use FXAA: add `-ExecCmds="r.AntiAliasingMethod 1"` to the game's launch command (merge it into an existing `-ExecCmds` with a comma, since only one is used), or type `r.AntiAliasingMethod 1` in the console before capturing. Type `r.AntiAliasingMethod` alone afterwards to check it reads **1**; some games reset it from their own graphics menu. **Project Settings → Rendering → Anti-Aliasing Method → FXAA** makes it permanent for a project.
+
 ### ffmpeg didn't download
 
 On locked-down corporate networks the automatic ffmpeg download can be blocked outright — even the revocation-check retry that `Setup.bat` performs. This only stops the **video** step: your captures still record fine (frames, labels, `annotation.json`), and they encode to MP4 later once ffmpeg is in place. To install it by hand:
@@ -1090,7 +1096,7 @@ touch the slab, the plugin traces a grid of rays **inside the slab only** agains
 
 ### 8.7 The per-frame label, anomaly by anomaly — and how to build a training label from it
 
-Under temporal anti-aliasing (TAA/TSR) the picture can trail the label by a few frames at an event's start and end; those frames are NOT flagged by default. Optional: set IAI.Label.TransitionOffFrames / OnFrames / HideFrames to flag them.
+Under temporal anti-aliasing (TAA/TSR) the picture can trail the label by a few frames at an event's start and end; those frames are NOT flagged by default. Optional: set IAI.Label.TransitionOffFrames / OnFrames / HideFrames to flag them. See the [anti-aliasing tip in section 7](#known-limitations-of-this-build) to use FXAA.
 
 **Exclude declared uncertain frames.** In schema 2.1, drop the union of all events' `transition_frames`
 before assigning positives or negatives: **never use as negatives, never as positives**. `transition_reasons` explains the entries counted;
diff --git a/docs/sessions/2026-10-04-091-08-release-readme-limits.md b/docs/sessions/2026-10-04-091-08-release-readme-limits.md
new file mode 100644
index 0000000..ee0d7b6
--- /dev/null
+++ b/docs/sessions/2026-10-04-091-08-release-readme-limits.md
@@ -0,0 +1,9 @@
+# 091-08 — release readme limitations
+
+Owner-approved documentation follow-up on `release/2026-10-05`, starting clean at `23098fa7415f148e935acbd4a97e18da04e6c46c`.
+
+Added section 7's three client notes: capture with one live game view, the two rare UV/normal edge cases, and the FXAA launch/console/project-setting tip. Section 8.7 links to the tip; existing text, tables, schema descriptions and numbers are otherwise unchanged. CLAUDE.md gains one release-block pointer.
+
+Verification: review the full diff and whitespace, require exactly these three documentation files, and require an empty diff for `Source Shaders tools Config *.uplugin` against `23098fa`. No source, shader, tool, test or config edits; no build, game/editor launch, binary/archive work or bench-setting changes. The brief's docs-only scope supersedes general build and source-maintenance steps. No problems or deviations.
+
+Hand-off: commit and fast-forward-push only `release/2026-10-05`, then verify the remote head. Final commit, checks and push receipt: `D:\IntrusiveAnomalies\_relay\runs\2026-10-04-091-08-release-readme-limits\report.md`. No decisions needed.
```

`git diff --stat 23098fa..HEAD`:

```text
 CLAUDE.md                                                | 1 +
 docs/client-readme.md                                    | 8 +++++++-
 docs/sessions/2026-10-04-091-08-release-readme-limits.md | 9 +++++++++
 3 files changed, 17 insertions(+), 1 deletion(-)
```

`git diff 23098fa..HEAD -- Source Shaders tools Config *.uplugin`: empty.
`git diff --check 23098fa..HEAD`: passed; no output.
Final `git status --porcelain=v1`: clean; no output.

No source, shader, tool, test or config changes. No build, tests, game or editor launch. EXE/DLL hashes and archive paths: none produced or changed. Bench levers/settings changed or restored: none. No other branches or tags pushed. Deviations: none; the explicit docs-only brief supersedes general source-maintenance/build steps.

NEEDS-DECISION: none.

VERDICT: GREEN
