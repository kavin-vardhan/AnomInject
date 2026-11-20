# START HERE — for the Claude agent on the office PC

You are the office agent. Read this file, then `OFFICE-AGENT-RULES.md`, before anything else.
Those rules override every line in `CLAUDE.md` (and anywhere else) that tells an agent to edit,
commit or push. This pack was assembled at home on 2026-11-20 (brief 096-01) from everything the
home side knew that was not already in the repo. It is a snapshot: the repo's `CLAUDE.md` status
block and journals move on; this file does not.

---

## 1. What the plugin is, in ten lines

1. `AnomalyInjector` is an Unreal Engine plugin (UE 5.1 canonical; a UE 4.25 port exists).
2. It injects **labelled visual anomalies** — graphics bugs on purpose — into a running game.
3. Anomaly types (the catalog in `docs/architecture.md` is authoritative): `blinking`, `missing_object`,
   `missing_texture`, `corrupted_texture`, `lod_corruption`, `lod_popping`, `lighting_mismatch`,
   `camera_clipping`, `stuck_low_mip`, `uv_corruption`, `normal_corruption`, and `time_dilation`.
   Which ones are in the auto-pool, and which are on by default, differs by build: check the catalog.
4. It captures the game's frames while it injects, and writes **ground-truth labels** for every frame:
   which anomaly, on which object, where on screen (box and per-pixel mask), and when.
5. The output is a training set for a machine-learning model that detects graphics bugs.
6. It is **game-agnostic**: engine modules only, never a host game's types. It must work on any game.
7. The customer is an external **client** who runs the plugin inside their own games (the "hosts")
   and receives datasets. They review the data and send feedback; that feedback drives milestones.
8. Controls: `IAI.*` console commands, a WebSocket control server, and a desktop dashboard
   (separate repo, "AnomDash"). Bench-only levers are `IAI.Bench.*` and need `-IAIBench`.
9. Outputs per capture session: PNG frames, `annotation.json` (client contract), `labels.jsonl`
   (per-frame rows), `run_summary.json`, masks, optional run log. Delivery mode trims to the client set.
10. The whole project's history is one long fight to make the labels say exactly what the pixels show.

## 2. The development path, M0 to today (one lesson each)

| step | what | the lesson it taught |
|---|---|---|
| M0–M1 | hide one actor; anomaly interface + registry | a world subsystem, Game+PIE only; match by name/class, never by editor label |
| M2–M3 | lighting, LOD, camera clipping; UE 5.4 → 5.1 port | a capture-baseline revert contract (restore what was there, not a guess) |
| m4–m6 | viewport visibility, object selector, automatic injection | one seeded random stream with a fixed draw protocol, or runs cannot be reproduced |
| m7 | labelled frame capture + 2D boxes | labels are the injector's own ground truth, stamped in the same tick as the frame |
| m8 | `missing_texture` (first shipped content asset) | the cook runs on EDITOR binaries (G47); swapped materials need every mesh usage flag (G49) |
| m9–m12 | session capture, targeted capture, pacing + honest fps stamp, delivery mode | the game clock and the wall clock differ; never stamp a frame rate the content did not have |
| m14–m16 | content clock (wall default), focus-gated start, token | the client's titles are wall-clock; a default must name where it came from |
| m17 | `missing_texture` revert on a runtime modular character (Concorde) | a host can re-create components mid-hold; revert must re-find the live component and say what it did |
| m18–m21 | label alignment, packaged preview, annotation fixes, arm→present pairing | validate in a PACKAGED build (G76); PIE hid every one of these; pairing must be by identity, not by order |
| m22 | blink subtype pin, traceability fields, selection provenance | report what selected an object, separately from the selection itself |
| S2–S3 / m24–m25 | capture moved to a scene-view-extension (SVE) path; UI-free frames by default | read the frame being rendered, so there is nothing to mis-pair; prove a lever reaches the process (G114) |
| H4/H5 → m26–m27 | invisible anomalies: occluded targets, types that draw nothing; the mask veto | a TYPE test is not a DRAWING test; veto only a MEASURED zero, never an unmeasured one |
| m28–m30 | output resolution, `corrupted_texture`, LOD proximity gate, `camera_clipping` | a black frame and a null result are the same number (G151): luma-check before measuring |
| m31 | the SVE path produced zero frames on Concorde | first defect found by a second host: certification depth on one host says nothing about another |
| m33–m35 | speed-ratio re-key, GPU mask reduce, readback crash on Bates (letterboxed view) | engine staging layouts differ by version; make the layout irrelevant by construction |
| m36–m41 | selection census (measured drawn pixels), run log, order-independent label sampling (Bates "P9") | a host can tick subsystems in a different order; sample labels at world-tick end, not mid-tick |
| m43–m46 | target ID mask, tag ownership, hidden-class masks, internal view rect | an actor under a live fire belongs to its event; passes must map through the internal view rect |
| m47–m50 | shader prewarm + black-frame gate, exposure-dip mark, per-frame observability, tag-pool headroom | `target_drawn_pixels` is evidence of absence only (G258); never force the game's exposure |
| m51 / 079 | release hardening (Codex-implemented); the label-vs-pixel verifier | a verifier that cannot fail is not a verifier; `CONSISTENT` does not confirm a label |
| m52–m55 | `stuck_low_mip`, UV/normal corruption, change evidence | `UTexture::LODBias` is ignored in a cooked build (G265); the cook strips texture LOD bias (G455) |
| release 2026-10-05 | the delivered M3 build (`master` = `release/2026-10-05` + docs) | temporal transition defaults set to 0/0/0 by the owner's choice |
| 092–093 | virtual-texture research; natural causes; follow-up capture work (local only) | research before code; much of 093 sits on a local branch (see `local-refs/`) |
| 094 | UE 4.25 port for the Europa host (`feat/europa-port`) | 4.25 differs in capture hooks, custom depth leaks into host outlines, office kit races |
| 095 | client review → m56 capture continuity, m57 LOD spatial trigger, m58 terrain blink subtype, m59 forced-LOD refusals | written and compiled at home, not yet validated on any engine run |

