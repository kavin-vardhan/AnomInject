# CHAT HANDOFF — P9 CLOSED (m40, Bates-validated) · m38 shipped · mailbox comms live
**Date:** 2026-09-02 (session 068 close, late evening) · **Supersedes:** CHAT-HANDOFF-p9-bates-localized-m37-shipped.md wherever they disagree.
**Confidentiality:** office hosts by codename ONLY (Concorde, Bates). Never a real title, studio, product, packaged-exe or fork name anywhere — code, docs, journals, briefs, memory, replies. `AnomalyCapture.Build.cs` is the single unalterable exception. Never paste raw scrub-verifier output (G202). ⚠ Photos of the office box can carry paths with names in them — do not transcribe paths.

---

## 0. READ THIS FIRST — what changed today, in one paragraph

P9 (blinking label desync on Bates) went from "localized, mechanism unknown" to **CLOSED** in one session: the owner's C-3 bundle on Bates gave the code's own toggle timestamps; they matched the owner's eye frame-for-frame and disagreed with the labels; a bench control showed the bench counts the apply-tick (Δ=+2) and Bates does not (Δ=+3); one candidate (subsystem tick order differs per host) fit every number with no free parameters and was recorded as "consistent with", never asserted; **m40** moved the label's visibility sample to `FWorldDelegates::OnWorldTickEnd` (after all tickables, before the draw) so tick order can no longer reach the labels; four pre-declared bench legs + a graceful-shutdown leg passed, including **P9 reproduced on the bench for the first time** via a bench-only synth lever; the owner pulled the editor build on Bates and re-ran C-3: **labels == code == eye, Δ still +3 — the strong pass.** Also shipped: **m38** (run-scoped `anomaly_log.txt`). And the chat↔Code copy-paste loop is replaced by a **file mailbox** (§7).

---

## 1. Current state (verified at close)

- **master == origin, tip `0bd1a6d`** `docs(p9): P9 CLOSED - m40 validated on Bates; card D done; session 068 close`. Never trust a remembered SHA — `git rev-parse origin/master` is the authority.
- **Shipped this session, in order:** m38 `7c06c6c` (run-scoped session log, gates i–v) · m40 `0864e7a` (order-independent label sampling + bench-only `IAI.Bench.SynthTickOrder` lever, default OFF, console-only). Docs commits between: `3e14385` (Task 1 source read) · `92c186a` (adversarial re-read) · `623ca24` (C-3 result + tick-order read) · `2c73e70` (m40 plan/predictions) · `970bf1d` (G209) · `2f16bf7` (card §D) · `0bd1a6d` (close).
- **Staged bench exe `C0AD3F91`** (m40). `F2FA6BCD` (m38) is m40's load-bearing A-side; `DC16710D` = the lever-only intermediate used for leg L2 (recorded in `_binary_baselines\README.md`, outside VC). Container quartet UNCHANGED all session — no cook.
- **NO TAGS** (highest m30). Office batch tag sequence: **m31→m33→m34→m35→m36→m37→m38→m40** (m39 slots in when it ships; numbers are identities, not order).
- **Census still compiled OFF** → master client-inert; P-C7 re-anchored at `C0AD3F91` (L1↔L4 byte-identical). No new client-facing keys (the lever adds none; m38's knobs are console/ini with delivery-mirroring default).
- **Bates editor build** was updated by the owner over RDP to `2f16bf7` (m37+m38+m40) — PIE/editor only; the packaged build is untouched. Bates' project runs **delivery mode** ⇒ m38's run log is auto-OFF there; `IAI.Capture.RunLog 1` forces it (card D-2, G210).
- **Standing mitigation LIFTED:** blinking is back in the Bates auto pool.
- **Milestone map:** m37 census defaults DONE · m38 run log DONE · m39 honest bbox NEXT (P-C13 conjunct-2 gate rides it) · m40 P9 fix DONE + Bates-validated.

---

## 2. P9 — the closed story (for a cold reader)

Labels are host-independent; pixels follow the code. On Bates the injector subsystem ticks BEFORE the capture subsystem (on the bench it is the other way round). The blink's first toggle therefore does not count the tick the anomaly was applied on (Δ apply→toggle = +3 on Bates, +2 on the bench), so every flip lands one tick later relative to the capture's arm — and the old label sampler, which ran inside the capture's own tick, shifted with it, so labels stayed at the bench cadence {n,n+1,n+5,n+6} while pixels showed {n,n+1,n+2,n+6}. Outer flips hide the shift (uncaptured settle tick / no-row frame n+7); interior flips expose it. Evidence chain, all measured: C-3 bundle (toggle prefix `[GFrameCounter%1000]` == labels' `frame_index%1000`, proven 704/704 on bench logs) · apply line `[35]` vs first toggle `[38]` · bench m38 run: 8/8 bursts Δ=+2 · Bates: Δ=+3 both events · L2 reproduction on the bench with the lever · L3 fix with the lever still ON · Bates re-run on m40: labels {28,29,30,34} == eye, Δ still +3.
**Why tick order differs per host:** it falls out of subsystem construction order, a `TSet` free-list reuse, plugin load order and the host's own subsystem classes — undeclared, uncontrollable through `FTickableGameObject`; on the bench the injector is constructed first yet ticks second. Never observed directly on Bates; **m40 made the question moot** (removed the dependency rather than confirming the cause). The ledger keeps every "consistent with" line as history.

