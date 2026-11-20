# m51 planning revision — Codex review and request to Chat

2026-09-06. **Verdict: CHANGES REQUIRED. No implementation or leg released.**

Chat: I read your `078-01-chat-predictions-review.md`, consolidated the rulings, issued documentation-only briefs 078-02 and 078-03, and read both final Code reports in full. The revisions improve the design substantially, but I do not accept the reports' blanket “C1–C4 done” claims. This is a static review, not new runtime evidence.

## Exact snapshot

- Repository: `D:/IntrusiveAnomalies/StackOBot/Plugins/AnomalyInjector`; branch **m51**.
- Reviewed commit: **760ad9473aa977f2789fb4a43777f0a9d11133c7**, parent **adb0d4d12f3b751bece396499bedd50d00fdeb76**. Earlier planning commit: `b532ddd97910c2357bc35e5bfe342ad21d58f11c`.
- Predictions: `docs/predictions/2026-09-06-m51-release-hardening.md`; SHA256 **885BAEAD60D3E4FDF4B2F528089C67A0738395F6ED5FF8FE06033CBF54475669**. Amendment 2 supersedes specified parts of Amendment 1; the original body is historical.
- Manifest: `docs/predictions/2026-09-06-m51-gate-manifest.json`; SHA256 **52508DBF3EA4468CDE21A9CF538FA822B21C50BE10CCABE882C7212103CC4A08**.
- Journal: `docs/sessions/2026-09-06-078-03-m51-bounded-corrections.md`; SHA256 **E94981B3B6CE0D205614A6990E63B0F7D4D4FCCCB3CFA2C17FAD7DCF1CF27EA8**.
- Actual report: `D:/IntrusiveAnomalies/_mailbox/to_chat/078-03-close-prediction-blockers.report.md`; finished **16:17:36**, session `<uuid>`, **NEEDS-DECISION**.
- Issued contracts: `D:/IntrusiveAnomalies/_reviews/078-02-revise-m51-predictions.brief.md` and `078-03-close-prediction-blockers.brief.md`.

I independently checked the documentation hashes and changed-path list. All changes from `ac13700` are documentation. Plugin source, shaders and tools remain unchanged; no builds, captures, gates, pushes, merges or tags were run in these two briefs. Master remains `ac13700`; the source backup remains `be16977`. The ten untracked owner handoffs are preserved. Code's binary hashes are provenance readings, not evidence that a candidate has run.

## Decisions retained

1. Preserve R1's zero-measured fallback and `anomaly_present`. Keep zero-observable events; retain `vetoed_events` at zero and add the separately defined `zero_observable_events`. Declare the empty-window -1 value change explicitly.
2. Preserve Phase B's predicate and both exposure operands. `target_drawn_pixels` stays diagnostic. A zero-dip row does not currently emit `exposure_dip_scope`; require exclusion-operand diagnostics and a known suppressed drop instead of changing that conditional emission.
3. Reject a 32-tick stale allowance. Unpublished requests expire by game-frame identity, with a pre-admission sweep and a publish-time guard. Published GPU work survives delayed completion. Retire capacity exactly once.
4. Keep requested history, committed receipts, and per-fire observations distinct. A required mask write failure cannot erase its own obligation by downgrading the row. Empty/unmeasured masks do not invent a PNG obligation. A fallback event on an incomplete session fails reconciliation, not an observable-only membership rule.
5. Reject empirical compression-ratio reservations and over-budget admission exceptions. Separate invalid configuration from actual in-capture saturation. Keep 600 requested frames, 60 timing-only warm-up frames, 1080p native, 30 fps and speed_ratio <= 1.05 as the proposed sustained-load bar.
6. Do not send new levers to B8AC65D8. Your original “A-side fills the cap” prediction cannot hold: that binary has no admission cap. Readback delay is not ViewLag; render withholding is not ForceMiss; custom-depth disable is not HideOmitDepthPassSilencing.

## Remaining findings

### 1. The manifest is not yet an executable inventory

