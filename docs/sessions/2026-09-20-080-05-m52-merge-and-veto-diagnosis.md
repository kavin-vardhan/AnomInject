# Session 080-05 — `m52` merges to `master`, and the `m26`/`m49` disagreement is diagnosed read-only

**2026-09-20.** ⛔ No code changed · no tag · no cook · no capture leg · no default flipped.
`master` **`4af249f`** == `origin/master` (pushed). Branch `feat/m52-stuck-mip` `791f8c8` KEPT.
`m51` `53bf725` UNTOUCHED. Staged bench exe `5588F6FB`; container quintet BYTE-UNCHANGED.

**VERDICT: GREEN** — the merge is done and inert, and the diagnosis is written. Its headline is a
**NEGATIVE**: the disagreement's mechanism is **NOT ESTABLISHED**, five candidates are excluded by
measurement, and the one datum that would settle it **was never logged and cannot be replayed**.
What IS established is a design mismatch worth fixing on its own terms.

---

## 1. The merge

| | |
|---|---|
| merge commit | **`4af249f`**, `--no-ff`, first parent `3ff88db`, second parent `791f8c8` |
| revert handle | `git revert -m 1 4af249f` |
| `master` | `4af249f` == `origin/master` (pushed `3ff88db..4af249f`) |
| **inert-merge proof** | `master^{tree}` == `feat/m52-stuck-mip^{tree}` == **`ec2ff258…`**, and `git diff master feat/m52-stuck-mip` is **EMPTY** |
| forecast | `git merge-tree --write-tree` predicted **`ec2ff258…`** BEFORE the merge ran — the same tree |
| tag | ⛔ **NONE.** Tags batch at the office |

🔑 **Nothing entered `master` that was not on the gated branch**, and that is a measurement
(identical trees), not an argument.

**Pre-merge gate — nothing else re-run, per the brief:**

| check | reading |
|---|---|
| `tools/test_verify_capture_consistency.py` | **25 tests, OK, exit 0** |
| `verify_capture.py --label-pixel-gate --selftest` | **98 cases OK, exit 0** |
| `verify_capture.py --selftest` (black-frame) | PASS on mid-grey, **FAIL on all-black**, exit 0 — proven both ways |

⚠ **`m51` was deliberately NOT touched and still carries an unmerged `CLAUDE.md` divergence.**
Resuming `m51` means resolving that conflict against `master`; it was not pre-resolved here, because
doing so would have put a `master`-shaped status block on a branch whose own campaign is still held.

**Docs on `master`:** `CLAUDE.md`'s status block now leads with the merge (m52 CLOSED, the catalogue
at **10**, the pool default list **unchanged**); the `docs/architecture.md` catalogue row is final;
and `client-readme.md` gained what it was missing — **a `stuck_low_mip` row in the anomaly-type
table** (it had the `stuck_mip.*` field reference but no row), the `anomaly_type` value list, the
NO-TRACE class note, and a paragraph saying why it ships off by default.

---

## 2. The prediction, written before the read

> The pre-roll is the leading candidate. `R1` made the `Positives` phase capture frames without
> opening the window until the hold is measurably down, so an event's labelled set now begins ~20
> captured frames after the fire. If the `m26` mask record is armed and resolved on a schedule tied
> to the *fire* rather than to the *labelled* frames — an arm budget, a hold-tick ceiling, or "arm on
> the first frame of the fire" — then the veto's `maxCount` would be taken entirely from pre-roll
> frames, where the anomaly has not engaged, while the row's `target_pixels` is filled from a later
> outcome that does contain the tag. Both would be reading the same mask pass and the same tag, but
> over **disjoint frame sets**. A weaker second candidate: a tag/namespace mismatch, where the census
> re-tags the actor between the veto's sample and the row's, so the veto looks up a tag the actor no
> longer carries. I expect the timeline in the banked log to discriminate: if the veto's arms sit
> before the first held frame, it is the first mechanism.

**Outcome: the timing half is CONFIRMED and is a real defect. The causal half is REFUTED — the
disjoint window does not explain the zero, and I could not establish what does.**

---

## 3. The two sources, with lines

**The m26 veto's number.** `R.MaxCount`, a MAX across contributing frames; each frame's count is

```
int32 Count = 0;
if (const FAnomalyMaskTagResult* Found = Mask.TagResults.Find(R.Tag)) { Count = Found->Count; }
++R.FramesContributed;
R.MaxCount = FMath::Max(R.MaxCount, Count);
R.State = (R.MaxCount > 0) ? MeasuredNonZero : MeasuredZero;    AnomalyMaskMeasure.cpp:538-546
```

where `Mask` is the result of an arm issued by `FAnomalyMaskMeasure::ArmIfMeasurable`
(`AnomalyMaskMeasure.cpp:255-307`) under request id `GFrameCounter`. The veto then reads only
`Rec->State` (`AnomalyCaptureSubsystem.cpp:5125`, `MaskStateVetoes` at `:4986`).

**m49's per-row `target_pixels`.** A different arm entirely:

