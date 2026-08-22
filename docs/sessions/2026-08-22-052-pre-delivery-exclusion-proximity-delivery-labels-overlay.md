# 2026-08-22 — session 052 — the final pre-delivery change set

**This file is SELF-CONTAINED. Read it and you have the whole session.** It covers seven plugin
commits and two AnomDash commits made the day before the client build ships.

⛔ **NOTHING WAS TAGGED. `m31` IS STILL THE OPEN MILESTONE AND STILL UNTAGGED. Highest tag is
`m30`.** `feature/stencil-capture` untouched at `76cac74`. `P6` did not move. No force-push.

---

## §0 THE ONE-SCREEN SUMMARY

| # | Commit | What |
|---|---|---|
| 1 | `979a4d0` | `fix(capture)` tick-pin fork probe hardened + build-time override |
| 2 | `8ddaab6` | `feat(injector)` config-driven target exclusion patterns |
| 3 | `2b0555c` | `feat(lod)` metric proximity gate for `lod_popping` |
| 4 | `f491514` | `feat(capture)` write `labels.jsonl` in delivery mode |
| 5 | `3baa200` | `feat(host-tools)` overlay inspection tool ships; two-colour boxes |
| 6 | `ed9092c` | `docs(client)` overlay tool, camera clipping, Pillow |
| 7 | `1821efc` | `docs(client)` labels.jsonl ordering + amber semantics |

AnomDash: `7bf62a0` (watcher/manifest/Run.bat) · `21d9fae` (packaging no longer needs a plugin repo).

**Owner ship decision, taken this day: the tick pin ships ON by default.** Concorde validation,
packaged and in-round: `TICKPIN active saved=1`, 300 frames, 7 events, 5 measurable, EVERY offset
`+0` at start and end, 4 of 5 at HIGH confidence, measurable range ±7.
`ticks_per_captured_frame` **1.2000 pinned vs 1.2699 unpinned** ⇒ **the pin does NOT accelerate
anomalies; the pre-registered blink recalibration DID NOT FIRE and no anomaly constant changed.**

---

## §1 THE TICK-PIN PROBE (`979a4d0`)

The `m31`-era probe keyed on a filename, `FWNetSubsystem.cpp`, taken from a third-party
assistant's prose rather than a verified listing. On Concorde it MISSED and the build logged
`TICKPIN not-compiled`. **That marker is RETIRED.**

🎯 **The probe is now a CONTENT probe for the literal token
`sUseFixedGameTickWithVariableRenderTick_Net`** — the exact symbol the pin writes to, which stock
UE 5.1 does not declare. Present ⇒ the fork is here AND the pin will compile. Absent ⇒ stock, the
pin compiles out. **This is not a heuristic marker, it is the exact precondition.**

Owner's measurement: the token appears in exactly four files on Concorde —
`Runtime/Core/Public/Misc/App.h`, `Runtime/Core/Private/Misc/App.cpp`,
`Runtime/Engine/Private/GameEngine.cpp`, `Editor/UnrealEd/Private/EditorEngine.cpp`.
**The fork modified CORE, not a separate net module** — which is why a net-module filename missed.

Routes, in order; **only the SYMBOL decides, never a filename**:
- **A (primary)** the four known sites, `App.h` first. All four exist on stock 5.1, so a miss
  reports `exists=True, symbol=False` rather than the useless `file not found`.
- **B** recursive content scan of `Source/Runtime/Core` (1603 files here), capped 20000.
- **C** fork-named files under `Source/Runtime` / `Source/Editor` + fork-named plugin folders
  (two levels). A filename hit with no symbol is a **HINT and decides nothing** — setting the
  define on a name alone would break the build wherever the symbol was renamed.

**Deliberately NOT searched, and the log says so:** `Engine/Plugins` is matched by plugin-folder
name only. A full file-by-file walk measured **18.6 s per build** and cannot hold a declaration of
`FApp`, which route B covers exhaustively. ⚠ **My first draft cost 13.6 s on every makefile build
and I only caught it because the probe prints its own elapsed time.** Now ~0.7 s on a stock miss,
~2 ms on a fork (route A hits the first file).

