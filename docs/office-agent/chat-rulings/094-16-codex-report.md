<!-- Collected by Claude Code (mailbox brief 094-16) from D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\report.md; sha256 B54B1E4F5B6CB83092558BDF53456E324B3870DD4E5F800EE54A3E13C3F85EFC, 12753 bytes; thread 01a14334, resume 02:51:57-04:04:02 IST, 4325 s, exit 0; body below is byte-identical -->
The A6 owner kit is **armed and dry-run verified** under EP1–EP2. All three real attempt slots remain unused; **0 Unreal/runtime launches** occurred. The 27/39/44 bound artifacts and manifests are unchanged. Entry, throwaway preflight and exit proofs pass against P9 v2. EP2 tests pass on the first attempt: **62 new checks plus 48 measured-gate regressions**. Final feature HEAD: `303d02d7439fce04e68e1b5e724b10888ebeae43`; local only.

**Protected state and prerequisites.** Entry and exit each checked 472,553 inventory paths and 23,033 full binary/config hashes (104,794,989,456 bytes). The 1,023 previously admitted EG1 bookkeeping paths remain accepted; this job changed no engine-tree path. P9 v2 re-derived byte-for-byte with SHA-256 `8e3ed0956a28e51c4205e334a5f9d81bbedd8c3afb606ed7234e83c111a32c74`. No full-tree hash was repeated. Protected refs, m51’s HEAD/index/15 untracked entries, other protected worktrees, engines and original hosts match. ReGate51 stays at `550e9b7`; Baseline51 stays at `d7eee97`.

ED3 confirms the pinned compiler/SDK files: 4.25 MSVC product 14.16.27051, directory 14.16.27023, SDK 10.0.17763.0; 5.1 directory 14.38.33130, compiler product 14.38.33144, SDK 10.0.22621.0. Installed directories and Visual Studio versions are recorded read-only. No UBT, build, setting change, push or tag occurred.

Proofs: [entry](D:/IA_Europa/Receipts/09416/P9/entry/summary.json), [exit](D:/IA_Europa/Receipts/09416/P9/exit/summary.json), [final git state](D:/IA_Europa/Receipts/09416/P9/final-git/git-state.json), [toolchains](D:/IA_Europa/Receipts/09416/toolchains.json), [110 artifact re-hashes](D:/IA_Europa/Receipts/09416/exit-bound-artifacts.json).

**EP2 implementation and tests.** Atomic directory claims take the first unused slot. A process-held Windows lock serializes double-clicks. A durable launch marker blocks every later attempt, including after the editor exits. A pre-start failure releases only its own launch reservation by receipt; ambiguous interrupted starts refuse until chat decides. Slow inventories precede the quiet-start gate. Only the owner profile omits human idle; measured profiles retain it.

| Control | Proven result |
|---|---|
| Slots 1, 2, 3; pre-existing slot; pre-start refusal | Ordered selection; next slot remains usable |
| All slots used; launched marker; fourth attempt | Refused |
| Concurrent double-click; interrupted reservation | Refused; no extra session |
| Unarmed or widened authorization; wrong dry/live scope | Refused |
| Measured profile without human idle | Refused; all 48 existing checks pass |
| Memory, GPU, foreign programs, unknown readings, disks | Required checks retained; floor breach stops |
| Waiting messages | Every unmet condition named; one line each minute |
| 45-minute timeout | 45 waiting lines, refusal at synthetic second 2700, 0 launches |
| Three passing polls | Clearance at synthetic seconds 0/25/50 |
| Popen start callback and callback failure | Two harmless Python children; own-child cleanup proven |

[Full 62-case table and 48 regressions](D:/IA_Europa/Receipts/09416/ep2-tests-attempt1.json). The live observe-only sample had 8.11 GiB free physical memory and 0% GPU: owner conditions had no unmet item while the measured profile still rejected recent input. One reporting-helper invocation omitted its owned root and counted its own collector; [the corrected observation](D:/IA_Europa/Receipts/09416/owner-gate-live-observation-corrected.json) supersedes it. The actual wrapper already passed its PID and required no gate fix. EP2’s second D2 attempt was not needed.

**Armed scope and slots.** Both `authorized_by_chat` and the scoped `runtime_authorized` are true, citing [094-15 ruling EP1](D:/IntrusiveAnomalies/_reviews/094-15-ruling.md). Authorization is exactly one owner-started 4.25 EditorPreview session: open `/Game/ParisStreet/Meshes/SM_WindowHouse3` for 60 seconds, then File > Exit, saving nothing. No PIE, commandlet, cook, packaging, automated input or second editor session is authorized.