```
const int32 Tag = Snap->MaskValues.IsValidIndex(i) ? Snap->MaskValues[i] : 0;
const int32* Found = (Outcome && Tag > 0) ? Outcome->Counts.Find((uint8)Tag) : nullptr;
Snap->TargetPixels[i] = Found ? *Found : GTargetPixelsUnmeasured;   AnomalyCaptureSubsystem.cpp:3912-3915
```

where `Outcome` is the **target mask's** outcome (`m43`/`m44`), armed by `ArmTargetMaskOwn`
(`:900`) under a high-bit request id.

⇒ **Two arms, two request-id namespaces, two consumers, two frame sets.** Measured in the banked
log: the veto's ids are small integers (`M23 PASS id=21…24`, `id=72…75`), the target mask's are
`4611686018427387905`-class.

---

## 4. ESTABLISHED: the veto's evidence window cannot overlap a deferred-onset event's labelled window

| fact | line |
|---|---|
| `ArmIfMeasurable` is called **every tick-end, unconditionally** — no labelled gate, no held gate | `AnomalyCaptureSubsystem.cpp:894` |
| a record is created for **every live fire**, also ungated | `EnsureMaskRecordsForCapturedFrame`, `:1085-1100` |
| an event gets **at most 4 arms, ever** | `MaxArmsPerEvent = 4`, `AnomalyMaskMeasure.h:47`, enforced `AnomalyMaskMeasure.cpp:265-268` |
| `ArmIfMeasurable` arms the **first** eligible record each tick and returns | `AnomalyMaskMeasure.cpp:262-303` |

⇒ an event's entire arm budget is spent on the **four ticks following its record's creation**.
Measured, exactly:

```
fire startFrame=20  ->  M23 ARM id=21, 22, 23, 24     (armIndex 1..4)
fire startFrame=71  ->  M23 ARM id=72, 73, 74, 75     (armIndex 1..4)
```

and the measured deferred-onset pre-roll on the same legs is **19–22 captured frames**
(`Capture(m52): DEFERRED-ONSET window opens after 19 pre-roll frame(s)`).

🚨 **So for `stuck_low_mip`, 100 % of the veto's evidence is gathered before the anomaly has
engaged.** The veto decides whether to delete an event using frames that are, by construction, not
the event's frames. **That is a defect regardless of whether any particular zero is correct**, and
it is the thing this session recommends fixing.

⚠ It is a mismatch `R1` INTRODUCED. Before `R1` the labelled window opened at the fire, so the four
arms landed inside it.

---

## 5. NOT ESTABLISHED: why the count was zero — five candidates excluded

The decisive comparison is **one leg, one actor, two fires**
(`M52C_M52C_G3_HOLD_NAT`, `StaticMeshActor_…_2044254803`):

```
M26S1 EVENT startFrame=20 tag=209 state=MEASURED_NONZERO maxCount=32534 arms=4 resolved=4
  framesDiscarded=0 framesResidual=0 framesUnconfirmed=0 framesNoPass=0 framesContributed=4
  probeArms=0 skippedHidden=0 collisions=0 tagFailed=0
M26S1 EVENT startFrame=71 tag=210 state=MEASURED_ZERO    maxCount=0     arms=4 resolved=4
  framesDiscarded=0 framesResidual=0 framesUnconfirmed=0 framesNoPass=0 framesContributed=4
  probeArms=0 skippedHidden=0 collisions=0 tagFailed=0
```

**Same actor. Same settled camera. Every bucket zero on both. Four clean contributing frames on
both. One reads 32,534 pixels and the other reads 0.**

| candidate | excluded by |
|---|---|
| a discard bucket swallowed the good frames | every bucket reads **0** on BOTH events; `framesContributed=4` on both |
| the custom-depth pass did not run | `M23 PASS … customStencilExtent=1280x720` on **all eight** passes; `framesNoPass=0`. The `!bPassRan` early-out (`AnomalyMaskMeasure.cpp:479-498`) never fired |
| the tag was not applied to the actor | `M23 ARM … taggedComponents=1` on all four arms of both events, **and** `TAG-OWNERS` shows the actor carrying `209` at ticks 23–26 and `210` at ticks 74–77 — the engine's own live tag map agrees |
| the target mask stole the render / re-tagged the actor | **identical structure on both events**: ids 21,22 solo then 23,24 shared with a target-mask arm; ids 72,73 solo then 74,75 shared. The event that read 32,534 shared exactly as much as the one that read 0 |
| MainWorld's ~25-frame camera settle | **excluded in the opposite direction**: the NONZERO event's arms (ticks 21–24) are INSIDE the settle; the ZERO event's (72–75) are outside it |
| the census re-tagged the actor (my second prediction) | the census holds `200–208` on this leg; the veto's records hold `209`/`210`, and `TAG-OWNERS` shows no collision (`shared=0`) |

⇒ **CAUSE NOT ESTABLISHED (`G120`), AND NOT CHASED FURTHER.**

