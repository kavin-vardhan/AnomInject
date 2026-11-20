# 092-03b ruling: Proof A stopped on a Drop 0 onset transient

Date: 2026-10-05 ~16:45 IST. Author: Claude Code Desktop (orchestrator). Report: `_relay\runs\2026-10-05-092-03b-vt-proxy-blur-proof\report.md` (Astra max, 5,105 s, NEEDS-DECISION). Local branch `feat/vt-proxy-blur` @ `465f2e7` (7 commits on `d7eee97`), unpushed. Build host restored to `23098fa`.

I read the full report, `identity-result.json` and frames 100/123/124/126/127/128/260/400 of `runs\identity-pie-01`.

## Findings accepted
- **The exported-constructor route works.** A genuine private VT (Drop 0, 2048², tile 128, one resource and one producer) binds through the host's compiled VT sampler.
  - Steady state renders within the null: late bound frames reach a maximum MAD of 6.594 against a limit of 6.629.
  - Restore is clean: maximum 6.579, with resource/producer counters 1/1 → 0/0.
  - Co-user and ordinary controls stayed within their limits on every frame.
- **The failure is real and correctly stopped on.** Frames 124–127 (0.133 s at 30 fps) peak at MAD 11.68 against a limit of 6.63. By eye the target's fine grain goes soft and returns at frame 128. This fits a freshly registered producer whose pages are not yet resident, so the sampler falls back to coarser levels. No page trace proves it.

## Ruling
1. **The onset transient is not accepted as-is. Prefetch before bind.**
   - Register the private VT and request and load its pages for the levels the target needs, using the public `IRendererModule` tile-request and pending-load API (see 092-01 §2a; verify signatures in 5.1.1).
   - Confirm residency, or another observable readiness signal, with a bounded wait, before installing the MID binding. The target must keep rendering the original binding while it waits.
   - Re-run the Drop 0 identity under the UNCHANGED frozen criterion: every frame including the bind and restore edges, limit = null + 0.05 MAD.
   - The prefetch acts only on the private producer. It must never force or alter the source's residency.
2. **Fallback, pre-authorized only if a genuine prefetch attempt cannot remove the transient:**
   - Continue to the Drop > 0 legs. Event onset stays at bind.
   - The transient frames must change in the anomaly's own direction (coarser than baseline). Record their count and magnitude per event.
   - E1 must show 0 unflagged edges and annotation == labels.
   - Report identity honestly as "steady-state pass, onset transient N frames". Do not call that an identity pass.
3. **Fixture fixes** (geometry and content, never rule changes):
   - Use a high-contrast, high-frequency target texture (fine checker or text) so blur is unambiguous.
   - Capture at a real 1920×1080: a standalone or new-window PIE at that size, or staged.
   - Satisfy the existing selection rules for Auto legs (the 25% coverage ceiling and the 8× ratio against the effective virtual width) by plane size and camera placement.
4. **Owner item (flag, do not change):** with this route the coarsest blur is one VT tile (128 px). Under the existing 8× rule, an Auto-picked target must be about 1,024 px or wider on screen, which will limit yield on the first office host. Measure and report the ratio at pick and the visible change per event. Any change to the rule is the owner's decision. No threshold is proposed here.
5. **Everything else in 092-03 still stands:** production `proxy_vt` integration, routing, census, telemetry, positive PIE and staged legs, revert and lifetime, can-fails, regression, gates and push. Resume as 092-03c from `465f2e7`.

Chat review status: per AGENTS.md this is Codex's convention. This ruling resolves an implementer NEEDS-DECISION at orchestrator level. The owner item in 4 is flagged, not decided.
