# 084-10 — Release hygiene on `fix/m52-label-timing`: one bench gate, G-LEVER-AUDIT, the packaging items as scripts (2026-09-29)

**Brief:** 084-10 (Code, xhigh), from chat ruling `_reviews\085-01-chat-ruling-dc-plan.md` "The unanticipated items" item 1
(+ ND-6), with `085-00` IN-7 and "Release housekeeping", and the DC plan `docs/predictions/2026-09-29-m53-dc.md` §8 (on
`feat/m53-uv-normal-corruption`) as the design m53 inherits.

**Outcome:** source commit `92c3500` (+ this docs commit); Game exe **`667FD4EF`** and the Editor DLLs archived at
`_binary_baselines\m52fix-667FD4EF\`; evidence `_reviews\084-10-evidence\`. ⛔ Nothing launched, no cook, nothing staged, not
swapped in (the 084-09 bench keeps its pins), NOT merged, NO tag.

## 0. Bootstrap, in three lines

1. The bench twins show in delivered builds today, and the m52-only fallback must ship clean, so hygiene lands here first
   and m53 inherits it through 085-02 (085-01 item 1; ND-6: `-IAIBenchFixture` also opens the gate).
2. 085-00 IN-7: every `IAI.Bench.*` lever (DestroyTarget included) registers only under the gate and is compiled out of
   Shipping; the twins are not in the catalogue or dashboard without it. Housekeeping: dashboard `.env`, readme host-tools
   path, AnomalyBench/fixture exclusion lines, AnomDash edits listed.
3. DC plan §8: one `Public/AnomalyBenchGate.h`; a sweep at `Initialize` that sets `ECVF_Unregistered` on every
   `IAI.Bench.*` object; inline flag checks switch to the gate; G-LEVER-AUDIT with a planted-registration can-fail.

## 1. What was built

### 1.1 The gate (`Source/AnomalyInjector/Public/AnomalyBenchGate.h`, `Private/AnomalyBenchGate.cpp`)

| function | what it does |
|---|---|
| `IsEnabled()` | `false` in Shipping; otherwise `FParse::Param(-IAIBench) \|\| FParse::Param(-IAIBenchFixture)`. `FParse::Param` needs whitespace or end after the token (`Parse.cpp`), so the two flags never alias. |
| `SweepUngatedLevers(Where)` | Collects every console object named `IAI.Bench.*`. Gate open: logs `IAI bench levers: ENABLED (<flags>) where=… present=<n>`. Gate shut: ORs `ECVF_Unregistered` into each (the console refuses it, `FindConsoleVariable` returns null, autocomplete skips it; the object stays alive), counts variables whose SetBy is not the constructor (`preset`, each named in a Warning), and logs `IAI bench levers: DISABLED (<n> masked) where=… newly=<k> preset=<p>`. |
| `FindLeverCommand(Name)` | The one masked-aware lookup: null if absent, masked, or not a command. |
| `DescribeFlags()` | The flags present, for log lines. |

**Where it runs:** `FCoreDelegates::OnPostEngineInit` (registered in `AnomalyInjectorModule::StartupModule`; covers the editor,
where the injector subsystem initializes only on PIE) and every `UAnomalyInjectorSubsystem::Initialize` (unconditional, after
the catalogue is built).

**Engine facts it rests on (UE 5.1, read before building — G374):** `ConsoleManager.cpp:1853` (console refuses masked),
`:1564` (`FindConsoleVariable` → null), `Console.cpp:97` (autocomplete skips). `SetFlags` **replaces** the flag word
(`:138`), so the sweep ORs; `UnregisterConsoleObject` on a command **deletes** it (`:1669-1672`), so the sweep never calls it;
`FindConsoleObject` does **not** filter the mask (`:1575-1623`), hence `FindLeverCommand`.

### 1.2 What moved behind it

| site | before | after |
|---|---|---|
| `null_effect` / `solid_swap` registration (AIS `Initialize`) | every non-Shipping build | `#if !UE_BUILD_SHIPPING` **and** `if (AnomalyBenchGate::IsEnabled())` ⇒ not in `IAI.ListAnomalies`, `GetAnomalyCatalog` or the dashboard without a flag |
| `AnomalyBench` `PlaceView` / `InputLock` / `SceneFixture` | registered in `StartupModule` unconditionally | registered only when the gate is open (the module can also be loaded lazily after a sweep, so it cannot rely on the sweep) |
| `IAI.Bench.CameraSchedule` (ABM), `StuckMipLegacyTiming/Purity` registration (ACS), TEXUSERS logging (ACS) | inline `FParse::Param(-IAIBench)` | `AnomalyBenchGate::IsEnabled()` (one mechanism; ND-6 opens them under `-IAIBenchFixture` too) |
| control server `bench_input_lock` / `bench_place_view` / `bench_scene_fixture` | `FindConsoleObject(...)->AsCommand()->Execute` (ignores the mask) | `AnomalyBenchGate::FindLeverCommand` (still also behind the stricter `-IAIBenchFixture` + map check) |
| every other `IAI.Bench.*` (27 file-scope statics) | always registered | masked by the sweep when the gate is shut |

