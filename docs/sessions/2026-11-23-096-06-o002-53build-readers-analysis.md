# 096-06 — After O-002 on Bates: the 5.3 build fix, reader and small bug fixes, analysis of six real-host findings

Date: 2026-11-23 (22:10–about 00:30 IST). Author: Claude Code (Opus 5.5, xhigh), headless. Light brief: Editor-target
compiles only (2 parallel actions, launched at ≥ 4 GiB free), pure selftests, Python suites. No Game build, no editor or game
launch, no capture, no cook, no Codex. Ruling `_reviews\096-06-o002-ruling.md`; readings `_bates_reads\2026-11-23-O-002-bates-reads.md`
(not copied anywhere). Receipts `_relay\runs\2026-11-20-096-office-agent\code-receipts-096-06\`.

## 1. Part 1 — builds on UE 5.3 as well as 5.1 (`9062cf1`, integ)
- **Include.** `TexCorruptTree.cpp` includes `StaticMeshComponentLODInfo.h` behind `__has_include`. 5.3 only forward-declares
  `FStaticMeshComponentLODInfo` in `StaticMeshComponent.h`, and the file indexes `LODData`. In 5.1 that header exists in
  `Engine/Public` as an empty placeholder, so the include changes nothing there; in 5.7.4 it holds the definition.
- **Vertex factory lookups.** The instanced static mesh factory and both GPU skin factories are looked up by name
  (`FVertexFactoryType::GetVFByName`), which is exactly experiment E1b.
  - In 5.1 every vertex factory type adds itself to the name map in its constructor (`VertexFactory.cpp`).
  - The names are `FInstancedStaticMeshVertexFactory`, `TGPUSkinVertexFactoryDefault` and `TGPUSkinVertexFactoryUnlimited`
    (`InstancedStaticMesh.cpp:1025`, `GPUSkinVertexFactory.cpp:76-89`). The same names are in 5.7.4.
  - So the lookup returns the object `&StaticType` named.
  - When nothing resolves, the readiness record says `vf_unresolved` and the slot is refused `draw_shaders_missing` by name;
    it never crashes.
  - `FLocalVertexFactory::StaticType` stays: it linked on 5.3 (E1b built with it) and is exported in 5.1 and 5.7.
- **FeatureLevel.** The nine `UWorld::FeatureLevel` member reads go through the new `AnomalyEngineCompat::WorldFeatureLevel`.
  The brief named `GetFeatureLevel()` as an accessor in both engines; **5.1's `UWorld` has none** (G586).
  - The helper reads the member before 5.3 and the accessor from 5.3 on, so 5.1 behaviour is unchanged.
  - No other plugin file uses a type that 5.3 only forward-declares in `StaticMeshComponent.h`. The other uses of
    `FMeshMapBuildData`, `FStaticMeshLODResources` and `FConvexVolume` already include their headers and built on Bates.
  - The office card listed `Anomaly_StuckLowMip` among the files with warnings, but no member read exists in that file at
    `d2f6873`. Not reproduced, so not explained; after this change no plugin file reads the member.
- **Builds.** Strict-include Editor build of the integ tree: 0 errors, 0 warnings, 74 header TUs, restore OK. Normal Editor
  build: exit 0, 0 warnings. Strict on the m57 tip: see §6.

## 2. Part 2 — readers and small bugs
**Item 2: PIE `pie_end_settle` is read wrongly, the labels are right.**
- **What the plugin writes.** `StepPieEndSettle` (`AnomalyCaptureSubsystem.cpp:8362-8430`) writes, in PIE only, an unlabelled
  transition-only entry (reason `pie_end_settle`) on the first frame after each fire-window label run. It is never a member
  of the event, never masked, and never an anti-aliasing excuse.
- **The record.** 090-10e (G450) measured that the old "PIE end +1" was the reader's: 84 of 84 PIE events were in sync
  against the labelled set. The flag stays as a precaution on a frame that is already unlabelled. So in PIE the revert does
  not reach the screen a frame late (measured), and that frame is flagged, not a clean negative.
  - The plugin's one-time log line still says the picture "was measured to stay changed"; that wording is from before
    090-10e (the readme was corrected then; the log line was not).
- **Why the readers failed.** Under m56 that frame is the revert tick.
  - `m56_continuity_check` C7 and `verify_capture --label-rule` both anchor the revert on "the last tick the event has a
    non-settle entry". They counted the PIE entry, put the revert one tick late and called both after-offset settle rows
    misplaced. The continuity reader then skipped the next real onset.
  - The second failure: a vetoed event (m26) leaves `annotation.json` and keeps its transition entries. The reader excused
    it only if every reason was `settle`; in PIE it also carries `pie_end_settle`.
  - The old readers, run on a card-shaped session, give the office numbers: 30 unflagged / 150 outside (office 29 / 133),
    and `ANNOTATION-TRANSITIONS 1` with `SETTLE-MISPLACED`.
- **Fixes.**
  - In a session marked PIE (`pie_end_settle_active` true), an unlabelled pie-only entry is not a live tick.
  - The vetoed excuse allows `settle` ∪ `pie_end_settle` in PIE only.
  - Outside PIE, or labelled, nothing changes and the reader still fails.
  - `verify_capture`: `75af026`, 5 new cases (99 on integ, 129 on m57). Office tools and CaptureBench `m57-gates`
    (`c235fe9`): continuity selftest 15 cases.

**Item 2: `m56_settle_content_check` on one target — the reader is half right.**
- **Why the target was never labelled.** In a game world, `ShouldRender()` = `IsVisible() && !Owner->IsHidden()` (engine
  `SceneComponent.cpp`). The texture anomalies count a slot as installed only if its component `ShouldRender()`s
  (`Anomaly_MissingTexture.cpp:331-332`, `Anomaly_CorruptedTexture.cpp:331-332`). On an actor the host hides, every
  pink/checker frame is `effect_interrupted`: never labelled, so it can have no labelled settle frame.
- **Why its blink masks were unmeasured.** `blinking` labels by our own hidden state (m45), but the target mask is armed only
  for actors with `!Actor->IsHidden()` (`AnomalyCaptureSubsystem.cpp:1723`), so its frames are `unmeasured`. The m26 record
  is then never resolved (NOT_MEASURED), which admits the event.
- **The fix, and the open problem.** This one cause explains both readings. The host-hidden state of that actor is
  **not established**; the run log line `TARGET MASK NOT ARMED ... withRecord=1 visible=0` on those frames would show it.
  - The reader now lists never-labelled events instead of failing them, with selftest 11 cases.
  - It keeps failing an event whose every labelled frame is unmeasured, and says why.
  - **If confirmed, that blink is a labelled event with no visible change on a real host. It is not fixed here.** The
    selector's renderable test does not look at the owner's hidden flag (H5 class (i)); see the analysis, item 10's
    neighbour.

**Item 3: `m57_lod_readout` L3.** It now also counts the pool's
`Auto.Yield lod_popping: 0 of N candidates eligible - lod_not_armable:<reason> <rounds>` line (selftest 27).

**Item 4: `lod_spatial_swap_visible` (`6b29ca7`, m57).**
- **Why it read null.** The event key was always written as a bool (`AnomalyLabelWriter.cpp:1783`); swap records never
  carried one, so the per-swap reading O-002 asked for found null on every swap.
- **The fix.** Each swap now carries it: true when its cause is `distance` or `view_entry` and `on_screen`. One rule,
  `AnomalyLabel::IsSpatialSwapVisible`, sets the swap's value and the event's.
  - `verify_capture` requires a bool on every swap once any swap has one (older sessions are read as before), with 5 cases.
  - Readme: the 2.3 changelog row and the field list.

## 3. Part 3 — analysis (`_reviews\096-06-analysis.md`)
One recommendation per item:
- **5.** Settable yaw floor of 5 deg/s, arm timeout 150.
- **6.** Distance mode was tried and refused (`not_approaching`, 28 < 50 cm/s); the pool's dominant-only tally hid it. Log all
  reasons, 20 cm/s floor, a 300-frame distance lookahead.
- **7.** Give `lod_corruption` m57's engine-exact shown LOD, the 4× contrast and the 7 % coverage floor.
- **8.** Refuse HLOD proxies by class (`hlod_proxy`); do not widen the texture census.
- **9.** A terrain-only blink ceiling, default 25, office test at 40 %.
- **10.** Every fire re-resolves its target by exact name and hits every same-named actor (G588). Carry `@path`, and refuse
  ambiguous targeted names.

## 4. Part 4 — office pack
- `docs/office-pack`: office-tools reader fixes (`d2be54c`).
- A START-HERE line: every office edit to plugin code must come home, with the 5.3 example.
- The integ tip merged in (true merge).

## 5. Gotchas
G586 (5.1 `UWorld` has no `GetFeatureLevel()`), G587 (under m56 the PIE end-settle entry sits on the revert tick), G588 (an
exact-name token is world-wide). Next free **G589**.

## 6. Builds and tests
- **integ.** Strict-include Editor build: 0/0, 74 header TUs. Normal Editor build: exit 0, 0 warnings.
  - 19 pure C++ selftests pass.
  - Python: label rule 99, pixel gate 101, black frame, change oracle 35, kit 102, unit tests, exclusion gate, measure offset.
  - Lever audit source and binary PASS (57), selftest 12.
  - Terrain mutants 5/5 fail; `lod_refusal_selftest` 141/0 with 13/13 mutants failing.
- **m57 (`6b29ca7`).** Strict-include Editor build: 0/0, 77 header TUs. Normal Editor build: exit 0, 0 warnings.
  - 20 pure C++ selftests pass.
  - Python: label rule 129, kit 104, the rest as above.
  - Lever audit source and binary PASS (62).
  - Terrain mutants 5/5; LOD refusal 141/0 with 13/13 mutants failing; `lod_trigger_selftest` 169/0 with 16/16 mutants failing.
- **Pre-push scans.** Scrub clean, token absent, no comment markers, no new mojibake (`AnomalyCaptureSubsystem.cpp` holds 32
  older sequences, unchanged).

## 7. State at the end
- `integ/m56-m58` = `75af026` plus this docs commit.
- `feat/m57-lod-spatial-trigger` = `6b29ca7` plus the merge of this docs commit. **That merge is the commit the office
  installs next**; the report names it.
- `docs/office-pack` = `de2ad1c` plus the merge of the integ tip.
- CaptureBench `m57-gates` = `c235fe9` (local, no remote).
- Checkout left on `integ/m56-m58`. The host plugin Editor DLLs were last built from the m57 tree. No Game build, no cook.
