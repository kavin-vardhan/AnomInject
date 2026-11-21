# Chat handoff: Europa (UE 4.25) port. FZ2 and the Phase B textures done and pushed (`c5d0919`); camera_clipping queued (094-46); paused for travel

**Written:** 2026-11-04, about 19:35 IST, by chat Claude (orchestrator), before the owner shut the laptop down at 20:45 to travel.
**Supersedes:** `CHAT-HANDOFF-094-europa-42c-running.md`.
**Codenames only:** Europa, Concorde, Bates. Never write a real title.

> 🔴 **Don't save this file into the plugin's `docs\` folder while a mailbox job runs.** The watcher runs Code inside the protected `m51` checkout, and a new untracked file there trips the P9 check. Keep it in the project and in `_reviews\`.

## 1. State right now
- **No job is in flight.** The mailbox is idle. 094-45r finished at 19:22 IST on 11-04 and is ruled (`094-45r-ruling.md`, GN1–GN5).
- **origin `feat/europa-port` = `c5d0919`** (pushed 18:22 on 11-04). It holds:
  - FZ2's private ID/depth pass;
  - FE3b;
  - the 4.25 materials;
  - the GF4 fixture targets;
  - **missing_texture and corrupted_texture, in the default pool** (blinking, lod_corruption, missing_texture, corrupted_texture).
- **Local `c706b7c`** = the camera_clipping port on top of `c5d0919`, not pushed.
  - Its D4 behaviour is right: 3 spans labelled exactly, the baseline-near can-fail labels 0, and the near plane is restored.
  - It failed only on "unlabelled frames match": 1–3 px of ±3 levels, which is session noise.
- **The office card to use: `_reviews\094-45r-office-card-europa.md`** (commit `c5d0919`). It replaces 094-43's.
- **The next brief is ready but not staged:** `094-46-europa-cameraclip-finish.QUEUED.md`, in the project and in `_reviews\`.
  - Stage it once the owner is back and has run `start_watcher.bat`.
  - Rename it to `094-46-europa-cameraclip-finish.md` on staging (`.part`, sha256, `mv -n`).
  - Its ACK-VERDICT line already resolves 094-45r.

## 2. Done this session (11-03 → 11-04)
- **094-42c GREEN:**
  - P9 v3 (BankOverflow retired);
  - the outline leak reproduced at home on the `Outline425` fixture, including the outline of a hidden "missing" object;
  - the guard refuses;
  - a direct `StaticType` link to the depth-only shaders fails.
- **094-43a:** finding the depth-only shaders by name links, with no recompile and no custom depth. Masked and WPO objects were one frame early (occlusion admission). Fix GD3: a scene-depth admission test.
- **094-43r GREEN, pushed `93613ae`: FZ2 is done.**
  - the private ID/depth pass, with 0 new magenta and labels identical to the legacy path;
  - Paris passes on D3D11, D3D12 and 1279×719;
  - the office card is `094-43-office-card-europa.md`.
- **094-44 → 44f, Phase B textures:**
  - 4.25 materials authored (`Content/Engine425/Materials/`) and matching `d7eee97`;
  - FE3b catches build-tool children at creation;
  - GF4: skeletal and instanced targets live at IoU 1.0;
  - on the fixture: G1–G5 and G4 pass, with IoU 1.0 and 0 spill;
  - on Paris D3D11: H4, G1, G2 and G5 pass, but G3 failed three times on bench-scene issues (overexposure and bloom, then the `-game` camera view, then surface that can't change).
  - pushed `8df75de`.
- **094-45:** GL3 (an owned stop is never counted as a crash) and the EA_Core view lock are accepted.
- **094-45r Stage 1 GREEN, pushed `c5d0919`:** strict whole-frame IoU 1.000 for both textures on all three Paris legs, 0 spill, clean edges. Bench at −1.875 EV with a mid-blue base material and six fills.
- **094-45r Stage 2:** camera_clipping ported (`c706b7c`); D4 needs GN2's noise floor (§4).

## 3. Decisions locked this session (why included)
- **FZ2's route is GD2:** the depth-only shaders found by name through public chains, plus GD3's scene-depth admission. *Why:* no new shader type means no first-launch recompile at the office. The fallback type is retired.
- **GD5's scope:** static, ISM/HISM and skeletal meshes. Anything else is `unavailable` and ineligible, never drawn with custom depth.
- **Capture without the private pass refuses outright,** which is stricter than FZ3. Legacy custom depth is bench-only behind three conditions.
- **GE1, P9 refs:** the 71 baseline refs stay exact. Extras are admitted only by name and rule (backup `08fe652`; origin feat as an ancestor of local feat; spike 42c `740220c`; spike 43a `c1193bd` or a descendant). Never re-baseline.
- **Phase B (GE3), matching 5.1:**
  - **D1:** the source is the release line `d7eee97`.
  - **D2:** the 4.25 materials live in the plugin's own Content, authored in the fixture's editor only.
  - **D3:** the skeletal target goes in the fixture.
  - **D4:** camera_clipping "fires" means at least 3 slab-positive spans in one held session, plus a baseline-near can-fail.
  - **D5:** the default pool becomes blinking, lod_corruption, missing_texture and corrupted_texture once their gates pass. camera_clipping is opt-in and never in the auto pool.
- **The owner's names (11-04):** pink = **corrupted_texture**, checkered = **missing_texture**, exactly as in 5.1.
- **Bench-scene rules (GH2, GK2, GL2, GM2):**
  - fix the bench, never the gate;
  - runtime-only, bench-only settings: bloom off, manual exposure (eye adaptation quality 2, method 3), shadowless fills, the view held on `EA_Camera`, and a distinct base material on the texture targets;
  - pre-fire checks: 0 saturated target pixels, mean ≥ 0.1, the pink magenta by hue, checker greys ≥ 20 apart, and ≥ 98 % of the target changed by the swap;
  - G3 stays whole-frame at 0.98. GM2's **explained-pixel fallback** (0 spill, clean edges, every unchanged mask pixel proven equal-colour or black, share ≥ 0.98) applies only to a target that fails the pre-fire check, named as such.
- **The default-route change is pushed only with its gates (GH4).** Done: it shipped in `c5d0919`.
- **GN2:** camera_clipping's "unlabelled frames match" is judged against a measured noise floor (a control pair at the baseline near plane, thresholds recorded first), never "identical".
- **GN3:** the regression after Stage 2 uses the default routes plus camera_clipping, because Stage 2 adds `run_summary.json` and a `whole_frame` key to every run. A default run must be additive-only against `c5d0919`.
- **GN4:** port 5.1's `label_transition_camera_clipping_unconfirmed_entries`. The three `session_global_*` keys are accepted as additive.
- **Fixture editor launches always carry `-BENCHMARK`** (GI3). Paris stays `-game`. Launcher descendants are closed only by PID, creation time and image (GI5).
- **FE3b:** a 10 ms poll records children at creation. Only `ValidatePlatforms` and `QueryTargets` are admitted. One re-run is allowed for an unreadable short-lived child (GG2).

## 4. Forward plan, in order
1. **Stage 094-46** (camera_clipping: GN4, GN2's floor and D4 on D3D11 and D3D12, GN3's three Paris legs, then push and card) once the owner is back and the watcher runs.
2. **After 094-46 is GREEN: Phase B is complete on 4.25.** Next comes a product call from the owner (the dataset items, or another anomaly).
3. **The owner's office errands,** on his next office day:
   - (a) the Europa re-test with the newest card;
   - (b) type home the `HOST-PP CUSTOM-DEPTH READERS` line for Concorde and Bates (GD6; m41 card Section E).
4. **FUTURE, in an owner-set order:**
   - update Node.js (22.23.4, 24.22.1 or 26.12.1+, for Node2Shell CVE-2026-111497) after the porting work;
   - D3D12 render-thread cost of about 1.6 ms per captured frame;
   - a runtime one-frame-early mutant (GI4);
   - D1–D5 dataset items; A5, A6 GC, A8; keys for pool slots; G498's 5.1 follow-up; `ReGate51`/`Baseline51` clean-up.

## 5. Open questions
- Whether old 5.1 office or client data carries the outline leak (it needs GD6's office reads).
- Whether the M3 "blurry, private copy per object" version shipped on 10-05. It's still unverified.

## 6. Corrections and things that changed
- **The 5.1 anomaly names:** pink = corrupted_texture, checkered = missing_texture. An earlier memory line said "missing_texture (magenta/checker)"; that was wrong.
- **The 5.1 material `.uasset`s can't load on 4.25** (newer file version), so they were re-authored for 4.25.
- **Pixel format 18 is `PF_A2B10G10R10`** (10-bit backbuffer) on both RHIs.
- **In 4.25, eye-adaptation quality 0 forces exposure compensation to 1.** Use quality 2 for manual exposure.
- **EA_Core has no skeletal actor.** The skeletal and instanced live checks live in the fixture.
- **Gotchas:** the next free number is after G539 (check the file).

## 7. Mechanics
- **The mailbox:**
  - stage `_mailbox\to_code\<name>.md.part`, check the sha256, then `mv -n` to `.md`;
  - one brief at a time; `# ACK-VERDICT:` after a NEEDS-DECISION or RED;
  - `# SESSION: new-default`, `# MODEL: claude-opus-5-5`, `# EFFORT: xhigh`.
