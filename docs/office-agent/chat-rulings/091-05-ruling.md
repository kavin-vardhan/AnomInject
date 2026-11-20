# 091-05 — Ruling (ACCEPTED): the independent review's F1, F2 and F6 are fixed and F3 is part-fixed, all proven in-engine with can-fails. **F1 was real.** Office/delivery candidate: `feat/m52-proxy-blur` `5d6352b`

Date: 2026-10-04 ~08:45 IST · Author: Claude Code Desktop (orchestrator/reviewer) · Implementer: Codex `gpt-6-astra` / `max`, full access · Run `_relay\runs\2026-10-04-091-05-review-fixes\` (03:23–08:21, VERDICT GREEN) · Basis: `_reviews\091-04-ruling.md`

**Heads (pushed fast-forward, verified):**
- proxy `5d6352b` (Source = proved `dc42e35`, exe `3ADC2EA4`)
- m53 `4c93e5f` (= proved `839f429`)
- fix `1eaf205` (= proved `97c20f2`)

master `b5f15a3` is untouched.

## Findings: fixed and proven
- **F1, capture ownership.** The colour SVE path and the mask arms accept a view family only if **both** its scene and its render target or viewport match the capture's owner (`AnomalyCaptureOwnership.h`). This is checked before dequeue, in `IsActiveThisFrame`, `BeginRenderViewFamily` and `AfterPass`. A missing bound fails closed. Family IDs key the SVE ring, so frame-number aliasing is gone. A skipped owner render ends as `capture_unpaired` (dropped, counted). Pure tests: ownership 19/19 and mask 16/16, with the review predicate, fail-open, scene-only, stale, no-expiry and frame-alias mutants all failing.
  - **Engine proof:** PIE in a new window with two realtime editor viewports and a game-only marker. New build: UV 144 and proxy 144 labelled frames, **0 wrong pictures**, colour rejects 2,753 and mask rejects 1,849 / 1,465, 0 missing masks, 0 unflagged t50/t10.
  - **Can-fail, reproduced:** the old `C7FB5212` (`d660b5d`-equivalent) labelled **144 editor-view pictures** per leg (942×869, no marker). I checked the evidence files myself.
  - Skipped-render controls drop exactly the skipped indices as unpaired, with masks reconciled exactly.
- **F2, positive changed-component scope.** UV/normal always installs the scope of the components it actually changed; excluded components release still-owned stencil tags. Fixture C1 (eligible) plus C2 (null material, engine default): mask, m26 and both boxes on C1 only; 0 C2 mask pixels; C2 RGB within the clean noise envelope + 0.05 in PIE and staged. Can-fail: the old build masks C2 on 144/144 frames.
  - **Accepted amendment:** C2's exact byte equality failed by 1–2 levels that also appear in clean frames. The gate became the existing clean-noise envelope + 0.05, plus 0 pixels over 12 levels, with a failing 20-level control. Declared before the final legs. Accepted.
- **F3 (part), unused alpha.** The UV uniformity rule judges RGB only unless the material is masked/translucent or has an opacity input connected. Opaque alpha-only texture: refused `texture_uniform` (0 events) in PIE and staged. Masked: 3 events, 144/144 frames visibly changed. Can-fail: the old build labels the opaque case with no visible change (max delta 1). The residual is in readme §8.8.
- **F6, checker identity.** Every labels-side event identity is cross-checked; the review's wrong-ID control fails both the CLI and the hook.
- **F4/F5:** readme only (selection heuristics; copy-snapshot robustness limit).

## Regression on the final proxy build (`dc42e35` / `3ADC2EA4`)
- Shared proxy PIE/staged TSR, UV/normal PIE/staged TSR (now on a co-user fixture), proxy AA-off PIE: 3 fires each, 0 unflagged t50/t10, t10 ≤ 27 (inside 40), 0 missing masks, co-users unchanged.
- Office kit: 3/3, start/end 0, wrong object 0.

## My independent checks
| check | result |
|---|---|
| origin heads / master | as above / unchanged |
| pushed Source/Shaders/descriptor/tools versus the proved commits | identical on all three |
| source diff `d660b5d..5d6352b` (19 files, +350/−55): the ownership guard, MaskServe, colour path, scope, alpha rule | read; correct and narrowly scoped |
| annotation.json versus labels.jsonl, my own script over every 091-05 capture (old controls included) | **125 events, 0 mismatches, 0 orphans** |
| release archive `09105-dc42e35-3ADC2EA4.zip` | SHA-256 matches the report |
| lever audit on the release exe | 53/53 PASS |
| F1 old/new evidence (`old_P_uv_owner_final` / `P_P_uv_owner_final`) | old: 942×869 frames, no marker; new: 1280×992, marker present |

## Consequences
1. **F1 also affects the build the client has now** (master; the SVE path dates from m25). PIE captures taken while another realtime editor viewport rendered could contain editor-view frames labelled as the game view. Packaged/standalone captures have one viewport and are not affected. **Owner: product/communication decision** (whether to tell the client and advise re-capture or filtering of PIE sessions from the old build).
2. **Fail-safe side of the guard:** a host that renders its game through an unusual viewport setup would get dropped (`capture_unpaired`) frames rather than wrong labels. The office check must confirm that captures still produce frames (run_summary counts).
3. **Residual F3** (unsampled packed channels, zero-strength normals) goes to the owner as a documented limitation; the follow-up is a measured visible-change gate.
4. **Next (Job F):** after the owner's ship decision on blurry, merge the chosen head to master; run the regression of every delivered anomaly (blinking, missing_object, missing_texture, corrupted_texture, lod_popping, camera_clipping, uv, normal, blurry if shipped) on the master build, PIE and staged, because F1 and the 40 window touch every anomaly; then the Development package and the rehearsal install.
