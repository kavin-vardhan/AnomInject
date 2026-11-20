# CHAT HANDOFF — m49 COMPLETE on master · external review → m51 · orchestration handed to the Codex pilot
**Date:** 2026-09-06 ~14:00 local. **Written for:** the Codex session taking over orchestration of the GDP mailbox (pilot = m51), and any cold reader after it. **Supersedes** `CHAT-HANDOFF-071-076-client-feedback-m49-lyra.md` wherever they disagree.
**Confidentiality:** hosts by INTERNAL codename only — **Concorde, Bates, Europa**. `StackOBot` and `LyraStarterGame` are the owner's Epic samples, no codename. ⛔ **OFFICE DATA NEVER LEAVES THE OFFICE PC** — no session folders, frames, logs, screenshots or client data ever come to this machine or into any chat/agent; only the owner's typed transcriptions of readings, into `D:\IntrusiveAnomalies\_bates_reads\`. This binds every agent on this machine, Codex included. Never ask for a session folder.

---

## 0. READ THIS FIRST (state as of 14:00, verified against disk)

1. **`master == origin/master == ac13700`.** m49 is COMPLETE (A1 + A2 + Phase B) on top of m50. Merge commit `4a4bfcc` (`--no-ff`, tree byte-identical to the gated `m49-phase-b` tip `9b27b05`); docs `ac13700`. **No tag** (highest tag m30; tags batch at the office). Checkout on `master`, clean apart from untracked `docs/CHAT-HANDOFF-*.md`.
2. **Bench:** staged exe **`B8AC65D8`** (Phase B, the binary every gate ran on) + Phase B container quartet `67EA1FE0`/`2CEFB8F4`/`E03C6610` + `A16A18A8`/`C70ECDAA`. A-side archived: `StackOBot.exe.m50-A159EDFD` + `m46-container-m50exe-A159EDFD\`. A fresh build of the same source hashes `A9B10CC0` — NOT staged by rule (G201: same source, different link; gates bind to the binary they ran on). `B8AC65D8` is 241,400,320 B (README corrected).
3. **Lyra worktree** `E:\UEProjects\LyraStarterGame\Plugins\AnomalyInjector` detached at `4a4bfcc`, clean; LyraEditor built from `1c3d62c` (docs-only diff since — no rebuild needed).
4. **`fix/capture-label-integrity` = `be16977`**, LOCAL ONLY (not on origin — verified `git branch -r`; 078-01 should push it first thing so the source material is backed up), off `21c1e0d`, 43 files, +1415/−421, untouched, never staged. **It is m51's source material** — see §2. Overlaps master on `AnomalyCaptureSubsystem.cpp/.h`, `AnomalyLabelWriter.cpp/.h`, `AnomalyMaskSceneViewExtension.cpp`, `AnomalyMaskTypes.h`; conflict resolution against `4a4bfcc` is expected and grows the longer it sits.
5. **Bates M2 SHIPPED 2026-09-04** (owner), client told to expect the Concorde-M2 class of issues. **No delivery pending. No hurry.** The next client build is an **M3 re-issue for both hosts on one build** after m51.
6. **Mailbox:** last report 077R-02 (GREEN work, but `last_verdict.txt` says **NEEDS-DECISION** because its §5 raised the branch question, which this handoff resolves) ⇒ **the next brief MUST carry `# ACK-VERDICT: fix/capture-label-integrity is m51 source material (chat ruling 2026-09-06)`** or the watcher holds it. Verdict gate details §8. Default Code session = 077R-02's (`03566811…`, 40 % context) — start m51 with `# SESSION: new-default`.
7. **Next authorized action:** brief 078-01 = m51, issued by Codex. Chat issues nothing during the pilot.

## 1. What m49 + m50 + Phase B deliver (so nobody relitigates it)

