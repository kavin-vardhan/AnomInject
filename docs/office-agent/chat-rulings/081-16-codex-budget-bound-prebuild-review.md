# 081-16 — pre-build stop: the proposed closure memory bound is not a bound

2026-09-23. **NEEDS-DECISION.** Read `081-15-chat-ruling-budget-bound-build-2.md`
in full. Source inspection contradicts its structural memory premise. No default,
diagnostic, clock, admission or harness implementation changed. **No build or
capture ran; the one-build allowance and all fixture attempts remain unused.**

**Chat review REQUIRED** to resolve this finding before build 2. **Chat review
NOT REQUIRED** for the completed authorized source audit, evidence preservation
and safe boundary documentation. The next mandatory checkpoint is Chat's
disposition of the bound below. Stage 3 remains held; no Code brief or merge.

## 1. What is accepted and what fails

The accepted completion-clock implementation is still source
`5649a6dddbb46fa32c4b056db92d13e8e37bb090`, Game **E8E2E43F**. The full-yield
Stack1080 bar is now Chat-mandated; partial coverage is refused. Original
081-15 audits and outcomes remain unchanged. Chat's dated classification of the
64 MiB specification as the **second Chat spec defect in this family** is
recorded here; this supersedes the earlier report's request to accept partial
coverage. No implementation-escape attribution or additional pilot defect count
is inferred from this audit.

The requested **11C + 9M**, plus one colour per additional open phase, is not a
structural bound on this source. A legal source-level state retains **13 colours
and 16 masks with only one phase**, while the worker runs after each completion
and correctly continues waiting. This fits the old64 MiB budget at720p and
exceeds the proposed bound. It also fits the proposed256 MiB budget at1080p and
exceeds the proposed1080p bound.

**Scope of finding:** this is a counterexample to the bound, not a newly observed
runtime high-water and not proof of a buffer leaked after its final use. The
source's pending work needs a different memory model. I did not label ordinary
all-frame retention a defect merely because future demand filtering could save
memory; the ruling explicitly parks that optimization for v2.

Ruling1(b) allows a different *derived structural bound*. I cannot substitute
another fixed buffer count honestly: neither completion ingress nor mask ingress
enforces one. Ruling1(c), and the owner's instruction to stop before building if
retention exceeds the bound, make this a pre-build decision. In the narrower
"after last use" wording, that specific leak has **not** been established; the
stop is the independently established bound/qualification contradiction. Simply
logging `MaxBytes` as the closure bound would make high-water <= bound tautological
and silently change the requested gate. I have not done that.

## 2. Source proof

Line numbers refer to the exact frozen-source copies in `081-16-evidence/`.
Their content matches built commit5649a6d and feature head after CRLF/LF
normalization; the manifest preserves both blob and frozen-file hashes.

| Site | What the code establishes |
|---|---|
| `AnomalyChangeStage.cpp:157–161` | Reservation checks only remaining bytes. No count or cursor-distance admission limit. |
| `AnomalyChangeStage.cpp:184–211` | Each accepted colour completion can retain its full allocation before scheduling the serial worker. |
| `AnomalyChangeStage.cpp:215–230` | Frozen masks reserve bytes independently of colour completion. Nine is not a mask limit. |
| `AnomalyChangeStage.cpp:172–181` | The helper counts received notifications beyond its argument; eight is a returned-count saturation, not a queue capacity. |
| `AnomalyChangeStage.cpp:291–315` | A closing phase with `Cursor <= Phase.Last` suppresses the four-completion gap rule. Its eight completions must be **beyond Phase.Last**, not beyond the cursor. |
| `AnomalyChangeMeasurement.cpp:98` | `Phase.Last` advances through the whole labelled phase. K=4 caps required measurement rows, not phase length or pending admissions. |
| `AnomalyChangeStage.cpp:254–259`; `AnomalyChangeMeasurement.cpp:222–239` | Work is asynchronous; ingress may also proceed while pixel arithmetic releases the lock. Scheduling overshoot is another uncapped count, even if a threshold was reached. |
| `AnomalyCaptureSubsystem.cpp:1396–1405`; `AnomalyAsyncWriter.cpp:18–28,64–79` | Mask admission precedes its writer job. Colour notification follows encode/write. Independent async jobs do not establish a maximum mask lead of nine. |

Five seconds is a timeout **checked by the worker**. It is not a rate limit, a
buffer count, or a bound on time before a phase starts closing. Converting it to
a fixed number of buffers would require additional assumptions about issue
rate, phase length, writer completion ordering and worker scheduling that this
source does not enforce. The requested unchanged byte-only admission and clocks
do not supply those assumptions.

### Reproducible counterexample, with a responsive worker

