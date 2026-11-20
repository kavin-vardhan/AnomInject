# SESSION: new
# EFFORT: xhigh
# NAME: 069 m47 shader-readiness (run)

SESSION 069 (chat thread A), BRIEF 25 — m47 UNBLOCKED: rulings on 069-24, then run Tasks A and B. Fresh side session: bootstrap from CLAUDE.md, docs/CHAT-HANDOFF-m44-m46-shipped-europa-phase0.md, and the 069-24 report at _mailbox/to_chat/069-24-m47-editor-shader-readiness-and-black-frame-gate.report.md (read it first — it carries Task A3 and the engine read). Codename discipline. Do not adopt this session as the mailbox default.

RULINGS:
 ND-1: chat rules the checkout. europa-e1 (1f5e305) == origin/europa-e1 and the tree is clean, so per CLAUDE.md's shared-tree rule: `git checkout master`, `git pull --ff-only`, work on master; at the END of this brief `git checkout europa-e1` so thread 070 finds the checkout as it left it. State both directions with SHAs. NEW SHARED RULE for CLAUDE.md (land it in this brief's docs commit): "Two chat threads share this tree. Every brief begins by checking out the ref it needs (never assumes the checkout), verifies it == origin, and leaves the checkout as it found it. Bench legs never run while another brief is running (the watcher's serial queue is the guarantee); before any leg, verify the staged exe hash against _binary_baselines/README and re-stage if another thread moved it."
 ND-2: YES — `frames_shaders_pending == 0` in packaged is a READING; the packaged claim rests on B3's pixel gate (G146/m19 "gate on pixels").
 ND-3: YES — add the mask-on-black-frame discriminator to A1: on any whole-frame-black frame, read the m43 target mask; correct silhouette ⇒ blackness lives in lighting/tonemap/exposure, not geometry or capture.
 ND-4: YES — A1 step 0: run tools/set_material_usage_flags.py headless; if it changes the assets, that IS a finding (flags were not set on disk) — commit the asset change as part of m47 with the report line; if unchanged, say so.
 Also test the competing hypothesis explicitly: DEFAULT_LIT with no emissive under zero/unconverged light renders black. Discriminators in A1: (i) `r.DynamicGlobalIlluminationMethod`/Lumen state and `r.Lumen.*` convergence at the black frames; (ii) a bench lever `IAI.Bench.SwapMaterialEmissive 1` (console-only, default off) that swaps in an UNLIT-emissive variant of the two materials for the leg — if the black first frames vanish under emissive and persist under lit, it is lighting, not shaders; (iii) shader-pending count per frame as already specified. Report which of the two mechanisms each symptom (1)/(2)/(3) lands on, with the numbers, or UNKNOWN + next discriminator. No mechanism asserted without its discriminator.

TASK A (as in 069-24, editor target then packaged), TASK B (as in 069-24 with ND-2 applied). Keep the whole-frame-black threshold derived from the bench's darkest legitimate frame and state it. B3 is net-new code in verify_capture.py — keep it small and tested against a synthetic black frame (prove it can fail).
IF the lighting hypothesis wins for (2)/(3): B1's pre-warm stays (it fixes (1)), and the fix for (2)/(3) is a design question — report options (e.g. an unlit-emissive variant with a Lumen-safe colour, or accept-and-mark via the pixel gate) and STOP before choosing; chat decides.

Commit/merge/docs as in 069-24 B5 (plus the shared-tree rule in CLAUDE.md, G231 shader on-demand compile, G232 if the lighting mechanism is confirmed). NO TAG (… m46 → m47). End: `git checkout europa-e1`. Bash may be non-functional in a side session (C:\ClaudeTemp) — use PowerShell as 069-24 did.

REPORT: A1/A2 tables, the per-symptom mechanism verdicts, usage-flag on-disk result, gate table, master tip == origin, the checkout restored, NEEDS-DECISION. STOP.
