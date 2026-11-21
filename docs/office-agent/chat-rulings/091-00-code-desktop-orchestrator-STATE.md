# Code Desktop orchestrator — live state (cold-reader handoff; keep current)

Last updated: 2026-10-03 00:25 IST by the Claude Code Desktop orchestrator session (kick-start `_reviews\090-HANDOVER-code-desktop-orchestrator.md`).

## Done
- **Job A:** 090-10f2 ruled **ACCEPTED** (`_reviews\090-10f2-ruling.md`). m53 `b8cd3cd` (code `2dbf6f2`, exe `36C8E30A`), fix `12a6db8`, both pushed. The **mailbox watcher is STOPPED** (`_mailbox\STOP` exists; status `stopped` 16:27:30). Do not remove STOP: one bench, one implementer.
- **Job B:** the implementer Relay exists: `_relay\relay_run_codex_gdp_impl_v1.ps1` (Astra only; effort high/xhigh/max; refuses while the mailbox runs; one implementer at a time; full access only with `-OwnerApprovedFullAccess`) and `_relay\gdp-impl-block-v1.txt` (rules appended to every implementer prompt).
  - The smoke `_relay\runs\2026-10-02-090-12s-impl-relay-smoke\` was RED: the Windows elevated sandbox runs as `CodexSandboxOnline`, so git reports dubious ownership and UBT is denied engine-plugin UHT `Timestamp` writes.
  - **Owner decision:** implementer jobs run with **full access**; the sandbox's `CodexSandboxUsers` ACL entries were removed from the 11 affected folders (receipt `acl-receipt-before-removal.txt` in the smoke folder). Recorded in `_reviews\090-12s-owner-decision-codex-full-access.md`.
  - Not removed: unresolvable per-workspace capability-SID entries; they can't be attributed to this run versus the owner's own Codex sessions, and no account holds them.

- **Job C:** proxy blur **ACCEPTED** for the office check (`_reviews\090-12-ruling.md`). `feat/m52-proxy-blur` `51e2fdb` (code `663a99b`, exe `CFFD276F`, archive `D:\IA_BankOverflow\_binary_baselines\m52-proxy-0912-CFFD276F`). Codex was interrupted once ("model at capacity") and resumed with `_relay\relay_resume_codex_impl_v1.ps1`; that runner's capacity detection was fixed after it misfired. Native-MainWorld hold auto-pool visibility is unproven (motion); accepted as a documented limitation.
- **Job D:** the Saturday pull and checklist were given to the owner (00:25 IST 10-03):
  1. Pull `feat/m52-proxy-blur` at `51e2fdb` and compile, on both office hosts.
  2. In PIE after Play: `IAI.TexCorrupt.Census allreasons`; read back the uv and normal eligible counts and the `routes id=stuck_low_mip` line.
  3. UV and normal on both hosts: do fired objects visibly corrupt?
  4. Blurry-only auto capture in PIE: does it fire, and does only the target blur? Then `python tools\anomaly_refusal_counts.py "<log>"` plus `--route-check`, and read back the blurry and route lines.
  5. The office check kit (`tools\OFFICE-CHECK.md`) READ BACK lines on a short capture.

- **091-02 (10-03):** the first office host failed to compile `51e2fdb` (8 missing-include errors). The include-only fix plus a whole-plugin audit were pushed on the owner's "push now, check later": the **office pull is now `feat/m52-proxy-blur` `e09fa9c`** (m53 `cf16766`, fix `8b86f43`). Ruling `_reviews\091-02-ruling.md`. The deferred §0 home game check waits in a detached idle waiter (run folder `_relay\runs\2026-10-03-091-02-include-compat\`). **Create `CANCEL-WAITER` there before launching any other implementer job.**
- **Cleanup (10-03):** the owner deleted the non-essential parts of `E:\IA_BuildCache` (E: 18 → 363 GB free). Real-size lesson: folder sizes double-count hard links. D: post-Monday cleanup list is in chat (the legs and `_reviews\089-03/089-04/090-06-evidence` go together; `_r912` / `_r910f2` after delivery).

- **First office host (10-03):** the census reads eligible uv 59 / normal 59 / blur 48 of 2,110 objects (virtual textures dominate: uv refusals 1,385 `virtual_texture`). 5 auto runs, 0 fires. Readings are in `_bates_reads\2026-10-03-first-office-host-proxy-census-owner-reads.md`. **Owner: deprioritised; "Bates can be tackled later"** (virtual-texture support = follow-up).
- **091-03 RUNNING (started 14:00 IST, Codex Astra/max, full access):** the owner saw on the second office host a ~2-frame blur tail after the labels end, with nothing in annotation.json. annotation.json gets per-event `transition_frames` / `transition_reasons`; temporal detection made fail-safe; tails measured vs flagged; plus the deferred 091-02 §0 legs. Run folder `_relay\runs\2026-10-03-091-03-annotation-transitions\`. The 091-02 idle waiter was CANCELLED (13:59) because 091-03's proof covers it. If the owner sends run_summary transition numbers, append them to that run folder as `office-reads.txt`.

- **091-03 first session ended 20:45 IST, NEEDS-DECISION:** the code is done locally and NOT pushed (fix `9c2c770`, m53 `2b68d18`, proxy `6a2dd9a`; built source proxy `58386be` / exe `C7FB5212`). Schema 2.1 `transition_frames`; fail-safe temporal detection; window 3/40/1 (measured t10 36 + 4); the long tail is temporal history, not re-streaming (residency observer). Final-source pixel proof, §0 and static hold are NOT RUN: the gate closed (D: 38.9 GB from page-file growth, headroom 0.9 GB while the owner used UE 5.7). **A resume with priorities waits in an idle waiter** (`idle-waiter-pid.txt` in the run folder; it now also requires D:/E: ≥ 50 GB). Create `CANCEL-WAITER` before any other implementer job.

- **091-03 resume 1 (20:49–21:22) was RED for an environmental reason:** DXGI device removed at the first Present, before any plugin work. Cause: the owner's UE 5.7 holding VRAM on the 8 GB laptop GPU. Ruling: no source change; this job's legs need NO foreign UnrealEditor at all. Resume 2's prompt is `resume-prompt-2.txt`. **The owner restarted the PC at ~21:40 IST:** the waiter died with it. Restart it with `D:\IntrusiveAnomalies\_relay\START-CODEX-WAITER.bat` (it already passes `-RequireNoEditor`). Its finish shows as `exit-code-attempt2.txt` plus a new `exit-code.txt` in the run folder.

- **091-03 ACCEPTED (02:45 IST 10-04):** `_reviews\091-03-ruling.md`. **The office pull is `feat/m52-proxy-blur` `d660b5d`** (m53 `783a768`, fix `8badc23`; exe `C7FB5212`). Independent recount: 80 events, 0 mismatches between annotation.json and labels.jsonl. Window 3/40/1. **Standing bench rule on this PC: no foreign UnrealEditor open during legs** (8 GB GPU).
- **Job E RUNNING (since 02:27 IST):** read-only Codex review `_relay\runs\2026-10-04-091-04-independent-review\` (reviewer relay, sandboxed). Rule on its findings in `_reviews\091-04-ruling.md`; fixes go to a later implementer job.
- **Owner's office checklist (second office host, `d660b5d`), numbers only:**
  1. compile;
  2. blurry-only PIE auto capture: only the target blurs; each annotation.json event has `transition_frames` (3 onset + 40 after, under temporal AA), covering the visible return frames;
  3. uv/normal visibly corrupt;
  4. the office kit READ BACK lines;
  5. `anomaly_refusal_counts.py` blurry and route lines.

- **Job E done (03:20 IST 10-04): HOLD MERGE.** F1 HIGH (a non-owner view family can take capture/mask arms), F2 HIGH (UV/normal scope falls back to the whole actor), F3 HIGH (texture_uniform passes unused-channel variation), F4/F5 MEDIUM, F6 LOW. Ruling `_reviews\091-04-ruling.md`: F1/F2/F6 fixed now; F3 part-fixed (alpha only counts if the material uses it); residual F3 to the owner as a documented limitation; F4/F5 follow-up.
- **091-05 RUNNING (since 03:23 IST, Codex Astra/max, full access):** review fixes plus a short regression. Run folder `_relay\runs\2026-10-04-091-05-review-fixes\`. Push only after proof. **The final delivery build = the 091-05 proxy head** (then Job F: extended smoke, merges to master, package, rehearsal).
- **Owner decisions to put to him:** (a) accept residual F3 as a documented limitation for Monday; (b) blurry default-on/off after his office check (Sunday noon cut-off).

- **091-05 ACCEPTED (08:45 IST 10-04):** `_reviews\091-05-ruling.md`. **The delivery candidate and office pull is now `feat/m52-proxy-blur` `5d6352b`** (m53 `4c93e5f`, fix `1eaf205`; exe `3ADC2EA4`). F1 was real (the old build labelled 144 editor-view frames in multi-viewport PIE); fixed. My recount: 125 events, 0 mismatches.
- **Job F next (091-06):** waits on the owner's blurry ship decision (by Sunday noon). Then merge the chosen head to master, run the all-anomaly regression on the master build (PIE and staged), the Development package and the rehearsal. The bench needs the PC idle with no foreign editor.

- **⛔ RELEASE FREEZE (owner, ~10:45 IST 10-04): "keep d660b5d as final and stop every other thing".** At the office, 5d6352b captured NO frames (the F1 ownership guard failed closed on the office's PIE setup). 091-06 was STOPPED by PID at 10:50 (nothing pushed; staging original; worktree clean). The owner also rejected the 40-frame temporal window: delivery = `d660b5d` + temporal transition defaults 0/0/0 (CVars still allow opting in).
- **091-07 RUNNING (since 10:54):** branch `release/2026-10-05` from d660b5d with only that change; two-leg check; push of that branch only. Run folder `_relay\runs\2026-10-04-091-07-release-transitions-off\`.
- **Not in Monday's build (follow-up drop):** the 091-05 fixes (F1 guard, F2 scope, F3 alpha, F6 checker) plus the office-robust F1 work, F3 visible-change, F4, F5. Readme limitations must cover F1 (capture with one live game view), F2, F3.
- **Owner's at-office workaround now:** `IAI.Label.TransitionOnFrames 0` / `OffFrames 0` / `HideFrames 0` before capture.
- **091-07 ACCEPTED (11:45 IST 10-04):** `_reviews\091-07-ruling.md`. **Monday's office pull = `release/2026-10-05` `23098fa`** (exe `218BA4C7`, archive `_binary_baselines\release-2026-10-05-23098fa-218BA4C7`). My recount of both legs: 0 mismatches, 0 temporal reasons, on/off/hide 0/0/0, frame sets identical to the C7FB5212 baselines.
- **091-08 ACCEPTED (11:50 IST):** docs-only readme limitations. **Final office pull = `release/2026-10-05` @ `d7eee97`**; its code is identical to `23098fa` (exe `218BA4C7`). Addendum in `_reviews\091-07-ruling.md`.
- **Parallel owner workstream (10-04): Dashboard v2** in `D:\IntrusiveAnomalies\anomaly-dashboard`, branch `feat/desktop-v2`. Goals: a client-friendly redesign; a single `Dashboard.exe` with no Setup, Python or ffmpeg; auto-connect by reading the token from the game's log; Generate-on-request for Video, Target masks and Labelled previews (masks are always recorded and kept in a hidden folder until released; annotation.json stays automatic). Claude Code implements this directly; it does not touch the plugin or the release. The old dashboard is the owner's fallback.
  - **12:35 IST: built and tested end to end on the packaged game.** It auto-connected, captured 300 frames, and Generate produced video (300 frames, 30 fps, 10.0 s by ffprobe), masks (28 = run_summary 28) and previews (144). It also reconnected by itself after a game restart. AnomDash `feat/desktop-v2` `09f080e`, pushed. The exe (`420C73D0`), QUICKSTART and screenshots are in `D:\IntrusiveAnomalies\Claude outputs\AnomalyDashboard-v2\`.
  - **Owner: "Can this dashboard be pushed, love this."** AnomDash `master` was fast-forwarded to `c888713` (v2 plus a README note) and pushed. The old browser dashboard is tagged `dashboard-v1-browser` at `ab6e63f`, also pushed.
  - **Office (owner):** v2 built at the office. A Visual Studio-launched editor gave `no_token`.
    - Fix `4f44164`: discovery reads the log the process has open (handle enumeration) and searches native projects next to the engine. Tested on the live game; 127/127 lab checks pass.
    - The owner asked for mask-outline previews. Done in `8409a61`: an exact red mask outline with the anomaly's name, and a dashed box where no mask exists. The old red/amber previews are replaced.
    - Exe v2.2 is in `Claude outputs\AnomalyDashboard-v2.2\`. The owner must click Settings → "Back to automatic" to drop the manual key.
  - **Not yet tested:** the editor/PIE connection against a real UnrealEditor (fakes plus real editor-log parsing only), any office PC, and Windows N / no-WebView2 machines.

- **10-05: explainer videos delivered** (`Claude outputs\Explainer-Videos\`): dashboard 64 s plus blurry, UV and normal at 60 s each. Motion graphics only, with no game footage (owner's instruction), captions only.
- **092-01 RUNNING (10-05, Codex Astra/max, read-only reviewer relay):** research on Bates (first office host) virtual-texture support for the three new anomalies. Run folder `_relay\runs\<run 2026-10-05-092-01>\`; collect `review.md` to `_reviews\092-01-codex-bates-vt-research.md`.
  - Orchestrator's own source checks:
    - A VT sampler slot must be given a virtual texture (`MaterialInstance.cpp:1280`), and the renderer casts the bound texture's resource to `FVirtualTexture2DResource` (`MaterialUniformExpressions.cpp:1263`). The texture-side copy route is therefore invalid on VT slots.
    - `VirtualTextureDataBuilder` is `WITH_EDITOR` only, so a packaged game cannot build a new VT.
  - Candidate routes:
    - mesh-side UV×N and tangent-sign flips for UV and normal (independent of texture storage and of `texture_not_parameter`);
    - a custom VT producer;
    - blur on VT is the hardest.
  - The office's 5 zero-fire PIE auto runs are unexplained (capture-log refusal counts never read).
- **092-01 ACCEPTED as research (07:52 IST 10-05):** ruling `_reviews\092-01-ruling.md`; report `_reviews\092-01-codex-bates-vt-research.md`.
  - Blurry on VT: a private coarse VT via the exported `FVirtualTexture2DResource(owner, VTData, FirstMipToUse)` is the strongest proof candidate.
  - Mesh routes are restricted and need new subtypes.
  - My brief's tangent recipe was wrong (Codex corrected it).
  - Next: the owner's go-ahead. The order is office readings, then Proof A (blurry VT), then design round B (page-remap/block-transform producer), then Proof C (constants).
- **Europa pivot question (owner, 10-05):** port the plugin to Europa (a 4.25 office title) instead of Bates?
  - Owner reads: Europa's VT is OFF, and it runs a MODIFIED 4.25 fork. Refer to it only as "Europa's fork"; never write the studio or title.
  - Stock UE 4.25.4 (CL 14469661) was re-downloaded to `E:\EpicGames\UE_4.25`.
  - The 4.25 bench project was re-added by the owner (12:25 IST 10-05): `E:\Unreal Projects\StylizedParisStreet`. It is blueprint-only, 2 maps, about 880 assets, and has `r.VirtualTextures=False` (matches Europa). It is untouched: no code module, no `Plugins\` junction yet.
  - Toolchain: VS2017 MSVC 14.16.27023 and SDK 10.0.17763 are present. The per-user UBT pin (14.38) is untouchable, so 4.25 builds need a per-process mechanism.
  - Plugin growth since the September study (`489c29d`): 19k → 47k lines.
- **Bench converted to a C++ project (owner's go, 13:56 IST 10-05):**
  - Backup `StylizedParisStreet.uproject.bak`. Added `Source\` (Game and Editor Target.cs, module Build.cs, `.h`/`.cpp`) and the `.uproject` `Modules` entry.
  - The editor target built in 56 s with VS2017 14.16.27051 and SDK 10.0.17763.0, exit 0.
  - Recipe (in memory `ue425-build-on-this-pc`): pin the cl.exe product version 14.16.27051 and set `bOverrideBuildEnvironment = true` in both targets. The global UBT xml is untouched (dated 26 Aug).
  - No plugin junction yet.
- **092-02 ACCEPTED as research (14:20 IST 10-05):** ruling `_reviews\092-02-ruling.md`; report `_reviews\092-02-codex-europa-port-study.md`.
  - Verdict: feasible in stages, not easy. Estimates: about 3–5 engineer-weeks for a six-anomaly subset with capture, 6–10+ for parity.
  - Confirmed: no post-process SVE hook in 4.25 (new capture backend); no public WebSocket API (control server/dashboard needs a new transport); 4.25 reads legacy descriptor keys, so our Shipping exclusion would be ignored.
  - Assessment given to the owner: the Europa pivot is NOT a shortcut for the three texture anomalies (they need phases A–E there). The Bates VT routes are smaller for those.
  - Owner's decision (10-05 ~14:40): **fix the first office host's VT problem** (no Europa port now).
- **092-03 RUNNING (since 14:56 IST 10-05; Codex Astra/max, full access, detached runner PID 37200):** Proof A, blurry on VT through a private coarse VT (route `proxy_vt`).
  - Branch `feat/vt-proxy-blur` from `release/2026-10-05` `d7eee97`; worktree `E:\IA_BuildCache\_r921\AnomalyInjector`; host `_r53_host` (its plugin worktree is to be restored to `23098fa`).
  - Run folder `_relay\runs\2026-10-05-092-03-vt-proxy-blur-proof\`.
  - Legs need the owner's UE 5.7 closed. The job polls up to 4 h, then reports NEEDS-DECISION.
  - **The first attempt stopped at preflight (15:05):** D: read 48.7 GiB, a transient dip from system-managed `D:\pagefile.sys` growth during the orchestrator's three parallel video renders. D: recovered to 65 GiB.
  - **Relaunched as 092-03b (15:08 IST, runner PID 32440),** with a resume note: if D: is below the floor, wait up to 60 min before stopping. Run folder `_relay\runs\2026-10-05-092-03b-vt-proxy-blur-proof\`.
  - Lesson: no heavy parallel renders or builds while an implementer job runs.
- **092-03b NEEDS-DECISION (16:33):** the private-VT constructor route works in steady state (identity within null, clean restore, co-users unchanged). It has a 4-frame onset transient (frames 124–127, MAD 11.68 against a limit of 6.63), consistent with fresh producer pages not being resident.
  - Local branch `feat/vt-proxy-blur` @ `465f2e7`, unpushed.
  - Ruling `_reviews\092-03b-ruling.md`:
    - prefetch-before-bind with the identity re-run under the unchanged criterion;
    - the pre-authorized fallback only if prefetch fails;
    - fixture fixes (contrast, real 1920×1080, geometry meeting the existing rules);
    - owner item flagged: the 128 px tile floor plus the 8× rule means Auto targets must be about 1,024 px wide or more.
- **092-03c RUNNING (since 16:36 IST, runner PID 29472):** resume from `465f2e7`. Run folder `_relay\runs\2026-10-05-092-03c-vt-proxy-blur-resume\`.
- **10-05 owner requests done:**
  - Explainer videos without the "Before you capture" section (52 s each) were re-sent.
  - **Dashboard v2.3** (AnomDash `master` `f111cdf`, pushed): Custom length is now an explicit form state. It used to be inferred and snapped back to presets. An empty custom count blocks Start. Exe `750B3CBA` is in `Claude outputs\AnomalyDashboard-v2.3\`.

## Next
- **Waiting on:** the owner's office numbers; then decide whether blurry joins Monday (cut-off Sunday noon IST).
- **Owner decision pending:** disk. D: ≈ 54 GB free (floor 50; `_r912` evidence is 43 GB), E: ≈ 20 GB. Ask before the extended smoke and packaging.
- **Job E:** one read-only Codex review (existing Relay `relay_run_codex_gdp_v1.ps1`) of 090-10b2 → 090-10f2 (m53 `0a69ea3` → `b8cd3cd`), with the 090-08 questions plus the two notes R-1 and R-2 from `_reviews\090-10f2-ruling.md`. Run it while the owner tests on Saturday, when no implementer job is running.
- **Job F:** the weekend delivery path; Sunday noon is the proxy-blur cut-off; Sunday, the ship/limit/slip call.

## Facts to keep
- **Disk (00:20 IST 10-03):** E: ≈ 20 GB free (below the 50 GB floor), D: ≈ 54 GB free (just above the floor). Delete nothing without the owner.
- **dbghelp** no longer blocked the 090-10f2 cooks.
- **The idle gate** with no foreign editor needs 60 s of no input (three reads); the owner's typing resets it.
- Every Codex job runs on `gpt-6-astra` only, at effort up to `max` (owner, 2026-10-02).

## 2026-10-05 18:35 IST: 092-03c progress check (not a ruling)
- Prefetch-before-bind WORKS: `09203c\runs\identity-prefetch-01\identity-result.json` PASS on all 480 frames (target peak 6.593 <= limit 6.629; co-user and ordinary pass). The frame size is still 942x869 for this leg; the 1920x1080 fixture legs (fixture-native-full-01..04) followed.
- Branch now `8829491` (9 commits past `465f2e7`), including `9cb82cc` integrate prefetched private VT proxy route.
- Since 18:13 Codex has been in its engine wait: the owner's UE 5.7 editor PID 36260 and Blender PID 43756 are open, with the GPU at 95% and 1.6 GB VRAM free. Its deadline is about 21:32. I did not touch the processes (standing rule); I asked the owner to close them.
- Waiter re-armed: task b1zs6tayb, 115 min.

## 2026-10-06 01:20 IST: 092-03c RULED GREEN; 092-04 launched (owner asleep, said "Continue overnight")
- 092-03c ACCEPTED as a bounded proof: `_reviews\092-03c-ruling.md`; report copied to `_reviews\092-03c-codex-vt-proxy-blur-report.md`. Branch `feat/vt-proxy-blur` pushed at `af79520` (runtime `c80ef9b`). Nothing merged; master `b5f15a3`, release `d7eee97` and tags are unchanged; the host is at `23098fa`, clean.
- Independent checks:
  - native identity recomputed from PNGs (bound span = null);
  - positive edges 120..167 exact in staged + PIE auto;
  - visual frames 100/140/170;
  - code read of `AnomalyPrivateVT.cpp`.
- Scope limits carried forward: single-layer VT stacks only (multi-layer refuses `stack_layout`, likely the main Bates yield limit); unnamed samples refuse; 128 px tile floor plus 8x means about 1,024 px on screen; render-fence stall per event.
- Owner items: office census on this branch (proxy_vt_eligible, auto_proxy_vt, stack_layout and prefetch_incomplete counts); the 8x yield question; the follow-up drop decision (Chat/owner REQUIRED before any merge).
- Ledger rows 092-03/03b/03c are already present (the runner appends them).
- 092-04 design round B (READ-ONLY reviewer, Astra max, workspace-write in its run folder) STARTED 01:18:24. Run `_relay\runs\2026-10-06-092-04-vt-transform-design\`. Covers the transforming VT producer (tile remap at same or Nx virtual size, scramble page/block permutation, BC5 invert/green_flip block transform), stacks (including blurry multi-layer), mesh route comparison, and the first proof. Waiter task btkeb1qtx (115 min).

## 2026-10-06 02:10 IST: 092-04 ruled; 092-05 launched
- 092-04 (read-only design round B, 2,779 s) ACCEPTED as research: `_reviews\092-04-ruling.md`; report at `_reviews\092-04-codex-vt-transform-design.md`; ledger row added by hand (the reviewer runner does not append).
  - Selected design (recommendation to the owner): a plugin-owned transforming VT producer with its own cooked-page reader, because FUploadingVirtualTexture is not exported.
    - tile: same size, L+log2 N (`tile_vt_mipcopy`);
    - scramble: per-mip cells with seam/tail reconstruction (`scramble_vt_cells`);
    - normal: BC5 block transform (`invert_vt_bc5` / `green_flip_vt_bc5`);
    - the mesh route is rejected.
  - Effort about 5-10 engineer-weeks for the family; the first proof would be the BC5 normal producer.
  - New subtypes, so the owner/chat must approve before ANY UV/normal VT build.
- 092-05 implementer STARTED 02:08:05 (Astra max, full access per the 090-12s standing decision). Run `_relay\runs\2026-10-06-092-05-vt-blur-stacks\`; branch `feat/vt-blur-stacks` off `af79520`; evidence `E:\IA_BuildCache\_r921\09205\`.
  - Part A: census VT diagnostics, numbers only, pushed once its gates pass.
  - Part B: multi-layer stack support for blurry proxy_vt (one private VT per layer, readiness proven on the combined allocation).
  - Waiter task launched (115 min). Mandate: the owner's "fix Bates" plus "Continue overnight"; no new subtypes and no shipped behaviour change.

## 2026-10-06 06:20 IST: 092-05 RULED GREEN; af79520 crash finding
- 092-05 ACCEPTED: `_reviews\092-05-ruling.md`; report at `_reviews\092-05-codex-vt-blur-stacks-report.md`; the ledger row was appended by the runner.
  - Branch `feat/vt-blur-stacks` @ `5dc8717` (runtime `502de71`) supersedes `af79520`. Refs are unchanged elsewhere and the host is at `23098fa`, clean.
  - Independent checks: identity recomputed from PNGs (bound = null); staged edges 120..167 exact with all layers changing together; visual check; numbers-only census lines; the CopyTexture readback is gone.
- **CRASH in af79520:** at capture start, with a multi-layer VT stack in view and UV, normal or blurry (non-hold) enabled, or with a legacy census form. D3D12 CopyCommandList Close E_INVALIDARG from the uniformity warm-up's page-table copy. The release is NOT affected. Amendment appended to the 092-03c ruling.
- **Office command now:** pull `5dc8717`; run `IAI.TexCorrupt.Census vt`, then `IAI.TexCorrupt.Census all vt`; type back the `IAI-VT-CENSUS v1` lines (family 0=UV, 1=normal, 2=blurry). Only then a test capture with blurry on.
- **Overnight work complete. Nothing is running. Waiting on the owner** for: the office reading; the 8x question; the follow-up drop; and a go/no-go on the UV/normal VT producer (092-04, new subtypes, 5-10 engineer-weeks).

## 2026-10-10: M3 shipped; Bates M3 dropped; post-delivery disk cleanup done
- Owner: M3 has shipped (`release/2026-10-05` @ `d7eee97`), and **Bates M3 will not ship**.
- At the owner's go, these were deleted permanently: `D:\IA_BankOverflow\_r912`, `_r910f2`, `090-10b2-evidence`, all `M52S09_*`/`M52S10_*`/`M53DC*` sessions, `_reviews\089-03/089-04/090-06-evidence`, and `E:\IA_BuildCache\_r912b`, `_r913`, `_r915`, `_r916`, `_dash`, `_r921` (Bates VT 092-03..05).
- Free space now: D: 207 GiB, E: 282 GiB. Paths above that pointed into those folders are gone; use the rulings and reports instead.
- The 091-06 uncommitted F1 office-robust WIP is saved as a patch. Receipt: `_reviews\2026-10-10-disk-cleanup\README.md`.
