# REPORT for 069-24-m47-editor-shader-readiness-and-black-frame-gate.md
brief: 069-24-m47-editor-shader-readiness-and-black-frame-gate.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-03 17:24:01 | finished: 2026-09-03 17:31:35 | duration: 8 min | exit code: 0
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\069-24-m47-editor-shader-readiness-and-black-frame-gate.live.txt

---- FINAL MESSAGE (Claude Code) ----
Everything checkout-independent is done. Repo untouched — no commits, no adds, no build, no staged-exe change; reads only.

```
SESSION 069 / BRIEF 24 (m47 shader-readiness) — HALTED AT THE OPENING GATE.
TASK A3 DELIVERED IN FULL. A1/A2 AND ALL OF TASK B ARE BLOCKED AND NOT STARTED.

=====================================================================
1. THE GATE FIRED — 2 OF 3 CONDITIONS. NOTHING WAS TOUCHED.
=====================================================================
  branch              = europa-e1          (brief requires master)          -> FIRES
  HEAD                = 1f5e305            != origin/master 489c29d          -> FIRES
  tracked mods        = NONE                                                -> clean
  untracked           = the 7 owner docs/CHAT-HANDOFF-*.md (permanent, never staged)

NOTHING IS AT RISK: europa-e1 == origin/europa-e1 == 1f5e305 (pushed, verified via
git ls-remote). master == origin/master == 489c29d. Session 070's work is safe on the
remote; the checkout was simply left parked on its branch.

I did NOT switch, build, stage, cook or run a leg. No commit, no git add. Reads only.

WHY I STOPPED RATHER THAN SWITCHED — the two rules disagree and I took the stricter:
CLAUDE.md's shared-tree rule PERMITS the switch here (no tracked mods; branch being left
== its origin). The brief's gate is stricter and I treated it as governing. The standing
rule protects against LOSING COMMITS; the brief's gate additionally protects the SHARED
PHYSICAL BENCH (staged exe + cooked container), and whether 070 is still live there is
invisible from git. A1/A2 are bench legs, so this is not a formality.

UNBLOCK (one line, owner/070's call — I did not run it):
    git checkout master        # from D:\...\Plugins\AnomalyInjector
then re-issue this brief. If 070 wants the checkout back afterwards: git checkout europa-e1.

RE-EXECUTION CHECK (per standing habit): no m47 exists on any ref, local or remote.
This brief has NOT run before. Safe to run once unblocked.

SCOPING BONUS: E1 phase A (1f5e305) touched only 4 *.Build.cs, AnomalyHiddenClass.cpp
(+6 lines) and docs. It did NOT touch either anomaly source, so the A3 read below is
byte-identical on master and needed no checkout change.

=====================================================================
2. TASK A3 — DONE. Three parts: (a) and (b) ANSWERED, (c) needs A1.
=====================================================================

(a) ARE THE SWAP MATERIALS REFERENCED AT MODULE START?  YES — BUT IT BUYS LESS THAN
    THE QUESTION ASSUMES, AND THIS IS THE KEY POINT.
    AnomalyInjectorSubsystem.cpp:36-45 — both materials are CDO hard-refs via
    ConstructorHelpers::FObjectFinder (m8's cook guarantee):
        /AnomalyInjector/Materials/M_MissingTexture_Checker
        /AnomalyInjector/Materials/M_CorruptedTexture_Pink
    Both .uasset files confirmed on disk (4,635 B and 15,111 B).
    ==> This guarantees the ASSET IS LOADED. It does NOT guarantee any shader map is
        COMPILED for a given vertex factory. So it does not pre-empt chat's hypothesis.
        A CDO hard-ref is a COOK guarantee, not a SHADER-READINESS guarantee — those are
        different facts and the m8 record only ever claimed the first.

(b) ARE THE USAGE FLAGS RIGHT FOR EVERY MESH USAGE?  YES, AND THE COUPLING HOLDS.
    7 flags on both materials, shared constant in tools/create_anomaly_materials.py
    (USAGE_FLAGS) and tools/set_material_usage_flags.py (USAGES) — verified identical, so
    re-authoring cannot reintroduce the m30/Concorde defect.
    set_material_usage_flags.py carries an explicit COUPLING WARNING: "if
    IsRenderableComponent or ResolveLodComponents is ever widened, THIS LIST MUST BE
    WIDENED IN THE SAME CHANGE." I CHECKED IT TODAY — NOT TRIPPED:
        AnomalyLod::ResolveLodComponents   -> UStaticMeshComponent + USkinnedMeshComponent
        IsRenderableGeometryComponent:614  -> IsA<UStaticMeshComponent> || IsA<USkinnedMeshComponent>
    Byte-for-byte the premise the 7-flag list was derived from. The 7 remain correct.

  ** LIMIT, STATED NOT PAPERED OVER: this proves the flag LIST is right. It does NOT
     prove the flags are SET on the assets right now. That needs the editor, and the
     obvious shortcut is closed — G159 already REJECTED a uasset string-scan for exactly
     this (its control failed: a known-TRUE flag was equally absent => blindness, not a
     negative). I did not run set_material_usage_flags.py because it SAVES assets and
     would dirty tracked binaries under the gate. It is idempotent and reports full state,
     so it is a clean first step once unblocked. **

(c) EDITOR LOG "missing bUsedWith..." LINE — BLOCKED, requires the A1 leg.

=====================================================================
3. ENGINE SOURCE READ (UE 5.1) — THE HYPOTHESIS IS REAL, BUT ITS
   PREDICTED COLOUR IS GREY, AND THE OWNER REPORTED BLACK.
=====================================================================
SOURCE-READ, not measured. MaterialShared.cpp:4207-4229,
FMaterialRenderProxy::GetMaterialWithFallback:

    if (!Material || !Material->IsRenderingThreadShaderMapComplete())
        walk GetFallback() until one IS complete
        BaseMaterial->SubmitCompileJobs_RenderThread(EShaderCompileJobPriority::Normal)

==> CONFIRMED: an incomplete shader map draws a FALLBACK and KICKS THE COMPILE FROM THE
    RENDER THREAD AT DRAW TIME. That is on-demand compilation at first use, exactly as
    chat hypothesised, and it is now source-grounded rather than assumed.

==> BUT THE FALLBACK CHAIN TERMINATES AT UMaterial::GetDefaultMaterial(Domain)
    (MaterialInstance.cpp:206; Material.cpp:419-428) = THE ENGINE DEFAULT GREY GRID.

    So the mechanism predicts:
      symptom (1) "swap not visible on first frame(s)"   -> CONSISTENT (grey grid, not pink/checker)
      symptom (2) "target renders BLACK/dark"            -> NOT EXPLAINED. Grey != black.
      symptom (3) "whole picture black for a burst"      -> NOT EXPLAINED. A per-material
                                                            fallback cannot blacken a whole frame.

    I am NOT calling this a refutation — a grey grid in a dim scene can read as "dark",
    and n=0 measurements exist. It is the discriminator A1 must settle. Per the standing
    invariant I am recording the observation and the mechanism as separate claims.

  ** COMPETING CANDIDATE FOR (2)/(3), HYPOTHESIS ONLY, NOT A FINDING — but it fits the
     colour where the compile story does not, and it is cheap to separate:
     BOTH shipped materials are MSM_DEFAULT_LIT, BLEND_OPAQUE, two-sided, driven by BASE
     COLOUR ONLY WITH NO EMISSIVE (create_anomaly_materials.py:99-105, 119-121 — Lit was
     deliberate, G50: unlit-emissive magenta bled onto neighbours via Lumen).
     A Lit opaque surface receiving ZERO light renders BLACK. A freshly swapped material on
     a statically-lit mesh, or one Lumen/GI has not yet converged for, is a black-then-
     recovers shape — which is what the owner described, and which the grey-grid fallback
     is not. NO MECHANISM ASSERTED. **

DISCRIMINATOR I RECOMMEND ADDING TO A1 (uses infrastructure that already exists, no new
code): on a whole-frame-black frame, READ THE m43 TARGET MASK. Custom depth/stencil is
written independently of lighting and tonemapping. If the mask is still correct (non-zero,
right silhouette) on a black frame, the geometry drew fine and the blackness lives in
lighting/tonemap/exposure — NOT in the scene or the capture path. That splits (3) in one
read and costs nothing.

=====================================================================
4. TWO FINDINGS THAT CHANGE TASK B BEFORE IT IS WRITTEN
=====================================================================
B-FINDING 1 — THE API IS SAFE AS SPECIFIED. I nearly reported the opposite and checked.
  GShaderCompilingManager is declared extern ENGINE_API (ShaderCompiler.h:1154) and
  GetNumRemainingJobs() exists as a public inline (ShaderCompiler.h:1034-1037).
  ShaderCompiler.cpp:3535 defines it = nullptr, which looked like a packaged null-deref —
  but LaunchEngineLoop.cpp:2885-2900 allocates it in BOTH branches of
  if (AllowShaderCompiling()) ... else { /* create a manager, but it won't do anything
  internally */ ... }.
  ==> NON-NULL IN PACKAGED TOO. B1/B2 are implementable exactly as the brief words them
      ("editor and packaged alike, harmless in packaged"), no #if WITH_EDITOR needed.

B-FINDING 2 — B2's PACKAGED GATE WOULD BE VACUOUS, AND THAT IS A G146 REPEAT.
  Because the packaged manager "won't do anything internally", GetNumRemainingJobs() is
  STRUCTURALLY 0 in a packaged build. So B2's "in packaged builds this must read 0" passes
  BECAUSE THE CONDITION CANNOT OCCUR, not because it was tested — the exact shape of G146
  (GATE D passing on an empty run) and of the "gate that has never fired is not a gate"
  rule.
  ==> RECOMMENDATION: keep frames_shaders_pending == 0 as a READING, and let the packaged
      leg's real evidence be B3's PIXEL gate (black frames 0, dark first frames 0). That
      keeps the packaged claim resting on pixels, per m19's standing "gate on PIXELS, not
      on a counter."

B3 SCOPING: tools/verify_capture.py exists but contains NO luma/luminance/mean/black
machinery. B3 is net-new code there, not an extension of something present.

=====================================================================
5. WHAT THE BRIEF ASKED FOR THAT I CANNOT SUPPLY
=====================================================================
  A1 correlation table (frame, pending, target-lum, frame-lum, verdict) ... NOT RUN — gated
  A2 packaged correlation table                                          ... NOT RUN — gated
  A3(c) editor "missing bUsedWith" log read                              ... NOT RUN — needs A1
  black-burst explanation with the naming log line                       ... UNKNOWN — see the
      discriminator in §3; I am not guessing a mechanism (standing invariant)
  B1-B5, gate table, commit, merge, master tip                           ... NOT STARTED — gated
  master tip                                                             ... UNMOVED at 489c29d

=====================================================================
6. NEEDS-DECISION
=====================================================================
 ND-1 (BLOCKING, procedural): who restores the checkout to master. The brief says do not
      touch another thread's work, so I did not run `git checkout master` even though
      CLAUDE.md's shared-tree rule would permit it (nothing is unpushed). Say the word and
      I will do it and state both directions with SHAs, or 070 does it.
 ND-2 (design, before B is written): accept B-FINDING 2 — demote the packaged
      frames_shaders_pending == 0 to a reading and make B3's pixel gate the packaged
      evidence? Recommend YES.
 ND-3 (scope, cheap): add the "mask-correct-on-a-black-frame" discriminator to A1? It
      separates symptom (3) with zero new code. Recommend YES.
 ND-4 (honest limit): the flags' CURRENT ON-DISK state is unverifiable from this box
      (G159 closes the string-scan route). Run set_material_usage_flags.py headless as
      A1 step 0? It is idempotent and would have dirtied tracked assets under the gate,
      which is why I held off.

NOTE: Bash tool is non-functional this session (per-call cwd bootstrap file never
materialises under C:\ClaudeTemp). All work above used PowerShell + the file tools. Worth
knowing before the next session plans a shell-heavy leg.

STOP.
```

