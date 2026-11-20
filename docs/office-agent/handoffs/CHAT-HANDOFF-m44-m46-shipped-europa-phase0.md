# CHAT HANDOFF — m44…m48 shipped · Bates delivery week · Europa (UE 4.25) opened
**Date:** 2026-09-03 ~22:00 local (session 069, second day; 28 briefs through the mailbox; amended after m47/m48) · **Supersedes:** CHAT-HANDOFF-m41-m43-shipped-delivery-week.md wherever they disagree.
**Confidentiality:** projects/hosts by INTERNAL codename only — **Concorde, Bates, Europa**. Never a real title, studio, product, packaged-exe or fork name anywhere; the office's own project codenames are never written here either, nor any mapping between names. `AnomalyCapture.Build.cs` is the single unalterable exception. Never paste raw scrub-verifier output (G202). Photos of the office box can carry real paths — never transcribe paths. All AAA projects the owner mentions live on his office PC.

---

## 0. READ THIS FIRST

the owner's four Bates observations from card Section E (target mask one frame late; no masks on blinking's hidden frames; blank PNGs everywhere; corrupted-texture "visible from n+1") drove the whole day. **Three real defects fixed and shipped (m44), the hidden-class masks built and shipped (m45), a latent screen-percentage defect found and fixed (m46), two bench cooks done, master re-baselined.** Nothing with masks has reached a client. **Bates ships THIS WEEK with the target mask included (owner ruling).** Next target after Bates: **Europa**, a title on a UE 4.25 (possibly modified) engine — Phase 0 audit is on master; three owner decisions pending.

## 1. Current state (verified at close)

- **master == origin, tip `324ebad`** `docs(europa): phase 0 port-surface audit`. Feat commits today: m44 `fafb81f` (merge `c5b3d27`) · m45 `4eab312` (merge; feat on branch `m45-hidden-class-masks`) · m46 `866aa0e` (merge `80b382d`). Docs commits: `d48e1ba` (m44), `d204598` (m45), `7284c6e` (m46). Earlier today: export fix `f40fee4` / `cdd0c4b`; Deimos retirement `b752934`.
- **Container quartet (m46, live):** exe `60AE8C61` · utoc `EF8EB23C` · ucas `A8BFFF88` · pak `3C026A8D`. Two cooks were run today (m45 post-cook quartet `38A55169/4621F571/513F4D35/2163A13A`, then m46); pre-cook quartet `2A66CA57/A7EF9B12/D8009AD7` archived. A4 re-baseline proved the cook changed no behaviour (G103); those legs are master's P-C7 controls.
- **NO TAGS** (highest m30). Office batch tag sequence: **m31→m33→m34→m35→m36→m37→m38→m40→m41→m43→m44→m45→m46** (m39, m42 slot in when they ship).
- Compiled defaults ON: census, mask, target mask. Masks correct at any screen percentage (m46). Hidden-class masks (m45) on every labelled hidden frame; Nanite targets excluded (G134).
- **Bates:** owner was told to pull `7284c6e`+ (checkout the log header first, `git pull --ff-only`, editor rebuild) and run card **Section F** (reads a–g). Section F results NOT yet reported at close.
- Branches: only master + `m45-hidden-class-masks` (merged). `m44-f1-…UNVALIDATED` deleted (content is m46).
- Mailbox: 23 GDP briefs today; running API-equivalent total ≈ $505 (none billed on Max).

## 2. What shipped today, in plain terms, and what each gate found

**m44 — target mask onset + hygiene.** (i) First-frame mask was empty on every event, in both tick orders, host-independent: **tag ownership** — the target could still carry another consumer's stencil value (census OR a previous event on the same actor) and `ArmTargetMaskOwn` trusted "already tagged". Fix: an actor under a live fire belongs to its event; the arm retags unconditionally. Census-OFF cured only half — that is how the second source was found. (ii) Records were born in the readback drain (one cycle late) — now created at OnWorldTickEnd. (iii) `mask_state` present|empty|unmeasured; no blank PNGs; `mask_file` null unless present; masks ONLY on labelled frames (G7 — killed the stray masks on blinking's visible in-between frames the owner saw). ONSET gate 4/4 both orders. Stencil pool split REFUSED with numbers (55 values vs 77 census candidates per leg).

