# Chat handoff — m27 shipped: the cure is ON by default, the dashboard is back in the browser

**Session date:** 2026-08-20
**Plugin repo:** `AnomalyInjector` — HEAD `485b467`, clean, pushed. **Tag `m27` at `4a92962`, pushed.** `m26` at `d6bee7a`, unmoved.
**Dashboard repo:** `AnomDash` — HEAD `b8273c2`, clean, pushed.
**Audience:** a cold reader — fresh chat, Claude Code, or a collaborator.

**Read the earlier set if you have not:** `CHAT-HANDOFF-s2-*` (three) → `CHAT-HANDOFF-s3-m24-capture-migration.md` → `CHAT-HANDOFF-m26-h5-cure.md` → `CHAT-HANDOFF-046-h6-documented-not-fixed.md`.
**This doc is the seventh and supersedes the sixth's status lines and forward plan.** Everything the sixth says about H6, I11-A and I11-B still stands as the record of how the defect was found.

**One-line status:** m26's cure was **off by default**, so delivered builds behaved as m25 and the client's original complaint came back. **m27 fixes that** — an ini key turns it on, a per-event readout says what was deleted, foliage is excluded from selection, and the dashboard is back to the browser workflow. All gates green, owner smoke passed. **H6 is still documented, not fixed.**

---

## 1. What m27 is

| deliverable | what it does |
|---|---|
| `bMaskMeasureDefault` | `[AnomalyCapture]` ini key, gates measure + report + **veto** together. Compiled default stays `false`; the client ini sets it `True`. `IAI.Capture.Mask` is the between-runs bisect and overrides it. |
| The unconditional echo | Every run logs `READ THIS LINE, NOT THE INI` with the effective mask value and its provenance. **Before m27 the log said nothing at all when the mask was off** — a missing key was indistinguishable from a deliberate off. |
| `VETOED-OBJECT` | One log line per deleted event: target, `asset_name`, `component_class`, state, `maxCount`, translucency verdict. **One literal token, verified unique in the codebase** — the owner greps for it. |
| `run_summary` +2 | `translucent_vetoes`, `translucency_unknown_vetoes`. UNKNOWN is a **distinct** counter and never folds into "opaque". |
| Foliage exclusion | `AInstancedFoliageActor` removed from the renderable-visible set at the G33 chokepoint. `Foliage` added to `PrivateDependencyModuleNames`. |
| Browser dashboard | `Run.bat` / `Setup.bat` serve `dist/` via `serve_dashboard.py`. Desktop app and WebView2 removed from the flow; the Tauri path stays in the repo, intact and unreferenced. |
| Packaging script | Prompts for a destination, builds a complete client bundle from an **explicit allowlist**, reaches cross-repo for `client-readme.md`, fails loudly with no bundle if anything is missing. |
| `client-readme.md` | Rewritten for the browser workflow. **The two-monitor note from the client's first feedback round landed here** — outstanding since m22. |

**`annotation.json`'s field set is unchanged. P6 did not move.** No sidecar. No ratio, no threshold, anywhere.

---

## 2. Current state

