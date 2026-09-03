# CHAT HANDOFF — m36 selection census shipped · m35 closed on Bates · first Bates census results · P8 opened
**Date:** 2026-09-02 · **Supersedes:** CHAT-HANDOFF-delivery-shipped-m33-m34.md wherever they disagree.
**Confidentiality:** office hosts are referred to by codename ONLY (Concorde, Bates). Never a real title, studio, product, or fork name — the repo was scrubbed for exactly this. `AnomalyCapture.Build.cs` is the single, permanent, owner-ruled exception (its fork-detection needles are functional and UNALTERABLE — never "fix" them).

---

## 1. Current state (verified, not assumed)

- **master `27dfba3`** carries m34 (GPU mask reduce) + m35 (readback-layout hotfix) + m36 (selection census). Census compiled default **OFF** → master is byte-inert for the client (measured: census-OFF leg identical to pre-m36 outside the run-unique timing trio; no `census_*` keys emitted when off).
- Branches `feature/mask-gpu-reduce` (`7151875`) and `feature/selection-census` (`7f82d52`) are both **merged**. `feature/stencil-capture` untouched at `76cac74` (read-only, never check out).
- **NO TAGS** — highest is m30. Tag sequence m31 → m33 → m34 → m35 → m36 happens at the **physical** office visit (m31 still needs its Concorde V-3/V-4 legs).
- **G-R7(ii) (Concorde eye + throughput split gate) has MOVED**: it is no longer a merge precondition; it is a **delivery precondition, run against MASTER's build** at the office. Eye/OBS judges the display fix only; throughput reads exclusively from the m33 wall instruments. Physical-only (RDP invalidates both halves; OBS-on-box + frame-step is the sanctioned remote route).
- **Exe chain:** … → 733FE83C (m35 Build B, the gate environment) → 6B579F91 (G-M9) → 1F6A2188 (post-merge master, m35) → 02C1DFA2 (m36 rebase) → E046D1CA (S2) → CBBF6644 (S2 gated) → 70F6B72C (S3 instrument) → **D2BB25A5 (master post-m36, currently staged)**. All archived and hash-verified. Note: identical source can produce different exe hashes (MSVC link non-determinism) — artifact hash is NOT a content-identity proof; source diff + behavioural identity is.
- **Disk:** D: filled mid-session (0.25 GB free at worst). `Binaries`, `_binary_baselines`, `_bench_sessions_bank`, `Builds` now live on E:\IA_BuildCache behind junctions at the historical paths; every move was copy → categorical manifest (count + per-file size, sha8 on exes) → remove. `D:\IntrusiveAnomalies\_TRASH_pending_owner_delete` awaits the owner's one-command delete. Runbook rule: <10 GB on the Binaries volume = NO-GO for ANY link, not just cooks.
- **CaptureBench** local-only (no remote), `28249c5` — holds the scrub verifier and the m35/m36 checkers.
- **Confidentiality scrub: CLOSED on every reachable ref.** Verifier: encoded term list, self-test on every invocation, whole-tree default, permanent printed exclusion for `AnomalyCapture.Build.cs`. Repos confirmed **private**; history rewrite ruled OUT (private + forward scrubs is the accepted posture; revisit ONLY if a repo ever goes public — old commit messages still carry the title in history, and GitHub caches unreachable objects). Last step — deleting `wip/session-061-backup` (origin + local) — was assigned to the dying Code session's wrap-up; **verify it happened** at next bootstrap (`git ls-remote` should show exactly master + the two feature branches).
- **RDP:** the owner can RDP into the office box **anytime**. Code still cannot reach it. `docs/office-rdp-card.md` (master) is the instrument — Section A (m35 validation) and Section B (census pair) are DONE; see §3.

## 2. What m36 is (one paragraph for a cold reader)

Candidate selection no longer trusts bounding boxes. A rolling **census** reuses the m26 stencil-mask pass + m34 GPU reduce as a multi-target measurement upstream of selection: prefilter (frustum, poll radius, renderable type, name patterns) → tag ≤55 candidates/frame (batches until ALL are measured; tags stay ON until results are collected or declared LOST; tag values never reused while in flight; host-written stencil values in 200–254 are RESERVED per run) → per-candidate verdict. Selection rule: MEASURED_ZERO → excluded categorically; MEASURED_NONZERO → eligible iff drawn/frame ≥ `CensusMinDrawnCoveragePct` (its own knob, default 6.0); translucent-only actors → EXCLUDED (knob); nanite / tag_failed / hidden / not_yet_measured → bounds-path fallback, per-candidate evidence only (never a view-level check — that is the H6 failure shape). The armed-frame mask + zero-only veto are untouched and remain the backstop. Census OFF = byte-identical old picker (P-C7, re-anchored at every build boundary). Census-ON is a **new comparability baseline from commit `72d6dd5`** (G140). The provider is **inverted** (AnomalyCapture registers a provider into the auto-injector; contract lives in the lower module) — with no provider registered, the selection path IS the pre-m36 code. WaitCensus defers only the first fire, ≤12 ticks.

