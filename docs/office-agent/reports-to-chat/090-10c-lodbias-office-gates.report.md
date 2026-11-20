# REPORT for 090-10c-lodbias-office-gates.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 090-10c-lodbias-office-gates.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-10-02 01:38:37 | finished: 2026-10-02 06:08:44 | duration: 270 min | exit code: 0
context at last turn: 417262 tokens (42% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\090-10c-lodbias-office-gates.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

```
VERDICT: NEEDS-DECISION
The fix works and is pushed. Three proofs the brief asked for are still missing; they are listed in section 9.

2. FOR THE OWNER
- Why the office refused everything: real games lower texture detail (LOD bias, lower texture quality, streaming).
  The plugin refused any texture not drawn at full detail. It now copies the texture at the detail the game
  actually draws, so those objects are no longer refused.
- At home, on content set up like the office (same bias settings), eligible UV objects went from 0 to 72 of 82
  on the fixture, and from 0 to 33 of 343 on MainWorld. On MainWorld, 85 more are blocked only by Nanite, which
  the office does not use.
- Pull feat/m53-uv-normal-corruption at 88801bd (its code is dcd8d29, exe B3103E32). Run the census command below.

3. ALL-REASONS CENSUS
Office command, in PIE after Play:  IAI.TexCorrupt.Census allreasons
(tools/anomaly_refusal_counts.py <log> --allreasons-check reads its output.)

Office-like home content: scratch device-profile texture-group LODBias 1, sg.TextureQuality 1, r.Streaming.PoolSize 300,
per-texture LODBias 1 on every 3rd actor, cinematic 2 on every 3rd+1, JobCacheDDC. All in memory or scratch, removed by receipt.

| leg                    | uv eligible, BEFORE (C1) -> AFTER (dcd8d29) | normal eligible |
| fixture plain          | 67 -> 72 of 82                              | 8 -> 8          |
| fixture office-like    | 0 -> 72 of 82 (before: first refusal runtime_lod_bias 74) | 0 -> 8 |
| MainWorld plain        | 25 -> 33 of 343                             | 8 -> 9          |
| MainWorld office-like  | 0 -> 33 of 343                              | 0 -> 9          |

BEFORE, office-like fixture, uv. Each entry is: objects failing it / objects failing only it.
- max_below_cooked 79/0
- per_texture 79/0
- cinematic 60/0
- streaming_budget 3/0
- top combination: max_below_cooked + cinematic + per_texture = 51 objects

AFTER, office-like fixture, uv. Blocking keys (all genuine product refusals; no handled key blocks anything):
- no_eligible_slot 2/2
- below_size_policy 1/1
- excluded_group 1/1
- map_set_over_cap 1/1
- texture_not_parameter 1/1
- PF_R16F 1/1
- cube 1/1
- virtual_texture 1/1
- Nanite (TC_Nanite) 1
- notes: resident_chain 79, per_texture 79, cinematic 60

AFTER, office-like MainWorld, uv:
- nanite_unmaskable 266, of which 85 are blocked by Nanite alone
- draw_shaders_missing + no_textures 155 (all on Nanite-only materials)
- texture_not_parameter 45
- host_mid 25
- virtual_texture 4
- no_eligible_slot 2
- excluded_group 1
- notes: resident_chain 188, per_texture 188, cinematic 42
- b_gain_objects 0 on every census
- plain MainWorld: over_budget is now a note on 102 objects (it was 134 refusals)

Selftests:
- the checker passes the real table and fails the leg run with the lever IAI.Bench.TexCorruptAllReasonsFirstOnly 1 (rc 1)
- census_read-only selftest 40/40
- refusal-counts selftest 21

4. CAN-FAIL: 0a69ea3 (archive m53-0910b-65607703) on the same setups
| variant                   | PIE                                         | staged                            |
| V1 per-texture LODBias 1  | refused per_texture, 0 events               | not runnable (G449)               |
| V2 texture group          | refused per_texture, 0 events               | bias did not reach the cook: applied 7 |
| V3 cinematic              | refused cinematic, 0 events                 | not runnable                      |
| V4 r.MipMapLODBias 1      | refused global_sampler, 0 events            | refused global_sampler, 0 events  |
| V5a streaming budget      | applied 7: the 128 px fixture textures do not stream; the refusal shows in the BEFORE census (streaming_budget 188) | applied 7 |
| V5b global streaming      | refused global_streaming, 0 events          | refused, 0 events                 |
| V6 partly resident        | refused not_fully_resident (7 of 12), 0 events | 6 events, other fires refused  |

5. ENGINE SEMANTICS, DESIGN, TESTS BOTH WAYS
Engine semantics:
- StreamableRenderResourceState.h:71-83: RHI mip 0 is cooked mip AssetLODBias + MaxNumLODs - NumResidentLODs.
- Texture.cpp:1400-1436 and Texture2DResource.cpp:101-110: per-texture, group, cinematic and budget bias only change residency.
- Texture2DResource.cpp:83: only r.MipMapLODBias reaches the sampler.
- TextureRenderTarget2D.cpp:648-655: the render-target sampler has no bias.
- TextureStreamingHelpers.cpp:299 and :325-328: the streaming bias is 0 in the editor, floored outside it, and 0 under UseAllMips.

Design as built:
(a) Resident chain. The copy is built from the resident first mip; rt_mip_mismatch and copy_source_changed read 0 on every leg.
    runtime_lod_bias, max_below_cooked and streaming_pending are now notes. Only not_fully_resident:unmappable still refuses.
(b) Effective streaming bias, not the raw cvar.
(c) r.MipMapLODBias. The copy's render-target sampler gets the bias, and the copy is drawn at SrcMip - bias. Measured identity:
    no compensation 17.9, neither 3.4, compensated 0.35 against a null of 0.34 (G447).
(d) Copy ceiling. An over-budget set halves its largest copy, down to 512 px (note over_budget).
(e) B. A component is corrupted whole or skipped. The event component scope limits the mask, the m26 measure and bbox_px to the
    corrupted components. Telemetry: components_corrupted, components_skipped, slots_corrupted_list.
(f) Census subs appear in the census line, in Auto.Yield and in anomaly_refusal_counts.

Tests both ways:
- pure 270/0; the mutant header (22 faults) gives 60 failures; mutants part_of_a_component_admitted, copy_ceiling_floor_ignored
  and editor_streaming_bias_not_zeroed
- A4 levers RtSamplerBias and SrcMipCompensation (measured above)
- NoPartScope lever exists (not exercised in engine)
- FirstOnly lever fails the checker
- 0a69ea3 per variant (section 4)

6. GATE AUDIT
Four rules: 1 label/picture disagreement, 2 wrong object, 3 crash or hard cap, 4 residue. Full table is journal section 6.
Counts are fixture / MainWorld on office-like content.

Now non-blocking:
- runtime_lod_bias per_texture / group / cinematic / streaming_budget / global_streaming: before 74/188, now notes 79/188
- global_sampler: matched
- not_fully_resident partly resident: note resident_chain 79/188
- streaming_pending: note
- over_budget: fit by the copy ceiling (102 notes); it still blocks above the floor under rule 3
- partial_footprint: now judged per component; it still blocks when no component is whole (0 / 38 uv), rule 1
- slot-level empty, translucent and no_textures: the slot is skipped, not the event

Kept:
- unmappable (rule 1): 0
- nanite_unmaskable / nanite_override (rule 1): 1/266
- draw_shaders_missing (rule 1): 1/155
- texture_not_parameter (rule 2, design limit): 1/45, about 13%
- host_mid (rule 2, C): 0/25, about 7%
- virtual_texture (C): 1/4, about 1%
- cube, R16F, excluded_group, map_set_over_cap (rule 3), below_size_policy, no_normal_map, normal_unconnected, dxt5_normal_host,
  no_eligible_slot (2/2): rule 1 unless marked
- identity and tile-probe checks: behind -IAIBench only

7. PROOF
Setup: exe B3103E32, AA-off, 150 frames, 7 fires per type. Corrupt = median at least max(4, 4 x null).
| variant | PIE uv / normal corrupt | identity exact | edges | staged |
| V1      | 7/7 (41.0) / 7/7 (17.5) | 7/7 | 7/7 | INVALID (G449) |
| V2      | 7/7 / 7/7 | 7/7 | 6/7 (end +1) | bias did not reach the cook (baseline only) |
| V3      | 7/7 / 7/7 | 7/7 | 7/7 | INVALID (G449) |
| V4      | 7/7 (34.2) / 7/7 | 7/7 | 6/7 | 7/7 / 7/7, identity 7/7, edges 7/7 |
| V5a     | 7/7 / 7/7 | 7/7 | 7/7 | 7/7 / 7/7, identity 7/7, edges 7/7 |
| V5b     | 7/7 / 7/7 | 7/7 | 6/7 | 7/7 / 7/7, identity 7/7, edges 7/7 |
| V6      | copy from mip 5 of 12; target only 2x2 px, not judgeable | | | not judgeable |
The PIE end +1 was already present in 090-10b2; staged legs show none.

Automatic application:
- Office-like fixture, with coverage 0 and poll radius 0 set for the leg only: 28 events applied. 17 visibly corrupt, edges 17/17
  in sync. The other 11 are all on the 16x16 T_TC_Chain* controls and show no visible change; not investigated.
- Office-like MainWorld: 0 events. Its view candidates are only Nanite or texture_not_parameter.

B in engine:
- BP_EnergyOrb with Nanite disallowed applies uv: 4 slots, 3 qualified, the translucent slot untouched, 7 events.
  0a69ea3 was not run on this object; its rule would refuse it (partial_footprint).
- The picture result is inconclusive: the orb rotates and pulses, its null reaches 4.5, and only 2/7 pass the rule.
- The component-scope mask is NOT SHOWN.

8. BUILDS, ARCHIVES, COMMITS
Pushed feat/m53-uv-normal-corruption, d7e396d..88801bd:
- d38bcc4: census
- 1ef8e6d, 60acb7a (does not compile), 8caf413: fix
- dcd8d29: copy ceiling, compensation, B sub
- 88801bd: docs (journal, status block, readme 8.8, architecture, checklist, G445-G449)
Origin equals local.

Legs ran on exe B3103E32. Editor DLLs:
- Injector 1E244F0C
- Capture E7A9676E
- Bench 9396BB40
- ControlServer B9D552B6
- Shaders 3BC78EFA
Archive: _binary_baselines\m53-0910c-B3103E32, re-hashed ok.

Checks:
- Strict pass on dcd8d29 (run after the legs): Game 105 and Editor 114 actions, 0 errors / 0 warnings, 64 header TUs, restore OK.
  It relinked the host to 1D4F99C6 (same source).
- C++ base 7/7, mutants 27/27
- Python 27/27
- lever audit PASS on source, exe and DLLs
- string scan vs E2BCE76B: 135 strings

Machine state:
- BenchGate is back on 2FCDF059.
- Scratch profiles removed by receipt.
- D: content manifest c583a8fe833268fc unchanged (2,207 files).
- m51 (53bf725), master, tags and the fix branch untouched. No processes left running.

9. NEEDS-DECISION
1. Staged per-texture and cinematic bias are not shown: the bench lever blanks cooked textures (G449). Its help text wrongly says it
   works in a packaged build; the next code round should make it refuse there. Staged texture-group bias also did not reach the cook.
   Options: cook a fixture with the bias baked in plus a cooked device profile, or accept PIE plus the office census.
2. B's component-scope mask has no in-engine proof (b_gain_objects is 0 at home). Options: author a two-component fixture actor,
   or accept pure tests + mutants and read b_gain_objects from the office census.
3. texture_not_parameter (about 13%) and host_mid (about 7%) are design limits that stay refused. Is a route wanted for either?
4. 11 of 28 auto-pool events on the T_TC_Chain* controls show no visible change. Investigate, or accept as the constant-texture
   class (C)?
```
api-equivalent cost (not billed on Max): $59.44 | turns: 375 | running total across all briefs: $4,315.09
