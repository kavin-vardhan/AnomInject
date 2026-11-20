# ToCodex — everything the incoming orchestrator needs, in reading order
Assembled 2026-09-06 ~14:30 by chat Claude at the handoff. Copies; the originals live where each file says. ⛔ Office data never leaves the office PC. ⛔ Codenames only: Concorde, Bates, Europa.

## Read in this order
1. `01-CHAT-HANDOFF-077-m49-complete-m51-codex-pilot.md` — the decision handoff written for you: verified state, every decision with its why, rejected approaches, compromises, forward plan, open questions, corrections, mailbox mechanics. (original: `StackOBot\Plugins\AnomalyInjector\docs\`)
2. `02-OPERATING-CONTRACT.md` — the one page that wins over every older doc: roles, hard rules, schema meanings, rulings R1–R8, gate bar, mailbox protocol, escalation. You own it from now; change it only by a dated line under its §9. (original: repo `docs\`)
3. `03-review-of-fix-capture-label-integrity.md` — chat's review of your branch `be16977` + the rulings, with §5 = the amendments after your reply. **This is 078-01's spec.** (original: `_reviews\`)
4. `04-external-release-review.md` — your 16-finding review, verbatim, with chat's spot-check note. (original: `_reviews\`)
5. `05-fix-capture-label-integrity.diff` — `git diff 21c1e0d..be16977`, the 3,424-line diff chat reviewed. Master has since moved to `ac13700` (Phase B merged); your rebase target is that.
6. `06-mailbox-README.md` — the GDP mailbox protocol incl. the new verdict gate. (original: `_mailbox\README.md`; the dashboard/relay README is `E:\MailBox\README.md`)
7. `07-report-077R-01.md`, `08-report-077R-02.md` — the last two Code reports (Phase B resume; the merge). Read them to see the FINAL MESSAGE shape Code produces and the evidence style the gates expect.
8. `09-brief-077R-02.md` — the last brief chat wrote, as a template for header directives, scope fences, step-0 state checks, rulings, gates, docs, final-message structure.

Then in the repo: `CLAUDE.md` status block (top = 077R-02), `docs/sessions/2026-09-04-077-m49-phase-b.md`, `docs/predictions/2026-09-04-m49-phase-b.md`, gotchas G236–G258, `docs/client-readme.md` §8.

## Your first actions
1. `last_verdict.txt` in `_mailbox\` reads **NEEDS-DECISION** (077R-02 §5 asked what your branch is). The decision is made (handoff §0.4, §2) — so brief 078-01 MUST carry a header line
   `# ACK-VERDICT: fix/capture-label-integrity is m51 source material (chat ruling 2026-09-06); not merged as a unit`
   or the watcher holds it. (Owner restarts the watcher once so the hold logic loads; `run_brief.ps1` hot-reloads.)
2. 078-01 step 0: push `fix/capture-label-integrity` to origin (it is LOCAL ONLY) before anything touches it.
3. Write predictions BEFORE the first leg and hand chat the predictions file for review (drop a note in `_reviews\` or tell the owner). Chat reviews the evidence bank before merge. Chat issues no briefs.
4. Ask Code to end every final message with a literal `VERDICT: GREEN | NEEDS-DECISION | RED` line.

## Mailbox quick reference
- Brief → `D:\IntrusiveAnomalies\_mailbox\to_code\NNN-name.md`; one in flight; a name runs at most once ever (`ran.txt`).
- Header directives: `# SESSION: new-default` (fresh session, adopted as default) · `# NAME: 078 m51` · `# EFFORT: xhigh` · `# MODEL: claude-opus-5` · `# ACK-VERDICT: …` when required.
- Report → `to_chat\NNN-name.report.md`; line 2 = `verdict:`; header carries cost, turns, context %. Live feed `to_chat\NNN-name.live.txt`. Wait on the file or `watcher_status.txt` (`state=idle`).
- Roll the Code session at ~70 % context. Model: Opus 5 at xhigh (never medium/low); Fable 5.1 only for crucial reasoning; ultracode only for large multi-stage batches.
- Cost ledger `costs.csv`; ≈ $1,082 across 071–077R-02.
