<!-- collected by Claude Code (mailbox brief 094-31b) from D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\report.md, 7269 B, sha256 C7163DC794F7B1D9A68673B38493FA953B4507933135CDEE2E73504AB96549EA, at 2026-11-01T00:35:10.455476+05:30 -->
Both 4.25 Development targets built successfully. **C1 remains incomplete:** the game retry wrote sixty PNGs, but all are black; PIE never passed its pixel gate. Both capture retry budgets are spent. No crash, upload sign or protected change occurred.

Entry tip was `466ecb1`; the recorded 094-31 leftovers were already gone. FJ1 entry/exit checks passed for m51, master, release/2026-10-05, tags, host Content/Config/descriptors and the .uproject. The rebound 27-file manifest matches disk. Exit free space: C:28.46, D:55.63, E:180.07 GiB; drive-root names are unchanged. [Entry](D:/IA_Europa/Receipts/09431b/entry.json), [exit](D:/IA_Europa/Receipts/09431b/exit.json).

Changes:

- Applied FM1: one 8 GiB commit reading, BelowNormal UBT, two parallel actions, owned-tree stop below 3 GiB, maximum three starts per target.
- Added the 4.25-only `RenderUtils.h` include. The 5.1 route is unchanged; ControlServer and Shaders remain off on 4.25. G488 was already renumbered in the retained C1 source.
- Requested an 8-bit backbuffer through a process-only INI override and removed prohibited editor-world queries during PIE.
- Added bounded child-command query retries; cached argv is accepted only after the retained handle signals exit. Live unreadable children still refuse.

| Build attempt | Start IST, October 31 | Start/minimum free commit GiB | Exit | Result |
|---|---|---:|---:|---|
| Editor 01 | 21:19:54 | 8.256 / 6.049 | 6 | Two compiler diagnostics |
| Editor 02 | 21:22:16 | 8.268 / 7.210 | 0 | PASS |
| Game 01 | 21:25:20 | 8.171 / 5.853 | 0 | PASS |

All builds used BelowNormal priority and `-MaxParallelActions=2`; **no in-build memory stop occurred**. First-build errors: AnomalyCapture **2**, AnomalyInjector **0**, EuropaPhaseAProbe **0**; other modules had no compile action. C2065 (`GPixelFormats`) and cascading C2789 (`BPP`) at AnomalyFrameCapturer.cpp:406 were fixed by the direct include; RenderCore/Public/RenderUtils.h:78 declares the symbol. Both successful builds use native source `024e5fc`. [Build logs and readings](D:/IA_Europa/Receipts/09431b/build-summary.json).

The FL3 negative passed: padded pitch, short pitch, short height, 0x0 and null maps are refused; the valid layout passes. These production-predicate assertions compiled in both targets; no malformed GPU map was injected. Changed-code FM1 tests, engine Python 2.7.14 harness, keyword-Rotator check, reader/driver negatives and override-refusal tests passed. The child-guard fix passed six cases, three admission negatives and two harmless-process rehearsal cases. Both tooling rebindings and the required fresh-core smoke passed. [Test and arming receipts](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/09431b-c1-receipts.json).

| Session | Plugin PNGs | Result | Exit |
|---|---:|---|---:|
| Smoke 01 | Not requested | Six bindings PASS; no load/save | 0 |
| PIE 01, EA_Core | 0/60 | Unsupported format 18; one drop | 0 |
| PIE 02, EA_Core | 0/60 | UBT command-query guard stopped before capture | 125, controlled |
| Game 01, EA_Travel | 0/60 | Same guard stopped before capture | 125, controlled |
| Smoke 02 | Not requested | Six bindings PASS; no load/save | 0 |
| Game 02, EA_Travel | 60/60 | Black-frame and region checks FAIL | 0 |

Each start passed five idle minutes and the 4/8 GiB physical/commit gate. Smoke02 waited 137.43 minutes; Game02 waited 10.79 minutes. Both normal capture completions and both smokes stayed within 90 seconds. Every session's upload and protected-file checks passed. Exact times, gate readings, UBT argv and adopted crash-monitor identities are in the [session receipts](D:/IA_Europa/Receipts/09431b/session-summary.json).