1. Cursor is SI3. Previous SI2 has a retained delivered colour. There is one
   labelled phase3..18 (16 frames); GT closes it, starting its five-second clock.
   SI3's colour notification is missing/delayed. No later colour completed before
   phase closure, so the ordinary four-completion gap did not advance this head.
2. Masks3..18 are admitted and available. All relevant observed/sealed flags
   are true. No phase-reference colour has yet been acquired because onset SI3
   has not finalized. Retention is one previous colour plus16 masks.
3. Before the wall deadline, colours4..15 arrive. Run the worker **after every
   callback**. All12 are later than the cursor; all are <= Phase.Last18.
   `ColourCompletionsAfterLocked(18)` remains zero; closure is not terminal and
   `bAwaitingClosure` disables the gap rule. SI3 is not ready, so Work breaks.
4. The stage now retains13 colours and16 masks. Every reservation succeeded.
   Additional-open-phase allowance is zero. The proposed bound is exceeded.

| Resolution | C bytes | M bytes | Proposed11C+9M | Legal13C+16M state | Byte cap |
|---|---:|---:|---:|---:|---:|
|1280x720|3,686,400|921,600|48,844,800|**62,668,800**|67,108,864 (existing)|
|1920x1080|8,294,400|2,073,600|109,900,800|**141,004,800**|268,435,456 (proposed)|

This is a legal notification schedule, not a claim that a particular gate or
host must produce it. It needs neither indefinite worker starvation nor an
allocation beyond the cap. A missing SI3 notification is already an accepted
can-fail mechanism; the source permits phases longer than that can-fail's
two-frame recipe. Receipt/epoch/geometry validity can all remain unchanged.

Nonmutating replay: `C:\Python313\python.exe _reviews\081-16-audit.py --verify-model`.
Initial evidence creation refuses overwrite. Saved exact trace: `source-bound-audit.json`.
The Python model checks the actual C++ predicates via source guards and emits
all12 decisions at each resolution. **It is not a compiled C++/Unreal test.**

### The observed11-colour arithmetic is not an ownership census

COUNT_FAIL's40,550,400-byte peak is indeed11 x3,686,400. But720p masks are a
quarter of that colour size: the same total could be10 colours+4 masks, or
9 colours+8 masks. Previous diagnostics did not record simultaneous ownership.
The number alone establishes neither11 retained colours nor a worst-case bound.
The new refusal-time ownership diagnostics remain useful and authorized in
principle; they are held with the build pending this finding.

## 3. Release audit and the actual invariant

I checked the normal release sites rather than presuming a leak:

- Finalize releases the previous colour, releases the current mask, and moves
  the current colour into Previous (`AnomalyChangeStage.cpp:427–431`). The latter
  is the existing all-frame adjacency policy, not the deferred v2 optimization.
- Phase references release at window3 (`AnomalyChangeMeasurement.cpp:288`), or
  at closing-phase finalization for shorter phases (`:146–153`).
- Run closure releases Previous (`AnomalyChangeStage.cpp:326`); epoch reset
  closes/persists before resetting (`:482–490`). Temporary arithmetic references
  are scoped to MeasureLocked. No extra persistent pixel owner was identified
  in those paths.

These checks do not prove every possible concurrency/lifetime property. They do
show that the bound counterexample does not require a missing release. Eventual
retained0 in old captures is preserved and does not establish a peak bound.

The supported accounting relation is the sum of **actual admitted colour
allocations (shared phase owners counted once)** and retained mask allocations,
including the worker's current item, constrained by `MaxBytes`. Equal-sized
buffers let one express a particular state as `Ncolour*C + Nmask*M`; the clock
does not fix those counts at11 and9. First-delivered sizes also cannot certify
later larger allocations if extents change. This is an accounting model, not a
new implementation or a proposed timeout/admission change.

The memory-envelope documentation must distinguish **m55 retained payload**
from total capture/process RAM: the CVar explicitly excludes the writer pool;
pending metadata, receipts, rows, compression and other pipeline allocations
are not all covered by that byte reservation. A256 MiB payload cap would be
true after implementation; a256 MiB whole-capture RAM ceiling would not.

The proposed formula evaluates to48,844,800 /109,900,800 /195,379,200 /
439,603,200 bytes at720p/1080p/1440p/4K respectively. Those arithmetic values
are correct but **not worst-case guarantees**. No1080p coverage is newly
established here;1440p/4K remain unexercised.

## 4. Pending execution and inherited limits

No build2 prediction was promoted from this false premise; no runtime result
was fitted or reclassified. The original ordered program remains pending:

1. Four Stack1080 twins,20/20 each,zero budget refusals anywhere,invariants0,
   latency and high-water against the **resolved** memory criterion. First valid
   failed measurement stops; no retry or further budget raise.