| | |
|---|---|
| `AnomalyInjector` HEAD | `485b467` — clean, pushed |
| **Tag `m27`** | **`4a92962`** — annotated, carries **seven limits** in its message |
| Tag `m26` | `d6bee7a` — unmoved |
| `AnomDash` HEAD | `b8273c2` — clean, pushed |
| `feature/stencil-capture` | `76cac74` — untouched, never checked out, not mined |
| Build quartet (G121) | exe `18081D39` + utoc `72262793` + ucas `6C26C482` + pak `0BEA8D24` |
| Preserved no-key cook | `_binary_baselines\m27-cook1-nokey-build\` — **Gate 3 leg 1 tests the ABSENCE of the key and is only reproducible there** |
| Banked smoke | `_bench_sessions_bank\M27_OWNER_PLAYGATE_SMOKE\` — both sessions + `StackOBot.log` |
| New gotchas | **G139–G142** |

**m27 is four commits, deliberately not squashed** (`0d5e458` capture · `409b67a` viewport/foliage · `9f86600` client docs · `3b91fe4` status), plus `4a92962` (findings, tagged) and `485b467` (status). The "one milestone one commit" convention was honoured in purpose rather than letter: squashing would have hidden which change carries which gate.

**G121, third instance:** the exe hash was **identical** across both m27 cooks while all three container files changed. The exe half carried **zero** information.

---

## 3. Gates — all green

| gate | result |
|---|---|
| **1** `bTagFailed` cannot manufacture a `MEASURED_ZERO` | PASS, from source. The `continue` precedes arm, counter and record insert. |
| **P** the bundler, run twice | PASS. 9/9 entries, served from an empty folder with no repo access. Bogus repo path → exit 2, no bundle. |
| **F** foliage exclusion, packaged | PASS. Zero foliage across all three artifacts; six distinct other actors. `DumpVisible` set-identity half **NOT RUN** and reported as such. |
| **2** browser path end to end | PASS. Token read from the **running process's own log**, `config.json` 200 OK, dashboard LIVE with real state. |
| **3** the ini key, four legs | PASS. Key absent → off, compiled default. Key True → on, from ini. Bisect beats ini. Delivery ON identical to delivery OFF. |

**The count gate is the best-constructed thing in the milestone.** Legs 1 and 2 ran the **same seed, map and exe**, so leg 1 is a true before-picture. Differencing the event sets by `(target, anomaly_type, start_frame)`: **6 missing, `vetoed_events` = 6, and the six missing are exactly the six named in the `VETOED-OBJECT` lines.** The counter is verified against the artifact, not against itself.

**G88 satisfied positively**, not merely avoided: the key was read back **out of the cooked pak**, the sentinel string was absent, and there was no loose Config directory beside the package.

---

## 4. Findings from this milestone

### 4.1 THE CURE'S REACH IS VIEW-DEPENDENT AND CAN BE LOW — the most decision-relevant thing here

Owner play-gate smoke, **two runs eleven minutes apart on the same build**, in real gameplay:

| run | NOT_MEASURED | `mask_nopass_discards` |
|---|---|---|
| 211024 | **1 of 8** | 4 |
| 211345 | **5 of 8** | 20 |

Those events were **admitted because nothing was measured**, not because they were measured to draw. On 211345 the cure was effectively **inert for five of eight events**.

**This is the designed safe direction and not a defect.** `NOT_MEASURED` → ADMIT. Nothing was wrongly deleted.

**No mechanism claimed (G120).** `framesNoPass` is not a Nanite counter — frustum culling and any route by which the target is absent from the view's relevant set reach the same mechanism. **One** of the five is established: `SM_Ramp2` is the known-Nanite control and must read `NOT_MEASURED` every time (G134). The other four were not chased.

**Why it was invisible until now, and this is the transferable part:** the bench legs **structurally could not have shown it**. They run unattended with a settled camera and read `notMeasured=0`. A moving camera with real streaming swung the rate from 1-in-8 to 5-in-8 between two runs minutes apart. **An owner-played run is a different instrument from a bench leg and sees things the bench cannot.**

**Consequence, decision-relevant and not yet decided:** how much of H5 the cure catches in a *client* capture is variable and can be small. Any future statement to a client about what the cure does must carry that.

### 4.2 The accepted cost has its first measurement — and the same run complicates it

`translucent_vetoes = 1`. One translucent target deleted. **The first number this project has ever had on route (e).**

But the same actor, same session, 32 frames apart:

| anomaly | state | maxCount |
|---|---|---|
| `blinking` | `MEASURED_ZERO` | 0 |
| `missing_texture` | `MEASURED_NONZERO` | 114,724 (12.4% of frame) |

Both fully interpretable — all discard counters zero on both. **If translucency simply prevented custom-depth writes, this actor would read zero in both. It did not.**

> **`translucent_vetoes` counts a PROPERTY OF THE TARGET. It is NOT evidence that translucency caused the zero.** The shipped log hedge is load-bearing, not decorative. Do not "tidy" it.

**CANDIDATE, UNVERIFIED, CHAT-ORIGIN, NOT AUTHORISED FOR INVESTIGATION:** `missing_texture` swaps the slot's material. If the swap material is **opaque**, the slot can write custom depth during that event while `blinking` leaves it translucent and cannot. The translucency readout is taken at `FinishRun`, *after* revert, so both read TRANSLUCENT regardless. **If this holds, the two readings are consistent and route (e) is confirmed, not refuted** — and the veto's harm on translucent targets would be **anomaly-type-dependent**. Cheapest test: read the blend mode of the corruption material. One property read. **Not claimed, not tested.** The `skippedHidden` 5-vs-0 difference is a separate unchased lead.

### 4.3 Deliverable A3 is PARTIAL — filed, not fixed

`IAI.Capture.Mask`'s console help still opens *"m26 SLICES 1+2 — MEASURE AND REPORT (default OFF)"*. Stale twice over: it describes slices 1+2 against a body describing slice 3, and "(default OFF)" is now only the *compiled* default.

**Not fixed, deliberately.** A rebuild and re-cook moves the exe and **invalidates all four Gate 3 legs**, for a string no gate ever read. **Clearing rule: fold into the next milestone that already requires a cook, together with G118's cooked placeholder token.** Filed at G139 and in CLAUDE.md's status block.

Related, also filed: when the mask is **off**, the StartRun echo is a **disjunction** — "compiled default OR `IAI.Capture.Mask`" — which alone cannot separate "key missing" from "console turned it off", the exact pair G139 exists to separate. **The Initialize banner + the StartRun line resolve it as a pair; the StartRun line alone does not.** Anyone debugging a delivered session needs both.

### 4.4 Two chat predictions corrected by measurement

- **"The foliage exclusion cuts a third of the selectable variety, six down to four."** **Wrong.** The count **held at six** — seeded selection backfilled with `BP_SpawnPad_C` and `SM_Ramp2`, neither of which appeared pre-change. The count held; the **membership** changed. n=1, one map, one pose. **G140 is unaffected**: the same seed picks different targets, so banked MainWorld auto-pool runs remain non-comparable across `409b67a`.
- **"Watch `customStencilExtent` to learn whether Concorde writes custom depth."** **Wrong, and it was in an earlier brief.** I11-B established the plugin's *own* tags drive that value view-sized. It will read view-sized regardless and says nothing about the host. **The host-custom-depth question needs its own census** — see §6.

---

## 5. H6 — five routes now, still not fixed

The set is **not closed**. Routes (b) through (e) are all **Nanite-independent**; the Support-Nanite argument covers route (a) **only**.

| route | harm | status |
|---|---|---|
| **(a)** Nanite — cannot write custom depth on 5.1 (G134) | HIGH | inert in Concorde as configured (Support Nanite unticked + `r.Nanite.ProxyRenderMode` at 0) |
| **(b)** off-screen | LOW | deletion arguably correct, reached unsoundly |
| **(c)** occlusion-culled | LOW | this is what H4 says should happen |
| **(d)** degenerate geometry | LOW | this is H5 class (i), the case the cure exists to catch |
| **(e)** translucent materials | **HIGH** | source-verified; **self-sufficient — needs no second tagged actor**. See §4.2: the practical claim is now weaker than the source trace. |

**Mitigation in code:** `MaxCount` is a MAX across contributing frames, so **one** real non-zero reading survives any number of phantom zeros. The exposed case is no real evidence on **every** armed frame.

**Owner decision stands: documented, not fixed.** Chat recommended fixing it; the disagreement is recorded so it stays visible rather than laundered.

---

## 6. The forward plan

### 6.1 IMMEDIATE — office machine, Concorde. Code cannot reach that box.

1. **Pull.** `git status` must be empty first — the clone is at `28bc6f1`, five milestones behind. Use HEAD, not the tag.
2. **Add the key to Concorde's `Config/DefaultGame.ini` BEFORE cooking:**
   ```
   [AnomalyCapture]
   bMaskMeasureDefault=True
   bDeliveryModeDefault=True
   ```
   G88: a loose ini beside a package is a **silent no-op**.
3. **Rebuild the editor target FIRST, then cook.** m26's shader module means a cook before the rebuild fails with `Missing global shader FAnomalyVisibleMaskPS's permutation 0` and produces no artifact. **The plugin has never been built against Concorde** — game-agnostic invariant says it should compile, but "should" is not "has". G86's compiler pin is the known failure shape.
4. **PIE sanity check before cooking** — `IAI.ListAnomalies` (expect 8), short capture, find `READ THIS LINE, NOT THE INI`. If it says COMPILED DEFAULT, the key did not land; fix before spending a cook.
5. **Capture, then read in this order:** `vetoed_events` → grep `VETOED-OBJECT` → `translucent_vetoes`. **Copy captures out of `Saved/AnomalyCaptures` immediately** — five unbanked-evidence incidents so far.
6. **The Concorde custom-depth census** — does the host itself write custom depth (outlines, highlights, post-process masks)? On the pre-delivery checklist, **never assessed on any title**, and if it does, H6's precondition is met permanently there. Python one-liner in the editor; `customStencilExtent` does **not** answer this.
7. **Concorde's Nanite-flagged asset count** — recorded **UNKNOWN** (A60). The 5.1 Content Browser has no Nanite filter. Must not be reconstructed from StackOBot's 244/350.

