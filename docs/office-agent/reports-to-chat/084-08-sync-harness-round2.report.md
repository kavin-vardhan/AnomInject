# REPORT for 084-08-sync-harness-round2.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 084-08-sync-harness-round2.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-29 05:18:57 | finished: 2026-09-29 06:15:18 | duration: 56 min | exit code: 0
context at last turn: 876139 tokens (88% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\084-08-sync-harness-round2.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

**Summary for the owner:** Both parts are done. The four small fixes are built, cooked and archived but not staged, and the harness for Tuesday's re-bench is written and passes its dry run. Nothing was launched. Two things need chat: no MainWorld target meets the declared LOD-fixture rule, and MASK55 v2 needs a different route to concurrent mask IDs than the brief assumed.

### 3. Part A (source `4bf0915`)
Each fix has a unit test showing the new behaviour and the old one side by side:
- **A1 (shear case):** an instanced mesh whose collision transform differs from the drawn one beyond ε is flagged `camera_clipping_unconfirmed` with no trace. On Codex's case the old path read an unflagged miss on a frame that is truly clipped. Uniform scale, rotation about the scaled axis and an unrotated instance still match and are traced.
- **A2:** a forced-unknown held set now defaults to `unresolved` and carries that reason with AA off. The 084-07c default read `not_held` with no reason. A classification backed by a real texture record survives forcing.
- **A3:** welded candidates are flagged, with no trace. On Codex's case the old path confirmed a positive off the parent's collision body.
- **A4:** a restore that writes back a mask value still in use is counted as `mask_prior_collision` and the value is quarantined. On Codex's saved-prior sequence, value 210 is never re-issued; without the fix it is recycled and aliases.
- **Selftests:** m52 282/0 (was 262), camera_clipping 154/0 (was 128).
- **Mutation check:** all 16 mutants (the nine from 084-07c plus seven for A1–A4) make the selftests fail.
- **Build:** Editor 13 actions / 70 s, Game 6 / 47 s, 0 warnings. A44 passes: 9 new strings present, the `_v3` rule string absent. Comment stripper changed 0 of 120 files.
  - Exe **`8A6074AA`**, archived at `_binary_baselines\m52fix-8A6074AA`.
- **Re-cook:** predeclared, all five predictions met. 46 s, 794 packages, 0 warnings.
  - The cooked descriptor is byte-identical to the branch's (`9EFB9B49…`); the map gate passes.
  - The content manifest is identical before and after (`E4C7F38D…`).
  - Pairing: exe `8A6074AA` + utoc **`4BF63FB8`** / ucas `4142CC1E` / pak `26FFC026` / globals `462B8AC6` / `BB05CF99`, archived at `_binary_baselines\m52fix-cook-4BF63FB8` (all six re-hashed at the destination).
  - The m53 set is still staged.

### 3b. Part B: harness `_reviews\084-09-*` (read the README first)
The self-test passes 24/24. On its first run it caught a real evaluator bug: the inherited "a transition flag on a leg without temporal AA fails" rule also fired on `partial`, which would have failed every AA-off partial onset. It now applies to temporal reasons only (G368).

- **G355 (restore):** in the dry run the restore hit an injected 8 s exclusive lock, failed twice with WinError 32, and succeeded on attempt 3. A lock that outlasts every retry raises an error rather than passing silently.
- **G356 (null):** the null now calibrates from its own sharpness series. The 084-06 B5 null (0 events) passes, where the 084-06 gate failed it.
- **Partial-flag guard (084-07 decisions 1–2 + N4):**
  - A record-backed partial onset is counted, not failed.
  - Must-fail cases all fail: a flag spread over a real late label, a flag on a frame with no held texture, an unflagged off frame, and an off frame backed only by `unresolved`.
  - Edges with more than 8 partial frames are listed for investigation.
- **`--label-rule`:** runs on every leg, using the pinned `4bf0915` `verify_capture.py` (its selftest passes 24 cases).
- **`labelled` rule:**
  - New sessions pass; old banks are read under the old rule and say so.
  - The `FF41BFF3` shape fails.
  - A `labelled:true` entry outside every event list is reported as a defect.
- **B-CC:** runs on the new cook, whose descriptor includes `AnomalyBench`. The gate adds per-edge label-vs-pixel offsets and an unconfirmed-frame count. The 084-06 bank reads INVALID.
- **B-CC-MW:** a labelled, confirmed frame must beat the per-frame null floor (`D_k > 2·N_k`). The 084-06 bank fails it: 169 of 200 frames sit at the floor.
- **MASK55 v2:**
  - Every frame must have a TAG-OWNERS line with 0 shared values, and `peak_live ≥ 2` is a validity condition.
  - The 084-06 bank reads INVALID (peak 1, the known answer).
  - An injected shared frame fails.

### 4. G356 re-evaluation of the 084-06 B0/B3 bank
**14 of the 17 CENSORED offsets resolve.**
- **B0:** 12/12 become PASS (16/16 PASS). The null ratio engages at lag 0, r 0.9987, and offsets land at +4…+6, inside the 8 off-flags.
- **B3:** 2 of 5 resolve, one to PASS and one to FAIL. The FAIL is a single pixel-visible frame at label end +16, not connected to the tail; cause not established.
- **Still open on B3:**
  - 3 offsets stay censored, with post/pre drift ±0.11–0.13 against the null ratio (lag 16, r 0.93).
  - 2 events that were PASS become CENSORED.
- **Unchanged:** B9 still fails 16/16 (that build had no partial flag), and the B0L lever still fails as designed.

### 5. missing_texture reference re-check (MASK55 bank)
Amendment R1 was written into the README at 05:55:54 and hashed before the re-check ran.
- **Result:** `StaticMeshActor_85` is **CONFOUNDED-REFERENCE**, neither PASS nor FAIL.
  - The only clean frames before it (180–182) sit inside the 4-frame settle after the previous event's `hide_return` at 179.
  - The usable post-event frames (191–192) are too few.
- **Per type under R1:** missing_texture 37 CONFOUNDED; corrupted_texture 29; missing_object 20 (+1 warm-up); blinking 0 (32 PASS).
- **Under the 084-06 rule** the same bank reads missing_texture 35 PASS + 1 FAIL.

The bank cannot decide the case, and R1 was not loosened after seeing the result (G369). I added an **MT85 pair** to the bench to decide it: missing_texture on `StaticMeshActor_85` with wide gaps (`2 4 8 14 0`), both tick orders.

### 6. LOD fixture
- **Rule (declared before selection in `lod-selection-rule.md`):**
  - multi-LOD and not Nanite;
  - predicted at LOD 0 at the bench camera;
  - bounds coverage 7–60 %;
  - LOD0-to-worst triangle ratio ≥ 8.
- **Scan:** all 421 MainWorld actors, via a commandlet on the host with temporary content junctions; the manifest was identical afterwards.
- **Only candidates:**

| Mesh | LODs | Triangles | Ratio | Coverage |
|---|---|---|---|---|
| `SM_rock` | 4 | 15,674 → 1,960 | 7.997 | 11.8 % |
| `SM_rock_02` | 4 | 21,465 → 2,684 | 7.997 | 9.2 % (runtime log 8.65 %) |
| `SM_Bush` | 2 | — | 2.0 | out of frustum |

- **Nothing qualifies (shortfall 2), and nothing was relaxed.** The harness carries premise legs on the two rocks, predicted to fail. The 8 gate legs run only with `--with-lod-gate` and a passed premise.

### 7. Legs, predictions, time
29 required legs, all on the part-A pairing:
- **M52:** B0, B5 (null), B3, B9 (AA off), B0L (lever), P1 (lever).
- **BCC:** 7 legs.
- **MASK55V2:** 1 leg.
- **MT85:** ×2.
- **BREAL:** the four proven types × both tick orders.
- **LOD premise:** LODP ×2.
- **BCCMW:** 3 legs.

Plus 8 optional LOD gate legs.

**Predictions (per leg in the README):**
- **m52:**
  - B0 passes with partial and TAA flags and ≤ 2 censored.
  - B9 passes 16/16 via record-backed `partial`.
  - Both levers fail.
- **B-CC:** 0 label-vs-pixel disagreements, and every edge's label flip matches the pixel flip.
- **MASK55V2:** peak live ≥ 2, recycling happens, 0 alias frames, 0 shared.
- **MT85 and BREAL:** 0/0 at t50.
- **LOD premise:** fails.
- **CCMW:** 0 over-labelled confirmed frames.
- **Every leg:** label-rule passes.

**Dry run:** exit 0, 0 preflight problems, 29/29 launches accepted, 8 correctly skipped as NOT-RUN-PREMISE, 516 s.

**Expected bench time:** about 66 min, about 80 with retries and person waits, and about 15 min more if the LOD gate runs.

### 8. Commits pushed (`fix/m52-label-timing`)
`4bf0915` (Part A fixes), `17b9d5d` (docs: journal, status block, client readme, architecture, gotchas G366–G369). No tag, no merge; `m51` is untouched at `53bf725`.

### 9. NEEDS-DECISION
1. **LOD fixture.** The declared rule selected no MainWorld target. Either accept the two rocks as the best available (triangle ratio 7.997), or authorise a separate brief to author a high-contrast LOD fixture and cook it; that changes project content, which this brief forbade. Until chat rules, the gate legs stay off.
2. **MASK55 v2 route.** A capture fires exactly one anomaly per burst, so `IAI.Auto.MaxConcurrent` never makes events concurrent (G366). The substitute is `stuck_low_mip` in the pool, whose restore trail kept 084-06's B0 at peak live 2, plus `IAI.Bench.TagPoolLimit 12`, with peak live ≥ 2 as a validity condition. Accept this, or rule otherwise.
3. **For information:**
   - The MT85 pair was added, since the bank can't settle the missing_texture question.
   - B-CC runs on the part-A cook rather than the 084-07b cook named in the brief, per the 084-07c ruling.
api-equivalent cost (not billed on Max): $32.94 | turns: 212 | running total across all briefs: $3,357.50