The harness catalog has absolute paths, but many recipes contain `...`, `<session>`, prose sequences and only `command_stackobot`. Fixture/order expansions do not supply the missing Lyra arguments, target selection or scheduling. This is more than formatting:

- **M-F4:** `CaptureBench/tools/run_leg_editor.ps1:98` launches UnrealEditor with **-game**, not a PIE session with an editor level viewport. Its name does not establish the required second world/viewport. The actual PIE setup must be a separate documented procedure or planned harness, with the two eligible families independently observed.
- **M-F12 / M-F15:** `run_leg.ps1:224` appends `ExtraExecCmds` **before** `IAI.Capture.Start` at line 231. `obj gc` and `IAI.SetMinScreenCoverage 8` there do not execute during an active event/capture. Deferred bench levers can be armed at startup, but these immediate commands are not deferred levers. Specify a session-index scheduler or an actual console dispatch during capture, and log the effective execution index. Preserve the console/WebSocket distinction in R4.
- **M-W5:** two invocations of `run_leg.ps1` launch separate processes, and the runner stops the process at line 319. That cannot establish a worker crossing FinishRun into the next StartRun in the **same process**. The recipe must keep one PID alive across both sessions and inject the delay near the first session's end, not merely at startup.

**Required correction:** structured script references and argument arrays, explicit per-fixture setup, scheduled actions and expected effective readbacks. Resolve placeholders before the first affected leg. Planned harness changes must have named dependencies rather than being described as existing behavior.

### 2. The stale-arm falsifier has two incompatible definitions

A2.1 and M-R8 retain the identity guard while bypassing stale capacity retirement. But **M-R8c.falsifier** names the same `NoStaleArmSweep` device and says it removes identity rejection, restores bare FIFO and must produce a mismatched row. Those cannot both be true on one binary.

**Ruling:** retain the identity guard. M-R8c checks that a short burst retires stale requests even below the cap; with retirement bypassed, assert the stale backlog/cancellation-accounting failure. Do not demand mislabelled pixels from a lever explicitly forbidden to mislabel them. A separate wrong-pairing falsifier, if desired, needs its own explicit contract. Also compare counts with observed capture opportunities, not an unqualified three game ticks.

### 3. Writer design is still open, and its arithmetic is overstated

A2.2 properly admits the encoder limitation, but calls its reservation exact while excluding encoded output and charging `Job.Record` a flat 64 KiB without specifying an enforced record-size bound. Array payload size is also not automatically allocated capacity. `WriterWorkerCount=2` bounds simultaneous workers, not all queued jobs; saying “N binds and the byte cap is headroom” conflicts with the same section's 16 charged queue slots.

The saturation arithmetic counts 30 jobs/sec while the current writer accepts separate image and mask jobs. With mask output enabled, state which tasks are delayed and charge both task classes. A bounded enqueue wait also does not imply that every arrival drops once the queue fills: some arrivals acquire newly freed capacity, and waiting changes the producer's wall-time rate. Keep the invariant gate, but remove unsupported numerical certainty.

**Ruling:** do not release the writer as designed. Specify charged buffers/allocated capacities and record enforcement, the separate scratch scope, concurrency and queue admission independently, and a complete image-plus-mask workload. The lack of a limit parameter on `IImageWrapper::GetCompressed` proves that interface does not enforce one; it does **not** prove streaming is the only possible bounded design. A verified bound for the pinned encoder implementation or a limit-checked output sink is another route to investigate. No empirical ratio becomes a release default; no global process-RSS cap is required.

### 4. P-C7 does not consume the new declarations

The manifest points at **m49b_pc7v3.py** as runnable. That script still hardcodes Phase B's declared sets (`CaptureBench/tools/m49b_pc7v3.py:24–30`), accepts only paired bank names, and has no manifest/allowlist input. Its bottom-level code prints an overall verdict without returning a failing process exit status. New schema-3 keys cannot pass that checker unchanged; exit 0 is not a pass.

Per-leg declarations are also still incomplete. For example M-F1 says “run_summary additions only,” whereas the common declarations name schema and row changes. M-F6 declares `vetoed_events 8 -> 0` across fixture expansions without pinning the corresponding A-side event count. Several recipes still say frame_indices identical despite declared membership changes elsewhere.

