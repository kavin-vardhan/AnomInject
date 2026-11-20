094-42r — ADDENDUM TO 094-42 (chat, 2026-11-03, about 11:10 IST)

094-42 is unchanged and still binding. This addendum says only where to restart.

1. **Why it stopped:** 094-42 waited about 10 h at FJ1's D: admission (14.3 GiB free against the 25 GiB start). The owner then rebooted, and the watcher recovered the brief as interrupted.
   - D: now has about 40 GiB free.
   - No build or launch had run.
2. **Where the branch stands:** `feat/europa-port` is at `4fdfe84`, which holds the FZ3 and FZ4 work from 094-42 step 2 (af0c455…4fdfe84).
   - **First,** re-run step 2's unit tests against `4fdfe84`. Then confirm, by code path, that each FZ3 and FZ4 item in the request is present.
   - **If an item is missing or a test fails,** finish or fix it and commit with W4.
   - **Then continue from step 3:** the Paris host rebuild and the EA_Core smoke, then FZ5, FZ6 and the exit.
3. **Builds:** the FM1 attempt budget is fresh. Nothing was attempted.
4. **The fixture:** `E:\IA_Fixtures\` may already hold partial work from 094-42. Reuse it, or start it over under a new subfolder. Delete nothing.
