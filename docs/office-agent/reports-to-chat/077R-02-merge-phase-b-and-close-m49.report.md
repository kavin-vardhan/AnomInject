# REPORT for 077R-02-merge-phase-b-and-close-m49.md
brief: 077R-02-merge-phase-b-and-close-m49.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-06 13:17:24 | finished: 2026-09-06 13:47:35 | duration: 30 min | exit code: 0
context at last turn: 404184 tokens (40% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\077R-02-merge-phase-b-and-close-m49.live.txt

---- FINAL MESSAGE (Claude Code) ----
Everything verified. `m49` is complete and merged.

---

## 1. Plain-language rundown

1. **`m49` is done and on `master`.** Phase B merged as `4a4bfcc` (`--no-ff`), then a docs commit `ac13700`; both pushed. `m49` = A1 + A2 + Phase B, sitting on top of `m50`. No tag — tags batch at the office.
2. **The merge is provably inert:** `master`'s tree is byte-identical to the gated branch tip's tree, so nothing entered `master` that hadn't been through the 21-leg campaign.
3. **Ruling 1 applied as written:** `F2`'s *reading* stands, its *premise* falls. AMENDMENT 2 records the pixel measurement (hidden 87.4–91.0 vs visible 94.5–97.0) and restates PB-1's scope as **both fixtures** with a different expected value on each.
4. **Ruling 2 applied:** `G258`, plus client wording in `client-readme` §8 and `client-delivery` §4b, §14 in the mechanisms ledger, and the real-host reading in `architecture.md`. The TSR mechanism chase is filed as a future item and **not started**.
5. 🚨 **Ruling 3 was executed by intent, not literally — the file was empty.** The plugin-root `m50_pc7v3.py` is **0 bytes**; the real 4,672-byte tool is **already in `CaptureBench/tools/` and already tracked**. A literal move would have overwritten the working tool with an empty file and logged it as a tidy-up. The stray was deleted; no `chore(tools)` commit was made because nothing moved.
6. 🚨 **Two stale gates found and fixed — both created by the Phase B docs, both would have failed a correct build.** `PRE-DELIVERY-CHECKLIST` said `frames_drawn_unexpected` "reads `0`" and **`client-delivery.md` told the client "it should be `0`"** — Lyra reads 7. The client-facing one would have told a client their dataset was broken when it wasn't.
7. **Smoke passes**, both tick orders, on the `master` build; both build targets exit 0 with zero real warnings (the only "warning/error" grep hit was the literal `-WarningsAsErrors` flag).
8. 🚨 **One thing the brief didn't anticipate, carried as NEEDS-DECISION:** the checkout was on an **unpushed local branch `fix/capture-label-integrity`** (43 files) that overlaps Phase B on six source files.

## 2. Commits, hashes, states

| item | value |
|---|---|
| merge commit | **`4a4bfcc`**, parents `21c1e0d` + `9b27b05` |
| docs commit | **`ac13700`** |
| `master` == `origin/master` | ✅ `ac13700` |
| `m49-phase-b` | `9b27b05`, fully merged (`master..m49-phase-b` empty) |
| tag | **none** (correct) |
| staged exe | **`B8AC65D8`** + Phase B container `67EA1FE0`/`2CEFB8F4`/`E03C6610` + `A16A18A8`/`C70ECDAA`, all hash-verified at the destination |
| archived A-side | `StackOBot.exe.m50-A159EDFD` + `m46-container-m50exe-A159EDFD\` (complete: 5 container files + exe) — swap fully reversible |
| rebuild needed? | **No** — `git diff --name-only 1c3d62c HEAD -- Source/ Shaders/ Config/ *.uplugin` is **empty**; only docs moved |
| fresh build hash | `A9B10CC0` — **deliberately not staged** (G201: same source, different link; `B8AC65D8` is the binary every gate ran on) |
| Lyra worktree | **`4a4bfcc`**, detached, clean, no rebuild (docs-only diff) |
| README correction | `B8AC65D8` is **241,400,320 B**, not the 241,399,296 B recorded — that was the *cooked* exe's size |

## 3. Smoke

| leg | order | attempts | `run_summary` keys | `frames_drawn_unexpected` | anom rows / with `target_drawn_pixels` | PB-1 rows (`px>0`, `drawn==0`) | subset violations |
|---|---|---|---|---|---|---|---|
| `R02S_F_NAT` | native | 1 | **68** ✅ | present, **0** | 43 / **43** | **8** | **0** |
| `R02S_F_SYN` | synth | 1 | **68** ✅ | present, **0** | 43 / **43** | **8** | **0** |

Both targets: **exit 0, zero real warnings**. Readings identical across orders.

## 4. Docs touched

`CLAUDE.md` (status → m49 COMPLETE, NEXT = m51 verbatim) · `docs/predictions/2026-09-04-m49-phase-b.md` (**AMENDMENT 2**) · `docs/gotchas.md` (**G258**) · `docs/invisible-anomaly-mechanisms.md` (**§14**) · `docs/sessions/2026-09-04-077-m49-phase-b.md` (**§8** + 🔻 pointer at §5) · `docs/client-readme.md` §8 · `docs/client-delivery.md` §4b · `docs/PRE-DELIVERY-CHECKLIST.md` (5th Phase B box; stale gate corrected) · `docs/architecture.md` · `_binary_baselines/README.md` (outside the repo).

Two deviations, both journalled: **AMENDMENT 2 restates PB-1's scope inside itself** rather than editing §2.1, because AMENDMENT 1's *"Nothing above this line has been edited"* is load-bearing; and **the journal section landed at §8, not §5**, because §5–§7 already existed and §5 holds the park decision this ruling reverses.

## 5. NEEDS-DECISION

**`fix/capture-label-integrity` (`be16977`) — an unpushed local branch no brief has described, and it was the checkout when this brief opened.** Authored today 12:09, 43 files / +1,415 / −421, *"Fix capture frame pairing and preserve auditable visibility evidence"*, sitting on the pre-merge `21c1e0d`. It overlaps the merge on **`AnomalyCaptureSubsystem.cpp/.h`, `AnomalyLabelWriter.cpp/.h`, `AnomalyMaskSceneViewExtension.cpp`, `AnomalyMaskTypes.h`** and adds a test target plus `tools/verify_integrity.py`.

Its shape says **m51**, which this brief fenced out, so I **did not merge, rebase, build or read it beyond its diffstat**. It is unharmed and recorded in the status block. **It will need conflict resolution against `4a4bfcc`** — the longer it sits, the more it diverges. Whether it belongs to m51 or is separate work is the owner's call. I did verify it was **never staged**: the bench binary was `A159EDFD` exactly as journal 077 §6 left it before I touched anything.
api-equivalent cost (not billed on Max): $22.52 | turns: 100 | running total across all briefs: $1,082.16
