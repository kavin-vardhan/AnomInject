# 2026-08-21 — session 049 — m31-S1: SVE wanted-handshake telemetry

## §1 Goal

m31 is OPEN and ASSIGNED by the owner: the SVE capture path — the SHIPPING DEFAULT since m25 —
produced ZERO frames on Concorde, the first host that was not the test rig. This session:
(1) verify the milestone number and repo state, (2) independently verify the office-side
diagnosis from source on this box (guard against relay error), (3) build and ship m31-S1, the
diagnostic instrumentation, (4) pre-register the reading before any result exists, (5) file the
quoted-outDir sharp edge without fixing it.

## §2 STATED PLAINLY, FOR THE RECORD

**THIS IS THE FIRST DEFECT EVER FOUND BY A SECOND HOST, AND THE SHIPPING DEFAULT CAPTURE PATH
WAS THE BROKEN ONE.** The SVE path was certified across ten configs, every ratio regime, both
delivery modes, packaged and PIE, over four milestones (m24–m28) — and every one of those legs
ran on THE SAME PROJECT. StackOBot is a simple single-viewport host; the design assumption under
test — exact frame-number equality between two publish/mark sites whose ordering the engine does
not guarantee — was never exposed to a host that could falsify it. **The test rig structurally
could not have shown this.**

