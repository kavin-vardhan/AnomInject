# CHAT HANDOFF — Delivery Endgame: m33 shipped in the Monday bundle, m34+display-fix parked at merge gate (Sun 2026-08-24)

**Read me first.** This chat picked up from `CHAT-HANDOFF-crisis-weekend-delivery.md` at its §1 (sped-up videos, packaging paused) and ran it to ground: diagnosis confirmed, m33 built and field-validated, the delivery packaged and shipped Monday morning. It also ran m34 (GPU mask reduction) through implementation on its branch AND discovered + fixed a second, unrelated defect hiding behind the word "hitch." This doc supersedes the crisis-weekend handoff wherever they disagree. Memory update rides alongside this doc.

**Client status: delivery SHIPPED Monday morning. Client captures immediately on receipt.** Expect field feedback; the branch merge waits on a post-delivery office pass.

---

## 1. CURRENT STATE

### Master (the delivery line) — SHIPPED
- Plugin master HEAD: `2b6c93f` (docs: AMENDMENT 2 + client-readme launcher note). AnomDash master HEAD: `ea19e16` (watcher in-burst estimator + stamp-wins). Both pushed.
- Ships in the bundle: **m33** (honest speed_ratio re-keyed to plugin-owned tick span + `game_clock_speed_ratio` diagnostic field + honest fps stamp live on the pinned fork) · watcher timebase cross-check (in-burst mean estimator; **stamp wins on m33+ sessions**) · camera_clipping out of the default pool (4 default-ticked, 6 boxes shown) · sparse overlay (boxed frames only, no renumbering) · client-readme: resolution paragraph (1080p/720p + stopwatch check + launcher-enforcement line), six-row anomaly table.
- Concorde config: the 8-line `DefaultGame.ini` block (7.5/300 LOD, clip radius 300, two `+ExcludedTargetNamePatterns`, mask+delivery defaults True). **G88 CLOSED: the ini route is field-proven for the first time** — owner read `lodMinCov=7.5000%(ini)` and `lodMaxDist=300cm(ini)` off the packaged build's StartRun echo.
- Delivery checklist: ALL PASSED — final cook · both `(ini)` echoes · G-C validation (see §2) · README dry-run (first ever; passed; owner added verify_capture + readme to deliverables) · make_delivery · client notes in (incl. amended display-stutter note) · packaged.
- **The launcher bat SHIPS and enforces 1920×1080 / 1280×720** (owner decision; clients capture low + AI-upscale). This makes 1080p the real delivery regime and killed the need for any 4K validation leg.
- **NO TAGS CUT. m31 still open/untagged** pending its own Concorde V-3/V-4 — unchanged from the crisis handoff; nothing this session touched that.

### Branch `feature/mask-gpu-reduce` — PARKED AT MERGE GATE
- HEAD `d3b9f08`, pushed. Contains, in order: m34 predictions + GPU mask reduction (compute-shader per-tag count/bounds, bit-exact vs CPU, `IAI.Capture.MaskReduce gpu|cpu|both` default gpu) at `0fc00ef`/`73ebffc` · display-fix predictions AMENDMENT 1 `ead7764` · A-I1 instrument `310a87f` · **stale-present fix** `b05066f` · session-059 journal `d3b9f08`.
- Bench staged exe = m34+fix, `7F37A4AC`, archived. Container = m34 quartet (2A66CA57/A7EF9B12/D8009AD7). Editor target rebuilt.
- All bench gates GREEN: G-R1..R6, G-F1 (owner eye gate — rubberband gone), G-F2 145/145 COMPARE identical, G-F3/F4 regression+artifact clean.
- **Merge gate = G-R7(ii) on Concorde, post-delivery, AS SPLIT** (see §4 locked decisions). Nothing merges before that leg passes. `feature/stencil-capture` untouched at `76cac74`.

