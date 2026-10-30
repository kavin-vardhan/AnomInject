# 094-27 runtime design and prediction — 2026-10-30

Authority: 094-27-request FB1–FB7, then FF1–FF5, FE1–FE5, FD1–FD6,
FC1–FC9, FA1–FA8 and the earlier rulings in the request. This kit uses
the three immutable packages authored in 094-26d and the unchanged EW1
4.25 binary manifest. It neither builds nor binds a 5.1 runtime.

One full EA8 entry proof serves the run (FF5); every launch still verifies
the exact Content/config/descriptor/binary/tooling sets, routing and slots.
The full exit proof closes the run. The authoring kit remains spent.

## Sessions and limits

| Order | Session | Limit | Purpose |
|---|---|---:|---|
| 0 | Smoke | 300 s | Native Python bindings and loaded probe, no asset save |
| 1 | Load | 300 s | Cold Core/Travel load, property readback and module diagnostics |
| 2 | WorldsPIE | 600 s | Two in-process PIE worlds, pause/resume/travel, three restart cycles, preview |
| 3 | WorldsGame | 300 s | Standalone pause/resume, travel and graceful exit |
| 4 | GC | 300 s | Native reflected-field/array/component-shell GC probes |
| 5–8 | A7 positive, one per anomaly | 900 s each | Native screenshots and apply/revert events |
| 9–16 | A7 NoApply and SkipRestore, one per anomaly/control | 300 s each | Real negative controls, retained evidence |
| 17 | Admission | 600 s | Registry/refusals, paired 4242 sequence, residue and travel |

Each slot is single-use. A failure retains all evidence and ends the run;
there is no retry of a real session. The deadline stops only the retained
owned process tree, including detached descendants. A sibling is never stopped.
All session starts require ten minutes of idle plus three measured gate polls.
The window closes 2026-10-31T00:30:00Z; an already-started session may finish.
Smoke precedes every other leg. No cook, commandlet, UAT or packaging path exists.

FB3 explicitly sets the measured-session free-commit floor to **6 GiB**;
this supersedes the carried FD3 4 GiB session line for this runtime kit.
Admission remains 8 GiB commit and 6 GiB physical. Physical memory, human
input and foreign GPU activity are recorded, not acting stops. The other
acting limits are disk floors, session deadline and 60 seconds of unreadable
acting measurements. Measured events with human input, foreign GPU above
15%, or missing observation coverage do not count. No threshold is relaxed.

## Source-backed execution

All paths below are in `E:/EpicGames/UE_4.25/Engine` unless stated otherwise.

* `Source/Editor/UnrealEd/Private/Tests/AutomationEditorCommon.cpp:635–649`
  runs SIE then ordinary PIE with explicit end commands. `:773–794` calls
  RequestPlaySession and RequestEndPlayMap. The stock `System.Maps.PIE` test
  in `Plugins/Tests/EditorTests/Source/EditorTests/Private/UnrealEd/EditorAutomationTests.cpp:185–209`
  uses the command-line-overridden AutomationTestmap. Only that test is selected.
* `Source/Editor/UnrealEd/Private/PlayLevel.cpp:821–838` duplicates play
  settings into the transient package. `:1050–1059` avoids save admission
  for in-process play. `:2453–2458` forces SIE to one world; it is not used
  as the two-world proof. Ordinary PIE uses two standalone clients under
  one process. `Classes/Settings/LevelEditorPlaySettings.h:299–312` declares
  the relevant config fields. No settings are saved.
* EditorLevelLibrary's `LoadLevel` is load-without-save
  (`Plugins/Editor/EditorScriptingUtilities/Source/EditorScriptingUtilities/Public/EditorLevelLibrary.h:233–240`).
  The Python driver uses only stock reflected APIs and the existing native
  Europa.Probe commands; it never authors assets.
  Runtime callbacks are started with `-ExecCmds=py <launcher>` rather than
  ExecutePythonScript: EditorPythonExecuter.cpp:148–153 and :185–195 closes
  the editor on the next tick after ExecutePythonScript returns. The ordinary
  PY executor (PythonScriptPlugin.cpp:560–566) permits the retained callback;
  UnrealEngine.cpp:1584–1600 queues the command. The driver requests QUIT_EDITOR
  after completion. AutomationTestSettings belongs to /Script/Engine and
  uses Engine config (Runtime/Engine/Classes/Tests/AutomationTestSettings.h:339–362).
