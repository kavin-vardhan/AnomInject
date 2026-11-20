# 078-13 independent artifact and gate review

2026-09-06, Codex sole orchestrator. These are direct reads of the banked artifacts, checker source/output and runtime files. This record does not depend on accepting Code's pending final report. No capture, repository change, gate change or checker rerun was performed by Codex.

## Campaign outcome observed

The A reference is VALID/PASS: checker exit0 means READ; its actual counts are decidable33, CURRENT33, PREVIOUS0, NEITHER0, SAME-POS0, NO-DATA7. The approved coverage floor20 is met. All40 checker rows were read. NO-DATA indices are15,24,25,26,32,34,35; these remain missing probe evidence, not proof of correct mask production. Specifically,15/24/25/26/35 have state empty and null mask_file,32 is unmeasured/null, while34 is PRESENT with target_mask/frame_00034.png on disk but zero sampled probe pixels in the checker. Seven NO-DATA readings therefore do NOT mean seven missing mask files. Sustained FPS29.9236945724; speed_ratio1.0025500002. Four counted events; schema2.

The first candidate cell exhausted four B1-invalid attempts, runner exit2, and has no accepted bank or valid pairing verdict. No further candidate or conditional control was released. This is an incomplete campaign, not a pairing pass or a proved candidate pairing failure. A reference pass cannot resolve the candidate's validity failure or erase earlier valid failures.

## Independent image/label reconciliation

Codex read labels.jsonl, annotation.json and run_summary.json in all five physical attempt banks and the accepted reference alias. Every bank has40 RGB PNGs and40 label rows, unique indices exactly0..39, no image/row index difference, four events, and schema2. Every RGB and mask PNG passed PIL verify AND full pixel decode; all dimensions are1280x720. Every non-null mask_file exists at its exact session-relative path, without the facts helper's fallback. No corrupt/zero-byte image or missing promised mask was found.

Base: D:/IntrusiveAnomalies/_bench_sessions_bank/. Accepted reference alias M51R_F1R_A_REF_NAT duplicates reference try1 and is not another physical attempt.

| physical attempt bank | session suffix | masks present/empty/unmeasured | sustained FPS | speed_ratio | validity / pairing |
|---|---|---|---:|---:|---|
| M51R_F1R_A_REF_NAT_try1 | 20260906-204916 |34/5/1|29.9236945724|1.0025500002|VALID / PASS33 current,0 previous,0 neither,0 same,7 no-data|
| M51R_F1R_B_NAT_try1 |20260906-205107|40/0/0|29.9818697770|1.0006047062|INVALID B1 / NOT READ|
| M51R_F1R_B_NAT_try2 |20260906-205114|36/4/0|29.9998323760|1.0000055875|INVALID B1 / NOT READ|
| M51R_F1R_B_NAT_try3 |20260906-205122|37/3/0|29.9854658693|1.0004847058|INVALID B1 / NOT READ|
| M51R_F1R_B_NAT_try4 |20260906-205130|35/5/0|29.9393569421|1.0020255297|INVALID B1 / NOT READ|

## Why B1 did not accept a matching modal bbox

Read check_pose.py in full and a54_oracle.py::settle_window/a56_check. The existing gate requires modal rounded-bbox coverage>=0.90, <=3 distinct boxes, and a matching modal bbox. It does not accept solely on pose_match=True.

Candidate try1 and try3 have the calibrated modal bbox(0.0,485.2,306.1,234.8), but only16 of25 usable boxes match that modal value:64%, below90%. Their second boxes are respectively(0.0,484.9,308.6,235.1) and(0.0,485.4,303.6,234.6). Actual view rotations change by approximately0.17 degrees (try1 yaw0 versus359.83; try3 yaw0 versus0.17). Try2 has displaced modal rotation(338.54,358.63,0) and bbox(9.9,240.4,342.9,230.4). Try4 has displaced modal rotation(358.05,349.65,0), bbox(173.5,449.0,271.1,238.9), and62.5% modal coverage after settle.

These are observed validity inputs, not an established cause. They do not authorize a looser B1 threshold, another attempt, an owner-app intervention, or a retrospective pass. Pacing is recorded; it supplies no waiver.

Additional observation: candidate label rows include out-of-order session indices (e.g. try1 order includes5,7,6). The index SET is complete. m44_pairing_probe.py explicitly sorts by session_index, so its previous-picture comparison does not follow JSONL append order. check_pose reads append order for settling. This should be considered in the later identity/writer review; no new frozen-campaign gate is invented here and no causal claim is made.

## Provenance and configuration

Read both full runner outputs and recorded commands. They match the approved natural100 recipe: cap40,1280x720, blinking/StaticMeshActor_49, seed4242, SVE1, Delivery0, Pace1, Marker1, AE0, MaxAttempts4, MaskPairingProbe1. Independently extracted effective configuration and probe evidence from ALL FIVE physical attempt logs: exactly one tag255 spawn,40 STEP lines, natural order, internal/unscaled1280x720, cap40, Marker1, AE0, SVE and effective mask/target/census enabled. Full relative-file/SHA256 maps of each banked session match both its original raw attempt session and the copy under evidence/attempts; all five comparisons pass with no missing or altered file.

Read all eight receipt files in M51_FROZEN_PAIRING_RECHECK_EVIDENCE/receipts: before/after each of two swaps, before campaign, after both legs, and final restoration. All record the expected staged identity, all five frozen container hashes and zero StackOBot processes. Post-swap receipts precede their leg launches. Final state already held the candidate, so restoration was an idempotent hash verification, not an unnecessary additional copy.

Codex independently rehashed both archives, staged executable, all five container files, run_leg.ps1 and m44_pairing_probe.py after execution. All match the approved pins:

- A B8AC65D87030D311632A4605C8EFF5C6D54A1ACCB23BE76682C6E643498EEAD8.
- Candidate/staged D50DDE789A94933182DC6B3FA74811F8BB57DC1F8332B4AC88ACE47337BD4796.
- Runner 9E8D669F1C194FE5998FC277D9285EC17602D4399CC6EA6B4D1137E16FADB945.
- Checker 918EB4B5888A65D589C079D355AC09FBFC3D0987A69979C4DF3FC0F477A7AF1F.

Container quintet values remain exactly those in the approved prediction document and banked receipts. No source/build/cook or mutable gate was used to obtain these results.

## Release remains held

No candidate pairing cell is certified by this campaign. Synth100,natural50,synth50 and every conditional A control are UNRUN. Full sixteen-finding closure, both fixtures/orders and the existing release manifest remain required. Independent production-source review is in078-12-codex-f1-source-review.md, including the unchecked label append result, mask payload ownership defect, unresolved request/render identity and the limits of the F1 waiting counter.

The next step is evidence review and a bounded proposal. No additional runtime attempt, gate waiver, identity implementation, merge, push or tag is authorized by this record.
