093-01 — RESEARCH (read-only): natural-cause anomalies — can each bug be produced through its real cause in UE 5.1? Requested by chat Claude (the orchestrator) on 2026-10-13, after the client asked at the workshop that anomalies happen the way they do in shipped games — produced by the real reason the bug occurs, not painted on — wherever possible.

This is a design and feasibility study, not an implementation. The delivered build (release/2026-10-05 @ d7eee97) is frozen and is not touched by this job.

INPUT — READ FIRST
- The design draft, r2: D:\IntrusiveAnomalies\_reviews\drafts\093-00-natural-causes-design.DRAFT.md. It defines:
  - "natural": we change the cause, the engine produces the symptom, and labels are measured from what the engine actually did;
  - the three cause families and the realism ladder R0–R3;
  - where today's anomalies stand (§4) and the label approach (§5);
  - a 40-entry candidate catalog (§6), the constraints (§7) and the proposed order (§8);
  - the client's answers (§1a).
  Every UE API, cvar or engine behaviour the draft names is a hypothesis marked (verify). Confirm or correct each one you touch.
- The client's answers bind this study (draft §1a):
  - every natural injection must produce something visible on screen, on an object we aim at;
  - bugs stay frequent (we decide when and how often);
  - today's controlled anomalies stay;
  - a cause label is good to have, not required.

WHERE TO READ (all read-only)
- Delivered plugin code: a clean worktree at release/2026-10-05 @ d7eee97. Claude Code adds its path below.
- For virtual-texture context only: feat/vt-blur-stacks @ 5dc8717, and the VT reports _reviews\092-01-*.md and _reviews\092-04-*.md.
- If git reports "dubious ownership", pass -c safe.directory=* on that command only. Never change any git config.
- Engine: Unreal Engine 5.1.1 source at D:\UESource\UnrealEngine. Use it for every claim about engine behaviour.
- Shipped behaviour: docs/client-readme.md in the release worktree.
- The four-rule principle any design must respect: D:\IntrusiveAnomalies\_reviews\090-10x-chat-ruling-compatibility.md.

FACTS ABOUT THE TARGETS (owner-stated; no office data is available or needed)
- Neither office host uses Nanite. The first office host is virtual-texture heavy.
- Office capture runs in PIE. The client receives Development/Test packaged builds; the plugin is excluded from Shipping.
- Captures are often hands-off (a static or near-static camera). Say so wherever an entry needs camera or player motion.

QUESTIONS
For every answer, give file:line evidence (plugin and engine) and mark each statement VERIFIED-FROM-SOURCE or INFERRED.

Q1. Catalog table: one row per catalog entry in draft §6 (all 40), with these columns:
- exists in 5.1 (file:line);
- reachable from a game-agnostic plugin (Core/CoreUObject/Engine only; public or exported API; no engine edits);
- thread;
- editor (PIE) vs cooked/staged differences;
- exact restore path, with no host asset dirtied or saved;
- scope (one object / every instance of an asset / global);
- needs motion?;
- can it be aimed so the effect lands on a chosen on-screen object, reliably and frequently?;
- office relevance (no Nanite; VT);
- effort S/M/L;
- main risks;
- verdict: FEASIBLE / FEASIBLE-WITH-CONDITIONS / NEEDS ENGINE CHANGES / NOT WORTH IT.

Q2. Probes for the five pilot candidates (MISS-1, MISS-2, BLUR-1, BLUR-2, GONE-1). Give the exact public API that reports, per object and per frame, what the engine actually did:
- the material and texture bound;
- resident mips against a pre-arm baseline, for every texture of a visible object;
- drawn or not this frame.
For each, give its thread and latency, and how it can be keyed to the plugin's capture frames. Name the plugin code that already does part of this (for example the stuck_low_mip render-thread residency observer), and say what is missing.

Q3. Re-check draft §4 (where today's anomalies stand) against the release source. Correct any mechanism or level.

Q4. Engine fallbacks in 5.1, PIE vs cooked, with file:line:
a. A material that fails (missing usage flag, shader compile failure, missing shader map or permutation): what renders, and does PIE auto-fix it?
b. A material-instance texture parameter set to null through a MID: is it "not overridden" (the parent's value) or a real null?
c. A null or resource-less texture at uniform-buffer fill: which engine texture is substituted?
d. A missing normal map.
e. The asset paths of the engine's default material, default checker texture and flat normal, and whether they are always cooked.

Q5. Streaming in 5.1:
- Which r.Streaming.* and r.VT.* cvars exist? Which are safe to change at runtime and restore exactly (value and set-by priority)?
- Does a pool squeeze print on-screen messages? If so, how do we keep them out of captured frames only?
- Can a squeeze be aimed so the textures of a chosen on-screen object lose mips first, or will the engine's priority drop off-screen mips first?
- Can resident mips be read against a baseline for every texture of a visible object?

Q6. Name plainly every catalog entry that cannot be done without engine changes.

Q7. Pilots. Rank the catalog by these criteria:
- reliably visible on an aimed on-screen target;
- frequent;
- works with a static camera;
- reuses existing plugin machinery;
- effort.
Then confirm or amend the draft's §8 pilots (MISS-1, MISS-2, BLUR-1, GONE-1), with reasons.

Q8. Label keys. Propose additive keys for natural events (cause record, effect events on several objects, severity, injection_mode) against the shipped schema, without changing any existing key's meaning. For each key, say:
- where it is written (annotation.json, labels.jsonl, run_summary);
- what a consumer that ignores it still sees.

Q9. Masks for objects the engine stops drawing (GONE-1, GONE-2, GONE-4).
- Blinking mode 1 keeps custom depth so its mask still renders. Propose a measured-mask route for an object that is distance- or occlusion-culled.
- Check whether the plugin's visibility layer depends on "was rendered". If it does, it would be blind to these bugs.

Q10. Interactions with the capture safeguards:
- the shader-pending rule;
- census selection;
- the m26 veto;
- the label transition flags;
- the view-ownership guard in the follow-up work (feat/m52-proxy-blur, 091-05 F1; not in the release).

Q11. For each pilot candidate:
- how to make the effect reliably visible on the aimed target (strength, timing);
- how quickly "no visible effect" can be detected, so the cause is reverted and not counted;
- the exact revert.

DELIVERABLE
review.md in your run folder, in this order:
1. A one-screen summary: what is feasible soon, what needs motion, what needs engine changes, and the pilot order you recommend.
2. The Q1 table.
3. Q2–Q11.
End with one line "VERDICT: <summary>", for example "VERDICT: MISS-1, MISS-2, BLUR-1 FEASIBLE plugin-only | GONE-1 FEASIBLE-WITH-CONDITIONS (mask route) | engine changes: <ids>".
