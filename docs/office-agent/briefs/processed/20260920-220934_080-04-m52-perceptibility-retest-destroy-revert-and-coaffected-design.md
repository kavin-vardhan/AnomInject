# SESSION: new-default
# NAME: 080 m52 stuck low mip (3)
# MODEL: claude-opus-5
# EFFORT: xhigh
# ACK-VERDICT: 080-03 NEEDS-DECISION resolved by chat 2026-09-20 — G11's two NO-TRACE events are accepted as true findings (labels unsupported by pixels); perceptibility deepened with a predeclared retest and an escalation path (m55 measured change evidence); target-destroyed revert to be built and gated; S2′ (one hold, N events) to be DESIGNED, not built; default-on deferred; veto/observability disagreement queued as 080-05.

# 080-04 — m52: perceptibility retest, destroy-revert, VT check, S2′ design. Fresh session (previous at 68%).

## 0. Bootstrap
CLAUDE.md; `docs/predictions/2026-09-20-m52-stuck-low-mip.md` (+A1, A2); journals 080-01..03; gotchas G265–G268. Branch `feat/m52-stuck-mip` at `fd5928d`; staged exe `DC8BA0C1`. `master` `3ff88db`, `m51` untouched.

## 1. Perceptibility — deepen, retest, escalate (AMENDMENT 3 first)
R1 The footprint/mip ratio gate stays a pick-time filter but the FORCED mip is chosen so that the top resident mip ≤ bbox_px/8 (ratio ≥ 8; today's events sat at 6.3 and failed). `mip_levels` semantics: `-1` = "as deep as the ratio rule requires, floored at 4 mips (16 px)"; explicit values unchanged. Telemetry: `stuck_mip.ratio_at_pick`, `stuck_mip.forced_top_px`.
R2 Predeclared retest: the SAME auto-pool recipe as 080-03's G11 leg (same seed/config) — prediction: the two `SM_rock` events (or their equivalents at that seed) read CONSISTENT/OFFSET-NOTE, zero NO-TRACE across the leg. If ANY NO-TRACE survives on a `held:true` event: STOP the perceptibility line, report NEEDS-DECISION — chat escalates to m55 (in-engine per-frame target change fraction from the mask pass, making `observable` mean measured visible change for every anomaly class). Do not iterate the ratio.
R3 The 8th-frame FSM behaviour (BeginRevert in the same tick as the last capture) is accepted as pre-existing; document in the field reference that windows are N−1 labelled frames for deferred-onset anomalies. No change.

## 2. Target destroyed — build and gate
R4 Bind to the target actor's destruction (OnDestroyed / EndPlay — cite the delegate used) ⇒ immediate BeginRevert for that event; frames after destruction are not labelled; run-summary counter `stuck_mip_revert_on_destroy`. Bench lever `IAI.Bench.DestroyTarget <substring>` (bench-only, gated like the other `IAI.Bench.*`). Gate G4e: fire, destroy mid-window, expect revert line, `restored=N … unresolved=0`, RESTORE VERIFIED, no labelled frames after the destroy tick, no crash, no dangling texture in `awaiting-restore` at FinishRun.
R5 World teardown: the "bias cleared but restore unverified" limit stands; add a `stuck_mip_unverified_at_teardown` counter and a client-readme line. No change to teardown itself.

## 3. Virtual textures — exercise the path once
R6 Neither fixture has a VT-streamed target. Add a bench-only unit check: construct (or find in Engine content) a `UTexture2D` with `VirtualTextureStreaming` true and assert the picker returns NOT_APPLICABLE with `stuck_mip_refused_virtual` incremented; if no such texture can be reached at runtime without a cook, say so and record the path as UNEXERCISED in the catalogue entry (not silently).

## 4. S2′ — one hold, N events (DESIGN ONLY, in the predictions file as a proposal section)
Problem: Lyra-like hosts share every texture ⇒ yield 0 at `MaxCoAffected 0`; raising the cap admits unlabelled blurred neighbours. Proposal to design and cost, not build: one texture hold ⇒ one event per VISIBLE co-affected actor (each with its own target, stencil tag, mask, observability, `stuck_mip.shared_hold_id` linking them), `MaxCoAffected` = max visible co-affected actors admitted (default proposal: 4); co-affected actors NOT visible at fire time get no event (documented limitation: if one enters view mid-window it is blurred and unlabelled — count it if the census can see it enter: `stuck_mip_unlabelled_entrants`). Answer in the design: stencil-pool cost per fire (55 values), m44 one-target ownership impact, m26 veto per event, annotation shape (N events with the same `shared_hold_id`), verifier reading (each event judged on its own mask), the Lyra yield it would give from 080-03's G8 numbers, and the implementation size (files, lines). Chat + owner decide after.

## 5. Gates and closure
Build/stage/hash/archive (predecessor `DC8BA0C1` verified at its archive first). Gates: G2 can-fail (unchanged), G3 hold, G4 all exits incl. G4e, G5 onset both orders, G7 yield (new refusal reason `too_small_for_ratio` split out), G9 schema additive, G10 orders, G11 retest per R2 on the SAME recipe + the targeted legs. Lyra: G8 hold/restore at the shipped default (expected 0 fires — reported, not fixed here) + one run at bench override 8 for regression. Docs: predictions A3 + S2′ proposal, journal, catalogue entry, client-readme, CLAUDE.md. Commits pushed; NO merge, NO tag, NO default flipped. Report per gate with exact lines; `VERDICT: GREEN` alone on the last line only if R2's prediction holds and G4e passes; else NEEDS-DECISION.
