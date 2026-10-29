# 094-26c — native fixture authoring and Config STOP

Smoke passed all 51 bindings; EA_Core exited cleanly but created a bound Saved/Config/WorldState file. The kit quarantined both Core packages and stopped before EA_Travel.

FE1 removed only override_anti_aliasing_method and anti_aliasing_method from the 4.25 post-process recipe item. The six exposure/motion-blur values remain. The recipe document, A5 fixture manifest and dated prediction addendum are bound by tooling commit 9f5ce85. FE2 excludes only access-denied OS Windows event logs in the exact winevt/Logs folder; readable logs remain scanned. FD2/FD3 and the FC2 launcher remain unchanged.

The engine Python 2.7.14 harness passed Smoke, Core and Travel call sequences with no asset saves and both expected mutants failing. FE/FC/EY/session/gate/environment suites passed (14/49/46/57/50 cases and 19 environment self-tests). The first FE run exposed a generator-versus-sized-list mistake in the new test; its second attempt passed after that test-only correction. No dry runs were performed (FD5).

Two authorized editor sessions exited normally: Smoke (29.311 s) and EA_Core (12.804 s), both code 0. Core postflight raised the Config STOP. There was no retry and no Travel session.

UE 4.25 FActorFolders::OnWorldSaved writes Saved/Config/WorldState on map save through a direct file writer. The filename CRC matches /Game/EuropaPhaseA/EA_Core. This path is outside FC5's four admitted classes; EN5 expressly excludes protected-state differences. The new file and Saved/Backup map are preserved, not deleted or retroactively admitted. The recipe outputs remain quarantined, so A6 is incomplete.

Smoke recorded one skipped cleanup and one probe_stop; Core recorded two skipped cleanup rows and one probe_stop. Neither entered PIE. Both admitted stock ValidatePlatforms UBT children with recorded argv; no compiler/linker/UHT child or build-output hash change occurred. All ten owned processes exited. Both session log pairs have no upload evidence.

All 1,458 pre-existing Content files, existing config files, descriptors, .uproject and 110 binary-manifest artifacts retain their hashes. The new 50-byte Saved/Config/WorldState/1202994220.json is a protected Config difference and remains in place. A 7,549-byte Saved/Backup map also remains. The 23 FC5 managed-file deltas include one CachedAssetRegistry hash change; nine zero-byte Intermediate config files changed mtime only. Marker 31124 is unchanged.

EA8 entry/exit against P9 v2 passed independently of this newly created host Config path. The launch-second canary scan, EU1 audit and ledger are indexed. Runtime work ended at the Core STOP; no output was restored from quarantine and no new policy exception was invented. A6 is incomplete. No push or tags.