**m45 — hidden-class masks (Blinking, MissingObject).** Hide via main-pass-off + depth-pass-off + every silencing flag, keeping `bRenderCustomDepth`; a **logical-hidden registry** is the single source of "hidden" for labels, m26 and the census (dropping SetActorHiddenInGame would otherwise have emptied blinking's hidden set silently). Identity gate: 0/60 frames differ at the AA-off arbiter, control floor 0; **can-fail lever = omit depth-pass silencing** (20/60 differ) — the shadow lever could not fire in the fixture. IoU vs the real silhouette 0.9987 / 0.9969. ONSET 6/6, G7 equality 35=35, census skips live-fire targets (counted). Client readme: hidden-class limitation REMOVED.

**m46 — resolution mapping.** Mask pass ran after upscale and sampled internal-res scene textures with output coordinates: at 50% screen percentage 25/26 masks were wrong; 0 at 100% (bench default, so it explained nothing observed). Fix: map through the internal view rect, nearest, clamped. 50% leg now 35/35; joins MASK-PICTURE-PAIRING permanently. Retires the nearest-neighbour resampling follow-up.

**Refuted along the way (all measured):** SynthTickOrder as the cause (identical both orders); the mask frame-handshake (probe never showed the previous frame; thread-lag cvar a no-op); resolution mapping as the bench cause. **Retracted:** 069-15's "a quarter of masks wrong, a quarter absent" — the probe's own stencil value (250, inside the pool) and pink material collided with the system under test (G226). Clean probe: 40/40 both orders.

**O4 (texture visible from n+1):** not a bug — pixels differ 6–8% on frame n vs a 0.5% baseline; temporal AA settles after. One readme sentence.

## 3. Decisions made today (owner and chat), with the why

- **Owner:** Bates ships THIS WEEK with the target mask included; first-frame gap (now fixed anyway) would simply have been told to the client. Owner's ML ruling: mask frames == labelled frames, would-be silhouette on hidden frames, never inverted/bracketed (a bracketed scheme contradicts the frame label and inverts semantics for two types only).
- **Owner:** "Deimos" retired (was a second name for Bates); office codenames never appear here; new target codename **Europa** (owner's pick, knowingly the same word as the office uses — flagged once).
- **Owner:** important updates lead with a one-line status banner (🟢/🔴/🟡/⛔). Adaptive polling replaced fixed 25–45 min checks; the mail-signal Monitor route now works when the session's permission mode allows shell fetches (see §7).
- **Chat:** identity/pixel arbiters run native-order at the AA-off configuration; alignment gates run both orders (G228/G230: SynthTickOrder is nondeterministic even at AA-off, by its own control; cross-run comparison at the delivered configuration has a ~9% floor). Not an exemption — the rule's scope.
- **Chat:** editing the frozen gate level for a can-fail lever refused (G99 — would move every baseline); depth-pass omission chosen because it changes pixels in any fixture.
- **Chat:** every capture-side alignment gate runs in BOTH tick orders (native + SynthTickOrder); ONSET and MASK-PICTURE-PAIRING (100% and 50% legs) are permanent; every milestone builds BOTH packaged and editor targets (G221).
- **Chat:** cooks are sequenced operations after all measurement legs close (G118); a shader-parameter change cannot be validated without one (G129).

## 4. Forward plan

1. **Owner, Bates:** pull master (≥ `7284c6e`), editor rebuild, card Section F reads (a)–(g). Report (e) internalViewRect vs viewRect (informational now — m46 covers either) and (f) fog card.
2. **Physical hitch/throughput gate G-R7(ii)** — owner in person or the OBS route; hard precondition for the client cook.
3. **Client cook on Bates** per PRE-DELIVERY-CHECKLIST §1.1 (cook-time gates now include ONSET both orders, zero blank PNGs, G7, MASK-PICTURE-PAIRING 100%+50%, identity arbiter with can-fail leg, bench-lever grep incl. MaskPairingProbe/HideMode/HideOmit*). Ship.
4. **Europa** (after Bates ships): owner answers the three Phase-0 decisions (§5), then Phase 1 walking skeleton (injector-only compile on 4.25, one anomaly fires, gates E1-G1..G6), then the capture backend behind two seams (capture hook; mask backend incl. reduce+readback), seam drawn above `FViewInfo`.
5. Later: m39 honest bbox; m42 persist tags (measure first); within-frame identity comparator (G-M9 shape) for the delivered configuration; the mask frame-handshake as hygiene; watcher archive-at-launch (chat-owned script fix); fog-card cause (G226 tag-reuse is a measured mechanism looking for the host observation).

## 5. Open vs locked

**🔴 Open — owner decisions (Europa Phase 0):** (1) the 4.25 deliverable — (a) injection only, (b) injection + backbuffer capture + labels, no masks (uses only shipped, gated paths), (c) full parity incl. masks (needs every renderer question answered first); chat recommends committing to (b) first, (c) on top. (2) Does the Europa title already use custom depth/stencil (the plugin reserves 200–254)? (3) Does the bench host the 4.25 engine (~150–250 GB, E: only, own frozen fixture) or does Europa work happen only on the office PC?
**Open — owner (Bates):** Section F results; G-R7(ii) scheduling; m38 run-log client default (chat: OFF).
**Open — technical:** m39; m42; sync-fallback staleness; within-frame comparator; mask handshake hygiene; fog-card confirmation; watcher archive-at-launch.
**Locked:** everything previously locked PLUS: tag ownership; records at OnWorldTickEnd; mask_state contract; masks only on labelled frames (equality for hidden-class); logical-hidden registry; main-pass-off hide keeps custom depth; identity arbiter scope rule; both-orders rule for alignment gates; both-targets build gate; internal-view-rect mapping; pool split refused; Europa = one codebase, two seams, no `#if` inside gate-bearing algorithms.

## 6. Corrections to discard

- "Deimos" as a host name → retired; two office hosts only (Concorde, Bates). Historical journals saying Deimos mean Bates (G222).
- "The +1 is tick order / thread lag / resolution mapping" → all refuted by measurement; it was tag ownership (G227).
- "A quarter of masks are wrong in content" (069-15) → retracted; fixture collision (G226).
- "A blank PNG means measured and nothing visible" → no blank PNGs exist any more; `mask_state` carries the meaning; the in-game help/echo strings were corrected too (G139).
- "Masks only correct at 100% screen percentage" → fixed by m46 (G225 FIXED).
- "Hidden-class masks not provided" → shipped (m45); the readme paragraph is gone.
- The README recipe for the mail-signal Monitor greps `"key":{…}`; the file is `"key": <count>` — grep `"key":[0-9]*`.

## 7. Working with Code — what changed today

- **Adaptive polling:** first check at the brief's expected finish (from past durations: reads/plans ~15 min, implement+gate ~20, docs ~12, cook ~22), then every ~5 min while `watcher_status.txt` says running, 60 min idle.
- **Mail-signal Monitor:** public `https://raw.githubusercontent.com/mr<HOSTNAME>/mailbox-signal/main/signal.json` (`{"f2e079e1": <gdp count>, "a1c057b4": <<other-home-project> count>}`); a persistent Monitor polling every 30 s wakes chat on change. It is refused under Cowork's "Automatically approve" mode (safety classifier blocks shell HTTP fetches) and works when the session's permission mode allows it — try once, never work around, fall back to adaptive polling. Keep a 30–60 min fallback wake and re-arm on any wake.
- Host crash mid-brief: the watcher's `ran.txt` prevented a re-run; the resume brief (same Code session) recovered cleanly. The brief file itself is archived only after completion — archive-at-launch is a pending chat-side fix.
- Code's `git add docs` swept the untracked CHAT-HANDOFF files once; corrected; path-scoped adds only (CLAUDE.md).

## 8. Pointers

`CLAUDE.md` (status block, invariants, milestone template) · journal 069 §§5–16 · predictions `2026-09-03-m44-m45-target-mask-onset-and-hidden-class.md` (+ appendices) · gotchas **G221–G230** · ledger §§11–13 · `docs/office-rdp-card.md` **Section F** · `docs/europa/PHASE0-port-surface-4.25.md` · `client-readme.md`, `client-delivery.md`, `PRE-DELIVERY-CHECKLIST.md` §1.1 · `_binary_baselines/README` (three quartets) · `E:\MailBox\README.md` (dashboard, toasts, signal) · previous handoff (m41/m43, mailbox protocol).

---

## 9. EVENING ADDENDUM (after §0–8): m47, m48, the auto-exposure finding, the pairing-gate scare

- **master == origin, tip `914f14b`.** m47 `3549312`/`93c2b65` · m47b docs `b6e5588` · m48 `20db6e5` · pairing closure `914f14b`. Staged exe **DE65F84A** (m48); F309D836 (m47) and 60AE8C61 (m46, doubly load-bearing) archived. Container quartet unchanged since m46 (no cook needed for m47/m48). Office batch: … m46 → m47 → m48. **Cook-time gates are all green; the Bates cook is unblocked.**
- **Owner observation (Bates AND Concorde, editor target only):** pink/checker sometimes not visible on the first frame (5–10%), object sometimes black on that frame, occasional whole-picture black burst that recovers.
- **m47 (shipped):** shader-readiness pre-warm at StartRun (free in packaged), per-frame `render_state: "shaders_pending"` keyed on OUR two materials (a process-wide counter would have marked all 90 good frames — G232), `shader_jobs_pending` reading, **black-frame gate** in verify_capture (threshold 6.0; proven to fail on synthetic black) + dark-first-frame count, both in the cook checklist. Finding: the on-demand-compile mechanism is REAL (5.1 source) but did NOT reproduce even under a cold shader DDC (3,760 jobs pending, magenta drew correctly on 90/90) — completeness is per whole shader map, a draw needs one vertex factory, the static-mesh VF compiles first; the fallback window bites skeletal/Nanite/cloth (Concorde's case was a held weapon). Skeletal cold-DDC leg = FUTURE (needs a fixture). Usage flags verified 14/14 on disk. Symptom (2) "object black" UNEXPLAINED — owner to note the mesh type next time.
- **m47b (measured):** every bench leg had auto-exposure PINNED OFF; the game (and the delivered build) run the engine defaults = adaptive AE. AE ON: whole-picture mean −24%, session-start convergence over ~30–40 frames, the same anomaly renders 23% darker late in a session; packaged reproduces editor exactly. AE is structurally refuted for (2) (global operator); nothing approaches black on this fixture (G135: target is 7% of frame). New confound: under live AE the first event of a session looks like an onset effect and is not (G234).
- **m48 (shipped):** additive `exposure_dip: true` per frame when whole-frame mean luma drops >4.0% vs the rolling mean of the previous 8 captured frames (first 8 frames never marked); `frames_exposure_dip` in run_summary; readme paragraph; the cook-time smoke run gains one AE-ON leg (dip count must be >0 with AE proven live; black frames 0). The plugin NEVER forces exposure. Detector fired 10/90 on the AE leg (predicted 9–12), 0 on pinned legs both orders.
- **Pairing-gate scare, resolved:** MASK-PICTURE-PAIRING read NEITHER 54 on m47/m48 legs. Bisect: the m46 binary reads the same with the same recipe → recipe, not binary. Cause: the probe wears M_CorruptedTexture_Pink and the leg's fixture anomaly was corrupted_texture — two magenta objects, centroid dragged (G226 recurring on the colour axis). Harness fix: the analyser derives its fixture from the run's own artifacts and REFUSES (exit 3) on a colliding fixture; gate re-run green on m48 both orders (33/33 N0 P0). G235: "moved nothing vs the previous binary" is not a gate result — the baseline is the last PASSING reading; fixture and instrument are versioned with the gate. Carried: synth order sits ~2 px from tolerance at si=8 (a NEITHER 1 there is variance). FUTURE F1: give the probe a colour no anomaly uses (binary change; after delivery).
- **Mailbox/operations:** two API-500 deaths (Anthropic side) mid-m48; resumed cleanly. Thread 070 (Europa) is PARKED by the owner; its checkout (europa-e1) is restored after every thread-A brief; the shared-tree rule is CLAUDE.md clause 4. Mail-signal Monitor works in this session (permission mode allowed the fetch) but times out every 30 min and re-arms; the signal count is mailbox-wide (both threads' reports bump it).
- **Forward plan unchanged:** owner Section F on Bates (reads a–h now) → physical hitch gate → client cook per checklist §1.1 (incl. the AE-ON smoke leg, black-frame gate, pairing with the refusal) → ship → Europa decisions (§5) → Europa Phase 1.
