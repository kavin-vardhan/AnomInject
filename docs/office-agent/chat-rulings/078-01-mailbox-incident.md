# 078-01 mailbox incident — 2026-09-06

Recorded by Codex during the authorized preflight. This is operational evidence, not a Code verdict or a replacement brief.

- The old idle watcher PID 21728 stopped through its STOP-file protocol at 14:47:28. Codex verified it had exited and the queue was empty, then started watcher PID 33896 hidden. It logged startup at 14:50:13 and dispatched 078-01 at 14:50:33, Code session `<uuid>` / PID 25988.
- Another watcher PID 59260 logged startup at 14:53:41 and ran crash recovery at 14:53:42 despite the original watcher and Code process still running. Its launch source is not established here. The resulting INTERRUPTED report is a false interruption report; it cites the previous default session rather than the live 078-01 session.
- At approximately 14:55–14:56, PID 59260 was no longer present; PID 33896 and Code PID 25988 remained. The active brief was present again. The status file still reported idle with the departed PID 59260. Do not treat that stale status or the stub report as completion.
- The stub report is preserved as `078-01-false-interruption-report-20260906-145341.md` in this directory. The original runner is expected to publish the actual final report when Code finishes. No duplicate brief was dispatched by Codex and no retry is authorized merely because the stub exists.
- Code recovered independently from an initial Bash shell/path failure by using its PowerShell tool. No shell installation or mailbox script was changed by Codex.

Follow-up: a mailbox startup guard should verify exclusive ownership before running crash recovery. This is a separate runner-maintenance concern, not m51 capture implementation. Do not change shared/canonical watcher scripts during this brief. Reconcile the actual final report, active brief, verdict, and session adoption after this run completes.

## Reconciled after completion

The log also records another watcher, PID 14448, starting at 14:56:19 and running the same erroneous recovery while PID 33896's Code session was still working. Both watchers were present after Code completed. The original runner nevertheless published the actual final report at 15:09:26: session `<uuid>`, exit 0, declared NEEDS-DECISION, documentation commit `b532ddd97910c2357bc35e5bfe342ad21d58f11c`. Codex read that report in full. The original brief is archived as `processed/20260906-150926_078-01-m51-preflight-and-predictions.md`; the queue and active-brief set are empty; config adopted the correct 078 session.

After verifying the queue was empty, the active brief was gone, and the process command line still identified Codex's own watcher, Codex stopped only its now-idle watcher PID 33896. The independently started watcher PID 14448 remains. No duplicate Code run or follow-up brief was dispatched. No runner scripts were changed. The startup-ownership defect remains a named maintenance concern; do not restart another watcher while this one owns the mailbox.