**THE TRANSFERABLE GENERALISATION: certification depth on one axis says nothing about a second
axis.** This is the second instance in three days — m27's play-gate finding was the first, where
every bench leg had a settled camera and so could not show view-dependence of the cure's reach.
Here every certification leg had one host, and so could not show host-dependence of the frame
handshake. Thorough on one axis, blind on another, and the blindness presented as four
milestones of clean passes (G135's shape, at project scale).

## §3 Milestone number and repo state (first action)

- `git log --oneline -6`: HEAD `1d9dc2e` (m30 status docs), tree CLEAN.
- `git tag --list "m*"`: **m28, m29 and m30 ALL EXIST** (m28 = `d2ef8fa`, m29 = `ab2fb41`,
  m30 = `1d9dc2e` by `rev-parse <tag>^{commit}` discipline, G143). The highest tag is **m30**.
- Chat's record ("last tag m27, next is m28") is STALE BY THREE MILESTONES. The owner's m31 is
  the CORRECT next number — there is NO gap and no renumbering in either direction.
- `git rev-parse --short feature/stencil-capture` = **`76cac74`**, untouched, as expected.

## §4 ROLE RULING, PERMANENT (recorded verbatim from the owner's brief)

There are now TWO Code instances. **This box is the ONLY CANONICAL AUTHOR.** The office
instance is EYES, BUILDER AND RUNNER ONLY — it reads source and logs, builds pulled code, runs,
and reports on screen. **IT COMMITS AND PUSHES NOTHING.** Everything reaches Concorde BY GIT
PULL. **NOTHING LEAVES THE OFFICE MACHINE** — no logs, no sessions, no builds, no files; the
only outbound channel is what the owner reads off the screen. Any plan step needing a file from
that box is a DESIGN ERROR.

## §5 The symptom (relayed) and the office diagnosis (relayed — EVIDENCE TO VERIFY, NOT GOSPEL)

Concorde, UE 5.1 source-built, packaged name FWChaos, m27 build, delivery ON, cooked ini
confirmed applied. SVE default: session folder + annotation.json + run_summary written,
`Actual_Frames/` created and EMPTY, `total_frames 0`, 9 bursts fired, pacer clean (29.78 vs 30).
`IAI.Capture.SVE 0`: PNGs written normally, same build, same machine.

Office-side findings (a separate Code instance with direct read access to that build's logs,
relayed via the owner): zero `submitted` lines (that log fires unconditionally inside
`SubmitInFlight_RenderThread` ⇒ never called) · drain stats `samples=0 notReadyPolls=0` ⇒ the
in-flight list was EMPTY, not pending-but-slow · `pendingAtDrainEntry=120
flushIterationsConsumed=8 pendingAfter=120` · ring HEALTHY: `published=332 consumed=332 missed=0
wrapped=268 corrupted=0`, no NO-KEY warnings, no empty-rect warnings, `sveExtension=1
sveCapturer=1 forceMiss=0` · the only silent exit on the chain is the `!Entry.bWanted` return ⇒
`bWanted` false on all keys, a SYSTEMATIC miss of the game-thread `MarkWanted(GFrameCounter)`
against the publish-time `GFrameCounter`. **HONESTLY FLAGGED OFFICE-SIDE: bWanted=false is
INFERRED from the 0/120 hit rate, not OBSERVED.** S1 exists to close exactly that gap.

## §6 TASK 1 — independent source verification on this box (m30 HEAD; office cited the m27 build, so line numbers differ slightly — mechanisms identical)

**Q1: is the `!Entry.bWanted` return truly the ONLY silent exit between a successful `LookupKey`
and submission? YES.** `AnomalySceneViewExtension.cpp`, `AfterPass_RenderThread`:

- `:73` `LookupKey` succeeds (a miss is LOUD — the NO-KEY warning at `:76-81`).
- `:84-87` `if (!Entry.bWanted) { return SceneColor; }` — **the ONLY silent return after a
  successful lookup.**
- `:91-96` empty texture/rect → return, but LOUD (`empty scene-colour rect` warning at `:93`).
- `:100-103` otherwise `AddEnqueueCopyPass` + `SubmitInFlight_RenderThread` ALWAYS follow.

The silent exits at `:65-68` (capturer invalid / SceneColor invalid / `bIsSceneCapture` /
`bIsReflectionCapture`) all precede `LookupKey`, and Concorde's `consumed == published == 332`
proves no keyed family died there — every published key was looked up and found, exactly once.
Corroborates the office read.

**Q2: can anything zero `bWanted` between publish and lookup? NO — publish-time is the only
writer.** `AnomalySveKeyRing.cpp` (pre-S1 numbering): `Entry.bWanted` is assigned once in
`PublishKey` (`:103`) before `Ring.Add` (`:116`); ring entries are never mutated after Add —
the only post-Add operation is `RemoveAt(0)` on wrap (`:119`); `LookupKey` (`:125-141`) only
reads and copies out; ForceMiss corruption touches `FamilyFrameNumber` ONLY (`:110`), never
`bWanted`; `Reset` (`:73-83`) clears everything and is called only at `StartRun`
(`AnomalyCaptureSubsystem.cpp:852`) and in the self-test. The capturer's `WantedFrames` set has
exactly two erasure sites — `SubmitInFlight_RenderThread` (which never ran on the failed leg)
and `Reset` (StartRun only) — so no mid-run path unmarks a frame before publish either.

**The office diagnosis survives independent verification.** The one open link remains the one
they flagged: bWanted's value at publish is inferred, and S1 measures it.

## §7 m31-S1 — WHAT SHIPPED (DIAGNOSTIC ONLY, ADDITIVE ONLY, PERMANENT)

The failed path failed SILENTLY END TO END on its first real host — it has earned permanent
instrumentation, exactly as m27's mask echo did. No behaviour change: `bWanted`'s computation is
untouched (`IsWanted(GFrameCounter)`, same call, its value now bound once and passed to both
publish and trace); no default flips (`bSveCaptureDefault` and the compiled default stay);
m27/m30 tags untouched; `feature/stencil-capture` untouched; `annotation.json` field set
UNCHANGED — **`P6` does not move**.

1. **`wantedPublished` counter** beside the existing ring counters (`AnomalySveKeyRing`:
   `FCounters.WantedPublished`, incremented in `PublishKey` iff `bWanted`, reset with the ring,
   reported by `GetCounters`).
2. **Bounded per-publish trace** — first **64** publishes (compiled constant
   `FAnomalySveCapturer::WantTracePublishLimit`), one line each from
   `FAnomalySveCapturer::TraceWantPublish`, called by `BeginRenderViewFamily` immediately after
   `PublishKey`. Each line carries: family frame number, publish-time `GFrameCounter`,
   `bWanted`, the capturer's last-marked frame (new additive read-only getter
   `GetLastMarkedFrame()`; `MarkWanted` now also records `LastMarkedFrame` — `IAnomaly`
   untouched), and the signed offset (publish GFrameCounter − last-marked, `n/a` before any
   mark). **One line per publish** — repeated `GFrameCounter` values against differing family
   numbers (the multiple-view-families-per-frame candidate, chat-origin, UNVERIFIED, not
   adopted) are readable directly from the lines, never averaged away. Offset min/max/histogram
   accumulate over the traced window, reset with the capturer at `StartRun`.
3. **Unconditional run-end summary** (no flag gates it; emitted with the key-ring line in
   `FinishRun` whenever the run used the SVE path): `wantedPublished=X of Y publishes`, traced
   count, offset min/max/mode plus the full histogram. Its wording states the two readings:
   wantedPublished≈0 with marks issued = the handshake missed; wantedPublished≈wanted count =
   the loss is downstream of publish.
