---
name: m53-s2-0-harness-boundary
description: m53 S2-0 shadow-trace harness (_reviews\082-14-s20-*) is locked by boundary 5a31d568 pinning feature head 7348237; any branch commit before 082-15 runs blocks the window and needs a re-issue.
metadata:
  node_type: memory
  type: project
  originSessionId: <uuid>
  modified: 2026-09-28T07:00:29.042Z
---

**HISTORICAL since 082-15 (2026-09-28):** the window ran to completion (24/24 accepted, primary STALE-NULL), and docs
commit `d117a9d` moved the feature head past `7348237`, so boundary `5a31d568` is stale. Any further S2-0 window needs a
re-issue with a new boundary.

The S2-0 harness lives in `D:\IntrusiveAnomalies\_reviews\082-14-s20-*`:
- the files: common, lib, synth, tokens, preflight, window, postflight;
- it imports the S1 runner and lib unmodified;
- it points S1's module at its own root `_reviews\082-14-s20-evidence\run` through `IAI_R53_ROOT` before import, so S1's
  live dir is never written.

**Lock:** `_reviews\082-14-s20-evidence\boundary.json` (sha `5a31d568`, issued 082-14, 2026-09-28) pins:
- the feature head `7348237` (`feat/m53-uv-normal-corruption`);
- the predictions `ee120fa6` (plan revision 2.2);
- 7 S2-0 files and 5 imported S1 files;
- Codex's `independent-shadow.json` `2ae6b518`;
- the staged set: exe `2FCDF059`, utoc `20DA6F98`, ucas `534C5863`.

**Invocation (082-15, only on the owner's "PC idle"):**
`C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\082-14-s20-window.py 082-15 540`.
- Stop codes: 20 first-of-type T-fail, 21 INVALID-TIMING twice, 22 known answer, 24 NOT-REPRODUCED, 12 FIXTURE; S1
  codes otherwise.
- A resume refuses a recorded stop without launching.

**Why:** the preflight refuses on any head, predictions, harness or staged change. Same trap as
[[m53-s1-leg-harness-boundary]] and [[m55-requal-prep-boundary-locks-feature-head]].

**How to apply:**
- If a chat ruling changes the evaluator, or docs must be committed before 082-15, re-issue with a NEW script that moves
  `boundary.json` aside. `prep/boundary-issue-082-14.py` refuses when one exists.
- Then re-run `prep/proof-s20.py`, `proof-sequencer.py`, `dryrun.py <name> window` and `preflight-replay.py`.
- Stub mode (`IAI_S20_STUB_LAUNCH=1` + `IAI_S20_ROOT`, and optionally `IAI_S20_STUB_GATES=1`) is refused on the live
  root. A real run is refused on any other root.
