# 090-10f2 — Ruling (ACCEPTED): every Job A criterion is met; the three decisions are ruled below; the mailbox era closes; m53 `b8cd3cd` is the base for the proxy blur

Date: 2026-10-02 ~16:45 IST · Author: Claude Code Desktop (orchestrator/reviewer, per `_reviews\090-HANDOVER-code-desktop-orchestrator.md`) · Report `_mailbox\to_chat\090-10f2-finish-e-mask-uniform-bscope-cooked.report.md` (Code, Opus 5.5 xhigh, 380 min, 466 turns, declared NEEDS-DECISION) · Heads: m53 `b8cd3cd` (code = `2dbf6f2`, exe `36C8E30A`), fix `12a6db8` (exe `822FA156`)

**Disposition: ACCEPTED.** The declared NEEDS-DECISION covers three scope questions, all ruled below; none blocks.
Chat review NOT REQUIRED (chat stopped orchestrating at 3a-41). The next independent check is Codex Job E (read-only review of 090-10b2 → 090-10f2).

## What I re-checked myself (not taken from the report)
| check | result |
|---|---|
| origin heads (`git ls-remote`) | m53 `b8cd3cd`, fix `12a6db8`, master `b5f15a3` unchanged; local m51 `53bf725` unchanged |
| `2dbf6f2..b8cd3cd` | docs only (CLAUDE.md, architecture, client-readme, gotchas, journal): the pushed head's code is the code every confirmation leg ran |
| source diffs read in full: `6f9ada2` (Part M), `4dfce28` (bbox scope), `a6fa73e` (G449 lever), `3e47fdb`/`2dbf6f2` (texture_uniform pure rules + tree wiring + census line) | correct and narrowly scoped; findings R-1, R-2 below |
| archives re-hashed vs their manifests | `m53-0910f2-36C8E30A`, `m53-0910f2-0A99BA3C`, `m52fix-0910f2-822FA156`, `m53-0910e-EBEED6F4`, `m52fix-0910e-68BA4D9A`: 6/6 each |
| lever audit, re-run on the archived binaries | m53 exe 52/52 PASS, m53 editor DLLs 52/52 PASS, selftest 12 cases OK; fix exe 34/34 PASS |
| auto-pool evidence (`_r910f2\logs\d_auto_*.txt`) | new `0A99BA3C`: 28/28 CORRUPTS (12.4–86.7), 0 of 219 labelled frames without a mask; can-fail `B3103E32` (= `88801bd`): 17 CORRUPTS, **11 NO-VISIBLE-CHANGE** (0.34–3.84) |
| strict include pass 0/0 | taken from the run record (needs a compile; not re-run) |

## Job A acceptance criteria
- **Part 0 pushed:** yes (fix `ba1af33`, m53 `48a63a2`, then onward to the final heads).
- **Part M, 0 labelled frames without a measured mask:** yes. PIE 55/672 → **0/672**; staged old binary 3/320 → fixed **0/936**. Fixed at the cause (each render serves only arms made before its family began rendering), not flagged.
- **`texture_uniform`, tests both ways, every labelled auto-pool event visibly corrupt in PIE, can-fail on `88801bd` (11 invisible):** yes (above). Pure selftest 30/30 with 6 mutants failing.
- **B scope in-engine with the `NoPartScope` can-fail:** yes. It found and fixed a real defect (`bbox_px` covered the whole actor; G454). Mask in the ineligible box 0 vs 11,260 on the can-fail; m26 11,260 vs 22,520; picture change in the ineligible part 0.00.
- **Cooked-bias staged proof, or a precise blocker:** done. The cooks ran; **dbghelp did not block** (the 09-30 blocker is gone at least for cook/stage).
- **Strict 0/0, lever audit, archives re-hashed:** yes (above).
- **m53 head pushed:** `b8cd3cd`.

## Rulings on the three decisions
1. **Item 3 can-fail scope: ACCEPTED as reported.** A cook bakes per-texture and texture-group bias into the cooked mip chain (the top mip is stripped), so a packaged game has no runtime bias left for any build to refuse; both binaries correctly handle such content (7/7, identity exact, edges 7/7). The question that matters for the office, refusal versus handling of *runtime* per-texture/group bias, lives in the editor/PIE, and its can-fail is 090-10c's (`0a69ea3` refuses V1–V6 in PIE). Cinematic mips remain a runtime property when cooked, and its staged can-fail refused as predicted. Record G455 as the reason.
2. **`texture_uniform:pending` costs a fire: ACCEPTED as is.** A pending measurement means we cannot yet show the texture can display the effect, so refusing is rule 1 (label/picture), failing safe, with no wrong label. **Re-measure on a cache miss is a yield follow-up (LOW)**, built only if office yield shows it matters.
3. **Identity on a still-streaming texture (first event 12.21 vs null ≈ 0.34): ACCEPTED as a bench-only note.** Harnesses must read the identity control only after the texture is fully streamed (or discard its first event when a mip is pending). No product change.

## My own review findings (for the record; neither blocks)
- **R-1 (residual label/picture risk; a question for Codex Job E and the Saturday office look).** `texture_uniform` detects (near-)constant textures only: raw per-channel ranges ≤ 2/255 on the resident top mip. It does not consider which channels the material actually samples, nor whether low-contrast variation makes the corruption perceptible. So a texture that varies only in an unused channel (for example alpha, or a packed-map channel the material ignores), or only faintly, can still give a labelled, hard-to-see event. Not observed at home (28/28 visible). Field check: the owner's office look (does every fired uv/normal visibly corrupt?) plus the kit READ BACK. If it appears, the next fix measures the visible change (e.g. the m26-style pixel delta at the bind frame) rather than texture statistics.
- **R-2 (assumption to verify in Job E).** The Part M fix assumes a frame's capture arms are all made on the game thread before that frame's `BeginRenderViewFamily`. The evidence supports this (most arms served by their own frame's render: 29 holds in a PIE leg, 2 in staged, `familiesWithoutBound` 0). Job E should confirm it on multi-family frames (PIE editor viewport plus game viewport, scene captures) and when a frame's render is skipped.

## Consequences
- **The mailbox era closes now:** `D:\IntrusiveAnomalies\_mailbox\STOP` is created (watcher idle at 16:24; its documented stop). From here, one implementer: Codex through the implementer Relay.
- **The proxy blur (Job C) branches from m53 `b8cd3cd`.**
- **Blurry default:** stays available-but-OFF for Monday unless the proxy blur is proven by Sunday noon (owner decision 090-12).
- **Disk:** E: is about 20 GB free (floor 50 GB); new work goes to `D:\IA_BankOverflow\`. The owner was told; nothing is deleted.
