# Office capture card draft — 094-43

Draft pending Stage B qualification and Code's scan. This is not a release instruction yet.

R1 must be read from the clone from which the plugin folder was copied: run `git -C "<source clone>" rev-parse HEAD` there and record that full hash. Do not read R1 from the game's repository or infer it from a copied folder.

Keep the copied plugin folder out of the game's repository. Copy it to the host Plugins directory as an untracked local deployment; do not add, commit or vendor it into the game's history.

Code's final card must supply the concrete engine executable, project, map and output directory. Run that launch line exactly as written, including the RHI, dimensions and capture arguments; do not substitute an older card or silently remove flags. A draft placeholder is deliberately not a runnable launch line.

Default routes are blinking and lod_corruption. Lighting and missing-object routes require an explicit validated list. Capture uses the player's view and plugin-owned ID/depth resources. Record the self-check size, RHI, client and unavailable counts. If the private path is unavailable and the host has a custom-depth reader or an unknown result, capture refuses rather than disabling the host effect.
