# SESSION: new
# EFFORT: xhigh

NEW SESSION KICKOFF — session 068. Bootstrap cold, then TASK 1 only.

HOW THIS SESSION IS DRIVEN (new): you are running headless. Briefs arrive as files; your FINAL MESSAGE is the report chat Claude reads — nobody sees anything else. So: put everything that matters in the final message, in the structure requested at the end of this brief. There is no one to answer questions mid-task; if a verification fails or you hit a genuine fork, do NOT guess — stop, and make the final message a REPORT of what you found and what decision is needed. Never wait for input.

CONFIDENTIALITY (unchanged, absolute): office hosts are Concorde / Bates by codename ONLY. Never a real title, studio, product, packaged-exe or fork name anywhere — code, docs, journal, commit messages, replies. AnomalyCapture.Build.cs is the single unalterable exception. Never paste raw scrub-verifier output; quote only SELFTEST ok / VERDICT lines (G202).

STEP 0 — BOOTSTRAP (read, in this order, before touching anything):
  1. CLAUDE.md (status block + stop block + milestone map).
  2. docs/sessions/2026-09-02-067-p9-ledger-plan-and-card-fixes.md — especially §15 (enumeration, overlay semantics, Bates protocol), §16 (m38 plan, APPROVED), §17 (close-out).
  3. docs/CHAT-HANDOFF-p9-bates-localized-m37-shipped.md (untracked by convention; the current cold-reader handoff — supersedes the m36 handoff wherever they disagree).
  4. docs/invisible-anomaly-mechanisms.md §8.6a/§8.6b and docs/office-rdp-card.md "THE P9 BATES PROTOCOL" (C-1 done · C-2 optional · C-3 decisive).
  Then verify: `git rev-parse HEAD origin/master` both = be4dd1b; working tree clean except the four untracked docs/CHAT-HANDOFF-*.md; no .git/index.lock present. (A folder D:\IntrusiveAnomalies\_mailbox exists OUTSIDE this repo — it is the brief/report bridge; ignore it, never write into it.) If anything differs, REPORT and STOP.
  Your final report begins with a ≤15-line STATE SUMMARY (milestone map, staged exe, tags posture, P9 (A) closed / (B) open with the two sets, what task 1 is and why it exists).

TASK 1 (xhigh) — THE TRANSITION-DRIVER + TOGGLE-LINE-ANCHOR SOURCE READ. Docs-only. ⛔ No source edits, no build, no legs, no m38 work, no tags. ⛔ NO MECHANISM, LEAD, OR LIKELY-CAUSE for P9 (B) anywhere — factual mapping with file:line only. Frame-saga discipline holds.

  1a. THE FOUR FLIPS. For one blinking event as labeled — hidden {n, n+1}, visible {n+2..n+4}, hidden {n+5, n+6}, visible from n+7 — map which code path drives EACH of the four visibility flips (first hide · mid-event show · second hide · final show): Apply(), the Tick() while-loop toggle, Revert(), or the scheduler/auto-injection end path. Files: Source/AnomalyInjector/Private/Anomalies/Anomaly_Blinking.{h,cpp} plus the injector subsystem's apply/tick/revert path and whatever schedules an anomaly's start and end. State explicitly whether the two OUTER flips and the two INTERIOR flips go through the same call (SetActorHiddenInGame from the same site) or different sites. Table: flip | driver | file:line | call made.

  1b. THE CLOCKS. FramesSinceToggle advances per Tick() call. State, with file:line: (i) what calls FAnomaly_Blinking::Tick and how often relative to a world tick; (ii) how the label writer derives a blinking event's frame_indices / labels.jsonl rows (which counter, stamped where — journal 067 §15 has the Snap->SessionIndex site at ~:1917-1922; confirm); (iii) whether the toggle counter and the label counter are the same clock or two clocks, and if two, where they are reconciled. Bates runs ticks_per_captured_frame = 1.3556 (closed as capture_game_ticks/total_frames by construction). Record what that number means for a half-period counted in Tick() calls vs a cadence certified in captured frames — as arithmetic only, no interpretation.

  1c. THE VERBOSE TOGGLE LINE. Quote Anomaly_Blinking.cpp:95 exactly. Confirm it stamps ONLY the phase word and actor count — no frame index, no session index, no run-relative anything. Then answer, from the UE 5.1 source build and the plugin, what the line's PREFIX carries on Bates' sealed installed build: the engine log-line prefix (wall-clock timestamp with ms; the bracketed frame counter and what it is — GFrameCounter modulo what — and whether it is on by default for that build configuration / LogTimes setting; cite the engine file:line). We can add NOTHING to Bates — use only what its build already prints.

  1d. THE BRACKETING LINES. List every Log-verbosity (not Verbose) line LogAnomaly / LogAnomalyCapture already prints per captured frame or per event during a run that carries the session/frame index or the anomaly start/end — file:line and exact format — i.e. the lines the toggle line can be bracketed between in the C-3 bundle. State the ordering guarantee (or lack of one) between a toggle line and its neighbours: same thread? same tick? before or after the capture stamps that frame's index?

  1e. CONCLUSION — WHAT C-3 CAN AND CANNOT DISCRIMINATE. For each INTERIOR flip (mid-event show: labels n+2 / eye n+3; second hide: labels n+5 / eye n+6), state whether the C-3 bundle (toggle lines + bracketing lines + labels.jsonl + frame_indices + the owner's eye read) can place the toggle on a specific captured frame index; the resolution (exact frame / ±1 / tick-level only); and what would make it indeterminate. Then restate the pre-declared three-way comparison in those terms: which outcome pattern says "pixels are the outlier on this host" vs "sampling/labeling is the outlier on this host" vs "report raw". Outcomes name WHERE, never why.

  WHERE IT GOES: open journal 068 (docs/sessions/2026-09-02-068-<slug>.md) with §1 = this read, tables included. If 1e changes how the owner should READ the bundle, add a short "C-3 READ GUIDE" subsection to the card's C-3 section — read instructions only, no new steps, no new commands; otherwise journal only and say so. Update CLAUDE.md's "task-1 never executed" note to DONE with a pointer. Commit docs-only: `docs(p9): transition-driver and toggle-line anchor source read` (Conventional Commits; no tag).

FINAL MESSAGE STRUCTURE (this is the report; nothing else is seen):
  1. STATE SUMMARY (≤15 lines) and the verification result (HEAD/origin SHA, tree clean y/n).
  2. 1a table · 1b clock finding · 1c prefix answer (with engine file:line) · 1d list · 1e table + the three-way restatement.
  3. What was written where (journal path, card touched y/n, CLAUDE.md line) and the commit SHA.
  4. Anything that needs a human decision, clearly marked NEEDS-DECISION, or "none".
Task 2 (m38 implementation per journal 067 §16, gates i–iv) is QUEUED and will arrive as a separate brief after this report is reviewed — do not start it.
