# 091-08 — release readme limitations

Owner-approved documentation follow-up on `release/2026-10-05`, starting clean at `23098fa7415f148e935acbd4a97e18da04e6c46c`.

Added section 7's three client notes: capture with one live game view, the two rare UV/normal edge cases, and the FXAA launch/console/project-setting tip. Section 8.7 links to the tip; existing text, tables, schema descriptions and numbers are otherwise unchanged. CLAUDE.md gains one release-block pointer.

Verification: review the full diff and whitespace, require exactly these three documentation files, and require an empty diff for `Source Shaders tools Config *.uplugin` against `23098fa`. No source, shader, tool, test or config edits; no build, game/editor launch, binary/archive work or bench-setting changes. The brief's docs-only scope supersedes general build and source-maintenance steps. No problems or deviations.

Hand-off: commit and fast-forward-push only `release/2026-10-05`, then verify the remote head. Final commit, checks and push receipt: `D:\IntrusiveAnomalies\_relay\runs\2026-10-04-091-08-release-readme-limits\report.md`. No decisions needed.
