# Pre-delivery checklist

Tick this before handing a bundle to a client. Every line here exists because it went wrong, or could
have gone wrong silently — the failure mode of each is written next to it, because the whole point is
that none of these announce themselves.

Companion docs: `client-delivery.md` (owner-facing: what delivery mode does and why), `client-readme.md`
(the client's own guide, shipped in the bundle), `capture-fps.md` (choosing a sustainable rate).

---

## 1. Game build — `Config/DefaultGame.ini`

- [ ] 🚨 **HOST PROJECT: `Project Settings > Engine > Rendering > Nanite > Support Nanite` is
      DISABLED.**
      *If it is ENABLED, `H6`'s high-harm route goes live across **every Nanite-flagged mesh at
      once, with no change to this plugin**: a fully visible, drawing target can be measured at zero
      and DELETED from `annotation.json`. The whole decision not to fix `H6` rests on this box, and
      it is a HOST setting that nobody here controls.*
      ⚠ **Also confirm `r.Nanite.ProxyRenderMode` is at its default `0`** — it is a **scalability**
      cvar and an ini, device profile or scalability group can set it without anyone opening project
      settings. Non-zero means Nanite-flagged meshes render NOTHING at all.
      → `docs/invisible-anomaly-mechanisms.md`, **"`H6` — DOCUMENTED, NOT FIXED"**.

- [ ] 🚨 **HOST PROJECT: the game does NOT itself write custom depth** — outlines, selection
      highlights, post-process masks, any `bRenderCustomDepth` / `SetRenderCustomDepth` usage.
      *A host that writes custom depth supplies `H6`'s precondition **permanently and independently
      of Nanite**, so the "Support Nanite is off" argument above does not cover it.* ⛔ **NEVER
      ASSESSED on any host title — this box is the first time it is being asked.** Grep the host's
      own source and Blueprints for `bRenderCustomDepth` / "Render CustomDepth Pass".
      → same entry.

- [ ] ⚠ **READ THIS BEFORE TICKING EITHER BOX ABOVE: THEY DO NOT COVER `H6`.**
      *The two boxes above close `H6`'s **Nanite** route only. Three further routes are
      named and **none of them depends on Nanite** — a fully **occlusion-culled** target,
      **degenerate geometry**, and 🚨 **a TRANSLUCENT-material target, which is HIGH HARM
      and self-sufficient: it tags, it makes the custom-depth pass run, it writes
      nothing, and its label is deleted while the object is plainly visible and drawing.***
      ⛔ **Ticking the Nanite boxes does NOT make the delivered build safe from `H6`.**
      **ACCEPTED BY OWNER RULING (Option A, 2026-08-20) as the cost of shipping the mask
      ON.** Chat recommended fixing route (e) first because its failure is invisible to
      the client; the owner ruled otherwise and the disagreement is recorded.
      📌 **The translucent population is UNMEASURED on any delivered title.** `m27` logs
      it per vetoed event so it stops being unmeasured — read
      `translucent_vetoes` / the per-veto log lines in a delivered session.
      → `docs/invisible-anomaly-mechanisms.md`, **"`H6` — DOCUMENTED, NOT FIXED"**,
      routes (a)–(e).

- [ ] **`IAI.ListAnomalies` returns the count recorded in `CLAUDE.md`'s Current-status block, and every
      id listed there is present.**
      *Phrased CATEGORICALLY, against a single source, and never as a number in this file. A literal
      count here goes stale the moment an anomaly ships and then reads as a passing check —
      `setup-runbook.md` asserted "seven" from m3 until m29 while the catalog had been 8 since m8.
      The failure mode is `G119`'s: a check that cannot fail is not a check.*

      ⚠ **Also assert the DELIVERED POOL, not just the catalog** — the startup line
      `AutoInjector subsystem initialized ... Default pool: <ids>` states what a client capture will
      actually fire, and it **must match the pool recorded in `CLAUDE.md`'s Current-status block,
      id for id**. Catalog membership and pool membership are different things, and only the second
      changes the delivered dataset. *(Also phrased against the single source, for the same reason:
      a pool list written out here would go stale the next time a member is added.)*

- [ ] **`[AnomalyControlServer] Token` is set to a long random value — RUN THE CHECK, do not read it.**
      *Absent → the server falls back to a random per-session token that a client with no console can
      never read; the dashboard cannot connect at all.*

      ⛔ **A PLACEHOLDER TOKEN HAS COME BACK ONCE ALREADY** after `m16` recorded it reverted, because the
      file is **outside version control** and two other legs of the dev pair kept the value alive
      (**G112**). Ticking a box did not catch it and will not. Run this instead — it exits non-zero and
      names the fault:

      ```powershell
      $ini = "D:\IntrusiveAnomalies\StackOBot\Config\DefaultGame.ini"
      $m = [regex]::Match((Get-Content $ini -Raw), '(?m)^\s*Token\s*=\s*(\S+)\s*$')
      if (-not $m.Success) { Write-Host "FAIL: no Token key" -Fore Red; exit 1 }
      $t = $m.Groups[1].Value
      if ($t -match 'TESTVALUE|CHANGEME|placeholder|^TEST$') { Write-Host "FAIL: PLACEHOLDER token" -Fore Red; exit 1 }
      if ($t.Length -lt 32) { Write-Host "FAIL: token is $($t.Length) chars, need >= 32" -Fore Red; exit 1 }
      Write-Host "PASS: token present, $($t.Length) chars, not a placeholder" -Fore Green
      ```

      Then confirm the **dashboard** half matches: `<delivery root>/dashboard/config.json` →
      `controlToken`. The two must be identical or the client's dashboard cannot connect.
      ⚠ **There is no pre-cook or pre-stage script in this project** — cooking and staging are run by
      hand from `setup-runbook.md` §8 — so this check has nowhere automatic to live. It runs here or
      it does not run.

- [ ] 🚨 **THE CHECK ABOVE IS NECESSARY BUT *NOT* SUFFICIENT — IT READS THE SOURCE INI, WHICH IS NOT
      WHAT THE BUILD ENFORCES. RUN THE READ-BACK BELOW OR THE TOKEN IS UNVERIFIED.**
      *A packaged build enforces the **COOKED** copy of `DefaultGame.ini` baked into its pak at cook
      time. Rotating the source ini changes nothing for a build already cooked, so the check above
      returns **PASS** on a build enforcing a placeholder (**G118**). Measured on the m25 staged binary:
      source ini 64 chars, build enforcing `TESTVALUE123`.*
      ⚠ **The build's log line says `(from DefaultGame.ini …)` and means the COOKED one.** Matching it
      against the source file is exactly the mistake.

      **A build is checked by what it ENFORCES, not by what its source says.** Start the build, read the
      token out of its own log, assert on *that*:

      ```powershell
      # start the packaged build briefly (any map), then:
      $log = "<staged>\StackOBot\Saved\Logs\StackOBot.log"
      $m = Select-String -Path $log -Pattern 'Control server token:\s*(\S+)\s*\(' | Select-Object -Last 1
      if (-not $m) { Write-Host "FAIL: build never logged a token (server not started?)" -Fore Red; exit 1 }
      $t = $m.Matches[0].Groups[1].Value
      if ($t -match 'TESTVALUE|CHANGEME|placeholder|^TEST$') { Write-Host "FAIL: BUILD ENFORCES A PLACEHOLDER: $t" -Fore Red; exit 1 }
      if ($t.Length -lt 32) { Write-Host "FAIL: enforced token is $($t.Length) chars, need >= 32" -Fore Red; exit 1 }
      Write-Host "PASS: BUILD ENFORCES a $($t.Length)-char non-placeholder token" -Fore Green
      ```

      *The log line appears once `IAI.Server.Start` has run, so include it in `-ExecCmds` for the probe
      launch. `CaptureBench/tools/ws_scoping_echo.ps1` already does this read-back and prints
      source-vs-enforced side by side.*
      ⛔ **If the two disagree, the build is STALE relative to the config and must be RE-COOKED before
      delivery.** Do not ship on the strength of the source file. → **G118**, **G119**.
- [ ] **GRAB POINT — confirm which one the build ships on, from the LOG, not from memory.**
      *Since S4 the default is the **SVE / scene-colour** path and **delivered frames contain NO game
      UI**. The startup log states it and where the default came from:*
      `Grab point: sve/scene-colour (… UI EXCLUDED), default from S4 COMPILED-IN DEFAULT …`
      *To ship a **UI-included** build instead, set `[AnomalyCapture] bSveCaptureDefault=False`
      (or `IAI.Capture.SVE 0` at runtime) and confirm the banner reads `UI INCLUDED`.*
      *Cross-check the artifact too: `run_summary.json` → `capture_path` is `"sve"` or `"backbuffer"`
      on every session since S4-3, so a delivered session states what produced it.*
- [ ] **`[AnomalyCapture] bDeliveryModeDefault=True`.**
      *Absent → the client's sessions ship `labels.jsonl` + `run.json`, which includes the seed. Delivery
      mode is what limits output to the client-facing artifacts.*
- [ ] **`[AnomalyCapture] ContentClockDefault` is NOT set to `game`** (leave it absent → `wall`).
      *`game` on a wall-clock title stamps a slow run at target and plays the client's videos ~2× fast —
      the Issue-2 regression. `game` is only correct for game-clock content such as StackOBot itself.*
- [ ] Build is **Development or Test**, not Shipping.
      *Capture and the control server are compiled out of Shipping entirely.*
- [ ] 🚨 **`IAI.Capture.MaskProbe` is OFF in anything that ships — check what the BUILD does, not what
      you intended.** *The probe is a GATE ARTEFACT (m26, F-6 item 5): under the flag it deliberately
      bypasses `LOCK-1` for one arm per run to prove the mask's detectors are live. It defaults OFF and
      is INERT in delivery mode by a code guard regardless of the flag — but this line exists for the
      same reason the token read-back does (G118/G119): a bench convenience left ON is invisible until
      it isn't. The read-back is in every capture run's log:*
      `Capture(mask): probe EFFECTIVE=0 (flag=0, deliveryMode=1 ...)`
      *— assert `EFFECTIVE=0` on a probe-free launch of the build you ship, and grep the delivered
      session's log copy (if any) for `PROBE ARM`: zero hits. `run_summary.json` → `mask_probe_arms`
      must read `0` on every delivered session — that field exists precisely so a probe cannot fire
      without leaving an artifact trace. Same class as the token check: fine on the bench, wrong in a
      delivery.*
- [ ] **`Config/DefaultEngine.ini` → `GameDefaultMap` points at the CLIENT's map, not a bench or test map.**
      *A packaged build's default map can only be changed by editing the PROJECT config and re-cooking —
      a loose `Config/DefaultEngine.ini` beside the package is silently ignored (G88). So anyone who needs
      a package to boot somewhere else must edit the same host-project file the client build is cooked
      from, and **nothing in git will catch it** — the host project config is outside the plugin repo.
      Left un-reverted, the next client package silently boots into whatever bench map was last used.
      Read the value; do not assume it. (Added 2026-08-06 after the CB_GateLevel work made this hazard
      real — see G87/G88/G89.)*

- [ ] 🚨 **`[AnomalyInjector] ExcludedTargetNamePatterns` carries `Decal` and `_CR_`** (added
      2026-08-24, lands at the NEXT cook — the already-shipped bundle is NOT re-cooked for it).
      ```ini
      +ExcludedTargetNamePatterns=Decal
      +ExcludedTargetNamePatterns=_CR_
      ```
      *First FIELD receipt of route (e), and it is the case the `H6` boxes above say is HIGH HARM
      and self-sufficient. A tester aimed at a wall; the dashboard's click-to-select was
      depth-blind and picked the small decal mesh lying on that wall's face
      (`Wall_Grime_F_CR_Decal_INST776`, `StaticMeshComponent`, **TRANSLUCENT**). The opaque
      material swap then rendered **visibly, on the wall, exactly where the tester was looking**,
      while the mask measured the decal at zero and the veto removed all 10 events — shipping an
      `annotation.json` with **no anomalies at all** for a session whose frames plainly contain
      one. `vetoed_events=10`, `translucent_vetoes=10`.*
      ⚠ **THIS INI EXCLUSION IS NOW THE ONLY DEFENCE — the dashboard-side half was REVERTED.**
      AnomDash `5d35cbe` made click-to-select rank nearest-first; it was reverted the same day at
      `be5b151` **on owner report, because it made objects behind a large bounding box
      unreachable** — the picker is depth-blind again by deliberate decision, and a click can once
      more land on a small overlay mesh in front of what the user aimed at. It also does not make a
      translucent target measurable: route (e) stays **documented, not fixed**.
      📌 **The "translucent population is UNMEASURED on any delivered title" note above is no
      longer true — this is the first measurement, and it is 10 of 10.**
      → `docs/sessions/2026-08-24-060-field-bugs-overlay-labels-and-depth-blind-picker.md`.

- [ ] **The client note for the NEXT delivery says decals / overlay meshes are excluded from
      targeting.** *Otherwise the client sees the anomaly-type mix shift between deliveries with
      no stated reason, and a silent change to what can be targeted is exactly the kind of thing
      that gets read as a regression.*

### 1.1 🆕 `m41` — THE CENSUS SHIPS ON. THREE BOXES, AND TWO OF THEM CAN ONLY BE TICKED AT THE COOK.

- [ ] 🚨 **`G-R7(ii)` IS A HARD DELIVERY PRECONDITION FOR THIS COOK — it is no longer a merge gate and
      it is not optional.** Physical-only (RDP invalidates both halves): the eye/OBS hitch read and the
      throughput read, run on **master's own build**. *`m41` turns the census on for every delivered
      capture. The census's measured cost is **+1.5352 ms per engine frame** (1080p, unpaced, dev box)
      **above an already-ON mask** — the dev box absorbed it entirely at the shipped paced 30 fps, but
      that is **headroom, not free**, and the client's box is not this box. `speed_ratio` is the
      instrument.* ⛔ **Nothing ships off master until this passes on master's own cook.**

- [ ] 🚨 **`C-G1b` — THE HOST-PP PREFLIGHT'S POSITIVE DIRECTION, BOTH WAYS.** Author (or point at) a
      host-project post-process material that **samples `SceneTexture: CustomDepth`**, apply it as a
      blendable, and read the `Capture(census): HOST-PP CUSTOM-DEPTH READERS =` line:
      **present ⇒ `= 1`, named, at Warning; removed ⇒ `= 0` at Log with NON-ZERO `scanned` counts.**
      *Why it is owed: **no material in the bench container lights any `UsesSceneTexture` bit**, so on
      the bench the detector's `0` has never been shown to be a reading rather than blindness. The
      enumeration, the resolution, the entries-vs-materials discriminator and the negative control are
      all proven (journal 069 §2.2); only the lit bit is not.* ⚠ **A `= 0` with `scanned 0/0/0` is
      BLINDNESS, not a clean read — read the scanned counts, never just the zero.**

- [ ] 🚨 **`B-G1` — THE TRANSLUCENT RULE, BOTH DIRECTIONS.** Needs one actor whose material is
      **translucent-blend AND opts into custom-depth writes**. With
      `IAI.Capture.CensusTranslucentWriters` **OFF** (the shipped default) it must read
      **`EXCLUDED(translucent)`** in the cycle's `NOT-MEASURED` listing and increment
      `census_excluded_translucent`; with it **ON** the same actor must read **`MEASURED_NONZERO`**.
      *Why it is owed: `IAI.Bench.SpawnTranslucentProbe` **REFUSED** on the bench — the three materials
      in that container are all opaque — so the rule that motivated the whole item has never been
      exercised on a real instance. **Recorded as UNRUNNABLE, never as passed.*** ⚠ That flag is a
      **compile-time `UMaterial` property**; a `UMaterialInstanceDynamic` inherits it and cannot change
      it, so this fixture cannot be improvised at runtime.

- [ ] **Read back the census keys from the delivered `run_summary.json`: there must be exactly
      16 `census_*` keys** (`census_frames`, `_cycles`, `_candidates`, `_zero`, `_below_floor`,
      `_above_ceiling`, `_excluded_translucent`, `_fires_fallback_all`, `_fires_partial_fallback`,
      `_fires_unseen_candidates`, `_host_pp_customdepth_readers`,
      **`_host_pp_customdepth_reader_names`**, `_unmeasurable_nanite`,
      `_unmeasurable_tag_failed`, `_unmeasurable_hidden`, `_unmeasurable_not_yet_measured`) **and
      🔻 **CORRECTED 2026-09-06 (session 077): this box said FIFTEEN and listed fifteen, and it has
      been WRONG SINCE `m49` A1**, which added `census_host_pp_customdepth_reader_names` (the `LG-3`
      reader names — journal 073 records `run_summary` 59 → 63 adding it). **Measured on both sides
      of the phase B cook: 16 and 16, added 0 removed 0.** *A literal count here would have FAILED a
      correct build — the third time this file has carried a stale literal count, after the
      `annotation.json` 48-key box below. **The lesson is the one already written there: phrase the
      check against a single source, not against a number copied at the time of writing.*** **and
      `annotation.json`'s key set matches the field table in `client-readme.md` §8.2/§8.3 exactly.**
      *A 16th census key, or any `annotation.json` key not in that table, is a contract change the
      client was not told about.*
      🔻 **CORRECTED 2026-09-04: this box used to read "`annotation.json` must still be 48 keys" and
      that is now WRONG — schema v2 deliberately adds keys** (`label_schema`, `injected_frames` and
      its five sub-keys, `affected_frames.span_frame_count`, `bbox_source`, `observable_frame_count`,
      `unmeasured_frame_count`, `observability_measured`). *A literal count here would now FAIL on a
      correct build, which is the same staleness failure this file already calls out for the anomaly
      count — so it is phrased against a single source, the client's own field table, for the same
      reason.*

- [ ] ⛔ **No bench lever is on in anything that ships.** Grep the delivered log for
      `IAI.Bench.` — `ProbeSceneTextureUsage`, `CensusFixedExpiry`, `CensusBatchCap`,
      `CensusDropEntry`, `SpawnTranslucentProbe`, `SynthTickOrder`, `MaskPairingProbe`, `HideMode`, `HideOmitShadowSilencing`, `HideOmitDepthPassSilencing`. *All are console-only with no ini
      key, so they cannot be on by accident — but a capture taken with one on is a GATE LEG, not a
      dataset, and `CensusDropEntry` in particular deliberately hides candidates from the census.*


### 🆕 `m43` — THE TARGET ID MASK: three boxes for this cook

- [ ] **The mask is PRESENT in the smoke run.** `target_mask/` exists and the run's
      `Capture(m43): TARGET MASK` echo names `ON` with a source.
      ⚠ **m44 CHANGED WHAT "COMPLETE" MEANS: there is one PNG per frame that HAS CONTENT, not one per
      captured frame.** A file exists iff it has content. Reconcile with `run_summary` instead:
      `target_mask_frames_measured` == the PNG count, and measured + `_hidden_blank` + `_unavailable`
      == the captured frame count. *Hidden-object anomalies contribute `unmeasured` frames by design
      until the follow-up build lands, so `_unavailable` is NOT expected to be 0.*
- [ ] 🆕 **ONSET, BOTH TICK ORDERS.** For every non-hidden event the first mask frame == the first
      labelled frame == the first frame whose picture differs. Instrument
      `CaptureBench/tools/m44_gates.py`; run native and with `IAI.Bench.SynthTickOrder 1`.
      *m43 shipped a systematic one-frame lag behind a green gate set because nothing compared the
      first labelled frame to the first mask frame.*
- [ ] 🆕 **ZERO all-zero PNGs**, and **no `mask_file` on any frame not labelled for its event**
      (`m44_gates.py` reports both as G2 and G7).
- [ ] 🆕 **MASK-PICTURE-PAIRING (100 % leg).** `IAI.Bench.MaskPairingProbe 1` + the analyser
      `CaptureBench/tools/m44_pairing_probe.py`: `NEITHER == 0` and `PREVIOUS == 0` over decidable
      frames, both orders. ⚠ **The 50 % screen-percentage leg joins this box when F1 lands** — until
      then the mask is correct only at 100 % screen percentage.
- [ ] 🆕 **`m47` BLACK-FRAME PIXEL GATE — RUN IT ON THE COOK'S OWN SMOKE SESSION.**
      `python tools/verify_capture.py --dir <session> --black-frame-gate` must exit **0** with
      **BLACK FRAMES 0** and **DARK FIRST FRAMES 0**.
      *This is the box that actually tests a packaged cook for the shader-readiness class. Do NOT tick
      it from `frames_shaders_pending == 0`: that counter is STRUCTURALLY zero in a packaged build,
      because the engine body the prewarm calls is `WITH_EDITOR` only, so a zero there is a READING and
      not a test that passed (`G232`, `G146`). Gate on the pixels (m19).*
      ⚠ **The threshold (6.0 on the 0..255 whole-frame mean) is derived from THIS bench's darkest
      legitimate frame (59.992). On a darker title re-derive it on that host's own frames** — the rule
      is "an order of magnitude below the darkest legitimate frame", and `--black-threshold` takes the
      new number. **Prove the gate can still fail first: `python tools/verify_capture.py --selftest`.**
- [ ] 🆕🚨 **`m49` LABEL-vs-PIXEL GATE — RUN IT ON THE HOST'S OWN SMOKE SESSION, ON THE HOST'S OWN
      MACHINE, AND TRANSCRIBE THE READINGS TO THE CARD.**
      `python tools/verify_capture.py --label-pixel-gate --dir <session>` must exit **0** with
      **SHIFT 0** and **NOT-VISIBLE 0**.
      *This is the box that answers the question the client actually asks: does the first labelled
      frame match the first frame whose pixels change, and is the frame after `end_frame` clean. It
      is a HOST-side box on purpose — every label/pixel desync found so far (P9 on Bates, the onset
      readings on Concorde) was host-specific and invisible on the bench.*
      🚨 **IT RUNS ON THE HOST BOX, NOT HERE, AND ONLY THE NUMBERS COME BACK.** Host session data
      never leaves the host machine, so the gate is executed there and its **per-event lines and
      summary are transcribed by hand** into `_bates_reads\<date>-<host>-verifier-read.md`. The
      runnable step is **`office-rdp-card.md` SECTION G** — do not invent a second route.
      ⚠ **This box has been UNRUNNABLE here for three sessions in a row** because it was waiting on a
      session folder to be copied across, which never happened (journals 072-02, 073, 074 all record
      it as UNRUNNABLE, never as passed). **Section G exists to remove that dependency.** Reported as
      unrunnable is a valid outcome; reported as passed without a reading is not.
      **Prove the gate can still fail first: `python tools/verify_capture.py --label-pixel-gate
      --selftest`** — seven cases, both edges, both directions (`G96`).
      ⚠ **`NOT-MEASURABLE` is neither a pass nor a failure** — it is an unread surface and the reason
      is printed on the event's own line. A session whose events are ALL `NOT-MEASURABLE` has not been
      tested; read the reasons before ticking this box.
      ⚠ **It needs `measure_label_offset.py` beside it** (it imports the region/baseline/threshold
      code); both ship via `bundle_manifest.txt`. Without masks it runs in bbox-only mode and says so.

### 🆕 `m49` — SCHEMA v2: two boxes, and both are documentation

- [ ] 🚨 **`client-readme.md` §8 IS IN THE BUNDLE AND CARRIES THE v2 FIELD TABLES AND THE v1→v2
      CHANGELOG.** *`annotation.json`'s `affected_frames` changed MEANING at v2 — same key, narrower
      set — and a client parser that was reading it now silently reads a different quantity. The
      README's §8.5 changelog and its one-line migration (`affected_frames` → `injected_frames`) are
      the only thing standing between that and a silent data change. **The delivered README ships as
      a bundle file; confirm the copy in the bundle is the current one, not a stale duplicate.***
- [ ] 🚨 **THE DELIVERY NOTE CARRIES THE CHANGELOG PARAGRAPH** from `client-delivery.md`
      → *"THE CHANGELOG PARAGRAPH FOR THE NEXT DROP"*, and **the `m50` paragraph in that file is
      REMOVED if `m50` is not in the build being shipped.** *A changelog describing a fix the binary
      does not carry is worse than no changelog.*
- [ ] **`run_summary.json` carries `observable_frames`, `frames_condition_lost` and
      `observable_min_pixels`, and `observable_min_pixels` reads the value you intended** (compiled
      default **1** = *"any drawn pixel counts as observable"*).
      *Read it from the run's own echo line, not from the ini — the echo prints the value, its source
      and its size as a per-mille of the picture, and says that the stored field is absolute pixels.*
- [ ] **`translucent_only_excluded_targets` is present in `run_summary.json`, and the selection echo
      names the rule and its source.** *`0` is a valid reading — it means no candidate on that map was
      translucent-only — but the echo must be there whether or not the rule ever fired, or a lost ini
      key is indistinguishable from a rule that never bit (`G139`).*

### ✅ `m50` — SHIPPED 2026-09-04 (session 076, `7ecdf5f`). THESE ARE NOW TICKABLE.

🔻 *These two were placeholders written in session 075 against a plan. `m50` shipped, so they are
live boxes now — and **both of their figures were corrected by the measurement that fixed them**,
which is recorded in place rather than silently overwritten.*
🚨 **Chat's ruling that `m50` lands BEFORE any client cook is DISCHARGED: it has landed.**

- [ ] 🚨 **`m50` STENCIL SINGLE-OWNER GATE.** On every captured frame, each stencil value must be
      carried by **exactly one actor**. The build asserts this itself and logs
      `Capture(mask): TAG-OWNERS si=… shared=N` per captured frame plus a run-end
      `Capture(m50): TAG-OWNER VIOLATIONS = N`. **Read that run-end line: it must be 0.**
      *Offline: `python Plugins/CaptureBench/tools/m50_tag_owner.py --gate <session>` (exit 1 on any
      violation, and it REFUSES rather than passing on a log with no `TAG-OWNERS` line).*
      🔻 **CORRECTED FIGURE: the pre-fix bench incidence is 12 of 448 tag-instances (2.7 %), all of
      them on one binary — NOT the 22 / 4.9 % this box previously carried from journal 074.** And the
      connected-component count that produced that figure **under-reads the real incidence by about
      7×**, so it is a proxy and gates nothing.
      ⚠ **`MASK-TIE` STRUCTURALLY CANNOT SEE THIS** — the intruder is in the reduce table *and* in
      the PNG, so the tie reads MATCH. **A green MASK-TIE is not evidence for this box.**
- [ ] 🚨 **`m50` ALLOCATOR HEADROOM — `TAG-POOL EXHAUSTED` must not appear in the run log.** It is an
      `Error` line and it is the tripwire for the defect above. *Its presence does not corrupt a
      label any more — the event is admitted unmeasured instead — but it means the pool ran out, and
      on a long capture that is the structural ceiling described in the `m50` journal §5.*
- [ ] 🚨 **`m50` UNMEASURABLE-TARGET ADMISSION.** On a Nanite-heavy host, a capture that fires N
      events must deliver N events with `observability_measured: false` — **not `anomalies: []`**.
      *Measured on a real Nanite-heavy game BEFORE the fix: 6 of 6 events vetoed, 90 frames and 43
      positive frames delivering an empty `anomalies` array. AFTER: the same leg delivers 6 events,
      all `observability_measured: false`, `vetoed_events 0`.* **Read `vetoed_events` and the event
      count TOGETHER; an empty array with a non-zero fire count is the failure this box exists for.**
- [ ] **`run_summary.json` carries `unmeasurable_targets_admitted`** (the `m50` key, `64 → 65`).
      *A non-zero value is NOT a defect — it is the count of events that shipped honestly labelled
      instead of being deleted. **A zero is a READING, not a pass**: on a host with Nanite disabled
      it is the correct answer, and `census_unmeasurable_nanite` is the field that says whether such
      a target existed at all.*
- [ ] 🆕 **`shader_prewarm_ms` is present in `run_summary.json` and `frames_shaders_pending` reads 0.**
      *Reported, not gated — see the box above for why. A non-zero `frames_shaders_pending` on a
      PACKAGED cook would be genuinely surprising and is worth stopping for.*
- [ ] 🚨 **`MASK-TIE` shows ZERO mismatches** in the smoke run's log
      (`Select-String -Pattern 'MASK-TIE' | Where-Object { \ -match 'MISMATCH' }` must be empty).
      *This is the check that the delivered mask is the same silhouette the labels were judged on. It is
      the load-bearing one; a mismatch means the artifact and the label disagree about the same pixels.*
- [ ] ⚠ **Read `speed_ratio` on the smoke run and compare it to a target-mask-OFF run.**
      *The mask adds a GPU→CPU readback per fire-active frame (921,600 B at 720p, scaling with capture
      resolution). The dev box absorbed it at paced 30 fps — that is HEADROOM, NOT FREE. If the client
      box hitches, `IAI.Capture.TargetMask 0` is the FIRST knob to turn off, and `G-R7(ii)` is the
      gate that catches it here.*

### 🆕 `m49` PHASE B — THE GPU DRAWN-COUNT: three boxes, and one of them needs the cook

⚠ **Phase B changes a GLOBAL SHADER and its parameter struct, so it CANNOT ride a code-only hot-swap
(`G129`).** A build carrying phase B's source against a stale container does not merely mis-measure —
it **cannot start** (`Missing global shader … permutation 0`). The shader-presence gate in
`setup-runbook` §8.6 step 3.7 is therefore mandatory for this cook, and it is the boot itself.

- [ ] 🚨 **`target_drawn_pixels` IS PRESENT ON EVERY ANOMALY ENTRY, and on a hide-class event it
      reads `0` while `target_pixels` reads `> 0`.** *That pairing is the renderer's own statement
      that the object is absent from the picture.* ⛔ **It is a READING, not a veto — `observable` does
      NOT depend on it** (session 077 measured a labelled hide frame reading `drawn == count` whose
      pixels were nonetheless the HIDDEN value, so `drawn > 0` does not establish presence; see
      journal 077 §2 and `architecture.md`). ⛔ **A `0` where the target is NOT measurable at all is
      WRONG — it must be `-1`.** On a Nanite target there is no custom-depth silhouette (`G134`), so
      there is no drawn count either, and `0` would assert *"measured, and absent"*, which is the
      `MEASURED_ZERO` vs `NOT_MEASURED` confusion `m26` exists to prevent.
- [ ] 🚨 **`run_summary.json` → `frames_drawn_unexpected` reads `0`.** *It counts labelled hide-class
      frames where the GPU still found the target's own depth front-most.* ⚠ **That is a DEPTH
      statement and it does NOT by itself mean the hide failed** — `m45` silences the main pass and
      the depth pass through separate flags, so the depth can linger for a frame while the picture is
      correct (measured, session 077). **A non-zero value is a FINDING TO EXPLAIN — check the pixels
      before concluding anything — not an automatic STOP, and never a label change.**
      **Its can-fail proof is the bench lever
      `IAI.Bench.HideOmitDepthPassSilencing 1`, which must drive it non-zero; without that leg having
      been run at least once on this binary, a zero here is blindness rather than a reading (`G96`).*
- [ ] 🚨 **`frames_exposure_dip` and `frames_exposure_dip_suppressed` ARE READ TOGETHER.**
      *`exposure_dip` now requires the drop to survive with every live target's silhouette excluded,
      so a disappearing anomaly no longer marks its own frames. **Zero-and-zero is a session with no
      exposure movement; zero-and-non-zero is the rule doing its job.*** ⛔ **A zero dip count on a
      leg with the game's auto-exposure LIVE is a failure of the detector, not a quiet success** —
      that is `m48`'s own prove-it-can-fire gate and phase B does not retire it.
- [ ] **`run_summary` key count is 68** (`65` at `m50`, plus `target_drawn_pixels_measured`,
      `frames_drawn_unexpected`, `frames_exposure_dip_suppressed`) **and `annotation.json`'s key set
      matches the README's own field table.** *Phase B adds NOTHING to `annotation.json`.*

- [ ] 🚨 **THE EDITOR TARGET BUILDS, EXIT 0 — RUN IT BEFORE THE COOK, NOT AFTER.**
      `Build.bat StackOBotEditor Win64 Development -Project=<uproject> -WaitMutex`
      *The cook runs on EDITOR binaries (`G47`, runbook §8.6 step 3.5), and the editor target is
      MODULAR while every bench gate runs the MONOLITHIC packaged build. A missing `MODULE_API`
      export is therefore **invisible to every test this project runs** and surfaces for the first time
      as a link failure inside the cook window.* **Measured instance: `LogAnomaly` was unexported from
      `m38` through `m43` — five milestones, every bench gate green, and the next cook would have
      failed at link.** → `G221`, journal 069 §5.
## 2. Desktop app + config

- [ ] **Built with the current source**: `npm run build:tauri && npm run tauri build` on a machine with
      **Rust ≥ 1.77.2**; copy `src-tauri/target/release/Dashboard.exe` to the delivery root.
- [ ] **`config.json` sits at the delivery root (next to `Dashboard.exe`) and its `controlToken` is the
      CLIENT's token** — byte-identical to the ini value in §1.
      *Mismatch → the app reaches the server and is rejected; since M1 that shows a clear "token rejected"
      screen naming `config.json`, but it still means a client who cannot capture.*
- [ ] **`config.json` is NOT embedded in the exe.** `build:tauri` deletes `dist/config.json` before Tauri
      compiles the frontend in, and the app reads the loose file at runtime — the token is editable with no
      rebuild. Confirm once by deleting the loose `config.json`: the app should open its manual connect
      screen (nothing baked). *This is the M2 footgun-fix; a regression here silently re-bakes the token.*
- [ ] Author `config.json` by hand (or from `config.example.json`) — do **not** ship the dev
      `public/config.json`. `capturesRoot` may be left `""`; `Setup.bat` fills it in.
      ⚠ This line used to name the dev token literally. **It no longer does, deliberately** — a
      placeholder written into a doc is another untracked leg that outlives every rotation (**G112**).
      The dev token is whatever `StackOBot\Config\DefaultGame.ini` currently holds; read it there, and
      run §1's check against the value you actually ship.

## 3. Bundle assembly

```
<delivery root>/  Setup.bat  Run.bat  Dashboard.exe  config.json  host-tools/  (game build)
```

- [ ] `Setup.bat` + `Run.bat` + `Dashboard.exe` + `config.json` sit at the **delivery root** (the `.bat`s
      resolve paths via `%~dp0`; the app reads `config.json` from its own folder).
- [ ] `host-tools/` contains `encode_watcher.py`, `selfcheck.py`, `write_config.py`, and `serve_dashboard.py`
      (the last is the fallback route only).
- [ ] **The client-readme's LAUNCH section is filled in** with this build's actual game-launch steps.
      *It ships as an empty stub; the client cannot start the game without it.*
- [ ] No `node_modules/`, no `package.json` in the bundle — the client needs **Python only** (for the encoder).

## 4. Dry run on a clean-ish machine

- [ ] **`Setup.bat`**: ffmpeg fetch exercised (including the corporate-network retry path, or the manual
      fallback documented in the client-readme), Python found, captures folder created.
- [ ] **`Setup.bat` did not clobber the token** — re-open `config.json` (at the delivery root) and confirm
      `controlToken` is still the client's value and `capturesRoot` is now filled in.
      *A config saved with a UTF-8 BOM used to make this file unparseable and cost the token silently.*
- [ ] **WebView2**: present by default on Win10 21H2+/Win11. `Setup.bat` checks the registry and
      silent-installs the Evergreen bootstrapper if absent. ⚠ **The silent-install path has NOT been tested
      against a machine actually lacking WebView2** (D-M3-4) — watch it on the first such client box; if it
      fails, install WebView2 by hand from the Microsoft Evergreen page and re-run `Setup.bat`.
- [ ] **SmartScreen**: `Dashboard.exe` is unsigned, so the first launch shows "Windows protected your PC" —
      the client clicks **More info → Run anyway** once. Tell them to expect it (it is not a virus warning).
- [ ] **`Run.bat`**: launches `Dashboard.exe` + the watcher; its self-check should print `dashboard`
      (Dashboard.exe running) OK, `watcher` OK, and `game server` OK once the game is running.
- [ ] **A real capture end-to-end**: start a capture *from the app*, frames land in the chosen captures
      folder, `annotation.json` + `run_summary.json` are written, and the mp4 appears a few seconds later.
- [ ] **This is the standing packaged-build gate (G76)** — validate against a packaged build, never PIE
      alone. Both first-smoke-test bugs (black preview; stuck `missing_texture` revert) were invisible in
      the editor.

## 5. The session you deliver

- [ ] **`run_summary.json` → `speed_ratio` ≤ ~1.05 with `paced: true`.** This is the **hard gate** (m21).
      *Above that, the run is rate-starved: the presented frames lag their own index and the labels are
      silently wrong. Lower `IAI.Capture.Fps` and re-capture — do not ship it.*
- [ ] Session contains `Actual_Frames/`, `Video_Clip/<session>.mp4`, `run_summary.json`, `annotation.json` —
      and, in delivery mode, **no** `labels.jsonl` and **no** `run.json`.
- [ ] Frame indices are **0-based** and match `annotation.json` (`frame_00000.png` is index 0). If the
      client's tooling is 1-based, that is a spec conversation — never a quiet ±1 shift.
- [ ] 🆕 **`m26`: read `run_summary.json` → `vetoed_events` BEFORE quoting an event count.**
      *Since `m26` an event whose target was MEASURED to draw ZERO pixels is REMOVED from
      `annotation.json`. `vetoed_events` counts them.* 🚨 **A post-`m26` event count is NOT
      comparable with a pre-`m26` one (`L2`) — quote the two numbers together or the comparison is
      wrong.* ⚠ **The captured FRAMES are not removed** (`L1`): `video.total_frames` and the PNG
      count are unaffected, so frames-without-events is EXPECTED, not a defect.
      ⚠ **Delivery OFF and ON disagree on event content** (`L3`): `labels.jsonl` is prebuilt and
      cannot be corrected by the veto. **Do not diff them and report a bug.**
- [ ] 🚨 **`m26`, OWNER-SIDE ONLY: if you OVERLAY a delivery-OFF session, expect boxes on VETOED
      events.** *`annotation.json` and `labels.jsonl` disagree inside one session folder — a
      fully-vetoed run ships an empty `anomalies` array beside 59 label rows still asserting
      `anomaly_present`. The overlay chain (`overlay_watcher.py` → `tools/verify_capture.py`) reads
      `labels.jsonl`, so it draws them.* **NO CLIENT IMPACT — delivery mode does not write
      `labels.jsonl` at all** — but do not read those boxes as a labelling regression, and do not
      ship overlay output made from a vetoed session as if it matched `annotation.json`.

## 6. Security note (owner-accepted, restate when handing over)

The token is a **static shared secret** in two plaintext files (the cooked ini and `config.json`). It is
not per-session-random. It exists because browser `ws://` connections ignore CORS: without it, any website
the client visits while the game runs could drive the control server and pull viewport JPEGs. Localhost
only, one client, private artifacts — accepted. **Do not disable auth to make auto-connect simpler**; that
removes the only defence against arbitrary local web origins.

### 🆕 `m45` — hidden-class masks: three boxes

- [ ] 🚨 **IDENTITY ARBITER + ITS CAN-FAIL LEG.** At the AA-off configuration
      (`r.AntiAliasingMethod 0, r.Lumen.DiffuseIndirect.Allow 0, r.DynamicGlobalIlluminationMethod 0,
      r.ReflectionMethod 0`), **native tick order**: control (`IAI.Bench.HideMode 0` twice) **0**
      frames differing, test (`HideMode 1`) **0**, and `IAI.Bench.HideOmitDepthPassSilencing 1`
      **> 0** (bench: 20 of 60, worst 4.69 %). *All three, or the gate proves nothing.*
- [ ] **G7 EQUALITY:** mask files == labelled frames exactly for hidden-class events; a blink's visible
      in-between frames carry **no** mask.
- [ ] ⚠ **Do not attempt pixel identity at the delivered configuration** — the cross-run floor is ~9 %
      of pixels between two runs of the SAME build (`G228`). No claim is made there and none should be
      recorded.

### 🆕 `m48` — exposure-dip marking: two boxes, and one of them can only be ticked at the cook

- [ ] **`run_summary.json` carries `frames_exposure_dip`** and the client README's `exposure_dip`
      paragraph is in the bundle. (`m48` adds **one** `run_summary` key and **one** conditional frame
      key; `annotation.json` does **not** move.)
- [ ] 🚨 **THE AUTO-EXPOSURE SMOKE LEG RAN, AND ITS DIP COUNT IS NOT ZERO.** One leg of the
      cook-time smoke run executes at **the game's own exposure defaults** (every other gate leg stays
      exposure-pinned). On that leg read **`frames_exposure_dip` > 0**, **BLACK FRAMES 0** and
      **DARK FIRST FRAMES 0**. ⛔ **`frames_exposure_dip` = 0 on a leg where auto-exposure is proven
      live is a FAILURE OF THE DETECTOR and blocks delivery** — prove exposure is live first, from the
      black-frame gate's own luminance spread (~32 units ON vs ~7 pinned), or the zero means nothing.
- [ ] ⛔ **The build does NOT force exposure.** The plugin never writes an exposure cvar and the
      shipped ini sets no exposure key. If a delivered build pins exposure, the dataset stops looking
      like the game — that is a regression, not a stabilisation.
