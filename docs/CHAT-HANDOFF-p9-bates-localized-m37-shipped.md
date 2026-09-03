# CHAT HANDOFF — P9 localized to the Bates host path · (A) closed · m37 shipped · m38 approved
**Date:** 2026-09-02 (session 067 close) · **Supersedes:** CHAT-HANDOFF-m36-census-and-bates-results.md wherever they disagree.
**Confidentiality:** office hosts are referred to by codename ONLY (Concorde, Bates). Never a real title, studio, product, packaged-exe, or fork name — anywhere: code, docs, journals, predictions, commit messages, handoffs, briefs, memory, replies. `AnomalyCapture.Build.cs` is the single permanent owner-ruled exception (fork-detection needles are functional and UNALTERABLE). The scrub verifier's self-test decodes the term table to stdout BY DESIGN — never paste raw verifier output anywhere; quote only `SELFTEST ok` and `VERDICT` lines (G202).

---

## 0. READ THIS FIRST — one item may not have landed

At session close, chat issued a **close-out brief** (docs-only) that lands the owner's second Bates test into the repo. **Verify it executed before anything else** (the owner may or may not have pasted it):

- Ledger §8.6a carries: the menu-off test paragraph ((A) CLOSED), the clean-binary (B) sets, the axis table, and the TRANSITION ARITHMETIC paragraph.
- Card: C-1 marked SUBSTANTIVELY DONE BY OWNER; C-3 promoted to the decisive item with prerequisites (FRESH run · `Log LogAnomaly Verbose` pre-typed · AA off) and the pre-declared three-way comparison; older sections A-6/A-7/B-2 use the `<TITLE-SAVED-LOGS>` placeholder instead of the hard-coded bench log filename.
- Journal 067 has its closing sections; CLAUDE.md status block refreshed (m37 done, m38 approved-unimplemented, m39 = bbox, staged exe 6C80E872, tag sequence …→m37).

If any of that is missing, the close-out brief content is reproduced in §9 of this file — it is the first paste of the new session.

Separately: the **transition-driver source read was NEVER EXECUTED** (the brief containing it was not delivered to Code). It is next session TASK 1 — see §6.

---

## 1. Current state (verified at close)

- **master == origin**, tip at session close `26170a8` (close-out commit will advance it). Standing invariant (in CLAUDE.md after three stale-SHA readings): master = the m34+m35+m36+**m37** merge/commit line; the tip is whatever `git rev-parse origin/master` says — never trust a remembered SHA.
- **Staged bench exe `6C80E872`** (241,036,800 bytes), built from `D2BB25A5` source + m37; **exe-only swap** — container quartet unchanged (utoc 2A66CA57 · ucas A7EF9B12 · pak D8009AD7 · global C70ECDAA/A16A18A8). `D2BB25A5` freeze RETIRED but the binary stays load-bearing as P-C7's A-side. Both in `_binary_baselines\README.md` (outside version control — G112; drift class noted).
- **NO TAGS** (highest m30). Office batch tag sequence: **m31→m33→m34→m35→m36→m37** (+m38 if landed by then).
- **Census still compiled OFF** → master remains client-inert; P-C7 byte-identity **re-anchored at 6C80E872** (gate (a), PASS — including "no census_* keys when off", so the client key set did not move despite the new counter).
- **m37 SHIPPED** (commits 906d35a docs + d257f7b feat(census)): floor 0.5 compiled default; NEW `CensusMaxDrawnCoveragePct` default 25.0, **INCLUSIVE band** (eligible iff floor ≤ coverage ≤ ceiling); above-ceiling → EXCLUDED categorically with `census_above_ceiling` counter + per-exclusion token `Census: ABOVE-CEILING '<actor>' … EXCLUDED (label unusable at scenery scale, not a failed anomaly)` + histogram note; console `IAI.Capture.CensusCeiling`; **ceiling ≤ 0 = DISABLED, loud in the StartRun echo both directions**. Gates (a)–(d) all PASS. Comparability: census-ON is a new G140-family baseline from 6C80E872; census-OFF stays comparable.
- **Milestone map (two deliberate chat renumbers):** m37 = census defaults (DONE) · m38 = run-scoped session log (plan APPROVED, unimplemented) · **m39 = honest bbox** (P-C13 conjunct-2 required gate rides it regardless of number).
- **Branches:** origin = master + feature/mask-gpu-reduce (7151875) + feature/selection-census (7f82d52). Local-only: feature/stencil-capture 76cac74 (verifier-CLEAN over 98 files; never push, never check out). The two dirty GATE-FAILED locals are DELETED — **"no reachable ref, origin OR local" is RESTORED** and re-verified.
- **CaptureBench `dcc056a`** (local-only, no remote): `tools/p9_hidden_set.py` (the P9 reader; SEP_RATIO **5.0 frozen**; MARGIN_FLOOR 0.5; d-unit classifier checks; SHIFTED reachable only when window slack ≥ |k|; join = target-name + window-overlap because annotation says "blink" while labels say "blinking" — two vocabularies by design), `-LetterboxedFixture` (asserts non-zero rect origin from the artifact; B1 declared N/A there, never "passed"), synchronous `anomaly_log_excerpt.txt` banking per leg (mtime header + line count; UNAVAILABLE file if source log missing; BOM-free UTF8 — G207), `-VerboseAnomalyLog` switch (default OFF).
- **Bench render config, MEASURED:** r.AntiAliasingMethod=4 (TSR, Constructor default), r.MotionBlurQuality=4 (Scalability), r.ScreenPercentage=100. The bench runs temporal AA and labels 16/16 perfectly — an axis reading that matters for P9.
- Environment: junctions healthy to E:\IA_BuildCache; `_TRASH_pending_owner_delete` deleted by owner; three (now four) untracked owner CHAT-HANDOFF docs in the tree by convention.

