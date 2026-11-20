VERDICT: GREEN

Pull `release/2026-10-05` at `23098fa7415f148e935acbd4a97e18da04e6c46c`.
Only behavior change from `d660b5d`: temporal On/Off/Hide transition defaults are **0/0/0**; explicit CVar overrides still work.

Builds on the named detached host: normal Editor/Game **0 errors / 0 warnings**; strict Editor/Game **0/0**, with **68 independent header TUs per target**. Strict settings restored. Lever audit **53/53**, self-tests **12/12**. String scan against `C7FB5212` found only the three requested semantic help-string replacements; PE metadata accounted for, **0 unresolved strings**. [Build evidence](E:/IA_BuildCache/_r917/strict-23098fa/summary.json), [artifact evidence](E:/IA_BuildCache/_r917/artifacts.json).

| TSR shared-proxy leg | Fires | Labelled frames | Temporal reasons | On/Off/Hide | Missing measured masks | Annotation = labels | Baseline frame sets |
|---|---:|---:|---:|---|---:|---|---|
| PIE | 8 | 384 | 0 | 0/0/0 | 0 | PASS | Exact match |
| Staged | 7 | 336 | 0 | 0/0/0 | 0 | PASS | Exact match |

Exactly two engine legs. PIE retains one `pie_end_settle` frame per event; staged transitions are empty. Injected and affected frame sets equal the retained `C7FB5212` captures for the same schedule, without shifting indices. Every labelled mask was read from pixels; every event visibly changed the target and all 20 co-users stayed unchanged. Three fresh idle samples preceded each launch; the two-second watch observed no foreign editor. [Two-leg evidence](E:/IA_BuildCache/_r917/two-leg-results.json), [restoration/rehash evidence](E:/IA_BuildCache/_r917/prepush-audit.json).

Tests: window **316/0**, exclusion **244/0**, PIE-settle **20/0**, mask-serve **15/0**, annotation **5/5**. Old defaults and ignored-override mutants each fail three checks; the added test restores explicit **3/40/1**. Retained old captures fail the zero-transition gate, and changed labels/zero-pixel masks fail the new readers. The retained shared-source mutant fails all eight events. Foreign-editor, unexpected-string and corrupted-archive controls fail as intended. [Pure tests](E:/IA_BuildCache/_r917/pure-results.json).

Push: only `release/2026-10-05` → `23098fa7415f148e935acbd4a97e18da04e6c46c`, new remote ref, no force. Remote verified; no other branch or tag changed. Release worktree and detached host are clean. [Push receipt](E:/IA_BuildCache/_r917/push-receipt.json).

Archive: `D:\IntrusiveAnomalies\_binary_baselines\release-2026-10-05-23098fa-218BA4C7`. All six files re-hashed against the build and live host after the legs:

| Binary | SHA-256 |
|---|---|
| `StackOBot.exe` | `218ba4c79bb2bf6d9e14706772c04b3c53027cc05b84d4ab100a3349b650b7d6` |
| `UnrealEditor-AnomalyBench.dll` | `9f967665827cd1358b5f914466c36ecc45e47e5e972c749aa0221283bc92dfce` |
| `UnrealEditor-AnomalyCapture.dll` | `470ebe5f6c5d8fa11d9872392035c62c2bab2480aee462a4d4d58ac59e8a5be3` |
| `UnrealEditor-AnomalyControlServer.dll` | `0c9c329bedf22be975a09042eb242a21cda726fcab63553240a46685b1dd8cf1` |
| `UnrealEditor-AnomalyInjector.dll` | `45a1512f0c7b59e321d8de97dd03e23b9bee175f503f974fb5a200f99b5a1b90` |
| `UnrealEditor-AnomalyShaders.dll` | `43d78d878cd1e8cd41b23db1fb747fc5c6bd6a103cb5089bb4fb984b1afc47db` |

Baseline EXE SHA-256: `c7fb52129a7007b4c7473b1d5009b9f49cf1bbc1b16259859fd4ae180704aede`. Standalone validation executables remain in scratch; hashes:

