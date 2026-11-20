# 095-07 — Chat ruling: the heavy run did not start (memory pre-flight); the stop was right; it is re-sent on the owner's word with a corrected floor

**Date:** 2026-11-14, about 14:55 IST · **Author:** chat Claude (orchestrator)
**Inputs:** mailbox reports 095-14 (GREEN, 44 min, $26.03 API-equivalent) and 095-07 (NEEDS-DECISION, 23 min, $6.45; never left pre-flight).

## 095-14: ACCEPTED
- `integ/m56-m58` = `0c71f6f` (pushed): the changelog row for the texture-event node name.
- `feat/m57-lod-spatial-trigger` = `d8d3a7e` (pushed; contains the integration tip): `IAI.Anomaly.LodDownPop` default 1 → 0, documented as experimental; the readme line on distance-event yield.
- CaptureBench `m57-gates` = `4ca8524` (local): fixture options `pair_cube` (`CB_LodPair`) and `minlod1` (`CB_LodFix_MinLod1`, scale 1.5 so it does not block the second pair). The script runs from an `m57-gates` worktree outside `Plugins\`.
- **Code corrected a fault in chat's brief:** the m56 G5 comparator read `labels.jsonl` only, and the node name lives in `annotation.json`, so the allowance chat asked for would have checked nothing (gotcha G583). G5 now compares node identity per event; only `asset_name` and `component_class` may differ, and only on scoped texture events. Proven both ways.
- **Note accepted:** the allowance also covers `stuck_low_mip` on the proxy route, reported as its own cause (`stuck_mip_proxy_route`). No narrowing unless the heavy run's reading shows it matters.
- Predictions: m56 file AMENDMENT 2, m57 file AMENDMENT 2, m59 file AMENDMENT 3; all dated before any leg, frozen prefixes unchanged.
- Hand-over file: `code-receipts-095-14\tips.txt`.

## 095-07: what happened
- Pre-flight passed on tips, checkout, disk (E: 116.5 GiB, D: 69.1 GiB) and processes.
- **Memory failed:** 6.6 to 9.0 GiB free over a 20-minute wait, against the brief's 12 GiB. The owner's own Blender render loops were running the whole time. Code touched nothing and stopped.
- Nothing was built, cooked or captured. Nothing needs re-doing.

## Rulings
- **D1. The run waits for the owner's word that his renders are done.** His work comes before heavy Code jobs (standing rule). Chat re-sends the same run as `095-07b`.
- **D2. The memory floor in chat's brief was wrong for this machine, and is corrected.** The PC has 23.6 GiB in all; with the desktop apps that are always open it idles at about 10 GiB free, so 12 could not be met even with no render running. Earlier Editor compiles started fine at 9.8 GiB.
  - **New start condition:** at least 8 GiB free **and** none of the owner's render apps running (Blender including headless loops, ComfyUI, Resolve). The second half is the one that matters: what starved the 13 Nov build was his apps, not the idle level.
  - **Unchanged during the run:** under 2 GiB free for five minutes → Code stops its own compilers by PID and reports.
  - **New:** no new build and no cook starts while one of his render apps is running; wait up to 20 minutes, then stop and report what is done and what is left.
- **The floor is not lowered to run alongside his renders.** A Game build beside a render loop slows both, and his renders use memory unpredictably.

## State
- Host: the plugin's Editor DLLs match `integ/m56-m58` `0c71f6f`. The Game exe is still the 24 Sep build. No cook exists. No clips exist.
- The re-send is written and held: `_mailbox\_held\095-07b-heavy-run.md` (xhigh). Releasing it is one move into `to_code`.
