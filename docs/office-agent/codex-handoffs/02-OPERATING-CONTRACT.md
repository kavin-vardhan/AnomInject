# OPERATING CONTRACT — AnomalyInjector (GDP) — seeded 2026-09-06 by chat Claude, owned by the orchestrator from here on
One authoritative page. When this file and older docs disagree, THIS file wins; fix the older doc. `CLAUDE.md`'s status block remains the "you are here"; this is the "how we work". Change it only by a dated line under §9.

## 1. Roles
- **Owner (the owner):** product/scope rulings, playtests, physical bench actions, anything that touches the office PC. Does not write code, does not make technical calls.
- **Orchestrator (Codex, pilot from 2026-09-06; previously chat Claude):** designs, writes briefs, reads every report in full, rules on technical questions, owns the brief queue and the checkout. ONE orchestrator per mailbox at a time.
- **Implementer (Claude Code via the mailbox):** implements against an approved plan; measures; may refuse a faulty premise with a measurement and STOP with NEEDS-DECISION. Runs all git on Windows. Never pushes tags.
- **Adversarial reviewer (chat Claude during the pilot):** reviews predictions before the first leg and the evidence bank before merge, in writing under `D:\IntrusiveAnomalies\_reviews\`. Issues no briefs.

## 2. Hard rules (no exceptions, no rewording)
- ⛔ **Office data never leaves the office PC.** Capture sessions, frames, logs, screenshots, client-provided data stay there. Only the owner's typed transcriptions of readings come home, into `_bates_reads\`. Anything that must run on office data runs on the office box and the owner reports numbers.
- ⛔ **Codenames only:** Concorde, Bates, Europa. Never a real title, studio, fork name, the office's own codenames, or any mapping — in briefs, docs, journals, memory, reviews.
- ⛔ **Nothing from `_mailbox` temp files into the repo; nothing temp into `_mailbox`.**
- ⛔ **No git write from a Linux mount** (CRLF repo). Code does git on Windows.
- ⛔ **Tags batch at the office.** Code never tags. Pushes to origin are allowed and expected after a green merge.

## 3. Schema and label meanings (client-facing — changing a meaning is an owner + client decision)
- `label_schema` 2 is what the client has (Bates M2, Concorde re-issue pending). `affected_frames` = observable subset; `injected_frames` = all frames the injection existed; `observability_measured` = Measured > 0; fallback at ZERO measured frames: `affected_frames = injected_frames`, `observability_measured:false`; zero-observable events stay in the file.
- **Schema 3 (m51) = schema 2 + ADDITIVE keys only.** No key changes meaning. New: `event_id`, `injection_id`, `observability_basis`, `observability_complete`, `frame_observations`, `capture_complete`, `requested_frames`, `written_frame_indices`, per-row `schema_version`, `condition_requested`; empty windows start/end −1. `anomaly_present` keeps meaning "injection present"; `visible_positive` = observable-only. A fallback window is injection-labelled data, not verified visible ground truth — the README says so.
- Evidence vocabulary: `target_pixels` = front-most drawn pixels of the target (silhouette); `target_drawn_pixels` = target itself drawn (evidence of ABSENCE only); `observable` true/false/null; `-1`/null = NOT MEASURED, never clean, never positive; `mask_state` present/empty/unmeasured; `render_state: shaders_pending` ⇒ no positive on that fire.
- Renegotiating any meaning = the **M3 contract**, agreed with the client, with a migration note.

## 4. Rulings in force for m51 (R1–R8, amended 2026-09-06 — full text in `_reviews\2026-09-06-review-of-fix-capture-label-integrity.md` §3 + §5)
R1 additive schema 3, fallback only at zero measured, `observability_complete` new · R2 Nanite rule `NaniteOnly == Renderable` · R3 exact targeting name-OR-class, plus path, log ambiguity · R4 WebSocket mutations refused during capture, console mutations allowed + counted · R5 shader-pending per FIRE, capture and auditor · R6 bounded writer pool, ≤ 1-frame wait then counted drop, mask failure downgrades row AND event, frame still commits · R7 readback cap knob (32), counted drops, zero-drop gate · R8 stale arms rejected with immediate snapshot cancellation + `unrendered_ticks`, can-fail = recovery after > cap missed ticks. Phase B behaviour survives, not just its keys.

## 5. Gate bar (a change to capture is not done until all of this is true)
- Predictions file written BEFORE the first leg; amended only with dated AMENDMENT blocks.
- Every gate in BOTH tick orders (natural, synth), on BOTH fixtures (StackOBot bench, Lyra worktree), on ONE binary whose hash is recorded; A-side archived and byte-identical.
- A can-fail lever per gate, fired in both orders; a certifying leg with < 3 counted events is INVALID.
- Regression set: ONSET, PAIRING 100/50, G7, MASK-TIE, TARGET-PIXELS TIE, G-EDGE ×8, OBS-1..4, m50 G1/G2/G6, PB-1..PB-8, black-frame, A44, P-C7 v3 (declared keys only, `frame_indices` identical), Lyra LG set + verifier report-only. m51 adds one leg per review finding, throughput at 1080p native, auditor `--integrity-only` on banked schema-3 sessions.
- Shader change ⇒ ONE cook, quartet recorded, re-baseline proving nothing outside declared keys moved. Same source rebuilt ⇒ NOT re-staged (G201).
- Journal per session; CLAUDE.md status block; gotchas append-only and G-numbered; Conventional Commits, one milestone = one merge.

## 6. Mailbox protocol
- One brief in flight. Header: `# SESSION: new|new-default`, `# NAME:`, `# EFFORT: xhigh` (Opus 5 default; never medium/low; Fable 5.1 crucial-reasoning only; ultracode = large batches only), `# MODEL:`, `# ACK-VERDICT: <resolution>` when `last_verdict.txt` is NEEDS-DECISION/RED.
- Report line 2 = `verdict:`; Code ends its final message with a literal `VERDICT: GREEN|NEEDS-DECISION|RED`. **A NEEDS-DECISION or RED report is not acceptance whatever the exit code.** Read every report in full before the next brief.
- Roll the Code session at ~70 % context. Journal + CLAUDE.md before every session end.
- Local orchestrator waits on the report file / `watcher_status.txt`; cloud sessions use the signal relay (E:\MailBox\README.md).

## 7. Escalation boundaries
- **Ask the owner:** any client-facing meaning change; any delivery/timeline commitment; anything requiring the office PC or a physical action; Europa/Bench scheduling; spend outside the normal pattern (a Fable 5.1 brief, ultracode).
- **Orchestrator decides alone:** design, sequencing, technical rulings, gate design, brief content, session rolls, when to STOP a thread.
- **Reviewer must see before merge:** any change under `Source/AnomalyCapture/`, shaders, the label schema, the verifier/auditor.
- **Never silently:** reverse a measured ruling (§3–§4), skip a gate, widen a brief's scope fence, or edit a prediction after its leg.

## 8. Standing facts
Repo `D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector` (master, origin exists) · CaptureBench sibling repo · Lyra worktree (never a junction) · bench binaries `_binary_baselines\` · legs `_bench_sessions_bank\` · reviews `_reviews\` · office reads `_bates_reads\` · dashboard `E:\MailBox` (tray app, toasts) · engine UE 5.1 source build; Europa = UE 4.25 (parked).

## 9. Change log
- 2026-09-06 seeded by chat Claude at the Codex-pilot handoff (m49 complete `ac13700`; m51 next).
