# 093-11 — Claude Code review: pairing-recipe control and the J2 continuation

**Date:** 2026-10-15 · **Author:** Claude Code (mailbox brief `093-11-j2-pairing-recipe-continue.md`), reviewer only.

**Method:** reading and recounting only. No build, cook, stage, editor or game launch, and no git write. Git objects were read with `GIT_OPTIONAL_LOCKS=0`. The pairing reader ran from my scratch copy (SHA-256 `54928b2e…`, byte-identical to the `_r53_host` CaptureBench copy every leg used). Every session I read was rehashed before and after and was unchanged. My own baselines were taken before launch. Scripts, baselines and outputs are in `_relay\runs\2026-10-14-093-06-j2-office-robust-f1\code-receipts-093-11\`.

**The Codex job:** resumed the same thread `<uuid>` (`gpt-6-astra` / `max` / danger-full-access), one try, 12:27:50 → 15:29:02 IST, **10,872 s**, exit 0, **VERDICT NEEDS-DECISION**. Report: `report.md` in the run folder (093-10's copy is preserved as `report-093-10.md`). Evidence: `E:\IA_BuildCache\_r9311`. It stopped on the frozen 50 % picture-size check, restored, archived UNQUALIFIED and pushed nothing.

**Counts: 27 OK · 0 GAP · 3 OPEN.** The three OPEN rows are work the stop left unrun.

| # | Item | Codex result | My check | Verdict |
|---|---|---|---|---|
| 1 | Launch and attempt numbering | Resumed | `GIT_AUTHOR_DATE`/`GIT_COMMITTER_DATE` are unset in my environment at process, user and machine scope; removed for the child anyway (`launch-receipt.txt`: child sees neither). Run folder held 093-10's `exit-code.txt` and no live process for the thread. The mapping is in `attempt-numbering.txt`: 093-10 (5th attempt) → `exit-code-attempt4.txt`; 093-11 (6th) → `exit-code.txt`. As predicted, `ledger.csv` now has two `…-resume4` rows (07:17:19 and 12:27:50) | OK |
| 2 | Pre-flight: candidate rehash | All match | My rehash before and after the job: both candidate banks (09310 archive and `_r939\candidate`), both EXEs, 10 product DLLs, 4 helpers: 16/16 match `eff797c` each time. Editor EXE `5fcbbf4f…`. No rebuild, so the 32 passes and PF-1 to PF-5 stand | OK |
| 3 | Pre-flight: restoration state | Matches | My pre-launch baselines are byte-identical to my 093-10 after-baselines: staging (5,764), both `Content` trees, host binaries, host config, refs, `ls-remote`, worktrees, `_r936`–`_r939` and the four earlier archives. Installed host binaries were the release state (`StackOBot.exe` `218BA4C7`), as restored | OK |
| 4 | Freeze content | Committed | `e4228be` `docs/predictions/093-11-pairing-recipe.md`: amended recipe = J2 minus exactly the three lighting overrides; Y2 table verbatim; reader pinned by SHA-256, 40 px, PREVIOUS = NEITHER = 0; C2 = failed recipe + `r.MotionBlurQuality 0`; coverage on the reader's own predicate and stride-2 grid; `speed_ratio` from `run_summary`; failed leg labelled "instrument precondition not met (probe unlit)". No branch spec carries a larger decidable floor (my `git grep` of the predictions and October journals, plus the `_r939` specs); the wrapper's floor is CURRENT > 0 | OK |
| 5 | Freeze before the first pairing leg | 12:37:49 | Commit 12:37:49 +05:30; C1's editor (PID 26072) launched 13:53:41.759; no pairing leg exists in `_r9311` before it | OK |
| 6 | Reader, tolerance, predicate unchanged | Unchanged | All 8 new `pairing-assessment.json` files carry `54928b2e…`; both on-disk reader copies equal my pre-launch hashes; `assess_pairing.py` and every assessor and marker reader in `_r9311` is byte-identical to `_r939` | OK |
| 7 | C1 (gate) | 52/0/0 | **My reader:** `stack_P_pair_native_100`: CURRENT 52 · PREVIOUS 0 · NEITHER 0, decidable 52 (floor 1), same-position 13, no-data 15, 80/80. Magenta bands coincide (clean 8,370 / positive 7,992). Plan commands carry `r.AntiAliasingMethod 0` and no lighting override | OK |
| 8 | C1 shifted-mask control | Rejects | **My reader on the copy:** 4/47/0 → rejected. 50 of 70 mask files differ from the original; pictures byte-identical; copy unchanged by my run | OK |
| 9 | C2 (reading) | 0/0/60 | **My reader:** 0/0/60, decidable 60, same-position 4, no-data 16. The command used is `r.MotionBlurQuality 0`, echoed live in the log (`r.MotionBlurQuality = "0"`, from the default 4) alongside the three lighting overrides. So motion blur off does not repair it | OK |
| 10 | Probe coverage | Reported | **Mine (stride-2 / full-res):** failed leg 5.107 % / 5.16 %; C1 68.47 % / 68.53 %; C2 5.146 % / 5.00 %; Lyra amended 68.70 %; Lyra historic 68.74 %. Stride-2 values equal Codex's to every digit shown. Mask area 40,938 px (Stack) | OK |
| 11 | `speed_ratio` | Reported | From `run_summary.json`: failed 1.36083, C1 1.33085, C2 1.49675, Lyra amended 1.43321, Lyra historic 1.76486; all equal Codex's | OK |
| 12 | Historic passing bank | Unavailable | `_bench_sessions_bank` (→ `E:\IA_BuildCache\_bench_sessions_bank`) is empty (0 entries, folder dated 03-10); my name search of `E:\IA_BuildCache` and `D:\IA_BankOverflow` found no retained historic pairing session | OK |
| 13 | Decision on C1 | Row 1 | PREVIOUS = NEITHER = 0, floor met, control rejects ⇒ adopt the amended recipe. Matches the table | OK |
| 14 | Lyra NEITHER at the adopted recipe | Budget rule | **My reader:** `lyra_P_pair_native_100` 49/0/9 (decidable 58). Y2 counts this against the same budget, so attempt 2 was correct. No PREVIOUS anywhere | OK |
| 15 | Attempt 2 (historic) | Pass → adopt | **My reader:** `lyra_P_pair_historic_native_100` 50/0/0 (decidable 50); its shifted copy 6/43/0 → rejected. Plan commands: no AA or lighting override; AE off and Marker 1 kept, as frozen. Re-running the four StackOBot pairings at historic follows "a pass means adopt it for every pairing leg" | OK |
| 16 | Stack historic native 100 % | 55/0/0 | **My reader:** 55/0/0 (decidable 55). Assessment: shifted copy 5/50/0, size PASS | OK |
| 17 | The stop: Stack historic native 50 % | Size FAIL → stop | **My reader:** pairing 56/0/0 (decidable 56); shifted copy 6/52/0 → rejected. The picture is 1280×992 against the frozen 640×496 expectation. The size check is frozen and unchanged, so stopping was right: changing it is outside 093-11's authority. No PREVIOUS reading exists, and the size failure is not evidence of a pairing slip | OK |
| 18 | Stack amended 50 % and synthetic pairings | Pass | Assessments read: native 50 % 64/0/0, synthetic 100 % 55/0/0, synthetic 50 % 64/0/0, all 80/80; shifted copies 6/56/0, 5/50/0, 6/56/0; size PASS at 640×496 | OK |
| 19 | Lyra synthetic UV and proxy alignment | PASS | **My recount:** each 1,152 rows / 1,152 PNGs, unique indices 0–1151, 0 zero-byte, 4 events, `total_frames` 1152, dropped 0, pending 0, O/N 0/0. `synthetic-alignment.json` and `integrity.json` pass with 0 issues; both identity controls reject mask, label and barcode defects, originals unchanged | OK |
| 20 | Rest of the PIE matrix | NR | Not reached: Lyra pairing ×3, the exact J1 delayed and pre-F1 wrong-view engine controls, and the admission probes | OPEN (stop) |
| 21 | Camera switch | NR | Not reached. No per-switch drops or event-continuation readings exist | OPEN (stop) |
| 22 | Cook and staged gates | NR | Not run. The prepared cook adds `-nodebuginfo` (archived PDBs are absent); that is unexercised | OPEN (stop) |
| 23 | Can-fails really failed | Reject | Every completed positive pairing leg's shifted copy reads PREVIOUS ≥ 43 (my reader on three of them). Alignment identity controls reject. The C1 control's masks really are shifted (50/70 files differ) | OK |
| 24 | Harness fixes | ≤ 2 each | Pairing recipe: 2/2, now spent. Reinstall packaging metadata: 2/2 (attempt 1 failed its own byte check because UBT `WriteMetadata` mints a new BuildId; attempt 2 regenerated `.target` receipts, EXEs/DLLs unchanged; spent). Restoration ownership lookup: 2. String-audit lookup: 2. Gotcha-heading comparison: 2. The changed scripts (`bench.py` recipe filter, `run_staged.py`, `cook_candidate.py`, `archive_candidate.py`) are orchestration only. **Marker: no change** (all marker and assessment readers equal 093-10's) | OK |
| 25 | Docs (Y3/Y4) | Committed | In `e4228be`: the B19 correction in the 093-10 journal (also in report §7); counter scope in `client-readme.md` and architecture, with the follow-up; **G484** (counter scope) and **G485** (lit probe, extended in `3d09293` with the Lyra and size findings). Of 71 refs, only `fix/followup-capture` holds G484/G485. `tools\office_checkpoint_i.md` gains exactly "Do not resize the window during a checkpoint session."; no `tools\*.py` changed | OK |
| 26 | Closing commit | `3d09293` | Docs only (CLAUDE, architecture, gotchas, 093-11 journal), dated 15:09:57 +05:30. `Source` tree `2f639a77…` and `Shaders` tree `64e18bf3…` equal `eff797c`'s; `tools` differs only by the `.md` sentence | OK |
| 27 | Archive | UNQUALIFIED | `followup-capture-J2-pairing-09311-eff797c-57F83797-C6D74FED-UNQUALIFIED`, 2,245 files. Candidate files 16/16 match their manifests. All nine new pairing sessions are present, and the stopped session is byte-identical to `_r9311` | OK |
| 28 | Restoration | Restored | Against my own before-baselines: staging 5,764, 0 added/removed/changed; both `Content` trees, host binaries, host config, `_r936`–`_r939` (nothing written into the retained failed leg) and all four earlier archives unchanged; hosts on `23098fa`/`caa68c6`, clean. The Lyra support DLLs (boost, tbb, D3D12) were absent before and are absent after | OK |
| 29 | Push | None | `ls-remote` unchanged: `fix/followup-capture` `1bf7054`. Local ref `7d5e5a4 → e4228be → 3d09293`, fast-forward. `m51`, `master`, `release/2026-10-05` and tags unchanged. Not pushing was correct | OK |
| 30 | Processes and safety | Own only | None of the 114 job commands is a stop or kill. Owner Codex desktop, `codex-windows-sandbox-service` and `blender-mcp` stayed alive; the vanished processes are browser tabs and Windows services. No security setting changed. D: 187.4 GB, E: 106.2 GB free | OK |

**Not applicable:** the office-reader line from a new healthy J2 session. J2 did not qualify, so I didn't run it.

---

## Observations (not counted)

**A. The stopped 50 % leg did render internally at 50 %, and the evidence is already in the leg.** All 82 mask-pass `M23 PASS` lines in `stack_P_pair_historic_native_50\game.log` read `viewRect=1280x992 internalViewRect=(0,0)-(640,496) 640x496 unscaledViewRect=1280x992 overrideOutput=1`. The amended (AA-off) 50 % leg's 80 lines read `viewRect=640x496 internalViewRect 640x496`. So:
- with AA off, the grabbed picture is the 640×496 internal render;
- under the host's TSR it is upsampled to the 1280×992 output before the grab, the same internal/output split m46 measured historically.

The frozen size check reads the final picture, so it encodes AA-off behaviour. Lyra also runs TSR by default (its `DefaultEngine.ini` sets `r.AntiAliasingMethod=4`, echoed in its leg log). So I expect every 50 % pairing leg (8 of 16) to fail the check the same way at the historic recipe. That is expected, not measured: no Lyra 50 % leg ran. Report §11 says a revised proof "needs independent internal-view-rect evidence"; the product log already provides one per frame. That's a fact for chat; I'm not proposing anything.

**B. Each recipe now fails a different frozen element.**
- **Amended (AA off):** the size check passes, but Lyra native 100 % reads NEITHER 9.
- **Historic (TSR):** Lyra and StackOBot 100 % pair cleanly, but the 50 % size check cannot pass (measured on StackOBot; expected on Lyra).

No leg at either recipe read PREVIOUS.

**C. Lyra's NEITHER frames: a reading, cause not established** (`lyra-neither-diagnostic.txt`).
- All 9 are at the probe's left position: picture centroid 291–305 against mask centroid 347.6, so 43–56 px. The previous position is ~650 px to the right, so the offset is not toward PREVIOUS.
- On frames 50/52, 9,426 bright-magenta pixels lie **outside** the mask (centroid x≈210, spanning x 0–513, y 398–670), to the left of and below the probe. That looks like spill.
- At historic, the outside pixels drop to 4,746 and spread wider (x 0–852), so the centroid lands about 29 px off. That passes against the 40 px tolerance, with a thin margin.
- I did not predict attempt 2's pass from this, so it stays a reading.

**D. Throughput.** Every passing PIE pairing leg ran at `speed_ratio` 1.33–1.76 with PREVIOUS 0. So m51's association between loaded throughput and PREVIOUS failures did not show up here. PIE only, small n; no mechanism claimed.

**E. A leftover trace daemon.** `UnrealTraceServer.exe daemon`, PID 36100, was created at 13:53:42, one second after C1's editor launched. Its parent, PID 2708, appears in no Codex launch receipt, no idle record and neither of my snapshots. It is not attributable by receipt, so it was left alone, as with 093-10's PID 36544.

**F. A second reboot.** The PC booted again at **11:34:11** today, after the 093-10 job had ended at 09:59:45, so it hit no job. Cause not established.

Nothing was fixed. Chat rules.