**Cost (S3, measured, no threshold):** +1.5 ms per engine frame at 1080p **unpaced** on the dev box; the timed tag block is only **5%** of that — 95% is deferred proxy recreates (~1,200/run) + render passes, so `tagBlockMs` must never be quoted as "the census's cost." At the shipped paced 30 fps the dev box absorbed it entirely (headroom, not free); on a client box `speed_ratio` is the instrument. Named-but-unbuilt optimization if a host lacks headroom: persist tags, rotate values in place.

## 3. Bates results (owner-run over RDP; figures transcribed by chat from screen photos — the repo journal should mirror these)

**Section A — m35 hotfix on Bates: PASS.** `READBACK-LAYOUT sourceExtent=1170x765 rect=(0,138)-(1170,628) picture=1170x490 bufferHeight=490 rowPitchInPixels=1216 fmt=18`; total_frames 90, files on disk 90, no crash. Two further Section-B runs on the same path also 90/90 (rect `(0,69)-(1170,559)`, sourceExtent 1170x627 — window resized between runs; both non-zero Y origins). **READBACK-GUARD / EXTENT-CLAMP counters were NOT read** — record as unread, not zero; one grep next RDP visit. No custom-depth line was seen; the census `reserved=` list was cut off in the photo — the card should name its exact text/location.

**Section B — census pair on Bates (mask ON, census ON, excludeTranslucent=1, reservation=1, maxAge 12):**

| | Leg 1 (floor **6.0**, compiled default) | Leg 2 (floor **0.5**, console) |
|---|---|---|
| census_frames / cycles / candidates | 90 / 28 / 62 | 90 / 29 / 63 |
| zero / below_floor / translucent | 37 / 21 / 1 | 38 / 15 / 1 |
| unmeasurable hidden / others | 2 / 0 | 3 / 0 |
| fires_fallback_all | 0 | 0 |
| **vetoed_events** | **0** | **0** |
| eligible at that floor | **2** | **~8** |

Shared: pattern_excluded_targets 112 (pre-existing ini patterns), mask_nopass_discards 2, key_ring clean (126/126 and 121/121, missed 0), wanted_matches 90, tickpin_compiled **false** (expected — the tick-pin is Concorde's fork, not Bates'), capture_game_ticks 122, **ticks_per_captured_frame 1.3556 (UNEXPLAINED — queued)**.

**Histogram (stable across cycles):** zero=37–38 · (0,1]=16–17 · (1,3]=3 · (3,6]=1 · (6,12]=1 · (12,25]=0 · >25=1. Named: `BP_SnowLandscape_Test_Child_C_1` ≈ **34.0%**, `BP_BatesCharacter_Female_C_0` ≈ 6.98%, `StaticMeshActor_1246` 5.65–5.69%, `_1158` 2.73%, `_1140` 1.73%, `_55` 1.21%, `_1550` 0.88%, `_54` 0.74%, `_1332` 0.33%, `_84` 0.31%, `BP_bushlarge_…_Snowy_UP_C_1` 0.29%, `_1184` 0.27%, `_1599` 0.23%.

**Owner eye judgment:** Leg 1 — some frames **pitch black** and "the same 2–3 objects picked" (consistent: at 6.0 the only eligible targets are the landscape blueprint and the player character; a landscape hide blacks the view). Leg 2 — "much better," **~90% of anomalies visible during capture**, across several runs.

**Headline:** `vetoed_events` went **12–15 per run → 0 on both legs**. ~60% of everything in view draws zero pixels; another ~27% draws under 1%. The bounds picker was choosing from junk; the census removed it before selection. **The Bates yield problem is cured.** m35 is Bates-validated → close-out unlocked (backup branch deletion).

## 4. The issues (open, owned, and who owns them)