2. LyraG270 at300 frames under all three081-14§3.4 branches and full required
   pair yield. All three fixture attempts unused.
3. Remaining inventory:24Stack720 measurement/finalization,24identity/order/
   budget/lifetime,2actual ChangeMaxBytes1 controls,2NoHold under theG2 policy,
   12legacy comparison legs,2Lyra720 twins, plus4count/wall can-fails. G270 is
   one of the original67, so do not run/count it twice: **75 total candidate
   legs =4new1080+67inventory+4can-fails; all75 UNRUN on build2.**

E8E2E43F readings earn no build2 credit. The approved256 MiB default and
diagnostics are unimplemented, not withdrawn; the single build is unspent.
Full predictions must precede the eventual build after this contradiction is
resolved. GREEN would auto-release Stage3 per081-03; this report is not GREEN.

All historical coverage limits remain verbatim in081-15§7 and the unchanged
`081-08-evidence/coverage-limits-verbatim.md`, with their existing dated
amendments. In particular: Stage1 accepted36,24Lyra historical legs dropped by
owner decision; previous accepted NoHold/Lyra720/teardown and other Stage2
readings keep their original binaries; real owner replacement and separate-run
stale work remain unexercised. Preserve FPS21.5vs30 on evidence-OFF MAIN4 for
explanation before cost; delay3 client paragraph and0,3,6,9,12 selftest;
deferred positive_frames semantics and Code client-reader review; oracle scope
limited to arithmetic/transport, peak machinery unchanged. Stage3 cost remains
Stack-only pacing-offABBA720/1080,<=+1ms game thread AND unchanged writer
throughput within uncertainty, otherwiseUNRESOLVED, with required-pair yield,
refusal reasons,latency and the resolved memory criterion.

Keep high-water priors17.5/19.4/39.4/49.8/66.4MB beside future observations.
Demand-filtered colour retention and GPU work remain futurev2. The budget
derivation gotcha remains applicable, with this addition: a wait threshold is
not by itself an admission/retention bound. No release or resolution guarantee
is inferred from the numeric formula alone.

## 5. Preservation and boundary

Only source review, a source-derived offline counterexample, and documentation
were performed. No source implementation, build,capture,cook,staging,swap,merge,
tag,Code brief,office or owner-app action. Two audit-helper assertions were
corrected: the first compared live repository state to historical `ref_delta`
metadata; the second compared LF git blobs to CRLF working-file snapshots.
The helper now compares actual state fields and normalized source content,
preserving separate exact hashes. The completed preflight was retained during
the source-only resume. No actual repository/source-content mismatch or runtime
measurement/harness failure occurred. The model's exact totals above correct
the preliminary mental arithmetic in the progress update (62,208,000). The
boundary checker also needed to allow the same feature-reference delta in the
Lyra worktree's shared Git repository; its detached HEAD and files did not move.

`preflight.json` and `postflight.json` verify repository state, prior artifacts,
bank files and all141 prior binary/container/archive hashes. Main restoredm51
53bf725; existing E8E2E43F Game/Editor remain despite m51source. Lyra remains
detachedcaa68c6 with original9modules; candidate11module archive is preserved.
CaptureBench,master,m51 and owner untracked handoffs are unchanged. Only the
feature branch's two audit documents advanced in commit **8f0f831**, pushed to
origin; exact full commit/ref delta is recorded in postflight. Built
implementation remains5649a6d.

CURRENT preserves all previous bytes, including unrelated Blender/video work.
No owned process or delegated task is in flight. Same coherent workstream;
no fresh-session recommendation. Codex implements,Chat orchestrates,Code
independently reviews afterStage3. Read this report FULL,then the ruling FULL,
the source-bound audit/guarded source copies,journal,and postflight before
continuing. A preservation claim is valid only when postflight saysPASS.

## Questions for chat

1. The source disproves11C+9M as a structural bound. May it instead be explicitly
   named a **reference envelope/qualification threshold**, retaining the strict
   high-water gate, while the actual hard payload ceiling is256 MiB? That
   preserves the accepted clock/admission semantics and allows an empirical
   qualification attempt, without promising worst-case full yield. Otherwise,
   please specify the stronger retention/scheduling contract needed for a true
   smaller bound; that would exceed this ruling's "nothing else changes" scope.
2. Confirm that field/client memory claims describe m55 admitted payload and
   measured recipes, not a total-process RAM ceiling or a structural guarantee
   of full1080p/1440p coverage. Then release the still-unused one build and the
   same75-leg fail-fast sequence under the corrected definition. No phase-clock,
   admission,full-yield,fixture or retry-policy waiver is proposed.

VERDICT: NEEDS-DECISION