**Kept stricter, deliberately:** every `-IAIBenchFixture` check (twin Apply, m55 `ChangeGate`, `ChangeTeardownAt`, the control
server's fixture commands, capture's L2 lines) — the fixture gate stays the named-map gate the DC plan keeps.

### 1.3 Compiled out of Shipping

- AIS: the nine levers that compiled into Shipping (`SynthTickOrder`, `HideMode`, `HideOmitShadowSilencing`,
  `HideOmitDepthPassSilencing`, `DestroyTarget`, `SpawnTranslucentProbe`, one block `:1132-1407`), `DestroyTarget`'s globals,
  `ServiceBenchDestroyLatch` and its call, and `SetSynthTickOrder`'s two lever log lines.
- SLM: `StuckMipNoHold`, `StuckMipVirtualProbe`, `StuckMipUnlinkLock` (one block to end of file) and the NoHold log line in
  the hold path.
- ACS: **the ten `SetBench*` setters, which sit OUTSIDE `#if ANOMALY_CAPTURE`** (three contiguous groups, wrapped as whole
  definitions so a future Shipping caller is a link error, not a silent no-op), and the host-PP census log line that names a
  lever. **Found by the audit's first run (§2.1), not by reading** — G375.

### 1.4 Byte-identity guard

`AnomalyChangeStage::ChangeSettingSource` read SetBy through `FindConsoleVariable`, which returns null for a masked lever, so
`run_summary.change_bench_gate_source` would have flipped `compiled` → `unavailable` on every change-evidence run without a
flag. It now reads through `FindConsoleObject` + `AsVariable` — identical for every unmasked variable, and still `compiled`
for the masked one (masking does not touch the SetBy bits).

## 2. Proof both ways

### 2.1 G-LEVER-AUDIT (`tools/lever_audit.py`)

Evaluates the preprocessor per configuration (Shipping / Development / Editor, with `ANOMALY_CAPTURE` and
`ANOMALY_CONTROL_SERVER` confirmed from their `Build.cs`), reads module Shipping eligibility from the `.uplugin`, finds every
`"IAI.Bench."` literal and classifies it (registration / lookup / other), and every catalogue `Register(MakeUnique<…>)`.

