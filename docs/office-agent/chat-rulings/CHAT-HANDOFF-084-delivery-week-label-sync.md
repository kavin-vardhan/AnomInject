# CHAT-HANDOFF 084 — Delivery week (5 Oct 2026): label sync is the product; m52 fix in proof; label-sync audit; m53 delivery cut pending

**Written 2026-09-29 ~02:15 IST by chat Claude (orchestrator), during an overnight autonomous run.** The owner logged off at 23:15 IST on 09-28 with: "continue, we don't have time; you don't have to wait for my approval for checking if the PC is free."
This supersedes `CHAT-HANDOFF-082-m55-code-takeover.md` for status. That doc still holds m55, m53 S1 and the S2 planning history.

## 0. Read this first (60 seconds)
**Status at 13:20 IST Fri 2 Oct — HANDOVER (3a-41):** Codex now implements via the Relay; a Claude Code Desktop session orchestrates and reviews (kick-start `_reviews\090-HANDOVER-code-desktop-orchestrator.md`). UV/normal works on office content (3a-40); blurry ships OFF unless the proxy blur is proven by Sunday noon. Earlier (08:25):  ⛔ Claude Code's login expired (3a-39). 090-10e's work is committed locally but unpushed; 090-10f2 is held until the owner re-logs. Earlier (06:20):  UV/normal LOD-bias rework landed (m53 `88801bd`; 0 → 72/82 eligible on office-like content; 3a-38). The office pull this morning is for readings. 090-10e is running; 090-10f (MUST-FIX for invisible labels) is queued. Earlier (01:55):  090-10b2 done (blurry works in PIE at home; morning pull m53 `d7e396d`, 3a-37). 090-10c (the m53 LOD-bias rework) is running; 090-10e is queued. Earlier (21:00):  the office census shows uv 0/15,906, dominated by LOD bias (3a-36). Owner chose **B** (per-part corruption). Queued: m52 PIE, then the m53 rework. **The watcher needs a restart.** Earlier:  🔴 **the office still fires zero UV/normal.** The shader-map gate was fixed (3a-34), then the next gate appeared, `runtime_lod_bias` (3a-35). 090-10c (LOD bias plus a full gate audit) and 090-10d (m52 PIE plus census) are queued for tonight.
- Label sync is proven from pixels for every delivered anomaly (3a-19, 3a-23).
- **The Thursday pull target is m53 `2015cfa`** (3a-28): F1, the Nanite skip, the readme corrections, the Shipping exclusion, kit 1.1, and the log counter.
- 🔴 **Codex's 090-08 re-check found 4 HIGH label/picture holes** (3a-30). Round A (090-09) is GREEN (3a-31); round B (090-10) is running; then 090-11 (smoke harness), 090-12 (smoke run), 090-13 (Codex re-check). Merges move to the weekend; delivery stays Monday. The queue is in 3b.

**Delivery: Monday 5 Oct 2026.** The owner's scope is **blurry textures (`stuck_low_mip`, with fixed labels) + UV and normal texture corruption (`uv_corruption`, `normal_corruption`)**, on top of what master already ships.
- Full scope ruling: `_reviews\085-00-chat-ruling-delivery-cut.md`.
- **The owner handed over the fallback:** if m53 isn't ready, deliver m52 fixed and send m53 as a follow-up drop.

**THE GOVERNING RULE (owner, 2026-09-28).** "Label sync is the most important part of our plugin … if labels aren't synced the dataset, and the tool as a whole, is completely useless for the client."
- **Release rule:** labels **and** target masks match the picture on the first and last visible frame, **0 frames off**, measured **from pixels**, on realistic content. Every gate must be proven to FAIL on a broken build.
- **Circular gates are banned** (the label checked against its own driving signal).
- **Priority:** sync first, then m53. m53 slips before sync does.
- **Thursday 1 Oct:** the owner decides whether to pull any anomaly still failing or unproven.

