# REPORT for 080-05-m52-merge-and-veto-observability-diagnosis.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 080-05-m52-merge-and-veto-observability-diagnosis.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: high
started: 2026-09-20 22:12:49 | finished: 2026-09-20 22:25:21 | duration: 13 min | exit code: 0
context at last turn: 760322 tokens (76% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\080-05-m52-merge-and-veto-observability-diagnosis.live.txt

---- FINAL MESSAGE (Claude Code) ----
Done. Checkout restored to `m51` as found; `m51`'s `CLAUDE.md` divergence left unresolved per the brief.

---

# 080-05 — merge + read-only diagnosis

## 1. Merge

| | |
|---|---|
| **merge SHA** | **`4af249f`** — `--no-ff`, first parent `3ff88db`, second parent `791f8c8` |
| **`master`** | **`ce4d1f4` == `origin/master`** (pushed `3ff88db..4af249f`, then `4af249f..ce4d1f4`) |
| **inert-merge proof** | `master^{tree}` == `feat/m52-stuck-mip^{tree}` == **`ec2ff258…`**, and `git diff master feat/m52-stuck-mip` is **EMPTY**. `merge-tree --write-tree` forecast **that same tree** before the merge ran |
| **revert handle** | `git revert -m 1 4af249f` |
| **tag** | ⛔ none — 30 tags, newest still `m30` |
| **branch** | `feat/m52-stuck-mip` `791f8c8` **kept**, == origin |
| **`m51`** | `53bf725` **untouched**; its `CLAUDE.md` divergence vs `master` (358 insertions / 440 deletions) is **left unresolved for the m51 resume**, as instructed |

**Pre-merge gate on the branch tip — nothing else re-run:** contracts `test_verify_capture_consistency.py` **25 tests OK exit 0**; `--label-pixel-gate --selftest` **98 cases OK exit 0**; black-frame `--selftest` PASS on grey / **FAIL on black** exit 0.

**CLAUDE.md diff (status commit `ce4d1f4`, 3 files, +328/−5):** the status block gains a 79-line 080-05 "you are here" — merge SHA + inert proof, `m52` CLOSED, **catalogue at 10**, **auto-pool default list UNCHANGED** (`blinking`, `missing_texture`, `corrupted_texture`, `lod_popping`), the `G270` limitation, `G269`, the `m55` dependency, S2′ costed-not-built, and the m51 conflict warning; the 080-04 block is demoted to "superseded". `client-readme.md` gained what it was genuinely missing — **a `stuck_low_mip` row in the anomaly-type table** (it had the `stuck_mip.*` field reference but no row), the `anomaly_type` value list, the conditional NO-TRACE note, and a why-it-ships-off paragraph.

## 2. Diagnosis — prediction, then the read

**Predicted:** pre-roll timing — the veto samples before the hold lands, so the two read disjoint frame sets. **The timing half is confirmed and is a real defect; the causal half is refuted.**

**Established (a), the two sources.** The veto's number is `R.MaxCount`, max over frames of `Mask.TagResults.Find(R.Tag)` — `AnomalyMaskMeasure.cpp:538-546` — from arms issued by `ArmIfMeasurable` (`:255-307`) under request id `GFrameCounter`; the veto reads only `Rec->State` (`AnomalyCaptureSubsystem.cpp:5125`, `MaskStateVetoes` `:4986`). m49's `target_pixels` is `Outcome->Counts.Find(Tag)` from the **target mask's** arm (`AnomalyCaptureSubsystem.cpp:3912-3915`, armed `:900`). **Two arms, two id namespaces, two frame sets** — confirmed in the log (`M23 PASS id=21…24` vs `id=4611686018427387905`-class).

**Established (b), and this is the defect.** `ArmIfMeasurable` is called **unconditionally every tick-end** (`:894`); records are created for every live fire, ungated (`:1085-1100`); `MaxArmsPerEvent = 4` (`AnomalyMaskMeasure.h:47`). So an event's whole budget lands on the four ticks after its record appears — measured: fire at `startFrame=20` → arms 21,22,23,24; fire at 71 → 72,73,74,75 — against a measured pre-roll of **19–22 captured frames**. ⇒ **for a deferred-onset anomaly, 100 % of the veto's evidence is gathered before the anomaly has engaged.** `R1` introduced that mismatch.

**NOT established — why the count was zero.** Decisive comparison: one leg, one actor, two fires — `tag=209 MEASURED_NONZERO maxCount=32534` vs `tag=210 MEASURED_ZERO maxCount=0`, **both** `arms=4 resolved=4 framesContributed=4` with every discard bucket **0**. Excluded by measurement: discard buckets; pass-didn't-run (`customStencilExtent=1280x720` on all eight passes); tag-not-applied (`taggedComponents=1` each, and `TAG-OWNERS` shows 209 then 210); target-mask contention (**identical** pass structure — two solo then two shared — on both); camera settle (**opposite direction**: the NONZERO event's arms are *inside* the settle); census re-tag (census holds 200–208, `shared=0`). ⛔ **Cause not established, not chased** (`G120`). The datum that would settle it — the reduce table's contents — **is logged nowhere** (`AnomalyMaskMeasure.cpp:538-542` reads it and prints nothing), so **no replay was run**: it would reproduce the same silence.

**Fix, sized, not built:** gate arms on the labelled window for deferred-onset records — `AnomalyMaskMeasure.h` +4, `.cpp` +6 (beside the guards at `:265-272`), `AnomalyCaptureSubsystem.cpp` +15 ≈ **25 lines**, all inside `AnomalyCapture`. **Gate:** every `stuck_low_mip` `M23 ARM` id inside that event's `injected_frames`; `vetoed_events` 0 where every row has `target_pixels > 0`; **can-fail** = a lever restoring the ungated arm that must reproduce a veto. ⚠ It would **not** prove the zero wrong — it makes a future zero interpretable.

**Other deferred-onset anomalies: none.** `HasDeferredOnset()` is overridden only at `Anomaly_StuckLowMip.h:22`; default false at `IAnomaly.h:34`. **`lod_popping` is not one** — it is `AnomalyState` (`AnomalyCaptureSubsystem.cpp:284`) but toggles inside its own window, and both its phases draw the object, so the arm budget is benign there.

**Incidence:** 0/46 (`A8742A4A`), 2/25 (`DC8BA0C1`), 1/26 (`5588F6FB`). ⚠ Association only, and **confounded** — `A8742A4A` predates `R1`, so "no pre-roll / no vetoes" is the same axis.

VERDICT: GREEN
api-equivalent cost (not billed on Max): $24.02 | turns: 44 | running total across all briefs: $1,925.64
