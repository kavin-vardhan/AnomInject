# 093-06 — office-robust F1 capture ownership

## Scope and starting state

Implementer for chat's 093-06 brief; Claude Code reviews independently. Binding:
093-05 E1–E5, 093-02 P2/P6/P10, plan A3/F2, Code's plan review section 2,
and the brief's D2 stop rule. Use `E:/IA_BuildCache/_r933/AnomalyInjector`,
`fix/followup-capture`, starting clean at pushed `1bf7054f5b8664f057282ed863a62ab5dbbd60e3`.
No natural-cause code, no merge to master/release/delivery, no tags, and no push
until all J2 gates pass. Job evidence: `E:/IA_BuildCache/_r936`.

## Part 0 — completed before applying J2

Predictions were written first to `_r936/part0-predictions.md`. `_r936/part0.py`
copies the J1 UV sessions, then consistently relabels the first 48-frame event
from `StaticMeshActor_0` onto `StaticMeshActor_12`. The new mask is that other
object's real measured silhouette from the matching J1 normal capture, recoded
to the event's mask value. Camera, dimensions and sample span must agree.
Annotation identity, row target, boxes and mask counts are changed together.
Every RGB image remains byte-identical. Actual/claimed silhouettes overlap by
zero pixels on all 48 frames. No source capture or previous report was edited.

| Reading | PIE | Staged |
|---|---:|---:|
| Original images / events / measured labelled masks | 1152 / 4 / 192 | 1152 / 4 / 192 |
| Original visibility and integrity exits | 0 / 0 | 0 / 0 |
| Wrong-object visibility and integrity exits | 1 / 1 | 1 / 1 |
| Claimed-target median RGB difference | 0.333440 | 0.336060 |
| Frozen visibility threshold | 4 | 4 |
| Minimum actual-target RGB difference | 42.920601 | 39.354332 |
| Mask count/bounds/dimensions and annotation errors | 0 | 0 |
| Changed RGB files | 0 | 0 |

Both unchanged visibility checks reject the claimed target as invisible. The
integrity reader's only failure is the expected below-threshold labelled
picture. Its structural checks pass. The observational label-pixel reader
still exits 0 with an unassessable edge; that is not visibility acceptance.
All files in the four original/donor sessions and all three readers rehash
unchanged. Full results and SHA-256 manifests: `_r936/part0-r1`.

The status block's blanket claim that J1 exercised every can-fail is corrected.
G479 and `client-readme.md` section 7 record the known masked-alpha limitation:
the mask follows the event-state silhouette, so pixels corrupted alpha cuts
away change outside the mask; edges remain 0 frames off. Future regular-line
item: mask the union of the pre-event and event silhouettes and check changed
area completeness. This is not part of J2 implementation.

Both staged proxy runs produced three events because attempt ordinal 0 was
refused as `texture_uniform:pending` (`F1_S_proxy/game.log:4936` and
`Proxy_S_aaoff_marker0/game.log:4933`); the separate TSR run has the same refusal
at line 4786. Exact log excerpts: `_r936/proxy-refusals.json`.

## Harness problem and fix

One fix attempt: the new wrapper compared the existing annotation checker's
empty error list to integer zero, falsely marking structural validity false
after the PIE control had correctly been rejected. Changed only that wrapper
assertion to test list emptiness and reran both modes under `part0-r1`.
The failed attempt and script are preserved in `part0` and
`part0-before-fix1.py`; `harness-fixes.json` records it. No checker, threshold,
gate or prediction was retuned.

## State at the Part 0 documentation commit

No product code, build, engine launch, host/staging setting or fixture content
was changed. This commit closes J1's documentation and missing-control items.
J2's saved WIP, ownership generation, diagnostics, office reader and full
two-fixture PIE/staged proof remain to be implemented and qualified.

## J2 implementation checkpoint — proof pending

Applied the saved nine-file WIP after Part 0, plus its untracked family helper.
Patch SHA-256: E5A64F5F85ACB9982CE5833A197C4A8509FDB612AF9556783A17111477278507.
The completion requires known local-player ownership, freezes request identities and
family/view bindings, splits NotGameView diagnostics, invalidates queued work and
measurement baselines when the display changes, and refuses unsupported ambiguity.
No target-unknown fallback is enabled. Mask files publish with their accepted colour
request. Census invalidation drains retired arms without crediting their old measurements.
Only run summaries gain the additive diagnostic block; no label key was added.
The office reader prints a compact local case line with a check digit and failure counters.
It requires independent picture inspections and real mask/visibility/edge measurements.

Initial host receipts are under `_r936/receipts`: StackOBot head `23098fa`, 201 original
binary/configuration files (2,752,599,340 bytes); Lyra head `caa68c6`, 183 files
(1,237,222,919 bytes). All backup hashes were verified. Original content hashes are recorded.
No host source/binary or staging mutation had occurred when these receipts were made.
Normal/strict builds and the full two-fixture engine matrix remain pending at this commit.

