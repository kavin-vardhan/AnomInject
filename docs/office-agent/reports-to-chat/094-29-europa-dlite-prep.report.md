# REPORT for 094-29-europa-dlite-prep.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 094-29-europa-dlite-prep.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-10-31 08:16:26 | finished: 2026-10-31 08:37:12 | duration: 21 min | exit code: 0
context at last turn: 513723 tokens (51% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\094-29-europa-dlite-prep.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN

The branch is safe to push, and the office card is written. Nothing was launched, built, committed or pushed. `m51` is the same at the end as at the start (HEAD `53bf725`, index `526aba7f`, 0 tracked changes, 15 untracked), and `feat/europa-port` is still `abb240b`.

**For the owner:**
1. Push is safe: no secrets, no large or binary files, and no office titles or names in what goes up.
2. The plugin code hasn't changed since the last 4.25 build at home, so the office build is just the first compile on Europa's fork.
3. At the office: clone the branch into Europa's `Plugins` folder, build the editor target, start the game with `-IAIBench`, apply and revert the four anomalies by eye, and type back 12 short lines.

**Scan verdict: `PUSH-READY: yes`**
- **What a push sends:** 92 commits from `d7eee97` to `abb240b`, as a new branch only. It is 670 files' worth of content, 55.2 MB uncompressed, largest file 790 KB. There is no remote copy of the branch yet.
- **Secrets: none.** I checked 116 name=value lines by hand. They are rule-citation text, fake test values, ordinary code, and one placeholder that is already public. None is a credential. The real control-server token does not appear anywhere in the push.
- **Not blocking, your call:** your Windows profile path appears in 19 lines across 9 new `tools/europa/` scripts. It isn't secret, and the same first name is already public on origin in `CLAUDE.md`, `onboarding.md` and every commit's author field. You can push as is or swap it out in a later commit.
- **E-mail addresses, machine name, GitHub account, login IDs: none.** All 92 commits carry the same author identity as the existing origin history.
- **Office names: clean.** The project's own name-check list matches only `AnomalyCapture.Build.cs`, which is the one file permanently allowed to carry those names, on lines this branch didn't add. The owner eyeball list in the scan report is all engine, tool or plain words. One limit: a title that is also an everyday English word would slip past that list.
- **Files: clean.** Nothing over 1 MB, no binaries or Unreal assets, nothing from `Receipts`, `Saved`, `Intermediate` or `Binaries`.
- **Plugin C++ since the last 4.25 build: unchanged.** That build ran in 094-21 at `40a8eaf`. The same code built with 0 errors and 0 warnings on stock 4.25.

**Push command for the owner:**
```
git -C "D:\IA_Europa\StylizedParisStreet\Plugins\AnomalyInjector" log -1 --format=%h
git -C "D:\IA_Europa\StylizedParisStreet\Plugins\AnomalyInjector" push origin feat/europa-port
git -C "D:\IA_Europa\StylizedParisStreet\Plugins\AnomalyInjector" ls-remote origin feat/europa-port
```
The first line must print `abb240b`, and the last line must start with `abb240b`.

**Card:** `D:\IntrusiveAnomalies\_reviews\094-29-office-card-europa-dlite.md`. Its read-back block:
```
R1  plugin tip abb240b: yes/no   (or: existing copy)
R2  build exit code: __
R3  profile: engine=__ version=__
R4  first Anomaly error from Source\ : ____ (or: none)
R5  game window up, no crash: yes/no   mode: game/PIE
R6  anomaly types: __
R7  blinking: applied y/n, visible y/n, reverted y/n
R8  lighting_mismatch: lights __, visible y/n, reverted y/n
R9  lod_corruption: LOD __ of __, visible y/n, reverted y/n
R10 missing_object: applied y/n, visible y/n, reverted y/n
R11 RevertAll reverted: __
R12 crash: none / at step __
```

**How the card is grounded at home:**
- **Launch:** the same command line the passing home sessions used (094-27e): the 4.25 editor binary in game mode with `-IAIBench`, minus the home-only probe flags.
- **Anomaly arguments:** the ones the home sessions used, from `tools/europa/runtime_scripts.py`. `IAI.Apply` reaches the same apply call the home probe used.
- **Exact-name targeting (`=Name`) and the on-screen object list:** both ran on 4.25 at home, in the 094-27c Admission session.
- **One adaptation:** for lod_corruption the card passes LOD `64` instead of the home `1`. The code clamps it to the mesh's coarsest LOD, so the change is visible on an ordinary level. The home fixture was built so that `1` looked different, which an ordinary level won't be.
- **missing_object needs `-IAIBench` on 4.25.** Without it, only three anomalies exist.

**Predicted, not proven, for Europa's fork:**
- The fork reports engine 4.25. Anything else stops the build with a clear message.
- The fork's compiler and headers accept the plugin. Home used stock 4.25 with VS2017 14.16 and C++17.
- The fork includes the `WebSocketNetworking` plugin, which the plugin asks for.
- `-NoEngineChanges` passes, meaning the fork's engine is already built for Development Editor.
- The plugin loads from Europa's `Plugins` folder without editing Europa's `.uproject`. The home project listed it explicitly.
- The editor binary in game mode reaches a playable level on Europa's project, and the console key works. The fallback (Play inside the editor) was never used for anomalies at home.
- lighting_mismatch shows only on a light that isn't Static. The home light was Movable.
- lod_corruption shows only on a mesh with at least 2 LODs.
- Blinking times per frame as at home, assuming the fork's world tick is unmodified.
api-equivalent cost (not billed on Max): $12.23 | turns: 88 | running total across all briefs: $5,298.51
