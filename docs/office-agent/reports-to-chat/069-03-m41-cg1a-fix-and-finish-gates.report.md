# REPORT for 069-03-m41-cg1a-fix-and-finish-gates.md
brief: 069-03-m41-cg1a-fix-and-finish-gates.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-02 23:30:30 | finished: 2026-09-02 23:53:27 | duration: 23 min | exit code: 0
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\069-03-m41-cg1a-fix-and-finish-gates.live.txt

---- FINAL MESSAGE (Claude Code) ----
---

# SESSION 069, BRIEF 3 — C-G1a DIAGNOSED AND CLOSED; A-G1 NOT MET AS WRITTEN → STOPPED

## 1. C-G1a diagnosis: **FIXTURE — and a real CODE defect found while measuring it**

**Fixture half, measured first, by two independent reads that do not use the suspect scan:**

1. **`CB_GateLevel` authors no `APostProcessVolume`.** Its authoring script `make_gate_level.py` spawns only `StaticMeshActor` (the grid), one skeletal actor, `DirectionalLight`, `SkyLight`, `SkyAtmosphere`, `PointLight`, `PlayerStart`. **`V = 0` is the true answer there.**
2. **The camera blend cache only ever holds camera-MODIFIER pushes and is emptied every update** — `ApplyCameraModifiers` calls `ClearCachedPPBlends()` as its first statement (`PlayerCameraManager.cpp:281`); `AddCachedPPBlend` (`:300-305`) is called only from modifier code. The `-unattended` pawn is a `SpectatorPawn` with no camera component and runs no modifier. **`C = 0` is also true there.**

⇒ **`0/0/0` on `CB_GateLevel` is FIXTURE, not code.** Stated explicitly, as ruled. The result **stands in the record** as blindness-on-that-fixture; the predictions file was not edited (addendum route).

🚨 **But the same measurement exposed a genuine CODE defect.** The engine builds a view's post-process from **three** sources (`LocalPlayer.cpp:866-881`): volumes · the cached blends the engine itself labels *"CameraAnim override"* · and **`View->OverridePostProcessSettings(ViewInfo.PostProcessSettings, …)` under the comment `// CAMERA OVERRIDE`** — which is where a `UCameraComponent`'s `PostProcessSettings` actually arrive, the most ordinary way a host applies a full-screen effect. **`52f6430` scanned the first two and missed the third.** Fixed in `6b150f0`. ⚠ **This would have shipped behind a green gate had C-G1a been written without the `scanned` counts.**

**MainWorld, after the fix:** `scanned 1 volume(s), 1 camera blend(s), **0 blendable entr(ies)**, 0 material(s)`. The pre-declared branch said M=0 with V/C≥1 ⇒ code defect. I built the discriminator before attributing: **`blendable entries = 0` proves those settings carry no blendable at all**, so `M = 0` is content, **not a broken walk**. Not a code defect.

## 2. Gates — predicted vs measured

| gate | predicted | measured | verdict |
|---|---|---|---|
| **C-G1a(b)** probe | 3 targets: CustomDepth bit set · FinalImage PostProcessInput0 · opaque material none | `BufferVisualization/CustomDepth`, `/CustomStencil`, `/FinalImage` all **NOT PRESENT IN THIS CONTAINER**; `M_CorruptedTexture_Pink` → `domain=0 blendMode=0 sceneTextures=0` | ⚠ **PARTIAL** — negative control PASS, resolution + shader-map fetch PASS; **no positive bit observable in this container** (declared branch fired) |
| **C-G1a(a)** MainWorld | V≥1 or C≥1, **and M≥1**, readers 0 | V=1, C=1, **entries=0, M=0**, readers 0 | ⚠ **PREDICTION NOT MET; its failure branch REFUTED by measurement** — content, not code |
| **A-G1 / m40 L4-shape** | `labels.jsonl` **0 row diffs** vs the m40 control | **90/90 rows differ**; extras beyond the run-unique set = `t`, `frame_index`, `view`, `anomalies` | 🔴 **NOT MET AS WRITTEN** — see §5 |
| **A-G2(a)** | census ON from `COMPILED DEFAULT (on)` | exactly that, single source | ✅ PASS |
| **A-G2(a)** mask | (ruling 2) ini reading satisfies it | `mask ON … from DefaultGame.ini [AnomalyCapture] bMaskMeasureDefault` | ✅ PASS per ruling 2 |
| **A-G2(b)** | `off` from `IAI.Capture.Census (console)`, single string | `census off (requested off, from IAI.Capture.Census (console))` | ✅ PASS |
| **A-G3** | exactly 15 `census_*`, nothing else | ON 52 keys vs OFF 37 = **+15, all `census_*`, non-census extras NONE, removed NONE** | ✅ PASS |
| **A-G4** `P6` | added 0 / removed 0 | annotation keyset identical vs banked m40 | ✅ PASS |
| census-OFF inertness | no keys, no lines | **0** `census_*` keys · **0** per-fire lines · no HOST-PP line | ✅ PASS |
| **B-G1** | both directions, or honest absence | lever **REFUSED**: 3 candidate materials present, all `blendMode=0 translucent=0`; 2 not present. Nothing spawned | ⚠ **UNRUNNABLE HERE** per ruling 2 — rides the cook with C-G1b |
| **D-G1 A-side** | `expired>0`, `window=12` | 5/5 fires `expired=3/3`, `eligible=0`, `window=12`, `fires_fallback_all=**5**`, cycle 41–47 ticks | ✅ PASS — defect reproduced |
| **D-G1 B-side** | `expired=0` every fire, window≈cycle+8 | `window=**49**/**55**` (=cycle+8) ✅; `fires_fallback_all=**0**` ✅; eligible **10/15** vs 0/15 ✅; but `expired>0` on **3 of 5** fires | ⚠ **PASS on the decisive statistic, prediction miss on one conjunct** (§5) |
| **D-G2** | window 12 | 12–14 (cycles 4–6 ticks) | ✅ corrected prediction, as ruled |
| **E-G1** | unseen = 0 | **0** | ✅ PASS |
| **E-G2** | unseen ≈ half consulted | **7/7 fires `unseen=3/3`**; `census_fires_unseen_candidates=7`; lever omitted **39 of 77** | ✅ PASS — counter proven able to fire |
| **m38 run log** | closes cleanly | present + close marker on **all 9 legs** | ✅ PASS |
| **A44** both encodings | all new strings | `ProbeSceneTextureUsage` 0/7 · `CAMERA POV OVERRIDE` 0/1 · all others 0/≥3; negative control 0/0 | ✅ PASS |

