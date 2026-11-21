# Bates mip-map corruption triage — 2026-10-02

Owner update: UV and normal targets are now getting picked up on one Bates build; another Bates workspace has not been tested (2026-10-02). This establishes reported selection success, not new pixel/label certification. Office commit is confirmed as the same88801bd. **The mip failure's dominant admission blocker is now confirmed: shared-texture purity.**

Typed office readings: purity enumeration7; HOLDING1; HELD NONE6; failed-attempt buckets not streamable4, excluded4, shared164; shared_world196; user-count bins2–8:32,9+:164; minimum/maximum users2/6155. Unreported buckets are not invented as zero.

Interpretation: seven enumeration records, one reported hold and six attempts holding nothing. A HOLDING record establishes acceptance/requested hold, not a measured mip drop, visible blur or correctly labelled frames. The counters therefore do not establish literally zero accepted attempts. Sharing dominates the reported rejection counts. `shared_world196` counts per-texture refusal log entries across attempts; the shared164 bucket belongs to failed HELD NONE summaries. Successful/partly successful attempts can also refuse individual shared textures, so the totals need not match. Neither total is a unique-object or unique-texture census. The maximum6155 is component users of a rejected texture in the loaded world, not6155 measured visible objects.

**Chat review NOT REQUIRED** for this read-only diagnosis. **Chat review REQUIRED** before changing the mip anomaly's eligibility/effect contract or lifting capture/release holds; that is the next mandatory checkpoint. This is the same office compatibility workstream.

Source inspected at88801bd: `Anomaly_StuckLowMip.cpp`, its defaults and helpers; preserved in the earlier `_reviews/m53-88801bd-review/source/` snapshot. This file has no difference between88801bd and inspected local9b911e7. The UV/normal resident-copy rework did not redesign mip eligibility.

## Mechanism and restrictions

UV/normal use private transformed texture copies. `stuck_low_mip` changes the original texture's streaming state. Its per-texture conditions include:

- Exactly one component user in the target world's loaded levels, including inactive/unregistered users. Two components of the same actor still fail this count. The check protects against affecting other users of the shared resource. It applies to targeted and automatic fires. `StuckMipMaxCoAffected` is not an override for the current strict rule; it belongs to the legacy bench path.
- Texture must be a supported, non-virtual streamable texture, with resident levels that can actually drop. Existing bias can leave a texture already at the reachable floor.
- No pending baseline transition or previous unfinished restoration.
- Automatic selection requires a projected-size / remaining-texture-width ratio of8 by compiled default. For a texture whose deepest achievable top mip is64px, the projected target must be512px along its longest side. That64px example is not a universal texture floor. This is a pick-time proxy, not a direct pixel visibility measurement.
- The current implementation's deepest hold is `max(engine NumNonStreamingLODs, 4)`. The four-mip minimum is a plugin policy, not an engine limit. The diagnostic overstates the engine restriction where the engine floor is lower than four. This is a secondary source issue, not a cause established by the supplied sharing-dominated readings.
- Runtime streaming must be enabled and UseAllMips must not defeat the per-texture bias. Some such failures occur after Apply, so distinguish no candidate, refused Apply and an applied hold that never becomes effective.

The shared-texture condition is established by the new office readings; the other conditions remain possible later blockers and are not promoted to causes. Sharing is tested before the baseline/floor/ratio checks, so the reported histogram cannot establish that every shared texture would otherwise qualify. Previous home investigations specifically refuted the idea that editor-world duplicates caused the purity refusal: the fixture intentionally had two users in the same PIE world. Do not revive that explanation without new evidence.

The full090-10b2 report records working home mip corruption on its rock fixture (PIE17/17, staged16/16 and automatic5/5). These are Code-reported m52-specific results, not new office evidence or a new independent pixel recomputation this turn. That report explicitly left the office mip failure unexplained pending counts.

## Reading already obtained; no further office test needed to identify this blocker

`IAI.TexCorrupt.Census` covers UV/normal, not `stuck_low_mip`.

The owner supplied the mip-specific count summary above. For future reference, the existing `stuck_low_mip` line containing `HELD NONE` carries numerical buckets: virtual; not streamable/already at floor; excluded group; shared; too small; imperceptible. If that line is absent in another case, the `Auto.Yield stuck_low_mip` reason or absence of any mip attempt is meaningful; do not infer zero refusals from missing lines.

The shipped alternative is a read-only extractor, run from the plugin directory on the office PC:

```powershell
python -B .\tools\anomaly_refusal_counts.py "<existing editor log path>"
```

It includes mip counts and upstream Auto outcomes. The smaller `m52_log_counts.py` alone omits upstream selection/exclusion information. Office logs, names and paths remain on that PC; only generic reasons/counts come back. Whole-log counts are cumulative attempts/texture refusals, not a unique-object census.

## Concrete corrective direction

**For useful per-object visual blur, recommend an isolated lower-resolution texture-copy mode using the material substitution path now qualifying UV/normal objects.** It can leave the shared original texture and its other users unchanged. It must retain a coherent source snapshot, sampling/material correctness, bounded allocation, exact restoration and truthful labels; the already-reported88801bd source-snapshot/component-scope defects must be corrected before relying on that path for capture correctness. This is a concrete extension for supported material bindings, not a promise to support every material or every previously rejected candidate.

A private-copy blur is a distinct implementation from an actual streaming hold and inherits material-binding support limits. Give it explicit mode/anomaly semantics; do not falsely report that the original asset's resident mip count was reduced. Validate blurred pixels against a clean control on a shared texture, require unchanged neighbouring users, and prove Auto selection/application/restoration with correct labels.

If the requirement is specifically to manipulate real streamer residency, keep that method for textures with isolated ownership or implement complete labels/masks for all affected users. Allowing multiple components wholly inside a proven target set could recover a smaller class, but does not solve widespread sharing across other actors. At6155 possible component users, simply removing the purity gate can affect many unlabelled surfaces. Raising the old visible-coaffected setting does not override this production rule.

Add mip-specific eligibility preflight to Auto so it avoids repeatedly selecting actors that the mip implementation will refuse. Unlike the UV/normal selection path, the current mip path checks these textures only inside Apply. Preflight improves selection efficiency; it cannot make shared textures eligible under the existing mutation model.

The ratio/floor and readiness restrictions remain separately assessable after sharing is addressed. A wider label/visual claim cannot be obtained merely by making the deepest hold lower than the engine allows. Do not change global streaming or quality settings merely to hide a rejection. **Chat review REQUIRED** for adopting the proposed mode/sharing/label contract; no implementation or gate bypass is released by this diagnosis.

## Live ownership

Code090-10f2 resumed at10:04:08 according to the watcher. Its worktree was in a documentation merge (CLAUDE.md/gotchas conflicts) during this read. It owns that in-flight work; this audit did not resolve, edit or interrupt it and dispatched no competing brief. All source reads use pinned88801bd. The prior88801bd copy/mask correctness findings and dataset hold remain visible despite the new office eligibility success.
