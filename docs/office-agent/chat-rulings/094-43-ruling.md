# 094-43 — Chat ruling: stopped at entry on a stale P9 ref list (chat's own GC1 wording exposed it). P9 admits the four recorded refs by name and SHA; 094-43r re-runs the same job. Phase B decisions D1–D5 are ruled now

**Date:** 2026-11-04, about 02:40 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Mailbox report 094-43:** NEEDS-DECISION. It ran 01:51–02:22 IST.
- `_reviews\094-43-code-review.md`, `_reviews\094-43-codex-report.md` and `_reviews\094-43-phaseB-plan.md`.

## Rulings

### GE1. P9's ref check admits recorded refs by name and SHA. The baseline is not regenerated
- **What failed:** `owner_p9.py:72` requires every ref except local feat to equal the 71 baseline refs. Four refs made by recorded jobs are extra:
  - `refs/backup/europa-pre-fv6-09440` = `08fe652` (094-40);
  - `refs/remotes/origin/feat/europa-port` = `d1cdc7b` (094-40's push);
  - `spike/europa-09442c-mesh-shader` = `740220c`;
  - `spike/europa-09443a-depthonly-byname` = `c1193bd`.
- **The fix, made by Codex at Stage A entry and committed with W4 on `feat/europa-port`, with a self-test:**
  - The 71 baseline refs must still be exact; any missing or moved baseline ref fails.
  - Extra refs are admitted only from a short named list in the kit, each with its rule:
    - the backup ref at exactly `08fe652`;
    - `origin/feat/europa-port` at a commit that is an ancestor of, or equal to, local feat;
    - `spike/europa-09442c-*` at exactly `740220c`;
    - `spike/europa-09443a-*` at `c1193bd` or a descendant of it, since Stage A works on that branch.
  - Any other extra ref fails.
  - **The self-test** must pass the current state and fail each of: an unknown extra ref, a moved backup ref, a moved 42c spike, and a moved baseline ref.
- **Then the full `main()` must pass,** as GC1 asked. Code's probe already passed every other step (382,970 files with 0 differences, and 100.7 GB rehashed equal).
- *Why not regenerate the baseline:* P9 protects the baseline. Re-baselining every time a job adds a ref would wash out that protection. Naming the refs with rules keeps it, and costs one small kit change.

### GE2. 094-43r re-runs 094-43 unchanged, plus GE1
- **Stages A, B and C stand exactly as in `_reviews\094-43-request.md`.** The same thread resumes.
- Code's polls stay at 2–3 minutes while anything may launch.

### GE3. Phase B decisions, ruled now so 094-44 starts the moment 094-43r lands
- **D1 — the source is the release line, `d7eee97`.** That is what the client runs, and the port branch already carries it, compiled out on 4.25.
- **D2 — the materials:**
  - Two 4.25-authored materials go in the plugin's own Content, in a 4.25-only folder (`Content/Engine425/Materials/`), with 4.25-only finders.
  - They are authored by the py27 port of `create_anomaly_materials.py` (the same graphs and constants, with 4.25 usage flags) in the **outline fixture's editor, never the Paris host's.** They are committed on `feat/europa-port`, and the Paris plugin copy receives them through the branch.
  - Committed plugin-Content changes on our branch are allowed. **The Paris host's own Content stays a STOP.**
  - The 5.1 `.uasset`s are untouched.
- **D3 — the skeletal target goes in the outline fixture** (`/Engine/EngineMeshes/SkeletalCube`, or the engine's nearest skeletal mesh), authored with `-nowrite`. EA_Core stays as it is.
- **D4 — "camera_clipping fires" means:** in one held session on the bench near-wall pose schedule, at least 3 distinct slab-positive spans, each labelled exactly as the same-seed control differs (no early or late frame). A held session at the baseline near plane must label 0 frames.
- **D5 — the routes, matching 5.1 within FZ4's client set:**
  - corrupted_texture and missing_texture join the **default** pool once their gates pass, so the default becomes blinking, lod_corruption, missing_texture and corrupted_texture.
  - camera_clipping stays as in 5.1: never in the auto pool, held for a whole session only when its route is requested.

### GE4. The Phase B plan is accepted as 094-44's design
- **Its order:**
  1. materials;
  2. missing_texture, then corrupted_texture;
  3. their gates, then the Paris regression on D3D11 and D3D12;
  4. camera_clipping, then its gates.
- **Its gates G1–G5,** with these can-fails:
  - a material with one usage flag removed renders grey, and G2 must fail;
  - a mutant that labels one frame early;
  - camera_clipping at the baseline near plane labels 0 frames.
- **Its warnings, kept:**
  - G4 is the paired, same-seed "new magenta" count only, because the fixture's outline colour equals the pink;
  - the private pass takes the proxy fresh every frame, because a swap recreates it;
  - fix the fixture's exposure, not the anomaly, if the checker hardly shows.

## For the owner
- 🟡 **Last night's job stopped after 7 minutes, and that was my fault.** I asked for a full safety check that hadn't been updated since our recent branches were added, so it tripped on them. Nothing was built or changed. The fix is small, and the job re-runs now.
- 🟢 **The new anomalies plan is ready and approved.** We port exactly the version your client has: pink = corrupted_texture, checkered = missing_texture, plus camera_clipping, all as in 5.1. The two textures join the default set once they pass their tests. Camera clipping works as in 5.1: it's switched on for a whole session when you ask for it.
