# 2026-08-23 — session 055 — camera_clipping pool default, sparse overlay output, client docs

## §1 Goal and shape

The final pre-delivery change set, executed the weekend of the client build. Three plugin commits
(`aeb1930..4bc9739`) plus one AnomDash commit (`2b4264c..dcf2004`), all pushed. **NO TAG — `m31`
remains the open milestone and still awaits Concorde V-3/V-4. Highest tag is still `m30`.**

| commit | repo | what |
|---|---|---|
| `a75d601` | plugin | `camera_clipping` off by default in the auto pool (one-line deletion) |
| `6e143f2` | plugin | overlay tool writes only frames that carry boxes (+56/−10, `tools/verify_capture.py`) |
| `4bc9739` | plugin | `client-readme.md`: pool defaults + sparse overlay output documented |
| `dcf2004` | AnomDash | `overlay_watcher` relays the overlay tool's new summary |

Staged exe **`DCF9C192`**; predecessor `060F7B07` archived at
`_binary_baselines\StackOBot.exe.session054-060F7B07`. Code-only hot-swap; the container is
unchanged (no cook this session).

## §2 `camera_clipping` IS UNTICKED BY DEFAULT IN THE AUTO POOL — owner product decision

It is a WHOLE-SESSION GLOBAL (m30): once drawn it lays one continuous condition under every other
anomaly in the session. The owner ruled it out of the default draw. **It STAYS in `GAutoPool` and
stays selectable** — only the default checkbox state moved.

**Where that default lives, recorded so nobody re-derives it: `GAutoPoolDefaultEnabled[]` in
`AnomalyAutoInjectorSubsystem.cpp`, consumed ONCE in `Initialize()`. ENGINE-SIDE ONLY.** The
dashboard holds no default of its own (`AutoPanel` renders `Object.keys(auto.pool)`; ticked state
comes from `ControlSnapshot`; `client.autoConfig` is sent only on a user click). ⚠ **Compiled
state, NO ini route ⇒ reaching a client build requires a REBUILD AND A COOK.**

✅ **Read back from the running build BOTH ways (G96/G119), not assumed:** the default echo reads
`Default pool: blinking, missing_texture, corrupted_texture, lod_popping`, and
`IAI.Auto.Pool camera_clipping 1` still takes it to `enabled pool (5)`.

## §3 THE CAPTURE-POOL PANEL SHOWS SIX BOXES, NOT FIVE — reconciled, documented

The snapshot emits every Object- or Global-scoped catalog entry; AnomDash hides
`{lod_corruption, time_dilation, lighting_mismatch}` (and `lighting_mismatch` is Component-scoped
anyway). What remains on screen: blinking, missing_texture, corrupted_texture, lod_popping,
**missing_object**, camera_clipping. `missing_object` has been available-but-unticked since m19
and was never written down. **The m30 status line "the delivered pool is FIVE" means the five
DEFAULT-ON at m30 — it was never the number of boxes on screen.** `client-readme.md` now documents
six (with the two default-off ones marked).

## §4 THE OVERLAY TOOL WRITES ONLY FRAMES CARRYING BOXES

`tools/verify_capture.py` output is now SPARSE and NON-CONTIGUOUS by design; the AnomDash watcher
relays its summary. Rules that must survive any future edit:

- **Filenames keep the original 0-based frame index and are NEVER renumbered.**
- AMBER-only frames are still written; `--red-only` (default OFF) narrows to shipped labels.
- Nothing enforces 1:1 with `Actual_Frames`, and `encode_watcher.py` builds video from
  `Actual_Frames` ONLY (`FRAMES_SUBDIR`) — a sparse `annotated/` cannot break any video.

📊 **Measured on a real 300-frame session: 300 files / 315.0 MB → 96 files / 100.4 MB (68.0 %
saved), identical box and category counts, all 96 retained images byte-identical to the
pre-change output.**

## §5 🚨 G164 — A KILLED BUILD LEFT A TRUNCATED EXE AND THE NEXT BUILD SAID "UP TO DATE" AT EXIT 0

A killed build task left `StackOBot.exe` TRUNCATED at ~2 MiB against the real ~240 MB — and the
next `Build.bat` reported "up to date" at EXIT 0 in 2.8 s. **UBT trusts the output's timestamp; it
never checks that the artifact is whole.** G119's shape applied to the build system itself: exit 0
plus "up to date" is not evidence the artifact exists in full — **verify SIZE (or hash) against a
known-good build before trusting a binary that a killed task may have touched.** Fix: delete
exe+pdb and rebuild (objects survive; ~90 s relink). Filed as **G164** in gotchas.md.

## §6 NOT DONE, NAMED

- ⛔ **NO TAG** — m31 still open, still awaiting Concorde V-3/V-4.
- This journal and the CLAUDE.md status refresh were OWED from the session itself and are
  discharged by the docs commit that carries this file (authorized by chat 2026-08-23).
- `P6` did not move · `feature/stencil-capture` untouched at `76cac74` · no force-push · no ratio,
  no threshold anywhere.