- **Waiting:** the public signal Monitor works in Cowork sessions (key `f2e079e1`). Wake at `>= previous count + 1`, never at "changed", because the GitHub cache lags. Re-arm every 30 min.
- **The run folder:** `_relay\runs\2026-10-16-094-01-europa-phase-a-plan\` (`code-receipts-094-<id>\`).
- **Gates:** FM1, FK1, FJ1, GA5 (3 h), and P9's full `main()` at entry (GC1, with GE1's ref rule).
- **The owner's time windows:** stage nothing heavy into a stated personal-use window. Put hard stop times in the brief, and remember that Code obeys "free from HH:MM" literally.
- **Standing rules:**
  - the HARD RULE (office data never comes home);
  - never kill by name;
  - no machine settings;
  - delete only by receipt;
  - never touch `m51`, `master`, `release/2026-10-05`, tags, `ToCodex\`, `E:\<personal-folder>`, `E:\UEProjects\LyraStarterGame` or `E:\Unreal Projects\StylizedParisStreet`.

## 8. Pointers
- **Rulings and requests** (project `claude/`): 094-42c → 094-45, and `094-43-phaseB-plan.md`, which is in `_reviews\` only.
- **Reviews** (`D:\IntrusiveAnomalies\_reviews\`): `094-4x-code-review.md`, `094-4x-codex-report.md` and the prepush scans.
- **Have Code read:** the plugin's `docs/sessions/2026-11-0*` journals on `feat/europa-port`, `docs/europa/`, and the gotchas file.