- **Labels derived from the frame's own render (T2b).** Per row per fire: `target_pixels` (front-most drawn pixels of the target from the mask pass), `target_drawn_pixels` (Phase B: pixels where the target ITSELF is drawn, |CustomZ−SceneZ| ≤ bias), `observable` ∈ {true,false,null}, `bbox_drawn_px`/`bbox_source`, `mask_value`, `mask_state`, `render_state` (`shaders_pending`), `exposure_dip` + `exposure_dip_scope`. Hide-class `observable` = labelled && DrawnCount == 0 && silhouette in view; texture-class = A1's condition + count.
- **Schema v2** (`label_schema: 2`): `affected_frames` = OBSERVABLE subset (meaning tightened, key kept), `injected_frames` = old meaning, `span_frame_count`, `observable_frame_count`, `unmeasured_frame_count`, `observability_measured` (= Measured > 0), `bbox_source`. **Fallback:** zero measured frames ⇒ `affected_frames = injected_frames`, `observability_measured:false`. Zero-observable events STAY in the file. Client field reference + v1→v2 changelog in `docs/client-readme.md` §8.
- **m50:** stencil single-owner — census leaves 8 values free; at pool exhaustion NO tag is issued and the event ships NOT_MEASURED (`target_pixels -1`, `observable null`). Nanite targets (all renderable components Nanite) admitted as NOT_MEASURED instead of vetoed. 55-value ceiling per session (200–254) is a STATED limitation — m51 F3 removes it.
- **Phase B finding (G255–G258):** "DrawnCount > 0 on a hidden frame ⇒ the hide failed" is FALSE on a real host — pixels showed the hides took while a small residual survived (Lyra, 7 rows, 0.27–0.81 %). `target_drawn_pixels` is evidence of ABSENCE, never of presence; `frames_drawn_unexpected` is a diagnostic counter (Lyra reads 7, StackOBot 0 — client docs say so; two stale "must read 0" gates were fixed in 077R-02).
- **Verifier** `tools/verify_capture.py --label-pixel-gate` ships to the client and is a delivery gate on the HOST's own smoke session (card Section G: the owner runs it on the office box and transcribes lines).
- **Gate bar (the culture, not a suggestion):** predictions file written BEFORE the first leg (AMENDMENT blocks after, never edits); every gate in BOTH tick orders (natural + synth), on BOTH fixtures (StackOBot bench + Lyra worktree), on ONE binary, with byte-identity A-side anchors; a can-fail lever per gate; a leg that certifies with < 3 counted events is invalid, never graded on the reduced set; journal + CLAUDE.md status block per session; gotchas append-only, G-numbered; Conventional Commits; a NEEDS-DECISION report is not green. Pricing a fix before writing it can refute it (G253) — Code is expected to stop and say so.

## 2. Decisions made today, with the why (not yet in code)

- **The external review (16 findings) is credible** — chat spot-checked 8 cited sites, all correct — and becomes **m51 (release hardening)**. Verbatim: `D:\IntrusiveAnomalies\_reviews\2026-09-06-external-release-review.md`.
- **The Codex branch `be16977` is m51's source, NOT a merge unit.** Chat review + rulings R1–R8 (amended after Codex's reply, §5 of that file): `D:\IntrusiveAnomalies\_reviews\2026-09-06-review-of-fix-capture-label-integrity.md`. Codex ACCEPTED the split and every ruling, added the R8 cancellation defect, and withdrew nothing of substance; chat withdrew one wrong claim (MoveTemp). Read that file before writing 078-01 — it is the spec.
- **Rulings in one breath:** R1 schema 3 = v2 + ADDITIVE keys only, no meaning change; fallback only at ZERO measured frames; `observability_measured` keeps Phase B's meaning, NEW `observability_complete` = every injected frame measured. R2 Nanite rule stays `NaniteOnly == Renderable`; mixed-component actors = a future Lyra leg. R3 keep name-OR-class exact targeting, add path, log ambiguity. R4 WebSocket mutations refused during capture (`capture_active`); console mutations ALLOWED but counted (`external_mutations`) — the bench levers need them. R5 shader-pending ⇒ unmeasured per FIRE, in capture AND the auditor. R6 writer: bounded N-worker pool, byte cap, ≤ one-frame wait then a COUNTED drop marking the session incomplete, never a 10 s game-thread stall; mask encode failure downgrades the row AND the event observation, frame still commits. R7 readback cap = ini knob default 32, drops counted, zero-drop gate on natural legs. R8 stale-arm rejection kept; cancellation must retire the `PendingSnapshots` entry immediately and count `unrendered_ticks`; can-fail = rendering resumes after > cap missed ticks and capture continues. Phase B BEHAVIOUR (target-excluded exposure detector) must survive, not just its keys.
- **Why no schema break now:** the client received v2 docs on 2026-09-04; changing `anomaly_present` a day later is a trust cost. Codex holds (rightly) that a fallback window is injection-labelled data, not verified visible ground truth — the client README will say that in those words; the meaning of `anomaly_present` is renegotiated in a **client-agreed M3 contract**, not flipped.
- **Roles:** Codex orchestrates m51 (writes briefs, evaluates reports, owns the queue); Claude Code implements and may refuse a faulty premise with a measurement; chat Claude is the adversarial reviewer — predictions before the first leg, evidence bank before merge, in writing under `_reviews/`. Codex codes only separable work (its second tranche) after m51, so the capture core keeps an independent reviewer. One orchestrator per mailbox; the contended resource is the bench checkout.
- **Pilot metrics (owner):** briefs per milestone, $ per brief (report headers), escaped defects found in review, post-hoc gate amendments, rework briefs, owner interventions, Codex token cost (owner notes it — not in the headers). Comparison baseline: chat sessions 071–077R (Sep 4–6): 13 briefs, ≈ $1,082 API-equivalent total, m49 + m50 + Phase B shipped, one crash recovery, zero escaped client defects known.