🔧 **BUILD-TIME OVERRIDE, because correctness must not depend on a probe guess.** An empty marker
file at the plugin root — `ANOMINJECT_TICKPIN_FORCE_ON` / `ANOMINJECT_TICKPIN_FORCE_OFF` — forces
the define either way, both targets, no source edit. Never silent: the probe result prints first,
then an `OVERRIDDEN` line naming the mechanism and what the probe had said. Registered as a UBT
`ExternalDependency` so REMOVING a marker auto-invalidates the cached makefile (UBT prints
`Invalidating makefile ... (ANOMINJECT_TICKPIN_FORCE_ON deleted)`). Markers are gitignored.

⚠ **CREATING a marker does not invalidate a cached makefile** — delete `Intermediate` (the office
procedure already does). 🚨 **If NO `TICKPIN probe` line appears in the build output at all, the
makefile was cached and nothing was re-probed.**

✅ **Guard proven BOTH ways.** Forced ON on stock ⇒ compile fails naming the symbol at both access
sites (`AnomalyCaptureSubsystem.cpp:67` and `:72`), exit 6 — and the log says IN ADVANCE that this
is a VALID DIAGNOSTIC RESULT, not a disaster. Forced OFF against a probe that says FOUND ⇒ clean
build. **Positive control (`G96`): a planted symbol makes the probe report FOUND**, so its NOT
FOUND is a reading and not blindness.

✅ **Build-graph no-op on stock, MEASURED:** the generated `Definitions.AnomalyCapture.h` is
**byte-identical pre- vs post-change** (`1827B204256D`, 10369 B).

---

## §2 TARGET EXCLUSION PATTERNS (`8ddaab6`)

Two owner-reported bugs, one mechanism, at the **`G33` chokepoint**
(`AnomalyViewport::IsRenderableComponent`) so it reaches selector, auto-injector and capture alike.

- **Bug A:** an anomaly fired on an actor whose asset is `lightblockerplane_sm`. Passes every
  predicate we have and manifests nothing visible. H5 family.
- **Bug B:** the skybox is selected even at a small poll radius — the radius is computed on
  **BOUNDS**, and a sky/backdrop mesh's bounds envelope the player, so it is always "within radius".

**Both are LABEL-QUALITY exclusions, not "the anomaly fails to occur"** — the m27
`AInstancedFoliageActor` rationale.

`[AnomalyInjector] ExcludedTargetNamePatterns` — an ARRAY of case-insensitive **substring**
patterns, matched against **ACTOR name, COMPONENT name AND MESH ASSET name**, reporting which field
matched. **Asset matching is required, not optional** — the owner's example is an asset-style name.

**COMPILED DEFAULT IS EMPTY** ⇒ byte-identical when the key is absent; cost when unset is one
`Num()==0` test. The plugin stays game-agnostic: host names live in the HOST ini.

📌 **`IAI.SetExcludedTargets <pattern>... | clear`** added for the standing `G88` reason (a loose
ini beside a package is a NO-OP), without which neither the owner nor a gate could exercise the
list on a packaged build. Precedence **console > ini > compiled-empty**.

**Silence is the failure mode this refuses:** one greppable `EXCLUDED-TARGET` line per excluded
actor (pattern, matched field, matched value, **source**), logged once per actor per run;
`run_summary.pattern_excluded_targets` counts DISTINCT ACTORS refused. `annotation.json` unchanged.

✅ **GUARD-BREAK, packaged, MainWorld auto-pool, 300 frames.** StackOBot's rocks reproduce Bug A's
exact shape — asset `SM_rock`/`SM_rock_02` while the actor name is an uninformative
`StaticMeshActor_UAID_...` — so this matches on the **ASSET FIELD ONLY**:

```
baseline (no patterns)          : 13 events [blink 2, missing_texture 6, corrupted_texture 3, lod_popping 2]
IAI.SetExcludedTargets SM_rock  :  5 events, ROCK EVENTS 0, pattern_excluded_targets 155
EXCLUDED-TARGET actor='StaticMeshActor_UAID_...' matched_field=asset
  matched_value='SM_RockFlats_01' pattern='SM_rock' source=IAI.SetExcludedTargets (console...)
```

