# START HERE — for the Claude agent on the office PC

You are the office agent. Read this file, then `OFFICE-AGENT-RULES.md`, before anything else.
Those rules override every line in `CLAUDE.md` (and anywhere else) that tells an agent to edit,
commit, push or run git. This pack was assembled at home on 2026-11-20 and 2026-11-21 (briefs 096-01 to 096-01c, ruling 096-01) from
everything the home side knew that was not already in the repo. It is a snapshot: the repo's
`CLAUDE.md` status block and journals move on; this file only moves when the home side refreshes the
pack. Copies carry placeholders and are not byte-identical to their home sources: see `REDACTIONS.md`.

**Where things are at the office** (the owner's workflow of 2026-11-22, brief 096-03):
- This pack is the folder `C:\OfficeAgent\pack`. The owner copies it from `docs/office-agent/` of the
  branch `docs/office-pack` and replaces it whole when chat says it changed. Your briefs arrive in its
  `briefs-to-office\` (read its `README.md`); the owner's steps are `OFFICE-SETUP.md`; his opening
  message is `FIRST-MESSAGE.md`. Your journal is `C:\OfficeAgent\office-journal`.
- You are started inside a game project. The plugin under test is the game's copy,
  `Plugins\AnomalyInjector`, at the commit written in its `COPIED_COMMIT.txt`. The owner checks out
  and copies; you never use git.
- Repo documents named in this file (`docs/...`, `tools/...`) are not in the pack. Read them in the
  game's plugin copy, at its version: a version older than a document does not have it. The repo's
  `CLAUDE.md` is the exception: it is the home agent's instruction file, the owner leaves it out of the
  copy, and if it is there you do not read it (`OFFICE-AGENT-RULES.md`).
  The pack's own folders (`chat-rulings/`, `briefs/`, `office-tools/`, …) are in `C:\OfficeAgent\pack`.
- The office setup in ruling 096-01 D5 (two worktrees, an `OFFICE_HOST` marker, the agent pulling) is
  superseded by this workflow. Documents in the archive that describe it are history.

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
| 095 | client review → m56 capture continuity, m57 LOD spatial trigger, m58 terrain blink subtype, m59 forced-LOD refusals | written, reviewed and compiled at home; none has run in an engine yet |
| 096 | this pack, and an office agent on the real hosts | the home side cannot see the hosts; the office agent can, and it reports only through the owner's photos |

## 3. Current state (as of 2026-11-20; each fact names its date and source)

**Branches that matter** (tips read with `git` on 2026-11-20; all are on `origin`)
- `master` = `ebc34ed` (2026-11-13) = the delivered release `release/2026-10-05` (`d7eee97`, the
  client's "M3", UE 5.1) plus one docs commit (ruling 095-00 M1).
- `integ/m56-m58` = `0c71f6f` (2026-11-14) = master + m56 + m58 + m59 merged. Editor-compiled with
  0 errors and 0 warnings, pure C++ selftests green (095-12, 2026-11-14). This is the line the first
  engine run gates and ships.
- `feat/m57-lod-spatial-trigger` = `d8d3a7e` (2026-11-14). Its review fixes are accepted (ruling 095-13,
  2026-11-14) and the prep for the first engine run is applied (095-14, 2026-11-14:
  `IAI.Anomaly.LodDownPop` default 0, marked experimental). It contains `integ/m56-m58` `0c71f6f`;
  it is **not** merged into `integ/m56-m58`.
- Unit branches: `fix/m56-capture-continuity` `e12180c`, `feat/m58-terrain-blink-subtype` `5e1bc03`,
  `fix/m59-forced-lod-refusals` `d23ab16` (all 2026-11-14).
- `feat/europa-port` = `2828f6a` (2026-11-06, 094-48d) = the UE 4.25 port for Europa; the last Europa
  office card names this commit.
- `docs/office-pack` = this pack, on its own long-lived branch (ruling 096-01 D5, 2026-11-20). It is
  never merged into a shipping line; the home side refreshes it by merging the newest shipping line in.
- Local-only branches, not on origin (`m51`, `fix/followup-capture`, two more): their documents are
  copied into `local-refs/`.

**Delivered:** the M3 release on UE 5.1 (`master`, 2026-10-05); the Europa 4.25 port at `2828f6a`
with its office card (2026-11-06).

**Written, reviewed and compiled, but NOT run in any engine** (no Game build, no cook, no capture leg)
as of 2026-11-20 — none of m56, m57, m58, m59:
- **m56 capture continuity** — settle ticks after every onset/offset are captured and flagged
  `transition_reason: settle`; rows gain `tick_index`; `annotation.json` schema 2.2; dropped-frame accounting.
- **m57 `lod_popping` spatial trigger** — the pop follows the player instead of a timer: a distance
  crossing, or a target turning into view (view entry: shown at low detail for about 30 frames, then it
  snaps; ruling 095-11 addendum and 095-13, 2026-11-14). The timer version leaves the default auto pool.
- **m58 terrain blink subtype** — `blinking` on terrain reads `blink / terrain_disappear_reappear`;
  terrain = engine landscape by class or a host name pattern in `[AnomalyInjector] TerrainNamePatterns`
  (default empty — **set per host at the office**). Blinking engine landscape is refused (`terrain_unlabelable`).
- **m59 forced-LOD refusals** — `lod_corruption`/`lod_popping` refuse targets where a forced LOD shows
  nothing different (instanced-foliage proxies ignore forced LOD; single usable LOD; not coarser than now);
  skeletal targets refused unless `IAI.Anomaly.LodAllowSkeletal 1`.

**The first engine run** (brief 095-07b, heavy: a Game build, one cook, every gate for m56 to m59 and the
settle-exactness table; the m57 clips for the owner follow it) is written and **held until the owner's home PC is
free of his renders** (ruling 095-07, 2026-11-14: the run stopped at its memory pre-flight while his
render loops were running; disk was not the problem that day, E: had 116.5 GiB free). On that date the
home host's Editor DLLs matched `integ/m56-m58` `0c71f6f`, its Game exe was a 24 September build, and
no cook existed (ruling 095-07).

**What that means at the office:** the plugin in each game is the delivered release unless a brief says
otherwise and the owner has copied that version in (its `COPIED_COMMIT.txt` says which). A reading taken
with the game's plugin copy at `integ/m56-m58` or `feat/m57`, and the copy untouched, is a valid report
(rules, item 3), and it would be the first engine reading those units have ever had: report surprises as
findings, with the commit.

## 4. The office hosts (codenames only — never a real title, studio or fork)

There are **three hosts: Concorde, Bates and Europa** (settled by the owner, 2026-11-21). There is no
fourth host. Each fact carries the date it was recorded and where. It is what the home side last knew; a host may
have changed since. Re-check anything you rely on, and report what you find.

**Concorde** — a UE 5.1 source-built engine fork whose game loop runs a fixed game tick with a variable
render tick.
- 2026-07-13 (m15, journal `2026-07-13-021`): its content runs on the wall clock, so the plugin's
  content-clock default is `wall`.
- 2026-07-15 (m17, journal `2026-07-15-023`): a runtime modular character re-creates its body mesh
  components during play; the `missing_texture` revert had to re-find the live component. Confirmed
  fixed on the real component by the owner.
- 2026-08-20 (`docs/invisible-anomaly-mechanisms.md`, the H6 entry): the project had Nanite support
  disabled, so Nanite-blindness (G134) was inert there, provided `r.Nanite.ProxyRenderMode` stays 0.
- 2026-08-21 (m31, journals `2026-08-21-049` and `-050`): the SVE capture path produced zero frames
  there, the first defect found by a second host; fixed by plugin-owned serials.
- 2026-08-21 (journal `2026-08-21-051`, G157): the swapped materials lacked a usage flag its static
  lighting needs, so statically lit static meshes drew the default material while labelled.
- 2026-08-22 (journal `2026-08-22-052`): the capture-time tick pin was validated there, packaged:
  label offsets +0; `ticks_per_captured_frame` 1.2000 pinned against 1.2699 unpinned. The fork changed
  engine Core, which is why a file-name probe for it missed.
- 2026-08-23 (journal `2026-08-23-056`): on its pinned loop the game clock follows the wall
  clock, so the old `speed_ratio` read about 1.000 by construction; m33 re-keyed it to the plugin's own
  tick span. Read capture health there from wall-clock arithmetic.

**Bates** — a UE 5.1-lineage game.
- 2026-08-26 (m35, journal `2026-08-26-061`): a letterboxed view (non-zero view-rect origin) crashed
  the capture readback; fixed by m35 and validated on Bates on 2026-09-02 (journal `2026-09-02-066`,
  rect origins 69 and 138).
- 2026-09-02 (journal `2026-09-02-066`): census floor 6.0 left 2 eligible targets; floor 0.5 gave 8,
  about 90 % of anomalies visible, 0 vetoed.
- 2026-09-02 (journal `2026-09-02-068`; `docs/office-rdp-card.md` Section D): it ticks the plugin's
  subsystems in a different order from the bench, which shifted `blinking` labels by one frame (P9);
  m40 fixed that and was validated there.
- 2026-09-02 (G210): its project runs delivery mode, so the run log is off unless forced with
  `IAI.Capture.RunLog 1`.
- 2026-09-02: `ticks_per_captured_frame` read 1.3556. It fits the settle skip that m56 removes; not
  proven for that host (ruling 095-00 §8).
- 2026-10-01 (journal `2026-10-01-090-10b`, written as "the second office host"): played in the editor
  (PIE), every texture-corruption fire was refused because the editor's shader-map flag stays 0; fixed
  in m53. 2026-10-02 (`chat-rulings/bates-mip-2026-10-02-triage.md`): UV and normal targets were then
  picked up on Bates.

**Older documents, read with care.** Some documents from 2026-09-29 to 2026-10-03 speak of a "first"
and a "second" office host, or set two builds or workspaces of one host side by side as if they were
two hosts (on 2026-09-29 one Bates build compiled clean while another failed with missing includes).
Read them as builds or workspaces of the three hosts above. Where it matters which host a reading
came from, re-check at the office. Copies in this pack write every host by its codename only
(`REDACTIONS.md`).

**Europa** — the UE 4.25 host. Port: `feat/europa-port`.
- 2026-11-01 (report 094-33): on 4.25 the plugin captures the frame before the game UI is drawn.
- 2026-11-02 (report 094-41, from the owner's office photos): the 4.25 capture switched on custom depth
  for the objects it measured, and the host's own outline effect drew around them on the saved frames,
  so labels leaked into the pictures. Office frames of that time are not clean training data. The fix
  line runs through reports 094-42 to 094-48.
- 2026-11-04 (report 094-44): the texture-anomaly materials were authored for 4.25 (the 5.1 assets do
  not load there).
- 2026-11-06 (report 094-48d): `2828f6a` pushed; the last office card names it.
- Its home stand-in is a marketplace street scene. "Europa" was the owner's own pick of codename
  (`handoffs/CHAT-HANDOFF-m44-m46-shipped-europa-phase0.md`).

Home fixtures that need no codename: StackOBot (5.1 bench), Lyra (5.1 second fixture), the 4.25 street scene.

## 5. Reading map — which document answers what

| question | read |
|---|---|
| What is in the code now, every anomaly, every module? | `docs/architecture.md` (anomaly catalog included) |
| Why does X behave strangely? | `docs/gotchas.md` (G1 … G581 on `integ/m56-m58`; G582 and G583 are on `feat/m57-lod-spatial-trigger`; append-only; search the number) |
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
| How a bench harness measured something | `harness/INDEX.md` (the `_reviews` scripts as text) |
| What a placeholder stands for; what is not in this pack | `REDACTIONS.md` |
| Your next brief | `briefs-to-office/` (`README.md` first) |
| How the owner set this PC up, and how he copies the pack and a plugin version | `OFFICE-SETUP.md` |
| The owner's opening message to a new session | `FIRST-MESSAGE.md` |

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
- **plugin copy / `COPIED_COMMIT.txt` / copy untouched** — at the office, the game's
  `Plugins\AnomalyInjector`, copied in by the owner at the commit written in its `COPIED_COMMIT.txt`;
  "untouched" means no experiment was applied when a reading was taken.
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

## 7. What is not in this pack

See `REDACTIONS.md`. In short: the owner's transcriptions of office readings and the client material he
has not released are not here (you can read the originals at the office); another home project's
documents, binaries, frames and data are not here. Twelve files that carried a real-name term are in
after a codename edit, and chat's rulings 095-00 and 095-11 and the 095 hand-off are in with the
client's quoted wording paraphrased (ruling 096-01, D2).