1. **P8 — blinking label offset (NEW ledger entry, OPEN, no mechanism claimed).** Owner-observed, 3 instances, both legs, blinking only (missing_object not reported affected): annotation `frame_indices` say hidden `{42,43,47,48}`; owner sees hidden `{42,43,44,48}`. One claimed frame absent (44) and one extra a half-period later (47) — NOT the P1 constant shift. **Owner-observation rule applies; the observation stands.** Frame-saga discipline applies: measure, then design — no mechanism goes into any brief without a measurement. Plan (next Code session): (a) bench reproduction under Bates-shaped config — letterbox lever (G193), census ON floor 0.5, mask ON, F-BLINK default, same burst structure, paced AND unpaced — read with the I10 local-contrast oracle after its A53 known-ALIGNED/known-ABSENT controls; (b) explain `ticks_per_captured_frame 1.3556` from source and compute the bench's own value under the same config (if equal, it is not a discriminator — say so); (c) source-verify the census never touches the fired target (removal + untag before the first hide; no proxy recreate lands on a hide frame); (d) DRAFT but do not send a one-event typed ask for Bates (annotation frame_indices, the labels.jsonl rows for the span, the event's log lines, session_index↔frame_index) — used only if the bench cannot reproduce. **Mitigation available now: untick blinking on Bates runs until P8 closes** — every other anomaly is single-state. The owner is worn out on desync-class bugs; chat + Code own P8 end-to-end and ask the owner for at most that one-event read, later.
2. **Floor DECIDE — open, with data.** At 6.0 Bates has 2 eligible (scenery + the player character → pitch-black frames + repetition); at 0.5 it has ~8 and ~90% eye-visible. **Chat's recommendation: default 0.5 + a NEW coverage-CEILING knob (~25%) excluding scenery-scale objects** (kills the landscape class with no name patterns; precedent: the dashboard's near-fullscreen non-clickable rule, the foliage exclusion). Alternatives offered: 1.0 + ceiling (untested by eye), or 0.5 with no ceiling. **The ceiling is UNBUILT until the owner rules.** Delivery follow-on once ruled: the client ini block gains `bSelectionCensusDefault=True` + the floor when the census ships ON.
3. **Pitch-black frames — one alternative to rule out.** Attributed to landscape hides (leg-1-only, floor-consistent). The alternative — a black-frame readback defect — is distinguishable in the banked leg-1 PNGs: a landscape hide leaves the character/sky visible; a readback failure is fully black. One glance next RDP visit; not urgent.
4. **Unread counters:** Section A's READBACK-GUARD / EXTENT-CLAMP — one grep next RDP visit. Card fix: make them a single command; also name the `reserved=` line's exact text.
5. **`ticks_per_captured_frame 1.3556`** on Bates — unexplained, queued inside P8 step (b).
6. **G-M7 backbuffer path at non-zero origin** — recorded scope limit; a backbuffer comparator is QUEUED, built only if the UI-on path is ever exercised on a letterboxed host.

## 5. Decisions made this session, with the why