### 6.2 Then, in order

1. **The over-claim rule** — the other half of H5, and the m27-era headline candidate. **Newly unblocked**: `SM_GratIng` is a complex-silhouette **non-Nanite** mesh with ~10 instances in shipped MainWorld — exactly the calibration target the measured set was missing when the ratio was refused. Its drawn-to-claimed ratio has **deliberately never been measured**.
2. **Route (e)'s one-property test** (§4.2) — if the swap material is opaque, route (e) is confirmed and its harm is anomaly-type-dependent. Cheapest open question in the project.
3. **Resolution selection** and **PNG→JPEG** — the only two client feature requests still untouched. Resolution is load-bearing (`bbox_norm` survives, `bbox_px` and coverage are resolution-derived). JPEG needs an A/B first: magenta `missing_texture` will survive q90–95; subtle lighting mismatches and LOD seams are where compression could eat the evidence.
4. **The defaults profile** — a document, not code. Send a concrete proposal to react to, not an open question.
5. **The H6 fix**, if ever authorised. Shape is known: replace the view-level `bPassRan` precondition with per-target evidence. `AnomalyVisibleMask.usf:25-30` already evaluates the tag **before** the depth test and discards that count. **Not designed, not queued.**
6. **L3** — stays deliberately unfixed and is **live in m27's own artifacts**. Owner-side overlay tooling will draw boxes for vetoed events.
7. H4 path (a) parked · H5 class (i) · P1 (H1 its only lead, no lever) · P5/P7 · P6 · B2 · B1-NDC · A17/A19.

