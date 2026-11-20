# EFFORT: medium

SESSION 069, BRIEF 12 — DOCS ONLY: retire the "Deimos" codename. Headless; codename discipline; master tip cdd0c4b (verify HEAD == origin/master before touching anything; no source changes, no builds).

OWNER CORRECTION (verbatim intent): "Bates and Deimos are one and the same" — Deimos was a SECOND codename for the Bates host. There are exactly TWO office hosts: Concorde and Bates. "Deimos" is retired as of 2026-09-03. Nothing else about either host changes. Do NOT record or guess any real title/studio/fork name; do not add any mapping beyond "Deimos == Bates".

TASK — tracked, LIVING docs only:
 1. CLAUDE.md
    - Invariants line (~:4403) `(Concorde, Bates, Deimos)` -> `(Concorde, Bates)` and append one sentence: "Deimos was a second codename for Bates, retired 2026-09-03; historical journals/predictions that say Deimos mean Bates."
    - ~:649 `A Bates/Deimos run` -> `A Bates run`; ~:652 `FOR BATES / DEIMOS` -> `FOR BATES`; ~:688 `a Bates/Deimos` -> `a Bates`.
    - Any other CLAUDE.md hit from `grep -n -i deimos CLAUDE.md` — same treatment.
 2. docs/office-rdp-card.md A-9 (~:213-215): DELETE the "Deimos, if reachable…" bullet entirely (it was stale — there is no third host and the "Being 5.2+" claim is withdrawn with it). Do not restate it anywhere.
 3. docs/gotchas.md: append G222 — "Deimos" was a duplicate codename for Bates (two office hosts only: Concorde, Bates); retired 2026-09-03; read "Deimos" in historical journals/predictions as Bates; never reintroduce.
 4. docs/setup-runbook.md, docs/PRE-DELIVERY-CHECKLIST.md, docs/client-readme.md, README(s), docs/architecture.md, docs/onboarding.md: grep and fix any living-doc hit the same way.
 DO NOT EDIT: docs/sessions/* and docs/predictions/* (append-only history — G222 covers them), and the untracked docs/CHAT-HANDOFF-*.md files (chat-owned; chat fixes those itself).

 5. Journal 069 §6 (short): the correction, what changed, and the post-edit `grep -rli deimos` list, which should contain ONLY docs/sessions/* and docs/predictions/* entries (list any other survivor as a defect and fix it).
 6. One commit: `docs: retire the Deimos codename - two office hosts only (Concorde, Bates), G222`. Push. Report: tip SHA, HEAD == origin/master, the survivor list, NEEDS-DECISION (expected none).
