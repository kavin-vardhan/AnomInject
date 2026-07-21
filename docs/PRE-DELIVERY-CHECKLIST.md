# Pre-delivery checklist

Tick this before handing a bundle to a client. Every line here exists because it went wrong, or could
have gone wrong silently — the failure mode of each is written next to it, because the whole point is
that none of these announce themselves.

Companion docs: `client-delivery.md` (owner-facing: what delivery mode does and why), `client-readme.md`
(the client's own guide, shipped in the bundle), `capture-fps.md` (choosing a sustainable rate).

---

## 1. Game build — `Config/DefaultGame.ini`

- [ ] **`[AnomalyControlServer] Token` is set to a long random value.**
      *Absent → the server falls back to a random per-session token that a client with no console can
      never read; the dashboard cannot connect at all.*
- [ ] **`[AnomalyCapture] bDeliveryModeDefault=True`.**
      *Absent → the client's sessions ship `labels.jsonl` + `run.json`, which includes the seed. Delivery
      mode is what limits output to the client-facing artifacts.*
- [ ] **`[AnomalyCapture] ContentClockDefault` is NOT set to `game`** (leave it absent → `wall`).
      *`game` on a wall-clock title stamps a slow run at target and plays the client's videos ~2× fast —
      the Issue-2 regression. `game` is only correct for game-clock content such as StackOBot itself.*
- [ ] Build is **Development or Test**, not Shipping.
      *Capture and the control server are compiled out of Shipping entirely.*

## 2. Dashboard bundle

- [ ] **Built with the current source**: `npm run build` on the build machine (not a stale `dist/`).
- [ ] **`dashboard/config.json` exists and its `controlToken` is the CLIENT's token** — byte-identical to
      the ini value in §1.
      *Mismatch → the dashboard reaches the server and is rejected; since M1 that shows a clear
      "token rejected" screen naming this file, but it still means a client who cannot capture.*
- [ ] **The dev token is not shipped.** `dist/config.json` is copied from `public/config.json` by Vite, so
      a fresh build carries the **dev** token (`TESTVALUE123`) unless you overwrite it.
      *Easy to miss precisely because the file is present and looks right.*
- [ ] `capturesRoot` may be left `""` — `Setup.bat` fills it in on the client's machine.
- [ ] **Assets load from a plain file path.** The build must use `base: './'` (relative `./assets/...` in
      `index.html`). Serve the bundle locally once and confirm it renders.
      *Absolute `/assets/...` breaks the moment the app is served from anywhere but a server root.*

## 3. Bundle assembly

```
<delivery root>/  Setup.bat  Run.bat  dashboard/{index.html,assets/,config.json}  host-tools/  (game build)
```

- [ ] `Setup.bat` + `Run.bat` sit at the **delivery root** (they resolve paths via `%~dp0`; they do not
      work from inside `host-tools/`).
- [ ] `host-tools/` contains `serve_dashboard.py`, `write_config.py`, `selfcheck.py`, `encode_watcher.py`.
- [ ] **The client-readme's LAUNCH section is filled in** with this build's actual game-launch steps.
      *It ships as an empty stub; the client cannot start the game without it.*
- [ ] No `node_modules/`, no `package.json` in the bundle — the client needs **Python only**.

## 4. Dry run on a clean-ish machine

- [ ] **`Setup.bat`**: ffmpeg fetch exercised (including the corporate-network retry path, or the manual
      fallback documented in the client-readme), Python found, captures folder created.
- [ ] **`Setup.bat` did not clobber the token** — re-open `dashboard/config.json` and confirm
      `controlToken` is still the client's value and `capturesRoot` is now filled in.
      *A config saved with a UTF-8 BOM used to make this file unparseable and cost the token silently.*
- [ ] **`Run.bat`**: three windows/statuses — its self-check should print `dashboard` OK, `watcher` OK, and
      `game server` OK once the game is running.
- [ ] **A real capture end-to-end**: frames land in the chosen captures folder, `annotation.json` +
      `run_summary.json` are written, and the mp4 appears a few seconds later.
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
