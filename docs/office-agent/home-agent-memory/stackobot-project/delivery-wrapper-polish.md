---
name: delivery-wrapper-polish
description: Client delivery-wrapper polish round (host-tools .bat scaffolding + client README); NOT engine/plugin
metadata: 
  node_type: memory
  type: project
  originSessionId: <uuid>
---

2026-07-16 delivery-polish round from external-tester feedback. **Zero engine/plugin C++** — wrapper only.
Implemented Workstreams A + C. **B1 (dashboard frame-count CSS overflow) DONE + PUSHED** (owner-eyeballed):
AnomDash `3413a15` `fix(dashboard): stop frame-count input overflowing the capture panel` — CSS-only in
`src/styles.css`: `.cap-row { flex-wrap: wrap }` + `.capture .inline input, .capture .inline select
{ box-sizing: border-box; min-width: 0 }` (no `.tsx`, no `.panel` change). See [[anomaly-injector-state]].

**Delivery source of truth = the git-TRACKED `anomaly-dashboard/host-tools/` copy** (comment-stripped), NOT the
unversioned top-level `D:\IntrusiveAnomalies\host-tools\` (that top-level copy is divergent/legacy — left alone).

**FIELD-CONFIRMED (2026-07-17, owner's real box):** clean-tree smoke test of the re-assembled
`E:\DashboardSmokeTest` bundle — first curl failed with `CRYPT_E_NO_REVOCATION_CHECK`, the `--ssl-no-revoke`
retry succeeded (160.8M pulled, nested-bin resolve worked), and `Run.bat` auto-opened the browser. Both
round-2 fixes verified on the actual corporate network.

**COMMITTED + PUSHED (2026-07-17, no m-tag — delivery-polish, not a milestone):**
- AnomDash `18b6395` (`f978f1b..18b6395`): `feat(host-tools): interactive Setup + unified Run launcher,
  corporate-network ffmpeg fetch` — Setup.bat, Run.bat, encode_watcher.py, host-tools/README.md, .gitignore,
  del start_encode_watcher.bat (src/styles.css NOT included — B1 still deferred).
- Plugin `3e5e455` (`a2c3127..3e5e455`, on top of m21): `docs(client-readme): track + rewrite for Setup/Run
  flow, Seed, ffmpeg troubleshooting` — client-readme.md now committed/tracked. Added a **"ffmpeg didn't
  download"** §7 subsection: manual install = drop the unzipped build anywhere under `host-tools\ffmpeg\`
  (recursive `dir /s /b` match) then re-run Setup.bat, OR put its bin on PATH; BtbN + gyan zip links.

**LATER COMMITS (2026-07-17, no m-tag):**
- AnomDash `40b411b` `chore(repo): .gitattributes` (`*.bat`/`*.cmd text eol=crlf`) — remote checkouts always CRLF.
- AnomDash `3413a15` `fix(dashboard)` = B1 CSS (see index).
- **CAPTURE-PATH UNIFICATION (owner-tested): AnomDash `e93b43b` `feat(capture-path): dashboard writes to the
  same captures root the watcher watches` + plugin `a4a8862` docs.** Was a real footgun: dashboard "output
  folder" defaulted BLANK → game wrote to its own `<ProjectSaved>/AnomalyCaptures`, watcher watched whatever
  `CAPTURES_ROOT` the user typed at Setup — only aligned by the README convention, nothing enforced.
  **FIX (zero engine C++ — plugin ALREADY honors an absolute `dir`: `AnomalyCaptureSubsystem.cpp:496`
  `Base = BaseDir.IsEmpty()? default : BaseDir`):** Setup.bat now also writes `dashboard/.env.local`
  `VITE_CAPTURES_ROOT=<root with / >` (gitignored via `*.local`; forward slashes to dodge dotenv `\n`/`\t`
  escapes); dashboard `config.ts` `BAKED_CAPTURES_ROOT` + `CapturePanel` pre-fills the `dir` field from it →
  game writes exactly where the watcher watches. `CAPTURES_ROOT` is now the SINGLE source of truth for both;
  Setup prompt reworded ("any folder", no longer "find the game's Saved folder"). Owner-dev (no `.env.local`)
  = field blank = unchanged. Chain: dashboard `dir`→ WS `capture_start {dir}` (ControlServer :603 `TryGetStringField("dir")`)
  → `StartRun(Dir,...)` → absolute base honored. `tsc --noEmit` clean.

**⚠ CRLF IS MANDATORY for `.bat`.** The Write tool emits **bare LF**, and `cmd.exe` **cannot resolve `goto`/`call :label`
in an LF-only `.bat`** ("cannot find batch label"). Any `.bat` with labels MUST be converted to CRLF after Write
(`ReadAllText → replace \n with \r\n → WriteAllText ASCII`). Verified: LF Setup.bat looped/failed; CRLF Setup.bat runs
clean end-to-end. Also matches the repo's CRLF convention. (`.py` stays LF — its original style; only labelled `.bat`s care.)
**PERMANENT FIX (commit `40b411b`, pushed): `anomaly-dashboard/.gitattributes` = `*.bat text eol=crlf` +
`*.cmd text eol=crlf`** → every checkout (any `core.autocrlf`) gets CRLF `.bat`, so remote pulls / re-bundles
are safe. (Index stays LF; adding the attr caused NO renormalization diff.) Still: when the Write tool CREATES
a new `.bat`, convert LF→CRLF before running/committing it — the attribute governs checkout, not the Write.

**As-built — round 1 (all in `anomaly-dashboard` repo unless noted; uncommitted — owner commits per his conventions):**
- **TWO PascalCase client entrypoints** (comment-free per repo invariant; **CRLF**), **repo source in `host-tools/` but
  authored to run from the DELIVERY-BUNDLE ROOT** (they use `%~dp0dashboard`, `%~dp0host-tools`, `%~dp0config.bat`, so
  non-functional from the repo location by design; assembly note in `host-tools/README.md`):
  `Setup.bat` (one-time: ffmpeg `where`→local `host-tools/ffmpeg/`→`curl.exe`+`tar` from `FFMPEG_URL` at top =
  BtbN `latest` win64-gpl zip, no version pinned, **normal download then `--ssl-no-revoke` retry** for corporate
  revocation-check blocks + zero-byte/rc guard before tar; `where py`/`where python`; `set /p` captures dir w/ validation
  loop; `npm.cmd install`; writes `config.bat`) and **`Run.bat`** (merged round 2 — sources config.bat, spawns TWO titled
  windows: **Anomaly Watcher** `start "…" cmd /k ""%PY%" …enc.py --ffmpeg "%FFMPEG%" --root "%CAPTURES_ROOT%""` (extra
  outer quote-pair so spaced paths survive cmd quote-stripping — verified) + **Anomaly Dashboard**
  `start "…" /D "%~dp0dashboard" cmd /k "npm.cmd run dev -- --open"` (browser auto-opens; bare `vite` forwards
  `--open`, so **`vite.config.ts` untouched**); guards config.bat + node_modules). **Round 2 DELETED the round-1
  `RunDashboard.bat` + `StartWatcher.bat`** (never committed).
- Bundle layout now: `{ Setup.bat, Run.bat, config.bat (runtime), /dashboard, /host-tools }`.
- **⚠ STALE:** the `E:\DashboardSmokeTest` bundle assembled in an earlier turn holds the OLD LF Setup.bat + the removed
  RunDashboard/StartWatcher and has no Run.bat → must be re-assembled before another smoke test.
- `config.bat` = hand-off (FFMPEG, CAPTURES_ROOT, PY); machine-specific → **gitignored** (added `config.bat` +
  `host-tools/ffmpeg/` to `.gitignore`).
- `encode_watcher.py`: `--root` now **hard-required** (removed `E:\AnomalyCaptures` default; errors+exits code 2 if
  missing — verified). `--ffmpeg` kept **fail-soft** (flag & skip, deliberate). Was ALREADY argparse'd — the brief's
  "add root as an arg" was half-wrong; only the default+required changed.
- **DELETED** legacy `start_encode_watcher.bat` (superseded + broken by required --root). `overlay_watcher.py`/
  `start_overlay_watcher.bat` untouched (dev/QA only).
- `host-tools/README.md` rewritten (new flow + assembly note).

**Client README (Workstream C):** canonical source = `StackOBot/Plugins/AnomalyInjector/docs/client-readme.md`
(plugin repo) — was UNTRACKED, now **`git add`ed (staged, tracked)**, NOT committed. Rewritten for the Setup.bat flow;
manual ffmpeg/path-edit/raw-npm steps collapsed; **Seed explainer added** (deterministic RNG seed → object/anomaly
selection; same seed = same run). **LAUNCH section = a clearly-marked EMPTY STUB** (Step 2, HTML-comment marker
`<!-- LAUNCH: ... -->` + blockquote) — owner maintains RunServer.bat→RunClient.bat (Bates) separately; do NOT write
launch steps, do NOT reference the old single-exe "run exe then IAI.Server.Start" flow. **The tester-followed delivered
README (with RunServer/RunClient) is NOT in the workspace** (full D:/E: search empty) — it lives on the owner's work box.
