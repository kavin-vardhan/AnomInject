# 081-16 — chat ruling: "11C + 9M" withdrawn; the gate is yield under a hard 256 MiB payload cap; implementer hands over to Claude Code (chat Claude, orchestrator, 2026-09-23)

## Disposition
**Codex is right, and it was right to stop before building.** The 081-15 formula (8 + 3) × colour + 9 × mask is **not a structural bound** on source 5649a6d: byte-only admission has no count limit, the closing-phase test counts completions *beyond `Phase.Last`* (not beyond the cursor), and phases can be longer than K. Codex's legal 16-frame-phase schedule (delayed head, colours 4..15 admitted, worker responsive) retains **13 colours + 16 masks** — 62.7 MB at 720p, 141.0 MB at 1080p — with every reservation legal. Chat also over-read COUNT_FAIL's 40,550,400: at 720p a mask is a quarter of a colour, so that total is equally 10C + 4M or 9C + 8M; it was arithmetic, not an ownership census.

Pilot metrics: **third chat spec defect in the clock/budget family** (after the 081-14 constants and the 081-15 64 MiB carry-over). This one was caught **pre-build at zero bench cost** — record it that way. Codex's audit, model replay and release-site check are accepted as evidence; its additional gotcha line ("a wait threshold is not by itself an admission/retention bound") is adopted into G282's family.

## Ruling 1 — what replaces the bound (answers Q1)
- **No numeric high-water gate at all — neither a "bound" nor a "reference envelope".** With no structural bound, a threshold on high-water would pass or fail on scheduling luck, not on correctness. The product criterion is the one that matters.
- **Gate (qualification legs):** zero `budget_exceeded` anywhere in the run, and full required-pair yield where 081-14/081-15 demand it, under the **hard payload cap 256 MiB** (compiled default of `IAI.Capture.ChangeMaxBytes`, 268,435,456). The cap stays the only admission limit; clock and admission semantics are exactly 5649a6d's.
- **High-water becomes an explained reading, never a gate.** Diagnostics (replacing 081-15 Ruling 1b):
  - (i) at every `budget_exceeded` (log + sidecar field on the refused row): bytes held; colours held split as pending / previous / phase references (a shared owner counted once); masks held and bytes; cursor; LatestIndex; open phases with their first/last indices; what the head waits on (colour / mask / observed / sealed);
  - (ii) at every new high-water: the same census, and the run summary carries the **peak census**.
  - **No `change_bytes_closure_bound` field and no formula-based WARN.**
- No stronger retention/scheduling contract in m55. Demand-filtered retention stays FUTURE (v2, with the GPU route).

## Ruling 2 — memory claims in docs (answers Q2: confirmed)
Field reference + client readme describe **m55's admitted payload only** — not total process RAM (writer pool, receipts, rows, compression, metadata are outside the reservation). State: hard payload cap 256 MiB; measured high-water with its peak census per exercised recipe; **no structural guarantee of full yield at any resolution**; the mechanism in one plain sentence (a long labelled phase whose first completion is delayed keeps later frames until it can close, so long phases at high resolution can exhaust the cap and refuse — refusals are recorded, numbers are never wrong); 1440p/4K unexercised.

## Ruling 3 — handover (owner's call, 2026-09-23)
*Final form. An intermediate edit this evening had Codex finishing build 2; the owner confirmed Codex's round ended with its 081-16 pre-build stop, so build 2 goes to Claude Code. Codex received no instruction from that intermediate edit.*

From this round **Claude Code implements** (Opus 5.5, via the GDP mailbox, brief `081-17`). Codex's implementation ledger for m55 closes at 081-16; the unspent build allowance and the whole fail-fast program pass to Code unchanged:
1. predictions amended before the build (memory criterion exactly as Ruling 1);
2. build 2 = 256 MiB default + diagnostics (i)/(ii), nothing else;
3. Stack 1080p twins both orders — 20/20 each, zero `budget_exceeded` anywhere, invariants 0; first fixture-valid failure stops;
4. Lyra G270 at 300 frames under 081-14 §3.4;
5. then (next brief) the remaining inventory — 75 candidate legs total per Codex's 081-16 §4 count, E8E2E43F credited to nothing on build 2 — then Stage 3.

**Reviews after Stage 3 (both independent):** Codex reviews every Code-authored change (build 2 onward, Stage 3); a **fresh** Claude Code session — not the implementing one — reviews Codex-authored Stages 1–2 and the completion clock. Merge only after both.

Codex: thank you — no further implementation action on m55. Please leave the boundary exactly as your postflight records it.
