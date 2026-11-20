---
name: reports-in-copy-block
description: "Always emit reports meant for the orchestrating Chat (plans, gate results, handoffs) as a single fenced copy block"
metadata: 
  node_type: memory
  type: feedback
  originSessionId: <uuid>
  modified: 2026-07-22T05:52:19.938Z
---

Any report destined for the orchestrating "chat Claude" (ferried by the owner) must be a **single fenced code block** so it can be copy-pasted verbatim — confirmed as a standing preference 2026-07-21.

**This explicitly includes plan-before-code deliverables**, not just gate/status/handoff reports. (I had been giving the M1/M2/M3 file-by-file plans as normal markdown; the owner corrected that — plans are Chat-ferried reports too.)

**Why:** the two-Claude split means these outputs are pasted into another session; markdown tables/headers don't survive a copy-paste cleanly, and a single fenced block does.
**How to apply:** wrap the whole report in one ``` fence; use plain-text layout inside (no nested md tables); keep it self-contained (the reader has no access to this session's scrollback). Normal conversational replies to the owner directly stay as-is. This is the CLAUDE.md "Reports to Chat go in a copy block" rule — the nuance saved here is that it covers plans. See [[dashboard-m1-m3-work-order]].