## 3. Rejected approaches and why (do not re-propose without new evidence)

- Merging `fix/capture-label-integrity` as a unit — wrong base, unmeasured semantic reversals, 640×360-only validation.
- Quarantining stencil values at exhaustion (075 plan) — refuted by its own priced cost: it shrinks the pool that was already running out (G250/G253).
- MPP_Outline refuse-or-warn rule — Leg B like-for-like control found no effect; reader names ship in run_summary, the census knob is the escape hatch.
- Chasing the Lyra drawn residual now — named host limitation (G258); mechanism chase is FUTURE.
- City Sample as main bench — all Nanite ⇒ mask instrument blind (G134), hours per cook; deferred as stress fixture.
- Copying session folders home for the verifier — NEVER; Section G runs it on the office box.
- Junction-mounting the plugin into Lyra — same engine ⇒ shared Binaries clobber byte-identity anchors; worktree only (rule 5).
- Staging a fresh build of unchanged source — G201; gates bind to the binary they ran on.
- Pinning Fable 5.1 on Code for routine briefs — token burn; Opus 5 xhigh default, Fable 5.1 only for crucial reasoning.

## 4. Intentional compromises (known, accepted, documented)

- 55-tag ceiling per session until m51 F3. · Nanite targets unmeasured on 5.1 (custom depth blind, G134). · `ObservableMinPixels` default 1 = pixel presence, not recognisability; calibrate from the client's data when it reaches the OFFICE. · Editor-only first-frame checker/black symptoms on Bates/Concorde: shader compile is real but not the whole story; "object black" UNEXPLAINED. · Auto-exposure is the game's (adaptive by default in delivered builds, pinned OFF on the bench); m48 marks dips, never forces exposure. · `-RenderOffScreen` capture launch (072-01) never shipped — ClaudePin is the stopgap for focus theft. · Europa (UE 4.25 port) PARKED on `europa-e1` (1f5e305) with two measured blockers solved in design; bench is free now — owner's call when to resume.

## 5. Forward plan, in order

1. **078-01 m51 (Codex):** master → `m51` branch → re-apply `be16977` (conflicts expected on six files; Phase B keys AND behaviour survive) → rulings R1–R8 as separate commits → fix stale counters (`SessionGlobalPositive/NegativeFrames`), vestigial `TargetMaskHoldTicks`, silent `Published` reset → predictions BEFORE legs → full gate set + one can-fail leg per review finding (list in the review file §4) + throughput at 1080p native + auditor `--integrity-only` on every banked schema-3 session → docs (client-readme §8 additive schema-3 + v2→v3 changelog "nothing you read changes meaning"; `capture-label-integrity.md` corrected to the rulings; checklist gains the auditor box; gotchas; journal). Chat reviews predictions and the evidence bank; Codex merges only after that review. Expect 3–4 Code sessions.
2. Codex's second tranche (evidence levels / component attribution; control-server responses; shipping config contract; private renderer deps) — Codex may code these on separate branches; Code runs gates.
3. **M3 re-issue** for Concorde AND Bates on one build: schema 3, verifier, auditor, changelog, card Section G read on the office box first. Client-agreed M3 contract decides the `anomaly_present` question.
4. When the client's Concorde M2 session data reaches the OFFICE: verifier there, transcribe, calibrate `ObservableMinPixels`, answer F1/F3/F6.
5. Later: mixed-Nanite Lyra leg; Lyra drawn-residual mechanism; LG-9 Nanite reach on Bates; m42 persist tags; `-RenderOffScreen` launch; Europa E1.

## 6. Open questions — with owners

