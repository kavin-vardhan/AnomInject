# NAME: 080 m52 stuck low mip (3)
# MODEL: claude-opus-5
# EFFORT: high

# 080-05 — merge m52 to master; then the read-only m26/m49 disagreement diagnosis. Resume session 6ffe5e76 (66%).

## 1. Merge (no code changes)
`feat/m52-stuck-mip` `791f8c8` → `master` `3ff88db` via `git merge --no-ff`, message `feat(stuck-mip): m52 stuck_low_mip — streamer-enforced low mip hold, verified restore, per-texture evidence, ratio gate (closes 080-01..04)` with a Conventional-Commits body: lever (L4′), sharing policy S4 + `MaxCoAffected 0`, window-at-first-held, verified restore, destroy-revert, G269 fix, verifier allowlist, `GAutoPoolDefaultEnabled` OFF (owner decision pending m55), S2′ designed-not-built, m55 named. Push master. NO tag. `m51` untouched (CLAUDE.md conflict remains for m51 resume). Keep the branch. Gate before merge: selftest + contracts pass on the branch tip; nothing else re-run. Update CLAUDE.md status on master (m52 CLOSED; catalogue now 10 anomalies + stuck_low_mip; pool default list unchanged), `docs/anomaly-catalog` entry final, client-readme row present.

## 2. Diagnosis — m26 veto vs m49 observability disagreement (READ-ONLY, report only)
From 080-03 §"Observed, not chased": an event deleted as `MEASURED_ZERO maxCount=0` by the m26 veto while its own rows carry `target_pixels` 16,020–16,184 and `observable:true`; association 2 vetoes / 25 fires on `DC8BA0C1` vs 0 / 46 on `A8742A4A`. Do: read the m26 veto's measurement source (which pass, which tag, which frames it samples — cite lines) against m49's per-row `target_pixels` source; identify how the two can disagree (candidates: different tag/namespace; veto samples before the hold lands / during pre-roll; veto reads the census bit; window-at-first-held moved the rows but not the veto's sampling window); check the banked leg's log for the vetoed event's timeline (fire tick, first held frame, veto sample frames). Prediction first (one paragraph in the journal), then the read. Output: mechanism established or explicitly not, with the exact lines; a proposed fix sized in lines and the gate that would prove it; whether it affects other deferred-onset anomalies (lod_popping?). No code. No captures unless a single targeted replay of the banked recipe is needed to read a log line that was not retained — say so if you do.

## 3. Report
Merge SHA, `master == origin/master`, CLAUDE.md diff summary; the diagnosis with citations; `VERDICT: GREEN` alone on the last line (merge done + diagnosis written) or NEEDS-DECISION.
