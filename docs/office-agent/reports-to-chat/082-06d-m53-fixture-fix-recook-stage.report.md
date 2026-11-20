# REPORT for 082-06d-m53-fixture-fix-recook-stage.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 082-06d-m53-fixture-fix-recook-stage.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: high
started: 2026-09-27 06:09:17 | finished: 2026-09-27 06:36:48 | duration: 28 min | exit code: 0
context at last turn: 531527 tokens (53% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\082-06d-m53-fixture-fix-recook-stage.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN

**Plain summary for the owner.** The test level's four broken meshes now hold their materials, and I confirmed that by reading the saved files back from disk. The rebuilt package passed its check under the amended rule: the new test content is all present and nothing existing changed beyond `MainWorld`'s normal run-to-run noise. That package is now on the bench. That noise check took 13 cooks rather than 3 to judge properly (item 3); worth a look, but not blocking.

**1. Fixture fix and the four proofs from disk**

The tool fix is CaptureBench `47d6fff` (`make_texcorrupt_fixture.py`):
- Static-mesh slots are now set with `SetMaterial`.
- The skeletal copy gets a newly built list of material entries.
- A new `slots` mode re-writes only the four affected meshes.
- The check pass now runs as a separate process that never saves, so it reads the slots from the saved files.

The new check first failed on all 4 meshes of the old fixture, as it should. After the fix, every file it read matched the file just saved (4 of 4):

| reason it must produce | mesh | what the saved file shows |
|---|---|---|
| `slot_empty` | `SM_TC_PlaneNoMat` | slot material is None |
| asset-slot MID (`host_mid`) | `SM_TC_PlaneAssetMid` | slot = `M_TC_AssetMid`, whose texture is `T_TC_AssetMid` |
| `nanite_override` | `SM_TC_CubeNanite` | Nanite on; slot = `M_TC_NaniteBase`, which is Nanite-enabled and overrides to `M_TC_NaniteOverride` |
| `default_material_path` | `SK_TC_Cube` | slot = `M_TC_SkelNoUsage`, with the skeletal-mesh flag False |

**2. M1′ and M2′ (the before/after file fingerprints)**
- **Before anything ran:** the tree matched 082-06's M2 exactly.
- **M1′:** only the 4 mesh files changed (`SK_TC_Cube`, `SM_TC_CubeNanite`, `SM_TC_PlaneAssetMid`, `SM_TC_PlaneNoMat`). The check pass changed nothing.
- **M2′:** taken after the cooks and again after staging; both are identical to M1′.
- **Against the original M0:** clean every time. `CB_GateLevel.umap`, `MainWorld.umap` and all 421 of `MainWorld`'s external actors are unchanged.

**3. Cook and G-COOK**

All 13 cooks built cleanly: 899 packages each (082-06 cooked 894), 0 warnings, 0 errors. I named cook d1 the candidate before reading any result; d2–d13 were controls.

- **`CB_GateLevel`:** exact match with the old package in all 13 cooks (`0x01AD80B5B5F83E…`).
- **`MainWorld`:**
  - Its header is byte-identical in the old package and all 13 cooks, and the data sizes are equal.
  - Every byte where d1 differs from the old package falls inside the region that varies between identical cooks: 0 outside at all four word alignments and at byte level. The same holds for each of the other 12 cooks.
  - With 3 cooks, my own stricter version of the test (every alignment must pass) failed: 4 / 20 / 25 / 0 words outside.
  - I looked at the raw bytes before deciding anything. All 49 bytes involved were the top byte of a random 0–1 value stored in each 16-byte record. Three cooks often land that byte on the same value by chance, so three is too few samples to map the noise.
  - The ruling allows extra cooks in that case. I wrote down a fixed count of 10 more before running them, keeping the same candidate and the same test.
  - The noise region stopped growing at 1,800 bytes from the 9th cook onward. A byte that genuinely differed between old and new content could never enter it, however many cooks ran.
- **Fixture packages:** all 101 fixture packages and all 3 plugin assets are in the container.
- **Map gate:** exit 0, with the new level named.
- **One check I could not make:** reading whether the cooked `M_TC_SkelNoUsage` carries the skeletal-mesh flag. My scan also missed the flag on a material known to have it, so the method can't see it and I make no claim either way. M2′ matching M1′ does show the cook saved nothing back to the source files.

**4. Staging**
- **New bench build:** exe `2FCDF059`, container `9A26D497` / `10D7F0C0` / `ABD931A2`, globals `462B8AC6` / `BB05CF99`. Every file was hash-checked after copying.
- **Archive:** also kept at `_binary_baselines\m53-s1-cook-9A26D497\`, with a provenance entry in that folder's README.
- **Rollback:** the 082-06 pre-cook archive (`E0BE6F0A` + Phase B container).
- **A44 string check** on the staged exe: every new m53 string is present, alongside the usual control strings (`ShaderPrewarm` 7, `StuckMipNoHold` 5).

**5. Docs and state**
- Branch commit `aadd7eb`, pushed. It adds:
  - plan revision 3.4 (§R0.00000), recording the amended `MainWorld` row and my reading of it;
  - runbook §8.6a on the linked-Content scratch host;
  - gotcha G312;
  - the journal `docs/sessions/2026-09-27-082-06d-code-m53-fixture-fix-recook-stage.md`;
  - the status block.
- No tag. `m51`, `master` and tags are untouched.
- Evidence is in `_reviews\082-06d-evidence\`; the 13 cook outputs stay on E:.

**NEEDS-DECISION:** none blocking. Please review the route to the `MainWorld` pass (journal §4.1): the 3-cook failure, what the bytes showed, and the 10 extra cooks fixed in advance.

The Figma, Asana, Slack and other plugin connectors listed at startup need authorising in claude.ai or `/mcp`, and the PDF viewer failed to connect. None was needed for this brief.
api-equivalent cost (not billed on Max): $13.29 | turns: 96 | running total across all briefs: $2,640.14