| Scratch executable under `_r917/pure` | SHA-256 |
|---|---|
| `exclusion_selftest/test.exe` | `02e660e362b5d40a4dc2bac6fde831a7d05429d4fb914dd6ce8ebd3485cdee03` |
| `ignored_override/test.exe` | `3245b246b5e29a6cce8cf254c65e647f48e6830ee196e5b23312b3a6fa21578b` |
| `m52_window_selftest/test.exe` | `2fa46d071e7f5cad5ec1cc3b92210b2b54f41bb333e00791c9eac5b5881f23fc` |
| `mask_serve_selftest/test.exe` | `adce7f66cb0d8c18921fc5ad6803f5c69975d2072dabdc835f9e06a2429469a9` |
| `old_defaults/test.exe` | `822e6788b635adc0109745d3452675ade67e9d7ef5b1fb6463f742f5ab8e85cf` |
| `pie_end_settle_selftest/test.exe` | `507f80307e92eaf646d10955bf05ae4460b543c1dff89e2d62bbf6124f249287` |

Existing engine launcher: `D:\UESource\UnrealEngine\Engine\Binaries\Win64\UnrealEditor.exe`, SHA-256 `5fcbbf4fe39adae10d1d67a3b21e998723ae07ed258c0fc7fa4103b029b6381b`.

Settings used in both disposable processes: `-IAIBench`; seed 777; shared fixture `CB_ProxyDetail`, target `StaticMeshActor_77`; capture config `0 192 48 192 8`; PNG limit 3000; 30 fps; requested height 1024. Commands: `r.DefaultFeature.AutoExposure 0`, `r.EyeAdaptationQuality 0`, `CaptureBench.Marker 0`, `IAI.Capture.SVE 1`, `IAI.Capture.Delivery 0`, `IAI.Capture.Pace 1`, `IAI.Capture.Fps 30`, `IAI.Capture.Mask 1`, `IAI.Capture.TargetMask 1`, `IAI.Capture.RunLog 1`, `r.AntiAliasingMethod 0` then `4`, `r.Lumen.DiffuseIndirect.Allow 0`, `r.DynamicGlobalIlluminationMethod 0`, `r.ReflectionMethod 0`, `IAI.Capture.FocusGate 0`, `IAI.Anomaly.StuckMipRoute auto`. Transition CVars stayed at `-1`; no temporal override was applied in-engine.

Temporary Windows/WindowsEditor device profile: Project02 LOD bias 1, min/max size 1/16384, anisotropic min/mag, point mip filter, simple-average mip generation. It was absent beforehand and removed by hash receipt. Staged EXE and five containers were restored byte-for-byte; five Editor DLLs restored to the verified release build. Both owned engine PIDs exited; only the owned staged PID was stopped by the runner. No foreign process or security setting was changed. Strict build settings/generated files were restored by receipt.

Scope deviations: none. Harness-only correction: the first artifact scan stopped on Windows text decoding before scanning; explicit UTF-8 fixed it, and the retained retry passed. The isolated launcher added the required during-leg foreign-editor watch before any engine launch. Human activity delayed the idle gate. No pixel threshold was changed. Temporal image lag remains unflagged by the owner's explicit choice; this report makes no new zero-lag claim.

Runtime note: PIE logged the same pre-capture `!DstMatrix->ContainsNaN()` reflection-matrix ensure as the retained `C7FB5212` baseline. The engine continued and the capture checks passed; the 0/0 counts above refer to builds. [Log comparison](E:/IA_BuildCache/_r917/runtime-log-notes.json).

Full diff against `d660b5d` below, with zero context; every changed line is included. Source is exactly three constants and three help strings; two pure-test files and the three requested documentation files complete the diff. [Exact diff audit](E:/IA_BuildCache/_r917/diff-audit.json).

