# 081-18 — chat ruling: bench blocked by another project; long-run readings deferred; bench-free Stage 3 oracle next (chat Claude, orchestrator, 2026-09-24 02:50 IST)

## Disposition
- **No long-run leg ran — correctly.** Foreign UE 5.7 HeistCrewUE editors (not ours) appeared from 01:32 IST and kept appearing (start times 01:32, 01:47, 02:05, 02:31 IST — an automated workload, not a person), so the brief's 60-minute bound fired. Code never touched them, stopped at the bound, restored the boundary (postflight PASS, `problems: []`), and used the idle time on an offline re-read of 081-17R. Conduct accepted in full.
- **Offline re-read accepted as a reading, not an answer:** on Lyra 1080p the PNG writer itself completes fewer frames than are armed (23.2 in, 21.7 out per second; writer latency decile medians 672 → 1528 ms) and the evidence stage's ready-head wait tracks the head's own writer latency (ρ 0.95); StackOBot plateaus (0–1 ms). The ON-vs-OFF question — does evidence slow the writer — is still unmeasured, and throughput cannot be measured while another project's editors share the machine.
- **Code's proposed smallest change** (stage worker queued at High priority in the same pool) is noted, **not adopted**: it cannot help while the stage waits for the writer's own completions, and it could take pool time from a writer that is already behind. The decision waits for the ON/OFF readings.
- **Chat process note:** the scorecard edit at 01:07 tripped Code's preflight (declared correctly as an external update). Rule for chat: never modify an existing file under `_reviews\` while a brief is in flight; add new files only.

## Next
1. **081-19 now (bench-free):** Stage 3's verifier `--change-oracle` + selftest, developed and run against the banked m55 captures. No Unreal launch, no build, no bench — it is unaffected by the other project's editors.
2. **081-18 rerun (unchanged predictions and harness)** once the owner schedules the PC so no other project runs Unreal during GDP bench legs. Then chat decides: requalification on build 2, or a scheduling/writer change as build 3 first.
3. **Owner decision (morning):** how the PC is shared between Heist Crew and GDP bench work.
