# 090-12 — Ruling (ACCEPTED for the owner's office check): the proxy blur is proven at home in PIE and staged; the native-MainWorld hold limitation is accepted as documented; `feat/m52-proxy-blur` `51e2fdb` is the single Saturday office pull

Date: 2026-10-03 ~00:20 IST · Author: Claude Code Desktop (orchestrator/reviewer) · Implementer: Codex `gpt-6-astra` / `max`, full access (owner decision `_reviews\090-12s-owner-decision-codex-full-access.md`) · Run `_relay\runs\2026-10-02-090-12-m52-proxy-blur\` (18:32 → 22:04 interrupted by "model at capacity"; resumed 22:05 → 23:58; about 5.5 h) · Report `report.md` there (declared NEEDS-DECISION)

**Disposition: ACCEPTED.** It goes to the office host for the owner's check. Not merged; the merge waits for the office result and the Sunday noon cut-off (owner decision 090-12).
Chat review NOT REQUIRED (chat stopped orchestrating at 3a-41). The next independent check is Codex Job E, which now also covers this branch.

## What I re-checked myself
| check | result |
|---|---|
| origin `feat/m52-proxy-blur` | `51e2fdb`; worktree clean; m53 `b8cd3cd` and master `b5f15a3` untouched |
| `663a99b..51e2fdb` | docs only (CLAUDE.md, client-readme, gotchas, journal): the pushed head's code is the proved code |
| source diff `b8cd3cd..663a99b` (17 files, +553/−39), read | see the design notes below |
| archive `m52-proxy-0912-CFFD276F` re-hashed vs its manifest | 6/6, `source_commit` `663a99b` |
| lever audit re-run on the archived exe and editor DLLs | 53/53 PASS both |
| BenchGate staged exe after the job | `2FCDF059` (the original; restored by receipt) |
| raw evidence `P_e1_663a99b.e1.json` / `.shared.json` | event 1: sharpness drop 0.0000 at frame 46, 0.9843 at frame 47 = label start; 8/8 visible, 0 co-user failures of 20 per event, 0 labelled frames without a mask, 0 mask-outside / m26 / bbox errors |

## Design as built (reviewed)
- **Route** per event: `IAI.Anomaly.StuckMipRoute auto|hold|proxy` (ini `[AnomalyInjector] StuckMipRoute`). `auto` picks proxy when an eligible texture has more than one user component in the loaded world. The choice is frozen per event, and `Revert` follows it.
- **The proxy path returns before any hold code**, so the shared source is never written in production. Only the bench mutant `IAI.Bench.ProxyBlurSharedSource` (behind `-IAIBench`, non-Shipping) writes it, and the co-user gate rejects it (PIE 8/160, staged 140/140).
- **The proxy reuses the m53 transaction** with a new `ProxyBlur` mode. `texture_uniform` applies (the UV-family rule, so a flat texture is refused); in the auto-pool the m52 ratio and perceptibility checks apply to the copy's top (`too_small_for_ratio` / `imperceptible`). Component scope is always on for blur (mask, m26, boxes = the admitted components).
- **Labels:** the proxy is FireWindow (installed-state authority, bind-frame onset, `effect_interrupted`) plus the temporal 3/16 track under TSR (its omission at `a0b35e9` was caught by the TSR leg and kept as a can-fail). Hold semantics are unchanged; hold fires now always take render truth by id.
- **Cross-anomaly change: the zero-K capture scheduling** (`BeginFire` / `BeginRevert` skip an empty settle phase when K = 0). K = 0 is reachable only through the bench console command `IAI.Capture.Config`; the delivered default is `SettleFrames = 2`. **No delivered-default behaviour changes.**

## Job C acceptance (handover checklist)
- **Only the target blurs, co-users unchanged from pixels:** yes. 0/160 PIE, 0/140 staged, 20 visible co-users per event; 1,022 users in total (1,001 off-screen, counted, not claimed measured).
- **Label edges, m52 E1 and the 0-frames-off FireWindow check:** AA-off 0/0 in PIE (8) and staged (7); TSR onset 0 with the tail inside the declared 3/16 flags; shifted-label controls fail.
- **Mask scope:** the two-part fixture puts the mask, m26 and boxes on the admitted part only; the `NoPartScope` can-fail fails.
- **Pure-texture hold route unchanged:** B9/B0 E1 PASS in PIE (17/17) and staged (16/16); B5 null passes; the blurred-leg-as-null controls fail.
- **Can-fail:** old `b8cd3cd` (exe `36C8E30A`) gives 0 events, `shared_world`, 1,022 users, in PIE and staged.
- **Strict 0/0, lever audit:** yes (above). UV/normal regression: 8/8 visible and in sync in PIE and staged.

## Ruling on the declared decision (native MainWorld hold auto-pool)
**ACCEPTED as a documented limitation; it does not block the office check.**
- **What happened.** On unmodified MainWorld with office stressors, the auto-pool chose the *hold* route 3 times, on moving bots. The RGB change on those boxes is matched by the no-hold null in the same windows (7.8–9.5 vs 8.1–8.5), and E1 cannot measure sharpness on them (0.26–1.72 % < its 6 % floor). Visibility and edges are **unproven** there; there is also no evidence of a mislabel.
- **Why it doesn't block.** The hold code path is unchanged by this branch. On office content the textures are overwhelmingly shared (the second office host: `shared_world` 196, of which 164 have ≥ 9 users; HELD NONE buckets: shared 164, not streamable 4, excluded 4), so `auto` sends almost every blurry fire through the proven proxy route. The authored office-like MainWorld variant passes 5/5 visible with 5/5 edges in PIE and staged with both routes firing.
- **Correction to the record:** 090-10b2's "PIE MainWorld auto-pool 5/5 visibly blurred" for the hold route was an RGB-only reading on the same kind of moving targets. By this result it is **not** proof of visible blur. Only static-target hold legs (B9/B0, E1) are proof.
- **Follow-up (not before Monday unless the office shows hold firing):** a motion-robust visibility reading for moving targets (for example the m26 render-difference at the bind frame, or E1 on a frozen pose), and a decision on whether delivery should default to `proxy` for every blurry fire (it has the tighter, bind-frame label semantics). Both go to Job E's questions.

## Notes
- **Process:** the run was interrupted once by the model service ("at capacity"). The partial staged B0 leg was marked interrupted, never scored, and re-run after restoring staging by receipt. The orchestrator's resume runner later misread Codex's own text as a second capacity error and was stopped by PID before any retry (the detection is fixed: only Codex error events count).
- **Disk:** D: is down to about 54 GB free (floor 50); the job's evidence `D:\IA_BankOverflow\_r912\` is 43 GB. E: about 20 GB (below the floor). **The owner decides any cleanup before the next heavy job** (the extended smoke and packaging).
- **Blurry's default stays available-but-OFF** for Monday unless the office check confirms the proxy blur by Sunday noon IST.

## Office check (Saturday, the owner; numbers only)
Pull `feat/m52-proxy-blur` at `51e2fdb` on both office hosts. The checklist was given to the owner in chat and is mirrored in `_reviews\091-00-code-desktop-orchestrator-STATE.md`.
