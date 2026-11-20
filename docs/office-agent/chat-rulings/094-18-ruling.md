# 094-18 — Chat ruling: A6's owner half is accepted; the crash's engine marker gets a narrow bookkeeping class; fix the probe and rebuild, and research the launcher and uploads before any further runtime leg

**Date:** 2026-10-27, about 12:30 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Codex's report:** `_reviews\094-18-codex-report.md` (10:45–11:38 IST, 3,153 s, exit 0). Its verdict: `NEEDS-DECISION | Owner-leg 4.25 EditorRuns marker is outside the allowed engine-write class; probe fix and rebuilds not started`.
- **Claude Code's review:** `_reviews\094-18-code-review.md`. 29 OK · 8 N/A · 3 GAP · 7 OPEN · 1 STOP (inherited, correctly raised) · 0 stop violations.
- **The owner:** he thinks he clicked "Send and Close". Code's reading of the logs confirms an upload.

## Verdict
**The stop was correct.** The owner leg's crash left `E:\EpicGames\UE_4.25\Engine\Intermediate\EditorRuns\31124` (0 bytes) in the 4.25 engine tree. That is outside EA4's allowed 4.25 class.

**The owner leg's evidence, established by Code from the logs:**
- **Content unchanged:** Paris `Content` (1,453 files) and plugin `Content` (5 files) are byte-equal to their pre-leg records.
- **Content admission:**
  - the plugin mounts;
  - none of the five 5.1 packages appears in the log;
  - no load, save, thumbnail or cook lines.

  4.25 rejects them silently, so absence plus unchanged bytes is the proof.
- **No dialogs and no rebuild prompts.** The DDC and the shader working directory were all on D:.
- **The probe stream:** 1,841,488 rows, in order and untorn. The Editor and EditorPreview worlds each show all five subsystem classes, 0 registered (correct for non-game worlds), and correct CDO state. Only the exit-time rows were lost.
- **Two reports went to Epic:**
  - an engine ensure (`TextureDerivedDataTask.cpp:252`), uploaded automatically with no click;
  - the exit crash, uploaded after "Send and Close".

  There was no restart. Code's scan found no secrets in the uploaded files.
- **The root cause is confirmed in source:**
  - `Cleanup():157` → `Snapshot():281` → `SnapshotState()`;
  - there, `TActorIterator<AActor>` (Driver.cpp:279) is dereferenced at :282;
  - 4.25 `EngineUtils.h:248` and 5.1 `:250` both assert;
  - 5.1 also has a GC-time `FindObject` (`WorldProbe.cpp:237`), which is fatal during garbage collection (`UObjectGlobals.cpp:331–334`).

**Protected state:**
- P9 v2 18,264 of 18,264;
- the protected-state check PASS ×3;
- `m51` equal to P9 ×4;
- the 5.1 engine trees unchanged;
- the canary found 0 times;
- 0 launches;
- manifests unchanged (27/39/44).

## Rulings

### ET1. A6's owner half is ACCEPTED
- **What it shows:**
  - the 4.25 editor runs with the port loaded;
  - the content admission matches the prediction, by absence;
  - no content changed;
  - the probe observed the Editor and EditorPreview worlds correctly.
- **The leg is not re-run.**
- **The lost exit-time rows** (the last actor snapshot, the final cleanups, `probe_stop`) are proven at the next runtime leg, after the fix.

### ET2. A narrow 4.25 runtime bookkeeping class: `EditorRuns` markers
**What is admitted,** under `E:\EpicGames\UE_4.25\Engine\Intermediate\EditorRuns\`:
- zero-byte `<pid>` markers that stock 4.25 creates when our editor starts and deletes when it exits cleanly (`HotReload.cpp:389–434`; `-NoHotReload` doesn't stop it);
- stock removal of stale markers.

**Receipt each one:** path, size, times and process.

**Every other write under the 4.25 engine tree stays a hard STOP.**

**The `31124` marker stays where it is,** and nobody deletes it. A later stock editor start may remove it inside this class. The kit's STOP on it is closed under this ruling.

### ET3. The Epic Games Launcher (Code's G2)
**What happened:** the editor started the launcher as its child process. During the session, the launcher wrote about 933 MB under `C:\ProgramData\Epic`. It also inherited our TEMP routing, outlived the session, and is still writing into our D: temp slot.

**For this leg: accepted.** These are the launcher's own maintenance writes, not port data, so they are foreign under EB1, with an "editor-spawned" note. C: never came near its floor.

**ED1's floors always apply,** whatever the attribution.

**Before any further runtime leg:**
- Codex researches, from 4.25 source, what starts the launcher and how to prevent it without any setting change (094-19, read-only, with a proposal);
- chat rules on the design;
- until then, no runtime leg runs.

### ET4. Uploads to Epic (Code's G3)
**Before any further runtime leg, both crash reports and ensure reports must stay on the PC.**

**Codex researches, from 4.25 source, in 094-19 (read-only, with a proposal):**
- the exact upload paths;
- the least-invasive way to keep reports local:
  - a command-line switch, if one exists;
  - otherwise, a port-owned per-project user setting under the routed `-UserDir`, bound and receipted.

**Never a machine-wide setting, and never a C: config.**

**Owner cards** will say "Close Without Sending".

### ET5. The probe fix also covers the 5.1 GC-time lookup
**In every cleanup, teardown or garbage-collection-time path, don't use:**
- actor iterators;
- `FindObject`, `StaticFindObject` or `LoadObject`.

**Instead,** use what was captured earlier (for example, weak pointers taken at world init), or skip and record the reason.

**The static check covers both pattern families,** with a mutant for each.

**ES2's other requirements stand:**
- one probe source;
- `ENGINE_MAJOR_VERSION` for engine differences;
- no production edit.

### ET6. The engine texture ensure (Code's O5)
**Research, read-only, from the log and source:**
- which texture triggered `TextureDerivedDataTask.cpp:252`;
- whether it is a Paris texture or an engine texture;
- whether it can affect A7's pictures.

### ET7. Small carries
- **Code's O4:** scan the two owner-slot temp files once the launcher releases them. If they are still locked, record it and carry it.
- **Code's O3:** content admission can only be shown by absence on 4.25. A8's cook needs a package-output absence check.
- **Carried unchanged:**
  - ER1's G3 refusal-reason fix;
  - ER3's bare-UBT gap (the 4.25 editor itself starts UBT at launch);
  - 8.3 and junction alias tests;
  - EO1's reader budget;
  - the 5.1 probe path (A9);
  - EJ1, EJ3 and EJ5.

## Next
**094-19 is queued, with no launch.** In order:
1. ET2's receipt;
2. ET7's temp-file scan;
3. the probe fix (ES2 and ET5);
4. the six rebuilds, EI4 and re-binding the manifests (ES4);
5. the ET3, ET4 and ET6 research, findings and proposals only.

**Owner actions, whenever convenient:**
- close the old kit window (it is waiting at "press any key");
- quit the Epic Games Launcher (tray icon → Exit).