- `D:\IA_Europa\Receipts\09416\owner-attempts\01`
- `D:\IA_Europa\Receipts\09416\owner-attempts\02`
- `D:\IA_Europa\Receipts\09416\owner-attempts\03`

Their TEMP paths mirror these under `D:\IA_Europa\Temp\09416\owner-attempts`. All six paths remain absent. Real launch state `D:\IA_Europa\Receipts\09416\owner-state` is also absent. A future invocation creates the claimed slot and its configuration; no real slot was consumed by testing.

**Dry run and hashes.** The actual `.bat` ran with `--dry-run`, a separate armed throwaway configuration and explicit synthetic gate samples. Real environment filtering, P9, binary/source hashes, routing and ledger checks passed. Synthetic readings first named low memory, GPU load and a foreign helper, then cleared three spaced polls. The result is `ARMED_DRY_RUN_PASS`, with 0 launches. [Dry-run receipt](D:/IA_Europa/Receipts/09416/dry-run/attempt-01/attempt-result.json); [real-slot preservation](D:/IA_Europa/Receipts/09416/armed-dry-run-result.json).

| Kit file | SHA-256 |
|---|---|
| Europa425-Editor-OwnerLeg.bat | `4b5b2812196e0d5256ec086f7d695ee46827c5b2bb65eb8e2b62973fa7dddd74` |
| Europa425-OwnerLeg.json | `1f328ec641fd2bbdc6d977d11e9f817e26c1c8b33cf62cb02c5c64159ed16b95` |

All 50 tooling/dependency hashes, including the retained P9 helpers and Python executable, are in [kit-state-final.json](D:/IA_Europa/Receipts/09416/kit-state-final.json). The launch and tracked `.bat` copies are byte-identical. The three immutable manifest hashes remain:

- 4.25, 27 files: `4beccca6a58a32b643a691d8c1b460bef0cb997db59d8c8bd17a954b9f42931f`.
- Baseline51, 39 files: `617a908243fccc3bd1f8fefc82768d79e7052fa2136be4223cfb7450ac079a8c`.
- ReGate51, 44 files: `f6e52a85b078fc44f713a32f649dd70dcf4daa74868e323568abbec09d8fa7b2`.

The linked artifact receipt includes every file path and full binary hash. No archive was created and no binary was rebuilt.

**Exact owner-visible console lines.** These are the recorded literals. The attempt number, PID and combination of unmet conditions vary; PID 42 is synthetic.

```text
Europa owner preview: checking authorization.
Attempt 1 of 3: checking protected files, binaries and routing. This can take several minutes.
Attempt 2 of 3: checking protected files, binaries and routing. This can take several minutes.
Attempt 3 of 3: checking protected files, binaries and routing. This can take several minutes.
Protected files, binaries and routing passed. Checking for a quiet start; you may use the mouse and keyboard.
Waiting: free memory is below 6 GiB. Leave this window open.
Waiting: GPU use is above 15%. Leave this window open.
Waiting: another Unreal or game program is running (PID 42). Leave this window open.
Waiting: free memory is below 6 GiB; GPU use is above 15%; another Unreal or game program is running (PID 42). Leave this window open.
Waiting: cannot read free memory; cannot read GPU use; cannot read running programs; cannot read free space on C; cannot read free space on D; cannot read free space on E. Leave this window open.
Waiting: quiet-start checks 1 of 3. Leave this window open.
Waiting: quiet-start checks 2 of 3. Leave this window open.
Quiet-start checks passed three times.
No quiet start within 45 minutes. No editor was started. Close heavy jobs, then double-click again to use the next attempt.
Starting the one authorized editor session. Do not open Play. Answer No to any rebuild prompt. Close with File > Exit and save nothing.
Editor closed. Tell chat what happened. This kit will not start another editor session.
Dry run complete: the armed path passed. No editor was started; real attempts were not used.
Stopped: This kit is not armed by chat. No editor was started.
Stopped: All three attempts have been used. Ask chat to prepare more; no editor was started by this attempt.
Stopped: The authorized editor session has already started. Ask chat before trying again.
Stopped: Another kit attempt is running. Keep its console open; no editor was started by this attempt.
Stopped: A prior launch has an incomplete receipt. Ask chat before trying again.
Stopped: A disk is below its safety floor. No editor was started.
Keep this console open and send the message to chat. Do not accept rebuild, convert, update or resave prompts.
```

[Console-line receipt](D:/IA_Europa/Receipts/09416/owner-console-lines.txt). The `.bat` adds Windows' localized pause prompt outside dry-run mode. Detailed JSON stays in receipts. Chat's card should retain "close Blender and other heavy jobs first" and "answer No to rebuild modules"; the owner need not wait for five minutes of human inactivity.

