# REPORT for 088-01-delivery-rehearsal.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 088-01-delivery-rehearsal.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-29 17:39:37 | finished: 2026-09-29 18:14:14 | duration: 35 min | exit code: 0
context at last turn: 641388 tokens (64% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\088-01-delivery-rehearsal.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

**Plain summary for the owner:** Yes, you could install either package on Thursday. Both built cleanly as a client would receive them, and both pass every packaging check that can run without starting the game. Before either goes out, two things have to be written in:
- **Both packages:** the game-launch steps in Step 1 of the README (it still says "fill this in"). The bundle checker now refuses to pass until this is done.
- **The full m52 + m53 package only:** the per-mode rate in §8.8 of the README (it still says "to be measured").

One short launch of the office build then confirms the dashboard token and that no test switches are exposed.

**What ran:** step 0 merges done (kit → fix `df63a8a`, fix → m53 `9bd7316`). Every suite is green on both merged trees, including all 23 m53 mutants failing as they must and the kit selftest 32/32. Both paths were staged with `stage_plugin_delivery.py`, then built and packaged as Development builds into `E:\IA_BuildCache\_rehearsal\<A|B>\game`:

| | exe | packages cooked | cook result |
|---|---|---|---|
| A (m52 only) | `6D1B659F` | 792 | 0 errors, 0 warnings |
| B (m52 + m53) | `C18D0F41` | 795 | 0 errors, 0 warnings |

AnomDash bundles were made after a fresh `npm run build`. The warm hosts were used for the builds and are back exactly as found. The D: Content manifest is unchanged. Nothing was launched and nothing was staged into BenchGate.

## 1. Checklist boxes

**PASS on both paths:**
- **G354 descriptor check without `--bench-cook`:** 4 modules identical, FIXTURE `hits=none`, descriptor SHA `62E6B9AD…`.
- **AnomalyBench excluded:** staged folder and package both; `IAI-BENCH READY` is absent from the exe.
- **Lever audit:** selftest passes; the binary audit passes when run against the staged folder (fault 2 below).
- **Cooked ini, read out of each package's pak:** 64-character token, `bDeliveryModeDefault=True`, no content clock, the Decal / `_CR_` exclusions.
- **Build shape:** Development, not Shipping; the editor target builds with exit 0 before the cook; no exposure keys.
- **Bundle token:** PASS against the build's cooked ini. It STOPs against the dev ini and against the other path's build, so the G376 fix is now shown on real builds.
- **Bundle README:** it is the current `client-readme.md` with the v2 tables and the §8.5 changelog.
- **`Setup.bat` walked on a copy:** ffmpeg found, Python found, captures folder set, token kept, `config.json` fetchable.
- **README Steps 5, 6 and 7 run from the bundle:** all selftests pass (kit 32/32).

**PASS, B only:** no m53 bench fixture ships; the comment strip found 0 changes; the lever audit passes on this delivery ref.

**FAIL (fill-ins, not builds):**
- The README Step 1 LAUNCH stub (both paths).
- The §8.8 `(rate: to be measured)` value (B).

**MANUAL:**
- **Needs a running build:** the token read back from the build's log; `IAI bench levers: DISABLED … preset=0`; no test anomalies in `IAI.ListAnomalies`; the default pool line; the grab point; `MaskProbe EFFECTIVE=0`; B's `IAI.TexCorrupt.Census` run.
- **Needs a captured session:** all the m41 to m50 and phase B session boxes; `Run.bat` (it opens windows and a browser) and a real capture end to end.
- **Office-host settings:** Nanite, host custom depth, G-R7(ii).
- **Delivery notes and the security note at handover.**

The Tauri-era boxes (desktop app, WebView2, SmartScreen) don't apply.

## 2. A against B
Nothing unexpected:
- **Plugin:** 29 files differ, and git and a folder hash agree. All 19 commits behind them are m53-round work. The shared-code changes are m53 by design: the m52 ⟂ m53 exclusion, `RenderCore`/`RHI`, and a subtype fix gated to m53 ids. The descriptor is identical.
- **Bundle:** 28 of 30 files identical. The README differs in 7 hunks, all m53 sections; `config.json` differs by each build's own token.
- **Package:** B is A plus the 3 m53 assets. The only other changed chunks are the ones that vary between any two cooks (two shader archives, MainWorld, the container header).

## 3. Packaging faults fixed
All three are in tools and docs on the fix branch; m53 got them by merging the fix branch again.
1. **`048d6c8`:** the never-cook list and G354's fixture scan missed m53's fixture folder `/Game/CaptureBenchTexCorrupt`. It was a gap, not a leak: no shipping content references it. Proven both ways (G400).
2. **`acfc46d`:** the checklist's binary lever audit **failed a correct delivery exe** on both paths. It ran against the branch, which still holds AnomalyBench. It now runs against the staged folder, and the staging script prints the exact command (G398).
3. **`acfc46d`:** nothing stopped an unfilled README Step 1 (the only box asking for it was in a superseded section). `check_delivery_bundle.py` now STOPs on it and on "to be measured" values: selftest 9 cases, both real bundles STOP, and a copy with the stub filled PASSes. A new checklist box covers it (G399).

**Product faults: none found.**

## 4. Commits pushed
- **`fix/m52-label-timing`:** `df63a8a` (kit merge) → `048d6c8` → `acfc46d` → `5c2bc83` (journal `docs/sessions/2026-09-29-088-01-delivery-rehearsal.md`, status block, G398–G400).
- **`feat/m53-uv-normal-corruption`:** `9bd7316` (fix merge) → `c2d11f5` (fix merged again).
- `master`, `m51` and the tags are untouched.

## 5. Decisions needed
1. **Where the one launch per package happens.** The brief asked for the token check on a real build's log, but that needs a launch, which this brief forbade. I used the cooked ini from each pak instead (what the build actually enforces). Two options for the launch:
   - Tonight, about 2 minutes per package, run straight from `_rehearsal\<A|B>\game` without staging into BenchGate. It would read the token line, the DISABLED echo, the anomaly list and the pool line, and for B the census.
   - Thursday, on the office build.
2. **Which run produces B's §8.8 per-mode rate,** so the bundle checker can pass B.
3. **Who writes README Step 1** (the office build's launch steps), before the bundle check on Thursday.
api-equivalent cost (not billed on Max): $22.34 | turns: 168 | running total across all briefs: $3,696.00