- **Merge-when-done (owner ruling):** `feature/mask-gpu-reduce` merged to master once m35 home-closed; G-R7(ii) relocated to a delivery precondition on master. Client risk unchanged (nothing ships without the office cook); if the eye gate fails there, the fix lands forward on master; the `--no-ff` merge commit is the `-m 1` revert handle.
- **Translucent excluded vs Nanite fallback (split, principled):** Nanite = the instrument blind to real geometry → admit bias, fallback. Translucent-only = owner-ruled unusable as targets; they read zero at the veto in every non-swap case anyway → excluded, behind a knob so route (e)'s v2 experiment needs no cook.
- **Separate floor knob:** never reuse the bounds-coverage knob for the drawn operand — one knob driving two operands couples the fallback path to the future floor decision.
- **Tag-lifetime rules (closed by construction):** a batch stays tagged until collected/LOST; values never reused in flight — a late SVE pop against rotated tags would attribute batch k+1's counts to batch k (a wrong answer that looks right). Host stencil **reservation** at run start, both allocators — quantified on the bench: with reservation OFF a host actor's ~48k px land on whichever candidate holds its value.
- **Loud-inert rule:** a fire whose entire pool is unmeasured/expired warns and counts (`census_fires_fallback_all`) — a silently inert census reads exactly like a clean result.
- **P-C2 ruling:** PASS-WITH-READING — a measured-control and a selected-control cannot be the same actor on one leg (selecting it hides it); each conjunct proven on its own leg; the dangerous direction (MEASURED_ZERO) 0/91.
- **P-C13 ruling:** conjunct 1 decisive (the denominator moves with the rect — the census is rect-relative, which Bates' letterbox requires); conjunct 3 REFUTED as written (drawn px are not crop-invariant — write geometry predictions as measurements, not directions); conjunct 2 at n=1 NOT certified → **required gate for m37**, instrument = the uniform PIE pillarbox leg.
- **Predictions files are never amended after measurements exist** — defective wordings are annotated in the journal, not edited (laundering shape). Boundary SHA `72d6dd5` recorded (G140).
- **Scrub rulings:** names of THINGS (title, abbreviation, studio/fork/packaged names) are scrub terms; internal FW*/class identifiers are not. Build.cs needles owner-ruled UNALTERABLE, permanent printed verifier exclusion. "Nowhere" ≡ no reachable ref (origin or local); unreachable remote objects are outside our control, stated once. Packaged exe name deliberately withheld everywhere (visible on the office box itself).
- **Shared-tree rule refined:** builds ONLY from the main checkout (switching permitted when tree clean + branch == origin, stated both directions with SHAs); worktrees are for read-only inspection and doc-only commits, never builds; path-scoped adds; `git add -A` banned.
- **Amend authority:** chat may authorize a lease-checked `--amend` of an UNMERGED branch's TIP until the office has pulled it; master and tags never; deeper = STOP.
- **RDP posture:** everything except G-R7(ii)'s two halves is RDP-valid; the m36 eye judgment IS RDP-valid (it judges what was selected, not how smoothly it drew).

## 6. Forward plan (in order)

1. **Dying Code session wrap-up (assigned):** delete `wip/session-061-backup` origin+local, verifier across all origin refs → "no reachable ref"; short journal with §3/§4 above; CLAUDE.md status refresh. Verify done at next bootstrap.
2. **Owner, two words:** the floor DECIDE (§4.2).
3. **New Code session:** bootstrap from CLAUDE.md + latest journal + THIS handoff → first tasks: P8 ledger entry + the P8 measurement plan (§4.1, exactly, no fix, no mechanism); ceiling knob only after the DECIDE; card fixes (§4.4).
4. **Next RDP window (owner, low effort):** guard/clamp grep, `reserved=` line, one glance at a leg-1 black frame (§4.3). Optional: repeat Section B with blinking unticked for a clean Bates dataset.
5. **Physical office visit (batched, unchanged in content):** G-R7(ii) split on MASTER's cook · m31 V-3/V-4 · Concorde custom-depth census · tick-pin probe echo baseline · then tags m31→m33→m34→m35→m36 · delete the parent branch · Concorde ini gains the census keys once the DECIDE lands.
6. **Then:** m37 honest bbox (drawn-box labels; P-C13-c2 required gate) → resolution selection + PNG→JPEG + defaults profile (client channel must be confirmed open first — standing session-start check) → P5 blend-ladder → S-track (S4 depth, S5 backbuffer demotion) → CPU-path deletion decision at next cook (+ the stale A3 console-help string rides that cook, G118 token).

## 7. Open vs locked

**Open:** floor + ceiling DECIDE · P8 · ticks 1.3556 · guard/clamp read · black-frame glance · m31 V-3/V-4 · tags · m37 · client channel status · CPU-path deletion · Native-fps/probability-sliders/ArgControls (crisis-§5, deferred).
**Locked (do not relitigate):** zero-only veto · census OFF = old picker · translucent-excluded/nanite-fallback split · tag-lifetime + reservation rules · Build.cs unalterable · codename-only invariant · no history rewrite while private · predictions never amended post-measurement · G-R7(ii) at delivery, on master · builds from main checkout only · `tagBlockMs` is not the census's cost.

## 8. Corrections — stale understanding to discard

- "The full title never reached Code / the repo" — **WRONG (chat error):** it had been in six tracked files + two commit messages since July's content-clock work; found by Code's tree-wide sweep; now scrubbed on all reachable refs.
- "master still carries the readback crash — do not cut a build from it" — **superseded** (`27dfba3` carries the fix; stop block corrected in place).
- "Office pass required before merge" — superseded by the owner's merge-when-done ruling.
- "m35/m36": chat's scoping brief called the census "m35" — m35 was already the Bates readback hotfix; the census is **m36**; honest bbox is **m37** (chat had earlier said m36 for bbox — wrong).
- Coverage floor is **6.0**, not the 12% chat quoted from stale memory.
- "A custom-depth census one-liner exists on master" — it does not; the card asks the narrower collision question, full enumeration comes free from m36's `reserved=` echo.
- "c4702f6 stays as-is under the extended term set" — wrong; m36 needed the extended scrub too (Code caught it by measurement).
- The m36 abbreviation of the title existed ONLY because chat introduced it in two briefs — the standing rule (codenames only, everywhere, including briefs and memory) now prevents the class.

## 9. Pointers (repo docs a new session should have Code read)

`CLAUDE.md` (status + stop block) · journals **060–066** (060 field bugs; 061/062 m35 hotfix; 063 m36 S1; 064 scrub+verifier; 065 S2 gates + prediction annotations; 066 wrap-up w/ Bates results — verify it exists) · `docs/predictions/2026-08-31-m36-selection-census-gates.md` · `docs/invisible-anomaly-mechanisms.md` (P1–P8) · `docs/office-rdp-card.md` · `docs/gotchas.md` (~G189–G196) · `docs/capture-fps.md` · CHAT-HANDOFF-delivery-shipped-m33-m34.md (older context).
