# Session 078-02 — m51 predictions revision after two written reviews

**Date:** 2026-09-06. **Brief:** 078-02, issued by Codex (orchestrator). **Scope fence:** targeted
read-only source/engine inspection, artifact hashing, and a documentation-only commit on `m51`.
**No implementation, build, cook, leg, test, staging, worktree/branch change, push, merge or tag.**
**Verdict at close:** NEEDS-DECISION — the corrected predictions require the reviewer's written
approval of this exact snapshot.

---

## 1. What arrived and what it means

Two independent written reviews of `b532ddd`, both **CHANGES REQUIRED**:

- `_reviews\078-01-codex-preflight-review.md` — eleven objections.
- `_reviews\078-01-chat-predictions-review.md` — agrees with all eleven, supplies concrete devices
  for the four that were "needs a lever" without naming one, and adds four independent blockers.

Neither review is new empirical evidence; both are static review of plan and source. The corrected
artifact is **AMENDMENT 1** appended to `docs/predictions/2026-09-06-m51-release-hardening.md`, which
is now the **active specification**; sections 0–8 of that file are the reviewed snapshot and are
superseded wherever they disagree. The draft was not deleted — the reviewed text has to remain
readable beside what replaced it — but its closing line now says so, so no stale row reads as
executable.

The pilot's own measure: the 078-01 turn produced a preflight that re-located all sixteen findings at
the current tree and declared unrunnable arms unrunnable, and then **two reviewers found five source
errors in it that the session had not** — the ForceMiss/stale-arm distinction, ViewLag not being a
readback lever, the omit-depth lever measuring the wrong pass, the game-thread call site of
`BeginRenderViewFamily`, and `PF_A2B10G10R10` at 4 B/px. Every one is confirmed below at source. That
is the review layer working exactly as the operating contract intends.

## 2. Source verification done this session (all at `ac13700`, engine at `D:\UESource\UnrealEngine`)

Nothing in AMENDMENT 1 rests on a reviewer's assertion alone. Each was re-read:

| claim | verified at | result |
|---|---|---|
| observable predicate | `AnomalyCaptureSubsystem.cpp:3872-3880` | `Px == Unmeasured ⇒ continue` at `:3874-3878`; `bObservable = bLabelled && bHeld && Px >= ObservableMinPixels` at `:3880`. **DrawnCount appears only at `:3887` and increments `FramesDrawnUnexpected`; it never touches `Observable[i]`.** Codex 1 / Chat 1 **CONFIRMED**. |
| mask-effective predicate | `:2965` (master) vs branch `:2901` | master `bTargetMask && bMaskMeasure && bAsyncCapture && (OutputHeight == 0)`; the branch **adds `bSveCapture`** and a reason string at its `:2935`. Chat blocker 3 **CONFIRMED**. |
| `exposure_dip_scope` emission | `AnomalyLabelWriter.cpp:156-162` | emitted **only inside `if (bExposureDip)`**. Chat blocker 2's "on every labelled row" is **unsatisfiable without a schema change**; the brief's correction stands. |
| exposure detector shape | `AnomalyCaptureSubsystem.cpp:3951-3959` | `if (bDipAll && bDipExcl)`; scope set from `bExclusionUsable` at `:3959`. The excluded operand is a genuine **AND** gate ⇒ a suppression count is the right proof. `frames_exposure_dip_suppressed` already exists (banked `PB1G_BL_NAT` reads **3**). |
| ForceMiss semantics | `AnomalySveKeyRing.cpp:16-28` | corrupts **published** keys; `1` = every key, `N>1` = every Nth, with a phase offset. A **rendered-but-unreadable** frame. Codex 3 **CONFIRMED**. |
| stale-arm path | `AnomalySveCapturer.cpp:45+` | `ConsumeWantedForPublish` pops `PendingWanted[0]` **FIFO with no expiry**. |
| omit-depth lever | `AnomalyInjectorSubsystem.cpp:861-868` | omits the **depth-pass half** (`bRenderInDepthPass` stays true, main pass off). It does **not** disable the custom-depth measurement pass. Codex 4 **CONFIRMED**. |
| `bDisableWorldRendering` | `GameViewportClient.cpp:1696-1707`, `.h:98` | gates `BeginRenderingViewFamily`; public `uint32:1`. Chat's device is **viable with no engine modification**. |
| PendingSnapshots cap | master vs branch | **master has NO admission cap** (only the drain bound `Iter < 8` at `:4040` and the finalize sweep at `:4082-4086`); the branch adds `if (PendingSnapshots.Num() >= 8)` at its `:4072`. |
| F1 delivery trigger | `client-delivery.md:529`, `client-readme.md:715`, host `DefaultGame.ini` | the docs only *describe* the refusal; **no `OutputHeight` key is set anywhere**. |

