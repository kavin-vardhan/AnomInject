# CHAT-HANDOFF 081 — m55 "measured change evidence": Stages 1–2 built and mostly gated; Codex implementing; Stage 3 next

**Written 2026-09-23 by chat Claude (orchestrator) at session close. Cold-reader brief for the next chat.** Supersedes `CHAT-HANDOFF-079-verifier-closed-codex-implementer.md` for status; that doc still holds the 079 verifier semantics and the Codex-pilot data.

## 0. Read this first (60 seconds)

- **What m55 is:** the plugin now measures, per captured frame and per tagged target, how many of the target's own mask pixels changed vs the previous captured frame (count > 8 byte-diff, 8-bin histogram, mean) with a paired whole-frame control, and per event latches onset/phase evidence — so a label can carry *measured* visible change instead of the m49 presence rule. **v1 publishes measurements only: no present/absent verdict, `observable` unchanged.** Calibration and any `observable` change are a later unit and an owner/client decision (operating contract §3).
- **Where it is:** branch `feat/m55-change-evidence` (built source `cb14f31`, docs commits after; exact head in `_reviews/081-12-evidence/postflight.json`). Candidate exe **`A699E9EE`** archived at `_binary_baselines/StackOBot.exe.m55-stage2-A699E9EE`; Lyra modules `m55-stage2-fixture-lyra-6ef55ade`. **No cook** (G-NOCOOK held throughout; five containers unchanged). Master still `031a103`, `m51` `53bf725` untouched. Live checkout is m51 with candidate binaries staged — do not infer binary identity from the checkout.
- **Who does what:** **Codex implements** (decided 2026-09-21 on the 079 pilot numbers: 5 turns/zero P1 escapes vs Code's 5/8/6). Chat rules on every Codex report via `_reviews/081-NN-chat-ruling-*.md`; the owner ferries by pointing Codex at the file. **Claude Code does the independent review after Stage 3**, then merges. No mailbox brief has been in flight since 081-02.
- **Next action for the new chat:** wait for Codex's `_reviews/081-13-…` (NoHold on MainWorld + R5 Lyra). GREEN there ⇒ write the Stage 3 request for Codex (spec in `_reviews/081-03-codex-implement-m55-v1.md` §Stage 3 + carry-forwards below). Then a Code review brief (mailbox) modelled on 079-14.

## 1. Current state

### Accepted
- **Stage 1 (identity/lifetime):** 36/60 legs — StackOBot G1–G15 both orders (30/30), Lyra G13–G15 both orders (6/6). The 24 Lyra G1–G12 legs are **DROPPED-BY-OWNER-DECISION** (the owner, 2026-09-21: "skip Lyra if not suitable"; host spawns the pawn wherever per launch and the Stage 1 candidate had no placement lever).
- **Stage 2 (measurement):** 22 gates on `A699E9EE` (081-11) + natural moving twins + both six-leg legacy comparisons vs `AEBD09EA` (081-12). Numbers that matter: static null onset `gt8/n` 0.00006–0.0005, solid 1.0, control ~0.01–0.02; moving null 0.028–0.044, solid ~1.0, control ~0.045 (rises with motion, reported not subtracted); delay=3 first three window samples < 0.1, index-3 transition > 0.9 — the F1 gradual-arrival counterexample answered end-to-end via the pre-onset `ref_*` comparison. Actual OpenLevel **teardown passes both orders** (was the Stage 1 UNEXERCISED line). Occluded onset via cooked `/Engine/BasicShapes/Cube.Cube` duplicate + line trace ⇒ `empty_region` ⇒ indeterminate.
- Both 5/5 fixture preflights (081-11) on both revisions.

### Open (in order)
1. **NoHold gate** (`StuckMipNoHold` ⇒ `no_labelled_frames`): ruled 081-12 to run on **MainWorld** with the banked m52 G11 recipe (target `SM_rock_02` actor `…2044254803`, `T_grunge_mask` ratio 20.48), no lock/placement, camera rows vs the m52 bank as pose precondition. Allowance natural 2 / synthetic 3. Earlier attempt failed-precondition (CB cube had zero textures).
2. **R5 Lyra**: null/solid natural + G-3 observation on the G270 recipe only; L2 placement + L1 check; **first-leg 3 invalid ⇒ Lyra dropped for m55 entirely, no question asked** (081-07). Zero attempts used.
3. **Stage 3**: verifier `--change-oracle` (same pair ids, current-mask region, > 8, count-vs-count, mean-vs-mean, "validates arithmetic/transport only") + selftest incl. `0,3,6,9,12`; cost A/B/B/A at 720p and 1080p StackOBot only (≤ +1 ms game thread and writer throughput unchanged within uncertainty, else UNRESOLVED); docs (field reference, client readme with Codex's wording adjusted to "no verdict in v1", catalogue note for bench twins, gotchas, CLAUDE.md on branch). Then Code independent review → merge by Code.

### Carry-forwards into Stage 3 (verbatim obligations)
- **FPS 21.5 vs 30 on evidence-OFF MAIN4** (081-05): any cost leg at that FPS must be explained before the gate is read.
- Retained-byte high-water priors 17.5 MB StackOBot / 19.4 MB Lyra.
- The historical UNEXERCISED block (same-size cut; letterbox; FOV-only; clean hitch; producer ramp; clean sync fallback; separate-run stale work / real world replacement; JPEG/resample/backbuffer legs; other colour formats) carries verbatim with dated amendments — Codex has been doing this correctly every round.
- Delay=3 readings go into the client-readme paragraph.

## 2. Decisions this session and why (not yet in repo journals beyond Codex's own)

- **Codex design review (081-02) → all nine findings accepted; plan rewritten as three contracts** (numeric / identity-lifetime / temporal), closed refusal enum (now **14** values incl. `current_undelivered`, `closure_timeout`), sidecar `change_evidence.jsonl` (a per-row field cannot ride `labels.jsonl` — row is serialised before the frame's canonical bytes exist), GPU route parked, calibration removed from m55. Why: frame-to-frame differencing cannot attribute cause (079), a missed onset must read indeterminate never absent, and identity must be proven per receipt (m51 lesson).
- **Identity contract (081-04):** epoch/cut bound at request creation; capture token minted at issue shared by colour/mask/counts; one supported view owner; closure watermark with 8-frame/2 s deadline; filter-then-freeze mask; byte budget ≤ 3 canonical buffers, never blocking writer workers; sidecar-only event detail; `cam_delta` as three quantised fields, caveat only.
- **CB_GateLevel mask invariant redefined** (081-05): stencil tag numbers vary run-to-run from the shared pool even on the old binary; compare occupancy + nonzero-byte == own `mask_value`, never raw bytes. Gotcha G274.
- **Fixture:** Lyra spawn is host-chosen per launch → L2 placement after settle then L1 check (081-06/07). StackOBot's flaky pose was **focus transitions feeding mouse deltas into look input** (`SearchHost` stealing foreground on the owner's PC; failures only on partial-foreground launches) → bench-only input lock (`SetIgnoreLookInput/MoveInput`) + placement on CB, focus still recorded and required for measurement legs (081-09). Harness self-faults (guard refusal before sampling) are HARNESS-INVALID, ≤ 2 fix-rebuild iterations per round, never counted against 5/5 (081-10).
- **Bench twins:** `null_effect` and `solid_swap` (Lit magenta — unlit glows onto neighbours and inflates the control, G50/m29); catalogue 10→12, excluded from auto pool, `Apply` refuses outside bench. `prev_target_pixels` is −1 on every onset by construction (masks arm only on a labelled fire) — v1 does not establish pre-onset visibility; pre-roll arming is a queued unit.
- **m54 (UV/normal corruption) shape agreed with the owner (chat, 2026-09-22):** reuse the `corrupted_texture` material takeover but with plugin materials `M_CorruptedUV` / `M_CorruptedNormal` that pull the host's own textures via `GetUsedTextures` (base = sRGB, normal = `TC_Normalmap`), dynamic instance per target; UV modes (tile, drift, U↔V swap, lerp to TexCoord[1]) and normal modes (green flip, invert, drop, noise) as label keys; one cook for both; opaque single-sided targets only, refuse others; mesh-level UV rewrite rejected (shared render data, stripped CPU vertex data). ~2 Codex days. Sequencing stays m55 → m53 flicker → m54 → m51 pairing → M3 packaging.

## 3. Locked vs open

**Locked:** Codex implements m55; Code reviews after Stage 3 and merges; v1 = measurements only; `observable` unchanged; no subtraction; CPU route; K=4 experimental; light class in m53; stuck-mip default-on unchanged until m55 lands and the owner rules; Lyra scoped to twins + G-3 with auto-drop; no cook in m55.

**Open — the owner's calls, when they become real:** stuck-mip default-on for M3; `observable` semantics change (needs calibrated policy first); S2′ one-hold-N-events for Lyra-like yield; M3 packaging checklist items (.env, setup script fail-loudly / accept `env`, verifier README); Jira GDP-6/GDP-9 re-scope; m51 resume (CLAUDE.md conflict, parked contract entry, never-kill-by-name rule).

## 4. Corrections / superseded
- "Stage 1 blocked on Lyra" — no: closed on 36 by owner decision.
- "StackOBot fixture regression in Stage 2" — no: focus confound on the host PC, both binaries affected; fixed bench-side.
- 081-08's expected "empty-onset via offscreen teleport" recipe was withdrawn by Codex (would not establish occlusion); replaced by the real occluder.
- Histogram claim "sweep any threshold later" is false — exact thresholds recoverable are 0/2/4/8/16/32/64 only.

## 5. Pointers (repo, for Code/Codex to read; a fresh chat cannot)
- Plan v2.1+: `docs/predictions/2026-09-20-m55-change-evidence.md` (amendments at top; v1 under SUPERSEDED fold).
- Field reference: `docs/change-evidence-fields.md`. Journals: `docs/sessions/2026-09-2*-081-*.md`. Gotchas G272–G278 (tag pool, look-input lock, L2, harness-invalid rule, delivered-bytes conversion).
- Review chain: `D:\IntrusiveAnomalies\_reviews\081-01 … 081-12` (each Codex report + `*-chat-ruling-*.md`; evidence dirs and frozen snapshots beside them). Ledgers: `081-12-evidence/final-ledger.json`.
- Client-side: `D:\IntrusiveAnomalies\M2-client-feedback-jira-tickets.md`; the `.env`/`env` client thread (rename to `.env`, rerun `host-tools\config.bat`; reassurance sent).