**Required correction:** plan an m51 comparison adapter/checker that consumes a pinned, per-leg allowlist and preserves the existing Phase B checker for its original use. Compare full row/event sets and identities, not only zipped intersections. Prove it rejects an undeclared key, removed row/event and changed protected value. Emit an actual failing status. Resolve each concrete pair's declarations before its leg; do not rely on a prose “except where a ruling changes membership.”

### 5. Both-fixture / both-order coverage remains incomplete

The count “147 expansions” is not the required coverage proof. In the manifest:

- R-ONSET-G7-TIE, R-PAIRING, R-GEDGE and R-OBS have only StackOBot expansions.
- R-M50, R-PB and R-PC7V3 omit Lyra synth.
- M-F5b and the M-W4v validation arm omit synth on both fixtures.

Some device-specific predicates may need fixture-specific counterparts. If so, name the counterpart and its independent reference, or mark the exact arm OPEN with the missing prerequisite. No silent contraction of the operating contract. Expand regression leg families, anomaly variants and control/failure pairs into identifiable runs before counting them as coverage.

### 6. The injection-ledger repair is not explicit enough

A2.3 correctly discovers that current accumulation occurs at drain, but its disposition says only to keep the agreed meanings and derives `observability_complete` from indices that are already in `injected_frames`. If an armed request never drains and its per-event injection membership is never recorded, that event-local check can miss the lost membership even though session completeness catches a hole.

**Required correction:** state the arm-boundary update to the per-event injection ledger, using the frozen `IsFireLabelledThisFrame` membership decision already named in A2.3(d), and persist it independently of drain/write success. Reconcile receipts into observations later. Do not replace that sampler with “any active injection.” Add a known-answer case in which an event's only eligible requested frame never renders: its membership survives, its observation is unknown/incomplete, the zero-measured fallback remains intact, and the session fails integrity. A failed write after a successful measurement must likewise not leave an observable claim tied to a missing image.

### 7. A device that changes inputs is not necessarily a defect injector

Several classifications still overclaim. M-W4's service delay should cause **counted** drops in a healthy implementation; it does not itself cause the specified checker failure “uncounted drop.” M-W5's delayed worker similarly exercises a condition without deliberately bypassing safe joining. M-R5 refers to “disabling the per-fire split” without naming that bypass among the executable actions.

For M-F3b, early lease release alone does not necessarily cause cross-attribution if the request's tag-to-event mapping is correctly frozen and the old component's tag is cleared. It also need not create two simultaneous owners for m50_tag_owner to detect. Separate lease-lifetime checks from attribution checks and identify which protection is bypassed by each can-fail device. Keep legitimate stress/control readings, but do not label them proof that a particular defect checker can fire without the discriminating mechanism.

## Requested Chat response and next phase

Please review these seven findings against the pinned snapshot and issue `078-03-chat-predictions-review.md` under `D:/IntrusiveAnomalies/_reviews/`. Agree/disagree with concrete source evidence or counterexamples. In particular, assess the bounded-writer alternatives and the exact injection-ledger repair. Preserve the existing OPEN arms: applied tick pin, real decoupled host, mixed Nanite, editor-view/A-side availability, and the missing discriminating controls.

I recommend **against blanket approval of implementation commits 1–13**. After the relevant recipe and schema-delta corrections are embodied in the plan, start with a narrow implementation brief for **F1 effective-mask resolution**, then independently review the request-identity/lifecycle changes before expanding into writer/schema work. This is a sequencing proposal, not authorization already granted. No capture leg is released by it.

The next correction should edit the affected manifest records and add a short dated disposition, not append another complete milestone narrative. I own the overly broad planning scope here: two rounds added substantial text without making the recipes executable. The next unit of review should be a small, testable contract whose actual outputs can be checked directly.

Mailbox is idle, queue empty, watcher PID 14448. No new brief has been queued after 078-03. Chat remains reviewer; Codex owns technical rulings and the queue. No owner/client semantic decision or office action is requested.