```diff
diff --git a/CLAUDE.md b/CLAUDE.md
index 85d62c4..75cf7ee 100644
--- a/CLAUDE.md
+++ b/CLAUDE.md
@@ -13,0 +14,7 @@ and is the single source of truth for the project.
+> **091-07, 2026-10-04 — RELEASE FREEZE, `release/2026-10-05`.**
+> Base: `d660b5d6e67c7af8b0f264e59cf66ed02f433968`. The single behavior change is temporal
+> transition defaults On/Off/Hide = **0/0/0**, by the owner's choice. Explicit nonnegative CVar
+> overrides retain their existing behavior. All other production source is unchanged, including labels,
+> capture, anomalies, non-temporal transition reasons and schema; no 091-05 or 091-06 changes.
+> Journal: `docs/sessions/2026-10-04-091-07-release-transitions-off.md`.
+
diff --git a/Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp b/Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp
index 3fd6ff0..a072e56 100644
--- a/Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp
+++ b/Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp
@@ -134 +134 @@ static TAutoConsoleVariable<int32> GLabelTransitionOnFrames(TEXT("IAI.Label.Tran
-	TEXT("effective value is 0. -1 (default) = 3 (ruled 084-05b). Read at the start of each capture run; echoed and ")
+	TEXT("effective value is 0. -1 (default) = 0 (owner's choice, release freeze). Read at the start of each capture run; echoed and ")
@@ -139,2 +139,2 @@ static TAutoConsoleVariable<int32> GLabelTransitionOffFrames(TEXT("IAI.Label.Tra
-	TEXT("an entry with transition=1 that does NOT set anomaly_present. Temporal or unknown AA only (0 when confirmed off). -1 (default) = 40 ")
-	TEXT("(091-03 measured t10 tail plus four frames). Override read at run start; effective value refreshed during capture and written to run_summary.label_transition_off_frames."));
+	TEXT("an entry with transition=1 that does NOT set anomaly_present. Temporal or unknown AA only (0 when confirmed off). -1 (default) = 0 ")
+	TEXT("(owner's choice, release freeze). Override read at run start; effective value refreshed during capture and written to run_summary.label_transition_off_frames."));
@@ -143 +143 @@ static TAutoConsoleVariable<int32> GLabelTransitionHideFrames(TEXT("IAI.Label.Tr
-	TEXT("object returns carry transition=1. Temporal AA only (0 without it). -1 (default) = 1. Read at run start; echoed ")
+	TEXT("object returns carry transition=1. Temporal AA only (0 without it). -1 (default) = 0 (owner's choice, release freeze). Read at run start; echoed ")
diff --git a/Source/AnomalyInjector/Public/AnomalyLabelSync.h b/Source/AnomalyInjector/Public/AnomalyLabelSync.h
index 6bcb437..68bdaec 100644
--- a/Source/AnomalyInjector/Public/AnomalyLabelSync.h
+++ b/Source/AnomalyInjector/Public/AnomalyLabelSync.h
@@ -13,3 +13,3 @@ namespace AnomalyLabelSync
-	static constexpr int DefaultOnFramesTemporal = 3;
-	static constexpr int DefaultOffFramesTemporal = 40;
-	static constexpr int DefaultHideFramesTemporal = 1;
+	static constexpr int DefaultOnFramesTemporal = 0;
+	static constexpr int DefaultOffFramesTemporal = 0;
+	static constexpr int DefaultHideFramesTemporal = 0;
diff --git a/docs/client-readme.md b/docs/client-readme.md
index 03e07fd..eb5ac14 100644
--- a/docs/client-readme.md
+++ b/docs/client-readme.md
@@ -841 +841 @@ single unbroken run.
-| `transition_frames` | *absent* | Sorted, unique session frame indices with `transition: 1` for this event, including onset, detached post-label tails and every transition reason. These can overlap `injected_frames` or lie outside it. Drop them from training: never use as negatives, never as positives. | **Added** |
+| `transition_frames` | *absent* | Sorted, unique session frame indices with `transition: 1` for this event, including optional onset and detached post-label tails and every emitted transition reason. Temporal tails are NOT flagged by default (0/0/0); opt in with `IAI.Label.TransitionOffFrames` / `IAI.Label.TransitionOnFrames` / `IAI.Label.TransitionHideFrames`. These can overlap `injected_frames` or lie outside it. Drop them from training: never use as negatives, never as positives. | **Added** |
@@ -935,14 +935,5 @@ Any other `anomaly_present` row outside the event's frame list is a defect — t
-Labels follow **what the game rendered on that exact frame**. With a **temporal** anti-aliasing method
-(TAA or TSR), the picture you see is blended with previous frames, so a change can look half-done for a
-frame or two after it starts, and linger for a few frames after it ends. We measured this on our bench:
-with anti-aliasing off the labels and pixels agree exactly at both ends; with TAA the blur reaches its
-midpoint about **2 frames** after a blurry-texture event starts, falls below half of its depth **4 to 8 frames**
-after it ends, and a faint trace (5–45 % of the depth) can linger to **13–29 frames** after it ends. That is
-why older builds flagged 16 frames. The 091-03 E1 measurement found a tail above 10% of the event's
-full strength lasting up to 26 frames under TSR and 36 under TAA. The default is **40** post-label frames:
-36 measured frames plus four frames of margin. The configuration cap remains 64. The unchanged half-strength
-E1 edge check is the release gate; smaller residuals above clean-to-clean variation are diagnostics, not a
-pixel-for-pixel clean guarantee. Final proof and its exact scope are in the 091-03 journal.
-
-Rather than move a label off the exact render, those frames carry an extra key, **`transition: 1`**, on
-the anomaly's entry, and the frame carries **`transition_present: true`**:
+Under temporal anti-aliasing (TAA/TSR) the picture can trail the label by a few frames at an event's start and end; those frames are NOT flagged by default. Optional: set IAI.Label.TransitionOffFrames / OnFrames / HideFrames to flag them.
+
+The owner chose **0 / 0 / 0** temporal transition windows for the release freeze. Labels are unchanged.
+When a transition is flagged, the anomaly's entry carries **`transition: 1`**, and the frame carries
+**`transition_present: true`**. Non-temporal reasons below remain independent.
@@ -952,3 +943,3 @@ the anomaly's entry, and the frame carries **`transition_present: true`**:
-| `stuck_low_mip`, UV/normal and other fire-window events: start | `temporal_aa` | the event's first **3** labelled frames | the normal entry, plus `transition: 1` | yes | yes |
-| `stuck_low_mip`, UV/normal and other fire-window events: end | `temporal_aa` | the **40** captured frames after its last labelled frame | a transition-only entry (`target_pixels` −1, `observable` null; the frame's target mask does not include it) | **no** | **no** |
-| `blinking`, `missing_object` | `hide_return` | the **first** captured frame after the object reappears | `transition: 1` (inside a `blinking` burst this is on the burst's own entry; after the event ends it is a transition-only entry) | only if the event is still running, as before | no |
+| `stuck_low_mip`, UV/normal and other fire-window events: start | `temporal_aa` | the event's first N labelled frames when `TransitionOnFrames` is enabled (default **0**) | the normal entry, plus `transition: 1` | yes | yes |
+| `stuck_low_mip`, UV/normal and other fire-window events: end | `temporal_aa` | the N captured frames after its last labelled frame when `TransitionOffFrames` is enabled (default **0**) | a transition-only entry (`target_pixels` −1, `observable` null; the frame's target mask does not include it) | **no** | **no** |
+| `blinking`, `missing_object` | `hide_return` | the first N captured frames after the object reappears when `TransitionHideFrames` is enabled (default **0**) | `transition: 1` (inside a `blinking` burst this is on the burst's own entry; after the event ends it is a transition-only entry) | only if the event is still running, as before | no |
@@ -996,2 +987,2 @@ the anomaly's entry, and the frame carries **`transition_present: true`**:
-| `temporal_aa` | the anti-aliasing history may still show the previous state (onset) or a fading copy during the declared off window (40 frames by default) | drop the frame |
-| `hide_return` | the first frame after a hidden object reappears; the history may still show it missing | drop the frame |
+| `temporal_aa` | the anti-aliasing history may still show the previous state (onset) or a fading copy during an optional off window (0 frames by default) | drop the frame |
+| `hide_return` | optional frames after a hidden object reappears (0 by default); the history may still show it missing | drop the frame |
@@ -1018,2 +1009,2 @@ were changed: restore them and capture again.
-  feature-level path. TAA, TSR, a registered temporal upscaler or missing/unknown evidence enables temporal flags.
-  Once observed, temporal protection stays on to run end so changing AA cannot erase a pending tail. This can add
+  feature-level path. TAA, TSR, a registered temporal upscaler or missing/unknown evidence allows optional temporal flags.
+  Once observed, temporal detection stays on to run end so changing AA cannot erase an optional pending tail. This can add
@@ -1027,2 +1018 @@ were changed: restore them and capture again.
-- **The numbers are 3 / 40 / 1** under temporal anti-aliasing and **0 / 0 / 0** without it (091-03 measured offset;
-  earlier builds used 3 / 16 / 1 and 3 / 8 / 1). They are console variables —
+- **The defaults are 0 / 0 / 0**, including under temporal anti-aliasing, by the owner's release-freeze choice. The optional windows are console variables —
@@ -1033,4 +1023,2 @@ were changed: restore them and capture again.
-  The 40 covers the maximum measured E1 t10 tail of 36 frames with four frames of margin. The previous
-  16-frame default failed t10 coverage. This exclusion window does not add positive labels.
-  The half-strength gate in §8.7a remains the release rule. Residuals below t10 may outlast the window;
-  their measured lengths are reported separately. Always drop every declared transition frame.
+  Optional windows do not add positive labels. Always drop every declared transition frame.
+  The historical temporal-tail measurements in the 091-03 journal used the former 3 / 40 / 1 defaults.
@@ -1039,2 +1027 @@ were changed: restore them and capture again.
-  The unchanged half-strength gate in §8.7a remains the release rule. The offset window also covers the measured
-  t10 tail plus margin; the journal separately records smaller residuals above clean-to-clean variation.
+  Temporal tails are NOT excluded by default; opt in to flag them with the three console variables above.
@@ -1106,3 +1093,4 @@ touch the slab, the plugin traces a grid of rays **inside the slab only** agains
-**First exclude settling and uncertain frames.** In schema 2.1, drop the union of all events' `transition_frames`
-before assigning positives or negatives: **never use as negatives, never as positives**. This includes frames after
-an event's last label, when the picture can still be settling. `transition_reasons` explains the entries counted;
+Under temporal anti-aliasing (TAA/TSR) the picture can trail the label by a few frames at an event's start and end; those frames are NOT flagged by default. Optional: set IAI.Label.TransitionOffFrames / OnFrames / HideFrames to flag them.
+
+**Exclude declared uncertain frames.** In schema 2.1, drop the union of all events' `transition_frames`
+before assigning positives or negatives: **never use as negatives, never as positives**. `transition_reasons` explains the entries counted;
@@ -1116 +1104 @@ captured frame. Defaults are also re-read during capture, including the engine's
-view evidence is treated as temporal. Once temporal history is detected it remains protected until that run ends,
+view evidence is treated as temporal. Once temporal history is detected it remains recorded until that run ends,
@@ -1120 +1108 @@ retain zero temporal windows; non-AA uncertainty flags remain independent. UV/no
-now receive temporal onset and offset flags as well as the existing PIE settle flag.
+can receive optional temporal onset and offset flags; the existing PIE settle flag remains enabled.
@@ -1130 +1118 @@ Five fields answer five different questions about one frame. Keep them apart:
-| `transition` / `transition_reason` | entry | Is this frame **uncertain** for this event, and why (§8.6a)? |
+| `transition` / `transition_reason` | entry | Is this frame **flagged uncertain** for this event, and why (§8.6a)? TAA/TSR start/end tails are NOT flagged by default; optionally set `IAI.Label.TransitionOffFrames` / `IAI.Label.TransitionOnFrames` / `IAI.Label.TransitionHideFrames`. |
@@ -1141 +1129 @@ Five fields answer five different questions about one frame. Keep them apart:
-| `stuck_low_mip`, hold route | only the frames whose render record shows the hold (§8.3a); the frames before the blur takes hold carry **no entry at all** | the same frames | none; under temporal anti-aliasing the off window after the last held frame (40 frames by default) carries **transition-only** entries that do not set `anomaly_present` | projected box | labelled frames | `temporal_aa`, `partial`, `unresolved` |
+| `stuck_low_mip`, hold route | only the frames whose render record shows the hold (§8.3a); the frames before the blur takes hold carry **no entry at all** | the same frames | none; an optional temporal off window after the last held frame (0 frames by default) carries **transition-only** entries that do not set `anomaly_present` | projected box | labelled frames | `temporal_aa`, `partial`, `unresolved` |
@@ -1145 +1133 @@ Five fields answer five different questions about one frame. Keep them apart:
-TAA, TSR and registered temporal upscalers. Fire-window events also carry temporal onset/offset flags and, in PIE,
+TAA, TSR and registered temporal upscalers. Their windows default to 0. Fire-window events can carry optional temporal onset/offset flags and, in PIE,
@@ -1181 +1169 @@ label — it says an event is running, not that its effect is on screen.
-**What "proven" means here — the release rule, exactly.** We captured each anomaly on our own machine and, from the
+**What "proven" means in the historical checks below.** The release freeze disables temporal flags by default; these historical passes used their stated flags and do not establish zero temporal lag with the new defaults. We captured each anomaly on our own machine and, from the
@@ -1220 +1208 @@ not listed was not run.
-| `stuck_low_mip` | large test level (one object with its own textures) · natural · AA on, targeted and in an Auto-pool run; the same object · natural · AA off | `partial` and `unresolved` (an event's first labelled frames), and `temporal_aa` (its first 3 labelled frames and the current **40** frames after its last one; temporal AA only) |
+| `stuck_low_mip` | large test level (one object with its own textures) · natural · AA on, targeted and in an Auto-pool run; the same object · natural · AA off | `partial` and `unresolved` (an event's first labelled frames), and `temporal_aa` (its first 3 labelled frames and 40 frames after its last one in those historical captures; the release defaults are now 0/0/0) |
@@ -1500 +1488 @@ frame, has been written **and** no render record it is still waiting on could sh
-several frames after its event ends (with temporal anti-aliasing, the declared off window (40 frames by default) for its `transition` tail,
+several frames after its event ends (with temporal anti-aliasing, an optional off window (0 frames by default) for its `transition` tail,
diff --git a/docs/sessions/2026-10-04-091-07-release-transitions-off.md b/docs/sessions/2026-10-04-091-07-release-transitions-off.md
new file mode 100644
index 0000000..ff77e5d
--- /dev/null
+++ b/docs/sessions/2026-10-04-091-07-release-transitions-off.md
@@ -0,0 +1,9 @@
+# 091-07 — release freeze: temporal transitions off
+
+Owner decision, 2026-10-04: deliver `d660b5d6e67c7af8b0f264e59cf66ed02f433968` plus one behavior change on `release/2026-10-05`: temporal On/Off/Hide defaults are 0/0/0. Explicit nonnegative CVar values keep the existing resolver and cap. No capture, label membership, anomaly, schema, ownership, scope or alpha changes; no 091-05/091-06 code.
+
+The header constants and three CVar help strings change. Pure-test expectations cover zero defaults, and a new assertion restores 3/40/1 using explicit values. README field tables and section 8.7 explain optional flags. Non-temporal reasons remain byte-identical.
+
+Verification plan: detached release commit on the named build host; strict and normal Editor/Game builds at 0 errors/0 warnings, lever audit, string comparison against C7FB5212 allowing only the three help strings, archive/re-hash. One shared-proxy TSR PIE leg and one staged leg, at least three visible fires each, with no foreign UnrealEditor and fresh idle/disk receipts. Require zero temporal_aa reasons, zero On/Off summary windows, annotation/labels equality, unchanged labelled frame sets against the same retained baseline event schedule, and zero labelled frames without measured mask pixels. Old 3/40/1 source and retained captures must fail the new-default gate; doctored missing-mask/label cases must fail the readers. Temporal picture lag is an accepted owner choice and is recorded, not hidden by reinstating flags.
+
+Evidence and final results: `E:\IA_BuildCache\_r917`; full report: `D:\IntrusiveAnomalies\_relay\runs\2026-10-04-091-07-release-transitions-off\report.md`. Push only the new release branch after verification. The built/proved release commit is the pull target; final measured results are in the external report to preserve that exact commit.
diff --git a/tools/exclusion_selftest.cpp b/tools/exclusion_selftest.cpp
index f5342fb..9a73682 100644
--- a/tools/exclusion_selftest.cpp
+++ b/tools/exclusion_selftest.cpp
@@ -371 +371 @@ static void TestF1TemporalOffTail()
-	const FVerdict V = Row("F1 m53 admitted inside m52's TAA off-frames (closed, detached, processed, tail owed)", EFamily::M53,
+	const FVerdict V = Row("F1 m53 admitted with default TAA off-frames disabled", EFamily::M53,
@@ -373 +373 @@ static void TestF1TemporalOffTail()
-		EState::LabelTail, true, false);
+		EState::Closed, false, false);
@@ -393,5 +393,2 @@ static void TestF1TemporalOffTail()
-	Check((int)TailFrames.size() == Off && TailFrames.front() == 102 && TailFrames.back() == 101 + Off,
-		"F1 sequence: m52 transition entries on 102.." + std::to_string(101 + Off) + " (the " + std::to_string(Off)
-		+ " temporal off-frames after 101)");
-	Check(AdmitSI == 103 + Off, "F1 sequence: m53 first admitted at si " + std::to_string(103 + Off) + ", after frame "
-		+ std::to_string(102 + Off) + " closed the off window (got " + std::to_string(AdmitSI) + ")");
+	Check(TailFrames.empty(), "F1 sequence: no default temporal off-frames after 101");
+	Check(AdmitSI == 106, "F1 sequence: m53 admitted on the first tested frame 106 (got " + std::to_string(AdmitSI) + ")");
@@ -428 +425 @@ static void TestF1CarriedTailOnly()
-	Check(AnomalyLabelSync::ShouldCarryTransitionTrack(Track, 105, Off, false), "F1 carry: run ends at 105 inside the off window");
+	Check(!AnomalyLabelSync::ShouldCarryTransitionTrack(Track, 105, Off, false), "F1 carry: no default off window remains at 105");
@@ -432,2 +429,2 @@ static void TestF1CarriedTailOnly()
-	Row("F1 carried tail-only next run (no trail, off tail owed)", EFamily::M53, M52Provider(Classify(NoTrail), false),
-		FM53Signals(), EState::LabelTail, true, false);
+	Row("F1 next run with no default off tail", EFamily::M53, M52Provider(Classify(NoTrail), false),
+		FM53Signals(), EState::Idle, false, false);
@@ -453,4 +450,2 @@ static void TestF1CarriedTailOnly()
-	Check((int)Emitted.size() == Off - 4 && Emitted.front() == 0 && Emitted.back() == Off - 5,
-		"F1 carry: the next run's frames 0.." + std::to_string(Off - 5) + " carry the carried m52 transition entries (old frames 106.."
-		+ std::to_string(101 + Off) + ")");
-	Check(AdmitSI == Off - 3, "F1 carry: m53 first admitted at next-run si " + std::to_string(Off - 3) + " (got "
+	Check(Emitted.empty(), "F1 carry: no default temporal entries in the next run");
+	Check(AdmitSI == 0, "F1 carry: m53 first admitted at next-run si 0 (got "
diff --git a/tools/m52_window_selftest.cpp b/tools/m52_window_selftest.cpp
index 1499f76..f413079 100644
--- a/tools/m52_window_selftest.cpp
+++ b/tools/m52_window_selftest.cpp
@@ -1153 +1153 @@ static void TestTransitionTrackOrder()
-	Check(TailFlags == 40 && !bOff, "track: the measured-tail default includes +40 and excludes +41");
+	Check(TailFlags == 0 && !bOff, "track: the release default emits no temporal tail flags");
@@ -1168,4 +1168,4 @@ static void TestAaResolve()
-	Check(ResolveTransitionFrames(-1, DefaultOnFramesTemporal, true) == 3
-		&& ResolveTransitionFrames(-1, DefaultOffFramesTemporal, true) == 40
-		&& ResolveTransitionFrames(-1, DefaultHideFramesTemporal, true) == 1, "aa: -1 resolves to 3/40/1 under temporal AA (091-03 measured offset)");
-	Check(DefaultOffFramesTemporal == 40 && MaxTransitionFrames >= DefaultOffFramesTemporal
+	Check(ResolveTransitionFrames(-1, DefaultOnFramesTemporal, true) == 0
+		&& ResolveTransitionFrames(-1, DefaultOffFramesTemporal, true) == 0
+		&& ResolveTransitionFrames(-1, DefaultHideFramesTemporal, true) == 0, "aa: -1 resolves to 0/0/0 under temporal AA (owner's release choice)");
+	Check(DefaultOffFramesTemporal == 0 && MaxTransitionFrames >= DefaultOffFramesTemporal
@@ -1173 +1173,5 @@ static void TestAaResolve()
-		"aa: the off default is 40 under temporal AA, retained by the cap, and still 0 without it");
+		"aa: the off default is 0 under temporal AA and without it");
+	Check(ResolveTransitionFrames(3, DefaultOnFramesTemporal, true) == 3
+		&& ResolveTransitionFrames(40, DefaultOffFramesTemporal, true) == 40
+		&& ResolveTransitionFrames(1, DefaultHideFramesTemporal, true) == 1,
+		"aa: explicit On/Off/Hide CVar values restore 3/40/1 over the zero defaults");
```

NEEDS-DECISION: none

VERDICT: GREEN
