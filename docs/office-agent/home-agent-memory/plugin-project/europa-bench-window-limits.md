---
name: europa-bench-window-limits
description: "094-20 facts - foreign D: dips and the EI2 commit gate can consume a whole 9-hour Europa build job; launcher-held temp files need FileShare.Delete; background tasks stop at 2 h"
metadata:
  node_type: memory
  type: project
  originSessionId: <uuid>
  modified: 2026-11-05T11:07:40.623Z
---

094-20 (2026-10-27/28) ran 9 h and built nothing: 17 EU2 pauses (~2.6 h) on foreign D: dips whose minima repeat at ~44.3 GiB (a ~22 GiB allocation that comes and goes; only 10 of 124 low readings explained by page-file growth; writer not established), plus ~2.5 h waiting at EI2's commit gate (>= 16 GiB free) that never passed. Commit holders with no Blender running: wlanext.exe ~9 GB and growing over the day, WSL vmmem 4 GB, launcher 2.3 GB, other Claude sessions' scripts (one asr.py at 9.6 GB).

**Why:** a rebuild job launched under the same conditions will likely end the same way (NEEDS-DECISION at the 9-hour limit).
**How to apply:** before launching a Europa build job, record D: free, commit free and the top commit holders, and say plainly in the report whether a build window is plausible; hold the launch for D: >= 70 GiB (ED1 start figure) as 094-20 did.

Tooling facts from the same job:
- The owner-slot temp files under D:\IA_Europa\Temp\09417\...\owner-leg-launch-admission are readable only when opened with FileShare.ReadWrite|Delete (the launcher holds them with delete sharing); a ReadWrite-only open reports "unreadable".
- Background Bash/PowerShell tasks are killed at 2 h even with the max timeout, and the harness says not to restart them; keep long series in foreground polls (wait20b.ps1 pattern: log once a minute while waiting).

094-47 (2026-11-05): the PC entered modern standby 15:30:45-16:13:50 IST mid-job (System log Kernel-Power 506/566 enter, 507 exit). Codex, the wrapper and my background poller were suspended and resumed on their own; a 43-min gap in poll lines is the sign. Check those event IDs before suspecting a hang; never change power settings.

Related: [[europa-owner-kit-review-facts]], [[gdp-impl-relay-orchestrator-guard]].