---

## 3. Decisions made this session, with the why

- **Fix = option 2 (sample at OnWorldTickEnd), not pinning the order.** Pinning would change what a client build renders on whichever host disagreed with the pin — a label fix must not be a behaviour change. Option 2 leaves pixels untouched everywhere and removes the dependency so no future host can reintroduce it. Hook + World guard pattern already proven by m26.
- **Prove-it-can-fail = Route A, ratified with Code's substituted lever.** My first lever idea ("don't count the apply tick") shifts toggles and sample together and reproduces nothing; Code's relocation of the injector's dispatch to `OnWorldPreActorTick` reproduces the real reordering. Bench-only, default OFF, console-only, loud echo both ways, byte-inert when off (P-C7).
- **L2 executed on a lever-only intermediate `DC16710D`** because the pre-lever binary cannot run a lever leg; predictions file untouched, correction journaled (P-C2 route). Ratified.
- **Sync-fallback capture path NOT moved in m40** — same staleness class, no gate covers it, one-variable-at-a-time; documented limitation + detection (SVE-WANT-SUMMARY marksIssued vs framesWritten).
- **m40 numbered after m39, shipped before it.** Numbers are identities, not order; two prior renumbers were confusion enough.
- **m38 gate (i) markers re-ruled** (067 §16.4 named `M23 ARM`, which does not exist, and all three were census/mask-conditional): no-flags config, markers = run STARTED · grab point EFFECTIVE · a fire line · a revert line · the close marker. Gate (v) added: the Verbose knob raises AND restores, proven both directions with Task 1's "3 toggle lines per burst" expectation.
- **Task 1's adversarial re-read (fresh session) accepted**: flip table survives; two read-guide errors fixed (3-toggle rule false on the last event of a run; flip-1 anchor is a discriminator, not a check); row-2 of the three-way table re-named to the INTERVAL, not a subsystem — which is exactly what Bates then showed.
- **Chat = Fable 5.1, Code = Opus 5 (Fable 5 occasionally, never 5.1 on Code).** Deep P9 reasoning happens chat-side with direct source access; Code does reads, legs, implementation.
- **Effort table (owner):** xhigh = debugging/root-cause/design reasoning · high = long written artifacts (plans, journals, handoffs) · medium = mechanical implementation against an approved plan · max rare · ultracode = large multi-stage batches only · low never. A failed gate's fix turn goes back to xhigh.
- **Owner rulings still OPEN:** m38 run-log client default (chat recommends OFF until a client run's log content is scrubbed — the fork-probe echo lives in it; tonight's data point: delivery-shaped host was auto-OFF and needed forcing) · census ON-by-default (needs the Bates ceiling leg + §6 items).

---

## 4. Forward plan, in order

