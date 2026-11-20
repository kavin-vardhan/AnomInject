# 081-20 review plan — independent review of Codex-authored m55 source (031a103..5649a6d, Source/ only)

Written before deep reading, 2026-09-24 ~03:15 IST. Reviewer: fresh Claude Code session (Opus 5.5).
Source snapshot: `src-5649a6d/` (git archive of 5649a6d). Diff: `m55-source.diff`.

## Contract anchors
- Numeric: plan §1.1-§1.6 (d = max channel |Δ| on canonical BGRA8; strict >8; 8 bins [0][1-2][3-4][5-8][9-16][17-32][33-64][65-255];
  sum; mean = sum/n/255 4dp; control = M==0 (complement of ALL tags); empty_region if chg_n==0 or ctl_n==0; three asserts).
- Delivery scope §1.2: PNG, no resample, view 0, 4 handled formats, SVE only -> else unsupported_delivery.
- Identity §2 + amendments D1/D2/D4/D5/D7: token/epoch/cut minted at GT issue; stale generation rejected before admission;
  unique serving family bound to token; mask join by receipt; duplicate callbacks rejected; filter-then-freeze mask; share
  writer's canonical conversion; reserve bytes before retaining a ref; budget_exceeded without retaining.
- Pair validity §2.3 (epoch, cut, view 0, session N-1, rect, extent, format, both delivered, mask receipt match, n>0).
- Temporal §3: phase = maximal run of labelled frames; onset = (f0-1, f0); pending/measured/indeterminate; K=4 window;
  reference = f0-1 canonical; ref_* ; running max named _sofar; finalisation at phase end / event end / drop / teardown / run end;
  late results rejected and counted, never mutate.
- 081-15 completion clock: closure terminal when >=8 colour completions (delivered or failed) received beyond Phase.Last /
  ClosureWatermark, OR 5 s wall; gap rule >=4 later completions; no rule counts issues; byte budget sole admission cap
  (64 MiB in 5649a6d; 256 MiB is build 2, out of scope).
- Enum: 14 values (D4 amendment).

## Review questions and what settles each
Q1 (numeric) Does the per-pixel loop implement d = max(|dR|,|dG|,|dB|), strict >8, the exact 8 bin edges, integer sum,
   mean=sum/n/255? Are the control pixels exactly M==0 (not M!=t)? Does ref use the same region M_i==t?
   Settle: read AnomalyChangeMeasurement.cpp; port the loop to Python and compare against a direct formula on random data.
Q2 (numeric) Are buffer sizes validated (colour W*H == mask W*H == rect)? Out-of-bounds read if sizes differ?
   Settle: read the size checks before the loop.
Q3 (identity) Can a pair be joined to the wrong predecessor (not N-1), wrong mask (not current receipt), wrong generation?
   Settle: trace session index, token, epoch/cut from GT issue to stage admission; model the cursor.
Q4 (identity) Does an epoch/cut reset clear every holder together; can a result from before a reset be admitted after it?
   Settle: read reset path and every admission check for stale-epoch rejection.
Q5 (refusal) Every path from receipt to row: can any row be chg_measured=true when a §2.3 condition fails? Can a missing
   onset leave a phase "measured" (F1)? Can a later quiet pair move a phase out of indeterminate?
   Settle: enumerate emit sites; check phase state transitions.
Q6 (lifetime) Is every byte reservation released exactly once (finalize, closure, teardown, epoch reset, refusal)? Are
   shared colour owners counted once? Use-after-release?
   Settle: list Reserve/Release sites; model; cross-check banked peak census "0 unaccounted bytes" is build-2 only.
Q7 (threading) Which data is touched by GT / RT / writer workers / stage worker; is every access under the lock; are there
   MoveTemp/reallocation hazards (references into TArray/TMap held across unlock or across insertions)?
   Settle: read locking in AnomalyChangeStage.cpp line by line.
Q8 (scheduling) Worker re-queue: can the stage stop re-queueing with work pending (hang/starve) or double-schedule?
   Shutdown: can a queued task outlive the stage (UAF)?
   Settle: read Kick/worker loop and teardown.
Q9 (completion clock) Completion counting beyond Phase.Last/watermark; 5 s backstop; gap rule 4 later completions; precedence
   bAwaitingClosure. Can it hang (no completions and no time check), starve, or finalize too early (counting something that is
   not a completion beyond the phase)? Can failed completions double-count?
   Settle: read 5649a6d diff; Python model of the clock with adversarial schedules.
Q10 (memory) Behaviour at the cap: refusal emitted without retaining; does refusing the predecessor cascade correctly?
   Unbounded growth of any container (rows, receipts map, late-reject bookkeeping, per-index maps) on a long run?
   Settle: read containers and their erase sites.
Q11 (bench isolation) null_effect/solid_swap: excluded from GAutoPool/defaults/selector; Apply refuses without flag;
   AnomalyBench module type (DeveloperTool/Runtime?), .uplugin entry, Shipping exclusion; fixture commands inert without flag.
   Settle: read .uplugin at 5649a6d, Build.cs, module code, injector registration.
Q12 (agnostic/conventions) New deps; host types; comment counts per file.
   Settle: grep Build.cs deps and includes; count comment lines in changed files.
Q13 (perf) Per-pair loop cost at 1080p; number of passes over the frame per pair (chg + ctl + ref); allocation per pair;
   lock held during pixel loop?
   Settle: read loop; estimate ops.
Q14 (evidence) Do banked sidecars exercise the suspect paths (closure_timeout, late results, budget, epoch)? Replay sidecar
   invariants (hist sum == n, gt8 == bins 4..7, mean == sum/n/255, onset ref == chg) on the bank.
   Settle: Python replay over _bench_sessions_bank m55 sessions.