## 3. SHAs, push, branch, exe

| | |
|---|---|
| `origin/master` | **`2543674`** — unchanged, predictions only, as instructed |
| branch `m41-CG1a-GATE-FAILED-do-not-merge` | `52f6430` → **`27c637f`** (addendum, pushed **before** the runs) → **`6b150f0`** (this turn's source). **All pushed.** |
| amend / ff-merge / branch delete | ⛔ **NOT DONE** — conditional on all gates passing |
| staged exe | **`5C073AC9`** (241,122,816 B), container unchanged (`2A66CA57`/`A7EF9B12`/`D8009AD7`), no cook |
| ⚠ not archived | `2BF9E1B9` and `7616F144` were overwritten in staging. **My gap, stated not hidden** — bounded: rebuildable from `52f6430`, and the only result on `2BF9E1B9` is banked in `M41_M41_ON_A`'s own log. Recorded in the baselines README. |

## 4. B-G1 material

**None — UNRUNNABLE HERE.** The lever's candidate table is the measurement: `M_MissingTexture_Checker`, `M_CorruptedTexture_Pink`, `WorldGridMaterial` all present and all `blendMode=0 translucent=0 writesCustomDepth=0`; `VertexColorMaterial` and `EditorBrushMaterial` not present. It refused, spawned nothing, and improvised no fixture.

## 5. Why I stopped, and what chat has to rule

**A-G1 is the blocker.** The honest picture:

- **Same-binary control pair** (two m41 census-OFF legs): the run-unique set is **`t_wall` alone**. `t`, `frame_index`, `view` and `anomalies` are all identical — a tighter envelope than this project has previously measured.
- **Cross-binary** (m41-OFF vs m40 control): extras are `t` (**one distinct delta, −0.164644**), `frame_index` (**one distinct delta, −4**), `view` (from `session_index` 36; `rot [0,0,0]` vs `[359.825,0,0]` — **0.175°**, the A47 rotation axis), `anomalies` (indices 3–82, pose-derived fields).
- **The selection is provably identical across all three legs**: same 6 events, same types, same targets, same spans, same `frame_indices` — `blink|StaticMeshActor_85|4|10` … `missing_texture|StaticMeshActor_49|75|82`, string-equal.

So P-C7's *substance* holds and the residual is a **constant 4-frame run-start offset plus a sub-degree settle-pose difference between two different binaries**. But the pre-declared gate says "0 row diffs", and **P30's ruling forbids widening the run-unique set to excuse a difference** — that is the laundering shape. I could have re-run the m40 control until it landed on the same start frame; **that is exactly the re-run-to-agreement route and I did not take it.** So I report it as NOT MET and stop.

**D-G1 B-side**, similarly: `fires_fallback_all` **5 → 0** and eligibility **0/15 → 10/15** on one binary with only the window semantics changed — decisive. But `expired = 0 on every fire` was predicted and 3 of 5 fires show 1–2. Named residual: at a synthetic 41–47-tick cycle the `LostAfterTicks = 8` margin does not cover cycle-to-cycle variance. Bench-only regime, produced by `CensusBatchCap 2`.

## 6. NEEDS-DECISION

1. **A-G1.** Rule one of: (i) **PASS on the invariant core** — event set, spans and `frame_indices` identical, cadence identical up to a constant offset — with the two confounds named in the record; (ii) **make the confound absent** by pinning the run-start frame (needs a harness change, not a re-run); (iii) genuinely **NOT MET**, m41 does not ship. ⛔ I will not choose this one myself — it is the gate the client default rests on.
2. **C-G1a's positive half.** Enumeration, resolution and the entries/materials discriminator are all proven; **no material in this container lights any `UsesSceneTexture` bit**, so a lit-bit control is unobtainable here. That is *exactly* C-G1b, already ruled to ride the cook. Confirm that folding it there closes C-G1a, or say what else would.
3. **D-G1 B-side's one conjunct** — accept as a corrected prediction with the residual named, or raise `LostAfterTicks`' margin for the window term (I recommend **not** tuning it on a synthetic bench regime).
4. **Docs commit + journal 069 §2 are still unwritten**, per the conditional in step (f)/(g).

**STOP.**
api-equivalent cost (not billed on Max): $19.02 | turns: 49 | running total across all briefs: $147.99
