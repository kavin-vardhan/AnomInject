# 095-04 office card — setting a host's terrain pattern for m58 (terrain blinks)

**For:** the owner, at the office, on a host build that contains m58 (branch `feat/m58-terrain-blink-subtype`, once it is
merged and pulled — it is not merged yet). **Takes:** about 15 minutes. **Nothing from this card comes home except the
yes/no and number answers in step 6.** The pattern word itself, asset names and actor names stay on the host.

## What this is for

After m58, a blink that hides terrain is labelled `blink` / `terrain_disappear_reappear` instead of
`disappear_reappear`. The plugin recognises Unreal landscape by itself. Most games build their ground from ordinary
meshes or Blueprints, and for those you tell the plugin one word from their names. The list is empty until you do, so
until then those ground blinks keep the old `disappear_reappear` label.

## 1. Find a terrain blink in a session

Use any session from this host where a blink visibly hid part of the ground: you saw it in the video, or the overlay
inspector boxed the ground. Open its `annotation.json` and find that event: `anomaly_type` is `blink`, and its
`affected_frames` cover the frames where the ground vanished.

## 2. Read the event's own label fields

In that event, look at `affected_objects.nodes[0]`:

- `component_class` — **first, the yes/no read chat asked for:** is it a landscape or grass class (it contains
  `Landscape` or `Grass`), or a mesh class (`StaticMeshComponent`, `InstancedStaticMeshComponent`, and so on)?
  - **Landscape or grass:** this host's ground is Unreal landscape. You need no pattern; the plugin recognises it. Note
    that m58 now **refuses** to blink engine landscape (it could not be labelled properly), so these events will stop
    appearing. Write down "landscape", skip to step 6.
  - **A mesh class:** carry on.
- `asset_name` — the mesh the ground piece uses.
- `name` — the object's name in the level. `path` ends with the same name; for a Blueprint it often starts with the
  Blueprint's class name.

## 3. Choose one word

Pick a word that **every ground piece shares and no prop has**. Look at two or three terrain blink events if you can.

- **Best:** a word from `asset_name` that all ground meshes share.
- **Next:** a word from the Blueprint or object name in `name` or `path`.
- **Never:** `StaticMeshComponent`, `Mesh`, `Actor`, `Component`, or any other word that ordinary props also carry. A
  pattern matches anywhere in the name, ignoring case, so a short common word labels props as terrain.

## 4. Try it for one session (no re-cook)

In the game console, **before** `IAI.Capture.Start`:

```
IAI.SetTerrainPatterns <your word>
```

The console answers `IAI.SetTerrainPatterns: EFFECTIVE READ-BACK = 1(console)[<your word>]`. Run a short capture with
blinks, then read its log:

- The run banner, printed when the run starts:
  `Capture(m58): TERRAIN RULES class=ACTIVE[...] | patterns=1(console)[<your word>] source=IAI.SetTerrainPatterns (...) | refusal=ON(...)`
- One line per terrain blink: `TERRAIN-SUBTYPE event=... target=<object> rule=pattern:<your word>@<field>=<value>`.
  **Check every `target=` here is ground, not a prop.** If a prop shows up, choose a longer word and repeat.
- At the end: `Capture(m58): TERRAIN SUMMARY blink_terrain_events=<n> ...`, and the same number in `run_summary.json`.

In that session's `annotation.json`, the ground blinks now read `"anomaly_subtype": "terrain_disappear_reappear"`.

## 5. Make it permanent

Add the word to the **host project's** `Config/DefaultGame.ini`:

```ini
[AnomalyInjector]
+TerrainNamePatterns=<your word>
```

More than one word is allowed, one `+TerrainNamePatterns=` line each. Then **re-cook / re-package**: an edited ini placed
next to a packaged game does nothing; only the cooked config counts.

**Confirm on the new build** with one capture. The banner must read
`patterns=1(ini)[<your word>] source=DefaultGame.ini [AnomalyInjector] TerrainNamePatterns`.

- If it reads `patterns=none(COMPILED DEFAULT; no ini key)`, the key did not reach the build: check the section name and
  re-cook.
- If the banner ever reads `class=INERT[...]`, the plugin could not find Unreal's landscape classes on this build. Report
  it.

## 6. What comes home (numbers and yes/no only)

1. Terrain on this host is: landscape / meshes (the step 2 read).
2. Pattern set in the host ini: yes / no. Banner shows it with source `DefaultGame.ini`: yes / no.
3. Banner `class=` reads ACTIVE: yes / no.
4. On the confirming capture: `blink_terrain_events` = <n>, and props wrongly labelled terrain = <n>.

**Do not bring home the word, any asset or object name, or any session file.**
