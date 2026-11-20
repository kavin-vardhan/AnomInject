# 094-40 — Chat ruling: GREEN. The 4.25 capture port is ready for the office. The smoke passed every gate first try, the history is redacted, the scan is clean, Code pushed `feat/europa-port` = `d1cdc7b`, and the office capture card is written

**Date:** 2026-11-02, about 06:25 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Mailbox report 094-40:** GREEN. It ran 04:03–05:03 IST.
- **Codex's report:** `_reviews\094-40-codex-report.md`.
- **Code's review:** `_reviews\094-40-code-review.md`.
- **The scan:** `_reviews\094-40-prepush-scan.md`.
- **The card:** `_reviews\094-40-office-card-europa-capture.md`.

## Verdict
**GREEN on Code's own numbers:**
- **FX2:** the injector is seeded once per session. The run made 69 draws with no restart.
- **FX3:** auto LOD swaps now force a LOD different from the predicted drawn LOD, using stock `ComputeStaticMeshLOD` (citations checked against the 4.25 source). All 19 of 19 LOD events changed 14,455–14,475 of the 15,022 target pixels.
- **The EA_Core smoke passed on its first slot:**
  - fires: blinking 22, lighting 13, LOD 19, missing_object 15;
  - masks on all 731 affected frames, and none on the 1,621 clean frames;
  - `annotation.json` at schema 2.1, with the sets agreeing on 69 events and the far plane at 1,000,000;
  - census, custom depth, player view and the per-frame pre-UI proof all pass;
  - 2,352 frames written, 0 dropped;
  - no crash and no upload.
- **The builds:** Editor and Game both passed on the first attempt.
- **FV6:**
  - 26 commits were rewritten, and each tree differs only in the redacted receipts JSON;
  - the backup ref is local only;
  - Source, Shaders and the descriptor equal the built commit.
- **The scan is clean** across `abb240b..d1cdc7b`.
- **The push:** a new branch, with `ls-remote` equal to `d1cdc7b`. No tags, no backup ref, no force.
- **Safety:** none. Paris and `m51` are unchanged, and nothing was deleted.

## Card check (chat)
- **Codenames:** "Europa" only. The card forbids typing any project, level, actor, class, folder or studio name, and asks for `standard`/`custom` instead.
- **The HARD RULE:** typed reads only (R1–R23). No files, logs, screenshots or clipboard leave the box.
- **The pinned commit:** `d1cdc7b`, equal to the pushed commit.
- **The selection floor:** 6 % (FX4); the 0.5 % override is gone from the office launch.
- **FX5's lighting note** is present.
- **Upload and analytics prompts** are off in the capture launch, and a crash means "Close Without Sending".
- **My amendment (both cards):** R3 now types only the numbers of `version=`. *Why:* a fork's version string can carry a branch name, so any trailing text is typed as `+text`.

## Rulings

### FY1. Accepted: the 4.25 capture port, for the office test on Europa's fork
Phase C is complete at home: C1–C4 on fixtures, C5's census, view and restore on the street map, and this smoke.

### FY2. The office capture card is the one to run
`_reviews\094-40-office-card-europa-capture.md` supersedes the D-lite card for the next office visit. Its §4 includes the D-lite eyeball smoke.

### FY3. A zero fire count at the office is a finding, not a failure
*Why:* the shipped 6 % floor hasn't run on a real level at home. Chat will rule on the reads.

### FY4. FUTURE, in an order the owner sets
- the dataset-quality items D1–D5 (both engines; the 5.1 data may carry D1–D3);
- Phase B (missing_texture and corrupted_texture on 4.25);
- the deferred A5, A6 GC, A8 and controls;
- keys for the two new pool slots;
- G498's 5.1 follow-up;
- a dependency-checked decision on `D:\IA_Europa\ReGate51` (33.6 GiB) and `Baseline51` (18.9 GiB).

## For the owner
- **🟢 The office capture test is ready.** Code pushed everything. At the office, follow the card.
- **Bring home only the typed read-back lines,** into `_bates_reads`.