## 3. Current state (2026-11-20)

**Branches that matter**
- `master` (`ebc34ed`) = the delivered release (`release/2026-10-05`, `d7eee97`) plus docs. This is what
  the client has (their "M3", UE 5.1).
- `integ/m56-m58` (`0c71f6f`) = master + m56 + m58 + m59 merged. **The pack branch was cut from here.**
- `feat/m57-lod-spatial-trigger` = m57 on its own branch (reviewed, fixes accepted, not merged into integ).
- `fix/m56-capture-continuity`, `feat/m58-terrain-blink-subtype`, `fix/m59-forced-lod-refusals` = the unit branches.
- `feat/europa-port` (`2828f6a`) = the UE 4.25 port for Europa; the last office card names this commit.
- Local-only branches with knowledge you cannot pull (`m51`, `fix/followup-capture`, two more) — their
  documents are copied into `local-refs/`.

**Delivered:** the M3 release on UE 5.1 (`master`); the Europa 4.25 port at `2828f6a` with its office card.

**Written but NOT validated (no game build, no cook, no engine run):**
- **m56 capture continuity** — settle ticks after every onset/offset are captured and flagged
  `transition_reason: settle`; rows gain `tick_index`; `annotation.json` schema 2.2; dropped-frame accounting.
- **m57 `lod_popping` spatial trigger** — pops as the player approaches/views an object, instead of on a timer.
- **m58 terrain blink subtype** — `blinking` on terrain reads `blink / terrain_disappear_reappear`;
  terrain = engine landscape by class or a host name pattern in `[AnomalyInjector] TerrainNamePatterns`
  (default empty — **set per host at the office**). Blinking engine landscape is refused (`terrain_unlabelable`).
- **m59 forced-LOD refusals** — `lod_corruption`/`lod_popping` refuse targets where a forced LOD shows
  nothing different (instanced-foliage proxies ignore forced LOD; single usable LOD; not coarser than now);
  skeletal targets refused unless `IAI.Anomaly.LodAllowSkeletal 1`.

**Waiting on the home bench:** a Game build, a cook and the frozen engine legs for m56–m59 (brief
095-07, held while the home PC is busy and its E: drive is below the 50 GiB build floor). Until those
legs pass at home, office testing of m56–m59 is EXPERIMENT-grade at best.

## 4. The three hosts (codenames only — never a real title, studio or fork)

