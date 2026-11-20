# EFFORT: xhigh

SESSION 069, BRIEF 22 — THE BENCH COOK (authorised, as its own sequenced operation) and F1 validation on the new container. Headless; codename discipline; master tip d204598 (verify HEAD == origin/master). 069-21 accepted in full (the reference choice for G3 was the right call and is well recorded; the thrown-away m44-binary legs were handled correctly).

WHY NOW: every measurement sequence on the current container quartet is closed (m44, m45 merged). G118 is satisfied. The cook is required to validate F1 (global shader parameter change, G129) and to refresh the container on master.

TASK A — THE COOK, exactly per the repo's own procedure (CLAUDE.md / setup-runbook / the cook section of PRE-DELIVERY-CHECKLIST — follow the written steps, cite them; do not improvise a procedure):
 A1. Pre-cook: declare the map set; record the current quartet hashes; archive the quartet with a name carrying the milestone (m45 master d204598); confirm no measurement leg is in flight.
 A2. Cook on master d204598 (F1 NOT included yet).
 A3. Post-cook verification per the checklist: map gate, enforced token, the A44 scan, bench-lever grep, both build targets exit 0, boot. Record the new quartet hashes. The staged exe after the cook must boot on the new container.
 A4. RE-BASELINE on the new container, both tick orders: the alignment gates (ONSET 6/6, MASK-PICTURE-PAIRING 100% leg, G7 equality, MASK-TIE, m26 known-answer control, census health) — these are the new P-C7 controls for master. Any gate that reads differently from the pre-cook container is a STOP (a cook must not change behaviour; G103).

TASK B — F1 ON THE NEW CONTAINER (only after A4 is clean):
 B1. Merge branch m44-f1-resolution-mapping-UNVALIDATED onto a working branch `m46-f1-resolution-mapping` (F1 becomes milestone m46), rebuild both targets, re-cook if and only if the procedure says a shader change needs it (it does — G129; do the second cook the same sequenced way, archiving the A4 quartet first).
 B2. Gates, native order for the pixel legs, both orders for the alignment legs: MASK-PICTURE-PAIRING at 100% (prediction: unchanged, NEITHER 0 / PREVIOUS 0) and at 50% screen percentage (prediction: NEITHER 0 / PREVIOUS 0 — was CURRENT 0 / NEITHER 25-of-26 before, the banked prove-it-can-fail leg); ONSET 6/6; G7 equality; MASK-TIE 0; m26 known-answer; census health; P-C7 v2 at 100% against the A4 controls (prediction: byte-identical outside declared keys — the mapping is the identity at 100%); both build targets.
 B3. IF ALL PASS: commit `feat(capture): m46 mask pass maps output pixels through the internal view rect - masks/census/m26 correct at any screen percentage`, merge to master, push, docs (client-readme: masks correct at any screen percentage/dynamic resolution; client-delivery: the 50% leg joins MASK-PICTURE-PAIRING as a cook-time gate; CLAUDE.md: invariant (iii) from PENDING to LANDED, retire the nearest-neighbour mask-resampling follow-up in the ledger; G225 annotated FIXED; journal 069 §16; _binary_baselines with the new quartet and exe). NO TAG (… m45 → m46). Delete the UNVALIDATED branch after merge.
 B4. IF ANY GATE FAILS: F1 stays on its branch (renamed `m46-f1-resolution-mapping-GATE-FAILED`), master stays on the A4 container, report the numbers, no merge.

REPORT: pre/post quartet hashes, the checklist verifications, the A4 re-baseline table, the F1 gate table (50% leg with numbers), master tip == origin, exe hash, NEEDS-DECISION (expected none). STOP.