### 6.3 UNRESOLVED, and it gates several of the above

**Is the client channel open?** The record says closed in both directions, permanently. It was asked twice this session and not answered. If it is still closed, items 3 and 4 above are **cancelled, not pending**, and the m27 cure has nowhere to go. **A fresh chat should establish this before planning client-facing work.**

---

## 7. Working agreements — carry these verbatim

**Roles.** Kavin is tech lead and project owner. He does **not** write code and does **not** make technical decisions — both are fully delegated. **Chat-Claude makes all design and technical calls** and writes the exact paste-back blocks. **Claude Code implements.** Kavin ferries messages, runs smoke tests, and applies executive judgement **only** on genuine product/scope tradeoffs.

**🔴 THE RED-CIRCLE RULE.** Mark with 🔴 every item he MUST read, and nothing else. Only three earn it: **ACT**, **DECIDE** (a closed choice with a recommendation attached, never open-ended), **HEADS-UP** (something that changes what ships, or a risk accepted on his behalf). Marked items go at the **TOP**.

**📊 STATUS LINE.** One line at the top of every reply: milestone, rough % complete, whether things are going well, turns remaining — **and say what the percentage is measuring.**

**Plain-language summaries, both directions, every exchange.** Scannable, no jargon. Lead with them, then the verdict, then the paste-back block, then what was proposed plus the **Claude Code effort setting**.

**Vocabulary discipline.** "**Bank**" means archiving session evidence to the bank folder **and nothing else**. Do not overload project terms.

**⚠ HALT DISCIPLINE — a lesson from this session.** Nearly every halt was a stop condition **chat wrote into a brief**, not Code hesitating. Each one costs the owner a ferry round trip. **Write stop conditions where a wrong answer would be expensive or invisible; do not write them by default.** Code stopping *itself* on a real inconsistency is a different and welcome thing — do not discourage it.

