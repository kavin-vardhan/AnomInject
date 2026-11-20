# Session 078-03 — 2026-09-06 — m51 bounded prediction corrections (C1–C4)

**Scope:** read-only inspection/hashing plus documentation-only edits on branch `m51`.
⛔ **NO source/tool/fixture/engine edit · NO implementation · NO build · NO cook · NO test or
self-test · NO capture · NO staging · NO branch/worktree change · NO push · NO merge · NO tag · NO
office data or action · NO mailbox maintenance.** Fresh session under the operating contract (078-02
reached ~74 % context). The source audit and the milestone were **not** restarted.

---

## 1. State verified at open — matches the brief exactly

| item | expected | found |
|---|---|---|
| branch | `m51` | `m51` |
| HEAD | `adb0d4d12f3b751bece396499bedd50d00fdeb76` | same |
| `master` | `ac13700d99fe116056f5aa11611f8931a563a3f2` | same |
| source backup | `be16977c0802f57457ec68edcf8d0f96c12c13d9` | present |
| predictions SHA256 | `725CD30F…D55B50B` | same (87,988 B, 968 lines) |
| working tree | ten owner handoffs untracked, no tracked WIP | same |

**No discrepancy to preserve.** All eight StackOBot provenance hashes in A1.13 were **recomputed**
and reproduce exactly; staged exe == archived A-side byte-identical.

---

## 2. What was delivered

**`docs/predictions/2026-09-06-m51-gate-manifest.json`** (new) — the single execution inventory:
40 recipes, 147 fixture/order expansions, 16 harnesses with CLI contracts read out of the scripts,
3 planned tools, 13 planned levers, full provenance.

**AMENDMENT 2** appended to `docs/predictions/2026-09-06-m51-release-hardening.md`, explicitly
superseding A1.5's completeness conjunction, A1.6's stale expiry, **A1.7 entirely**, A1.8.1's
falsifier scope, A1.9's saturation device and three falsifiers, A1.9's "no separate manifest file",
A1.11's leg classes and A1.13's deferred Lyra hashes. Everything else in AMENDMENT 1 stands.

⚠ **The amendment is 314 lines against the brief's ~250 guidance.** Reported rather than met: the
remaining paragraphs each carry a mandated element (C1's six-rule specification, C2's reservation
arithmetic plus the codec source reason and its OPEN prerequisite, C3's five-part correction, C4's
four falsifier corrections, the eight self-check cases). Cutting further would have dropped required
content; padding the manifest with design rationale would have given it two jobs. Neither was done.

---

## 3. The measurements that settled each correction

Every finding below is a **source read of the current tree or the local engine**, made this session.

**C1 — the identity is not in the code today.** `FAnomalySveCapturer::ArmWanted` stores only the
RequestId (`TArray<uint64> PendingWanted`, `AnomalySveCapturer.h`); `ConsumeWantedForPublish`
(`AnomalySveCapturer.cpp:45+`) pops `PendingWanted[0]` FIFO with **no frame comparison at all** —
`FamilyFrameNumber` is passed in and used only for logging. A1.6 spoke of *keeping* a publish-time
check that **does not exist**. The identity is nonetheless sound to add:
`ISceneViewExtension::BeginRenderViewFamily` is a **game-thread** callback (`SceneViewExtension.h:141`,
*"Called on game thread when view family is about to be rendered"*), invoked at
`SceneRendering.cpp:4590`; `GFrameCounter++` is at `LaunchEngineLoop.cpp:5568`, **after**
`GEngine->Tick()` at `:5363`. ⇒ a healthy arm and its publish share one `GFrameCounter`.
✅ A1.8.1's `WithholdWorldRender` mechanism is **confirmed**: `GameViewportClient.cpp:1696-1707` gates
`BeginRenderingViewFamily` on `!bDisableWorldRendering`, and the plugin's own `BeginRenderViewFamily`
is invoked **downstream** of that gate, so nothing publishes during the burst.

