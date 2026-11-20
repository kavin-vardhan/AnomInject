# REPORT for 094-43-europa-fz2-private-pass.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 094-43-europa-fz2-private-pass.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-11-04 01:51:36 | finished: 2026-11-04 02:22:09 | duration: 31 min | exit code: 0
context at last turn: 659218 tokens (66% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\094-43-europa-fz2-private-pass.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

**For the owner:**
- **Outline leak on the test level:** not fixed yet. Codex stopped after 7 minutes at its first entry check, so it never ran the depth-check proof or the build.
- **Paris host on D3D11 and D3D12:** not re-tested. Nothing was built or launched, and the host's files are byte-for-byte unchanged.
- **Push and office card:** nothing was pushed and no new office card was written. The branch is still `75ca580` locally and `d1cdc7b` on origin.
- **Phase B plan:** written and ready (`_reviews\094-43-phaseB-plan.md`).

**Why it stopped (Stage A entry, not a gate result):**
- GC1 called for the first-ever end-to-end run of the full `owner_p9.py main()`. It fails at `owner_p9.py:72`, which requires every ref except local feat to equal the P9 baseline's 71 refs.
- Today there are 4 extra refs, each made by a recorded job:
  - the 094-40 backup ref (`08fe652`);
  - `origin/feat/europa-port` (`d1cdc7b`, from the 094-40 push);
  - spike 42c (`740220c`);
  - spike 43a (`c1193bd`).
- My own recompute agrees: 71 equal, 0 missing, 0 changed. The checker is unchanged (`hash-object` matches its `75ca580` blob).
- So the gate's baseline has been out of date since 094-40; this job changed nothing. Codex stopped as required and touched neither the checker nor the baseline.
- **A probe of the rest of P9:**
  - **What it was:** I ran `main()` with only that ref line relaxed to "the baseline refs plus exactly these four". It wrote only to my review folder. This is review evidence, not the GC1 gate.
  - **Result:** it passed every remaining step in 371 s. The inventory of 382,970 files shows 0 differences, 22,628 binary and config files (100.7 GB) rehash equal, the source manifests and toolchains match, and fsck and the ledger database check are clean.
  - **Meaning:** the four refs are the only thing blocking P9.
- **Stage A and Stage B:** neither started. There are no A-P2–A-P5 or H1–H5 numbers.
- **Stage C:** I ran no scan and made no push, because no gate ran.
- **The resume:** it worked first time, so no fresh thread was needed.

**Phase B plan, headline:**
- **Already there:** the 5.1 release code for all three anomalies is on `feat/europa-port`, switched off for 4.25 at compile time.
- **Order of work:**
  0. 094-43 must go green first.
  1. Author the two materials again in 4.25.
  2. missing_texture, then corrupted_texture.
  3. Their gates on EA_Core and the outline fixture, then the Paris host on D3D11 and D3D12.
  4. camera_clipping last.
- **Top risks:**
  - The two 5.1 material files cannot be loaded by 4.25 (newer file version, checked against the 4.25 loader). Any missing material usage flag makes the swap render grey in game while the label still says it is applied.
  - camera_clipping is never fired by the auto pool. On 4.25 it needs a whole-session hold, a whole-frame label path (today's 4.25 labelling needs a target object) and a test camera position near a wall; 5.1's version of that test device is not built on 4.25.
  - The fixture's outline is the same magenta as the pink, so the leak check must compare against a same-seed run without the outline.
  - EA_Core has no skeletal mesh to test against.
  - The checkered texture may barely show on EA_Core's over-bright frames.

**Safety:** none.
- **Crashes and uploads:** no crash and no upload sign.
- **Paris host:** Content, Config, descriptor, `.uproject` and binaries are hash-equal before and after.
- **Fixture:** the seven assets and the preserved `Engine.ini` are hash-equal.
- **Git:** `m51` passed at start, prelaunch, exit and after the probe. `master`, the release branch and the tags are unchanged; both spike branches are still local and unmerged.
- **Deletions:** nothing deleted.
- **My slip:** my two polls were 6.7 minutes apart instead of 2–3. The no-launch finding doesn't depend on them: 0 binary changes and no new `Saved` files.

**Decisions chat must make:**
1. How P9 handles the four refs: admit them by name and SHA in the ref check, or regenerate the baseline once. Then re-issue 094-43; the same thread resumes cleanly.
2. Phase B:
   - **D1:** port from the release line (`d7eee97`), not `master`/`53bf725`. Both are older than the release the client runs.
   - **D2:** where the two new 4.25 materials live, and whether authoring them in the Paris plugin copy is allowed. Today any Paris Content change is a stop.
   - **D3:** where to add a skeletal test object: EA_Core or the outline fixture.
   - **D4:** what "fires at least 3 times" means for camera_clipping, which is held as one event for the whole session.
   - **D5:** keep the new anomalies as opt-in routes, or match 5.1's default pool.

Files are in `D:\IntrusiveAnomalies\_reviews\`:
- 094-43-code-review.md
- 094-43-phaseB-plan.md
- 094-43-codex-report.md

Receipts are in `code-receipts-094-43\`.
api-equivalent cost (not billed on Max): $20.45 | turns: 138 | running total across all briefs: $5,600.34
