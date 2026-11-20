---
name: europa-port-pushed-d1cdc7b
description: "feat/europa-port origin = 2828f6a since 2026-11-06 (094-48d, Code-pushed after chat GR6b made free-play texture IoU information-only); final office card _reviews\\094-48b-office-card-europa-dashboard.md names 2828f6a; pushes must fast-forward from origin"
metadata:
  node_type: memory
  type: project
  originSessionId: <uuid>
  modified: 2026-11-06T12:07:01.481Z
---

**Push history of `feat/europa-port`:**
- **2026-11-02 (094-40):** Code first pushed it at `d1cdc7b` (a new branch, `ls-remote` equal). The owner's call (11-02) is that Code pushes once a pre-push scan is clean.
  - FV6 had rewritten 26 commits (47a6972..08fe652) to redact a drive-root listing.
  - The local-only backup ref `refs/backup/europa-pre-fv6-09440` → `08fe652` must never be pushed.
  - Office card: `_reviews\094-40-office-card-europa-capture.md`. Scan: `_reviews\094-40-prepush-scan.md`.
- **2026-11-04 (094-43r):** Code pushed `d1cdc7b..93613ae` as a fast-forward (20 commits: 094-42r, 42c, 43a and 43r).
  - Scan: `_reviews\094-43r-prepush-scan.md`. Office card: `_reviews\094-43-office-card-europa.md`.
  - The spike branches stay local; neither is an ancestor of feat.

