# 082-08 — Chat ruling on Codex's S1 evidence review (APPROVE-WITH-FIXES): all findings accepted; S1 closes as a scoped stage

Date: 2026-09-28 (IST) · Author: chat Claude · Branch `2ca9faf` · Review `_reviews\082-08-codex-m53-s1-evidence-review.md` (Astra/max, 31 min, Relay)

## What Codex established independently
It used its own code, never the campaign lib, reading the raw bank.
- **Every sampled verdict reproduces exactly:**
  - G-ID U-C2 0/194, N-N1 0/180, MASKED 0/154;
  - PRE-CLIP 2,048 px, with real clipping;
  - G-ID-M M-A-MIN 0/200/182;
  - G3 U-C2 and N-N1 at 0 same-index, with exact restores and balanced ledgers;
  - G-BIND (3);
  - G-REASON T9S;
  - the FS2O rollback, with the delayed-release accounting explained byte for byte.
- **Integrity:**
  - the chain is consistent, with 15 superseded attempts plus 1 retained invalid, all enumerated;
  - **no fishing;**
  - the old FAILs are preserved in history.

## Rulings on the findings
| Finding | Ruling | Where it goes |
|---|---|---|
| **P2-1:** after `TARGET LOST`, rows read `observable: null` instead of false | **ACCEPTED as a real capture-layer label defect.** My 082-07l ruling asked "is it false?", and it is not; the known negative is lost. It is not an m53 defect: `annotation.json` is correct and m53 stops and restores correctly. | **m51/M3:** a known termination gives `observable: false`, and `target_pixels: -1` stays. It applies to every anomaly with a destroy path. |
| **P2-2:** the G-BIND layer check is existential per event | **ACCEPTED as a harness defect.** The historical result is recovered: Codex found 1,192 px at d ≥ 16 on **every one of the 57 frames**. | **S2 harness:** a per-frame (minimum) check that reports the failing si. The S1 record cites Codex's per-frame reading. |
| **P2-3:** partial application is disclosed as a count only | **ACCEPTED.** The event is honest as an installation with partial coverage, but a consumer can't localise it. | **S2 source:** additive telemetry keys with the positive component/slot identities and the texture-to-slot association. |
| **P2-4:** TRIP accepts a missing counter | **ACCEPTED as a harness defect.** The bank shows the key present and zero in 191/191 summary-bearing sessions. | **S2 harness:** the key is required on summary-bearing legs; census legs are explicitly N/A. |
| **P3-1:** the completion headline overstates | **ACCEPTED. It was chat's wording in the 082-07l ruling; I own it.** MainWorld has **zero measured target-region frames**: its target is mask-unmeasurable (`nanite`). | **S1 record:** "43 F-SYN pixel-plus-log restore rows + 1 MainWorld log-only row"; "TRIP: 191 measured zero + 2 census N/A". **S3:** a MainWorld arbiter for a nanite target. |
| **Block-only proof scope** | **ACCEPTED with Codex's condition:** scoping to rows with a block needs a presence requirement over the active lifetime. The bank satisfies it: of 9,657 m53 label entries, 21 omissions, all exactly the post-destroy rows. | **S1 record:** cite the scan. **S2 harness:** an active-lifetime block-presence assertion. |
| **The cheapest escape:** a < 32 tint on the global half, or a one-frame-per-event layer transform | **ACCEPTED as the declared cost.** The actual global half reads 1, so it passes the original ≤ 2. | **S2 harness:** the global-half far strip (> 16 px from the layer half) becomes a **gate at ≤ 2**, together with the per-frame layer check. |

## The S1 record must say (honest scope)
- **Established:**
  - identity on the synthetic encoding, mip/geometry and shared-sampler rows, under the tested recipe (AA off, native order);
  - that the instrumentation faults are visible, including real alpha clipping;
  - slot-binding restore and release accounting on the exercised paths;
  - live refusal plumbing and the exercised precedence cases;
  - FailStep rollback at the instrumented steps (including a resource created before rejection), the destroy and NoApply-2 watches, frame-cap finalisation, graceful world end, GC survival, foreign-slot preservation, and simultaneous UV/normal ownership;
  - collateral set membership (the collateral readings themselves are incomplete, not clean).
- **Not established:**
  - the five declared debts (T10 live, `no_normal_map` Q, LEVEL-CHANGE, RECREATED, cancel-before-focus);
  - production-mode perceptibility;
  - both onset tick orders;
  - m52 interaction;
  - host yield, strength and cost;
  - any MainWorld pixel verdict;
  - label completeness (P2-1, P2-3).
- **The amendments,** classified as Codex did: evidence-backed corrections (G3, the fixture repairs, TRIP's source), coverage deferrals (T10, CAN, A2), and **declared weakenings** (the G-BIND ring and global half < 32, the F-MW partial-yield rule). Do not call them all "corrections".
- **S1 authorises neither default-on nor a release.**

## Next
- **082-09 (high, continue the 082-07l/08 session):** the S1 close-out docs only. Everything above goes into the predictions (an "S1 RESULT" section), the journal, the status block and the gotchas. No harness or source change, no bench. Commit and push; no tag, no merge.
- **Then:** S2 planning (xhigh, fresh session). It folds in every S2 item above, plus the earlier debts and the cause-read list.
