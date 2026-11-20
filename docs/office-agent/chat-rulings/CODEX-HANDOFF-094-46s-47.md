# Handoff to Codex (owner-approved, 2026-11-05 ~12:30 IST): finish camera_clipping (094-46s), then run the dashboard and packaged-build job (094-47)

**Go-ahead:** the owner has approved Codex to orchestrate the rest of today's Europa work while chat is unavailable (the owner's usage limit). Follow the existing mailbox workflow and the standing rules below. Don't loosen any gate. If a ruling is needed that these notes don't cover, stop and leave it for chat or the owner. The owner is back at about 20:30 IST.

## 1. In flight: 094-46s (camera_clipping finish)
- **Staged:** 11:41 IST. Brief `_mailbox\to_code\…094-46s-europa-cameraclip-finish.md`; ruling `_reviews\094-46r-ruling.md` (GP1–GP4).
- **What it does:**
  - D4 under GP2: an unlabelled frame matches if no pixel exceeds the larger floor's max **level**; the count is information only;
  - the D3D12 1279×719 leg;
  - the default-only key diff against `c5d0919`;
  - then Code pushes and writes `_reviews\094-46s-office-card-europa.md`.
- **The report:** `_mailbox\to_chat\094-46s-*.report.md`.
- **If GREEN:** camera_clipping is DONE and pushed. Note the new tip (`git ls-remote origin feat/europa-port`).
- **If NEEDS-DECISION:** apply only what GP1–GP4 already decide. Anything new waits for chat or the owner. Don't stage 094-47 on top of an unpushed camera_clipping unless the report says camera_clipping's commits stand alone and compile; then 094-47 may start from the local tip, unpushed.

## 2. Next: 094-47 (dashboard + packaged Development build), the owner's top priority
- **The design:** `_reviews\094-47-codex-controlserver-plan.md`, your own read-only plan, approved with bounds in `_reviews\094-47-plan-ruling.md` (GQ1–GQ5).
- **The brief, ready:** `_reviews\094-47-europa-dashboard-packaged.QUEUED.md` (also in the claude.ai project as `claude/094-47-europa-dashboard-packaged.QUEUED.md`).
  1. Replace both `<46s TIP>` placeholders with the pushed tip from 094-46s.
  2. If 094-46s was NEEDS-DECISION, add a first line `# ACK-VERDICT: 094-46s NEEDS-DECISION is resolved by <ruling>`. The watcher holds any brief that follows a NEEDS-DECISION or RED report without one.
  3. Stage it as `_mailbox\to_code\094-47-europa-dashboard-packaged.md.part`, check its sha256, then `mv -n` to `.md`.
  4. The watcher runs it: Code (Opus 5.5, xhigh) resumes your thread `01a14334`.
- **Its stages:**
  - **A, the code:** transport, C5 bridge, the office include-order fix, the 4.25 and 5.1 compiles, and an editor `-game` protocol smoke.
  - **B, the package:** a packaged Win64 Development build of the Paris host; console C5 from the packaged exe; the dashboard `feat/europa-425` change (LOD corruption shown) and desktop build.
  - **C, equivalence:** console against dashboard C5 on the packaged exe, on D3D11 and D3D12; the early stop, reconnect, and a second capture.
  - **Then:** Code scans and pushes `feat/europa-port` and the dashboard `feat/europa-425` (never the dashboard's `master`), and writes `_reviews\094-47-office-card-europa-dashboard.md`.
- **Limits accepted (FUTURE):** no 4.25 preview stream in the dashboard; no arbitrary durations, JPEG, resizing or targeted captures; Shipping excluded.

## 3. State at handoff
- **origin `feat/europa-port` = `c5d0919`:** FZ2 (the outline-safe private label pass) plus the four defaults (blinking, lod_corruption, missing_texture, corrupted_texture).
- **Office re-test of `c5d0919` (11-05): PASS.**
  - no outlines, 0 tagged, 0 polluted;
  - fires 15/19/22/13;
  - 0 dropped;
  - reads in `_bates_reads\2026-11-05-europa-094-45r-office-capture-reads.md`.
- **The current office card:** `_reviews\094-45r-office-card-europa.md` (also in the project), until 094-46s or 094-47 writes a newer one.
- **Local camera_clipping work:** on top of `c5d0919`, being finished by 094-46s.

## 4. Open items for the owner or chat (don't decide these)
- **lod_popping:** port it? Should it be in the default set? (The client's 5.1 release has it in the default pool.)
- **FUTURE:**
  - widen label coverage (the office read showed 50 `unavailable`: translucent, no scene proxy, other component types);
  - D3D12 render-thread cost (3.8 ms per frame at the office);
  - the 5.1 dashboard runtime regression before any 5.1 merge;
  - update Node.js after the port (Node2Shell CVE-2026-111497).
- **Office errand:** type home the `HOST-PP CUSTOM-DEPTH READERS` line for Concorde and Bates.

## 5. Standing rules (unchanged)
- **The HARD RULE:** office data never comes home. Only the owner's typed or photographed reads, numbers only, go into `_bates_reads`.
- **Codenames only** (Europa, Concorde, Bates). Never a real title.
- **Never touch:** `m51` (its working tree or index), `master`, `release/2026-10-05`, tags, the dashboard's `master`, `E:\<personal-folder>`, `E:\UEProjects\LyraStarterGame`, `E:\Unreal Projects\StylizedParisStreet`.
- **Never kill by name;** no machine settings; delete only by receipt.
- **One brief in flight at a time.** Gates are never loosened to pass.
- **Pushes:** Code pushes only after its own review and a clean scan. Never tags or force.
