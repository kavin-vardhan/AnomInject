# 094-47 final report — Europa 4.25 and AnomDash

**DRAFT: final GO3, protection, publication and project-copy results remain pending.** The packaged Development control server and dashboard have passed the D3D11 capture comparison under the explicit GR1 ruling. LOD corruption remains in the dashboard. Shipping remains excluded.

## Delivery

- Plugin `feat/europa-port`: `bd767ca8ac7cd1ca18abe34d9cc66764fee100a6` — PUBLICATION PENDING.
- Dashboard `feat/europa-425`: `b8ffaca1d88a2c89d1ed69adaac85501cc2fc0b5` — PUBLICATION PENDING.
- Development package: `E:\IA_BuildCache\09447u-package\WindowsNoEditor`.
- Desktop: `D:\IA_Europa\Receipts\09447t\dashboard-build-r4\AnomalyDashboard.exe`.
- Office instructions: `_reviews/094-47-office-card-europa-dashboard.md` — FINAL UPDATE AND CLAUDE PROJECT COPY PENDING.

The card covers rebuilding and packaging with Europa's office fork, the loopback listener, local tokens, desktop discovery, the exact four-default/PNG/native/seed4242/2352-frame preset and numbers-only readback. Home qualification does not substitute for the office fork's results.

## Evidence and rulings

Both full D3D11 captures requested, wrote and paired **2352 frames with zero drops**. Labels, actor-bound mask pixels, event schedule, routes, seed, counts and restoration agree. Each capture passed **443 strict texture measurements at IoU1.000 with zero spill**. The existing desktop lifecycle evidence covers discovery, authentication, the anomaly catalog, start/stop, reconnect, a second capture and normal close on D3D11 and D3D12. Intentional early stop reports PARTIAL and restores state.

GR1 compared the existing console A and console B captures; no new baseline was launched:

| Difference | Console/console | Console/dashboard |
|---|---:|---:|
| Census cycles: differing rows | 1339 | 1639 |
| Maximum absolute cycle delta | 8 | 26 |
| Mean absolute cycle delta | 2.72 | 11.00 |
| Private-verification rows (all deltas1) | 77 | 76 |
| In-flight rows (all deltas1) | 163 | 172 |

Chat explicitly classifies the cycles as session nondeterminism. The likely mechanism is a cumulative counter driven by readback timing, with additional game-thread work serving dashboard snapshots; this is not an isolated causal measurement. The six schema entries describe one permitted JSON number representation (`t=320` written as an integer), including its sorted-list shifts. No non-clock schema change was found. **GR1 PASS.** Future work: serialize time fields as floats. The original strict-difference reports remain preserved.

**GR2:** the D3D12 full comparison pair is dropped; current-package lifecycle and prior console capture evidence remain valid. **GO3:** PENDING final default-only1279×719 key/type compatibility check and card reads. **GR4:** PENDING final FJ1, one canonical full exit P9 and exact owned-process closure; content scan PASS, fresh remote publication checks pending.

**GR3:** `camera_clipping` is opt-in and qualified at1280×720 on D3D11/D3D12. Held near100 at1279×719 produced distant textured-floor speckle in rows373–375, up to117 levels; plain floor stayed within2, and neither1280×720 leg showed it. The one-pixel miss at(375,239) is accepted only inside held camera sessions. Defaults retain IoU1.000 and zero spill. Prior archived-binary compression is accepted with unchanged hashes/mtimes; future compression is limited to receipts/evidence.

## Evidence locations and remaining scope

GR1 baseline, direct content review and final disposition: `_relay/runs/2026-10-16-094-01-europa-phase-a-plan/root-receipts-094-47w/`. Final gate and publication receipts: the adjacent `root-receipts-094-47x/`; worker evidence: `D:\IA_Europa\Receipts\09447x`.

No new5.1 runtime qualification, release-branch merge or office-fork qualification is claimed. The four-default dashboard preset does not enable camera clipping; live preview, arbitrary frame counts, JPEG/resizing and targeted captures remain outside4.25 support.

**Chat review NOT REQUIRED** for the authorized GR4 publication and handover. The next mandatory Chat checkpoint is a new correctness failure or a proposed change to capture/label meaning, tolerance or release scope.