**C2 — the codec API cannot supply the bound, and the exact reason is a signature.**
`IImageWrapper::GetCompressed` (`…/ImageWrapper/Public/IImageWrapper.h:164`) is
`virtual TArray64<uint8> GetCompressed(int32 Quality = 0) = 0;` — a whole-buffer return **by value**,
no size hint, no maximum, no chunked/streaming variant, no allocator injection, no way to query the
compressed size beforehand. ⇒ **an a-priori byte bound on encoded output is not obtainable through
this interface**, and no `EncoderReserveFactor` may stand in for one. 🚨 Worse:
`AnomalyPreviewCapture.cpp`'s `EncodePixels`/`EncodeGray8Png` bind the returned temporary to a const
reference and then `Memcpy` it into `OutBytes`, so **the encoded image is resident twice** in
plugin-visible memory. Removing that copy is a strict improvement available today.
✅ What **is** boundable, exactly: `Job.RawBytes` (`W*H*BPP`, moved in and held for the job's life),
`Pixels` (`W*H*4`, `AnomalyLabelWriter.cpp:248`), `Scaled` (`OutW*OutH*4`, `:347`, only when
`OutputHeight != 0`). A mask job has **no** second buffer — `EncodeGray8Png` calls `SetRaw` with
`ERGBFormat::Gray`. ⚠ And the writer has **no pool of its own**: `Enqueue` fires
`Async(EAsyncExecution::ThreadPool, …)` per job, so "bounded writer, N=2" is a real implementation
change, not a parameter.

**C3 — A1.5's blanket membership claim is wrong twice, and the two errors point opposite ways.**
`AccumulateFrameEvents` is called from the **drain** (`AnomalyCaptureSubsystem.cpp:3998`) and the sync
path (`:4282`) — only for frames that produced a readback result — so an **armed-but-never-drained**
frame is in **neither** ledger, and `injected_frames` is therefore *not* a faithful requested ledger
today. Meanwhile the writer is fire-and-forget with accumulation **before** it, so a
**drained-but-write-failed** frame *is* in `injected_frames` **and carries an observability verdict**
with no image on disk. The R1 fallback (`:5641-5670`) sets `bObservabilityMeasured = (Measured > 0)`
and, at zero measured, leaves `affected_frames == injected_frames` ⇒ **on a zero-measured incomplete
session `affected_frames` contains indices with no image.**
✅ Mask policy located: `Snap.MaskFileRel` is assigned **at arm** when `bTargetMaskEffective`
(`:4196-4198`) — that is *requested* — while the effective policy is *"the PNG exists iff it has
content — no all-zero PNG is ever written"* (`:3011`, `:6472`). So "mask iff armed" would fail every
legitimately empty frame.
✅ Membership sampler named: `IsFireLabelledThisFrame` (`:4620`), sampled once per fire per armed
frame at `OnWorldTickEnd` into `Snap->FireLabelled` (`:4580-4584`) and frozen; the observable predicate
at `:3880` consumes exactly that bit.
✅ Write order is already correct: annotation `:5002`, run summary `:5240` — **run_summary is last
today**; what is missing is tmp+rename and the success checks.
✅ Second-context isolation is impossible: `AnomalySveKeyRing` is a **namespace singleton** (free
functions, one ring per process) and `AnomalyStencilTag` applies tags directly to actor components
through free functions. ⇒ explicit `StartRun` refusal is the only sound disposition.

**C4 — the inventory gaps were real.** `m51_bbox_oracle.py` and `m51_reconcile.py` **do not exist**
(verified). `tools/verify_integrity.py` exists on `be16977` **only**, CLI
`<directory> [--reference] [--integrity-only] [--pixel-delta] [--min-changed-pixels]`. Lyra's five
files were hashed at **full length** and `UnrealEditor.modules` resolved to the actual DLL names
(`BuildId <uuid>`).

---

## 4. Three A1.9 falsifiers were WRONG, not merely missing

This is the part most likely to be lost if only the manifest is read.

- **M-F3b** — `NoTagRetire` **holds** leases, so it *prevents* the reuse the recipe needs and cannot
  manufacture cross-attribution. The device is `IAI.Bench.ForceEarlyTagRetire` (release at fire-end
  ignoring `ArmsIssued == ArmsResolved`). `NoTagRetire` stays **M-F3's** exhaustion falsifier.
- **M-R5** — forcing **both** fires pending makes both unknown under *either* the per-fire or the
  frame-wide rule, so it cannot discriminate between them. Only a single-fire device can.
- **M-F4** — synth tick order is **not** a bypass of the view binding.
- **M-F11 and M-F10 have NO defect injector at all**, and are recorded as `MISSING` in `open_arms`
  rather than dressed up as covered.

---

## 5. What this session did NOT do

⛔ No implementation, no leg, no build, no cook, no staging, no push, no tag, no worktree change.
⛔ No source, shader, tool, fixture or engine file was edited — verified: `git diff ac13700 -- Source
Shaders tools` is **EMPTY**, and the only refs that moved are `m51`'s docs commit.
⛔ `_mailbox` untouched; the ten owner handoffs remain untracked and unstaged.
⚠ **Still OPEN and not converted:** a bounded encoded-output guarantee (C2's item (b)); M-F11's and
M-F10's defect injectors; M-F4's A-side (no editor-target archive exists for either fixture, and a
packaged exe cannot host PIE); and AMENDMENT 1's four open arms.

---

## 6. Next

The reviewer's written approval of AMENDMENT 2 **and** the manifest. No leg and no implementation may
start until a follow-up brief releases execution. Chat's stated ceiling for the next pass remains
**approved for commits 1–13, no legs**.
