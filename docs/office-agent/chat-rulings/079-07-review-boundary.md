# 079-07 independent re-review boundary — 2026-09-15

Owner requested re-review only, with no briefs to Code. Review is complete:
D:/IntrusiveAnomalies/_reviews/079-07-codex-merge-rereview.md — **CHANGES REQUIRED**.
Read that report first, then its linked probe, follow-up, cohort and bank evidence as needed.
The active request is D:/IntrusiveAnomalies/_reviews/079-06-chat-request-for-codex-rereview.md.

Reviewed candidate fix/verifier-field-validity and its local origin-tracking ref remain
bff1b0058688feb1dfe427f840e1c1ce8eac4a37. Live checkout is m51 at
53bf725bd406f8ce3e38d6ed153b7ff77e3a3e67; master/origin-master ac13700; CaptureBench 472a409.
Both repositories are tracked-clean; existing untracked handoffs/tools were preserved.

All 24 self-tests and six pinned sessions pass. The saved 780-cell cohorts are complete;
28 independent rerun groups (432 cells) reproduce all 19 guard-off wrong cells and all
52 guard-on recovered cells exactly. New tests still produce a false SHIFT on aligned labels
and a false PASS on a late onset at m_edge=0. The report names eight remaining findings,
including incomplete edge constraints, empty-search verdicts, scorer accounting and batch collisions.

**Chat review REQUIRED for F1–F8, the corrected design and revised evidence before merge.**
The next mandatory checkpoint is Chat's disposition of this report. Chat retains orchestration;
Codex is the independent checkpoint reviewer under the 2026-09-15 operating-contract correction.
No brief, implementation, build, capture, cook, checkout, merge, push or external message was issued.
All review processes finished and this review's scratch directories/links were removed. Last watcher
observation idle, PID 18520, 2026-09-15 15:45:46 +0530. The 079-06 mailbox wrapper remains UNDECLARED;
its existing queue hold was not modified. Client/Section G, m51/campaign and paused <other-home-project> holds remain.
No fresh Codex session or context-setting change is recommended for this coherent workstream.

---

