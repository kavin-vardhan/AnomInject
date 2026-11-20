# REPORT for 082-07f-m53-masked-fixture-audit-recook.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 082-07f-m53-masked-fixture-audit-recook.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-28 01:07:22 | finished: 2026-09-28 01:34:50 | duration: 27 min | exit code: 0
context at last turn: 670503 tokens (67% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\082-07f-m53-masked-fixture-audit-recook.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

The fixture is diagnosed, audited, fixed and proven. G-COOK passes and the new build is staged. The in-engine probe shows the masked tile now clips. But checks (b) and (c) read up to 7 and 5 within 48 px of the fixed tile, which is the brief's stop condition, so I stopped there. I believe this is a normal rendering halo rather than a cook side effect (evidence in §8); it needs your ruling.

**2. Plain summary for the owner**
- The masked test tile never cut out its see-through parts because of a stale internal "treat as opaque" flag saved by our fixture tool. It is fixed, the new build is on the bench, and the tile now shows the sky where it should.
- A full check of every other test tile found nothing else wrong.
- One stop is left for chat to rule on: changing that tile also shifts a few nearby pixels slightly. Normal runs on the old build show the same effect, so the new build did not change anything else.

**3. Diagnosis (with evidence)**
- **Cause:** `M_TC_Masked` was saved with `bCanMaskedBeAssumedOpaque = true`, so `UMaterial::GetBlendMode()` returned **Opaque** (`Material.cpp:6177-6185`).
  - That flag is recomputed only in `PostEditChangeProperty` (`Material.cpp:4383`).
  - The tool set the blend mode and the clip value first, then connected OpacityMask. Connecting a pin does not trigger that recompute, and nothing recompiled before saving.
- **Read-back from disk:**
  - the slot holds `M_TC_Masked` itself (no instance overriding the blend mode);
  - authored blend is Masked, clip 0.5;
  - OpacityMask is fed by the texture's alpha output, Emissive by the same texture's RGB;
  - `T_TC_UC2` keeps its alpha (`TC_DEFAULT`, no-alpha flag off).
- **Alpha survives the cook:** the U-C2 opaque tile, same texture and same build, shows four alpha greys that map back to a ≈ 0.28 / 0.41 / 0.60 / 0.72.
- **The decisive read:** as loaded from disk, `get_blend_mode()` = `BLEND_OPAQUE`. After an in-memory recompile (not saved) it reads `BLEND_MASKED`. It is the only fixture material whose effective blend differs from its authored one.
- **It is CaptureBench content, not an engine or project setting.** The tile draws unclipped in every leg on both builds, including legs that never target it.

**4. Audit table** (post-fix, `082-07f-evidence/audit.json`: channel route from disk read-back, slot from the level read-back, offline minimum)

All 42 fault rows read OK (the 6 D-class rows are readings only):

| fault row | tile / slot material | fault(s) | channel route | offline min |
|---|---|---|---|---|
| U-C1 | `StaticMeshActor_0` / `M_TC_UC1` | chanswap, srgbtwice | RGB to Emissive | 120, 73 |
| U-C2 | `StaticMeshActor_1` / `M_TC_UC2` | chanswap, srgbtwice, alpha | AlphaSplit readout | 120, 73, 72 |
| U-C3 | `StaticMeshActor_2` / `M_TC_UC3` | chanswap, srgbtwice, alpha | AlphaSplit readout | 120, 73, 72 |
| U-C4 | `StaticMeshActor_3` / `M_TC_UC4` | chanswap, srgbtwice | RGB | 120, 73 |
| U-C5 | `StaticMeshActor_4` / `M_TC_UC5` | srgbtwice | RGB | 37 |
| U-D1 | `StaticMeshActor_5` / `M_TC_UD1` | chanswap | RGB | 120 |
| U-D2 | `StaticMeshActor_6` / `M_TC_UD2` | chanswap, alpha | AlphaSplit readout | 120, 72 |
| U-D3 | `StaticMeshActor_7` / `M_TC_UD3` | chanswap, alpha | AlphaSplit readout | 120, 72 |
| U-D4 | `StaticMeshActor_8` / `M_TC_UD4` | chanswap | RGB | 120 |
| U-D5 | `StaticMeshActor_9` / `M_TC_UD5` | chanswap | RGB | 56 |
| U-D6 | `StaticMeshActor_10` / `M_TC_UD6` | chanswap | RGB | 56 |
| U-N1 | `StaticMeshActor_11` / `M_TC_UN1` | normal, chanswap | normal readout to Emissive, texture to Normal | 89, 69 |
| N-N1 | `StaticMeshActor_12` / `M_TC_NN1` | normal, chanswap | same | 89, 74 |
| **MASKED** | `StaticMeshActor_13` / `M_TC_Masked` | alpha | alpha to the clip; effective blend **Masked** after the fix (was Opaque) | **138 (new)** |
| Shared | `StaticMeshActor_14` / `M_TC_Shared` | chanswap | RGB | 120 |
| G-ID-M A Far / Graze / Trans | `StaticMeshActor_37/27/47` / `M_TC_ChainA` | mipshift (all three), mipgen (Far, Trans) | RGB | 80 |
| G-ID-M B Mag | `StaticMeshActor_18` / `M_TC_ChainB` | texelshift | RGB | 128 |
| G-ID-M D Mag, uv and normal | `StaticMeshActor_22` / `M_TC_ChainD` | texelshift | normal readout | 89 |
| G-ID-M C chains (D-class, 6 rows) | Sharpen 39/49, Blur 40/50, AlphaCov 41/51 | mipgen | RGB or AlphaSplit | not provable offline (declared) |
| G-RD | `StaticMeshActor_15/16` / `M_TC_RedrawOpaque` / `M_TC_RedrawAlpha` | noclear | RGB / AlphaSplit | 112, 51 |
| G-BIND (3) | `StaticMeshActor_57` / `M_TC_Layer` | tile probe | LayerSplit to Emissive | new proof: layer max 120, 12,288 of 16,384 texels ≥ 16; global 0 |

- **The 46 identity rows** (every G-ID, G-ID-M, RD and BIND_ID leg) all hold their readout, and every effective blend equals the authored one.
- **Rows with no channel dependence:** G3 compares applied against null; G4, G-REASON and G6 are judged from logs.
- **The audit detects the defect:** run on the pre-fix read-back, it flags exactly `WC_MASKED_ALPHA` and `ID_MASKED`. Post-fix it reads 0 defects.

**5. The fixture fix and read-back proofs** (CaptureBench `2c5ec56`)
- **Tool changes:**
  - the author path now recompiles after connecting OpacityMask;
  - a new `masked` mode re-saves only `M_TC_Masked`;
  - verify now requires every material's effective blend to equal its authored one, plus a full MASKED read-back;
  - two offline proofs run from the source PNGs through a pure-Python reader, which matches Pillow on every fixture PNG.
- **Negative control:** the new verify on the unfixed fixture failed on exactly the two MASKED checks and nothing else.
- **After the fix:** masked mode reads Opaque → Masked (file hash `49f132a6…` → `b687fa92…`). A separate verify run reports 0 problems; slot and normal proofs 4/4 each.
- **Offline MASKED proof:**
  - quarters with alpha 72 and 104 clip in both copies (not relied on);
  - alpha 152 (cyan) and 184 (yellow) are drawn by the right copy and clipped under the alpha fault;
  - predicted minimum |d| against the sky = **138** (cyan) and 140 (yellow), floor 32. The sky colour range is the one measured input, taken from 082-07e.

**6. Manifests and G-COOK**
- **Manifests:** M1pre ≡ 082-07c's final manifest. M1‴ changes exactly `M_TC_Masked.uasset`. M2‴-prestage ≡ M2‴ ≡ M1‴, all clean against M0.
- **Pre-declaration** `36F325E0…` was written before any cook: f1 the candidate, f2 and f3 controls; 899 packages each, 0 warnings.

| G-COOK row | reading | verdict |
|---|---|---|
| map gate | five maps present, exit 0 | PASS |
| `CB_GateLevel` | exact | PASS |
| `MainWorld` | header identical in 20 containers; diff(A, f1) 0 words outside the region at all four alignments | PASS |
| fixture / plugin packages | 101/101, 3/3 | PASS |
| changes from the old build (reading) | `M_TC_Masked`, plus the same shader-archive / `MainWorld` / header pattern as 082-07c; shader chunks 1,075 → 1,077 | reading |

G-COOK: **PASS**.

**7. Staged build, A44, archive**
- **Staged:** exe `2FCDF059` (unchanged), utoc `30FE0FDE→20DA6F98`, ucas `11231356→534C5863`, pak `FD766B7B`, globals `462B8AC6` / `BB05CF99`. Each file verified at the destination; no game running during staging.
- **A44:** token table identical to 082-07c's (same exe).
- **Archives:**
  - candidate `_binary_baselines\m53-s1-maskedfix-cook-20DA6F98\`, verified 6/6;
  - rollback `m53-s1-normalfix-cook-30FE0FDE\`, re-verified 6/6 before staging;
  - provenance entry added to the README.

**8. Probe readings** (label `PROBE_M53_082_07F_NULL_MASKED`; frames kept out of the bank, ledger and live run dir)
- **Launch:** passed the harness quiet and person-present gates. Validity 0 errors and pose matched; no person evidence.
- **Probe postflight PASS:** live run dir, bank and exe side byte-unchanged.
- **Locked postflight:** reports only the three expected items (staged utoc and ucas, and the feature head).
- **(a) PASS:** every alpha < 0.5 cell shows the sky on all 100 frames, and the target mask drops from 4,096 to 2,048 px.
- **(b) FAILS as written:** max 7 outside the tile, only within 48 px of it (far field ≤ 1).
  - The lit neighbour NN1's nearest 22 px are uniformly brighter (312 of 312).
  - The sky ring within 3 px of the tile is uniformly darker (197 of 197).
  - The pattern is identical in all 100 frames.
- **(c) FAILS as written:** drawn cells differ by up to 5, inside the same halo; their colours are otherwise intact.
- **Why I read this as not the re-cook:**
  - On the **same old build**, with no cook involved, every 082-07e wrong copy against its null leaks 8–17 levels within 4 px of its tile, ≤ 2 beyond 16 px.
  - The cook changed only `M_TC_Masked`.
  - The mechanism is not established; candidates are bloom and screen-space AO.

**9. Rows the amendment must change**
1. **MASKED:** add the PRE row: the null leg must show the clip (the probe measured it).
2. **G-BIND (3)'s "everything outside the target ≤ 2"** (whole frame, tolerance 2) will **FAIL as written**. The tile probe changes the layer half by up to 120, and the halo puts 8–17 levels next to any changed tile. It needs a ring exclusion plus a far-field test. A fixture change cannot remove a renderer effect.
3. **The build under test** is now `20DA6F98` / `534C5863`.
   - `082-07-common.py` hard-codes the old staged hashes, so the re-issue must edit that locked file as well as the boundary.
4. Cite the new G-BIND (3) offline proof.

**10. Commits pushed**
- CaptureBench `2c5ec56` (local-only repo): the fixture tool.
- Plugin `feat/m53-uv-normal-corruption` `1395377`, local = origin: journal, status block, G323–G324.
- `m51` `53bf725`, `master` `b5f15a3`, no tags.

**11. NEEDS-DECISION**
1. **Rule the probe stop.** Either accept (b) and (c) as a neighbourhood halo, with the far field ≤ 1 and the same-build precedent, or require more. For example, one probe with bloom and AO off would establish the mechanism.
2. **Confirm the build stays staged.** I left the new build on the bench; rollback is one archive copy away.
3. **Rule on the G-BIND (3) outside clause** in AMENDMENT 2, in 082-07g.

Evidence is in `D:\IntrusiveAnomalies\_reviews\082-07f-evidence\`.
api-equivalent cost (not billed on Max): $22.10 | turns: 107 | running total across all briefs: $2,739.01
