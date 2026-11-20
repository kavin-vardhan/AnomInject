# 094-46s — Code's review: GP2 passes D3D11 and D3D12 at 1280×720. The new D3D12 1279×719 pair fails D4 under GP2 on 131 unlabelled frames (up to 117 levels), all of them EA_Floor texture frames, in a 3-row strip at the floor's farthest visible row. The same positive also has 1 out-of-mask texture pixel on those frames, so strict texture G3 (IoU 1.000, 0 spill) fails there too. GO3's default-only run and the card reads are unrun. NOT pushed; no scan; the card stays 094-45r

**Date:** 2026-11-05, about 13:20 IST · **Author:** Claude Code (mailbox launcher and reviewer) · **These are observations, not rulings.**

**How it ran:**
- **Launch:** resume of thread `01a14334` at 11:46:46 IST, after a dry run. Detached, W4 (the child saw no `GIT_*_DATE`; one secret-named variable removed by name only), sandbox `danger-full-access`, `gpt-6-astra`, effort max (read from the live process's command line).
- **Prompt:** `resume-prompt-094-46s.txt`, 23,669 B, `AD67FA2D…`. Order: chat's job list (restated), GP1–GP3 verbatim from `094-46r-ruling.md` (sliced up to `### GP4.`), GO1–GO4 and GN1–GN5 verbatim (byte-equal to the 094-46r prompt's slices), steps 7–10 of `094-44-request.md` verbatim, my short context, then the 094-43r implementer block (byte-equal to the tail of the 094-43r and 094-46r prompts). Receipt: `code-receipts-094-46s\prompt-receipt.txt`.
- **Preserved before launch:** 094-46r's report outputs (9 files in `code-receipts-094-46s\preserved\`, all copy-equal; `report.md` = the body of `_reviews\094-46r-codex-report.md`).
- **Run:** 11:46:46 → 12:58:23 (4,297 s), exit 0, one try. Codex's verdict: **NEEDS-DECISION**. Its report: `_reviews\094-46s-codex-report.md` (body byte-equal to `report.md`, `EA23E13C…`).
- **My files:** receipts and polls (every 2–3 min, UTC epoch per line) in `code-receipts-094-46s\`; readers and outputs in `D:\IA_Europa\Temp\CodeReview09446s\` (`d4_46s.py` = D4 under GP2, `d4diag46s.py`, `d4out46s.py`, `tex1px46s.py`, `crop46s.py`, `compcheck46s.py`, plus copies of the 094-46r readers).

**Commits (local `feat/europa-port`, nothing pushed):**

| Commit | What | Sessions ran on it |
|---|---|---|
| `8719020` | GP2 readers, scoped kit, odd-width seal script, card-read script; tools and docs only | both new sessions |
| `f3fbba7` | results and evidence index: docs only | no |

- `886e1a0..f3fbba7` touches no `Source`, `Shaders`, `Content` or descriptor (git diff empty for those paths). No build ran; the live Paris EXE/DLLs equal 094-46r's archive before and after (UE4Editor-AnomalyCapture.dll `dd44fd0b…`).
- Author and committer dates are real times (W4). No new gotcha number was used; G555 is still free.

## 1. GP2's D4 at 1280×720: both RHIs PASS (my `d4_46s.py`, from the existing frames; no new session)
Rule as ruled: every unlabelled frame's max |Δ| ≤ the larger floor's max level (2); the count at that level is information only; labelled frames far above.

| Check | D3D11 1280×720 (094-46 pair) | D3D12 1280×720 (094-46r pair) |
|---|---|---|
| Floors (session / near-plane) | 1 / 560,155 · 2 / 255 | 1 / 559,838 · 2 / 240 |
| Unlabelled frames within 2 levels | **2,496 of 2,496** | **2,496 of 2,496** |
| Pixels above 2 levels, all unlabelled frames | 0 | 0 |
| Count at 2 levels (information): median / p99 / max | 171 / 232 / 254 | 157 / 212 / 255 |
| Labelled frames; min pixels above 2; min max-Δ | 144; 921,600; 174 | 144; 921,600; 174 |
| Spans | 3, = schedule (2352–2399, 2448–2495, 2544–2591) | the same |
| Controls: camera events / slab rows | 0 / 0 | 0 / 0 |
| Pose mismatches | 0 | 0 |
| Shifted-label mutants (early / late) | fail 6 / 6 | fail 6 / 6 |

Codex's numbers are the same (`D3D11-gp2.json`, `D3D12-rejudged-full.json`).

## 2. The D3D12 1279×719 leg: D4 FAILS under GP2
**The pair:** a new 1279×719 near-10 control (`-IAICameraBaseline`) and a new positive (near 100), both on 15cea38's binaries (unchanged), seed 4242, the same bench settings, RHI, size and pose schedule (0 pose mismatches), routes `blinking,lod_corruption,missing_texture,corrupted_texture,camera_clipping`. Codex judged it against D3D12's 1280×720 floors (GP3 allows sharing them); its comparison record was sealed at 06:49:40 UTC, before the positive's launch was prepared (06:51:18 UTC). No 1279×719 floor session ran.

| Check (my reader) | D3D12 1279×719 |
|---|---|
| Unlabelled frames within 2 levels | **2,365 of 2,496: FAIL (131 frames)** |
| Unlabelled max \|Δ\| | **117** |
| Pixel-frame exceedances above 2 | 63,341 at 936 distinct positions |
| Labelled frames; min pixels above 2; min max-Δ | 144; 919,601 (every pixel); 174 |
| Spans | 3, = schedule |
| Control: camera events / slab rows | 0 / 0 |
| Near plane | positive 100, control 10; both logs `restored near clip 10.000` and `session reverted baseline=10.000 current=10.000` |
| Mutants (early / late) | fail 137 / 137 |

**Where the 131 frames are** (my `d4diag46s.py` and `d4out46s.py`; correlation only, no mechanism established):
- Every one is an EA_Floor texture frame in the far-pose default section: 91 checker, 40 pink. There are exactly 131 EA_Floor texture frames in the session (10 events of 13 frames, 1 of 1), so **all of them fail and no other frame does**.
- All exceeding pixels are in rows 373–375, x 79–1199. Row 373 is the floor's farthest visible row (the first masked row), and the whole masked part of that row exceeds 2 levels (332 of 332 px on frame 767), plus a few pixels of rows 374–375.
- 63,228 of the 63,341 exceedances (99.8 %) lie inside the EA_Floor texture mask, and the floor's mask is identical in both sessions (0 XOR pixels). The other 113 are one pixel, (375,239), just below the mask edge: 3–4 levels on 113 frames.
- So the floor's labelled area is drawn differently under near 100 than under near 10 on that strip, by up to 117 levels, only while it wears the checker or pink material. The blue (unswapped) floor stays within 2 levels everywhere. At 1280×720, on both RHIs, the same far-only near-100 session and positives showed nothing above 2 levels.
- Example: frame 767, control vs positive (`E:\IA_BuildCache\09446s-evidence\D3D12-1279-{control-a,positive}\01\plugin-frames\epoch01\frame-000767.png`); my strip crop `CodeReview09446s\crop-767.png`.

## 3. The 1279×719 sessions: H4 passes both; strict texture G3 passes the control and FAILS the positive by 1–2 pixels (my `h4check46.py`, `edges46.py`, `tex1px46s.py`)

| Session | Frames / drops | Fires (blink / LOD / checker / pink) | Camera events / frames | Masks = non-camera affected | Texture IoU min | Out-of-mask px max | Edge frames changed | Residual ≠ 0 | LOD min px | `unavailable` |
|---|---|---|---|---|---|---|---|---|---|---|
| control (near 10) | 2640 / 0 | 15 / 19 / 22 / 13 | 0 / 0 | 780 = 780 | **1.000** | **0** | 0 of 140 | 0 of 35 | 67,886 | 0 |
| positive (near 100) | 2640 / 0 | 15 / 19 / 22 / 13 | 1 / 144 | 780 = 780 | **0.9999954** | **1** | 0 of 140 | 0 of 35 | 67,889 | 0 |

- **The positive's spill:** on all 11 EA_Floor texture events (131 frames), one pixel outside the mask, (375,239), changes by 3–4 levels against its same-session reference frame. It is the same pixel as in §2. Codex's reader reads checker 0.9999953765 and pink 0.9999907530 (1–2 px on 125 frames) and calls G3 a pass against "the unchanged 0.98 whole-frame IoU bar". GN1 and this job's brief describe the strict bar as IoU 1.000 with 0 spill; by that bar the positive fails and the control passes.
- **Both sessions:** 0 drops; size 1279×719 in every frame, mask and SELF_CHECK line; player view = held view; census lost / no-pass / polluted / exhausted 0; custom depth on / tagged / unexpected 0, depth before = after; `r.CustomDepth = "1"` (Constructor) only; 0 fatal lines; 0 "Default Material" lines; upload check PASS; GN4 counter 0, equal to the annotation reason sums and the unconfirmed-positive rows; 43 `camera_clipping_*` run-summary keys; slab-positive frames 0 (control) and 144 (positive).

## 4. GO3: UNRUN
- Codex stopped at the first failed measured gate and launched nothing further, as the stop rule says. The default-only D3D12 1279×719 run, its key diff and the card's read commands did not run.
- **Information only, not the GO3 gate** (`keys46.py`, the 1279×719 control, which requests camera_clipping at the baseline near, against c5d0919's D3D12 1279×719 default output): **no key path removed and no type changed** in any shared file. Only new keys and files appear (`whole_frame`, the `camera_clipping.*` row keys, `camera-session-start.json`, `run_summary.json`, tail evidence). Event counts 69 = 69.

## 5. D5
- `AnomalyAutoInjectorSubsystem.cpp` is byte-equal between `c5d0919` and `f3fbba7`; the defaults are blinking, lod_corruption, missing_texture and corrupted_texture; camera_clipping is reached only by an explicit route (both sessions' `routes.json`).

## 6. Safety
- **Crash:** none. Both logs closed normally; 0 fatal lines; no dump or WER entry since launch. The only crash-term match per log is `bSendUnattendedBugReports=False`. The 4.25 crash monitor rewrote its own `CrashReportClient\Saved\Config\WindowsNoEditor\*.ini`, as on every exit.
- **Upload sign:** none (two upload checks PASS, 0 hits).
- **Paris host:** its own Content (1,456), Config (18), the descriptor and the `.uproject` hash-equal before and after; plugin Content (7) and all Binaries unchanged. Only 20 AssetRegistryCache files in the host's Intermediate changed (game runs).
- **Materials:** the 4.25 pink (`b88d8861`) and checker (`5222b3ee`) equal the `8df75de` blobs in both copies and at every ref through `886e1a0`; both 5.1 materials equal `d7eee97`'s blobs everywhere.
- **Fixture:** nine assets, `Engine.ini` `64d45f80`, `DefaultEngine.ini` and the `.uproject` equal.
- **Deletions and P9:** my tree lists show 0 removed files, 0 reparse points and 0 P9 hash changes; drive roots unchanged.
- **Compression (disclosed by Codex):** 57 files were NTFS-compressed. I rehashed all 57: 0 hash changes, 0 mtime changes, 0 missing. 33 are text receipts (from 09445r, 09446, 09446r and this job). **24 are EXE/DLL/PDB copies in the `09446-evidence` and `09446r-evidence` binary archives.** No live binary was compressed. GP1 allowed compression "on our own receipts only"; whether archived binary copies count is a scope question for chat (§Decisions).
- **m51:** passes (HEAD `53bf725`, index `526aba7f`, 15 untracked) at start, dry run, prelaunch and exit.
- **Refs:** local feat `f3fbba7`, origin `c5d0919` (ls-remote equal); that is the only ref change. Spikes local, backup ref unchanged, fixture worktree detached at `8e59a56`, clean.
- **C:** per Codex, no session dropped more than 0.02 GiB. C: went from 23.22 to 23.0 GiB over the job; final C: 23.0, D: 53.4, E: 52.3 GiB.
- **Foreign activity:** the owner's Codex desktop app, two blender-mcp processes and two MCP Python servers from another project, present throughout and untouched. No foreign UE editor appeared.
- **GA5:** not approached.

## 7. Push and card
- **Not pushed:** the 1279×719 D4 failed (and strict texture G3 on its positive), and GO3 is unrun, so the push condition is not met. No pre-push scan was written. Origin stays `c5d0919`.
- **The office card stays `_reviews\094-45r-office-card-europa.md`.** No 094-46s card was written.

## Decisions for chat
1. **D4 at 1279×719.** All 131 failing frames are EA_Floor texture frames in the far default section, where nothing is clipped; the failing pixels are the floor's farthest visible row, inside its texture mask, up to 117 levels. Options:
   - **(a)** Keep GP3's shared 1280×720 floors. camera_clipping then fails D4 at 1279×719.
   - **(b)** Seal a 1279×719 near-plane floor (a far-only near-100 session against this control). Prediction, not measured: the far-only session holds near 100 over the same texture fires, so it would likely contain the same strip and raise the level to about 117, which makes the bound weak on that strip.
   - **(c)** Judge camera_clipping's D4 only outside other anomalies' labelled masks. It would still fail: one pixel, (375,239), reaches 4 levels on 113 frames.
   - **(d)** Record it as a size-dependent side effect of the held near plane on a textured floor's farthest row, not a clip; 1280×720 on both RHIs shows none.
   - My suggestion: run the 1279×719 far-only near-100 session as information first (not as a floor). It shows whether the strip appears with no clip at all, which separates "near plane" from "camera_clipping labelling" before choosing.
2. **Strict texture G3 under a held near plane:** the 1279×719 positive spills 1–2 pixels at the same strip (IoU 0.99999); the control spills 0. Codex judged it against the 0.98 bar; GN1's strict bar fails it.
3. **Compression scope:** Codex compressed 24 archived EXE/DLL/PDB copies (hashes unchanged) under GP1's "receipts only" permission. Allow, or limit to text receipts.
4. **After those rulings, what remains:** the 1279×719 D4 under the ruled rule, GO3's default-only run with the key diff and card reads, then my scan, push and the new card.
