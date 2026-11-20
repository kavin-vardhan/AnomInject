# 094-47t r2 — measured-write admission correction (OD16)

2026-11-05. Root records a correction under the owner's explicit "its fine if its below50GB, please proceed" instruction and existing technical delegation. OD15's fixed D:start40GiB was unnecessarily restrictive for a read-only protection proof. The worker correctly refused at32.709GiB and waited; root canceled only the verified owned pre-entry process at21:36:41IST, with no P9/build/runtime child started, and independently verified closure and unchanged protected state. The cancellation is documented in094-47t-codex-cancellation-review.md. No full worker report exists for that canceled attempt.

## Corrected admission for the same bounded job09447t

Replace ONLY the fixed D:start40GiB with operation-specific admission:
D: required free bytes =25GiB stop reserve +3GiB buffer/drain reserve + the proved maximum remaining additional D:allocation for the admitted operation/stage, including simultaneous copies and incidental growth. D:warning30GiB and stop at/below25GiB remain. A missing, negative, stale, wrong-operation or unaccounted peak budget is REFUSED for a large write/physical operation. Never treat zero as an implicit unknown budget.

For the fresh canonical entry P9 and its bounded parent/proof/report preparation, authorize a512MiB maximum additional D:allocation. Prior09447s proof/report logical footprint was95,727,111bytes; the512MiB allowance includes inventory, all hash records, source/compiler/connectivity receipts, parent admission, report and allocation rounding. This operation performs no build, query that mutates engine state, source edit or runtime. Its D:admission is therefore28.5GiB. Measure/check the operation's actual growth and retain the stop floor. Do not reuse this proof-only budget to admit a compiler or capture.

C:start13/warning11/floor10GiB and E:start15/warning12/floor10GiB remain. Apply the stricter of their start value and their measured peak+floor+retained reserve where applicable. All memory/idle gates and attempt/wait bounds remain. Use a job-scoped wrapper/integration only; no global policy reduction, no protected baseline modification, no fixed-start check hidden in a second wrapper.

The same calculation governs independent source/test/build stages once their complete incremental peak is proved. Before package/capture, still prove the full dependent campaign peak: package/cook/DDC/staging/archive/copies, all five full2352-frame sessions, lifecycle and review/drain margin. Do not require the package's capacity before a harmless proof, but do not treat a proof budget as package capacity. Actual write budgets, provenance and correctness gates remain mandatory.

Keep exact observed free-byte samples, effective thresholds, operation, budget identity and timestamp on admission/refusal/stop. Synthetic tests must reject missing/invalid budgets, wrong operation/job/root, insufficient headroom, floor crossing, cumulative/actual-growth over-budget and a large-write attempt using the proof-only budget. No invented margin or acceptance by a single transient reading.

## Continuation and preservation

Continue the SAME authorized worker job09447t as a fresh r2 execution, after the prior worker is fully closed. Root review/launcher receipts use existing run/root-receipts-094-47t-r2. Worker roots remain D:Receipts/09447t andTemp/09447t,E:09447t-evidence/package,C:IA_EuropaEvidence/09447t. This avoids new output-root expansion.

The14 existing047t receipt files and earlier helpers are preserved read-only inputs. Add distinct r2 admission/config/resource/validation files; never overwrite the first refusal, samples, parent admission, prior report or any old helper. Canonical P9/entry is still absent and must be freshly created by the unchanged full owner_p9.main(). Do not rerun the old fixed40GiB waiting helper. New operation helpers may reuse verified logic by copying to new filenames.

The reviewed47s1021-record reconciliation remains the parent. Revalidate it and the current47t parent admission against fresh independent root preflight. Existing47t parent SHA and exact pre-existing file inventory are pinned in the new prompt. Revalidate all368 Timestamp derivations and5147 protected headers; no new inherited drift or broad old-receipt trust. FullP9 precedes source repairs. ForwardEG1 integration, active can-fails, zero-engine-action query/full exitP9 remain.

One resumed implementer, no in-flight steering, root owns monitoring/full review. The previous process exit-1 and lack of a worker final are explicit; do not adopt47s report.md as a47t report. Preserve all prior outputs/events before reuse. This is a deliberate corrected continuation, not an automatic retry or erased failure. The owner's current instruction still authorizes this bounded work beyond the former20:30checkpoint; no automatic change of roles.

All OD1-OD15 and current047t brief obligations remain except this D:admission calculation and exact r2 receipt/config naming. No capture/label/texture/drop tolerance, Shipping/LOD/default, memory/idle, attempt, write cap, camera hold or publication gate changes. No owner-app/process/settings action or wider cleanup. Plugin push/merge/tag stays held; only the independently passing dashboard feature branch can be published underOD4.

Chat review NOT REQUIRED for this correction to resource admission under the owner's explicit lower-reserve/proceed decision. Next mandatory owner/Chat checkpoint: capture/label meaning, correctness tolerance or release-authority change; existing merge/release reviews remain.

