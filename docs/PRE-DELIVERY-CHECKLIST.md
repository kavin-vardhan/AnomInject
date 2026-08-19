# Pre-delivery checklist

Tick this before handing a bundle to a client. Every line here exists because it went wrong, or could
have gone wrong silently — the failure mode of each is written next to it, because the whole point is
that none of these announce themselves.

Companion docs: `client-delivery.md` (owner-facing: what delivery mode does and why), `client-readme.md`
(the client's own guide, shipped in the bundle), `capture-fps.md` (choosing a sustainable rate).

---

## 1. Game build — `Config/DefaultGame.ini`

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
- [ ] **`Config/DefaultEngine.ini` → `GameDefaultMap` points at the CLIENT's map, not a bench or test map.**
      *A packaged build's default map can only be changed by editing the PROJECT config and re-cooking —
      a loose `Config/DefaultEngine.ini` beside the package is silently ignored (G88). So anyone who needs
      a package to boot somewhere else must edit the same host-project file the client build is cooked
      from, and **nothing in git will catch it** — the host project config is outside the plugin repo.
      Left un-reverted, the next client package silently boots into whatever bench map was last used.
      Read the value; do not assume it. (Added 2026-08-06 after the CB_GateLevel work made this hazard
      real — see G87/G88/G89.)*

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

## 6. Security note (owner-accepted, restate when handing over)

The token is a **static shared secret** in two plaintext files (the cooked ini and `config.json`). It is
not per-session-random. It exists because browser `ws://` connections ignore CORS: without it, any website
the client visits while the game runs could drive the control server and pull viewport JPEGs. Localhost
only, one client, private artifacts — accepted. **Do not disable auth to make auto-connect simpler**; that
removes the only defence against arbitrary local web origins.
