# HANDOVER — Claude Code Desktop becomes the GDP orchestrator and reviewer; Codex becomes the implementer through the Relay

Written 2026-10-02 ~13:20 IST by chat Claude, at the owner's request ("after this round, we're going to delegate the work to Codex instead of Code as the token usage is pretty high … Code's job will simply be to check Codex's code and then to keep sending briefs … We will use the Relay workflow").

**You are a Claude Code Desktop session on the owner's PC, talking with the owner (the owner) directly.** This file is your kick-start. Read it fully, then do the bootstrap reads (§1). Summarise the current state to the owner in 6–8 plain lines before you do anything else.

## 0. Your role, and everyone else's
- **The owner (owner):** product and scope calls only. He does **not** write code and does **not** make technical decisions. He runs the office tests and reports **numbers only.**
- **You (Code Desktop):** the orchestrator and reviewer, replacing chat Claude. You:
  - write each implementation brief for Codex;
  - launch it through the Relay;
  - **review Codex's code and evidence in full** (read the diff, re-run the key checks yourself);
  - rule (accept / fix / reject) in writing under `D:\IntrusiveAnomalies\_reviews\`;
  - send the next brief.

  **You make the technical calls.** Ask the owner only for product, scope or timeline decisions, anything at the office, or a physical action.
  - Keep your own token use low: let Codex do the building, bench runs and long investigations. You read, check, rule and brief.
- **Codex (`gpt-6-astra`):** the implementer. It changes source, builds, runs the in-engine proofs, commits and pushes, through the Relay (§3).
- **Communication with the owner** (his standing preferences):
  - lead every update with a one-line bold banner (🟢 DONE · 🔴 ACT/DECIDE · 🟡 HEADS-UP · ⛔ BLOCKED);
  - plain words, scannable, no jargon; mark items DONE / QUEUED / FUTURE;
  - 🔴 only for things he must act on or decide;
  - talk like a friend, not a report generator;
  - **never quote his messages back to him.**

## 1. Bootstrap reads (in this order)
1. `D:\IntrusiveAnomalies\_reviews\CHAT-HANDOFF-084-delivery-week-label-sync.md`: chat's living handoff. Read **§0** and **§3a-30 through §3a-40** fully; skim the rest.
2. The rulings since 10-01:
   - `_reviews\090-08-chat-ruling-codex-recheck.md`, `090-09-chat-ruling-round-a.md`;
   - `090-10x-chat-ruling-compatibility.md` (**the four-rule principle**), `090-10x-owner-answer-B.md`;
   - `090-10b2-chat-ruling.md`, `090-10c-chat-ruling-r1.md`;
   - `090-12-owner-decision-proxy-blur.md`.
3. Codex's diagnoses: `_reviews\bates-2026-10-01-diagnosis\shader-admission-review.md` and `compatibility-review.md`.
4. The office readings (numbers only): `D:\IntrusiveAnomalies\_bates_reads\2026-10-01-second-office-host-m53-census-owner-reads.md`.
5. The Relay: `D:\IntrusiveAnomalies\_relay\README.md`, `relay_run_codex_gdp_v1.ps1`, `gdp-relay-block-v1.txt`. The pattern it borrows: `E:\<personal-folder>\tools\relay\README.md` (**read only; never modify anything under `E:\<personal-folder>`**).
6. The running round's brief: `_mailbox\to_chat\090-10f2-finish-e-mask-uniform-bscope-cooked.live.txt`; the brief text is the `_active_` copy, or `_mailbox\processed\` once it's done.
7. The repo's own `CLAUDE.md` status block on `feat/m53-uv-normal-corruption`, and `docs/PRE-DELIVERY-CHECKLIST.md` §0.

## 2. Where things stand (13:20 IST Fri 2 Oct; delivery **Mon 5 Oct**)
- **Delivery scope (owner):** blurry textures (`stuck_low_mip`), plus UV and normal corruption (uv tile/scramble, normal invert/green_flip). **The governing rule:** labels and masks match the picture, 0 frames off, measured from pixels; every gate proven able to fail; no circular gates.
- **UV/normal now works on real office content:** on m53 `88801bd`, the second office host found targets that **visibly corrupt** (owner, 10-02 ~10:30). The first office host isn't tested yet.
- **Blurry still fires ~0 on office content.** The office counts show `shared_world` 196 (164 with ≥ 9 users, max 6,155).
  - **Owner decision, option 2:** Monday ships blurry **available but OFF**, with a readme limitation.
  - The **per-target private-copy ("proxy") blur** is built on branch `feat/m52-proxy-blur` and joins Monday **only if fully proven at home (PIE and staged) and on an office host by Sunday noon IST.**
- **The running round, 090-10f2** (Claude Code via the mailbox, headless), started 10:04. It covers:
  - Part 0: finish and push 090-10e;
  - Part M: no labelled frame without a measured mask;
  - item 1: the `texture_uniform` refusal (**MUST-FIX**: labelled-but-invisible events on uniform textures);
  - item 2: the B component-scope proof (it found and fixed a real bug: `bbox_px` covered the whole actor);
  - item 3: the cooked-bias staged proof.

  At 13:05, the 12 PIE A-legs were running and the staged legs came next. **The owner wants it to finish; it is the last mailbox round.**
- **Heads:**
  - **origin** m53 `48a63a2` (090-10e wrap-up), fix `ba1af33`;
  - **local, unpushed until 090-10f2 finishes:** m53 `9e081fe`, fix `7d4debc`, and beyond;
  - master `b5f15a3`; m51 `53bf725` (**never touch**).
- **The owner will pull ONCE, on Saturday morning,** after the blurry fix is proven: one commit with everything, plus a short test checklist from you.

## 3. Your first jobs, in order
**Job A: close the mailbox era cleanly.**
- Wait for 090-10f2's report (`_mailbox\to_chat\090-10f2-*.report.md`; the watcher writes it when Code ends).
- **Review it in full and rule** in `_reviews\090-10f2-ruling.md`. Accept only if:
  - Part 0 is pushed;
  - Part M reads 0 labelled-without-mask frames;
  - `texture_uniform` has tests both ways and **every labelled auto-pool event is visibly corrupt in PIE**, with the can-fail on `88801bd` (11 invisible);
  - the B scope is proven in-engine (mask, bbox and m26 confined to the eligible component), with the `NoPartScope` can-fail;
  - the cooked-bias staged proof is done, or a precise blocker is reported (dbghelp: report it, never work around it);
  - strict 0/0, the lever audit passes, archives are re-hashed;
  - the m53 head is pushed.

  Anything missing becomes Codex's first fix item.
- **Then stop the mailbox watcher** by creating an empty file `D:\IntrusiveAnomalies\_mailbox\STOP` (its documented stop). **One bench, one implementer:** the mailbox and Codex must never run builds or bench legs at the same time.

**Job B: build the implementer Relay** (the existing GDP Relay is reviewer-only and read-only).
- Create `D:\IntrusiveAnomalies\_relay\relay_run_codex_gdp_impl_v1.ps1` (from the reviewer runner) and `gdp-impl-block-v1.txt`. Codex as **implementer** may:
  - edit source in the designated worktree, build with UBT, cook, stage and restore by receipt, launch the editor/game for bench legs, commit and **push feature branches**;
  - **never** touch `m51`, `master` (except a merge **you** explicitly brief), tags, `ToCodex\` or `E:\<personal-folder>`;
  - **never** change any security setting (Defender, firewall, exclusions);
  - **never** kill a process it didn't start, and never by name; never touch the owner's processes;
  - **delete nothing** except its own scratch, by receipt;
  - use **codenames only** (office hosts: "the first/second office host"); **office data never comes home.**
- **Sandbox:** prefer `-s workspace-write` with `-C <worktree>` plus `--add-dir` for each root the job needs (`E:\IA_BuildCache`, `D:\IA_BankOverflow`, `D:\IntrusiveAnomalies\_reviews`, `_binary_baselines`, the staging folder, the engine's Intermediate if UBT needs it).
  - **Prove it with a smoke job** (a tiny build of the fix-branch host, no source change).
  - If UBT or the editor can't run under it, **ask the owner once** before using `danger-full-access` for implementer jobs. **Never edit his global `~/.codex/config.toml`;** pass overrides per job.
- **Model and effort:** pin `-m gpt-6-astra` and `-c model_reasoning_effort="max"` per job (the owner's Relay pin), and record both in `run-meta.txt`. Never below `high`.
- **Waiting:** Codex jobs take hours. Launch them in the background and wake on `exit-code.txt`. **One Codex implementer job at a time.**
- Keep the per-job prompt / report / ledger pattern (`runs\<date>-<id>\`, `ledger.csv`). The implementer's final message ends with `VERDICT: GREEN|NEEDS-DECISION|RED`, like the mailbox briefs.

**Job C: the first Codex implementation, the proxy blur.**
- The spec is ready: `D:\IntrusiveAnomalies\_mailbox\_held\090-12-m52-proxy-blur.md`. Adapt it to the Relay (drop the mailbox header lines; append the implementer block); it builds on the latest pushed m53 head after 090-10f2.
- **Deadline:** proven at home by Saturday morning, so the owner can pull once.
- **Review it yourself** before telling the owner to pull:
  - the diff;
  - only the target blurs (co-users unchanged, from pixels);
  - the label edges (the m52 E1 gate **and** the 0-frames-off FireWindow edge check);
  - mask scope;
  - the pure-texture hold route unchanged;
  - the can-fail;
  - strict 0/0;
  - the lever audit.

**Job D: Saturday morning, the owner's single office pull.** Give him **the exact branch and commit** (the proxy branch contains everything), plus a 5-line checklist:
1. Compile.
2. `IAI.TexCorrupt.Census allreasons`: read the eligible counts for uv and normal.
3. UV and normal on both office hosts: do they visibly corrupt?
4. Blurry: does it fire and visibly blur only the target? Then `python tools\anomaly_refusal_counts.py "<log>"` and the blurry lines.
5. The office check kit (`tools\OFFICE-CHECK.md`) READ BACK lines on a short capture.

Numbers only, as always.

**Job E: the independent review of the Claude-Code-built rounds.** Run **one Codex reviewer job** (the existing read-only Relay, Astra/max) over 090-10b2 → 090-10f2 (m53 `0a69ea3` → the 090-10f2 head), using the same questions as 090-08: label vs picture, mask scope, gate can-fail, the four-rule audit. **You rule on its findings; Codex fixes them as implementer in a later job.** Run it while the owner tests on Saturday, if the bench is free of implementer jobs (it's read-only, so it can overlap a bench-idle period).

**Job F: weekend delivery path** (`docs/PRE-DELIVERY-CHECKLIST.md`):
- fixes;
- the extended smoke (chat's draft: `_reviews\drafts\090-11-smoke-levers-harness.DRAFT.md`; re-pin to the final head);
- merges `fix/m52-label-timing` → `feat/m53-uv-normal-corruption` → `master` (only once the gates are green);
- the delivery package: **Development/Test, never Shipping** (the plugin is excluded from Shipping);
- the rehearsal install and launch check.
- **`dbghelp.dll` has been refused PC-wide since 09-30.** If packaging needs it, tell the owner plainly; never work around security.
- **Sunday noon:** the proxy-blur cut-off. **Sunday:** give the owner one clear ship / limit / slip decision with the evidence.

## 4. Rules that carry over (non-negotiable)
- **No build goes to an office host until it has visibly worked in a running game at home, in PIE AND staged** (checklist §0). Audit gates with `allreasons` on office-like content.
- **The four-rule principle:** a check may block a fire only for (1) label/picture disagreement, (2) the wrong object, (3) crash/leak/hard cap, or (4) revert residue. Everything else is a logged note. Bench-only proofs live behind `-IAIBench`.
- **Label sync is the product.** Every gate needs a can-fail. Circular gates are banned. Codex's reports are information; **you** rule.
- **Idle gate for bench legs:** no foreign editor or GPU-heavy process; an open but idle foreign editor doesn't block when the owner has been idle ≥ 15 min, GPU < 10 %, headroom ≥ 6 GB. **Never touch his processes** (he renders in UE 5.7 and uses Blender on this PC).
- **Office data never comes home** (capture sessions, frames, logs, client data). Only the owner's typed readings go into `_bates_reads\`.
- **Codenames only.** Never a real title, studio or fork name; never the office's own project names.
- Never touch `m51`, tags, `ToCodex\` or `E:\<personal-folder>`. Delete nothing (E: and D: floors are 50 GB; ask the owner before cleaning).
- **Every Codex prompt is UTF-8 through `cmd /c` redirection** (not a PowerShell pipe), per the Relay README.
- **Session end:** a cold-reader handoff markdown in `_reviews\` (or the plugin's `docs/` via Codex), the repo journal and `CLAUDE.md` status, and a scorecard row in `_reviews\081-17-codex-vs-code-comparison.md` per job (turns, minutes, defects found in your review).

## 5. Open owner items (ask only when they become real)
- **Default-on decisions per anomaly**, after his Saturday office test.
- The codename question (the repo lists a host name the owner still used in chat as retired, G222. In files, write "the first/second office host"). Settled 2026-11-21: the hosts are Concorde, Bates and Europa only (`START-HERE.md` §4).
- README Step 1 (the only placeholder left).
- `dbghelp.dll` (Windows Security → Protection history).
- Restart the MailBox tray app (the GitHub signal has been stuck since 09-30).
