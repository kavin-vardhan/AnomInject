# Chat handoff: Europa (UE 4.25) port, 094-42c running

**Written:** 2026-11-03, about 17:10 IST, by chat Claude (orchestrator).
**Supersedes:** `CHAT-HANDOFF-094-europa-A6-done-A7-running.md`.
**Codenames only:** Europa, Europa's fork, Concorde, Bates. Never write a real title.

> 🔴 **Don't save this file into the plugin's `docs\` folder while a mailbox job runs.** The watcher runs Code inside the protected `m51` checkout, and a new untracked file there trips the P9 check. Keep it in the project and in `_reviews\`.

## 1. State right now
- **In flight:** `094-42c-europa-p9v3-fixture-gates-spike.md`, staged 17:04 IST 11-03, Code (opus-5-5, xhigh), and Codex via thread `01a14334` with full access. Request: `_reviews\094-42c-request.md`. The next check-in is chat's job: read `to_chat\094-42c-*.report.md` and `_reviews\094-42c-code-review.md`, then rule (`094-42c-ruling.md`).
- **What 094-42c does:**
  1. **P9 v3:** v2 minus the deleted `D:\IA_BankOverflow`, plus three lines in `owner_p9.py` and one in `root_attribution.py`.
  2. **The outline fixture** at `E:\IA_Fixtures\Outline425\`: author the outline level (every launch with `-nowrite`), prove the outline leak at home by pixel difference, and prove the guard refuses before any tag.
  3. **FZ6(b):** the recompile cost of one new `FMeshMaterialShader` type, plus a real link test of the depth-only shaders' `StaticType`, on a throwaway branch that is never merged.
  - It includes GA5: any admission gate held for 3 h stops the job with NEEDS-DECISION.
- **Branch:** `feat/europa-port`, local `f7b32f0` (8 commits on top of `d1cdc7b`, unpushed). `origin/feat/europa-port` = `d1cdc7b`.
- **The 4.25 port:**
  - **Done:** capture, labels, masks and census at 5.1 parity; auto-injection with blinking and lod_corruption (plus test-only lighting_mismatch and missing_object).
  - **New in `f7b32f0`:** the guard (FZ3) and client-only default routes (FZ4).
  - **Still open:** custom-depth label leakage on hosts with outline effects. The fix (FZ2) is not built yet.
- **Disk:** D: about 62 GiB free. The owner deleted `D:\IA_BankOverflow` (23.6 GiB); an empty `_r9315` folder may remain, which is harmless. E: about 130 GiB free.

## 2. What happened this session, and why
- **094-40 GREEN, then the office capture (11-02).** The card was `094-40-office-card-europa-capture.md`, sha `337cbb61…`.
  - Reads are in `_bates_reads\2026-11-02-europa-094-40-office-capture-reads.md` (numbers only).
  - Results: 2,352 frames, 0 dropped; fires 22/13/19/15, identical to home because the seed and fixed step match; receipt PASS.
  - Differences from home: the game forced **1920×1080**, it runs **D3D12**, the client is custom, and the self-check showed **`polluted=1`**.
  - An office-side tool ran the 0.5 % floor and rewrote `-ExecCmds`. A quoted `-ExecCmds` was already fine on stock 4.25.
- **The outlines (094-41, plan only, GREEN).** Europa's game has a post-process outline that reads custom depth, and our 4.25 capture writes custom depth in four ways:
  - the forced `r.CustomDepth 3`;
  - census batches re-tagged every request;
  - event targets for the whole live fire;
  - the street bench's three planted tags.
  - These writes happen in the same frame whose colour is saved, so the result is **label leakage**: the labelled object is outlined. Hidden objects (missing, blink-hidden) are drawn only by custom depth, so the outline shows exactly where a "missing" object is.
- **Rulings 094-41 (FZ1–FZ10), in `claude/094-41-ruling.md`:**
  - **FZ1:** the 11-02 office frames are not training-clean. They stay on the office PC.
  - **FZ2:** the fix is a plugin-owned private ID/depth pass that never touches the host's custom depth. The **preferred route uses the engine's existing depth-only shaders**, so no new material shader type is needed. A new `FMeshMaterialShader` type is the fallback only if that route is infeasible. The same-frame scrub is rejected (leaky).
  - **FZ3:** an interim guard (scan for host custom-depth readers, refuse before any tag), and planting only on fixture `EA_` actors.
  - **FZ4:** routes default to the client's anomalies.
  - **FZ5:** a home outline fixture.
  - **FZ6:** a spike to size the shader route.
  - **FZ7:** resolution, D3D12 and the card fixes are folded into the FZ2 job.
  - **FZ8:** read-only jobs use fresh review-runner threads; full-access resume only for jobs that build or launch.
  - **FZ9:** a read-only 5.1 check is queued.
  - **FZ10:** foreign load is noted, not ours.
- **094-42 ran into a disk wait.** It waited about 10 h at FJ1's D: admission (14.3 GiB free against 25), because the D: page file grew under foreign load (ComfyUI, renders, a Wi-Fi driver holding about 8 GB). The owner rebooted, and it was re-staged as 094-42r from `4fdfe84`.
- **094-42r NEEDS-DECISION, ruled GA1–GA6 (`claude/094-42r-ruling.md`):**
  - FZ3, FZ4 and the EA_Core regression are accepted (all gates; scanner `none`).
  - The fixture authoring stopped because `-nowrite` was missing (its own `Engine.ini` is evidence, not a Paris change).
  - FZ6(a) is buildable by source; the remaining risk is the unexported `StaticType` link.
  - GA5 sets a 3 h admission bound.
- **094-42d (read-only delete check)** found BankOverflow safe to delete. The owner deleted it, which is why 094-42c writes P9 v3.

## 3. Owner decisions (locked)
- **Client set (11-02):** missing_object and lighting_mismatch are not client anomalies. The capture fires **only the client's anomalies by default**: blinking + lod_corruption now, the texture bugs after Phase B. The other two are test-only, via an explicit route list.
- **Office plugin install:** at the office the plugin is **copied** into the game folder, not cloned, and it never goes into the game's repo. R1 is therefore read from the clone it was copied from. Card fix pending (FZ7).
- **Pushes:** **Code pushes** (`feat/europa-port` only; no tags, backups or force). Owner's call, 11-02.
- **J2:** BankOverflow deleted, and J2 is no longer resumable from the archive. If J2 ever resumes, it is rebuilt from git; 16 small `_r9315` scripts would need rewriting.
- **Scheduling:** the owner's renders come before heavy jobs. When he says he's rendering, stage only read-only work.

## 4. Forward plan, in order
1. **Rule 094-42c.**
   - If GREEN: pick the shader route from the numbers (depth-only preferred), and the repro fixture becomes the home gate for the fix.
   - If FZ6(b) shows the depth-only link fails, decide the fallback from the recompile cost.
2. **FZ9 (read-only):** did the 5.1 hosts' recorded scanner readings ever show a custom-depth reader? Earlier 5.1 data may carry the same leak. Home records and typed reads only.
3. **094-43, the FZ2 implementation:**
   - the private ID/depth pass for the census and masks;
   - remove `r.CustomDepth` forcing;
   - keep FZ3 as the fallback;
   - plus FZ7: any viewport size with pitch-aware colour and mask copies; size, RHI and client as readings; D3D12 qualification including an odd width;
   - the card fixes (run the line as written, R1 from the clone, keep the copy out of the game repo).
   - **Gates:**
     - the fixture shows no outline in any saved frame;
     - `polluted=0`;
     - masks are still correct;
     - the EA_Core regression passes.
   - **Then:** a scan, a push, and a new office card.
4. **An office re-test on Europa** with the new card (blinking + LOD by default).
5. **Then Phase B:** missing_texture and corrupted_texture on 4.25.
- **FUTURE, in an owner-set order:**
  - D1–D5, the dataset-quality items;
  - the deferred A5, A6 GC, A8 and controls;
  - keys for the new pool slots;
  - G498's 5.1 follow-up;
  - `ReGate51` (33.6 GiB) and `Baseline51` (18.9 GiB): a dependency check, then the owner's OK.

## 5. Open questions
- 094-42c's result (route choice, fallback cost).
- Whether earlier 5.1 data is affected (FZ9).
- Whether the M3 "blurry, private copy per object" version shipped on 10-05 or stayed for the follow-up drop. Chat had no record; verify before telling anyone.

## 6. Corrections and things that changed
- The value **203 / EA_BlinkCube** was planted by C5's street bench, not owned by the host (Code's earlier notes were wrong).
- **`SetRenderCustomDepth` on 4.25 updates the proxy directly.** Only a write-mask change rebuilds it.
- **The office game forces its own resolution.** Never assume 1280×720 there.
- **The relay's resume script refuses any resume without `-OwnerApprovedFullAccess`** (exit 7). Read-only jobs use fresh threads (FZ8).
- **`D:\IA_BankOverflow` no longer exists.** It's off every "don't touch" list. The implementer block's overflow rule is now "stop and report".
- **Brief template changes from 094-42c on:**
  - no BankOverflow in "Don't touch";
  - GA5's 3 h bound;
  - `-nowrite` on fixture launches.

## 7. Mechanics a new chat needs
- **The mailbox:**
  - stage `D:\IntrusiveAnomalies\_mailbox\to_code\<name>.md.part`, check the sha256, then `mv -n` to `.md`;
  - one brief in flight at a time, and each name runs once (`ran.txt`);
  - after a NEEDS-DECISION or RED report, the next brief's first line is `# ACK-VERDICT: …`;
  - state is in `watcher_status.txt` and `watcher.log`, and reports land in `to_chat\`.
- **The run folder:** `D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\`, with receipts in `code-receipts-094-<id>\` and `midjob-notes.txt` for progress.
- **The gates:** FM1 (builds), FK1 (memory), FJ1 (lean profile; D: starts at 25 GiB, floor 15 GiB), GA5 (3 h bound).
- **Waiting for Code:** adaptive polling worked this session. Use `send_later` check-ins at the expected finish, and no tight polling.
- **Standing rules:**
  - the HARD RULE: office data never comes home, only typed or photographed reads, saved numbers-only into `_bates_reads`, and strip names, levels and IPs from photos;
  - never kill by name;
  - no machine settings;
  - delete only by receipt;
  - never touch `m51`, `master`, `release/2026-10-05`, tags, `ToCodex\`, `E:\<personal-folder>`, `E:\UEProjects\LyraStarterGame` or `E:\Unreal Projects\StylizedParisStreet`.

## 8. Pointers
- **Rulings and requests** (project `claude/`): 094-40 to 094-42c, and `094-41-queued-decisions.md`.
- **Reviews** (`D:\IntrusiveAnomalies\_reviews\`): `094-41-codex-plan.md` (the options and file plan), `094-41-code-review.md`, `094-42r-code-review.md`, `094-42d-code-findings.md`.
- **Office card:** `094-40-office-card-europa-capture.md`. It will be replaced after 094-43.
- **Have Code read, for operational detail:** the plugin's `docs/sessions/2026-11-0*` journals on `feat/europa-port`, `docs/europa/`, and the gotchas file.