- **Concorde** — UE 5.1 **source-built fork** with a **decoupled fixed game tick / variable render tick**.
  Wall-clock content. First host that was not the home bench: the SVE capture path produced zero frames
  there (m31, fixed by plugin-owned serials). A runtime modular character re-creates its mesh components
  during play (m17). Project setting had Nanite support disabled (so Nanite-blindness, G134, was inert
  there). Material usage flags mattered (G157). `speed_ratio` is vacuous on its pinned fork (read health
  from wall-clock math). A capture-time tick pin exists for its fork (build-time content probe).
- **Bates** — UE 5.1-lineage game ("Deimos" was an old second name for it; retired). Letterboxed view
  (non-zero view-rect origin) crashed the readback (m35, fixed). Ticks subsystems in the opposite order to
  the bench, which shifted `blinking` labels by a frame (P9; fixed by m40 and validated there). Runs in
  delivery mode (so the m38 run log is auto-off unless forced). Low anomaly yield cured by the census
  floor 0.5. Office cards and readings for it are in `docs/office-rdp-card.md`.
- **Europa** — the **UE 4.25** host; the port is `feat/europa-port`. Its home stand-in is a marketplace
  street scene. Host material outlines read custom depth/stencil, so our tags can leak into saved frames
  (094-41); capture runs before the UI on 4.25; the 4.25 texture materials were authored for the port.
  The owner picked "Europa" knowing the office uses the same word; never write the office's own codenames.

Home fixtures that need no codename: StackOBot (5.1 bench), Lyra (5.1 second fixture), the 4.25 street scene.

## 5. Reading map — which document answers what

| question | read |
|---|---|
| What is in the code now, every anomaly, every module? | `docs/architecture.md` (anomaly catalog included) |
| Why does X behave strangely? | `docs/gotchas.md` (G1 … G581, append-only; search the number) |
| How do I build, cook, stage, run a leg? | `docs/setup-runbook.md` (§8 packaged legs, §8.6 full cook) |
| How does capture timing / fps / pacing work? | `docs/capture-fps.md` |
| What does each label field mean? Schema versions? | `docs/client-readme.md` (§8 field tables, changelog) |
| How to check labels against pixels | `tools/label_sync_check.py` + `tools/OFFICE-CHECK.md` (the check kit); `tools/verify_capture.py`; `docs/verifier-characterisation.md` |
| Invisible-anomaly causes (occluded, non-drawing, Nanite) | `docs/invisible-anomaly-mechanisms.md` |
| What to check before a delivery | `docs/PRE-DELIVERY-CHECKLIST.md`, `docs/client-delivery.md` |
| Office procedures written so far | `docs/office-rdp-card.md`; office cards in `chat-rulings/` (`*office-card*`, `*read-card*`) |
| Europa port details | `docs/europa/`, the 094 journals, `chat-rulings/094-*` |
| What happened in session N? | `docs/sessions/` (one journal per session), `docs/predictions/` |
| What did chat rule, and why? | `chat-rulings/INDEX.md` |
| What did a run predict before it ran? | `runs/INDEX.md`, `docs/predictions/` |
| What exactly was a brief, and its report? | `briefs/INDEX.md` + `reports-to-chat/INDEX.md` |
| How the bench gates work | `bench/BENCH.md` |
| What the home agent remembered | `home-agent-memory/INDEX.md` |
| Knowledge on unpushed branches | `local-refs/INDEX.md` |

**The 20 gotchas most likely to matter on a real host** (read these in `docs/gotchas.md` first):

| G | one line |
|---|---|
| G76 | validate against a local PACKAGED build, not just PIE |
| G88 | a loose `Config/*.ini` next to a package is ignored; the cooked config wins |
| G119 | a check on a baked artifact's SOURCE validates a copy the running system never reads — read it back from the running game |
| G47 | the cook runs on EDITOR binaries: rebuild the editor target before cooking |
| G92 | a packaged build can be compiled and never staged; the archive keeps the old exe |
| G121 | the exe hash does not identify the build; build identity = exe + cooked container |
| G221 | a monolithic packaged build cannot see a missing `MODULE_API`; the editor target can |
| G49 | a material swapped onto arbitrary meshes needs every mesh usage flag, or renders default grey |
| G157 | a project setting can make a defect unreproducible on one host while clean logs look healthy |
| G134 | Nanite cannot write custom depth in 5.1: masks are blind to Nanite targets |
| G265 | `UTexture::LODBias` is ignored on a cooked platform |
| G455 | in a packaged build per-texture/group LOD bias is applied by the cook |
| G439 | in the editor a material's "shader map complete" flag stays 0 while it draws |
| G79 | viewport `ReadPixels` is black in a packaged build |
| G114 | a packaged UE game is DPI-unaware; a lever that does nothing gives a clean null |
| G139 | a default that can come from an ini must echo its effective value AND provenance |
| G237 | a pre/post snapshot cannot attribute a difference; a host that writes the same state looks like our fault |
| G120 | an unverified mechanism attached to an observation forecloses routes that were never closed |
| G96 | an instrument's blindness only shows on known-answer controls: prove it can fail |
| G579 | a forced LOD is not a visible LOD: instanced proxies ignore it, skinned meshes read it a frame late (see also G581) |