* The committed host `Source/EuropaPhaseAProbe/EuropaPhaseAProbeDriver.cpp:149–242`
  implements preview, snapshots, pause, GC, auto, screenshots and travel.
  `:210–215` calls OpenLevel and clears the script on travel. A standalone
  leg therefore ends its script with travel, observes the new world, then
  requests normal closure of its own window. No Python module is forced
  into a game: PythonScriptPlugin.uplugin:26–28 is Editor-only and
  ModuleDescriptor.cpp:494–499 requires GIsEditor.
* `EuropaPhaseAProbeDriver.cpp:245–300` uses the engine screenshot request
  and completion delegate. Images must actually be 1280x720 PNGs, one
  outstanding request, no UI or HighResShot. The native receipt binds
  world/frame/epoch and completion. WindowsPlatformTime.h:23–29 uses QPC
  seconds plus 16777216; the monitor records that same clock for event joins.
* Analytics: `Source/Runtime/Engine/Private/EngineAnalytics.cpp:105–122`
  creates the provider only for an editor run with AreEditorAnalyticsEnabled.
  A `-game` run is explicitly excluded at :107–108. Every argv nevertheless
  carries both accepted FC4/EU4 EditorSettings overrides. EditorServer.cpp:1235–1237
  reads bSendUsageData. ConfigCacheIni.cpp:1100–1158 implements `-ini:`;
  :1476–1481 implements `-nowrite`. GenericPlatformCrashContext.cpp:389
  reads the crash privacy setting. Both session logs are checked for uploads.
* AA is disabled through `-ExecCmds=r.PostProcessAAQuality 0` (FE1 and plan
  E3), with a native `prepare` echo. `Source/Runtime/Engine/Private/SceneView.cpp:845–858`
  reads that console variable and sets AAM_None for quality zero.
  No nonexistent post-process AA property
  is set. Fixed exposure and motion blur come from the saved FE1 recipe.
* A standalone window is closed only by its retained PID after the native
  completion row. `Source/Runtime/Engine/Private/GameEngine.cpp:658–663`
  routes its window-close callback to GameViewport.CloseRequested and normal shutdown.
  WindowsApplication.cpp:2347–2352 handles WM_CLOSE. No arbitrary process is closed.
* LOD prediction amendment, 2026-10-30, before the runtime tests: the bound
  plugin's `Source/AnomalyInjector/Private/AnomalyLod.cpp:85–90,144–151`
  uses one-based ForcedLodModel. The runtime test selects 2 for the copied cube
  LOD, rather than 1 (the sphere's original LOD). No recipe or asset is changed.

## Protection and evidence

FF2 records all host Saved changes except every Saved/Config .ini; the latter,
project Config, descriptors, .uproject, all Content and bound binaries remain
protected. A new crash folder is independently a crash STOP, regardless of
its managed-file class. FD2 build writes/children remain STOPs. FE3 additionally
refuses unreadable UBT argv, implicit TargetList builds and Clean/Execute/Deploy.
The argv of each UBT descendant is retained. EU4/FC4 switches are mandatory,
with exact argv comparison, including standalone runs.

The existing EA fixtures contain no glass actor or glass material. The frozen
ROIs cover only those primitive fixtures and their floor/shadow, excluding
all Paris glass. E3 thresholds stay N<=0.25, active mean>=4 and >=4N, >=1%
pixels differing by >=8; two restored frames mean<=max(0.5,2N), <=0.1% pixels
differing by >=8. State must match exactly. Blinking needs both visible and
hidden subframes selected by pixels, never by the command signal.
Frozen `(x,y,width,height)` ROIs: blinking `(805,310,360,360)`, missing_object
`(115,310,360,360)`, lod_corruption `(450,170,380,420)`, lighting_mismatch
`(80,250,1120,450)`. These are predicted from the committed fixture coordinates,
camera and FOV before any real picture. A positive leg attempts six events;
at least three must pass both the unchanged picture reader and FB3 validity.
NoApply must fail visibility; SkipRestore must fail restoration. Counts exclude
contaminated windows. Contact sheets and their index retain all counted events;
the owner's eye check remains distinct from numeric qualification.

Prediction, frozen before tests or launches: existing fixtures and binaries stay
hash-equal; smoke exposes every actual binding; all four effects have observable
apply/revert and the controls fail the unmodified picture predicates. A missing
binding, unsupported native route or unqualified image is reported as a failed
claim, never replaced by fabricated evidence or a looser threshold. D2 permits
two harness-repair attempts per problem before real execution; there is no real
session repair/retry. Stand-ins exercise the same runner admission and owned-tree
paths without touching the host, real slots or Unreal.
