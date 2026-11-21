# EFFORT: medium

SESSION 069, BRIEF 12 — DOCS ONLY: retire a duplicate codename for Bates. Headless; codename discipline; master tip cdd0c4b (verify HEAD == origin/master before touching anything; no source changes, no builds).

OWNER CORRECTION (2026-09-03): a second codename that some files used for the Bates host is retired. There are exactly TWO office hosts: Concorde and Bates. Nothing else about either host changes. Do NOT record or guess any real title/studio/fork name; do not add any mapping between names.

TASK — tracked, LIVING docs only:
 1. CLAUDE.md
    - Invariants line (~:4403) drop the retired name from the host list, leaving `(Concorde, Bates)`, and append one sentence saying that the second codename for Bates is retired as of 2026-09-03 and that historical journals and predictions using it mean Bates.
    - ~:649, ~:652 and ~:688: three places that named Bates under both names now name Bates once.
    - Any other CLAUDE.md hit for the retired name in CLAUDE.md — same treatment.
 2. docs/office-rdp-card.md A-9 (~:213-215): DELETE the bullet about a third host "if reachable…" entirely (it was stale — there is no third host and the "Being 5.2+" claim is withdrawn with it). Do not restate it anywhere.
 3. docs/gotchas.md: append G222 — a duplicate codename for Bates is retired as of 2026-09-03 (two office hosts only: Concorde, Bates); read it as Bates in historical journals and predictions; never reintroduce it.
 4. docs/setup-runbook.md, docs/PRE-DELIVERY-CHECKLIST.md, docs/client-readme.md, README(s), docs/architecture.md, docs/onboarding.md: grep and fix any living-doc hit the same way.
 DO NOT EDIT: docs/sessions/* and docs/predictions/* (append-only history — G222 covers them), and the untracked docs/CHAT-HANDOFF-*.md files (chat-owned; chat fixes those itself).

 5. Journal 069 §6 (short): the correction, what changed, and the post-edit grep list for the retired name, which should contain ONLY docs/sessions/* and docs/predictions/* entries (list any other survivor as a defect and fix it).
 6. One commit retiring the duplicate codename (two office hosts only: Concorde, Bates; G222). Push. Report: tip SHA, HEAD == origin/master, the survivor list, NEEDS-DECISION (expected none).