⚠ **A `G139` bug of my own, caught and fixed mid-session:** the first version's log line named
`DefaultGame.ini` as the source even when the patterns came from the console. Provenance is now
computed, not assumed.

⛔ **THE INI ROUTE ITSELF IS UNTESTED HERE.** `G88` means a loose ini beside the staged package is
ignored, so only the CONSOLE route and the shared resolve/echo path were exercised. The ini route
uses the same shape as the four existing `[AnomalyInjector]` keys. **Concorde's cook is what proves
it — confirm by reading `excludePatterns=N(ini)[...]` on the StartRun line.**

### The ini block for Concorde
```ini
[AnomalyInjector]
+ExcludedTargetNamePatterns=lightblockerplane
+ExcludedTargetNamePatterns=REPLACE_ME_WITH_THE_SKY_NAME
```
To discover the sky's real name: capture once with the sky hit, read
`affected_objects.nodes[].asset_name` in that session's `annotation.json`. To confirm a pattern
bites, grep the run log for `EXCLUDED-TARGET`. **No shipped log line prints asset names for actors
that are NOT excluded**, which is why discovery goes via `annotation.json`.

---

## §3 `lod_popping` METRIC PROXIMITY GATE (`2b0555c`) — READ THE NUMBER

`[AnomalyInjector] LodPoppingMaxDistanceCm`, **compiled default 200**, plus
`IAI.Anomaly.LodMaxDistance <cm|default>` (same `G88` reason). Range `[0..1000000]`, out of range
**REFUSED not clamped**; `0` disables the DISTANCE gate only.

**The metric is the SAME one the poll radius uses** — sphere-approx bounds distance from
`ResolvePollOrigin` — so the two numbers are directly comparable. Negative means the bounds sphere
already contains the poll origin.

🚨 **IT ANDs WITH THE m30 7.0 % COVERAGE GATE AND DOES NOT REPLACE IT.** Recorded in the code's own
log text: the coverage gate was **CALIBRATED** against measured visibility (last visible 9.3453 %,
first invisible 3.9045 %); a metric distance is an owner **PRODUCT PREFERENCE**. **Removing a
calibrated gate to install an uncalibrated one is backwards.**

### 📊 FIRE RATE, MEASURED BEFORE SHIPPING — AND IT IS ZERO

A/B, same seed 777, same map, same 300 frames, the gate the only difference:

| leg | events | `lod_popping` |
|---|---|---|
| gate DISABLED (current behaviour) | 13 | **2** |
| gate at 200 cm (the new default) | 11 | **0** |

**Both draws were refused on DISTANCE ALONE, at `863.91 cm` and `1221.19 cm`** — 4.3× and 6.1× the
maximum — **and both had PASSED the coverage gate** (9.2572 % and 11.7191 %).

⚠ **WHY, and it is a property of the BENCH, not evidence the number is wrong:** an unattended
capture settles at a fixed pose and the pawn never walks up to anything, so **the owner's actual
case — a player standing next to an object — cannot occur in an unattended run.** The two measured
distances are on the record so a larger value can be chosen knowingly. ⛔ **NO VALUE IS
RECOMMENDED — the owner decides, and `IAI.Anomaly.LodMaxDistance` retunes it with no re-cook.**

---

## §4 `labels.jsonl` IN DELIVERY MODE (`f491514`)

The overlay tool draws from `labels.jsonl`, and `m12` delivery mode SUPPRESSED that file while
still COMPUTING the data — so the tool could not run in the config the client actually ships.

**Default ON.** Client-facing change, stated plainly: **it ADDS ONE FILE and nothing else.**

```
before : Actual_Frames/  annotation.json  run_summary.json
after  : Actual_Frames/  annotation.json  labels.jsonl  run_summary.json
```

`run.json` stays suppressed (seed still withheld, session still not client-reproducible).
**`annotation.json` keyset measured 48 vs 48 — `P6` DID NOT MOVE.**
Off switch: `IAI.Capture.DeliveryLabels <0|1>` / `[AnomalyCapture] bWriteLabelsInDeliveryDefault`.