- **2026-11-04 (094-44):** NOT pushed. Local feat is `9c87836`, one commit ahead of origin `93613ae`; it contains unbuilt native finders. The scan of `93613ae..9c87836` is clean (`_reviews\094-44-prepush-scan.md`; scripts in `code-receipts-094-44\prepush\`). The office card stays `_reviews\094-43-office-card-europa.md`.

- **2026-11-04 07:38 (094-44r):** Code pushed `93613ae..8df75de` (9c87836 materials+finders, ae82fe7 FE3b, 6ee4a0e, 8df75de docs) as a sha refspec fast-forward; `ls-remote` = 8df75de. Scan: `_reviews\094-44r-prepush-scan.md`, scripts in `code-receipts-094-44r\prepush-8df75de\` (the sibling `prepush\` has a mistyped tip and was never run). Local feat `2c99a6f` keeps `8e59a56` (texture port, which also puts the textures in the C4/C5 default routes before their gates passed) plus two docs commits: hold them until the textures qualify. The office card stays `_reviews\094-43-office-card-europa.md` at 93613ae.

- **2026-11-04 (094-44c):** NOT pushed; no scan was written. Local feat is `90d4275` (adds 7c3303e kit scope, 9788fd6 and 90d4275 docs). Everything past `8df75de` descends from `8e59a56` (the default-route change), so under GH4 nothing can go out until G4, G5, the Paris regression and camera_clipping pass. The card is unchanged.

**Why:** the branch history is public. Any rewrite would need a force-push, which the push rules forbid.

**How to apply:**
- Future Europa pushes are plain fast-forwards from the current origin tip (`8df75de`).
- Scan only `<origin>..<tip>` with the scan scripts (latest: `code-receipts-094-44r\make_prepush44r.py` regenerates 094-44's five scripts for a given tip; their BASE is still 93613ae, so change it to 8df75de next time).
- A non-fast-forward means stop and report.
- The owner's Windows profile name is a carried, non-blocking item: it is already public, and UBT warnings in build receipts repeat it.

Related: [[europa-425-editor-runtime-facts]], [[europa-port-51-base-is-release]], [[europa-custom-depth-leak-094-41]].

- **2026-11-04 (094-44d):** NOT pushed. Local feat is `4cc8b45` (fa207fb, 035ec20, 4cc8b45: tools and docs only); origin still `8df75de`. Paris texture G3 failed, so GH4 still blocks everything after `8e59a56`. No scan written; card unchanged (094-43).
- **2026-11-04 (094-45, 15:15-16:08):** NOT pushed. Local feat `2d93e67` (96d8f01 GL3, 93e7435 view lock + bench geometry probe, 422ac34 probe fix, docs). Stage 1 failed D3D11 G3, so no STAGE1-READY and no handshake file. My 094-45 scan generator `code-receipts-094-45\make_prepush45.py <base> <tip> <newdir>` takes both shas (never run yet). Card unchanged (094-43).
- **2026-11-04 18:22 (094-45r Stage 1):** Code pushed `8df75de..c5d0919` (26 commits incl. texture port 8e59a56, GM2 bench fixes) as a sha-refspec fast-forward; ls-remote = c5d0919. Scan `_reviews\094-45r-stage1-prepush-scan.md` (scripts `code-receipts-094-45r\prepush-stage1-c5d0919\`, generator `make_prepush45r.py <base> <tip> <dir>`). New office card `_reviews\094-45r-office-card-europa.md` (4 default routes). Stage 2 (camera_clipping) commits go on top of c5d0919. Stage 2 then failed D4 strictly; local feat = c706b7c (896b641, 24a120e, 0ec5bec, c706b7c), NOT pushed; next push must scan c5d0919..<tip>.
- **2026-11-05 (094-46):** NOT pushed. Local feat `eea2acd` (1e65b6f GN4 counter + IAICameraRegression combined schedule, afa3ea0 kit/readers, eea2acd docs) on origin `c5d0919`. D4 failed GN2's measured floor by one level on D3D11; D3D12 and GN3 unrun. No scan written; card stays 094-45r. Next push must scan `c5d0919..<tip>`.
- **2026-11-05 (094-46r):** NOT pushed. Local feat `886e1a0` (cd2d60c guarded `-IAICameraFarOnly` schedule, 15cea38 kit/readers, 886e1a0 docs) on origin `c5d0919`. D3D11 D4 passed GO2; D3D12 failed the near-plane count bound (255 vs 240 px at 2 levels on 6 frames). No scan; card stays 094-45r. Next push must scan `c5d0919..<tip>`; generator `code-receipts-094-46r\make_prepush46r.py <base> <tip> <newdir>` (never run).
- **2026-11-04 (094-44e):** NOT pushed. Local feat is `d33689f` (cfe8e06, ee56edc, cc8c2d5, d33689f: tools and docs only). The EA_Core pre-fire calibration failed, so no Paris leg ran; GH4 still blocks. `code-receipts-094-44e\make_prepush44e.py <full tip sha> <newdir>` regenerates the five scan scripts with BASE already 8df75de (not yet run).

- **2026-11-05 (094-47, control-server port):** NOT pushed (OD4 plugin hold). Local feat `a2250fd` (2af16c0 transport + shared C5 entry, dfd11ca, a231a57, a2250fd docs) on origin `c5d0919`; 4.25 Editor/Game compile PASS; stopped at GQ2: the 5.1 ReGate51 compile links but UBT prints 14 "Expected X.h to be first header included" errors, caused by `#include "Compat/AnomalyCaptureFeatures.h"` placed first in 14 AnomalyCapture .cpp files by a2cc30d (2026-10-31, already on origin since d1cdc7b); the last 5.1 port compile (094-21 at 40a8eaf) had 0. ReGate51 source is back at 40a8eaf but its AnomalyCapture/ControlServer DLLs are a231a57's (rebuild before use). E: is ~1.3 GiB above its 50 GiB floor; a C5 session costs ~1.6-1.8 GiB of E: evidence. Scan generator `code-receipts-094-47\make_prepush47.py <base> <tip> <dir>`; scan of c5d0919..a2250fd clean (`_reviews\094-47-prepush-scan.md`, PUSH HELD). Dashboard untouched (no feat/europa-425).
- **2026-11-05 (094-46s):** NOT pushed. Local feat `f3fbba7` (8719020 GP2 readers/kit/odd-width seal, f3fbba7 docs) on origin `c5d0919`; no Source change, no build. GP2 D4 passes both 1280 RHIs, but the new D3D12 1279x719 pair fails D4 (131 EA_Floor texture frames, rows 373-375, up to 117 levels) and GO3 is unrun. No scan; card stays 094-45r. Next push must scan `c5d0919..<tip>`. Toolkit: code-receipts-094-46s (clone_scripts46s.py, build_prompt46s.py slices GP1-GP3 up to `### GP4.`).
- **2026-11-05 (094-47r):** NOT pushed; no commit. Worker stopped at entry (full P9 STOP on 1,021 inherited 5.1 engine bookkeeping files from 094-47's ReGate51 build, no ledger receipt). Local feat still `a2250fd`, origin `c5d0919`; dashboard untouched. Scan = 094-47's (same range). Toolkit: code-receipts-094-47r (clone_scripts47r.py, make_launch47r.py, build_prompt47r.py, base_attest47r.py).
- **2026-11-06 (094-47 GR4):** Codex pushed `c5d0919..bd767ca` (094-47 dashboard/control-server delivery; dashboard `feat/europa-425` b8ffaca created). Origin = `bd767ca` from then on.
- **2026-11-06 (094-48 -> 094-48b):** 094-48 was interrupted (watcher died 15:34, Codex child died with it, change uncommitted). 094-48b committed `6b171b5` (free play) + `1667d05` (flat Library sessions) locally; PUSH HELD because S4/F1/F3/F5 fail strict texture IoU in free play (labels exact: 0 spill, clean edges; ~64 % of missing_texture mask px visibly unchanged). Scan of bd767ca..1667d05 clean (`_reviews\094-48b-prepush-scan.md`, generator `code-receipts-094-48b\make_prepush48b.py <base> <tip> <dir>`). Held card `_reviews\094-48b-office-card-europa-dashboard.md`. Dashboard unchanged.
- **2026-11-06 (094-48c):** chat's GR6 (whole-frame texture fires = info) did NOT clear the push: its premise was false. Every failing free-play texture measurement is non-whole-frame (masks 19,812/28,472/135,984 px, `whole_frame=false`); 921,600 is only the reader's clean-edge `target_pixels`. Rule 4 still blocks. Part C ran locally anyway: Codex `2828f6a` (extras behind dev-only `IAI.Capture.Evidence`, default 0; no blank masks; 5.1 `mask_map.json`), E1-E3 pass. Local feat `2828f6a`, origin still `bd767ca`. Scan of bd767ca..2828f6a clean (`code-receipts-094-48c\prepush-2828f6a`, generator `make_prepush48c.py`); review `_reviews\094-48c-code-review.md`; held card rewritten for 2828f6a only.
- **2026-11-06 (094-48d):** chat ruling GR6b (`_reviews\094-48d-chat-ruling-GR6b.md`): the texture label is an object mask; free-play blocking checks are 0 changed px outside mask, 0 on clean neighbour frames, 0 no-change labelled frames, polluted/tagged/drops 0, annotation PASS, masks inside boxes; IoU is information. All held on F1/F2x2/F3/F5/S4/S4-F2x2/E2 (+E1 files). Code re-ran the 48c scan (byte-identical, clean) and pushed `bd767ca..2828f6a` (fast-forward, ls-remote = 2828f6ab85406cacc6b5e4c06840a3a1b4cebbc0). Dashboard `b8ffaca` unchanged. Final card `_reviews\094-48b-office-card-europa-dashboard.md`. Next push scans `2828f6a..<tip>`; generator `code-receipts-094-48c\make_prepush48c.py <base> <tip> <dir>` (its RC output root is the 48c receipts dir).
