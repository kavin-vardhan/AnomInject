# 095-03b — Chat ruling: E: is solved; the bench waits for the owner's apps; code work continues lightly

**Date:** 2026-11-13, about 14:55 IST · **Author:** chat Claude (orchestrator)
**Inputs:** mailbox reports 095-03 (NEEDS-DECISION, 12 min) and 095-03b (NEEDS-DECISION, 94 min, $14.77 API-equivalent).

## What happened
- **E: is clean.** Two owner-approved passes deleted 73.3 GiB of old Europa home evidence and outline-fixture receipts. E: is at 93 GiB free. The current Europa package was verified before and after (34 hashes; starts and quits cleanly).
- **Editor target rebuilt at `master`:** 0 errors; 2 warnings, both in the host's own file, none in the plugin.
- **Game target stopped by Code at step 392 of 764,** by PID, its own processes only. The owner's Blender (about 21 GB), ComfyUI and Resolve had taken free memory to 0–0.4 GiB and the build was crawling and slowing his work. The game exe is still the 24 Sep one and does not match `master`.
- **No baseline leg ran. No docs commit.** `master` = `ebc34ed`, unchanged.
- **D:'s free space swings 15–43 GiB** with the Windows page file while the owner's apps run.

## Rulings
- **E1. Code's stop was right.** The owner's own work comes before heavy Code jobs (his standing rule). Nothing of his was touched.
- **E2. The bench is closed until the owner says the PC is free.** That covers the Game build, the cook, and every editor or game launch and capture leg. Chat does not release a heavy brief before his word.
- **E3. The separate baseline legs B1 and B2 are dropped.** The plan already holds the "before" evidence: a banked session reads `ticks_per_captured_frame` 1.344 with a `frame_index` jump of 3 at every edge. The same readings come again, properly, as m56's can-fails G4(a) (the release build must fail the continuity check) and G4(d) (the mask-wait defect reproduced before the fix). Running them twice buys nothing.
- **E4. Code work continues, lightly.** Implementation briefs write code, compile only the plugin's Editor modules (2 parallel actions, only with 4 GiB or more of memory free), and run pure selftests. They push a branch only after a clean Editor compile. No Game build, no launches.
- **E5. D:'s floor is not a gate for light briefs,** which write only source files there. For heavy briefs it is checked at the moment of each large write, not once at the start, because the page file moves it within minutes. The page file itself is the owner's to change.
- **E6. The 31 "kept, unsure" files (33 MB) stay.** Not worth a brief.
- **E7. Host and fixture:** nothing needed aligning. The primary host's config hash equals the release-proof host's. The proof host's CaptureBench revision is unrecorded; the primary's is its last commit. Accepted as the bench of record from here on.

## Order of work now
1. **095-04 (xhigh, light):** m58 code, Editor compile, selftest.
2. **095-05 (xhigh, light):** m56 code (commits 1 to 5), Editor compile, offline tests.
3. **095-06 (xhigh, side session, read-only):** independent review of both branches.
4. **When the owner frees the PC, 095-07 (high, heavy):** Game build, one cook, every gate for both units including the settle-exactness table.
5. Then as in 095-02's ruling: the K default, docs, merge, dashboard brief and re-bundle, the 4.25 forward-port, one office pull.