---

## §5 THE OVERLAY INSPECTION TOOL (`3baa200`, AnomDash `7bf62a0`)

🚨 **REFRAMING, AND IT IS LOAD-BEARING: THIS IS NOT A LABEL PRODUCER.** Engine-side labels remain
authoritative. It draws boxes onto **COPIES** of PNGs for **HUMAN INSPECTION** and never modifies a
captured frame or a label file.

### §5.1 The phantom-box diagnosis, from artifacts

Across **389 banked sessions**: 12,548 shipped labels, 8,790 candidate-only boxes.

| mechanism | count | share |
|---|---|---|
| hide-type **span vs subset** | 7,919 | **90.1 %** — BY DESIGN, dominant |
| **vetoed** (m26/m27) | 871 | 9.9 % — confirmed present |
| **non-manifested** (m23) | 0 | 0 % — zero instances in the bank |

For hide types `annotation.json` carries only the frames where the object was actually HIDDEN,
while `labels.jsonl` covers the whole fire-active window — the lead-in frame and the un-hidden half
of each blink. One session dissected: annotation union 30 frames, labels-active 59, extras exactly
{lead-in, un-hidden halves}, annotation a **STRICT SUBSET** with zero orphans.

The veto category cross-checks against an independently recorded fact: on
`M27_OWNER_PLAYGATE_SMOKE` it names exactly the three vetoed targets against `vetoed_events = 3`.

