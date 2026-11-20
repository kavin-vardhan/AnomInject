# REPORT for 090-01-final-source-round.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 090-01-final-source-round.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-30 09:09:29 | finished: 2026-09-30 09:53:25 | duration: 44 min | exit code: 0
context at last turn: 614163 tokens (61% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\090-01-final-source-round.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN

**For the owner:** For Thursday, pull **`feat/m53-uv-normal-corruption` at `da902d6`**. It is the full delivery: every m52 fix plus m53. If m53 gets pulled, the fallback is `fix/m52-label-timing` at `0347e8c`. README Step 1 (how to launch your build) is still a marked placeholder that only you can fill. The final delivery build (cook, package, launch check) hasn't been done yet.

**1. Fix branch (`_r84_host`)**
- **Off-frames default 16** (`c12b7e9`): `DefaultOffFramesTemporal` went from 8 to 16. The source confirms it only reaches stuck_low_mip, the one anomaly that uses the render-truth path. OnFrames 3 and HideFrames 1 are unchanged, everything is still 0 without temporal AA, and an explicit CVar still overrides it (capped at 64). The CVar help text and the run echo now say 16 and 3/16/1.
- **Tested both ways:** `m52_window_selftest` passes 284/0 (282 plus 2 new checks). A header copy with the old default 8 fails exactly the 2 new default checks, and the override check passes in both. camera passes 154/0.
- **LOD log wording:** the CURRENT-LOD and REFUSED lines now print `forced_lod_model=N (LOD index N-1 of M)`. Log text only; no behaviour change.
- **Builds:** editor 7 actions, game 4 actions, 0 warnings. Exe **`EB01156E`**.
- **Suites and audits, all passing:**
  - Python: `--label-rule` 24, pixel gate 98, change oracle 35, both unittest files, kit 32, lever audit selftest 11 plus source PASS, G354 9, bundle 9.
  - Binary lever audit against the branch: 33/33, echo present. String scan: new strings present, old ones absent.
- **Binary audit on the staged folder:** the host exe fails it only on `IAI.Bench.CameraSchedule`. That is expected (G398): the host exe is compiled with AnomalyBench, which delivery excludes. So I compiled a delivery-shaped game exe from the staged folder (a compile only, no cook): `D6B094DD`, which passes 32/32.
- **Archive:** `_binary_baselines\m52fix-EB01156E\` (6/6 files re-hashed at the destination).
- **Host restore trap:** after swapping the host back, UBT reported the exe "up to date" while it was still the staged build. I moved the exe and the game makefile aside and relinked: host exe is now `4B804F41`, same objects, audit 33/33. Recorded as G422.

**2. Merge into m53** (no-ff, `3ca5dcb`)
- **Conflicts:** only `CLAUDE.md` (status blocks) and `gotchas.md`. Both are append-only, so I kept both sides: fix-branch blocks above m53's, gotchas in number order G401–G422. Source and readme merged automatically.
- **Suites before (`620e3cb`):** all C++ suites pass (tc 251, draw 978, excl 243, src 47, m52 282, camera 154), 23/23 mutants fail as required, Python 18/18.
- **After the merge, one failure:** `exclusion_selftest` read 243 with 7 failures. Its F1 scenarios read the default but had the frame numbers of an 8-frame tail written in as literals. The product was doing what was ruled: m53 now waits up to 16 frames behind a stuck_low_mip tail.
- **Test fix (`d57246c`):** the expectations are now derived from the default. It passes 243/0 at 16 and also at 8 (a header copy with the old value), and all 11 exclusion mutants still fail. Recorded as G423.
- **This goes beyond "keep-both conflicts only".** It is a test-only change, and I'm flagging it for the 090-02 Codex review.
- **Suites after:** all C++ pass (m52 now 284), 23/23 mutants, Python 18/18.

**3. m53 build**
- Exe **`CE55D997`**. Editor 16 actions, game 7 actions, 0 warnings, built after clearing the plugin build products.
- Binary lever audit against the branch: 46/46. A delivery-shaped exe from the staged folder (`5E44E39F`) passes the staged-folder audit 45/45. String scan passes.
- **Archive:** `_binary_baselines\m53-09001-CE55D997\` (6/6 re-hashed). Host relinked to `1A99DDDA`, audit 46/46.

**4. Readme**
- **New §8.7a, on both branches:** for each anomaly, the content its label sync was proven on and which flags a strict training set should drop. It covers:
  - stuck_low_mip: the partial onset frames (2 per event, up to 5 on the first event) and the 16 fade-out frames (the fade actually ended 4–6 frames after the label);
  - camera_clipping: unconfirmed frames are over-labels by design;
  - lod_popping: proven on the purpose-built fixture only;
  - masks recycled correctly at runtime;
  - **Nanite objects get labels and boxes, but no mask**, with the exact fields a client will see.
- **§8.6a / §8.7:** 3/16/1, the fade measurement, and why 16. The camera_clipping note now states the bench result.
- **Stale sentence corrected:** the yield note said stuck_low_mip only refuses textures shared with *visible* objects. That was the old bench rule; the shipping rule is exactly one user in the whole loaded level. It now also says it fires rarely on shared-texture content and is off in auto-injection by default (G421).
- **§8.8, m53 only:**
  - the evidence basis;
  - the per-mode "barely changes" rate as "not measured on realistic content" (0 of 17 per mode on the fixture);
  - the 128 MiB default with the large-test-level example (nothing eligible at 128 MiB, two uv hosts at 256 MiB, normal never);
  - the stuck_low_mip tail as "up to 16";
  - two new §8.7a rows for uv and normal corruption.
- **Placeholder left: README Step 1 only**, on both branches. The bundle checker now stops on that one line (m53 used to stop on 2).

**5. Commits pushed**
- `fix/m52-label-timing`: `c12b7e9`, `0347e8c`.
- `feat/m53-uv-normal-corruption`: `3ca5dcb` (merge), `d57246c`, `da902d6`.
- No tag, no merge to master. `m51`, `master` and `ToCodex\` untouched. No game launch, cook or staging.

**6. NEEDS-DECISION:** none. For awareness:
- the m53 test fix above;
- the stuck_low_mip "HELD NONE" log line still says "shared with a visible component". I left it for the yield round, where it was already queued;
- E: drive is at about 48 GB free, below the ~50 GB level used in earlier rounds;
- the `dbghelp.dll` refusal didn't come up, because nothing was cooked.
api-equivalent cost (not billed on Max): $25.04 | turns: 218 | running total across all briefs: $3,872.61
