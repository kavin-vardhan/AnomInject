# ACK-VERDICT: 094-42r NEEDS-DECISION is ruled in _reviews\094-42r-ruling.md (GA1-GA6): FZ3/FZ4 and the EA_Core regression are accepted; the fixture's Engine.ini is not a Paris change; the fixture gates and FZ6(b) continue in 094-42c after the owner's renders, with -nowrite, a fresh FM1 budget and a 3 h admission bound. This brief is the read-only BankOverflow delete check only.
# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 094-42d read-only delete check for D:\IA_BankOverflow (owner decided to delete it all)

# 094-42d — Read-only: size `D:\IA_BankOverflow`, prove nothing still depends on it, retire it from the job snapshot checks, and write the owner's delete list

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

**Context:** the owner decided on 11-03 to delete all of `D:\IA_BankOverflow`, accepting that the paused J2 work would be rebuilt from git if it ever resumes. The folder holds:
- the J2 archive `followup-capture-J2-donor-09314-…-OVERFLOW-UNQUALIFIED`;
- `_r9315`;
- `_binary_baselines`;
- `09102-final-binaries.zip`.

**The owner is rendering on this PC meanwhile.** This job is **light and read-only:**
- no Codex, no builds, no launches;
- no deletes, by anyone;
- lowest priority where you can choose it.

## Do
1. **Check `m51` against P9** with read-only git (`GIT_OPTIONAL_LOCKS=0`). Any difference: stop with NEEDS-DECISION.
2. **Exact sizes** (bytes and GiB) of each top-level entry under `D:\IA_BankOverflow`, and the total.
   - Count hardlinked payloads once.
   - Say how much D: free space would rise.
3. **The E: twin:** does the incomplete J2 archive still exist under `D:\IntrusiveAnomalies\_binary_baselines\followup-capture-J2-donor-09314-9781031-D8D30951-DFE6F164-UNQUALIFIED` (junctioned to E:)? If so, give its size and confirm the `INCOMPLETE-ATTEMPT.txt` marker. It joins the list.
4. **Dependencies:** search these for any reference into the listed paths:
   - the plugin repo, every branch (read-only `git grep` on refs);
   - `_relay` scripts and kits;
   - the P9 list and the keep list;
   - the watcher, the mailbox scripts;
   - `_reviews` docs from 094-* onward.

   For each hit, say whether it is a live dependency (something that would run or fail) or only a historic mention.
5. **Retire the snapshot checks:** list exactly where Code's or the kit's start/exit checks snapshot `D:\IA_BankOverflow` (scripts and briefs). Propose the one-line change for each.
   - Do not edit live kit scripts. Chat will rule.
   - The current shared brief text is chat's, so only list it.
6. **Write `_reviews\094-42d-OWNER-DELETE-LIST.md`:**
   - each path, its size, and "safe: yes/no + why";
   - the total;
   - plain steps for the owner to delete in Explorer (Shift+Delete), or "tell chat to do it";
   - a note that nothing may be deleted while a mailbox job is running, until the checks are retired.

**Where you may write:** `D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\code-receipts-094-42d\` and `_reviews\094-42d-*`.

## Hard rules
- **Read-only everywhere else.** Delete nothing, move nothing, kill nothing.
- **No builds, no launches, no Codex, no commits, no pushes.**
- **No security, paging, power or machine-setting changes. No elevation.**
- **Never touch the owner's render processes.**
- **Never print or copy a secret value.**
- **Codenames only.**

## Final message
1. **`VERDICT: GREEN | NEEDS-DECISION | RED`.**
   - **GREEN:** sizes measured, no live dependency, and the list written.
   - **NEEDS-DECISION:** a live dependency was found.
   - **RED:** anything was changed outside your write paths.
2. **A plain summary for the owner** (3 lines): how much space, and whether it's safe.
3. **Sizes, the E: twin, dependencies, the snapshot-check locations with the proposed changes.**
4. **Safety,** or "none".
