# REPORT for 090-10f2-finish-e-mask-uniform-bscope-cooked.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 090-10f2-finish-e-mask-uniform-bscope-cooked.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-10-02 10:04:08 | finished: 2026-10-02 16:23:59 | duration: 380 min | exit code: 0
context at last turn: 392580 tokens (39% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\090-10f2-finish-e-mask-uniform-bscope-cooked.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

**Owner summary**
1. Pull `feat/m53-uv-normal-corruption` at **`b8cd3cd`**. Its code is the same as `2dbf6f2`, and the exe is `36C8E30A`, the build every confirmation leg ran on.
2. In PIE, labelled frames no longer ship without their mask. uv/normal corruption now refuses textures that cannot visibly change, so the office-like auto-pool is 28 of 28 visibly corrupt (it was 17 of 28). On objects with several parts, the mask and box now cover only the part that was corrupted.
3. One thing needs a ruling. A packaged build bakes per-texture and texture-group LOD bias in during the cook, so the "old build must refuse" test cannot happen there for those two. It only happens for cinematic mips, and that one passed.

**Part 0 and Part M**
- **Part 0:** 090-10e is finished: fix-branch strict pass 0/0, lever audit, string scan, archives, journal, status blocks and G450. The readme now says `pie_end_settle` is a precaution, not a correction. The "PIE end +1" was the edge reader using `affected_frames`; against the labelled set, 0 of 147 events end late. It was pushed as fix `ba1af33` and m53 `48a63a2`.
- **Part M cause:**
  - One Tonemap mask render served every pending arm.
  - In PIE warm-up the game thread had already armed frame N+1 when frame N's pass ran (log: `MASK-SERVED captureArms=2`).
  - Only the first arm gets pixels, so frame N+1's labelled row shipped `mask_state unmeasured` with no PNG. Had it got the pixels, they would have been frame N's.
- **Fix, not a flag** (`6f9ada2`, plus the include fix `7d4debc`, G451):
  - Each arm carries a sequence number.
  - At `BeginRenderViewFamily` the family gets that frame's last sequence number as a bound, and the pass serves only arms up to it.
  - Self-test 15/15; three mutants fail.
- **Before and after:**

| build | labelled frames without a measured mask |
|---|---|
| old build `EBEED6F4`, PIE | 55 of 672 |
| fixed builds, PIE | 0 of 672 |
| old binary `65607703`, staged (this session) | 3 of 320 |
| fixed build, staged | 0 of 936 |

- In a packaged game the race is rare, not absent. No delivered client session is on this box, so I could not read delivered data; the staged rate is the only stand-in.

**Item 1: `texture_uniform` (new refusal rank A7)**
- **Rule:** a slot is refused when every texture it needs is uniform on the resident top mip, judged for the mode actually drawn:
  - `tile` / `scramble`: `no_spatial_variation`, meaning every channel's max − min is within 2/255.
  - `invert`: `flat_normal`, meaning X and Y are both within 0.5 ± 2/255.
  - `green_flip`: `flat_normal_y`, judged on Y only.
  - One varying texture admits the slot. A measurement that is still `pending`, or `unmeasurable`, refuses. B applies, so only that component is skipped.
- **Tolerance:** 2 of 255 steps. Constants and flat normals stay within 1 step after compression; real detail is far above it (fixture 0.22–0.69, real normals 0.35–0.67).
- **Cost:**
  - A GPU compute pass, read back asynchronously and cached per texture and resident first mip. At most 4 starts per frame and 16 in flight; nothing waits on the game thread.
  - The fixture's 53 textures were measured in 16 uncaptured frames before the first captured frame.
  - MainWorld census: longest frame 25.0 ms on `36C8E30A` (it was 27.6 ms in 090-10e).
- **Tests:** uniformity self-test 30/30, with 6 mutants failing; the 090-10c suites are unchanged at 7/7 and 27/27.
- **Auto-pool, office-like fixture, PIE:**

| build | events | visibly corrupt | no visible change | edges | labelled frames without a mask |
|---|---|---|---|---|---|
| `88801bd` (can-fail) | 28 | 17 | **11** | 17/17 | 0 |
| `36C8E30A` | 28 | **28** | **0** | 27/27 | 0 of 219 |

- **Eligibility before / after:**

| content | uv eligible | normal eligible | why it dropped |
|---|---|---|---|
| fixture | 72 → 32 | 8 → 4 | `texture_uniform` is the only blocker in every case: 16×16 `T_TC_Chain*`, `LayerGlobal`, `UI1x1` |
| MainWorld | 33 → 25 | 9 → 9 | engine default textures such as `127grey`, `WhiteSquareTexture`, `T_black` |

- No other class of labelled-but-invisible event showed up.

**Item 2: B's component scope**
- **Fixture:** `AAnomalyBenchTwoPartActor`, a static object with one eligible cube and one cube refused as `texture_not_parameter`.
- **Defect found and fixed** (`4dfce28`, G454): `bbox_px` / `bbox_norm` covered the whole object (x 273–551) while the mask covered only the eligible part. `GetActorLabelBounds` now feeds both.

| leg (7 fires) | mask px/frame in eligible / ineligible box | `bbox_px` inside eligible box | picture change eligible / ineligible | m26 count | edges |
|---|---|---|---|---|---|
| uv, `36C8E30A` | 11,260 / **0** | 8/8 ×7 | 0.92 / **0.00** | 11,260 | 7/7 |
| normal, `36C8E30A` | 11,260 / **0** | 8/8 ×7 | 0.94 / **0.00** | 11,260 | 7/7 |
| uv and normal, `NoPartScope` can-fail | 11,260 / **11,260** | 0/8 | same | **22,520** | 7/7 |

Edges were off by 0 frames on every leg.

**Item 3: cooked bias, staged**
- **Cooks:** `f2a`, `f2b` and `f2c` all succeeded (57–106 s); dbghelp did not block.
- **What the cook does:** V1 (the texture's own bias) and V2 (a dedicated texture group plus a cooked device profile) arrive without their top mip: 128 → 64, 4096 → 2048, and 2048 → 1024 for the streaming copy. Runtime bias then reads 0. V3 (cinematic mips) keeps its full chain, so its bias is a runtime property (G455).

| variant | uv | normal | identity (null ≈ 0.34) | edges | frames without a mask |
|---|---|---|---|---|---|
| V1, V2, V3 | 7/7 corrupt | 7/7 corrupt | 0.333–0.337, no change | 7/7 | 0 |
| V1S, V2S (4k) | 7/7 | — | no change | 7/7 | 0 |
| V1T, V2T (streaming) | 6/6 | — | first event 12.21, then no change | 6/6 | 0 |

- **Can-fail on `65607703`:** V3 is refused (`runtime_lod_bias:cinematic`, 0 events). V1, V2, V1S, V2S and V1T, V2T corrupt correctly on the old binary too, because the bias has already been cooked out.
- **Lever (G449):** `IAI.Bench.TexCorruptHostTexBias` now refuses in any cooked build, and its help text says editor and PIE only. A staged leg confirmed it logs "REFUSED … Nothing was changed" and still corrupts 7/7.

**Builds, archives, commits**
- **Strict include pass:** PASS, 0 strict errors and 0 normal errors or warnings, on the fix branch (`7d4debc`) and on m53 (`2dbf6f2`).
- **m53 checks:** lever audit 52/52 PASS; read-only census guard PASS (self-test 40/40); Python suite 27/27.
- **String scans:** fix +4 / −0, m53 +37 / −5; m53's 11 unclassified additions are listed in `strings_m53_final.json`.
- **Comment stripper:** 0 files changed on both trees.
- **Archives** in `D:\IA_BankOverflow\_binary_baselines\`: `m52fix-0910e-68BA4D9A`, `m53-0910e-EBEED6F4`, `m52fix-0910f2-822FA156`, `m53-0910f2-0A99BA3C`, `m53-0910f2-36C8E30A`. BenchGate is back on the S1 set (`2FCDF059` / `20DA6F98`), with receipts.
- **Pushed:**
  - fix: `ba1af33..12a6db8`
  - m53: `48a63a2..b8cd3cd`
  - `m51`, `master` and tags were not touched.
- **Journals:**
  - `docs/sessions/2026-10-02-090-10e-pie-endsettle-census-slice.md` (fix branch)
  - `docs/sessions/2026-10-02-090-10f2-uniform-bscope-cooked.md` (m53)

**Decisions needed**
1. **Item 3 can-fail scope.** In a cooked build the old binary cannot be shown refusing per-texture or group bias, because the cook already strips it. The staged can-fail exists only for cinematic, which refused. Per-texture and group handling rests on the 090-10c PIE can-fail. Please accept that scope, or name a different test.
2. **`texture_uniform:pending` can cost a fire.** When a streaming texture finishes loading after the pre-run measurement, its next fire is refused while the new mip is measured (1 of 7 on V1T and V2T). It fails safe, with no wrong label. Re-measuring on a cache miss before refusing would get the fire back; this is not built.
3. **Identity mode on a still-streaming texture copies whatever resolution is loaded,** so the first event read 12.21 against ≈0.34. This is bench-only; the real uv/normal corruption still shows 6/6.
api-equivalent cost (not billed on Max): $70.40 | turns: 466 | running total across all briefs: $4,431.81