🚨 **A CORRECTION TO MY OWN ANALYSIS THAT CHANGED THE ANSWER — `G161`.** The first pass joined
`annotation.json` to `labels.jsonl` on labels' **`frame_index`** and produced 14,399 bogus
"outside window" hits plus 15 annotation frames with no label row at all. **The correct key is
`session_index`;** `frame_index` is the arm-time `GFrameCounter` and indexes a different space.
**The checker was wrong, not the build** (`G142`'s shape).

### §5.2 The feature

**RED** = in `annotation.json` for that frame, a SHIPPED label. **AMBER** = in `labels.jsonl` but
not `annotation.json`, tagged `OUTSIDE-SUBSET` / `VETOED` / `NON-MANIFESTED` / `UNMATCHED`. Legend
burned into each image; watcher prints per-session counts by category with target names.

Sample counts, real sessions: delivery-mode 30 RED / 29 AMBER `OUTSIDE-SUBSET`; m27 play-gate
40 RED / 19 AMBER `VETOED`; fully-vetoed 0 RED / 59 AMBER `VETOED`.

### §5.3 Shipping

`overlay_watcher.py` (AnomDash) + `verify_capture.py` (**PLUGINFILE cross-repo entry** — ships from
where it lives rather than being duplicated). `Run.bat` auto-launches the watcher in its own
console beside the encoder watcher. Live progress streams; Pillow is checked at startup and prints
the exact pip line for the running interpreter, **proven by blocking the import**.

---

## §6 PACKAGING NO LONGER REQUIRES A PLUGIN REPO (AnomDash `21d9fae`)

🚨 **THE CROSS-REPO ENTRY BROKE PACKAGING ON THE MACHINE THAT ACTUALLY PACKAGES — `G163`.**
`make_delivery.py` derived the plugin repo from the dashboard repo's own location, assuming the two
trees sit side by side. On the owner's box the dashboard is at `D:\AnomDashboardV1\AnomDash` with
**no sibling plugin tree at all**, so it refused to build.

**The refusal was CORRECT and is unchanged. The assumption was wrong.** There is now **no derived
default**; cross-repo reach is **OPT-IN**:

- `--plugin-repo` **NOT given** ⇒ PLUGINFILE entries SKIPPED, bundle built from the dashboard repo
  alone, **exit 0**, and a closing `ACTION REQUIRED` notice names each omitted file, where it goes
  in the bundle and where to take it from. Success line reads
  `9/11 manifest entries present (dashboard-only; 2 plugin-side file(s) NOT included)` — **nobody
  can read "success" and infer completeness they do not have.**
- `--plugin-repo` **GIVEN** ⇒ resolved as before; a missing one **FAILS LOUDLY**, names the path
  and deletes the partial bundle. A non-existent `--plugin-repo` fails up front with its own message.

⛔ **NOT WEAKENED: `FILE`/`DIR` entries stay unconditional** — verified by removing
`host-tools/selfcheck.py` and watching the dashboard-only run still exit 2 with no bundle.
⛔ No directories created, no placeholders invented, no second copy in the dashboard repo.
**No plugin-side change was needed — the manifest lives in the dashboard repo. NO COOK REQUIRED.**

---

## §7 DETERMINISM, AND AN HONEST RED

Subset gate, test leg vs a two-leg control pair on the previous binary, A63 satisfied:

- **INVARIANT CORE: ALL IDENTICAL** (8 events, every `frame_indices` set, every `manifested` flag,
  type/subtype, video block, 11 `run_summary` fields incl. `end_frame 122`).
- `annotation.json` keyset unchanged. **Only non-labels extra: `run_summary/pattern_excluded_targets`
  — the one deliberately added field.**

⚠ **THE GATE EXITS 1 AND IT WAS NOT RELABELLED A PASS.** Its other 15 extras are all
`labels.jsonl` fields, and the cause was measured rather than assumed → **`G162`**:

```
CONTROL A vs CONTROL B : 4 positional row mismatches, 0 field diffs when sorted
CONTROL A vs TEST      : 8 positional row mismatches, 0 field diffs when sorted
```

**`labels.jsonl` ROW ORDER varies run to run** (async writer completion order) **and the CONTROL
PAIR EXHIBITS IT TOO** — it simply under-sampled it. Content is identical when compared by
`session_index`. This is a limitation of the gate's line-by-line comparison against a file whose
order is not deterministic — **verification tooling, not a build defect. Deliberately NOT fixed.**

⚠ **My own first check was order-BLIND** — PowerShell `Compare-Object` treats input as a SET and
reported "0 differing lines" while the gate compared positionally. The two disagreed until the
right instrument was used.

---

## §8 DOCS (`ed9092c`, `1821efc`)

`client-readme.md` gained: the overlay step (what red/amber mean, each amber tag in plain language,
and that it never alters frames or labels); Pillow in prerequisites, scoped honestly; the
`camera_clipping` note (**a whole-session global, so a first-person or held weapon sits inside the
near-clip radius for the ENTIRE capture and will appear clipped in every frame — expected, not a
defect**, owner-confirmed on Concorde); the amber-proportions table using the measured 90.1 / 9.9;
and **the `labels.jsonl` ordering subsection** — rows complete but NOT ordered, key or sort by
`session_index`, **never sort or join on `frame_index`**.

Verified on the DELIVERED file, not inferred: 90 rows, 90 distinct `session_index` covering 0..89
with none missing or duplicated, **2 descending steps proving it is not sorted**, and
`session_index 0 -> frame_00000.png`.

---

## §9 NOT DONE, NAMED

- ⛔ **NO TAG.** `m31` still open, still awaiting Concorde V-3/V-4.
- ⛔ The **ini route** for the two new `[AnomalyInjector]` keys is unproven here (`G88`); the
  console route and the shared resolve/echo path are proven. Concorde's cook proves the rest.
- ⛔ `lod_popping` at 200 cm fires **zero on this bench** — reported, not silently shipped; the
  value is the owner's call.
- ⛔ `G162` (`labels.jsonl` row order vs the subset gate's positional compare) filed, not fixed.
- ⛔ Intermediate commits were **symbol-checked, not compiled** — the split was designed so no
  commit references a symbol arriving later, and that was verified mechanically; only the tip was
  built.
- ⛔ `P6` did not move · `feature/stencil-capture` untouched at `76cac74` · no force-push · no
  ratio, no threshold proposed anywhere.

## §10 ENVIRONMENT

Staged bench exe left at the m32 candidate **`8F58661B`**. The as-found session-051 exe
**`DD76385F`** is archived and hash-verified at
`_binary_baselines\StackOBot.exe.session051-DD76385F`. Legs banked under `M32_*` and
`TICKPINPROBE_*`.
