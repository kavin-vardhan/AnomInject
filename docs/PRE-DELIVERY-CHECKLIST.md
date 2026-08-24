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