4. **`run_summary` gains `wanted_published` ONLY** (beside the `key_ring_*` fields, SVE runs
   only). The one-line argument: it is the only artifact-side record of the handshake outcome
   when a client box's log is lost, and `run_summary` is not `P6` territory (m25 `capture_path`,
   m27 veto counters, m28 precedent). `annotation.json` untouched.

**Tokens (VETOED-OBJECT discipline):** trace lines carry `SVE-WANT-TRACE`, the summary carries
`SVE-WANT-SUMMARY` — both verified unique repo-wide BEFORE settling on them (0 prior
occurrences), and the summary line does not repeat the trace token's literal.

Files: `AnomalySveKeyRing.{h,cpp}` · `AnomalySveCapturer.{h,cpp}` ·
`AnomalySceneViewExtension.cpp` · `AnomalyLabelWriter.{h,cpp}` · `AnomalyCaptureSubsystem.cpp`.

## §8 TASK 3 — the reading is PRE-REGISTERED

`docs/predictions/2026-08-21-m31-s1-branch-table.md`, committed WITH S1, to be restated verbatim
before any result is read. Branch table R-1/R-2/R-3 as ruled; ALL branches: PIE licenses
mechanism only (G76), and the fix, when built, validates PACKAGED with a same-seed before/after
against the broken run (the m27 count-gate shape). The three candidate fixes are pre-registered
and NONE is authorised: (1) FIFO pairing (costs exact-frame identity), (2) a render-side
tolerance window (needs the number R-1 would supply — why it cannot be chosen first),
(3) instrument first — which is S1. Chat rules the fix once the number exists.

**The pairing fact, established read-only for option 1's later viability debate (NOT ruled on):**
`PendingSnapshots` pairing is an exact `TMap::Find` on `RequestId`
(`AnomalyCaptureSubsystem.cpp:102` declaration, `:1448` Add keyed by arm-time `GFrameCounter`
via `:1436`, `:1251` Find) with NO tolerance. A one-engine-frame slip either (a) DROPS with a
VERBOSE-level log (`:1252-1257` — invisible at default verbosity), surfacing only in the
run-end `did not resolve` warning (`:1353-1357`), or (b) **SILENTLY MISPAIRS** when the
off-by-one key is itself pending — guaranteed in burst interiors by the ≥1-frame readback
latency — pairing one frame's pixels with the adjacent frame's snapshot (`:1265-1286`),
removing the neighbour's snapshot (`:1288`) and orphaning the true owner into case (a). It
tolerates nothing; the failure direction depends on burst position.

## §9 TASK 4 — FILED, NOT FIXED

- **G153** (new): `IAI.Capture.Start` with a QUOTED outDir carries the quote characters into
  `RunDir` as literal path characters ⇒ `failed to write annotation.json`. Client-facing sharp
  edge in console arg parsing. Fix candidate folded into the milestone that fixes the
  handshake — same cook — per the G139 precedent.
- Still filed, unchanged, same fold-in rule, same cook: `IAI.Capture.Mask`'s stale help string
  (m27 RULING 2, Deliverable A3 PARTIAL) and G118's cooked placeholder token.

## §10 What is NOT done, named

- **No fix exists, none is designed, none is authorised.** S1 measures; chat rules on the number.
- **The office inference is verified as INFERENCE, not yet as measurement** — the instrumented
  Concorde run has not happened. The owner pulls at the office; results return by screen.
- **No local run of the instrumentation** — this box compiled it (editor target, exit 0); the
  behaviour-bearing path is unchanged by construction (all additions are reads and log lines).
  A StackOBot leg would show offset=0 and wantedPublished≈wanted-count and would be a sanity
  check, not evidence about Concorde (the whole point of m31 is that this rig cannot reproduce
  the miss).
- The ~2.8× publishes-per-wanted-frame lead (332 vs 120) is a CANDIDATE, NOT A CLAIM — the
  trace is designed so it would show immediately if real.

## §11 Hand-off

Owner: pull on the office box, rebuild, re-run the failed Concorde leg with the SVE default (no
config change needed — the instrumentation is unconditional), and read back off the screen:
the run-end summary line (its token is named in §7), the first-64 trace lines (theirs is named
in §7 too), and `run_summary.wanted_published`. Then restate the branch table from
`docs/predictions/2026-08-21-m31-s1-branch-table.md` verbatim and grade R-1/R-2/R-3 in chat.
