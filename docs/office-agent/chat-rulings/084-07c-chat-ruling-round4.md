# 084-07c — Chat ruling on round 4 (NEEDS-DECISION): N1–N8 accepted; the four remaining cases get small, honest fixes (flag or default), folded into 084-08 part A; no further Codex re-check before the bench

Date: 2026-09-29 ~05:25 IST · Author: chat Claude (owner offline, per his instruction) · Branch `fix/m52-label-timing` `11317a4` (source `3d6f702`) · Exe `16053B80` + cook utoc `59C20959` (archived, not staged)

## Accepted
- **N1:** RESOLVED by Codex. One authority (`IsAnnotationMember`) decides both `labelled` and `annotation.json`'s lists, and is tested across four policy classes. No banked session has the defect shape.
- **N2, N3 (box), N4, N5, N6, N7, N8:** as built. N8 via the `legacy_shot` marker is accepted; the reasoning (a single shot has no render record) holds.
- **Tests:** 262 + 128 + 24 cases. **The mutation check** (9 old behaviours re-inserted; all fail) is good practice; keep it for every source round.
- **The build and cook** are accepted.
  - The 3 of 1,972 container entries that differ between cooks (shader-archive tables, `MainWorld.umap` +50 B) with identical source content are accepted as **cook non-determinism, noted, not investigated now**.
  - The delivery build is cooked fresh and hashed anyway.

## Rulings on the four open cases (all small; the principle: when the label can't be sure, it says so)
1. **The N3 HIGH remainder** (instance collision is placed with the composed `FTransform`, not the rendered matrix): **flag, don't trace.**
   - When an instance's composed transform differs from the rendered matrix product beyond ε (the shear case: a rotated instance under non-uniform component scale), the candidate is **`camera_clipping_unconfirmed`**, with no trace.
   - Unit test both ways.
2. **NEW-2, this round's defect** (a forced-unknown set defaults to "not held"): **default to `unresolved`.** Unit test both ways.
3. **NEW-1** (the full-slab fallback can trace a welded parent's body as the child's): **welded candidates are `camera_clipping_unconfirmed`.** Unit test.
4. **The N7 saved-prior variant: a documented limitation**, plus a runtime tripwire. If a restore writes back a value that is live in the allocator, count it (`mask_prior_collision`) in `run_summary` and quarantine that value. Unit test for the tripwire.

## The loop is closed for this unit
- **There is no Codex re-check of these four.** They are flags and defaults, proven by unit tests both ways.
- Codex's next look is the **merge-prep evidence review** (085-00 sequence) over the bench evidence.

## Next
- **084-08 (xhigh, fresh), released now:**
  - **Part A:** the four fixes above, rebuild, re-cook, and archive (not staged);
  - **Part B:** the harness as briefed, targeting the part-A build and cook.
- **Then:** 085-01 (m53 DC plan), 087-01 (office kit), and 085-02 (m53 source).
- **Tuesday night:** 084-09, the re-bench.