## 3. The two places where a reviewer was corrected

Both reviews are accepted in substance. Two specific assertions could not be adopted as written, and
in both cases the source is the arbiter.

**3.1 — Chat's A-side prediction for the R8 leg is wrong for this binary.** Chat predicts
"A-side (`B8AC65D8`): cap fills, no recovery". Measured: **`ac13700` has no `PendingSnapshots`
admission cap**; that cap is the branch's addition. A binary cannot fail by filling a cap it lacks.
What `B8AC65D8` *will* do follows from `ConsumeWantedForPublish`'s FIFO-with-no-expiry: through a
40-tick render-withholding burst, `PendingWanted` and `PendingSnapshots` both grow, and **on
resumption each new family consumes the OLDEST pending arm** — pairing a fresh render with a
~40-tick-old snapshot. That is **mass mis-pairing followed by a bulk drop at finalize**, which is a
sharper and more alarming A-side signature than the one predicted, and it is exactly the hazard
journal 049 §8 banked ("silently mispairs with the adjacent armed frame's snapshot").

The consequence for gate design is the one the brief insisted on: **a console lever absent from an
archived executable is not a device**, so the cap/cancellation regression gets an **in-candidate**
falsifier (`IAI.Bench.NoStaleArmSweep 1`) run against the healthy control on the same binary, with
distinct expected signatures. This also settles the expiry question the brief raised: because the
FIFO never expires, waiting for the next `BeginRenderViewFamily` is **not** sufficient, so AMENDMENT 1
specifies a **game-thread sweep before the cap check**, retiring unpublished arms older than
`StaleArmMaxTicks` while leaving already-published GPU work intact — and it declines to predict a
bare `unrendered_ticks == 40`, requiring instead that the counter equal the leg's own logged count of
armed-but-never-published ticks.

**3.2 — Chat's Phase B row-field demand cannot be met without breaking R1.**
`exposure_dip_scope` is emitted only when a dip is detected, so requiring it on every labelled row of
a **zero-dip** leg would change legacy conditional-emission behaviour. AMENDMENT 1 preserves the
conditional emission and meets the demand's real intent — proving the excluded operand was
*evaluated* — with per-frame `LumaAll`/`LumaExcl`/rolling-means/usable-pixel diagnostics plus a
required non-zero `frames_exposure_dip_suppressed` on a leg containing a known hide-induced
whole-frame drop. That is a stronger assertion than the field's presence: it shows the exclusion
actively **suppressed a false dip**.

## 4. The ruling that mattered most, and its cost

**Fork F-b is resolved DELETE.** The 078-01 draft proposed re-pointing the m26 event veto at
per-frame evidence. Both reviewers rejected it on the same ground: re-pointing still *deletes events*,
and operating contract §3 with the m49 ruling says zero-observable events **stay in the file**. My
counter-argument — that the client's dataset has never contained such events — described the m26-era
Bates M2 data, which the m49 ruling deliberately changed and then shipped in the schema-v2 docs.
The argument was about the wrong baseline.

The information the veto carried is preserved without deleting anything: `vetoed_events` is kept as a
compatibility key reading **0** with a changelog line, and a new additive `zero_observable_events`
counts the retained events that were measured and had no observable frame. The definition is written
tightly on purpose — **a fully unmeasured event is excluded**, because counting it as a measured
negative would manufacture false negatives out of missing evidence, which is the direction `m26`'s
admit bias exists to forbid. The P-C7 consequence (the event set grows on the legs where the veto used
to fire) is declared per leg in advance, not discovered.

