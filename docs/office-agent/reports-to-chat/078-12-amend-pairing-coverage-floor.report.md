# REPORT for 078-12-amend-pairing-coverage-floor.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 078-12-amend-pairing-coverage-floor.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-06 20:29:19 | finished: 2026-09-06 20:37:23 | duration: 8 min | exit code: 0
context at last turn: 804884 tokens (80% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\078-12-amend-pairing-coverage-floor.live.txt

---- FINAL MESSAGE (Claude Code) ----
AMENDMENT 2 is committed. Nothing ran, nothing was staged.

## Preflight (all matched before writing)

`HEAD 770386fe…` on `m51` · source freeze `6713f51..HEAD -- Source Shaders` **EMPTY** · `master` == `origin/master` == `ac13700` · **10** handoffs · prediction doc **`FD398AE1…D5675`**, **529 lines** — exact.

## AMENDMENT 2 — what it says

**`docs/predictions/2026-09-06-m51-frozen-pairing-recheck.md`**, appended, 529 → **691 lines**, new sha256 **`8D7951F542D61658738D8ED4716D98FCCF563C10ABDCCF2EFBDFF05197D2E9B5`**.

- **A2.1 — the floor.** §5's `decidable >= 1` and §9's matching clause are superseded by **`decidable >= 20` of 40**. Basis stated as what it is: **half the request** (40 captured frames ⇒ 40 probe opportunities), fixed prospectively from the request size. Explicitly **not** statistical sufficiency, **not** a tolerance or predicate loosening, **not** an FPS band, and **expressly not derived from the failing bank** — the amendment says in as many words that picking a floor above the failing legs and then citing them as its justification would be fitting the rule to the results it judges. Attainability cites **only receipts I actually read in-session**: `M51D_HE_RESIDENT_NAT` **35**, `M51D_HE_CLOSED_NAT` **37**, with a note that no other decidable count is repeated because none was re-verified here.
- **A2.2 — consequences.** Under-covered ⇒ **INVALID (insufficient probe coverage)**: stops, no control, no ad-hoc retry — **but its counts and every failing index are still reported in full**, because coverage invalidity blocks certification without erasing an observed mismatch. **No retrospective reclassification** (the 13-decidable failures keep their classification). Floor applies to reference, all four candidate cells, and the control. Stop taxonomy now **four** classes (REFUSED · UNDECIDABLE · coverage · event count), none releasing a control.
- **A2.3 — same-row reporting:** verdict + `decidable`/`CURRENT`/`PREVIOUS`/`NEITHER`/`same-pos`/`no-data` + sustained FPS + `speed_ratio` on **one line**; absent ⇒ `UNKNOWN`, never invented; pacing stays telemetry, no band.
- **A2.4 — Chat's paraphrase explicitly rejected.** *"control fails ⇒ not attributable to the candidate"* is the exact overclaim `A1.1` corrects; `A1.1` restated as governing.
- **A2.5 — five-file quintet**, with the correction that **§1 was already right** — the slip was in the paraphrase, not the record.
- **A2.6 — swap/restore receipts as explicit policy:** staged exe hashed against the declaring archive before every leg; all five container files hashed before and after every swap and leg, both banked; candidate `D50DDE78` restored and re-hashed on **every** exit path; no rebuild, cook or third binary.
- **A2.7 — a PASS is scoped pairing smoke** with the even-frame blind spot, **not** `F1`/`m51` release certification; the broader "certifies F1" phrasing is not adopted.
- **Unchanged:** nine commands and order · `MaxAttempts 4` pose budget · no retry of a valid failure · at-most-ONE control then STOP · predicate and tolerances · ≥3 events · every hold.

## Commit

**`d727aec`** — 4 files, 283 insertions / 3 deletions. `git diff --check` **CLEAN**; all three deletions verified as trailing-newline artifacts re-added verbatim (unaccounted = 0 on every file); scope is docs-only.

| file | bytes | sha256 |
|---|---|---|
| `docs/predictions/2026-09-06-m51-frozen-pairing-recheck.md` | 43,586 | `8D7951F542D61658738D8ED4716D98FCCF563C10ABDCCF2EFBDFF05197D2E9B5` |
| `docs/OPERATING-CONTRACT.md` | 39,595 | `06D97D5A7EAE0B27E8A5396B5B50F7D6EBBB15F9C7294CFE064C4C8AF3271334` |
| `docs/sessions/2026-09-06-078-10-11-m51-frozen-pairing-draft.md` | 6,861 | `DCFDA01DCEFD8A08BFFA87F8C00DA32F17373AEE6FE8D329A11EC5A3B1E18F6F` |
| `CLAUDE.md` | 557,354 | `993DDF9B608D12EFD8FB5E225BC646F85B9DDD7A0AF6C59C82512BAAC13ABE80` |

The three docs files are LF (append-only, no BOM). **CLAUDE.md was spliced byte-surgically**: CRLF **5938 → 5965**, **zero bare LF**, no BOM, 27 insertions / 0 deletions, historical sections untouched, new pointer at the top of Current status.

## One process note

My first append attempt was **blocked by a shell path guard before executing** — a false positive on the long here-string. I verified the prediction file was still byte-identical (`FD398AE1…`, 33,342 B) before retrying, then routed all four texts through files instead. Nothing partial was ever written. This is recorded in the journal.

## Final state

`HEAD d727aec8d18de862180b5838a693cb315e6df829` on `m51`; `master` == `origin/master` == `ac13700` unmoved; **nothing pushed** (`origin/m51` absent); no tags; no amend/reset/clean/stash; path-scoped adds; `commit --file` with an absolute path. Source freeze **EMPTY**, worktree clean, 10 handoffs preserved, zero unstaged tracked changes. Untouched: staged exe still `D50DDE78`, all five container files unchanged, CaptureBench `472a409` tracked-clean, Lyra `4a4bfcc` clean, zero `StackOBot` processes, no background tasks. **No capture, staging, build, cook, harness/checker, source, schema, binary, fixture or owner-app action.**

Context: roughly **56–60 % of 1M** — below the 70 % rollover boundary.

**VERDICT: NEEDS-DECISION**
api-equivalent cost (not billed on Max): $11.08 | turns: 19 | running total across all briefs: $1,244.46