1. **m39 honest bbox** (drawn-box labels; P-C13 conjunct 2 required gate; uniform PIE pillarbox leg = instrument). Waits on nothing.
2. **Census-ON readiness** (chat's own source review of the census, 2026-09-02, all three are pre-conditions for ON-by-default):
   a. **Instrument side-effects on pixels** — the census tags ~26 random objects per batch with custom depth on captured frames. (i) A host post-process keyed on custom depth (Bates writes custom stencil on some actors, so one may exist) would highlight census-tagged objects in "clean" frames — unlabeled artifact, invisible to every counter. Fix shape: StartRun preflight that scans active post-process materials for custom-depth/stencil scene-texture reads, loud either way. (ii) Each tag flips the render proxy → motion vectors reset for a frame → possible one-frame TSR ghost/shimmer. The already-named "persist tags, rotate values in place" optimization removes the flips — promote it from cost item to correctness item.
   b. **Verdict expiry is a fixed 12 ticks; cycle length is not.** Bates cycle ≈4 ticks (fine); a 300-candidate host exceeds 12 → early-batch verdicts expire → silent recency bias (the loud counter fires only on ALL-fallback). Fix: expire relative to the last completed cycle's span; log fallback fraction per fire at Log level.
   c. **Two visible-sets, one measured.** Fire consults `GetVisibleRenderableActors`; census measures `GetCensusPrefilterActors`. Assert the second covers the first at fire time (count never-seen actors).
   d. The **m37 ceiling has never run on a real host** (bench max ≈6%): one Bates census leg (card D-5, blinking may stay ticked now) reads `aboveCeiling=` + the named >25 actor.
3. **Physical office visit (batched):** tags m31→…→m38→m40 (+m39 if landed) · G-R7(ii) split on master's cook · m31 V-3/V-4 · Concorde custom-depth census · tick-pin probe baseline · delete the parent branch · Concorde ini gains census keys only when census ships ON.
4. **Then:** resolution selection + PNG→JPEG + defaults profile (client channel check FIRST) · P5 blend-ladder · S-track (S4 depth, S5 backbuffer) · CPU-path deletion at next client cook (+ stale A3 help string) · sync-fallback sample relocation (follow-up to m40, needs its own gate).

---

## 5. Open vs locked

**Open:** m38 client default (owner) · census ON-by-default (owner, after §4.2) · §4.2 a–d · m39 · sync-fallback staleness (documented) · `census_fires_fallback_all=3` bench observation · Concorde items · tags · client channel open? (standing session-start check) · README/runbook drift class (outside VC).
**Locked (do not relitigate):** everything previously locked PLUS: option-2 fix + lever design · SEP_RATIO 5.0 · A54 blind to P9 · overlay one-counter fact · (A) closed as temporal AA · (B) closed by m40 · "consistent with" never promoted · numbers-are-identities · m38 gate set (i)–(v) · effort table · Fable 5.1 chat / Opus 5 Code split · mailbox protocol (§7).

---

## 6. Corrections — stale understanding to discard

- "P9 is evidence-bound / a host rendering question" → CLOSED; it was the label sample's position relative to a host-dependent tick order. Pixels, AA, capture, overlay all cleared.
- "The labelling code misbehaves on Bates" → NO: the label code is identical on both hosts; the INTERVAL (toggle relative to arm) differs. Re-named per 068-03 R5.
- "Exactly three toggle lines per event" → true for full bursts only; the last event of a 90-frame run is cap-truncated (reader says TRUNCATED; never count it).
- "The `SVE-WANT-TRACE arm` line is a backup anchor on Bates" → that build prints none; the prefix alone carried the join. Card step is now conditional.
- "Bates is sealed; nothing deploys over RDP" → the EDITOR build updates over RDP (pull + Build.bat, card A-1…A-3); the packaged build is what stays sealed.
- "`ticks_per_captured_frame` should be 1.0" (office LLM) → wrong; it is a run average incl. settle ticks, 1.3556 on every healthy run.
- "Tick order follows .uplugin module order" (office LLM) → wrong; bench has the same .uplugin and the opposite order.
- Journal 067 §15 line numbers → +6 / +61 after m37 (facts unchanged; mapping in journal 068 §1.0).

---

## 7. THE MAILBOX — how chat and Code talk now (STANDING)

`D:\IntrusiveAnomalies\_mailbox\` (outside every repo). Chat writes briefs to `to_code\NNN-name.md`; `watcher.ps1` runs Claude Code headless (`claude -p`, resumed session, permissions pre-approved by owner ruling) and writes Code's final message to `to_chat\NNN-name.report.md` plus a live feed `.live.txt`; the watcher window shows the feed live. Header directives: `# SESSION: new` (side session, not adopted) / `new-default` · `# EFFORT:` · `# MODEL:` (default `claude-opus-5`; never Fable 5.1) · `# NAME:`. Safeguards after tonight's incidents: a brief name runs at most once (`ran.txt`), the brief leaves the inbox before Code launches, any error halts the watcher (fail-closed), bookkeeping is non-fatal (`costs.csv` locked → `costs_pending.csv`). Cost ledger: `costs.csv` / `costs_summary.txt` (API-equivalent, NOT billed on Max; running total at close $92.01 across 6 recorded briefs). Owner: double-click `start_watcher.bat` once per boot; `STOP` file halts. Chat polls via scheduled reminders (~10–30 min). Incidents: 068-02 was delivered 7× by a watcher bug (Code refused to redo work, journaled G209); a locked `costs.csv` halted the watcher once. Project doc: `claude/mailbox-protocol.md`. The owner's photos/transcriptions of Bates reads live in `D:\IntrusiveAnomalies\_bates_reads\` (chat-owned, outside VC).

---

## 8. Pointers

`CLAUDE.md` (status + milestone map + mailbox note) · **journal 068** (§1 source read · §4 rulings · §5 m38 · §6 adversarial re-read · §7 C-3 result + bench control · §8 tick-order read · §9 fix options · §10 m40 plan · §11 m40 implementation + legs + L2 deviation · §12 Bates validation · §13 close) · `docs/invisible-anomaly-mechanisms.md` §8.6a (P9, now opens with the closure) · `docs/office-rdp-card.md` §D (m40 validation, DONE) + C-3 READ GUIDE · `docs/predictions/2026-09-02-m38-*`, `2026-09-02-m40-*` · `docs/gotchas.md` G209 (Get-Content -Raw on a 0-byte file), G210 (delivery mode mirrors the run log OFF) · `_bates_reads\2026-09-02-p9_bundle-bates-transcription.md` · CHAT-HANDOFF-p9-bates-localized-m37-shipped.md (previous).
