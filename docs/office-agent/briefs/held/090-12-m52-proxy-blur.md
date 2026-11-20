# ACK-VERDICT: (chat fills in at release)
# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 090-12 blurry textures via a per-target private copy ("proxy blur") on a separate branch (Code, Opus 5.5)

# 090-12 — The owner's decision (2026-10-02, option 2): build a per-target **private-copy blur** for stuck_low_mip that blurs **only the target**, even when its texture is shared by thousands of objects. It reuses m53's proven resident-chain copy and MID binding. Separate branch; it goes into Monday's delivery **only if fully proven at home and on an office host by Sunday noon**. In-engine proof in PIE and staged

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

## How your turns end — standing instruction from the owner, the owner
A message with no tool call ends the whole run. Do not end a turn to report progress, to announce the next step, or to offer options. Put status notes in the same message as your next tool call and keep going. End the run only when every step is done, a stop condition fires, or something needs the owner. The final message follows the structure at the end and starts with a `VERDICT:` line. Builds and game runs outlive the tool timeout: run them in the background and poll. **Keep context lean (finish below 70 %).**

**Launches only when the idle gate passes** (the clarified rule: an open but idle foreign editor doesn't block when the person has been idle ≥ 15 min, GPU < 10 %, headroom ≥ 6 GB; never touch it).

## Why (the office readings, numbers only)
`_bates_reads\2026-10-01-second-office-host-m53-census-owner-reads.md`, 10-02 ~10:45, second office host, PIE, m53 `88801bd`:
- stuck_low_mip: PURITY 7, HOLDING 1, HELD NONE 6 (not streamable 4, excluded 4, **shared 164**);
- **shared_world 196: users 2–8 = 32, ≥ 9 = 164**, max 6,155.

Today's route lowers the **shared** texture's resident mips, which would blur every user, so purity refuses. On real content textures are shared, so blur practically never fires. Meanwhile, m53's private-copy route is **proven on that same office host** (uv/normal visibly corrupt).

## The design (chat's ruling)
- **Mechanism:** for each eligible streamable texture parameter on the target's slots, make a private copy that **starts at the held mip `k`**. The copy's mip 0 is the source's mip `k` (the resident chain from `k` down), and it's bound through the target's MID. A smaller texture sampled at the same UVs is exactly the "stuck at low mip" look. Only the target's slots change. **The shared source texture is never touched.**
- **Reuse m53's machinery:**
  - the resident-chain copy (090-10c) with the `r.MipMapLODBias` compensation;
  - MID binding, revert and residue assertions;
  - the installed-state rule (R1/R3) as label authority;
  - B component scope for the mask;
  - the copy ceiling and budget;
  - `texture_uniform` (090-10f2).
- **Hold depth `k`:** the existing m52 depth and perceptibility policy (the ratio wall and the imperceptible checks keep their meaning). Declare `k` per event in telemetry.
- **Route selection:** a setting `IAI.Anomaly.StuckMipRoute auto|hold|proxy` (ini-settable). `auto` (the default) uses **hold** when the texture is pure (today's proven route) and **proxy** when hold would refuse for `shared_world`. Every fire reports its route.
- **Label semantics:** a FireWindow-type label, the same policy m53 uses (proven with AA on and off): labelled while the proxy is installed and rendering on the target, `effect_interrupted` otherwise. **There's no streaming wait for the proxy route**, so the onset is the bind frame. Keep today's hold-route semantics unchanged.
- **m52 ⟂ m53 exclusion stays.** A target can't carry both at once.
- **Identity control (bench only):** a proxy at `k = 0` is pixel-exact to the original (as m53 identity).
- **Census:** blur joins the all-reasons census (`IAI.TexCorrupt.Census allreasons` or a sibling), with eligible counts per route.

## Proof (mandatory, in-engine, PIE and staged)
- **A fixture texture shared by ≥ 20 actors** (plus one shared by > 1,000 instances or components if feasible), with office-like settings (per-texture LOD bias, a texture-group bias, partly resident).
- **auto route:** stuck_low_mip fires on the shared texture's target. **Only the target visibly blurs** (pixel diff on the target above the declared threshold; **every co-user's pixels unchanged**, within null).
  - The label edges pass the existing m52 E1 gate (half-strength edge, 0 frames off outside flagged frames) **and** the 0-frames-off FireWindow edge check. The mask covers only the target's corrupted components.
  - ≥ 7 fires, AA off. Also one AA-on (TSR) leg with its declared transition policy.
- **The pure-texture regression:** the hold route is unchanged (B9/B0/B5 as 090-10b2).
- **Auto-pool on the office-like MainWorld variant:** eligible counts per route; every labelled event visibly blurred.
- **Can-fail:** the current head refuses `shared_world` on the shared fixture with 0 events; a `proxy` route mutant that writes into the shared source must fail the co-user-unchanged check.
- Tests both ways in the pure suites (route selection, the depth mapping, label policy).

## Branch and merge
- **Branch `feat/m52-proxy-blur` off the latest pushed m53 head** (after 090-10f2). **Do not merge it into m53 or master**; chat decides on Sunday.
- Builds, the string scan, strict 0/0, the lever audit, archive. Push the branch.
- **Office errand ready:** the exact pull and the one-command read-out for the owner to confirm on an office host (census plus a visible blur check).

## Hard rules
- **Never change any security setting.**
- Never kill anything that is not yours, and never by name. Close only what you launched, by PID.
- Delete nothing, except scratch you created, by receipt. Bank to `D:\IA_BankOverflow\`.
- Don't touch `m51`, `master`, tags, `ToCodex\`, `E:\<personal-folder>`, or the frozen `_reviews\090-06-*`.
- **Codenames only.**

## Final message structure
1. `VERDICT: GREEN | NEEDS-DECISION | RED`.
2. **Plain summary for the owner**: 3 lines: does blur now fire on shared textures, blurring only the target, with labels in sync; **the exact branch and commit** for the office check.
3. The design as built (route selection, depth, label policy, reuse), and tests both ways.
4. The proof tables: shared fixture (target blur, co-users unchanged, edges, mask scope), AA-on, pure regression, auto-pool counts, can-fail.
5. Builds, archives, branch pushed.
6. NEEDS-DECISION items, or "none".
