# Owner decision — 2026-10-02 ~17:10 IST: Codex implementer jobs run with full access; the sandbox's permission entries are removed

Context: the implementer-Relay smoke (`_relay\runs\2026-10-02-090-12s-impl-relay-smoke\report.md`, Astra/max, 38 min, VERDICT RED) showed that Codex's Windows "elevated" workspace-write sandbox runs commands as a separate local user (`CodexSandboxOnline`):
- git refuses every repository ("dubious ownership"; the worktrees are owned by the owner's user), and pushes would need the owner's credentials, which belong to his Windows user;
- UnrealBuildTool crashes within 4 s: UHT writes `Timestamp` files under every engine plugin's `Intermediate`, and MSBuild cannot read `%LOCALAPPDATA%\Microsoft SDKs`;
- it can open a game window and initialise D3D12, and it correctly refused the out-of-root write probe.
The handover (§3 Job B) required asking the owner once before using `danger-full-access` for implementer jobs.

**Owner's answers (via the Code Desktop question card):**
1. **Full access** for Codex implementer jobs (builds, game legs, commits, pushes). This is the same trust level the mailbox Code jobs had (`--dangerously-skip-permissions`). The written implementer rules (`_relay\gdp-impl-block-v1.txt`) still bind every job, and Code Desktop reviews every diff and re-checks the evidence. **Read-only reviewer jobs stay sandboxed** (`relay_run_codex_gdp_v1.ps1`, workspace-write in the run folder).
2. **Remove** the "Modify" entries the smoke's sandbox set-up added for the local group `CodexSandboxUsers` on the folders it was given. Receipt of the prior ACLs: `_relay\runs\2026-10-02-090-12s-impl-relay-smoke\acl-receipt-before-removal.txt`.

How it is applied: `relay_run_codex_gdp_impl_v1.ps1 ... -Sandbox danger-full-access -OwnerApprovedFullAccess`. The runner records `owner_approved_full_access=True` in each run's `run-meta.txt`. Model and effort stay pinned: `gpt-6-astra` only, effort up to `max` (owner, 2026-10-02: "only astra", "up to Max effort").