Before engine qualification, source review found that rejecting a colour submission
must still retain its already scheduled RDG readback until the GPU completes. The
completion now marks it rejected, drains safely and discards the result; it cannot
publish or become a baseline. No engine gate had run when this was corrected.
The first harness build found a missing direct AnomalyInjector module dependency
for the public capture header. Harness fix attempt 1 adds it; product modules compiled.

The pre-proof completion assigns display generations from a process-wide serial, so
reopening a PIE world does not reuse generation 1 and the local checkpoint reader
can compare before/after runs. Epochs still distinguish capture runs independently.
The office reader also accepts a whole office_session.json case list, one stdout
line per case, and retains available I-failure counters when a zero-frame run has
no labels file. All case evidence and inspection hashes remain on that host.
The real family predicate header passed 20 CPU checks through a minimal view-type
adapter; eight mutated predicates each failed. These cover all three NotGameView
branches, unknown target, multi-view, both stereo passes, ambiguity and RT view state.
This is supplementary predicate evidence, not engine/pixel qualification.

A final pre-qualification review also added a publication check between each colour
result, its frozen label snapshot and the resolved target-mask binding. A mismatch
in epoch/generation/family/view is discarded through the existing unpaired accounting
before a PNG/label pair can publish. The request-level and RT guards remain in place.
The lower world stamp and upper sample barcode are separate pixel oracles. The new
runtime/editor harness modules use AnomalyBenchGate and are denied to Shipping.
All final engine gates must use the completed publication guard and harness hashes.

## Stopped qualification: selected-viewport label/picture failure

The product remains exactly `f608d56248e4c2ab11515b1fdd8f969cae4ead74` in Source,
Shaders and tools. The primary worktree is still `_r933/AnomalyInjector` on
`fix/followup-capture`; no other branch is advanced and nothing is pushed.

Twenty-two of the 84 planned engine cases pass; one fails; 61 remain unrun under
D2. An additional StackOBot healthy validation of the R3 helper passes. The 26
launches include two retained failed harness attempts. StackOBot completes all
15 primary PIE cases and both exact archived-DLL controls. Lyra completes delayed
viewport, new-window/non-owner draw and all three NotGameView negatives before
the selected-viewport failure. No staged, synthetic-order or pairing-probe leg ran.
`_r936/qualification.json` enumerates every case and its status.

The failed session is `_r936/legs/lyra_P_selected/capture/session_20261014-095057`.
It writes all 360 requested pictures from the correct world with matching sampled
frame barcodes. Four event attempts produce no visible injected interval and no
measured target masks. Ninety-six rows nevertheless contain `labelled=true`,
`visible_positive=true`, `target_pixels=0`, `observable=false`, and an empty mask.
The actual pre-run projection puts the target entirely outside the 942x869 picture
at X -79.365 to -21.432. Example row 35 claims an on-screen box starting at X 23.550
with width 47.100. The original PNG confirms the target is outside the image.
Both visibility and mask-integrity gates fail; edge error is unavailable, not zero.

The fixture's original `DefaultEngine.ini` uses `AspectRatio_MaintainYFOV`.
`AnomalyViewport.cpp:716` stores the camera's FOV as horizontal, then reconstructs
the frustum from viewport aspect. That code and its public header are unchanged
from J1. This is the likely cause, not a completed causal fix. Correcting the
projection/label behavior needs a decision before further product work. No target,
framing, threshold or product source was changed to turn this result green.
Exact values, source references and hashes are in `_r936/stop-review.json`.

## Completed proof and can-fails

| Gate | StackOBot PIE | Lyra PIE | Staged, both fixtures | Can-fail evidence |
|---|---|---|---|---|
| Delayed viewport | 360/360, 4 visible, edge 0, owner update 1 | Same | Not run | Exact J1 DLLs: StackOBot 360 requests, 0 written; Lyra not run |
| New-window + different editor picture | Wrong view 0; 2,477 non-owner rejections | Wrong view 0; 2,508 non-owner rejections | Not run | Exact release DLLs: StackOBot 360 wrong-view pictures; Lyra not run |
| Selected viewport | 360/360, 4 visible, edge 0 | FAIL: 0 visible, 0 measured masks, 96 positive rows | Editor-only case | Cross-world/target/family predicate mutants reject; observed Lyra failure retained |
| Two-client PIE | Both 360/360; independently decoded worlds 2 and 3 | Not run | Editor-only case | Wrong-client PNG copy rejected by office reader |
| Resize | 360 requested, 358 written, 2 dropped; generation +1; stale 5; no stale-size PNG | Not run | Not run | Generation/binding mutants reject |
| Two same-world displays | 0/360; ambiguity count 2,163; accepted ambiguous 0 | Not run | Not run | First-wins predicate mutant rejects |
| Split/stereo refusal | 0/360 each; counts 1,414 / 728 | Not run | Not run | Multi-view/stereo predicate mutants reject; stereo lever is not HMD qualification |
| NotGameView: controller/game-thread/show flag | 0/360 each; counts 1,067 / 1,069 / 1,067 | 0/360 each; 1,072 / 1,067 / 1,067 | Not run | Forced independent sub-reasons; healthy delayed paths pass both fixtures |
| UV/proxy regression | Each 1,152/1,152, 4 visible, edge 0, 192 measured labelled masks | Not run | Not run | Ten copied-capture controls per native regression; source hashes unchanged |
| Synthetic alignment / pairing 100% and 50% | Not run | Not run | Not run | Recipes and controls prepared; no engine claim |

