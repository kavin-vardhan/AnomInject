---
name: gdp-impl-relay-orchestrator-guard
description: "How a mailbox brief launches a Codex implementer job: -OrchestratorBrief guard, -Sandbox danger-full-access, detached Start-Process"
metadata:
  node_type: memory
  type: reference
  originSessionId: <uuid>
  modified: 2026-11-02T18:05:36.114Z
---

Since 093-03 (2026-10-13), `_relay\relay_run_codex_gdp_impl_v1.ps1` takes `-OrchestratorBrief <brief file name>`.
- While the watcher runs, the runner normally refuses with exit 6. This flag lets a launch through only when `_mailbox\watcher_status.txt`'s `brief=` value starts with exactly that name, followed by a space or the end of the value (ordinal match).
- `-GuardCheckOnly` exits right after the guard (0 pass / 6 refuse) and writes nothing. Use it to prove the guard both ways before a real launch.
- `run-meta.txt` records `orchestrator_brief=... watcher_match=...`.
- Receipts of the pre-change files: `_relay\runs\2026-10-13-093-03-j1-natural-base\code-receipts\`.

**Gotchas when launching:**
- Implementer jobs need `-Sandbox danger-full-access -OwnerApprovedFullAccess` (owner decision 090-12s). `-OwnerApprovedFullAccess` alone leaves the default `workspace-write` sandbox, which breaks git and UBT. 093-03's brief command omitted the sandbox flag; I added it, as every earlier implementer run did.
- Under full access the `-AddDir` list is recorded but not passed. The defaults already cover `E:\IA_BuildCache`.

**How to launch detached:**
- `Start-Process powershell.exe -ArgumentList ... -RedirectStandardOutput <run>\runner-stdout.txt -RedirectStandardError <run>\runner-stderr.txt -WindowStyle Hidden -PassThru`.
- Then poll for `exit-code.txt` with a bounded condition loop (≤570 s per call).
- 093-03's J1 job took 4,434 s; it stopped early at a gate.

**Resuming an implementer session (093-04, 2026-10-13):**
- `relay_resume_codex_impl_v1.ps1 -Run <folder> -Workdir <dir> -ResumePrompt <file> -OwnerApprovedFullAccess` worked first try, with the same thread id.
- Before resuming, copy `report.md`, `last-message.txt` and the runner stdout aside: Codex overwrites them in the same folder. The script moves `exit-code.txt` to `exit-code-attemptN.txt`.
- Build the prompt with `cmd /c type a b c > out` and byte-compare it to the concatenation.
- Codex applies stop rules strictly, even to its own harness errors: 093-04 halted after 1,508 s on a cook-path mistake. Take your own staging/refs/process baselines before launch, so the review can check its restore claims independently.
- Ruling 093-04 D2 since then: harness-only failures are fixed in-job (≤2 attempts, disclosed). The 093-05 resume ran the full PIE+cook+staged matrix in 10,698 s (~3 h) and pushed. Review traps it produced: post-result checker "scope fixes" (verify against the written prediction AND scan whether any certifying leg depends on the fix), can-fails named in predictions but silently not run (grep the control harness for each named control), and reader caveats the report omits.

**A read-only resume is impossible with the relay script (094-41, 2026-11-02):** `relay_resume_codex_impl_v1.ps1` refuses without `-OwnerApprovedFullAccess` (exit 7, nothing written) and always resumes with `danger-full-access`. When a brief says "resume, no full access", the refusal counts as the failed resume. 094-41 recorded it twice, then started a fresh thread via `relay_run_codex_gdp_v1.ps1 -Run <code-receipts-…\fresh-run-…>` (workspace-write confined to that folder; Codex writes report.md there, Code copies it to `_reviews`). The review runner writes no ledger row.

**After a PC shutdown kills a job (093-10, 2026-10-15):**
- The dead attempt leaves `events.jsonl` with no `exit-code.txt`; never write one. Confirm no process has the thread id in its command line, compare `LastBootUpTime`, write a note, then treat the relay as free.
- `relay_resume_codex_impl_v1.ps1` does NOT refuse on the missing exit code; it resumed fine. Its attempt numbering counts existing `exit-code-attemptN.txt` files, so a crash leaves numbering one behind. Consequence seen in 094-07 (2026-10-24): the next resume after the post-crash one reused the ledger id `…-resume5` in `_relay\ledger.csv`. It is cosmetic, but note it in the review; never edit the script or the ledger.
- Codex's job after a crash: rehash the candidate vs the manifest, restore Saved/Config from its own pre-run copies, then continue. Readers it edits mid-campaign are preserved under `harness-repairs\...`; diff them against the pre-leg freeze hashes.

Related: [[gdp-relay-codex-delta-timing]], [[gotcha-numbering-diverges-across-branches]], [[buildcookrun-cookoutputdir-needs-platform]].

**Fast mode (094-48b, 2026-11-06):** owner-ordered copy `_relay\relay_resume_codex_impl_fast_v1.ps1` adds `-c service_tier="fast" -c features.fast_mode=true` to the resume line; use with `-Effort high`. codex-cli 0.156.1 accepted it; the rollout's `thread_settings_applied` reads `service_tier="priority"` (vs "default"). 094-48b (finish 4 gates + a new change + 2 builds + 6 game launches) took 3,385 s. Never edit config.toml for it.
**Watcher death kills the Codex job:** 094-48's detached resume died with the watcher at 15:34 (no exit-code, no ledger row). Recover as after a PC shutdown: check no process carries the thread id, record the events.jsonl length, resume with a new prompt.
**Wrapper trap:** passing a bare `-` as a parameter value through `powershell.exe -File` silently aborts the child (no output, no exit code); use a word sentinel like `absent`.
