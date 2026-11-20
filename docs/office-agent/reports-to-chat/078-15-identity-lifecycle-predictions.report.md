# REPORT for 078-15-identity-lifecycle-predictions.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 078-15-identity-lifecycle-predictions.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-07 07:35:42 | finished: 2026-09-07 07:59:48 | duration: 24 min | exit code: 0
context at last turn: 532244 tokens (53% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\078-15-identity-lifecycle-predictions.live.txt

---- FINAL MESSAGE (Claude Code) ----
Documentation unit complete. Nothing ran; nothing is in flight.

## Deliverable

**`docs/predictions/2026-09-07-m51-identity-lifecycle-geometry.md`**
- SHA-256 `021B26E9C341B4C603F4D7E849E649C1FADB2003C42BD358AD25DBC81F672B3F`
- **664 lines**, 46,971 bytes, LF, no BOM

Journal `docs/sessions/2026-09-07-078-15-m51-identity-lifecycle-predictions.md` — SHA-256 `F16915FA289469A2BE18194BA6B4776C07798E11B5BCEC1EA4EE24D9F9611C80`, 211 lines.

**Commit `a031b469315daa7815efd5a3fc732e1eca556525`** on `m51`, four named paths, `4 files changed, 1084 insertions(+), 0 deletions`:

| file | change |
|---|---|
| `CLAUDE.md` | +92, **insert-only** at `@@ -13,0 +14,92 @@` |
| `docs/OPERATING-CONTRACT.md` | +117, **append-only** at `@@ -589,0 +590,117 @@` |
| `docs/predictions/2026-09-07-…-identity-lifecycle-geometry.md` | +664 (new) |
| `docs/sessions/2026-09-07-078-15-…-predictions.md` | +211 (new) |

Zero deletions on both existing files — no historical line was touched. `git diff --check` clean; CLAUDE.md verified uniformly CRLF (6170/6170, zero lone LF); the other three LF, no BOM.

## Pivotal mechanisms, with citations

**Four request-identifier domains, three sharing one keyspace.** Colour `++CaptureRequestSerial`, no namespace bit (`AnomalyCaptureSubsystem.cpp:4197`); the normal mask measurement's request id **is `GFrameCounter` itself** (`:833`); target mask `(1ull<<61)|serial` (`:903`, `:1159`); census `(1ull<<62)|serial` (`AnomalyCensus.cpp:40`, `:705`). The last three key one `Results` map (`AnomalyMaskSceneViewExtension.h:87`) — one a *time* value, two *counts*. Bit layout confirmed arithmetically against the banked ids.

**Colour binds 1:1; the mask binds N:1 and clones.** `ConsumeWantedForPublish` takes `FamilyFrameNumber` and never compares it — pure FIFO, the parameter reaching only the trace log (`AnomalySveCapturer.cpp:45-73`); family→readback *is* identity-bound via the key ring. The mask SVE takes **all** pending arms into one render (`AnomalyMaskSceneViewExtension.cpp:126-131`) and clones the reduction to every served id (`:521-529`), pixels to the first asker (`:512-520`). Under arm accumulation the picture and the mask for one `session_index` can come from **different renders**, joined only by `session_index` (`:1162`, `:1195`, `:3830`). Corroborated by 078-07's request-ID joins. The mis-bound frame reports unmeasured today only by **ordinal luck** (`:1192-1203` `continue`s before reading the cloned table) — and the m26 consumer has no pixel check at all (`AnomalyMaskMeasure.cpp:404-432`).

**The label bbox uses the live near plane and hardcodes perspective.** `AccumulateFrameEvents` runs in the drain (`:4024`), projecting the **live** actor (`:5504`, `:5525`, `:5528`) against the **frozen** four-scalar view; `BuildProjectionMatrix` hardcodes `Perspective`/`bConstrainAspectRatio=false` (`AnomalyViewport.cpp:173-174`) and never sets `PerspectiveNearClipPlane` (default `-1.0f`) ⇒ the engine uses `GNearClippingPlane` at drain time (`CameraTypes.h`; `CameraStackTypes.cpp:115-121`) — the global `camera_clipping` mutates.

**Serial-reset asymmetry.** Colour ids never repeat (`AnomalyCaptureSubsystem.h:283`); target-mask (`:3003`) and census (`AnomalyCensus.cpp:116`) ids repeat every run — the `M-F5` leak surface.

Plus: `LookupKey` never invalidates (`AnomalySveKeyRing.cpp:132-148`); unresolved snapshots dropped by count (`:4108-4113`) — where 078-05/06's 57 missing indices went; F1's hold moves the entire remaining batch (`:3834-3838`), which is *why* its counter is drain visits; label append unchecked (`AnomalyLabelWriter.cpp:533-540`); writer unbounded (`AnomalyAsyncWriter.cpp:17-27`, `:114-127`); second context not refused (`:2762-2766`).

## Scope verification

HEAD `a031b46`; `master` == `origin/master` == `ac13700`; `6713f51..HEAD -- Source Shaders` **empty**; ten owner handoffs preserved untracked; staged exe re-read as candidate `D50DDE78…4796`, untouched. No capture, checker run, build, cook, staging, swap, source/shader/tool/fixture/schema edit, agent, worktree, merge, push or tag. **No runtime validation is claimed.** The gate manifest is unchanged; §D.7 is a proposed delta, marked not runnable.

## Remaining blockers

No valid candidate pairing baseline (the four attempts stay INVALID, **no cause established in either direction**) · 078-05/06 valid failures remain failures, root cause OPEN · B1 untouched, no verdict flipped under sorting so no gate edit triggered — but the reference's settle start *did* move (1→0), so no general order-insensitivity claim · A/B/A/B/A/B interleave **not adopted** · `M-F1b` UNRUN · writer byte bound OPEN through `IImageWrapper` · `M-F11`'s orthographic/constrained arms additionally blocked because the projector hardcodes perspective · publication/orphan/auditor semantics HELD · every `open_arms` entry, all sixteen findings and the full regression set still required.

**Deviation, stated:** the crosswalk proposes three *new* rows (`M-ID1`, `M-ID2`, `M-GEO2`) beyond the records the brief named, because no existing recipe asserts one-render-one-frame identity or near-plane provenance. Marked proposed, not runnable; manifest file unchanged.

**One thing I could have claimed and did not:** the mask/colour cross-binding is a **source path** — only the target-mask payload loss is banked, and only for the second serial. No leg is cited as an instance of a wrong box from the near-plane finding.

VERDICT: NEEDS-DECISION
api-equivalent cost (not billed on Max): $26.31 | turns: 93 | running total across all briefs: $1,312.40