Normal and strict Editor/Game builds pass on both fixtures, zero warnings/errors.
The initial standalone-header passes compile 71 TUs per target; those headers are
identical at the final product head. All 69 CPU commands have their expected exits,
including 18 deliberate mutant failures. The actual family predicate header passes
20 adapter checks and eight mutants fail; Python runs 51 tests. Source lever audit
finds 53 registrations and its 12-case self-test passes. Final binary lever audits
pass for both Editor/Game fixture sets. R3 string review attributes all 1,622 additions;
12 residual records are verified against source/PE bytes, leaving zero unresolved.
ASCII/UTF-16 scanner controls and old-J1 binary marker checks fail as intended.
Across all written sessions, 6,982 label rows contain no ownership diagnostic keys.

Office self-test lines (unchanged reader SHA-256
`7380e31bd272f7819c0d56d4a172a41d7e16d25a2c429b7495c891f51be26579`):

```text
I1.11 PASS r=360 w=360 v=4 e=0 x=0 a=0 #0
I1.11 FAIL r=360 w=0 v=-1 e=-1 x=-1 a=0 #2 I-fail=360,0,0,0,0,3,0,1067,0,0,360,0,0,0,0,1067
```

Copies with an actual other-client PNG, absent telemetry and shifted onset also
fail. These are home reader checks; neither office host is qualified by this job.

## Harness fixes, provenance and restoration

`_r936/harness-fixes.json` records all fixes and preserves failed evidence. The
two engine interruptions attributable to this job's helper were (1) drawing an
extra viewport without its required temporary world-context mapping, fixed once
in R2, and (2) placing the world marker using an assumed frustum, clipping its
reference cell under Lyra's projection, fixed once in R3. Both retries pass.
R3 uses the actual constrained local-player projection; marker codes, sampling
positions and contrast threshold are unchanged. Product editor DLLs are identical
across R1/R2/R3; new monolithic EXE links are separately hashed and unqualified.

Other disclosed fixes: Part 0's empty-list assertion (one); missing helper module
dependency (one); game-only ResizeFrame usage caught before launch (one); an ad hoc
CPU-results display misclassifying mutants then mutant compiles (two); scoped stereo
enum names (one); stale preserved copy timestamps (one); scanner control byte
separation (one); screen-percentage overrides completed before any pairing leg
(one); manual string review of printable IEEE-754 marker constants (one). No gate
threshold or product source was changed after qualification began.

The bench uses process-local PNG/seed 777/30 fps, native order, SVE/Mask/TargetMask/
Pace/RunLog/Marker on, Delivery/FocusGate/AA/auto exposure/GI/reflections off. Primary
schedule is `0 30 24 60 4`, cap 360; regression is `0 192 48 192 4`, cap 1400, ending
at 1,152 ordinary samples. The helper toggles its negative view modes, extra display,
split-screen and resize at sample 100 to 960x720. Lyra uses a process-only GameModeBase
override while retaining its own game instance and viewport client. Synthetic order,
pairing probe and 50% settings were prepared but never executed. No shipping helper
module is enabled; both descriptors deny Shipping and use AnomalyBenchGate.

All launches exited; 38 recorded PID/creation-time identities are no longer alive.
Hosts are restored by receipt: StackOBot detached `23098fa` with all 201 original
binary/configuration files, Lyra detached `caa68c6` with all 183. Original content
hashes match; receipt-owned helper/fixture roots and 19/26 new build files are removed.
All 5,764 files in live staging and all 37 original J1 cook-source files rehash
unchanged. Exact archived-DLL swaps restored all five candidate DLLs after each control;
host originals were restored at closure. Foreign processes and security settings were
untouched. Protected branch/tag refs are checked at closure; no push is attempted.

Unqualified binary archive:
`D:/IntrusiveAnomalies/_binary_baselines/followup-capture-J2-09306-f608d56-4BB0F074-B0F2B30C-UNQUALIFIED`.
It preserves current/retired binaries, matching available PDBs, StackOBot's unchanged
J1 containers, source patch, helper source and evidence manifests. Its files rehash
and a copied one-byte DLL mutant is rejected. Raw captures remain in `_r936/legs`.
The full report is in the 093-06 relay run folder. Decision needed: projection/label
repair scope and resumption of the unrun qualification. No delivery or natural work.