PIE01's map was exactly 1280x720, but stock 4.25 supplied PF_A2B10G10R10. The retry override selects PF_B8G8R8A8 before viewport creation; it edits no Config file. PIE02 first recorded the allowed ValidatePlatforms argv, then lost a command-length query. Game01's first query was unreadable. Their exact OS timing cause is unconfirmed. Neither Smoke02 nor Game02 spawned a recorded UBT child, so live recovery of that query race remains unproven. [Diagnosis and 4.25 citations](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/09431b-c1-format-retry.md).

Game02 produced indices **0-59**, no missing/duplicate indices, native **1280x720**, known format **2**, AA **0**, **D3D11**, **zero drops**, and capture time **9.083 seconds**, with normal exit. However, **60/60 PNGs are identical RGB zero**. The fixed region `(450,170)-(830,590)` has mean error **216.997** and fraction above 24 of **1.0**, against limits 8 and 0.10. The reader remains FAIL. First/middle/last pictures were inspected. The probe records EA_Travel's visible fixture objects and light, but no independent same-session source image or intermediate pixel checksum was retained. **The black-image root cause is unconfirmed.** Further diagnosis needs active-camera and source/copy/staging pixel evidence in a newly authorized session. [Pixel measurements](D:/IA_Europa/Receipts/09431b/game02-pixel-diagnosis.json), [first frame](D:/IA_Europa/Receipts/09431b/capture-attempts/C1Game/02/plugin-frames/frame-000000.png).

Process-only settings were the 8-bit backbuffer, AA=0, screen percentage=100, dynamic resolution=0, motion blur=0, MaxFPS=30, 1280x720 window, autosave/PIE auto-compile disabled, and EU4/FC4 upload opt-outs. They ended with the processes. No machine/security/paging setting changed. No archive was created. All nine host/plugin EXE/DLL hashes are listed below; their full paths and sizes are in [artifact-hashes.json](D:/IA_Europa/Receipts/09431b/artifact-hashes.json).

| EXE/DLL | SHA-256 |
|---|---|
| StylizedParisStreet-Win64-Shipping.exe | `6f669b6337eebf97d9b6343557b596f7ab25fa7c03273abebb5621599a24c861` |
| StylizedParisStreet.exe | `951eebbdeb369e2d84a00599e59ed08b28a82a741b0fa5d351294d499deb1b14` |
| UE4Editor-EuropaPhaseAProbe.dll | `1d1b8d2fd8f0b1972818cdf6fb8623a773cb2ff53658b03e8a71a6ef1f8f36be` |
| UE4Editor-StylizedParisStreet.dll | `8c553e7fc999ef9ca995a933d8d573a7c67791d116f328842f9f3c95d91a6f2c` |
| UE4Editor-AnomalyBench.dll | `2bf4268777f3c459b6812bdf56f24d6624fe0b6d282b0e7ae2f31a53f55ad940` |
| UE4Editor-AnomalyCapture.dll | `984d0a23801c4def54db3692f2032920ecdee0167d25e28a56ddd3bc2e9ccd52` |
| UE4Editor-AnomalyControlServer.dll | `0b0dc378b5fa47f65eff3246eebd7c03d4b4bcf9bce4515f993ec9f253bc772b` |
| UE4Editor-AnomalyInjector.dll | `b7170c263d1b722ad7e8e050e4032ea2bb211a14a462a5c6068a47566b6b8249` |
| UE4Editor-AnomalyShaders.dll | `c3db764fc6566f01ca1f425d3bde0469ca8d1ae88ceb01bfb8f5a82f5e8cfd96` |

Local commits on `feat/europa-port`: `1ab6897` (FM1), `024e5fc` (compile fix), `3e44a29` (format/PIE driver), `31f6ab8` (child guard), and **`32c5bf5ca5566c7a9ff75261897f39e95795b0e5`** (closeout). The worktree is clean; W4 author/committer date overrides were absent. **Nothing pushed; no tags.** Heavy proofs/scans/audit were omitted under FJ1. Pixel thresholds and failed verdicts were unchanged; the unfinished work is C1 pixel diagnosis and fresh session authorization.

VERDICT: NEEDS-DECISION | Both targets build; C1 pixels fail and both capture retry budgets are spent.