| rule | asserts |
|---|---|
| R1 | no `IAI.Bench.` string live in a Shipping compile of a module Shipping builds (only exception: the gate's own prefix constant) |
| R2 | every registration live in Development/Editor is a file-scope static (swept) or runtime-registered only through `IsEnabled()` (enclosing `if` or an early `return` on `!IsEnabled()`) |
| R3 | the sweep is called unconditionally in `Initialize` and at `OnPostEngineInit`; `IsEnabled()` opens on both flags and is false in Shipping; the sweep masks by prefix with `ECVF_Unregistered` |
| R4 | bench catalogue entries (anomaly classes whose source requires `-IAIBenchFixture`, plus the twin ids) are gated and Shipping-excluded; no pool/selector names a twin |
| R5 | a lever is looked up by name only through `FindLeverCommand` (or the read-only `ChangeSettingSource`) |
| R6 | no inline `TEXT("IAIBench")` outside the gate |
| B1–B3 | with `--binary`: no lever name in the binary the source does not compile; every registered name present (stale build fails); the echo strings present |

| run | result |
|---|---|
| **first run on the finished source edits** | **FAIL R1 × 26** — the `SetBench*` setters and three log lines (§1.3); fixed, then: |
| source (`92c3500`) | **PASS** — 33 lever names, 33 registrations (27 static/swept, 6 runtime/gated), 4 lookups all via allowed helpers, 12 catalogue entries, 2 bench entries gated, 0 twins in pools |
| `--selftest` | **11 cases OK**: clean copy PASS; each planted violation FAILs on its rule — static lever appended to a Shipping file (R1), the AIS block's `#if` removed (R1), a runtime `RegisterConsoleCommand` in `Initialize` without the gate (R2), the bench module's commands ungated (R2), the sweep call removed (R3), `IsEnabled()` without `-IAIBenchFixture` (R3), twins outside the gate (R4), twins compiled into Shipping (R4), a raw `FindConsoleObject` lookup (R5), an inline `-IAIBench` test (R6) |
| `--binary` Game `667FD4EF` (Development) | PASS — `names_in_binary=33 names_in_source=33 registered=33 unknown=0 missing=0 echo=present` |
| `--binary` the five Editor DLLs (Editor) | PASS — same counts |
| `--binary` pinned pre-gate `8A6074AA` | **FAIL B3** (no echo strings) — the stale-binary direction on real data |
| staged `37bb750` source (pre-gate) | **FAIL R1–R6, 76 failures** |

⚠ **Not shown here, by the brief's rule (no launch): the runtime half.** That the sweep fires, that `IAI.ListAnomalies` omits
the twins and that a masked lever is refused are predicted from source; G-LEVER (DC plan §8, two night legs) is their first
observation. No Shipping build exists; the Shipping claims rest on the preprocessor evaluation (R1).

### 2.2 G354 descriptor + fixture check (`tools/check_package_descriptor.py`)

`UnrealPak <pak> -Extract -Filter=*.uplugin` → module-by-module diff against the branch descriptor minus `AnomalyBench`, the
plugin dependency list, and a `.utoc` scan for `AnomalyFixtures`, `CaptureBenchGate` and the `CB_*` maps (the index stores
folder names without slashes — the first needle set with `/…/` missed them).

| input | verdict |
|---|---|
| `--selftest` | 8 cases OK |
| 084-08b bench cook `m52fix-cook-BF06AE61` (delivery mode) | **STOP** `EXTRA AnomalyBench, FIXTURE` (`CB_GateLevel`, `CB_LodFixture`); descriptor SHA-256 `9EFB9B49…` = 084-07b's reading |
| same with `--bench-cook` | **PASS** (5 modules identical) |
| `Builds\Windows` package of 2026-07-15 | **STOP** `MISSING AnomalyShaders`, fixture `hits=none` |

### 2.3 The exclusion lines (`tools/stage_plugin_delivery.py`)

`git archive <ref>` through an allowlist (`AnomalyInjector.uplugin` rewritten without `AnomalyBench`, `LICENSE.txt`,
`Content/`, `Shaders/`, `Source/` minus `Source/AnomalyBench/`; `docs/`, `tools/`, `CLAUDE.md`, `WebClient/` excluded), then
self-verifies (G354 + lever audit on the staged tree) and prints the cook exclusion lines (`+DirectoriesToNeverCook` for
`/Game/AnomalyFixtures` and `/Game/CaptureBenchGate`, no fixture map on `-map=`). `37bb750` → **FAIL** (lever audit R1–R6);
`92c3500` → **PASS - deliverable** (staged descriptor SHA-256 `62E6B9AD…`).

### 2.4 The dashboard token file and README paths (`tools/check_delivery_bundle.py`)

`--selftest` 7 cases OK (absent `config.json`, empty token, placeholder, mismatch, README `tools\` path all STOP; stray `env`
only WARNs). On a real bundle made by AnomDash `make_delivery.py --yes --plugin-repo <this plugin>` (read-only for AnomDash;
the bundle was deleted afterwards because it held a token): **PASS** with the current README, **STOP
`README tools/verify_capture.py`** with `37bb750`'s README; the M2-era `_M2Smoke` folder STOPs on three missing files.

## 3. Nothing else changes

- **String scan, Game `8A6074AA` → `667FD4EF`** (`string-scan-8A6074AA-vs-667FD4EF.txt`): UTF-16 (every `TEXT()` literal —
  field names, JSON keys, log lines) 13 added / 4 removed, **all gate or bench text** (the two echo lines, the preset
  warning, `post_engine_init` / `subsystem_init`, the flag names, the `IAI.Bench.` prefix, the reworded
  `StuckMipLegacy*` / `CameraSchedule` help and the m52 bench line), plus 2 adjacency artifacts (the same string with a
  neighbouring byte changed). Text-like ASCII: only the new `AnomalyBenchGate.cpp` path.
- **Unit suites:** `m52_window_selftest` **282 / 0**, `camera_clipping_slab_selftest` **154 / 0** (both `/W4 /WX`),
  `verify_capture.py --label-rule --selftest` **24 OK**; also `--label-pixel-gate` 98 OK, `--change-oracle` 35 OK, bare
  `--selftest` OK.
- **Build:** two runs, both targets exit 0, **0 warnings**; run 2 is archived (every Source file predates its start).
- **Development behaviour that does change, and only under a bench flag:** with `-IAIBenchFixture` alone the
  `StuckMipLegacy*` commands and `CameraSchedule` now register and TEXUSERS lines are logged (ND-6's one mechanism). Log
  lines only; no label, mask or row reads them.

## 4. Findings

1. **G375** — the registrations were all Shipping-excluded before the audit ran, and it still found 26 lever strings compiled
   into Shipping (setters outside `ANOMALY_CAPTURE`). Stated because a grep-only check would have passed.
2. **G376 — the AnomDash bundle ships the owner's DEV token.** `npm run build` copies the gitignored `public/config.json` into
   `dist/`, `DIR dist → dashboard` ships it, and the bundler prints *"config.json was NOT copied"*. Measured: bundle
   `dashboard\config.json` byte-identical to `public/config.json` (SHA-256 prefix `860D1969A04D`, 64 chars; it matches the
   scratch host's ini). This is the M2 incident's shape; the other shape (no file ⇒ `Setup.bat` writes an empty token) is
   what the brief's `.env` wording points at. ⛔ AnomDash was not changed; the checklist's §2 box + `check_delivery_bundle.py
   --expect-token-log` catch both.
3. **G374's fourth point:** a lever set by ini or `-dpcvars` before the sweep keeps its value; the sweep reports it
   (`preset`) and the checklist makes `preset>0` a STOP.
4. **G377:** from this build on, a leg that uses a lever needs `-IAIBench`; without it the lever is refused and the leg is a
   clean null. The 084-09 harness is pinned to `8A6074AA` and unaffected; 085-03's harness must pass the flag and assert the
   ENABLED echo (the DC plan already puts `-IAIBench` on its legs).

## 5. Deviations and interpretations, stated

- **"The dashboard `.env` goes into the delivery package":** the current dashboard reads no `.env`; its token file is
  `dashboard\config.json` (M2 replaced the `.env` bake). Implemented as: that file must be in the package carrying the
  delivered build's token, proven by a script, with the manual step written into the checklist. No AnomDash code changed.
- **"The AnomalyBench/fixture exclusion lines":** implemented as the staging script's allowlist + the cook exclusion lines,
  with the G354 script's `FIXTURE` scan as the proof from a package.
- **Runtime proof (G-LEVER) not run** — the brief forbids a launch. **No Shipping build** — not requested; R1 is a source proof.
- **The archive also carries the five Editor DLLs** (the audit's Editor cross-check reads them).
- `SetBench*` setters are wrapped as whole definitions, not empty bodies (§1.3) — a deliberate choice for loud failure.

## 6. AnomDash on this PC

One clone, `D:\IntrusiveAnomalies\anomaly-dashboard`, `master` at `618b8df` == `origin/master`; `git status` clean (only
ignored `public/` and `src-tauri/gen/`), no stash, no extra worktree. **No uncommitted AnomDash edits exist on the home PC.**
The `_M2Smoke` and `D:\IntrusiveAnomalies\host-tools\` folders are not repositories.

## 7. State and hand-off

- `fix/m52-label-timing`: `92c3500` (source + tools) and this docs commit, pushed. No tag, no merge.
- Archive `_binary_baselines\m52fix-667FD4EF\` (6/6 re-hashed); index line added to `_binary_baselines\README.md`.
- **085-02 (merge fix → m53):** m53's `IAI.Bench.TexCorrupt*` levers are file-scope statics under `#if !UE_BUILD_SHIPPING`
  (swept), but any S2 lever that tests `-IAIBench` inline will fail R6 — re-run `python tools\lever_audit.py` (and
  `--selftest`) on the merged tree and convert what it names. Add m53's bench twins or fixture classes (if any) to the
  audit's bench-catalogue rule by the same structural test (their source requires `-IAIBenchFixture`).
- **G-LEVER (night):** Development with no flag — the DISABLED echo with `preset=0`, spot-check `SynthTickOrder`,
  `DestroyTarget` refused, no twins in `IAI.ListAnomalies`; with `-IAIBench` — ENABLED echo, twins listed.