**Discipline that has repeatedly paid.** Plan-before-code. Stage gates with concrete thresholds. Stop-on-failure, **no same-turn fixes to a validity instrument**. Predictions pre-declared as a committed file before the instrument exists, restated **verbatim** before results. **Measure then design.** Numbers never reused. Conventional Commits; Code commits **and** pushes including tags, reporting for the record not for approval. **Never force-push. Tags are never rewritten.**

**The amendment rule.** A pre-declaration may be amended **before any measurement exists** and **only to TIGHTEN** a validity condition. Never to loosen. A prediction or verdict is never amended once the instrument exists. Every amendment is dated, appends, deletes nothing.

**No tolerance may be proposed** where nothing calibrates it — G135's shape. Refused twice: the veto ratio, and pose. Every gate in the I11 design is **categorical, not comparative**.

---

## 8. Standing lessons, with this milestone's receipts

- **A verification script is a defect surface of its own, and its failures wear the costume of a build failure** (G142, new). Two defects in Code's own checker would each have manufactured a false "COUNTS DISAGREE — STOP".
- **Report your own tool as broken rather than quote a number you do not believe.** Three instances this session.
- **A false positive in your own verification is worse than no verification.** `Select-String` is case-insensitive and matched an unrelated line; the check was reported **not run** rather than passed.
- **An absence-of-finding is weaker than a positive measurement and is only as good as the surface searched** (G136). The PIE foreclosure survived only because the owner's memory challenged it.
- **A validity gate written CATEGORICALLY survives a wrong expectation** — "must DIFFER", not "must be 1280x720"; the real PIE panel was 876x872.
- **A guard that has never fired is not a guard.** Every new check this milestone was proven by breaking it on purpose.
- **The log can be the only copy of a result** (G92, fifth incident). A vetoed event leaves **no trace** in `annotation.json`.
- **Build identity is a quartet** (G121, third instance).
- **PowerShell's `-Encoding utf8` emits a BOM** (G141) — bit this session twice.

**Chat-side errors on record (this session):** wrote a brief for a machine Code cannot reach; misread a StackOBot screenshot as Concorde; framed H6 as Nanite-only when one of its proven routes is not; left a hole where Arm A had no on-screen requirement; asked for `git status` on a directory that is not a repo; overloaded "bank"; quoted a percentage against a moving target; **carried a two-month-old unverified explanation into a brief as an established mechanism** (the July foliage story, refuted by our own banked data); **used the wrong denominator** for the foliage cost; **predicted the pool would drop six→four when it held at six**; **told the owner `customStencilExtent` would answer the host-custom-depth question when it cannot**.

---

## 9. Pointers

**Plugin repo:** `CLAUDE.md` (status block, refreshed at `485b467`, carries the A3 gap) · `docs/invisible-anomaly-mechanisms.md` (**the ledger — read this second**; H6's five routes, the view-dependent-reach finding, the foliage justification) · `docs/PRE-DELIVERY-CHECKLIST.md` (§1's two host boxes, and the note that they cover route (a) only) · `docs/client-delivery.md` · `docs/client-readme.md` (client-facing, rewritten) · `docs/predictions/` (I11, I11-A, I11-B — dated amendment lineage, zero deletions) · `docs/gotchas.md` (**G139–G142 new**) · `docs/sessions/2026-08-20-046-*.md` (§§22–26 cover m27) · `docs/setup-runbook.md` §8.6.

**Dashboard repo:** `host-tools/serve_dashboard.py`, `Run.bat`, `Setup.bat`, the packaging script and its manifest, `host-tools/README.md`.

**Banked evidence:** `D:\IntrusiveAnomalies\_bench_sessions_bank` — `M27_*` legs and `M27_OWNER_PLAYGATE_SMOKE` (both sessions **plus** `StackOBot.log`). **Re-bank before any staging step (G92); match BY SESSION ID, never by directory name.**

**Engine source:** `D:\UESource\UnrealEngine` — the Nanite fallback chain (§4.8 of the previous handoff) and the custom-depth translucency trace both cite files and lines there.