---

## 2. P9 — the full story for a cold reader (this session's headline)

**What P9 is now.** On Bates, blinking anomaly labels desync from pixels at hide boundaries — on **every** reviewed blinking event, both legs ("3 instances" was the full reviewed population, not a sample). Labels are **host-independent** (the fixed certified 30fps sampled cadence `(n, n+1, n+5, n+6)`, identical on both hosts). Pixels are **host-dependent**. The owner-observation rule applied in full all session and was vindicated: the bench failing to reproduce was a fact about the bench.

**Two phenomena, split and separately resolved:**
- **(A) BOUNDARY SMEAR — CLOSED.** Partial opacity at hide edges (n ≈20%, n+1 ≈10%, n+6 ≈20% in the owner's AA-on ladder). The owner disabled all AA + motion blur via the title's own menu → **partials vanished** → the pre-declared C-1 discriminator fired: (A) is temporal accumulation on Bates' pipeline. A capture-fidelity property, not a label defect. (Caveat on record: menu-set, effective cvars to be collected by C-3(e).) The ladder's "n+3 still gone" is superseded by the AA-off read (n+3 visible) and filed under (A).
- **(B) PHASE DISPLACEMENT — OPEN. This is P9 proper.** Clean binary with AA off: observed hidden **{n, n+1, n+2, n+6}** vs claimed **{n, n+1, n+5, n+6}**. Identical structure to the original transcription ({42,43,44,48} vs {42,43,47,48}). Deterministic; AA on or off.

**Transition arithmetic (recorded as arithmetic, NO mechanism):** four visibility flips per event. The OUTER pair (first hide at n, final show at n+7) matches labels exactly on both owner observations. The INTERIOR pair (mid-event show: labels n+2 / eye n+3; second hide: labels n+5 / eye n+6) is each **+1 frame late in pixels**. Hidden-frame count conserved (4 vs 4) ⇒ no dropped frames; outer-pinned ⇒ no whole-event offset. Which code path drives each flip = next session's source read (§6 task 1).

**Axes EXCLUDED for (B)** (each by measurement or both-directions logic): census + mask (owner reproduced with no flags), delivery (v1 leg C identical to A; also settled from source — delivery suppresses run.json + selection_provenance.json, NOT labels.jsonl), pacing (bench aligned both pacings; Bates is paced), tick ratio (closed: `ticks_per_captured_frame` = capture_game_ticks/total_frames by construction; bench reproduces Bates' 1.3556 exactly), letterbox (v2 fixture letterboxed, aligned), AA/temporal method (bench TSR-on aligned 16/16; Bates displaced AA-on AND AA-off). **REMAINING: Bates host build + content.**

**Bench campaigns (both on exe D2BB25A5):**
- v1, MainWorld: UNDECIDABLE — the intro camera moves, A56 per-event collapses (modal 1-in-8). Lesson G206: requirements satisfiable separately, conjunction satisfiable by nothing (letterbox ⇒ MainWorld ⇒ moving camera vs settled camera ⇒ CB_GateLevel ⇒ no letterbox). Check conjunctions against a NAMED fixture at plan time.
- Fixture-v2: the **R3 zero-cook route** — `Set PlayerCameraManager bDefaultConstrainAspectRatio true` + `Set PlayerCameraManager DefaultAspectRatio 2.39` letterboxes ANY view target (engine default path sets both FMinimalViewInfo fields before view-target-specific work; identical rect to the lever, measured). **Bench device only: `Set` is live-instance and not shipping-gated — NEVER in client-facing payloads; every use named in read-backs.** Result on letterboxed CB_GateLevel: **16/16 readable events across 4 legs (targeted/auto × paced/unpaced) ALIGNED, k=0, empty differences; zero P9-SHAPE; 8 events UNDECIDABLE on separation.** The strict pre-declared NOT-REPRODUCED (every event aligned) was NOT satisfied and was NOT loosened. Caveats on record: low-contrast events unreadable (but Bates instances were eye-visible = high-contrast, so the readable class is the diagnostic one); SHIFTED division-of-labour untested on real shifted pixels.

**Instruments:**
- **A54 CANNOT see P9** — its hypothesis space is uniform shift only; on a P9 event it returns ALIGNED (fails toward clean; would have blessed the bug). Also **N/A on aspect-constrained fixtures** (A59): its CALIB_BBOX is frozen against an unconstrained view; its "P8/pose" message there is a known misattribution (the view was constrained, the camera did not move). a54_oracle.py deliberately untouched (any edit re-triggers A53). Its header's "published R30 median 0.10737" did not reproduce (0.10478/0.109008) — do not cite it.
- **`p9_hidden_set.py` is THE P9 instrument.** Within-event two-cluster split anchored by flank frames; emits observed set, claimed set, both differences, best-k over −6..+6, residual-by-k, per-frame margins. Taxonomy: ALIGNED / SHIFTED(k) / P9-SHAPE / ONE-DIRECTIONAL / UNDECIDABLE / TRUNCATED. **SEP_RATIO 5.0 FROZEN** from two control populations (known-ABSENT max 2.54, known-ALIGNED min 8.96; geometric middle; biased away from confident-on-noise) — the TAU-from-controls pattern, never retuned; near-floor events annotated, never reclassified (leg B/B' ev3 at 5.531/5.548 are the precedent). Gated 4/4 controls + d-unit 3/3 (incl. the Bates pair as a literal-set fixture). Anchor guard: on flush-boundary cadences ANY uniform shift puts truth under a flank ⇒ reader refuses rather than certifying SHIFTED; anchor-refusal-naming-a-hidden-flank is the P1-suggestive signature.
- **Overlay semantics RESOLVED from code:** RED = frame IS in annotation frame_indices (shipped label); AMBER = labels.jsonl row not in frame_indices. Both stamped from the same Snap->SessionIndex one line apart (:1917/:1919/:1921-1922) — **one-frame drift between them is impossible by construction.** Consequence (load-bearing): the owner's overlay reading ALONE evidences (B) in both directions — red on fully-visible n+5, amber on fully-gone n+3 — independent of which event anchored the owner's n (C-3(c) prints the array; never re-ask).
- **Pipeline enumeration** (journal 067 §15.1, 18 rows, file:line): the SVE grab is at the LAST post-process pass — **post-AA, so whatever AA painted IS the captured pixel** — before UI and Present. Only host-dependent rows: the AA/upscaler chain and RHI-thread/frame-lag. frame_indices and labels.jsonl share one stamping site. The blinking toggle log line is **Verbose-only** (Anomaly_Blinking.cpp:95) ⇒ absent from every banked run ⇒ the decisive Bates read needs a FRESH run.

**NEXT EVIDENCE = card C-3 on Bates** (owner, unblocked once close-out lands, their timing): fresh run · `Log LogAnomaly Verbose` pre-typed · AA off · then the card's self-saving bundle (`p9_bundle.txt` via Tee-Object, verified on PS 5.1; writes a BOM — noted). Pre-declared three-way comparison per interior flip: toggle log sides with LABELS ⇒ pixels are the outlier on this host; toggle log sides with EYE ⇒ sampling/labeling is the outlier on this host; mixed ⇒ report raw. Outcomes name WHERE, never why. C-2 (target-class: plain StaticMeshActor vs Blueprint actor) stays optional/cheap. **Standing mitigation: blinking unticked on any Bates run outside C-1..C-3.**

**NO MECHANISM has been claimed anywhere** — frame-saga discipline held the entire session. Candidates exist in the record (m20's fixed one-tick-stale state; the m31 arm/present family) and none is asserted.

---

## 3. Decisions made this session, with the why (not yet all in repo journals)

- **SEP_RATIO := 5.0, chat-ruled, frozen.** Code refused to propose (correctly — post-hoc constant-picking). The no-tolerance-without-calibration rule didn't bite: two known-answer populations calibrate it. Ratio form confirmed by the data (absolute separations overlap; ratios gap).
- **Control (d) redefined honestly:** the anchor guard is right; the construction (shift the claim ⇒ flank slides onto truth) can't demonstrate SHIFTED on flush fixtures. Division of labour made explicit + d-unit added to test the classifier branch set-level. A54's banked shift control cited, not re-run.
- **Floor/ceiling DECIDE, owner-ruled: Option A** — floor 0.5 + ceiling 25 inclusive. Rationale: at 6.0 Bates had 2 eligible (landscape ≈34% + character ≈7% → pitch-black frames + repetition); at 0.5 ~8 eligible, ~90% eye-visible; 25 sits in empty histogram space and kills the scenery class without name patterns. Built as m37; the exe freeze (P9 one-variable-at-a-time) was retired because the bench campaign completed.
- **m37 mechanism-vs-default honesty:** the bench map's largest candidate ≈6.06%, so the DEFAULT 25 is untestable there (a clean pass would mean nothing — G96/G135); the gate proved the MECHANISM at console ceiling 5.0; default 25 is Bates-validated later.
- **m38 approved** (run-scoped session log, `AnomalyRunLog` FOutputDevice): filters LogAnomaly + LogAnomalyCapture into `anomaly_log.txt` beside annotation.json; registered after RunDir exists; **flush + unregister on EVERY teardown path** (a held handle blocks Remove-Item and breaks harness re-runs — THE risk; gate (iii) is literally deleting the folder); thread-safe; verbosity never silently changed (separate raise-and-restore knob); delivery default mirrors run.json with three-state override (−1/0/1); StartRun echo states state+path (loud-inert); post-EndRun PNG-worker lines go to main log only, stated. Gates (i)–(iv) incl. artifact-set comparators updated.
- **Two dirty local branches DELETED** (m29-GATE-FAILED… ab2fb41, s3a-2-GATE-FAILED… 087f4d9): dead by name, upstreams gone, journaled before deletion; restores the no-reachable-ref posture at zero scrub cost. The verifier had never covered local-only refs — the two never-scrubbed were exactly the two dirty ones.
- **R3-over-cook preference:** when a zero-cook route exists, take it (ruling said "prefer it and say so"); running v2 on the SAME exe strengthened one-variable-at-a-time.
- **Retired belief:** "delivery suppresses labels.jsonl" → WRONG; it suppresses run.json + selection_provenance.json (`bWriteLabelsInDeliveryDefault` compiled ON). Confirmed empirically by leg C.
- **Journals are records:** superseded-by notes, never rewrites. Predictions never amended post-measurement — the join fix and the SEP derivation went via tool header + journal annotation (the P-C2/P-C13 route).
- **Owner-comms protocol additions (STANDING, inherit them):** every deliverable marked **DONE / QUEUED / FUTURE**; **ONE brief in flight** — new information queues chat-side until Code's report returns, no replacement blocks; **owner errands only against VERIFIED tooling** (DONE-confirmed, not merely ordered). Born from this session's triple-replacement failure — do not repeat it.

---

## 4. What was executed vs what was written (the retro item a fresh chat must know)

After the owner's AA-off observation, chat wrote a consolidated replacement brief. The owner — mid-protocol-dispute, rightly — pasted the EARLIER block instead. Code executed that one faithfully and well. Net effect at `26170a8`:
- **Landed:** overlay-resolved ledger paragraph; card C-3 self-saving (Tee-Object) + `<TITLE-SAVED-LOGS>` placeholder for C-3; renumber sweep; harness banking; m37; m38 plan.
- **NOT landed (close-out brief carries it — verify per §0):** (A)-closed paragraph, (B) clean-binary sets, axis table, transition arithmetic; C-1 marked done; C-3 decisive framing + prerequisites; older-card-section path placeholders.
- **Never executed at all:** the transition-driver + toggle-line-anchor source read → next session TASK 1.

---

## 5. Open vs locked

**Open:** P9 (B) — awaiting C-3 bundle, then the Bates-side hunt (host build/content axes, measurement-first) · which event anchored the owner's n (C-3(c)) · toggle-line anchor verdict (task 1) · C-2 optional leg · m38 implementation · SHIFTED division-of-labour on real shifted pixels (owed instrument coverage) · `census_fires_fallback_all=3` on auto-pool legs (census-cadence observation, 77-candidate map; Bates read 0; explicitly out of m37 scope — revisit when census-ON shipping is planned) · m31 V-3/V-4 Concorde legs · Concorde custom-depth census · Concorde HDR preview check · tags · **client channel open?** (standing session-start check; if closed both directions, resolution/JPEG/defaults are cancelled, not pending) · optional never-answered: may cropped frame PNGs leave Bates (text-only C-3 suffices; only relevant if the bundle is ambiguous) · README/runbook drift class (both outside VC).

**Locked (do not relitigate):** everything previously locked (zero-only veto · census OFF = byte-identical old picker · translucent-excluded/nanite-fallback split · tag-lifetime + reservation · Build.cs unalterable · codename-only invariant · no history rewrite while private · predictions never amended post-measurement · G-R7(ii) at delivery on master · builds from main checkout only · `tagBlockMs` is not the census's cost) **PLUS this session:** SEP_RATIO 5.0 frozen · A54 blind to P9 + N/A on constrained fixtures (A59) · overlay one-counter fact · (A) closed as temporal accumulation · owner-observation rule reaffirmed · verifier-stdout rule (G202) · `Set` exec = bench-only device · one-brief-in-flight + DONE/QUEUED/FUTURE + verified-tooling gating · m37 defaults + inclusive-band semantics.

---

## 6. Forward plan, in order

0. **Verify the close-out landed** (§0). If not, paste §9.
1. **New Code session** (bootstrap cold: CLAUDE.md → journal 067 → this handoff; summarize state back first). **TASK 1 (xhigh):** the source read — map which code path drives each of a blinking event's four flips (Apply / Tick toggle / Revert; Anomaly_Blinking.{h,cpp} + injector apply/revert path), and quote the Verbose toggle line's EXACT format + what it stamps + whether it anchors to session/frame index **using only what Bates' sealed installed build already prints**; conclude explicitly what C-3 can and cannot discriminate. Factual mapping only, no mechanism. This lands BEFORE any bundle is interpreted.
2. **TASK 2:** m38 implementation per the approved plan (gates i–iv; the handle-leak gate is a literal folder delete).
3. **Owner, their timing:** C-3 on Bates (fresh run · Verbose · AA off · bundle self-saves). Then chat+Code interpret via the pre-declared three-way comparison.
4. **Depending on C-3:** the (B) hunt proceeds Bates-side — still measurement-first, still no mechanism into any brief without a measurement.
5. **Physical office visit (batched):** G-R7(ii) split on MASTER's cook · m31 V-3/V-4 · Concorde custom-depth census (per-component render_custom_depth; customStencilExtent does NOT answer it) · tick-pin probe echo baseline · **tags m31→m33→m34→m35→m36→m37 (+m38 if landed)** · delete the parent branch · Concorde ini gains the census keys (now incl. ceiling) only when census ships ON. Note: m38 reaches office hosts only when their builds are next updated — Bates is sealed; nothing deploys over RDP.
6. **Then:** m39 honest bbox (P-C13-c2 gate; uniform PIE pillarbox leg = instrument) · resolution selection + PNG→JPEG + defaults profile (client channel check FIRST) · P5 blend-ladder · S-track (S4 depth — note the P9 product angle below — S5 backbuffer demotion) · CPU-path deletion decision at next CLIENT cook (+ stale A3 console-help string rides it, G118).
- **Product note, parked but recorded:** with any temporal AA on, hide-boundary frames are soft by nature — single-frame hidden/visible labels at boundaries are ±1-frame ambiguous in pixels on such hosts. Argues for pre-AA scene-color capture (parked S4) or a "transition" label state eventually. Do not action without owner scope approval.

---

## 7. Corrections — stale understanding to discard

- P8 → **P9** (chat error twice; P8 taken 2026-08-18; numbers never reused).
- "Delivery suppresses labels.jsonl" → suppresses run.json + selection_provenance.json.
- "m37 = honest bbox" → m37 = census defaults (SHIPPED); honest bbox = **m39**; m38 = session log.
- "Ceiling knob unbuilt" → BUILT and gated (m37).
- "Exe D2BB25A5 frozen" → freeze retired; staged bench exe = **6C80E872**.
- "feature/stencil-capture on origin" → LOCAL-ONLY (and verifier-clean).
- "_TRASH_pending_owner_delete awaiting deletion" → already deleted by owner; runbook line updated.
- "ticks_per_captured_frame 1.3556 unexplained" → CLOSED: = capture_game_ticks/total_frames; bench reproduces it exactly; not a discriminator.
- "AA method 2 = AA off" → method 2 is TAA (history-retaining); the owner's first config test never reached a non-temporal state; superseded by the menu-off test.
- AA-on ladder's "n+3 gone" → superseded by AA-off (n+3 visible); filed under (A).
- The overlay "one frame later" puzzle → NOT two artifacts disagreeing (impossible by construction); different-event anchor or transcription slip; C-3(c) settles it; the owner is never re-asked.
- "3 instances" → the full reviewed population; deterministic per event.
- "A54 will carry the P9 read" → A54 is shape-blind to P9 and N/A on constrained fixtures; `p9_hidden_set.py` is the instrument.

---

## 8. Pointers (what a new Code session reads; this handoff does not duplicate them)

`CLAUDE.md` (status + stop block + milestone map) · **journal 067** (the whole session: §9 reader+first gate, §10 rulings+re-gate+branch deletions, §11 v1 legs, §12 fixture-v2, §13 renumbers, §14 m37 plan, §15 enumeration+overlay+bundle commands, §16 m38 plan, closing sections) · `docs/invisible-anomaly-mechanisms.md` §8.6a/§8.6b · `docs/predictions/` (2026-09-02 p9 v1 · 2026-09-03 fixture-v2 · 2026-09-02 m37 gates) · `docs/office-rdp-card.md` "THE P9 BATES PROTOCOL" (C-1 done · C-2 optional · C-3 decisive) · `docs/gotchas.md` ~G202–G207 (verifier stdout · splat-literal/_leg_geometry convicts · conjunction-fixture · BOM/Set-Content · `Set` exec live-instance · BOM-free UTF8 harness rule) · `CaptureBench/tools/p9_hidden_set.py` header (method, sign convention, SEP derivation, SHIFTED-reachability limit, join correction) · CHAT-HANDOFF-m36-census-and-bates-results.md (older context).

---

## 9. Appendix — the close-out brief (paste ONLY if §0's verification fails)

Docs-only final turn for the 067 Code session: (1) Ledger §8.6a additions — menu-off paragraph ((A) CLOSED per the pre-declared discriminator; cvar caveat; ladder n+3 superseded), (B) clean binary {n,n+1,n+2,n+6} vs {n,n+1,n+5,n+6}, axis table (AA excluded both directions; remaining = Bates host build + content), transition-arithmetic paragraph (outer flips match, interior +1, counts conserved, no mechanism). (2) Card — C-1 marked done-by-owner (residue → C-3(e) incl. `fidelityfx.` completion lookup); C-3 promoted to decisive with prerequisites (fresh run · `Log LogAnomaly Verbose` · AA off) and the three-way comparison (log-vs-labels-vs-eye; outcomes name WHERE, never why); A-6/A-7/B-2 hard-coded bench log filename → `<TITLE-SAVED-LOGS>` placeholder. (3) Journal 067 closing sections + CLAUDE.md refresh (m37 done · m38 approved-unimplemented · m39 bbox · exe 6C80E872 · tags …→m37 · P9 one-liner · task-1 note that the driver/anchor read was never executed). No source, no legs, no m38 implementation, no tags.
