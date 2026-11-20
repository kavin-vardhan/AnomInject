# REPORT for 082-04-m53-plan-amend-delta2.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 082-04-m53-plan-amend-delta2.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-27 01:06:39 | finished: 2026-09-27 01:57:41 | duration: 51 min | exit code: 0
context at last turn: 731313 tokens (73% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\082-04-m53-plan-amend-delta2.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN

**Plain summary for the owner**
- The m53 plan now follows chat's ruling. Each texture mip is copied from the source's own mip, a bench-only test level is specified, and the budget is 128 MiB with a check that we don't starve the game's other textures of mips. Pushed as `49dece1`; still plan only, nothing built or run.
- The review relay now always runs Codex on Astra at max effort. The run record confirms it: requested and actual both `gpt-6-astra` / `max`.
- Codex's second check found **no P1**. It found three P2 plan-text fixes and one small arithmetic slip, and its answer to "is S1 ready?" is **"No, as written"** until those three are fixed. I have not acted on them.

**Resolution rows added (plan §R0.0, with this file's line ranges)**
- **N1(a):** each output mip m is drawn from source mip m.
  - The source is sampled at an explicit mip level with the view mip bias switched off.
  - Mip 0 is drawn straight into the output using the canvas draw pair that never regenerates mips.
  - Mips ≥ 1 are drawn into 1-mip scratch targets, then copied into mip m with the engine's `AddCopyTexturePass`.
  - The output's pending engine update is flushed once, on the empty target, when it is allocated.
  - New refusals `mip_chain_shape` and `runtime_lod_bias`, plus a render-thread tripwire if the source's mip count isn't what we expected.
  - `RenderCore` and `RHI` become private dependencies in S1.
- **N2:** new test level `CB_TexCorruptLevel` in its own new folder `/Game/CaptureBenchTexCorrupt/`.
  - The fixture tools are specified here and written in S1.
  - Additive-only proof: a byte manifest of all content and config before authoring, after authoring and after the cook.
  - The current cooked container is archived before the cook; after it, `CB_GateLevel`/`MainWorld` cooked chunks are compared with the archive.
  - The pre-delivery checklist line is written out.
- **N3:** 128 MiB budget via a CVar, with scratch memory counted; `G7` reads 64/128/256 MiB; new `G-COLL` compares collateral mip residency against a null leg that allocates nothing.
- **N4:** pass if max |d| ≤ 2 levels per channel; every assigned wrong-copy control must read ≥ 16; per-row error histograms reported.
- **Δ1:** the runtime-material refusal now walks the whole resolved and effective material chains (a MID, a transient outer, or not loaded from disk). A lever puts a MID in the fixture mesh's own asset slot to prove it.
- **Δ2:** the sRGB wrong-copy lever is withdrawn. Eight replacement faults, each assigned only to the rows it can affect, each to be proven ≥ 32 levels offline before use.
- **Δ3:** a bench-only tile probe for `G-BIND` in S1; `G-RD` covers identity redraws only; `drift` gets its own gate, `G-RD-DRIFT`, in S2.
- **P1-3:** only `snapshot_mip == 0` is admitted; runtime LOD bias refuses under four named sub-reasons.
- **P2-8:** any excluded map blocks its whole slot; the only exemption is a 1×1 texture.
- **P2-9:** the virtual-texture check now comes before the type check, and a new column shows each refusal is reachable in order.
- **P2-10:** 13 G-ID encoding rows; BC5-without-normal-map is refused, since in 5.1 only `TC_Normalmap` produces BC5.

**S1**
- **Scope:** identity only plus the tile probe, with the per-mip copy, decision tree, apply transaction, restore, budget, levers, the two corruptor materials and noise texture, and the new level and its tools.
- **Builds and cook:** 2 builds (editor and game), 1 cook, 2 authoring runs.
- **Order:** byte manifest → code and builds → authoring and offline proofs → archive the container → cook with the new level added to `-map=` → gate legs.
- **Stop list (14 rows):**
  - build (`G10`);
  - `G-BIND`, `G-ID`, `G-ID-M`;
  - the mip tripwire;
  - `G-RD`, `G3`, `G4`, `G-REASON`/`G6`, `G11`;
  - `G-COOK` (the additive-only checks);
  - an offline wrong-copy proof below 32;
  - the brightness check on the new level;
  - the fixture tool touching anything outside its folder.
- **Predicted yield:** S1 claims no host yield. Identity runs on the test level, plus `SM_FloorBase` if eligible (6.67 MiB). At 128 MiB, Lyra's cube (60 / 81 MiB) and weapon UV (96 MiB) are predicted to fit.

**Codex delta check #2**
- 518 s, `gpt-6-astra` / `max` (requested and actual, from `run-meta.txt`), exit 0.
- `VERDICT: APPROVE-WITH-CHANGES`
- **Per item:**
  - Δ1, Δ3, P2-8, P2-9, P2-10 and the S1 stop list: RESOLVED.
  - P1-2: RESOLVED in design.
  - Δ2 and P1-3: PARTIAL.
  - N1(a)'s copy mechanism: sound on the cooked 5.1 Windows path.
- **New findings:**
  - **A (P2):** the `texelshift` lever shifts diagonally, and a diagonal shift of a checker leaves it unchanged. My ≥ 128 proof is false; shifting one axis only fixes it.
  - **B (P2):** `G-COLL` subtracts counts of textures that lost mips, which can hide extra mip loss. It should pair textures and compare their actual resident levels.
  - **C (P2):** with the engine default of per-texture streaming bias, a budget bias shows up as `not_fully_resident` instead of the ruled `runtime_lod_bias`.
  - **P3:** two same-class 4096² maps total 192 MiB, not 213.33; the outcomes don't change.
- **Is S1 ready:** "No, as written. Exact blockers: A, B and C." All three are plan-text fixes; no new mechanism or purity ruling needed.
- Copied to `_reviews\082-04-codex-m53-delta2.md` with a header line; ledger row added. Codex's only stderr line is a failed internal patch attempt; `review.md` was written 14 s later.

**Commits pushed**
- `49dece1` on `feat/m53-uv-normal-corruption` (== origin): the plan, the journal `docs/sessions/2026-09-27-082-04-code-m53-plan-revision-3.md`, and a status block in the branch's `CLAUDE.md`. No tag.
- The first attempt put a byte-order mark on the commit subject; I fixed it before pushing.
- The relay runner and README edits are outside any repo.
- `m51` (`53bf725`), `master`, tags, containers, CaptureBench, `ToCodex\` and `E:\<personal-folder>` are untouched.

**NEEDS-DECISION**
- **Codex A, B, C and the P3 arithmetic:** chat's disposition; per the ruling, S1 is authorised only once they are fixed.
- **Five interpretations I made inside the ruling** (plan §R15), for chat to confirm or overrule:
  - **I1:** `G-COLL` uses a null that allocates nothing, because the ruled NoApply allocates the same memory as the applied leg.
  - **I2:** mip 0 is drawn straight into the output; drawing it through scratch too would push every 4096² map to 170.67 MiB, over budget.
  - **I3:** the Δ1 check adds a "not loaded from disk" test on top of the two ruled ones.
  - **I4:** the cooked-chunk comparison is a qualification gate only for `CB_GateLevel` and `MainWorld`, and a reading for everything else.
  - **I5:** the mip wrong-copy levers are relied on only on the authored per-mip-colour texture, where they can be proven offline.

Separately: several MCP connectors (GitHub, Slack, Notion and others) need authorising via `claude mcp` or `/mcp` in an interactive session, and the PDF-viewer server failed to connect. None was needed for this brief.
api-equivalent cost (not billed on Max): $31.03 | turns: 134 | running total across all briefs: $2,523.97
