# SESSION: new-default
# NAME: 072 lyra-fixture
# EFFORT: xhigh
# MODEL: claude-opus-5

# 072-01 — Second bench fixture: mount AnomalyInjector in Lyra (UE 5.1, same source build), editor build, one auto-pool smoke session, verifier + readouts
Fresh session. COLD START FIRST: read `CLAUDE.md` status block, then the "COLD START FOR 072" block at the top of `docs/sessions/2026-09-04-071-concorde-feedback-triage.md` and the reading order it names. Summarise the state in 5 lines before doing anything.
Headless; nobody answers; FINAL MESSAGE structure in §6; a failed build or a real fork = report and STOP with NEEDS-DECISION. Codename discipline: Concorde / Bates / Europa only. `LyraStarterGame` is the owner's own Epic sample project and needs no codename.

## 0. Why (so the report is judged against it)
Every desync the client found was invisible on StackOBot. Lyra is the host-shaped fixture: held weapons on skeletal meshes, a fast first-person camera so targets leave the frame mid-event, bots that move, foliage and fog cards in its maps, dozens of plugins/subsystems. The purpose of this brief is NOT to pass — it is to make the verifier and the m26/mask readouts SHOW the client's shapes (F4 target leaves frame, F5/F7 not-visible, F8 game retakes the material, translucent-only picks) on a real game BEFORE Phase A fixes them. Findings are the deliverable. Do not "fix" anything you find in plugin code here.

## 1. Audit first (read-only, 10 min cap)
`E:\UEProjects\LyraStarterGame`: `.uproject` EngineAssociation and Modules (it is a C++ project); existing `Plugins\`; disk free on E:; any project setting or material that reads custom depth/stencil (the plugin reserves stencil 200–254; the m41 StartRun preflight will also report HOST-PP CUSTOM-DEPTH READERS — quote it later); the project's AA method and whether Nanite is enabled (G134: Nanite geometry is invisible to the mask instrument — count how much of the chosen map is Nanite); which maps have moving bots. Record all of it.

## 2. Mount — a git worktree, NOT a junction, and why
The 4.25 host uses a junction because its build outputs cannot collide with 5.1's. Lyra is the SAME engine: a junction would make StackOBot and Lyra write the same `Binaries\` and `Intermediate\` inside the one plugin folder and could silently invalidate the bench's byte-identity anchors (P-C7). So: `git worktree add E:\UEProjects\LyraStarterGame\Plugins\AnomalyInjector master` (detached or a tracking `lyra-fixture` branch that is NEVER committed to — state which). Rule refinement (chat, 2026-09-04): bench/delivery binaries still come ONLY from the main checkout; the Lyra worktree may build the Lyra fixture and nothing else; it is refreshed by `git pull --ff-only`/checkout, never edited. Record it in the journal and CLAUDE.md. If `.gitignore` does not already cover the plugin's own `Binaries/` and `Intermediate/`, STOP and report.
Back up `LyraStarterGame.uproject` to `.uproject.bak`, then add the plugin to `Plugins` (Enabled true). Do not enable anything else.

## 3. Build
Development Editor for LyraStarterGame via the engine's `Build.bat` (the same toolchain as StackOBot). If the plugin fails to compile against Lyra, quote the first error and STOP — it is game-agnostic and must not need host edits. Also confirm the StackOBot staged exe DE65F84A and the main checkout are untouched afterwards (hash + `git status`).

## 4. Smoke session
Reuse the CaptureBench leg mechanism against the Lyra EDITOR target run `-game` (windowed, 1920x1080, AA and screen percentage at Lyra's defaults — do NOT pin AA off; this is a host-shaped run, not an arbiter leg). Pick the map with the most moving bots and some foliage/fog (say which and why). Auto pool ON with the delivered default pool, delivery mode OFF, run log ON, census+mask+target mask at compiled defaults, `IAI.Capture.Config` at the shipped schedule, 90 frames if a leg fits, otherwise the longest that does. If the pawn/camera needs input to move, use whatever the harness already supports (a scripted look/move, or bots only) and say what the camera did. Bank under `_bench_sessions_bank\LYRA_SMOKE_01`.

## 5. Readouts (this is the deliverable)
1. `verify_capture.py --label-pixel-gate --report-only` on the session: every event line; counts of PASS / SHIFT / NOT-VISIBLE / NOT-MEASURABLE and the reasons.
2. m26/mask summary: `vetoed_events`, VETOED-OBJECT lines, `mask_state` histogram, `target_mask_frames_*`, MASK-TIE.
3. Census: the StartRun preflight line (HOST-PP CUSTOM-DEPTH READERS), candidates, translucent-only exclusions, Nanite-skipped count, `aboveCeiling`.
4. Per event: target class, component class, was it skeletal / Nanite / translucent-only / foliage / a blueprint with its own material logic; did the target leave the frame mid-event (bbox_valid false on any positive frame); any "the game replaced it after apply" revert line.
5. Throughput: speed_ratio and game_clock_speed_ratio; frames_exposure_dip; shaders_pending count; black-frame gate.
6. A short table mapping each client finding (F1–F8) to "SEEN on Lyra / NOT SEEN / CANNOT SEE (why)".

## 6. Final message
1. Plain-language rundown (10 lines).
2. Audit table (§1).
3. Mount + build result, hashes, main-checkout untouched proof.
4. Session summary + the §5 readouts, every event line quoted.
5. The F1–F8 mapping table.
6. What Phase A must therefore prove on Lyra (gate proposals, one line each).
7. Commits (docs only: journal `docs/sessions/2026-09-04-072-lyra-fixture.md`, CLAUDE.md rule refinement, gotchas if any), SHAs, checkout states of BOTH trees.
8. NEEDS-DECISION if any.