## 5. What AMENDMENT 1 contains

A1.1 provenance corrections · A1.2 disposition of all 19 review items · A1.3 the corrected truth
table (rows A–O, master's predicate, row B with `bSveCapture`, row K as the G258 ruling, row N2 for
ForceMiss, the mixed-fire worked example, globals/sync) · A1.4 event derivation with the veto deleted
· A1.5 reconciliation-based `capture_complete` with publication order and the declared orphan policy
· A1.6 lifecycle with the GT/RT correction, per-request terminal state, the stale-arm sweep and lease
release on fire-end **and** arms-resolved · A1.7 writer accounting with a reservation policy and
honest overrun reporting · A1.8 the seven devices, each with what it actually proves · A1.9 the gate
matrix, both fixtures, 24 rows with falsifiers, stop conditions and expected auditor exits · A1.10
throughput (600/60, the numeric 1.05 band, the older-prose conflict resolved) · A1.11 per-leg P-C7
allowlists · A1.12 the auditor with four exit codes and its known-answer cases · A1.13 full-path
full-hash provenance · A1.14 the OPEN arms · A1.15 the dependency-corrected commit order · A1.16 a
self-review.

Two structural choices worth naming. **The writer's byte accounting does not claim a bound it cannot
prove**: PNG can exceed raw on incompressible input, a 1.1× cap is a rejection policy rather than a
memory bound, so admission charges a reservation whose encoder factor is *measured on the calibration
leg* and an actual overrun is completed, counted and reported rather than truncated or rejected. And
**the F2 oracle is forbidden from calling the implementation's projection helper or reading the frozen
label snapshot** — otherwise it tests the code against itself — with the rounding conventions, the
1-px tolerance and its justification written down, and a deliberately perturbed input proving the
oracle can fail before any verdict is read.

## 6. Provenance recorded

**m51's runtime A-side is `B8AC65D8` + the Phase B container**, verified byte-identical between the
staged bench set and `_binary_baselines\m49b-postcook-container-2AFD2DDB\`. The older
**`A159EDFD` + m46 container** is preserved as the still-load-bearing historical anchor (m50's A-side
and the exposure BEFORE picture) and is **not** reused as m51's A-side. Full paths, full SHA-256 and
byte counts for both sets are in A1.13.

**The Lyra baseline is a module set, not a plugin SHA** — a checkout SHA does not establish which
binary an editor loads. The five files that do (four DLLs plus `UnrealEditor.modules`) are recorded,
and one of them is worth flagging: **`UnrealEditor-AnomalyInjector.dll` is dated 2026-09-04, two days
older than the other three.** That is consistent with Phase B touching only `AnomalyCapture` and
`AnomalyShaders`, but it means the freeze must capture the whole set with its manifest.

**No candidate hash is invented.** m51's binaries do not exist; A1.13 specifies the later freeze step
— one plugin source revision per fixture, hashed and archived before the first certifying leg, with
the one-binary rule applied **per fixture** because the two games cannot share a host executable.

## 7. What this session did NOT do

No source, shader, tool, fixture or engine file was edited — `git diff ac13700..HEAD -- Source Shaders
tools` is empty. No lever was built, no test or self-test run, no build, cook, capture, leg, staging,
worktree or branch change, no push, merge or tag. No mailbox write. No office data. The staged bench
exe and container and the Lyra worktree were hashed and left byte-identical. The ten owner handoffs
remain untracked and unmodified.

## 8. Outstanding

Reviewer approval of the exact corrected snapshot. Chat's stated ceiling for the next pass is
**"APPROVED FOR IMPLEMENTATION COMMITS 1–13 (no legs)"**, with legs released per phase after each
commit group's own predictions are frozen — which matches AMENDMENT 1's structure. The OPEN arms in
A1.14 stay open and are not convertible by any leg in this plan.
