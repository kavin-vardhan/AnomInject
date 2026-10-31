# 094-31b C1 retry prediction — 2026-10-31 21:38 IST

PIE slot 01 exited normally with zero PNGs and one dropped frame. Its Runtime.log:833–838 records a 1280×720 mapping with pitch 1280, height 720 and format 18. CaptureSession425 rejects format 18; the layout guard did not reject this frame. PixelFormat.h:32 names format 18 PF_A2B10G10R10. No crash, upload sign or protected change occurred.

Stock 4.25 defaults r.DefaultBackBufferPixelFormat to 4 (10-bit RGB) in Renderer/Private/PostProcess/SceneRenderTargets.cpp:125–134. Engine/Private/RendererSettings.cpp:30–34 and :54–58 map value 0 to PF_B8G8R8A8. Windows/D3D11RHI/Private/D3D11Viewport.cpp:640–647 reads this setting when creating a viewport. Launch/Private/LaunchEngineLoop.cpp:2242 applies /Script/Engine.RendererSettings before viewport creation; Core/Private/Misc/ConfigCacheIni.cpp:1100–1158 supplies command-line INI overrides.

The retry uses -ini:Engine:[/Script/Engine.RendererSettings]:r.DefaultBackBufferPixelFormat=0 in this process only, with -nowrite. It does not edit a Config file or change machine settings. C1 is therefore qualified on an explicitly requested 8-bit backbuffer; 10-bit capture remains unsupported and refused. The reader's format, size and pixel thresholds are unchanged.

The driver also stops querying get_editor_world during PIE. Slot 01 logged that prohibited editor scripting query every tick; the game-world context already exists and suffices for the end/exit commands. The recording stub now raises on such a query and still models reentrant multi-tick loading and single-shot failure.

Prediction before retry: fresh PIE slot 02 writes exactly 60 native 1280×720 PNGs in format 2 with zero dropped frames and normal exit; the unchanged independent fixture region check passes. The unspent Game slot uses the same process override. Preserve slot 01 and its raw hashes; rebind only changed tooling, retain the qualified native manifest and all slot histories. No further PIE retry is authorized.

## Guard diagnosis and remaining Game retry

The PIE retry stopped before the driver started. child-001-process-polls.jsonl records PID31860 with the allowed ValidatePlatforms argv at 16:09:00.724799Z, then a command-line length read failure at 16:09:00.961339Z. Game slot 01 likewise stopped on PID35772's first unreadable command at 16:10:58.217726Z. These are controlled stops, not crashes; no binary write was observed. The exact OS timing behind the unreadability is unconfirmed.

runtime_children.poll previously discarded an already read argv whenever another command-line query failed. build_memory.OwnedTree.command_line uses NtQueryInformationProcess class 60; its first length query returned no usable length. The focused fix retries reads for at most 500 ms against the retained process handle. It can use a cached argv only when that same handle is signalled exited; a live unreadable process, or an exited process whose argv was never captured, still stops. Build modes and target triples still stop. No PID/name fallback is added.

The six guard cases, three revision admission negatives and two harmless actual-process cases qualify this change. FJ1's core-change exception requires Smoke slot 02, a dry preparation of Smoke02/Game02 and one observe-only reading. Game slot 02 is the remaining C1 retry; PIE remains failed with its two slots spent. Prediction: the fresh smoke passes and Game writes sixty format-2 frames with the unchanged pixel check. No native rebuild or extra PIE session is authorized by this tooling repair.


Final measured outcome: Smoke02 passed; Game02 completed sixty format-2 PNGs in 9.083175 seconds with exact indices, mapped dimensions and zero drops, but every image is RGB zero. The pixel reader remains FAIL. Source image versus copy/readback failure is not distinguishable from the retained evidence; the root cause is unconfirmed. The session journal records measurements and the next diagnostic boundary. Both PIE and Game retry budgets are exhausted.
