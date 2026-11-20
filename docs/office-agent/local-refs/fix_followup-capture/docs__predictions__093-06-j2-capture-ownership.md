# 093-06 J2 capture ownership — predictions before all new engine legs

Branch `fix/followup-capture`, J1 base `1bf7054`, Part 0 documentation `59b2fa1`.
The saved nine-file WIP and its untracked family helper were applied cleanly after
Part 0. Patch SHA-256 E5A64F5F85ACB9982CE5833A197C4A8509FDB612AF9556783A17111477278507;
helper EC7ED4B2AA059A404AEA335BFB1C44FA9A55492BA6EFEBCA36C8B7E9C1307948.

## Scope and fixed measurements

No natural-cause code or label-schema changes. Activation only participates;
GT resolves exactly one local player/display before every request. Unknown
targets are deferred/refused; no target-unknown/game-flag fallback is enabled.
Freeze run epoch, generation, scene, target, local player and controller/view
identity. All consumers bind one unique family and view index before dequeue
and require that binding on RT. Split-screen, stereo and multiple same-world
displays are refused and counted. Generation changes invalidate pending work
and baselines. An already accepted, paired publication is immutable.

Primary identity cases use PNG, seed 777, 30 fps, SVE=1, Delivery=0, Pace=1,
Mask=1, TargetMask=1, RunLog=1, FocusGate=0, native AA-off with Lumen/GI/
reflections and auto exposure disabled. Config `0 30 24 60 4`, cap 360,
requires at least three visible events and exactly 360 requested/written
frames in every healthy case. The initial uniformity-pending refusal may
consume one attempt; the positive minimum is unchanged. Extended regression
uses the J1 Config `0 192 48 192 4`, cap 1400 (1152 ordinary samples).

Use unchanged J1 target RGB/noise thresholds and label-pixel reader. Every
qualifying AA-off onset/offset is 0 frames off; each labelled row has a
same-size measured nonzero mask with matching count, bounds and event ID.
An unassessable pixel-reader note is not a visibility pass. The game-only
barcode independently proves picture origin; no ownership counter substitutes
for pixels. Declare arguments and fixture metadata per leg before launch.

## Required matrix

Run on StackOBot and the Lyra worktree, PIE and staged where the mode exists.
Selected-viewport/two-client PIE are editor cases; staged counterparts use the
single game display and separate client worlds. All engine launches require a
fresh idle receipt, foreign processes untouched, and the 50 GiB disk floors.

| Gate | Positive prediction | Required can-fail |
|---|---|---|
| Delayed viewport | Owner target unknown at start, later update, 360/360 right-view pictures, no ambiguous acceptance | J1 E41ABF93 on the rebuilt late-viewport helper drops every requested picture |
| New-window + realtime editor view | Different editor picture actually renders; wrong-view 0, non-owner rejections >0, 360/360 | Release 218BA4C7 captures wrong-view pictures under forced editor draws |
| Selected viewport / two-client PIE | Each capture uses its world's one local display; all requested pictures from that view | Cross-world/target binding mutants rejected; forced foreign draw fails old source |
| Resize/reopen mid-run | Owner generation increases, stale pending requests/results rejected and counted, requested = written + dropped, no stale accepted | Frozen-generation binding mutant rejected; forced held old-generation result rejected |
| Ambiguity / single-view scope | Two same-world game displays, split-screen and stereo refused/count >0; accepted ambiguous 0 | First-wins owner-selection / multi-view predicate mutants fail |
| NotGameView | Healthy path captures all requested samples on both fixtures/modes | Forced negative controller ID, cleared Game show flag and cleared game-view flag refuse and increment their own sub-reason |
| J1 regression | UV/shared proxy legs, measured masks, label-pixel gate and native edges retain 0-frame error | Frozen target, wrong-object, +/-1 edge, missing/blank/wrong-count mask and erased marker copies reject |
| Alignment | Identity/mask pairing remains correct in native and synthetic order; native-only pixels | Cross-family/sample and unbound-arm controls reject |
| Office checkpoint reader | One short PASS line for a qualifying home session, one FAIL line with I-failure counters for a failing home session; missing evidence never reads zero | Wrong-view/shifted-label/missing-telemetry controls fail |
| Build/hygiene | Normal and strict Editor/Game, meaningful CPU suites, source/binary lever audits, attributable string changes, archive rehash | Existing compiler/audit/ownership mutants remain failing; archive-byte change detected |

The archived EXE identities name the StackOBot controls. If a Lyra staged
control must be rebuilt, retain the exact corresponding J1/release source and
record its new binary hash; never claim a rebuilt binary has the archived hash.
J2's final proved product tree and all binaries are frozen before qualification.

## Stop, receipts and delivery

Product gate failure, needed out-of-scope change, idle timeout at four hours,
security block or disk floor fires the brief's stop rule. Own harness defects
get at most two disclosed fixes with their failed evidence retained; never
retune thresholds. Restore hosts and staging from exact-path/hash receipts.
Only fast-forward push `fix/followup-capture` after every required gate is green.
Office qualification still requires the owner's local checkpoint I readings;
the actual prior office outage cause remains unidentified.

### Pre-launch harness and provenance completion

The rebuilt harness adds a constant five-cell world stamp near the lower-left
corner: two contrast references and three bits encode the independent PIE world
instance (instance + 1), or 7 for the single staged game world. The existing upper
barcode still encodes engine frame identity. Decode every written PNG: both the
expected world's stamp and the same-sample barcode must agree. The fixed contrast
threshold is 40, as for the existing barcode. No capture owner counter drives either
marker. This distinguishes client worlds as well as editor/game pictures. Twenty
or more distributed inspection records and their image hashes feed the office reader.
Both new harness modules use the repository's one AnomalyBenchGate, and their
module descriptors deny Shipping. Audit their actual source in an additive source
projection for binary cross-checking; do not hide helper strings from the scan.
The first strict StackOBot pass proved all headers at 744b36d. Headers are identical
at 33106f5; the final strict pass rechecks changed C++ with the saved header proof.