**Secret and canary proof.** The launch environment contained the actual canary and `CLAUDECODE`; the positive control matched the launch hash. Child receipts contain no blocked name. The final canary scan starts at the launch second and finds 0 matches in 328 new/changed files. EK1 inspected 38,200 files with 0 errors and 0 secret values; all six retained makefiles decode and contain no blocked environment name. Only accepted placeholders, source constants and the engine’s fixed build define remain. No secret value was printed, written or redacted in this job. [Scan qualification](D:/IA_Europa/Receipts/09416/secret-scan/qualified.json); [final canary scan](D:/IA_Europa/Receipts/09416/secret-scan/canary-final.json).

**Write ledger and free space.** Every operation passes. Resolved-volume readings below are GiB before → after. Reservations are raw C/E/EG1 budget bytes; tooling deltas are `.claude`/`.codex`/`ClaudeTemp` bytes and remain outside the port budget. The two dry rows are nested inside operation 02 and are not additive.

| Operation | C: | D: | E: | Reserved B | Foreign events | Tooling deltas B |
|---|---|---|---|---|---:|---|
| dry/launch-admission | 18.09 → 18.09 | 80.30 → 80.30 | 123.45 → 123.45 | 0/0/0 | 2 | 1205991/108229/52141 |
| dry/preflight | 18.11 → 18.09 | 80.62 → 80.30 | 123.45 → 123.45 | 0/0/0 | 27 | 204226/293365/24 |
| 01-ep2-tooling | 18.16 → 18.12 | 80.83 → 80.62 | 123.45 → 124.09 | 10036/0/0 | 135 | 194672/4565976/17087 |
| 02-arm-and-dry-run | 18.11 → 18.09 | 80.62 → 80.30 | 123.45 → 123.45 | 0/0/0 | 31 | 2127176/-3591698/52175 |
| 03-documentation | 18.09 → 18.09 | 80.30 → 80.30 | 123.45 → 123.45 | 0/0/0 | 1 | -1781211/50923/-42 |
| 04-exit-proofs | 18.09 → 18.03 | 80.30 → 80.18 | 123.45 → 123.43 | 0/0/0 | 26 | 3300307/6528921/1730 |
| 05-closeout | 18.03 → 18.03 | 80.18 → 80.18 | 123.43 → 123.43 | 0/0/0 | 3 | 297542/92951/1113 |
| 06-final-binding | 18.03 → 18.03 | 80.18 → 80.18 | 123.43 → 123.43 | 0/0/0 | 5 | 5397/-17207/-233 |

All recorded drive readings remain above ED1 warnings and floors. The ledger conservatively reserves 10,036 C: bytes for 3 shared CrashReportClient log changes; no port UE or crash-report child ran, so the attribution addendum records them as foreign while retaining that budget reservation. Port-attributed C:, E: and engine-tree writes are 0. Ledger storage totals 0.414 GiB, below 2 GiB. No bound path changed in any ledger delta. Snapshots retain 205 access-denied entries in OS/tooling locations; protected enumeration has 0 errors. Per-path foreign writes, snapshot hashes, full tooling measurements and drive readings are in [ledger-summary.json](D:/IA_Europa/Receipts/09416/ledger-summary.json).

**Commits and scope.** All changes are tooling/documentation effect class C; production, probe and host sources are unchanged. Commits on `feat/europa-port`, with W4 date variables absent:

- `27e6b3c99decfbe085d43c5d634f74b4d00ef338` — 2026-10-26T03:05:56+05:30 — feat(europa): add bounded owner preview attempts and quiet-start messages.
- `d751456fa272cc8c2df636716503e3afea64b79b` — 2026-10-26T03:26:14+05:30 — docs(europa): record armed owner kit scope and dry-run evidence.
- `303d02d7439fce04e68e1b5e724b10888ebeae43` — 2026-10-26T03:56:21+05:30 — docs(europa): bind owner kit closeout and EP2 prediction addendum.

The dated EP2 prediction addendum is committed before any owner or authoring leg. No commit was pushed; no tag, setting or bench lever changed.

**EN5 applications: none.** EP1–EP2 directly authorize the kit changes; no stock-behavior exception was needed under [EN5](D:/IntrusiveAnomalies/_reviews/094-13-ruling.md). No hard STOP fired. This job ends with the armed kit; chat supplies the final card and stages 094-17 after the owner says done. Fixture authoring remains a separate ruled job.

VERDICT: GREEN | A6 owner kit armed; three real slots unused; armed dry run passes; zero runtime launches.