**Summary of what happened:** the brief's opening repo-state gate fired — the shared checkout is parked on session 070's `europa-e1` branch, one commit ahead of `origin/master` — so I stopped before any build, leg, or repo write. Session 070's work is safe (fully pushed); nothing is at risk.

I delivered the part that didn't depend on the checkout. **Task A3 is complete**: the materials are CDO hard-refs (a *cook* guarantee, not a *shader-readiness* one — so it doesn't pre-empt the hypothesis), and the 7 usage flags are still correct because the reachable component set has not widened.

Two things worth your attention beyond the gate:

- Chat's hypothesis is **confirmed as a real mechanism** from 5.1 source (incomplete shader map → fallback + compile kicked from the render thread at draw time) — but it predicts the engine **grey grid**, while the owner reported **black**. Both our materials are `DEFAULT_LIT` with no emissive, so zero light renders black; that fits symptom (2)/(3) where the compile story doesn't. I've flagged it as a hypothesis, not a finding, and proposed a zero-cost discriminator using the existing m43 mask.
- **Task B has a latent vacuity problem**: `frames_shaders_pending` is structurally 0 in packaged builds, so that gate would pass because the condition can't occur — a G146 repeat. The packaged evidence should rest on B3's pixel gate instead.
api-equivalent cost (not billed on Max): $7.35 | turns: 31 | running total across all briefs: $557.28
