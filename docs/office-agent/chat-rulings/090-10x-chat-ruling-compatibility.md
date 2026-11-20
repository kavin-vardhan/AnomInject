# Chat ruling on Codex's m53 compatibility review (`_reviews\bates-2026-10-01-diagnosis\compatibility-review.md`): accepted; A1–A4 are the spec for 090-10c; B is the owner's product call; C is out of scope for Monday; m52 runs first

Date: 2026-10-01 ~20:55 IST · Author: chat Claude · Office census on m53 `0a69ea3`: uv eligible 0 / 15,906 (`runtime_lod_bias` 14,454); normal `runtime_lod_bias` 15,538

## Accepted (Codex's findings)
- **The dominant bucket is per-binding T9** (`cinematic` / `per_texture` / `streaming_budget`), not the global E4/E5. That is a source-order inference: a failing global check would have rejected every candidate before any other reason appeared.
- **A proven over-refusal:** `StreamingBudgetPossible` refuses a merely *possible* streaming bias from raw cvars, even for fully resident textures. The engine computes `GlobalMipBias` as 0 in the Editor, floors fractional bias, and lets `UseAllMips` zero it (`TextureStreamingHelpers.cpp:299, 325-328`).
- **The copy design assumes the full mip chain from mip 0:**
  - T10 (`MaxNumLODs == platform mips`, all resident);
  - the render-thread tripwire;
  - `snapshot_mip=0`;
  - the RT sampler doesn't carry the global mip bias.

  So **deleting T9 alone would expose residency next, or copy wrongly.** The 55 `not_fully_resident` readings are first-failure only.
- **V1P whole-actor atomicity** (29 of 30 qualifying slots → refuse) is a chosen footprint rule, not a correctness requirement.
- `host_mid`, constant textures, VT and unsupported types need separate implementations.
- **Acceptance (Codex §5):** declared home-content positives, frozen before tuning; targeted **and** automatic application; visible corruption, restoration and truthful timing; unchanged-image control; genuine-refusal controls; full denominators. **A supported-host claim cannot pass with zero eligible.**
- **The mailbox status is stale:** the watcher's PID was absent at ~20:44 and output stopped at 17:38. The owner restarts the watcher. Nothing runs until then.

## Rulings
1. **The 090-10c spec = Codex A1–A4**, with chat's four-rule principle (a check may block only for label/picture disagreement, the wrong object, crash/leak/hard cap, or revert residue):
   - **A1:** suffixes reported separately; the all-reasons census (bounded; dependent checks marked unassessed).
   - **A2:** `streaming_budget` / `global_*` read the **effective** engine state, not raw cvars (Editor 0, floor, `UseAllMips`). This is the quick win.
   - **A3:** reduced resident chains supported. Take a coherent render-thread snapshot of the accessible mip interval; map source and destination mips; retain the source until the copy completes; handle resource changes mid-copy; update tripwires, accounting and telemetry together. **This also covers `per_texture` / `cinematic` / `not_fully_resident`.**
   - **A4:** the identity control preserves the effective appearance (sampler and global bias, addressing, filtering, sRGB, normal interpretation). The anomaly is applied only after it passes. **Don't force host textures resident and don't change host quality settings.**
2. **B (per-part admission) is a product decision; the owner was asked.** The question: when only some material parts of an object can be corrupted, (A) skip the object (his 09-28 all-or-nothing call), or (B) corrupt just those parts, **with the mask/label covering only the corrupted parts.**
   - **Chat recommends (B):** more yield, and labels stay honest because the mask is exactly the changed components.
   - Implement B **only** if his answer is recorded in `_reviews\090-10x-owner-answer-B.md` before 090-10c reaches that step. Otherwise keep all-or-nothing and **measure** what B would add (counts from the all-reasons census).
3. **C is out of scope for Monday:** `host_mid` sibling instances, constant textures, VT and unsupported types. Report their counts; it's the follow-up drop.
4. **Sequencing:** the m52 PIE round runs **first** (`090-10b2`): it is smaller and the more likely Monday deliverable. Then 090-10c (m53 A1–A4). Both are idle-gated tonight.
5. **Honesty to the owner:** UV/normal on his content needs the A3 rework, so it is **at risk for Monday.** The Sunday decision (ship what works on office content; the rest as a follow-up drop) is on the table now, not hidden.