Also worth knowing early: G161 (join on `session_index`, never `frame_index`), G225 (internal view rect),
G228 (~9 % cross-run picture floor at the delivered config), G258 (drawn pixels are evidence of absence only),
G431 (UBT `-Mode=JsonExport` lists modules without compiling).

## 6. Glossary of our own terms

- **anomaly / fire / burst / event** — an anomaly type is applied ("fired") to a target for a burst of
  frames; each fire becomes one **event** in `annotation.json`.
- **positive frame** — a frame labelled as containing the anomaly. **hide-class** = `blinking`,
  `missing_object` (target dropped from rendering); **FireWindow** types label the whole active window.
- **`annotation.json`** — the client's per-session contract (events, frame lists, boxes, schema version).
  **`labels.jsonl`** — one row per captured frame. **`run_summary.json`** — counters. **`run.json`** — seed/config.
- **`session_index` vs `frame_index`** — the PNG number vs the engine frame counter at arm time; join on `session_index`.
- **delivery mode** — writes only the client file set (frames, annotation, run summary, labels).
- **SVE path / backbuffer path** — capture grab points: scene-colour via a scene view extension (default,
  UI-free) vs the presented backbuffer (UI-on, the bisect switch `IAI.Capture.SVE 0`).
- **settle frames / transition flag** — frames next to an onset/offset that may show partial change; flagged so
  training can drop them (`transition_reason`).
- **mask / stencil tag** — per-frame target ID mask from custom depth/stencil (values 200–254).
- **census** — measures each candidate's drawn pixels before selection; **veto** (m26) — removes an event whose
  target was MEASURED to draw zero pixels. **observable / `target_pixels`** — per-frame visibility of the target.
- **auto-pool / targeted** — random pick among enabled anomalies vs one named anomaly on one named actor.
- **poll radius / screen-coverage cull** — selection limits (distance from the player; minimum on-screen share).
- **speed_ratio / content clock / pacing / tick pin** — capture timing health and controls (see `docs/capture-fps.md`).
- **G-number** — a gotcha; **P-number** — a phenomenon (P1 … P9); **H-number** — a hypothesis; **A-number** — a
  standing method rule; **m-number** — a milestone (identities, not an order).
- **leg** — one scripted capture run on the bench; **bench / fixture** — the home test setup (`CB_GateLevel`
  in StackOBot, Lyra, the 4.25 street scene); **arbiter** — the AA-off configuration used for pixel comparisons.
- **can-fail lever** — a bench switch that deliberately breaks the thing under test, proving a gate can fail.
- **predictions / predeclared** — what a run must show, frozen in writing BEFORE it runs; amendments are dated.
- **verdict line** — the first line of a final report: `VERDICT: GREEN | NEEDS-DECISION | RED`.
- **office card** — a step-by-step procedure for the owner at the office (codenames only).
- **check kit** — `tools/label_sync_check.py`, the client-shippable label-vs-pixel checker.
- **A44 scan** — read new strings back out of the staged binary (both encodings) to prove a change is in it.
- **quartet** — build identity: exe hash + the cooked container files.
- **hot-swap** — staging a code-only change by copying the exe (no cook). A shader change needs a full cook (G129).
- **Codex / relay** — a second AI implementer/reviewer driven at home through `_relay` scripts.

## 7. What is held back from this pack, and why

Held for a chat or owner ruling (never committed): every owner transcription of office readings
(`_bates_reads`, one owner write-up of office results); every file carrying the client's own words or data
(client feedback hand-offs, a ruling quoting the client's review topics, a note about the client's dataset,
the client ticket list); every file where the codename scrub verifier found a real title/studio/fork term
(12 files, one of which carries the term in its file name). The home agent's report for 096-01 lists them.
