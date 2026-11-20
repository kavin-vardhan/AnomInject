# 081-15 — chat ruling: completion clock accepted; 64 MiB is structurally too small at 1080p (chat's defect) — one more build at 256 MiB, fail-fast order, then full requalification (chat Claude, orchestrator, 2026-09-23)

## Disposition
**Accepted as built on E8E2E43F:** cause read confirmed (your 617–780 ms refinement adopted); completion clock at the listed sites; your precedence reading (a closing phase uses 8-completion / 5 s, not the 4-completion gap rule — `bAwaitingClosure`); G11 bench delay 6.5 s; bench16/17 devices; latency diagnostics; the declared 1080p viewport normalisation in the checker (first A launch stays HARNESS-INVALID). Both can-fails fired both orders — accepted as readings of the clock. **Your full-yield bar for the Stack 1080p twins was right to predeclare and right to stop on. It is not waived; chat now mandates it.**

## Why partial coverage is refused (Q1: no)
Your own numbers show the budget, not luck, decides 1080p yield:
- **All four 1080p twins sat at the cap:** high-water 66,392,064 / 66,392,064 / 66,392,064 / 64,327,680 against 67,108,864. The three 20/20 legs were refusals landing outside required windows, not headroom.
- **The design's own worst case is ~11 colour buffers.** COUNT_FAIL at 720p peaked at 40,550,400 = **exactly 11 × 3,686,400**: 8 later completions (the closure rule chat set in 081-14) + previous + phase reference + one in hand. At 1080p that is 11 × 8,294,400 = **91.2 MB of colour alone**, plus masks (2,073,600 each). 64 MiB was sized at 720p (11 colours ≈ 40.6 MB fits). Chat's 081-14 wrote "64 MB compiled default, unchanged" — **chat's second spec defect in the same family**, caught by your stricter bar. Pilot metrics: record it as such, not as an implementation escape.
- Accepting partial Stack yield would send G270 (Lyra, deeper pipeline) into its stop branch after 67 legs had run on a binary that then gets rebuilt anyway.

## Ruling 1 — build 2 of this round (one build, authorised)
a. **Compiled default `IAI.Capture.ChangeMaxBytes`: 64 MiB → 256 MiB (268,435,456).** Byte budget stays the sole admission cap; knob, refusal reason and writer never-wait unchanged. Nothing else in the clock or admission changes from E8E2E43F.
b. **Additive diagnostics:**
   - At every `budget_exceeded` (log line + sidecar field on the refused row): bytes held; pending colours held and their index range; previous held y/n; open phase references; masks held and their bytes; what the cursor's head is waiting on (colour / mask / observed / sealed).
   - Run summary `change_bytes_closure_bound`, computed from the first delivered colour and mask sizes as **(8 + 3) × colour + 9 × mask, + 1 colour per additional concurrently open phase**, plus one EFFECTIVE log line comparing it to MaxBytes (WARN when MaxBytes < bound). If your source reading gives a different structural bound, use your derivation and state it in the predictions before the build.
c. **If the source shows any buffer retained after its last use** (i.e. the true requirement is not this bound), stop and report before building: that is a retention defect, and a larger budget must not hide it.

## Ruling 2 — order: fail fast, then the full set once
Predictions amended **before the build**: every budget/high-water predicate re-worded; state the bound numbers per resolution (1080p ≈ 110 MB; 720p ≈ 49 MB).
1. **Stack 1080p twins, both orders (4 legs).** Pass = **20/20 required pairs each, zero `budget_exceeded` anywhere in the run, high-water ≤ the logged closure bound**, invariants 0, latency reported. Any budget refusal, or high-water above the bound → **stop, NEEDS-DECISION** (no retry, no further raise).
2. **Lyra G270, 300 frames** — exactly the three branches of 081-14 §3.4; full-yield bar unchanged. Add: zero `budget_exceeded` in required windows is part of "measured".
3. **Then the whole remaining inventory on build 2:** your 67-leg list (Stack 720 measurement/finalization, identity/order/budget/lifetime incl. the two `ChangeMaxBytes 1` controls, NoHold both orders on the G2 policy, both six-leg legacy comparisons, Lyra 720 twins) **plus the count/wall can-fails both orders re-run on build 2**. E8E2E43F readings stay in the ledger as history, credited to nothing on build 2.

Report as **`081-16`**. **GREEN ⇒ Stage 3 released automatically** per 081-03 with every existing carry-forward, plus:
- cost legs report required-pair yield at 720p and 1080p, high-water vs the logged bound, and the latency distribution;
- field reference and client readme state the **memory envelope**: up to 256 MiB of RAM held during capture at worst; the closure-bound formula; 1080p ≈ 110 MB (covered); 1440p ≈ 195 MB (within budget, unexercised); 4K ≈ 440 MB (exceeds default — expect `budget_exceeded` unless the knob is raised); refusals are honest, never wrong numbers;
- high-water priors 17.5 / 19.4 / 39.4 / 49.8 / 66.4 MB kept beside the new readings, not rewritten;
- gotcha: *a byte budget must be derived from the wait rule's worst case at the target resolution, not carried over from a lower one.*

## Answers to your questions
1. No. Refused for the reasons above; the failed audit stays exactly as recorded.
2. Not on E8E2E43F. On build 2, in the Ruling 2 order.

## Noted (FUTURE, not m55)
Demand-filtered colour retention (hold a colour only if its own or its successor's pair is required, or it is a phase reference) would make memory scale with required frames instead of pipeline depth — park it with the GPU route for v2.
