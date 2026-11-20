# 078-15 offline findings and disposition of Chat's07814 review

2026-09-07. Chat's review was read in full (SHA256324D25578484526FF11A35B572407AD00C1AC059238050C17650CB2AD9E3B7B2). Codex performed the two approved offline checks directly. No capture, binary swap, build/cook, pairing checker run, gate/source edit or repository write occurred. Banks are unchanged. This is not a new Code mailbox brief or a fresh campaign.

Scope was recorded before analysis in078-15-offline-check-scope.md. Reproducible script:078-15-offline-analysis.py. Evidence:_reviews/078-15-offline-evidence, with original/sorted copies, exact command arguments, full stdout/stderr, JSON details and input/tool SHA256s. Python -B prevented importing the unchanged checker from writing bytecode into the repository. All10 checker invocations returned the expected0/1 with empty stderr. Every copied row preserved its parsed content; source and tool hashes were verified unchanged after analysis.

## 1. Sorting does not reverse a candidate verdict

| physical attempt | descending adjacent index pairs in original | original exit / sorted exit | original settle start / sorted start | effect |
|---|---:|---|---|---|
|A reference try1|8|0/0|1/0|diagnostic start changes; still PASS B1|
|candidate try1|7|1/1|0/0|all printed diagnostics unchanged|
|candidate try2|5|1/1|14/14|all printed diagnostics unchanged|
|candidate try3|11|1/1|0/0|all printed diagnostics unchanged|
|candidate try4|5|1/1|4/4|all printed diagnostics unchanged|

The hypothesis that file ordering changes the B1 disposition of these four candidate attempts is not supported. The reference start changes, so do not claim the checker is generally order-insensitive or exonerated. Nonmonotonic append order also exists in the pre-F1 A reference. All original candidate attempts remain INVALID. No gate edit or historical known-answer repair test is triggered by a candidate verdict flip, because none occurred.

## 2. Same-index field comparisons

All ten pairings of the five physical attempts were compared at40 common session indices: four A-versus-B comparisons plus six B-versus-B comparisons. Anomalies were joined by(id,target_name,start_frame), not by arbitrary array position. All250 matched anomaly-record comparisons had no unmatched identity records. Dimensions, frame_index and saved view.origin agree in all pairings. Exact saved view comparisons include rot, origin, fovDeg, aspect and valid; no tolerance was invented.

| A versus candidate | rows with identical full saved view /40 | matched anomaly records /25 with differing bbox | matched anomaly records with identical view | differing bbox with identical view |
|---|---:|---:|---:|---:|
|try1|24|9|16|0|
|try2|1|25|0|not observed: zero comparable anomaly records|
|try3|26|9|16|0|
|try4|1|25|0|not observed: zero comparable anomaly records|

Across all ten pairings there are48 matched anomaly-record comparisons with exactly identical full saved view data; none has a differing bbox_px or bbox_valid. This is repeated comparison coverage, not48 independent experimental samples. The try1-versus-try3 candidate comparison supplies the other16 equal-view matched records. All other candidate-pair comparisons have different views on their anomaly rows.

Thus Chat's proposed same-view/different-bbox signal is NOT OBSERVED in these saved comparisons. Different saved rotations accompany all observed bbox differences. This neither proves a pacing/settling cause nor certifies that the saved view and projected geometry belong to the correct render. The origin of the rotation differences remains unresolved. In particular, there is no equal-view anomaly coverage with which to clear tries2/4. The existing live-projection concern must remain in the identity/geometry design.

## 3. Corrections and limits on Chat's causal predictions

- This was NOT the F1 candidate's first effective-mask run.07805 had candidate effective-mask pairing PREVIOUS counts9/12/10/17 in both orders at100/50.07806 preserved those failures. See the full07805 report and the earlier independent reviews. Do not reset that history.
- Session204916 to first candidate205107 is111seconds, not20seconds. No interleaved randomization occurred.
- B1 uses file-order settling and saved label fields, but identical fields do not establish time-clustering; different bbox under identical recorded view would not alone establish an F1 cause; different view does not identify why it changed. Queue/arm/render/view identity still requires direct evidence.
- The proposed A/B/A/B/A/B physical probe is NOT released. 'Candidate failure rate much greater' is not a defined criterion, and three attempts per binary cannot prove the binary-versus-time dichotomy asserted by Chat. Any such proposal needs exact predictions, meaningful uncertainty, explicit outcomes and review before execution. Do not automatically replay the exhausted frozen campaign or change B1.

## 4. Next step and review request

The two approved offline prerequisites are complete. Under Chat's07814 section3 authorization, the next Code unit may DRAFT identity/lifecycle/geometry predictions using these findings, without execution. Source/gate/fixture changes, new probes/levers, builds/cooks and physical legs remain held until the exact draft is reviewed. The invalid candidate baseline and old valid pairing failures remain explicit unresolved inputs, not waived gates. Do not blindly clone one rendered mask into records for different capture frames.

Chat: please review these offline readings and corrected interpretations alongside the next prediction draft. No additional owner approval is needed for already-authorized prediction-only drafting; execution still needs its established review. Codex remains sole queue owner.

Current m51 HEAD46139fd3ef20fab90d0768492a987a9952cceb62, Source/Shaders still frozen6713f51, ten owner handoffs preserved. The old Code session is80% and must roll before reuse. Former watcherPID16056 is absent and heartbeat stale since2026-09-06 22:07:02; no brief is queued. These checks do not require the watcher. See ToCodex/11-CURRENT-CODEX-HANDOFF-2026-09-07.md for safe session continuity and next dispatch.
