# 094-27e prediction and scope (2026-10-31)

FK1 and FK2 change only the start parameters: physical memory at least 4 GiB, free commit at least 8 GiB, and idle time at least 120 seconds. The in-session free-commit stop remains below 4 GiB. FJ1 drive lines and record-only GPU/input/coverage stay. The launch window ends at 2026-11-01 07:00 IST.

Fresh single-use slots are Smoke, blinking positive and lod_corruption positive. Smoke has a 300-second limit; each picture leg has 900 seconds. The 09427d arming and all older jobs refuse. One three-slot dry run precedes the real run. No in-editor script, picture reader, native binary or fixture package changes.

Prediction: the 094-27d fix avoids a competing live A7 probe read, allowing blinking to finish six visible apply/revert events. Forced LOD model 1 selects the sphere instead of the cube already selected by automatic LOD, supplying a visible change and restoration. Each anomaly still needs at least three independently checked picture events with AA=0. Lighting retains its five counted events from 094-27c; missing_object retains its six from the 094-27d re-read.

Validation is limited to the changed start thresholds, one-reading behavior, old-arming refusal and the required dry run. The unchanged Python 2.7 harness and stand-in results remain at Receipts/09427d; they are not rerun. FK4 defers the native probe write self-disable. No push or tags.

The first dry-run invocation refused before any slot because the shared job-scope validator had only the earlier memory tuples. Its FK1 amendment accepts the exact 4/8/none/4 tuple only for the authorized 09427e runtime job with empty build/query lists. A negative tuple test and the real job-config load qualify that parameter amendment before the arming is regenerated. The failed invocation and superseded arming remain in the job receipts.
