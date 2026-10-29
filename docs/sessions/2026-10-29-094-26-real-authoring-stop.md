# 094-26r real authoring STOP — 2026-10-29

RED. The unchanged 09425 kit ran once after the fresh FA8 entry proof and FA5 idle pre-check. EA_Core used slot 01; EA_Travel never started. No package was authored and no retry or fix was attempted.

The BAT ran 18:30:21–19:00:54 IST (exit 2). Its editor ran 18:58:33–18:59:17 IST, 43.426 seconds, exit 0. The probe emitted one skipped engine-exit cleanup and exactly one final probe_stop; no PIE occurred. The Python recipe failed at author_425_fixture.py:9 because os was None at os.path access. Runtime.log:666–671 is our authoring-tooling traceback; the later missing authoring.json triggered the kit STOP. The cause was not reproduced or repaired.

Two independent hard failures remain. The existing 112-byte Saved/Config/CrashReportClient/UE4CC-Windows-57F07A004884350F454788B699A5B40C/CrashReportClient.ini disappeared. GenericPlatformCrashContext.cpp:363–397 and 909–932 document stock old-config purging, consistent with this loss; EN5 does not excuse protected-state changes. CrashClient.log:86 reports a sent shutdown/session summary; line 93 records a data-router POST with UploadType=eteventstream. The EU4 crash-upload switch was present. No crash-payload upload was found, but the required no-upload proof fails. No remote acknowledgement is claimed.

All 1,458 existing Content files, descriptors, .uproject, 110 bound artifacts and three manifest hashes are unchanged. The other 41 bound config files are unchanged. The 31124 marker is unchanged and the sole final EditorRuns entry. EX3 quarantined zero partial outputs and retained the spent launch lock. The kit and code remain unchanged, and both 5.1 hosts remain frozen.

One foreign C: floor breach paused preparation for 1,320 seconds; it resumed after three spaced recovery polls. No edit occurred in that pause. Foreign launcher/owner activity was observed and left alone. The ledger's original foreign-root STOP is preserved; identical metadata supports a foreign rename from E:/codex-home to E:/_DELETE-ME-codex-copy-2026-10-29 (writer PID unknown). FA6 permitted report-only closeout, with no foreign file contents opened.

Fresh P9 v2 entry and full exit passed. This does not erase the separately bound Paris config failure. The launch-second canary scan passed; the full EK1 scan was omitted as explicitly required by FA6 for this run. The EU1 audit records the initial read-only deviation, wrapper refusals before any child, and no direct write-capable command. Exact final counts are in the run report.

No EN5 intent exception was used. A bare UBT child was observed in the editor tree; DesktopPlatformBase.cpp:835–909 documents a stock target-discovery query path, but the child command line was not retained, so this is a source-supported interpretation rather than a proved argv. No compiler or linker child was recorded and the bound binaries did not change. No build was initiated by the agent.

The receipts index is docs/europa/09426-authoring-receipts.json. Full evidence is D:/IA_Europa/Receipts/09426r and the spent EA_Core slot under Receipts/09425. The run-folder report is the final closeout. This commit changes documentation only, using W4's actual dates. No push, tags, new archives, source change, settings change or re-arm.