**Owner:** when to resume Europa E1 · Bates Nanite status (never read; `census_unmeasurable_nanite` is the reading) · office batch tags m31→m50 (physical visit) · mailbox signal publisher health after reboots (it silently stops).
**Client (via owner):** Concorde M2 session artifacts (to the OFFICE only) · PNG vs MP4 viewer base · M3 schema contract.
**Codex (m51):** conflict strategy for the six overlapping files · per-fire shader attribution source · N for the writer pool · the decoupled-order leg design for R8.
**Chat (reviewer):** predictions review before leg 1; evidence-bank review before merge.

## 7. Corrections — discard these

- ~~Bates ON HOLD until m49+m50 pass on the host~~ → SHIPPED 2026-09-04 as M2. ~~Client cook next~~ → M3 after m51.
- ~~"The stencil collision is the allocator handing out live values"~~ → one line at exhaustion re-issued a proven-taken value (m50).
- ~~"DrawnCount > 0 on a hidden frame means the hide failed"~~ → refuted on pixels (G258).
- ~~"The post-cook build changed the camera pose"~~ → bench flakiness in a 4-minute window; same bytes passed 4/4 (G257).
- ~~"m50_pc7v3.py must move to CaptureBench"~~ → it was an empty stray; the real tool was already tracked there.
- ~~"held frames are copied via MoveTemp on a const ref"~~ (chat review) → wrong, withdrawn.
- ~~"no push; no remote exists"~~ (mailbox README) → remote exists; Code pushes; tags never.
- ~~Chat orchestrates the GDP mailbox~~ → Codex, for the m51 pilot.

## 8. Mailbox mechanics for the new orchestrator

- Brief = `to_code\NNN-name.md`; header directives `# SESSION: new | new-default`, `# NAME:`, `# EFFORT: xhigh`, `# MODEL: claude-opus-5`, and now **`# ACK-VERDICT: <text>`** (required when `last_verdict.txt` says NEEDS-DECISION or RED, else the brief is HELD as `processed\HELD_<name>`, unrun, re-droppable under the same name). A brief name runs at most ONCE ever (`ran.txt`).
- Report = `to_chat\NNN-name.report.md`; **line 2 = `verdict: GREEN | NEEDS-DECISION | RED (declared|derived)`**. Ask Code to end its final message with a literal `VERDICT: …` line; the runner derives one otherwise. Exit code 0 is not acceptance.
- Wake-up for a LOCAL orchestrator: wait on the report file or `watcher_status.txt` (`state=idle`) — no GitHub relay needed (that is for cloud sessions). The dashboard (tray app, http://localhost:7337) toasts the owner regardless.
- Sessions: roll at ~70 % context (`context at last turn` in each report header) via `# SESSION: new-default`; a side session is `# SESSION: new` (not adopted as default). Model rule: Opus 5 at **xhigh** default (never medium/low); Fable 5.1 only for crucial reasoning briefs; ultracode only for large multi-stage batches.
- Every brief must end with the FINAL MESSAGE structure (plain rundown → tables → commits/hashes/checkouts → NEEDS-DECISION → VERDICT line). Every report is read in full before the next brief. One brief in flight at a time.
- Never run git write commands against the repo from a Linux mount (CRLF); Code does all git work on Windows. Codex runs on Windows — fine, but the checkout belongs to Code while a brief is running.
- Cost ledger: `costs.csv` / `costs_summary.txt`; ≈ $1,082 API-equivalent across 071–077R-02 (none billed on Max).

## 9. Pointers

`CLAUDE.md` status block (top = 077R-02) · `docs/OPERATING-CONTRACT.md` (seeded today — Codex owns it) · journals `docs/sessions/2026-09-04-071…077-*.md` · `docs/predictions/2026-09-04-m49-phase-b.md` (+AMENDMENT 1, 2), `…-m50.md` · gotchas G236–G258 · `docs/client-readme.md` §8 · `docs/client-delivery.md` · `docs/PRE-DELIVERY-CHECKLIST.md` · `docs/office-rdp-card.md` Section G · `docs/invisible-anomaly-mechanisms.md` §14 · `_reviews\` (external review, chat review + amended rulings, the branch diff) · `_bench_sessions_bank\` (legs `PB*`, `R02S_*`, `M49_*`, `A1_*`, `A2_*`, `LYRA_*`) · `_binary_baselines\README.md` · `E:\MailBox\README.md` (dashboard, signal, local-orchestrator note) · previous handoff `CHAT-HANDOFF-071-076-client-feedback-m49-lyra.md` (Lyra setup, client findings table F1–F8).