🚨 **And it cannot be recovered from the banked evidence.** `CollectResults` reads
`Mask.TagResults.Find(R.Tag)` at `AnomalyMaskMeasure.cpp:538-542` and **logs nothing about that
table's contents** — no line anywhere says which tags the reduce actually contained, or with what
counts. ⛔ **No replay was run, and one would not have helped**: the same recipe on the same binary
would reproduce the same silence. Surfacing it needs a log line, which is a code change and out of
this brief's scope.

📌 **A LEAD, NOT A FINDING, and not chased:** on the `M52E_G7_YIELD` vetoed event
(`BP_SpawnPad_C`, tag 209) `taggedComponents` reads **3, 3, 2, 2** across its four arms — the set of
components `TagActor` reached **changed mid-event**. On the `G3` event it is `1,1,1,1`.

---

## 6. The proposed fix, sized, and the gate that would prove it

**Fix: gate the veto's arms on the labelled window for deferred-onset records only.**

| file | change | ~lines |
|---|---|---|
| `AnomalyMaskMeasure.h` | `bool bAwaitLabelled = false;` on `FAnomalyMaskRecord`, plus a per-tick "labelled now" set | 4 |
| `AnomalyMaskMeasure.cpp` | in `ArmIfMeasurable`, `continue` past a record whose `bAwaitLabelled` is true and which is not labelled this tick — beside the existing `ArmsIssued`/`bKnownUnmeasurable` guards at `:265-272` | 6 |
| `AnomalyCaptureSubsystem.cpp` | set `bAwaitLabelled` from `Injector->DoesAnomalyHaveDeferredOnset(F.Id)` at `EnsureMaskRecordsForCapturedFrame` (`:1098`), and pass the per-fire labelled bit before the `ArmIfMeasurable` call at `:894` | 15 |

**≈ 25 lines, entirely inside `AnomalyCapture`.** No `IAnomaly` change, no artifact field, no
`annotation.json` movement.

**The gate that would prove it**, on the banked `G3_HOLD_NAT` recipe:
1. **every** `M23 ARM` request id belonging to a `stuck_low_mip` record falls **inside** that event's
   labelled window, read from `annotation.json`'s `injected_frames` — joined through `TAG-OWNERS`'
   `si=/tick=` pairs, which the log already emits per captured frame;
2. `vetoed_events` **0** on a leg where every event's rows carry `target_pixels > 0`;
3. 🚨 **can-fail (`G96`)**: a bench lever restoring the ungated arm must **reproduce a veto** on the
   same recipe. Without it, (2)'s zero is unfalsifiable.

⚠ **What the fix would NOT do, stated plainly: it would not prove the zero was wrong.** It moves
the veto's evidence onto the frames the event actually claims — correct on its own terms, and the
thing that makes a future zero *interpretable* rather than a disagreement between two measurements
of different frames. **The §5 mechanism would remain open**, and if a zero survives the fix it
becomes a much sharper question.

---

## 7. Does it affect other deferred-onset anomalies? **No — none exist.**

- `HasDeferredOnset()` is overridden in exactly one place: `Anomaly_StuckLowMip.h:22`. The default
  is `false` (`IAnomaly.h:34`). **`stuck_low_mip` is the only deferred-onset anomaly.**
- **`lod_popping` is NOT one.** It is `EAnomalyActiveSource::AnomalyState`
  (`AnomalyCaptureSubsystem.cpp:284`) like `stuck_low_mip`, but it has no deferred onset: it toggles
  inside its own window, so its four arms land inside its labelled window. And **both of its phases
  DRAW the object** — a different LOD is still a silhouette — so an arm on an un-popped frame still
  measures non-zero. The arm-budget shape is benign there.
- `camera_clipping` is the other `AnomalyState` id (`:289`) and is global, so it has no mask record
  to arm.

⇒ **The fix is scoped to `stuck_low_mip` by construction**, exactly as `R1` itself was.

---

## 8. Incidence — association only

| binary | vetoes / applied fires |
|---|---|
| `A8742A4A` (080-02, **before** `R1`'s pre-roll) | **0 / 46** |
| `DC8BA0C1` (080-03) | 2 / 25 |
| `5588F6FB` (080-04) | **1 / 26** |

⚠ **NO CAUSE IS ESTABLISHED IN EITHER DIRECTION.** The association is *confounded with the very
change that separates the two windows* — `A8742A4A` predates `R1`, so "no pre-roll, no vetoes" and
"pre-roll, some vetoes" are the same axis. That makes it suggestive of §4 and **evidence for
nothing**. The sample is also small: 3 vetoes total across 97 applied fires.

---

## 9. What this session did not do

- ⛔ **No code changed.** The fix in §6 is sized, not written.
- ⛔ **No capture leg run**, and no replay — §5 says why one would not have helped.
- ⛔ **No tag**, no cook, no office data, no default flipped.
- ⛔ **`m51` untouched**, its `CLAUDE.md` divergence deliberately unresolved.
- ⛔ The branch `feat/m52-stuck-mip` is **kept, not deleted**.
- ⛔ `m55` and S2′ remain named and not built; the `GAutoPoolDefaultEnabled` decision is still the
  owner's and still waits on `m55`.
