# AnomalyInjector — canonical context

Personal research project **"GDP: Anomaly Injection"** (intrusive UE5 track). This is a
UE5 plugin that injects **labeled visual anomalies** (graphics bugs — missing objects,
lighting mismatch, LOD corruption, blinking, etc.) into UE5 games, to generate synthetic
training data for bug-detection ML. It is **game-agnostic** (public UE APIs only) and is
tested on Stack O Bot. A separate non-intrusive tool exists elsewhere and is out of scope.

This file is the **canonical entry point**. The folder it lives in is its own git repository
and is the single source of truth for the project.

## Current status — keep this current; it is the cold-start "you are here"

> 🏁🏁 **SESSION 067 CLOSE-OUT, 2026-09-02 — THIS IS THE CURRENT "YOU ARE HERE". Everything below it
> is older and is superseded wherever they disagree.** 🏁🏁
> **Cold start: `docs/sessions/2026-09-02-067-p9-ledger-plan-and-card-fixes.md` §17.**
>
> **`master` carries m34 + m35 + m36 + m37**, all by merge or direct commit on master.
> ⚠ **SHA-INVARIANT PHRASING ON PURPOSE:** the tip moves with every docs commit — **`git rev-parse
> origin/master` is the authority**, never a SHA written here.
> 📦 **Staged bench exe `F2FA6BCD`** (m38, 241,061,376 B, archived
> `_binary_baselines\StackOBot.exe.m38-runlog-F2FA6BCD`). Predecessor **`6C80E872`** (m37) archived and
> re-hashed at the archive before the swap. ⛔ **`D2BB25A5` STAYS LOAD-BEARING** — it is the `P-C7`
> A-side for m37's gate (a) and the exe every `P9` bench leg ran on; **its freeze is RETIRED**.
> Container unchanged throughout — **no cook**. ⚠ `6C80E872` is a **new `G140` boundary for
> census-ON legs**; census-OFF stays comparable.
>
> 🔢 **MILESTONE MAP — renumbered twice, both deliberate:**
> **`m37` = census selection defaults — ✅ DONE** (floor `0.5`, ceiling `25.0` inclusive,
> above-ceiling EXCLUDED with counter + token, `≤ 0` DISABLES with a loud echo; census compiled
> default stays **OFF** ⇒ client-inert, measured).
> **`m38` = the run-scoped session log — ✅ BUILT, GATED AND SHIPPED (2026-09-02, session 068;
> journal 068 §5, predictions `docs/predictions/2026-09-02-m38-run-log.md`).** `FAnomalyRunLog`, an
> `FOutputDevice` filtering **`LogAnomaly` + `LogAnomalyCapture` only** into **`anomaly_log.txt`**
> beside `annotation.json`. **Gates (i)–(v) ALL PASS.** ⛔ **NO ARTIFACT FIELD MOVED** —
> `annotation.json` **48 keys** and `run_summary.json` **48 keys**, both IDENTICAL across the binary
> change; it adds one FILE, not a field. Delivery default **mirrors `run.json`** (auto = `!delivery`),
> three-state `−1/0/1` via `IAI.Capture.RunLog` and ini `RunLogDefault`. 🔑 **`IAI.Capture.RunLogVerbose`
> raises `LogAnomaly` to Verbose FOR ONE RUN AND RESTORES IT**, both transitions echoed and proven
> BOTH WAYS by a paired probe (`raised=1` emitted once, `raised=0` zero times).
> 🎯 **It formats with `ELogTimes::UTC` DELIBERATELY, so the `[GFrameCounter % 1000]` prefix — `P9`'s
> anchor — is present whatever a host's `log.Timestamp` says.** ⚠ Per-line write-through flush: a
> hard-killed process leaves everything logged before the kill, with **no** close marker.
> **`m39` = honest bbox** — **`P-C13` conjunct 2 rides it**, regardless of number.
> ⛔ **NO TAG.** Highest remains `m30`; the office batch is now
> `m31 → m33 → m34 → m35 → m36 → m37 → m38`.
>
> 🔴 **`P9`, IN ONE PARAGRAPH.** Owner **reproduced it on Bates with NO FLAGS** (census OFF, mask
> OFF), deterministic across events. **Two phenomena, kept apart: (A) BOUNDARY SMEAR is CLOSED** —
> attributed to **temporal accumulation**, because with AA and motion blur off via the title's menu
> the partial-opacity frames **vanished**, which is what `C-1` pre-declared. **(B) PHASE DISPLACEMENT
> is OPEN**, now a clean binary read — observed `{n,n+1,n+2,n+6}` vs claimed `{n,n+1,n+5,n+6}`, the
> outer flips matching exactly and **both interior flips +1 frame late in pixels**, hidden-frame count
> conserved. **Every other axis is excluded — AA in BOTH directions, census, mask, delivery, pacing,
> tick ratio, letterbox — so (B) is LOCALIZED TO THE BATES HOST BUILD + CONTENT.** ⛔ **NO MECHANISM
> CLAIMED.** **Next evidence: RDP card `C-3`** (fresh run, `Log LogAnomaly Verbose` set first, AA off;
> a pre-declared three-way comparison that names WHERE the divergence sits, never why).
> ✅ **THE TRANSITION-DRIVER + TOGGLE-LINE-ANCHOR SOURCE READ IS DONE (2026-09-02, session 068 TASK 1 —
> journal `docs/sessions/2026-09-02-068-p9-transition-drivers-and-toggle-anchor.md` §1).** It had
> **NEVER BEEN EXECUTED** (the brief carrying it was never delivered), and journal 067 §17.6 and the
> `p9-bates-localized` handoff §4 both list it as outstanding — **those are RECORDS and are not
> retro-edited; read them as DONE.** Findings, factual mapping only, **NO MECHANISM**:
> **`Apply` NEVER HIDES** (`Anomaly_Blinking.cpp:10-71`) ⇒ flips **1, 2 and 3 are all the SAME
> statement**, the `Tick` toggle at **`:91`**; **flip 4 (final show) is `Revert()` at `:106`**, reached
> `BeginRevert` `AnomalyCaptureSubsystem.cpp:623` → `RevertAllLiveFires` → `RevertAnomaly`. ⇒ the
> call-site partition `{1,2,3}·{4}` and the observed-agreement partition `{1,4}·{2,3}` **DO NOT
> COINCIDE** — stated, with **no inference drawn.**
> 🔑 **TWO CLOCKS, NEVER CONVERTED:** `FramesSinceToggle` counts **world-tick CALLS** (`:80`; the
> `DeltaSeconds` parameter is ignored) · `SessionFrameIndex` counts **armed captured frames**
> (`AnomalyCaptureSubsystem.cpp:2180`). They meet at **one sample, not a conversion** —
> `SampleDeferredActiveState`'s `IsHidden()` read at `:2558`. ⚠ **`ticks_per_captured_frame` 1.3556 is
> a RUN-AVERAGE and is NOT a per-window conversion factor — inside the Positives phase every tick
> captures, so the local ratio is exactly 1.0.**
> 🎯 **THE ANCHOR EXISTS AND IS EXACT: the toggle line stamps ONLY the phase word and the actor count
> (`:95-96`), but the ENGINE PREFIX carries `GFrameCounter % 1000`** (`OutputDeviceHelper.cpp:30`;
> `log.Timestamp` default **1 = UTC**, `LaunchEngineLoop.cpp:5678-5687`), and **`GFrameCounter`
> increments AFTER the world tick** (`LaunchEngineLoop.cpp:5568` vs `:5363`) ⇒ **the toggle line's
> `[fff]` == `labels.jsonl.frame_index % 1000` of the frame armed on that same tick.** Measured on this
> box's packaged log. ⚠ **If Bates prints no prefix, the exact-frame join is gone and only ORDER
> survives — that is a READ, and the bundle shows it directly.**
> 📌 **`n+7` IS NOT "LABELLED VISIBLE" — IT CARRIES NO ANOMALY ROW AT ALL** (`BeginRevert` at `:623`
> precedes `FinalizeArmedLabel` at `:670`), so the overlay should show **NO box at `n+7`** and an
> **AMBER box at `n−1`**. Verified in a banked leg (`session_index 23`, `anomaly_present=false`).
> ⚠ **`SVE-WANT-TRACE arm` STOPS AT 64 FRAMES** (`AnomalySveCapturer.h:38`).
> ⇒ **Card `C-3` gained a READ GUIDE subsection (read instructions only; no new steps, no new
> commands).**
> ⚠ **Journal 067 §15's `AnomalyCaptureSubsystem.cpp` line numbers were correct at `d257f7b^` and are
> now `+6` (early file) / `+61` (from `m37`'s ceiling knob onward) — the stamping trio is `:1978` /
> `:1980` / `:1982-1983`, `ActiveByIndex.Add` is `:3232`. Facts unchanged; the mapping table is journal
> 068 §1.0.**
> 📌 **Standing mitigation unchanged: `blinking` stays UNTICKED on any Bates run that is not
> `C-1`..`C-3`.**
>
> ---
>
> 🛑🛑 **READ THIS BEFORE YOU TOUCH ANYTHING — 2026-08-26, session 061 close-out.** 🛑🛑
>
> **1. 🆕 THE `WIP` COMMIT IS NOW PUSHED. THE DO-NOT-PUSH RULE IS LIFTED — OWNER RULING, 2026-08-26.**
> `feature/mask-gpu-reduce` is **fully published: `origin` == local, nothing withheld.** ✅ **THE
> BRANCH IS COOKABLE NOW** — the office can pull and start rebuild-and-cook immediately, which is the
> longest wall-clock item and was the only thing this rule was blocking.
> 🆕 **MERGE RULE CHANGED, OWNER RULING 2026-09-01 — THE MERGE MOVED EARLIER, THE CONCORDE GATE MOVED
> LATER.** `feature/mask-gpu-reduce` **MERGES TO MASTER WHEN m35 IS HOME-CLOSED (`G-M9` GREEN)**.
> `G-R7(ii)` is **unchanged in content** and keeps AMENDMENT 1 and 2 in full, but it is now a
> **DELIVERY precondition, not a merge precondition**: office visit A pulls and cooks **master** and
> runs the gate **there**. ⛔ This RELOCATES the gate, it does not weaken it — **nothing ships to the
> client off master until `G-R7(ii)` passes on master's own build.** Full ordered sequence, including
> the inert-merge proof and the `-m 1` revert handle, is journal 061 **§12.0**.
> ⛔ **STILL NOTHING IS TAGGED** (highest remains `m30`); the tag sequence is still
> `m31` → `m33` → `m34` → `m35` → `m36`, and it runs at the END of office visit A.
> *(2026-09-02: `m36` appended to the sequence — it is merged to `master` and its bench phase is
> closed, so it tags with the rest at the physical visit.)*
> ✅ **`G-M9` IS GREEN — m35 IS HOME-CLOSED (2026-09-01).** Binary **`6B579F91`** = Build B + `G-M9`,
> code-only, container unchanged. Exe chain **`733FE83C` → `6B579F91`**, both archived and verified.
> Self-proof both ways: forced-mismatch fires on 90/90 with **one-byte** sensitivity; cvar-OFF
> reproduces the banked `733FE83C` leg (`run_summary` 34/37 identical, the 3 differing being the
> declared run-unique timing fields). Comparator **270 frames, ZERO byte differences**, at
> `rect=(0,0)`, `rect=(0,92)` and **`rect=(280,0)`** — the first non-zero `Rect.Min.X` ever run here.
> ⚠ The zero-origin leg is necessary and **not** sufficient — at zero origin the two indexings are the
> same expression (`G192`). The gate rests on the two non-zero-origin legs. Journal 061 **§18**.
> 🆕 **§10's "must run in PIE" is REFUTED** — the lever refuses on `CB_GateLevel` (SpectatorPawn) but
> **APPLIES on packaged `MainWorld`** (`G193`). `G-M8`'s pillarbox gap is closed unattended.
> ⛔ **`G-M7` (backbuffer path) is still OPEN** — `G-M9` instruments the SVE path only.
> ⇒ **NEXT: the merge sequence in journal 061 §12.0**, starting with a fresh `merge-tree` pre-check.
>
> 🔧 **SHARED-TREE RULE, REFINED (2026-09-01, supersedes the scrub-era wording):**
> **1.** Builds come **ONLY** from the main checkout. Switching the main checkout to another branch to
> build it is **PERMITTED** when `git status` shows **no tracked modifications** and the branch being
> left is **== its origin** (or its exact state is recorded first). **Every switch is stated in the
> report, both directions, with the SHAs.**
> **2.** Worktrees are for **read-only inspection and doc-only commits** on other branches while a
> build-bearing branch stays checked out. **Never for builds.**
> **3.** Path-scoped adds everywhere; **`git add -A` stays banned**; untracked owner docs are never
> staged.
>
> 📌 **CODENAME-ONLY INVARIANT (2026-09-01):** hosts and their engine lineage are written as **Bates**
> and **Concorde** everywhere. All three refs verified clean by the scrub instrument, which proves
> itself against a synthetic known-answer fixture before every verdict. One permanent, printed
> exclusion: `Source/AnomalyCapture/AnomalyCapture.Build.cs` — **never altered** (its strings are the
> build-time fork-detection needles, and this box cannot tell a working probe from a broken one).
> Session 064 journal has the ledger and the receipts.
> 🚨 **CONSEQUENCE, STATED RATHER THAN DISCOVERED: THE WIP CAN NO LONGER BE AMENDED.** If a remaining
> gate forces a change it lands as a **FOLLOW-UP COMMIT**, so **m35 is two commits**. That
> deliberately overrides one-milestone-one-commit for m35, and **the tag goes on the FINAL commit.**
> ⛔ No re-parenting, no force-push, ever.
> 📌 **AND SO THE SHA IS NOW STABLE AND IS GIVEN.** The de-SHA rule elsewhere in these docs existed
> because an *amendable* commit's hash changes by construction; the amend path is closed, so that
> reason has lapsed **for this commit only**. The rule itself stands for any future amendable commit
> (`G185`). ⛔ Still do not `git add -A` anywhere in this repo — the owner's two untracked
> `docs/CHAT-HANDOFF-*.md` would be swept.
> 📎 ✅ *(closed 2026-09-02: `wip/session-061-backup` is DELETED from origin and local per RDP card
> A-8, after the Bates Section A pass — journal 066 §1. The line below is history.)*
> `wip/session-061-backup` stays on origin until the fix lands. **The remote is now the backup:**
> the insurance diff is **NO LONGER LOAD-BEARING** and must not be treated as protection again — it
> survives only as `G181`'s receipt.
>
> ✅ **2. SUPERSEDED 2026-09-01 — `master` NO LONGER CARRIES THE READBACK CRASH.** This item used to
> read *"`master` IS UNTOUCHED AND STILL CARRIES THE READBACK CRASH, tip `9f52cab`"*, and a cold
> reader acting on it today would refuse a build that is now correct. **`master` is `a73f87f`** and
> carries **m34 (the GPU mask reduce), m35 (the readback fix) AND m36 (the selection census)**, all by **MERGE — ONE ROUTE
> ONLY**, never also by cherry-pick. The m36 merge is `0f35d7a`, with `merge-tree` predicting its
> tree exactly and an inert-merge proof on both halves: `Source/` byte-identical to the branch, and
> master's own build census-OFF artifact-identical to the branch build's.
> 📌 **`feature/stencil-capture` IS LOCAL-ONLY, at `76cac74`, AND HAS NEVER BEEN PUSHED** (verified
> 2026-09-02: it is absent from `git ls-remote --heads origin`, which lists only `master` and the two
> feature branches). ⛔ **Never check it out** — that rule is unchanged. ⛔ **And never push it
> without a clean scrub-verifier pass first.**
> ✅ *(2026-09-02, later the same day: it HAS now had a verifier pass — `clean over 98 file(s)`. The
> "cleanliness unknown" caveat is retired for this branch.)*
> 🔴 **THE SAME PASS FOUND TWO OTHER LOCAL-ONLY BRANCHES THAT WERE NOT CLEAN** —
> `m29-GATE-FAILED-lod-popping-invisible` (`ab2fb41`) and `s3a-2-GATE-FAILED-do-not-merge`
> (`087f4d9`), both reading `TERMS PRESENT`. ✅ **OWNER-RULED AND EXECUTED 2026-09-02: BOTH
> DELETED.** Dead by name, `[gone]` upstreams, never merged, unreachable from any live branch, and
> their findings already journaled. ⇒ **`"no reachable ref, origin OR local"` IS RESTORED**, and it
> now rests on a verifier pass over **every** remaining ref. Surviving local refs: `master`,
> `feature/mask-gpu-reduce`, `feature/selection-census`, `feature/stencil-capture`. Journal 067
> §9.5 and §10.4.
> 📌 **THE SHA IN THIS ITEM MOVES WITH EVERY DOCS COMMIT AND HAS DONE SO TWICE.** It read `9c94c55`
> until 2026-09-02; the merge line itself is unchanged and the invariant is the one to read —
> **`master` is the m34+m35+m36 merge line, and its tip is whatever `git rev-parse origin/master`
> says.** Recorded here so the next stale-SHA reading is a known shape, not a surprise.
> ⛔ **STILL NO TAG** — highest remains `m30`, and the order `m31` → `m33` → `m34` → `m35` → `m36`
> runs at the end of the physical office visit, after `G-R7(ii)`.
> ⚠ **A DELIVERY BUILD FROM `master` IS NOW THE CORRECT THING TO CUT** as far as the crash is
> concerned — but it is **still gated on `G-R7(ii)` passing on master's own build**, which is a
> DELIVERY precondition and is unchanged.
> 📌 **m36 is INERT on `master` for the client:** the census's compiled default is **OFF**, and with
> no provider registered the selection path is byte-identical to the pre-census picker — **measured
> (`P-C7`), not asserted.**
>
> **3. THE PACKAGED BENCH BUILD IS CURRENTLY m35 BUILD B — exe `733FE83C`** on the **unchanged m34
> container** (utoc `2A66CA57` · ucas `A7EF9B12` · pak `D8009AD7`). Code-only hot-swap (`G103`); **no
> cook this session.**
>
> **4. ⛔ DO NOT DELETE `_binary_baselines\StackOBot.exe.m34-fix-candidate-7F37A4AC`.** That archived
> pre-m35 exe **IS `G-M6`'s A-SIDE** — the hook-cost prior is taken by swapping it against `733FE83C`
> with pacing OFF, and it cannot be rebuilt from the current tree once the WIP commit is amended.
> Deleting it makes `G-M6` unobtainable without a rebuild the owner has explicitly ruled out.
>
> **5. THE BRANCH IS A FOUR-ITEM STAGING LINE, NOT "the m34 branch".** In merge order:
> **m34** (the GPU mask reduction, `0fc00ef`) · **`b05066f`** (the stale-present display fix) ·
> **`5495aa6`** (targeted global anomaly held for the whole capture) · **m35** (`9aec10f` Build A +
> the WIP fix). The cook that gates it is therefore a **four-item cook**, and `G-R7(ii)`'s display
> half judges **`b05066f`**, not m34. Per-file attribution is in the m34 gate file **A2.1**.
>
> 🧭 **COLD START: `docs/sessions/2026-08-26-061-m35-readback-sub-rect-copy-handoff.md`.** It opens
> with a **DO NOT DO THIS** list and is self-contained.
>
> 🆕🆕 **2026-09-02 — `m36`'s BENCH PHASE IS CLOSED, ACCEPTED, AND ON `master`. GO STRAIGHT TO
> `docs/sessions/2026-09-01-065-m36-s2-selection-wired-and-gated.md`; it is self-contained (§10 is
> the close-out, §11 is what m37's predictions must carry).**
> **`master` = the m36 merge line** (branch `feature/selection-census` merged `--no-ff` at
> `0f35d7a`, both refs pushed; the branch tip `7f82d52` stays == origin). **NO TAG, NO COOK.**
> 🆕 **2026-09-02, SESSION 066 — THE BATES READS ARE IN (journal
> `docs/sessions/2026-09-02-066-m36-bates-results-and-m35-closeout.md`; the owner's own write-up
> arrives as the untracked `docs/CHAT-HANDOFF-m36-census-and-bates-results.md` — NEVER staged).**
> ✅ **Section A: m35 VALIDATED ON BATES — PASS.** 90/90 frames, rect Y-origins **69 AND 138** (two
> real non-zero letterbox origins on the crash host), no crash. ⚠ guard/clamp counters **UNREAD, not
> zero** — stated, not claimed. **Consequence executed: `wip/session-061-backup` DELETED from origin
> AND local** (no `wip` ref reachable anywhere), and the scrub verifier re-ran over ALL remaining
> origin refs via read-only worktrees: master 197 / selection-census 196 / mask-gpu-reduce 189 files,
> **all CLEAN** (a first 0-file vacuous read was discarded and re-run) ⇒ **"no reachable ref"
> ACHIEVED for the codename invariant.**
> ✅ **Section B: THE BATES YIELD PROBLEM IS CURED.** Floor 6.0 → **2** eligible (SnowLandscape 34 %,
> player 7 %), pitch-black frames + the same 2–3 objects repeating; floor 0.5 → **8** eligible,
> **~90 % of anomalies eye-visible**; **`vetoed_events` 0 on BOTH legs vs the banked 12–15 band** —
> the census removes at selection what the veto used to delete after the fact.
> ✅ **FLOOR/CEILING DECIDE — LANDED 2026-09-02. OWNER RULED: floor `0.5` + a coverage CEILING of
> `25`.** Concretely: `CensusMinDrawnCoveragePct` default → **0.5**, and a **NEW**
> `CensusMaxDrawnCoveragePct` default **25.0** excluding scenery-scale targets (the 34 % landscape
> blueprint is the pitch-black-frame producer).
> ⛔ **BUILD QUEUED UNTIL AFTER THE `P9` LEGS — the exe stays `D2BB25A5` for the entire `P9` read.
> ONE VARIABLE AT A TIME.** ⛔ **No source change to the census has been made.** The ceiling gets its
> own milestone and its own plan next session; the ini-block updates (Bates, Concorde, client keys)
> ride that milestone. 📌 Interim Bates guidance unchanged: **console floor 0.5, blinking unticked**
> until `P9` closes.
> 🆕 **P9 minted** (blinking annotation-vs-observed mismatch on Bates, owner-observed ×3, NO
> mechanism claimed — see the phenomenon ledger).
> 🔴 **P9 MEASURED 2026-09-02 (session 067) — VERDICT `UNDECIDABLE` ON ALL FIVE LEGS. NEITHER
> REPRODUCED NOR REFUTED.** ⛔ **Do NOT read this as "not reproduced"** — that verdict requires every
> event graded ALIGNED, and **zero events were graded**. Both instruments *refused*: the new
> `p9_hidden_set` reader and `a54_oracle` independently, on the same cause.
> 🚨 **v1's CAUSE WAS A STRUCTURAL FIXTURE CONFLICT, MEASURED:** a non-zero view-rect origin needs the
> letterbox lever, which refuses on `CB_GateLevel`'s `SpectatorPawn` and so forces **`MainWorld`**
> (`G193`) — but `MainWorld`'s intro camera **MOVES during capture** (32 distinct origins over 90
> frames, pitch −20°→0°), so the per-event bbox changes every frame and `A56` collapses to
> modal 1-in-8. `G135`'s shape → **`G206`**. Journal 067 §11.
> ✅ **FIXTURE-V2 SOLVED IT WITH NO SOURCE CHANGE AND NO COOK (journal 067 §12).** Two engine console
> commands letterbox `CB_GateLevel` itself —
> `Set PlayerCameraManager bDefaultConstrainAspectRatio true` + `Set PlayerCameraManager
> DefaultAspectRatio 2.39` — giving `rect=(0,92)-(1280,628)` on the **settled-camera** fixture
> (`distinct=1` bbox at `modal 100 %`). Works because `UpdateViewTarget` applies those defaults
> **view-target-agnostically** (`PlayerCameraManager.cpp:352-355`) and a camera-less `CalcCamera`
> never overwrites them (`Actor.cpp:3085`). ⚠ **BENCH DEVICE ONLY — never in a client payload.**
> ⛔ **The lever is UNTOUCHED, no module recompiled, the exe is STILL `D2BB25A5`.**
> 🔴 **FIXTURE-V2 RESULT: `P9` DID NOT APPEAR ON ANY GRADEABLE EVENT.** Four legs (A/A′/B/B′), **16
> counted events, ALL ALIGNED, `k=0`, both differences empty, ZERO `P9`-SHAPE**; 8 further events
> UNDECIDABLE on separation. ⛔ **This is NOT the pre-declared "NOT REPRODUCED"** — that required
> *every* event graded, and 8 were not. **`P9` stays OPEN: owner-observed on Bates, not reproduced
> and not refuted here.** Instrument passed three **in-regime** controls on the leg's own data.
> ⚠ `A54` **cannot answer the `P1` question on this fixture** (its `B1` conjunct fails because
> `CALIB_BBOX` is frozen unconstrained, and its message misattributes that to pose); the `P1`
> exclusion rests on the reader's own best-`k` search returning `k=0` on all 16.
> ✅ **`ticks_per_captured_frame` — CLOSED AS A NON-FINDING.** Bates' **1.3556** is reproduced
> **exactly** on three paced legs here (122 ticks / 90 frames); the counter jitters 122↔124 run to
> run. ⇒ **NOT a discriminator.** It is simply `capture_game_ticks / total_frames`
> (`AnomalyLabelWriter.cpp:546-548`).
> ✅ **Delivery mode is EXCLUDED as a `P9` factor by measurement** — leg C ran `delivery_mode=True`
> and matched leg A's structure and separations.
> Do not start the next milestone unprompted; do not re-run bench gates while waiting.
> 🔢 **MILESTONE NUMBERING — RENUMBERED TWICE, DELIBERATELY. THIS BLOCK IS THE LIVE MAP; READ IT
> BEFORE ANY DOC THAT SAYS "m37" OR "m38".**
> **`m37` = THE CENSUS SELECTION DEFAULTS** (floor `0.5` + coverage ceiling `25`).
> **`m38` = THE RUN-SCOPED SESSION LOG** (the plugin writes a `LogAnomaly` excerpt into the capture
> folder itself). 🆕 *(second renumber, chat-owned, 2026-09-02.)*
> **`m39` = HONEST BBOX** (drawn-box labels).
> ⛔ **`P-C13` conjunct 2 RIDES HONEST BBOX REGARDLESS OF ITS NUMBER** — not weakened, not dropped,
> not attached to `m37` or `m38`. 📌 Journals 065/066 and the m36 handoff say *"m37 (honest bbox)"*;
> journal 067 §13.2 says *"m38 (honest bbox)"*. **All are RECORDS and are NOT retro-edited** — read
> them as "the honest-bbox milestone", now **`m39`**.
> 🔢 **AMENDMENT `A65` MINTED** (`A59` was already taken — MCP-bridge provenance): **`A54` requires
> the unconstrained calibration pose; on an aspect-constrained fixture it is `N/A`, DECLARED, never
> "failed", and its `P8`/pose message there is a KNOWN MISATTRIBUTION** (the view was constrained,
> the camera did not move). ⛔ `a54_oracle.py` stays untouched — any edit re-triggers `A53`. On such
> fixtures the constant-shift question is carried by the `P9` reader's best-`k` over −6…+6.
> Journal 067 §13.1.
> 📌 **SHIPPED SHAPE:** census **compiled default OFF** (master is INERT for the client — with no
> provider registered the selection path is byte-identical to the pre-census picker, measured via
> `P-C7`, re-anchored twice) · **floor default 6.0** (`CensusMinDrawnCoveragePct`; 0.5 is a LEG
> parameter, never a default) · `run_summary` +11 `census_*` keys ONLY when the census ran ·
> **`P6` unmoved, measured 48/48.**
> Selection consults MEASURED DRAWN PIXELS before the bounds path. **Every S2 gate ran and none
> failed** — `P-C1`·`P-C2`·`P-C3`+comp·`P-C4`+comps·`P-C5`·`P-C8`·`P-C10`·`P-C11`+comp·
> `P-C12`+comp·`P-C13`, plus the `run_summary` subset gate (delta **exactly** the 11 `census_*`
> keys). **Owner rulings: P-C2 = PASS-WITH-READING (→`G195`); P-C13 conjunct 1 decisive, conjunct 3
> REFUTED AS A PREDICTION (→`G196`), conjunct 2 NOT CERTIFIED at n=1 and now a REQUIRED m37 GATE
> with the uniform PIE pillarbox leg as its instrument** (journal 065 §11).
> 🎯 **THE NAMED OPTIMISATION — recorded, NOT built, never built unprompted: PERSIST-TAGS** (leave
> `bRenderCustomDepth` ON across census batches, rotate stencil VALUES in place — the value-set is
> an in-place proxy update; the flag flip is the full recreate). Named because S3 measured **95 % of
> the census's cost OUTSIDE the timed block**, in the deferred recreates and render passes. ⛔ It is
> built only if a REAL HOST shows no headroom in `speed_ratio` — **measured there first.**
> 📚 **Gotchas minted at close: `G194`–`G201`** (capped-sorted listing hides its tail class ·
> measured-control ≠ selected-control on one leg · geometry predictions as measurements not
> directions · truncated search = unread surface · namespaced `struct` forward-decl · a runbook
> step that never enables its own precondition · inline `-m` quoting swallows a commit · artifact
> hash ≠ content identity across links).
> 🔑 **`72d6dd5` IS THE `G140` BOUNDARY SHA.** Wiring the census changes the candidate set, so the
> same seed picks different targets across it ⇒ **banked auto-pool runs are non-comparable to any
> census-ON leg from here on.** ⛔ **What is NOT lost, and this is the whole point: census OFF stays
> BYTE-IDENTICAL to the old picker** (`P-C7` re-verified post-S2 on a pose-matched pair), so every
> banked run stays comparable to any census-OFF leg. **Census-ON is a NEW baseline starting at this
> commit, not a lost one. If `P-C7` ever fails after `72d6dd5`, THAT is the door closing, and it is
> a STOP.**
> 🚨 **THE PROVIDER IS INVERTED AND MUST STAY THAT WAY:** `AnomalyCapture` **depends on**
> `AnomalyInjector`, so `TryFireOnce` cannot call the census — that edge does not exist and adding
> it is a dependency CYCLE. Capture **registers** a provider at census `Begin` and **clears** it at
> `End`; the contract lives in the LOWER module (`AnomalyCensusProvider.h`). ⛔ Do not "simplify"
> this into a direct call.
> ⚠ **TWO PREDICTIONS WERE DEFECTIVE AS WRITTEN — neither a build defect, neither relabelled, and
> NOTHING appended to the closed predictions file. BOTH NOW RULED (journal 065 §10); the detail
> stays in §5:** **(1) `P-C2` asks for
> `MEASURED_NONZERO` on EVERY cycle AND for the control to be SELECTABLE — mutually exclusive,
> because selection fires on it and firing HIDES it (8/91 measured on the main leg; the conjunct is
> demonstrated **30/30** on P-C2's own floor-10 companion, where the floor refuses it so nothing
> fires). MEASURED_ZERO occurred **0 of 91** — the dangerous direction never fired. **(2) `P-C13`
> conjunct 3 predicts the drawn share RISES under pillarbox; it FELL** (3.183→1.792 %, 2.263→1.648 %)
> because the pillarbox CROPS as well as shrinking the denominator. **P-C13's actual claim rests on
> conjunct 1, which is decisive: derived `frame_px` 518,400 on the offset leg vs 921,600 on the
> zero-origin control — the census is RECT-RELATIVE.**
> 📦 Binary chain: `02C1DFA2` (pre-S2) → `E046D1CA` (S2) → `CBBF6644` (S2+instruments) →
> `70F6B72C` (S3 cost line) → **`D2BB25A5` (master's own post-merge build, STAGED on the bench)** —
> all five archived under `_binary_baselines\StackOBot.exe.m36-*`; **container UNCHANGED throughout,
> code-only hot-swap (G103)**. ⚠ `70F6B72C` vs `D2BB25A5` is `G201` in action: byte-identical
> `Source/`, different exe hashes — a hash is not content identity across two links.
> Harness CaptureBench **`28249c5`** (⚠ LOCAL-ONLY — that repo has NO remote; its commits live on
> this box) — `run_leg.ps1` gains **`-RequireModalRotZero`**, an A47
> ROTATION validity gate for AUTO-POOL legs (B1 is scoped to `StaticMeshActor_49` and honestly says
> NOT APPLICABLE there, which left the bifurcation uncontrolled on exactly the legs whose candidate
> set depends on the settled camera). ⛔ **It is for `CB_GateLevel` only — MainWorld settles at
> `(0,-40,0)` and applying it there would be `G117`'s error on a new axis.**
> ✅ **S3 (`P-C9`) IS DONE — REPORTED, NOT GATED, and there is NO THRESHOLD anywhere.** 1920×1080,
> `A,B,B,A` after a declared discard, pacing OFF, **both sides mask-ON so only the census differs**.
> **B−A = +2.1390 ms per captured frame** (A spread 0.5662, B spread 0.0233) and **+1.5352 ms per
> ENGINE frame** ⇒ unlike `G-M6` this cost is **ABOVE the instrument's resolution**. Per-MP
> **+1.0316**; the Concorde figure **6.60 ms/frame is an EXTRAPOLATION, not a measurement**.
> 🚨 **THE COMPONENT SPLIT IS THE FINDING: the timed tag block is 0.0778 ms/engine frame against a
> measured 1.5352 — only 5 % of the cost is inside it, 95 % is the DEFERRED PROXY RECREATES and the
> render-side passes. ⛔ NEVER quote `tagBlockMs` as "the census's cost"; it under-reads it about
> twentyfold.** ✅ **Paced pair at the shipped 30 fps: per-captured-frame IDENTICAL TO FOUR DECIMALS
> (44.5693 ms both), `speed_ratio` 1.0000005 vs 1.0000006 — the pacer absorbs it entirely.** That is
> journal 061 finding 1 reproduced, and it is why the measuring legs ran pacing OFF. ⚠ It does NOT
> mean the census is free — it means this box had headroom at 1080p; a host without it shows up in
> `speed_ratio`. Binary **`70F6B72C`**, `P-C7` re-anchored to it. Journal 065 §7.5.

- **STANDING CONVENTION (owner directive, 2026-07-29): this Current-status block is REFRESHED AT EVERY MILESTONE
  CLOSE — same discipline as the session journals.** It is the cold-start contract: if it says "in flight / not
  committed", a fresh session believes it. Rationale: stale docs have now caused a real miss twice (the m20 "Bug A"
  slipped because annotation.json's path sat outside validation scope; and this block claimed m21 was uncommitted
  for six commits after it had shipped). A status refresh is a standalone `docs:` commit, never folded into feature work.
- 🟦 *(superseded as "you are here" by the 2026-09-02 m36 close-out in the stop block above — m35 is
  MERGED to master and home-closed; kept as the record of sessions 061–062)*
  **2026-08-26 (sessions 061–062). `m35` IS IN FLIGHT ON BRANCH
  `feature/mask-gpu-reduce`: THE FIX IS BUILT, STAGED AND GREEN ON EVERY GATE THAT HAS RUN, AND IT IS
  COMMITTED LOCALLY AS THE UNPUSHED `WIP` TIP** (8 files, **192 insertions / 16 deletions** — the
  stop block above is the operating contract for it). **NO LEG HAS FAILED. ⛔ NO TAG — highest tag is
  still `m30`, and `m31` is still the open milestone awaiting Concorde V-3/V-4.**
  📌 **SESSION 062 RULINGS — read these before designing any remaining leg:**
  **(i) A RE-RUN LEG'S PAYLOAD IS DERIVED FROM THE BANKED LEG'S OWN RECORDED CONFIG**
  (`_leg_geometry.json` / `run.json`), **never hand-transcribed into a doc.** The two outstanding
  `G-M5` payloads written into the session-061 journal diverged from their own known-answer leg
  (`M34_R3_CYL73`) on **four axes**, one of which (`IAI.Capture.MaskReduce both` omitted) makes the
  leg **ungradeable — no `MASK-REDUCE COMPARE` line is emitted at all**. The banked config is
  authoritative; the journal payload is a **docs defect**, corrected by APPENDING (journal §7.1, gate
  file §14) so the divergence stays visible. **Before running ANY leg graded against a banked datum,
  diff the intended payload against that datum's recorded config on every axis and report the diff,
  even when it is empty.** → `G184`.
  **(ii) `G-M6`'s ORDER IS `A,B,B,A`, NOT `A,B,A,B`,** preceded by ONE leg **declared a discard before
  it runs**. Warm-up (`G66`) makes earlier legs slower, and in `A,B,A,B` the A-side occupies the
  earlier mean position, which **biases the reading toward "B is not slower" — it would hide the very
  cost the prior exists to size.** → `G186`. **A difference not larger than the within-build spread
  across positions is "BELOW THE RESOLUTION OF THIS INSTRUMENT", never "no cost"** → `G169`.
  **(iii) `G-M9`'s cvar is `IAI.Bench.DualPathReadback <0|1>`, default OFF**, and it **must echo its
  effective state at `StartRun`** the way the mask key does (`A48`) — a diagnostic that can be
  silently off is a clean null waiting to be misread.
  **(iv) BUILD IDENTITY IS THE FIRST 8 HEX OF SHA-256** — `(Get-FileHash <exe> -Algorithm
  SHA256).Hash.Substring(0,8)`. Stated in `_binary_baselines\README.md`, **never in the runbook**;
  verified this session against the archived predecessor and now in `setup-runbook.md` §8.1.
  **(v) `_binary_baselines\StackOBot.exe.m34-candidate-17DEAA74` IS LOAD-BEARING AND IS NAMED IN NO
  STOP BLOCK:** it is the exe **every m34 home gate leg (`G-R1`..`G-R6`, `G-R7`'s StackOBot half)
  ran on** (journal 058). ⛔ **No archived baseline is deleted until the docs say what gate depends
  on it.**
  **(vi) THE "FRAME-2 LESSON" GAP IS CLOSED FROM THE LOG — AND THE LOG CORRECTED THE ACCOUNT.** The
  crash run's log survives (`Saved\Logs\StackOBot-backup-2026.08.26-10.27.55.log`, matched by UTC
  start, viewport and seed). ✅ **The defensible lesson: SUBMISSION SUCCEEDING PROVES NOTHING ABOUT
  EXECUTION** — `keyed frame id=1 submitted` printed with a **correct** rect (`821x344`) and format,
  and the process died **22 ms later on the RHI THREAD** in the D3D12 transient allocator.
  🚨 **The assert fired on the FIRST armed frame's execution, BEFORE arm 2 was ever submitted** (arm
  2's `submitted` line is 6.5 s later, inside crash handling) ⇒ **the "it landed on armed frame 2"
  account is refuted, and so is "a one-frame smoke test would have passed" — it would have CRASHED.**
  "Why did frame 1 survive?" was never a real question, so the transient-reuse candidate explains
  nothing. → `G187`; detail in journal §14.1.
  📛 **LOGGED AS CHAT ERROR #6** (the numbering continues `#5`, the fabricated client observation).
  **RETRACTED BY THE OWNER on the receipt**, in his words: *"I gave inference dressed as testimony —
  the quoted submit line was accurate, the 'second armed frame' was not, and one grep beat it."*
  ⚠ Scope of the damage, measured rather than assumed: the false lesson was published **exactly
  once**, in `a57af4e`'s journal §14, and **it was explicitly labelled *"a hypothesis, not a
  finding"*** — the label is why it never hardened. It is corrected in the current tip by APPENDING
  (the retracted claim is shown beside the log), not by silent edit. ⛔ **Any wording of the form
  "a defect that first fires on frame 2 is invisible to a smoke test" is FALSE and must not be
  reintroduced.**
  ✅ **OPERATIONAL CONSEQUENCE, AND IT IS GOOD NEWS: THIS CRASH CLASS ANNOUNCES ITSELF WITHIN THE
  FIRST SECOND OF CAPTURE, NOT AFTER 90 FRAMES.** A Bates/Deimos run that survives its first armed
  frame's **EXECUTION** has cleared this failure mode. **The 90-frame floor governs DECLARING SUCCESS;
  FAILURE IS FAST.** Nobody should wait out a run to learn what the first second already told them.
  **(vii) MINIMUM ARMED-FRAME COUNT FOR BATES / DEIMOS = 90** — the full standard leg, evidenced by
  **90 files on disk** and `total_frames = 90`, guard 0, clamp 0. Below that m35 is **not reported
  working on that host**; not an `IAI.Capture.Shot`. ⚠ **A declared FLOOR, not a derivation** — 90 is
  the only count with a control behind it (every passing home leg ran it, and the guard leg confirmed
  90 arms for a 90-frame cap). Journal §14.2; it belongs beside §12's three-row table.
  🕳 **NAMED JOURNAL GAP — 2026-08-25 has NO session journal.** `master`'s tip `9f52cab`
  (*fix(capture): hold a targeted global anomaly for the whole capture, not per burst*) landed that
  day between session 060 (2026-08-24) and session 061 (2026-08-26). It is nonetheless **recorded**:
  in the m34 gate file's **A2.1** table and in this block's four-item staging line, under its branch
  twin **`5495aa6`**. **The gap is named, not filled — a reconstructed journal would be a fabrication**
  (`G120`).
  🔗 **`master` AND THE BRANCH CARRY FOUR CHERRY-PICK TWIN PAIRS** — `9f52cab`↔`5495aa6` ·
  `e9bf96d`↔`3363d5f` · `20c6a4e`↔`3be67fc` · `962dd29`↔`f5e3f0f`, **identical `git patch-id --stable`
  per pair**. ⇒ nothing exists on `master` IN CONTENT that the branch does not already carry, those
  four collapse at the merge, and `git merge-tree --write-tree master feature/mask-gpu-reduce`
  forecasts **CLEAN (exit 0, no conflict list)**. Merge-base `1a3b1eb`. Recorded so the office pass
  does not rediscover it.
  🧭 **COLD START: read `docs/sessions/2026-08-26-061-m35-readback-sub-rect-copy-handoff.md` — it is
  SELF-CONTAINED (the defect, the dead first design, the shipped design, the uncommitted file list and
  how to recover it, every gate result with numbers, the owner's ordered remaining sequence, and the
  traps).** Then `docs/predictions/2026-08-26-m35-build-b-gates.md` (pre-declared gates **and** the
  full results ledger, §12) and `docs/predictions/2026-08-26-m35-readback-layout-build-a.md`.
  🎯 **`m35` = a hotfix for a capture-readback crash observed on a SECOND HOST** ("Bates", a
  Bates-lineage UE 5.1 game): an access violation reading past a mapped readback buffer at a **non-zero
  view-rect origin** (letterboxed, `Rect.Min.Y = 104`, view rect 1389×581, `fmt = PF_A2B10G10R10`).
  🛑 **THE FIRST DESIGN IS DEAD AND ITS STOP CONDITION FIRED — do not resurrect it.** Measured on this
  box, both legs: `bufferHeight 869 == sourceExtent.y 869` ⇒ **stock UE 5.1 allocates FULL-SOURCE-SIZE
  staging and copies the sub-rect to its OWN position**; the layout changed at **UE 5.2**. So the
  unconditional offset removal is refuted by measurement AND by source, and the pre-m35 indexing is
  CORRECT here. ⛔ **A `bufferHeight`/pitch SNIFF IS ALSO REJECTED, AND THE REJECTION IS MEASURED:**
  `rowPitch 832` vs `width 821` = **11 px of padding** (832×4 = 3328 = 13×256, D3D12 256-byte
  alignment), so at a pillarbox narrower than the padding the two engine layouts are **numerically
  indistinguishable** and the sniff **fails silently inside its own blind spot**.
  ✅ **THE SHIPPED DESIGN MAKES THE ENGINE'S STAGING LAYOUT IRRELEVANT BY CONSTRUCTION:** copy the view
  sub-rect into a **plugin-owned W×H texture at (0,0)**, enqueue a **whole-texture** readback with **no
  rect at all**, and index the drain **sub-rect-locally**. We own the texture, so `bufferHeight ==
  picture height` on every engine. ⚠ **This REMOVES the discriminator a Bates/Deimos `READBACK-LAYOUT`
  photo used to provide** — what the photo decides now is tabulated in the Build B gate file §4.
  ✅ **GREEN SO FAR:** `G-M1` letterboxed — **exactly one field moved, `buffer_height 869 → 344`, and
  that IS the fix working** (written into the gate file BEFORE the leg, because it is precisely the
  misreading that would fire) · `G-M2` un-letterboxed — **NOTHING moved**, the leg that proves the
  shipped path did not move · `G-M3` guard proven **BOTH ways** (`G96`): inflate 1 ⇒ 90
  `READBACK-GUARD FIRED`, **0 frames on disk, no crash, run completed and wrote artifacts**; inflate 0
  ⇒ silent · **`G-M4` the display fix REPRODUCES** — `A-I1` 29/29 `overrideOutput=1` and `G-F2`
  **29 IDENTICAL / 0 FIRST-DIFF**, all 8 events in band with both endpoints (66,843 / 66,878) exact ⇒
  m35 has NOT disturbed `b05066f` · `G-M5` 3 of 5 legs — `SM_Ramp2` NOT_MEASURED ×8 with
  MEASURED_ZERO **0**, `BP_SplineSpawn_C` MEASURED_ZERO ×8 / 8 `VETOED-OBJECT` / annotation anomalies
  **0**, and m34's `COMPARE` **29 IDENTICAL / 0 FIRST-DIFF across two maps** · guard 0 / clamp 0 on
  every non-guard leg · **`P6` MEASURED unmoved (48 keys); `run_summary` gains `readback_layout` and
  nothing else.**
  🚨 **A CRASH FIRED AND WAS NOT A DESIGN FAILURE:** Build B's first leg died on
  `State != D3D12_RESOURCE_STATE_COMMON` — the D3D12 **transient** allocator derives an initial state
  ONLY from RT/DS/UAV flags, so the RDG texture needed `TexCreate_RenderTargetable`. ⚠ **The flag is
  added to the RDG texture ONLY** — the backbuffer texture is persistent and declares
  `SetInitialState(ERHIAccess::CopyDest)` directly. **The asymmetry is deliberate and is named in that
  path's own log line so nobody "tidies" the two into agreement.**
  🚨 **STRICT CROSS-RUN FRAME BYTE-IDENTITY IS UNOBTAINABLE IN EVERY ENVIRONMENT MEASURED** — even a
  same-binary control pair (`M33_CTRL_A` vs `M33_CTRL_B`) differs: mean |Δ| **0.00117**, max 3,
  **0.116 %** of pixels, concentrated in the lower half, top four grid rows exactly 0.000, **no corner
  box so it is NOT `G125`'s marker**; MainWorld is far worse (mean |Δ| 4.42, 78 % of pixels). ⇒
  `G-M1c`/`G-M2c` are **NOT OBTAINED, never FAILED**, and the replacement instrument is **`G-M9`, a
  WITHIN-FRAME DUAL-PATH COMPARATOR** (owner-designed, NOT YET BUILT — its premise, that RDG executes
  passes in handle order and never reorders, was **verified before building it**).
  🚨 **THE m11 PACER MASKS HOOK COST — median `t_wall` delta `0.03334` on ALL FIVE paced legs**, so a
  paced leg structurally cannot measure a sub-budget hook and **a rebuild cannot answer `G-M6`.** The
  prior is taken with **pacing OFF** by swapping the archived predecessor exe. ⛔ **NUMBERS ONLY, NO
  THRESHOLD**, reported per-captured-frame ms **and** per-megapixel, with any Concorde figure labelled
  an **EXTRAPOLATION**.
  📌 **CORRECTION CARRIED, OWNER-RAISED: the added copy is PER CAPTURED FRAME, NOT per armed frame** —
  confirmed from the call site (`CaptureCurrentFrame()` `AnomalyCaptureSubsystem.cpp:1736` mints one
  RequestId, arms one readback, and increments the index that NAMES the PNG) ⇒ **one arm == one
  captured frame == one PNG, 1:1.** The slip came from the MASK, which really does arm a few times per
  burst — this session's own receipts separate the rates (**90** guard fires for a 90-frame cap vs
  **29** `M23 PASS` mask arms).
  🚨 **THE BACKBUFFER PATH HAS NO FORMAT ASSERT AND NO GRACEFUL FAILURE BY DEFAULT:**
  `FValidationRHIUtils::ValidateCopyTexture` DOES carry a format `checkf` — behind
  `#if ENABLE_RHI_VALIDATION`, **OFF in a default Development build** — and D3D12's `RHICopyTexture`
  checks only block alignment. ⇒ a format mismatch there is **undefined behaviour**, and the structural
  guarantee (read the format from `BackBuffer->GetFormat()` every frame, recreate before the copy in
  the same block) is the **only** protection. Contrast the SVE path, whose `checkf` is unconditional.
  ⛔ **`IAI.Bench.Letterbox` CANNOT BITE PACKAGED — MEASURED, NOT ASSUMED:** the `-unattended` bench
  pawn is a **SpectatorPawn with no `UCameraComponent`** and the lever **REFUSED BY NAME**. 🎯 That
  refusal path is what stopped the leg becoming a false pass — its bbox and `READBACK-LAYOUT` were
  identical to an un-letterboxed run, so a silent no-op would have read GREEN. The leg is **VOID by the
  pre-declared rule.** ⇒ **`G-M7` (backbuffer), `G-M8` (pillarbox / non-zero `Rect.Min.X`, the origin
  half that has been ZERO in every leg ever run) and `G-M9`'s both-origins half must run in PIE on
  MainWorld.** Other bench cvars DO reach a packaged leg.
  📦 **IDENTITY (`G121`): staged exe `733FE83C` (archived), predecessor `7F37A4AC` (archived,
  hash-verified before the swap — it is `G-M6`'s A-side), container UNCHANGED m34 quartet
  (`2A66CA57`/`A7EF9B12`/`D8009AD7`), NO COOK.**
  🚨 **THE INSURANCE DIFF DID NOT APPLY AND WAS REGENERATED (session 062).** Both previously recorded
  copies were **unusable** — `7A0CC269…` (18,756 B, BOM + CRLF) failed `git apply --check` on **all 8
  files**, and `8479FFE7…` (18,374 B, LF but **no trailing newline**) was a **`corrupt patch`**. The
  recorded check had been "the diffstat matches", which is true and irrelevant; **insurance is
  verified by APPLYING it**. Now `_binary_baselines\m35-buildb-uncommitted-2026-08-26.diff`,
  **18,375 B, `sha256 1069B190…`, LF, no BOM**, written by `git diff --output=` and **`apply --check`
  exit 0 from its final location**. CRLF was the sole cause; the BOM is inert to `git apply`. The
  382-byte delta counts out exactly as 3 (BOM) + 378 (one CR per line) + 1 (trailing newline) → `G181`.
  📌 The stronger insurance is now the origin backup branch **`wip/session-061-backup`** — a whole
  commit on a remote, not a patch file on one disk.
  ⛔ **OWNER RULINGS, VERBATIM WHERE GIVEN:** *"Do NOT checkout master. The checkout lift I authorised
  last turn is WITHDRAWN"* (never exercised) · *"ONE ROUTE ONLY: m35 must not ALSO be cherry-picked
  onto master in parallel"* — master reaches the fix by the branch **MERGE** · *"Stop rule unchanged:
  any leg fails, report and stop, do not fix in the same turn"* · **"EVERY CHECKER IS PROVEN AGAINST A
  KNOWN ANSWER BEFORE ITS VERDICT IS READ"** — minted after a pose checker of mine read
  `annotation["camera"]` (which lives under `anomalies[]`), got `None == None`, and printed
  **"MATCHED"**.
  ⛔ **NOT DONE, named:** `G-M5`'s last two legs (`StaticMeshActor_73` and the `MaskProbe` leg —
  **run them from the banked configs in journal §7.1, NOT from the §7 payloads**) · `G-M6`'s fine
  prior (**`A,B,B,A` after a declared discard leg**) · **`G-M9` NOT BUILT** (premise (a) passed; (b)
  prove-it-can-fail and (c) cvar-OFF-reproduces are unrun because the code does not exist) · the
  `G-M8` column checker (its known-answer datum is recorded) · `G-M7`/`G-M8` PIE legs · **the fix
  commit + push**. **Master untouched and STILL CARRIES THE CRASH.**
- 🟦 *(superseded as "you are here" by the m35 entry above — still the record of sessions 053–057)*
  **2026-08-23 (sessions 053–057). ⛔ STILL NO TAG —
  `m31` REMAINS THE OPEN MILESTONE, STILL AWAITING CONCORDE V-3/V-4. Highest tag is still `m30`.**
  🎬 **m34 (GPU mask reduction) IS CLEARED TO START — its cold-start contract is
  `docs/sessions/2026-08-23-057-m34-gpu-mask-reduce-approved-plan-handoff.md`. READ THAT FILE
  FIRST if you are the m34 session: it carries the approved design, file list, gates G-R1..G-R7,
  chat's amendments A1–A3, and the branch rule (ALL m34 work on `feature/mask-gpu-reduce`,
  commits+pushes to that branch ONLY; master is the delivery line; merge gated on G-R7's Concorde
  leg; the Monday delivery pipeline preempts at any moment). m34 needs a FULL COOK (G129 — a new
  global shader cannot hot-swap) and does NOT enter the client delivery build.**
  🧭 **COLD START: journals 053 (toggling anomalies label the ACTIVE SUBSET), 054 (LOD-contrast
  gate · mask-pass hitching · tick-pin perf) and 055 (camera_clipping pool default · sparse overlay
  output) — each self-contained.** Plugin HEAD = the 055 set (`a75d601` · `6e143f2` · `4bc9739` +
  this docs commit); AnomDash `dcf2004`. Staged exe `DCF9C192`; **no cook since session 051 — the
  container is still that quartet** (utoc `E4FE9B35` · ucas `D9929F6F` · pak `BFB95333`).
  🩹 **HITCHING = the m26/m27 mask measure pass** (054 §7, owner-bisected). ⚖ **OWNER DECISION: THE
  MASK STAYS ON** — Mask 0 re-admits invisible-object labels (complaint #1); the hitch ships as a
  documented limitation. The named bisect remains `IAI.Capture.Mask 0`.
  🚨 **POST-055 (2026-08-23, chat-ACCEPTED diagnosis): THE SPED-UP-VIDEO DEFECT.** On the pinned
  decoupled fork the WORLD GAME CLOCK ADVANCES WITH WALL — owner artifact, keyed by
  `session_index`: labels `t` span **34.219865 s** vs `t_wall` span **34.220319 s** (equal to
  0.45 ms) against a fixed-step prediction of **3.967 s**; `ticks_per_captured_frame` 1.35
  (162 ticks / 120 frames — ticks STRETCHED, not multiplied). ⇒ `speed_ratio = WallSpan/GameSpan`
  (`ComputeRunPacing`, AnomalyCaptureSubsystem.cpp:2114-2123, game side `World->GetTimeSeconds()`
  :1680) **reads ~1.000 BY CONSTRUCTION at ANY starvation in that regime** — the m11 honest stamp
  never fires, and a mask-starved capture ships `video.fps=30` MP4s that play **2–5.75× fast**.
  📌 **CONSEQUENCES, RULED:** 054 §8's PINNED ratios (1.0006/1.0012) are CLOCK-AGREEMENT readings,
  NOT health · **the m21 ship rule (`speed_ratio ≤ ~1.05` ⇒ deliver) is VACUOUS on the pinned
  fork** — on Concorde read capture health from WALL math (frames/VideoFps vs the labels.jsonl
  `t_wall` span), never from `speed_ratio` · fork-Core mechanism line = a CANDIDATE, one
  office-side source read, logged NOT scheduled · content_clock override was NOT active (owner
  answer; run_summary read = evidence-grade closure in flight).
  🎯 **RULED AND NUMBERED (chat, 2026-08-23): `m32` is BURNED** on the pre-delivery bench legs
  (never-reuse; the hole is deliberate) · **`m33` = the GameSpan re-key + encode_watcher
  cross-check** · **`m34` = the GPU-side mask reduction — plan ACCEPTED with amendments A1–A3;
  ALL m34 work on branch `feature/mask-gpu-reduce` cut from master HEAD, commits and pushes to
  that branch ONLY; its own POST-delivery cook; merge to master gated on G-R7's Concorde leg;
  ⛔ NO m34 code before m33 is committed with G-A/G-B green — that condition is NOW MET, but the
  Monday pipeline still preempts branch work at any moment.** VFR is REJECTED — the "NO frame
  duplication, NO variable frame rate, ever" doctrine line STANDS unmodified.
  ✅ **m33 IS BUILT, HOME-GATED AND PUSHED (session 056, 2026-08-23): plugin `491eca5` + `03b0b7a`
  (predictions `0298143` first), AnomDash `132d27d`. ⛔ m33 CLOSES ONLY AFTER G-C (owner-run
  starved Concorde capture, post-cook); NO TAG.** `speed_ratio`'s denominator is now the
  plugin-owned tick span (stamped beside the wall stamps in `StampArmWallClock`); the old
  world-clock ratio ships beside it as **`run_summary.game_clock_speed_ratio`** (+1 field exactly,
  P6 unmoved — subset gate exit 1 with that as the ONLY extra, invariant core ALL IDENTICAL).
  Gates: G-A identity 1e-7, armTicks 119 (the withdrawn `(ArmedFrames−1)` form would have read
  **1.337 on a healthy leg** — the settle-gap flag, quantified) · G-B 40 ms stall ⇒ ratio
  **1.2374** (model 1.239), honest stamp FIRED end-to-end (`stamped 24.244 == annotation
  video.fps`) · G-W all four fixtures as pre-declared (healthy silent at 30; ×2-stretched loud at
  14.998). 🧭 **COLD START: journal
  `docs/sessions/2026-08-23-056-m33-gamespan-rekey-and-watcher-crosscheck.md`.**
  🔴 **CHAT-DECISION PENDING: the watcher ESTIMATOR deviates from the ruled expression** — the
  ruled `(N−1)/t_wall-span` under-reads every healthy gapped session by ~23 % (measured 22.99 vs
  30 on the fixture source; the plugin formula's settle-gap disease again), so the shipped form is
  `1/median(consecutive t_wall delta)`; pre-registered as AMENDMENT 1 (`491eca5`), ratify or
  revert. ⚠ Runbook §8.2's A44 example control `IsHideTypeAnomaly` is STALE (renamed at 053) —
  flagged, not yet edited. Staged bench exe **`757A5DD4`**; predecessor archived
  `_binary_baselines\StackOBot.exe.session055-DCF9C192`; container still the session-051 quartet
  (the delivery cook picks m33 up by pull). Legs banked `M33_CTRL_A/B`, `M33_GA`, `M33_GB`.
  🆕 **`G164`** — a killed build task left a TRUNCATED exe (~2 MiB vs ~240 MB) and the next
  `Build.bat` said "up to date" at exit 0 in 2.8 s: **verify artifact SIZE/hash after any killed
  build; exit codes and "up to date" prove nothing about wholeness** (055 §5).
- 🟦 *(superseded as "you are here" by the entry above — still the record of session 052)*
  **2026-08-22 (session 052). THE FINAL PRE-DELIVERY CHANGE SET: SEVEN
  PLUGIN COMMITS + TWO AnomDash COMMITS, SHIPPED THE DAY BEFORE THE CLIENT BUILD. ⛔ NOTHING WAS
  TAGGED — `m31` IS STILL THE OPEN MILESTONE AND STILL AWAITS CONCORDE V-3/V-4. Highest tag is
  still `m30`.**
  🧭 **COLD START: read `docs/sessions/2026-08-22-052-pre-delivery-exclusion-proximity-delivery-labels-overlay.md`
  — it is self-contained and covers all nine commits.**
  **Plugin HEAD `1821efc`.** Commits: `979a4d0` tick-pin probe hardening · `8ddaab6` target
  exclusion patterns · `2b0555c` lod_popping proximity gate · `f491514` labels.jsonl in delivery ·
  `3baa200` overlay tool ships · `ed9092c` + `1821efc` client docs.
  **AnomDash at `21d9fae`**: `7bf62a0` watcher/manifest/Run.bat · `21d9fae` packaging no longer
  requires a plugin repo.
  ⏱ **1. THE TICK PIN SHIPS ON BY DEFAULT — OWNER SHIP DECISION, VALIDATED ON CONCORDE**
  (packaged, in-round): `TICKPIN active saved=1`, 300 frames, 7 events, 5 measurable, EVERY offset
  **+0** at start and end, 4 of 5 HIGH confidence, measurable range ±7.
  **`ticks_per_captured_frame` 1.2000 pinned vs 1.2699 unpinned ⇒ THE PIN DOES NOT ACCELERATE
  ANOMALIES; the pre-registered blink recalibration DID NOT FIRE and NO ANOMALY CONSTANT CHANGED.**
  🚨 **THE `FWNetSubsystem.cpp` PROBE MARKER IS RETIRED — it came from prose, not a listing, and it
  MISSED on Concorde.** The probe is now a CONTENT probe for the literal token
  `sUseFixedGameTickWithVariableRenderTick_Net` (route A = the four known sites, `App.h` first;
  B = a capped content scan of `Source/Runtime/Core`; C = fork-named files/plugin folders).
  **ONLY THE SYMBOL DECIDES — a filename hit with no symbol is a HINT and sets nothing**, because
  setting the define on a name alone breaks the build wherever the symbol was renamed. The fork
  modified **CORE**, which is why a net-module filename missed. ⚠ `Engine/Plugins` is matched by
  folder name only — a full walk measured **18.6 s per build**; probe cost is now ~0.7 s on a stock
  miss, ~2 ms on a fork. 🔧 **BUILD-TIME OVERRIDE: an empty `ANOMINJECT_TICKPIN_FORCE_ON` /
  `_OFF` file at the plugin root**, both targets, no source edit, never silent (probe result then an
  `OVERRIDDEN` line). Registered as a UBT `ExternalDependency` so REMOVAL auto-invalidates the
  makefile; **CREATION does not — delete `Intermediate`.** 🚨 **NO `TICKPIN probe` LINE IN THE BUILD
  OUTPUT AT ALL ⇒ the makefile was cached and nothing was re-probed.** Guard proven BOTH ways
  (forced ON on stock fails naming the symbol at both sites, exit 6; forced OFF suppresses a FOUND
  probe) plus a **positive control**: a planted symbol makes the probe say FOUND, so its NOT FOUND
  is a reading and not blindness. Build-graph no-op MEASURED — generated
  `Definitions.AnomalyCapture.h` byte-identical pre/post (`1827B204256D`).
  🚫 **2. CONFIG-DRIVEN TARGET EXCLUSION — `[AnomalyInjector] ExcludedTargetNamePatterns`**, an
  array of case-insensitive SUBSTRING patterns matched against **ACTOR name, COMPONENT name AND
  MESH ASSET name**, at the **`G33` chokepoint** so it reaches selector, auto-injector and capture
  alike. Fixes the `lightblockerplane_sm` case (Bug A) and the always-in-radius **skybox** (Bug B —
  poll radius is computed on BOUNDS and a backdrop's bounds envelope the player). **Both are
  LABEL-QUALITY exclusions, the m27 foliage rationale.** **COMPILED DEFAULT EMPTY ⇒ byte-identical
  when absent**; cost when unset is one `Num()==0` test. `IAI.SetExcludedTargets` added for the
  `G88` reason (precedence **console > ini > compiled-empty**). One greppable **`EXCLUDED-TARGET`**
  line per actor naming pattern/field/value/**source**; `run_summary` gains
  **`pattern_excluded_targets`**; `annotation.json` unchanged. ✅ **Guard broken deliberately:
  `IAI.SetExcludedTargets SM_rock` took MainWorld auto-pool from 13 events to 5 with ROCK EVENTS 0
  and 155 actors refused, matching on the ASSET FIELD ONLY** — the rocks carry `SM_rock`/`SM_rock_02`
  while their actor names are uninformative `StaticMeshActor_UAID_...`, i.e. Bug A's exact shape.
  ⛔ **THE INI ROUTE IS UNTESTED HERE (`G88`)** — only the console route and the shared resolve/echo
  path are proven; **Concorde's cook proves the rest, read `excludePatterns=N(ini)[...]` off the
  StartRun line.**
  📏 **3. `lod_popping` METRIC PROXIMITY GATE — `[AnomalyInjector] LodPoppingMaxDistanceCm`,
  compiled default 200**, plus `IAI.Anomaly.LodMaxDistance`. Same metric as the poll radius
  (sphere-approx bounds distance from `ResolvePollOrigin`). **IT ANDs WITH THE m30 7.0 % COVERAGE
  GATE AND DOES NOT REPLACE IT** — coverage was CALIBRATED (visible 9.3453 / invisible 3.9045); a
  metric distance is an owner PRODUCT PREFERENCE, and **removing a calibrated gate to install an
  uncalibrated one is backwards.**
  🚨 **AT 200 cm `lod_popping` FIRES ZERO ON THIS BENCH — MEASURED, REPORTED, NOT SILENTLY SHIPPED.**
  A/B same seed/map/300 frames: gate off ⇒ 13 events / **2** lod_popping; gate at 200 ⇒ 11 events /
  **0**. Both draws refused on DISTANCE ALONE at **863.91 cm** and **1221.19 cm**, both having
  PASSED coverage (9.2572 %, 11.7191 %). ⚠ **This is a property of the BENCH, not evidence the
  number is wrong** — an unattended run settles at a fixed pose and the pawn never walks up to
  anything, so the owner's "player standing next to it" case cannot occur. ⛔ **NO VALUE
  RECOMMENDED — the owner decides; `IAI.Anomaly.LodMaxDistance` retunes with no re-cook.**
  📄 **4. `labels.jsonl` IS NOW WRITTEN IN DELIVERY MODE, default ON** (`m12` suppressed it while
  still COMPUTING it, so the overlay tool could not run in the config the client ships).
  **IT ADDS EXACTLY ONE FILE AND NOTHING ELSE** — measured: before `Actual_Frames/ annotation.json
  run_summary.json`, after the same **plus `labels.jsonl`**. `run.json` STAYS SUPPRESSED (seed still
  withheld) and **`annotation.json` keyset 48 vs 48 — `P6` DID NOT MOVE**. Off switch
  `IAI.Capture.DeliveryLabels` / `bWriteLabelsInDeliveryDefault`.
  🔍 **5. THE OVERLAY INSPECTION TOOL SHIPS — AND IT IS NOT A LABEL PRODUCER.** Engine labels stay
  authoritative; it draws onto COPIES for HUMAN INSPECTION and never edits a frame or a label.
  **RED** = in `annotation.json` (a shipped label); **AMBER** = candidate only, tagged
  `OUTSIDE-SUBSET` / `VETOED` / `NON-MANIFESTED` / `UNMATCHED`.
  📊 **PHANTOM BOXES DIAGNOSED FROM 389 BANKED SESSIONS: 12,548 shipped vs 8,790 candidate-only —
  hide-type SPAN-vs-SUBSET 7,919 (90.1 %, BY DESIGN) · VETOED 871 (9.9 %) · NON-MANIFESTED 0.**
  For hide types `annotation.json` carries only the HIDDEN frames while `labels.jsonl` covers the
  whole fire-active window. Veto category cross-checks against m27's independently recorded three
  vetoed targets. 🚨 **`G161` — THE JOIN KEY IS `session_index`, NOT `frame_index`;** my first pass
  used `frame_index` (the arm-time `GFrameCounter`, a different space) and produced 14,399 bogus
  hits. **The checker was wrong, not the build.**
  🚨 **6. PACKAGING NO LONGER REQUIRES A PLUGIN REPO (`G163`).** The PLUGINFILE entry derived the
  plugin repo from the dashboard repo's location and **broke packaging on the machine that actually
  packages** (dashboard at `D:\AnomDashboardV1\AnomDash`, no sibling plugin tree). **The refusal was
  CORRECT; the side-by-side assumption was wrong.** Cross-repo reach is now **OPT-IN**: without
  `--plugin-repo` the bundle builds from the dashboard repo alone and **exits 0** with a loud
  `ACTION REQUIRED` notice naming the omitted files and a success line reading
  **`9/11 ... (dashboard-only; 2 plugin-side file(s) NOT included)`**; with it, a missing file
  **FAILS LOUDLY** as before. ⛔ **`FILE`/`DIR` entries stay unconditional — verified.**
  ⚠ **DETERMINISM: THE SUBSET GATE EXITS 1 AND WAS NOT RELABELLED A PASS.** Invariant core ALL
  IDENTICAL, `annotation.json` keyset unchanged, and the ONLY non-labels extra is the one declared
  `pattern_excluded_targets`. The other 15 extras are `labels.jsonl` fields → **`G162`: labels.jsonl
  ROW ORDER varies run to run (async writer completion order) and THE CONTROL PAIR EXHIBITS IT TOO**
  (4 vs 8 positional mismatches, **0 field diffs when sorted by `session_index`**). Verification
  tooling, not a build defect; **deliberately NOT fixed**. ⚠ My own first check was order-BLIND
  (`Compare-Object` compares as a SET) and disagreed with the gate until the right instrument ran.
  📘 **CLIENT DOCS: the `labels.jsonl` ordering rule is now IN `client-readme.md`** — rows complete
  but NOT ordered, **key or sort by `session_index`, never sort or join on `frame_index`** — plus
  the amber-proportions table and the `camera_clipping` note (a whole-session global, so a
  first-person/held weapon is clipped in EVERY frame; expected, not a defect, owner-confirmed).
  ⛔ **NOT DONE, named:** no tag · ini route for the two new keys unproven here · `lod_popping`
  zero-fire at 200 cm is the owner's call · `G162` filed not fixed · intermediate commits
  symbol-checked not compiled (only the tip was built) · `P6` did not move ·
  `feature/stencil-capture` untouched at `76cac74` · no force-push · no ratio, no threshold.
  📦 **ENVIRONMENT: staged bench exe left at the m32 candidate `8F58661B`;** the as-found
  session-051 exe `DD76385F` is archived and hash-verified at
  `_binary_baselines\StackOBot.exe.session051-DD76385F`. **NO COOK WAS RUN THIS SESSION — the
  container is still session 051's quartet** (utoc `E4FE9B35` · ucas `D9929F6F` · pak `BFB95333`).
- 🟦 *(superseded as "you are here" by the entry above — still the record of session 051)*
  **2026-08-21 (session 051). SIX COMMITS AND ONE COOK ON TOP OF THE m31
  FIX. ⛔ NO MILESTONE WAS OPENED AND NOTHING WAS TAGGED — `m31` IS STILL THE OPEN MILESTONE AND
  STILL AWAITS CONCORDE V-3/V-4 (its entry is the next bullet down and is STILL LIVE, not
  superseded). Highest tag remains `m30`.**
  🧭 **COLD START: read `docs/sessions/2026-08-21-051-label-offset-instrument-material-flags-tickpin.md`
  — it is self-contained — THEN journal 050 for the m31 fix itself.**
  📦 **NEW BUILD QUARTET (`G121`): exe `DD76385F` · utoc `E4FE9B35` · ucas `D9929F6F` · pak
  `BFB95333`. Map gate PASS at exit 0 (CB_GateLevel + MainMenu + MainWorld + Entry, and CLEAN
  without `CB_LodCalib`, so the expected set was not silenced).** The m31 V-2 quartet it replaces is
  preserved and hash-verified at `_binary_baselines\m31-v2-container\` (5/5) with its exe at
  `_binary_baselines\StackOBot.exe.m31-v2-postfix-DC55CB9B`.
  **Commits:** `ea99d6b` tools move · `d3f3152` material usage flags · `98d04d4` ceiling banner ·
  `f999d7a` tick-mode pin · `b4e07e0` pin console override · `a3aa1e6` auto-pool anomaly defaults.
  AnomDash at `7922457`.
  🔧 **1. THE MEASUREMENT INSTRUMENT — `tools/measure_label_offset.py`.** Read-only; per annotated
  event it measures where the anomaly MANIFESTS IN PIXELS against where `annotation.json` CLAIMS it
  is. Python 3 + Pillow, `--selftest` built in. **Its own module docstring is the reference.**
  ⚖ **STANDING RULE: INTERNAL diagnostics ride the PLUGIN repo's `tools/`; CLIENT-SHIPPED host
  tooling stays in AnomDash `host-tools/`** (encode_watcher precedent) — the office pulls this repo.
  🚨 **ITS CEILING IS THE HEADLINE: the baseline comes from frames the ANNOTATION calls clean, so an
  offset larger than the clean gap between bursts CONTAMINATES that baseline — and the first version
  reported a confident, wrong, UNDER-READ offset.** It now prints
  `MEASURABLE RANGE +/-N (min clean gap G)` with `N = G//2`, raises
  `*** BASELINE CONTAMINATED ***` per type, and `--require-gap N` EXITS NONZERO.
  **On the standard `2 4 8 4 0` config that ceiling is about ±2 frames.**
  ✅ **Gates: G-C classifier 10/10 · G-B trusted sessions median +0 on blink, missing_texture and
  missing_object · G-A synthetic ±3 EXACT on four types × three variants · G-A real ±1 EXACT on nine
  fixtures.** ⚠ **±3 on a real bench session is NOT achievable** (the duty cycle forbids it) and no
  pass was manufactured for it.
  🎨 **2. MATERIAL USAGE FLAGS — a ship-visible defect, CONFIRMED ON CONCORDE.** Both shipped
  materials were missing `bUsedWithStaticLighting` (measured `False` on our own assets); now **7
  flags each**, set by headless idempotent `tools/set_material_usage_flags.py`, with
  `create_anomaly_materials.py` drawing from the same constant so re-authoring cannot reintroduce it.
  🚨 **THIS BOX STRUCTURALLY CANNOT REPRODUCE OR VERIFY IT — `r.AllowStaticLighting=False` in
  StackOBot's `DefaultEngine.ini` means `MATUSAGE_StaticLighting` is never queried here** (11 packaged
  logs, ZERO `LogMaterial` lines, no suppression configured). **Concorde's re-cook plus a
  `missing bUsedWith` grep is the ONLY verification that exists.** → `G157`.
  🚨 **ESCALATION, STATED NOT SOFTENED: previously delivered CONCORDE-CAPTURED datasets are
  affected.** Statically-lit STATIC-MESH targets drew the engine default material while the event was
  labelled manifested with full coverage. **These are WRONG-APPEARANCE samples, not
  labelled-but-invisible ones** — the default IS a visible change; but grey is not magenta
  corruption, so a model learned the wrong appearance for that class. Skeletal targets are unaffected
  (that flag was already set), which is exactly why the owner saw a magenta weapon beside
  grid-rendered props. ⛔ **No remediation attempted — owner's call, recorded not chased.**
  ⏱ **3. CAPTURE-TIME ENGINE TICK-MODE PIN**, behind a BUILD-TIME probe for the decoupled-tick fork
  🚨 *(⛔ **CORRECTED 2026-08-22 — THE `FWNetSubsystem.cpp` MARKER IS RETIRED. It came from prose
  rather than a verified listing and it MISSED on Concorde. The probe is now a CONTENT probe for the
  literal token `sUseFixedGameTickWithVariableRenderTick_Net`, and the pin now ships ON by default.
  See the session-052 entry at the top.** The rest of this bullet — the save/force/re-apply/restore
  behaviour, the log lines and the six `run_summary` fields — is UNCHANGED and still current.)*
  (marker `FWNetSubsystem.cpp` → `ANOMINJECT_FW_TICKPIN`); it compiles out entirely on stock and the
  probe LOGS its result either way. Save → force false → **re-apply every capture tick (a SET, never
  a toggle)** → restore at finish. **Guard proven by breaking it: forcing the define on stock FAILS
  the build naming `FApp::sUseFixedGameTickWithVariableRenderTick_Net` at both access sites, exit 6.**
  **One unconditional greppable line per run** — `TICKPIN active saved=<0|1>` /
  `TICKPIN disabled-by-ini` / `TICKPIN not-compiled (no decoupled-tick fork detected)` — naming its
  provenance. **`run_summary` +6 fields; `annotation.json` DID NOT MOVE** (m27 precedent):
  `tickpin_compiled/applied/saved/reasserts`, `capture_game_ticks`, `ticks_per_captured_frame`.
  📊 **UNPINNED HOME BASELINE MEASURED: `ticks_per_captured_frame` = 1.3556** — the reference the
  office comparison needs. 🎯 **`IAI.Capture.TickPin <0|1>` IS THE NAMED BISECT** (the
  `IAI.Capture.SVE 0` precedent), added because **`G88` makes an ini-only lever a no-op beside a
  package — without it the unpinned control leg would cost a SECOND COOK.** It exists and answers
  explicitly even on builds where the pin compiled out.
  🔩 **4. AUTO-POOL ANOMALY PARAMETER DEFAULTS** — `[AnomalyInjector]`
  `BlinkingHalfPeriodFramesDefault` (compiled 3) and `LodPoppingHalfPeriodFramesDefault` (compiled 8),
  plus console overrides `IAI.Anomaly.BlinkHalfPeriod` / `IAI.Anomaly.LodHalfPeriod` for the same G88
  reason. **Precedence: targeted-fire arg > console > ini > compiled.** Range `[1..600]`,
  **out-of-range REFUSED not clamped**; **absent key ⇒ compiled default, byte-identical**. Echoed on
  the EXISTING run-config line as `blinkHalf=3(compiled) lodHalf=8(compiled)`. **NO VALUE WAS
  CHANGED** — this is a lever so Saturday's pre-registered recalibration does not need a re-cook.
  🚨 **CORRECTION CARRIED FORWARD (`G160`): `preFrames` is a ONE-TIME LEAD-IN; `postFrames` governs
  the clean gap between annotated windows.** Measured: `2 14 8 4 0` left the gap at **4**;
  `2 4 8 14 0` gives **14**. `IAI.Capture.Config` DOES govern spacing on the auto-pool path (the FSM
  is targeting-agnostic). **Diagnostic capture = `IAI.Capture.Delivery 0` + `IAI.Capture.Config
  2 4 8 14 0`, then let `--require-gap 12` enforce it rather than trusting arithmetic.**
  🚨 **`G156` — DELIVERY MODE CANNOT SELF-VERIFY.** No `labels.jsonl` ⇒ no bbox ⇒ FULLFRAME region
  and no ambient ring. Measured against KNOWN offsets: a known-+0 `missing_texture` session reads
  **median startΔ +6 (WRONG)**, a known-+1 one goes **entirely unmeasurable**. **An offset of 1–6
  frames is NOT reliably detectable in delivery mode; verification is ALWAYS a delivery-OFF exercise.**
  ✅ **POST-COOK APPEARANCES UNCHANGED** — pre-fix vs post-fix cook, same seed and config:
  `corrupted_texture` MAG 8/CHK 0/OTH 0 both; `missing_texture` MAG 0/CHK 16/OTH 0 both, all offsets
  **+0**. So `missing_texture` reading CHECKER at home is the INTENDED appearance, **confirmed not
  refuted**.
  ⛔ **NOT DONE, named:** a DIRECT read-back of the usage flags from inside the cooked container —
  the string-scan instrument was **REJECTED because its control failed** (`bUsedWithSkeletalMesh`,
  known TRUE, is equally absent → blindness not a negative, `G159`) · no cadence compensation and no
  constant invented · no remediation of past datasets · `P6` did not move ·
  `feature/stencil-capture` untouched at `76cac74` · no force-push · no ratio, no threshold anywhere.
- 🟦 *(STILL THE LIVE MILESTONE — only the "you are here" marker moved to the entry above; m31 is
  OPEN and UNTAGGED)* **2026-08-21. `m31` FIX IS BUILT, GATED LOCALLY AND PUSHED — AWAITING
  THE CONCORDE LEGS (V-3 in-editor, V-4 packaged). ⛔ m31 DOES NOT TAG UNTIL V-4 PASSES.**
  🧭 **COLD START: read `docs/sessions/2026-08-21-050-m31-fix-handshake-rekey.md` — it is
  self-contained (the defect, the retraction chain, the Option-B design, every consumer re-keyed,
  the V-2 A/B results, the owner's office procedure).**
  🎯 **THE FIX (Option B, mechanism-independent BY DESIGN — the fork's exact mechanism is
  DELIBERATELY UNSETTLED and no code, comment or doc asserts one):** the SVE wanted-handshake no
  longer compares two independent reads of `GFrameCounter`. A PLUGIN-OWNED monotonic serial is
  minted ONCE at the arm site, keys `PendingSnapshots`, rides BOTH capturers (`ArmForCapture` and
  the SVE path's new pending-wanted FIFO), and is carried BY VALUE end to end; the publish site
  consumes the oldest intent for the next ELIGIBLE family (scene/reflection-capture families
  guarded out AND COUNTED); the ring key stays `FSceneViewFamily::FrameNumber` — the one value
  MEASURED-CORRECT on the broken host. **The backbuffer pattern transplanted — proven
  fixed/variable-safe BY DESIGN from source (mint-once / carry-by-value / consume-in-order; its
  one latent flaw, GFrameCounter-as-token uniqueness, is removed in both paths by the serial).**
  ⚖ **INVARIANT WIDENED (owner ruling, dated, in architecture.md §Game-agnostic): never let
  correctness depend on ANYTHING a host can redefine — engine globals included. No mode
  sniffing; no tolerance windows (clocks at different rates diverge without bound).**
  ✅ **V-2 PASSED, BOTH PATHS, TRUE A/B** (pre-fix legs on the certified m30 exe `99AE7526`,
  archived; post-fix exe `DC55CB9B`, built==staged, A44 both-encodings: 5 new tokens at exact
  multiplicity, retired symbol ABSENT): 90/90 frames · 8/8 events IDENTICAL pre-vs-post on BOTH
  paths (canonical gapped cadence) · `annotation.json` KEYSET IDENTICAL — **P6 unmoved,
  MEASURED** · ring 121/121/0 · run_summary differs by exactly the one pre-declared field.
  🎯 **THE GATE LINE (permanent, unconditional):** post-fix SVE leg reads `marksIssued=90
  publishesSeen=121 wantedMatches=90 submitsIssued=90 framesWritten=90 pendingWantedAtEnd=0
  maxPendingDepth=1` — handshake CONNECTED, and `maxPendingDepth=1` is the lockstep degeneration
  MEASURED: on a stock loop the FIFO reproduces the pre-m31 pairing exactly. A failed run
  localises itself: marks>matches = arm side · matches>submits = render pass ·
  submits>frames = pairing (per-frame drops now WARN — promoted from Verbose, G154) ·
  ineligible-family count exposes a mis-guard (Amendment 2).
  📌 **ALSO IN THE BUILD:** `G153` CLOSED — `IAI.Capture.Start` dequotes outDir and REFUSES
  LOUDLY at StartRun on a surviving quote char or uncreatable dir (refusal POSITIVELY tested:
  fired on `Q:\`, no run started) · run_summary `wanted_published` → **`wanted_matches`**
  (Amendment 3, name=semantics; counts publishes that consumed a pending arm; semantics line in
  capture-fps.md §Arm→frame pairing) · ⚠ the `IAI.Capture.Mask` help-string brief item was STALE
  — already fixed at m29, verified, refused as work.
  ⚠ **SEMANTICS ON DECOUPLED HOSTS, stated not buried (capture-fps.md):** on a fixed/variable
  fork the captured frame is THE NEXT ELIGIBLE FAMILY AFTER THE ARM (the backbuffer's
  m21/m22-characterised semantics; pre-m31 such hosts got NOTHING); `labels.jsonl frame_index`
  keeps its source (arm-time GFrameCounter) and on such hosts identifies the ARM TICK. Lockstep
  hosts byte-equivalent. **FIFO overrun is LOUD twice** (existing did-not-resolve WARNING +
  `pendingWantedAtEnd`).
  🧭 **PRE-REGISTERED: `docs/predictions/2026-08-21-m31-fix-validation.md`** — V-1…V-4 with
  both-path variants + V-3's failure branch (an unlocalisable zero is a gate failure OF THE
  INSTRUMENTATION). ⚠ **V-1 (StackOBot PIE) NOT RUN here — no editor/bridge in the session;
  stated, not skipped silently; packaged pair is the stronger instrument (G76) and the office
  procedure covers the in-editor instrument on the host that matters.**
  ⛔ **NOT DONE:** V-3/V-4 (office-side; owner procedure at journal 050 §10: pull → rebuild
  editor target → Play → one SVE capture → one backbuffer capture → grep the tokens named in
  journal 050 §6) · the mask system's measurement cadence under a decoupled loop (named;
  fails-safe NOT_MEASURED⇒ADMIT) · m27/m30 tags untouched · `feature/stencil-capture` untouched
  at `76cac74` · no ratio/threshold anywhere.
- 🟦 *(superseded as "you are here" by the fix entry above — kept as the m31-S1 record; the S1
  telemetry it describes was SUBSUMED by the fix's gate instrumentation, and its branch table
  R-1/R-3 was superseded by the fix-validation pre-registration)* **2026-08-21 (earlier).
  `m31` OPENED: THE SVE CAPTURE PATH — THE SHIPPING
  DEFAULT SINCE `m25` — PRODUCED ZERO FRAMES ON CONCORDE, THE FIRST HOST THAT WAS NOT THE TEST RIG.
  `m31-S1` (the instrumentation) WAS BUILT AND SHIPPED; the fix above followed.**
  🧭 **COLD START: read `docs/sessions/2026-08-21-049-m31-s1-sve-wanted-handshake.md` — it is
  self-contained (symptom, relayed diagnosis, source verification, what S1 measures, hand-off).**
  🚨 **STATED FOR THE RECORD: FIRST DEFECT EVER FOUND BY A SECOND HOST, AND THE SHIPPING DEFAULT
  PATH WAS THE BROKEN ONE.** SVE was certified across ten configs, every ratio regime, both delivery
  modes, over four milestones — every leg on THE SAME PROJECT. **Certification depth on one axis
  says nothing about a second axis** — second instance in three days (`m27`'s settled camera was the
  first). ⚠ **Milestone numbering verified at cold start: m28/m29/m30 ALL EXIST, highest tag is
  `m30`; chat's "last tag m27" record was stale by three. m31 is the correct next number — NO gap.**
  🎯 **THE SYMPTOM (Concorde, UE 5.1 source-built, m27 build, delivery ON):** SVE default ⇒
  session folder + `annotation.json` + `run_summary` written, `Actual_Frames/` EMPTY, `total_frames
  0`, 9 bursts fired, pacer clean · `IAI.Capture.SVE 0` ⇒ PNGs written normally, same build, same
  machine. **THE OFFICE DIAGNOSIS (relayed, then INDEPENDENTLY VERIFIED FROM SOURCE HERE — journal
  049 §6):** zero `submitted` lines, in-flight list EMPTY at every drain, `pendingAfter=120`, ring
  HEALTHY (`published=332 consumed=332 missed=0`) ⇒ the only silent exit on the chain is the
  `!Entry.bWanted` return (`AnomalySceneViewExtension.cpp:84-87`) — **verified the ONLY silent exit
  between a successful `LookupKey` and submission, and publish-time is `bWanted`'s ONLY writer.**
  The game-thread `MarkWanted(GFrameCounter)` systematically missed the publish-time
  `IsWanted(GFrameCounter)` — a DESIGN ASSUMPTION (exact frame equality across two sites whose
  ordering the engine does not guarantee) falsified by the first real second host. ⚠ **`bWanted`
  false is INFERRED from 0/120, not OBSERVED — S1 exists to close exactly that gap.**
  ✅ **`m31-S1` SHIPPED — DIAGNOSTIC ONLY, ADDITIVE ONLY, PERMANENT (the path failed silently end to
  end on its first real host; it earned permanent instrumentation, as m27's mask echo did):**
  `wantedPublished` beside the ring counters · a bounded per-publish trace (first 64, compiled
  constant; family frame + publish `GFrameCounter` + `bWanted` + last-marked frame + signed offset,
  ONE LINE PER PUBLISH so repeated game frames against differing family numbers read directly) · an
  UNCONDITIONAL run-end summary (wantedPublished=X of Y, offset min/max/mode + histogram) ·
  `run_summary` gains **`wanted_published` ONLY**. Tokens verified unique repo-wide before adoption;
  named ONCE, in journal 049 §7 and the predictions file, per the VETOED-OBJECT discipline.
  **NO BEHAVIOUR CHANGE: `bWanted`'s computation untouched · no default flip · `P6` NOT MOVED ·
  `annotation.json` unchanged · m27/m30 tags untouched · `feature/stencil-capture` untouched.**
  🧭 **THE READING IS PRE-REGISTERED — `docs/predictions/2026-08-21-m31-s1-branch-table.md`,
  committed WITH S1. RESTATE IT VERBATIM BEFORE ANY RESULT IS READ.** R-1 wantedPublished≈0 ⇒ miss
  OBSERVED, the measured offset becomes the fix's calibration, design to chat WITH the number ·
  R-2 wantedPublished≈120 ⇒ the loss is between publish and lookup, the ring is the suspect, STOP
  and report · R-3 mixed ⇒ report verbatim, chat rules. ALL branches: PIE licenses mechanism only
  (G76); the fix validates PACKAGED, same-seed before/after (the m27 count-gate shape). **THREE
  candidate fixes pre-registered, NONE authorised** (FIFO pairing — costs exact-frame identity;
  tolerance window — needs the number only R-1 supplies; instrument first — that is S1). **The
  pairing fact is banked for option 1's debate (journal 049 §8): `PendingSnapshots` is an exact
  `TMap::Find` with NO tolerance — a one-frame slip either drops on a VERBOSE-only log or SILENTLY
  MISPAIRS with the adjacent armed frame's snapshot.**
  ⚖ **ROLE RULING, PERMANENT: TWO Code instances exist. THIS BOX IS THE ONLY CANONICAL AUTHOR; the
  office instance is EYES, BUILDER AND RUNNER ONLY — it commits and pushes NOTHING; everything
  reaches Concorde BY GIT PULL; NOTHING leaves the office machine (the only outbound channel is
  what the owner reads off the screen).**
  📌 **FILED, NOT FIXED — `G153`:** a QUOTED `IAI.Capture.Start` outDir carries the quote characters
  into `RunDir` ⇒ `annotation.json` write fails. Fix candidate FOLDS INTO THE MILESTONE THAT FIXES
  THE HANDSHAKE — same cook — together with `IAI.Capture.Mask`'s stale help string (A3 PARTIAL) and
  `G118`'s placeholder token.
  ⛔ **NOT DONE, named:** no fix designed or authorised · the instrumented Concorde run has not
  happened (owner pulls at the office; results return by screen) · the ~2.8× publishes-per-wanted-
  frame lead (332 vs 120) is a CANDIDATE, NOT A CLAIM · a StackOBot leg of S1 would be a sanity
  check, not evidence about Concorde.
- 🟦 *(superseded as "you are here" by the `m31` entry above — `m30` is the last TAGGED milestone)*
  **2026-08-21. 🎯 `m30` IS SHIPPED AND TAGGED. THE MILESTONE IS CLOSED.
  BOTH BLOCKING EYEBALL GATES CONFIRMED BY THE OWNER: `G-P1` VISIBLE, `G-C1` VISIBLE.**
  🚨 **THE DELIVERED POOL IS FIVE — `blinking`, `missing_texture`, `corrupted_texture`,
  `lod_popping`, `camera_clipping`. This line is the SINGLE SOURCE the categorical
  `PRE-DELIVERY-CHECKLIST` box and `setup-runbook` both point at; catalog count is NINE.**
  📌 **RESIDUALS — recorded, deliberately NOT queued.** (1) **`G-10` still never confirmed against a
  running dashboard** — `corrupted_texture` and `lod_popping` are engine-side default-enabled so they
  fire regardless; risk is **cosmetic**; close it opportunistically at the next natural dashboard use.
  (2) **The pink material's `used_with_instanced_static_meshes` flag has never been exercised** (no
  instanced target has ever been drawn by the pool). Low risk — foliage is excluded at the `G33`
  chokepoint — but non-zero for non-foliage ISM targets, and ⚠ **the failure mode is SILENT: it would
  render default-gray and still measure `MEASURED_NONZERO`, so only an eye catches it** (the reason
  `G-4S` was an eyeball gate). (3) **The label projector's inverted rect on synthetic levels** —
  untouched, out of scope, stays filed.
  🧭 **COLD START: read `docs/sessions/2026-08-21-048-m30-lod-proximity-gate-and-camera-clipping.md`.**
  🎯 **`m30` = TWO POOL MEMBERS, BOTH DEFAULT-CHECKED. The delivered pool is now FIVE: `blinking`,
  `missing_texture`, `corrupted_texture`, `lod_popping`, `camera_clipping`.**
  ✅ **`lod_popping` FINISHES m29's DEFERRAL — the proximity gate is CALIBRATED, not chosen.**
  `MinCoveragePct = 7.0`, **bounds-projected screen coverage at PICK TIME**, stacked on the ≥2-LOD
  guard; below it Apply returns false through the same AMB-2 matched-zero path ⇒ **no fire, no label**.
  Bounds only, no pixel read (`G127`-safe). **Bracket: 9.3453 % VISIBLE / 3.9045 % INVISIBLE**, the
  visible signal collapsing **three orders of magnitude** between them (12,489 px → 14 px). Margins
  **1.79× above the invisible anchor, 1.34× below the visible one**, biased toward REFUSING because a
  positive label with no visible change is the dataset-poisoning direction.
  🔑 **WHY m29 COULD NOT CLOSE IT AND m30 COULD, IN ONE LINE: m29 read coverage from
  `annotation.json` and hit the `-1` sentinel and an inverted rect. THE GATE COMPUTES COVERAGE ITSELF
  — instrumenting THAT and logging it made the label projector irrelevant.** Its inverted-rect bug is
  untouched and out of scope. ✅ **Sound, not merely self-consistent: on the two rungs where both
  routes produce a number they agree exactly — 33.0365 vs 33.04, 9.3453 vs 9.35.**
  ⚠ **ONE QUANTITY THROUGHOUT: bounds coverage, NEVER drawn extent — they differ ~4× (the MainWorld
  rock reads 11.83 % bounds while DRAWING 2.78 %), so mixing them silently moves the threshold.**
  ✅ **`camera_clipping` IS THE FIRST GLOBAL-SCOPED POOL MEMBER.** Snapshot filter widened to
  `Object || Global` ⇒ it renders in the existing checkbox list, **no dashboard layout work**;
  `time_dilation` became eligible too and is **VERIFIED still hidden**, not assumed. Held for the whole
  session (applied in `BeginActualRun`, AFTER `StartRun`'s clean slate, reverted at `FinishRun`) and
  **NEVER routed through `TryFireOnce`**, which now skips Global ids — **the `"=ActorName"` misparse is
  removed STRUCTURALLY, not guarded against.**
  🚨 **THE DESIGN CALL, AND IT IS THE POINT: A FRAME IS LABELLED POSITIVE ONLY WHEN GEOMETRY IS
  ACTUALLY WITHIN THE NEAR-CLIP RADIUS** — a per-frame sphere overlap at the camera, bounds only.
  **The near plane being wrong is not the same as the viewer seeing anything wrong.** Labelling a whole
  session positive would ship thousands of frames showing nothing — the client's original complaint at
  scale — and **the m26 mask veto CANNOT catch it, because there is no target and therefore no mask.**
  ✅ **`P6` DOES NOT MOVE, VERIFIED:** the event key set and `run_summary`'s key set are unchanged.
  Whole-frame rides the existing shape as `coverage_ratio = 1` and per-frame `bbox_norm = 0,0,1,1`,
  empty `asset_name`, `coverage_pct` left at its `-1` sentinel (it comes from selection provenance, and
  a global anomaly has no selected actor).
  ✅ **BOTH BLOCKING EYEBALL GATES CONFIRMED BY THE OWNER (2026-08-21):** **`G-P1`** lod_popping
  visibly pops (MainWorld rock, LOD0 vs LOD3, **2,090 strong px in-bbox** against a 3,406 px
  out-of-bbox control channel carrying the level's movers) · **`G-C1`** camera_clipping visibly slices
  (**54.65 % of frame differs**; the wall fills the view OFF and is clipped away entirely ON).
  🎯 **The tag was HELD until both were answered and cut only afterwards — m29's lesson applied: a tag
  cut before the verdict cannot carry it.**
  ✅ **EVERYTHING ELSE PASSES.** **`G-P2′` CATEGORICAL** — `half_period_frames=8` **and 5 toggles at
  BOTH 30 and 60 fps**; under the old seconds design doubling fps halves the per-frame cadence ·
  **`G-P4` the gate FIRES** — rung D at **1.7681 %** refused, **0 `lod_popping` events in
  `annotation.json`** · **`G-C2` BOTH DIRECTIONS** — close pose **60 positive / 0 negative**, open space
  **0 / 120** · `G-C3` near-clip **10 → 100 → 10**, baseline read from the log not assumed ·
  `G-C4` toggling camera_clipping is **ACCEPTED** (the R1 trap, tested positively) · `G-9′` ·
  `G-8′` · **`G-R` regression TARGETED** — `corrupted_texture` and `missing_texture` both 5 events, all
  manifested, canonical spans, all `n=8`.
  ⚠ **HONEST LIMIT, NOT CLAIMED: `G-C2`'s "SAME SESSION" WAS NOT ACHIEVABLE.** Both directions are
  proven on the same build across **two** sessions; one session cannot show both because **no cooked
  level gives camera motion relative to nearby geometry.**
  ⚠ **A NEAR-VACUOUS TEST, CAUGHT (`G96`'s shape):** B4's non-interference gate was specified against a
  TARGETED leg, but session globals are deliberately skipped in targeted mode — so it would have passed
  **because the condition never occurs.** Non-interference is instead evidenced where camera_clipping is
  positive on EVERY frame, with the other anomalies' spans unchanged.
  📊 **`A6` FIRE RATE, REPORTED NOT TUNED: 8 `lod_popping` draws across four packaged auto-pool legs —
  2 SURVIVED the gate, 6 REFUSED (25 %).** The refusals are dominated by the **single-LOD** guard, not
  the new coverage gate: MainWorld's structural geometry is largely single-LOD/Nanite. **Nothing was
  loosened.** If 25 % is too thin in real play that is a pool-composition decision, not a threshold one.
  ✅ **HYGIENE: `CB_LodCalib` is EXCLUDED from the shipping cook and the map gate is CLEAN at exit 0
  with its expected set UNTOUCHED — the gate was not silenced to make it quiet.** ⚠ The gates needing
  that level (`G-P4`, `G-C1`, `G-C2`, the calibration) ran on the gating build, which differed from the
  shipping build **only** by its presence; the code is identical. Stated rather than glossed.
  🧪 **`G151` EARNED ITS KEEP THREE TIMES IN ONE SESSION.** The near-wall demo took three iterations —
  first too far (the query's `0 positive` was CORRECT, my geometry was wrong), then unlit (a black
  rectangle: a valid diff, a useless artifact), then lit. **Every intermediate frame was luma-checked
  before being trusted.**
  📦 **BUILD QUARTET (`G121`):** exe **`99AE7526`** · utoc **`3D4C02D9`** · ucas **`D15236B2`** ·
  pak **`BFB95333`**.
  ⛔ **NOT DONE, named:** `G-10` still never confirmed against a running dashboard · the label
  projector's inverted rect on synthetic levels is **untouched and out of scope** · instanced/foliage
  mesh class still never drawn by the pool for the pink check.
  🧭 **`P6` NOT MOVED · `feature/stencil-capture` UNTOUCHED · no force-push · no ratio proposed.**
- 🟦 *(superseded as "you are here" by the `m30` entry above — `m29` is the last TAGGED milestone)* **2026-08-21. 🎯 `m29` IS SHIPPED AND TAGGED. THE MILESTONE IS CLOSED.**
  🧭 **COLD START: read `docs/sessions/2026-08-21-047-m29-corrupted-texture-and-lod-popping.md`. It is
  self-contained and OPENS WITH TWO CORRECTIONS TO ITS OWN EARLIER REPORT — read those first.**
  🎯 **`m29` = `corrupted_texture`, a NEW 9th anomaly, object-scoped, DEFAULT-CHECKED in the delivered
  pool.** Solid magenta, **OPAQUE, two-sided, Lit** per-component material swap
  (`M_CorruptedTexture_Pink`, the plugin's second Content asset), the m17 revert contract MIRRORED
  (copied, not extracted — owner ruling: m17 is confirmed on Concorde's real
  `FWMasterSkeletalMeshComponent` and that gate cannot be re-run here, so an extraction whose
  regression gate is unreachable would be unmeasurable).
  ✅ **BOTH BLOCKING GATES PASS.** **`G-8`**: fired 3× from the auto-pool, `MEASURED_NONZERO`
  **104,300 px (11.32 %)** and **25,609 px (2.78 %)**, all discard buckets zero on non-Nanite targets
  declared before the leg; the third fire measured `MEASURED_ZERO` and was **correctly VETOED**, so
  **the m26 cure reaches the new id without anyone wiring it there**. **`G-4S`**: the Bot's
  **SKELETAL** mesh renders solid pink — not default-gray — **out of the cooked artifact**, which is
  what proves `used_with_skeletal_mesh` survived the cook (`G49`).
  ✅ **`G-4S` IS OWNER-CONFIRMED BY EYE (2026-08-21): "pink confirmed".** That was the last blocking
  item outstanding on `m29`, and **NOTHING IS NOW OUTSTANDING ON THIS MILESTONE.** ⚠ The tag `m29`
  was cut BEFORE that confirmation arrived and is **NOT being moved** — a tag object cannot be
  amended (no force-push, fix-forward; the `m25` precedent). The confirmation lives in this block and
  in journal 047's gate table.
  ⚖ **`lod_popping` — ITS TIMING FIX AND ITS ≥2-LOD GUARD SHIP; ITS POOL MEMBERSHIP IS m30.**
  🚨 **IT IS A CLIENT REQUIREMENT AND IT WILL SHIP. NOTHING HERE SAYS OTHERWISE.** It is deferred
  under the owner's PRE-AUTHORISED CONTINGENCY because its proximity gate could not be calibrated in
  one pass — not because of any doubt about the anomaly. It remains fully usable by targeted fire and
  by the selector; it is simply not in `GAutoPool`.
  ⛔ **STRUCK, AND MUST NOT BE CARRIED FORWARD: this session's earlier verdict that `lod_popping` is
  "not viable on well-authored content" / "produces no visible change".** It was drawn from a single
  test condition that structurally could not contain the effect — every leg ran under the shipped
  18 m poll radius only, with the target at ~3 % of frame. **The owner's actual requirement — "if the
  player is NEAR the object, popping happens" — was never in the test.** `G135`'s shape.
  🎯 **CORRECTED BY MEASUREMENT: `lod_popping` IS PLAINLY VISIBLE AT CLOSE RANGE.** LOD 1 vs LOD 4,
  two legs, matched camera, whole-frame pixels differing by ≥8/255: **33.04 % bounds coverage →
  66,615 px** · **9.35 % → 12,489 px** · farther rungs → **14 px** and **8 px**. **Three orders of
  magnitude of separation.** On real content, MainWorld's rock shows **2,133 strong px in-bbox**
  against an out-of-bbox control channel.
  🚨 **THE OWNER CAUGHT A VOID MEASUREMENT AND IT CHANGED THE ANSWER — `G151`.** The synthetic
  calibration level rendered **100 % BLACK** (`mean_luma 0.0000`, zero non-zero px of 921,600), and
  LOD 1 vs LOD 4 frames from it were **byte-identical**, which had been read as "coverage does not
  separate visible from invisible". **It was black-vs-black.** ⚠ **AND A CONTRADICTION WAS SITTING IN
  THE DATA, READ AS NOISE:** the mask reported a systematic one-directional difference while the
  colour frames showed nothing — **custom depth needs no lighting, so the mask saw real geometry
  while the frames carried no light.** ⇒ **`m19`'s "gate on PIXELS" in its third instance, and the
  first where the misleading number was a DIFFERENCE rather than a count. A LUMA CHECK NOW PRECEDES
  ANY MEASUREMENT IN A NEW OR REBUILT CAPTURE ENVIRONMENT.** Isolation was verified, not assumed: on
  the SAME build MainWorld read `mean_luma 107.95 / 99.14 %` while the synthetic level read `0.0000`.
  🛑 **WHY m30 AND NOT m29: `D3` FORBIDS A THRESHOLD UNTIL BOTH ANCHORS EXIST, AND ONLY ONE DOES.**
  Visible side measured (33.04 %, 9.35 %); **invisible side UNMEASURABLE** — the label projector
  returns **inverted rects** on that level (`bbox_px [945,205,335,257]`, right < left) and
  `annotation.json` gave the `-1` sentinel. MainWorld cannot supply it either: its only multi-LOD
  targets are the two rocks, both on the visible side, and every other pool target is single-LOD and
  refused before it can be measured. **A threshold placed on one end is invented, not calibrated.**
  📌 **m30 INHERITS A SHORT PASS:** `CB_LodCalib` exists, is cooked and is luma-gated
  (`CaptureBench/tools/make_lod_calib_level.py`); the two-leg strong-diff instrument exists; the
  visible anchors are measured. **It needs the invisible-side coverage — fix the inverted rect or read
  coverage by a route that does not depend on that projector — plus one or two farther rungs.**
  ⚠ **AND A REAL TRAP FOR IT: bounds coverage OVER-READS drawn extent — the MainWorld rock reads
  11.83 % bounds coverage while DRAWING 2.78 % of frame, ~4×. A bounds threshold is a proxy for a
  proxy** (`G149`, amended same-day).
  🆕 **`G149` AMENDED (append, nothing deleted)** — count 1 ⇒ certainly invisible **STANDS**; count ≥2
  ⇒ not certainly visible **STANDS**; nothing downstream catches it **STANDS**; **the missing variable
  is ON-SCREEN SIZE, not LOD authoring quality — CORRECTED.** Also corrected: comparing **adjacent
  frames within one leg** is an unsound instrument for a toggle; **two legs at fixed LODs with a
  matched camera** is the sound one.
  🆕 **`G151`** (a black frame and a null result are the same number) · **`G150`/`T4`** (adding a pool
  id re-rolls the seeded draw ⇒ banked auto-pool runs are non-comparable; regression legs must be
  **TARGETED**).
  ✅ **ALSO IN `m29`:** `lod_popping`'s wall-time-vs-Hz accumulator converted to **FRAMES** (F-BLINK's
  shape; default **8** — ⚠ 2 Hz at 30 fps is **7.5 frames**, so "reproduce exactly" was arithmetically
  impossible and 8 is the first half-period the old code yields) · the **≥2-LOD guard**, which fired
  **5× unprompted on real content** · the **`G139` `IAI.Capture.Mask` help string**, read back out of
  the staged exe **both directions** (new present, old absent) — this **clears m27 RULING 2** on the
  cook it required · **two stale catalog arg-specs** (`blinking` still declared a float `hz` arg,
  **stale since m23**) · `PRE-DELIVERY-CHECKLIST` gains a **CATEGORICAL** catalog box plus a
  delivered-pool box, and `setup-runbook` stops asserting **"seven"**, which it had since m3 while the
  catalog was 8 from m8.
  📌 **CORRECTED RECORD 1: `missing_texture` is CHECKERED, not magenta.** The live docs were always
  right; only two chat handoffs said magenta, and **historical handoffs are NOT rewritten**. The
  flat-magenta variant deferred at m8 (`G50`) ships here as its own id.
  📌 **CORRECTED RECORD 2: `camera_clipping` and `lod_popping` left the dashboard by an OWNER SCOPE
  DECISION for the M1 release, NOT by any architectural rule excluding globals from capture surfaces.
  Any note implying such a rule is HISTORY, NOT POLICY.**
  ⚠ **KNOWN AND ACCEPTED, owner aware, not a blocker:** at high coverage the label bbox covers the
  whole target while only the silhouette edge changes — an **over-claiming label**, the same accepted
  class as the foliage over-claim.
  📦 **BUILD IDENTITY (`G121`, the QUARTET):** exe **`1ABB8E3C`** · utoc **`FB7F958A`** · ucas
  **`A359878A`** · pak **`65C060A3`**. Preserved and 6/6 hash-verified at the new location beforehand:
  `_binary_baselines\m28-precook-build\` and `_binary_baselines\m29-gate1-build-14F45C34\`.
  ⚠ **The cooked container also carries `CB_LodCalib`, so the map gate reports an UNEXPECTED ENTRY
  (exit 2). That is EXPECTED for this build** — the level was added deliberately and deliberately NOT
  written into the gate's expected set, so the gate keeps an independent voice.
  ⛔ **NOT DONE, named rather than implied:** `G-10` not confirmed against a running dashboard (69/69
  unit tests + clean build only) · `G-P2′` and `G-P4` deferred to m30 with the proximity gate ·
  `G-4` exercised static and skeletal classes; **instanced/foliage was never drawn by the pool** ·
  the AnomDash branch `m29-pool-lod-popping` is pushed and **deliberately NOT merged** (it removes
  `lod_popping` from the client denylist, which is m30's change).
  🧭 **`P6` NOT MOVED · `feature/stencil-capture` UNTOUCHED · no force-push · no ratio, no threshold
  proposed anywhere.**

- 🟦 *(superseded as "you are here" by the `m29` entry above — `m29` is now the last tagged milestone)* **2026-08-21. 🎯 `m28` IS SHIPPED AND TAGGED. THE MILESTONE IS CLOSED.
  ALL NINE GATES `A`–`I` PASSED, AND THE OWNER SMOKE PASSED ON `MainWorld` UNDER SHIPPED SELECTION.**
  ⛔ **Do not start anything new unprompted — the next milestone is the owner's call.**
  🎯 **THE SMOKE'S STRONGEST RESULT: native and downscaled runs of the same seed produced an IDENTICAL
  EVENT OUTCOME — same kept anomaly, same 40 positive frames, and the SAME 4 VETOED EVENTS — with the
  `m26`/`m27` veto LIVE AND DELETING.** That is `m28`'s premise shown in real content: **a write-time
  downscale does not reach the veto.** Banked at `_bench_sessions_bank\M28_OWNER_SMOKE\`.
  🚨 **THE SMOKE CAUGHT A REAL REGRESSION THAT THE GATES STRUCTURALLY COULD NOT: the dashboard's
  `Start capture` BUTTON WAS UNREACHABLE.** The fourth control wrapped `.cap-row` to three lines and
  the panel — default `flex-shrink: 1` — was squashed (`clientHeight 209` vs `scrollHeight 290`),
  putting the button outside the visible column. **In the DOM, enabled, and unclickable.** Fixed with
  `.col.right > .panel { flex-shrink: 0; }` — `overflow: auto` was already there and never got to
  scroll. ⚠ **`read_page` saying an element EXISTS is not evidence a human can REACH it, and a
  wire-only verification would have shipped it** — `GATE E`'s fourth leg had already proven the wire.
  ✅ **The dashboard control is now VISUALLY VERIFIED**: `size` sits beside `format`, defaults to
  *native (as rendered)*; 540p ⇒ *from **PER-RUN ARGUMENT***; native ⇒ *from **COMPILED DEFAULT** … no
  per-run argument*, **proving the field is OMITTED, not sent as 0.**
  🧭 **Gates pre-declared VERBATIM at
  `docs/predictions/2026-08-20-m28-gates.md` (+ AMENDMENT 1 + an ADDITION made during the run).
  Read that file before reading any result.** Journal:
  `docs/sessions/2026-08-20-047-m28-output-resolution.md` — §11 results, **§12 the vacuity
  finding**, §14 closing rulings. Artifacts banked at `_bench_sessions_bank\M28_GATES\`
  (4 sessions, BOTH logs, 139-file manifest).
  🚨 **THE MOST IMPORTANT THING IN THE MILESTONE IS A HOLE IN THE GATE, NOT IN THE CODE:
  `GATE D` — the control the whole design rests on — PASSED ON AN EMPTY RUN.** Both legs held
  ZERO anomalies (`positive_frames=0`; the 6 % coverage cull removes all 69 candidates in an
  875×869 PIE panel), and comparing two empty sets returns equal. **The emptiest possible run
  produced the gate's cleanest pass.** Caught by an **un-pre-declared counter-check** —
  *`bbox_px` rows differing MUST be > 0* — then re-run with 19 valid bboxes per leg.
  ⚖ **`GATE D` IS NOT AMENDED RETROACTIVELY: it passed on its written terms BOTH times and its
  written form REMAINS VACUOUSLY SATISFIABLE.** → new standing rule **`G146`**.
  ⚠ **EVERY GATE LEG RAN WITH `IAI.SetMinScreenCoverage 0` AND `IAI.Capture.FocusGate 0` — LEG
  CONDITIONS, NOT DEFAULTS. Neither shipped default changed.** ⇒ **nothing in the gate set
  exercised `m28` under the shipped selection behaviour; that is what the owner smoke closes.**
  🎯 **`m28` = ONE capture knob, a target output HEIGHT, as a DOWNSCALE ON WRITE. The render stays
  native; only the WRITTEN frame is resampled.** ⛔ **THERE IS NO WIDTH PARAMETER AND THERE MUST
  NEVER BE ONE** — width is derived from each frame's own aspect, so a non-aspect-preserving output
  is **UNREPRESENTABLE, not guarded against**. `0` = native and the written bytes are identical to a
  pre-`m28` build.
  🚨 **WHY THIS SHAPE IS SAFE, ESTABLISHED FROM SOURCE BEFORE ANY CODE: the `m26`/`m27` mask counts
  at the VIEW'S RENDER RESOLUTION (`SceneColor.ViewRect` at the Tonemap pass) and NEVER SEES THE
  CAPTURE OUTPUT BUFFER, so a write-time downscale STRUCTURALLY CANNOT REACH THE VETO.**
  `AnomalyViewport.*`, `AnomalyMaskMeasure.*` and `AnomalyMaskSceneViewExtension.*` gained **not one
  line** — that is the premise, and **`GATE D` is the control that proves it** (every `bbox_norm`
  identical across a native/downscale pair at one seed; if any MOVES, the design is wrong and the
  standing instruction is STOP AND REPORT, no same-turn fix).
  🎯 **THE DEFECT `m28` ACTUALLY FIXES, from survey S0:** `annotation.video.resolution` came from
  `GetViewportSize()` at `StartRun`, **not from any written frame**, and in DELIVERY MODE
  `labels.jsonl` is not written at all — so **a delivered dataset contained NO artifact recording
  the true dimensions of its own pixels.** It now comes from the **FIRST WRITTEN FRAME**.
  **`run.json`'s `viewport` is UNCHANGED** and still reports `GetViewportSize()`; the two fields now
  answer different questions on purpose. **No new fields, no new counters in any delivered artifact.**
  📌 **PRECEDENCE (`G139`'s pattern, applied before it could bite):** per-run argument (dashboard
  `outputHeight` / console `oh=<n>`) → `IAI.Capture.OutputHeight` → `DefaultGame.ini`
  `[AnomalyCapture] CaptureOutputHeightDefault` → compiled default `0`. **`-1` means ABSENT and `0`
  means a deliberate NATIVE** — the sentinel is what keeps every level distinguishable, so **`m27`'s
  FINDING 3 disjunction problem does NOT recur here.**
  ⚖ **`D7` WAS SELF-CONTRADICTORY AGAINST `D3` AND THE RULING WAS TO CUT, NOT ADD.** At `StartRun` no
  frame has been grabbed, so naming "native WxH" there could only come from `GetViewportSize()` — the
  source `D3` forbids. **The `StartRun` line therefore carries REQUESTED HEIGHT + PROVENANCE ONLY**;
  the authoritative pair is logged from the first written frame. **The viewport-vs-frame disagreement
  was ALREADY instrumented by the RESOLUTION DELTA (3-rect) line — a second predictor is duplication,
  not evidence.** `GATE B` was re-aimed at the measured line, recorded as **AMENDMENT 1 before any
  leg ran** (a TIGHTENING; it now tests the whole chain from grabbed frame to written file).
  🆕 **`GATE I` ADDED for the same reason the defect existed:** every other leg runs delivery OFF
  because `GATE D` needs `labels.jsonl`, so **without it the fix would never be tested in the mode
  client captures actually use.**
  ⚠ **`D6` ASKED FOR ONE RESAMPLE SITE AND THE THREE WRITE PATHS DO NOT CONVERGE ON A SAFE ONE.**
  Reported rather than duplicated: the nearest common function is `AnomalyPreview::EncodePixels` and
  it is **CONTAMINATED — `AnomalyPreviewTee.cpp` calls it too**, so resampling there would silently
  downscale the live preview, which is the dashboard's click-to-select coordinate frame. As built:
  **ONE implementation, TWO invocation points, preview untouched BY CONSTRUCTION.**
  🎯 **`bbox_px` NEEDED NO NEW CODE** — it is already computed from the same `W,H` the labels carry,
  so threading ONE derived pair fixes the resampler, `labels.jsonl` `width`/`height`, every `bbox_px`
  and `video.resolution` together. **Two derivations would be two chances to disagree.**
  ✅ **`W1` FIXED, and it was worse than "a typo bug": `bPng = !Format.Equals("jpeg")` meant `"jpg"`
  silently produced PNG while the console accepted both spellings, every unrecognised string became
  PNG with NO warning, and the failure is INVISIBLE IN THE ARTIFACT because `run.json` faithfully
  records `"png"` — the value actually used.** → **`G144`**. ⛔ **`W2` FILED NOT FIXED** (the format
  is never echoed back) under **`G139`**, not the mechanisms ledger — owner-ruled.
  🆕 **`G143`** (`git tag -l --format='%(objectname:short)'` prints the TAG OBJECT for an annotated
  tag, not the commit — it made a CORRECT repo look mismatched at cold start; **use
  `git rev-parse --short <tag>^{commit}`**, now folded into the Workflow rules below) · **`G144`**
  (a parser mapping "everything else" to a default turns a typo into a silent behaviour change).
  ⚠ **DECLARED RESIDUAL, stated BEFORE measurement:** even-snapping both axes lets the delivered
  aspect differ from the render's by up to ~one part in a thousand. **Labels stay EXACTLY consistent
  with the delivered pixels** (same derived pair everywhere); only the render-vs-output aspect drifts.
  Sub-pixel, not gated. ⛔ **NO ALIGNMENT CLAIM IS EXTENDED — `m25` certifies 1280×720 and 1281×721
  ONLY, and `m28` certifies nothing at any new size.**
  ⚠ **I DID NOT VISUALLY VERIFY THE DASHBOARD CONTROL** — `App.tsx` returns `ConnectScreen` until a
  live WS connection exists, so it is unreachable without PIE. It is in the owner's runbook.
  ⛔ **`H6` REMAINS DOCUMENTED, NOT FIXED · `P6` DID NOT MOVE · `m26`/`m27` tags NOT moved ·
  `feature/stencil-capture` UNTOUCHED at `76cac74` · NO RATIO OR THRESHOLD ANYWHERE.**
- 🟦 *(superseded as "you are here" by the `m28` entry above — `m27` is the last SHIPPED, TAGGED state)*
  **2026-08-20. 🎯 `m27` IS SHIPPED AND TAGGED. THE MILESTONE IS CLOSED.
  THE OWNER'S PLAY-GATE SMOKE PASSED IN REAL GAMEPLAY.** Plugin `4a92962`, tag **`m27`**, pushed;
  AnomDash `b8273c2`, pushed. ⛔ **Do not start anything new unprompted — the next milestone is the
  owner's call.**
  🎯 **`m27` = three unrelated problems that became visible while `H6` was being written up. IT IS
  NOT AN `H6` FIX AND MUST NEVER BE READ AS ONE — `H6` REMAINS DOCUMENTED, NOT FIXED.**
  **(A)** `[AnomalyCapture] bMaskMeasureDefault` — without it the `m26` cure **did nothing in a
  delivered build** (compiled default `false`), so a client build labelled exactly as `m25` did.
  Compiled default **STAYS `false`**; the ini carries delivered behaviour; `IAI.Capture.Mask` still
  overrides. **(A2)** the **`VETOED-OBJECT`** readout — one line per removed event (target,
  `asset_name`, `component_class`, state, `maxCount`, translucency verdict) + `run_summary`
  `translucent_vetoes` / `translucency_unknown_vetoes`. **(A3)** the bisect switch's help now says
  what it is and that it acts **BETWEEN RUNS**. **(F)** `AInstancedFoliageActor` **excluded from
  selection**. **(B)** the client is back on the **browser** workflow with a **manifest-driven
  bundler**.
  🚨 **THE LOAD-BEARING PART IS NOT THE KEY — IT IS THE ECHO (`G139`). Until `m27` the log said
  NOTHING about the mask when it was off**, so a delivered session with a missing, misspelt or
  `G88`-silenced key produced a log **byte-identical** to a deliberate off. `StartRun` now echoes the
  **EFFECTIVE value AND its PROVENANCE unconditionally**, and says in the line itself that a loose
  ini beside a package is a no-op.
  ✅ **ALL GATES PASS.** `Gate 1` (a `bTagFailed` cannot manufacture a `MEASURED_ZERO` — the
  `continue` at `AnomalyMaskMeasure.cpp:180-185` **precedes** the arm, so it lands `NOT_MEASURED`
  ⇒ ADMIT) · `Gate P` (bundler, **run twice**, 9/9 entries, 24 files, served with no repo access,
  bogus plugin repo ⇒ exit 2 and no bundle) · **`Gate F`** · **`Gate 2`** · **`Gate 3`, four legs**.
  🎯 **`GATE 3`, EVERY READING FROM THE `StartRun` ECHO (A48), NEVER FROM THE VALUE SET:**
  **1** key absent ⇒ *mask off, **COMPILED DEFAULT (off); no ini key present*** (0 vetoed, 8 events)
  · **2** key True ⇒ *mask **ON**, from **`DefaultGame.ini`*** (**6 vetoed, 2 events**)
  · **3** key True + `IAI.Capture.Mask 0` ⇒ *mask **off*** — **the bisect beats the ini** (0 vetoed,
  8 events) · **4** key True + **delivery ON** ⇒ all six `VETOED-OBJECT` lines present **and** both
  new counters in `run_summary` — **identical veto outcome to leg 2, so `G-9` orthogonality holds at
  `m27`**.
  🚨 **THE COUNT GATE WAS VERIFIED AGAINST THE ARTIFACT, NOT AGAINST ITSELF.** Legs 1 and 2 ran the
  **same seed, map and exe**, so leg 1 is a true BEFORE picture: differencing the two event sets by
  `(target, anomaly_type, start_frame)` gives **6 events actually missing**, against
  `vetoed_events = 6` and `countedEventsBefore=8 After=2`. **`vetoed_events` is the owner's ONLY
  signal that anything was deleted — an off-by-one there is completely silent to him.**
  ✅ **`G88` SATISFIED POSITIVELY:** `bMaskMeasureDefault` read back **out of the cooked
  `StackOBot-Windows.pak`**, `TESTVALUE123` absent, and **no loose `Config` dir exists beside the
  package at all**. ✅ **`G118` stays closed** — the enforced 64-char token was read from the
  **running process's own log**, not the source ini.
  🆕 **FINDING 1 — THE ACCEPTED COST HAS ITS FIRST MEASUREMENT: `translucent_vetoes = 1`.** The owner
  accepted route (e)'s cost with **nobody having measured it**; on the first leg that could report
  one, `BP_SplineSpawn_C` / `SM_GenericPlane` was vetoed with **`translucentSlots=1/1`**.
  🆕 🚨 **FINDING 2 — AND THE SAME LEG CONSTRAINS ROUTE (e). SAME ACTOR, SAME SESSION, 32 FRAMES
  APART:** `blinking` (tag 203) ⇒ **`MEASURED_ZERO`**, `maxCount=0`; `missing_texture` (tag 205) ⇒
  **`MEASURED_NONZERO`, `maxCount=114,724` = 12.4484 % of frame**. **Both fully interpretable** —
  `arms=4 resolved=4 framesContributed=4` with `framesDiscarded`/`framesResidual`/
  `framesUnconfirmed`/`framesNoPass`/`probeArms`/`collisions`/`tagFailed` **ALL ZERO** on both.
  ⇒ **if translucency simply prevented custom-depth writes, this actor would read zero in BOTH. It
  did not.** ⛔ **NO MECHANISM CLAIMED, NONE GUESSED (`G120`); the `skippedHidden` 5-vs-0 difference
  is a LEAD, NOT A FINDING.** 📌 **Consequence that travels: `translucent_vetoes` counts a property
  of the TARGET, and is NOT evidence that translucency caused the zero — this very leg is the
  counter-example. The shipped log wording already hedges ("its zero MAY mean…"); that hedge is now
  load-bearing.**
  ⚠ **FINDING 3, AN HONEST LIMITATION, REPORTED NOT FIXED:** when the mask is **ON** the echo names
  its source exactly, but when **OFF** it reads *"COMPILED DEFAULT (off) **or** `IAI.Capture.Mask`"*
  — **a disjunction that alone cannot separate "key missing" from "console turned it off"**, the two
  states `G139` exists to separate. **The `Initialize` banner + the `StartRun` echo TOGETHER do
  resolve it; the `StartRun` line alone does not.** Not fixed — no same-turn change to a line four
  legs were just read from.
  ⚠ **`G121`, THIRD AND CLEANEST INSTANCE: THE EXE HASH IS IDENTICAL ACROSS BOTH COOKS
  (`18081D39`, same mtime) WHILE ALL THREE CONTAINER FILES CHANGED.** Cook #1 (no key)
  `utoc 9ABDEC35 · ucas AED3DEC2 · pak 2047F41D`; **cook #2 (the `m27` candidate)
  `utoc 72262793 · ucas 6C26C482 · pak 0BEA8D24`**. **Two builds answer to the same exe hash — here
  the exe half carries ZERO information.** Cook #1's quartet preserved at
  `_binary_baselines\m27-cook1-nokey-build\`, **6/6 hash-verified at the new location (A62)** —
  **leg 1 is only reproducible there, because it tests the ABSENCE of the key.**
  🚨 **`F1` REFUTED THE STATED REASON FOR THE FOLIAGE EXCLUSION, FROM OUR OWN BANKED DATA.**
  *"HISM keeps rendering after `SetActorHiddenInGame`"* is **NOT SUPPORTED** — journal 045 Parts
  Nine/Ten (post marker-correction) measured a whole-frame mean of **0.0059** in **4 of 64 cells,
  peak 0.1242**, exactly where the bushes are; if the hide did nothing that number is zero.
  **The OBSERVATION stands; the EXPLANATION was never verified and must not be written as the reason
  again (`G120`'s shape, caught before it reached a doc).** **The real reason is that ITS LABEL IS
  UNUSABLE:** `coverage_pct 100` and `bbox_px` = the entire frame while ~1.4 % changes.
  ⚠ **COST, AT ITS REAL SIZE: MainWorld's settled SELECTABLE pool is about SIX actors, not ~350** —
  so the exclusion removes **roughly a third of the selectable variety**. ⚠ **But my prediction that
  the pool would thin to four was itself corrected by measurement: the seeded selection BACKFILLED
  and `m27`'s auto-pool leg selected SIX distinct actors**, two of which (`BP_SpawnPad_C`,
  `SM_Ramp2`) had not appeared in the pre-change smoke. **Count held; MEMBERSHIP changed** (`n=1`
  leg). **`G140`**: the same seed now picks different targets ⇒ **every banked MainWorld auto-pool
  run is NON-COMPARABLE across this commit.**
  ⛔ **The August ruling that a class blacklist is NOT a fix for `H5` STANDS — scoped, not
  reversed.** Oversized bounds stay open for every other actor; `BP_SpawnPad_C` (a plain
  `StaticMeshComponent` at `poll_distance −114.8`) is the named example no foliage blacklist touches.
  📌 **`Foliage` added to `AnomalyInjector`'s PRIVATE deps by owner ruling** — recorded in the
  Invariants above as a dated ruling, the treatment `InputCore` got at `m5`. **A class-NAME string
  match was REFUSED: a rename makes a name match silently stop excluding, while a type reference
  BREAKS THE BUILD.** `AInstancedFoliageActor` is `MinimalAPI`, which does not export member
  functions **but DOES export `StaticClass()` — all `IsA<T>()` needs.**
  🆕 **`config.json` MOVED BACK INTO `dashboard/`, AND THE HISTORY IS RECORDED SO IT IS NOT
  RE-BROKEN.** The app fetches `./config.json` **relative to the served root**. `c32f858` + doc
  `6d01bc9` (2026-07-21) **agreed** on `dashboard/`; the Tauri commit `7963be5` (2026-07-22) moved it
  to the delivery root — **correct for a desktop app** — and the doc was not updated. **The doc was
  stale FOR THE TAURI ERA ONLY.** Left as it was, **`m27` would have shipped a dashboard that LOADS
  AND SILENTLY FAILS TO AUTHENTICATE.** `Setup.bat` now **ASSERTS it is fetchable over HTTP**, proven
  both ways (present → exit 0, removed → exit 4).
  📦 **DELIVERY:** `bundle_manifest.txt` is an **ALLOWLIST, not copy-except** — a client-facing file
  not listed **does not ship**. **`PLUGINFILE` is a CROSS-REPO entry** (`docs/client-readme.md` →
  the bundle's `README.md`); a missing one **fails, names the path searched, and deletes the partial
  bundle**. The bundler **does not copy itself** and **never copies the dev `config.json`** (it
  carries the owner's token). ⚠ **The real risk it guards is not a missing file — it is a STALE
  `dist/`**, so it refuses without a build, demands typed confirmation when `src/` is newer, and
  prints `dist/`'s build time every run. **Tauri is untouched and simply does not ship.**
  ⛔ **DELIBERATELY LEFT OUT:** no disclosure about vetoed labels, `H6` or the known limitations was
  added to `client-readme.md` — **that is a COMMS decision and it is the owner's** · **`L3` is
  unfixed and now visible in `m27`'s own artifacts** (leg 2's `labels.jsonl` asserts positives for
  events `annotation.json` no longer contains; **no client impact** — delivery mode never writes it —
  **but owner-side overlay tooling will draw boxes for vetoed events**) · `SM_GratIng`'s
  drawn-to-claimed ratio was **not measured in any leg**.
  🆕 **`G139`** (echo the EFFECTIVE value **and its PROVENANCE**) · **`G140`** (changing the
  selectable set changes seeded selection ⇒ banked runs stop being comparable) · **`G141`**
  (PowerShell `-Encoding utf8` **WRITES A BOM** — it corrupted files **twice in one session**; use
  the editor tool, or `File.AppendAllText` with `UTF8Encoding($false)`).
  🎯 **THE OWNER PLAY-GATE SMOKE — PASSED, real gameplay on `MainWorld`, and HE CAPTURED TWICE, which
  between them exercised BOTH SIDES OF THE VETO:** `session_20260820-211024` **8 events → 5 kept, 3
  VETOED** · `session_20260820-211345` **8 events → 8 kept, 0 vetoed**. **CHECK 1** the echo names the
  ini · **CHECK 2** `vetoed_events` matches the `VETOED-OBJECT` line count · **CHECK 3** zero foliage in
  `annotation.json`, `labels.jsonl` AND `selection_provenance.json`, pool alive with
  **`BP_MovingPlatform_C` and `BP_PressurePlate_C` — targets no bench leg has ever selected**.
  🚨 **THE ZERO-VETO RUN WAS READ AS INTERNALLY CONSISTENT, NOT AS A SILENT FAILURE:** it produced
  **5 `NOT_MEASURED` / 3 `MEASURED_NONZERO` / 0 `MEASURED_ZERO`** with `mask.provided` matching the
  tri-state on all 8 rows ⇒ **zero vetoes is the CORRECT OUTPUT.** **A zero shown to be the right zero
  is worth more than a green tick.**
  🆕 🚨 **THE ARC'S MOST DECISION-RELEVANT FINDING — STATED ONCE, NOT RESTATED HERE:
  `docs/invisible-anomaly-mechanisms.md` → "THE CURE'S REACH IS VIEW-DEPENDENT AND CAN BE LOW",
  beside the `H6` entry.** Two runs **eleven minutes apart on the SAME BUILD** went **1-of-8 and
  5-of-8 `NOT_MEASURED`** (`mask_nopass_discards` 4 → 20): on `211345` the cure was **effectively
  INERT for five of eight events**, admitted because **nothing was measured**, not because they were
  measured to draw. ⛔ **The designed SAFE direction, NOT a defect** (`NOT_MEASURED` ⇒ ADMIT; nothing
  wrongly deleted). ⛔ **NO MECHANISM CLAIMED (`G120`)** — one of the five IS established (`SM_Ramp2`,
  the known-Nanite control, `G134`); **the other four were NOT chased.** ⚠ **THE TRANSFERABLE HALF:
  THE BENCH LEGS STRUCTURALLY COULD NOT HAVE SHOWN THIS** — unattended, settled camera,
  `notMeasured=0`. **`G135`'s shape again, and an OWNER-PLAYED RUN IS A DIFFERENT INSTRUMENT FROM A
  BENCH LEG.** 📌 **CONSEQUENCE, UNDECIDED: how much of `H5` the cure catches in a CLIENT capture is
  VARIABLE and can be small, and any statement to a client must carry that. Not queued, no number
  minted.**
  🆕 **`G142` — A VERIFICATION SCRIPT IS A DEFECT SURFACE OF ITS OWN. TWO defects in MY OWN checker,
  found while reporting a PASS, EITHER of which would have manufactured a false `COUNTS DISAGREE —
  STOP` on a gate that was working:** (1) it assumed **one capture run per log** and the owner captured
  twice; (2) anchoring on `Capture run STARTED` is wrong **at both ends in opposite directions** — the
  per-run **echo prints BEFORE** it, the per-event `M26S1 EVENT` lines print **AFTER the `FINISHED`
  banner**. ⚠ **A FALSE failure is expensive here: it teaches the owner to distrust a gate that was
  correct.** Recorded as **VERIFICATION-TOOLING defects, explicitly NOT build defects.**
  📌 **`G92` NOW CARRIES A RUNNING COUNT: UNBANKED EVIDENCE HAS BEEN FOUND IN THE PROJECT TREE FIVE
  TIMES, TWO OF THEM ON 2026-08-20.** Every one was found **by SESSION ID, never by directory name**,
  and the most valuable item is repeatedly an **owner-played run** — the harness banks its own legs and
  creates exactly that blind spot. 🚨 **NEW SUB-LESSON: THE LOG CAN BE THE ONLY COPY OF A RESULT** — a
  vetoed event leaves **no trace in `annotation.json`**, so the `VETOED-OBJECT` lines naming which three
  objects `211024` deleted existed **nowhere else**. Both sessions **and the log** are banked at
  `_bench_sessions_bank\M27_OWNER_PLAYGATE_SMOKE\`, manifest-verified 95/95 each.
  ⚖ **RULING 1 — FOUR COMMITS, NOT SQUASHED, AND THE REASON IS RECORDED SO IT IS NOT READ AS DRIFT:**
  the brief's *"one commit"* **predates** foliage and the client-readme rewrite entering scope, and
  **squashing would hide which change carries which gate — the convention exists FOR traceability.**
  Five plugin commits: `0d5e458` · `409b67a` · `9f86600` · `3b91fe4` · `4a92962`.
  ⚖ **RULING 2 — ⛔ FILED, DELIBERATELY NOT FIXED: `IAI.Capture.Mask`'s console help still opens
  *"m26 SLICES 1+2 - MEASURE AND REPORT (default OFF)"*, so DELIVERABLE A3 IS *PARTIAL*.** It is stale
  against **its own body** (which describes slice 3) and against the ini key (`default OFF` is now only
  the COMPILED default) — **this gotcha's own failure mode surviving inside the fix for it**, and a
  reader meets an **internal inconsistency** rather than a plain error. **Filed at `G139`'s addendum.**
  **NOT fixed because a re-cook MOVES THE EXE AND INVALIDATES ALL FOUR GATE 3 LEGS for a string no gate
  ever read** — the same trade already made for `G118` and the `m26` bench binary. 🧭 **CLEARING RULE:
  FOLD INTO THE NEXT MILESTONE THAT ALREADY REQUIRES A COOK, with `G118`'s placeholder token.**
  📌 **THE TAG CARRIES SEVEN LIMITS** — reach is view-dependent · `m26`'s zero-only partial cure is
  unchanged (**the over-claim case still ships as a valid label; NO RATIO, NO THRESHOLD, and none must
  ever be proposed**) · **`translucent_vetoes` counts a PROPERTY OF THE TARGET, NOT A CAUSE** (the m27
  gate run holds the counter-example) · **delivered datasets contain NO FOLIAGE ANOMALIES AT ALL** ·
  A3 partial · **`L3` live and deliberately unfixed** · **a vetoed event leaves NO TRACE in
  `annotation.json`, so the log lines and `vetoed_events` are the only record.**
  🧭 **NOTHING IS OUTSTANDING. `m26` tag NOT moved · `feature/stencil-capture` UNTOUCHED at `76cac74` ·
  no force-push · `P6` DID NOT MOVE · `H6` REMAINS DOCUMENTED, NOT FIXED · `L3` stays unfixed.**
- 🟦 *(superseded as "you are here" by the `m27` entry above)* **2026-08-20 (later). 🚨 `m26`'s SAFETY ARGUMENT IS CORRECTED BY MEASUREMENT.
  `I11-A` SUPPORTS `H6` ON TWO INDEPENDENT ROUTES: a target the mask CANNOT SEE can reach
  `MEASURED_ZERO` and be VETOED, because `bPassRan` is a VIEW-LEVEL property used as a PER-TARGET
  precondition.** Five legs, every gate passed, the only change between admitted and deleted was
  **one boolean on an unrelated lamp**.
  📌 **THE CORRECTION IS STATED ONCE, AND NOT RESTATED HERE:
  `docs/invisible-anomaly-mechanisms.md` → "SAFETY-PROPERTY CORRECTION — the admit bias is sound at
  the enum and unsound at the assignment".** `client-delivery.md`'s Nanite KNOWN LIMITATION points
  at the same entry.
  ⛔ **MECHANISM CLAIM ONLY — PIE (`G76`), NO INCIDENCE CLAIM, the lever was CONSTRUCTED.** Whether
  the shipping path supplies its own writer is **OPEN and is `I11-B`**. ⛔ **NO FIX IS AUTHORISED,
  DESIGNED OR PROPOSED — owner ruling: `I11-B` first, because it is the BEFORE picture and a fix
  would destroy it.** ⛔ **The `m26` tag does NOT carry this and is NOT being rewritten.**
  🧭 Pre-declarations live in `docs/predictions/`; `I11-A`'s is `8a42809`, amended twice, both
  amendments BEFORE any measurement existed, zero deletions.
  📌 **`L3` IS NOW LOAD-BEARING, NOT A PREDICTION: `I11-A`'s vetoed legs keep their view evidence
  ONLY in `labels.jsonl`. Nobody "fixes" `L3` until this line of work closes.**
  🎯 **`I11-B` STAGE 1 CLOSED IT AT BRANCH `Y-1`: THE SHIPPING PATH SUPPLIES ITS OWN WRITER.** In the
  owner's play-gate smoke auto-pool run, with nothing constructed, the extent was view-sized on
  **26 armed frames from the plugin's OWN accumulated tags**, and **both `MEASURED_ZERO` targets
  were wholly Nanite AND on screen** (`BP_Stomper_C`, `RoomBuilderSquare_C` — every component
  Nanite by direct property read). ⛔ **`H6` does not need an external lever.** ⛔ **The smoke's four
  vetoes are still NOT attributed to `H6` — present and active is not the same as caused (`G120`).**
  ⚠ **CORRECTED: the "NO `m27`" half of this decision was REVERSED by the owner the same day —
  `m27` exists, is built and is gated. The `H6` half is UNCHANGED: `H6` remains DOCUMENTED, NOT
  FIXED, and `m27` is not an `H6` fix.**
  ⚖ **OWNER DECISION 2026-08-20: `H6` IS DOCUMENTED, NOT FIXED. NO `m27`. NO FIX. NO VETO-DEFAULT
  CHANGE.** The near-term ship target (Concorde) has **SUPPORT NANITE DISABLED**, which makes
  Nanite-flagged meshes render through the conventional path and therefore MEASURABLE — verified
  from 5.1 source, **and conditional on `r.Nanite.ProxyRenderMode` also being at its default `0`**.
  📌 **STATED ONCE, NOT RESTATED HERE: `docs/invisible-anomaly-mechanisms.md` → "`H6` — DOCUMENTED,
  NOT FIXED".** Two new boxes in `PRE-DELIVERY-CHECKLIST.md` §1 are what make the condition survive
  a session that has forgotten it.
  📌 **`I11-B` is CLOSED AT STAGE 1. Stage 2 (packaged) is UNRUN, NOT CANCELLED — its
  pre-declaration stands as written if it is ever needed.**
- 🟦 *(superseded as "you are here" by the entry above — `m26` is still the shipped, tagged state)*
  **2026-08-20. 🎯 `m26` IS SHIPPED AND TAGGED. THE MILESTONE IS CLOSED.**
  **`m26` = the `H5` class-(ii) cure: an event whose target is MEASURED to draw ZERO pixels is
  removed from `annotation.json` before it is written. Slices 1, 2 and 3 all gated; `F-6` complete;
  the OWNER PLAY-GATE SMOKE PASSED in real gameplay, including the AUTO-POOL path no bench leg ever
  exercised.** ⛔ **Do not start anything new unprompted — the next milestone is the owner's call.**
  🧭 **COLD START: read `docs/invisible-anomaly-mechanisms.md` (the ledger), then go STRAIGHT to the
  `HANDOFF` section at the END of `docs/sessions/2026-08-19-045-h4-cook-and-h5-mainworld-arc.md`
  (PART THIRTY-TWO). THAT HANDOFF IS SELF-CONTAINED.**
  🎯 **THE SMOKE, THREE TESTS IN REAL CONTENT ON `MainWorld`:** **A** targeted `BP_SplineSpawn_C` —
  `vetoed_events 8`, `anomalies []`, 90 PNGs intact · **B** targeted `InstancedFoliageActor_0_0_0` —
  **0 vetoed, 8 kept: THE RULE'S OWN GUARD HELD** · **C AUTO-POOL** (the new coverage — the selector
  picking its own targets with the veto live) — 8 events, 4 kept / 4 vetoed, **ALL FOUR
  `state=MEASURED_ZERO`**, `countedEventsBefore=8 After=4 vetoed=4`, **`notMeasured=0`**.
  ⚠ **Three of the four vetoed events show `framesNoPass=1 collisions=1` beside
  `framesContributed=3` — the frame-scoped discard working as designed: one polluted frame dropped,
  three clean frames contributing, MAX = 0 ⇒ MEASURED_ZERO. These are measured zeros, not
  unexamined ones.**
  🚨 **THE VETO RULE (ruled; NEVER re-open by proposing a ratio): veto IFF manifested AND
  `MEASURED_ZERO`. `NOT_MEASURED` never vetoed; a measured NON-ZERO count never vetoed however small
  a fraction of its claimed extent. NO RATIO, NO THRESHOLD — the owner's reasoning is verbatim in
  the tag scope statement and at journal §209.**
  🚨 **THE ACCEPTED COST, QUOTE IT RATHER THAN SOFTEN IT: `m26` is a PARTIAL cure for `H5`. It
  removes the zero-contribution case and LEAVES THE OVER-CLAIM CASE** — the foliage (5,689–13,342 px
  against a 921,600 px claim) ships as a valid label. **`m26` does NOT close `P1`, does NOT cure
  `H4`, and `H5` class (i) is still ENUMERATED-NOT-OBSERVED.**
  🆕 **FINDING 1 — `L3` IS LIVE AND SHARPER THAN RECORDED:** *"in a single delivery-OFF session
  folder, `annotation.json` and `labels.jsonl` NOW DISAGREE ON EVENT CONTENT"* — a fully-vetoed
  session ships `anomalies: []` beside **59 label rows asserting `anomaly_present` and
  `visible_positive`**. ✅ **NO CLIENT IMPACT** (delivery mode never writes `labels.jsonl`) ⚠ **but
  OWNER-SIDE OVERLAY TOOLING WILL DRAW BOXES FOR VETOED EVENTS: `tools/verify_capture.py`, invoked
  by `overlay_watcher.py` — which exists in THREE copies (`host-tools\`,
  `anomaly-dashboard\host-tools\`, `_M2Smoke\host-tools\`). The Dashboard app itself is NOT
  affected.** ⛔ **DELIBERATELY NOT FIXED** — a separate change with its own gates.
  🆕 **FINDING 2 — a prediction of MINE that did not fire, recorded as an OBSERVATION (n=1):** I
  predicted many Nanite `NOT_MEASURED` events on MainWorld; Test C produced **ZERO** and
  `mask_nopass_discards=3`. The auto-pool picked foliage and simple meshes. ⛔ **Does NOT contradict
  `G134`** (measurability ≠ what the selector picks) — **it touches the unstated assumption that the
  two coincide. If the auto-pool systematically avoids Nanite structural geometry, the cure's reach
  may be WIDER than `G134` implies. A LEAD, NOT A LANE — do not investigate unprompted.**
  🆕 **FINDING 3 — THE MEASUREMENT IS VIEW-DEPENDENT:** `BP_SplineSpawn_C` measured **54,779 px
  (KEPT)** in Test C and **ZERO 8/8 (VETOED)** on the bench. Correct, not contradictory — the mask
  reports drawn pixels IN THAT VIEW. 🚨 **"`H5`-shaped target" is a property of a TARGET IN A VIEW,
  not of a target.**
  ⚠ **BUILD IDENTITY (`G121`), STATED: the staged bench exe `5EA6AB92` is the tagged source MINUS
  one commit** — `49d1c7a`, a log-wording fix with **no behavioural effect** (`SetMaskMeasure` still
  announced "slice 1 MEASURE ONLY"; the owner hit it on step 2 of the smoke). ⛔ **The binary was
  deliberately NOT rebuilt — `5EA6AB92` is the exact binary all nine slice-3 gate legs ran on, and
  swapping it for an ungated one to fix a log string is the worse trade.**
  📌 **OPEN LEADS, none a lane until ruled:** the over-claim case · Finding 2 · `labels.jsonl` (`L3`)
  · `H4` · `H5` class (i) · `P1`.
- 🟦 *(superseded — pre-tag: slice 3 shipped and gated, awaiting the owner smoke)*
  ✅ **SLICE 3 SHIPPED (`65deadc`) AND GATED. THE RULE, AS RULED: VETO IF AND ONLY IF the event
  manifested AND its target's state is `MEASURED_ZERO`.** ⛔ **`NOT_MEASURED` is NEVER vetoed
  (never measured ⇒ MUST ADMIT) · a measured NON-ZERO count is NEVER vetoed however small a
  fraction of its claimed extent · `manifested == false` never vetoed (`A-1` precedence).**
  🚨 **NO RATIO. NO THRESHOLD. NO COMPARISON AGAINST CLAIMED AREA — and none must ever be
  proposed.** The implementation tests the ENUM STATE only (`MaskStateVetoes`); `MaxCount` is never
  read and no constant exists that could become a threshold. **The owner's reasoning for refusing a
  ratio is recorded VERBATIM at journal §209 — in short: every GOOD target we have measured is a
  convex primitive viewed head-on, a complex-silhouette legitimate target (fence, railing, grate,
  sparse foliage) would draw a small fraction of its rect while being perfectly valid, and NO SUCH
  TARGET EXISTS IN OUR MEASURED SET. Calibrating on four convex points is `G135`'s exact failure.
  A count of ZERO needs no calibration.**
  🎯 **GATE RESULTS — every prediction met, no failure branch fired** (pre-declared at bench
  `043b110` before implementation): **`BP_SplineSpawn_C` 8/8 VETOED, `annotation.json` `anomalies:
  []`, `vetoed_events=8`** · 🚨 **the FOLIAGE at ~1.4 % of its claim NOT vetoed — the rule's own
  guard; a veto there would have meant a ratio crept in** · 🚨 **`SM_Ramp2`'s 8 `NOT_MEASURED`
  events ALL KEPT — the data-destroying direction, closed** · Cube and Cylinder controls untouched
  · **`G-9` re-run at slice 3: `EXTRAS = 0`, and the invariant core now includes `vetoed_events`
  AND the EVENT SET itself, identical across delivery modes** · `G-8` loud in the ARTIFACT
  (`provided:false` + `mask_nopass_discards=30`) · `G-11` before/after logged, `before − vetoed ==
  after`, counters disjoint · **`P6` 48/48 on every leg; `run_summary` +4 exactly** · **`F-7`
  inert: mask OFF ⇒ zero vetoes.**
  ⚠ **PRE-DECLARED AND CONFIRMED, not discovered: the spline leg vetoes ALL its events, so its
  `annotation.json` has ZERO anomalies.** That is the veto WORKING and simultaneously below
  `G-11`'s 3-event floor — reconciled by the leg's ROLE, fixed in advance: **it is a DEMONSTRATION
  leg, not a certifying one.** `F-8` checked: the empty artifact parses, `video.total_frames` is
  still 90, and **all 90 PNGs remain on disk (`L1` demonstrated, not asserted)**.
  🚨 **THE ACCEPTED COST, IN THE TAG STATEMENT AND `client-delivery.md`, NOT BURIED:** *"`m26`
  vetoes only targets measured at ZERO drawn pixels. A target that OVER-CLAIMS — measured non-zero
  but far below its claimed extent, such as the `InstancedFoliageActor` measured at 5,689–13,342 px
  against a claimed 921,600 px (the entire frame) — IS NOT VETOED and ships as a valid label. `m26`
  is a PARTIAL cure for `H5`: it removes the zero-contribution case and leaves the over-claim case.
  The over-claim rule requires a calibration campaign including complex-silhouette legitimate
  targets, which do not exist in the current measured set."*
  ⚠ **`A35`, AS A RULING WITH ITS REASON:** `BP_SplineSpawn_C`'s banked hide showed a small in-bbox
  luma change (`0.0175`) while the mask reads exactly zero — a zero-silhouette target can still
  have indirect visual effect (shadow, GI). **`m26` vetoes it anyway, because the label points at
  the OBJECT and not at its shadow.**
  📌 **`L1`–`L3` stated in the docs:** frames are on disk and NOT un-written · **a post-`m26` event
  count is NOT comparable with a pre-`m26` one; `vetoed_events` carries the delta** · `labels.jsonl`
  (delivery OFF) is prebuilt and uncorrectable, so **delivery OFF and ON WILL DISAGREE on event
  content**.
  📦 staged exe **`5EA6AB92`** (`F4EBEAD7` archived first), container unchanged, A44 green. Banked:
  `P31_S3_{SPLINE,CTRL49,CYL73,FOLIAGE,RAMP,G9_OFF_A,G9_OFF_B,G9_ON,INERT}`.
  ⛔ **NEXT: THE OWNER'S PLAY-GATE SMOKE. DO NOT TAG UNTIL IT PASSES, and do not start anything new
  unprompted.** `P6` does not move · CB_GateLevel untouched (`G99`) · stencil range 200/255 ·
  `feature/stencil-capture` READ-ONLY.
- 🟦 *(superseded — the eighth 2026-08-20 session: slice 2 and the `G-9` closure)* **`G-9` closed
  at `EXTRAS = 0`; `framesNoPass` is not a Nanite counter.**
  ✅ **SLICE 2 SHIPPED (`ece343f`) AND GATED.** `annotation.json`'s already-shipping
  `mask{provided}` stops being a hardcoded `false` and carries the tri-state's bool:
  **`NOT_MEASURED` → `false` (never measured, MUST ADMIT) · `MEASURED_ZERO` → `true` ·
  `MEASURED_NONZERO` → `true`.**
  🚨 **THE GUARANTEE, NAMED: `provided` is produced by ONE function
  (`MaskStateProvidesMeasurement`) switching on `State` ALONE. `MaxCount` is never consulted and
  never emitted, so no code path can collapse `MEASURED_ZERO` into `NOT_MEASURED` or the reverse.**
  🎯 **ALL FIVE KNOWN-ANSWER ROWS CORRECT, tested in BOTH directions on banked answers:** Cube
  `true`×8 · **Cylinder `true`×4 AND `false`×4 in ONE leg** · foliage `true`×8 ·
  🚨 **`BP_SplineSpawn_C` (`MEASURED_ZERO`) → `true`×8 (`F-2` did not fire)** ·
  🚨 **`SM_Ramp2` (`NOT_MEASURED`, the known-Nanite control) → `false`×8 (`F-1` did not fire).**
  **0 mapping mismatches; `mask` sub-keys exactly `['provided']`; `depth` untouched.**
  ✅ **GATES: G-2 field SET 48/48 in BOTH delivery modes (7 legs) · P6 unchanged, `run_summary` +3
  and nothing more · INERT — mask OFF ⇒ `provided:false` everywhere, 0 MAP lines · F-1…F-7 none
  fired.**
  🚨 **`A64` JUSTIFIED ITSELF AND STOPPED A FALSE FINDING.** The first delivery leg read
  `provided:false` on every event with `framesNoPass=4` — which reads as *"delivery mode breaks the
  mask"*, a G-9 failure. **It was a BIFURCATED POSE** (`coverage_ratio` 0.051/0 vs the OFF legs'
  0.078). Refuted two ways: a **POSE-MATCHED delivery leg reads `true` on every event**, and the
  **`Cylinder` leg reproduces the identical signature with delivery OFF** — its `coverage_ratio`
  falls to 0 and `framesNoPass` rises to 4 in lockstep as the target leaves the frustum.
  🆕 **CONSEQUENCE WORTH CARRYING: `framesNoPass` means "not in the view's relevant set", NOT
  "Nanite" — FRUSTUM CULLING reaches it too, and the admit bias handles it correctly (off-screen ⇒
  NOT_MEASURED ⇒ `provided:false` ⇒ ADMIT).**
  ✅ 🎯 **`G-9` IS CLOSED (PART THIRTY): `EXTRAS = 0`.** P29's path-level result was INCONCLUSIVE
  (6 pose-derived residuals — my control pair had under-sampled pitch). **Closed by ROUTE (a): make
  the confound ABSENT rather than EXCUSED.** All three legs settled at the SAME pose (rotation
  `0/0/0`, `coverage_ratio 0.077977`) on the first attempt each, so 🚨 **the run-unique set SHRANK
  from 26 members to 4** — `/session_id`, `/speed_ratio`, `/sustained_wall_fps`, `/video/path` —
  **containing NO pose field at all, which makes the subset test STRICTER than the one that was
  inconclusive.** The delivery-ON vs delivery-OFF difference set is **exactly those four plus
  `/delivery_mode`**. Invariant core re-asserted identical (mask.provided ×8, depth, all three
  `m26` fields, event count, full key set). ⛔ **Route (b) — widening the run-unique set — was
  REFUSED and the reason recorded: it means re-running legs until the baseline widens enough to
  excuse the difference being cleared, which is the laundering shape even when every step is
  legitimate.**
  ✅ **`framesNoPass` HAS ITS DEFINITION FIXED WHERE A READER HITS IT** (`7ea8ce9`): *"counts frames
  where the custom-depth pass did not produce for this target. Causes include Nanite geometry
  (`G134`), frustum culling, and any other route by which the target is absent from the view's
  relevant set. **It is NOT a Nanite counter.** In all cases the frame is discarded and the event
  tends toward `NOT_MEASURED`, which ADMITS."* — corrected in the NO-PASS log line, the
  NOT_MEASURED warning, `G134`, and `client-delivery.md`.
  ✅ **THE CLIENT-FACING SENTENCE IS IN THE TAG SCOPE STATEMENT, VERBATIM:** *"`mask.provided`
  `false` NEVER means 'the target drew nothing' — it means no measurement exists, and such an event
  carries exactly as much evidence as it did before `m26`: none from this measurement."*
  📊 **REPORTED FOR THE SLICE-3 DECISION, NUMBERS ONLY, NO RULE PROPOSED** (journal §207 — measured
  px vs claimed px, viewport 921,600): `StaticMeshActor_49` **66,843–66,878 vs 71,864** ·
  `StaticMeshActor_73` **48,590–48,597 vs 63,296** · foliage **5,689 then 12,514–13,342 vs
  921,600** · `BP_SplineSpawn_C` **0 vs 35,535 (×2) then 210,921–210,942 (×6)**.
  📦 staged exe **`F4EBEAD7`** (`047FA489` archived first), container unchanged, A44 green. Banked:
  `P29_S2_*` (nine legs) and `P30_G9_{OFF_A,OFF_B,ON_1}`.
  ⛔ **SLICE 3 NOT STARTED. Its veto rule is NOT designed, NOT proposed, and NOT assumed anywhere —
  and NO THRESHOLD EXISTS in the code or the docs.** The §195.3 ratio table is a distribution
  sketch on four targets from one title, not a calibration. **Do not start slice 3 unprompted.**
  📌 **§198 is in the journal for cold readers: eleven parts ago this instrument returned a constant
  255. The path to a clean result ran through two source-refuted repairs, a fault in our own
  collection code, a stale read, a structurally blind detector, and a calibration level that could
  not have shown the blindness — every one caught before it shipped.**
- 🟦 *(superseded — the sixth 2026-08-20 session: `F-6` completed and the `H5` legs run)*
  **The cure identifies both `H5` instances; slice 2 was the owner's next call.**
  🎯 **THE `H5` LEGS — the measurement the milestone was blocked on. Branches pre-declared at bench
  `2c2e60a` BEFORE either leg ran:**
  · **`BP_SplineSpawn_C` (`SM_GenericPlane`): label claims `coverage_pct` 22.89 % — mask measures
  `MEASURED_ZERO`, ZERO pixels, on ALL 8 EVENTS. Branch `Z1`, the aimed-at result.**
  · **`InstancedFoliageActor_0_0_0` (`SM_Bush`): label claims `coverage_pct` 100 % with
  `bbox_px (0,0,1280,720)` — THE ENTIRE FRAME — mask measures 5,689–13,342 px = 0.62–1.45 %, i.e.
  it draws ≈1.4 % of what it claims. Branch `Z2`.**
  🚨 **BOTH ZEROS ARRIVED INTERPRETABLE — that is what makes them evidence:** `framesContributed =
  arms` on every event, and `framesDiscarded / framesResidual / framesUnconfirmed / framesNoPass /
  probeArms / collisions / tagFailed` **ALL ZERO**, with **29/29 armed frames view-sized (zero
  dummies)** on every leg. **Pre-declared branch `X` — "any zero with any bucket non-zero is NOT
  YET INTERPRETABLE" — did NOT fire.** The target was tagged, the tag verified held, the pass
  produced, and visibility confirmed at both brackets — and the mask still found nothing.
  ✅ **`F-6` IS COMPLETE, ALL FIVE ITEMS.** Item 2 satisfied by the replacement `N-2` control
  **`StaticMeshActor_73`** (`Cylinder`, non-Nanite): **8/8 `MEASURED_NONZERO` at 5.27 % of frame
  against its own claimed 6.87 %**, all buckets clean, accepted attempt 1. **`SM_Ramp2` is retired
  from `N-2` and now serves as the KNOWN-NANITE control** (must read `NOT_MEASURED` every time — a
  positive test for `G134` and the first place a future engine bump would show).
  📊 **The four targets, reported and DELIBERATELY NOT THRESHOLDED** (drawn ÷ claimed):
  `StaticMeshActor_49` **0.93** · `StaticMeshActor_73` **0.77** · foliage **0.014** ·
  `BP_SplineSpawn_C` **0.000**. ⛔ **NO threshold is proposed or implied — four targets on one title
  is a distribution sketch, not a calibration; slice 3's veto rule is a separate decision.**
  ⛔ **WHAT THESE LEGS DO NOT ESTABLISH, declared in advance and restated after: NO INCIDENCE
  CLAIM · `H5` class (i) still ENUMERATED-NOT-OBSERVED · NOT a veto test (slice 3 does not exist;
  nothing was removed from any artifact — these legs MEASURE, they do not ACT) · NOT a Nanite
  result (both targets are non-Nanite).**
  ⚠ **ONE NUANCE RECORDED, NOT SMOOTHED:** the spline's banked hide showed a small non-zero luma
  change (**0.0175**) while the mask reads exactly 0 — **consistent with plan risk 7 / A35: the
  mask measures DRAWN SILHOUETTE, not VISUAL EFFECT.** The already-recorded limit showing up where
  the plan said it would; not re-opened.
  ✅ **RULINGS RECORDED: (1) the third field APPROVED — `run_summary` is `+3`
  (`mask_probe_arms`, `mask_residual_discards`, `mask_nopass_discards`), and the `m26` scope
  statement now SAYS `+3`, not `+2`. (2) The Nanite entanglement is in `client-delivery.md` as its
  own `⛔ KNOWN LIMITATION` headline section, in the ruling's own terms, with no percentage.
  (3) 🆕 `G135` — a calibration environment built from a RESTRICTED ASSET SET cannot exhibit defect
  classes outside that set, and the blindness presents as a CLEAN PASS; the tension is STATED, not
  resolved, and CB_GateLevel is NOT changed (`G99`).**
  📦 staged exe **`F93AEF71`**, container unchanged, A44 green. Banked: `P28_N2_CYL73`,
  `P28_H5_FOLIAGE`, `P28_H5_SPLINE` — **all three accepted on attempt 1**; `B1` NOT APPLICABLE on
  all three, declared in advance (`G117`). **`P6` measured unchanged on all three: 48/48.**
  ⛔ **NEXT IS THE OWNER'S: whether to start slice 2 (reporting) and slice 3 (the veto). DO NOT
  START EITHER UNPROMPTED.** `P6` does not move · CB_GateLevel untouched · stencil range 200/255 ·
  `feature/stencil-capture` READ-ONLY · depth work parked · **NO TAG.**
- 🟦 *(superseded — the fifth 2026-08-20 session: T-1 answered, the extent precondition shipped)*
  **Both `H5` targets confirmed NON-NANITE; `G133` closed.**
  🎯 **`T-1` ANSWERED FIRST, AS RULED — IT OUTRANKED THE THREE RULINGS.** Asset-side, read-only,
  pre-declared at bench `3fdea29` before the first read: **`InstancedFoliageActor_0_0_0` →
  `SM_Bush` = NON-NANITE · `BP_SplineSpawn_C` → `SM_GenericPlane` = NON-NANITE** ⇒ **branch one:
  the cure applies to the instances that motivated it.** **The discriminator closes too:
  `StaticMeshActor_49` → `/Engine/BasicShapes/Cube` = NON-NANITE, and `make_gate_level.py:54-58`
  builds the WHOLE calibration level from `/Engine/BasicShapes/` — which is exactly why the
  instrument was green there all milestone, and why the blindness was invisible.** `SM_Ramp` =
  NANITE, confirming PART TWENTY-SIX. **The signature predicts measurability 5 for 5.**
  ⚠ **THE HONEST OTHER HALF, MEASURED NOT PROJECTED: StackOBot's authored structural geometry
  (walls, floors, platforms, pillars, roofs, pipes, fences, crates, doors, ramps) is overwhelmingly
  NANITE — 46 assets carry the signature — while foliage and simple planes are not. `G134`'s
  "common case, not corner case" is measured ON THIS TITLE. The two `H5` instances are reachable
  because of what they are made of, not because `H5` favours non-Nanite geometry.**
  📌 **Method: `CaptureBench/tools/nanite_signature_scan.py` — EVIDENCE, not a measurement; it
  prints its own two weaknesses and is load-bearing only as a DIFFERENTIAL. The editor bridge was
  refused, so `A59` corroboration was ABANDONED rather than worked around.**
  ✅ **`RULING 1` BUILT AND GATED (`3beb3ba`): `customStencilExtent` is now a CONTRIBUTION
  PRECONDITION** — a frame contributes only on POSITIVE evidence the pass ran; new disjoint bucket
  `framesNoPass`; **the 255 detector is DEMOTED to a SECONDARY signal.** 🚨 **The decisive gate
  passes: `SM_Ramp2` went `MEASURED_ZERO` (event 1) → `NOT_MEASURED` on ALL 8 EVENTS,
  `framesNoPass=29`, `mask_nopass_discards=29` — the false accusation is gone, and the recorded
  reason is now the true one.** **L1–L4 unchanged** (L2/L3 byte-identical maxCounts; L4's probe
  still fires all three detectors, `mask_probe_arms=1`).
  ⚠ **`F-T2-A` FIRED ON A LITERAL READING AND WAS INVESTIGATED, NOT WAVED AWAY: L1's maxCount moved
  — cause is POSE, not the precondition.** Counts group by SETTLE POSE, not by build (P24 exact
  pose 66832-66862 · P26 4.5 px-narrower bbox 66321-66516 · P27 exact pose 66843-66878); **arm tick
  ids byte-identical P26↔P27**; dispositions identical; and structurally the precondition is a
  `continue` BEFORE contribution, so it can only remove a frame, never alter a contributing
  frame's count. **L2 confirms from the other side: a different pose with a MATCHING bbox AREA
  gives byte-identical counts.** 📌 **Criterion corrected in the record: it should read "any
  maxCount that moves AT A MATCHED POSE" — a refuter that fires on a characterised confound
  (`A47`/`B1`) is badly drawn, and the fix is to say so, not to grant an exception.**
  ✅ **`RULING 2`: `SM_Ramp2` RETIRED as `N-2` and REPURPOSED as the KNOWN-NANITE CONTROL** — it
  must read `NOT_MEASURED` every time (a POSITIVE test for the limit; first pass banked, and where
  a future engine bump adding Nanite custom-depth would show first). 🚨 **NO NON-NANITE A35-SHAPED
  CONTROL EXISTS — said so, per the ruling's own fallback:** the peak-IN/OUT split is banked for
  `SM_Ramp2` ALONE, and **every other measured legitimate target is Nanite**, so the A35 risk and
  the Nanite limit are entangled on this content. ⇒ **`N-2` → a plain non-Nanite drawing target
  (recommended `StaticMeshActor_73`, `Cylinder`); A35 GOES INTO THE TAG AS *UNTESTED*.** ⚠ Two
  stated weaknesses: it shares the gate level with the item-1 control (little independence), and a
  stronger MainWorld candidate's existence is UNESTABLISHED (needs a census leg, `G122`). **No
  asset was modified to manufacture a control.**
  ✅ **`RULING 3`: the Nanite scope statement is DRAFTED AND UNSOFTENED** (journal §191.1) — a
  Nanite target is ALWAYS ADMITTED, never vetoed, which is safe **and is also the cure not working
  there**; `run_summary.mask_nopass_discards` is how a delivered session shows it.
  ✅ **THE DEPTH QUESTION, ANSWERED FROM SOURCE, ANSWER ONLY: SCENE DEPTH *IS* NANITE-INCLUSIVE ON
  5.1.** `Nanite::EmitDepthTargets` binds `FDepthStencilBinding(SceneDepth, ELoad,
  DepthWrite_StencilWrite)` (`NaniteMaterials.cpp:896` and `:930`; compute path `FDepthExportCS`
  `:856`; HTile resummarize `:1024`). ⇒ **a path exists where `C-1` has none.** ⛔ **Nothing
  designed, nothing costed, the depth work stays parked** — and PART TWELVE §6.3's finding stands:
  `C-2`'s reference depth comes from the same bounds `H5` calls untrustworthy.
  ⚠ **AWAITING VETO OR BLESSING: `run_summary.mask_nopass_discards` is a THIRD artifact field
  beyond the declared +2** — added deliberately so the known-Nanite control is auditable from a
  delivered session rather than only a log. Measured: **`annotation.json` 48/48, `P6` NOT MOVED;
  `run_summary` +3.**
  📦 staged exe **`F93AEF71`** (built==staged verified; `DBA2D8EC` and `444D4812` both archived
  first in `_binary_baselines\`) · container UNCHANGED · A44 green. Legs banked:
  `P27_EXT_{RAMP,CTRL49,MTEX49,MOBJ49,PROBE49}` + discarded attempts (A63).
  ⛔ **NEXT, AND IT IS ONE LEG: the replacement `N-2` control (pre-declare its prediction first),
  THEN the `H5` legs — both now confirmed measurable.** Slices 2/3 NOT STARTED · `P6` DOES NOT
  MOVE · stencil range 200/255 · `feature/stencil-capture` READ-ONLY · **NO TAG.**
- 🟦 *(superseded — the fourth 2026-08-20 session: the fix gated, `F-6` halted at item 2)*
  **`G133`/`G134` exposed by the `SM_Ramp2` control.**
  ✅ **THE FAULT-(ii) FIX SHIPPED (`4a9631a`, the Part-25 design + amendments A-1/A-2) AND PASSED
  EVERY PRE-DECLARED PREDICTION ON FOUR GATE LEGS** (bench pre-declaration `84106bd` BEFORE the
  build existed): mask block armed from `OnWorldTickEnd`; render BRACKETED by the enforcing
  whitelist confirmation; disjoint buckets (`framesDiscarded`/`framesResidual`/`framesUnconfirmed`/
  `probeArms`); the `bRunning` guard retired §172's stray arm (zero mode≠3 records on all five
  legs). **L1 blinking: 4/4 contributed per event, 0 dummies, 7.20–7.22 % · L2 missing_texture:
  4/4, byte-matching P24 · L3 missing_object: 0 in-window arms + 4 post-revert, capped final event
  = `NOT_MEASURED` MUST-ADMIT (the admit path live again) · L4 probe: ONE known-hidden arm fired
  the 255 detector + confirmation + discard on the shipped binary, `run_summary.mask_probe_arms=1`
  — `F-6` ITEMS 1/3/4/5 PASS, `G96` both ways.** `mask_residual_discards=0` everywhere (A-2's
  bench expectation). `P6` measured unchanged (48/48); `run_summary` +2 exactly as declared
  (`mask_probe_arms`, `mask_residual_discards`).
  🛑 **`F-6` ITEM 2 FAILED — `P26_FIX2_RAMP`: 29/29 armed frames DUMMY while the target was
  un-hidden, tagged-and-verified, in-frustum, AND DRAWING (`CM_CM_RAMP` at the IDENTICAL camera:
  in-bbox change 0.1785). TWO LIMITS EXPOSED, BOTH RECORDED:**
  🆕 **`G134` — THE INSTRUMENT IS STRUCTURALLY BLIND TO NANITE ON UE 5.1. ESTABLISHED:** `SM_Ramp`
  is Nanite-enabled (asset signature); `Nanite::FSceneProxy::GetViewRelevance` NEVER sets
  `bRenderCustomDepth` (`NaniteResources.cpp:941-1010`); `bHasCustomDepthPrimitives` rises only
  from that flag (`SceneVisibility.cpp:2470`); the 5.1 custom-depth pass has NO Nanite path.
  **A Nanite target is selectable, taggable, verifiable — and permanently unmeasurable by C-1 on
  this engine. On a Nanite-heavy host title that is the COMMON CASE.** Ledger updated.
  🆕 **`G133` — THE 255 DETECTOR IS A SINGLE-PIXEL, VIEW-CONTINGENT SIGNAL. MEASURED:** the dummy
  is 1×1; out-of-bounds loads return 0; the depth gate must also pass at texel (0,0) — **every
  fire this milestone was `unassignedCount=1`.** Ramp event 1 had a non-far (0,0): no fire, and
  four zeros from a NEVER-RUN pass CONTRIBUTED ⇒ **`MEASURED_ZERO` on the A35 control — Ruling
  1's hazard realised on our own bench.** The pass-ran discriminator (`customStencilExtent` 1×1
  vs view-sized) exists per frame since M-2 and **is NOT yet a contribution precondition — NOT
  fixed this turn (the gate failed; no same-turn fix to the validity instrument).**
  ⛔ **HALTED AT THE FIRST FAILURE, AS RULED. `H5` LEGS NOT RUN — now DOUBLY uninterpretable (no
  extent precondition; the foliage targets' own Nanite status unestablished).** 🧭 **OWED THE
  OWNER: (1) the extent contribution precondition (safe: all-dummy events land `NOT_MEASURED` ⇒
  ADMIT; cost: Nanite targets permanently unmeasured), (2) `m26`'s Nanite scope statement, (3)
  whether `SM_Ramp2` can remain the `N-2` control for an instrument that cannot see it.**
  📦 staged exe **`DBA2D8EC`** (built==staged verified; predecessor `444D4812` archived FIRST at
  `_binary_baselines\StackOBot.exe.m26-p24-fault1-fix-444D4812`) · container UNCHANGED
  (`utoc 9334496D · ucas 62EB0072 · pak 78C977A5`, 4 maps) · A44 all seven new strings. Legs
  banked: `P26_FIX2_{CTRL49,MTEX49,MOBJ49,PROBE49,RAMP}` + every discarded attempt (A63; two
  full 3-discard pose cycles re-run per the pre-declared F-H branch).
  ⛔ **STILL TRUE: slices 2/3 NOT STARTED · `P6` DOES NOT MOVE · stencil range 200/255 ·
  `feature/stencil-capture` READ-ONLY at `76cac74` · NO TAG.**
- 🟦 *(superseded — the third 2026-08-20 session: the design turn; implemented and gated above)*
  **FAULT (ii) FIX DESIGNED UNDER RULING 1 — Option B, the bracket, the probe, the guard.**
  🚨 **`RULING 1` (owner, 2026-08-20, recorded verbatim in journal §175) GOVERNS THE FAULT-(ii)
  FIX: the 255 dummy is a property of THIS BENCH, not of the defect — on a host title a stale arm
  yields a CLEAN `MEASURED_ZERO` with no tell, and under slice 3 that silently deletes a good
  event. THE FIX MUST CLOSE THE STALE READ ITSELF; any fix that relies on detecting the dummy
  works only where we happen to be looking.**
  📐 **THE DESIGN (journal §176–§180, H-1..H-5 answered): Option B — move the mask block
  (verify → drain → collect → arm) from `Tick` to `FWorldDelegates::OnWorldTickEnd`** — post-toggle
  BY POSITION (`LevelTick.cpp:1814` is the last line of `UWorld::Tick`, after every tickable incl.
  the injector), pre-draw SAME frame (`GameEngine.cpp:1891`), request-id semantics unchanged
  (`GFrameCounter` still N there), **zero tick reordering, zero behaviour outside `AnomalyCapture`**.
  Plus: **the M-4 end-frame sampler becomes PRODUCT and ENFORCING, whitelist polarity** — a frame
  contributes ONLY if its `OnEndFrame` sample ran and read visible, so the render is BRACKETED and
  a stale read cannot reach `MEASURED_ZERO` even in the residual cases the anchor cannot cover;
  **the `F-6` item-5 probe** (`IAI.Capture.MaskProbe`, default OFF: ONE deliberate known-hidden arm
  on a gate leg proves the 255 detector + confirmation + frame-scoped discard all live on the new
  binary, and the admit bias disposes of the probe frame); **the `bRunning` guard** retires §172's
  stray post-`FinishRun` arm. **`LOCK-1` preserved and strengthened** — F-2's rule maps to ARM
  (refusal, now post-toggle) / VERIFY (write side, unchanged) / RESOLVE (enforcing confirmation).
  **Budgets: non-hide 4/4 · blinking 4 issued / 4 usable (was 4/2) · `missing_object` 0 in-window
  (correct) + 4 of the 6 post-revert ticks — NO type drops to zero.**
  ⛔ **REJECTED, with reasons banked: Option A** (tick reorder — injector-earlier changes when
  anomalies APPLY = shipping behaviour; capture-later silently changes the `m20`-characterised
  hidden-sample labels; and tickable order is registration order, no stable lever) · **Option C**
  (injector publishes state — fixes blinking, not the class; Ruling 1's exact failure; G127
  boundary crossed backwards) · **Option D** (post-hoc discard as the fix — its premise is itself
  tick-order-fragile and the budget stays wasted; its IDEA survives as the enforcing confirmation).
  ⛔ **NOTHING IMPLEMENTED THIS TURN. If ruled GO: pre-declare the §177 gate-leg predictions as a
  file BEFORE any leg, implement, then the adopted order: `F-6` ALL FIVE ITEMS (item 5 via the
  probe) → `SM_Ramp2` (`A-4` peak-IN/OUT beside it, must be ADMITTED) → only then the `H5` legs.**
  ✅ **FAULT (i) FIXED at `795f2a4` — the discard is FRAME-SCOPED.** A polluted read discards that
  frame only; clean frames feed the MAX. **`MEASURED_ZERO` is reachable ONLY from a clean resolved
  read; an event with no clean frame stays `NOT_MEASURED` (admit)** — `State` initialises to
  `NotMeasured` and `CollectResults` writes it only on the clean path. **Measured on the accepted
  control leg `P24_M26S1F1_CTRL49` (B1 PASSED, modal pose exactly): every full event
  `arms=4 resolved=4 framesDiscarded=2 framesContributed=2 MEASURED_NONZERO 7.2517–7.2550 %` vs
  the banked 7.80 % rect — and the frame-cap-truncated final event (all frames discarded) landed
  `NOT_MEASURED`, the admit path demonstrated live in an artifact.**
  🚨 **FAULT (ii) MECHANISM ESTABLISHED — journal PART TWENTY-FOUR §171, branches pre-declared at
  `CaptureBench/tools/p24_fault2_predeclared.md` (`cb299aa`) with refuters BEFORE measuring. THE ARM
  GATE'S `IsHidden()` READ IS ONE GAME TICK STALE relative to the frame it arms:** the capture
  subsystem ticks (and arms) before the injector subsystem toggles `blinking`, and the toggle
  reaches the SAME frame's render (`F-1`'s guarantee — the one that exonerated the tag path — is
  what makes the hide path bite). Hidden at render ⇒ not in the visible set ⇒
  `bHasCustomDepthPrimitives` false (`SceneVisibility.cpp:2470`) ⇒ `RenderCustomDepthPass` false
  (`CustomDepthRendering.cpp:148`) ⇒ `StencilDummy` ⇒ 255. **Measured: 15/15 dummy frames hidden at
  end-of-frame, 14/14 joined real frames visible, ZERO refuter violations, D-R-R-D ⇔ 1-0-0-1 on all
  seven events — and the `missing_texture` control (never hidden) produced 32/32 REAL, 0 dummies,
  8/8 events clean (`framesDiscarded=0 contributed=4 collisions=0`), refuting the
  unrelated-per-burst-reason candidate.** The write side stays exonerated (0 verify collisions).
  ⛔ *(superseded by PART TWENTY-FIVE above: the fix is now DESIGNED, still not written)* **the
  gate order stands — `F-6` all five items, THEN the `SM_Ramp2` control, THEN the `H5` legs
  unblock.** 🚨 **The §171.4 hazard was elevated to `RULING 1` (see above).** 📌 **§172's stray
  post-`FinishRun` arm is folded into the design as the `bRunning` guard (H-5)** — it re-tags the
  target post-`RestoreAll` until fixed.
  🆕 **`G132`** — `GFrameCounter++` precedes `OnEndFrame.Broadcast()` (`LaunchEngineLoop.cpp:5568`
  vs `:5623`), so an end-frame sampler keyed on the counter matches nothing; the first M-4 build
  reported 0 lines and the pre-declared **B4** branch caught it (fixed at `9f91472`).
  ✅ **`P6` MEASURED UNCHANGED both ways: 48/48 keys, 0 added 0 removed, `annotation.json` AND
  `run_summary.json`, post-fix vs pre-fix banked leg.**
  📦 staged exe **`444D4812`** (hot-swap, built==staged verified; `722266A7` archived FIRST to
  `_binary_baselines\StackOBot.exe.m26-slice1-m1m3-instrument-722266A7`) · container UNCHANGED
  (`utoc 9334496D · ucas 62EB0072 · pak 78C977A5`, 4 maps). A44: all four new strings in the
  STAGED exe. Legs banked: `P24_M26S1F1_CTRL49`, `P24_M26S1F1_MTEX49` (+ per-attempt copies incl.
  3 pose-discarded first-cycle tries, A63).
  ⛔ **STILL TRUE: slice 1 NOT `F-6`-VALIDATED (necessary-not-sufficient: the blinking control is
  green but `SM_Ramp2` and item 5 are owed AFTER the fault-(ii) fix) · slices 2/3 NOT STARTED ·
  `H5` LEGS BLOCKED · `P6` DOES NOT MOVE · stencil range stays 200/255 · `feature/stencil-capture`
  READ-ONLY at `76cac74` · NO TAG.**
- 🟦 *(superseded — the first 2026-08-20 session's state; both faults were then open)* **`m26` SLICE 1
  SHIPPED, ITS MEASUREMENT PROVEN CORRECT, SLICE 1 NOT VALIDATED, TWO FAULTS OPEN.**
  **`m26` = the `H5` class-(ii) cure: an event whose target is MEASURED to draw nothing is removed
  from `annotation.json` before it is written. Shape (c) deferred veto + (b)'s reporting.**
  · **slice 1 (MEASURE ONLY, log-only, `IAI.Capture.Mask` default OFF) — SHIPPED.**
  🚨 **ITS MEASUREMENT IS PROVEN CORRECT: 7.23–7.25 % of frame, spread < 0.03 % over 14 frames,
  against `StaticMeshActor_49`'s banked 7.80 % rect — the right magnitude and slightly UNDER the
  bounding rect, which is what an occlusion-correct silhouette should be.**
  ⛔ **BUT SLICE 1 IS NOT VALIDATED** · **slice 2 (reporting) and slice 3 (the veto) NOT STARTED**
  · ⛔ **`H5` LEGS BLOCKED** until BOTH controls read NON-ZERO.
  🚨 **TWO OPEN FAULTS, INDEPENDENT, NEITHER FIXED — do not let one explain the other:**
  **(i) `CollectResults` discards frame-scoped results on an EVENT-scoped flag.** CAUSE ESTABLISHED,
  in our code: `if (R.CollisionHits > 0) { continue; }` skips every later frame of an event **before
  its count is read**, so MAX-across-frames never runs. ⚠ **Fixing this ALONE turns the control green
  at ~7.25 % while fault (ii) still fails half the frames — a partial instrument passing a gate,
  which is what `F-6` item 5 exists for.**
  **(ii) Custom depth is NOT PRODUCED on exactly half the armed frames** — 15 of 30, in a **fixed
  per-burst pattern (arm1 DUMMY · arm2 REAL · arm3 REAL · arm4 DUMMY), identical across all seven
  full events.** ⛔ **MECHANISM NOT ESTABLISHED AND NOT GUESSED.** ⚠ **LEAD ONLY, not a claim:**
  `blinking`'s half-period is 3 frames and `m20` established the hidden sample is ONE GAME TICK STALE.
  ✅ **PROVEN — DO NOT RE-PROVE:** `LOCK-1`'s hidden-tick refusal · the tag→mask→readback plumbing ·
  `AnomalyShaders` (`PostConfigInit`, no `Renderer` dep, other modules' load order untouched) · the
  four gates (cook · map set · **shader presence = the BOOT** · token read-back) · the
  `m26-slice1` quartet · the **write side exonerated** · **the cvar exonerated (`r.CustomDepth` = 3 on
  ALL 30 armed frames at the pass point)** · **255 = `StencilDummy` (`FColor::White`)**.
  ⛔ **WITHDRAWN, NOT DEFERRED: the tag/arm separation** — `F-1` refutes it at source (the proxy is
  already up to date; `SendAllEndOfFrameUpdates` runs inside `BeginRenderingViewFamilies`, same
  frame). **Zero ticks needed, and it is a guarantee.**
  📌 **BANKED for any future tag/arm split:** hidden-state tested at **TAG, ARM and RESOLVE**; hidden
  at **ANY** ⇒ **`NOT_MEASURED`**, never `MEASURED_ZERO`.
  ✅ **ADOPTED — `F-6` IS THE FIX GATE, all five items**, including 🚨 **item 5: the 255 detector
  PROVEN STILL LIVE, both ways (`G96`)** — without it items 1–4 can pass on an instrument that has
  stopped looking.
  ⛔ **Stencil range stays `200`/`255`. `CollectResults` is still event-scoped — left as found,
  deliberately. NO same-turn fix to a validity instrument, and the mask IS `m26`'s validity
  instrument.**
  📦 **ENVIRONMENT:** staged exe **`722266A7`** (code-only hot-swap over the `m26` cook) · container
  `utoc 9334496D` / `ucas 62EB0072` / `pak 78C977A5`, **4 maps** · quartets preserved at
  `m25-h4h5m1-measurement-build` (Parts 2–14) and `m26-slice1-measurement-build`.
  🗺 ⚠ **DISK TOPOLOGY: `Intermediate` and `Saved` are JUNCTIONS to `E:\IA_BuildCache\...`** — every
  path stays `D:\...` and **no tool needed editing. Do NOT "fix" the ~21 GB apparently missing from
  `D:`.** Runbook **§3.6**. ⚠ **Runbook §8.6 STEP 0 (disk floor) and STEP 3.5 (rebuild the EDITOR
  target — `G47`/`G131`) are NOT OPTIONAL.**
- 🟦 *(superseded — the H4/H5 investigation that produced `m26`)* **`H5` WAS THE PRIMARY LEAD.**
  📒 **READ `docs/invisible-anomaly-mechanisms.md` FIRST** — the five-row ledger of *distinct*
  mechanisms with *potentially distinct cures*, **now carrying `§6` THE COSTED CURE OPTION SPACE**.
  Then journal `docs/sessions/2026-08-19-045-h4-cook-and-h5-mainworld-arc.md`, which has a **PART
  INDEX** at the top covering **THIRTEEN parts** (H4 pre-flight → the H4 run → environment scout →
  pre-cook gate → the re-cook → MainWorld first launch → geometry survey → `H5` → traceability →
  cure measurement → cure OPTIONS costed + disk prune → **`C-1` ruled + the TIMING design**).
  🧮 **PART TWELVE — THE OPTION SPACE IS COSTED FROM SOURCE. NOTHING IMPLEMENTED, NOTHING PICKED.**
  **THE ANSWER, in one line: `C-1` (stencil pixel count) is the ONLY candidate addressing BOTH `H5`
  classes; `C-3` (`WasRecentlyRendered`) is the only cheap one and it answers `H4`'s question rather
  than `H5`'s; `C-5` (render-relevant bounds) is a `P6` correctness fix and a MEASURED NO-OP on `H5`;
  and NO candidate is blocked by DELIVERY MODE.**
  🚨 **`G128` — DELIVERY MODE GATES *REPORTING*, NOT *MEASUREMENT*.** `EvaluateSelectionProvenance`
  runs **unconditionally** (`:1599`); only the **sidecar** is suppressed (`:1720`); and `coverage_pct`
  from that same struct **already reaches `annotation.json` in BOTH modes** (`:1691`). ⇒ *"a cure that
  only works with delivery off is not a cure"* — **none of the five is one.** 🆕 **And
  `annotation.json` ALREADY EMITS `mask:{provided:false}` and `depth:{provided:false}` on every event**
  (`AnomalyLabelWriter.cpp:452-459`) — **the slots a mask/depth cure reports through already ship**, so
  populating them changes a VALUE, not the field SHAPE.
  🚨 **`G127` — THE FILTER SHIPS IN EVERY CONFIG; EVERY PIXEL MEASUREMENT IS COMPILED OUT OF SHIPPING.**
  `IsRenderableComponent` lives in **`AnomalyInjector`** (no render deps, all configs);
  the SVE/readback live in **`AnomalyCapture`** (`ANOMALY_CAPTURE=0` in Shipping). ⇒ *"have the selector
  ask the mask"* is a **MODULE-SHAPE DECISION.** ⚠ **Second half, easier to miss: a pixel measurement
  CANNOT INFORM A SAME-FRAME PICK-TIME DECISION** — async readback, and the stencil branch budgets
  **12 frames** before abandoning a mask. **Any pixel-derived cure is a PRE-FLIGHT (arm → wait →
  decide), never an inline predicate.**
  🚨 **`G126` — `WasRecentlyRendered()` is TRUE for a SHADOW-ONLY contributor.** The shadow path bumps
  `LastRenderTime` with `bUpdateLastRenderTimeOnScreen=false` (`ShadowSetup.cpp:1672,1909`) and
  `AActor::WasRecentlyRendered` reads exactly that field. **`SM_Ramp2` is that shape.** It is also
  **BINARY** (4 px reads like 400,000) and doubly latent. ✅ Its `true` path *is* gated on
  `PrimitiveDefinitelyUnoccludedMap`, so it is genuinely occlusion-aware — **an `H4` candidate, not an
  `H5` one.**
  🔎 **`C-1` READ-ONLY FINDINGS (branch never checked out):** the measurement is **already built** —
  `FAnomalyMaskAABB.Count` **IS** the surviving-pixel count. ⚠ **But three things travel with it:**
  its last commit **excludes `InstancedFoliageActor`** (the blacklist the owner RULED is not a fix)
  **on a code comment asserting "standalone ISM/HISM crates have sane bounds" — now MEASURED FALSE**;
  its tagging tests **`USkeletalMeshComponent`** while the selector tests the base
  **`USkinnedMeshComponent`**, so a `UPoseableMeshComponent` target is **selectable but never tagged ⇒
  mask Count 0 on a target that draws**, failing in the dangerous direction; and its reduction is an
  **unpriced W×H CPU scan ON THE RENDER THREAD** per armed frame.
  🆕 **A REAL DEFECT `C-5` WOULD FIX, found in source:** the **label** path
  (`ProjectActorBoundsToScreenRect`, `:653-685`) unions components on a **TYPE-ONLY** test with **no
  `IsVisible()` gate**, while **selection** (`:493`) **does** check it — **label geometry and selection
  geometry already disagree about what "the object" is.** ⛔ Still a no-op on `H5`.
  📌 **RULING 1 (ledger §3.3) — the `H4`/`H5` SHARED CURE IS A HYPOTHESIS, NOT A FINDING**, recorded
  verbatim; it is attractive *because* it would collapse two mechanisms into one fix, which is the
  reason to test it rather than assume it. 📌 **RULING 2 (ledger §3.2) — `SM_Ramp2` IS AN A35 CASE AND
  IT CONSTRAINS THE CURE:** peak-OUT **0.2955** > peak-IN **0.1785** on a legitimate target ⇒ **any
  bbox-scoped measurement inherits that failure mode and must say so.**
  💾 **DISK PRUNED — 12.89 → 19.12 GB free (6.23 GB recovered).** 79 leg dirs deleted, each proven a
  duplicate **BY SESSION ID *and* by an identical per-file path+size manifest** (stronger than asked,
  chosen because PART ELEVEN's zero-byte frames prove a size-blind check would bless a truncated copy).
  ⛔ **4 dirs NOT verifiable ⇒ NOT deleted, listed and left alone** — **`H4_WSECHO` and
  `MW_STOMPER_try1` are UNBANKED EVIDENCE** (session ids absent from the bank; a name-based sweep would
  have destroyed them) plus `D3D12` and an empty `S3A2_OFF`. **`_binary_baselines` (11 files, the
  `pathA` quartet hash-identical), the bank (148 dirs) and the staged build are UNTOUCHED and verified
  after the prune.**
  **WHERE THINGS STAND, in one read:**
  · **`m23`/`P3`** — FIXED, shipped.
  · **`H4`** (occluded target, occlusion-blind projector) — **SUPPORTED, path (b), mechanism only, n=1
  leg. NO INCIDENCE CLAIM.** Cure = `feature/stencil-capture`, **UNTOUCHED**.
  · **`H5`** (the selector's renderable test is a **TYPE test, not a DRAWING test**) — **class (ii)
  SUPPORTED with n=2 measured instances** (`InstancedFoliageActor`, **`BP_SplineSpawn_C` — not
  foliage**); **class (i) ENUMERATED, NOT OBSERVED** (owner's `BP_LocalVolumetricFog` is not
  reproducible here — the client runs her own game).
  · **Traceability** — characterised, **NOT a cause**: `asset_name`/`component_class` are intact in
  builds (15/15); only `node.name` is uninformative, and identity is **non-deterministic** for actors
  that toggle component visibility.
  · **Path (a)** — **PARKED, NOT REFUTED.** A *priority* decision, not a scope one (G120).
  🎯 **OWNER RULINGS — DIRECTION AND SHAPE ARE BOTH SETTLED. `C-1` (SURVIVING-PIXEL COUNT VIA A MASK)
  IS THE DIRECTION; THE SHAPE IS `(c)` DEFERRED VETO WITH `(b)`'s REPORTING.** ⛔ **NOT A REVIVAL OF
  `feature/stencil-capture`: MINE IT, DO NOT RESUME IT** (its foliage blacklist must not survive; its
  `USkeletalMeshComponent` narrowing **manufactures the exact defect the cure exists to detect**).
  **THE CURE ITSELF IS NOT WRITTEN.**
  ⛔ **SHAPE (a) PRE-FLIGHT VETO IS REJECTED — PERMANENTLY, NOT DEFERRED — on TWO INDEPENDENT
  BLOCKERS.** (1) **Selection → fire is ZERO frames**: `BeginFire` → `TryFireOnce` runs
  `GetVisibleRenderableActors`, **both seeded picks** and `ApplyAnomaly` (which hides the actor) in
  **one synchronous call in one tick**; the longest gap anywhere is **6 frames** and `LeadIn` (4) runs
  **once per RUN**. (2) **A re-picking veto destroys the seeded draw protocol** — `R-SEED` is
  deliberately independent of apply-result and `m22` gated on *"seed 4242, two runs byte-identical"*.
  **If a future reader proposes "just check before firing", both blockers are in journal §103.**
  🚨 **THE MASK WORKS. → journal PART TWENTY-THREE §162-§168. MEASUREMENT TURN, NO FIX. NO TAG.**
  **On every frame where custom depth was produced it returns `totalMasked` **66,635–66,862 px =
  7.23–7.25 % of frame**, spread **< 0.03 %** across 14 frames — against `StaticMeshActor_49`'s banked
  **7.80 %** rect. **The right magnitude, and slightly UNDER the bounding rect, which is exactly what
  an occlusion-correct silhouette should be.** ⇒ **the instrument is fundamentally sound; it was never
  broken, it was being discarded.**
  ✅ **`M-1`/`M-3`: `r.CustomDepth` IS EXONERATED — the last link anyone could reason about.**
  `beginRun before=1 after=3`, `finishRun before restore=3`, and **`rCustomDepth_renderThread=3` on
  ALL 30 armed frames**, read at the pass point where it is consumed.
  🚨 **`M-2`: BRANCH "THEY DISAGREE" — mode 3 AND custom depth NOT produced, on EXACTLY HALF the armed
  frames (15/30), in a FIXED per-burst pattern: arm1 DUMMY · arm2 REAL · arm3 REAL · arm4 DUMMY,
  identical across all seven full events.** Measured directly by the pre-declared discriminator —
  `StencilDummy` is **1×1**, the real texture is view-sized, so `customStencilExtent` settles it
  without inference. ⛔ **THE MECHANISM FOR THE ALTERNATION IS NOT ESTABLISHED AND IS NOT GUESSED**
  *(`blinking`'s 3-frame half-period and `m20`'s one-tick-stale hidden sample are an obvious place to
  look, and that is a lead, not a claim)*.
  🚨 **SECOND, INDEPENDENT FAULT, AND IT IS MINE: ONE BAD FRAME DISCARDS A WHOLE EVENT.**
  `CollectResults` does `if (R.CollisionHits > 0) { continue; }` — **the collision flag is
  EVENT-scoped where the observation is FRAME-scoped**, so after the first dummy frame every later
  frame of that event is skipped **before its count is read**, including the good ones. **The
  MAX-across-frames design never runs.** ⚠ **The two faults are INDEPENDENT: even with the alternation
  unexplained, a frame-scoped discard would have produced `MEASURED_NONZERO` at ~7.25 % on this
  control.** ⛔ **NOT FIXED — no same-turn fix to a validity instrument.**
  📌 **RECORDED: the tag/arm separation is WITHDRAWN (not deferred).** **`F-2`'s rule is BANKED** for
  any future split — hidden-state tested at **TAG, ARM and RESOLVE**, hidden at ANY ⇒ `NOT_MEASURED`.
  **`F-6` ADOPTED IN FULL as the fix gate**, including **item 5: the 255 detector proven still live
  both ways (`G96`)**, without which items 1–4 can pass on an instrument that has stopped looking.
  📦 build: exe **`722266A7`** (code-only hot-swap; container unchanged from the `m26` cook, boot
  re-verified). Leg `P23_M23_CVAR_CTRL49`, **B1 PASSED**, A63 attempt 2 (attempt 1 banked as a pose
  discard).
  🚨 *(superseded)* **`F-1` REFUTED THE APPROVED TIMING FIX. → PART TWENTY-TWO §157-§161.** Its
  refutation **STANDS**; the cvar it pointed at as next has now been measured and is clean.
  **THE PROXY IS ALREADY UP TO DATE.** `SetRenderCustomDepth` → `MarkRenderStateDirty` →
  `MarkForNeededEndOfFrameRecreate`, **and that recreate is flushed INSIDE
  `FRendererModule::BeginRenderingViewFamilies` in the SAME frame** (`SceneRendering.cpp:4528`), whose
  own comment reads *"Guarantee that all render proxies are up to date before kicking off a
  BeginRenderViewFamily."* ⇒ **tag-and-arm-in-the-same-tick is NOT the fault; separating them would
  change nothing.** ⇒ **`F-1`'s answer is ZERO ticks, and it is a GUARANTEE, not a typical case.**
  ⚠ **That is the `ReservedStencilMax` mistake in a new place — a change targeting a symptom whose
  mechanism source refutes. Asking `F-1` BEFORE writing the design is the only reason it was caught.**
  ✅ **TWO MORE CANDIDATES EXONERATED FROM SOURCE:** the **post-Tonemap pass point** is legitimate —
  `SceneTextures.SetupMode |= CustomDepth` and the uniform buffer is **rebuilt** whenever
  `RenderCustomDepthPass` returns true (`DeferredShadingRenderer.cpp:2981,3306`); and the **cvar
  priority** is fine — `ECVF_SetByCode` is second-highest and **no project ini sets `r.CustomDepth`
  at all**. ⛔ *(priority only — that the write would not be rejected, NOT that the value was 3 at
  pass time.)*
  🚨 **SOURCE-ONLY DIAGNOSIS IS NOW EXHAUSTED: every link reads as correct and the measurement
  disagrees.** ⇒ **NEXT IS MEASUREMENT — `r.CustomDepth`'s EFFECTIVE value at pass time, the one link
  still carrying its "assumed, not measured" label.** `GetCustomDepthMode()` maps **1→`Enabled`** and
  only **3→`EnabledWithStencil`**; `FCustomDepthTextures::Create` returns an INVALID set when not
  enabled, `RenderCustomDepthPass` then returns false, and no custom depth is produced. **A value of 1
  at pass time produces EXACTLY the observed symptom.** ⛔ **Not a claim it IS the cause — a claim it
  is the only unmeasured link with a mechanism that reaches the symptom.**
  📐 **`F-4`/`F-5`/`F-6` ANSWERED (cause-independent).** Tags release only at `FinishRun` and the
  release is **symmetric** (same deferred path, same guarantee); a residual 255 after any fix is
  **diagnostic** (uniform ⇒ pass not run that frame · geometry-shaped or any other reserved value ⇒
  something genuinely writing); and 🚨 **the GATE must be POSITIVE evidence — both controls NON-ZERO
  with `collisions=0`, arm counts matching prediction, a plausible `pctOfFrame`, and `G96` BOTH WAYS:
  THE 255 DETECTOR PROVEN STILL LIVE, so its silence means absence and not blindness.**
  ⚠ **`F-2`/`F-3` CONTINGENT** — but the rule is fixed now for any future tag/arm split: **the
  hidden-state test applies at tag time, arm time AND resolve time, and hidden at ANY of them ⇒
  `NOT_MEASURED`, never `MEASURED_ZERO`** (the window is exactly `blinking`'s 3-frame half-period, so
  the transition is the common case, not a rare race). And `missing_object` has only a **6-tick**
  post-revert window, so a large separation risks **ZERO arms** — a cure that never fires while
  looking like a clean pass.
  🎯 *(carried)* **THE SLICE-1 FAULT: `255` IS THE ENGINE'S `StencilDummy`. → PART TWENTY-ONE
  §149-§156.** ⚠ **That part's `D-4` "one-frame ordering" explanation is SUPERSEDED above; its
  `255 = StencilDummy` finding STANDS.**
  🚨 **`D-3` ESTABLISHED FROM ENGINE SOURCE: 255 IS THE ENGINE'S OWN FALLBACK.**
  `SystemTextures.cpp:247-256` creates `StencilDummy` as a **1×1 `PF_R8G8B8A8_UINT` filled with
  `FColor::White` = 255**, and `SceneTextures.cpp:959` binds
  `CustomStencilTexture = bCustomDepthProduced ? CustomDepthTextures.Stencil : StencilDummySRV`.
  ⇒ **when custom depth is not produced, `CalcSceneCustomStencil` returns 255 at EVERY pixel** —
  which is the observation exactly (always 255, never 254, uniform, both levels, 25 times).
  ⇒ **`D-1`: the read is LIVE and the pass point is fine; it resolves to the DUMMY.**
  🚨 **THE `ReservedStencilMax` CANDIDATE IS REFUTED — AND REPAIRING IT WOULD HAVE BEEN WORSE THAN
  LEAVING IT BROKEN.** Moving the range to e.g. 100-155 leaves every read returning 255, which would
  then fall **outside** our range ⇒ the shader writes 0 ⇒ **the unassigned-tag detector GOES SILENT**
  ⇒ every event returns `MEASURED_ZERO` with `collisions=0`, a clean-looking answer that **under
  slice 3 would VETO EVERY EVENT.** **A loud fault converted into a silent one** (`G118`: a guard that
  passes the unsafe case is worse than no guard). **The pre-declared refuters are what caught this.**
  ⚠ **`D-4` LEADING CANDIDATE, source-grounded, NOT measured: A ONE-FRAME ORDERING BUG.**
  `SetRenderCustomDepth` → **`MarkRenderStateDirty()`, a DEFERRED render-state recreate** — the proxy
  flag is **not live for the frame it is set in** — and `ArmIfMeasurable` **tags and arms in the SAME
  tick**. ⇒ the mask pass for frame N runs against a proxy that has not received the flag ⇒ custom
  depth not produced ⇒ dummy ⇒ 255. ⚠ **HONEST LIMIT: `r.CustomDepth`'s EFFECTIVE value at pass time
  was NOT read back** (`-ExecCmds` is startup-only and would report the default, 1); *"the cvar is
  fine"* is **assumed, not measured**, and is recorded as such.
  ✅ **`D-2`: THE WRITE SIDE IS EXONERATED.** One event (`P20_M26S1_RAMP` `startFrame=4`) had
  `collisions=0` — property verified `bRenderCustomDepth true` and value == our tag — and **still
  returned count 0** ⇒ read-side fault. It also explains why that one event saw no 255: earlier tags
  had propagated by then, so the real stencil was bound, while its own target had only just been
  tagged. **One cause, two different-looking symptoms.**
  ✅ **`D-5`: benign** — the outlier (`arms=1`) is the LAST event of each leg, truncated by the
  90-frame cap. Not a second fault.
  ✅ **TASK 2 DONE — the detector no longer names a cause it has not established.** It said *"a host
  title is writing into the reserved range"*, which **would have sent a future reader hunting a host
  game in a sample project** (`G120`). It now reports the OBSERVATION plus the DISCRIMINATOR (255
  uniform = engine fallback; geometry-shaped or any other value = something genuinely writing).
  Message only, no logic changed.
  ⛔ **STILL STANDING, not re-opened by this fault: `LOCK-1` is PROVEN** (`skippedHidden=3..4` on
  every event) · the **tag → mask → readback plumbing round-trips** · the **module** · the **four
  gates** · the **new quartet**.
  ⛔ **DO NOT: run the H5 legs until a control reads NON-ZERO · move the stencil range · start slice
  2 or 3 · tag.**
  🛑 *(superseded)* **THE COOK SUCCEEDS AND THE BUILD CANNOT BOOT — HALTED ON `LoadingPhase`.
  → PART NINETEEN §136-§142.** ✅ **RESOLVED in PART TWENTY by Option B.**
  ✅ **DISK IS SOLVED: `Intermediate` (9,106 files / 13.0 GB) and `Saved` (5,443 files / 7.7 GB) are
  MOVED TO `E:` AND JUNCTIONED**, so every path stays literally `D:\...` and **`run_leg.ps1` and the
  other 15 baked paths needed NO edits**; the queued `E:` migration stays queued. Move byte-verified;
  read-back through the junction confirms the file counts. **free D: 39.9 → 59.2 GB.** ✅ Through the
  whole rebuild **`freeD` stayed flat while `freeE` absorbed the churn.** ⚠ **Residual, not
  junctioned: `D:\UESource\UnrealEngine\Engine\Intermediate` (59.8 GB) is engine-side and still on
  `D:`** — it swung D: between 42 and 59 GB and always recovered.
  ✅ **COOK: `BUILD SUCCESSFUL`, ExitCode=0, 39m 26s. MAP GATE PASS** — `CB_GateLevel` · `Entry` ·
  `MainMenu` · `MainWorld` all PRESENT, read from the artifact, both encodings.
  🚨 **AND THE PACKAGED BUILD STILL DIED AT ENGINE INIT on `Missing global shader
  FAnomalyVisibleMaskPS's permutation 0`. TWO CAUSES, EACH REPORTING SUCCESS AT THE STEP THAT CAUSED
  IT → `G131`.**
  **(1) `G47` AGAIN — THE COOK RUNS ON EDITOR BINARIES.** `UnrealEditor-AnomalyCapture.dll` was
  **two days stale** and A44 on it read **`AnomalyVisibleMask` 0 · `/Plugin/AnomalyInjector` 0 ·
  `IAI.Capture.Mask` 0** against **`IsHideTypeAnomaly` 1** ⇒ **scan sound, not blind.** So
  `AddShaderSourceDirectoryMapping` never ran and the shader was never compiled. ✅ Fixed by
  `Build.bat StackOBotEditor` (**45 s / 22 actions**, dll 473,600 → 590,336 B). ⚠ **`G47` has said
  this since `m8`; RUNBOOK §8.6 DID NOT — its recipe builds only the GAME target. The knowledge
  existed and the recipe did not carry it.** ✅ **§8.6 now has STEP 3.5.**
  **(2) 🛑 THE BLOCKER — `LoadingPhase`.** With fresh editor binaries the cook crashed at commandlet
  startup and the engine named the fix: *`Assertion failed: !bInitializedSerializationHistory
  [Shader.cpp:246] — Shader type was loaded after engine init, use ELoadingPhase::PostConfigInit on
  your module to cause it to load earlier.`* **`AnomalyCapture` is `"LoadingPhase": "Default"`.**
  ⛔ **NOT a one-line flip — `AnomalyCapture` DEPENDS ON `AnomalyInjector`, also `Default`, and a
  module cannot load before its dependency.** ⇒ **a PLUGIN-DESCRIPTOR change ⇒ HALT per the brief.**
  🧭 **THE OWNER'S PICK — A / B / C, journal §139.1. RECOMMEND B:** a **new tiny `PostConfigInit`
  module declaring ONLY the shader** (the standard UE pattern), leaving `AnomalyCapture` and
  `AnomalyInjector` at `Default` with load order **untouched**. A = flip both modules (broad blast
  radius); C = drop the global shader (re-opens what `M-2` already settled on correctness).
  ✅ **BENCH RESTORED AND VERIFIED BOOTING** — cook #1 left the staged build non-bootable; restored
  from `_binary_baselines\m25-h4h5m1-measurement-build\`, **all six files hash-MATCH**, control server
  answers over WS, A48 behavioural echo present, **no missing-shader fatal.** **The preservation
  precondition earned its keep within two hours of being taken.**
  ⚠ **`G115` FIRED ON ME AND THE MECHANICAL DIFFSTAT CHECK IS THE ONLY REASON IT DID NOT LAND.** A
  `Get-Content -Raw` → `Set-Content` round-trip for one PART-INDEX line returned **`2940 ++++----` on
  a ~130-line addition**; diagnosis showed **`⚠` had become `Ã¢Å¡Â `** — every non-ASCII line in a
  2,900-line journal double-encoded. **Reverted and re-applied through the editor tool.** ⚠ **I
  attempted two "fixes" (strip BOM, convert EOL) BEFORE establishing the cause — both wrong;
  `core.autocrlf=true` and the blob is LF/no-BOM, so neither was ever the real diff.**
  🚧 *(superseded)* **THE COOK IS IN FLIGHT. → journal PART EIGHTEEN §129-§135.**
  Cleanup and banking are **DONE**; the cook is a **full 761-action rebuild** (because `Intermediate`
  was deleted to buy its disk space) running at **ONE parallel process** — UBT's own line:
  *"Requested 1.5 GB free memory per action, 2.36 GB available: limiting max parallel actions to 1"*,
  because a second `UnrealEditor` is resident at **3.19 GB** (**`G97`**, a permanent environmental
  fact). **Memory-bound, not CPU-bound: 10 cores idle.** ⛔ **NOT interrupted** — the build phase
  writes only `Intermediate`/`Binaries`, **no container is being written yet**, so it is safe where it
  stands. ⛔ **NO completion estimate is offered as a finding** — the action list is PCH-heavy at the
  head and a linear extrapolation would be a claim the data does not support.
  ⛔ **CONSEQUENTLY NOT RUN: the `verify_cooked_maps.ps1` map gate · the A44 scan of the new artifact ·
  the token read-back · the new quartet identity · THE ENTIRE SLICE-1 VALIDATION.** ✅ **Ready so the
  next turn starts clean: the slice-1 PRE-DECLARED BRANCH TABLE, `CaptureBench/tools/
  p18_slice1_predeclared_branches.md`, commit `972840d` — written and committed BEFORE the cook
  finished and before any leg ran.**
  ✅ **DISK: 0.94 GB → 21.93 GB, per-tree** — `.vs` +4.65 · `Intermediate` +14.56 · *(banking −3.89)* ·
  `Builds\BenchGate\...\Saved` +5.67 (**75 of 75 re-verified duplicate by session ID AND per-file
  manifest, in the SAME script as the deletion** so nothing separated verify from delete).
  ✅ **21 PIE-ERA SESSIONS BANKED, 21/21 manifest-verified; bank 158 → 159 dirs / 183 session ids.**
  🚨 **ONE OF THEM IS CITED EVIDENCE — `session_20260817-132214` IS THE `m23` OWNER PLAY-GATE SMOKE**,
  the first confirmation of the `m23` fix in real gameplay content, sitting unbanked in the project
  tree. **Third time the session-ID method has found unbanked evidence; a name-based sweep found none
  of the three.** ⚠ Originals left in place until the cook succeeds.
  🚨 **RULING 1's ORDER WAS IMPOSSIBLE AND THE RESOLUTION IS THE LESSON (`G130`): banking 3.89 GB needs
  3.89 GB, and 0.94 GB existed.** The first attempt died mid-copy and left **a PARTIAL bank dir (652
  files / 891 MB) indistinguishable by name from a complete one** — deleted at once, source
  re-verified. ⇒ **when preservation and cleanup contend for the same disk, FREE ONLY WHAT CONTAINS NO
  EVIDENCE FIRST, then bank, then free the verified duplicates.** The literal order changed; the
  protection did not.
  🆕 **`G130`** — *an operation's WORKING SET is not its OUTPUT SIZE; running out of room mid-way
  yields a half-written artifact behind a system that still starts.* + **`setup-runbook.md` §8.6
  STEP 0**: a go/no-go disk floor (**≥15 GB GO · <10 GB NO-GO**) naming what is **not** free space.
  🗺 **`E:` MOVE SCOPE NOTE DELIVERED (journal §135) — LIST ONLY, NOTHING CHANGED.** 🚨 **The hazard is
  the UNTRACKED set, and the worst item is not a file: `HKCU\Software\Epic Games\Unreal Engine\Builds`
  maps the `.uproject`'s GUID → `D:/UESource/UnrealEngine`** — in no repo, no file, no backup. Also
  untracked: **a SECOND copy of the host-tools watchers at `D:\IntrusiveAnomalies\host-tools\`** (that
  directory is not a git repo) and a **third** in `_M2Smoke\`, plus both preserved-evidence READMEs.
  **Engine: LEAVE IT ON `D:`** — the `.uproject` resolves by GUID, our `Build.cs` files are
  engine-relative, and it is 230 GB. ⚠ **A full rebuild is required after any move regardless** —
  `Intermediate\**\*.dep.json` stores absolute paths. 🚨 **Worst failure mode is SILENT: the 5 python
  sweeps' `BANK = r"D:\..."` would report an EMPTY CENSUS as a clean result, and
  `overlay_watcher.py`'s `CAPTURES_ROOT` would poll a missing dir forever without erroring.**
  🛑 *(carried)* **OWNER RULED *COOK*; PART SEVENTEEN's disk halt is now CLEARED. → §124-§128.** ✅ **Precondition 1 PASSED FIRST — the COMPLETE quartet (exe `101AFEA4` +
  `utoc 939B9C9B` + `ucas 8A602D4D` + `pak 7CAE22DD` + both `global.*`) is preserved at
  `_binary_baselines\m25-h4h5m1-measurement-build\`, **6/6 hash-verified AT THE NEW LOCATION** (A62),
  README updated with what rests on it — **every result in journal PARTS TWO–FOURTEEN**. ✅
  **Precondition 2 RECORDED:** map set declared in writing = `CB_GateLevel` + `MainMenu` + `Entry` +
  `MainWorld`; `verify_cooked_maps.ps1` is the post-cook gate and has nothing to read yet.
  📉 **WHERE THE DISK WENT (19.12 GB after the PART TWELVE prune → 0.94 GB): this session's own work**
  — two full UBT builds with UHT, three engine-fatal launches, shader-compile attempts, eight legs.
  `StackOBot\Intermediate` alone is **14.54 GB**. **Classified, nothing deleted:** ✅ regenerable —
  `Intermediate` 14.54 GB, `.vs` 4.72 GB, and **`Builds\BenchGate\...\Saved\` 5.66 GB which is a
  VERIFIED DUPLICATE (75 sessions, ALL 75 banked by session ID — the PART TWELVE prune never covered
  this tree)**; ⚠ needs a ruling — `Builds\MidRepro` 6.57 GB, `Builds\Windows` 3.38 GB, the bank
  16.28 GB; 🚨 **DO NOT TOUCH — `StackOBot\Saved\AnomalyCaptures` 3.89 GB: 21 sessions, ZERO BANKED.**
  🚨 **THAT LAST ONE IS A FINDING IN ITS OWN RIGHT — 3.89 GB of UNBANKED evidence in the project tree,
  found by the session-ID method while looking for disk space. Bank it before deleting anything near
  it.**
  ⚖ **`G-3` IS AMENDED (RULING 1, journal §124) AND THE REASON TRAVELS WITH IT:** *"byte-identical
  with the switch OFF"* is **unobtainable** for a global shader (`G129`), so `G-3` becomes a **CONTROL
  PAIR against a build that does not contain the shader** — `m25`'s preserved binary vs
  `m26`-switch-OFF, run-unique set established from a same-binary pair first, **extras must be 0**,
  decided by a rule fixed in advance. **Still BOTH WAYS (`G96`).** ⚠ **Switch-OFF inertness was
  RETIRED BY MEASUREMENT, NOT WEAKENED BY CONVENIENCE.**
  🛑 *(carried)* **`m26` SLICE 1 IS WRITTEN AND COMPILES CLEAN — VALIDATION BLOCKED ON THE COOK.
  → journal PART SIXTEEN §119-§123.**
  🚨 **`G129` — THE HALT, AND IT RETIRES A PRECEDENT: A NEW GLOBAL SHADER CANNOT RIDE THE CODE-ONLY
  HOT-SWAP, AND A DEFAULT-OFF SWITCH DOES NOT MAKE IT INERT.** The slice-1 exe (`15A87075`)
  hot-swapped cleanly and **A44 confirmed every new symbol, both encodings, including the `.usf`
  virtual path** — then the build **died at ENGINE INIT, 3 of 3 attempts, no artifact**:
  *`Fatal error: [ShaderCompiler.cpp:6931] Missing global shader FAnomalyVisibleMaskPS's permutation
  0, Please make sure cooking was successful.`* **Global shaders live in the COOKED CONTAINER, which
  the hot-swap does not touch** (`G121`'s quartet biting from the other side). ⇒ **it fires BEFORE
  anything runs and it fires WITH THE MASK SWITCH OFF — `IMPLEMENT_GLOBAL_SHADER` is not gated by any
  cvar.** ⚠ **So `S3a`'s "switch-OFF inertness is STRUCTURAL" precedent DOES NOT EXTEND to a global
  shader, and `G-3` must become a CONTROL PAIR against a build without the shader (the `m24` method),
  not a switch-OFF leg on the same binary.** ⚠ **A44 passing is NOT sufficient for a shader change —
  the symbol reached the binary and the build still could not boot.**
  🧭 **NEXT, AND IT IS THE OWNER'S CALL: (1) COOK OR NOT** — slice 1 cannot be validated without one,
  and a cook **retires the pak quartet every `H4`/`H5`/`M-1` measurement was taken on**, and this
  project has always treated cooks as owner-sequenced (`G118` was explicitly *"AFTER the current
  measurement sequence and NEVER inside one"*; `G92` wipes `Saved`). **(2) If cooking, DECLARE THE MAP
  SET IN WRITING FIRST** (`CB_GateLevel` + `MainMenu` + `MainWorld` + `Entry`). **(3) Amend `G-3`'s
  wording** per `G129`.
  ✅ **BENCH RESTORED AND VERIFIED: staged exe back to `101AFEA4`** (m25 baseline, hash-verified from
  `_binary_baselines`), pak quartet untouched; **it boots and writes full sessions** (3 restore-smoke
  attempts, 97 files each, banked). ⚠ **Those 3 attempts FAILED THE B1 POSE GATE** (`modal_rot
  (0,2.27,0)`, `distinct=10`, bbox width `69.0` vs `306.1`) — by the harness's own discriminator that
  reads as **genuine A47 bifurcation**, ⛔ **CAUSE NOT ESTABLISHED and NOT attributed** (3 consecutive
  is above the recorded ~2-in-5; the box was memory-pressured at ~3.7 GB free after three
  engine-fatal launches — **association only**). **It is NOT evidence about slice 1: the bench boots
  and produces artifacts.**
  ⚠ **MY OWN GAP, RECORDED: I overwrote the staged `1EBA8944` (the `M-1` instrument build) WITHOUT
  archiving it, having archived `101AFEA4` before the previous swap.** Loss is bounded — `1EBA8944`
  rebuilds from commit `0185c10` and `M-1`'s results are banked — **but the rule I followed once I did
  not follow twice.**
  📋 **THE `m26` PLAN IS APPROVED WITH FOUR AMENDMENTS (journal PART FIFTEEN §110-§118, PART SIXTEEN
  §119).** **`A-1`** veto only `manifested == true` — the two counters stay disjoint and are defined
  side by side (*"the hide never showed in pixels"* vs *"the target contributed no pixels to hide"*).
  **`A-2`** risk 4 becomes **gate `G-11`** (report counted events BEFORE and AFTER the veto; below 3
  ⇒ the leg is INVALID) **plus a client-facing derivation note — a post-`m26` event count is NOT
  comparable with a pre-`m26` one**. **`A-3`** the stencil collision **IS detectable and both
  detectors are implemented** — a game-thread property read-back, and unassigned-reserved-tag
  detection in the mask; either ⇒ `NOT_MEASURED` + a loud warning. **`A-4`** `G-5` now reports
  peak-IN/peak-OUT beside the veto decision, so `SM_Ramp2` surviving becomes a **measured** data point.
  ⛔ **SLICES 2 (reporting) AND 3 (veto) ARE NOT STARTED.** `P6` DOES NOT MOVE — `mask.provided`
  false→true is a VALUE change and is in scope; SUB-FIELDS under `mask` are a SHAPE change and are NOT.
  🚨 **THE PLAN'S RISKIEST ITEM, `P-2`, NAMED SO IT IS NOT LOST: a hide-type target is HIDDEN during
  the positives, so a naive measurement reads ZERO and would INVALIDATE EVERY HIDE-TYPE EVENT EVER
  RECORDED** (`blinking` + `missing_object` = the bulk of the dataset). **The design survives ONLY
  because "no qualifying frame" lands in `NOT_MEASURED`, never `MEASURED_ZERO`** — which is precisely
  why the two zeros were required to be different states. Rule: **arm the mask only on a tick where
  the target is KNOWN NOT HIDDEN** — in-window for non-hide types and for `blinking`'s un-hidden
  frames (`HiddenByIndex` already records them), and **post-revert** (`SettleAfterRevert` + `PostGap`)
  for `missing_object`, which never draws in-window. ⚠ **Post-revert rests on a SETTLED CAMERA and a
  STATIONARY TARGET** — measured here (`dYaw 0.0000` over 200 frames; gate-level eye invariant
  844/844) and **NOT true in a level with motion**; it goes in the tag's scope statement.
  ⚖ **RULING 2 — THE NEGATIVE BRANCH IS A SHIP GATE, NOT A TEST CASE** (a guard that has never fired
  is not a guard, `G96` ×3): it must **FIRE** on `InstancedFoliageActor_0_0_0` / `BP_SplineSpawn_C`,
  **NOT OVER-FIRE** on `StaticMeshActor_49` **and 🚨 `SM_Ramp2`** (peak-OUT `0.2955` > peak-IN
  `0.1785` — **if the reduction is rect-scoped anywhere, `SM_Ramp2` is where it wrongly fires**, so
  the reduction is **WHOLE-FRAME**), **ADMIT** when blind byte-identically, and be **LOUD** when blind.
  ⛔ **WHAT DOES NOT SURVIVE FROM `feature/stencil-capture` INTO THE CURE:** its
  **`InstancedFoliageActor` blacklist** (the selector is not touched at all); its
  **`IsRenderableMesh`** narrowing (replaced by the shared `IsRenderableComponent`); **`StencilViz`**
  (the only post-Slate path that can bake into a delivered frame — not shipping it removes that
  hazard); and the branch's **`bbox_norm` re-sourcing**, which is the label-RECT fix and is **`P6`
  movement, out of scope**.
  ⚠ **RISKS CARRIED INTO THE PLAN:** ✅ seeded reproducibility **CONFIRMED UNAFFECTED** from source
  (selection untouched; fixed timestep means GPU cost cannot perturb the stream) · `m23` needs a
  precedence rule (**veto only `manifested == true` events**, keeping the two counters disjoint) ·
  `m25` delivery orthogonality becomes **gate `G-9`** · 🚨 **a veto REMOVES events and `≥3 counted
  events` is a VALIDITY condition, so a veto can silently turn a leg from EVIDENCE into INVALID** ·
  `positive_frames` is fire-active and will disagree with a vetoed event · `ReservedStencilBase 200`
  is a **convention, not a reservation**, on a host title using custom stencil · 🚨 **the mask
  measures DRAWN SILHOUETTE, NOT VISUAL EFFECT**, so an almost-entirely-shadow target would be wrongly
  vetoed and **none has ever been measured**.
  ⚙ **`T-4`'s 256-entry array IS in the plan** (precondition now met): worst case **~9–28 ms → ~1–3
  ms**, and the mask arms **a few times per burst, not per captured frame**. ⛔ **GPU-side reduction:
  PREMATURE, FILED NOT BUILT** — no measurement says it is a problem, and `speed_ratio` is already the
  instrument that would show it (gate `G-6`).
  🚨 **`M-1` ANSWERED — AND THE 10-vs-12 CONFLICT DISSOLVED RATHER THAN RESOLVING.** Measured on the
  existing colour readback, both delivery modes, 90 samples each: **readback latency is ONE render
  frame** (delivery OFF `min 1 max 2 mean 1.011`, hist `1:89 2:1`; delivery ON `min 1 max 1 mean
  1.000`, hist `1:90`). **`pendingAtDrainEntry = 0` and `flushIterationsConsumed = 0 of 8` on BOTH
  legs** — `DrainTail` alone had already resolved everything and the blocking flushes were **never
  entered**. ⇒ **branch `B1`; shape (c) needs NO lifecycle change.** **The 10 and the 12 were two
  BUDGETS, neither ever a measurement** — the real figure is 1, a 5× margin to the smaller.
  ✅ **Free confirmation from the artifact rather than from source: the delivery-ON session's
  non-image file set is `annotation.json` + `run_summary.json` and NOTHING ELSE.**
  🚨 **`M-2` ANSWERED — BRANCH `C2`, AND IT IS A CORRECTNESS DISQUALIFICATION, NOT A COST ONE.**
  `RQT_Occlusion` **is** a hardware pixel counter (*"the number of samples that are not culled"*,
  `RHIDefinitions.h:1077`) — **but UE's batcher rasterises the primitive's BOUNDING BOX**
  (`BatchPrimitive(BoundsOrigin, BoundsBoxExtent)`, `GCubeIndexBuffer`, 8 verts / 12 tris,
  `SceneOcclusion.cpp:485-500,680,694`). **That is the very quantity `H5` names as the lie:** for the
  foliage container it would return a huge count while ~6 % of the frame is drawn — **it would ACTIVELY
  CONFIRM THE FALSE CLAIM.** ⇒ **a COUNT suffices for the veto, but the cheap hardware path cannot
  supply a trustworthy one. THE MASK STAYS.** ⛔ **Its cost was DELIBERATELY NOT MEASURED** — a path
  disqualified on correctness is not made more disqualified by a millisecond number. ⇒ **T-4's
  256-entry array was NOT applied** (its precondition was *"if you touch the reduction"*, and M-2
  never needed it).
  📌 **THE ONE DEFINITION = `AnomalyViewport::IsRenderableComponent`** — the **already-locked**
  `G33`/`P6` ruling, so adopting it is **not** a fourth definition. **Masking can adopt it TODAY**
  (public `ANOMALYINJECTOR_API`; `AnomalyCapture` already depends on `AnomalyInjector`) and that alone
  **removes the `UPoseableMeshComponent` narrowing**. ⚠ **Labelling CANNOT — that is the
  locked-but-unimplemented `P6` bounds ruling, so the disagreement REMAINS, recorded.**
  🚨 **REQUIRED, AND IT NEEDS NO NEW FIELD: *never measured* ≠ *measured zero*.** A target selectable
  but never tagged reads mask 0 and would be **invalidated despite drawing perfectly** — under (c) that
  **silently deletes a good event**. **`mask.provided` false/true IS the two states**: invalidate ONLY
  when `provided == true`; `false` ⇒ **ADMIT**. Internally a **tri-state**
  (`NOT_MEASURED`/`MEASURED_ZERO`/`MEASURED_NONZERO`), and **the two zeros must never share a
  representation.** ⛔ **Stated as required, NOT implemented.**
  ⚠ **ACCEPTED LIMITS OF (c), recorded now:** the **frames are already on disk** — (c) removes the
  EVENT, it does not un-write PNGs; **dropped events must be COUNTED** (a `run_summary` counter, which
  does **not** move `P6`); and **`labels.jsonl` (delivery OFF) is prebuilt and uncorrectable, so
  delivery OFF and ON WILL DISAGREE — acceptable only because it is stated.**
  🚨 **PART THIRTEEN'S THREE DECIDING FACTS:**
  **(1) SELECTION → FIRE IS *ZERO FRAMES*.** `BeginFire` → `TryFireOnce` does
  `GetVisibleRenderableActors` → the seeded pick → `ApplyAnomaly` (which **hides the actor
  immediately**) **in ONE synchronous call inside ONE tick.** The longest gap anywhere in the burst
  cycle is **6 frames** (`SettleAfterRevert` 2 + `PostGap` 4, and PostGap frames are *captured*);
  `LeadIn` is **4** and runs **once per RUN, not per burst**. ⇒ **a 12-frame pre-flight DOES NOT FIT
  and would have to be CREATED, changing when anomalies fire.** ⚠ **AND IT WOULD BREAK SEEDED
  REPRODUCIBILITY** — `R-SEED`'s draw protocol is deliberately independent of apply-result, and `m22`
  gated on *"seed 4242, two runs byte-identical"*; a reject-and-re-pick makes the **draw count depend
  on a render-thread result.**
  **(2) `annotation.json` IS STILL OPEN AT `FinishRun`** — written **once** (`:1472`) from the
  in-memory `Async->SessionEvents` accumulator, **after** `DrainAsyncToCompletion()`'s **8 iterations
  each with a blocking `FlushRenderingCommands()`**. `labels.jsonl` is **not** re-openable
  (`Job.Record` is prebuilt at `:1013`). 🚨 **AND `Job.bWriteLabels = !bDeliveryMode` ⇒ IN DELIVERY
  MODE `labels.jsonl` IS NOT WRITTEN AT ALL, so `annotation.json` is the ONLY label artifact and it is
  entirely deferred** ⇒ **a DEFERRED VETO is viable exactly in the mode the client uses.** ⚠ One
  number does not line up: **`DrainTail` is `max(10, ViewLag+4)` = 10 frames against a 12-frame mask
  budget** — flagged, and reconciling it is a MEASUREMENT, not a source read.
  **(3) SHIPPING HAS NO CAPTURE AT ALL** (`ANOMALY_CAPTURE=0`), while the selector/auto-injector DO
  ship. **`H5` is a LABELLING defect, so where there is no label there is no `H5` defect** ⇒ ✅ **a
  NON-SHIPPING-ONLY CURE LEAVES NO HOLE**, which removes the only motive for putting render deps into
  `AnomalyInjector`. **The fallback on an absent measurement must be *ADMIT* (byte-identical to
  today), never *reject* — but it MUST NOT BE SILENT:** a silent admit in a non-Shipping build
  reproduces today's defect **while looking cured** (`G119`'s diagnostic; `m19`'s "gate on pixels").
  ⚠ **`T-4` — THE REDUCTION IS MOST EXPENSIVE EXACTLY WHERE THE DEFECT LIVES.** The W×H render-thread
  scan is **ANALYTICAL, NOT MEASURED**: ~0.1–0.3 ms all-zero, **~9–28 ms at 100 % tagged** — and the
  100 % case *is* `InstancedFoliageActor`. ✅ **Cheap fix inside the existing shape: tags are <256, so
  a fixed 256-entry array replaces the per-pixel `TMap::FindOrAdd` and makes worst ≈ best.**
  🚩 **AND THE INSTRUMENT FOLLOWS THE SHAPE, not the reverse: a VETO needs only a COUNT** — a hardware
  `RQT_Occlusion` query *is* a pixel counter with an ~8-byte result — **only fixing the label RECT
  needs the mask.** ⛔ A **downsampled** mask destroys the low end, **which is the signal**; a
  **rect-scoped** count fails twice (the foliage rect *is* the frame, and A35/`SM_Ramp2` puts the
  effect outside the rect).
  📌 **`P6`'s FOURTH OBSERVATION — RECORDED, NOT FIXED (ledger §7):** selection checks `IsVisible()`,
  the **label rect is TYPE-ONLY with no `IsVisible()` gate**, and `node.bounds` unions every
  primitive. **Three code paths, three answers to "what is the object?"** 🚨 **A mask would be a
  FOURTH — and a cure that adds one is a defect generator.**
  🆕 **PRODUCTION CODE EXISTS AGAIN — narrowly, and it is LOG-ONLY.** PART FOURTEEN added `M-1`
  readback-latency instrumentation to `AnomalySveCapturer.{h,cpp}` + two `UE_LOG` lines in
  `DrainAsyncToCompletion`. **No behaviour change, no artifact field. `P6` VERIFIED UNCHANGED BY
  MEASUREMENT — 48 fields, 0 added, 0 removed**, against a pre-change banked leg.
  📦 **BUILD IDENTITY MOVED — ONE HALF OF THE QUARTET (`G121` in action): exe `101AFEA4` →
  `1EBA8944`; `utoc 939B9C9B` / `ucas 8A602D4D` / `pak 7CAE22DD` ALL UNCHANGED** (code-only hot-swap,
  no cook). ✅ **`101AFEA4` preserved** at `_binary_baselines\StackOBot.exe.m25-baseline`,
  hash-verified against the staged copy **before** the swap. ✅ **A44 on the STAGED artifact, both
  encodings:** the two new symbols `ascii=0 utf16=1`, alongside pre-existing symbols also at utf16=1
  ⇒ the change reached the package **and the scan is sound, not blind**.
  ⚠ **OPERATIONAL:** disk **19.1 GB free** (was 12.9 — PART TWELVE recovered 6.23 GB); bank
  **154 dirs** (148 → 150 via `RESCUE_P12_*` on owner ruling, manifest-verified, bank now has a
  **`README.md`**; → **154** with the two `P14_M1_*` legs, which bank as **FOUR** dirs because A63
  keeps the `_try1` attempt alongside the accepted alias); exe-side leg dirs **83 → 4**.
  🚨 **THE NAME-SWEEP HAZARD IS NOW CONCRETE:** the bank already held `RESCUE_H4_WSECHO` — a
  **DIFFERENT session** (`…-140533`) from the exe-side `H4_WSECHO` (`…-170238`). **A name-based sweep
  would have matched them and destroyed the only copy while reporting a clean duplicate.** **Match by
  SESSION ID + manifest — `CaptureBench/tools/prune_verify.ps1`.** **Next disk lever = BANK RETENTION,
  the owner's call.**
- 🟦 *(superseded as "you are here", still the LAST TAGGED MILESTONE)* **`S4` IS COMPLETE AND TAGGED
  `m25`. THE DEFAULT GRAB POINT IS THE SVE / SCENE-COLOUR PATH: DELIVERED FRAMES NO LONGER CONTAIN
  GAME UI.**
  ✅ **`S4-3`** — `run_summary.capture_path` is emitted on **BOTH** paths (`"sve"` / `"backbuffer"`), so a
  delivered session states what produced it; an absent field used to be indistinguishable from a pre-S3
  build. `key_ring_*` stay SVE-only. Gate by **control pair**: SVE field set added `[]` removed `[]`;
  backbuffer added **exactly `['capture_path']`**; **`annotation.json` field set UNCHANGED on both paths
  — `P6` NOT MOVED.** ⛔ **RULING: C1's leak-check invariant is FORMALLY RETIRED** — it existed to prove
  S3a was inert when off and has no remaining job. A ruling, not a side effect.
  ✅ **`S4-4`** — delivery orthogonality re-asserted at the new default: **EXTRAS = 0**, **127 invariants
  asserted POSITIVELY, 127 identical**, `capture_path` **invariant across delivery mode**. ⚠ **The first
  pair was INVALID, not FAILED** — non-pose-matched, and the extras were *entirely* the camera-dependent
  fields (A64's exact predicted shape, and journal 042 §4's). The indicator was read BEFORE the
  comparison, so **the comparison did not run**; a pose-matched partner arrived on attempt 3 and every
  attempt was banked.
  ⚠ **CORRECTION TO m24:** `key_ring_published`/`consumed`/`wrapped` are **RUN-UNIQUE, not invariant**
  (measured on a same-binary control pair — they count view families rendered before capture starts).
  Only `missed`/`corrupted` are invariant. **m24's verdicts are undisturbed** (they rest on
  `missed == corrupted == 0`), but its "all five identical across every pair" does not generalise.
  → journal `docs/sessions/2026-08-19-044-s4-instrument-matrix-and-flip.md`.
  **`IAI.Capture.SVE 0` IS THE BISECT SWITCH** — the one setting that reaches the unchanged backbuffer
  path, no rebuild, no re-cook. `bSveCaptureDefault` in `DefaultGame.ini` still overrides; **the shipped
  default deliberately needs NO ini key**, which is what makes it immune to G88. The startup banner now
  states the grab point **and where its default came from**.
  ✅ **S4-1 MATRIX — CASE A ON ALL TEN LEGS.** All four rect sources (PNG-from-IHDR as ground truth,
  `labels.jsonl`, `annotation.video.resolution`, `run.json` viewport) agree at 1280×720, 1280×1024,
  fullscreen 1920×1080, odd 1281×721, non-multiple 1001×721, `r.ScreenPercentage` 50 **and** 170, and
  desktop scale 150 % in two DPI regimes. **42 counted events, 42 ALIGNED, 42/42 decidable** on the six
  judgeable legs. **CASE D never fired.**
  ✅ **S4-2 GATES (a)–(d) ALL PASS.** Default leg with `IAI.Capture.SVE` **absent** from ExecCmds →
  `capture_path: "sve"`, ring 121/121/0. Backbuffer leg contains UI by **positive pixel check**
  (`max(R−B)` **1.00000 vs 0.00392**, separation 0.996 against a control band of 0.004; the pair is
  A63-comparable and pose-matched with **identical** bboxes).
  🚨 **G114 — THE FINDING OF THIS ARC, ABOVE THE RECTS: a packaged UE game is DPI-UNAWARE**, so a
  display-scale change **never reaches the process** and the clean null that produces is an **ARTIFACT
  OF INSULATION**. `GetProcessDpiAwareness` read `PROCESS_DPI_UNAWARE` **with and without** a
  `~ DPIUNAWARE` override ⇒ the override was a **no-op** and two legs were one regime measured twice.
  Fixed with the **opposite** override (`~ HIGHDPIAWARE` → `PER_MONITOR_DPI_AWARE`, verified before the
  leg). **This is G96's principle applied to a LEVER rather than an oracle, and that is worse: a blind
  oracle leaves unevaluable output to notice, while a lever that does nothing produces a clean null
  INDISTINGUISHABLE from a clean result.** Read the LIVE PROCESS, not the cvar default
  (`EnableHighDPIAwareness` defaults to 1 and reads the opposite way).
  ⛔ **B1 CANNOT RUN OFF-CALIBRATION** — `CALIB_BBOX` is frozen **in PIXELS** at 1280×720, so four legs
  were blocked and **three had a provably motionless camera** (M3's per-component ratio is a uniform
  **1.5** = the resolution ratio). **RULED: accept and scope; B1 is UNTOUCHED.** Alignment is certified
  at **1280×720 and 1281×721 only**, and that gap is **INHERITED — unverifiable on BOTH grab points**,
  not introduced by S4. NDC-normalising `CALIB_BBOX` is **filed alongside `B2`**; per-resolution
  constants **REJECTED** (same failure family as G107/P8).
  🆕 **A gate that FAILS SAFE still misleads if its LABEL names a cause it has not established.** The
  harness called those four failures *"INVALID (A47 bifurcated pose)"* then *"an ENVIRONMENTAL problem"* —
  neither true. The causal attribution is **DELETED**; the label is now **"POSE GATE FAILED (B1) — CAUSE
  NOT ESTABLISHED"**; `check_pose.py` prints the per-component ratio and the discriminator, **reporting
  only**. Proven on both causes: resolution scope = uniform ratio + camera still; genuine A47 = width
  ratio **0.055** + rotation displaced.
  🔬 **The backbuffer leg reproduces I10-L1's startup marker noise** (indices 0–4, diffs
  `+1,+2,+3,+4,+7`) **and the SVE leg shows zero** — on a **same-binary** pair, which removes the confound
  journal 042 §10.1 flagged. ⛔ **Still NOT a mechanism claim about B′; n = 1 on that axis.**
  🚨 **`m25` TAG SCOPE, when it is cut:** *"Label alignment on the default (SVE) path is certified at
  1280×720 and 1281×721 only. Rect equivalence is certified across ten legs including 1280×1024,
  fullscreen 1920×1080, SP50, SP170, and a VERIFIED DPI-AWARE process at 150 % including a non-multiple
  1001×721 rect. Alignment at any other resolution is UNVERIFIED ON BOTH GRAB POINTS and is blocked by
  B1's pixel calibration, not by the grab point."* Plus m24's carried limits (modal pose only; VideoFps
  30 pinned; P7; **S3/S4 going green does not close `P1`**).
  📦 **ENVIRONMENT:** staged exe **`101AFEA4`** = `m25`. Baselines beside it: `.s4-2-baseline`
  `259BF64F`, `.s4-0-baseline` `834BB30A` (the binary all ten S4-1 legs ran on), `.m24-baseline`
  `3BA854FB`, `.m23-baseline` `85A39CFB`.
  ⚠ **`m25` CARRIES A CORRECTION TO `m24`, because a tag object cannot be amended (no force-push,
  fix-forward).** A reader arriving at m24 via its tag is routed forward to journal 044 §6.5:
  *"of the five `key_ring_*` counters, only `missed` and `corrupted` are invariant across runs.
  `published`/`consumed`/`wrapped` are RUN-UNIQUE — they count view families rendered before capture
  begins. m24's verdicts are UNDISTURBED; they rest on `missed == corrupted == 0` and on the file set,
  not on the publish count. The claim as written did not generalise."* Annotated in place in journal 042
  §1 as well. **Second instance of a certified claim resting on PROSE rather than a MEASUREMENT** —
  G106 was the first. **The control pair is authoritative over the prose.**
  ⛔ **`annotation.json`'s field set is UNCHANGED across the whole of S4 and `P6` is NOT MOVED.**
  Content clock untouched, still `wall`, key still unset. **`S4` DOES NOT CLOSE `P1` — it never could.**
  **Nothing in S4 is outstanding.**
  🧭 **FORWARD QUEUE, in order:** **`H4`** (occlusion-blind labelling) → **`P5`/`P7` — the
  blend-ladder** (serves two open items; discriminators pre-declared chat-side BEFORE it is built) →
  **`P1`'s `H1` lever** (GPU-load starvation shape — **P1's ONLY named lead, and it has NO LEVER IN
  EXISTENCE**; design **chat-side first, NEVER same-turn as its first measurement**; if H1 also comes
  back clean, **P1 HAS NO NAMED LEADS**).
  🟢 **`H4` RAN 2026-08-19 (after its brief was amended chat-side) AND THE RESULT IS
  `H4-SUPPORTED` — THE MECHANISM IS OBSERVED, PATH (b), 8/8 EVENTS.** → journal
  `docs/sessions/2026-08-19-045-h4-cook-and-h5-mainworld-arc.md` (**PART TWO** is the run; part one is
  the pre-flight that stopped the *original* brief; **the file has a PART INDEX at the top**).
  **STILL ZERO PLUGIN PRODUCTION CODE. NO TAG.**
  **THE OBSERVATION:** on a target proven fully occluded (`StaticMeshActor_100`; `StaticMeshActor_86`
  blocks all 9 rays alone, 0 rays floor-only), the label path emitted **`bbox_valid: true` on 59/59
  rows**, `visible_positive` on 59 rows, `coverage_ratio 0.01160339 > 0`, and **8 annotation events all
  `manifested: true`** — while `selection_provenance` read **`valid:false`, `0/0`, `coverage_pct −1`**
  on all 8. **That pairing — provenance `valid:false` WITH `bbox_valid: true` at the same anchor — is
  the C4 divergence, and it had NEVER been seen in 780 banked records. It occurred 8/8.**
  **THE RAW SERIES (Ruling 3a — a SERIES, NOT AN A54 VERDICT; A54 is out of scope on this target,
  G117):** in-bbox luminance moves by at most **2.0 × 10⁻⁴** with claimed/flank ranges **OVERLAPPING on
  7 of 8 events**, against the control's **0.1023–0.1116** on the same binary and pose — a factor of
  ~**500**, with inconsistent sign. **Labelled positive, boxed, `coverage_ratio > 0`, and changing
  essentially nothing inside that box.**
  ✅ **CONTROL (`StaticMeshActor_49`, same binary/geometry/schedule, adjacent launch):** provenance
  **9/9 `valid:true`** ×8, and A54 — **in scope there** — returns **7/7 ALIGNED, 7/7 decidable, median
  margin 0.10527**, with the positive control **decisive in BOTH directions** (`+1` → 7/7 SHIFTED,
  `−1` → 8/8 SHIFTED). `modal_rot` **identical** across the two legs (delta 0.0, tol 0.5) —
  **reported as a DISCRIMINATOR, never a gate.**
  ⛔ **SUPPORTED, NOT CONFIRMED** — the word was chosen before the run, because the A54 leg of the
  original signature is structurally unobtainable (G117); this rests on the provenance divergence plus
  a raw series, not on an oracle verdict. ⛔ **NO INCIDENCE CLAIM** — path (b) only; path (a)
  (selected while visible, becomes occluded during the window) **cannot be produced in `CB_GateLevel`**
  (all targets STATIC, eye invariant on 844/844) and remains unbuilt. ⛔ **n = 1 leg** on the occluded
  side; 8 events in one leg are not 8 legs. **Routes to `feature/stencil-capture` as its cure —
  that branch is NOT touched and NOT rebased.**
  ⚠ **CLAUSE CHAIN, and one link is NOT from the artifact:** C1 excluded by the artifact (every anchor
  is absent from its event's hidden set); **C2 (poll radius) is NOT decidable from the artifact** — the
  projector has no radius test and `poll_distance` is the −1 sentinel — so it is excluded by the
  certified offline model (`_100` at 1031.9 cm, verified to **8e-6** against `_49`'s banked
  418.09228516) **plus the live `pollRadius: 1800` read-back**; C3 excluded by `bbox_valid: true`;
  **C4 is the remainder**, and is independently predicted from the geometry.
  ✅ **A48 — BOTH ECHOES OBTAINED AND THEY AGREE.** Primary (behavioural, banked per leg):
  `blinking: matched 1 actor(s) for '=StaticMeshActor_100'` ×8 — `matched 0` **is** the scoping-ON
  signature, so `matched 1` on an occluded target proves the occlusion-aware path did not run.
  Corroborating (live WS `ControlSnapshot`): `viewportScoping: False`, `pollRadius: 1800`,
  `minScreenCoverage: 6`. **Ruling 2 honoured: NO `IAI.SetPollRadius`, shipping defaults throughout;
  recorded that the 1800 cm radius is what ADMITS `_100` and is why `_11` was rejected.**
  🔬 **P6's FIRST OBSERVATION:** `coverage_pct −1` alongside `coverage_ratio 0.01160339` on all 8
  events. **It is −1, NOT 0** — journal 036 §3.5 and the original H4 filing both said 0; the source
  said −1 in pre-flight and the run confirms it. ⛔ **`P6` DOES NOT MOVE — no `annotation.json` field
  added, removed, renamed or recomputed.**
  🌒 **SHADOW, resolved by WORDING not by target (Ruling 5), and it travels with the result:** no
  candidate is clean on both poll radius and shadow. **H4 concerns whether the PROJECTOR boxes a target
  contributing no pixels WITHIN ITS OWN BBOX.** `_100` contributes none inside its bbox and does throw
  shadow outside it (2 of 9 patch samples visible); A54 keys strictly inside the bbox (A35), so the
  shadow is outside the claim's scope **by construction** — and outside the client's complaint, which
  is a labelled box around nothing. The measured ~2e-4 in-bbox excursion is what A35 predicts.
  📌 **THE CLAIM, FIXED IN THESE WORDS (record verbatim; this is what travels):** *"H4 is SUPPORTED as
  a mechanism, path (b), n=1 leg / 8 events. The label path emits a positive label, a valid projected
  bbox, `coverage_ratio > 0` and `manifested:true` for a target that is fully occluded and changes
  essentially nothing inside its own bbox (~2e-4 against a control's ~0.10, factor ~500, ranges
  overlapping on 7 of 8 events with inconsistent sign). The C4 divergence — provenance `valid:false` +
  `0/0` WITH `bbox_valid` TRUE — occurred 8/8 and had never been observed in 780+ banked records. NOT
  CONFIRMED: the A54 leg of the original signature is structurally unobtainable (G117), so this rests on
  the provenance divergence plus a raw series, not an oracle verdict. NO INCIDENCE CLAIM. This used
  TARGETED fire, which bypasses `IsUnoccluded`. The shipped auto-pool screens occlusion AT PICK TIME.
  Whether the shipped path is exposed is a SEPARATE, UNASKED QUESTION. Path (a) — visible at pick,
  occluded during the window — is unbuilt and cannot be produced in `CB_GateLevel` (all targets STATIC,
  eye invariant 844/844)."* **`H4`: NAMED, NOT ADOPTED → SUPPORTED (path b), MECHANISM ONLY. It does
  NOT become a phenomenon number.**
  🧭 **THE OPEN QUESTION, NOW PRECISELY STATED — and it is the one that decides whether this reaches a
  client dataset: THE SHIPPED AUTO-POOL SCREENS OCCLUSION AT PICK TIME AND NEVER RE-CHECKS.** A target
  selected while visible that becomes occluded mid-window would be labelled by the same occlusion-blind
  projector, **with no manual targeting involved**. That is **path (a)**, and it is the only path that
  can reach a delivered dataset. ⛔ **NOT DESIGNED, NOT RUN — its design is chat-side first and it needs
  a level with motion, which `CB_GateLevel` is not.** ⛔ **THE CURE (`feature/stencil-capture`) IS NOT
  BUILT BEFORE THE INCIDENCE QUESTION IS ANSWERED** — the D-B lesson: the SVE migration would not have
  cured P3, and building it first would have shipped a migration while the poisoning continued.
  🔎 **RECON DONE (read-only, journal 045 §19). THREE FINDINGS:**
  **(1) NOTHING re-validates a live target after the fire begins — not occlusion, renderability, screen
  coverage or distance.** `AdvanceTime` → `ServiceReverts` is a pure countdown; `Blinking::Tick`
  iterates a `Targets` array resolved ONCE in `Apply` and never re-resolved (`Weak.Get()` is *object
  validity*, not visibility); `MissingObject` has **no `Tick` at all**; and both per-frame label writers
  call **only** `ProjectActorBoundsToScreenRect`. **The single occlusion evaluation downstream of pick
  time is `EvaluateSelectionProvenance` — it runs ONCE at the anchor frame, its RETURN VALUE IS
  DISCARDED, and its sidecar is SUPPRESSED IN DELIVERY MODE. So in the mode the client receives there is
  no occlusion evaluation after pick time at all. Path (a) is NOT closed by any existing re-check.**
  **(2) The pick-time guard is weaker than it looks:** 9 rays, **camera** origin, targets = bounds
  centre + the **8 AABB corners** (not the mesh), `ECC_Visibility`, **`bTraceComplex=false`** (simple
  collision only), owner ignored, and it **returns true on the FIRST CLEAR RAY** — so **1 of 9 clear ⇒
  "unoccluded"**, while this project's bar for "occluded" is **9 of 9 blocked**. The two are **not
  complements**; there is a wide band between them. AABB corners lie **outside** a sphere/cylinder/cone
  (3 of the gate level's 4 shapes), a `NoCollision` or non-`ECC_Visibility` occluder does not occlude at
  all, and **there is no pixel test anywhere**. ⚠ **Source reading, NOT measurement.**
  **(3) Candidate environments NAMED.** *(Superseded by the scout below — see §23-§26.)*
  🛰 **ENVIRONMENT SCOUT DONE (read-only, journal 045 §21-§28). PATH (a) IS STRUCTURALLY OPEN AND
  ENVIRONMENTALLY BLOCKED.**
  🚨 **S-1 — G87's MECHANISM IS WRONG, AND IT FORECLOSED A ROUTE THAT WAS NEVER CLOSED → G120.**
  **`MainWorld` IS NOT COOKED INTO ANY STAGED BUILD.** Read directly from each
  `StackOBot-Windows.utoc`: **BenchGate (`101AFEA4`) = `CB_GateLevel`, `Entry`, `MainMenu`; MidRepro
  (`3814E080`) and `Builds\Windows` (`B3A49D82`) = `Entry`, `MainMenu`.** No `MainWorld`, no
  `Struct_00*`, anywhere. `Builds\Windows`' exe is dated the day G87 was written. ⇒ `LoadMap MainWorld`
  **fails because the map is not in the pak** and the engine falls back to `GameDefaultMap` — **engine
  fallback, NOT an in-game redirect.** Source search confirms: the **only** `OpenLevel` in the framework
  is in **`HUD_MainMenu`** and travels **MainMenu → MainWorld**; `GI_StackOBot` is a **save-game
  manager** (its `LevelName` is a save-slot key); `GM_InGame` has only `ReceiveBeginPlay`;
  `MainWorld_C` has no travel strings. **G87's headline rule ("check the level NAME, not the picture")
  is UNAFFECTED and still correct** — only its explanation was wrong. ⚠ **CORRECTED BUT NOT
  RE-MEASURED** (no packaged run was in scope); the settlement is one launch with the log read for the
  missing-map browse error. ⛔ **The ONLY route to MainWorld is A RE-COOK** — and **it is THE SAME
  RE-COOK `G118` CLOSURE NEEDS. Two debts collapse into one operation; both retire `101AFEA4`.**
  🛑 **S-2 — NO LEVEL IN ANY STAGED BUILD HAS UNATTENDED MOTION THAT COULD CHANGE OCCLUSION STATE.**
  `CB_GateLevel`: authored `STATIC` throughout, no Blueprints, eye invariant 844/844 — **none**.
  `MainMenu`: 1 `SkeletalMeshActor` + `ABP_Bot`, `Landscape`, 2 `StaticMeshActor`, `BP_Cable`,
  `BP_Spline`, **3 `CameraActor` with nothing driving them**, **no `LevelSequence`/Matinee/auto-play/
  movement component** — the only motion is **skeletal animation inside one component**, not an actor
  translating, so it cannot put an occluder between camera and target. `Struct_002`/`Struct_003`
  external-actor sets (814 / 389 files): **zero Blueprint instances** — geometry only.
  ✅ **`MainWorld` WOULD provide it, and the assets say so in their own words:** **`BP_Stomper`** —
  *"When no trigger is referenced it move constantly"*; **`BP_MovingPlatform`** —
  `InterpToMovementComponent` with **`EInterpToBehaviourType::PingPong`**, trigger **optional**;
  **`BP_Fan`** — `AddLocalRotation` + `SetTimer` + `bLooping` (its overlap pushes the Bot, it does not
  start the fan); **`BP_EnergyOrb`** — *"Add a bit of rotation per tick"*. Player-gated by contrast:
  `BP_Elevator`, `BP_Button`, `BP_PressurePlate`, `BP_SpawnPad`. ⚠ **Per-instance `Trigger` assignment
  and exact instance counts are NOT established** — the constant-motion cases are conditional on an
  empty Trigger, which was not read.
  🧱 **S-3 — of the three occluder shapes, NONE is available today.** (i) moving occluder crossing a
  static target and (ii) moving target behind a static occluder both need `MainWorld`'s
  `BP_MovingPlatform` / `BP_Stomper`; (iii) a moving camera has **no driver in any cooked level** and
  an unattended run has no input.
  🗺 **S-4 SETTLED — `MainWorld` is WORLD PARTITION with ONE-FILE-PER-ACTOR**, not a classic streaming
  shell: **419 external actor files** under `__ExternalActors__/StackOBot/Maps/MainWorld/`, which is why
  the `.umap` is 21 KB. *(PART THREE's "streams the Structures/ levels" guess was the right clue and the
  wrong inference; `Struct_001-004` are partitioned companions with their own external-actor trees.)*
  **Design-relevant: World Partition streams by distance, so streamed-in geometry is ITSELF a candidate
  occluder and a capture run must survive that load timing.** ⛔ Cell size / runtime grid / whether
  streaming is active in a packaged build were **NOT evaluated**.
  ⛔ **MCP BRIDGE UNAVAILABLE** (`Connection refused 127.0.0.1:12029`) — **A59 satisfied vacuously: no
  measurement was taken over it, so none is attributed and G97 has nothing to catch.** Everything above
  is offline asset/container reading; the Blueprint evidence is **name-table strings plus the developer
  comments embedded in them** (the quotes are direct; the machinery lists are indicative, not a graph
  read).
  🔬 **CURE MEASUREMENT DONE (journal 045 PART ELEVEN). 7 targets marker-off, 5 GOOD / 2 BAD. n small,
  nothing graded, NO CURE PROPOSED. ZERO production code. NO TAG.**
  🚨 **Q2's ANSWER: THE CURE NEEDS A NEW MEASUREMENT, NOT A NEW THRESHOLD.** Every field the artifact
  carries — `cov_pct`, `coverage_ratio`, `bbox_px`, `poll_distance`, `node.bounds` — describes **CLAIMED
  extent. NONE describes DRAWN extent.** The one plausible proxy, `cov_pct`, **does not separate at
  all**: BAD holds both the **highest** value in the table (**100.00**) and nearly the **lowest**
  (**3.86**). ⛔ **And the SELECTION BOUNDS EXTENT IS NOT RECOVERABLE FROM ANY ARTIFACT** — `node.bounds`
  is the whole actor (`P6`) and `poll_distance` is one equation in two unknowns, so **the quantity every
  guard is computed from cannot be read back.**
  **Q1: the only clean separator is POST-hide** — change ÷ claimed area, BAD {0.00603, 0.00838} vs GOOD
  {0.0303 … 0.498}, 3.6× gap, no overlap. ⛔ **A cure decides BEFORE hiding, so it is not a rule.**
  ⚠ **One PRE-hide quantity DID separate here — the OCCLUSION SAMPLE COUNT (BAD ≤ 3/9, GOOD ≥ 5/9).**
  n=2 vs n=5 on a 9-valued integer, two GOOD one step from the boundary, mechanism post-hoc.
  ⛔ **OBSERVATION, NOT A PROPOSED THRESHOLD.** *(It is the `P-a1` band again; `P-a1`…`P-a5` UNTESTED.)*
  **Q3, measured cost of naive rules:** `poll_distance<0` → 2/2 BAD but **breaks 2/5 GOOD**;
  `cov_pct>90` → 1/2 BAD, 0 GOOD *(misses `BP_SplineSpawn_C`)*; `cov_pct>3` → 2/2 BAD, **breaks 4/5
  GOOD**; **blacklist ISM/HISM/Foliage → 2/2 BAD but breaks `RoomBuilderSquare_C`**; bounds÷rect
  **not computable**. ⚠ **THE STRAWMAN FAILED DIFFERENTLY THAN PREDICTED** — `BP_SpawnPad_C` is **GOOD**,
  so not rejecting it is correct behaviour; the blacklist's real cost is a **legitimate** target with a
  **75:1** in/out ratio.
  🆕 **A SECOND CLASS-(ii) INSTANCE, AND IT IS NOT FOLIAGE: `BP_SplineSpawn_C`** claims **22.9 %** of the
  frame and changes essentially nothing anywhere (peak-in **0.0175** vs the control's **0.5515**, 31×).
  ⚠ **I had EXCLUDED it last turn on the inference "it clearly draws", from its RECT SIZE — which is
  CLAIMED extent, the very quantity H5 says is untrustworthy. Measurement overturned my call.**
  🔧 **A DEFECT IN MY OWN HARNESS, found because the table tool SILENTLY SWALLOWED a leg:** `CM_SPLINE`
  had **7 ZERO-BYTE PNGs** (highest indices) while `total_frames` read 76, files numbered 88,
  `key_ring` read **121/121/0/0** and **nothing was logged** — every counter clean, the artifact
  internally inconsistent. **CAUSE: `run_leg.ps1` killed the process the instant `run_summary.json`
  appeared, truncating the async writer's tail flush.** ✅ **Proven both ways** — with a flush-wait the
  re-run gave **90 frames / 0 zero-byte / total_frames 90**. **Harness, not product.** ⚠ **Product-side
  OBSERVATION, narrow: an interrupted run leaves counters disagreeing with files, undisclosed** — the
  `GetDropped()` warning cannot fire because the writer never counts them. **m19's lesson recurring:
  gate on PIXELS, not on a counter.** Both fixes are harness-side; the tool now **reports every skipped
  leg with its reason** (the silent skip is what hid the corrupt frames).
  📐 **`SM_Ramp2` is an A35 case on the record: peak OUT 0.2955 > peak IN 0.1785, marker OFF** — the
  largest change from hiding it is **outside its own bbox**. Any in-bbox-only rule scores this
  legitimate target low.
  📒 **THE MECHANISM LEDGER IS `docs/invisible-anomaly-mechanisms.md` — READ IT FIRST.** Five rows
  (`m23`/`P3` FIXED · `H4` SUPPORTED · `H5`(ii) SUPPORTED · `H5`(i) ENUMERATED-NOT-OBSERVED ·
  **traceability degradation, marked NOT A CAUSE**), each stating **MEASURED vs SOURCE-READ**, evidence,
  limits and cure. **These are DISTINCT mechanisms with potentially DISTINCT CURES and NO SINGLE FIX IS
  KNOWN TO ADDRESS ALL OF THEM** — `feature/stencil-capture` is **H4's** cure and has not been shown to
  cure H5.
  🚨 **CORRECTION TO PART NINE — A HARNESS MARKER CONTAMINATED THE PUBLISHED FOLIAGE GRID (journal 045
  §69 → `G125`).** *"Four cells carry the change (0.1800, 0.1510, 0.1228, 0.0860) — the peak exceeds the
  control's whole-bbox score"* — **the first two cells were the `CaptureBench` frame-identity marker,
  which changes every frame BY CONSTRUCTION.** Caught by running a second leg and seeing the SAME two
  cells within 0.006 across **three levels and three targets** (foliage 0.1800/0.1510 · spawn pad
  0.1797/0.1528 · **CB_GateLevel control 0.1808/0.1570**). **Fixed AT SOURCE (`-Marker 0`), not masked
  in the reader.** Corrected: whole-frame mean **0.0069 → 0.0059**, grid peak **0.1800 → 0.1242**, top
  row **→ 0.0018–0.0072**. ⛔ **The "peak exceeds the control" sentence is WITHDRAWN.** ✅ **The class
  (ii) finding STRENGTHENED — the real change is smaller and more concentrated.**
  🧾 **OWNER OBSERVATION 2 (eyeball-level, NOT measured):** *"`asset_name`/`component_class` show
  properly in the EDITOR but in BUILDS it shows `StaticMeshActor_xxx` for most objects."*
  ✅ **CHARACTERISED FROM BANKED DATA — 1,267 node entries, 109 packaged legs — AND IT IS NOT A FIELD
  DEGRADATION.** `asset_name` **15/15 populated**, `component_class` **15/15 populated**, **ZERO empty**.
  ⛔ **Nothing in `ResolveNodeIdentity` is `WITH_EDITOR`-guarded; there is NO editor/build branch.**
  **What degrades is `node.name` alone** — it is `AActor::GetName()` (the internal object name,
  **identical** in editor and build), and its quality depends on **HOW THE ACTOR WAS AUTHORED**:
  `CB_GateLevel`'s actors were script-spawned with only `set_actor_label()`, so `GetName()` stays
  **`StaticMeshActor_<n>`** while the label reads `CB_Target_NN`; MainWorld's editor-placed and BP
  actors already carry meaningful names. **The editor DISPLAYS the LABEL, which does not exist in a
  cooked build** — measured this session: `IAI.ListActors` printed **`(no-label)` for all 432**
  MainWorld actors. ⇒ **The owner is comparing the editor's LABEL against the build's `GetName()`.**
  📌 **CONSEQUENCE FOR CLIENT DATA: PARTIALLY YES** — the remaining invisible cases **CAN** be attributed
  to a culprit **class** today, **via `asset_name` + `component_class`**, but **NOT from `node.name`
  alone**. **CANNOT** be done: distinguish two instances of the same class, or tell which predicate
  admitted an actor. ⚠ **AND `BP_SpawnPad_C` REPORTS TWO DIFFERENT `asset_name`s ACROSS LEGS** (`Plane`
  / `SM_SpawnPad_Base`) — `ResolveNodeIdentity` takes the **first VISIBLE** mesh component and that BP
  toggles visibility, so **the identity fields are NON-DETERMINISTIC** for such actors. ⛔ **Recorded,
  not fixed. `P6` DOES NOT MOVE.**
  🛑 **OWNER RULING: `InstancedFoliageActor` DROPPED FROM THE INVESTIGATION. ⚠ SCOPE IS NARROW —
  applies to the INVESTIGATION only, NOT to selection and NOT to any cure. NOTHING is excluded from the
  selector and A CLASS BLACKLIST IS NOT ADOPTED AS A FIX.**
  🚨 **`G124` GENERALISES, MEASURED: 3 of 13 NON-FOLIAGE selectable actors carry a NEGATIVE
  `poll_distance`** — **`BP_SpawnPad_C` −114.8 (a PLAIN `StaticMeshComponent`)**, `BP_SplineSpawn_C`
  **−19405.5**, `RoomBuilderSquare_C` **−1737.8**. Negative ⇒ the bounds sphere already contains the poll
  origin ⇒ **the 1800 cm cull can NEVER fire from anywhere in the level.** ⛔ **The mechanism is
  OVERSIZED BOUNDS, not aggregation — a blacklist of instanced/foliage types would MISS `BP_SpawnPad_C`
  while appearing to close the hole.** ⚠ `poll_distance == −1` is the **sentinel**, i.e. UNMEASURED, not
  small.
  ⚠ **`H5` CLASS (i): BRANCH *SELECTED BUT MANIFESTS*.** Candidates were enumerated with reasons and
  pre-registered before firing; the strongest (`BP_SpawnPad_C` — `SetVisibility` in the asset **and** a
  banked two-asset identity split) **DRAWS, and its label points at it correctly** (in-bbox Δ −0.0355…
  −0.0367; the two brightest cells in the frame, **0.1861 / 0.1920, are INSIDE the bbox**). ⛔ **Why the
  prediction was wrong: I read "has a toggled component" as "the toggled component is the SELECTED one",
  and those are different claims.** ⇒ **`H5` (i) remains ENUMERATED, NOT OBSERVED.** ⚠ **StackOBot is a
  polished sample and may not contain the pattern — a property of THIS PROJECT, not evidence against
  class (i) in the client's game.**
  🚨 **NEW PRIMARY LEAD — `H5` IS MINTED AND CLASS (ii) IS SUPPORTED, REPRODUCED HERE (journal 045
  PART NINE, §62-§68). ZERO production code. NO TAG.**
  🧾 **OWNER OBSERVATION (real evidence, EYEBALL-LEVEL, NOT MEASURED — do not upgrade, do not
  discard):** the client's invisible anomalies were **NOT partially hidden — they could not be found at
  all**; two culprit classes named in her data, **`InstancedMeshActor`** and **`BP_LocalVolumetricFog`**;
  and the rest **could not be attributed because every actor reads `StaticMeshActor_xxx`.**
  ⛔ **A CHAT-SIDE RULING WAS WITHDRAWN: *"partial occlusion is the right target"*** — it rested on what
  our levels can produce, not on the client's data. **The Part Eight "no crossing pair" answer and
  `H4 SUPPORTED (path b)` both STAND.** ⚠ **PATH (a) IS PARKED, NOT REFUTED — a PRIORITY decision, not
  a SCOPE one (G120). Re-opening it needs a decision, not an argument.**
  🆕 **`H5` — THE SELECTOR ADMITS OBJECTS THAT CANNOT MANIFEST A VISIBLE HIDE.** Number verified free
  first: `H1`/`H3`/`H4` minted, **`H2` RETIRED-UNKNOWN**, no `H5`–`H9` anywhere. *Selection requires a
  renderable component — **a TYPE test, not a DRAWING test**. An actor can satisfy it while contributing
  no pixels, or pixels nowhere near where its label says.* **(i) non-drawing mesh component**
  (`BP_LocalVolumetricFog`, **not reproducible here** — the client runs her own game; **what IS ours is
  the FILTER that admitted it**); **(ii) aggregate/instanced actor** (`InstancedMeshActor`,
  **reproducible here**). ⛔ **H4 is a target that WOULD draw but is BLOCKED; H5 is a target that WOULD
  NOT DRAW ANYWAY. Same symptom, different mechanism, different cure. TWO ITEMS.**
  🔎 **TASK 1 — THE FILTER, SOURCE READING NOT MEASUREMENT.** `IsRenderableComponent` checks exactly
  **two** things: `Component->IsVisible()` and, for ISM, `GetInstanceCount() > 0`. ⛔ **ABSENT:** the
  **owner actor's `bHidden`** (`IsVisible()` does not consult it — `ShouldRender()` would, and is not
  used) · `bRenderInMainPass` · `GetStaticMesh() != nullptr` · section/triangle count · material
  presence · `WasRecentlyRendered()` · **any distinction between ISM/HISM and a plain SMC** (they
  DERIVE from `UStaticMeshComponent`, so they pass trivially). **Every companion predicate — frustum,
  poll radius, occlusion, coverage — is computed on BOUNDS. Not one reads a pixel, a material or a draw
  call.** Routes a fog-shaped actor could pass by: not-in-main-pass · null mesh · nothing-rendering
  material · owner-hidden · an SMC used as an editor gizmo. **Any one suffices.**
  ✅ **TASK 2 — CLASS (ii) REPRODUCED. Branches pre-registered as a FILE before the result was read.**
  `InstancedFoliageActor_0_0_0`, MainWorld, delivery OFF, B1 **NOT APPLICABLE (declared)**, **no A54
  verdict** (G117), raw series only. Selected and fired (`matched 1 actor(s)` → applied), 8 events.
  **THE LABEL: `bbox_px (0,0,1280,720)` on 59/59 rows — THE ENTIRE FRAME · `coverage_ratio 1.00000000`
  · `coverage_pct 100` · `manifested true` · `component_class FoliageInstancedStaticMeshComponent` ·
  `asset SM_Bush` · `global_position [12800,12800,12800]` (a cell corner) · bounds 252 m × 217 m × 67 m.**
  **THE PIXELS: mean |Δ| ≈ 0.0069 whole-frame**, against the `CB_GateLevel` control hide's
  **0.1023–0.1116** and H4's occluded **≤2.0e-4** ⇒ **~6 % of a proper hide.** **THE GRID (8×8) is the
  discriminator: FOUR cells carry it (0.1800, 0.1510, 0.1228, 0.0860) — the peak EXCEEDS the control's
  whole-bbox score — and the other 60 are flat.** ⇒ **the label claims 100 % of the frame while the
  change lives in ~6 % of it.** ⚠ **The predicted "change outside the bbox" test DEGENERATED — a
  full-frame bbox has no outside — and the spatial grid answered instead.** ⛔ **MECHANISM ONLY, NO
  INCIDENCE CLAIM;** the owner's `InstancedMeshActor` is not this actor.
  🆕 **`G124` — AN AGGREGATE'S BOUNDS DEFEAT EVERY BOUNDS-BASED GUARD AT ONCE.** `poll_distance`
  **−5396.0 (NEGATIVE** — the bounds sphere exceeds the distance, so the 1800 cm cull can never fire
  **from anywhere in the level**) · `coverage_pct 100` against a 6 % floor · `IsUnoccluded` **1/9, the
  exact minimum** · label rect the whole frame. ⚠ **The guards LOOK independent and are not — they are
  four readings of ONE number.**
  🔬 **`samples 1/9` is `P-a1`'s exact boundary in live data — CORROBORATION, NOT A TEST** (provenance
  is computed at the anchor, *after* selection, so it cannot speak to passing PICK TIME). ⛔ **`P-a1`…
  `P-a5` REMAIN UNTESTED; none marked touched.**
  📐 **`P6`'s SECOND OBSERVATION (record only):** `nodes[].bounds` is
  `GetComponentsBoundingBox(**true**)` — the whole actor — and `nodes[].global_position` is the actor
  origin, while `bbox_norm`/`bbox_px` come from the **projector** over SM/SK `Component->Bounds`.
  **LABELS ARE UNAFFECTED; any consumer using `node.bounds` or `global_position` for geometry can be
  wrong by kilometres.** ⛔ **`P6` DOES NOT MOVE.** 🔗 Adjacency to `H5` class (ii) **noted, NOT merged**
  — both concern *which components count as "the object"*; adjacency is not identity.
  🚨 **TRACEABILITY IS NOW BLOCKING, and the `B1` NAME COLLIDES.** **`B1` (current)** = the pose-match
  precondition on A56 (live, ~18 mentions here, in `run_leg.ps1`/`check_pose.py`). **`B1` (older, m22)**
  = *"B1 traceability — `nodes[]` gains `asset_name`, `component_class`, bounds"*, **SHIPPED at
  `03a51d5`**. **PROPOSED, NOT ADOPTED:** keep `B1` for the pose gate, retire the older to
  **`m22-B1-traceability`**. ⚠ **`asset_name` and `component_class` ALREADY EXIST** and on this leg read
  `SM_Bush` / `FoliageInstancedStaticMeshComponent` — **naming the culprit class outright.** ⛔ **WHY
  THEY WERE INSUFFICIENT FOR THE OWNER IS NOT ESTABLISHED AND IS NOT GUESSED — the cheapest next step
  is to ask which fields her copy actually shows.** Minimal additions, all **`P6` TERRITORY / MILESTONE
  CANDIDATES because they change the `annotation.json` contract**: `instance_count` (would have named
  class (ii) outright) · `component_name` · `render_bounds`. ⛔ **Nothing implemented.**
  🟦 *(superseded — kept for the record)* **GEOMETRY SURVEY DONE (journal 045 §51-§61). THE HALT OBTAINED: *NO CROSSING PAIR EXISTS* — from
  the settled camera, NO moving platform can FULLY occlude any selectable target.** ZERO production
  code. NO TAG. Nothing graded. `P-a1`…`P-a5` still UNTESTED. **Design is the owner's call.**
  🧭 **Q-4, the deciding answer:** only **1 of 4** platforms is in the frustum at all (the other three
  sit at **68.9° / 75.5° / 75.0°** off-axis against a 45° half-FOV). That one is at range **2191 cm**
  and is **FARTHER than every compact selectable target** — `SM_Ramp2` 798, `RoomBuilderSquare` 1185,
  `BP_SpawnPad` 504 — **so they occlude IT**. It has **no rect overlap** with `BP_SpawnPad` or
  `BP_SplineSpawn`. ⚠ **It IS in front of both `InstancedFoliageActor`s (10831 / 16740 cm) with
  overlapping rects — but its rect is 240×20 px against ~1280×713, so CONTAINMENT IS IMPOSSIBLE:
  PARTIAL occlusion pairs exist, FULL ones do not.** Full occlusion (9/9 rays) needs containment.
  ⛔ **Scoped to ONE camera pose** — the one an unattended run settles into. Not a claim about MainWorld.
  ✅ **Q-1 the near platform PingPongs and REPEATS:** **Z only**, `1658.4 ↔ 2393.1` (**734.7 cm**),
  **126.67 cm/s**, **period ≈ 12.1 s**, X/Y fixed, no dwell at the turnarounds.
  ✅ **Q-2 ALL FOUR unbound platforms MOVE** — two on **Z** (126.67 and 210.00 cm/s), **two
  HORIZONTALLY** (752.5 cm on Y; 995.6 cm on X). Horizontal travel is the shape that crosses a view.
  ✅ **Q-5 THE CAMERA HOLDS:** after the ~25-frame settle, over the **last 200 frames (6.7 s)**:
  **dX 0.0004 · dY 0.0003 · dZ 0.0000 · dPitch 0.0000 · dYaw 0.0000.** Sub-millimetre, exactly zero
  rotation. **The still-camera assumption every design rests on is MEASURED and holds.**
  ⚠ **CORRECTION TO §41-§50: the platform is 126.67 cm/s, NOT the ~168.9 I published.** I divided a
  12-anchor gap by 0.4 s assuming captured frames are the only ticks; **game time between those anchors
  is 0.5333 s.** 🚨 **AND IT IS A DESIGN CONSTRAINT: cm-per-CAPTURED-FRAME depends on the CAPTURE
  CONFIG** — 5.630 at `2 4 8 4 0` vs **7.841** at `3 2 5 2 0`, a 39 % difference, while game-time speed
  is identical to 2 d.p. **Any design saying "the occluder crosses at frame N" MUST state the config.
  Never convert frames to seconds by dividing by `VideoFps` — read `t` from `labels.jsonl`.**
  🚨 **TWO annotation FIELDS ARE NOT WHAT A SURVEY WOULD ASSUME, and using either would have INVERTED
  Q-4:** `nodes[].bounds` is `GetComponentsBoundingBox(**true**)` — the **whole actor**, so
  `BP_MovingPlatform` reports a **42 m × 26 m × 17 m box centred ~2.2 km from the actor** while its real
  label rect is 240×20 px; and `global_position` is the **ACTOR ORIGIN**, so `BP_SplineSpawn` reads
  **142.8° off-axis (behind the camera)** while its geometry projects on screen. **The projector and
  `IsUnoccluded` use only SM/SK `Component->Bounds`.** ⛔ **Observation of the shipped contract only —
  `P6` DOES NOT MOVE.**
  ⚠ **Anchor-time and settled-time visibility are DIFFERENT SETS** — three of the six selectable
  targets report `poll_distance` as the **−1 sentinel** because their anchors fall inside the ~25-frame
  camera settle.
  🆕 **`G122` — an ASSET census and a RUNTIME census are DIFFERENT NAMESPACES; a claim proven in one is
  UNPROVEN in the other until joined by an exact key.** The join is now a committed artifact —
  `CaptureBench/tools/mainworld_instance_join.md` (18 instances: class · file · runtime UAID · trigger ·
  motion **OBSERVED/REFUTED/UNTESTED**) + `mainworld_join.ps1`. **Both signals AGREE on all 18 rows**,
  and that cross-check is what caught **`BP_Fan` keying on `Triggers` (an ARRAY)**. ⛔ **`UNTESTED` is
  NOT "presumed moving".**
  🆕 **`G123` — a code path labelled REPORTING ONLY that can terminate the run is not a reporting
  path.** `check_pose.py`'s B1-detail block raised `TypeError` on `bbox is None` — the **normal**
  off-calibration case — and killed the harness **after the artifacts were written**, from a block whose
  own header says it cannot affect anything.
  📋 **REQUEST, NOT A DECISION: journal 045 is at EIGHT PARTS / ~1,500 lines.** Still one
  investigation, so **NOT split** — that ruling is the owner's. A rename plus a part index would fix
  most of it.
  🟦 *(superseded — kept for the record)* **MAINWORLD FIRST LAUNCH DONE — RECON, NOT PATH (a) (journal 045 §41-§50). BRANCH: "IT ALL
  WORKS", with one partial. ZERO PRODUCTION CODE. NO TAG. B1 DECLARED NOT APPLICABLE, not skipped.**
  ✅ **R-1 IT BOOTS AND STAYS.** `Bringing World …MainWorld up for play`; **1102 probe ticks, every one
  `level=MainWorld, actors=432`, ZERO MainMenu loads, nothing redirects** — by NAME, not by picture
  (G87's headline rule, which survives its own correction). The process **stays alive** where the
  pre-cook build exited.
  ✅ **R-2 THERE IS A POPULATED GAMEPLAY VIEW** — not black, not a default origin view. **A streaming
  source exists with NO input** (`PC_InGame_C … X=3450 Y=4020 Z=1519.8`, `CellsToActivate(1)`), so the
  *"an unattended run may have no streaming source"* prior is **REFUTED**. Post-settle view origin
  `(3073.76, 4335.69, 1634.48)` rot `(0,−39.999,0)`, 6 renderable-visible actors. ⚠ **The camera is NOT
  static for the first ~25 frames** — the pawn settles from pitch −20 to 0, which is why the visible set
  differs between startup and settle.
  ✅ **R-3 THE MOVERS ARE ALL LOADED.** `IAI.ListActors` at startup: **432 actors in world MainWorld**
  against 419 external files ⇒ World Partition had already streamed essentially everything. **7
  `BP_Stomper`, 7 `BP_MovingPlatform`, 4 `BP_Fan`** named, matching the asset census. Targeted fire
  resolved every mover tried (`matched 1 actor(s)` → `applied`). ⚠ **`snapshot.visible` sees only 6 —
  what the auto-pool can SELECT is far narrower than what is LOADED**, and conflating them would read
  as "not loaded".
  ⚠ **R-4 SPLITS BY CLASS — AND THE FIRST ANSWER WAS WRONG, CAUGHT BEFORE IT WAS REPORTED.** The first
  leg fired at a Stomper that measured perfectly static over a **10-frame window with the camera
  identical to one decimal** — which looked like the pre-declared *MOVERS LOADED BUT STATIC* halt.
  🚨 **Joining runtime UAID names back to the asset files showed that instance is one of the two BOUND
  Stompers.** A trigger-bound Stomper standing still with nobody on the plate is the EXPECTED result
  and tests nothing. **The gap was mine: G-1 keyed on FILES, the leg keyed on RUNTIME UAID NAMES, and
  the two had never been joined.** The join is now done for all 13 Stomper/MovingPlatform instances.
  **Re-measured on UNBOUND instances at TWO cadences to exclude aliasing** (8 events at 12-frame
  spacing + **13 events at 7-frame spacing** via `IAI.Capture.Config 3 2 5 2 0`):
  ⛔ **`BP_Stomper` (unbound) is STATIC — 21 samples, two incommensurate cadences, ONE position**
  `(8962.014, 8754.727, 2599.993)` to three decimals. ✅ **`BP_MovingPlatform` (unbound) MOVES —
  Z 1666.889 → 2139.779, `+67.556 cm` per 0.4 s ⇒ **~168.9 cm/s**, **472.9 cm** across the leg, X and Y
  constant.** `global_position` is a WORLD position, so this is actor motion, not view motion.
  ⇒ **G-1's asset reading is CONFIRMED at runtime for `BP_MovingPlatform` and REFUTED at runtime for
  `BP_Stomper`.** ⛔ **n = 1 instance each; no mechanism proposed for the still Stomper.**
  ✅ **R-5 PIPELINE, RECORDED NOT GRADED:** four legs, all `capture_path sve` · `clock wall` ·
  `delivery false` · `non_manifested 0` · `zero-match 0` · ring **121/121/0** (166/166/0 on the 12-burst
  leg) · `speed_ratio` **1.0000–1.0020**, sustained **29.94–30.00 fps**. **No streaming hitch visible.**
  🔬 **FREE OBSERVATION, RECORDED AND NOT ACTED ON:** on the MovingPlatform leg `bbox_valid` is **59/59**
  and the **occlusion sample count VARIES WITHIN ONE WINDOW — 6/9 → 7/9 → 9/9 → 7/9 → 9/9** — with the
  **6/9 and 7/9 rows `valid:true`, i.e. SELECTED.** That is the `P-a1` band appearing in live data.
  ⛔ **RECON ONLY. No path (a) hypothesis declared, nothing graded, `P-a1`…`P-a5` remain UNTESTED.**
  🔧 **HARNESS FAULTS FIXED:** `check_pose.py` **crashed the harness** (`TypeError` on `bbox is None`)
  for a leg with no bbox rows in the settle window — **from inside the block whose own header says
  REPORTING ONLY**, and after the artifacts were written; now says plainly that **B1 has nothing to
  judge**. `run_leg.ps1` gained **`-Map`**, recorded in `_leg_geometry.json`.
  📌 **RULINGS 1–3 LANDED:** build identity is a **QUARTET** and the **PATH-(a) MEASUREMENT BUILD IS
  PRESERVED COMPLETE** (`pathA-measurement-build-paks/`, hash-verified, 282.9 MB); ⛔ **`.m25-baseline`
  is EXE-ONLY and does NOT reconstruct the build H4 ran on — that pak is GONE, the loss is BOUNDED and
  the G-2 sweep is the receipt, and NO reconstruction is to be attempted**; `setup-runbook.md` **§8.6
  FULL COOK** written; and **any future comparison against an H4 leg must treat the poses as
  NEAR-identical, not identical.**
  🟦 *(superseded — kept for the record)* **THE COOK IS DONE (journal 045 §34-§40). `MainWorld` IS IN THE BUILD, `G118` IS CLOSED, AND THE
  SMOKE LEG REPRODUCES m25's CERTIFIED BEHAVIOUR. ZERO PRODUCTION CODE. NO TAG.**
  🚨 **`G121` — THE EXE HASH DID NOT CHANGE, SO IT DOES NOT IDENTIFY THE BUILD.** No code changed, so
  nothing compiled and the archived exe kept its compile time: **before and after are BOTH `101AFEA4`,
  same mtime** — while the cooked maps went 3 → **4 (+MainWorld)**, the enforced token went
  **`TESTVALUE123` → 64-char**, and `.ucas` went 125 MB → **284 MB**. **Same hash. Different build.**
  ⛔ **Every A44 hash reference in this project identifies only HALF the artifact** — *"staged exe
  `101AFEA4` = m25"* remains true and is **no longer sufficient**, because **two builds answer to it**.
  ⚠ **The reverse is the dangerous case:** a code-only hot-swap (G103) moves the exe and leaves the pak,
  so **the halves move independently and a same-hash comparison is not a same-build comparison in either
  direction.** **RULE: build identity = exe hash + pak identity.**
  📦 **THE MEASUREMENT BUILD, in full:** exe **`101AFEA4`** · `StackOBot-Windows.utoc` **`939B9C9B`**
  (268,036 B, 2026-08-19 17:00:27, **4 maps**) · `.ucas` **`8A602D4D`** (284,469,920 B) · `.pak`
  **`7CAE22DD`** (10,115,703 B). **Prior build for contrast: same exe + `.utoc` 194,996 B (16-08),
  3 maps, placeholder token.**
  ✅ **MAP-SET GATE PASS** — read from the `.utoc` container index, not from the `-map=` argument
  (G119 applied to the cook): `CB_GateLevel` · `MainMenu` · **`MainWorld`** · `Entry` all PRESENT.
  New permanent tool `CaptureBench/tools/verify_cooked_maps.ps1`; it scans **both encodings** and exits
  **2** on zero-in-both rather than reporting a clean absence — **this container answered in ASCII while
  the pre-cook ones answered in UTF-16, so the encoding is NOT stable and a single-encoding scan would
  have read as "no maps cooked"** (G103's shape).
  ✅ **`G118` CLOSED** — enforced token read back from the running build: **64-char, matches the source
  ini, no placeholder.**
  ✅ **A44 SCAN** of the staged exe, both encodings: 7 symbols, **ascii 0 / utf16 non-zero throughout** —
  sound, not suspect tooling.
  ✅ **SMOKE LEG (CB_GateLevel, 1280×720, fps 30 pinned, SVE not forced, delivery OFF, target `_49`):**
  `capture_path sve` · `content_clock wall` · `key_ring 121/121 missed 0 corrupted 0` · **B1
  pose-matched** · A56 **100 % of 59 rows, 1 distinct** · **A54 ALL-ALIGNED 7/7, decidable 7/7, median
  margin 0.103835** · **positive control decisive BOTH ways (+1 → 7/7 SHIFTED, −1 → 8/8 SHIFTED)** ·
  **7 counted events** · provenance **8 events all `valid:true` 9/9**. ⇒ **THE NEW BUILD IS THE
  MEASUREMENT BUILD FOR PATH (a).**
  📏 **WHAT THE STAGE DID: NOTHING WAS WIPED** — leg dirs 56 → 56, `.baseline` exes 4 → 4, `Saved\`
  23 → 23 incl. `M23B`. ⚠ **ONE cook, ONE flag set (no `-clean`, archiving into an existing tree). The
  2026-08-16 wipe is NOT retracted; the archive step is simply not UNCONDITIONALLY destructive and which
  factor decides is NOT established. THE PRECAUTION STAYS.** G92 annotated in place.
  🛟 **BASELINE CHAIN EVACUATED to `D:\IntrusiveAnomalies\_binary_baselines\` (a sibling of the bank,
  outside `Builds\`), hash-verified AT THE NEW LOCATION: 5 of 5 match** (`101AFEA4` as `.m25-baseline`,
  `259BF64F`, `834BB30A`, `3BA854FB`, `85A39CFB`), with a README mapping each hash to what cites it.
  ⚠ **THE RE-BANK SWEEP FOUND UNBANKED EVIDENCE: `Saved\M23B` and EIGHT exe-side leg outputs, four of
  which (`S43_defaultA/B`, `S43_backbuffer`, `S44_deliveryON`) are the RAW EVIDENCE BEHIND m25's S4-3
  AND S4-4 CLAIMS.** Bank **91 → 104** dirs.
  🚨 **A THIRD CONFIRMATION OF S-1, from our own docs:** `G91`'s documented cook command carries
  `-map="…CB_GateLevel+…MainMenu"` and **never contained MainWorld** — the cause was written down here
  since 2026-08-06, on the same page as the claim that the title actively redirects it away.
  ⚠ **RUNBOOK GAP: `setup-runbook.md` §8 documents only the CODE-ONLY hot-swap** (*"no cook, G103"*).
  **There is no full-cook recipe in §8** — the command used came from `G91`. Flagged, not silently fixed.
  ⛔ **MainWorld is COOKED IN but has NOT been launched.** No path (a) leg. No occlusion measurement.
  🔬 **Two observations, recorded not acted on:** the smoke leg needed **3 A63 attempts, two banked
  bifurcation discards** with genuine A47 signatures (`modal_rot` 359.46/358.38 and 354.24/353.71) —
  **2-of-3 exceeds the record's ~2-in-5 and the pak is much larger, so startup timing plausibly differs;
  n=3, association only, nothing adopted**; and the accepted leg rests at **`(0.0, 0.35, 0.0)` not
  exactly `(0,0,0)`**, moving `coverage_pct` 7.7977 → 7.6575 and modal bbox width 306.1 → 301.1 — **near
  identical to H4's legs, not identical.**
  🟦 *(superseded — kept for the record)* **PRE-COOK GATE RAN 2026-08-19 (journal 045 §29-§33). OWNER
  RULED OPTION A. NOTHING WAS COOKED THAT TURN.**
  ✅ **`G-1` PASS — the Trigger question is settled per PLACED INSTANCE.** MainWorld's 419
  one-file-per-actor externals, keyed on each file's **own** `PersistentLevel.<Class>_C_UAID_` path
  (a first pass keyed on *mentions* was wrong — it called two PressurePlate files Stompers — and was
  corrected before use). **`BP_Stomper` 7 placed, 5 with an EMPTY Trigger · `BP_MovingPlatform` 6
  placed, 4 EMPTY · `BP_Fan` 4 placed, 2 EMPTY · `BP_EnergyOrb` 21 placed, 21 (the class has no
  Trigger variable).** ⇒ **NINE TRANSLATING MOVERS RUN UNATTENDED.** **Discriminator verified BOTH
  WAYS (G96): 16 MainWorld files DO carry a serialised `Trigger`**, so a `False` is a reading, not
  blindness; and the two signals (property serialised / references a trigger class) agree on every
  Stomper and MovingPlatform. ⚠ **CORRECTION to PART FOUR: `BP_Fan` uses `Triggers` (an ARRAY), not a
  scalar `Trigger`** — keyed wrongly it read 4-of-4 unattended and contradicted its own reference
  signal; re-keyed, **2 of 4 are BOUND**. ⛔ Still NOT established: which mover is positioned to occlude
  what (placement geometry unanalysed — a design question, not a gate question).
  ✅ **`G-2` — THE `101AFEA4` DEBT SWEEP: THE LIST IS EMPTY. Nothing is owed against it; the binary can
  be retired.** Only S4-4's two delivery legs and H4's five dirs ran on it; the S4-1 matrix is on
  `.s4-0-baseline` `834BB30A` and the S4-2 gates on `.s4-2-baseline` `259BF64F`, both preserved.
  H4 needs no re-measurement (its limits are limits, not pending work, and a replication on a NEW
  binary is stronger); path (a) **cannot** be owed against a binary that lacks MainWorld; `P5`/`P7`,
  `B2`, `B1`-NDC and the G117 sibling are all gated against **banked** data, which a re-cook does not
  touch. 🚨 **BUT THE SWEEP SURFACED TWO COOK RISKS: (1) ALL FOUR `.baseline` EXES LIVE INSIDE
  `Builds\BenchGate\...\Binaries\Win64\` — the exact directory a re-stage writes into. COPY THEM OUT
  BEFORE THE COOK**, with `101AFEA4` as the new `.m25-baseline`; losing them orphans every hash
  reference in this block and four journals. **(2) DECIDE THE COOK'S MAP SET BEFORE RUNNING IT and READ
  THE NEW `.utoc` MAP INDEX BACK AFTERWARDS**, asserting `CB_GateLevel` + `MainMenu` + `MainWorld` —
  **a cook that silently omitted a map is exactly what created this situation**, and nothing checks the
  cooked map set today. *(G119's rule, applied to the cook.)*
  ✅ **`G-3` — THE G87 CORRECTION IS NOW CONFIRMED, NOT MERELY CORRECTED.** One launch of the unchanged
  `101AFEA4` at MainWorld: `LogStreaming: Warning: LoadPackage: SkipPackage: /Game/StackOBot/Maps/
  MainWorld — THE PACKAGE TO LOAD DOES NOT EXIST ON DISK OR IN THE LOADER`, then
  `LogLoad: Error: Failed to enter …`. Banked at `_bench_sessions_bank/G3_MAINWORLD_BROWSE_PROBE/`.
  ⚠ Under `-unattended` this build **exits** rather than falling back to MainMenu, where G87 recorded a
  fallback; that difference is plausibly an `-unattended` artifact, **was not isolated, and is NOT
  claimed as a further correction** — the CAUSE is what G-3 settles.
  🔢 **PREDICTION SET `P-a1`…`P-a5`, PRE-DECLARED (journal 045 §22) — recon 2's five items are
  PREDICTIONS, not findings, and numbers are never reused:** `P-a1` first-clear-ray (8 of 9 blocked
  passes) · `P-a2` AABB corners lie outside sphere/cylinder/cone · `P-a3` `bTraceComplex=false` ⇒ a
  no-collision or non-`ECC_Visibility` occluder does not occlude · `P-a4` pick-time success never
  implies pixels drawn · `P-a5` single sample in time, so pick-time truth decays. **Any path (a) design
  must state WHICH it exercises and which it leaves untested.**
  🚨 **G118 — FOUND WHILE COLLECTING THE CORROBORATING ECHO, ORTHOGONAL TO H4, AND SECURITY-RELEVANT:
  the STAGED build enforces the placeholder control-server token `TESTVALUE123`** (cooked before the
  rotation), while `Config/DefaultGame.ini` carries a clean 64-char one. **G112's guard checks the
  SOURCE ini and therefore validates the artifact that enforces nothing.** The binary's own log says
  *"from DefaultGame.ini"* and means the **cooked** one. ⛔ **NOT FIXED — the fix is a re-cook + re-stage,
  which wipes `Saved` (G92) and would replace the exact binary `101AFEA4` every m25 result is measured
  against. FILED, and it must not ride inside a measurement turn.**
  ✅ **G112 AMENDED IN PLACE (2026-08-19): its runnable detector is DEMOTED to NECESSARY-BUT-NOT-
  SUFFICIENT** — not deleted, since it still catches a source-side regression, it just cannot certify a
  build. `PRE-DELIVERY-CHECKLIST.md` §1 gains a second mandatory box: **the READ-BACK check** (start the
  build, read `Control server token:` from its own log, assert non-placeholder and ≥32 chars), with the
  stop *"if source and enforced disagree the build is STALE and must be RE-COOKED before delivery"*.
  **A build is checked by what it ENFORCES, not by what its source says.**
  🆕 **G119 — THE GENERALISATION, AND IT IS THE THIRD INSTANCE OF ONE SHAPE:** **G92** trusted "it
  compiled" without verifying it was **staged**; **G113** trusted an **exit code** without verifying it
  was **earned**; **G118** trusted the **source config** without verifying what the artifact
  **enforces**. *For anything BAKED — cooked config, embedded resources, compiled-in defaults,
  generated headers, packaged assets — the source file is an INPUT, not the artifact. Read it back out
  of the running system.* Diagnostic: **"what would I observe if the thing I edited never reached the
  thing under test?"** If the answer is *"exactly what I am observing now"*, it is not a check.
  ⚠ **A guard that PASSES the unsafe case is worse than no guard** — the false pass is silent, looks
  like diligence, and retires the suspicion that would otherwise catch the fault.
  ⚠ **A63 record: the control needed 2 attempts and try1 was banked before being discarded.** Its
  failure was A56's **self-consistency** conjuncts (`distinct=8, modal=45.8%`) with **`pose_match=True`
  and the bbox matching CALIB_BBOX exactly** — a **THIRD** cause behind the generic label *"POSE GATE
  FAILED (B1) — CAUSE NOT ESTABLISHED"*, after resolution scope (S4-1) and genuine A47.
  🛑 *(part one, kept — it is why the run above is trustworthy)* **THE ORIGINAL BRIEF WAS HALTED AT THE
  SCOPE GATE BEFORE ANY RUN.**
  Three blockers, all found before the run: **(1)** a capture run carries **exactly ONE** targeted
  `(anomaly, actor)` pair — session events come only from `LiveFires`, and the only route that reaches a
  NAMED occluded actor is `BeginFire → TryFireSpecific` with the run's single configured pair
  (`IAI.Auto.FireOnce` routes through `IsUnoccluded`; `IAI.Apply` / WS `inject` never create a `LiveFire`
  and so emit **no label at all**), so *"two targets in the same run"* needs **production code**;
  **(2)** `a56_check` takes **one modal bbox per leg**, so a two-target leg is `NOT-A54-CERTIFIABLE` at
  modal coverage ≈ 0.50 **before a pixel is read**; **(3) → G117 — `CALIB_BBOX` is `StaticMeshActor_49`'s
  bbox and `pose_match` is a conjunct of `a56_check`, so the oracle CANNOT return ABSENT on any other
  target, in any run design.** That is S4-1's off-calibration ruling recurring **on the target axis**;
  the H4-CONFIRMED branch's *"A54 = ABSENT"* half is therefore **structurally unobtainable** as briefed.
  🚨 **G116 — THE PRE-FLIGHT'S REAL FINDING: H4's pre-declared CAUSE signature is NOT unique to H4.**
  `selection_provenance` `valid:false` + `0/0` is written by **four** short-circuit clauses (renderable /
  **poll radius** / **frustum** / occlusion) and **21 of 780 banked records already carry it**, all on
  `StaticMeshActor_49` which is **unoccluded 9/9** — **21/21 out of frustum, 0/21 occluded.** The
  discriminator is now the **PAIR**: provenance `valid:false` **together with the anchor frame's
  `bbox_valid: true`** (frustum kills the bbox too; occlusion does not). **The bank holds ZERO instances
  of the divergence.** ⛔ **`StaticMeshActor_11` — the brief's first choice — is REJECTED**: at 2543.7 cm
  it is outside the default **1800 cm poll radius**, so its `valid:false` would be the poll cull. Of the
  briefed set only **`StaticMeshActor_100`** survives (poll 1031.9 cm, 97.6 % on-screen, all 9 rays
  blocked by `StaticMeshActor_86` alone, **0 rays floor-only**). ⚠ **PF3, recorded and carried:
  `_100` DOES throw shadow into visible pixels** (2 of 9 patch samples reachable), so *"contributes no
  pixels"* is **false as stated** for it — and the two candidates whose shadows are fully hidden
  (`_11`, `_139`) are exactly the two the poll radius rejects. ⚠ **PF4: viewport scoping is in NO capture
  artifact** — the usable A48 echoes are the packaged log's `blinking: matched N actor(s)` (behavioural,
  and `matched 0` *is* the scoping-ON signature) and WS `ControlSnapshot.viewportScoping`.
  ⚠ **`coverage_pct` under that condition is `-1`, NOT `0`** (the `FSelectionProvenance` sentinel;
  confirmed on all 21 banked records) — a correction to journal 036 §3.5 and to the §8 prediction, made
  **before** the run. **Third instance of a claim resting on prose rather than a measurement (G106 first,
  m24's `key_ring_*` second). `P6` DOES NOT MOVE.**
  🔒 **`G118` CLOSURE — SEQUENCED BY THE OWNER, NOT BY THE IMPLEMENTER, AND IT IS A GATE ON EVERYTHING
  ELSE THAT USES THE BENCH BINARY.** `G118 CLOSURE = re-cook + re-stage + re-bank (G92 wipes `Saved`) +
  re-run the A44 hash scan`. **It runs AFTER the current measurement sequence and NEVER inside one**,
  because **closing it RETIRES staged exe `101AFEA4` as the m25 measurement binary.** ⛔ **Any result
  still owed against `101AFEA4` must land BEFORE closure.** Until it closes, treat any WS work against
  this build as running with a **known-public token**.
  ✅ **THE SAME RE-COOK ALSO PUTS `MainWorld` IN A BUILD (§23), so `G118` CLOSURE AND THE PATH-(a)
  ENVIRONMENT ARE ONE OPERATION, NOT TWO.** Sequence it once: decide the cook's map set (`CB_GateLevel`
  must stay — every m25 result depends on it) **before** running it, because a re-cook that omits a map
  is what created this situation in the first place.
  **LATER / UNORDERED, named but not sequenced:** `B2` (scale-free separability; eight-control gate; may
  reopen m23's DA60 floor) · **`B1`-NDC** (normalise `CALIB_BBOX` — unblocks alignment certification at
  any resolution; four legs blocked today, three with a motionless camera; definition change needing its
  own eight-control gate) — **now paired with its target-axis sibling from G117 (per-leg calibration
  bbox); both are A53 definition changes and neither belongs in a measurement turn** · `P6` bounds
  implementation · the `A17`/`A19` retroactive audit · resolution selection / JPEG / defaults profile.
- 🟦 *(superseded — kept for the record)* **`S3` IS CERTIFIED AND TAGGED `m24`. RATIO-INDEPENDENCE IS
  DISCHARGED. THE I10 WINDOW-RE-CHECK DEBT IS DISCHARGED.**
  **Cold-start reading order: this block → `docs/sessions/2026-08-18-043-session-close-i10-recheck.md`
  (session close-out, written for a cold reader) → `docs/sessions/2026-08-18-042-s3b-matrix-certified.md`
  (the whole S3b arc; §10 is the re-check) → `docs/CHAT-HANDOFF-s3a-sve-landed.md` → the three S2 handoffs.**
  ✅ **I10's NULL SURVIVES a MECHANICAL analysis window** — five of six legs certify, all ALL-ALIGNED,
  7/7 decidable each, margins 0.108–0.112, in-leg positive control decisive both directions on every one;
  **no leg returned SHIFTED or ABSENT**. **CPU starvation remains REFUTED as a cause of `P1`** and the
  null no longer rests on hand-chosen windows. The hand-chosen windows were excluding **STARTUP MARKER
  NOISE, not defect evidence** — the mechanical window is **STRICTER** on L1 (it includes all five noisy
  rows the hand-choice removed) and L1 still returns ALL-ALIGNED. **This STRENGTHENS the null.**
  ✅ **B1 PASSED A KNOWN-ANSWER CASE:** `L3_client39` (banked known-ALIGNED, bifurcated pose) returned
  **NOT-A54-CERTIFIABLE, not a false ABSENT** — the exact property B1 was adopted for, exercised on the
  case it was adopted against. **A control that PASSED**, not a leg that dropped out.
  ⚠ **BUT THE CLIENT BAND IS NOW CARRIED BY ONE LEG (`L6_client40`) WHERE IT HAD TWO. `P1` IS A
  CLIENT-BAND PHENOMENON**, so the refutation of CPU starvation at ~1.2 rests on **a single certified leg
  plus one honestly unjudgeable leg**. Overturns nothing, not a defect — it is the **THINNESS OF THE
  EVIDENCE, stated now** rather than discovered later if H1 also comes back clean. **`B2`'s payoff now
  includes recovering `L3_client39` and restoring the client band to two legs** — a concrete gain it did
  not have when filed; **B2 stays FILED, NOT SCHEDULED.** ⛔ **L3's bifurcation is NOT to be
  investigated, recovered or re-windowed — it stays unjudgeable.**
  ⚠ **CORRECTION TO JOURNAL 031, reconciled (BRANCH R):** its `532 / 534` is an **arithmetic slip** —
  the corpus is **540**, and its "only two exceptions … in L3" is **eight**, in **L1 (5) and L3 (3)**.
  Its own per-leg table (`85, 90, 87, 90, 90, 90`) reproduces this re-check **exactly**, so the data was
  always right and only the summary was wrong. ⛔ **The "windows filtered the denominator" explanation
  offered for it is WITHDRAWN — refuted by that same table** (85 and 87 exceed those legs' window sizes).
  The same slip propagates to §9.2's combined `1052 / 1054` (true corpus 1080); **render half NOT
  re-measured, flagged not corrected.** → **G110**.
  **Stage 1** marker survives an SVE production capture (90/90, strictly increasing, ~5× the decoder
  floor). **Stage 2a** the A54 oracle was **rebuilt from prose and certified against eight banked
  controls**. **Stage 2** five legs — **33 counted events, 33 ALIGNED, 0 SHIFTED, 0 ABSENT, 33/33
  decidable** across nominal / client ×2 / deep 3.03 / pacing-off, positive control decisive in **both**
  directions on every leg. **Stage 3** three delivery pairs — **zero extras**, invariant core
  **IDENTICAL**. **Both pre-declared predictions HELD.**
  ✅ **A10 DISCHARGED AT EVERY REGIME BY MARKER** (`frame_index − decoded_marker = {0}`, one distinct
  value, all five legs) — strictly stronger than S3a's count+cadence inference. ✅ **RING UNDER STALL
  DISCHARGED** (`published == consumed`, `missed = 0` everywhere). ✅ **`LastRunDir` DISCHARGED** —
  verified post-run over a WS client against a capture already finished on disk.
  🔬 **B′ behaves IDENTICALLY in delivery mode, not merely acceptably** — all five `key_ring_*` counters
  and `capture_path` identical across every pair.
  🚨 **`m24` SCOPE LIMITS — THESE TRAVEL WITH THE TAG: (1) MODAL CAMERA POSE ONLY** — a defect
  manifesting only in a bifurcated pose would systematically not be seen by this design; **(2) A52 —
  `VideoFps` 30 PINNED throughout; clean at 30 licenses NOTHING at other fps, in either direction;
  (3) Stage 3 is ANNOTATION-SHAPE EVIDENCE ONLY** (the pixel oracle cannot run in delivery mode);
  **(4) the oracle itself is certified at 30 fps and its MARGINS are NOT reproduced above it (`P7`).**
  🚨 **`S3` GOING GREEN DOES NOT CLOSE `P1` — AND `P1` IS NOW NARROWED TO A SINGLE NAMED LEAD.**
  P1 has never been reproduced and a fix cannot be demonstrated for something that cannot be summoned.
  **The DELIVERY-MODE GAP IS CLOSED AS A DIVERGENCE HYPOTHESIS** — delivery changes nothing in the
  annotation contract, so whatever produced the client's −1, **it was not delivery mode**. ⚠ **`H1`
  (GPU-load starvation shape) is P1's ONLY REMAINING NAMED LEAD, and H1 HAS NO LEVER IN EXISTENCE. IF H1
  ALSO COMES BACK CLEAN, `P1` HAS NO NAMED LEADS** — stated now, while a queue still sits in front of it.
  Lever design for H1 is **chat-side first, NEVER same-turn as its first measurement.**
  🆕 **`A64` — a DELIVERY-PAIR comparison requires a POSE-MATCH PRECONDITION ON THE PAIR**, not merely
  per-leg B1 admissibility: two legs can each pass B1 and still sit in **different admissible poses**
  (0.35° apart, inside tolerance, enough to move `coverage_ratio` ~1.9 % and read as a divergence).
  B1 constrains each leg against CALIBRATION; nothing constrained the pair against EACH OTHER. Use
  `coverage_ratio` as the pose indicator — **a discriminator, NEVER a gate.**
  🆕 **ALSO RULED:** **≥ 3 COUNTED EVENTS PER LEG is a validity condition** (joins A31; a leg below 3 is
  INVALID, not evidence — the settle window scales with frame time, so deep stalls can silently drop a
  leg under the bar while still reporting ALL-ALIGNED); **A40 bands are compared at their own stated
  precision (2 d.p.)**; **camera bifurcation measured at 2-IN-5, not 1-in-12, and BOTH instances were on
  STALLED legs — association only, no mechanism adopted without measurement.**
  🆕 **`P7`** A54 **margin scale is regime-dependent** (×0.98 sharp → ×2.05 spread); verdicts unaffected,
  **decidability annotations affected**. OPEN. Leading hypothesis named, NOT adopted. **`P8`** TAU is an
  **absolute** luminance difference and is therefore **NOT camera-pose invariant** — a bifurcated pose
  produced a **FALSE ABSENT** on a hide that is real, perfectly aligned and perfectly separated, and
  **A50 reads ABSENT as reproduction of the defect**, so it fails in the **dangerous** direction. Cured
  for now by **B1**'s pose precondition; the real fix is **B2**, filed not built. → **G106/G107**.
  📦 **ENVIRONMENT:** staged exe unchanged, SHA-256 `3BA854FB…`. **Bank 58 dirs.** `CaptureBench` carries
  the analysis instruments (`a54_oracle.py`, `run_leg.ps1`, `eval_leg.py`, `check_pose.py`,
  `verify_lastrundir.ps1`), local-only, **probe untouched all arc**. **The packaged-leg recipe is
  `docs/setup-runbook.md` §8; the A63 harness is `CaptureBench/tools/run_leg.ps1` — use them, do not
  reconstruct.**
  ⛔ **OPEN DEBTS — see journal 042 §8 for the complete list.** Headline ones: **`A11` OPEN — the design
  prevented the condition it wanted to observe** (`wrapped > 0` does not close it; `ForceMiss` never
  will); **the I10 HAND-CHOSEN-WINDOW RE-CHECK is NEXT** (paper only, banked data, no runs — I10's null
  is **WEAKENED, not overturned**, and a milestone tag must not bury it); **`B2`** now gateable against
  **eight** controls incl. `L3_client39`; **two bifurcated legs were LOST to a compliance failure and are
  recorded as lost**; `H4`; `P6`.
  **NEXT: `S4` — the backbuffer demotion. NOT planned.** ⚠ **C2: it is a CLIENT-VISIBLE CHANGE, not a
  silent default flip** — the pre-Slate SVE grab is **UI-free by construction**, so flipping the default
  **changes delivered image content**. **Depth remains PARKED and UNNUMBERED.**
- 🟦 *(superseded by the above — kept for the record)* **`S3a` COMPLETE AND CERTIFIED.**
  Repo `AnomalyInjector` **`f922ba8`**, clean, pushed, **NO TAG**. `CaptureBench` `8dad64e`, probe untouched.
  **B′ is landed in production behind a default-OFF switch, has run CROSS-THREAD (published 121 /
  consumed 121 / missed 0), and is green on every gate S3a defined.**
  🚨 **RATIO-INDEPENDENCE IS STILL UNDISCHARGED. Every S3a leg ran at ratio ≈1.0000006 — the easy
  regime, and the one that has historically MASKED bugs in this project. S3a proves the MECHANISM
  works; it proves NOTHING about the regimes S3 exists to certify. Do not read "S3a certified" as
  "S3 done".** Also NOT established: behaviour under stall (the ring has never run starved) and
  **marker-verified frame identity** (all legs ran **marker-OFF**, so the decoded-marker↔`frame_index`
  check was never performed; byte comparison of frames is unusable — A47 makes 90/90 images differ
  between two runs of the SAME binary).
  🔒 **S3 IS NOT A MILESTONE UNTIL S3b CERTIFIES IT — no tag. S3 GOING GREEN DOES NOT CLOSE `P1`:** P1
  has never been reproduced and a fix cannot be demonstrated for something that cannot be summoned. A
  clean matrix proves the new path does not carry the **OLD** race; it is **not** evidence it cures her
  defect. **P1 stays OPEN** — leads **H1** (GPU load, no lever exists) and the **delivery-mode gap**.
  **If the S3b matrix goes red that is a DESIGN FAILURE of B′ — redesign, not patch.**
  📦 **ENVIRONMENT THE NEXT SESSION INHERITS:** staged
  `Builds\BenchGate\Windows\StackOBot\Binaries\Win64\StackOBot.exe` = the **S3a-3 build**
  (240,539,648 B), A44-verified **in both encodings**; the **pre-S3 m23 baseline is preserved beside it**
  as `StackOBot.exe.m23-baseline` (240,502,272 B). ⚠ `Builds\BenchGate\Windows\StackOBot.exe` (217 KB) is
  the **launcher stub** (G90). **Bank = 34 dirs**; `S3A2_BASE`+`S3A2_BASE2` are the m23 **control pair**;
  **`S3A3_P2` is the ONLY banked leg exercising a dropped positive frame.** Branches: `master`,
  `s3a-2-GATE-FAILED-do-not-merge` (**unmerged ON PURPOSE — evidence of the re-parented-`else` defect**),
  `feature/stencil-capture` (do not rebase). **The packaged-leg recipe is `docs/setup-runbook.md` §8 —
  launch line, the CWD output trap, focus forcing, the both-encoding A44 scan. Use it, do not
  reconstruct one.**
  ⛔ **OPEN DEBTS (carried, not dropped):** (1) **`LastRunDir` ships UNCERTIFIED** — its runtime dashboard
  check needs a **WS client against the control server post-run** (`-ExecCmds` is startup-only);
  **verify in S3b**. (2) marker-OFF frame-identity limit. (3) `ForceMiss` phase-lock coarseness — 3 of 4
  phases reproduce the same drop pattern. (4) `video.total_frames` vs index range under partial loss —
  **noted, deliberately NOT fixed** (post-guard path only). (5) pre-existing: P6 `camera.path` naming;
  P6 `coverage_pct` vs `coverage_ratio` (predicted from source, never observed); the render-relevant
  `node.bounds` milestone candidate; **H4**.
  🧭 **NO CLIENT-FACING ACTION ITEM EXISTS ANYWHERE IN THE PLAN** (struck 2026-08-18 — there is no client
  channel in either direction).
- ✅ **`S3a-3` CERTIFIED AND MERGED — all gates green once the confound was controlled.** → journal
  `docs/sessions/2026-08-18-041-a63-and-s3a3-certified.md`. **C1 subset re-run under A63: extras = 0**
  (control 54 fields, test 54 fields); **C1 leak check PASS** (switch-OFF `run_summary` has no
  `capture_path`, no `key_ring_*`); dropped-positive gate accepted last turn and **not re-run**.
  🆕 **A63 — FOCUS-MATCH IS A LEG VALIDITY CONDITION.** Any leg entering a **cross-binary comparison**
  must record its focus condition; two legs are **COMPARABLE only if `start_frame` matches** (neither
  rode the 30 s focus timeout). Mismatch ⇒ **legs NOT comparable, the comparison DOES NOT RUN, the leg
  is INVALID and re-run** — joining the A31 validity conditions. **INVALID ≠ FAILED, and that is what
  stops it laundering a red: invalidity is declared by a PRE-FIXED rule, on a condition INDEPENDENT of
  the outcome, decided BEFORE the comparison is read.** ⛔ **REJECTED: pinning with
  `IAI.Capture.FocusGate 0` — that changes the system under test (G93: focus-gate × fixed-timestep
  corrupts the camera intermittently even at 240 fps with the gate ON).**
  ✅ **A63 RETROACTIVE CHECK ON S3a-2: SATISFIED** — `S3A2_BASE`/`BASE2`/`FIX_OFF` all `start_frame=1`,
  so **S3a-2's green was robust, not lucky** (journal 040 raised the possibility; this closes it).
  ⚠ **`S3A2_FM4` DID ride the timeout** (`start_frame` 2098) but was never in a cross-binary
  comparison, so A63 does not invalidate it.
  ⚠ **CORRECTION to journal 039: the large `key_ring_published` counts (2228 vs 121) were NOT
  "dropped frames stalling the phase machine" — they are the FOCUS-GATE WAIT.** The SVE activates in
  `StartRun` *before* the focus branch, so it publishes for every view family across the whole 30 s
  `ArmedPending` wait. Every high-count leg rode the timeout. **Verdicts unaffected** — they rest on
  `missed == corrupted`, the file set and 1:1 file↔row, none of which depend on the publish count.
  ⚠ **The dropped-positive evidence (`S3A3_P2`) came from a timeout leg**; its criteria are internal to
  the single leg, so A63 does not apply and it stands.
  🆕 **G96 FOURTH INSTANCE, and the first caught INSIDE an already-accepted result:**
  `overlap = missing ∩ claimed = 0` is **ambiguous** — it is simultaneously "never tested" and "tested
  and handled perfectly", because a correctly dropped positive vanishes from files AND claims. **The
  sound discriminator is whether the CLAIMED SET SHRANK vs the clean run.** A blind metric survives
  review when its output looks like the answer you expected.
  ⛔ **DEBT — `LastRunDir` SHIPS BUT IS NOT CERTIFIED.** The runtime dashboard check
  (`capture_stop`/`capture_status`/`ControlSnapshot` read it post-run) needs a **WS client against the
  control server**, which `-ExecCmds` cannot reach (startup-only). **Deliberately not built. VERIFY IN
  S3b.** Other open debts: marker-OFF limit on frame identity; `ForceMiss` phase-lock coarseness;
  `video.total_frames` vs index range under partial loss.
  **Bank 34 dirs** (+`S3A3_OFF/P1/P2/P3/OFF_T1`); **`S3A3_P2` is the only banked leg exercising a
  dropped positive.** Staged exe = S3a-3, A44-verified **in both encodings**; pre-S3 binary preserved
  as `StackOBot.exe.m23-baseline`. **NEXT: S3b — the matrix. Not planned this turn.**
- 🛑 *(superseded by the above — kept for the record)* **`S3a-3` HALTED on a focus confound; code was held
  OFF `master` on `s3a-3-GATE-MISS-focus-confound`.** → journal
  `docs/sessions/2026-08-18-040-s3a3-gate-miss-focus-confound.md`. Built + compiled: `run_summary`
  `capture_path` + five `key_ring_*` fields **emitted only when the switch is ON** (**C1 leak-check
  PASSED** — a switch-OFF `run_summary` carries exactly the pre-S3 key set);
  `IAI.Capture.SVE.ForceMissPhase`; **FIX 2 as ruled** (`LastRunDir` set at `StartRun` + returned by
  `GetStatus`, `RunDir` cleared in the lifecycle reset); banner reports the grab point via
  `DescribeGrabPoint()`.
  ✅ **DROPPED-POSITIVE GATE PASSES.** ⚠ **A metric of mine had to be corrected first:
  `overlap = missing ∩ claimed = 0` is AMBIGUOUS — it is ALSO the pass condition, because a correctly
  dropped positive vanishes from files AND from claims.** The sound discriminator is whether the
  **claimed set SHRANK** vs the clean run. Whole N=4 phase space exhausted: phases 0/1/3 reproduce the
  same lock (drops at offsets 1,7,11 mod 12; claims full), **phase 2 breaks it** (offsets 0,6,10;
  claims 4,5,9) and **dropped 7 positives `[10,22,34,46,58,70,82]`**. All 7 absent from disk **and**
  from claims; **claimed indices with no file behind them = `[]`**; 68 files ↔ 68 rows 1:1;
  `session_index` == image number on every row. **Pre-declared prediction held — no label-fabrication
  defect in the SVE path; P3b does not arrive by this route.**
  🛑 **C1 SUBSET RE-RUN MISSED — 16 extras** (`engine/ticks_msec` ×8, `run.json/start_frame`,
  `run_summary/end_frame`, 6 label fields). **Cause ENVIRONMENTAL, not code: the switch-OFF leg never
  got window focus and sat out the 30 s focus-gate timeout** — `start_frame` **2559 vs 1** on all three
  prior legs; `ticks_msec` 30270 vs 134 (**30270 ms IS the timeout**).
  ⚠ **THE REAL FINDING: the gate has an UNCONTROLLED ENVIRONMENTAL VARIABLE that can flip it either
  way. The control pair was n=2 with BOTH halves on prompt focus, so it never sampled that variance —
  and S3a-2's re-gate (journal 039) passed partly because all four of its legs happened to get prompt
  focus. Luck, not design.** A57's bracket-vs-contain problem, in the harness rather than in an oracle.
  **NOT re-run for a green** (that would be discarding a red). ⚠ **Cannot claim S3a-3 switch-OFF IS
  subset-identical — the same-focus-condition comparison has not been run.** Chat-side gate-design
  call: pin the variable (`IAI.Capture.FocusGate 0` — ⚠ **G93**, 30 fps only) or make equal
  `start_frame` a precondition of the comparison.
  ⚠ **FIX 2's dashboard verification NOT PERFORMED** (the halt came first; it needs a WS client
  post-run and `-ExecCmds` only fires at startup) — **`LastRunDir` is implemented but
  RUNTIME-UNVERIFIED.**
  ⚠ **The staged package exe is now AHEAD of `master`** — restore `StackOBot.exe.m23-baseline` or
  re-stage before any leg that must run certified code.
- ✅ **`S3a-2` FIXED AND RE-GATED IN FULL — ALL GATES GREEN (`130efaa`).** → journal
  `docs/sessions/2026-08-18-039-s3a2-fixed-and-regated.md`. `IAI.Capture.SVE` (default **0**, mid-run
  guarded, GConfig `bSveCaptureDefault`) now selects the B′ path; intermittent `ForceMiss` (0/1/N);
  C3 log; A48 echo. **FIX 1 is STRUCTURAL: `FinishRun`'s finish logic now contains NO `else` at all** —
  two independent `if`s over a captured `bWroteSession` — so no future append can inherit the branch
  that deletes the session dir. SVE teardown moved to sit with the lifecycle reset.
  ⛔ **FIX 2 (clear `RunDir`) is BLOCKED and was NOT worked around:** `capture_stop`
  (`AnomalyControlServerSubsystem.cpp:625-643`) calls `StopRun()` then `GetStatus(...RunDir...)` and
  ships `runDir` to the dashboard; `capture_status` and the snapshot do the same. Clearing it would
  send an empty path. **The latent hazard from journal 038 therefore REMAINS** (unreachable today; FIX 1
  does not remove it) — chat-side contract decision: add a `LastRunDir`, or accept and document.
  **GATES:** **G-S3a-1 (amended) PASS** — control pair of two m23 runs measured a **54-field
  run-unique set**; m23-vs-S3a-OFF is **54 fields with ZERO extras** ⇒ subset; invariant core
  (event count, `frame_indices`, `manifested`, types, `video` minus `path`, and 11 `run_summary`
  fields incl. `end_frame`) **all identical**; **A62 verified ON DISK after process exit** — 90 frames,
  no `CANCELLED`. **G-S3a-3 PASS** — SVE leg wrote 90 frames, cadence **byte-exact** to
  `[[4,5,9,10]…[88,89]]`, and **the ring round-tripped ACROSS THREADS for the first time ever:
  published 121 · consumed 121 · missed 0.** **C3 measured: SVE view rect 1280×720 vs backbuffer
  window rect 1280×720, dW=dH=0** (not generalisable to DPI-scaled/letterboxed configs).
  **G-S3a-2 PASS, three readings on the artifact** — ForceMiss 0/1/4 → **90 / 0 / 68 frames on disk**,
  `missed == corrupted` **exactly 25.0 %** at N=4, and the 68 survivors are **1:1 with their label
  rows** carrying the canonical cadence.
  ⚠ **A47 CAUGHT LIVE IN THE CONTROL PAIR — and it is why C1's original form was unsatisfiable:** the
  two m23 legs differ in `camera.rotation` (one mid-settle, one at rest) and therefore in coverage,
  bboxes, `visible_positive` **and all 90 of 90 frame images byte-wise**. **Byte identity of frames is
  NOT available even between two runs of the same binary**, so frame identity rests on count + names +
  cadence + the label index series. ⚠ **Marker was OFF on these legs**, so the decoded-marker↔`frame_index`
  check was **not** performed.
  ⚠ **TWO LIMITS ON THE FORCEMISS PROOF, recorded not smoothed:** (1) **`ForceMiss N` is PERIODIC and
  the capture cadence is PERIODIC, so they PHASE-LOCK** — at N=4 the 22 dropped indices and the 30
  claimed positives had **zero overlap** (≈0.02 % by chance), so **a dropped POSITIVE frame was never
  exercised**; a randomised/offset mode is needed to reach it. (2) under partial loss
  **`video.total_frames` (68) disagrees with the index range (0…89 with gaps)** — self-inconsistent
  artifact, reachable only after the guard has fired. **Cosmetic, unfixed:** the run-`STARTED` banner
  still prints `capture=async/backbuffer` on an SVE run (it reports `bAsyncCapture`, not the grab
  point); the `grab point EFFECTIVE` line is authoritative.
  ⚠ **A44 NEAR-MISS ON MY OWN SCAN:** an intermediate scan read **0** for every SVE symbol because it
  decoded **ASCII only** — these strings are **UTF-16** (`ascii=0 utf16=8`). **A single-encoding scan is
  a false-negative generator; always decode both.**
  **Bank now 29 dirs** (+`S3A2_BASE2`, `S3A2_FIX_OFF`, `S3A2_FIX_ON`, `S3A2_FM1`, `S3A2_FM4`). Staged
  package carries the **S3a** binary; the pre-S3 one is preserved as `StackOBot.exe.m23-baseline`.
  **NEXT: S3a-3** — `run_summary` `capture_path` + ring counters, emitted **only when the switch is ON**.
- 🛑 *(superseded by the above — kept for the record)* **`S3a-2` FAILED ITS OWN GATE AND WAS DIAGNOSED. Code on branch
  `s3a-2-GATE-FAILED-do-not-merge` (`087f4d9`, pushed). DO NOT MERGE. `master` is untouched.**
  → journal `docs/sessions/2026-08-18-038-s3a2-gate-failure-diagnosis.md`.
  **With the switch OFF a packaged run wrote a complete 90-frame session and then DELETED it.**
  **CAUSE (confirmed from source, not inferred): a re-parented `else`.** `FinishRun` is
  `if (bRunBegun) {write…"FINISHED"} else {DeleteDirectory(RunDir); "CANCELLED before focus"}`; S3a-2
  appended a block after the closing brace of the `if` body, and the next token was `else`, so **the
  `else` changed owner** to the inserted `if (SveCapturer.IsValid())`. With the switch OFF that `if`
  is always false ⇒ **the delete fired on every successful run, inside the same call** (the 20 ms
  between `FINISHED` and `CANCELLED` **was the recursive delete**). Compiles clean; no warning; both
  branches live. → **G102.** Both of Code's own leads were **eliminated**, and "FinishRun ran twice"
  is **dead** — it ran once.
  ✅ **BLAST RADIUS ANSWERED FROM SOURCE: `m23` and every shipped client build are NOT affected.**
  All four `FinishRun` callers are guarded (`StopRun` on `!bRunning`; the frame-cap block on
  `Phase != Idle`; the `PostGap`/`DrainTail` cases by a switch with no `case Idle`), and `FinishRun`
  ends by setting `bRunning=false; Phase=Idle` — so it runs **at most once per run** and a successful
  run **cannot** reach the delete. ⚠ **Latent, reported not overstated:** `RunDir` is *not* cleared at
  the end of `FinishRun`, so the delete branch is one unguarded future call away from destroying a
  *previous* session. Hardening candidate, not a defect, not in scope.
  ⚠ **C1 IS AMENDED — the original was unsatisfiable (chat-side error, recorded).** Byte-identity of
  `annotation.json`/`labels.jsonl`/`run_summary.json` across two runs is impossible: `session_id` is a
  timestamp (and appears in `video.path`), and `run_summary` carries `end_frame` plus wall-derived
  `speed_ratio`/`sustained_wall_fps`. **G-S3a-1 (AMENDED) = (1) CONTROL PAIR — two runs of the SAME
  m23 binary establish the run-unique field set EMPIRICALLY, recorded field by field; (2) SUBSET TEST
  — the m23-vs-S3a-OFF difference set must be a SUBSET of it, any extra field FAILS, no judgement
  call; (3) frame identity.** Stricter than a hand-waved allowance, not looser.
  🆕 **A62 — for any gate whose subject is written output, THE ARTIFACT ON DISK IS THE GATE.** Both
  legs' logs read identically and perfectly (`FINISHED: 90 frame(s) (positive=59)`); a log-gated check
  calls the failing leg GREEN. m19's lesson recurring in a new place.
  **New gotchas G101** (`IAI.Capture.Start`'s `outDir` is **CWD-relative**, not `Saved`-relative — a
  fresh run's output is beside the exe, so "it wrote nothing" is the wrong conclusion), **G102** (the
  stolen `else`), **G103** (staging a code-only change is an **exe hot-swap** — ~85 s build + one file
  copy, **no cook, and G92's archive-wipe is NOT in play**; but the hot-swap *is* the stage step, so
  A44-scan the **staged** artifact).
  **Banked:** `S3A2_BASE\session_20260818-110348` — a clean **m23 30 fps gate leg** (8 events, gapped
  `[4,5,9,10]`, `manifested` 8/8, ratio 1.0000004), reusable as half the control pair;
  `S3A2_OFF_FAILED_EVIDENCE\` — both run logs. **Staged package RESTORED to the m23 binary and
  re-scanned (0 SVE strings).** **NEXT: the fix, as its own turn — diagnosis and fix do not share a turn.**
- 🚧 **S3 IN FLIGHT — `S3a-1` SHIPPED `dbf139e`, GATE GREEN. Slices S3a-2 and S3a-3 NOT started.**
  The B′ key ring, scene-view extension and SVE capturer exist and **NOTHING SELECTS THEM** —
  `AnomalyCaptureSubsystem` is untouched, `StartRun` still always registers the backbuffer hook, and
  the six new symbols appear in their own six files and **nowhere else**. Switch-OFF inertness is
  **STRUCTURAL, not demonstrated** (there is no way to reach the code), which is the strongest form
  that gate can take. **Gate evidence:** editor compiles exit 0; ring round-trip 8/8 with the
  `bWanted` bit intact; wrap exact (100 published vs capacity 64 → 64 hits / 36 missed / 36 wrapped);
  **`ForceMiss` proven BOTH ways before it is wired** (1 → 0 hits, 8 misses; 0 → back to 8/8).
  ⚠ **DEVIATION, flagged:** the plan put both console vars in S3a-1, but registering the
  `IAI.Capture.SVE` **selector** would have meant touching the subsystem and forfeiting
  inert-by-construction — so the selector + mid-run guard + GConfig default move to **S3a-2**;
  `IAI.Capture.SVE.ForceMiss` stays in S3a-1 because it is a **ring** property.
  ⚠ **The ring is proven as LOGIC, not yet across threads** — publish and lookup ran on one thread
  from the console; the real game-publishes/render-looks-up round trip is first exercised in S3a-2,
  and under stall only in S3b. **New gotcha G100** — `AnomalyCapture` now compiles against a
  **Renderer PRIVATE include path** (`PostProcess/PostProcessMaterial.h`, and the `class FViewInfo;`
  forward declaration that must not be tidied away); an engine bump breaks it **inside our module,
  far from the `Build.cs` line that caused it**. Non-Shipping only; core module and the backbuffer
  fallback unaffected.
  ⚠ **C1 RULING — G-S3a-1 HAS NO EXCLUSIONS:** when the switch is OFF the new `run_summary` fields
  (`capture_path` + ring counters) are **not emitted at all**, so `run_summary.json` is byte-identical
  too and rejoins the identity set alongside `annotation.json` and `labels.jsonl`. **S3a-3 must emit
  those fields only when the switch is ON.**
  ⚠ **C2 — S4 IS A CLIENT-VISIBLE CHANGE, NOT A SILENT DEFAULT FLIP:** the SVE grab is pre-Slate and
  therefore **UI-free by construction** (which independently satisfies the client's stated UI-excluded
  ask), so flipping the default at S4 **changes delivered image content** and must be planned as such.
  **C3:** S3a-2 must **measure and report** the SVE-view-rect vs backbuffer-window-rect resolution
  delta — "may differ" is not something to carry into the S4 decision.
- 🚧 **S3 STARTED (2026-08-18) — S3a PLAN** → journal
  `docs/sessions/2026-08-18-037-s3a-plan-stage-renumber-h4-filed.md`.
  ⚠ **STAGE RENUMBERING — A NUMBER MOVED, READ THIS BEFORE TRUSTING ANY OLDER DOC:**
  **`S3`** = B′ into `AnomalyCapture` behind a **default-OFF** switch, **COLOUR ONLY**, full ratio ×
  config matrix on the **real paced path** — *this is where ratio-independence is DISCHARGED* (unchanged);
  **`S4` = the BACKBUFFER DEMOTION to the UI-on option + defaults flip + client config — `S4` WAS
  "depth" and NO LONGER IS (it was S5)**; **DEPTH is PARKED and UNNUMBERED** (`SceneDepthTexture` in
  `PrePostProcessPass_RenderThread`, FP32, + the typed FP16/FP32 path) — **parked, NOT deleted**,
  revivable if the ML side wants it or the H4/stencil lane needs a cheap instrument. **THERE IS NO HOLE
  AT S4 — the number moved down.** Any earlier text calling depth "S4" or the demotion "S5" is superseded.
  **S3 SPLITS INTO TWO GATED TURNS (structural, not a suggestion): `S3a` = implementation (land B′
  behind the switch, prove switch-OFF inert, prove the loud-miss guard fires ON THE PRODUCTION PATH);
  `S3b` = the full ratio × config matrix.** Bundling them would let a validation miss halt a turn that
  also holds uncommitted code. **S3a plan = 3 new files (`AnomalySveKeyRing`, `AnomalySceneViewExtension`,
  `AnomalySveCapturer`) + `Renderer`/Renderer-private in `AnomalyCapture.Build.cs` (sanctioned: architecture.md
  deferred it to Stage 3) + `IAI.Capture.SVE` (default 0, GConfig `bSveCaptureDefault`) + `IAI.Capture.SVE.ForceMiss`
  + `run_summary.capture_path`; sliced into 3 gated commits; NO tag until after S3b.**
  **THE SEAM IS `FAnomalyCapturedFrame::RequestId`** — B′ swaps the PRODUCER of that id and touches no
  consumer (`PendingSnapshots`, label record, accumulator, writer, `labels.jsonl`, `annotation.json` all untouched).
  **PRE-DECLARED PREDICTION (before the matrix exists): ratio-independence HOLDS at every ratio incl. deep and
  pacing-off** — B′ keys by IDENTITY, not order, so the arm→present race has no positional step left to fail on.
  **IF THE MATRIX GOES RED THAT IS A DESIGN FAILURE OF B′, NOT A BUG — it means REDESIGN, not patch.**
  🚨 **S3 GOING GREEN DOES NOT CLOSE `P1`.** P1 has never been reproduced and you cannot demonstrate a fix
  for something you cannot summon. A clean matrix proves the new path does not carry the OLD race; it is NOT
  evidence it cures her defect. **P1 stays OPEN after S3**; leads unchanged: **H1** (GPU load, no lever exists)
  and the **delivery-mode gap**. Free-run debts: **A10 discharges in S3a** (nominal paced leg only);
  **key-ring-under-stall and A11 wait for S3b** (`ForceMiss` counters are synthetic, so they cannot honestly
  discharge A11).
- ⚠ **A47 AMENDED (2026-08-18): the bifurcation is in camera ROTATION, not position.** Measured: eye position
  invariant at `(-1500,0,260)` on **369/369** banked gate samples; rotation modal `(0,0,0)` on 278/369.
  **A47's original per-leg-bbox ruling is UNCHANGED.** New clause: **inter-actor occlusion is invariant across
  the bifurcation** (it depends only on eye position + static geometry; rotation changes only frustum membership).
  ⚠ **DO NOT GENERALISE — this holds because gate-level targets are all STATIC and the player start is fixed;
  it FAILS in any level with motion.**
- **PHENOMENON LEDGER — numbers are NEVER reused.** **P1** client's one-frame shift @ratio≈1.2, 30 fps —
  **OPEN, never reproduced; NARROWED 2026-08-18: the delivery-mode lead is ELIMINATED by measurement, so
  `H1` is its ONLY remaining named lead.** **P2** stale/duplicate present — signature absent.
  **P3** labelled hide never manifests — FIXED at `m23`. **P4 — PERMANENTLY RETIRED, never re-mint.**
  **P5** single-frame alignment undecidable ≥90 fps — queued, founding instrument (the blend-ladder)
  assigned, unbuilt. **P6** `annotation.json` field-contract defects — `node.bounds` SETTLED,
  `camera.path` OPEN, `coverage_pct` vs `coverage_ratio` OPEN (predicted from source, never measured).
  🆕 **P7** A54 **margin scale is regime-dependent** — ×0.98 where the signal is sharp (30 fps), ×2.04–2.05
  where it is spread (60/120 fps). **Verdicts unaffected; DECIDABILITY ANNOTATIONS AFFECTED.** OPEN.
  Leading hypothesis **named and NOT adopted** (mean-of-claimed vs a per-frame formulation). ⚠ **P7 and P5
  are BOTH about spread-signal behaviour at high fps and MAY BE THE SAME PHENOMENON SEEN FROM TWO SIDES** —
  not asserted, not merged; whichever is investigated first checks the other. The **blend-ladder serves
  both** (it manufactures **spread**, not pose).
  🆕 **P8** **TAU is an ABSOLUTE luminance difference and is therefore NOT camera-pose invariant** — an
  A47-bifurcated pose yields −0.0383 against the modal pose's +0.1126 on the *same* binary/target/seed, so
  the oracle returns **ABSENT on a hide that is real, perfectly aligned and perfectly separated (zero
  overlap)**. **A50 reads ABSENT as reproduction of the defect ⇒ this fails in the DANGEROUS direction.**
  Contained by **B1**'s pose precondition (an honest NOT-CERTIFIABLE replaces the false ABSENT); the real
  fix is **B2** (a scale-free separability statistic) — **filed, not built**, now gateable against **eight**
  controls incl. `L3_client39`. ⚠ **P8's axis is POSE, so the blend-ladder does NOT serve it** — its
  missing control was L3 and we now have it. → **G107**.
  🆕 **P9** (2026-09-02, Bates, **owner-observed — eyeball-level, NOT yet measured; do not upgrade,
  do not discard**): `blinking` only, 3 instances across both m36 Section B legs — annotation hidden
  set **{42,43,47,48}** vs observed **{42,43,44,48}**, one frame differing INSIDE the window (an
  observation about the SIGNATURE only; **NO mechanism claimed**, G120). **The measurement is owned
  by the NEXT session** — a pixel-ground-truth leg, the m18/m20 instrument class. ⚠ Minted as P9
  because the minting instruction said "P8" and **P8 is taken** — numbers are never reused; the
  deviation is stated in journal 066 §3.
- **HYPOTHESIS LEDGER:** **H1** GPU-load starvation — **OPEN, no lever exists, and now `P1`'s ONLY named
  lead — if it comes back clean, P1 has NO named leads.** **H2 — RETIRED-UNKNOWN**
  (appears nowhere in this repo; history unrecoverable; **never re-mint this number** — the entry exists only so
  nobody reclaims it). **H3** auto-exposure active — OPEN, likely, unconfirmed. **H4** occlusion-blind labelling —
  OPEN, named, NOT adopted (see its bullet below).
- **`P6` WIDENS to "annotation.json field-contract defects" — NO NEW NUMBER (A61 applied to phenomena).**
  Three instances: **`node.bounds`** — **SETTLED** (editor-only frustum cube; contract ruling locked; parked as a
  milestone candidate); **`camera.path`** — **OPEN**, naming/contract; **`coverage_pct` = 0 while `coverage_ratio` > 0**
  — **OPEN, PREDICTED FROM SOURCE, NOT MEASURED** (a source read, never observed in any artifact; manifests **only**
  under H4's exact condition, i.e. contingent on an unconfirmed hypothesis).
- ✅ **make_gate_level.py FOOTGUN DEFUSED — `CaptureBench` `8dad64e`** (tools edit, **probe untouched**; the freeze
  is not invoked). The script deleted the asset at `LEVEL_PATH` before authoring, so running it unmodified
  **destroyed the frozen `CB_GateLevel`** that every banked leg and every A54 calibration is measured against. It
  now **refuses by default**, names what is at stake, points at the sibling-level route, and yields only to
  `--allow-overwrite-frozen`. Verified three ways (default refuses / override yields / sibling passes). **G99.**
- ✅ **m23 "P3-fix" SHIPPED — commit `2f74799`, TAGGED `m23`, tag pushed and remote-confirmed.**
  → journal `docs/sessions/2026-08-16-034-m23-p3-fix-and-the-oracle-saga.md` (§7 = the smoke addendum).
  **OWNER PLAY-GATE SMOKE PASSED** in **PIE / StackOBot MainWorld** (`session_20260817-132214`, 90 frames,
  1068×604, fps 30) — verified from disk: 8 blink events, **gapped cadence byte-exact to the historical
  shape**, `manifested: true` 8/8, `non_manifested_events: 0`, **zero** occurrences of the 8-consecutive
  fabrication shape; the tail event `[88,89]` is TRUNCATED per the A50 addendum. First confirmation in
  **real gameplay content** rather than synthetic `CB_GateLevel`. ⚠ **A PIE smoke is an owner sanity gate,
  NOT packaged evidence — G76 stands; m23's certification evidence remains the packaged BenchGate legs.**
- ✅ **`P6` BOUNDS SIDE — SETTLED 2026-08-17 (diagnosis only, NO code change).** → journal
  `docs/sessions/2026-08-17-035-p6-bounds-settled-and-auditor-premise-halt.md`.
  In `annotation.json`, `camera.path` equals the anomaly node's path and `camera.global_position`
  equals `node.bounds.origin` to 13 s.f. **The camera block is NOT mis-sourced** — `labels.jsonl` shows
  all 90 frames reporting the same `view.origin`, a genuine `PC->GetPlayerViewPoint`
  (`AnomalyViewport.cpp:404-408` → `ViewRing` → `AnomalyCaptureSubsystem.cpp:1417`). **The 1010 cube is
  `UDrawFrustumComponent`**, auto-created on the OWNING ACTOR by `UCameraComponent::OnRegister`
  (`CameraComponent.cpp:118-152`): `UpdateDrawFrustum` sets `FrustumStartDist=10` +
  `FrustumDrawDistance=1000` ⇒ `FrustumEndDist=1010` (`:203-212`), and `CalcBounds`
  (`DrawFrustumComponent.cpp:164-167`) returns a box **centred on the camera** with extent
  `(1010,1010,1010)`. It is a `UPrimitiveComponent`, so `GetComponentsBoundingBox(true)`
  (`AnomalyCaptureSubsystem.cpp:159-164`) admits it via **`bNonColliding`** — visualisation-only and
  hidden-in-game do not exclude it. It **contains** the capsule and mesh, so the union is unchanged and
  the centre lands exactly on the camera (**containment, not compromise**).
  ⚠ **CORRECTION — the earlier "unions spring arm / camera / collision" wording is STRUCTURALLY
  IMPOSSIBLE and is struck:** the union iterates `UPrimitiveComponent` only (`Actor.cpp:1685`), and
  spring arms and camera components are `USceneComponent`s. **1010 is a hardcoded editor visualisation
  constant, not geometry.**
  ⚠ **CLIENT-IMPACT DOWNGRADED — this is EDITOR/PIE-ONLY.** The creation is behind
  `WITH_EDITORONLY_DATA`, and the packaged game target defines it **0** (measured in
  `Intermediate\Build\Win64\StackOBot\Development\...\Definitions.*.h`). **Prediction on the record:** a
  packaged capture of a camera-bearing pawn gives order-of-capsule bounds, not 1010. **NOT yet
  measured — the corpus is confounded** (every packaged node with a bounds field is a camera-less
  `StaticMeshActor`); the confirmation run is **DEFERRED by owner ruling**, and the gate level must NOT
  be mutated to enable it. `camera.path` is the **view-target actor** path (`ResolveCameraPath`
  `:102-116`), equal to the node path here only because the anomaly fired on the player pawn — an open
  naming/contract question.
  🔒 **CONTRACT DECISION — RULED AND LOCKED, NOT IMPLEMENTED, DO NOT RELITIGATE:** `node.bounds` must be
  **render-relevant bounds** — the union over components that contribute drawn pixels — never a
  whole-actor union admitting collision capsules and visualisation primitives, and it must reuse the
  existing renderable definition (`IsRenderableComponent`, static-or-skinned, **G33**) so label geometry
  and selection geometry agree on what "the object" is. **Parked as a milestone candidate.** Residual
  that survives even packaged: the capsule is still unioned in, so `node.bounds ≠ mesh bounds` in both
  configs — by a capsule, not by 1010.
  **New rules A59** (MCP-bridge provenance: echo `Paths.project_dir()` + engine version or the
  measurement is not attributed to this project), **A60** (a quantity absent from the artifact is
  operator-supplied or the claim is UNDECIDABLE — never reconstructed, never defaulted, never replaced
  by a weaker test reported as the original), **A61** (a new shape earns a diagnostic tag, never a new
  verdict bucket). **New gotchas G97** (the bridge attaches to whichever editor is listening — a second
  UE project on this box, `HeistCrewUE`/5.7.4, silently captured it; permanent environmental fact) and
  **G98** (`AffectedFrames` is a PROJECTION-FILTERED SET, not a frame range — see the auditor entry).
- ⛔ **DELIVERED-SESSION FABRICATION AUDITOR — CANCELLED 2026-08-18 (not paused). NEVER IMPLEMENTED.**
  → journal `docs/sessions/2026-08-18-036-auditor-cancelled-and-h4-occlusion-recon.md`.
  Owner constraint: **there is no client communication channel in either direction**, so the audit's
  output has **no consumer**. A cold reader who finds the approved plan in journal 035 is looking at
  work that is **closed, not outstanding**. **KEPT AS REFERENCE ONLY** (all in journal 035): the
  verified schema mapping, the three schema traps, the located control sessions, **G98**, and the
  shipped-default observation. **NEG2 stays banked** — that rescue was correct regardless (G92).
  🚫 **STRUCK FROM THE STANDING PLAN — struck, NOT deferred: the office-machine `target_fps` audit and
  the precautionary "cap VideoFps at 30" client note. NO CLIENT-FACING ACTION ITEM SURVIVES ANYWHERE.**
  Any older doc that still lists one — including `CHAT-HANDOFF-s2-i10-and-m23-p3-fix.md` §8 — is
  superseded by this line.
- ⚠ **NEW OPEN HYPOTHESIS `H4` — occlusion-blind labelling. NAMED, NOT ADOPTED, NEVER OBSERVED.**
  (H1 and H3 are minted; H2 appears nowhere, and numbers are never reused, so H4 is next free.)
  **The selection path is occlusion-aware and the label path is not:**
  `IsComponentRenderableVisibleInternal` (`AnomalyViewport.cpp:165-181`) = renderable ∧ poll-radius ∧
  frustum ∧ **`IsUnoccluded`**, while `ProjectActorBoundsToScreenRect` (`:653-685`, called at
  `AnomalyCaptureSubsystem.cpp:1438`) runs **no trace at all** ⇒ a target on-screen but fully occluded
  is **labelled positive while contributing no pixels**. `IsUnoccluded` traces centre+8 corners and
  passes on ANY clear sample, so "fully occluded" = **9/9 blocked**. **Routed to
  `feature/stencil-capture`** — that branch's premise (report actual pixel contribution before hiding)
  is its cure, so H4 **strengthens a locked ruling** rather than opening a lane. **Pre-declared test:**
  a target fully occluded for a whole event window, labelled, pixels unchanged. **RECON DONE, READ-ONLY,
  NO TEST RUN:** `CB_GateLevel` holds **7 targets that are fully occluded AND on-screen** under the
  rigorous cube-occluder-only floor (26/144 realistic, 52/144 upper bound) — **no scene mutation
  needed**, and `make_gate_level.py` **deletes `LEVEL_PATH` before authoring**, so a sibling level must
  rename first. **A47 is a ROTATION bifurcation, not a position one — measured: camera position
  invariant at `(-1500,0,260)` on 369/369 banked gate samples, modal rotation `(0,0,0)` on 278/369** ⇒
  occlusion is stable across the bifurcation by construction (rotation only changes frustum
  membership). Verified name map: **`StaticMeshActor_K` ⇔ grid spawn `n = K−1`** (3 data points).
  Targeted fire **bypasses occlusion** (`TryFireSpecific` has no viewport predicate) **only while
  `IAI.SetViewportScoping` is OFF (its default)** — ON, the anomaly's own `Apply` re-filters and
  selects nothing. Auto-pool **does** exclude occluded actors. **Only path (b)** — fire at an
  already-occluded actor — is producible; **path (a)** (becomes occluded mid-window) is impossible here
  (all-STATIC actors, invariant camera). **A54 would read this as ABSENT — the same verdict as P3 —
  so it detects the symptom, not the cause;** the cause is already instrumented in
  `selection_provenance.json`, ⚠ **but for a fully occluded target `EvaluateSelectionProvenance`
  early-outs**, so the sidecar reads `valid:false` + `0/0` (not 9-blocked) and **`annotation.json`
  ships `coverage_pct = 0` with `coverage_ratio > 0`** — observation only, not a designed
  discriminator. Test design is chat-side; nothing runs until it returns.
- ⛔ *(superseded — kept for the record)* **AUDITOR HALTED AT ITS SOURCE-PREMISE GATE (2026-08-17).** The instrument is meant to decide
  whether the client's already-delivered **pre-m23** sessions carry P3b-fabricated windows. Its
  window-blind design rests on "a gap is mechanically impossible for a fabricated event". **First half
  confirmed** (the pre-m23 fallback does emit `Ev.AffectedFrames` verbatim — `git show m23^`);
  **second half FALSIFIED → G98:** `AffectedFrames` is accumulated only on frames passing
  `ProjectActorBoundsToScreenRect` (`AnomalyViewport.cpp:653-685`), which fails on an invalid view, no
  static/skinned bounds, a box entirely behind the camera, or a rect off-screen — so a target that
  leaves frame mid-window **gaps** the set. **0/1,367 events show it, but that null is CONFOUNDED —
  every banked leg is static-camera; the client captures moving-camera gameplay.** Blast radius covers
  the windowed path too: a fabricated-but-gapped event reads as "strict gapped subset" = GENUINE-SHAPED,
  i.e. **a false blessing in the dangerous direction**. Halted for a chat verdict; no in-turn repair.
  **Also banked this turn:** control #3 (`NEG2\session_20260816-183524`, guard fired 8/8) rescued out of
  the archive-wipe path (**G92**) into `_bench_sessions_bank` (95 files, SHA-256 verified). **Control #4
  (delivery-ON) still does not exist** — 0 of 74 banked sessions has `delivery_mode=true`.
  **Shipped-default observation (report-only, NOT wired in):** the burst window is `PositiveFrames = 8`,
  hardcoded at `AnomalyCaptureSubsystem.h:182`, changeable **only** by the `IAI.Capture.Config` console
  command — no dashboard command, no `config.json` key, no ini (`GConfig` reads only
  `bDeliveryModeDefault` / `ContentClockDefault` / `bFocusGateDefault`). **The window is NOT obtainable
  from the client** (owner constraint: not possible), which is why the design is window-blind.
- ⚠ **m23 as-built (unchanged):**
  **PRODUCTION CHANGED — the "production byte-unchanged" invariant of S2 RETIRES HERE**; from m23 on it is
  *production changes only via approved milestone plans*. 8 files, +103/−34. `IAnomaly` untouched;
  `labels.jsonl` untouched; auto-pool (`TryFireOnce`) untouched.
  **P3a** — blink's half-period is now in **FRAMES** (default **3** = the previous 30 fps cadence,
  byte-exact) instead of seconds, so the toggle no longer depends on `VideoFps`.
  **P3b** — hide-type identity comes from the anomaly **ID** (`IsHideTypeAnomaly`), never from the
  sampling outcome; zero sampled-hidden ⇒ **zero positives** + `manifested:false` + loud warning +
  `non_manifested_events`; an **unregistered id** falls back loudly rather than silently.
  **Targeted-fire args** — tokens after the target on `IAI.Capture.Start` forward verbatim to the
  anomaly's parser (no tokens = byte-identical to before). It exists because **the guard is untestable
  without it**.
  **THE GUARD IS PROVEN BOTH WAYS, and that pair IS the certification:** forced non-manifestation
  (`half_period_frames 40`) → **8/8 events `manifested:false`, zero `frame_indices`, counter 8**, verified
  in the annotation rows; the 30 fps control → guard **silent**, counter 0, cadence byte-exact.
  **CERTIFICATION SCOPE: 30 fps CERTIFIED, floor-robust** (12 decidable ALIGNED / 0 non-ALIGNED under
  BOTH candidate floors). **60 fps FLOOR-BLOCKED — deferred, NOT failed. ≥90 fps P5-BLOCKED.**
  ⚠ `positive_frames` stays **fire-active** and is unchanged by design — **fire-active ≠ manifested**;
  the client artifact carries neither per-frame flag (delivery gates `labels.jsonl` entirely), so
  event-level `manifested` is the only channel to the client.
  New rules **A57** (floor-robustness: certify only what is invariant across all defensible calibration
  constructions; a set that *brackets* a regime without *containing* it yields no floor) and **A58**
  (diff-isolation rules are invariants-to-preserve, never confinement predictions; when a brief
  contradicts itself take the conservative branch and flag it). New gotcha **G96** — oracle blindness is
  exposed only by known-answer controls, three instances one principle.
  **P5's founding instrument is assigned** (the blend-ladder from the certified 30 fps leg) and is also
  the eventual source of DA60's deferred floor. Not built.
- 🔴 **P3 DIAGNOSED — MECHANISM ADOPTED (superseded by m23 above; kept for the diagnosis record).** → journal
  `docs/sessions/2026-08-16-033-p3-mechanism-adopted.md` (2026-08-16). **P3 is TWO stacked defects:**
  **P3a (timing)** — `FAnomaly_Blinking` accumulates forwarded tick dt (`Anomaly_Blinking.cpp:58-69`)
  against `HalfPeriodSeconds = 0.5/Hz` (:44, default 5 Hz ⇒ 0.100 s), and under capture that dt **is**
  `1/VideoFps` (`AnomalyCaptureSubsystem.cpp:672`, clamp :442). When a window's ticks × (1/fps) never
  reach the half-period the toggle never fires and **the actor is never hidden** (:76) — the pixels are
  CORRECT, the scene truly has no anomaly. `Revert` resets the accumulator (:85-98), so no phase carries.
  **P3b (labelling, anomaly-agnostic, the amplifier)** — `:1466` infers `bHideType` from the sampling
  OUTCOME (`HiddenIdx.Num() > 0`) and `:1467` **silently substitutes `Ev.AffectedFrames`**, turning a
  non-event into a full-window block of positives. **Fingerprint: genuine hide sets are GAPPED
  (`[4,5,9,10]`); the fallback shape is CONSECUTIVE (`[3..10]`).** → **G94**.
  **DISCRIMINATORS:** D-A fps bisection — a **THRESHOLD between 90 and 120 fps, not a gradient**
  (60fps 12 ALIGNED, 90fps 6 ALIGNED, 120fps 13 ABSENT) with **zero SHIFTED events anywhere**; a
  survey-derived 60–90 expectation was stated in advance and **FALSIFIED**. D-B grab-point test — true
  dual capture, marker-matched, one oracle: **backbuffer and SceneColor agree to four decimals, both show
  the object VISIBLE during labelled windows ⇒ P3 is scene-level and grab-point-independent; THE SVE
  MIGRATION WOULD NOT CURE IT.** D-C — `missing_object` (hides in `Apply`) **MANIFESTS at 120 fps, 8
  ALIGNED** ⇒ P3a is pinned on the toggle clock. **G95** (a second capturer's write load starves the
  production writer; overlapping captures need the focus gate managed).
  **FIX DIRECTION (plan only, no code yet):** **F-LABEL** — zero sampled-hidden ⇒ **zero positives**, row
  kept with additive `manifested:false` + loud warning + session counter; hide-type identity from
  existing routing, never from sample outcomes; `IAnomaly` LOCKED. **F-BLINK** — half-period in **FRAMES**
  (integer), **default 3**, reproducing today's 30 fps cadence exactly; 60/90 fps cadence changes and that
  is accepted. **SEQUENCING RULING: P3-fix lands and validates BEFORE S3** — client-facing dataset
  poisoning outranks the internal migration, and D-B proves the SVE is orthogonal.
  **OPEN, evidence-only: P4-CANDIDATE** — the D-C leg shows 41/96 shift-0 mismatches while all 8 event
  edges are ALIGNED (a tail-length disagreement), **distinct from P3 and from P1, not conflated**.
  New rules **A53** (any oracle/analysis change re-verifies against one known-ALIGNED and one
  known-ABSENT control first) and the **A50 TRUNCATED addendum** (events cut by the frame cap are
  classified TRUNCATED and excluded from taxonomy counts).
- 🔴 **THE REPRODUCTION ITSELF — "P3": at `VideoFps` 120/240 a hide-type anomaly window is LABELLED IN
  `annotation.json` BUT NEVER APPEARS IN THE PIXELS. 49/49 events across four legs, zero manifested.**
  → `docs/sessions/2026-08-16-032-s2-high-fps-sweep-and-p3-reproduction.md` (2026-08-16).
  99 labelled-positive frames per leg, target plainly visible in every one (eyes-confirmed) —
  **dataset-poisoning severity.** **DO NOT CAPTURE HIDE-TYPE ANOMALIES ABOVE 30 fps until this closes**;
  treat any existing high-fps session as suspect. Legs: HF1 120fps ratio 1.6916, HF2 240fps 3.8262,
  HF3 120fps+26ms 3.3684, HF4 240fps+24ms 6.1092 — **all ABSENT**.
  ⚠ **NOT the m21 stale-present mechanism**, and three discriminators say so (**A51** signature kit):
  identity `label.frame_index == marker gfc` **590/590 diff 0**; adjacent-duplicate scan **0/149
  byte-identical AND 0/149 near-identical** on every leg (frames are fresh); claimed-hidden frames at
  **−1.8..+3.5 robust sigmas** vs **+22..+29** at 30 fps. Perfect pairing, fresh frames, anomaly state
  never reaching the rendered scene. **NO MECHANISM CLAIM.**
  ⚠ **`VideoFps`-SCOPED, NOT RATIO-SCOPED (A52):** HF1 at ratio 1.69 and HF4 at 6.11 are equally ABSENT
  while the 30-fps legs at ratio 3.0027/3.4840 were perfectly ALIGNED — same target/seed/anomaly/binary.
  🔎 **m21 ARCHAEOLOGY — the residual was P3 filed as P2, and its attribution was confounded.** Journal
  027's R7 (`blinking@240`, "pixels never show the hide at all") **is P3**; R3/R6 were also at **240**.
  Every m21 residual run was at `VideoFps 240`, so "deep starvation (ratio ≳3) → stale scene" conflated
  ratio with fps — **our 30-fps deep legs hit ratio 3.0027/3.4840 and were clean on all three
  discriminators.** Journal 027's "the staleness is change-type-dependent" inference is SUPERSEDED.
  **THREE PHENOMENA, tracked separately from here:** **P1** the client's one-frame shift @ratio≈1.2,
  30fps — **NOT reproduced** (12 legs); **P2** stale/duplicate present — **signature absent** here;
  **P3** labelled hide never manifests — **REPRODUCED**, fps-dependent.
  New rules **A48** (config echo — report the EFFECTIVE value from independent read-backs, never the one
  requested), **A49** (pre-declared regime windows where A40's bands don't apply), **A50** (per-event
  taxonomy ALIGNED/SHIFTED(N)/**ABSENT**; ABSENT counts as reproduction and the oracle must be *able* to
  say it), **A51**, **A52**. New gotcha **G93** (`FocusGate 0` + high fps corrupts the camera — neither
  alone does; keep the focus gate ON above 30 fps). **Also measured: above the box's sustainable capture
  rate `speed_ratio` stops being a dial and becomes a readout** — natural frame cost 12.7–17.8 ms here, so
  ratios 1.5–2.1 (120fps) and 3.2–4.1 (240fps) occur with ZERO induced load, and the nominal/client bands
  are unreachable there. Still open and NOT closed by this: **H1** (GPU-load — both levers are CPU
  busy-waits), the **delivery-mode gap** (every I10 leg ran delivery OFF), **A47**.
- **IN FLIGHT — SVE capture migration, stage S2 (render-thread keying design). Production BYTE-UNCHANGED;
  production still captures via the BACKBUFFER; no S3 work started.** → journals
  `docs/sessions/2026-08-06-030-s2-keying-design-and-gate-environment.md` and
  **`docs/sessions/2026-08-16-031-s2-i10-game-lever-and-render-lever-provenance.md` (latest)**.
  **I10 GAME-LEVER LEGS DONE — THE CLIENT'S DEFECT DOES NOT REPRODUCE UNDER GAME-THREAD STARVATION.**
  Six targeted single-anomaly hide-type legs (blink on one measured-prominent actor) across every required
  A40 band — nominal 1.0000, mild 1.0558, **client 1.2148 and 1.2342**, **deep 3.0027**, **pacing-off
  0.3312** — gave **44 hide events, 494 frames, ZERO misaligned frames**, with a live positive control (a
  one-frame shift costs 19–30 mismatches per leg). An independent identity check agrees:
  `labels.jsonl frame_index == the marker's decoded GFrameCounter in the pixels` on **532/534** frames, in
  every regime. **Three of the four frozen predictions FAILED** (client/deep/pacing-off all predicted −1);
  by the pre-declared rule the clean CLIENT band is a **MAJOR RESULT** — her defect has a different
  mechanism. ⚠ **This licenses exactly one claim** — "does not reproduce under GAME-THREAD starvation, this
  box, this level, VideoFps 30" — and **no mechanism claim**. **Deep starvation stays OPEN**: ratio 3.0 was
  reached by a *game-thread* stall, which by the concurrency model never starves the renderer, so it cannot
  have tested the present-side m21 residual. The high-`VideoFps` (120/240) legs are still owed.
  **RENDER LEVER FIXED AND CHARACTERISED.** Root cause was **binary provenance, not code** (G92): the fix
  *was* compiled at 11:53:58, 36 s before the legs — but never **staged**, so the package kept serving a
  2026-08-06 exe. Re-staged, A44-verified by string scan, and it fires with **zero probe edits**
  (`fired=780` at 40 ms; 0 at 0 ms). ⇒ **`speed_ratio` is NOT blind to render-side starvation** — the near-miss
  major finding is refuted. Sweep `0→1.000 · 20→1.000 · 30→1.116 · 40→1.407 · 70→2.308 · 110→3.507`;
  **one model for both levers**, `frame_time ≈ max(1/VideoFps, stall + residual)`, residual **1.3 ms game /
  6.9 ms render**, knee **32.0 / 26.4 ms**. ⚠ **Corollary: the ratio cannot attribute** — a client's 1.2 says
  frame time ≈ 40 ms and nothing about which thread starved. **The counter story is CORRECTED**: `163dd12`'s
  counters were never in the binary, so "stall_fired=0" was never a reading — ratio arithmetic made that
  catch. *"A counter that never printed is not a counter that printed 0."*
  **RENDER I10 LEGS ALSO CLEAN — six for six in-band on the FIRST attempt, zero retries** (nominal
  1.0000, mild 1.0815, **client 1.2145 / 1.3071**, **deep 3.4840**, pacing-off 1.4317): 43 events,
  480 frames, **0 misalignments**, identity **520/520**. Bands were declared chat-side from the sweep
  before any leg ran; the defect outcome was **UNPREDICTED**.
  ⇒ **COMBINED across both I10 sets: 87 hide events · 974 oracle frames · 0 misalignments · 1052/1054
  identity frames at diff 0. "CPU starvation breaks the arm→present pairing" is REFUTED for this
  instrument** — nominal through deep, either thread, pacing-off from both sides. ⚠ **Pacing-off is
  TWO regimes**: game-set L5 free-ran FAST (0.3312), render-set R5 was render-limited and ran SLOW
  (1.4317) — the slow one is closer to a struggling client box. **Still NOT ruled out and named in
  advance:** H1 (GPU-load shape — no lever exists for it), the high-`VideoFps` 120/240 pacing-ON shape
  where the m21 residual was actually seen, and **the DELIVERY-MODE GAP — both I10 sets ran delivery
  OFF because the oracle needs the `labels.jsonl` bbox that delivery suppresses, while the client
  captures in delivery mode; closing it needs a delivery-compatible oracle.**
  Precisions on the record: the analysis-window start is a **wall-clock ~570 ms luminance ramp**, not a
  frame count (5 frames at 116 ms/frame, 16 at 36 ms/frame); `fired=N` is **process-lifetime and
  quantised to 60** by the `%60` log filter, so it reads "the lever executed", never "N times during
  capture"; the A47 camera bifurcation has now occurred **once in twelve legs** and is NOT retired by
  these six.
  ✅ **A20 item 4 was DISCHARGED by `fbf8ad1`** (the `GameDefaultMap` bullet in PRE-DELIVERY-CHECKLIST
  §1 — that commit calls it the "delivery-checklist guard"); the debt had been carried forward wrongly
  by the handoff, journal 030 and early drafts of 031. Struck.
  New standing rules **A44** (prove the change is in the binary; string scan, not timestamp), **A45**
  (a valid marker read is a strictly increasing series — the decoder confidently misreads markerless
  frames), **A46** (kill by process name + assert an idle box; the 217 KB launcher trap already produced one
  wrong ratio), **A47** (per-leg bbox — the gate level is content-deterministic, NOT camera-deterministic).
  New gotchas **G90/G91/G92**. Banked sessions were moved out of the package before re-staging, to
  `D:\IntrusiveAnomalies\_bench_sessions_bank` (1347.2 MB) — the archive step wipes that tree.
  **B′ is LOCKED**: a key-only ring — the game thread publishes at **`BeginRenderViewFamily`** (the ONLY
  hook where `ViewFamily.FrameNumber` is assigned; `SetupViewFamily` still reports `UINT_MAX`), the
  render-thread pass looks up by `View.Family->FrameNumber`, the label snapshot never crosses threads, and a
  lookup miss is loud (counted, warned, frame dropped — never labelled by guess). Measured: counters advance
  1:1, ring round-trip 100%, forced-miss 26/26 warned.
  **Gate environment now exists and is characterised** — synthetic bench level `CB_GateLevel`, cooked to
  `Builds\BenchGate` (**NOT deliverable**), reached by command-line map argument so the host project config
  is never touched. See **G87–G89** for the whole saga (packaged runs always boot MainMenu; loose config
  beside a package is ignored; black-level, exposure-pin and cost-model corrections).
  **Game-thread stall→ratio table BANKED** (dev-box instrument, not a portable spec): knee 30→34 ms and
  sharp, ratio **3.004 at 99 ms**, `frame_time ≈ stall + 1.3 ms`. *(Re-verified on the rebuilt binary:
  0 ms→1.0000, 99 ms→3.0242. ⚠ Those banked legs ran **marker-OFF** — the decoder's constant-`0`@row-105
  false signature — so they carry no frame-identity evidence of their own; an A17/A19 input.)*
  **Render-thread lever now WORKS and is characterised — see the 031 entry above.**
  **Open and NOT drifting toward "fine":** deep starvation (m21 residual never reproduced, and the I10 deep
  leg could not test it — game-thread stalls do not starve the renderer), the latch lifetime rule
  (unbuilt), the captured-view matrix equality check (ViewRing/`ViewLagFrames` deletion is blocked on it).
  **Nothing shows the migration FIXES the client's defect** — I10-game has now run and came back CLEAN,
  which means the defect's mechanism is still unlocated; a render-side reproduction is the next place to
  look, and it must exist before any fix can be shown to remove it.
- **Prior milestone (as-built): m22 — blink subtype pin + annotation traceability + selection provenance —
  COMPLETE (tagged `m22`, pushed).**
  Three commits on `master`: `af8d937` `feat(blinking)` + `03a51d5` `feat(capture)` + `28bc6f1` `feat(capture)`.
  **(1) Subtype pin** — the per-event subtype derivation is DELETED; `blink` now emits
  `anomaly_subtype = "disappear_reappear"` **always**, however many toggles the event contains. **Blink's own
  behaviour is UNCHANGED and is CORRECT** — multi-toggle within a window is intended, not a defect. `"flicker"`
  leaves the blink family entirely and is **reserved for the future separate `flickering` class** (unbuilt);
  the `anomaly_subtype` FIELD is retained so that class has a slot and the client file shape doesn't churn twice.
  **(2) G81 FIXED** — `affected_frames.frame_count` was a SPAN (`end-start+1`, reporting 7 for 4 real indices on
  gapped events); it is now a TRUE COUNT = `len(frame_indices)`. The span stays recoverable from
  `start_frame`/`end_frame`. Confined to `WriteSessionAnnotation`; the m18-validated labels/range-builder path is
  untouched. **(3) B1 traceability** — `affected_objects.nodes[]` gains `asset_name`, `component_class` and
  `bounds{origin,extent}` so auto-named `StaticMeshActor_###` level actors are identifiable (verified:
  `StaticMeshActor_0` → `SM_SlopeWarpLandscape`). ⚠ **Anchor-frame semantics — sampled ONCE at the event's first
  captured frame, NOT per-frame truth** (documented in architecture.md). **(4) B2 selection provenance** — new
  `AnomalyViewport::EvaluateSelectionProvenance`; `coverage_pct` → **annotation.json (client-visible, both
  modes)**, occlusion samples passed/total + poll distance → new **`selection_provenance.json` sidecar
  (internal, suppressed in delivery mode)**. **OBSERVATIONAL ONLY, enforced STRUCTURALLY: the selection path has
  ZERO edits** (both `AnomalyViewport.cpp` hunks are pure insertions; the auto-injector/selector are untouched),
  so the early-out boolean still decides selection. Gates: seeded selection identity (seed 4242, 8 events,
  two runs byte-identical), delivery suppression, `frame_count == len(frame_indices)` on all 10 events, blink
  subtype pinned on `runs=2` events, and the m20 trailing reappear frame still present.
  **TWO DEVIATIONS from the approved plan, owner accept/reject pending:** B2 is a standalone evaluator rather
  than an opt-in out-param threaded through `ClassifyRenderableVisibleLive` (so "observational" holds by
  construction and costs 9 traces per EVENT, not per candidate per poll — but there is no runtime ON/OFF toggle,
  so that gate is discharged structurally); and the internal half went to a finish-time sidecar rather than
  `run.json`, because `run.json` is written at StartRun **before any event exists**.
  **CLIENT-FACING:** `frame_count` and blink `anomaly_subtype` change value in files the client already receives.
  → `docs/sessions/2026-07-29-029-m22-subtype-pin-traceability-provenance.md`.
- **Previous HEAD before m22 = `ed2b851`. Latest TAG = `m21` = `a2c3127`** (m22 is not tagged yet).
  **Post-m21, untagged, on `master`** (all shipped/pushed): `3e5e455` + `a4a8862` docs(client-readme) — Setup/Run flow,
  shared captures folder, ffmpeg troubleshooting; `3a46c1f` fix(control-server) — reply `{type:"error",
  code:"bad_token"}` before rejecting a peer; `9c46ef5` docs(gotchas) **G83** — 5.1 `INetworkingWebSocket` has no
  `Close`, the error reply is the only bad-token signal; `6d01bc9` docs(delivery) — PRE-DELIVERY-CHECKLIST + client
  docs migrated to a runtime `config.json`; `ed2b851` docs(delivery) — desktop-app delivery (WebView2, SmartScreen,
  Python-for-encoder) + **G85**. The **dashboard M1–M3 (Tauri `Dashboard.exe`)** work lives in the SEPARATE AnomDash
  repo, not here.
- **NEXT MAJOR TRACK (approved 2026-07-29): the SVE capture migration** — see
  `docs/sessions/2026-07-29-028-capturebench-s1-and-traceability-plan.md`.
  **NEW LOAD-BEARING REQUIREMENT: label correctness must be RATIO-INDEPENDENT.** A real client session at
  `speed_ratio` **1.2** produced a confirmed **−1** on a `missing_object` boundary (effect frame 50, annotation 51) —
  the **1.1–1.5 band that was never measured** (m21 validated ratio≈1.0 exact and ratio≳3 residual; this sat between
  the two measured points). Clients capture on their own hardware and **will not be asked to discard or re-capture
  sessions**. ⇒ **The `speed_ratio ≤ ~1.05` SHIP RULE IS DEMOTED from correctness gate to INTERNAL TELEMETRY**, and
  retires entirely once the SVE migration lands. **The SCENE-IDENTITY-MARKER proposal is SUPERSEDED and dead** —
  the SVE grab point solves the same problem structurally (it reads the frame being rendered, so there is no
  arm→present matching to get wrong). ⚠ **That dead proposal once claimed the name "m22"; it was NEVER tagged,
  and the m22 number is REASSIGNED to the blink-subtype/traceability milestone below. Refer to the dead
  proposal by NAME (scene-identity marker), never by number.** **UI decision: UI on/off ships as a GRAB-POINT CHOICE — SVE = UI-free,
  backbuffer = UI-on. No UI-isolation work** (SDR has no isolated UI layer: `GetCompositeUIRenderTarget()` is
  HDR-gated, `SlateRHIRenderer.cpp:980-991`). **Ground-truth contract AMENDED: UI presence is per-run config**
  (proposed delivered default UI-off, pending client sign-off). 8-bit delivered color stands; the typed FP16/FP32 path
  lands **with depth**. **`Plugins/CaptureBench/` is a PERMANENT non-shipping perf-regression harness** (own plugin,
  gated `CAPTURE_BENCH`, never ships; it is NOT part of this repo).
- **STANDING TEST BASELINE (owner directive, 2026-07-15): validate against a LOCAL PACKAGED BUILD under
  `D:\IntrusiveAnomalies\StackOBot\Builds`, not just PIE.** The editor masks packaged-only behavior — both
  first-smoke-test bugs (packaged black preview; missing_texture stuck revert) were invisible in PIE. A package runs
  fully headless: `StackOBot.exe -windowed -ExecCmds="IAI.Server.Start, ..."` + the control server's own WS surface as
  the driver; iterative cook + exe hot-swap = ~1 min edit→validate loop. See **G76**.
- **Latest milestone (as-built): m21 — deterministic arm→present pairing — COMPLETE (commit `a2c3127`, tagged `m21`,
  pushed) (2026-07-15).** *(Corrected 2026-07-29: this entry previously read "IN FLIGHT … NO COMMIT this turn" and
  stayed that way for six subsequent commits. m21 shipped.)* The owner's −1 (annotation/labels one earlier than
  pixels) was **not delivery mode and not content clock** — both refuted by A/B + code (the clock only reaches the fps
  stamp; the dev box was already running WALL: no `[AnomalyCapture]` ini section, engine default Wall since m15). And
  it was never an annotation bug: **labels.jsonl shifts too.** **Real variable = the rate regime:** paced+sustainable
  (`speed_ratio`≈1) = exact; starved (ratio≫1) OR pace-off (ratio 0.38, running FAST) = content lags index by one —
  the m11 pacer's sleep was the only thing making the old pairing correct, and every m18/m19/m20 validation ran in
  that one masking regime. **Measured (STEP 1):** arm-id→consume-rtframe delta = d=2 (99%) at ratio≈1, d=1 (100%)
  starved/pace-off, **MIXED 88/12 within one starved run** → no fixed delta exists; pairing must be by identity/order.
  **Fix (~18 lines, `AnomalyFrameCapturer.{h,cpp}` only):** arm registration now rides the render-thread command
  stream (`ENQUEUE_RENDER_COMMAND`, weak-ptr guarded) → FIFO-ordered after present(N−1), before present(N) → "next
  present wins" deterministic in every regime; `armRt` telemetry added to the armed-frame log. Preview tee inherits
  (shared class). **Post-fix:** pace-off **FIXED** (0/100, was −1); ratio 1.05 exact; ratio≈1 byte-identical
  (pattern/cadence match m20); pairing telemetry `consume−armRt=0` 100/100 even at ratio 3–5. **⚠ RESIDUAL EXPOSED
  (open → proposed m22): under DEEP starvation (ratio≳3) the presented backbuffer can carry a STALE SCENE** — a
  one-time mid-run event permanently shifts content −1 (missing_texture) while pairing stays perfect, and
  render-STATE changes are worse: an 8-tick hide window (game-state-proven via annotation) **never appeared in any
  presented frame** (blinking@240, visually confirmed). No arm-side pairing can fix content the present never
  contained → the proposed **scene-identity marker**. **⚠ SUPERSEDED 2026-07-29 — that proposal is DEAD and was
  never tagged; the SVE migration replaces it** (it reads the frame being rendered, so no arm→present matching exists
  to get wrong). *(It once claimed the name "m22"; that number now means the blink-subtype/traceability milestone.)* **The old SHIP RULE (`run_summary.speed_ratio ≤ ~1.05 & paced` → trust) is DEMOTED to internal
  telemetry** — it is NO LONGER a correctness gate, because a client session at ratio **1.2** shifted anyway and
  clients will not re-capture. Honest gates as of m21: G2/G3/G4/G6(ratio≈1) GREEN; **G1/G5 (deep starve) NOT green** —
  improved but residual, deferred with evidence. G82.
  → `docs/sessions/2026-07-15-027-m21-arm-present-pairing.md`.
- **Latest milestone (as-built): m20 — annotation.json labeling — COMPLETE (commit `c01a214`, tagged `m20`, pushed)
  (2026-07-15).**
  Three reported annotation bugs; measured against PIXEL ground truth in a package. **One was NOT a bug and two had
  different mechanisms than first diagnosed** — the fixes below are what the evidence supports (the brief's own
  pixel-exact gates are what prove it). **Bug A (missing_texture range shifted one earlier) = NOT A BUG, no code change.**
  annotation is NOT a separate path from labels.jsonl: `BuildLabelRecordForSnapshot` (:800) and `AccumulateFrameEvents`
  (:802) are ADJACENT LINES fed by the SAME m18-corrected `Snap`. Measured: annotation `[3..10]` == pixels == labels, zero
  disagreement; index 3 ↔ `frame_00003.png` ↔ visibly checkered. **Cause of the field report = 0-BASED vs 1-BASED
  numbering** (pipeline is 0-based, `frame_%05d.png` + encode_watcher :17-18; a 1-based player shows index 15 as "frame
  16" = the exact reported +1). **The tell: pre-m18 the range ran one LATE and accidentally matched a 1-based read; m18
  made it 0-based-exact and "broke" that read.** Shifting +1 would RE-INTRODUCE the m18 bug in the client deliverable →
  **OPEN owner question (blocks any change): which artifact was "actual 16..20" read from? Decisive test = open
  frame_00015.png vs frame_00016.png; the first corrupted one IS start_frame.** If the client's tooling is 1-based that's
  a SPEC change (all indices + docs + slicer contract), never a sample shift. **Bug B (blinking "tail clip") = blinking's
  hidden state was ONE GAME TICK STALE** — there is no range end to fix (`frame_indices` is emitted verbatim,
  AnomalyLabelWriter.cpp:345-355). Measured `annotation(G) == pixels(G-1)` on every blink edge: blinking toggles in the
  INJECTOR's tick (AnomalyInjectorSubsystem.cpp:169-173), a different tickable running AFTER capture, so m18's
  end-of-our-Tick sample predates it. (`missing_object` immune — hides in `Apply` inside our Tick. ⇒ hide-type splits:
  Apply/Revert-driven = correct; self-ticking = stale. This is the m18/G78 predicted risk, now measured.) **FIX: new
  `SampleDeferredHidden()` fills `FireHidden` at the TOP of the next Tick** (before `ProcessCompletedFrames`; also at
  `FinishRun` top). **Zero blast radius on labels.jsonl — `FireHidden` never reaches it.** **Bug C = TWO independent
  causes, NOT B's root** (a uniform shift preserves transition counts, so B's fix changes C by nothing): (C1) the
  transition count was ORDER-DEPENDENT (accumulated on arrival, but `Drain_RenderThread` appends REVERSE →
  out-of-order → spurious transitions → "flicker") → now `TMap HiddenByIndex` + derive hidden set AND transitions from
  the SORTED keys at write time; (C2) `MapAnomalyToClient` else-branch never set a subtype → now mirrors the type.
  *Note: "single vanish → flicker" did NOT reproduce — `DefaultHz=5.0f` (6-frame period) means the default 8-frame window
  genuinely holds ~2 hidden blocks, so "flicker" is CORRECT there.* **Gates G1–G6 GREEN in a LOCAL PACKAGE, pixel-exact:**
  G1 annotation==pixels==labels `[3..10]` (already true on m19); G2 blinking annotation `{4,5,9,10}` == pixels `{4,5,9,10}`
  exactly (was `{4,5,6,10}`), tail frame included; G3 single-vanish window → **14/14 `disappear_reappear`**, default
  multi-toggle → `flicker` (derivation intact, still data-driven), missing_texture → `missing_texture`; G5 **labels.jsonl
  BYTE-IDENTICAL to m19** (same pattern/cadence/counts — m18 undisturbed); G6 overlay clean (65 present / 65 boxes / 0
  present-no-box). **PARKED (owner D1): the shared range-builder refactor** — single source of truth so this off-by-one
  class can't recur; deferred post-delivery (touches the m18-validated path). *Note: annotation + labels already share one
  snapshot and one accumulator, so the drift risk is lower than feared — it's an emit-layer consolidation.* **ALSO PARKED
  (found, unapproved): `frame_count` is a SPAN not a count** (`End-Start+1`, AnomalyLabelWriter.cpp:349) — measured 7 vs 4
  indices for gapped hide-type sets; ships in the client deliverable; owner decision. G81. **SHIPPED** — one
  `fix(capture)` commit `c01a214` + tag `m20`, pushed; dashboard untouched. *(Post-ship: the "Bug A" question was
  pursued through two more owner hypotheses — delivery mode, then content clock — both refuted; the real root is the
  m21 arm→present pairing race above, which also explains why the owner's box showed −1 while this box did not.)*
  → `docs/sessions/2026-07-15-026-m20-annotation-labeling.md`.
- **Latest milestone (as-built): m19 — preview backbuffer tee + targeting defaults — COMPLETE (commit `c8aa3fa`, tagged
  `m19`, pushed) (2026-07-15).**
  Fixes the **black dashboard preview in ANY packaged build** (delivery-gating: no client build had a working preview).
  **Two premise corrections, both evidence-backed (docs must not restate the myths):** (1) the preview was **NEVER
  editor-gated** — no `WITH_EDITOR` guard exists; the only guard is `ANOMALY_CONTROL_SERVER` (= not-Shipping), so the
  packaged build compiled AND RAN it; (2) frames were **NOT "never generated"** — 117 frames/20 s were generated,
  encoded and SENT, each **exactly 15027 B**, decoding to a valid 1280×720 image of mean **0.00** = BLACK. ⇒ **a
  "frame counter increments" gate PASSES on the broken build; gate on PIXELS.** Cause: a packaged viewport has no
  render target → `ReadPixels` zero-fills and reports success (G79). **Fix:** the preview tees off the same
  `OnBackBufferReadyToPresent` stream as capture via its **own** `FAnomalyFrameCapturer` instance (new
  `FAnomalyPreviewTee`) — it cannot share capture's grab (m16 makes them mutually exclusive in time; the capturer's
  arm/queue are single-consumer). Capturer class **unmodified**. `UAnomalyCaptureSubsystem` owns the tee + a 3-method
  facade (`PreviewPump`/`PreviewArm`/`PreviewPoll`) so the control server gains **no render deps**. **m16 suppression
  gates the ARM, not just the send** (pump still drains-and-discards so an in-flight readback can't leak). Encode
  off-thread; WS send stays on the game thread; **no `FlushRenderingCommands` left in the path**; `ViewEpoch` stamped
  at arm. `ConvertTightToBGRA` promoted to `namespace AnomalyLabel` so capture + preview share ONE conversion.
  **Dashboard UNCHANGED** (AIF1 format identical). **Gates in a LOCAL PACKAGE:** G1 preview WORKS — pixel-verified,
  ~5.8/s, JPEGs ~71 KB and 60/60 **distinct** (vs the constant-15027 B black fingerprint), real scene decoded, and the
  `InRHITexture` ensure is GONE; G2 suppression — **13.2 s capture running → 0 preview frames**, 51 after; G4 capture
  **byte-identical to m18** (100 frames / 65 pos / same pattern / same cadence / same annotation `[3..10]` / 9 events);
  G5 format `fmt=18` `PF_B8G8R8A8` correct on StackOBot; G6 no render-thread stall, ~5% fps delta **inside the noise
  floor** (baseline itself 68–105 fps). **G3 (PIE) OWNER-PENDING** — needs the editor; same hook/rect/capturer class
  that capture already proves in PIE. **HONEST PERF (no overclaim): m19 does NOT speed up capture** — m16 already
  suppressed preview during captures; the win is a working packaged preview + no ~6 Hz game-thread flush OUTSIDE
  capture (structural; unmeasurable on the light local scene). **KNOWN LIMITATION — Concorde/HDR format is an owner
  post-push check** (mirrors m17): `ConvertTightToBGRA`'s `default:` branch returns BLACK, so any title whose captures
  are correct will preview correctly (Concorde's captures are good ⇒ inference, not measurement); a true HDR
  `PF_FloatRGBA` swapchain would look washed, not black. **Exact place to add a format/tonemap step if needed:
  `AnomalyLabel::ConvertTightToBGRA` — it now fixes capture AND preview at once**; the `Preview(tee): first backbuffer
  frame (fmt=…)` log makes it a 5-second check.
  **BUNDLED INTO m19 (owner) — NEW TARGETING DEFAULTS: coverage `0 → 6`%, poll radius `0 → 1800` cm (18 m), auto-pool
  default-enabled `{missing_object, blinking, missing_texture} → {blinking, missing_texture}`.** All three were
  **hardcoded engine constants** (`GPollRadius`/`GMinScreenCoveragePct` in `AnomalyViewport.cpp`; a new
  `GAutoPoolDefaultEnabled` consumed by `AnomalyAutoInjectorSubsystem::Initialize`) — **not** ini-backed (GConfig
  remains a follow-up option). **The ENGINE IS AUTHORITATIVE and the dashboard has NO defaults of its own** — its
  sliders/checkboxes are pure snapshot mirrors (`session.pollRadius`, `session.minScreenCoverage`, `auto.pool[id]` ←
  `IsAnomalyEnabled`), so **zero dashboard changes were needed and the dashboard repo stays untouched at `f978f1b`**;
  adding a UI-side default would have created a second source of truth that could drift. `missing_object` stays
  **selectable, just not default-on**; `SetAllAnomaliesEnabled(true)` still means all of `GAutoPool`. **UNITS:** poll
  radius is **cm** (18 m = 1800); coverage is **percent 0–100** (6% = `6.0f`). **Verified in a FRESH PACKAGED session,
  no dashboard:** `1800.0 cm (cull ON)` / `6.00% (cull ON)` / `enabled pool (2): blinking, missing_texture`; snapshot
  parity confirmed. **6% was measured, not guessed** — the Bot is 9.98% of the viewport (= `annotation.json`
  `coverage_ratio: 0.10026`), so 10%/12% CULL THE TEST SCENE'S HERO CHARACTER; 6% keeps it (15→5 targets, Bot in).
  **⚠ The MainMenu set collapses to 1 under the POLL radius (not coverage) — a MENU-MAP ARTIFACT:** the poll cull is
  pawn-relative (G34) and a menu map's pawn is nowhere near the menu camera; in a real gameplay level the pawn IS the
  player. **The poll default therefore cannot be judged in this map — owner sanity check wanted in a gameplay level /
  on Concorde.** G80. **SHIPPED** — one `feat(capture)` commit `c8aa3fa` + tag `m19`, pushed; dashboard untouched
  (`f978f1b`) because it has no defaults of its own. → `docs/sessions/2026-07-15-025-m19-preview-backbuffer-tee.md`.
- **Prior milestone (as-built): m18 — burst-boundary label alignment (async label stamp → end of tick) — COMPLETE
  (commit `4559c8c`, tagged `m18`, pushed) (2026-07-15).** Fixes the ~17% label/pixel misalignment found while validating m17 —
  same dataset-poisoning class, and it hit EVERY anomaly type. **Mechanism:** `CaptureCurrentFrame()` sampled the label
  from `GetLiveFires()` at mid-tick N, but `BeginFire()`/`BeginRevert()` run LATER IN THE SAME `Tick`, and the **async**
  grab (default) returns the render of the ARM TICK ITSELF (frame N) → the frame armed on a transition tick renders the
  POST-transition world while its label described the PRE-transition one. Measured: pixels positive [3..10] vs labels
  positive [4..11] → **the label span ran one frame LATE**; fire edge = labeled clean / pixels anomalous (**false
  negative — a "clean" example containing the bug**), revert edge = labeled positive / pixels clean (false positive).
  Scale = **2/(PositiveFrames+PostFrames) = 16.7%** at defaults (17/100 measured), independent of settle-K and pre.
  **⚠ DIRECTION NOTE: the brief's diagnosis ("pixels change on N+1 → shift the span LATER") was the exact inverse of the
  measurement; the fix shifts the span one frame EARLIER.** The render thread's lag is a THREAD lag, not a state lag — a
  change at end-of-tick-N lands in frame N's own render. **Fix (async-only; `AnomalyCaptureSubsystem.{h,cpp}` only):**
  the arm stores the snapshot WITHOUT the fire state; new **`FinalizeArmedLabel()`** (last statement of `Tick`, after
  the phase switch) fills `Fires`/`FireHidden`/`FirePos` post-transition. **Hide vs non-hide needs no special-casing** —
  `anomaly_present`, the bbox/`bbox_valid`, `AffectedFrames` (non-hide `frame_indices`) and `HiddenIndices` (hide-type)
  all derive from that ONE sample, so every surface shifts coherently and `visible_positive` stays consistent by
  construction. **Sync is untouched and was already correct** (its `ReadPixels` returns the PREVIOUS frame — the same
  fact L=0 rests on); **phase timing, settle-K, L=0 and the view ring are byte-unchanged.** **Gates G1–G6 GREEN in a
  LOCAL PACKAGE:** 0/100 mismatches (was 17/100); hide-type `frame_indices`=[3..10]=hidden frames; non-hide
  `frame_indices`=[3..10] non-empty; **frame cadence byte-identical pre/post** (proves settle-K/phase timing untouched),
  positives 64→65 = exactly +9 FN −8 FP corrected; `verify_capture.py` clean (65 present / 65 boxes / 0 present-no-box)
  + visual confirmation at both corrected edges. **SHIPPED** — one `fix(capture)` commit `4559c8c` + tag `m18`, pushed
  (carried the m17-Concorde docs flip too). **OPEN (same root, NOT fixed):** the VIEW half — async grabs
  camera N while the ring yields camera N-1 → bbox predicted 1 frame stale under camera motion (unmeasured; static-camera
  scene; `IAI.Capture.ViewLag` is the knob G41 reserved for it). Also open: bbox projected from LIVE actor bounds at
  record-build time (wrong for moving targets); `blinking` toggle edges vs tickable order; **sync capture writes BLACK
  frames in a package** (same `ReadPixels` root as m19). G78.
  → `docs/sessions/2026-07-15-024-m18-label-alignment.md`.
- **Prior milestone (as-built): m17 — missing_texture revert hardening for runtime/modular-character materials —
  COMPLETE (commit `e2c6dd2`, tagged `m17`, pushed; on top of `m16` `84dfa52`) (2026-07-15).
  ✅ CONFIRMED ON THE REAL TITLE — no open items.** The owner pulled + rebuilt m17 on the office box and verified
  `missing_texture` apply → revert on Concorde's **actual `FWMasterSkeletalMeshComponent`**: the body reverts clean both
  for an **immediate** revert and for the **CHURN** case (apply → ~30 s play, character system re-creates the body
  mid-hold → revert — the case that previously stuck). The slot reset STICKS on the real merged/master-pose proxy; the
  character system does not re-assert the checker back. **The D4 question is RESOLVED and the modular-proxy follow-up is
  NOT needed** (G77 closed; its outcome map is retained as regression history only). Also validated on the LOCAL
  StackOBot repro in a package: stuck revert clears immediate + after-churn; `revert_all` clears; regression
  byte-identical on plain props/`SKM_Bot` incl. a real game MID restored as the same object; the our-material-only guard
  leaves a game-re-asserted MID untouched. Fixes the confirmed Concorde bug (body-only stuck corruption;
  `IAI.RevertAll` also failed): the anomaly restored through a saved component ptr + a saved original material ptr, and
  on characters whose own runtime logic re-creates the component and/or its MIDs (Concorde's
  `FWMasterSkeletalMeshComponent`) BOTH went stale during the hold → the stale-skip silently skipped → corruption
  persisted while frames were labeled CLEAN (dataset contamination). **Fix (only `Anomaly_MissingTexture.{h,cpp}`;
  `IAnomaly`/injector/other anomalies/capture loop byte-unchanged; no new dep; catalog stays 8):** Apply additionally
  records each slot's **owning actor + component `FName`** (+ the applied checker, since `Revert()` has no world
  access); Revert **re-finds the live component** (saved ptr → else same-named on the owner), **guards** (touch a slot
  only if it still holds our checker, incl. a MID whose parent chain reaches it — never stomp a material the game
  re-took), restores the saved original if alive **else resets to the mesh built-in default** (`SetMaterial(i,
  nullptr)`) so the game re-takes ownership, then **sweeps** every live mesh component of each touched actor for
  leftover checker (catches corruption copied onto a successor component we never captured). Every revert now logs
  `restored/default-reset/left-to-game/unresolved/swept/re-found` — **no more silent failures**. G74–G77.
  **D4 finding (repro-derived, since CONFIRMED on the real component):** the correct restore target is *whatever
  components are live on the actor at revert time* (not "the master" vs "the sub-parts") — one revert handled a master +
  master-posed sub-part with different dispositions; the restore must NOT survive the character system's next
  re-assertion (yielding the slot IS correct). Limits (untested rather than known-broken): per-actor only (sub-parts on
  a *different* actor are out of reach); a swept successor can only be default-reset.
  **Gates G1–G5 all GREEN in a LOCAL PACKAGE** (`Builds\MidRepro`, headless via WS): immediate + after-churn revert
  clean; `revert_all` clean; guard leaves a game-re-asserted MID untouched; **regression byte-identical on plain
  static/skeletal content incl. a real game MID**; targeted capture reverts within every burst (verified at pixel
  level via PNG decode). **Repro harness is a VALIDATION ASSET, NOT in this repo:**
  `D:\IntrusiveAnomalies\StackOBot\Source\StackOBot\MidReproActor.{h,cpp}` (project game module) +
  `SOB.MidRepro.*` console commands; the plugin repo tracks zero test files. **SHIPPED** — one
  `fix(missing-texture)` commit + tag `m17`, pushed (pushed BEFORE the Concorde test on purpose: the fix must reach
  the office box via GitHub before the real component can be exercised). Comment stripper run pre-commit: 0 changed /
  59 no-change. → `docs/sessions/2026-07-15-023-m17-missing-texture-revert.md`.
- *(The burst-boundary label misalignment found while validating m17 is now **m18**, above — built this session,
  awaiting review. Note the fix landed on the label-stamp timing, NOT on the phase transition: the anomalies already
  fire at the right time.)*
- **DEFERRED to m18 (diagnosed + locally reproduced, NOT started): packaged black dashboard preview.** Title-independent
  (repros on local StackOBot package). The preview's `FViewport::ReadPixels` reads a game-viewport render target that
  **does not exist in a package** (packaged viewports render straight to the swapchain; `GetRenderTargetTexture()` is
  null → D3D12 `RHIReadSurfaceData` zero-fills and returns "success" → ~6 valid all-black 15 KB JPEGs/s on the wire,
  frame counter ticking). PIE has a separate RT → works → editor masked it. Fix shape: TEE the preview off the
  **backbuffer** stream the capture path already proves out (`OnBackBufferReadyToPresent` + GPU readback, throttled +
  JPEG off-thread) — this also IS the long-deferred async-preview upgrade (kills the synchronous
  `FlushRenderingCommands` game-thread stall). Must gate **arming** on `IsCaptureActive()` (preserve G73) and use its
  OWN capturer instance (arm-match + Completed queue are single-consumer). **Delivery-gating: the preview is black in
  ANY client package.**
- **Prior milestone (as-built): m16 — three capture-delivery fixes — COMPLETE (tagged `m16` `84dfa52`, dashboard
  `f978f1b`) (2026-07-13).**
  (1) **Client token auto-populate** — `AnomalyControlServerSubsystem::StartListening` reads `[AnomalyControlServer] Token`
  from `DefaultGame.ini` via GConfig (present → fixed token; absent/empty → the existing random per-session GUID + log line,
  so the owner in-editor is byte-unchanged). The dashboard bakes a matching `VITE_CONTROL_TOKEN` (via `.env`) and
  auto-connects with zero client copy-paste; also persists the last-used token in localStorage so the owner stops re-pasting.
  Static shared secret, localhost-only tradeoff owner-accepted (G71). (2) **Focus-gated capture start** — a Start ARMS
  immediately but holds the first frame until the game window has foreground focus (new `ECapturePhase::ArmedPending`
  resolved in `Tick`; focus = `FViewport::IsForegroundWindow`); the timing bundle (StartFrame/manifest/fixed-timestep) is
  deferred out of `StartRun` into new `BeginActualRun` at focus-in; cancel-before-focus writes nothing + deletes the empty
  session dir (`bRunBegun` guard). Skipped when there is no game window (headless/**Simulate → bridge gates don't deadlock**);
  `IAI.Capture.FocusGate <0|1>` override + `[AnomalyCapture] bFocusGateDefault` + 30 s safety timeout (G72). (3)
  **Preview-pause hardening** — the control server's `PushFrames` suppresses live-preview JPEG generation while a capture is
  active (engine-side, immediate, no snapshot round-trip), so the synchronous preview `ReadPixels` can't drag sustained fps
  (G73). A single `bRunning`/`IsCaptureActive()` signal (true arm→finish, armed-pending included) drives both the focus-gate
  and the preview suppression. **Catalog stays 8; no new module dep** (GConfig=Core; focus via Engine `FViewport`;
  AnomalyCapture already links Slate in non-Shipping). **Gates:** dashboard `npm run build` GREEN (tsc+vite; baked-token
  inlining verified); owner-gate GREEN in-editor on real hardware; plugin compiled clean. **SHIPPED** — plugin `84dfa52`
  tag `m16` pushed, dashboard `f978f1b` pushed (untagged per dash precedent), both trees clean.
  → `docs/sessions/2026-07-13-022-m16-capture-delivery-fixes.md`, `docs/client-delivery.md`.
  *(Field note from the first packaged smoke test: the m16 focus-gate + preview suppression are sound, but the preview
  itself is black in ANY packaged build for an unrelated reason — see the m18 entry above; the G73 suppression is not
  implicated.)*
- **Prior milestone (as-built): Content-clock default reverted to WALL — COMPLETE (tagged `m15`) (2026-07-13).**
  Small settle-milestone on top of m14: flips the `IAI.Capture.ContentClock` **default back to `wall`** (m14 had briefly
  shipped `game` on an owner override pending validation). RESOLVED by the owner testing wall vs game on the actual office
  machine: **the client titles (Bates/Concorde) are WALL-clock** — wall gives correct-SPEED video (length varies with
  real capture duration = correct for wall-clock content); the earlier Fps 120/240 "slow motion" was an extreme-forced-ratio
  artifact, not game-clock evidence. Wall default is client-safe (a `game` default would play their real-time-clock videos
  ~2× FAST = the Issue-2 regression). **StackOBot is game-clock → set `game` via its build's `DefaultGame.ini [AnomalyCapture]
  ContentClockDefault=game`.** One-line code change (`EContentClock` member init `Game→Wall`; GConfig-absent fallback follows)
  + doc correction to the settled state (journal 021 closes journal 020's open item). Re-verified: fresh session, no ini key
  → clock=wall. The m14 machinery (game/wall stamp branches, warnings, setting, run_summary `content_clock`) is otherwise
  unchanged. Catalog stays 8. → `docs/sessions/2026-07-13-021-m15-content-clock-default-wall.md`, `docs/capture-fps.md`.
- **Prior milestone (as-built): Content-clock-aware fps stamp — COMPLETE (tagged `m14`) (2026-07-13).**
  Fixes game-clock captures playing `speed_ratio`× SLOW (the m11 honest stamp always stamped the sustained wall rate,
  which is correct for real-time content but wrong for game-clock content under fixed step, where every frame is an exact
  `1/target` game-slice → the natural stamp is TARGET). New setting **`IAI.Capture.ContentClock <game|wall>`** (mid-run
  guarded), **default `game` at m14 → REVERTED to `wall` in m15 (see above)**, packaged default `DefaultGame.ini
  [AnomalyCapture] ContentClockDefault` (GConfig at Initialize; same mechanism as delivery mode). **game** = stamp TARGET at
  any ratio (a high ratio only means the LIVE capture ran slow — perf issue, not a video defect); **wall** = unchanged m11
  behavior (ratio>tol → sustained). `run_summary.json` gains `content_clock`; annotation client-clean. (m14's "mixed-clock
  UNRESOLVED / client FAST-risk OPEN" note is now CLOSED by m15: the client titles tested wall-clock; default is wall;
  StackOBot uses game via ini.) Only AnomalyCapture (stamp branch + warnings + setting) + the run_summary field changed; fixed timestep / pacing
  / labeling / ground-truth UNCHANGED. All 5 gates + end-to-end mp4 GREEN (game 60→2.0s natural; wall→sustained fractional;
  ini default; mid-run guard; bad-token reject) + default-flip re-verify. G70. Catalog stays 8.
  → `docs/sessions/2026-07-13-020-m14-content-clock.md`, `docs/capture-fps.md`.
- **Prior milestone (as-built): Client delivery mode — COMPLETE (tagged `m12`) (2026-07-12).**
  A capture DELIVERY MODE for shipping the plugin to an external client who runs capture in their own build (no
  post-processing between their capture and them → whatever capture writes IS what the client gets). `bDeliveryMode`
  **default OFF** (full fidelity, byte-identical to m11 except the D3 annotation change). Console
  `IAI.Capture.Delivery <0|1>` (mid-run guarded); packaged default read at Initialize via GConfig from the project
  `Config/DefaultGame.ini` `[AnomalyCapture] bDeliveryModeDefault=True` (GConfig caches at startup → edit needs an
  editor restart; console overrides per session, no SaveConfig; chose GConfig over UDeveloperSettings to avoid a new
  dep/UCLASS — G69). **ON writes ONLY** `Actual_Frames/` + `Video_Clip/` + `run_summary.json` + `annotation.json`;
  **suppresses** `labels.jsonl` + `run.json` (never created — label record still COMPUTED, uniform path; threaded
  `bWriteLabels` through FJob→EncodeAndWriteFrame async + CaptureLabeledShot→AppendRecordAndImage sync, image always
  written). run_summary kept (encode_watcher's done-signal); seed lives only in run.json → delivery withholds it →
  session NOT client-reproducible (intended — G68). **D3 (both modes, always):** removed `schema_version` +
  per-anomaly `source_id` from annotation.json (+ dead-field tidy). **D4:** run_summary gains a `delivery_mode` bool.
  Manual `IAI.Capture.Shot` UNAFFECTED. Our QA tools (overlay_watcher.py/verify_capture.py) no-op on delivery
  sessions BY DESIGN (need labels.jsonl — G67); encode_watcher unaffected. No new module dep (GConfig=Core); no
  dashboard change (packaging-time decision, console+config only). All 5 bridge gates GREEN (OFF regression / ON
  file-set + end-to-end mp4 / GConfig default / mid-run guard / annotation strip; both async+sync; 0 drops); fully
  bridge-verifiable, no owner eyeball. Catalog stays 8. Files: AnomalyCaptureSubsystem.{h,cpp},
  AnomalyLabelWriter.{h,cpp}, AnomalyAsyncWriter.{h,cpp}. → `docs/sessions/2026-07-12-018-m12-delivery-mode.md`,
  `docs/client-delivery.md`.
- **Prior milestone (as-built): Capture pacing + honest fps stamping — COMPLETE (tagged `m11`) (2026-07-11).**
  Fixes the Issue-2 office "2x-fast mp4" on real-time-clock-driven client games. **Two-clock model (G64):** fixed
  timestep (m10-era) pins only the GAME clock; real-time-driven content (client sequencer/audio-synced scenes) runs
  on the WALL clock, so with fixed-step alone the mp4 plays fast by `VideoFps / sustained_wall_fps` (StackOBot is
  game-clock-driven → was always exact). **Fix = real-time pacing** `IAI.Capture.Pace <0|1>` **default ON**: a
  drift-free coarse-sleep+spin at the top of `UAnomalyCaptureSubsystem::Tick` holds every tick to ≥ `1/VideoFps` wall
  → game == wall == video clock, correct for BOTH families (UE's own limiter is bypassed under fixed timestep, so
  ours is the only pacer — G65). **Fallback = one-sided honest stamp:** every armed frame wall-stamped (`t_wall` per
  labels.jsonl row, both async+sync); at finalize `speed_ratio = wallSpan/gameSpan` (same first/last armed frames,
  settle gaps cancel), `sustained = VideoFps/ratio`; ratio > 1.02 → `annotation.video.fps` = sustained (fractional,
  3dp; encode watcher float-parses) + warnings; else fps = VideoFps exactly (never stamp faster-than-target).
  `video.target_fps` always written; `run.json` += target_fps/paced; `run_summary.json` += target_fps/
  sustained_wall_fps/speed_ratio/stamped_fps/paced. NO frame dup / NO VFR (1:1 mapping inviolate). WS
  `capture_stopped`/`capture_status` carry `{targetFps,stampedFps,speedRatio,paced}`; dashboard shows a post-run
  badge on fallback (own untagged feat commit). All 5 bridge gates + owner eyeball GREEN (G-P1 paced 30 exact int
  stamp; G-P2 throttled 60 → 58.055 fractional end-to-end via encode_watcher+ffprobe; G-P3 Pace-0 keeps 30; G-P4
  zero drops; G-P5 sync t_wall coherent). Warm-up + background-editor-throttle skew measurements (G66). Deferred to
  possible m11.1: hitch-robust median ratio (2% constexpr tol stands). Catalog stays 8.
  → `docs/sessions/2026-07-11-017-m11-capture-pacing.md`, `docs/capture-fps.md` (rewritten).
- **IN FLIGHT (branch `feature/stencil-capture` off `master` `d4a77db`, NOT on `master`): Occlusion-correct stencil bounding boxes + async unified capture.**
  Multi-stage; **Stage 1 COMPLETE, owner re-eyeball GREEN (2026-06-30), committed on the branch** (`refactor(capture)`, no tag). A new
  **quarantined `AnomalyCapture` module** (gated `ANOMALY_CAPTURE`; render/`RHI`/`RenderCore`/`Slate`/`SlateCore`/`ApplicationCore` + a
  `bBuildEditor`-only `UnrealEd` dep, all compiled OUT of Shipping — `Renderer`/Renderer-private deferred to Stage 3) extracted from
  `AnomalyControlServer`: the m7 capture (`UAnomalyCaptureSubsystem` + `AnomalyLabelWriter` + `AnomalyPreviewCapture`) **MOVED** there with
  its own log cat `LogAnomalyCapture`; `AnomalyControlServer` now **depends on** `AnomalyCapture` (DAG: core ← AnomalyCapture ← ControlServer).
  New **async, non-blocking capture** that grabs the REAL player frame (**game UI IN**): `FAnomalyFrameCapturer` hooks
  `FSlateRenderer::OnBackBufferReadyToPresent` (post-Slate), clips to the game-viewport rect (FFrameGrabber `TargetWindowPtr`+`CaptureRect`
  pattern → no editor chrome even in docked PIE), stages an `FRHIGPUTextureReadback`, the render thread does only the lock-copy-out, and a
  **thread-pool `FAnomalyAsyncWriter`** does convert+encode+write OFF the game thread (G53 — fixes a per-frame stall/animation judder). Frame↔state
  carry keyed by submit `GFrameCounter`; `IAI.Capture.Async <0|1>` falls back to the sync `ReadPixels` path. **Only OUR overlays** are suppressed
  for a run via a generalized core flag `AnomalyViewport::SetOverlaysSuppressed` (poll-radius sphere + selector HUD/box + auto HUD + the heartbeat
  **actively evicted** each tick, G54); the PIE mouse-control-label is disabled **per-PIE-session** at subsystem `Initialize` (G55). A `DrainTail`
  FSM phase makes clean burst-count runs **0-drop** (G56). The m7 projected label box is UNCHANGED this stage (the stencil box is Stage 3; color and
  stencil are now two grab points joined by frame id — G52). **Clean 5.1 Dev-Editor compile (exit 0); core dep set unchanged (render deps quarantined);
  `IAnomaly` untouched; catalog stays 8.** Gotchas **G52–G57**; journal `docs/sessions/2026-06-30-015-stencil-capture-stage1.md`.
  **Next:** Stage 2 — custom-stencil tagging (`r.CustomDepth 3`, set/restore), then Stage 3 (stencil/depth SVE + occlusion-correct box), Stage 4 (multi-actor + docs + tag).
- **Latest milestone (as-built): Targeted capture modes + pre-run clean slate + entry-point parity — COMPLETE (tagged `m10`) (built 2026-07-10, closed 2026-07-11).**
  Capture runs fire in **targeted** mode (`IAI.Capture.Start [outDir] [png|jpeg] [seed] [maxFrames] [anomaly] [targetActor]`,
  `""` placeholders — G60; WS `capture_start {anomaly, target}`; new `UAnomalyAutoInjectorSubsystem::TryFireSpecific` — exact
  `=`-match, keeps all m6 guards, visibility-independent G61) or **auto-pool** (unchanged). `run.json` records
  `mode`/`target_anomaly`/`target_actor`. **Clean slate:** StartRun reverts auto live fires + `Injector->RevertAllActive()`
  (no unlabeled contamination — G63; contam gate green, `IAI.DumpActive`=0 after start). **Parity:** StartRun/FinishRun own the
  auto-injector pause/resume for BOTH entry points (`bDeinitializing` teardown guard — G62; WS-local pause/resume deleted).
  Dashboard is capture-first (Targeted/Auto-pool toggle; auto panel → "Capture pool"; Inject/Arg panels deleted; own feat
  commit in the dashboard repo). Also landed in the close turn: the m9-era follow-on `fix(capture)` (`6d4eb01` — client-shaped
  `affected_frames` object + seedless `session_<ts>` naming) and the previously-untracked `docs/capture-fps.md` (`16a5c19`).
  **NOTE:** "m10" in some earlier notes meant the untagged fixed-timestep capture-fps cluster (`c5d58b0`/`500eac7`/`417833a`) —
  that naming is CORRECTED: m10 = this milestone; the approved capture-pacing/honest-fps plan = **m11** (next).
  Catalog stays 8. → `docs/sessions/2026-07-11-016-m10-targeted-capture.md`.
- **Latest as-built (post-m8, NO tag — both on `master`, 2026-06-22): Screen-coverage candidate cull + its dashboard slider.**
  **(1) Cull** (commit `a96f8bb`) — an optional **actor-level** cull on the renderable-visible set in `AnomalyViewport`: an actor
  is an injectable target iff its on-screen footprint (the **clamped projected union** of its renderable-visible component bounds)
  covers **≥ P%** of the viewport. `IAI.SetMinScreenCoverage <pct>` (plain world-independent cmd; `P <= 0` = OFF = byte-identical) +
  `IAI.DumpCoverage` (ascending-coverage tuning diagnostic). Applied to **both** live entry points
  (`GetVisibleRenderableActors` + `GetVisibleRenderableActorInfos`) through a new shared per-actor classifier
  **`ClassifyRenderableVisibleLive`** (OFF = byte-identical in result **and** cost via the kept first-match short-circuit; ON = one
  union pass, no double tracing) so the `IAI.DumpVisible` set-identity gate holds with the cull ON. Reuses the clamped
  `ProjectBoundsToScreenRect` fed the visible-component union (NOT the m7 type-only/unclamped projector). Touches only
  `AnomalyViewport.{h,cpp}` + docs; **no `IAnomaly`/dep change**. Gotcha **G51**; journal
  `docs/sessions/2026-06-22-014-screen-coverage-cull.md`. **(2) Dashboard slider** (plugin `81bc841` + dashboard repo `8c148b6`) —
  surfaced as a **throttled live slider** on the Tier-2 dashboard via a new `AnomalyControlServer` WS command
  `set_min_screen_coverage {pct}` (→ `SetMinScreenCoveragePct`) + a `session.minScreenCoverage` snapshot field (→ `GetMinScreenCoveragePct`),
  cloned from the poll-radius precedent; server stays compiled out of Shipping. Session-level live value only — **NOT** per-actor
  `FRenderableActorInfo` (still deferred). New dashboard `src/lib/throttle.ts` (~10/sec + authoritative send on release; numeric %
  snapshot-bound). **No new anomaly — catalog stays 8** (cull = targeting infrastructure, slider = UI). No tag, no version bump
  (same framing as the poll-radius pair). FF-merged into `master` in both repos; review branches deleted.
- **Latest milestone (as-built): Missing-Texture Anomaly — COMPLETE (tagged `m8`, VersionName 0.9.0) (2026-06-21).**
  New **8th** anomaly **`missing_texture`** (object-scoped, `Private/Anomalies/Anomaly_MissingTexture.{h,cpp}`): per-component
  `UMeshComponent::SetMaterial` swaps every renderable static/skeletal mesh slot to a plugin-**shipped Lit gray/white UV-checker**
  material (per-component override = object isolation, never touches the shared mesh/material asset; per-slot original +
  `bWasExplicitOverride` captured for an exact revert; skip stale). **First `Content/` asset in the plugin** — cook guarantee =
  a **CDO hard-ref** (`ConstructorHelpers::FObjectFinder` → non-transient `UPROPERTY TObjectPtr` on `UAnomalyInjectorSubsystem`)
  + flip `"CanContainContent": true`; **no host `DefaultGame.ini`** (G45). Found the hard way: the cook runs on **editor** binaries
  (rebuild before cooking — G47); 5.1 **IoStore** puts cooked assets in `.ucas`/`.utoc`, not `.pak` (verify by runtime load — G48).
  Material declares all mesh **usage flags** (skeletal/nanite/ISM/morph/spline) or it renders **default-gray** at runtime (G49).
  Reproducible authoring via `tools/create_missing_texture_materials.py`. Wired: `Register()`; `GetAuthoredSpec` (Object, **no
  args**); added to the selector `GAnomalyChoices` + the auto `GAutoPool` (`NumPoolKeys` 4→5, key `5`/`pool5`). **`IAnomaly`
  untouched; deps `Core/CoreUObject/Engine/InputCore`; catalog 7→8.** Gates driven green over the bridge: G-Compile (DumpCatalog=8),
  G-Apply (static multi-slot `SM_Ramp3` + skeletal `SKM_Bot`, exact revert incl. both override branches), G-Isolation
  (`SM_RockFlats_02`/`M_Rock` sibling untouched), G-BornComplete (selector cycle+inject, auto FireOnce, a 14-burst capture run).
  **DEFERRED — the flat-magenta variant + a `mode` arg:** unlit-emissive magenta lit the Lumen scene ("glowed" onto neighbours);
  the canonical fix is Lit base-colour but the owner is revisiting the look (G50). One `feat(missing-texture)` commit, tagged
  **`m8`**. **NOTE:** the unreal-mcpython bridge was unstable this session (crashed the editor on some calls) — the lit-checker
  live render is owner-eyeball-pending. → `docs/sessions/2026-06-21-013-missing-texture.md`.
- **Prior milestone (as-built): Labeled Frame-Capture + 2D BBox Labeling — COMPLETE (tagged `m7`, VersionName 0.8.0)
  (2026-06-20).** A capture/labeling layer producing an ML-friendly **labeled image sequence** from a LIVE
  auto-injection run (labels = the injector's OWN ground truth, L1). New **`UAnomalyCaptureSubsystem`** + `AnomalyLabelWriter`
  **housed in the `AnomalyControlServer` module** (reuses its game-viewport capture primitive + ImageWrapper; gated by
  `ANOMALY_CONTROL_SERVER`, compiled out of Shipping — dataset capture is a dev/research activity in a packaged
  Development/Test build, never retail Shipping). Drives the m6 deterministic core in **capture-driven bursts**
  (`[pre] → FireOnce → [settle K] → [positives] → RevertAllLiveFires → [settle K] → [post]`, looped); per captured frame
  writes `frame_<GFrameCounter>.png` (opaque, native res) + a JSONL label record + `run.json`/`run_summary.json`, all
  same-tick + `GFrameCounter`-stamped (exact image↔label alignment). The 2D bbox projects the fired actor's PERSISTED
  bounds (works when hidden). **Three sanctioned core exposures only — `IAnomaly`/injector/anomalies/leaf-helpers/`=`-match/
  `GetVisibleRenderableActors` byte-clean:** `AnomalyViewport::ProjectActorBoundsToScreenRect` (type-only bounds union,
  NOT `IsVisible`-gated — G38); `FAutoLiveFireInfo` widened with `TargetActor` + `StartFrame`; `RevertAllLiveFires()`
  exposed (keeps `GetLiveFires()` accurate). **No new dep; catalog stays 7.** Settle-K SYMMETRIC at both boundaries (G37);
  view-lag **L=0 validated** (tickables tick before the camera update → the view already lags 1 frame; L=0 = "1 render-frame
  back", FPS-invariant — G41); **`visible_positive`** = present + a valid box (off-screen-during-hold frames kept as hard
  negatives — G42). `tools/verify_capture.py` overlays boxes (Pillow). Gates 1/2/3 GREEN incl. owner moving-eyeball.
  **Closed as TWO commits (Plan A):** `ff1be3c` `feat(control-server): Slice-1 dashboard …` (the parallel track's
  uncommitted Slice-1 WIP promoted first, NO tag — it owns the shared capture primitive) + the m7 commit on top (tagged
  **`m7`**). The async backbuffer capture path (`OnBackBufferReadyToPresent` + GPU readback) is the documented superseder,
  REQUIRED before framerate-bug anomalies + for exact-under-motion (G40). → `docs/sessions/2026-06-20-012-frame-capture-labeling.md`,
  `docs/post-m7-capture-labeling-handoff.md`.
- **Prior as-built (post-m6 viewport fixes, 2026-06-20):** two surgical, owner-locked fixes to the shared
  **renderable-visible set** in `AnomalyViewport` (the one source of truth consumed by the M5 selector, the m6
  auto-injector, AND — new since m6 — the control-server **A4 read-back** `GetVisibleRenderableActorInfos` + the
  `IAI.DumpVisible` set-identity gate). **(1) VFX removed (G33):** dropped the `UFXSystemComponent` clause from
  `IsRenderableComponent` → the set is **SM ∥ SK only** (reverses the G29/R1 VFX inclusion; HARD REMOVE). The `=name`
  console escape hatch still reaches VFX actors (it bypasses the predicate). **(2) Changeable poll-radius distance
  cull (G34):** `IAI.SetPollRadius <cm>` adds an optional cull — actor in the set iff renderable AND within R of the
  **player pawn** (sphere-approx bounds metric) AND in-frustum AND unoccluded; `R <= 0` disables it (default OFF,
  byte-identical); applied identically at both live entry points (DumpVisible MATCH preserved); a dev debug sphere
  visualizes R around the live pawn. **Only `AnomalyViewport.{h,cpp}` touched** (+ docs); `IAnomaly`/injector/anomalies/
  selector/auto/control-server cores untouched; **no new dep**, no `.uplugin` bump. **Clean Development-Editor compile
  on 5.1 (exit 0)** before each commit (control-server module re-links clean against the changed header). **Two atomic
  path-scoped commits, NO tag:** `9bbd398` `fix(viewport): remove VFX from renderable-visible set` +
  `<fix2>` `feat(viewport): add changeable poll-radius distance cull` (the uncommitted control-server WIP in the tree
  was left untouched). Owner smoke-test pending. → `docs/sessions/2026-06-20-011-viewport-vfx-removal-poll-radius.md`.
- **Prior as-built (control server, in flight):** the Tier-2 runtime control surface is under construction — committed
  slices `2645236` (transport spike: WS server + auth + loopback gate + backbuffer→JPEG) and `4c05344` (core read-back
  / A4: `GetVisibleRenderableActorInfos` + `FRenderableActorInfo`), plus uncommitted WIP. Separate `AnomalyControlServer`
  module with its own log category (G32). Not yet journaled as a milestone.
- **Latest milestone (as-built):** **Automatic Injection — COMPLETE (committed `41ba104`, tagged `m6`) (2026-06-19).** New
  **separate** `UAnomalyAutoInjectorSubsystem`
  (`Public/AnomalyAutoInjectorSubsystem.h` + `Private/AnomalyAutoInjectorSubsystem.cpp`, `UTickableWorldSubsystem`,
  Game+PIE) that auto-fires the **4** object-scoped anomalies **randomly on the renderable objects currently on-screen**
  (drawn from `AnomalyViewport::GetVisibleRenderableActors` + applied via the `=` exact-match token), each
  **auto-reverting** after a randomized hold. **Concurrent but collision-free by construction** (no coordinator) via two
  invariants: **(i)** one live fire per id (the registry's one-instance-per-id) + **(ii)** **one anomaly per actor**
  (`OVERRIDE-1` — subsumes both conflict groups *and* the hide-masks-LOD case, so **no id→group table**; supersedes the
  planning turn's per-group guard). All randomness from **one seeded `FRandomStream`** (console-settable seed, default
  time-based) on a **fixed draw protocol** independent of apply-result (R-SEED). **Explicit-core / thin-shell split (as
  m4/m5):** the deterministic core `AdvanceTime`/`TryFireOnce` is bridge-driveable as `IAI.Auto.Step`/`IAI.Auto.FireOnce`
  **without real time and without Enable/Run**; two thin shells drive it — the `IAI.Auto.*` console (bridge gate) +
  raw-input poll (keys `1-4`/`J`/`K`, distinct from the selector's) + a right-anchored immediate-mode HUD (eyeball).
  **Two switches, both default OFF → dormant → existing gates byte-identical:** `IAI.Auto.Enable <0|1>` (HUD/keys) and
  `IAI.Auto.Run <0|1>` (firing; forced OFF when !Enabled). Fires **auto-revert** after a randomized hold (R-LIFE;
  `IAI.Auto.Persist` flag, default off). **Self-scoping** — does NOT touch `IAI.SetViewportScoping` (warns if it is ON);
  no view → fire nothing (never blind). **Manual selector/console injection of a pool id during an auto run is
  unsupported → warn-not-block (R-COEXIST).** **No `IAnomaly`/injector/anomaly/leaf-helper change; no new dep**
  (`FRandomStream` = Core; deps stay `Core/CoreUObject/Engine/InputCore`); **catalog stays 7** (orchestration over the
  existing catalog). VersionName → **0.7.0**. **Clean Development-Editor compile on 5.1 (exit 0).** **Bridge state-gates
  GREEN (MainWorld Simulate):** deterministic headless fire + `=` exact-match (1 of 21 EnergyOrb siblings hit),
  auto-revert on hold-elapse, collision-free concurrent (3 distinct ids × 3 distinct actors, no 4th fire — invariants
  (i)+(ii)+cap), seed-reproducible target, OFF-regression byte-identical (`SM_Ramp`→2), both coexistence warnings fire
  without blocking. **Owner real-Play eyeball green — ACCEPTED.** → `docs/sessions/2026-06-19-010-auto-injection.md`.
- **Prior milestone (as-built):** **Object Selector + Inject UI (minimal) — COMPLETE (committed `aa2a3a4`, tagged `m5`) (2026-06-19).**
  A new **separate** `UAnomalySelectorSubsystem` (`Public/AnomalySelectorSubsystem.h` + `Private/AnomalySelectorSubsystem.cpp`,
  `UTickableWorldSubsystem`, Game+PIE only) that lets the player **select a visible on-screen object** (Tab-cycle over the
  **renderable-visible set** — frustum AND occlusion AND renders-to-screen) and **inject** one of the four object-scoped anomalies on it (default args), then
  revert — calling the existing injector's public `ApplyAnomaly`/`RevertAnomaly`. **Explicit-core / thin-shell split (as m4):**
  public methods `AdvanceSelection`/`SelectPrevious`/`CycleAnomalyChoice`/`InjectSelected`/`RevertSelected` + readbacks
  `GetSelectedActorName`/`GetVisibleActorNames`/`GetAnomalyChoice` are the bridge-gatable surface; two thin shells drive them —
  the `IAI.Selector.*` console commands (bridge gate) and per-tick **raw input polling** + an **immediate-mode HUD**
  (real-Play eyeball). Targeting is made exact by a new **`=` sentinel** in `AnomalyTargeting::FindActorsMatching`
  (leading `=` → `GetName().Equals(IgnoreCase)`; substring path **byte-identical** with no `=`); `InjectSelected` passes
  `"=" + GetName()` so it hits only the selected actor (the **only** leaf-helper change — additive; verify-item 5 pre-authorized).
  HUD = `UDebugDrawService::Register("Game", …)` (host-blind, no game HUD class — G25) drawing a visible-names list + an
  anomaly list + a `DrawDebugBox`/label on the selection; input = `WasInputKeyJustPressed`/`IsInputKeyDown` raw key state
  (no host mappings — G26); defaults Tab/Shift+Tab/C/G/H, rebindable via `IAI.SelectorBind`. Activation **`IAI.SelectorUI <0|1>`,
  default OFF → dormant → existing gates byte-identical.** **First dep since M0: `InputCore`** (FKey/EKeys; transitive via Engine,
  declared for IWYU) — **no Slate/UMG** (immediate-mode). **Renderable-target filter folded in** (m5 follow-on): the selector's
  visible set means **renderable-visible** — new additive `AnomalyViewport::IsRenderableComponent` (`IsVisible()` + a
  static/skeletal/`UFXSystemComponent` base-type allowlist; VFX caught with no Niagara dep) excludes volumes/spawn-points/
  debug/landscape (the m4 visibility funcs stay byte-identical); a HUD `LastInjectResult` line surfaces the AMB-2 zero-match;
  `GetVisibleRenderableActors` returns empty on no-view (offer nothing, never blind). This is the set **auto-injection** will
  consume (gotcha G29). **No `IAnomaly` change, injector subsystem + all 7 anomalies untouched;
  catalog stays 7.** VersionName → **0.6.0**. **Clean Development-Editor compile on 5.1 (exit 0).** Combined gate **green**
  over the bridge (MainWorld Simulate): selection cycles the name-sorted renderable-visible set; `=` exact-match inject
  hits exactly the selected actor (1 of 17 prefix-siblings); the renderable filter excludes RVTVolume / PlayerStart /
  GameplayDebugger / zero-instance-grass LandscapeStreamingProxy while keeping meshes + foliage + NiagaraActors +
  RoomBuilderSquare *(NiagaraActors/VFX were later removed from the set — G33, 2026-06-20)*;
  zero-match (Niagara + `lod_corruption`) surfaced; OFF-regression byte-identical (`SM_Ramp`→2,
  `=SM_Ramp2…`→1). **Owner real-Play eyeball green — ACCEPTED.** → `docs/sessions/2026-06-19-009-selector-inject-ui.md`.
- **Prior milestone (as-built):** **Viewport-Visibility Layer — COMPLETE (committed `7c34275`, tagged `m4`) (2026-06-18).**
  New shared helper **`AnomalyViewport`** (`Public/AnomalyViewport.h` + `Private/AnomalyViewport.cpp`,
  AnomalyTargeting/Args/Lod convention) = "is this object visible to the player" via **frustum AND occlusion**
  over an explicit view spec `FAnomalyViewInfo` (deterministic, synthetic-view-gatable) + a thin live resolver
  `GetActiveViewInfo` (first local player's POV; treat-as-unscoped + warn on no view). Occlusion backend (AMB-V1)
  = **multi-sample camera-to-bounds line trace** (`ECC_Visibility`, center+8 corners), private behind the
  backend-agnostic API; `GetLastRenderTimeOnScreen()` is the documented live backend for the future
  capture/live-injection milestone (.cpp-only swap — G22). New opt-in toggle **`IAI.SetViewportScoping <0|1>`
  (default OFF)** + diagnostic **`IAI.TestVisibility`** (synthetic-gate driver). The **4** object-scoped
  primitive-backed anomalies (`missing_object`, `flicker`, `lod_corruption`, `lod_popping`) consult the toggle and
  route through `AnomalyViewport` only when ON; `lighting_mismatch` + the two globals are excluded by design.
  **No `IAnomaly` change, no new module dependency** (frustum/traces/camera = Engine, `FReversedZPerspectiveMatrix` =
  Core; both locks held). **Clean Development-Editor compile on 5.1 (exit 0)**; over the bridge (MainWorld Simulate):
  synthetic frustum gate (behind→out, far→in, in-cone→in — reversed-Z VP validated, G24), synthetic occlusion gate
  (controlled wall: blocked→0 / clear→1 at frustum=1), and **OFF-is-byte-identical regression** (`missing_object`
  + `lod_corruption` round-trips M-identical, ListAnomalies still 7) all **green**. Catalog unchanged at **7**.
  VersionName → **0.5.0**. → `docs/sessions/2026-06-18-008-viewport-visibility-layer.md`.
- **Prior as-built:** **Refactor — "GDP" prefix removed from the plugin — COMPLETE + COMMITTED `351c7e8` (2026-06-18).**
  Pure mechanical rename, **no behavior change**: module/plugin/folder/`Build.cs`/`.uplugin` `GDPAnomalyInjector`→`AnomalyInjector`;
  `UGDPAnomalyInjectorSubsystem`→`UAnomalyInjectorSubsystem`; `IGDPAnomaly`→`IAnomaly`; `FGDPAnomaly_*`→`FAnomaly_*`;
  API macro `GDPANOMALYINJECTOR_API`→`ANOMALYINJECTOR_API`; log category `LogGDPAnomaly`→`LogAnomaly`;
  helpers `GDPTargeting/GDPArgs/GDPLod`→`AnomalyTargeting/AnomalyArgs/AnomalyLod`; console commands `GDP.*`→`IAI.*`.
  Project identity **"GDP: Anomaly Injection"** retained (code-prefix strip only; copyright/`CreatedBy` unchanged). Clean
  Development-Editor compile on 5.1 (exit 0) + light bridge re-gate green (module loads under the new name, `IAI.ListAnomalies`
  lists the **7** sorted under `LogAnomaly`, `IAI.Apply/Revert missing_object SM_Ramp` round-trips). One `refactor:` commit, **no tag**;
  bridge/host unchanged (G21). → `docs/sessions/2026-06-18-007-rename-strip-gdp-prefix.md`.
- **Prior milestone:** **M3 — LOD breadth fill — COMPLETE (committed `c54351a`, tagged `m3`).**
  `lod_corruption` extended to **static OR skeletal** meshes (same ID — one "LOD corruption" category; mesh
  type is an implementation detail), new ticking **`lod_popping`** (flicker mechanics), and a new shared
  helper **`AnomalyLod`** (`Public/AnomalyLod.h`+`Private/AnomalyLod.cpp`) absorbing the static/skeletal forced-LOD
  dispatch (2 consumers). Registry lists **7** (sorted). **No `IAnomaly` change** (M1 lock held again)
  and **no new module dependency**. Clean Development-Editor compile on 5.1 (exit 0); all 9 state gates
  driven green over the bridge in a `MainWorld` Simulate session — incl. the static **regression**
  (M2-identical), the **heterogeneous** apply (`lod_corruption Bot` = 1 static + 2 skinned in one apply),
  `lod_popping` oscillation, re-apply no-leak, RevertAll, teardown. **The Bot is single-LOD → skeletal
  anomalies are state-validated, no Bot visual** (G20). VersionName → 0.4.0.
  → `docs/sessions/2026-06-13-006-m3-lod-breadth.md`.
- **Prior as-built:** **M2.5 (UE 5.1 port) + M2.6 (bridge sever) — COMPLETE (2026-06-10).** **UE 5.1 is now
  the canonical engine** (the two real target games are on 5.1). Host = `D:\IntrusiveAnomalies\StackOBot`
  (natively-5.1); source engine = 5.1 at `D:\UESource\UnrealEngine`. The six anomalies compile clean on 5.1
  with **zero plugin-source changes** (all 7 port watch-items unchanged; only host-target build constants
  changed — G17), and all **10** stage gates were re-driven **green over the MCP bridge** + owner-confirmed
  visuals (flicker blink, magenta movable sun, near-clip). The `unreal-mcpython` bridge was ported to 5.1 by
  **severing its `BehaviorTreeEditor` dependency** (G8) — costs only the 2 BT-authoring tools.
  → `docs/sessions/2026-06-10-005-m2.5-m2.6-5.1-port-bridge-sever.md`.
- **Earlier:** **M2 — Breadth Round 1 — COMPLETE (all 8 stage gates passed).**
  Adds two shared helpers — **A1** `AnomalyTargeting::FindComponentsMatching<T>` (component targeting) and
  **A3** `AnomalyArgs` (parse/clamp/warn) — and three anomalies: `lighting_mismatch` (component, ULightComponent),
  `lod_corruption` (component, UStaticMeshComponent, static-only), `camera_clipping` (global near-clip).
  Registry lists **6** (sorted). **No `IAnomaly` change was needed — the M1 lock held.** Clean headless
  compile + gates 2–7 verified live in PIE `MainWorld` (unreal-mcpython bridge + owner eyeball, 2026-06-09).
  → `docs/sessions/2026-06-09-004-m2-breadth-round-1.md`, `docs/architecture.md`.
- **Resolved (M3):** **AMB-1 → skinned LOD count via `USkinnedMeshComponent::GetNumLODs()`** (runtime
  render-data count — the analog of static `GetNumLODs()`; not the asset's authored `GetLODNum()`) — G19.
  **AMB-2 → single tagged capture record keyed to the common base `UMeshComponent`** + `Cast<>` dispatch in
  `AnomalyLod` (not two typed lists); this is what lets one apply span a heterogeneous static+skeletal set.
  **AMB-3 → `lod_popping` default 2 Hz, ceiling 30 Hz.** Supersedes G16's static-only scope.
- **Resolved (M2):** **AMB-M2-1 → defer A2/`AnomalyCvar`** — near-clip is a console *command* + the
  `GNearClippingPlane` global, not an `IConsoleVariable`, so `camera_clipping` is self-contained (no
  `RenderCore` dep); AnomalyCvar lands with its first real cvar consumer (G13). **AMB-M2-2 → static-only
  `lod_corruption`** was the M2 stopgap; **resolved in M3** (static + skeletal via `AnomalyLod`, G19). M2 ships 2 helpers (A1, A3).
- **Resolved (M1):** **AMB-3 → capture-baseline** — `time_dilation` Revert restores the pre-Apply value.
  Generalized in M2 to the **per-target/global state-capture convention** (see architecture.md). G11.
- **In flight:** the **Tier-2 runtime control server** (`AnomalyControlServer` module — WS transport + A4 read-back;
  committed `2645236`/`4c05344` + uncommitted WIP in the tree; not yet journaled as a milestone). The two post-m6
  viewport fixes (G33 VFX removal + G34 poll-radius cull) are committed (`9bbd398` + `<fix2>`, no tag); owner smoke-test
  pending. **Next action:** finish the control-server slice, then the High-priority new bug types (born viewport-aware
  AND auto-injectable). The `flicker→blinking` rename is **DONE** (`refactor(blinking)`, no tag). Also still queued: a new `flickering` anomaly (scene-region / light toggling; handoff §2.3),
  region-darkening (§2.4), the selector's screen-X ordering polish. Bridge/host stay unversioned (G8 unchanged).
- Milestones: M0 (`…-001`), M1 (`…-003`), M2 (`…-004`), M2.5+M2.6 (`…-005`), M3 (`…-006`) fully passed
  + tagged; rename refactor (`…-007`) committed `351c7e8` (no tag); **Viewport-Visibility Layer (`…-008`) committed
  `7c34275`, tagged `m4`**; **Object Selector + Inject UI (`…-009`) committed `aa2a3a4`, tagged `m5`**;
  **Automatic Injection (`…-010`) committed `41ba104`, tagged `m6`**; viewport VFX-removal + poll-radius (`…-011`,
  no tag); **Labeled Frame-Capture + 2D BBox Labeling (`…-012`) tagged `m7`** (control-server Slice-1 promoted first
  as `ff1be3c`, no tag); **Missing-Texture (`…-013`) tagged `m8`**; screen-coverage cull + slider (`…-014`, no tag);
  **multi-anomaly session capture tagged `m9`** (`88f519c`); fixed-timestep capture-fps (`c5d58b0`/`500eac7`, no tag,
  `docs/capture-fps.md`); **targeted capture (`…-016`) tagged `m10`**; **capture pacing + honest fps stamping
  (`…-017`) tagged `m11`**; **client delivery mode (`…-018`) tagged `m12`**; **confirmation-bounded dashboard optimism
  (`…-019`, AnomDash, no tag)**; **content-clock-aware fps stamp (`…-020`) tagged `m14`**; **content-clock default → wall
  (`…-021`) tagged `m15`**; **three capture-delivery fixes (`…-022`) tagged `m16`** (`84dfa52`);
  **missing_texture revert hardening (`…-023`) tagged `m17`** (`e2c6dd2`; validated on the local repro **and confirmed
  on Concorde's real `FWMasterSkeletalMeshComponent`**, immediate + churn — G77 closed); **burst-boundary label
  alignment (`…-024`) tagged `m18`** (`4559c8c`); **preview backbuffer tee + targeting defaults (`…-025`) tagged `m19`**
  (`c8aa3fa`; G3/PIE + Concorde format = owner post-push checks); **annotation.json labeling (`…-026`) tagged `m20`**
  (`c01a214`); **deterministic arm→present pairing (`…-027`) = m21, BUILT + package-gated, NOT yet committed/tagged**
  (fixes the −1 in pace-off + mild-overrun regimes; deep-starvation residual → proposed m22 scene-identity marker).

## Documentation system — how these docs fit together (read in this order)
- **CLAUDE.md** (this file) — canonical context, environment, invariants, workflow rules, and the
  **Current status** above. Start here.
- **[docs/architecture.md](docs/architecture.md)** — **living** current-as-built design reference
  + the **anomaly catalog**. "The whole picture in one read." Describes only what is in the code
  *now*; forward plans live in the journals, never here.
- **[docs/onboarding.md](docs/onboarding.md)** — what this is, how the work is run, where things live.
- **[docs/setup-runbook.md](docs/setup-runbook.md)** — **living** recipe to build/run from scratch.
- **[docs/invisible-anomaly-mechanisms.md](docs/invisible-anomaly-mechanisms.md)** — **the ledger of
  invisible-anomaly mechanisms** (`m23`/`P3`, `H4`, `H5` (i) and (ii), and traceability marked as **NOT
  a cause**). One symptom, several causes, **potentially distinct cures, and no single fix is known to
  address all of them.** Each row states **MEASURED vs SOURCE-READ**, its limits, and its cure if known.
  **Read this before touching anything in that line of work.**
- **[docs/gotchas.md](docs/gotchas.md)** — **append-only** non-obvious lessons (G1, G2, …).
- **[docs/sessions/](docs/sessions/)** — one journal per session, `YYYY-MM-DD-NNN-slug.md`: the
  chronological record (Goal / What done / Problem→Resolution / Deviations / State / Hand-off) and
  the home for milestone **plans** and **design decisions** (including open/blocking ones).

## Environment
- Engine: **source-built UE 5.1** (Release-5.1) at `D:\UESource\UnrealEngine`, registered to the
  `.uproject`'s `EngineAssociation` GUID `{B34F356C-4AE7-256A-F0E1-318A632BB902}` under
  `HKCU\Software\Epic Games\Unreal Engine\Builds`. (Originally validated on source-built UE 5.4.4 — see
  the Engine support note in architecture.md. After any engine re-sync, **rebuild ShaderCompileWorker** — G18.)
- Host project: **StackOBot** at `D:\IntrusiveAnomalies\StackOBot` (natively-5.1; the old 5.4 host at
  `D:\Unreal Projects\StackOBot` is retired).
- Plugin in-tree at `D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector\` (its own git repo, `master`).
- Windows, MSVC. Build target: **StackOBotEditor / Development / Win64**. Host-target build constants:
  `BuildSettingsVersion.V2` / `EngineIncludeOrderVersion.Unreal5_1` (G17).
- Functional smoke tests run in **PIE via the `unreal-mcpython` MCP bridge** (host tooling, NOT part of
  this repo — see gotcha G8; on 5.1 its `BehaviorTreeEditor` dependency is severed). State/log reads close
  the non-visual gates; the owner eyeballs visuals.

## Architecture (current as-built: M0 — full detail in docs/architecture.md)
- One **Runtime** module `AnomalyInjector`, `LoadingPhase = Default`, `EnabledByDefault: true`.
- Build.cs deps: `Core`, `CoreUObject`, `Engine` (later may add `Renderer`, `RenderCore`, `RHI`,
  `Slate`, `InputCore`).
- Core injector = a `UTickableWorldSubsystem` (`UAnomalyInjectorSubsystem`) — auto-ticks,
  world-scoped, gives `GetWorld()`. Restricted to **Game + PIE** worlds via `DoesSupportWorldType`
  (never the editor preview world).
- Control surface = console commands via `FAutoConsoleCommandWithWorldAndArgs`, module-scoped,
  resolving the subsystem from the world the console passes in, null-guarded.
- M0 anomaly = ONE hardcoded hide (`IAI.HideActor` / `IAI.ShowAllActors`). The general anomaly
  **interface + registry is the M1 design** (see Current status + journal 002), not yet in code.

## Invariants (do not violate)
- 🔒 **OFFICE HOSTS ARE REFERRED TO BY CODENAME ONLY (Concorde, Bates, Deimos); NEVER A TITLE.**
  This covers code, docs, journals, predictions, commit messages, logs and chat reports alike, and
  it applies to abbreviations of a title as well as to the title itself. (Owner directive,
  2026-08-31.)
- 🚨 **AN OBSERVATION AND ITS EXPLANATION ARE SEPARATE CLAIMS, RECORDED SEPARATELY. NEVER DERIVE A
  SCOPE DECISION — *"X is impossible"*, *"that approach is dead"*, *"do not try Y"* — FROM AN
  UNVERIFIED MECHANISM.** Write the observation as fact; write the mechanism as a hypothesis with its
  evidence, or as **"cause not established"**. A scope decision may rest **only on the observation**.
  **Why this is an invariant and not merely a gotcha (→ G120):** an unverified mechanism that produces
  a false *positive* (G116) or a false *null* (G114) is eventually caught, because someone re-runs the
  measurement. **A false FORECLOSURE is never re-run — by definition, because foreclosing is the act of
  telling everyone not to.** It is the most durable failure this project can commit. Measured instance:
  G87 attached *"the redirect is ACTIVE … every approach is dead"* to a true observation and closed off
  `MainWorld` for ~13 days; the map was simply **not cooked into the build**, which one launch and one
  log read settled. The same discipline `CaptureBench/tools/check_pose.py` already applies to a failing
  gate — print the numbers, name the discriminator, let the reader attribute — **applies to prose.**
- **Source carries NO comments — by deliberate convention.** Every source file (C++ `.h/.cpp`, C#
  `.Build.cs`, Python, `.bat`) is kept comment-free, *including* the top-of-file copyright/banner header.
  **Do NOT add comments; strip any before committing.** (Feature work keeps re-introducing them — first
  stripped in `d4a77db`, re-stripped 2026-07-08 after the `AnomalyCapture` module re-added them.) Enforced
  with a deterministic, byte-preserving stripper kept in the workspace root alongside the two repos
  (`_strip_comments.py`): run `python _strip_comments.py <repo-root>`. It removes only comments while
  preserving every other byte — all string/char/template/regex literal contents, CRLF endings, and the BOM
  (idempotent; validated byte-identical against the original strip). Put rationale and design notes in commit
  messages, `docs/`, and the session journals — **never in code.** `LICENSE.txt` and the `.uplugin` JSON are
  intentionally exempt (not source).
- **Plugin stays game-agnostic.** The `AnomalyInjector` module may depend only on
  `Core`/`CoreUObject`/`Engine`/`InputCore`/**`Foliage`** (later `Renderer`/`RenderCore`/`RHI`/`Slate`)
  and must **never `#include` or reference host game-module types** (e.g. anything from the
  `StackOBot` module). Host-specific buildability lives in the project, never in the plugin.
  ⚖ **`Foliage` ADDED 2026-08-20 BY OWNER RULING, at `m27`, and recorded here as a ruling rather
  than left as drift** — same treatment `InputCore` got at `m5` ("first dep since M0"). **The
  invariant is about not depending on HOST types; `Foliage` is an ENGINE Runtime module present in
  every UE build** (`Runtime/Foliage/Foliage.Build.cs` has no editor gating and no `ModuleType`
  override). It exists so `IsRenderableComponent` can exclude `AInstancedFoliageActor` **by TYPE**.
  🚨 **The alternative — a class-NAME string match — was REFUSED, and the reason is the rule:** a
  rename would make a name match SILENTLY STOP EXCLUDING, whereas a type reference **breaks the
  build**. A compile error is the loudest failure available; a missing check must never read as a
  passed check. ⛔ **Do NOT add a string match "as belt and braces"** — two mechanisms means one can
  rot unnoticed while the other covers for it. It is `PrivateDependencyModuleNames`, so it does not
  propagate to `AnomalyCapture` or `AnomalyControlServer`.
- **Matching is label-free.** Targeting matches by actor Name or Class only.
  `GetActorLabel()` is editor-only and absent in cooked builds — `ListActors` may print the
  label (guarded by `WITH_EDITOR`) but nothing matches on it.

## Workflow & doc-maintenance rules
- **Two-Claude split.** Design decisions come from an orchestrating "chat Claude" and are
  ferried by Kavin (project owner). The implementing Claude implements. Genuine design forks
  or ambiguities are surfaced back (listed standalone), not improvised.
- 🆕 **HOW BRIEFS ARRIVE, AND WHERE THE REPORT GOES (2026-09-02, session 068).** Briefs are delivered
  as files through the mailbox **`D:\IntrusiveAnomalies\_mailbox`**, which sits **OUTSIDE every repo**
  — ⛔ **never write into it and never stage anything from it.**
  In a headless run **the FINAL MESSAGE IS THE REPORT** chat reads; nothing else is seen, so
  everything that matters goes there, and a blocked fork is reported as `NEEDS-DECISION` rather than
  waited on.
- ⚠ **CONVENTIONS IN SHARED DOCS MUST NAME THEIR ADDRESSEE — and this one does.**
  `docs/CHAT-HANDOFF-s3-m24-capture-migration.md` §13 "Working agreements" describes
  **chat-Claude's** output conventions to the owner. **Claude Code does NOT emit the 🔴 marker and does
  NOT route decisions to the owner** — the owner does not make technical calls, so an unresolved call
  goes to **chat-Claude**, flagged in plain text (e.g. `CHAT-DECISION REQUIRED: …`) as loudly as it
  deserves. Plain-language summaries are mandatory for chat-Claude, **optional** for Code; Code's
  technical density is correct as-is. *(2026-08-19: a fresh Code session was told to read that handoff,
  read §13 as its own contract, and emitted 🔴 markers plus owner-addressed decisions in an S4 plan.
  The section was right; it never named who it was for. → **G111**.)*
- **Reports to Chat go in a copy block.** Any status/gate/handoff report meant to be ferried
  back to the orchestrating chat Claude must be emitted as a single fenced code block (so the
  owner can copy-paste it verbatim). Applies to stage-gate results, plan summaries, and any
  "report back" deliverable.
- **Plan-before-code.** A new milestone's first response is a file-by-file plan only; no
  implementation until approved.
- ⚠ **PRE-COMMIT HABIT (standing, 2026-08-19): read `git diff --stat` BEFORE every commit. A diffstat
  wildly larger than the intended change HALTS the commit** — it is almost always an **encoding smell**
  (**G115**): a shell round-trip (`Get-Content -Raw` → `Out-File`/`Set-Content`) re-encodes the whole
  file, rewriting every non-ASCII line and adding a BOM, **while the text still reads correctly**. Use
  the editor tool for edits to tracked files. *Measured twice in one turn — 377/377 and 700/685 on files
  meant to change in a handful of places — the second time two minutes after the lesson was written
  down, which is why this is a mechanical check and not a note.* **Deliberately NOT automated.**
- **Commits — Conventional Commits.** Prefixes: `feat:` (new anomaly or capability), `fix:` (bug),
  `docs:` (doc-only), `refactor:` (no behavior change), `chore:` (build/tooling). Scope anomaly-specific
  changes, e.g. `feat(blinking): …`. **Tag each milestone** with `git tag m<N>` after its commit so
  milestones diff cleanly (`m1..m2`, and a changelog can be auto-derived later). The git repo is the
  plugin folder (`master`); host scaffolding lives outside it and is not committed here. **Before every
  commit, run the comment stripper (see Invariants) — the source must stay comment-free.**
- ⚠ **COLD-START TAG VERIFICATION — USE `rev-parse`, NOT `git tag -l --format`.** To confirm which
  commit a milestone tag points at:
  ```
  git rev-parse --short m26^{commit}
  git rev-parse --short m27^{commit}
  ```
  **Why this is written down: our tags are ANNOTATED, so `git tag -l m27 --format='%(objectname:short)'`
  prints the TAG OBJECT's hash, not the commit's** — `m26` reads `4328961` and `m27` reads `1756f52`
  that way, against the real commits `d6bee7a` and `4a92962`. A cold session comparing those against a
  handoff doc sees a MISMATCH ON A REPO THAT IS CORRECT, and the pre-declared response to a bootstrap
  mismatch is to halt. `%(*objectname:short)` (with the asterisk) also dereferences, but `rev-parse
  <tag>^{commit}` is the form that cannot be got subtly wrong. *(2026-08-20, m28 Stage 0 — the
  instruction sheet itself carried the broken form; `G142`'s shape, the CHECKER wrong and not the
  build.)*
- **PUSH — CODE OWNS PUSHES (standing rule, 2026-07-29; SUPERSEDES the old "owner owns remote pushes").**
  When work is committed and gated, **push it yourself, including tags.** Do not wait for the owner to push —
  the old rule added a round trip and nothing else. **KEEP:** before pushing, report `git status` +
  `git log origin/master..master` so what went up is on the record — that is a **LOG, not an approval
  request**; report it and push **in the same turn**. (Rationale: two tracks share this repo and entangled
  once — G43 — so the record matters; the gate does not.) **KEEP:** **never force-push on a rejection** —
  stop and flag it to the owner. **Auth stays with the owner; never handle credentials.**
  ⚠ **This does NOT dissolve owner-owned QUALITY gates.** An owner Play-gate/eyeball smoke still comes
  **before** a tag when one is required — the hold there is the **smoke gate**, not push ownership.
- **Doc discipline — leave the docs able to (a) cold-start a fresh session and (b) explain the
  whole plugin to any UE dev.** When you start or advance a milestone you MUST, before the session
  closes:
  1. Update **Current status** (above) — the single "you are here" marker (latest as-built /
     in flight / open decisions / next action).
  2. Update **docs/architecture.md** to match the new as-built state, including the **anomaly
     catalog** — describe current code only, never aspirational.
  3. Write/append the **session journal** under `docs/sessions/` (history + the milestone plan +
     design decisions, including any open/blocking decisions).
  4. **Append** new lessons to `docs/gotchas.md` (never delete; supersede).
  5. Keep `docs/setup-runbook.md` and `docs/onboarding.md` current with the build/run steps and
     the control surface as they actually are.
  - Division of labor: **architecture.md = current state** ("what it is"); **journals = history +
    plans** ("how we got here / where we're going"); **runbook = repro**; **gotchas = lessons**.
