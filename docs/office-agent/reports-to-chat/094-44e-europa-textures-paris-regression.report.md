# REPORT for 094-44e-europa-textures-paris-regression.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 094-44e-europa-textures-paris-regression.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-11-04 11:04:56 | finished: 2026-11-04 11:58:06 | duration: 53 min | exit code: 0
context at last turn: 553592 tokens (55% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\094-44e-europa-textures-paris-regression.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

**For the owner:** Paris did not run on D3D11 or D3D12. The bench-only lighting fix for our EA_Core test level went in without touching your game, but no single brightness setting met all four colour checks, so the tests stopped before any Paris run. Nothing was pushed and the office card is unchanged.

**Why it stopped: no exposure qualifies**
- Codex added a small bench helper to the host's probe module. It only works with `-IAIBench`, on the `EA_Core` map, and not in Shipping builds. It turns bloom off (5→0), sets manual exposure (quality 2, method 3), turns physical-camera exposure off and tilts the camera to −12. It read `r.CustomDepth` as 1 and never set it. The setting is never saved: the session ran `-nowrite`, and the Paris host's Content, Config, descriptor and `.uproject` are hash-equal before and after. The launch reserve is back at 3 GiB (GJ4).
- Codex wrote the pass rules down before launching:
  - the untouched targets' brightness between 51 and 204 (0.2–0.8 of full);
  - no saturated pixel;
  - the checker's greys at least 40 apart;
  - the pink with R and B at least 200 and G at most 80.
- It then tested 65 exposures from 0 to −8 EV (195 previews, 1,560 frames, 0 drops, no anomalies fired).
- My own recheck from the raw frames agrees: **no exposure passes all the checks on all three targets.**

  | Target | Pink passes at | No saturated pixels at |
  |---|---|---|
  | the two cubes | −3.0 to −3.75 EV (e.g. 207/46/201 at −3.75) | −5.0 / −5.125 EV and darker |
  | EA_LOD | −5.25 to −6.0 EV | −2.125 EV and darker |

  - So **the cubes cannot pass at any exposure**: about 2,800 of their 16,815 pixels (17 %) are still saturated wherever their pink is bright enough.
  - EA_LOD's pink window does not overlap the cubes' anyway.
  - I haven't established why the cubes saturate on part of their surface. The numbers fit uneven lighting across their faces.
- My checker measure differs from Codex's (whole silhouette against a front-face patch). By mine EA_LOD alone passes at −5.25 to −6.0; by Codex's it never does. The outcome is the same.
- My pre-fire reader does fail when it should: on 094-44d's failed D3D11 run it reads targets at 0.95–0.975 of full, 49 % saturated, checker 15.9 apart, pale pink.

**Paris regression:** none of the three legs ran (D3D11 1280×720, D3D12 1280×720, D3D12 1279×719), as the stop rule requires. There are no new H4, texture G1–G3, G5 or `unavailable` numbers.

**Push and card:** nothing pushed. Every commit after `8df75de` sits on top of the default-route change, so none can go out without the texture gates (GH4).
- origin stays at `8df75de`, with blinking and lod_corruption as defaults.
- local `feat/europa-port` is at `d33689f`, four commits of tools and docs only, with the textures in the default set.
- No pre-push scan was written, and the office card stays `_reviews\094-43-office-card-europa.md`.

**Stop times (IST):**
- Builds started 11:19, 11:21 and 11:22.
- The last launch (calibration) started at 11:25:46 and its log closed at 11:39:48.
- No process of ours has been alive since 11:40. Codex's process audit at 11:42 found all five gone, and my readers finished by 11:55.
- Both the 13:10 and 13:35 limits were kept with a wide margin.

**Safety:**
- **No crashes or upload signs;** 0 fatal lines.
- **Binaries:** the 7 host Binaries files changed only while a build was running; plugin Binaries are unchanged.
- **Host Source** gained one file, the bench helper. It isn't on the protected list, and a byte-equal copy is committed in `tools/europa`.
- **Deletions:** none except three cache files Codex logged during the builds; 0 removed files in my tree lists.
- **Unchanged:** the fixture, the 5.1 `.uasset`s, `m51` (checked at every point) and every protected ref.
- **Calibration frames** are on `E:\IA_BuildCache\09444e-evidence\`, outside my usual tree checks.
- **Gotchas:** G539 added; the next free number is G540.

**Decisions for chat:**
1. **GJ3's checks can't all be met on EA_Core by exposure alone.**
   - **(a) Add the fixture's lighting recipe (GI1) as more bench-only runtime settings on EA_Core:** shadowless fills, with EA_Light turned down. Then recalibrate under the same rules. I recommend (a): it follows GH2's "fix the fixture, never the gate". Whether it makes the cubes' and EA_LOD's pink windows overlap is untested.
   - **(b)** Read the checks differently (front face only, or per object). This loosens GJ3.
   - **(c)** One of the options not adopted earlier: carry whole-frame G3 on the fixture only, or keep the textures out of the default set.
2. **Timing:** a new job needs about 5 min of build, about 15 min of calibration and about 3 min per leg. Ruled by about 12:30 IST it can still finish before 13:10; otherwise it waits until after 15:15, alongside 094-45 (camera_clipping).

Files are in `D:\IntrusiveAnomalies\_reviews\`:
- 094-44e-code-review.md
- 094-44e-codex-report.md
api-equivalent cost (not billed on Max): $13.90 | turns: 122 | running total across all briefs: $5,713.05
