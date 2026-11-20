# Europa dashboard on a packaged 4.25 build — LOCAL VALIDATION / NOT OFFICE-READY (094-47r, 2026-11-05)

> ⛔ **NOT OFFICE-READY. DO NOT USE AT THE OFFICE.** The plugin code below exists only on this PC and is **not on origin**; none of it has run. The shipped office instruction is still **`_reviews\094-45r-office-card-europa.md`** (origin `c5d0919`).

## Where things stand after 094-47r
| Item | State |
|---|---|
| Plugin `feat/europa-port` | local `a2250fda7a9b2f3754fd13c6061fe632f45b9d6e`; origin `c5d0919e5074404d26da8c16ccf845b360bde383`. **Not pushed** (OD4/OD10 hold). 094-47r made no commit. |
| Dashboard | `master` `7ea2e51`; **no `feat/europa-425` branch** (the LOD corruption display is not written). Nothing pushed. |
| 094-47r | **Stopped at its entry check:** the full P9 check fails on 1,021 5.1-engine build-bookkeeping files left without a receipt by 094-47's ReGate51 compile. A reconciliation decision is needed before anything else runs. |
| Stage A | 4.25 transport + shared C5 entry written; 4.25 Editor and Game compiled at a231a57 (094-47). **The 5.1 compile still FAILS the gate** (14 first-header errors), the lever audit still FAILS (12 + 32), strict-include is incomplete, the include/audit repairs are **not made**, and the protocol smoke is **UNRUN**. |
| Stage B (dashboard component, package, packaged C5) | **UNRUN** (no branch, no package) |
| Stage C (equivalence, early stop, reconnect, desktop connection) | **UNRUN** |
| Disk | E: about 1.3 GiB above its 50 GiB floor; one capture costs about 1.6–1.8 GiB, so the package and Stage C do not fit yet. |
| camera_clipping | **unqualified**: D3D12 1279×719 D4 and strict texture G3 fail; GO3 unrun. Opt-in only, not in the defaults. |

## What the procedure is meant to be (for review; none of it is validated)
1. **Package:** Win64 **Development** of the Europa host with the plugin (Shipping is excluded and has no listener). Command and output folder are set by Stage B; none exists yet.
2. **Start the game** with the C5 bench flags (`-IAIBench` …, as in the 094-45r card) and **without `-IAICaptureExit`**, so the game stays alive after a capture.
3. **Start the listener:** `IAI.Server.Start` (console or `-ExecCmds`); default `ws://127.0.0.1:8077`, loopback only.
4. **Token:** the game writes a `LogAnomalyServer` line announcing the token; the desktop dashboard finds the process and reads it from the log. **Never copy, photograph or type the token anywhere else.** A wrong token is refused (`bad_token`).
5. **Dashboard preset (the only one 4.25 accepts):** Random mix; the four defaults **blinking, LOD corruption, missing texture, corrupted texture**; **PNG**; **native size**; **seed 4242**; custom frame count **2352**. Anything else (JPEG, resizing, targeted capture, another frame count) is refused with an error.
6. **No live preview on 4.25** (GQ4): the dashboard shows status and controls only.

## Before this card can become office-ready
- A decision on reconciling the 1,021 inherited engine-bookkeeping records with P9; then the include/audit repairs, clean 4.25 and 5.1 compiles, the audit and strict-include checks and the editor `-game` smoke.
- The dashboard branch with its tests and desktop build.
- Enough free space for the package and the capture sessions.
- Stage B and Stage C passing on both RHIs, Code's review and scans, and the push decisions (dashboard branch; the plugin only after the camera/plugin hold is lifted).
