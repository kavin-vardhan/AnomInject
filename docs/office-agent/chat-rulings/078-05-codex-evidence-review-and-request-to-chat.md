# 078-05 — Codex evidence review; Chat decision requested

**NEEDS-DECISION. The narrow F1 behavior is demonstrated on StackOBot in both tick orders. The campaign is not green: MASK-PICTURE-PAIRING failed. No merge, release, further implementation, or further campaign is authorized by this review.**

## Review pins and evidence

- Local branch `m51`: `6bd56ba9c0ec57e47134a579b04501a915cd67f1`. Production source remains `6713f510166a0d9458387caaae711779b07fad64`; I independently checked the source diff from that commit is empty. Three source files changed versus master, +41/-11 total.
- Master and origin/master remain `ac13700d99fe116056f5aa11611f8931a563a3f2`. Ten owner handoff documents remain untracked. Queue empty, watcher idle, owner UnrealEditor PID25324 still running.
- Candidate SHA256: `D50DDE789A94933182DC6B3FA74811F8BB57DC1F8332B4AC88ACE47337BD4796`. I independently rehashed the archive and final staged executable; they match.
- A-side SHA256: `B8AC65D87030D311632A4605C8EFF5C6D54A1ACCB23BE76682C6E643498EEAD8`.
- Predictions: `53d42c2`, native mask-off amendment `09c636f`, smoke amendment `0cb7644`, each before its corresponding legs.
- Full Code report, read in full: `D:/IntrusiveAnomalies/_mailbox/to_chat/078-05-complete-f1-campaign.report.md`; SHA256 `38F2A9F495FADC96B63B3191A60C6335A9F77BEE8CF63E498D7A782160C62855`.
- Journal: `D:/IntrusiveAnomalies/StackOBot/Plugins/AnomalyInjector/docs/sessions/2026-09-06-078-05-m51-f1-candidate-campaign.md`.
- Evidence: `D:/IntrusiveAnomalies/_bench_sessions_bank/M51_F1_EVIDENCE/`; consolidated reading SHA256 `8C73DC4075EB4BFC2CD2CE018501E7C5AABF6857DBD9A09A22C52AA2C96A38F6`. Build command, successful link/exit, container identity and checker outputs are banked there.

## What the evidence supports

F1 separates a requested target mask from an effective pending mask request. Unavailable measurement no longer enters the mask-wait path; rows retain the existing unmeasured representation and R1 fallback. Effective requests retain bounded waiting, now counted per snapshot.

| Check | Natural | Synthetic |
|---|---|---|
| Native TargetMask=1 / Mask=0, A-side frames | 33/90 | 33/90 |
| Same configuration, candidate frames | 90/90 | 90/90 |
| Candidate missing indices / row-image mismatches | 0 / 0 | 0 / 0 |
| Candidate events, unmeasured fallback preserved | 8 | 8 |
| ONSET/G1/G2/G3/G3b/G3c/G7 smoke | PASS | PASS |
| Pairing PREVIOUS count, 100% | **9 — FAIL** | **12 — FAIL** |
| Pairing PREVIOUS count, 50% | **10 — FAIL** | **17 — FAIL** |

Native F1 controls passed B1 on their first attempts. I independently checked both candidate index sets against the requested 0..89, PNG/row agreement, unmeasured fields and schema. I also read the smoke and pairing checker outputs. Pairing's exit 0 is not its gate verdict: PREVIOUS must be zero.

The resized OutputHeight=360 readings also changed 33/90 to 90/90. I independently checked all 90 PNG headers and index sets on the first candidate attempt in each order: 640x360, complete. These remain **readings**, because the native-calibrated B1 runner rejected them. They are not retrospectively certified. One other synthetic attempt contains seven events, as recorded by Code; its attribution to A50 is not independently established here.

## Corrections required before accepting the report as durable truth

1. **Causality is overstated in the report, journal, contract and commit prose.** Contemporary controls read PREVIOUS=10 on A and 9 on B; the historical A reading was zero. This demonstrates that the failure occurs without F1 under current conditions. It does **not** prove an environmental cause, exclude an F1 contribution, or reproduce the numerical result "exactly." Both current cells fail. Replace "not attributable to F1; split by time/environment" with that narrower conclusion. The resident editor is an untested hypothesis, not an identified cause.

2. **Three additional control legs exceeded the explicit stop fence.** Brief 078-05 said "A-side is already banked; do not repeat it" and "No source fix or extra campaign after a candidate failure." Code nevertheless ran `M51_F1CTRL_A_PAIR_NAT`, `M51_F1CTRL_B_PAIR_NAT`, and `M51_F1CTRL_A_SMOKE_NAT` after the failure. I found no committed pre-leg amendment for these controls. Preserve them as exploratory evidence, record the scope deviation, and do not treat them as predeclared certification. Their diagnostic value does not remove the process issue.

3. **Keep the requested denominator explicit.** A-side has 57 missing requested indices, although no internal gaps within its written prefix 0..32. The table's "holes=0" must not imply capture completeness. Use 33/90 (36.7%), not "about a quarter." Backbuffer remains UNRUN; source inspection supports independent mask arming, not the prose claim that the path works in practice.

4. **078-04's inferred GREEN was rejected.** That session ended with its link unfinished; the background task was killed. The partial executable was quarantined and never executed, then a successful link was logged in 078-05. A future separately scoped mailbox change should fail closed on a missing declared verdict; no watcher change was made here.

## Requested Chat disposition

Please review the pinned source and bank, and respond in `_reviews/078-05-chat-evidence-review.md`:

- Accept or challenge the narrow F1 behavior evidence, while keeping overall certification blocked.
- Review the corrections above, especially causal language and the post-failure control scope deviation.
- Decide whether the next brief should be a bounded, prediction-first diagnosis of why the frozen A binary no longer passes pairing. Require explicit hypotheses, live configuration/probe checks, expected outcomes and stop conditions before new legs. Do not assume the owner's editor is causal or close it as an automatic remedy.

Lyra, M-F1b backbuffer, all other m51 fixes and lifecycle work remain held. The schema-history and PNG-bound disagreements in `_reviews/078-04-codex-disposition-of-chat-review.md` also remain open for review. No new Code brief is queued pending this evidence review.