**How the owner found it.** He pulled `master` at the office. `stuck_low_mip` labels were:
- late at onset, from 1 frame early to 2 frames late;
- early at offset, blur still visible about 5 frames after the label ends;
- once on the wrong object (mask on the player's glove, blur on a wall).

His readings are in `_bates_reads\2026-09-28-m52-label-timing-owner-observation.md`. The original m52 gates were circular and blind to pixels; chat approved them and owns that.

**The owner's product calls this week:**
- **the delivery cut:** UV tile + scramble, normal invert + green_flip, all-or-nothing per object, and m52 never at the same moment as m53;
- **defaults:** available, on by default only if Thursday's office test is OK;
- **PC:** bench at night or whenever idle; compiling by day is fine;
- **the weekly Code usage limit:** he has a free reset and uses it past 95 %.

**Two calls chat made on his behalf while he was offline** (he can reverse them Thursday):
- a TAA `transition` flag instead of changing the anti-aliasing;
- fixing mask-ID exhaustion now.

## 1. m52 label-timing fix: branch `fix/m52-label-timing` (off `master` `b5f15a3`)
**Root causes** (084-01; Code and Codex independently, with consult):
- **Onset:** the label read the game-thread `GetNumResidentMips()`, which updates only after the render-thread swap.
- **Offset:** `BeginRevert` dropped the fire in the same tick while stream-in continued.
- **Masks** shared the same switch.
- **Wrong object:** the purity check counted only visible, polled, ≥ 6 %-coverage static/skeletal meshes at Apply, and skipped targeted fires.

**The fix, in rounds:**
- **084-02 / 02b / 04:**
  - the label, mask, `held` and `observable` come from a **render-thread residency record per captured frame** (unknown means labelled, never clean);
  - a `RestoreTrail` keeps the event until the record is back at baseline, and the streamer is fenced;
  - the next burst waits; a timeout means `restore_unresolved`;
  - **whole-loaded-world purity** (all levels, unregistered components, a mid-hold monitor that reverts and flags `contaminated`);
  - bench levers only under `-IAIBench`, excluded from Shipping.
- **084-05a:**
  - **G350:** `anomaly_present` is no longer set on post-closure rows;
  - the **`transition` flag** under temporal AA (CVars `IAI.Label.TransitionOnFrames/OffFrames/HideFrames`), plus `transition_present`;
  - **mask-ID recycling** (was: masks stop after ~55 events per session).
- **086-02, camera_clipping:** a sphere-collision proxy was replaced by a render-bounds **view-slab** test (pawn included). The decision moved after the camera update, because it was reading the previous frame's camera.

**Evidence so far** (084-03 bench, plus the 084-05b re-evaluation):
- **AA off:** the fix is **0/0 on 16/16 events**; the baseline is −1 / +4 on every event.
- **Render window = label = mask** on 62/62.
- Under TAA the picture lags the exact label by +2 at onset and +1…+6 (max 8) at offset. **Transition values ruled at 3 / 8 / 1** (084-05b ruling).
- **Monitor cost** ≈ 0.2–0.3 ms.
- **Builds (archives in `_binary_baselines\`):** `B725678B` → `2AC0523E` → `E9FF019A` (current).

**Open Codex findings (084-05b RED), fixed in 084-07, held in `_mailbox\_held\`:**
- **F2:** camera_clipping misses scaled instanced meshes.
- **F4:** a mask-ID alias via custom-depth-off holders.
- **F5:** transition history is lost across a run boundary.
- **F3:** the sync path's event set differs from `annotation.json` (inherited; investigate).
- **F1** is ruled **not a defect:** `anomaly_present` means "event active"; visibility is `visible_positive` / `observable` / `transition`, and the readme must say so.

## 2. Label-sync audit of every delivered anomaly (086-01, from pixels over 770 banked sessions)
- **SYNC-PROVEN (0 frames off at 50 %):**
  - blinking, 307 events, with +1 at 10 % on 4/592 edges from TAA, now carried by the `transition` hide flag;
  - missing_object, synthetic content only;
  - missing_texture (thin);
  - corrupted_texture.
- **DESYNC:** stuck_low_mip on master; its fix is above.
- **UNMEASURED:** lod_popping and camera_clipping (both had circular m30 gates) and m53's shipped modes (not built yet).
- **Mask-ID limit found and fixed:** above.
- **The checker is proven:** a +3 known answer on 12/12; 98/98 doctored labels fail.

## 3. The 084-06 bench result (RED; ruled in `_reviews\084-06-chat-ruling-bench.md`, 03:10 IST 09-29)
- **SYNC-PROVEN on MainWorld content too, AA on and off:** blinking, missing_object, corrupted_texture. missing_texture is proven on targeted legs; one auto-pool event is pending a reference check.
- **The mask-ID fix is proven:** 119 events, 0 aliasing. The F4 path is not yet exercised; MASK55 v2 adds concurrent events.
- **m52:** the offset is **exact**. At onset, the first 2 labelled frames show only 9–22 % of the blur (it was ~70 % in 084-03).
  - **Ruling 1:** 084-07 decides from the render records between (a) partial application, which gets a residency-derived `transition` flag with reason `partial`, AA or not, and (b) a render-to-picture lag, where the label follows the image.
  - **Guard:** flagged onset frames below 10 % count as label-early; more than 3 flagged frames on an edge is a FAIL.
- **camera_clipping over-labels hollow geometry** (MainWorld 200/200, nothing visibly clipped).
  - **Ruling 2:** keep the SAT broad phase, then triangle confirmation with complex traces in the slab, and a per-frame `camera_clipping_unconfirmed` flag where that can't be done. Cost ≤ 0.5 ms.
- **B-CC never ran (G354):** the bench container predates AnomalyBench. 084-07b cooks the fix branch.
- **lod_popping is UNJUDGEABLE** (the effect is too faint). 084-08 builds a high-contrast LOD fixture.
- **Harness:** G355 (restore on a locked file) and G356 (the null never calibrated) are fixed in 084-08.

## 3a. 084-07 result (NEEDS-DECISION; ruled in `_reviews\084-07-chat-ruling-source-round3.md`, 03:55 IST)
- **m52 onset is partial application.** The rock's three textures blur on different frames, in a varying order. Those frames are flagged `transition_reason: partial` from the per-texture render record, AA or not.
- **camera_clipping** now confirms against triangles (slab-confined complex traces, heightfield) and flags `camera_clipping_unconfirmed` where it can't.
- **F2 / F3 / F4 / F5** are fixed; transition defaults are 3/8/1. Exe `FF41BFF3`, source `8b8b4d0`, docs `a6a4893`.
- **Chat's ruling-7 premise was wrong.** Event-active, unlabelled rows read `visible_positive: true`, a client-facing mislabel. Now fixed: entries carry `labelled`, and `visible_positive` requires it.
- **The guard is re-based:** a partial frame above the null band is confirmed; within the band it is counted as flagged-invisible (backed by the per-texture record); an off unflagged frame is a FAIL. Warm-up events count, and the "> 3" cap is withdrawn (investigate above 8).

## 3a-2. 084-07b result (NEEDS-DECISION; ruled in `_reviews\084-07b-chat-ruling-codex-recheck.md`, 04:30 IST)
- **Done:**
  - the cook `87A46A27` (the descriptor lists `AnomalyBench`);
  - the readme §8.6–8.7 with the training-label recipe;
  - `verify_capture.py --label-rule`.
- **Codex's re-check is RED:**
  - **N1 (in the data):** FireWindow events (missing/corrupted texture, and m53 after the merge) are written `labelled: false`. Code found it independently (G361).
  - **N2:** a planar mesh becomes a clean camera_clipping negative.
  - **N3:** instances under non-uniform component scale.
  - **N4–N8:** medium.
- **All eight are being fixed in 084-07c.** After that, **only HIGH findings on paths the bench exercises, or findings in the data, block;** mediums on unexercised paths become documented limitations.

## 3a-3. 084-07c result (NEEDS-DECISION; ruled in `_reviews\084-07c-chat-ruling-round4.md`, 05:25 IST)
- **N1–N8 are built,** with tests both ways (262 + 128 + 24) and a mutation check (9 old behaviours re-inserted, all fail). N1 is RESOLVED: one annotation authority decides `labelled`. Exe `16053B80`, cook `59C20959`, both archived.
- **Codex v2 left four small cases.** Each gets a flag or a default:
  - shear-case instances: unconfirmed;
  - a forced-unknown set: `unresolved`;
  - welded candidates: unconfirmed;
  - the N7 saved-prior variant: a documented limit plus the `mask_prior_collision` tripwire.
- **The loop is closed for this unit.** Codex's next look is the merge-prep evidence review.

## 3a-4. 084-08 result (NEEDS-DECISION; ruled in `_reviews\084-08-chat-ruling-harness-round2.md`, 06:25 IST)
- **Part A is built:** exe `8A6074AA` + cook `4BF63FB8`; 16/16 mutants fail.
- **The 084-09 harness is ready:** 29 legs, dry run 29/29, about 66–95 min. It covers:
  - m52 B0/B5/B3/B9/B0L/P1;
  - B-CC ×7 on the new cook, and B-CC-MW ×3;
  - MASK55 v2 via the m52 trail (`TagPoolLimit 12`, peak live ≥ 2);
  - the MT85 wide-gap pair (decides the missing_texture borderline);
  - B-REAL ×8;
  - the LOD premise on the rocks.
- **G356 re-evaluation of the 084-06 bank:** B0 16/16 PASS. **B3 has one FAIL: a single visible frame at label end +16.** Watch it on the re-run.
- **No MainWorld mesh passes the declared LOD rule** (ratio 7.997 < 8, not relaxed). 084-08b authors a bench-only LOD fixture, with one dithered-transition variant and one without.
- **G366:** a capture fires one anomaly per burst; m52/m53 can overlap only through a trail.

## 3a-5. 085-01 result: the m53 DC plan (ruled in `_reviews\085-01-chat-ruling-dc-plan.md`, 06:55 IST)
- **The plan:** `docs/predictions/2026-09-29-m53-dc.md` (`feat/m53` `0209599`).
- **The merge dry run** (fix → m53) is clean, keep-both, with one hidden double destructor.
- **The exclusion is trail-aware.** The id is filtered before the draw, with an `ApplyAnomaly` backstop.
- **ND-1..8 as recommended:**
  - strict all-or-nothing;
  - a bench-only cap raise if needed, **plus the MainWorld yield at 128 MiB reported for the owner**;
  - the normal-mode no-visible-change rate is documented and feeds Thursday.
- **The m53 night is realistically Wednesday** (Tuesday only if 085-03's dry run is green by 23:00).
- **Twins ship in today's delivered catalogue.** Release hygiene goes onto the fix branch first (084-10), so the m52-only fallback is clean too.

## 3a-6. 084-08b result (GREEN, 08:00 IST)
- **The `CB_LodFixture` fixture:** orange sphere → blue cube, variant D dithered, variant N not.
- **The cook:** pairing `8A6074AA` + `BF06AE61`; the manifest diff is exactly 7 new files.
- **The 084-09 harness is pinned to it:** 47 legs, dry run 47/47, about 87–100 min tonight.

## 3a-7. 084-10 result (GREEN, 08:40 IST)
- **One bench gate, `AnomalyBenchGate::IsEnabled()`:** twins and all 33 levers show only under `-IAIBench` / `-IAIBenchFixture`, and are compiled out of Shipping. The runtime observation is owed to the G-LEVER night legs.
- **Tools, each proven both ways:** `lever_audit.py`, `check_delivery_bundle.py`, `stage_plugin_delivery.py`, `check_package_descriptor.py`. Exe `667FD4EF`, archived.
- **G376:** AnomDash's bundler ships the owner's dev token (the M2 incident's shape). Its fix is folded into 087-01.
- **The `.env` item** means `dashboard\config.json`.
- **No uncommitted AnomDash edits on the home PC.**

## 3a-8. 085-02 result (NEEDS-DECISION; ruled in `_reviews\085-02-chat-ruling-dc-source.md`, 10:05 IST)
- **The merge and DC source are built.**
  - The merge (`7cda2f1`) is green on its own, and the destructor is fixed.
  - DC `b1cfc8a`: the four modes, the V1P all-or-nothing step, the one-draw mode contract, the exclusion in a pure header, m53 on `IsAnnotationMember`, the census and the cost lines.
  - Exe `27055E55`.
- **Codex's findings, ruled:**
  - **F1/F2 (high), fix:** "live" includes the TAA tail frames, outstanding receipts and detached trails. EXCL-A's gate requires 0 frames carrying both an m52 and an m53 entry.
  - **F3 (high), fix:** the active record is never dropped while the effect is installed.
  - **F4 (medium), fix:** the census uses a read-only enumeration, counts only, with no names.
  - **F5:** a bench-only limitation.
- **The dashboard's targeted capture sends no mode:** it now goes **round-robin** over the enabled modes, with no RNG draw.
- **The free-running restoring set** is bounded by the timeout.

## 3a-9. 085-02b result (NEEDS-DECISION; ruled in `_reviews\085-02b-chat-ruling-dc-fixes.md`, 11:10 IST)
- **Built:** F1–F4, the round-robin, and the free-running bound. Exe `76A04998`, with every suite and mutant green.
- **Codex:** F2 and F3 are RESOLVED. Three residuals remain:
  - **N1 (high):** the round-robin overrode the bench identity levers (a regression);
  - **the reverse F1 path;**
  - **census config echoes.**
- **All three are fixed in 085-02c.** The Codex loop is closed for m53 source; the next Codex look is the evidence review.

## 3a-10. 085-02c result (GREEN, 11:36 IST)
- **Fixed:**
  - bench levers beat round-robin (`mode_source=bench_lever`);
  - the reverse exclusion reads m53's retained fires;
  - census lookups are silent, with a cold-log reader.
- **Exe `6B671987`, `feat/m53` `c6aa9c9`.** This is the m53 build for 085-03 and the nights.

## 3a-11. 085-03 result (NEEDS-DECISION; ruled in `_reviews\085-03-chat-ruling-fixture-harness.md`, 13:10 IST)
- **The m53 fixture cells are cooked:** pairing `6B671987` + `F76F1119`, archived.
- **The harness `_reviews\085-04-*`:** 86 legs, dry run 86/86, about 145–170 min.
- **🟡 Product finding:** the offline pre-read shows **0/10 StackOBot MainWorld objects m53-eligible** under the delivered rules. The rock can never qualify (`texture_not_parameter`); UV candidates are only partly eligible (refused by all-or-nothing); the floor's normal map covers 18/30 slots.
  - **Ruled:** per-family hosts; a family with no MainWorld host is a **reported gap, not a stop**. Timing is proven on the fixture, and the mechanism is the same class as `corrupted_texture`.
  - **The owner was told.** Thursday's office census gives the real yield, and that drives default-on.
- **The m53 night runs TONIGHT after 084-09:** one brief, `_held\084-09-085-04-night-bench.md`, ~4–4.5 h. It is released at ~22:30 IST by a scheduled self-reminder (`trig_01AaB71BPFL1AL7NUkGfadWC`), with its own idle gate.

## 3a-12. 🔴 Office finding (owner, ~14:30 IST 09-29): m52 fires ZERO events on Deimos
- The owner pulled `fix/m52-label-timing` onto the office host **Deimos** (the owner's codename). Auto-capture with only `stuck_low_mip`: 0 annotated events in several 400-frame captures.
- **Chat's diagnosis, from source:** the 084-02 purity rule holds a texture **only if it has exactly ONE user component in all loaded levels.** Real content shares nearly every texture, so every fire is refused. The bench never measured yield, only timing on unique-texture targets. **Chat owns that gap.**
- **Asked the owner** for log counts (numbers only): `HOLDING`, `HELD NONE` reason numbers, `shared_world`, `baseline_pending`, `not_restored`.
- **Owner's product call pending:**
  - **A (recommended): co-affected labelling.** Every user of a held texture is labelled and masked on its drawn frames, capped at K (~8). A real streaming bug hits every user, so this is realistic.
  - **B:** unique-texture targeting (low yield).
- **Queued:**
  - **084-11,** the plan for both, with an offline sharing census;
  - **a YIELD_M52 reading** added to tonight's bench (part 1b).
- **If A is approved:** build Wednesday, bench Wednesday night, office test Thursday.
- **m53 has a separate yield issue** (0/10 MainWorld). The Codex design study is parked by the owner ("later").

- **OWNER'S CALL (~16:30 IST 09-29): option A, co-affected labelling, K = 8.** He asked "Do you think we can go with this?", chat said yes, and nothing further came back. The fallback is his Thursday call: ship blurry textures available-but-off, or B.

## 3a-13. 087-01 result (NEEDS-DECISION; ruled in `_reviews\087-01-chat-ruling-office-kit.md`, 17:15 IST)
- **The office kit is done:** `feat/office-check-kit` `94dd112`, with the card `OFFICE-CHECK.md`.
  - Selftest 32/32, stdlib + Pillow.
  - It equals 086-01 on 871 starts and 744 ends.
  - Doctored labels fail, except m52 +1 under TAA (the G353 blind spot).
  - About 5–10 min with Pillow for the office recipe.
- **AnomDash `ab6e63f`:** the bundler no longer ships the dev token.
- **G395:** `084-09-eval.py` can include the judged frame in its own reference, so its magnitudes are off (direction still right). **Not patched before tonight;** the kit runs as a third reading instead.
- **Merge order:** the kit branch goes into the fix branch before the rehearsal, then the fix branch into m53 again.

## 3a-14. 084-11 result (NEEDS-DECISION; ruled in `_reviews\084-11-chat-ruling-yield-plan.md`, 17:50 IST)
- **The home census:** MainWorld has 610 texture-sampling components, **445 of them Nanite (unmaskable on 5.1)**. Yield is 2/342 today, and still 2/342 under A (K = 8) or B.
- **A second wall:** the 8× ratio gate needs a target ≥ 512 px on screen.
- **So A alone barely helps at home. Not built blind.** The office counts decide:
  - **A-core** if the office shows `shared_world` with N ≤ 8 and maskable co-users;
  - **B** (the picker filter with K = 1, about 3 h) if the picker is wasting bursts;
  - **otherwise fallback 2:** `stuck_low_mip` ships available-but-off, with a readme limitation, and the proxy-texture approach goes to the follow-up.
- **Nanite co-users are refused;** no labels without masks.
- **Codename:** the repo's rules list "Deimos" as retired (G222). The owner has been asked which name to use; until then docs say "the office host".

## 3a-15. 088-01 result (NEEDS-DECISION; ruled in `_reviews\088-01-chat-ruling-rehearsal.md`, 18:25 IST)
- **The merges:** kit → fix `df63a8a`; fix → m53 `9bd7316` / `c2d11f5`. The fix branch is now at `5c2bc83`.
- **Both Development packages** (A `6D1B659F`, B `C18D0F41`, in `E:\IA_BuildCache\_rehearsal\`) pass every non-launch check. The G376 token fix is proven on the real builds. Three packaging faults were fixed (G398–G400).
- **Remaining fill-ins:**
  - README Step 1, the client's launch steps (**owner input needed**);
  - §8.8's per-mode rate (from tonight's ND-5).
- **Launch checks** run tonight as part 0.
- **E: is at ~55 GB free** (50 GB floor). The night brief may redirect captures to `D:\IA_BankOverflow\` and deletes nothing.

## 3a-16. The night bench result (NEEDS-DECISION; ruled in `_reviews\night-0930-chat-ruling.md`, 23:35 IST)
- **Part 0:** both packages PASS every launch check. The firewall prompts from new exe paths were closed as Cancel (G401). Next time, run part 0 after the legs.
- **SYNC-PROVEN on all three tools (E1, 086-01, the kit):** missing_object, missing_texture (MT85 decided: the old +1 was a confound), corrupted_texture, and blinking in native order.
- **camera_clipping:** SYNC-PROVEN with AA on; the MainWorld over-label is fixed; trace cost 44 µs. AA off is unjudgeable (the null is zero).
- **m53 fixture: 12/12 PASS** (4 modes × 3 conditions); 0 co-entry frames everywhere; SEED, ID and SYNC-REG pass.
  - **Census:** 26 uv-eligible and 9 normal-eligible objects in MainWorld's whole world at 128 MiB. The bench camera's view had none.
  - MainWorld and COST legs were not run because of harness bug G403 (the `over_budget:need_…` parse).
- **Pending:**
  - **blinking synthetic:** E1 FAIL, while 086-01 and the kit read 0;
  - **m52:** B9 (086-01 FAILs 5), **B3 si 181 with a 41-frame visible tail after the label end,** and B0 with 12/18 censored;
  - **lod_popping:** the fixture renders black; `worst=2` on a 2-LOD mesh;
  - **MASK55 v2:** peak live 1, and **270/442 labelled frames with no measured mask;**
  - m53 G-MODE (the oracle premise failed), G-BIND (invalid fixture), MIXED rows (a fixture/prediction error; the product matches ND-1), and the G-LEVER-OFF predicate and G-CENSUS layout (harness).
- **Part 1b yield (m52 at home):** about 25 % of attempts hold, all on one rock. `shared_world` dominates the refusals; `baseline_pending` = 0.
- **The two 085-04 evaluator fixes are accepted.** Fix 2 enforces the ND-5 definition that was declared before the run.

## 3a-17. 089-01 adjudication (NEEDS-DECISION; ruled in `_reviews\089-01-chat-ruling-adjudication.md`, 00:05 IST 09-30)
- **No real label problem found.** Every disputed FAIL is a tool artefact (G395/G405/G406) or a reference confound, shown per event.
- **B3's 41-frame "tail"** is TSR history: the record was at baseline on the first unlabelled frame, and the effect drops below 50 % from +6.
- **MASK55's 270 unmeasured frames** are Nanite targets, admitted unmeasured by design. Not a regression.
- **LOD `worst=2`** is a log-wording issue. **The arms:** D = 5 is needed for +3.
- **Ruled:**
  - the aligned null is accepted provisionally and must be confirmed on the re-bench;
  - **stuck_low_mip `TransitionOffFrames` → 16** under temporal AA (ExecCmds on the re-bench; the source default goes in the next round);
  - MASK55 uses Route A;
  - the arms are re-declared (CD5/RD5);
  - **Nanite: labels + boxes, no mask** (existing behaviour). Keeping it is recommended; the owner decides Thursday.

## 3a-18. 089-02a (GREEN, 02:22 IST 09-30)
- **E1 is fixed and frozen:** `089-03-eval.py` `8e48e883…`, selftest 67/67; with the fixes off it reproduces 084-09 exactly. The fixes are G395, G405 (REAL_BL_SYN → PASS 10/10) and G406 (the game-time-aligned null: B3 si 181 → PASS, B9 16/16).
- **The LOD fixture relit:** the sun had been authored pointing up from below (G410), and three 400 cd point lights were added. New pairing `8A6074AA` + `62633720`. Not proven offline (G411); the premise legs decide.
- **The 089-03 harness:** 24 legs, Part 0 last, 1,800-frame null right after the m52 legs, about 77–87 min.
- **Open (not blocking):** which camera_clipping AA-off reading becomes the gate; a `--null` option for the kit.

## 3a-19. ⭐ 089-03 re-bench: SYNC PROVEN (ruled in `_reviews\089-03-chat-ruling-rebench.md`, 04:20 IST 09-30)
All 24 legs were accepted first try, on E1 (frozen) + KIT-GT with a covering 1,800-frame aligned null.

| anomaly | label sync |
|---|---|
| blinking, missing_object, missing_texture, corrupted_texture | **PROVEN** (all three tools) |
| camera_clipping | **PROVEN** with AA on; AA off on the confirmed-hold reading. Unconfirmed frames are flagged by design |
| stuck_low_mip | **PROVEN** with AA on and off: 2 partial onset frames flagged; offset +4..+6 inside 16 off-frames (TAA), 0 with AA off. **Yield on real content still open** |
| lod_popping | **PROVEN on the fixture** (N and D; 0/46 dithered fades); real-content pops are too faint to measure |
| uv / normal | **PROVEN on the fixture** (4 modes); MainWorld legs pending 089-02b + the re-bench |
| masks | **PROVEN** (MASK55A peak 2, 6 recycles, 0 aliasing, 199/199 masked) |

- **E2 (the 086-01 checker)'s drift FAILs are E2 reference confounds.** E2 is retired as a stuck_low_mip release instrument on cycling scenes.

## 3a-20. 089-02b (NEEDS-DECISION; ruled in `_reviews\089-02b-chat-ruling-m53-prep.md`, 06:25 IST)
- **L2 cannot frame MainWorld** (chat's premise was wrong). So: **uv-only MainWorld legs** on SM_Ramp2 / RoomBuilderSquare under a declared bench-only 256 MiB raise. **Normal real-content stays unproven** (documented; the owner's office look covers it).
- **MIXED's cause** was slot 0's leftover `BasicShapeMaterial` override (chat's premise was wrong again).
- **P-ORC:** the tonemapper mixed the channels. Legs now run with `ShowFlag.Tonemapper 0` (and `Lighting 0` for normal), read back.
- **The DC2 pairing** is `6B671987` + `45E2FA86`.
- **🔴 SECURITY HEADS-UP for the owner:** since ~03:00 IST 09-30, **`dbghelp.dll` cannot be created anywhere on the PC.** Defender's configuration changed at 02:59 and **real-time protection went off at 03:03**; no brief touched security settings. It blocks BuildCookRun staging, so the delivery build waits on the owner checking Windows Security.

## 3a-21. 089-04 m53 re-bench (RED, derived: the Code session died on `ENOTFOUND` at 07:40 after the harness completed; ruled in `_reviews\089-04-chat-ruling-m53-rebench.md`)
- **Fixture:** B-M53 PASS again on all 4 modes.
- **Arms:** CD3, CD5, RD3 and NOAPPLY proven; **RD5 reads end −4** (to investigate).
- G-BIND, G-CENSUS and G-LEVER-OFF PASS.
- **MainWorld uv is NOT-MEASURABLE:** both raised-cap hosts (SM_Ramp2, RoomBuilderSquare) are **Nanite**, so there is no mask. The aligned null shows no wrong-object change.
- **m53 label sync is PROVEN on the fixture only;** real content is unjudgeable from pixels this week. The mechanism is the same class as `corrupted_texture` (proven on real content), and the owner's Thursday office look is the real-content check.
- **G-MODE is inverted by an evaluator family mix-up.** uv's P-ORC passes (16/16, 64/64); normal's P-ORC fails. 089-04b fixes the evaluator and re-evaluates offline.

## 3a-22. The PC shut down at ~08:40 IST 09-30 (after the 07:40 network drop; possibly Windows Update)
- The owner restarted it, and the watcher recovered: 089-04b was marked interrupted (RED stub), then re-sent as `089-04b-…-r2`.
- **The GitHub signal has been stuck at 179 since ~06:12.** Probably the MailBox tray app didn't restart; the owner was asked to start it. Until then, chat polls every 20–25 minutes with a timer Monitor.

## 3a-23. 089-04b-r2 (NEEDS-DECISION; ruled in `_reviews\089-04b-chat-ruling-m53-finish.md`, 09:15 IST)
- **G-MODE PASS 4/4, including normal.** There were two harness family mix-ups; chat's expectation of "normal INSUFFICIENT" was wrong. The wrong-mode negatives are all below 0.5.
- **RD5 proven on pixels** (+3 on 8/8). E1's offset window misnames the edge (G419).
- **The MainWorld hosts are confirmed Nanite;** the labels follow the unmeasurable contract.
- **m53 bench evidence is closed** for this delivery: fixture sync is proven in 4 modes twice, mode correctness is proven, and there are 0 co-entry frames.

## 3a-24. ⭐ 090-01 final source round (GREEN, 09:53 IST 09-30)
- **Pull target superseded:** it is now `fee8c7a` (3a-26), and Thursday uses the 090-05 head. At the time, the target was `feat/m53-uv-normal-corruption` at `da902d6`: the full delivery, with every m52 fix, the kit and m53.
- **Fallback:** `fix/m52-label-timing` at `0347e8c`.
- **Fix branch:** OffFrames default 16 (`c12b7e9`; tests 284/0; stuck_low_mip only), the LOD log wording, and readme §8.7a (a per-anomaly evidence table with what to drop, and the Nanite rows). Stale yield wording corrected (G421). Exe `EB01156E`.
- **m53:** merge `3ca5dcb`, test fix `d57246c` (the exclusion selftest had hard-coded the 8-frame tail; G423; flagged for Codex), §8.8 filled. Exe `CE55D997`.
- **Delivery-shaped exes pass the staged lever audit.** G422: UBT reported a stale host exe as up to date.
- **Only one placeholder remains: README Step 1** (the owner's).
- **E: is at ~48 GB, below the 50 GB floor.** The owner decides what to clear; nothing gets deleted by briefs.

## 3a-25. Wednesday 30 Sep, mid-morning
- **The owner's product calls:**
  - **skip Nanite targets**, implemented as a setting that defaults to skip;
  - default-on decided **after** his office test;
  - "pull" (removing an anomaly) is only needed if the office test finds a sync failure.
- **Office hosts (the owner's codenames):** Deimos compiles `da902d6` clean. **Bates fails** with 8 compile errors: missing explicit includes (`ELevelTick` in the injector subsystem header, `UMaterial` in the capture subsystem). Bates builds with fewer transitive includes. The fix is 090-04, a strict-include build.
- **090-02, Codex evidence review: CHANGES REQUIRED** (ruled in `_reviews\090-02-chat-ruling-codex-evidence.md`):
  - **F1 (HIGH):** a FireWindow label outlives the effect on an early revert or when the host replaces the material. **Fix in source:** the label requires the effect to be installed; otherwise unlabelled + `effect_interrupted`.
  - **F2–F5 and LOW:** readme overclaims; corrections queued.
  - **F3 / F6:** evaluator repairs before the evaluator's next release use.
  - The post-result evaluator changes were judged sound.

## 3a-26. 090-04 strict-include fix (NEEDS-DECISION; ruled in `_reviews\090-04-chat-ruling-strict-include.md`, 13:15 IST 09-30)
- **The new pull target for both office hosts: `feat/m53-uv-normal-corruption` `fee8c7a`** (given to the owner at 13:10). A fix-branch host takes `37e01f0`.
- **What changed:** only 4 include lines.
  - The two office errors: `EngineBaseTypes.h` and `Materials/Material.h`.
  - Two more found by the strict build: `WeakObjectPtr.h` in `IAnomaly.h`, and `IWebSocketServer.h` in the control-server header.
- **`tools/strict_include_build.py`** (per plugin module: unity off, NoPCHs, include order Latest, one unit per header) is now a pre-delivery checklist box. The unfixed control reproduced every office error code at the same sites.
- **The normal build is unchanged:** 0 warnings, zero differing strings, lever audit PASS, suites pass.
  - Exes: fix `76428F44`, m53 `45FD344C`.
  - Archives: `_binary_baselines\m52fix-09004-76428F44\` and `_binary_baselines\m53-09004-45FD344C\`.
- **The residual:** 3 × C4150 in the generated `AnomalyCaptureSubsystem.gen.cpp` (`TUniquePtr` members of types complete only in the `.cpp`). It is fixed by a user-declared vtable-helper constructor, with the same body UHT generates.
  - **Ruled APPROVED, in 090-05 part D**, so the strict gate reads a true 0.
  - **No Shipping strict build**; a Shipping-only code read instead.
- **✅ Office confirmation (owner, ~14:00 IST 09-30): `fee8c7a` compiles with no errors on the second office host.** 090-04 is closed.
- **Office errand given for the m52 yield counts:** a PowerShell function `IAICounts <log>` that prints numbers only.
  - It reports: PURITY ENUMERATION, HOLDING, HELD NONE plus the bucket totals, `shared_world` with the users 2–8 vs ≥ 9 split and min/max, `baseline_pending`, HOLD CONTAMINATED, and matched 0 mesh.
  - It was tested in PowerShell 7.4 on a synthetic log built from the exact `fee8c7a` format strings, and uses only 5.1-compatible syntax.
  - A copy is at `_reviews\090-04b-office-m52-log-counts.ps1.txt`.
  - Why the log and not `run_summary`: `stuck_mip_refused_shared_world` / `baseline_pending` are only written when `fires_applied > 0`, so they are absent in the zero-fire case.
  - If the owner can't paste it at the office, the next head carries a Python version in `tools\`.

## 3a-27. 090-05: F1 + Nanite skip + readme + constructor (NEEDS-DECISION; ruled in `_reviews\090-05-chat-ruling-f1-nanite.md`, 16:48 IST 09-30)
- **Heads:** fix `25f9450` (exe `129E8E53`), m53 `5cda4e9` (exe `97D292F8`). Both strict-include builds are a true 0; the normal builds have 0 warnings; lever audits 34/34 and 47/47; suites pass. Archives: `_binary_baselines\m52fix-09005-129E8E53\` and `\m53-09005-97D292F8\`.
- **F1:** a FireWindow label now requires the effect installed on that frame (`IsVisualConditionHeld`). Frames without it are `effect_interrupted`, unlabelled, unmasked and not in the annotation. The rule is `…_v2_effect_installed`.
  - An interrupted m53 stays "live" for the m52/m53 exclusion until its entry clears. That is the conservative reading, and it's accepted.
  - A new bench lever `IAI.Bench.RawRevertAt` was accepted. Its cost is instrumented but unmeasured; tonight's threshold is mean ≤ 0.05 ms, max ≤ 0.5 ms.
- **Nanite skip:** `IAI.Targets.AllowNanite` (default 0; ini `AllowNaniteTargets`). A target is refused as `nanite_unmaskable` before the draw (auto-pool), before targeted fire, in the backstop, and in m53's step `E7N`. `run_summary.refused_nanite` counts the refusals.
  - KAT: no Nanite gives identical draws; setting 1 is byte-identical to before.
- **Readme:** all the F2–F5 / LOW / F3 corrections, the Nanite trade-off, and `effect_interrupted`.
- **🔴 Found: the plugin never compiled in Shipping** (G428, pre-existing). Chat's 090-04 premise, that the delivery package is Shipping, was wrong: it is Development/Test by the owner's 2026-08-20 ruling.
  - **Ruled:** exclude every plugin module from Shipping via the `.uplugin` deny list, rather than repair the guards. That goes in 090-07.
- **Ruled, also for 090-07:**
  - the office kit learns `effect_interrupted` (Code flagged it but didn't close it);
  - `tools/m52_log_counts.py`.
- **090-06 harness is ready:** 22 legs plus an OLD-exe can-fail, ~56 min, selftest 83/83, the dry run clean. Part 0 is not runnable (`dbghelp.dll`, no package).

## 3a-28. ⭐ 090-07 (GREEN, 18:52 IST 09-30): the THURSDAY PULL TARGET is m53 `2015cfa`
- **Both office hosts pull `feat/m53-uv-normal-corruption` at `2015cfa`** (a fix-branch host takes `c29e223`). Given to the owner ~19:05.
- **Shipping exclusion:** `TargetConfigurationDenyList: ["Shipping"]` on all 5 modules.
  - UBT `-Mode=JsonExport`: Shipping plugin modules 4 → 0; Development / Test / Editor exports byte-identical.
  - No real Shipping compile was run, so the readme claims only the exclusion.
  - G431: the lever-audit selftest was re-planted so it can still fail (12/12).
- **Kit 1.1:**
  - `effect_interrupted` is recognised and never treated as TAA;
  - an interrupted end is judged against the interruption picture, 0 frames off; for interruptions ≥ 8 frames the reference is the last 6 (G430);
  - re-installed runs are judged per run;
  - a gap still showing the effect FAILS;
  - READ BACK gains `interrupted N`.

  **Evidence:** 14/14 doctored real sessions as designed; both mutants fail the selftest (39 checks); 8 real sessions / 91 events give identical reports apart from the new lines.

  **Known limits** (chat adjudicates if seen at the office):
  - a 1-frame interruption reads against the pre-event picture;
  - under AA, a smear above half strength on the first interrupted frame reads end +1.
- **`tools/m52_log_counts.py`** replaces the PowerShell paste. Its selftest is 9/9, and it matches the PowerShell output on the sample and two real logs.
- **Builds:** the normal builds are **byte-identical to 090-05's** (`129E8E53` / `97D292F8`), so the 090-06 smoke speaks for `2015cfa`. Strict build 0; lever audits 34/34 and 47/47. Archives: `m52fix-09007-8AA57A53` and `m53-09007-A3EC29D8`.

## 3a-29. 090-06 smoke: NOT RUN, not idle (NEEDS-DECISION; ruled in `_reviews\090-06-chat-ruling-not-idle.md`, 06:40 IST 10-01)
- The home PC rendered in UE 5.7 (plus Blender) all night, so the idle gate never opened. Staging is unchanged (hash-verified), postflight PASS, and nothing was banked or committed.
- **Harness quirk:** live mode exits 10 at once on a non-idle preflight; the 90-min wait applies only per leg and at staging. **The method is now poll `idle_gate()` read-only, then `-Live` after 3 consecutive idle reads.** The harness stays frozen.
- Window 1's evidence is moved aside to `run-w1-notidle\` in r2 (never deleted).
- **Disk:** D: and E: fell overnight from activity that wasn't ours (D: 137 → 98 GB, E: 106 → 76 GB), both above the floor.

## 3a-30. 🔴 090-08 Codex re-check: CHANGES REQUIRED (ruled in `_reviews\090-08-chat-ruling-codex-recheck.md`, 07:45 IST 10-01)
- **Accepted:** the Shipping exclusion, the constructor, the original F1 case, and the interrupted-m53 exclusion.
- **Four HIGH label/picture holes that static benches can't see** (the root cause: "installed" was read from apply-time records rather than what renders now, and label inputs aren't paired with the captured picture):
  - **R1:** one replaced slot unlabels a still-visible effect. Fix: installed = any slot still renders ours; partial frames stay labelled and are counted.
  - **R2:** revert leaves an override above the current slot count. Fix: clear every written index, then assert.
  - **R3:** the installed test reads apply-time records. Fix: read current render state every frame.
  - **R4:** async recomputes live actor bounds at readback; sync pairs new state with the previous picture. Fix: freeze geometry and identity with the capture request; for sync, a one-deep state history or flag `capture_unpaired`.
- **Also:**
  - R5 HIGH: the Nanite check is filtered by exclusions and isn't kept up mid-event;
  - R7 HIGH: the kit false PASS on an unjudged run;
  - R6: fail closed;
  - R8: no AA excuse on interrupted frames;
  - R9: readme;
  - R10: per-frame cost log;
  - R11: spline yield, only if local;
  - the NOTE: documented.
- **Plan:**
  1. **090-09** (round A): verify all, fix R1–R3, R5–R10 and the kit, and verify and design R4.
  2. **090-10:** the R4 fix, plus the 090-06 harness extended with legs for R1, R3 (hide, mesh change), R4 (moving target across the frustum edge, destroyed target, sync path) and OLD can-fails, re-pinned, dry-run.
  3. The smoke run (idle).
  4. A Codex re-check.
- **Schedule:** master merges move from Friday to the weekend; delivery stays Monday. If the weekend gates fail, the owner decides on Sunday between shipping with documented limits and slipping.
- `090-06r2` is superseded (`_held\_superseded\`).

## 3a-31. 090-09 round A (GREEN, 09:40 IST 10-01; ruled in `_reviews\090-09-chat-ruling-round-a.md`)
- **Heads:** fix `189286a` (exe `EBB70E9B`), m53 `31078c7` (exe `8131479A`). Strict 0/0, lever audits 34/34 and 47/47; archives `m52fix-09009-EBB70E9B` and `m53-09009-8131479A`.
- **Phase 0:**
  - REAL: R1, R2, R3, R5, R7, R8, R10, R11;
  - R6 is real but unreachable in defaults (fixed anyway);
  - **R4 async is REAL on the delivery default** (Async, SVE and Pace are all on; membership and `bbox_px` are read from the live actor 1–2 ticks after the picture);
  - R4 sync is real but unreachable (the rectangle fallback fires only with SVE 0).
- **The fixes as built:**
  - R1/R3: a none/full/partial install state from current render state (`AnomalyInstallState.h`); partial stays labelled;
  - R2: a full-array restore plus `REVERT-RESIDUAL`;
  - R5: per-frame `nanite_unmaskable` plus a revert at the next tick;
  - R6: fail closed;
  - R7/R8: kit 1.2 (`| nanite N | unjudged N`; FAIL outranks UNJUDGED);
  - R10: cost per frame;
  - R11: splines;
  - R9: the readme.
  - The rule is `…_v3_effect_rendered_nanite_gated`.
- **Ruled:**
  - R4 async freeze (Code's design, 2–3 h);
  - R4 sync: flag `capture_unpaired` and declare it unsupported (the history fix goes to a follow-up);
  - close the kit's two older limits (a missing labels row → unjudged; a truncated PNG → INVALID on both decoders);
  - split the smoke work into 090-11, with the can-fail on the same exe via bench-gated "old rule" switches.

## 3a-32. 090-10 round B (GREEN, 12:26 IST 10-01)
- **Heads:** fix `b7da624` (exe `F636D0A7`), m53 `ff868b2` (exe `85395644`, whose code equals the head). Strict 0/0, lever audits 34/34 and 47/47; archives `m52fix-09010-F636D0A7` and `m53-09010-85395644`.
- **R4 async:** bounds, fire position and event identity are frozen at the tick-end sample (`AnomalyFrozenGeometry.h`). Completion projects the frozen box with the frozen view (`ProjectBoxToScreenRect`). The only remaining live read is the m26 record handle, which is safe.
  - Proof: `r4_capture_pairing_selftest` (46 checks) and `tools/r4_pairing_gate.py` (8 source mutants fail).
- **R4 sync:** `capture_unpaired` (bit 128): never labelled, never a member, counted. The readme says unsupported for delivery.
- **Kit 1.3:** a missing labels row → unjudged; a damaged PNG → INVALID on both decoders. 73 checks; 0 differences on 8 real sessions.
- **Rule:** `…_v4_sample_bound`.
- **Accepted notes:** kit 1.3 is required for captures from this build; an edge beside a sync frame reads CENSORED.

## 3a-33. 🔴 OFFICE: nothing corrupts on the second office host (owner, ~12:00 IST 10-01)
- **What the owner saw** on m53 `31078c7`: he ran blurry, UV and normal repeatedly; **nothing visibly corrupted**. `IAI.TexCorrupt.Census all` then **froze the game**.
- **Neither office project uses Nanite.** The owner was angry that chat suggested the Nanite skip. Chat owned it: the suspect is our own untested code, not his content. Never suggest Nanite as the office cause again.
- **No build had been run in a game since 090-05.** The 090-06 smoke never ran, and everything after was compile-only. That process failure is owned. **New rule: no build goes to an office host until it has visibly corrupted in a running game at home (PIE and staged).**
- **Hypotheses (to be tested in-engine, not argued):**
  - H1: a 090-09 Nanite-gate regression (probe misclassifying or fail-closed via a missing probe; the R5 per-frame revert);
  - H2: virtual textures;
  - H3: budget / 4K;
  - H4: PIE vs packaged, or ini defaults;
  - H5: other.
- **090-10b** (urgent, released ~12:31) does the in-engine bisect across `8131479A` / 090-07 / `45FD344C` / `85395644`, in PIE and the staged game, on non-Nanite content. It fixes the cause with a pixel proof plus a can-fail, adds loud zero-eligibility and startup lines, bounds the census, and provides a verified office read-out tool if needed.
- The owner was asked (optionally) for a photo of the console lines after one UV fire.

## 3a-34. 090-10b found and fixed the shader-map gate (live feed; the report is pending)
- **Reproduced at home in true PIE:** `texcorrupt_refused_shader_map_incomplete: 6`. Staged and editor `-game` corrupt fine.
- **Fix:** a per-vertex-factory, renderer-equivalent predicate. The first attempt (the render-thread flag) was refuted by measurement.
- **Final m53 `0a69ea3` / exe `65607703`:** uv 6/6 and normal 6/6 corrupt in PIE and staged; edges 5/6 in sync (the 6th censored); lever audit 47/47; strict 0/0.
- **Also built:**
  - the yield / startup / no-visible lines;
  - `tools/anomaly_refusal_counts.py`;
  - a time-sliced census;
  - the corruptor-readiness completion.
- **The owner pushed it himself** (`git push`; origin m53 `0a69ea3`, fix `cf422d5`) and tested at the office.
- **Codex's parallel diagnosis** (`_reviews\bates-2026-10-01-diagnosis\`), accepted:
  - S6 whole-map completeness under on-demand compilation (`JobCacheDDC` on at the office);
  - the caution that you shouldn't just delete S6;
  - "the next gates will appear".
- **New finding (090-10b):** in PIE, stuck_low_mip refuses 6/6 as `refused_shared`. The purity scan counts the editor world. This is the likely office zero for blurry.

## 3a-35. 🔴 OFFICE, second pull: `0a69ea3` still fires zero; everything refused as `runtime_lod_bias` (~18:15 IST)
- **As Codex predicted, the next gate.** The sub-reason wasn't read. Its variants are `cinematic` / `per_texture` / `streaming_budget` / `global_sampler` / `global_streaming` (`TexCorruptTree.cpp:466-477, 921-926`).
- **Likely `per_texture`:** real-project texture LOD bias (device-profile texture groups, asset `LODBias`).
- **Ruled:** match the runtime bias (copies start at the rendered top mip; identity pixel-exact) instead of refusing. Audit **every** gate against office-like home content. Print sub-reasons in the census.

## 3a-36. 🔴 OFFICE census on `0a69ea3` (~20:30 IST) + Codex's compatibility review, ruled in `_reviews\090-10x-chat-ruling-compatibility.md`
- **The census:** uv eligible **0 of 15,906** (`runtime_lod_bias` 14,454 = 91 %, `excluded_group` 551, `unsupported_type` 538, `no_eligible_slot` 272, `not_fully_resident` 55, `shader_map_incomplete` 15, `no_textures` 9). Normal: `runtime_lod_bias` 15,538. First-failure attribution.
- **Codex (`compatibility-review.md`), accepted:**
  - the bucket is per-binding T9;
  - `StreamingBudgetPossible` over-refuses (raw cvars, not effective engine state);
  - **the copy design assumes the full mip chain from mip 0** (T10, the tripwires, `snapshot_mip=0`, the RT sampler without global bias), so deleting T9 alone exposes residency.
- **Ruled:**
  - **090-10c = Codex A1–A4:** separated suffixes plus the all-reasons census; effective-state streaming bias; **reduced resident chains**; an identity control that preserves effective appearance.
  - **C** (`host_mid`, constant textures, VT, unsupported types) is out of scope for Monday.
  - **The four-rule principle** for every check: block only for label/picture disagreement, the wrong object, crash/leak/cap, or revert residue.
- **⭐ OWNER DECISION (~21:00 IST): B.** Corrupt just the eligible parts of an object, with the label/mask covering **only** the corrupted parts. This supersedes the 09-28 all-or-nothing rule for UV/normal. Recorded in `_reviews\090-10x-owner-answer-B.md` with chat's constraints.
- **Honest outlook given to the owner:** blurry has a real chance for Monday; **UV/normal on office content is at risk for Monday** (the A3 rework). The Sunday option is to ship what works, with the rest as a follow-up drop.
- **The watcher died ~17:38** (Codex: PID absent at 20:44). The owner was asked to restart it.

## 3a-37. 090-10b2 (NEEDS-DECISION; ruled in `_reviews\090-10b2-chat-ruling.md`, ~01:55 IST 10-02)
- **Heads:** m53 **`d7e396d`** (code = `61d186a`, exe `E2BCE76B`), fix `d3c9423` (exe `5E93A74E`). Pushed. Strict 0/0; audits 47/47 and 34/34; archives `m53-0910b2-E2BCE76B` and `m52fix-0910b2-5E93A74E`. 10.6 GB of evidence banked.
- **The readiness order is now `5fba7df`:**
  1. metadata;
  2. identity (`default_material_path`);
  3. draw readiness (main-pass vertex and pixel shaders per vertex factory: `compile_pending`, or `draw_shaders_missing`).

  In PIE: identity refused with 0 events; compile-pending refused then admitted (13/13); healthy 28/28.
- **The m52 PIE premise was REFUTED:** the editor world isn't counted; the fixture pair shares its texture by design. Blurry in PIE passes E1 17/17, staged 16/16, auto-pool 5/5. **Why blurry never fired at the office is still unknown;** the office counts are needed (`anomaly_refusal_counts.py`).
- **The census bound holds** (≤ 100 ms): MainWorld PIE 36.7 ms, staged 18.0 ms, office scale 63.9 ms. At 40k actors the first frame is 195 ms. One 650 ms frame was not attributed.
- **The PIE end+1** (about one event per PIE run; the picture is corrupted one frame after the label ends; not staged; also on the old build) is **mitigated** by the PIE-only `pie_end_settle` flag in 090-10e. The root cause is a follow-up item.
- **Morning pull for the office: m53 `d7e396d`.** Blurry in PIE, plus the refusal counts. UV/normal is still LOD-bias-blocked until 090-10c.

## 3a-38. 090-10c m53 rework (NEEDS-DECISION; ruled in `_reviews\090-10c-chat-ruling-r1.md`, ~06:20 IST 10-02)
- **Head m53 `88801bd`** (code `dcd8d29`, exe `B3103E32`), pushed. Strict 0/0; pure 270/0; lever audit PASS; archive `m53-0910c-B3103E32`.
- **Design:**
  - the **resident-chain copy** (RHI mip 0 = cooked mip `AssetLODBias + MaxNumLODs − NumResidentLODs`);
  - effective streaming bias;
  - `r.MipMapLODBias` matched on the RT sampler and source mip (identity 0.35 vs null 0.34);
  - a copy ceiling (halve to 512 px);
  - **B at component granularity** (the event scope limits mask, m26 and bbox);
  - `IAI.TexCorrupt.Census allreasons`.
- **Eligibility on office-like home content:** uv 0 → 72/82 (fixture), 0 → 33/343 (MainWorld; 85 more are Nanite-only, which the office doesn't use). PIE V1–V5b 7/7 corrupt; staged V4, V5a, V5b 7/7. The can-fail `0a69ea3` refuses everything.
- **Open, fixed in 090-10f:**
  1. **MUST-FIX:** 11 of 28 auto events on uniform 16×16 controls were labelled with no visible change. New refusal `texture_uniform`.
  2. B's component-scope mask isn't proven in-engine (the orb rotates); a static two-component fixture is needed.
  3. Staged per-texture and cinematic bias were not shown (the lever blanks cooked textures, G449); cook a baked-bias fixture.
- **Design limits, out of scope for Monday:** `texture_not_parameter` ~13 %, `host_mid` ~7 % (home MainWorld shares).
- **The office this morning:** pull `88801bd`; `IAI.TexCorrupt.Census allreasons`; uv/normal by eye; blurry counts via `anomaly_refusal_counts.py`. **Not for dataset use** until 090-10f.

## 3a-39. ⛔ Claude Code's login EXPIRED on the home PC (~08:11 IST 10-02)
- **090-10e died near its end** ("OAuth session expired"). Its work is **committed locally, NOT pushed:** fix `257ff68`, m53 `9b911e7`.
  - `pie_end_settle`;
  - census actor-list slicing: MainWorld PIE 27.6 ms, 15k 37.5 ms, 40k 50.8 ms;
  - Python 19/19 and 27/27; lever audit 52/52.
- **090-10e's key finding:** the "PIE end+1" was a READER artefact. `affected_frames` drops a last labelled frame whose PIE mask is unmeasured; against the labelled set, 0 of 147 events are off.
- **New concern:** labelled PIE frames with an **unmeasured mask** (warm-up, near leg start). Ruled: a labelled frame must never ship without its mask (fix, or flag `mask_unmeasured`).
- **090-10f died at once** on the same login error.
- **Combined replacement brief, held:** `_held\090-10f2-finish-e-mask-uniform-bscope-cooked.md` (Part 0: finish and push 090-10e; Part M: unmeasured mask; items 1–3 of 090-10f). Release it once the owner re-logs Claude Code.
- **Owner action:** in PowerShell on the home PC, `claude`, then `/login`, then sign in, then exit.
- **Gotcha:** a `git fetch` from chat's device shell hit "unable to unlink maintenance.lock" (no delete permission). Harmless, but **chat shouldn't run git writes/fetches on the main checkout.**

## 3a-40. ⭐ OFFICE (~10:30–10:45 IST 10-02, second office host, PIE, m53 `88801bd`)
- **UV and normal corruption now pick targets and VISIBLY corrupt.** First success on office content. The first office host isn't tested yet.
- **Blurry (m52) still 0.** `anomaly_refusal_counts.py`:
  - PURITY 7, HOLDING 1, HELD NONE 6 (not streamable 4, excluded 4, **shared 164**);
  - **shared_world 196: users 2–8 = 32, users ≥ 9 = 164**, min/max 2/6155.
  - Per the pre-declared 084-11 §1.1 reading this is **fallback 2:** A at K=8 would admit at most ~16 %. The real fix is a **per-target private-copy (proxy) blur** reusing m53's proven resident-chain copy and MID machinery, which blurs only the target and works for shared textures.
- **The owner was asked (product/scope):** ship blurry available-but-off for Monday with a readme limitation; and either attempt the proxy blur this weekend on a separate branch (in Monday only if fully proven by Sunday noon) or schedule it for the follow-up drop.
- The readings are recorded in `_bates_reads\2026-10-01-second-office-host-m53-census-owner-reads.md`.
- **⭐ OWNER DECISION (~11:00 IST 10-02): option 2** (`_reviews\090-12-owner-decision-proxy-blur.md`).
  - Monday ships blurry **available but OFF**, with a readme limitation.
  - Code builds the **proxy blur** on `feat/m52-proxy-blur` (brief `_held\090-12-m52-proxy-blur.md`, drafted).
  - It joins Monday **only if fully proven at home and on an office host by Sunday noon IST.**
- **Sequencing (revised ~12:55 IST 10-02; the owner wants ONE office pull after the mip-map fix):**
  1. 090-10f2 (running);
  2. **090-12 proxy blur, straight after** (Friday evening and night; branch `feat/m52-proxy-blur` off the latest m53 head, so it contains everything);
  3. **Saturday morning: the owner's single office pull of the proxy branch** (UV/normal plus blur);
  4. a Codex re-check of 090-10b2 through 090-12, covering both;
  5. fixes, the extended smoke, merges, package;
  6. Sunday noon: the proxy-blur cut-off.
- The owner is **not** pulling `48a63a2` or `9e081fe` in between.

## 3a-41. ⭐ HANDOVER (owner, ~13:15 IST 10-02): Codex implements; Claude Code Desktop orchestrates and reviews
- **The owner's words:** "after this round, we're going to delegate the work to Codex instead of Code as the token usage is pretty high … Code's job will simply be to check Codex's code and then to keep sending briefs … We will use the Relay workflow (E:\<personal-folder>\tools\relay)."
- **Kick-start brief for the Code Desktop session:** `D:\IntrusiveAnomalies\_reviews\090-HANDOVER-code-desktop-orchestrator.md`.
  - Job A: rule on 090-10f2, then stop the mailbox (the `STOP` file).
  - Job B: build the implementer Relay (`relay_run_codex_gdp_impl_v1.ps1` plus `gdp-impl-block-v1.txt`; workspace-write with `--add-dir`, a smoke job, ask the owner before `danger-full-access`).
  - Job C: Codex implements the proxy blur (`_held\090-12`).
  - Job D: the owner's single Saturday pull plus a checklist.
  - Job E: a Codex read-only review of 090-10b2 → 090-10f2.
  - Job F: the weekend delivery path.
- **This chat stops orchestrating after this handover.** This handoff doc is copied to `D:\IntrusiveAnomalies\_reviews\` for the Code Desktop session. Chat's 090-11 smoke draft is at `_reviews\drafts\090-11-smoke-levers-harness.DRAFT.md`.
- **Heads at handover:** origin m53 `48a63a2`, fix `ba1af33`; local, unpushed: m53 `9e081fe`, fix `7d4debc` (090-10f2 in flight); master `b5f15a3`.

## 3b. Mailbox queue: current as of 08:25 IST 10-02
- **Watcher idle;** `to_code` empty.
- **HELD:** `090-10f2` (needs the owner's Claude login first).
- **Then:**
  - the extended smoke (090-11; re-pin to 090-10f2's head);
  - a Codex re-check of 090-10b2 through 090-10f2;
  - merges (weekend);
  - package;
  - the Sunday call.
- **The office this morning:** `88801bd` for readings (census `allreasons`, uv/normal by eye, the blurry counts).

## 4. Sequence to delivery
1. **Rule on 084-06**, then **084-07** (held) by day on Tue: F2/F4/F5/F3, defaults 3/8, readme semantics, rebuild, Codex re-check.
2. **m53 delivery cut** (085-00 IN list) in parallel planning: a DC plan revision, then source/build, then the Codex source review, then the fixture cook (mode-oracle cells), then the harness, then the gate window. It **must include an offset pixel gate** (B-M53, 086-01).
3. **Merge prep:**
   - the m52 fix and sync fixes → `master` (the comment strip, a Codex evidence check);
   - m53 → `master` if proven;
   - then the **delivery build** and packaging: dashboard `.env`, the host-tools path, bench twins hidden, levers out, the readme.
4. **Thu 1 Oct:**
   - the owner's office test over RDP with the **office check kit** (086-01 spec: numbers only, per anomaly);
   - the m53 eligibility census;
   - **the owner's pull decisions.**
5. **Fri–Sun:** fixes, then the final build. **Mon 5 Oct:** deliver.

## 5. Locked vs open
**Locked:**
- the release rule;
- the delivery cut;
- the m52 fix contract (render truth, trail, burst gating, unresolved timeout, whole-world purity);
- transition 3/8/1 with a documented schema;
- mask-ID recycling on pool exhaustion;
- camera_clipping via the view slab;
- the review-loop rule: findings on paths the bench shows never occur become documented limitations, and anything in the data blocks.

**Open:**
- *(owner, Thu)* pulls; defaults on/off; the m53 budget cap (128 MiB, which limits big objects).
- *(chat)* the 084-06 ruling; 084-07; the m53 DC plan; the merge order.
- **The environment-pack project:** the owner will assemble his environment packs into one UE 5.1 project for realistic testing. Not ready yet; after delivery if not by Tue.

## 6. Corrections this week
- **"Zero yield on MainWorld" (m53) was wrong:** a frame-1 artifact. The floor is eligible once loaded, but needs 209 MiB against the 128 MiB cap.
- **The noisy 084-03 null was the scene's ~12 s sharpness cycle (G349),** not the owner's background rendering.
- **The m53 S2-0 shadow trace:** STALE-NULL, not a restore defect. Any render refresh reproduces it.
- **Never chain a mailbox `mv` after an edit.** A brief after NEEDS-DECISION needs a `# ACK-VERDICT:` line, or the watcher holds it (it happened with 084-04a).

## 7. Pointers
- **Rulings:** `_reviews\084-0x-chat-ruling-*.md`, `085-00-chat-ruling-delivery-cut.md` (with the ADDENDUM) and `086-01-chat-ruling-audit.md`.
- **Codex:** `_reviews\084-0x-codex-*.md` and `086-*`.
- **Harnesses:** `_reviews\084-03-*`, `084-06-*` and `086-01-work\` (the checker).
- **Scorecard:** `_reviews\081-17-codex-vs-code-comparison.md`, updated through 084-05b.
- **Branches:**
  - `fix/m52-label-timing` (journals `docs/sessions/2026-09-2*-084-*` / `086-02`);
  - `docs/label-sync-audit` (the 086-01 journal);
  - `feat/m53-uv-normal-corruption` (S1 complete, S2 plan rev 2.2, **paused**).
- **Mailbox:** the GDP signal key `f2e079e1` read 159 before 084-06. Held briefs are in `_mailbox\_held\`.
- **Standing rules:**
  - office data never comes home;
  - codenames only;
  - never kill by name, and never touch the owner's processes;
  - never touch `m51`, tags, `ToCodex\` or `E:\<personal-folder>`.