### The two defects this chat resolved (they wore one name, "hitch")
1. **Throughput starvation + blind instrument** (the §1 crisis): speed_ratio was 1.000-by-construction on the pinned fork (game clock tracks wall → ratio divides a clock by itself). Owner's numbers confirmed the pre-registration exactly (t span 34.2199 s vs t_wall span 34.2203 s; 162 ticks/120 frames). **Cured by m33 on master, in the shipped bundle.**
2. **Display-only stale-present** (owner-isolated on the bench): on armed frames the mask SVE is the engine-designated final screen writer and ignored `OverrideOutput` → swapchain re-presents stale content → visible rubberband. Dataset/labels/videos untouched (owner's frame-reassembly = the known-answer proof; corrected instrument reads ≤ +0.4 ms excess). Present since m26. **Cured on the branch** (OCIO pattern, both SVEs, A-I1 measured 29/29 before the fix was written). **The shipped Monday build still has it** — deliberately, per the venue ruling; the client note describes it as a harmless display artifact with a fix in development.

---

## 2. DECISIONS MADE THIS SESSION (with the WHY — the part a fresh chat must not relitigate)

1. **Diagnosis promoted to established.** Owner's labels.jsonl/run_summary numbers hit the pre-registered readings exactly. The remaining fork-Core mechanism line (which source line ignores the fixed-step flag) stays a logged, unscheduled curiosity — not needed for any fix.
2. **m33 fix shape = GameSpan re-key + watcher cross-check; VFR rejected** (doctrine line stands); 720p was the floor, never needed. WHY re-key: it makes ratio, early warning, and the stamped fps all honest in one change, and the honesty lands *in the client artifact*, not just telemetry.
3. **The re-key operand is Code's tick-span form, NOT chat's literal (ArmedFrames−1) form** — chat's form was refuted (settle gaps sit inside the span; 162-vs-120 would misread healthy runs at ~1.3 and ship 0.8× slow videos). Structural identity on stock hosts (each tick = 1/fps game time) is the no-op guarantee, verified at G-A to 1e-7.
4. **Watcher AMENDMENT 1 ratified, then superseded by AMENDMENT 2 after a field failure.** The G-C leg caught the median estimator overriding an honest stamp (spike-blind: median reads in-burst spacing, misses minority-slow armed frames — the mask-hitch shape). Chat's contiguity discriminator was then refuted by measurement (ALL sessions are index-contiguous, none time-contiguous — index carries no time information). **Shipped and ratified: (2a) in-burst mean** (intervals whose game-delta = one tick; agrees with the plugin ≤0.08% on four banked sessions; median fallback when t unusable) **+ (2b) an honest stamp is NEVER overridden** on m33+ sessions — note-and-defer only; pre-m33 sessions keep the override (the only regime the override was built for). WHY 2b: post-m33 an overriding watcher is a second source of truth that can only add error — and it did, in the field, on the delivery build.
5. **G-C ruled CLOSED/PASSED with two recorded deviations** (not relabelled): leg ran at launcher-enforced 1080p, not the pre-registered starved 3200×2000 — ruled intent-satisfied because the launcher makes 1080p the actual delivery regime; the "old ratio ≈1.000" prediction was deep-starvation-scoped → not-fired-not-refuted. Field closure: re-encode at honest 27.149, "stamp WINS" note fired, plays true by stopwatch.
6. **Resolution DECIDE = Option B** (recommend 1920×1080 / 1280×720; owner verified true-speed at both) — then made moot-but-consistent by the launcher decision. Stopwatch line stays in the README regardless (her sustained rate is still formally unknown; owner's 5 fps observations were RDP-confounded).
7. **Display-fix venue = the m34 branch, not master.** WHY: pre-existing master defect, but the file is in m34's edit list, two branches must never edit one file, and both cures reach the client in one post-delivery pass. Monday shipped with it as a documented limitation per the standing owner ruling.
8. **Measurement-before-fix enforced on the display fix** (A-I1 log line, one leg, 29/29, THEN the fix). WHY: four "obviously right" mechanism claims died this weekend; 95% source-derived confidence is not 100% measured.
9. **G-R7(ii) merge gate RE-SPECIFIED (locked):** eye/OBS judges ONLY the display fix; throughput reads EXCLUSIVELY from m33 wall instruments (t_wall span vs frames/VideoFps; speed_ratio with game_clock_speed_ratio beside it). The eye is never again a throughput instrument — the two phenomena are only separable by instrument.
10. **Milestone numbers:** m32 stays burned (bench legs, never-reuse). m33 = re-key+watcher. m34 = GPU reduction (+ the display fix rides its branch). CPU mask path lives behind `MaskReduce` through m34; **deletion is a decision item at the NEXT cook after m34's.**
11. **Client-notes claim discipline:** chat asserted "she has seen the stutter" — fabricated; only the owner ever observed it, on his two machines. Whether her post-process chain even manifests it is UNKNOWN. The note was reworded as an advance heads-up ("you may see…"), which is both honest and better positioning.

## 3. FORWARD PLAN (in order)

1. **Field-feedback watch.** Client captures Monday. Likeliest signals: the watcher's "stamp WINS" note (benign, expected under load — the note itself says investigate the capture, not the video) · visible rubberband during anomaly frames (known, note covers it, fix is on the branch) · anything else comes home as an artifact first.
2. **Post-delivery office pass (one visit, batched):** pull the branch on Concorde → rebuild editor target FIRST, then cook (G-R2: new global shader CANNOT hot-swap) → **G-R7(ii) as split** → if green, **merge branch → master**. Same visit, still owed from the crisis handoff: m31's V-3/V-4 Concorde validation (then TAG m31, then m33/m34 tagging can be sequenced) · Concorde custom-depth census (H6) · HDR preview-format check (pending since m19).
3. **Docs pass (queued, not urgent):** worktree standing rule → process doc · open-semantic entry → capture-fps.md · stale runbook §8.2 A44 example (IsHideTypeAnomaly renamed at 053) · G165 candidates (cadence-vs-health instrument; PowerShell quoted-commit-message trap) · memory-edit consolidation (list was 30/30 — this session's memory update addresses it; verify).
4. **CPU-path deletion decision** at the next cook after m34 merges.
5. **Then the crisis-handoff §5 feature queue, gates now clearable:** Native-fps toggle (Q7 ANSWERED — see §5 corrections — but the client-facing DECIDE on host-load-dependent temporal signatures still stands) · probability sliders (DRAW→R-SEED risk; renormalization rule; missing_object status — design pass required) · dashboard ArgControls (typed-schema brief exists in the crisis thread) · SM_GratIng ratio measurement · route (e) one-property test · client channel status confirmation remains the standing session-start check.

## 4. OPEN vs LOCKED

**LOCKED (do not reopen):**
- Stamp-wins (2b) on m33+ sessions; in-burst mean estimator (2a); pre-m33 override retained.
- G-R7(ii) split (eye=display only; instruments=throughput only).
- Display-fix venue on the branch; Monday shipped with the display artifact documented.
- Concorde health = stopwatch / wall math, never speed_ratio alone — though post-m33 the ratio is honest there; the *pair* (speed_ratio vs game_clock_speed_ratio) is the diagnostic.
- VFR rejected; doctrine line stands. Veto ZERO-ONLY. Content-clock default wall.
- Git standing rule: live-branch box ⇒ master work via `git worktree`, path-scoped `git add`, `-A` banned.
- Launcher ships, enforcing 1080p/720p.

**OPEN (needs a call, with owner where marked):**
- **The gap-semantics question (chat-owned, deliberately deferred):** if Concorde's in-burst mean disagrees with the plugin's sustained rate, the excess is gap-side wall time — which number is the "true playback rate" of a session with deliberately-uncaptured wall spans? Standing order: first Concorde session where the note fires comes home as the measurement; rule THEN, gated. Until then stamp wins, note is the tripwire.
- Estimator's named limitation: gapped session WITH in-burst spikes keeps median spike-blindness (documented, revisit gated).
- m31 tag timing (post V-3/V-4) and m33/m34 tag sequencing after merge.
- Whether the client's own hardware manifests the display artifact at all (unknown; her chain decides).
- H6 census, HDR check, client-channel confirmation — carried from the crisis handoff, still open.

## 5. CORRECTIONS / SUPERSEDED (so stale understanding dies here)

- **"The hitch" was TWO phenomena.** The 054 Concorde bisect saw the display defect; Concorde additionally had real throughput starvation. Any prior note reading "mask pass stalls armed frames ~100 ms" is superseded: corrected instrument shows ≤ +0.4 ms game-thread excess; the ~100 ms readings were the burst cadence's scheduled 3-tick gaps misread (labels.jsonl consecutive t_wall deltas measure CADENCE, not frame health — normalize by frame_index delta).
- **The m26/m27 "mask hitching = GPU pixel scan cost, documented limitation" framing is superseded** for the visible symptom; the throughput cost is real but bench-invisible; the visible symptom was stale-present.
- **speed_ratio on the pinned fork pre-m33 = 1.000 by construction** (established, measured). m21 ship rule was vacuous there; post-m33 the re-keyed ratio is honest everywhere.
- **Chat formulas refuted this session:** literal (ArmedFrames−1) span (settle gaps) · watcher span estimator (same disease, −23–25% on every banked session) · index-contiguity discriminator (carries no time information). All withdrawn on record.
- **Chat's fabricated client observation** (§2.11) — logged as chat error #5; owner caught it.
- **Q7 ANSWERED (git archaeology):** there was never a Native selector; the "Native era" = one-frame-per-tick free-run, and 0.5×/3× playback was pure stamped-vs-actual arithmetic (60 fps box @ stamp 30 = 0.5×). Nothing forced a 0.5× clock. The parked note's revisit question is RESOLVED; the crisis-handoff §5 gate on the Native-fps feature is cleared on the mechanism side.
- **"720p is the only certified resolution"** was already stale; now moot — launcher enforces 1080p/720p, and label alignment holds at 3200×2000 regardless.
- CYL73 offset note (session 058) closed: pose-scoped count band, not drift (G-F2 read the original banked band).
- **G88 ini-route doubt for the LOD keys: CLOSED** — field-proven `(ini)` on the packaged build.

## 6. POINTERS (what a fresh session has Code read)

- **This doc first**, then `CHAT-HANDOFF-crisis-weekend-delivery.md` (background; superseded where they disagree).
- Branch: predictions file `docs/predictions/2026-08-23-m33-gates.md` + the m34 predictions incl. AMENDMENT 1 — **read predictions BEFORE results**, house style. Journals 056 (m33) and 059 (display fix + 054 reinterpretation); journal for the watcher fix session.
- `docs/capture-fps.md` (re-written ratio formula + ship-rule scope note + Pace-0 note) · journal 054 §8 dated correction.
- Verify state by git log, never CLAUDE.md status block (stale-by-design between milestones): master plugin `2b6c93f`, AnomDash `ea19e16`; branch `d3b9f08`. Banked legs: M33_*, M34_AI1_OVR, M34_F2_*/F3_*; owner eye sessions beside the staged exe.
- Bundle contents: `bundle_manifest.txt` (resolution paragraph verified at :54; owner added verify_capture + readme entries).
